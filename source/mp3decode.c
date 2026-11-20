#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"
#include "layer3.h"
#include "byte_rate.h"
#include "downsample.h"
#include <stdio.h>
#include <stdlib.h>
#include "field_hold.h"
#include "pack_writer.h"
#include "bitrate.h"
#include <sched.h>
#include <stdatomic.h>
enum { FRAME_QUEUE = 12 };
typedef struct {
    unsigned char bytes[L3_FRAME_MAX + 16];
    int used, length, prime_ticks, playing, pos, have, frozen, sounding, split_mono;
    int tail_frames, draining;
    unsigned rd, wr;
    SpectralFrame ring[FRAME_QUEUE], last, scratch;
} ByteInput;
typedef struct {
    FieldHold hold;
    float audio[2][576];
    int prev_sync;
    Rate rate;
    Fractional converter;
    ByteInput bytes;
} Stream;
typedef struct {
    PackMonitor monitor;
    PackWriter *worker;
    int bitrate;
} Preview;
typedef struct {
    t_pxobject obj;
    Stream *s;
    Synthesis *synth;
    FieldBus bus;
    _Atomic int freeze, level;
    int byte_mode, channels;
    _Atomic(Preview *) preview;
    _Atomic unsigned callbacks;
    _Atomic double bitrate;
    ByteRates *byte_rates;
    int divisor;
    _Atomic int received_kbps, received_hz, playback_hz, changing_format;
    void *status;
} External;
static t_class *cls;
static long outputs(External *x, long port) { return x->channels; }
static int count_channels(External *x) { return x->byte_mode ? 2 : bus_channels(&x->bus); }
static long changed(External *x, long port, long count) {
    if (port < 0 || port >= (x->byte_mode ? 2 : FIELD_INLETS))
        return 0;
    x->bus.width[port] = count > 0 ? count : 1;
    int n = count_channels(x), change = n != x->channels;
    x->channels = n;
    return change;
}
static void holdmode(External *x, t_symbol *mode) {
    if (mode == gensym("level") || mode == gensym("coeff"))
        atomic_store(&x->level, mode == gensym("level"));
    else
        object_error((t_object *)x, "holdmode expects coeff or level");
}
static void freeze(External *x, long value) { atomic_store(&x->freeze, value != 0); }
static void byte_push(External *x, double value) {
    if (!isfinite(value) || value < 0 || value > 255)
        return;
    ByteInput *b = &x->s->bytes;
    b->bytes[b->used++] = (unsigned char)(value + 0.5);
    if (b->used < 4)
        return;
    if (!b->length) {
        b->length = layer3_frame_length(b->bytes);
        if (!b->length) {
            memmove(b->bytes, b->bytes + 1, 3);
            b->used = 3;
            return;
        }
    }
    if (b->used == b->length) {
        atomic_store(&x->received_kbps, br[(b->bytes[1] & 8) != 0][b->bytes[2] >> 4]);
        int hz = layer3_header_rate(b->bytes);
        if (hz != atomic_load(&x->received_hz)) {
            int path = byte_rates_select(x->byte_rates, hz);
            if (path < 0) {
                b->length = b->used = 0;
                return;
            }
            /* A new rate starts a separate MP3 stream. Keep the complete packet
               and discard the old rate's reservoir, queued time and held frame. */
            layer3_clear(x->synth);
            b->rd = b->wr = 0;
            b->prime_ticks = b->playing = b->pos = b->have = b->frozen = b->sounding = 0;
            b->split_mono = b->tail_frames = b->draining = 0;
            memset(&b->last, 0, sizeof(b->last));
            atomic_store(&x->received_hz, hz);
            atomic_store(&x->playback_hz, x->byte_rates->paths[path].output.hz);
        }
        SpectralFrame *f =
            b->wr - b->rd < FRAME_QUEUE ? &b->ring[b->wr % FRAME_QUEUE] : &b->scratch;
        if (layer3_read_spectrum(x->synth, b->bytes, b->length, hz, f) && f != &b->scratch) {
            if (!b->playing && b->wr == b->rd)
                b->prime_ticks = f->granules * 576;
            ++b->wr;
        }
        b->length = b->used = 0;
    }
}
/* last contains the previous output frame, including bands that were already
   held. Keep its gated lines and update the other lines from the new packet.
   With an idle cable the open bands receive zero spectral energy. */
static void byte_mix_frame(External *x, const SpectralFrame *input, const uint32_t mask[2]) {
    ByteInput *b = &x->s->bytes;
    SpectralFrame *last = &b->last;
    int channels = last->channels;
    if (input) {
        last->granules = input->granules;
        channels = input->channels;
    }
    if (channels == 1) {
        if (mask[0] != mask[1] || (last->channels == 2 && mask[0]))
            b->split_mono = 1;
        if (b->split_mono)
            channels = 2;
    } else if (input)
        b->split_mono = 0;
    if (last->channels == 1 && channels == 2) {
        /* A held right band inherits the mono frame that was audible. */
        for (int gr = 0; gr < last->granules; ++gr)
            last->gr[gr][1] = last->gr[gr][0];
        if (b->split_mono || mask[1])
            layer3_split_mono(x->synth);
    }
    last->channels = channels;
    for (int gr = 0; gr < last->granules; ++gr) {
        for (int ch = 0; ch < channels; ++ch) {
            Spectrum *dst = &last->gr[gr][ch];
            const Spectrum *src = input ? &input->gr[gr][input->channels == 1 ? 0 : ch] : NULL;
            if (!mask[ch] && src) {
                *dst = *src;
                continue;
            }
            if (src)
                dst->window = src->window;
            for (int band = 0; band < 32; ++band) {
                if (mask[ch] & (UINT32_C(1) << band))
                    continue;
                if (src)
                    memcpy(dst->lines + 18 * band, src->lines + 18 * band, 18 * sizeof(float));
                else
                    memset(dst->lines + 18 * band, 0, 18 * sizeof(float));
            }
        }
    }
}
static void bytes_tick(External *x, int freeze, double **in, long i, float sample[2]) {
    Stream *s = x->s;
    ByteInput *b = &s->bytes;
    if (b->prime_ticks)
        --b->prime_ticks;
    else if (b->rd != b->wr)
        b->playing = 1;
    if (!b->pos) {
        int incoming = b->playing && b->rd != b->wr;
        b->frozen = freeze && b->have;
        uint32_t mask[2] = {0, 0};
        if (b->have && !b->frozen) {
            for (int ch = 0; ch < 2; ++ch)
                mask[ch] = bus_hold_mask(&x->bus, 1, in, ch, i);
        }
        int partial = mask[0] || mask[1];
        if (!b->frozen && (incoming || partial)) {
            const SpectralFrame *frame = incoming ? &b->ring[b->rd % FRAME_QUEUE] : NULL;
            byte_mix_frame(x, frame, mask);
            b->have = 1;
        }
        if (incoming)
            ++b->rd;
        else
            b->playing = 0;
        int held = b->frozen || partial;
        if (incoming || held)
            b->tail_frames = held ? 2 / b->last.granules : 0;
        b->draining = !incoming && !held && b->tail_frames > 0;
        if (b->draining)
            --b->tail_frames;
        b->sounding = incoming || held || b->draining;
    }
    if (!b->sounding) {
        sample[0] = sample[1] = 0;
        return;
    }
    int within = b->pos % 576;
    if (!within) {
        /* On release into idle, let the IMDCT and synthesis filters finish
           without erasing the saved frame or advancing the input reservoir. */
        static const Spectrum silence = {0};
        for (int ch = 0; ch < b->last.channels; ch++) {
            const Spectrum *spectrum = b->draining ? &silence : &b->last.gr[b->pos / 576][ch];
            layer3_render_spectrum(x->synth, spectrum, ch, s->audio[ch]);
        }
    }
    sample[0] = s->audio[0][within];
    sample[1] = s->audio[b->last.channels - 1][within];
    if (++b->pos == b->last.granules * 576)
        b->pos = 0;
}
static void byte_sample(External *x, int freeze, double **in, double **out, long i) {
    ByteRates *bank = x->byte_rates;
    if (bank->active < 0) {
        out[0][i] = out[1][i] = 0;
        return;
    }
    ByteRate *b = &bank->paths[bank->active];
    Rate *r = &b->output, *d = &b->decimator;
    int ticks = codec_ticks(&b->source_clock);
    codec_ticks(&r->clock);
    float sample[2] = {0};
    for (int k = 0; k < ticks; ++k) {
        float v[2];
        bytes_tick(x, freeze, in, i, v);
        if (d->factor > 1)
            rate_push(d, v[0], v[1]);
        if (b->until_next) {
            --b->until_next;
            continue;
        }
        b->until_next = d->factor - 1;
        for (int c = 0; c < 2; ++c)
            sample[c] = d->factor > 1 ? rate_filter(d, c) : v[c];
        if (r->factor < 0)
            fractional_push(&b->interpolator, sample[0], sample[1]);
    }
    if (r->factor > 1)
        rate_push(r, sample[0], sample[1]);
    for (int c = 0; c < 2; ++c)
        out[c][i] = r->factor == 1 ? sample[c]
                    : r->factor > 1
                        ? rate_interpolate(r, c)
                        : fractional_read(&b->interpolator, c, -r->clock.phase / r->clock.host);
}
static Preview *create_preview(void) {
    Preview *p = calloc(1, sizeof(*p));
    if (!p)
        return NULL;
    monitor_enable(&p->monitor, 1);
    p->worker = writer_preview(&p->monitor);
    if (!p->worker) {
        free(p);
        return NULL;
    }
    return p;
}
static void release_preview(External *x) {
    Preview *p = atomic_exchange(&x->preview, NULL);
    if (!p)
        return;
    while (atomic_load(&x->callbacks))
        sched_yield();
    writer_close(p->worker);
    free(p);
}
static void getbitrate_deferred(External *x, t_symbol *s, long ac, t_atom *av) {
    double req = atomic_load(&x->bitrate);
    int kbps = x->byte_mode ? atomic_load(&x->received_kbps)
               : req > 0    ? pack_bitrate(x->s->rate.hz, req)
                            : 0;
    t_atom a;
    atom_setlong(&a, kbps);
    outlet_anything(x->status, gensym("bitrate"), 1, &a);
}
static void getbitrate(External *x) { defer(x, (method)getbitrate_deferred, NULL, 0, NULL); }
static void bitrate_deferred(External *x, t_symbol *s, long ac, t_atom *av) {
    double kbps = atom_getfloat(av);
    if (!kbps) {
        atomic_store(&x->bitrate, 0);
        release_preview(x);
    } else {
        Preview *p = atomic_load(&x->preview);
        if (!p) {
            p = create_preview();
            if (!p) {
                object_error((t_object *)x, "cannot start bitrate audition");
                return;
            }
            atomic_store(&x->bitrate, kbps);
            atomic_store(&x->preview, p);
        } else
            atomic_store(&x->bitrate, kbps);
    }
    getbitrate(x);
}
static void bitrate(External *x, double kbps) {
    if (x->byte_mode) {
        object_post((t_object *)x, "Use bitrate on the upstream encoder or packer. getbitrate "
                                   "reports the received frame rate in kb/s.");
        return;
    }
    if (!isfinite(kbps) || kbps < 0) {
        object_error((t_object *)x, "bitrate expects a nonnegative number in kb/s");
        return;
    }
    t_atom a;
    atom_setfloat(&a, fmin(kbps, 320.));
    defer(x, (method)bitrate_deferred, NULL, 1, &a);
}
static void perform(External *x, t_object *chain, double **ins, long nin, double **out, long nout,
                    long n, long flags, void *user) {
    atomic_fetch_add(&x->callbacks, 1);
    if (atomic_load(&x->changing_format) || !x->s->rate.factor || nout != x->channels) {
        for (long k = 0; k < nout; k++)
            memset(out[k], 0, n * sizeof(double));
        atomic_fetch_sub(&x->callbacks, 1);
        return;
    }
    Stream *s = x->s;
    Rate *r = &s->rate;
    s->hold.request_level = atomic_load(&x->level);
    int freeze = atomic_load(&x->freeze);
    if (x->byte_mode) {
        for (long i = 0; i < n; ++i) {
            if (x->bus.connected[0])
                byte_push(x, ins[0][i]);
            byte_sample(x, freeze, ins, out, i);
        }
        atomic_fetch_sub(&x->callbacks, 1);
        return;
    }
    Preview *preview = atomic_load(&x->preview);
    for (long i = 0; i < n; i++) {
        int ticks = codec_ticks(&r->clock);
        if (x->bus.connected[4]) {
            double level = ins[x->bus.offset[4]][i];
            int sync = isfinite(level) && level > 0;
            ticks = codec_sync(&r->clock, ticks, &s->hold.pos, 576, sync && !s->prev_sync);
            s->prev_sync = sync;
        }
        float sample[2] = {0};
        for (int k = 0; k < ticks; k++) {
            FieldHold *h = &s->hold;
            int pos = h->pos % 576, gr = h->pos / 576, ng = r->hz >= 32000 ? 2 : 1;
            if (preview && !h->pos)
                preview->bitrate = pack_bitrate(r->hz, atomic_load(&x->bitrate));
            unsigned ignored = 0;
            hold_capture(h, x->channels, freeze, &x->bus, ins, i, &ignored);
            for (int ch = 0; ch < x->channels; ch++) {
                sample[ch] = s->audio[ch][pos];
                if (pos == 575)
                    layer3_render(x->synth, &hold_output(h)[gr][ch], ch, s->audio[ch]);
            }
            if (preview) {
                if (h->pos + 1 == ng * 576 && preview->bitrate)
                    writer_offer_at(preview->worker, r->hz, x->channels, hold_output(h), 0,
                                    preview->bitrate, 0);
                float packed[2];
                monitor_tick(&preview->monitor, 1, r->hz, x->channels, packed);
                if (preview->monitor.playing) {
                    sample[0] = packed[0];
                    sample[1] = packed[1];
                }
            }
            hold_advance(h, ng, x->channels);
            if (r->factor < 0)
                fractional_push(&s->converter, sample[0], sample[1]);
        }
        if (r->factor > 1)
            rate_push(r, sample[0], sample[1]);
        for (int ch = 0; ch < x->channels; ch++)
            out[ch][i] = r->factor == 1 ? sample[ch]
                         : r->factor > 1
                             ? r->factor * rate_filter(r, ch)
                             : fractional_read(&s->converter, ch, -r->clock.phase / r->clock.host);
    }
    atomic_fetch_sub(&x->callbacks, 1);
}
static int configure(External *x, double host, int factor, int restart) {
    int hz = downsample_hz(host, factor);
    Fractional converter = {0};
    ByteRates *bank = NULL;
    Preview *preview = NULL;
    Rate rate;
    if (!hz || !rate_setup_native(&rate, host, hz))
        goto failed;
    if (x->byte_mode) {
        bank = byte_rates_new(host, hz, factor);
        if (!bank)
            goto failed;
    } else {
        if (rate.factor < 0 && !fractional_setup(&converter, hz, host))
            goto failed;
        if (atomic_load(&x->bitrate) > 0 && !(preview = create_preview()))
            goto failed;
    }
    downsample_quiesce(&x->changing_format, &x->callbacks);
    ByteRates *previous_bank = x->byte_rates;
    Preview *previous_preview = atomic_exchange(&x->preview, preview);
    Fractional old = x->s->converter;
    int received = atomic_load(&x->received_hz);
    if (restart || !x->byte_mode) {
        memset(x->s, 0, sizeof(*x->s));
        layer3_clear(x->synth);
        atomic_store(&x->received_kbps, 0);
        atomic_store(&x->received_hz, 0);
        received = 0;
    }
    x->s->converter = converter;
    x->s->rate = rate;
    x->byte_rates = bank;
    int active = bank && received ? byte_rates_select(bank, received) : -1;
    atomic_store(&x->playback_hz, active < 0 ? hz : bank->paths[active].output.hz);
    x->divisor = factor;
    atomic_store(&x->changing_format, 0);
    if (previous_preview) {
        writer_close(previous_preview->worker);
        free(previous_preview);
    }
    byte_rates_free(previous_bank);
    fractional_free(&old);
    return 1;
failed:
    byte_rates_free(bank);
    fractional_free(&converter);
    object_error((t_object *)x, "cannot prepare decoder at %d Hz", hz);
    return 0;
}
static void getrate_deferred(External *x, t_symbol *s, long ac, t_atom *av) {
    downsample_status(x->status, x->s->rate.clock.host, atomic_load(&x->playback_hz), x->channels);
}
static void getrate(External *x) { defer(x, (method)getrate_deferred, NULL, 0, NULL); }
static void downsample_deferred(External *x, t_symbol *s, long ac, t_atom *av) {
    int factor = downsample_message((t_object *)x, ac, av);
    if (factor < 0)
        return;
    double host = x->s->rate.clock.host;
    if (!host || (downsample_hz(host, factor) == x->s->rate.hz &&
                  (!x->byte_mode || (factor == 1) == (x->divisor == 1))))
        x->divisor = factor;
    else if (!configure(x, host, factor, 0))
        return;
    if (host)
        getrate(x);
}
static void downsample(External *x, t_symbol *s, long ac, t_atom *av) {
    defer(x, (method)downsample_deferred, s, ac, av);
}
static void dsp(External *x, t_object *chain, short *count, double rate, long vec, long flags) {
    long offset = 0;
    for (int j = 0; j < (x->byte_mode ? 2 : FIELD_INLETS); ++j) {
        x->bus.width[j] = (long)object_method(chain, gensym("getnuminputchannels"), x, (long)j);
        if (x->bus.width[j] < 1)
            x->bus.width[j] = 1;
        x->bus.offset[j] = offset;
        offset += x->bus.width[j];
        x->bus.connected[j] = count[j];
    }
    x->channels = count_channels(x);
    if (!configure(x, rate, x->divisor, 1))
        x->s->rate.factor = 0;
    object_method(chain, gensym("dsp_add64"), x, perform, 0, NULL);
}
static void destroy(External *x) {
    dsp_free(&x->obj);
    release_preview(x);
    if (x->s)
        fractional_free(&x->s->converter);
    byte_rates_free(x->byte_rates);
    free(x->s);
    layer3_free(x->synth);
}
static void assist(External *x, void *b, long kind, long port, char *s) {
    const char *in[] = {"Signed quantized coefficients (32 MC lanes per channel)",
                        "Effective attenuation (MC, quarter-octave steps; default 0)",
                        "Global gain (0..255; default 160)",
                        "Window (0 long, 1 start, 2 short, 3 stop, 4/5 mixed)",
                        "Granule sync (optional; free-runs when disconnected)",
                        "MC band hold (1 holds, 0 follows)"};
    snprintf(s, 512, "%s",
             kind != ASSIST_INLET ? (port ? "Rate and bitrate status" : "Reconstructed MC audio")
             : x->byte_mode ? (port == 0 ? "MP3 bytes (0..255; -1 idle)" : in[BAND_HOLD_INLET])
                            : in[port]);
}
static void *create(t_symbol *s, long ac, t_atom *av) {
    int divisor = downsample_args(&ac, av);
    if (divisor < 0)
        return NULL;
    if (ac && (ac != 1 || atom_gettype(av) != A_SYM || atom_getsym(av) != gensym("bytes"))) {
        error("mp3decode~: optional argument is bytes");
        return NULL;
    }
    External *x = (External *)object_alloc(cls);
    if (!x)
        return NULL;
    x->divisor = divisor;
    x->byte_mode = ac != 0;
    x->channels = ac ? 2 : 1;
    dsp_setup(&x->obj, ac ? 2 : FIELD_INLETS);
    x->obj.z_misc |= Z_MC_INLETS | Z_NO_INPLACE;
    x->s = calloc(1, sizeof(*x->s));
    x->synth = layer3_new();
    if (!x->s || !x->synth) {
        object_free(x);
        return NULL;
    }
    x->status = outlet_new((t_object *)x, NULL);
    outlet_new((t_object *)x, "multichannelsignal");
    return x;
}
void C74_EXPORT ext_main(void *module) {
    layer3_tables();
    cls = class_new("mp3decode~", (method)create, (method)destroy, sizeof(External), 0, A_GIMME, 0);
    class_addmethod(cls, (method)outputs, "multichanneloutputs", A_CANT, 0);
    class_addmethod(cls, (method)changed, "inputchanged", A_CANT, 0);
    class_addmethod(cls, (method)dsp, "dsp64", A_CANT, 0);
    class_addmethod(cls, (method)assist, "assist", A_CANT, 0);
    class_addmethod(cls, (method)bitrate, "bitrate", A_FLOAT, 0);
    class_addmethod(cls, (method)bitrate, "kbps", A_FLOAT, 0);
    class_addmethod(cls, (method)getbitrate, "getbitrate", 0);
    class_addmethod(cls, (method)downsample, "downsample", A_GIMME, 0);
    class_addmethod(cls, (method)getrate, "getrate", 0);
    class_addmethod(cls, (method)holdmode, "holdmode", A_SYM, 0);
    class_addmethod(cls, (method)freeze, "freeze", A_LONG, 0);
    class_dspinit(cls);
    class_register(CLASS_BOX, cls);
}
