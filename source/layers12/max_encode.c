#include "max_common.h"
#include "pcm.h"
#include "worker.h"
typedef struct {
    PcmBank bank;
    Analysis analysis[2];
    BandAudio audio;
    float samples[2][32];
    Packet visible;
    View view;
    int hz, pos, show, frame_kbps;
    unsigned epoch;
    unsigned char bytes[2 * SB_PACKET];
    unsigned byte_read, byte_write;
} Engine;
typedef struct {
    t_pxobject obj;
    Engine *e;
    Worker *worker;
    int channels, valid;
    double host;
    _Atomic int divisor;
    _Atomic double kbps;
    Report report;
    void *status;
} Encoder;
static t_class *cls;
static long outputs(Encoder *x, long p) {
    return p == 5 ? 2 : width_for_fields(x->channels, p);
}
static long changed(Encoder *x, long p, long n) {
    int ch = n > 1 ? 2 : 1, old = x->channels;
    x->channels = ch;
    return ch != old;
}
static void reset(Encoder *x, int hz) {
    Engine *e = x->e;
    e->epoch++;
    e->hz = hz;
    e->pos = e->show = 0;
    memset(e->analysis, 0, sizeof(e->analysis));
    pcm_reset(&e->bank);
}
static void perform(Encoder *x, t_object *chain, double **in, long nin, double **out, long nout,
                    long n, long flags, void *u) {
    Engine *e = x->e;
    int ch = x->channels, sync = 96 * ch, bytes = sync + 1 + FORMAT_LANES;
    if (!x->valid || nout != bytes + 2 || nin < ch) {
        zeros(out, nout, n);
        return;
    }
    int hz = downsample_hz(x->host, atomic_load(&x->divisor));
    if (hz != e->hz)
        reset(x, hz);
    PcmPath *path = pcm_select(&e->bank, hz);
    int rows = sb_rows(LAYER), from = 0;
    for (int i = 0; i < n; i++) {
        int marker = e->show ? 0 : -1;
        float value[2];
        for (int c = 0; c < 2; c++) {
            double v = in[c < ch ? c : 0][i];
            value[c] = isfinite(v) ? fmin(1., fmax(-1., v)) : 0;
        }
        pcm_input_push(path, value[0], value[1]);
        int ticks = codec_ticks(&path->rate.clock);
        for (int tick = 0; tick < ticks; tick++) {
            if (!e->pos) {
                view_fill(&e->view, out, ch, from, i, e->show);
                from = i;
                e->show = 0;
                while (1) {
                    unsigned gen;
                    if (!worker_pop(x->worker, &e->visible, &gen))
                        break;
                    if (gen == e->epoch) {
                        e->show = 1;
                        break;
                    }
                }
                /* Let an outstanding packet finish when the codec clock changes. */
                if (e->show) {
                    for (int k = 0; k < e->visible.length; k++)
                        e->bytes[e->byte_write++ % sizeof e->bytes] = e->visible.data[k];
                }
                e->frame_kbps =
                    sb_bitrate(LAYER, hz, ch, atomic_load(&x->kbps), floor(x->host * .016));
                report_set(&x->report, hz, ch, e->frame_kbps);
                marker = e->show ? 0 : -1;
            }
            int row = e->pos / 32, col = e->pos % 32;
            if (!col && e->show) {
                view_fill(&e->view, out, ch, from, i, e->show);
                from = i;
                latch(&e->view, &e->visible.fields, row, ch);
                marker = row + 1;
            }
            for (int c = 0; c < ch; c++)
                e->samples[c][col] = pcm_input_read(path, c, ticks - tick - 1, value[c]);
            if (col == 31)
                for (int c = 0; c < ch; c++)
                    filter_analyze(e->analysis + c, e->samples[c], e->audio.x[row][c]);
            if (++e->pos == rows * 32) {
                Format f = live_format(hz, ch);
                f.kbps = e->frame_kbps;
                f.original_hz = downsample_hz(x->host, 1);
                if (f.kbps)
                    worker_audio(x->worker, &e->audio, f, e->epoch);
                e->pos = 0;
            }
        }
        out[sync][i] = marker;
        for (int b = 0; b < 2; b++)
            out[bytes + b][i] =
                e->byte_read != e->byte_write ? e->bytes[e->byte_read++ % sizeof e->bytes] : -1;
    }
    view_fill(&e->view, out, ch, from, (int)n, e->show);
}
static void dsp(Encoder *x, t_object *chain, short *connected, double host, long vec, long flags) {
    x->channels = (long)object_method(chain, gensym("getnuminputchannels"), x, 0) > 1 ? 2 : 1;
    x->host = host;
    x->e->byte_read = x->e->byte_write = 0;
    x->valid = pcm_setup(&x->e->bank, host, 1);
    reset(x, downsample_hz(host, atomic_load(&x->divisor)));
    if (!x->valid)
        object_error((t_object *)x, "could not prepare rate conversion");
    object_method(chain, gensym("dsp_add64"), x, perform, 0, NULL);
}
static void down(Encoder *x, t_symbol *s, long ac, t_atom *av) {
    set_divisor((t_object *)x, &x->divisor, ac, av);
}
static void bitrate(Encoder *x, double v) {
    set_kbps((t_object *)x, &x->kbps, v);
}
static void getrate(Encoder *x) {
    report_rate(x->status, x->host, &x->report);
}
static void getbitrate(Encoder *x) {
    report_bitrate(x->status, &x->report);
}
static void stats(Encoder *x) {
    t_atom a;
    atom_setlong(&a, worker_drops(x->worker));
    outlet_anything(x->status, gensym("drops"), 1, &a);
}
static void assist(Encoder *x, void *b, long io, long p, char *s) {
    snprintf(s, 512, "%s",
             io == ASSIST_INLET ? "Mono or stereo MC audio"
             : p == 5           ? "Packed bytes (two MC lanes in order, -1 idle)"
                                : field_name(p));
}
static void destroy(Encoder *x) {
    dsp_free(&x->obj);
    worker_close(x->worker);
    if (x->e)
        pcm_free(&x->e->bank);
    free(x->e);
}
static void *create(t_symbol *s, long ac, t_atom *av) {
    int divisor = downsample_args(&ac, av);
    if (divisor < 0 || ac > 1 || (ac && (atom_gettype(av) != A_LONG || atom_getlong(av) < 0)))
        return NULL;
    Encoder *x = (Encoder *)object_alloc(cls);
    if (!x)
        return NULL;
    dsp_setup(&x->obj, 1);
    x->obj.z_misc |= Z_MC_INLETS | Z_NO_INPLACE;
    x->channels = 1;
    x->divisor = divisor;
    x->kbps = ac ? atom_getlong(av) : 0;
    x->e = calloc(1, sizeof(*x->e));
    x->worker = worker_new(NULL);
    if (!x->e || !x->worker) {
        object_free(x);
        return NULL;
    }
    x->status = outlet_new((t_object *)x, NULL);
    outlet_new((t_object *)x, "multichannelsignal");
    field_outlets((t_object *)x);
    return x;
}
void C74_EXPORT ext_main(void *m) {
    sb_tables();
    filter_init();
    cls = class_new(MP "encode~", (method)create, (method)destroy, sizeof(Encoder), 0, A_GIMME, 0);
    class_addmethod(cls, (method)dsp, "dsp64", A_CANT, 0);
    class_addmethod(cls, (method)outputs, "multichanneloutputs", A_CANT, 0);
    class_addmethod(cls, (method)changed, "inputchanged", A_CANT, 0);
    class_addmethod(cls, (method)down, "downsample", A_GIMME, 0);
    class_addmethod(cls, (method)bitrate, "bitrate", A_FLOAT, 0);
    class_addmethod(cls, (method)bitrate, "kbps", A_FLOAT, 0);
    class_addmethod(cls, (method)getrate, "getrate", 0);
    class_addmethod(cls, (method)getbitrate, "getbitrate", 0);
    class_addmethod(cls, (method)stats, "stats", 0);
    class_addmethod(cls, (method)assist, "assist", A_CANT, 0);
    class_dspinit(cls);
    class_register(CLASS_BOX, cls);
}
