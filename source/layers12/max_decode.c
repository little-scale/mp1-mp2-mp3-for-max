#include "max_common.h"
#include "pcm.h"
#include "worker.h"
#if LAYER == 1
#include "layer1.h"
#endif
enum { FIELDS, BYTES, SERIAL };
typedef struct {
    Capture capture;
    Timing timing;
    Playback playback;
    Converter converter;
    BandFrame converted, idle;
    Packet packet;
    unsigned char stream[SB_PACKET];
    int used, need, src, dst, src_ch, preview, wait, ended, grace, header_channels;
    int packet_timeout, partial_wait;
    unsigned epoch;
#if LAYER == 1
    Layer1 serial;
#endif
} Engine;
typedef struct {
    t_pxobject obj;
    Engine *e;
    Worker *worker;
    Bus bus;
    int mode, channels, raw, valid;
    double host;
    _Atomic int divisor, freeze, level, normalized;
    _Atomic double kbps;
    Report report;
    void *status;
} Decoder;
static t_class *cls;
static long outputs(Decoder *x, long p) {
    return x->mode == SERIAL ? 1 : x->channels;
}
static long changed(Decoder *x, long p, long n) {
    if (x->mode != FIELDS || p < 0 || p >= F_PORTS)
        return 0;
    int old = x->channels;
    x->bus.width[p] = n;
    x->channels = band_channels(&x->bus);
    return x->channels != old;
}
static void play_fields(Decoder *x, const BandFrame *f) {
    Engine *e = x->e;
    if (e->preview) {
        int kb = sb_bitrate(LAYER, f->fmt.hz, f->fmt.channels, atomic_load(&x->kbps), 1000);
        worker_fields(x->worker, f, f->fmt.hz, kb, 1, e->epoch, 1);
    } else
        playback_frame(&e->playback, f);
}
static void render(Decoder *x, const BandFrame *f, int synthesize_idle) {
    Engine *e = x->e;
    int div = atomic_load(&x->divisor), dst;
    if (x->mode == BYTES) {
        int ceiling = downsample_hz(x->host, div);
        dst = f->fmt.hz;
        if (div > 1 && dst > ceiling)
            for (int i = 0; i < 6; i++) {
                int rate = codec_rates[i];
                if (f->fmt.hz % rate == 0) {
                    dst = rate;
                    if (dst <= ceiling)
                        break;
                }
            }
    } else
        dst = convert_target(f->fmt, div);
    double req = atomic_load(&x->kbps);
    int preview = x->mode == FIELDS && req > 0;
    if (e->src != f->fmt.hz || e->dst != dst || e->src_ch != f->fmt.channels ||
        e->preview != preview) {
        e->epoch++;
        e->src = f->fmt.hz;
        e->dst = dst;
        e->src_ch = f->fmt.channels;
        e->preview = preview;
        convert_start(&e->converter, f->fmt, dst);
        playback_reset(&e->playback);
    } else if (e->ended && !synthesize_idle) {
        convert_start(&e->converter, f->fmt, dst);
    }
    e->ended = 0;
    e->wait = (int)ceil(x->host * sb_rows(LAYER) * 32 / f->fmt.hz);
    e->grace = x->mode == BYTES ? e->wait : 0;
    e->packet_timeout = 2 * e->wait;
    int kb = preview            ? sb_bitrate(LAYER, dst, f->fmt.channels, req, 1000)
             : x->mode == BYTES ? f->fmt.kbps
                                : 0;
    report_set(&x->report, dst, x->mode == BYTES ? e->header_channels : f->fmt.channels, kb);
    if (dst == f->fmt.hz)
        play_fields(x, f);
    else if (convert_frame(&e->converter, f, &e->converted))
        play_fields(x, &e->converted);
}
static void bytes(Decoder *x, double v, double **in, int i) {
    if (!isfinite(v) || v < 0 || v > 255)
        return;
    Engine *e = x->e;
    e->partial_wait = e->packet_timeout ? e->packet_timeout : SB_PACKET;
    e->stream[e->used++] = (uint8_t)(v + .5);
    if (e->used < 4)
        return;
    if (!e->need) {
        Format f;
        e->need = sb_header(e->stream, &f);
        if (e->need <= 4 || e->need > SB_PACKET || f.layer != LAYER) {
            memmove(e->stream, e->stream + 1, 3);
            e->used = 3;
            e->need = 0;
            return;
        }
    }
    if (e->used != e->need)
        return;
    int ok = sb_unpack(e->stream, e->need, &e->packet);
    e->used = e->need = 0;
    if (!ok)
        return;
    BandFrame *f = &e->packet.fields;
    Capture *cap = &e->capture;
    e->header_channels = f->fmt.channels;
    if (f->fmt.channels == 1 && x->bus.width[1] > 32) {
        f->fmt.channels = 2;
        f->fmt.mode = 0;
        memcpy(f->allocation[1], f->allocation[0], sizeof f->allocation[0]);
        for (int g = 0; g < 3; g++)
            memcpy(f->scale[g][1], f->scale[g][0], sizeof f->scale[g][0]);
        for (int j = 0; j < sb_rows(LAYER); j++)
            memcpy(f->q[j][1], f->q[j][0], sizeof f->q[j][0]);
    }
    uint32_t m[2];
    for (int c = 0; c < 2; c++)
        m[c] = gate_mask(&x->bus, in, 1, c, i);
    int whole = atomic_load(&x->freeze);
    if (cap->have && same_layout(f->fmt, cap->saved.fmt)) {
        for (int c = 0; c < f->fmt.channels; c++)
            for (int b = 0; b < 32; b++)
                if (whole || (m[c] & (UINT32_C(1) << b)))
                    hold_band(f, &cap->saved, c, b, 0, sb_rows(LAYER),
                              whole || atomic_load(&x->level));
    }
    cap->saved = *f;
    cap->have = 1;
    render(x, f, 0);
}
static void idle(Decoder *x, double **in, int i) {
    Engine *e = x->e;
    Capture *cap = &e->capture;
    if (!cap->have)
        return;
    if (e->wait > 0) {
        e->wait--;
        return;
    }
    int all = atomic_load(&x->freeze), port = x->mode == BYTES ? 1 : F_GATE;
    uint32_t masks[2];
    for (int c = 0; c < 2; c++)
        masks[c] = gate_mask(&x->bus, in, port, c, i);
    if (all || masks[0] || masks[1]) {
        e->idle = cap->saved;
        if (!all)
            for (int c = 0; c < e->idle.fmt.channels; c++)
                for (int b = 0; b < 32; b++)
                    if (!(masks[c] & (UINT32_C(1) << b)))
                        e->idle.allocation[c][b] = 0;
        render(x, &e->idle, 1);
    } else if (!e->ended) {
        if (e->grace > 0) {
            e->grace--;
            return;
        }
        while (convert_finish(&e->converter, &e->converted))
            play_fields(x, &e->converted);
        e->ended = 1;
    }
}
static void perform(Decoder *x, t_object *chain, double **in, long nin, double **out, long nout,
                    long n, long fl, void *u) {
    Engine *e = x->e;
#if LAYER == 1
    if (x->mode == SERIAL) {
        if (nout != 3) {
            zeros(out, nout, n);
            return;
        }
        for (int i = 0; i < n; i++) {
            int col = e->serial.column;
            out[1][i] = col;
            out[2][i] = !col && !e->serial.slot;
            out[0][i] =
                layer1_tick(&e->serial, in[0][i], x->bus.connected[1] ? in[1][i] : -1,
                            x->bus.connected[2] ? in[2][i] : 7, x->bus.connected[3] ? in[3][i] : 18,
                            x->raw, x->bus.connected[0]);
        }
        return;
    }
#endif
    if (!x->valid || nout != x->channels) {
        zeros(out, nout, n);
        return;
    }
    int factor = atomic_load(&x->divisor), hz = downsample_hz(x->host, factor);
    if (hz != e->timing.clock.hz) {
        timing_setup(&e->timing, x->host, factor);
        if (x->mode == FIELDS && !x->bus.connected[F_SYNC]) {
            e->capture.have = 0;
            e->capture.next = -1;
        }
    }
    if (x->worker) {
        unsigned gen;
        while (worker_pop(x->worker, &e->packet, &gen))
            if (gen == e->epoch && e->preview)
                playback_frame(&e->playback, &e->packet.fields);
    }
    for (int i = 0; i < n; i++) {
        if (x->mode == BYTES) {
            if (x->bus.connected[0])
                for (int c = 0; c < x->bus.width[0]; c++)
                    bytes(x, in[c][i], in, i);
            /* A bitrate increase can delay the end of the next byte burst. A
               frame still arriving must not also trigger a repeated hold frame. */
            if (e->partial_wait > 0)
                e->partial_wait--;
            else if (e->used)
                e->used = e->need = 0;
            if (e->wait > 0)
                e->wait--;
            else if (!e->used)
                idle(x, in, i);
        } else {
            int mark = wire_mark(&x->bus, in, i, &e->timing);
            if (mark == 1) {
                Format f = read_format(&x->bus, in, i, hz, x->channels, downsample_hz(x->host, 1));
                uint32_t masks[2];
                for (int c = 0; c < 2; c++)
                    masks[c] = gate_mask(&x->bus, in, F_GATE, c, i);
                capture_begin(&e->capture, f, atomic_load(&x->freeze), atomic_load(&x->level),
                              masks);
            }
            if (mark > 0 &&
                capture_row(&e->capture, &x->bus, in, mark - 1, i, atomic_load(&x->normalized)))
                render(x, &e->capture.frame, 0);
            if (mark < 0) {
                e->capture.next = -1;
                idle(x, in, i);
            } else if (e->wait > 0)
                e->wait--;
        }
        float sample[2];
        playback_sample(&e->playback, sample);
        for (int c = 0; c < x->channels; c++)
            out[c][i] = sample[c];
    }
}
static void dsp(Decoder *x, t_object *chain, short *count, double host, long vec, long flags) {
    Engine *e = x->e;
    x->host = host;
    e->epoch++;
    e->preview = 0;
    int ports = x->mode == SERIAL ? 4 : x->mode == BYTES ? 2 : F_PORTS;
    bus_setup(&x->bus, x, chain, count, ports);
    e->used = e->need = e->src = e->ended = e->wait = 0;
    e->capture.have = 0;
    e->capture.next = -1;
    x->channels = x->mode == BYTES ? 2 : x->mode == SERIAL ? 1 : band_channels(&x->bus);
#if LAYER == 1
    if (x->mode == SERIAL)
        layer1_reset(&e->serial);
#endif
    if (x->mode != SERIAL) {
        playback_reset(&e->playback);
        x->valid = pcm_setup(&e->playback.bank, host, 0);
        timing_setup(&e->timing, host, atomic_load(&x->divisor));
    }
    object_method(chain, gensym("dsp_add64"), x, perform, 0, NULL);
}
static void freeze(Decoder *x, long v) {
    atomic_store(&x->freeze, v != 0);
}
static void normalized(Decoder *x, long v) {
    atomic_store(&x->normalized, v != 0);
}
static void holdmode(Decoder *x, t_symbol *s) {
    if (s == gensym("coeff") || s == gensym("level"))
        atomic_store(&x->level, s == gensym("level"));
    else
        object_error((t_object *)x, "holdmode expects coeff or level");
}
static void down(Decoder *x, t_symbol *s, long ac, t_atom *av) {
    if (x->mode != SERIAL)
        set_divisor((t_object *)x, &x->divisor, ac, av);
    else
        object_post((t_object *)x, "Use mc or bytes mode for a divided codec clock.");
}
static void bitrate(Decoder *x, double v) {
    if (x->mode == FIELDS)
        set_kbps((t_object *)x, &x->kbps, v);
    else
        object_post((t_object *)x, "Set the bitrate on the upstream encoder or packer.");
}
static void getrate(Decoder *x) {
    if (x->status)
        report_rate(x->status, x->host, &x->report);
}
static void getbitrate(Decoder *x) {
    if (x->status)
        report_bitrate(x->status, &x->report);
}
static void assist(Decoder *x, void *b, long io, long p, char *s) {
    const char *in[] = {"Serial data", "Band address", "Allocation", "Scale"};
    const char *out[] = {"Mono audio", "Scan column", "Frame sync"};
    const char *text =
        x->mode == SERIAL     ? (io == ASSIST_INLET ? in[p] : out[p])
        : io == ASSIST_OUTLET ? (p ? "Rate and bitrate status" : "Reconstructed MC audio")
        : x->mode == BYTES    ? (p ? "MC band freeze gates" : "Packed bytes in lane order, -1 idle")
                              : field_name(p);
    snprintf(s, 512, "%s", text);
}
static void destroy(Decoder *x) {
    dsp_free(&x->obj);
    worker_close(x->worker);
    if (x->e)
        pcm_free(&x->e->playback.bank);
    free(x->e);
}
static void *create(t_symbol *s, long ac, t_atom *av) {
    int div = downsample_args(&ac, av);
    if (div < 0 || ac > 1 || (ac && atom_gettype(av) != A_SYM))
        return NULL;
    t_symbol *name = ac ? atom_getsym(av) : NULL;
    int mode = name == gensym("bytes")                          ? BYTES
               : name == gensym("mc")                           ? FIELDS
               : LAYER == 1 && (!name || name == gensym("raw")) ? SERIAL
               : !name                                          ? FIELDS
                                                                : -1;
    if (mode < 0) {
        error(MP "decode~ expects mc or bytes");
        return NULL;
    }
    Decoder *x = (Decoder *)object_alloc(cls);
    if (!x)
        return NULL;
    x->mode = mode;
    x->raw = name == gensym("raw");
    x->divisor = div;
    x->channels = mode == BYTES ? 2 : 1;
    dsp_setup(&x->obj, mode == SERIAL ? 4 : mode == BYTES ? 2 : F_PORTS);
    x->obj.z_misc |= Z_NO_INPLACE;
    if (mode != SERIAL)
        x->obj.z_misc |= Z_MC_INLETS;
    x->e = calloc(1, sizeof(*x->e));
    if (mode == FIELDS)
        x->worker = worker_new(NULL);
    if (!x->e || (mode == FIELDS && !x->worker)) {
        object_free(x);
        return NULL;
    }
    if (mode == SERIAL) {
        for (int j = 0; j < 3; j++)
            outlet_new((t_object *)x, "signal");
    } else {
        x->status = outlet_new((t_object *)x, NULL);
        outlet_new((t_object *)x, "multichannelsignal");
    }
    return x;
}
void C74_EXPORT ext_main(void *m) {
    sb_tables();
    filter_init();
    if (!convert_tables())
        return;
#if LAYER == 1
    layer1_init_tables();
#endif
    cls = class_new(MP "decode~", (method)create, (method)destroy, sizeof(Decoder), 0, A_GIMME, 0);
    class_addmethod(cls, (method)dsp, "dsp64", A_CANT, 0);
    class_addmethod(cls, (method)outputs, "multichanneloutputs", A_CANT, 0);
    class_addmethod(cls, (method)changed, "inputchanged", A_CANT, 0);
    class_addmethod(cls, (method)assist, "assist", A_CANT, 0);
    class_addmethod(cls, (method)freeze, "freeze", A_LONG, 0);
    class_addmethod(cls, (method)holdmode, "holdmode", A_SYM, 0);
    class_addmethod(cls, (method)normalized, "normalized", A_LONG, 0);
    class_addmethod(cls, (method)down, "downsample", A_GIMME, 0);
    class_addmethod(cls, (method)bitrate, "bitrate", A_FLOAT, 0);
    class_addmethod(cls, (method)bitrate, "kbps", A_FLOAT, 0);
    class_addmethod(cls, (method)getrate, "getrate", 0);
    class_addmethod(cls, (method)getbitrate, "getbitrate", 0);
    class_dspinit(cls);
    class_register(CLASS_BOX, cls);
}
