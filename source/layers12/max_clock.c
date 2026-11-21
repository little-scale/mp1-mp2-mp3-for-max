#include "max_common.h"
typedef struct {
    t_pxobject obj;
    Timing timing;
    double host;
    int sync, row;
    _Atomic int divisor;
} Clock;
static t_class *cls;
static void perform(Clock *x, t_object *chain, double **in, long ni, double **out, long no, long n,
                    long fl, void *u) {
    int f = atomic_load(&x->divisor);
    if (x->timing.clock.hz != downsample_hz(x->host, f))
        timing_setup(&x->timing, x->host, f);
    for (int i = 0; i < n; i++) {
        double v = x->sync ? in[0][i] : timing_mark(&x->timing);
        int mark = isfinite(v) && v == floor(v) && v >= -1 && v <= sb_rows(LAYER) ? (int)v : 0;
        if (mark > 0)
            x->row = mark - 1;
        out[0][i] = x->row;
        out[1][i] = mark == 1;
        out[2][i] = mark > 0 && x->row % 12 == 0;
        out[3][i] = mark;
    }
}
static void dsp(Clock *x, t_object *chain, short *connected, double host, long v, long f) {
    x->host = host;
    x->sync = connected[0];
    x->row = 0;
    timing_setup(&x->timing, host, atomic_load(&x->divisor));
    object_method(chain, gensym("dsp_add64"), x, perform, 0, NULL);
}
static void down(Clock *x, t_symbol *s, long ac, t_atom *av) {
    set_divisor((t_object *)x, &x->divisor, ac, av);
}
static void assist(Clock *x, void *b, long io, long p, char *s) {
    const char *o[] = {"Slot index", "Frame start", "Scalefactor group start",
                       "Slot marker for field objects"};
    snprintf(s, 512, "%s", io == ASSIST_INLET ? "Optional slot marker from a field source" : o[p]);
}
static void *create(t_symbol *s, long ac, t_atom *av) {
    int d = downsample_args(&ac, av);
    if (d < 0 || ac)
        return NULL;
    Clock *x = (Clock *)object_alloc(cls);
    if (!x)
        return NULL;
    x->divisor = d;
    dsp_setup(&x->obj, 1);
    x->obj.z_misc |= Z_NO_INPLACE;
    for (int i = 0; i < 4; i++)
        outlet_new((t_object *)x, "signal");
    return x;
}
void C74_EXPORT ext_main(void *m) {
    cls = class_new(MP "clock~", (method)create, (method)dsp_free, sizeof(Clock), 0, A_GIMME, 0);
    class_addmethod(cls, (method)dsp, "dsp64", A_CANT, 0);
    class_addmethod(cls, (method)down, "downsample", A_GIMME, 0);
    class_addmethod(cls, (method)assist, "assist", A_CANT, 0);
    class_dspinit(cls);
    class_register(CLASS_BOX, cls);
}
