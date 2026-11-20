#ifndef MP3PATCH_FIELD_HOLD_H
#define MP3PATCH_FIELD_HOLD_H
#include "field_bus.h"
#include <string.h>
typedef struct {
    Fields live[2][2], held[2][2];
    Fields anchor[2][2];
    int anchor_gain[2][2][32], input_gain[2][2];
    uint32_t mask[2];
    int request_level, level;
    int pos, visible, have, frozen;
} FieldHold;
static inline void hold_capture(FieldHold *h, int channels, int freeze, const FieldBus *b,
                                double **in, long i, unsigned *limited) {
    int pos = h->pos, within = pos % 576;
    if (!pos) {
        h->frozen = freeze && h->have;
        if (!h->frozen) {
            for (int ch = 0; ch < channels; ++ch) {
                uint32_t mask = h->have ? bus_hold_mask(b, BAND_HOLD_INLET, in, ch, i) : 0;
                uint32_t capture = h->request_level ? mask & ~(h->level ? h->mask[ch] : 0) : 0;
                for (int band = 0; capture && band < 32; ++band) {
                    uint32_t bit = UINT32_C(1) << band;
                    if (!(capture & bit))
                        continue;
                    for (int gr = 0; gr < 2; ++gr) {
                        int j = band * 18;
                        Fields *a = &h->anchor[gr][ch], *prev = &h->held[gr][ch];
                        memcpy(a->q + j, prev->q + j, 18 * sizeof(float));
                        memcpy(a->attenuation + j, prev->attenuation + j, 18 * sizeof(float));
                        h->anchor_gain[gr][ch][band] = prev->gain;
                    }
                    capture &= ~bit;
                }
                h->mask[ch] = mask;
            }
            h->level = h->request_level;
        }
    }
    h->visible = pos;
    if (within % 32 || h->frozen)
        return;
    int gr = pos / 576, row = within / 32;
    for (int ch = 0; ch < channels; ++ch) {
        Fields *live = &h->live[gr][ch];
        uint32_t mask = h->mask[ch];
        bus_capture(b, in, live, &h->held[gr][ch], mask, ch, within, i, limited);
        if (!h->level || !mask)
            continue;
        if (!within) {
            h->input_gain[gr][ch] = live->gain;
            int common = mask == UINT32_MAX ? 0 : live->gain;
            for (int band = 0; band < 32; ++band)
                if ((mask & (UINT32_C(1) << band)) && h->anchor_gain[gr][ch][band] > common)
                    common = h->anchor_gain[gr][ch][band];
            live->gain = common;
        }
        for (int band = 0; band < 32; ++band) {
            int j = 18 * band + row;
            if (mask & (UINT32_C(1) << band))
                field_rebase(live, &h->anchor[gr][ch], j, h->anchor_gain[gr][ch][band]);
            else
                field_rebase(live, live, j, h->input_gain[gr][ch]);
        }
    }
}
static inline Fields (*hold_output(FieldHold *h))[2] { return h->frozen ? h->held : h->live; }
static inline void hold_advance(FieldHold *h, int granules, int channels) {
    if (++h->pos == 576 * granules) {
        if (!h->frozen) {
            for (int gr = 0; gr < granules; gr++)
                memcpy(h->held[gr], h->live[gr], channels * sizeof(Fields));
            h->have = 1;
        }
        h->pos = 0;
    }
}
#endif
