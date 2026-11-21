#include "max_common.h"
typedef struct {
    t_pxobject obj;
    Bus bus;
    Timing timing;
    View view;
    Capture capture;
    BandFrame result, *ring;
    Format layout;
    int channels, capacity, used, head, indices[2][32], active, taking;
    _Atomic int record, clear, divisor;
    double host;
} Memory;
static t_class *cls;
static long outputs(Memory *x, long p) {
    return width_for_fields(x->channels, p);
}
static long changed(Memory *x, long p, long n) {
    if (p < 0 || p >= F_PORTS)
        return 0;
    int old = x->channels;
    x->bus.width[p] = n;
    x->channels = band_channels(&x->bus);
    return old != x->channels;
}
static void perform(Memory *x, t_object *chain, double **in, long nin, double **out, long nout,
                    long n, long fl, void *u) {
    int ch = x->channels, sync = 96 * ch, div = atomic_load(&x->divisor),
        hz = downsample_hz(x->host, div);
    if (nout != sync + 1 + FORMAT_LANES) {
        zeros(out, nout, n);
        return;
    }
    if (hz != x->timing.clock.hz)
        timing_setup(&x->timing, x->host, div);
    int from = 0;
    for (int i = 0; i < n; i++) {
        int mark = wire_mark(&x->bus, in, i, &x->timing);
        if (mark > 0 || (mark < 0 && x->active)) {
            view_fill(&x->view, out, ch, from, i, x->active);
            from = i;
        }
        if (mark < 0) {
            x->active = 0;
            x->capture.next = -1;
        }
        if (mark == 1) {
            Format f = read_format(&x->bus, in, i, hz, ch, downsample_hz(x->host, 1));
            int reset = atomic_exchange(&x->clear, 0);
            if (reset || !same_layout(f, x->layout))
                x->used = x->head = 0;
            x->layout = f;
            x->taking = atomic_load(&x->record);
            uint32_t none[2] = {0};
            capture_begin(&x->capture, f, 0, 0, none);
            x->result.fmt = f;
            for (int c = 0; c < f.channels; c++)
                for (int b = 0; b < 32; b++) {
                    int age = sb_clamp(lane(&x->bus, in, F_GATE, c, b, i, 0), 0, x->used);
                    x->indices[c][b] = age ? (x->head + x->capacity - age) % x->capacity : -1;
                }
        }
        if (mark > 0 && x->capture.next == mark - 1) {
            int j = mark - 1, ready = capture_row(&x->capture, &x->bus, in, j, i, 0);
            for (int c = 0; c < x->layout.channels; c++)
                for (int b = 0; b < 32; b++) {
                    int k = x->indices[c][b];
                    const BandFrame *src = k < 0 ? &x->capture.frame : x->ring + k;
                    hold_band(&x->result, src, c, b, j, 1, 1);
                }
            latch(&x->view, &x->result, j, ch);
            x->active = 1;
            if (ready && x->taking) {
                x->ring[x->head] = x->capture.frame;
                x->head = (x->head + 1) % x->capacity;
                if (x->used < x->capacity)
                    x->used++;
            }
        }
        out[sync][i] = x->active ? mark : -1;
    }
    view_fill(&x->view, out, ch, from, n, x->active);
}
static void dsp(Memory *x, t_object *chain, short *count, double host, long vec, long flags) {
    bus_setup(&x->bus, x, chain, count, F_PORTS);
    x->channels = band_channels(&x->bus);
    x->host = host;
    x->used = x->head = x->active = 0;
    x->capture.next = -1;
    timing_setup(&x->timing, host, atomic_load(&x->divisor));
    object_method(chain, gensym("dsp_add64"), x, perform, 0, NULL);
}
static void down(Memory *x, t_symbol *s, long ac, t_atom *av) {
    set_divisor((t_object *)x, &x->divisor, ac, av);
}
static void record(Memory *x, long v) {
    atomic_store(&x->record, v != 0);
}
static void clear(Memory *x) {
    atomic_store(&x->clear, 1);
}
static void assist(Memory *x, void *b, long io, long p, char *s) {
    snprintf(s, 512, "%s",
             p == F_GATE ? "MC age in complete frames (0 live, 1 newest)" : field_name(p));
}
static void destroy(Memory *x) {
    dsp_free(&x->obj);
    free(x->ring);
}
static void *create(t_symbol *s, long ac, t_atom *av) {
    int d = downsample_args(&ac, av);
    long cap = ac ? atom_getlong(av) : 64;
    if (d < 0 || ac > 1 || (ac && atom_gettype(av) != A_LONG) || cap < 1 || cap > 1024)
        return NULL;
    Memory *x = (Memory *)object_alloc(cls);
    if (!x)
        return NULL;
    dsp_setup(&x->obj, F_PORTS);
    x->obj.z_misc |= Z_MC_INLETS | Z_NO_INPLACE;
    x->divisor = d;
    x->record = 1;
    x->capacity = cap;
    x->channels = 1;
    x->ring = calloc(cap, sizeof(BandFrame));
    if (!x->ring) {
        object_free(x);
        return NULL;
    }
    field_outlets((t_object *)x);
    return x;
}
void C74_EXPORT ext_main(void *m) {
    cls = class_new(MP "frames~", (method)create, (method)destroy, sizeof(Memory), 0, A_GIMME, 0);
    class_addmethod(cls, (method)dsp, "dsp64", A_CANT, 0);
    class_addmethod(cls, (method)changed, "inputchanged", A_CANT, 0);
    class_addmethod(cls, (method)outputs, "multichanneloutputs", A_CANT, 0);
    class_addmethod(cls, (method)record, "record", A_LONG, 0);
    class_addmethod(cls, (method)clear, "clear", 0);
    class_addmethod(cls, (method)down, "downsample", A_GIMME, 0);
    class_addmethod(cls, (method)assist, "assist", A_CANT, 0);
    class_dspinit(cls);
    class_register(CLASS_BOX, cls);
}
