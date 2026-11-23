#include "bands2.h"

typedef struct {
    t_pxobject object;
    Layer2Bands *engine;
    double host;
    _Atomic int divisor;
    BandWordMode mode;
    int ready;
} Bands;
static t_class *bands_class;

static void perform(Bands *x, t_object *chain, double **in, long nin, double **out, long nout,
                    long n, long flags, void *user) {
    if (!x->ready) {
        zeros(out, nout, n);
        return;
    }
    int hz = downsample_hz(x->host, atomic_load(&x->divisor));
    bands2_render(x->engine, hz, x->mode, in, out, (int)n);
}
static void dsp(Bands *x, t_object *chain, short *connected, double host, long n, long flags) {
    bus_setup(&x->engine->inputs, x, chain, connected, 4);
    x->host = host;
    x->engine->hz = 0;
    x->ready = pcm_setup(&x->engine->bank, host, 0);
    if (!x->ready)
        object_error((t_object *)x, "could not prepare sample-rate conversion");
    object_method(chain, gensym("dsp_add64"), x, perform, 0, NULL);
}
static void downsample(Bands *x, t_symbol *sym, long ac, t_atom *av) {
    set_divisor((t_object *)x, &x->divisor, ac, av);
}
static void assist(Bands *x, void *box, long kind, long port, char *text) {
    const char *inputs[] = {"MC subband samples (normalized by default)",
        "MC destination bands 0..31 (mono rotates all lanes)",
        "MC Layer II allocation class per destination band (0 mute, 1..17 active)",
        "MC scalefactor index per destination band (0..62, lower is louder)"};
    const char *outputs[] = {"Decoded mono audio", "Scan column (0..31)", "Frame start pulse"};
    snprintf(text, 512, "(signal) %s", kind == ASSIST_INLET ? inputs[port] : outputs[port]);
}
static void destroy(Bands *x) {
    dsp_free(&x->object);
    if (x->engine) {
        pcm_free(&x->engine->bank);
        free(x->engine);
    }
}
static void *create(t_symbol *sym, long ac, t_atom *av) {
    int divisor = downsample_args(&ac, av);
    if (divisor < 0)
        return NULL;
    BandWordMode mode = BAND_NORMALIZED;
    if (ac) {
        t_symbol *name = ac == 1 && atom_gettype(av) == A_SYM ? atom_getsym(av) : NULL;
        if (name == gensym("raw"))
            mode = BAND_RAW;
        else if (name == gensym("signed"))
            mode = BAND_SIGNED;
        else {
            error("mp2bands~ expects signed, raw, or no argument for normalized oscillators");
            return NULL;
        }
    }
    Bands *x = (Bands *)object_alloc(bands_class);
    if (!x)
        return NULL;
    dsp_setup(&x->object, 4);
    x->object.z_misc |= Z_MC_INLETS | Z_NO_INPLACE;
    x->engine = calloc(1, sizeof(*x->engine));
    if (!x->engine) {
        object_free(x);
        return NULL;
    }
    x->divisor = divisor;
    x->mode = mode;
    for (int i = 0; i < 3; ++i)
        outlet_new((t_object *)x, "signal");
    return x;
}
void C74_EXPORT ext_main(void *module) {
    filter_init();
    sb_tables();
    bands_class = class_new("mp2bands~", (method)create, (method)destroy, sizeof(Bands),
                            NULL, A_GIMME, 0);
    class_addmethod(bands_class, (method)dsp, "dsp64", A_CANT, 0);
    class_addmethod(bands_class, (method)assist, "assist", A_CANT, 0);
    class_addmethod(bands_class, (method)downsample, "downsample", A_GIMME, 0);
    class_dspinit(bands_class);
    class_register(CLASS_BOX, bands_class);
}
