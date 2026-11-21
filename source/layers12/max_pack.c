#include "max_common.h"
#include "pcm.h"
#include "worker.h"
#include "ext_path.h"
typedef struct {
    Capture capture;
    View view;
    Timing timing;
    Playback playback;
    Packet result;
    int active, frame_kbps, target, source, target_ch;
    unsigned armed, take;
} Engine;
typedef struct {
    t_pxobject obj;
    Engine *e;
    Bus bus;
    int channels, valid;
    double host;
    _Atomic int divisor, freeze, level, normalized, monitor, changing;
    _Atomic int original_hz, input_hz, record_hz;
    _Atomic double kbps;
    _Atomic(Worker *) writer;
    _Atomic unsigned callbacks, take, closing;
    Report report;
    void *status, *close_event;
    t_symbol *path;
} Packer;
static t_class *cls;
static void finish(Packer *x, int announce) {
    Worker *w = atomic_exchange(&x->writer, NULL);
    if (!w)
        return;
    while (atomic_load(&x->callbacks))
        sched_yield();
    Result r = worker_close(w);
    atomic_store(&x->record_hz, 0);
    if (!announce)
        return;
    if (r.error) {
        t_atom a[2];
        atom_setsym(a, x->path);
        atom_setsym(a + 1, gensym(r.error));
        outlet_anything(x->status, gensym("error"), 2, a);
        object_error((t_object *)x, "%s", r.error);
    } else {
        t_atom a[6];
        atom_setsym(a, x->path);
        atom_setlong(a + 1, r.frames);
        atom_setlong(a + 2, r.fitted);
        atom_setlong(a + 3, r.hz);
        atom_setlong(a + 4, r.channels);
        atom_setlong(a + 5, r.kbps);
        outlet_anything(x->status, gensym("done"), 6, a);
    }
}
static void pending(Packer *x) {
    unsigned id = atomic_exchange(&x->closing, 0);
    if (id && id == atomic_load(&x->take))
        finish(x, 1);
}
static void request_close(Packer *x, unsigned id) {
    if (atomic_exchange(&x->closing, id) != id)
        qelem_set(x->close_event);
}
static void stop_deferred(Packer *x, t_symbol *s, long ac, t_atom *av) {
    finish(x, 1);
}
static void stop(Packer *x) {
    defer_low(x, (method)stop_deferred, NULL, 0, NULL);
}
static void open_file(Packer *x, t_symbol *s, long ac, t_atom *av) {
    char name[MAX_FILENAME_CHARS] = "Subband take." MP, path[MAX_PATH_CHARS],
         native[MAX_PATH_CHARS];
    if (ac) {
        const char *a = atom_getsym(av)->s_name;
        if (strlen(a) >= sizeof(path)) {
            object_error((t_object *)x, "file path too long");
            return;
        }
        if (a[0] == '/' || strchr(a, ':'))
            strcpy(path, a);
        else if (path_topathname(path_getdefault(), a, path)) {
            object_error((t_object *)x, "cannot resolve path");
            return;
        }
    } else {
        short dir = 0;
        t_fourcc type = 0;
        if (saveasdialog_extended(name, &dir, &type, NULL, 0))
            return;
        if (path_topathname(dir, name, path))
            return;
    }
    if (path[0] == '/')
        strcpy(native, path);
    else if (path_nameconform(path, native, PATH_STYLE_NATIVE, PATH_TYPE_ABSOLUTE))
        return;
    Worker *w = worker_new(native);
    if (!w) {
        object_error((t_object *)x, "cannot create recording");
        return;
    }
    finish(x, 1);
    downsample_quiesce(&x->changing, &x->callbacks);
    x->path = gensym(native);
    atomic_fetch_add(&x->take, 1);
    atomic_store(&x->closing, 0);
    atomic_store(&x->writer, w);
    atomic_store(&x->changing, 0);
    t_atom a;
    atom_setsym(&a, x->path);
    outlet_anything(x->status, gensym("recording"), 1, &a);
}
static void write_file(Packer *x, t_symbol *s, long ac, t_atom *av) {
    if (ac > 1 || (ac && atom_gettype(av) != A_SYM)) {
        object_error((t_object *)x, "write takes one optional file path");
        return;
    }
    defer_low(x, (method)open_file, s, ac, av);
}
static long outputs(Packer *x, long p) {
    return p == 5 ? x->channels : width_for_fields(x->channels, p);
}
static long changed(Packer *x, long p, long n) {
    if (p < 0 || p >= F_PORTS)
        return 0;
    int old = x->channels;
    x->bus.width[p] = n;
    x->channels = band_channels(&x->bus);
    return old != x->channels;
}
static void perform(Packer *x, t_object *chain, double **in, long nin, double **out, long nout,
                    long n, long fl, void *u) {
    atomic_fetch_add(&x->callbacks, 1);
    int ch = x->channels, sync = ch * 96, monitor_lane = sync + 1 + FORMAT_LANES;
    if (atomic_load(&x->changing) || !x->valid || nout != monitor_lane + ch) {
        zeros(out, nout, n);
        goto end;
    }
    Engine *e = x->e;
    Worker *w = atomic_load(&x->writer);
    unsigned id = atomic_load(&x->take);
    int factor = atomic_load(&x->divisor), hz = downsample_hz(x->host, factor),
        listen = atomic_load(&x->monitor);
    if (e->timing.clock.hz != hz) {
        timing_setup(&e->timing, x->host, factor);
        if (!x->bus.connected[F_SYNC]) {
            e->capture.have = 0;
            e->capture.next = -1;
            e->active = 0;
        }
    }
    if (e->take != id) {
        e->take = id;
        e->target = e->source = e->target_ch = 0;
        playback_reset(&e->playback);
    }
    if (!listen && e->playback.hz)
        playback_reset(&e->playback);
    if (w) {
        unsigned epoch;
        while (worker_pop(w, &e->result, &epoch))
            if (listen && epoch == id)
                playback_frame(&e->playback, &e->result.fields);
        if (worker_error(w))
            request_close(x, id);
    }
    int from = 0;
    for (int i = 0; i < n; i++) {
        int mark = wire_mark(&x->bus, in, i, &e->timing);
        if (mark > 0 || (mark < 0 && e->active)) {
            view_fill(&e->view, out, ch, from, i, e->active);
            from = i;
        }
        if (mark < 0) {
            e->active = 0;
            e->capture.next = -1;
        }
        if (mark == 1) {
            Format f = read_format(&x->bus, in, i, hz, ch, downsample_hz(x->host, 1));
            uint32_t mask[2];
            for (int c = 0; c < 2; c++)
                mask[c] = gate_mask(&x->bus, in, F_GATE, c, i);
            capture_begin(&e->capture, f, atomic_load(&x->freeze), atomic_load(&x->level), mask);
            e->armed = w ? id : 0;
            int target = convert_target(f, factor);
            double req = atomic_load(&x->kbps);
            e->frame_kbps = sb_bitrate(LAYER, target, f.channels, req > 0 ? req : f.kbps, 1000);
            atomic_store(&x->original_hz, f.original_hz);
            atomic_store(&x->input_hz, f.hz);
            report_set(&x->report, target, f.channels, e->frame_kbps);
            if (w && e->target &&
                (e->target != target || e->source != f.hz || e->target_ch != f.channels))
                request_close(x, id);
        }
        if (mark > 0 && e->capture.next == mark - 1) {
            int ready =
                capture_row(&e->capture, &x->bus, in, mark - 1, i, atomic_load(&x->normalized));
            latch(&e->view, &e->capture.frame, mark - 1, ch);
            e->active = 1;
            if (ready && w && e->armed == id && atomic_load(&x->closing) != id) {
                BandFrame *f = &e->capture.frame;
                e->target = convert_target(f->fmt, factor);
                e->source = f->fmt.hz;
                e->target_ch = f->fmt.channels;
                atomic_store(&x->record_hz, e->target);
                worker_fields(w, f, e->target, e->frame_kbps, x->bus.connected[F_FORMAT], id,
                              listen);
            }
        }
        out[sync][i] = e->active ? mark : -1;
        float pair[2] = {0};
        if (listen)
            playback_sample(&e->playback, pair);
        for (int c = 0; c < ch; c++)
            out[monitor_lane + c][i] = pair[c];
    }
    view_fill(&e->view, out, ch, from, n, e->active);
end:
    atomic_fetch_sub(&x->callbacks, 1);
}
static void dsp(Packer *x, t_object *chain, short *count, double host, long vec, long flags) {
    bus_setup(&x->bus, x, chain, count, F_PORTS);
    x->channels = band_channels(&x->bus);
    if (x->host > 0 && host != x->host && !x->bus.connected[F_FORMAT] && atomic_load(&x->writer) &&
        atomic_load(&x->record_hz) != downsample_hz(host, atomic_load(&x->divisor)))
        finish(x, 1);
    x->host = host;
    Engine *e = x->e;
    e->capture.have = 0;
    e->capture.next = -1;
    e->active = 0;
    timing_setup(&e->timing, host, atomic_load(&x->divisor));
    playback_reset(&e->playback);
    x->valid = pcm_setup(&e->playback.bank, host, 0);
    object_method(chain, gensym("dsp_add64"), x, perform, 0, NULL);
}
static void down_deferred(Packer *x, t_symbol *s, long ac, t_atom *av) {
    int f = downsample_message((t_object *)x, ac, av);
    if (f < 1)
        return;
    int src = atomic_load(&x->input_hz), old = atomic_load(&x->record_hz);
    int dest =
        x->bus.connected[F_FORMAT] && src
            ? convert_target((Format){.hz = src, .original_hz = atomic_load(&x->original_hz)}, f)
            : downsample_hz(x->host, f);
    if (old && dest != old)
        finish(x, 1);
    atomic_store(&x->divisor, f);
}
static void down(Packer *x, t_symbol *s, long ac, t_atom *av) {
    defer(x, (method)down_deferred, s, ac, av);
}
static void bitrate(Packer *x, double v) {
    set_kbps((t_object *)x, &x->kbps, v);
}
static void freeze(Packer *x, long n) {
    atomic_store(&x->freeze, n != 0);
}
static void normalized(Packer *x, long n) {
    atomic_store(&x->normalized, n != 0);
}
static void monitor(Packer *x, long n) {
    atomic_store(&x->monitor, n != 0);
}
static void holdmode(Packer *x, t_symbol *s) {
    if (s == gensym("level") || s == gensym("coeff"))
        atomic_store(&x->level, s == gensym("level"));
    else
        object_error((t_object *)x, "holdmode expects coeff or level");
}
static void getrate(Packer *x) {
    report_rate(x->status, x->host, &x->report);
}
static void getbitrate(Packer *x) {
    report_bitrate(x->status, &x->report);
}
static void assist(Packer *x, void *b, long io, long p, char *s) {
    snprintf(s, 512, "%s",
             io == ASSIST_OUTLET && p >= 5
                 ? (p == 5 ? "Recording status" : "Audition of written frames (MC audio)")
                 : field_name(p));
}
static void destroy(Packer *x) {
    dsp_free(&x->obj);
    finish(x, 0);
    if (x->close_event)
        qelem_free(x->close_event);
    if (x->e)
        pcm_free(&x->e->playback.bank);
    free(x->e);
}
static void *create(t_symbol *s, long ac, t_atom *av) {
    int d = downsample_args(&ac, av);
    if (d < 0 || ac > 1 || (ac && (atom_gettype(av) != A_LONG || atom_getlong(av) < 0)))
        return NULL;
    Packer *x = (Packer *)object_alloc(cls);
    if (!x)
        return NULL;
    dsp_setup(&x->obj, F_PORTS);
    x->obj.z_misc |= Z_MC_INLETS | Z_NO_INPLACE;
    x->divisor = d;
    x->kbps = ac ? atom_getlong(av) : 0;
    x->channels = 1;
    x->e = calloc(1, sizeof(*x->e));
    x->close_event = qelem_new(x, (method)pending);
    if (!x->e || !x->close_event) {
        object_free(x);
        return NULL;
    }
    outlet_new((t_object *)x, "multichannelsignal");
    x->status = outlet_new((t_object *)x, NULL);
    field_outlets((t_object *)x);
    return x;
}
void C74_EXPORT ext_main(void *m) {
    sb_tables();
    filter_init();
    if (!convert_tables())
        return;
    cls = class_new(MP "pack~", (method)create, (method)destroy, sizeof(Packer), 0, A_GIMME, 0);
    class_addmethod(cls, (method)dsp, "dsp64", A_CANT, 0);
    class_addmethod(cls, (method)outputs, "multichanneloutputs", A_CANT, 0);
    class_addmethod(cls, (method)changed, "inputchanged", A_CANT, 0);
    class_addmethod(cls, (method)assist, "assist", A_CANT, 0);
    class_addmethod(cls, (method)write_file, "write", A_GIMME, 0);
    class_addmethod(cls, (method)stop, "stop", 0);
    class_addmethod(cls, (method)monitor, "monitor", A_LONG, 0);
    class_addmethod(cls, (method)down, "downsample", A_GIMME, 0);
    class_addmethod(cls, (method)freeze, "freeze", A_LONG, 0);
    class_addmethod(cls, (method)holdmode, "holdmode", A_SYM, 0);
    class_addmethod(cls, (method)normalized, "normalized", A_LONG, 0);
    class_addmethod(cls, (method)bitrate, "bitrate", A_FLOAT, 0);
    class_addmethod(cls, (method)bitrate, "kbps", A_FLOAT, 0);
    class_addmethod(cls, (method)getrate, "getrate", 0);
    class_addmethod(cls, (method)getrate, "info", 0);
    class_addmethod(cls, (method)getbitrate, "getbitrate", 0);
    class_dspinit(cls);
    class_register(CLASS_BOX, cls);
}
