#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"
#include "encoder.h"
#include "rate.h"
#include "downsample.h"
#include "bitrate.h"
#include <stdatomic.h>
#include <stdio.h>
typedef struct {
    t_pxobject obj;
    Encoder *enc;
    const EncodedFrame *frame;
    Rate rate;
    Fractional converter;
    int channels, pos, visible, byte_pos, frame_samples;
    int divisor;
    _Atomic int changing_format;
    _Atomic unsigned callbacks;
    _Atomic double bitrate;
    void *status;
} External;
static t_class *cls;
static long outputs(External *x, long port) {
    return port < 2 ? 32 * x->channels : port < 4 ? x->channels : 1;
}
static long changed(External *x, long idx, long n) {
    int channels = n > 1 ? 2 : 1;
    int change = channels != x->channels;
    x->channels = channels;
    return change;
}
static float input(double v) { return isfinite(v) ? (float)fmax(-1., fmin(1., v)) : 0; }
static void perform(External *x, t_object *chain, double **in, long nin, double **out, long nout,
                    long n, long flags, void *user) {
    atomic_fetch_add(&x->callbacks, 1);
    int channels = x->channels, bands = channels * 32;
    int sync_lane = bands * 2 + channels * 2;
    if (atomic_load(&x->changing_format) || !x->enc || nout != sync_lane + 4) {
        for (long k = 0; k < nout; ++k)
            memset(out[k], 0, n * sizeof(double));
        if (nout > sync_lane + 1)
            for (long i = 0; i < n; ++i)
                out[sync_lane + 1][i] = -1;
        atomic_fetch_sub(&x->callbacks, 1);
        return;
    }
    for (long i = 0; i < n; ++i) {
        float l = input(in[0][i]), r = channels == 2 ? input(in[1][i]) : l;
        Rate *rclock = &x->rate;
        int sync = 0, frame_start = 0, ticks = codec_ticks(&rclock->clock);
        if (rclock->factor > 1)
            rate_push(rclock, l, r);
        if (rclock->factor < 0)
            fractional_push(&x->converter, l, r);
        for (int k = 0; k < ticks; k++) {
            if (!x->pos) {
                encoder_set_bitrate(
                    x->enc, live_bitrate(rclock->hz, rclock->clock.host, atomic_load(&x->bitrate)));
                frame_start = 1;
                x->frame = encoder_next(x->enc);
                x->byte_pos = 0;
            }
            if (rclock->factor > 1) {
                l = rate_filter(rclock, 0);
                r = rate_filter(rclock, 1);
            }
            if (rclock->factor < 0) {
                double age =
                    ((ticks - 1 - k) * rclock->clock.host + rclock->clock.phase) / rclock->hz;
                l = fractional_read(&x->converter, 0, age);
                r = fractional_read(&x->converter, 1, age);
            }
            encoder_feed(x->enc, l, r);
            sync |= x->pos % 576 == 0;
            x->visible = x->pos;
            if (++x->pos == x->frame_samples)
                x->pos = 0;
        }
        const EncodedFrame *frame = x->frame;
        int gr = x->visible / 576, line = (x->visible % 576) / 32;
        for (int ch = 0; ch < channels; ++ch) {
            const Fields *f = frame ? &frame->fields[gr][ch] : NULL;
            for (int b = 0; b < 32; ++b) {
                int k = ch * 32 + b, j = b * 18 + line;
                out[k][i] = f ? f->q[j] : 0;
                out[bands + k][i] = f ? f->attenuation[j] : 0;
            }
            out[2 * bands + ch][i] = f ? f->gain : 160;
            out[2 * bands + channels + ch][i] = f ? f->window : 0;
        }
        out[sync_lane][i] = sync;
        out[sync_lane + 2][i] = line;
        out[sync_lane + 3][i] = frame_start;
        out[sync_lane + 1][i] =
            ticks && frame && x->byte_pos < frame->length ? frame->bytes[x->byte_pos++] : -1;
    }
    atomic_fetch_sub(&x->callbacks, 1);
}
static int configure(External *x, double host, int channels, int factor) {
    int hz = downsample_hz(host, factor);
    Rate rate;
    Fractional converter = {0};
    int kbps = live_bitrate(hz, host, atomic_load(&x->bitrate));
    Encoder *next = NULL;
    if (hz && rate_setup_native(&rate, host, hz) &&
        (rate.factor >= 0 || fractional_setup(&converter, host, hz)))
        next = encoder_new(hz, channels, kbps);
    if (!next) {
        fractional_free(&converter);
        object_error((t_object *)x, "cannot initialize encoder at %d Hz and %d kb/s", hz, kbps);
        return 0;
    }
    downsample_quiesce(&x->changing_format, &x->callbacks);
    Encoder *previous = x->enc;
    Fractional old = x->converter;
    x->divisor = factor;
    x->enc = next;
    x->frame = NULL;
    x->pos = x->visible = x->byte_pos = 0;
    x->frame_samples = hz >= 32000 ? 1152 : 576;
    x->rate = rate;
    x->converter = converter;
    atomic_store(&x->changing_format, 0);
    encoder_free(previous);
    fractional_free(&old);
    return 1;
}
static void getrate_deferred(External *x, t_symbol *s, long ac, t_atom *av) {
    downsample_status(x->status, x->rate.clock.host, x->rate.hz, x->channels);
}
static void getrate(External *x) { defer(x, (method)getrate_deferred, NULL, 0, NULL); }
static void downsample_deferred(External *x, t_symbol *s, long ac, t_atom *av) {
    int factor = downsample_message((t_object *)x, ac, av);
    if (factor < 0)
        return;
    double host = x->rate.clock.host;
    if (!host || downsample_hz(host, factor) == x->rate.hz)
        x->divisor = factor;
    else if (!configure(x, host, x->channels, factor))
        return;
    if (host)
        getrate(x);
}
static void downsample(External *x, t_symbol *s, long ac, t_atom *av) {
    defer(x, (method)downsample_deferred, s, ac, av);
}
static void dsp(External *x, t_object *chain, short *count, double rate, long vec, long flags) {
    int ch = (long)object_method(chain, gensym("getnuminputchannels"), x, 0) > 1 ? 2 : 1;
    x->channels = ch;
    if (!configure(x, rate, ch, x->divisor)) {
        encoder_free(x->enc);
        x->enc = NULL;
        x->frame = NULL;
        x->rate = (Rate){0};
        x->channels = ch;
    }
    object_method(chain, gensym("dsp_add64"), x, perform, 0, NULL);
}
static void destroy(External *x) {
    dsp_free(&x->obj);
    fractional_free(&x->converter);
    encoder_free(x->enc);
}
static void getbitrate_deferred(External *x, t_symbol *s, long ac, t_atom *av) {
    if (!x->rate.hz)
        return;
    t_atom a;
    atom_setlong(&a, live_bitrate(x->rate.hz, x->rate.clock.host, atomic_load(&x->bitrate)));
    outlet_anything(x->status, gensym("bitrate"), 1, &a);
}
static void getbitrate(External *x) { defer(x, (method)getbitrate_deferred, NULL, 0, NULL); }
static void bitrate(External *x, double kbps) {
    if (!isfinite(kbps) || kbps < 0) {
        object_error((t_object *)x, "bitrate expects a nonnegative number in kb/s");
        return;
    }
    atomic_store(&x->bitrate, fmin(kbps, 320.));
    getbitrate(x);
}
static void stats(External *x) {
    object_post((t_object *)x,
                "Worker queue drops, underruns and encode errors since DSP start: %u",
                encoder_dropouts(x->enc));
}
static void assist(External *x, void *b, long kind, long port, char *s) {
    const char *outs[] = {"Quantized spectral integers (32 MC lanes per audio channel)",
                          "Effective scalefactor attenuation (MC, quarter-octave steps)",
                          "Global gain (one lane per audio channel)",
                          "Window type (one lane per audio channel)",
                          "Granule sync pulse",
                          "MP3 bytes: integers 0..255, -1 idle",
                          "Spectral row (0..17)",
                          "Full-frame sync",
                          "Encoder rate and bitrate status"};
    snprintf(s, 512, "%s", kind == ASSIST_INLET ? "Audio (mono or two-channel MC)" : outs[port]);
}
static void *create(t_symbol *s, long ac, t_atom *av) {
    int divisor = downsample_args(&ac, av);
    if (divisor < 0)
        return NULL;
    if (ac > 1 ||
        (ac && (atom_gettype(av) != A_LONG || atom_getlong(av) < 8 || atom_getlong(av) > 320))) {
        error("mp3encode~: optional argument is an integer bitrate in kb/s (8..320)");
        return NULL;
    }
    External *x = (External *)object_alloc(cls);
    if (!x)
        return NULL;
    x->divisor = divisor;
    x->channels = 1;
    x->bitrate = ac ? (int)atom_getlong(av) : 0;
    x->enc = NULL;
    dsp_setup(&x->obj, 1);
    x->obj.z_misc |= Z_MC_INLETS | Z_NO_INPLACE;
    x->status = outlet_new((t_object *)x, NULL);
    for (int k = 7; k >= 0; --k)
        outlet_new((t_object *)x, k < 4 ? "multichannelsignal" : "signal");
    return x;
}
void C74_EXPORT ext_main(void *module) {
    layer3_tables();
    cls = class_new("mp3encode~", (method)create, (method)destroy, sizeof(External), 0, A_GIMME, 0);
    class_addmethod(cls, (method)outputs, "multichanneloutputs", A_CANT, 0);
    class_addmethod(cls, (method)changed, "inputchanged", A_CANT, 0);
    class_addmethod(cls, (method)dsp, "dsp64", A_CANT, 0);
    class_addmethod(cls, (method)bitrate, "bitrate", A_FLOAT, 0);
    class_addmethod(cls, (method)bitrate, "kbps", A_FLOAT, 0);
    class_addmethod(cls, (method)getbitrate, "getbitrate", 0);
    class_addmethod(cls, (method)downsample, "downsample", A_GIMME, 0);
    class_addmethod(cls, (method)getrate, "getrate", 0);
    class_addmethod(cls, (method)stats, "stats", 0);
    class_addmethod(cls, (method)assist, "assist", A_CANT, 0);
    class_dspinit(cls);
    class_register(CLASS_BOX, cls);
}
