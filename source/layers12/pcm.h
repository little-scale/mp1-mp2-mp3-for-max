#ifndef MPEG_SUBBAND_PCM_H
#define MPEG_SUBBAND_PCM_H
#include "../rate.h"
#include "filter.h"
typedef struct {
    Rate rate;
    Fractional fractional;
} PcmPath;
typedef struct {
    PcmPath path[6];
    int active;
} PcmBank;
static inline void pcm_free(PcmBank *p) {
    for (int i = 0; i < 6; i++)
        fractional_free(&p->path[i].fractional);
}
static inline int pcm_setup(PcmBank *p, double host, int input) {
    pcm_free(p);
    memset(p, 0, sizeof(*p));
    p->active = -1;
    for (int i = 0; i < 6; i++) {
        int hz = codec_rates[i];
        PcmPath *b = p->path + i;
        if (!rate_setup_native(&b->rate, host, hz) ||
            (b->rate.factor < 0 &&
             !fractional_setup(&b->fractional, input ? host : hz, input ? hz : host))) {
            pcm_free(p);
            return 0;
        }
    }
    return 1;
}
static inline void pcm_reset(PcmBank *p) {
    p->active = -1;
}
static inline PcmPath *pcm_select(PcmBank *p, int hz) {
    if (p->active < 0 || p->path[p->active].rate.hz != hz) {
        int i = 0;
        while (i < 6 && codec_rates[i] != hz)
            i++;
        if (i == 6)
            return NULL;
        p->active = i;
        PcmPath *b = p->path + i;
        Rate *r = &b->rate;
        r->head = 0;
        r->clock.first = 1;
        r->clock.phase = 0;
        memset(r->delay, 0, sizeof(r->delay));
        b->fractional.head = 0;
        if (b->fractional.history)
            memset(b->fractional.history, 0, 2 * b->fractional.taps * sizeof(float));
    }
    return p->path + p->active;
}
static inline void pcm_input_push(PcmPath *p, float l, float r) {
    if (p->rate.factor > 1)
        rate_push(&p->rate, l, r);
    if (p->rate.factor < 0)
        fractional_push(&p->fractional, l, r);
}
static inline float pcm_input_read(PcmPath *p, int ch, int remaining, float direct) {
    Rate *r = &p->rate;
    if (r->factor == 1)
        return direct;
    if (r->factor > 1)
        return rate_filter(r, ch);
    return fractional_read(&p->fractional, ch,
                           (remaining * r->clock.host + r->clock.phase) / r->hz);
}
typedef struct {
    float pcm[1152][2];
    int samples;
} PcmFrame;
typedef struct {
    PcmBank bank;
    Synthesis synth[2];
    PcmFrame queue[8];
    unsigned rd, wr;
    int pos, hz, channels, tail, preroll;
} Playback;
static inline void playback_reset(Playback *p) {
    p->rd = p->wr = 0;
    p->pos = p->tail = p->preroll = 0;
    p->hz = 0;
    memset(p->synth, 0, sizeof(p->synth));
    pcm_reset(&p->bank);
}
static inline void playback_frame(Playback *p, const BandFrame *f) {
    if (p->hz != f->fmt.hz || p->channels != f->fmt.channels) {
        playback_reset(p);
        p->hz = f->fmt.hz;
        p->channels = f->fmt.channels;
    }
    if (p->wr - p->rd >= 8)
        return;
    /* Frame bytes are bursty at variable bitrates. One frame of headroom keeps
       consecutive frames on a continuous native sample clock. */
    if (p->rd == p->wr && !p->tail)
        p->preroll = 32 * sb_rows(f->fmt.layer);
    PcmFrame *dst = &p->queue[p->wr++ % 8];
    dst->samples = 32 * sb_rows(f->fmt.layer);
    for (int row = 0; row < dst->samples / 32; row++) {
        for (int c = 0; c < p->channels; c++) {
            float band[32], pcm[32];
            for (int b = 0; b < 32; b++)
                band[b] = sb_amplitude(f, row, c, b);
            filter_synthesize(p->synth + c, band, pcm);
            for (int j = 0; j < 32; j++)
                dst->pcm[32 * row + j][c] = pcm[j];
        }
    }
    if (p->channels == 1)
        for (int j = 0; j < dst->samples; j++)
            dst->pcm[j][1] = dst->pcm[j][0];
    p->tail = 16;
}
static inline void playback_sample(Playback *p, float pair[2]) {
    pair[0] = pair[1] = 0;
    if (!p->hz)
        return;
    PcmPath *b = pcm_select(&p->bank, p->hz);
    if (!b)
        return;
    Rate *r = &b->rate;
    int ticks = codec_ticks(&r->clock);
    for (int k = 0; k < ticks; k++) {
        pair[0] = pair[1] = 0;
        if (p->preroll) {
            p->preroll--;
            continue;
        }
        if (p->rd == p->wr && p->tail) {
            static const float silence[32] = {0};
            PcmFrame *dst = &p->queue[p->wr++ % 8];
            dst->samples = 32;
            for (int c = 0; c < p->channels; c++) {
                float v[32];
                filter_synthesize(p->synth + c, silence, v);
                for (int j = 0; j < 32; j++)
                    dst->pcm[j][c] = v[j];
            }
            if (p->channels == 1)
                for (int j = 0; j < 32; j++)
                    dst->pcm[j][1] = dst->pcm[j][0];
            p->tail--;
        }
        if (p->rd != p->wr) {
            PcmFrame *src = &p->queue[p->rd % 8];
            memcpy(pair, src->pcm[p->pos++], 2 * sizeof(float));
            if (p->pos == src->samples) {
                p->rd++;
                p->pos = 0;
            }
        }
        if (r->factor < 0)
            fractional_push(&b->fractional, pair[0], pair[1]);
    }
    if (r->factor > 1) {
        rate_push(r, pair[0], pair[1]);
        for (int c = 0; c < 2; c++)
            pair[c] = rate_interpolate(r, c);
    }
    if (r->factor < 0)
        for (int c = 0; c < 2; c++)
            pair[c] = fractional_read(&b->fractional, c, -r->clock.phase / r->clock.host);
}
#endif
