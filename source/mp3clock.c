#include "ext.h"
#include "z_dsp.h"
#include "downsample.h"
#include <stdio.h>
typedef struct {
    t_pxobject obj;
    CodecClock clock;
    _Atomic int divisor;
    int applied;
    int pos, row, prev, sync;
} Clock;
static t_class *cls;
static void downsample(Clock *x, t_symbol *s, long ac, t_atom *av) {
    int factor = downsample_message((t_object *)x, ac, av);
    if (factor >= 1)
        atomic_store(&x->divisor, factor);
}
static void setup_rate(Clock *x, double host, int reset) {
    int requested = atomic_load(&x->divisor);
    if (!reset && x->applied == requested)
        return;
    x->applied = requested;
    CodecClock clock;
    codec_clock_setup_divided(&clock, host, requested);
    if (!reset && clock.hz == x->clock.hz)
        return;
    x->clock = clock;
    x->pos = x->row = x->prev = 0;
}
static void perform(Clock *x, t_object *chain, double **in, long ni, double **out, long no, long n,
                    long fl, void *u) {
    setup_rate(x, x->clock.host, 0);
    int period = x->clock.hz >= 32000 ? 1152 : 576;
    for (long i = 0; i < n; i++) {
        int ticks = codec_ticks(&x->clock), frame = 0, granule = 0;
        if (x->sync) {
            int on = isfinite(in[0][i]) && in[0][i] > 0;
            ticks = codec_sync(&x->clock, ticks, &x->pos, period, on && !x->prev);
            x->prev = on;
        }
        for (int k = 0; k < ticks; k++) {
            frame |= x->pos == 0;
            granule |= x->pos % 576 == 0;
            x->row = x->pos % 576 / 32;
            if (++x->pos == period)
                x->pos = 0;
        }
        out[0][i] = x->row;
        out[1][i] = frame;
        out[2][i] = granule;
    }
}
static void dsp(Clock *x, t_object *chain, short *count, double hz, long vec, long fl) {
    setup_rate(x, hz, 1);
    x->sync = count[0];
    object_method(chain, gensym("dsp_add64"), x, perform, 0, NULL);
}
static void assist(Clock *x, void *b, long kind, long p, char *s) {
    const char *ports[] = {"Spectral row (0..17)", "Full-frame sync", "Granule sync"};
    snprintf(s, 512, "%s", kind == ASSIST_INLET ? "Optional full-frame sync" : ports[p]);
}
static void *create(t_symbol *s, long ac, t_atom *av) {
    int divisor = downsample_args(&ac, av);
    if (divisor < 0 || ac)
        return NULL;
    Clock *x = (Clock *)object_alloc(cls);
    if (x) {
        atomic_store(&x->divisor, divisor);
        dsp_setup(&x->obj, 1);
        x->obj.z_misc |= Z_NO_INPLACE;
        for (int j = 0; j < 3; j++)
            outlet_new((t_object *)x, "signal");
    }
    return x;
}
void C74_EXPORT ext_main(void *module) {
    cls = class_new("mp3clock~", (method)create, (method)dsp_free, sizeof(Clock), 0, A_GIMME, 0);
    class_addmethod(cls, (method)dsp, "dsp64", A_CANT, 0);
    class_addmethod(cls, (method)downsample, "downsample", A_GIMME, 0);
    class_addmethod(cls, (method)assist, "assist", A_CANT, 0);
    class_dspinit(cls);
    class_register(CLASS_BOX, cls);
}
