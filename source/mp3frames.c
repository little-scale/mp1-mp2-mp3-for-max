#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"
#include "downsample.h"
#include "field_bus.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    Fields g[2][2];
} Frame;
typedef struct {
    t_pxobject obj;
    CodecClock clock;
    _Atomic int divisor;
    int applied;
    FieldBus bus;
    Frame *ring;
    Fields output[2];
    int capacity, write_slot, used, source[2][32], channels;
    int pos, row, prev_sync, use_frame_sync, frame_sync_lane, capturing;
    _Atomic int record, clear;
} Memory;
static t_class *cls;

static void select_sources(Memory *x, double **in, long i) {
    if (atomic_exchange(&x->clear, 0))
        x->used = 0;
    x->capturing = atomic_load(&x->record);
    for (int ch = 0; ch < x->channels; ++ch)
        for (int band = 0; band < 32; ++band) {
            double v = bus_read(&x->bus, in, 5, ch, band, i, 0);
            int age = isfinite(v) ? (int)round(fmax(0, fmin(x->used, v))) : 0;
            x->source[ch][band] = (x->write_slot + x->capacity + 1 - age) % (x->capacity + 1);
        }
}
static void take_row(Memory *x, double **in, long i) {
    int gr = x->pos / 576, within = x->pos % 576;
    x->row = within / 32;
    unsigned limited = 0;
    for (int ch = 0; ch < x->channels; ++ch) {
        Fields *live = &x->ring[x->write_slot].g[gr][ch], *out = &x->output[ch];
        bus_capture(&x->bus, in, live, NULL, 0, ch, within, i, &limited);
        if (!within) {
            int first = x->source[ch][0], uniform = 1, gain = 0;
            for (int band = 0; band < 32; ++band) {
                int slot = x->source[ch][band];
                uniform &= slot == first;
                int g = x->ring[slot].g[gr][ch].gain;
                if (g > gain)
                    gain = g;
            }
            out->gain = gain;
            out->window = uniform ? x->ring[first].g[gr][ch].window : live->window;
        }
        for (int band = 0; band < 32; ++band) {
            Fields *from = &x->ring[x->source[ch][band]].g[gr][ch];
            field_rebase(out, from, 18 * band + x->row, from->gain);
        }
    }
}
static void downsample(Memory *x, t_symbol *s, long ac, t_atom *av) {
    int factor = downsample_message((t_object *)x, ac, av);
    if (factor >= 1)
        atomic_store(&x->divisor, factor);
}
static void setup_rate(Memory *x, double host, int reset) {
    int requested = atomic_load(&x->divisor);
    if (!reset && x->applied == requested)
        return;
    x->applied = requested;
    CodecClock clock;
    codec_clock_setup_divided(&clock, host, requested);
    if (!reset && clock.hz == x->clock.hz)
        return;
    x->clock = clock;
    x->write_slot = x->used = x->pos = x->row = x->prev_sync = 0;
    memset(x->output, 0, sizeof(x->output));
}
static void perform(Memory *x, t_object *chain, double **in, long nin, double **out, long nout,
                    long count, long flags, void *user) {
    setup_rate(x, x->clock.host, 0);
    int ch = x->channels, bands = 32 * ch, sync_lane = 2 * bands + 2 * ch;
    int period = x->clock.hz >= 32000 ? 1152 : 576;
    if (!x->clock.hz || nout != sync_lane + 3) {
        for (long j = 0; j < nout; ++j)
            memset(out[j], 0, count * sizeof(double));
        return;
    }
    for (long i = 0; i < count; ++i) {
        int ticks = codec_ticks(&x->clock), granule = 0, frame = 0;
        if (x->use_frame_sync || x->bus.connected[4]) {
            double v = in[x->use_frame_sync ? x->frame_sync_lane : x->bus.offset[4]][i];
            int on = isfinite(v) && v > 0;
            ticks = codec_sync(&x->clock, ticks, &x->pos, x->use_frame_sync ? period : 576,
                               on && !x->prev_sync);
            x->prev_sync = on;
        }
        for (int k = 0; k < ticks; ++k) {
            if (!x->pos) {
                select_sources(x, in, i);
                frame = 1;
            }
            if (!(x->pos % 576))
                granule = 1;
            if (!(x->pos % 32))
                take_row(x, in, i);
            if (++x->pos == period) {
                x->pos = 0;
                if (x->capturing) {
                    x->write_slot = (x->write_slot + 1) % (x->capacity + 1);
                    if (x->used < x->capacity)
                        ++x->used;
                }
            }
        }
        for (int c = 0; c < ch; ++c) {
            Fields *f = &x->output[c];
            for (int b = 0; b < 32; ++b) {
                int j = 18 * b + x->row, lane = 32 * c + b;
                out[lane][i] = f->q[j];
                out[bands + lane][i] = f->attenuation[j];
            }
            out[2 * bands + c][i] = f->gain;
            out[2 * bands + ch + c][i] = f->window;
        }
        out[sync_lane][i] = granule;
        out[sync_lane + 1][i] = x->row;
        out[sync_lane + 2][i] = frame;
    }
}
static long outputs(Memory *x, long p) {
    return p < 2 ? 32 * x->channels : p < 4 ? x->channels : 1;
}
static long changed(Memory *x, long port, long width) {
    if (port < 0 || port >= FIELD_INLETS)
        return 0;
    x->bus.width[port] = width > 0 ? width : 1;
    int old = x->channels;
    x->channels = bus_channels(&x->bus);
    return old != x->channels;
}
static void dsp(Memory *x, t_object *chain, short *count, double hz, long vec, long flags) {
    long offset = 0;
    for (int p = 0; p < FIELD_INLETS; ++p) {
        long n = (long)object_method(chain, gensym("getnuminputchannels"), x, (long)p);
        x->bus.width[p] = n > 0 ? n : 1;
        x->bus.offset[p] = offset;
        x->bus.connected[p] = count[p];
        offset += x->bus.width[p];
    }
    x->channels = bus_channels(&x->bus);
    x->use_frame_sync = count[6];
    x->frame_sync_lane = offset;
    setup_rate(x, hz, 1);
    object_method(chain, gensym("dsp_add64"), x, perform, 0, NULL);
}
static void record(Memory *x, long value) { atomic_store(&x->record, value != 0); }
static void clear(Memory *x) { atomic_store(&x->clear, 1); }
static void assist(Memory *x, void *b, long kind, long p, char *s) {
    static const char *ports[] = {"Spectral integers", "Effective attenuation", "Global gain",
                                  "Window type", "Granule sync"};
    snprintf(s, 512, "%s",
             p < 5                  ? ports[p]
             : p == 6               ? "Full-frame sync"
             : kind == ASSIST_INLET ? "MC age in frames (0 live, 1 newest)"
                                    : "Spectral row (0..17)");
}
static void destroy(Memory *x) {
    dsp_free(&x->obj);
    free(x->ring);
}
static void *create(t_symbol *s, long ac, t_atom *av) {
    int divisor = downsample_args(&ac, av);
    if (divisor < 0)
        return NULL;
    long n = ac ? atom_getlong(av) : 64;
    if (ac > 1 || (ac && atom_gettype(av) != A_LONG) || n < 1 || n > 1024) {
        error("mp3frames~: optional capacity is 1..1024 frames");
        return NULL;
    }
    Memory *x = (Memory *)object_alloc(cls);
    if (!x)
        return NULL;
    dsp_setup(&x->obj, 7);
    x->obj.z_misc |= Z_MC_INLETS | Z_NO_INPLACE;
    atomic_store(&x->divisor, divisor);
    x->capacity = (int)n;
    x->channels = 1;
    atomic_store(&x->record, 1);
    x->ring = calloc(n + 1, sizeof(Frame));
    if (!x->ring) {
        object_free(x);
        return NULL;
    }
    for (int k = 6; k >= 0; --k)
        outlet_new((t_object *)x, k < 4 ? "multichannelsignal" : "signal");
    return x;
}
void C74_EXPORT ext_main(void *module) {
    cls = class_new("mp3frames~", (method)create, (method)destroy, sizeof(Memory), 0, A_GIMME, 0);
    class_addmethod(cls, (method)outputs, "multichanneloutputs", A_CANT, 0);
    class_addmethod(cls, (method)changed, "inputchanged", A_CANT, 0);
    class_addmethod(cls, (method)dsp, "dsp64", A_CANT, 0);
    class_addmethod(cls, (method)downsample, "downsample", A_GIMME, 0);
    class_addmethod(cls, (method)assist, "assist", A_CANT, 0);
    class_addmethod(cls, (method)record, "record", A_LONG, 0);
    class_addmethod(cls, (method)clear, "clear", 0);
    class_dspinit(cls);
    class_register(CLASS_BOX, cls);
}
