#include "ext.h"
#include "ext_obex.h"
#include "ext_path.h"
#include "z_dsp.h"
#include "field_file.h"
#include "downsample.h"
#include "file_clock.h"
#include <stdatomic.h>
#include <sched.h>
#include <stdio.h>
#include <string.h>

enum { LANES = 64, GAIN = 2 * LANES, WINDOW = GAIN + 2, SYNC = WINDOW + 2 };
typedef struct {
    t_pxobject obj;
    _Atomic(FieldFile *) source;
    _Atomic unsigned callbacks;
    _Atomic int notice;
    int next_notice, primed, row, have_row, waiting, divisor;
    _Atomic int playing;
    unsigned long frames, stalls;
    double host, phase, values[SYNC];
    void *status, *events;
    t_symbol *path;
} External;
static t_class *cls;
static void file_error(External *x, const char *reason) {
    object_error((t_object *)x, "%s", reason);
    t_atom a;
    atom_setsym(&a, gensym(reason));
    outlet_anything(x->status, gensym("error"), 1, &a);
}
static FieldFile *detach(External *x) {
    FieldFile *f = atomic_exchange(&x->source, NULL);
    while (atomic_load(&x->callbacks))
        sched_yield();
    return f;
}
static void reset_transport(External *x) {
    x->playing = x->have_row = x->waiting = x->next_notice = 0;
    x->row = -1;
    x->frames = x->stalls = 0;
    x->phase = 0;
    atomic_store(&x->notice, 0);
    qelem_unset(x->events);
    memset(x->values, 0, sizeof(x->values));
}
static void info(External *x) {
    FieldFile *f = atomic_load(&x->source);
    if (!f)
        return;
    t_atom a[3];
    atom_setlong(a, field_file_rate(f));
    atom_setlong(a + 1, field_file_channels(f));
    atom_setlong(a + 2, field_file_source_rate(f));
    int count = field_file_rate(f) == field_file_source_rate(f) ? 2 : 3;
    outlet_anything(x->status, gensym("sourceformat"), count, a);
}
static void events(External *x) {
    int notice = atomic_exchange(&x->notice, 0);
    FieldFile *f = atomic_load(&x->source);
    if (!f || !notice)
        return;
    if (notice < 0) {
        file_error(x, notice == -2 ? "host rate too low to carry every spectral row"
                                   : field_file_error(f));
        return;
    }
    t_atom a[2];
    atom_setlong(a, (t_atom_long)x->frames);
    atom_setlong(a + 1, (t_atom_long)x->stalls);
    outlet_anything(x->status, gensym("done"), 2, a);
}
static void read_deferred(External *x, t_symbol *s, long ac, t_atom *av) {
    char name[MAX_FILENAME_CHARS] = "", path[MAX_PATH_CHARS], native[MAX_PATH_CHARS];
    short dir = 0;
    t_fourcc type = 0;
    if (ac) {
        const char *arg = atom_getsym(av)->s_name;
        if (strlen(arg) >= sizeof(path)) {
            file_error(x, "file path too long");
            return;
        }
        if (arg[0] == '/' || strchr(arg, ':'))
            strcpy(path, arg);
        else {
            if (strlen(arg) >= sizeof(name)) {
                file_error(x, "file name too long");
                return;
            }
            strcpy(name, arg);
            if (locatefile_extended(name, &dir, &type, NULL, 0) ||
                path_topathname(dir, name, path)) {
                file_error(x, "could not locate MP3");
                return;
            }
        }
    } else {
        if (open_dialog(name, &dir, &type, NULL, 0))
            return;
        if (path_topathname(dir, name, path)) {
            file_error(x, "cannot resolve selected file");
            return;
        }
    }
    if (path[0] == '/')
        strcpy(native, path);
    else if (path_nameconform(path, native, PATH_STYLE_NATIVE, PATH_TYPE_ABSOLUTE)) {
        file_error(x, "invalid file path");
        return;
    }
    const char *problem = NULL;
    FieldFile *new_file = field_file_open_at(native, x->divisor, &problem);
    if (!new_file) {
        file_error(x, problem);
        return;
    }
    FieldFile *old = detach(x);
    reset_transport(x);
    x->primed = 1;
    x->path = gensym(native);
    atomic_store(&x->source, new_file);
    field_file_close(old);
    info(x);
    t_atom a;
    atom_setsym(&a, x->path);
    outlet_anything(x->status, gensym("ready"), 1, &a);
}
static void read_file(External *x, t_symbol *s, long ac, t_atom *av) {
    if (ac > 1 || (ac && atom_gettype(av) != A_SYM)) {
        object_error((t_object *)x, "read takes one optional MP3 path");
        return;
    }
    defer_low(x, (method)read_deferred, s, ac, av);
}
/* Prepare the new file before detaching the current take. A bad or removed
   file leaves the previous transport usable. A playing reader adopts a new
   divisor on its next start so no row or fraction of a frame is duplicated. */
static int prepare_rate(External *x) {
    FieldFile *f = atomic_load(&x->source);
    if (!f || field_file_rate(f) == codec_divide(field_file_source_rate(f), x->divisor))
        return 1;
    const char *problem = NULL;
    FieldFile *next = field_file_open_at(x->path->s_name, x->divisor, &problem);
    if (!next) {
        file_error(x, problem);
        return 0;
    }
    FieldFile *previous = detach(x);
    reset_transport(x);
    x->primed = 1;
    atomic_store(&x->source, next);
    field_file_close(previous);
    info(x);
    return 1;
}
static void downsample_deferred(External *x, t_symbol *s, long ac, t_atom *av) {
    int factor = downsample_message((t_object *)x, ac, av);
    if (factor < 0)
        return;
    x->divisor = factor;
    if (!atomic_load(&x->playing))
        prepare_rate(x);
}
static void downsample(External *x, t_symbol *s, long ac, t_atom *av) {
    defer(x, (method)downsample_deferred, s, ac, av);
}
static void getrate_deferred(External *x, t_symbol *s, long ac, t_atom *av) {
    FieldFile *f = atomic_load(&x->source);
    if (f)
        downsample_status(x->status, x->host, field_file_rate(f), field_file_channels(f));
}
static void getrate(External *x) {
    defer(x, (method)getrate_deferred, NULL, 0, NULL);
}
static void start_deferred(External *x, t_symbol *s, long ac, t_atom *av) {
    if (!prepare_rate(x))
        return;
    FieldFile *f = detach(x);
    if (!f) {
        file_error(x, "read an MP3 first");
        return;
    }
    reset_transport(x);
    const char *problem = NULL;
    if (x->primed || field_file_rewind(f, &problem))
        x->playing = 1;
    x->primed = 0;
    atomic_store(&x->source, f);
    if (problem)
        file_error(x, problem);
}
static void stop_deferred(External *x, t_symbol *s, long ac, t_atom *av) {
    FieldFile *f = detach(x);
    reset_transport(x);
    atomic_store(&x->source, f);
}
static void start(External *x) { defer_low(x, (method)start_deferred, NULL, 0, NULL); }
static void stop(External *x) { defer_low(x, (method)stop_deferred, NULL, 0, NULL); }
static void report_info(External *x, t_symbol *s, long ac, t_atom *av) { info(x); }
static void request_info(External *x) { defer_low(x, (method)report_info, NULL, 0, NULL); }
static void fill(External *x, double **out, long from, long to, int live) {
    for (int j = 0; j < SYNC; j++) {
        double v = live ? x->values[j] : 0;
        for (long i = from; i < to; i++)
            out[j][i] = v;
    }
}
static void latch(External *x, const EncodedFrame *f) {
    int gr = x->row / 18, row = x->row % 18;
    for (int c = 0; c < 2; c++) {
        const Fields *fields = &f->fields[gr][c < f->channels ? c : 0];
        for (int b = 0; b < 32; b++) {
            int lane = c * 32 + b, line = 18 * b + row;
            x->values[lane] = fields->q[line];
            x->values[LANES + lane] = fields->attenuation[line];
        }
        x->values[GAIN + c] = fields->gain;
        x->values[WINDOW + c] = fields->window;
    }
}
static void perform(External *x, t_object *chain, double **in, long nin, double **out, long nout,
                    long count, long flags, void *user) {
    atomic_fetch_add(&x->callbacks, 1);
    FieldFile *f = atomic_load(&x->source);
    if (nout != SYNC + 1 || !f) {
        for (long j = 0; j < nout; j++) {
            if (j == SYNC)
                for (long i = 0; i < count; i++)
                    out[j][i] = -1;
            else
                memset(out[j], 0, count * sizeof(double));
        }
        goto leave;
    }
    if (x->next_notice) {
        atomic_store(&x->notice, x->next_notice);
        x->next_notice = 0;
        qelem_set(x->events);
    }
    int hz = field_file_rate(f), rows = hz >= 32000 ? 36 : 18;
    if (x->playing && x->host * 32 < hz) {
        x->playing = 0;
        x->have_row = 0;
        x->next_notice = -2;
    }
    long from = 0;
    for (long i = 0; i < count; i++) {
        if (!x->playing) {
            for (long j = i; j < count; j++)
                out[SYNC][j] = -1;
            break;
        }
        if (x->have_row) {
            x->phase += hz;
            if (x->phase < 32 * x->host) {
                out[SYNC][i] = 0;
                continue;
            }
            x->phase -= 32 * x->host;
        }
        fill(x, out, from, i, x->have_row);
        from = i;
        if (x->have_row && x->row + 1 == rows) {
            field_file_advance(f);
            ++x->frames;
            x->row = -1;
        }
        int status;
        const EncodedFrame *frame = field_file_peek(f, &status);
        if (!frame) {
            if (!status && !x->waiting) {
                ++x->stalls;
                x->waiting = 1;
            }
            x->have_row = 0;
            x->phase = 0;
            out[SYNC][i] = -1;
            if (status) {
                x->playing = 0;
                x->next_notice = status;
            }
            continue;
        }
        x->waiting = 0;
        x->have_row = 1;
        ++x->row;
        latch(x, frame);
        out[SYNC][i] = file_row_marker(x->row);
    }
    fill(x, out, from, count, x->have_row);
leave:
    atomic_fetch_sub(&x->callbacks, 1);
}
static void dsp(External *x, t_object *chain, short *count, double rate, long vec, long flags) {
    if (x->host > 0)
        x->phase *= rate / x->host;
    x->host = rate;
    object_method(chain, gensym("dsp_add64"), x, perform, 0, NULL);
}
static long outputs(External *x, long port) { return port < 2 ? 64 : port < 4 ? 2 : 1; }
static void assist(External *x, void *b, long kind, long port, char *s) {
    const char *ports[] = {"MC spectral integers (32 left, 32 right)",
                           "MC attenuation (32 left, 32 right)",
                           "MC global gain",
                           "MC window type",
                           "File row and granule sync",
                           "sourceformat, ready, done, error"};
    snprintf(s, 512, "%s", kind == ASSIST_INLET ? "read [file], start, stop, info" : ports[port]);
}
static void destroy(External *x) {
    dsp_free(&x->obj);
    field_file_close(detach(x));
    if (x->events)
        qelem_free(x->events);
}
static void *create(t_symbol *s, long ac, t_atom *av) {
    int divisor = downsample_args(&ac, av);
    if (divisor < 0 || ac)
        return NULL;
    External *x = (External *)object_alloc(cls);
    if (!x)
        return NULL;
    dsp_setup(&x->obj, 0);
    x->obj.z_misc |= Z_NO_INPLACE;
    x->divisor = divisor;
    x->row = -1;
    x->status = outlet_new((t_object *)x, NULL);
    x->events = qelem_new(x, (method)events);
    if (!x->events) {
        object_free(x);
        return NULL;
    }
    for (int k = 4; k >= 0; k--)
        outlet_new((t_object *)x, k == 4 ? "signal" : "multichannelsignal");
    return x;
}
void C74_EXPORT ext_main(void *module) {
    layer3_tables();
    cls = class_new("mp3unpack~", (method)create, (method)destroy, sizeof(External), 0, A_GIMME, 0);
    class_addmethod(cls, (method)outputs, "multichanneloutputs", A_CANT, 0);
    class_addmethod(cls, (method)dsp, "dsp64", A_CANT, 0);
    class_addmethod(cls, (method)assist, "assist", A_CANT, 0);
    class_addmethod(cls, (method)read_file, "read", A_GIMME, 0);
    class_addmethod(cls, (method)start, "start", 0);
    class_addmethod(cls, (method)stop, "stop", 0);
    class_addmethod(cls, (method)downsample, "downsample", A_GIMME, 0);
    class_addmethod(cls, (method)getrate, "getrate", 0);
    class_addmethod(cls, (method)request_info, "info", 0);
    class_dspinit(cls);
    class_register(CLASS_BOX, cls);
}
