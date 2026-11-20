#include "ext.h"
#include "ext_obex.h"
#include "ext_path.h"
#include "z_dsp.h"
#include "rate.h"
#include "downsample.h"
#include "field_hold.h"
#include "file_clock.h"
#include "bitrate.h"
#include "pack_writer.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
typedef struct {
    t_pxobject obj;
    FieldHold *hold;
    CodecClock clock;
    FieldBus bus;
    int channels, prev_sync, captured_kbps, configured_channels;
    _Atomic double bitrate;
    _Atomic int freeze, level, changing_format;
    int source_hz, source_base, source_channels, source_active;
    _Atomic int divisor;
    _Atomic(PackWriter *) writer;
    _Atomic unsigned callbacks;
    unsigned long generation, armed;
    unsigned limited, held_limited;
    t_symbol *path;
    void *status, *stats_clock, *stats_queue;
    PackMonitor *monitor;
    Rate playback;
    Fractional converter;
    unsigned playback_epoch;
    int playback_tail;
    _Atomic unsigned long last_fit;
} External;
static t_class *cls;
static int file_channels(External *x) {
    return x->source_channels ? x->source_channels : x->channels;
}
static int file_target(External *x, int factor) {
    int used = x->source_base / x->source_hz;
    return codec_divide(x->source_hz, factor / (used > 0 ? used : 1));
}
static int output_hz(External *x) {
    return x->source_hz ? file_target(x, x->divisor) : x->clock.hz;
}
static void finish(External *x, int announce) {
    PackWriter *w = atomic_exchange(&x->writer, NULL);
    if (!w)
        return;
    /* A DSP vector that acquired the old pointer releases it before disposal. */
    while (atomic_load(&x->callbacks))
        sched_yield();
    clock_unset(x->stats_clock);
    qelem_unset(x->stats_queue);
    PackResult r = writer_close(w);
    if (!announce)
        return;
    if (r.error) {
        object_error((t_object *)x, "%s (%s)", writer_error(r.error), x->path->s_name);
        t_atom args[2];
        atom_setsym(args, x->path);
        atom_setsym(args + 1, gensym(writer_error(r.error)));
        outlet_anything(x->status, gensym("error"), 2, args);
        return;
    }
    t_atom a[8];
    atom_setsym(a, x->path);
    atom_setlong(a + 1, r.format.frames);
    atom_setlong(a + 2, r.format.fitted);
    atom_setlong(a + 3, r.limited);
    atom_setlong(a + 4, r.format.windows);
    atom_setlong(a + 5, r.format.rate);
    atom_setlong(a + 6, r.format.channels);
    atom_setlong(a + 7, r.format.bitrate);
    outlet_anything(x->status, gensym("done"), 8, a);
}
static void stats(External *x) {
    PackMonitor *m = x->monitor;
    t_atom a[4];
    atom_setlong(a, atomic_load(&m->fitted));
    atom_setlong(a + 1, atomic_load(&m->written));
    atom_setlong(a + 2, atomic_load(&m->windows));
    atom_setlong(a + 3, atomic_load(&m->drops));
    outlet_anything(x->status, gensym("fit"), 4, a);
}
static void stats_deferred(External *x, t_symbol *s, long ac, t_atom *av) { stats(x); }
static void request_stats(External *x) { defer_low(x, (method)stats_deferred, NULL, 0, NULL); }
static void queue_poll(External *x) { qelem_set(x->stats_queue); }
static void poll_fit(External *x) {
    if (!atomic_load(&x->writer))
        return;
    unsigned long fitted = atomic_load(&x->monitor->fitted);
    if (fitted != x->last_fit) {
        x->last_fit = fitted;
        stats(x);
    }
    clock_fdelay(x->stats_clock, 100.);
}
static void monitor(External *x, long flag) { monitor_enable(x->monitor, flag != 0); }
static void stop_deferred(External *x, t_symbol *s, long ac, t_atom *av) { finish(x, 1); }
static void stop(External *x) { defer_low(x, (method)stop_deferred, NULL, 0, NULL); }
static void open_deferred(External *x, t_symbol *s, long ac, t_atom *av) {
    char name[MAX_FILENAME_CHARS] = "Spectral take.mp3", path[MAX_PATH_CHARS],
         native[MAX_PATH_CHARS];
    if (ac) {
        const char *arg = atom_getsym(av)->s_name;
        if (strlen(arg) >= sizeof(path)) {
            object_error((t_object *)x, "file path too long");
            return;
        }
        if (arg[0] == '/' || strchr(arg, ':'))
            strcpy(path, arg);
        else if (path_topathname(path_getdefault(), arg, path)) {
            object_error((t_object *)x, "cannot resolve file path");
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
    else if (path_nameconform(path, native, PATH_STYLE_NATIVE, PATH_TYPE_ABSOLUTE)) {
        object_error((t_object *)x, "invalid file path");
        return;
    }
    int kbps = pack_bitrate(output_hz(x), atomic_load(&x->bitrate));
    if (output_hz(x)) {
        Pack probe;
        if (!pack_init(&probe, output_hz(x), file_channels(x), kbps)) {
            object_error((t_object *)x, "bitrate unavailable at %d Hz", output_hz(x));
            return;
        }
    }
    PackWriter *w = writer_open(native, kbps, ++x->generation);
    if (!w) {
        object_error((t_object *)x, "could not open %s", native);
        return;
    }
    finish(x, 1);
    writer_attach_monitor(w, x->monitor);
    x->last_fit = 0;
    x->path = gensym(native);
    atomic_store(&x->writer, w);
    clock_fdelay(x->stats_clock, 100.);
    t_atom a;
    atom_setsym(&a, x->path);
    outlet_anything(x->status, gensym("recording"), 1, &a);
}
static void write_file(External *x, t_symbol *s, long ac, t_atom *av) {
    if (ac > 1 || (ac && atom_gettype(av) != A_SYM)) {
        object_error((t_object *)x, "write takes an optional file path");
        return;
    }
    defer_low(x, (method)open_deferred, s, ac, av);
}
static void holdmode(External *x, t_symbol *mode) {
    if (mode == gensym("level") || mode == gensym("coeff"))
        atomic_store(&x->level, mode == gensym("level"));
    else
        object_error((t_object *)x, "holdmode expects coeff or level");
}
static void freeze(External *x, long flag) { atomic_store(&x->freeze, flag != 0); }
static long outputs(External *x, long port) {
    /* MSP indexes only signal outlets here. The status outlet sits between
       granule sync (signal 4) and the audition cable (signal 5). */
    return port < 2 ? 32 * x->channels : port == 4 ? 1 : x->channels;
}
static long changed(External *x, long port, long n) {
    if (port < 0 || port >= FIELD_INLETS)
        return 0;
    x->bus.width[port] = n > 0 ? n : 1;
    int ch = bus_channels(&x->bus), change = ch != x->channels;
    x->channels = ch;
    return change;
}
static void audition(External *x, unsigned epoch, double **out, long i) {
    Rate *r = &x->playback;
    float sample[2] = {0};
    int ticks = codec_ticks(&r->clock);
    PackMonitor *m = x->monitor;
    if (!x->playback_tail && atomic_load(&m->rd) == atomic_load(&m->wr)) {
        m->playing = m->wait = 0;
        for (int c = 0; c < x->channels; ++c)
            out[c][i] = 0;
        return;
    }
    for (int k = 0; k < ticks; ++k) {
        monitor_tick(m, epoch, r->hz, file_channels(x), sample);
        if (r->factor < 0) {
            fractional_push(&x->converter, sample[0], sample[1]);
            if (sample[0] || sample[1])
                x->playback_tail = x->converter.taps;
            else if (x->playback_tail)
                --x->playback_tail;
        }
    }
    if (r->factor > 1) {
        rate_push(r, sample[0], sample[1]);
        if (sample[0] || sample[1])
            x->playback_tail = r->taps;
        else if (x->playback_tail)
            --x->playback_tail;
    }
    for (int c = 0; c < x->channels; ++c) {
        if (r->factor > 1)
            out[c][i] = rate_interpolate(r, c);
        else
            out[c][i] = r->factor == 1 ? sample[c]
                        : r->factor < 0
                            ? fractional_read(&x->converter, c, -r->clock.phase / r->clock.host)
                            : 0;
    }
}
/* Capture a complete MC row only once. The last row already contains every
   field in the frame, so an asynchronous stop at EOF cannot drop its tail. */
static int file_capture(External *x, PackWriter *w, unsigned long id, int ng, int freeze,
                        double **in, long i) {
    FieldHold *h = x->hold;
    double marker = x->bus.connected[4] ? in[x->bus.offset[4]][i] : -1;
    if (marker == -1) {
        x->source_active = 0;
        h->pos = 0;
        return 0;
    }
    if (marker == 2) {
        x->source_active = 1;
        h->pos = 0;
        x->armed = id;
        x->captured_kbps = pack_bitrate(output_hz(x), atomic_load(&x->bitrate));
        x->limited = 0;
    }
    if (!x->source_active || marker != file_row_marker(h->pos / 32))
        return 0;
    int pulse = h->pos % 576 == 0;
    hold_capture(h, x->channels, freeze, &x->bus, in, i, &x->limited);
    h->pos += 31;
    if (h->pos + 1 == ng * 576) {
        if (!h->frozen)
            x->held_limited = x->limited;
        if (w && x->armed == id)
            writer_offer_source(w, x->clock.hz, output_hz(x), file_channels(x), hold_output(h),
                                x->held_limited, x->captured_kbps, 1);
        x->source_active = 0;
    }
    hold_advance(h, ng, x->channels);
    return pulse;
}
static void perform(External *x, t_object *chain, double **in, long nin, double **out, long nout,
                    long count, long flags, void *user) {
    atomic_fetch_add(&x->callbacks, 1);
    if (atomic_load(&x->changing_format)) {
        for (long j = 0; j < nout; j++)
            memset(out[j], 0, count * sizeof(double));
        atomic_fetch_sub(&x->callbacks, 1);
        return;
    }
    int ch = x->channels, bands = 32 * ch, ng = x->clock.hz >= 32000 ? 2 : 1,
        freeze = atomic_load(&x->freeze);
    int sync_lane = 2 * bands + 2 * ch;
    if (!x->clock.hz || nout != sync_lane + 1 + ch) {
        for (long j = 0; j < nout; j++)
            memset(out[j], 0, count * sizeof(double));
        atomic_fetch_sub(&x->callbacks, 1);
        return;
    }
    PackWriter *w = atomic_load(&x->writer);
    unsigned long id = writer_id(w);
    FieldHold *h = x->hold;
    h->request_level = atomic_load(&x->level);
    unsigned epoch = atomic_load(&x->monitor->epoch);
    if (epoch != x->playback_epoch) {
        x->playback_epoch = epoch;
        x->playback_tail = 0;
        memset(x->playback.delay, 0, sizeof(x->playback.delay));
        if (x->converter.history)
            memset(x->converter.history, 0, 2 * x->converter.taps * sizeof(float));
    }
    if (!(epoch & 1u)) {
        atomic_store(&x->monitor->rd, atomic_load(&x->monitor->wr));
        for (int c = 0; c < ch; ++c)
            memset(out[sync_lane + 1 + c], 0, count * sizeof(double));
    }
    for (long i = 0; i < count; i++) {
        int ticks = 0, pulse = 0;
        if (x->source_hz)
            pulse = file_capture(x, w, id, ng, freeze, in, i);
        else
            ticks = codec_ticks(&x->clock);
        if (!x->source_hz && x->bus.connected[4]) {
            double level = in[x->bus.offset[4]][i];
            int sync = isfinite(level) && level > 0;
            ticks = codec_sync(&x->clock, ticks, &h->pos, 576, sync && !x->prev_sync);
            x->prev_sync = sync;
        }
        for (int k = 0; k < ticks; k++) {
            if (!h->pos) {
                x->armed = id;
                x->captured_kbps = pack_bitrate(output_hz(x), atomic_load(&x->bitrate));
                x->limited = 0;
            }
            pulse |= h->pos % 576 == 0;
            hold_capture(h, ch, freeze, &x->bus, in, i, &x->limited);
            if (h->pos + 1 == ng * 576) {
                if (!h->frozen)
                    x->held_limited = x->limited;
                if (w && x->armed == id)
                    writer_offer_at(w, x->clock.hz, ch, hold_output(h), x->held_limited,
                                    x->captured_kbps, 0);
            }
            hold_advance(h, ng, ch);
        }
        int gr = h->visible / 576, row = h->visible % 576 / 32;
        for (int c = 0; c < ch; c++) {
            Fields *f = &hold_output(h)[gr][c];
            for (int b = 0; b < 32; b++) {
                int lane = c * 32 + b, j = 18 * b + row;
                out[lane][i] = f->q[j];
                out[bands + lane][i] = f->attenuation[j];
            }
            out[2 * bands + c][i] = f->gain;
            out[2 * bands + ch + c][i] = f->window;
        }
        out[sync_lane][i] = pulse;
        if (epoch & 1u)
            audition(x, epoch, out + sync_lane + 1, i);
    }
    atomic_fetch_sub(&x->callbacks, 1);
}
static void setup_clocks(External *x, double rate) {
    CodecClock clock;
    codec_clock_setup_divided(&clock, rate, x->divisor);
    if (x->source_hz)
        clock.hz = x->source_hz;
    x->clock = clock;
    fractional_free(&x->converter);
    rate_setup_native(&x->playback, rate, output_hz(x));
    if (x->playback.factor < 0 && !fractional_setup(&x->converter, x->playback.hz, rate)) {
        x->playback.factor = 0;
        object_error((t_object *)x, "cannot allocate audition rate converter");
    }
    atomic_fetch_add(&x->monitor->epoch, 2);
    x->source_active = x->prev_sync = 0;
    x->armed = 0;
    memset(x->hold, 0, sizeof(*x->hold));
}
static void format_deferred(External *x, t_symbol *s, long ac, t_atom *av) {
    int hz = 0, ch = 0, base = 0;
    if ((ac == 2 || ac == 3) && atom_gettype(av) == A_LONG && atom_gettype(av + 1) == A_LONG) {
        long h = atom_getlong(av), c = atom_getlong(av + 1);
        Pack probe;
        if (h < 8000 || h > 48000 || (c != 1 && c != 2) || !pack_init(&probe, (int)h, (int)c, 0))
            goto invalid;
        long original = ac == 2 ? h : atom_getlong(av + 2);
        if ((ac == 3 && atom_gettype(av + 2) != A_LONG) || original < h || original > 48000 ||
            original % h || !pack_init(&probe, (int)original, (int)c, 0))
            goto invalid;
        base = (int)original;
        hz = (int)h;
        ch = (int)c;
    } else if (ac != 1 || atom_gettype(av) != A_SYM || atom_getsym(av) != gensym("auto")) {
    invalid:
        object_error((t_object *)x, "sourceformat expects native Hz and 1 or 2 channels, or auto");
        return;
    }
    if (hz == x->source_hz && ch == x->source_channels && base == x->source_base)
        return;
    atomic_store(&x->changing_format, 1);
    while (atomic_load(&x->callbacks))
        sched_yield();
    if (writer_has_input(atomic_load(&x->writer)))
        finish(x, 1);
    x->source_hz = hz;
    x->source_base = base;
    x->source_channels = ch;
    if (x->clock.host > 0)
        setup_clocks(x, x->clock.host);
    atomic_store(&x->changing_format, 0);
}
static void sourceformat(External *x, t_symbol *s, long ac, t_atom *av) {
    defer(x, (method)format_deferred, s, ac, av);
}
static void dsp(External *x, t_object *chain, short *count, double rate, long vec, long flags) {
    long offset = 0;
    for (int i = 0; i < FIELD_INLETS; i++) {
        long n = (long)object_method(chain, gensym("getnuminputchannels"), x, (long)i);
        x->bus.width[i] = n > 0 ? n : 1;
        x->bus.offset[i] = offset;
        x->bus.connected[i] = count[i];
        offset += x->bus.width[i];
    }
    int ch = bus_channels(&x->bus);
    CodecClock next;
    codec_clock_setup_divided(&next, rate, x->divisor);
    if (x->source_hz)
        next.hz = x->source_hz;
    if (x->clock.hz &&
        (next.hz != x->clock.hz || (!x->source_channels && ch != x->configured_channels)))
        finish(x, 1);
    x->configured_channels = x->channels = ch;
    setup_clocks(x, rate);
    object_method(chain, gensym("dsp_add64"), x, perform, 0, NULL);
}
static void getbitrate_deferred(External *x, t_symbol *s, long ac, t_atom *av) {
    if (!x->clock.hz && !x->source_hz)
        return;
    t_atom a;
    atom_setlong(&a, pack_bitrate(output_hz(x), atomic_load(&x->bitrate)));
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
static void info_deferred(External *x, t_symbol *s, long ac, t_atom *av) {
    downsample_status(x->status, x->clock.host, output_hz(x), file_channels(x));
}
static void info(External *x) { defer(x, (method)info_deferred, NULL, 0, NULL); }
static void downsample_deferred(External *x, t_symbol *s, long ac, t_atom *av) {
    int factor = downsample_message((t_object *)x, ac, av);
    if (factor < 0)
        return;
    double host = x->clock.host;
    int target = x->source_hz ? file_target(x, factor) : downsample_hz(host, factor);
    if (target != output_hz(x)) {
        downsample_quiesce(&x->changing_format, &x->callbacks);
        finish(x, 1);
        x->divisor = factor;
        if (host)
            setup_clocks(x, host);
        atomic_store(&x->changing_format, 0);
    } else
        x->divisor = factor;
    if (host)
        info(x);
}
static void downsample(External *x, t_symbol *s, long ac, t_atom *av) {
    defer(x, (method)downsample_deferred, s, ac, av);
}
static void assist(External *x, void *b, long kind, long port, char *s) {
    const char *fields[] = {"Spectral integers, 32 lanes per channel",
                            "Effective attenuation, 32 lanes per channel", "Global gain",
                            "Window type", "Granule sync"};
    snprintf(s, 512, "%s",
             port == 6 ? "Audition of saved MP3 frames (MC audio)"
             : port == BAND_HOLD_INLET
                 ? (kind == ASSIST_INLET ? "MC band hold (1 holds, 0 follows)" : "Recording status")
                 : fields[port]);
}
static void destroy(External *x) {
    dsp_free(&x->obj);
    finish(x, 0);
    if (x->stats_clock)
        object_free(x->stats_clock);
    if (x->stats_queue)
        qelem_free(x->stats_queue);
    fractional_free(&x->converter);
    free(x->monitor);
    free(x->hold);
}
static void *create(t_symbol *s, long ac, t_atom *av) {
    int divisor = downsample_args(&ac, av);
    if (divisor < 0)
        return NULL;
    long kb = ac ? atom_getlong(av) : 0;
    if (ac > 1 || (ac && (atom_gettype(av) != A_LONG || kb < 0 || kb > 320))) {
        error("mp3pack~: optional integer bitrate 0..320");
        return NULL;
    }
    External *x = (External *)object_alloc(cls);
    if (!x)
        return NULL;
    x->divisor = divisor;
    x->bitrate = (int)kb;
    x->channels = 1;
    dsp_setup(&x->obj, FIELD_INLETS);
    x->obj.z_misc |= Z_MC_INLETS | Z_NO_INPLACE;
    x->hold = calloc(1, sizeof(*x->hold));
    x->monitor = calloc(1, sizeof(*x->monitor));
    x->stats_queue = qelem_new(x, (method)poll_fit);
    x->stats_clock = clock_new(x, (method)queue_poll);
    if (!x->hold || !x->monitor || !x->stats_clock || !x->stats_queue) {
        object_free(x);
        return NULL;
    }
    outlet_new((t_object *)x, "multichannelsignal");
    x->status = outlet_new((t_object *)x, NULL);
    for (int k = 4; k >= 0; k--)
        outlet_new((t_object *)x, k == 4 ? "signal" : "multichannelsignal");
    return x;
}
void C74_EXPORT ext_main(void *module) {
    layer3_tables();
    cls = class_new("mp3pack~", (method)create, (method)destroy, sizeof(External), 0, A_GIMME, 0);
    class_addmethod(cls, (method)outputs, "multichanneloutputs", A_CANT, 0);
    class_addmethod(cls, (method)changed, "inputchanged", A_CANT, 0);
    class_addmethod(cls, (method)dsp, "dsp64", A_CANT, 0);
    class_addmethod(cls, (method)assist, "assist", A_CANT, 0);
    class_addmethod(cls, (method)sourceformat, "sourceformat", A_GIMME, 0);
    class_addmethod(cls, (method)write_file, "write", A_GIMME, 0);
    class_addmethod(cls, (method)stop, "stop", 0);
    class_addmethod(cls, (method)bitrate, "bitrate", A_FLOAT, 0);
    class_addmethod(cls, (method)bitrate, "kbps", A_FLOAT, 0);
    class_addmethod(cls, (method)getbitrate, "getbitrate", 0);
    class_addmethod(cls, (method)monitor, "monitor", A_LONG, 0);
    class_addmethod(cls, (method)request_stats, "stats", 0);
    class_addmethod(cls, (method)downsample, "downsample", A_GIMME, 0);
    class_addmethod(cls, (method)info, "getrate", 0);
    class_addmethod(cls, (method)info, "info", 0);
    class_addmethod(cls, (method)holdmode, "holdmode", A_SYM, 0);
    class_addmethod(cls, (method)freeze, "freeze", A_LONG, 0);
    class_dspinit(cls);
    class_register(CLASS_BOX, cls);
}
