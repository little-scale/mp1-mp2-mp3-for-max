#ifndef MPEG_LAYER2_BANDS_H
#define MPEG_LAYER2_BANDS_H
#include "band_input.h"
#include "pcm.h"

typedef enum { BAND_NORMALIZED, BAND_SIGNED, BAND_RAW } BandWordMode;
typedef struct {
    Bus inputs;
    PcmBank bank;
    Synthesis synthesis;
    BandFrame fields;
    float pcm[32];
    int hz, position, column;
} Layer2Bands;

static inline int band_word(double value, int center, BandWordMode mode) {
    if (!isfinite(value))
        return 0;
    if (mode == BAND_RAW)
        return sb_clamp(value, 0, center * 2) - center;
    if (mode == BAND_NORMALIZED)
        value = fmin(1., fmax(-1., value)) * center;
    return sb_clamp(value, -center, center);
}

static inline void bands2_slot(Layer2Bands *s, double **in, int i, int row, BandWordMode mode) {
    BandFrame *f = &s->fields;
    double words[32];
    uint32_t written = route_band_words(&s->inputs, in, i, words);
    float bands[32];
    for (int b = 0; b < 32; ++b) {
        if (row == 0)
            f->allocation[0][b] = sb_clamp(lane(&s->inputs, in, 2, 0, b, i, 7), 0, 17);
        if (row % 12 == 0)
            f->scale[row / 12][0][b] = sb_clamp(lane(&s->inputs, in, 3, 0, b, i, 18), 0, 62);
        int center = sb_levels(2, f->allocation[0][b]) / 2;
        f->q[row][0][b] = (written & (UINT32_C(1) << b)) ? band_word(words[b], center, mode) : 0;
        bands[b] = sb_amplitude(f, row, 0, b);
    }
    filter_synthesize(&s->synthesis, bands, s->pcm);
}

static inline void bands2_render(Layer2Bands *s, int hz, BandWordMode mode, double **in,
                                 double **out, int count) {
    PcmPath *path = pcm_select(&s->bank, hz);
    if (!path) {
        zeros(out, 3, count);
        return;
    }
    if (hz != s->hz) {
        s->hz = hz;
        s->position = s->column = 0;
        s->fields.fmt = live_format(hz, 1);
        memset(&s->synthesis, 0, sizeof(s->synthesis));
    }
    for (int i = 0; i < count; ++i) {
        int ticks = codec_ticks(&path->rate.clock), frame_start = 0;
        float pair[2] = {0, 0};
        for (int k = 0; k < ticks; ++k) {
            s->column = s->position % 32;
            if (!s->column)
                bands2_slot(s, in, i, s->position / 32, mode);
            if (!s->position)
                frame_start = 1;
            pair[0] = pair[1] = s->pcm[s->column];
            if (path->rate.factor < 0)
                fractional_push(&path->fractional, pair[0], pair[1]);
            if (++s->position == 32 * sb_rows(2))
                s->position = 0;
        }
        pcm_output_finish(path, pair);
        out[0][i] = pair[0];
        out[1][i] = s->column;
        out[2][i] = frame_start;
    }
}
#endif
