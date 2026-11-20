#include "ext.h"
#include "z_dsp.h"
#include "field_bus.h"
#include <stdio.h>
typedef struct {
    t_pxobject obj;
    long width[4], offset[4];
    short connected[4];
    int channels;
} Short;
static t_class *cls;
static int channels(Short *x) {
    return 1 + (x->width[0] > 32 || x->width[1] > 1 || x->width[3] > 3);
}
static long changed(Short *x, long port, long width) {
    if (port < 0 || port > 3)
        return 0;
    int old = x->channels;
    x->width[port] = width;
    x->channels = channels(x);
    return old != x->channels;
}
static long outputs(Short *x, long p) { return 32 * x->channels; }
static double read_lane(Short *x, double **in, int port, int lane, long i) {
    if (x->width[port] == 1)
        lane = 0;
    return x->connected[port] && lane < x->width[port] ? in[x->offset[port] + lane][i] : 0;
}
static void perform(Short *x, t_object *chain, double **in, long ni, double **out, long no, long n,
                    long fl, void *u) {
    if (no != 32 * x->channels) {
        for (long c = 0; c < no; c++)
            memset(out[c], 0, n * sizeof(double));
        return;
    }
    for (long i = 0; i < n; i++) {
        unsigned ignored = 0;
        int row = bus_limit(read_lane(x, in, 2, 0, i), 0, 17, &ignored);
        for (int ch = 0; ch < x->channels; ch++) {
            int window = bus_limit(read_lane(x, in, 1, ch, i), 0, 5, &ignored);
            int first = window == 2 ? 0 : window == 4 ? 2 : window == 5 ? 4 : 32;
            int lane = (x->width[3] > 3 ? 3 * ch : 0) + row % 3;
            double db = read_lane(x, in, 3, lane, i);
            double delta =
                isfinite(db) ? round(fmax(-210., fmin(210., -db * (4. / 6.020599913279624)))) : 0;
            for (int band = 0; band < 32; band++) {
                int source = (x->width[0] > 32 ? 32 * ch : 0) + band;
                double a = read_lane(x, in, 0, source, i);
                a = bus_limit(a, 0, 210, &ignored);
                out[ch * 32 + band][i] = band < first ? a : fmax(0., fmin(210., a + delta));
            }
        }
    }
}
static void dsp(Short *x, t_object *chain, short *count, double hz, long vec, long fl) {
    long off = 0;
    for (int j = 0; j < 4; j++) {
        long n = (long)object_method(chain, gensym("getnuminputchannels"), x, (long)j);
        x->width[j] = n > 0 ? n : 1;
        x->offset[j] = off;
        off += x->width[j];
        x->connected[j] = count[j];
    }
    x->channels = channels(x);
    object_method(chain, gensym("dsp_add64"), x, perform, 0, NULL);
}
static void assist(Short *x, void *b, long kind, long p, char *s) {
    const char *ports[] = {"Effective attenuation (MC)", "Window (one lane per channel)",
                           "Spectral row (0..17)",
                           "Short-window trims in dB (3 shared or 6 stereo lanes)"};
    snprintf(s, 512, "%s", kind == ASSIST_INLET ? ports[p] : "Adjusted effective attenuation (MC)");
}
static void *create(void) {
    Short *x = (Short *)object_alloc(cls);
    if (x) {
        x->channels = 1;
        dsp_setup(&x->obj, 4);
        x->obj.z_misc |= Z_MC_INLETS | Z_NO_INPLACE;
        outlet_new((t_object *)x, "multichannelsignal");
    }
    return x;
}
void C74_EXPORT ext_main(void *module) {
    cls = class_new("mp3short~", (method)create, (method)dsp_free, sizeof(Short), 0, 0);
    class_addmethod(cls, (method)changed, "inputchanged", A_CANT, 0);
    class_addmethod(cls, (method)outputs, "multichanneloutputs", A_CANT, 0);
    class_addmethod(cls, (method)dsp, "dsp64", A_CANT, 0);
    class_addmethod(cls, (method)assist, "assist", A_CANT, 0);
    class_dspinit(cls);
    class_register(CLASS_BOX, cls);
}
