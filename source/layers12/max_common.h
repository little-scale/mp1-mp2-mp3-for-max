#ifndef MPEG_SUBBAND_MAX_COMMON_H
#define MPEG_SUBBAND_MAX_COMMON_H
#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"
#include "../downsample.h"
#include "subband.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef LAYER
#error Define LAYER as 1 or 2
#endif
#if LAYER == 1
#define MP "mp1"
#else
#define MP "mp2"
#endif
enum { F_DATA, F_ALLOC, F_SCALE, F_SYNC, F_FORMAT, F_GATE, F_PORTS, FORMAT_LANES = 7 };
typedef struct {
    int width[F_PORTS], offset[F_PORTS], connected[F_PORTS];
} Bus;
static inline int band_channels(const Bus *b) {
    return b->width[0] > 32 || b->width[1] > 32 || b->width[2] > 32 || b->width[5] > 32 ? 2 : 1;
}
static inline void bus_setup(Bus *b, void *x, t_object *chain, short *count, int ports) {
    int offset = 0;
    for (int i = 0; i < ports; i++) {
        int w = (int)(long)object_method(chain, gensym("getnuminputchannels"), x, (long)i);
        b->width[i] = w > 0 ? w : 1;
        b->offset[i] = offset;
        offset += b->width[i];
        b->connected[i] = count[i];
    }
}
static inline double lane(const Bus *b, double **in, int port, int c, int band, int at,
                          double fallback) {
    int w = b->width[port];
    if (!b->connected[port])
        return fallback;
    int k = w == 1 ? 0 : w <= 32 ? band : 32 * c + band;
    return k < w ? in[b->offset[port] + k][at] : fallback;
}
static inline int native_rate(int hz) {
    for (int i = 0; i < 6; i++)
        if (codec_rates[i] == hz)
            return hz;
    return 0;
}
static inline Format live_format(int hz, int ch) {
    return (Format){LAYER, hz, 0, ch, ch == 1 ? 3 : 0, 32, -1, hz};
}
static inline Format read_format(Bus *b, double **in, int i, int hz, int ch, int base) {
    Format f = live_format(hz, ch);
    f.original_hz = base;
    if (!b->connected[F_FORMAT] || b->width[F_FORMAT] < FORMAT_LANES)
        return f;
    double **v = in + b->offset[F_FORMAT];
    int source = native_rate(sb_clamp(v[0][i], 0, 48000));
    if (!source)
        return f;
    f.hz = source;
    f.kbps = sb_clamp(v[1][i], 0, 448);
    f.channels = sb_clamp(v[2][i], 1, 2);
    if (f.channels < 1)
        f.channels = 1;
    f.mode = sb_clamp(v[3][i], 0, 3);
    f.bound = f.mode == 1 ? 4 * sb_clamp(v[4][i] / 4, 1, 4) : 32;
    f.pad = sb_clamp(v[5][i], -1, 1);
    int original = native_rate(sb_clamp(v[6][i], 0, 48000));
    f.original_hz = original >= source && original % source == 0 ? original : source;
    return f;
}
static inline int width_for_fields(int ch, int port) {
    return port < 3 ? 32 * ch : port == F_FORMAT ? FORMAT_LANES : 1;
}
typedef struct {
    double v[3][64], meta[FORMAT_LANES];
} View;
static inline void latch(View *v, const BandFrame *f, int row, int ch) {
    if (!row) {
        Format m = f->fmt;
        double meta[] = {m.hz, m.kbps, m.channels, m.mode, m.bound, m.pad, m.original_hz};
        memcpy(v->meta, meta, sizeof(meta));
    }
    for (int c = 0; c < ch; c++)
        for (int b = 0; b < 32; b++) {
            int k = c * 32 + b, input = c < f->fmt.channels ? c : 0;
            v->v[0][k] = f->q[row][input][b];
            v->v[1][k] = f->allocation[input][b];
            v->v[2][k] = f->scale[row / 12][input][b];
        }
}
static inline void view_fill(const View *v, double **out, int ch, int a, int z, int active) {
    if (a >= z)
        return;
    int lanes = ch * 32;
    for (int port = 0; port < 3; port++)
        for (int k = 0; k < lanes; k++) {
            double value = active ? v->v[port][k] : 0;
            double *d = out[port * lanes + k];
            for (int i = a; i < z; i++)
                d[i] = value;
        }
    for (int k = 0; k < FORMAT_LANES; k++) {
        double *d = out[3 * lanes + 1 + k];
        double value = active ? v->meta[k] : 0;
        for (int i = a; i < z; i++)
            d[i] = value;
    }
}
typedef struct {
    CodecClock clock;
    int pos;
} Timing;
static inline void timing_setup(Timing *t, double host, int factor) {
    codec_clock_setup_divided(&t->clock, host, factor);
    t->pos = 0;
}
static inline int timing_mark(Timing *t) {
    int ticks = codec_ticks(&t->clock), mark = 0;
    while (ticks--) {
        if (t->pos % 32 == 0)
            mark = t->pos / 32 + 1;
        if (++t->pos == 32 * sb_rows(LAYER))
            t->pos = 0;
    }
    return mark;
}
static inline int wire_mark(const Bus *b, double **in, int i, Timing *t) {
    if (!b->connected[F_SYNC])
        return timing_mark(t);
    double v = in[b->offset[F_SYNC]][i];
    return isfinite(v) && v == floor(v) && v >= -1 && v <= sb_rows(LAYER) ? (int)v : 0;
}
static inline uint32_t gate_mask(const Bus *b, double **in, int port, int c, int at) {
    uint32_t mask = 0;
    for (int j = 0; j < 32; j++) {
        double v = lane(b, in, port, c, j, at, 0);
        if (isfinite(v) && v >= .5)
            mask |= UINT32_C(1) << j;
    }
    return mask;
}
/* Only a complete, ordered slot sequence can become a freeze reference. */
typedef struct {
    BandFrame frame, saved;
    int next, have, all, level;
    uint32_t mask[2];
} Capture;
static inline int same_layout(Format a, Format b) {
    return a.hz == b.hz && a.channels == b.channels;
}
static inline void capture_begin(Capture *s, Format f, int freeze, int level,
                                 const uint32_t mask[2]) {
    s->next = 0;
    s->frame.fmt = f;
    if (s->have && !same_layout(s->saved.fmt, f))
        s->have = 0;
    s->all = s->have && freeze;
    s->level = level;
    for (int c = 0; c < 2; c++)
        s->mask[c] = !s->have ? 0 : s->all ? UINT32_MAX : mask[c];
}
static inline void hold_band(BandFrame *d, const BandFrame *h, int c, int b, int first, int count,
                             int level) {
    d->allocation[c][b] = h->allocation[c][b];
    for (int j = first; j < first + count; j++) {
        d->q[j][c][b] = h->q[j][c][b];
        if (level)
            d->scale[j / 12][c][b] = h->scale[j / 12][c][b];
    }
}
static inline int capture_row(Capture *s, const Bus *bus, double **in, int row, int i,
                              int normalized) {
    if (s->next != row)
        return 0;
    BandFrame *f = &s->frame;
    for (int c = 0; c < f->fmt.channels; c++)
        for (int b = 0; b < 32; b++) {
            if (!row)
                f->allocation[c][b] =
                    sb_clamp(lane(bus, in, F_ALLOC, c, b, i, 5), 0, LAYER == 1 ? 14 : 17);
            if (row % 12 == 0)
                f->scale[row / 12][c][b] = sb_clamp(lane(bus, in, F_SCALE, c, b, i, 18), 0, 62);
            int center = sb_levels(LAYER, f->allocation[c][b]) / 2;
            double q = lane(bus, in, F_DATA, c, b, i, 0);
            f->q[row][c][b] = sb_clamp(normalized ? q * center : q, -center, center);
            if (s->mask[c] & (UINT32_C(1) << b))
                hold_band(f, &s->saved, c, b, row, 1, s->all || s->level);
        }
    if (++s->next != sb_rows(LAYER))
        return 0;
    s->saved = *f;
    s->have = 1;
    s->next = -1;
    return 1;
}
typedef struct {
    _Atomic int hz, ch, kbps;
} Report;
static inline void report_set(Report *r, int hz, int ch, int kbps) {
    atomic_store(&r->hz, hz);
    atomic_store(&r->ch, ch);
    atomic_store(&r->kbps, kbps);
}
static inline void report_rate(void *status, double host, Report *r) {
    downsample_status(status, host, atomic_load(&r->hz), atomic_load(&r->ch));
}
static inline void report_bitrate(void *status, Report *r) {
    t_atom a;
    atom_setlong(&a, atomic_load(&r->kbps));
    outlet_anything(status, gensym("bitrate"), 1, &a);
}
static inline void zeros(double **out, long nout, long n) {
    for (long k = 0; k < nout; k++)
        memset(out[k], 0, n * sizeof(double));
}
static inline void field_outlets(t_object *x) {
    for (int i = F_FORMAT; i >= 0; i--)
        outlet_new(x, i == F_SYNC ? "signal" : "multichannelsignal");
}
static inline int set_kbps(t_object *x, _Atomic double *dest, double v) {
    if (!isfinite(v) || v < 0) {
        object_error(x, "bitrate expects a nonnegative number in kb/s");
        return 0;
    }
    atomic_store(dest, fmin(448., v));
    return 1;
}
static inline void set_divisor(t_object *x, _Atomic int *dest, long ac, t_atom *av) {
    int f = downsample_message(x, ac, av);
    if (f > 0)
        atomic_store(dest, f);
}
static inline const char *field_name(int port) {
    const char *const names[] = {
        "Signed subband sample words (MC)",   "Quantizer allocation per subband (MC)",
        "Scalefactor index per subband (MC)", "Slot marker (1 = frame start, -1 = idle)",
        "Stream format (seven MC lanes)",     "Band freeze gates (MC)"};
    return port >= 0 && port < 6 ? names[port] : "Status";
}
#endif
