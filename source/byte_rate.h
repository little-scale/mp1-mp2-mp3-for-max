#ifndef MP3PATCH_BYTE_RATE_H
#define MP3PATCH_BYTE_RATE_H
#include "rate.h"
typedef struct {
    CodecClock source_clock;
    Rate decimator, output;
    Fractional interpolator;
    int until_next;
} ByteRate;
typedef struct {
    ByteRate paths[9];
    int active;
} ByteRates;
static inline int byte_target(int source, int ceiling, int divide) {
    if (divide <= 1 || source <= ceiling)
        return source;
    int target = source;
    for (int i = 0; i < 9; ++i) {
        int hz = codec_rates[i];
        if (hz <= source && source % hz == 0) {
            target = hz;
            if (hz <= ceiling)
                break;
        }
    }
    return target;
}
static inline void byte_rates_free(ByteRates *b) {
    if (!b)
        return;
    for (int i = 0; i < 9; ++i)
        fractional_free(&b->paths[i].interpolator);
    free(b);
}
static inline ByteRates *byte_rates_new(double host, int ceiling, int divide) {
    ByteRates *b = calloc(1, sizeof(*b));
    if (!b)
        return NULL;
    b->active = -1;
    for (int i = 0; i < 9; ++i) {
        int src = codec_rates[i], dest = byte_target(src, ceiling, divide);
        ByteRate *r = &b->paths[i];
        if (!codec_clock_setup(&r->source_clock, host) ||
            !rate_setup_native(&r->decimator, src, dest) ||
            !rate_setup_native(&r->output, host, dest) ||
            (r->output.factor < 0 && !fractional_setup(&r->interpolator, dest, host))) {
            byte_rates_free(b);
            return NULL;
        }
        r->source_clock.hz = src;
    }
    return b;
}
static inline int byte_rates_select(ByteRates *b, int hz) {
    for (int i = 0; i < 9; ++i)
        if (codec_rates[i] == hz) {
            if (b->active != i) {
                ByteRate *r = &b->paths[i];
                r->source_clock.first = r->output.clock.first = 1;
                r->source_clock.phase = r->output.clock.phase = 0;
                r->until_next = r->decimator.head = r->output.head = 0;
                memset(r->decimator.delay, 0, sizeof(r->decimator.delay));
                memset(r->output.delay, 0, sizeof(r->output.delay));
                if (r->interpolator.history) {
                    memset(r->interpolator.history, 0, 2 * r->interpolator.taps * sizeof(float));
                    r->interpolator.head = 0;
                }
                b->active = i;
            }
            return i;
        }
    return -1;
}
#endif
