/* cc -std=c11 -O2 -I.. check-band-hold.c -lm -o check-band-hold */
#include "../field_hold.h"
#include <assert.h>
#include <stdio.h>

static double gate(int frame, int lane) {
    static const double levels[] = {1., 0., .5, .49, NAN, -1., INFINITY, 0.9};
    return levels[(frame / 2 + lane * 3) % 8];
}
static int coefficient(int frame, int gr, int channel, int band, int row) {
    return frame * 200 + gr * 60 + channel * 32 + band - row;
}
static int attenuation(int frame, int gr, int channel, int band, int row) {
    return (frame * 3 + gr * 6 + channel + band + row) % 60;
}
static void exercise(int granules, int width, int connected) {
    FieldBus bus = {.width = {64, 64, 2, 2, 1, width},
                    .offset = {0, 64, 128, 130, 132, 133},
                    .connected = {1, 1, 1, 1, 0, connected}};
    FieldHold h = {0};
    double values[197] = {0}, *in[197];
    for (int i = 0; i < 197; i++)
        in[i] = values + i;
    int from[2][32] = {{0}}, meta = 0;
    unsigned limited = 0;
    for (int frame = 0; frame < 19; frame++) {
        int all = frame == 7 || frame == 8 || frame == 16;
        if (!all)
            meta = frame;
        for (int ch = 0; ch < 2; ch++)
            for (int b = 0; b < 32; b++) {
                int lane = width == 1 ? 0 : width <= 32 ? b : ch * 32 + b;
                double v = lane < width ? gate(frame, lane) : 0;
                if (!all && !(frame && connected && isfinite(v) && v >= .5))
                    from[ch][b] = frame;
            }
        for (int p = 0; p < granules * 576; p++) {
            int gr = p / 576, row = p % 576 / 32;
            if (!(p % 32)) {
                for (int ch = 0; ch < 2; ch++) {
                    values[128 + ch] = 127 + frame + ch + gr;
                    values[130 + ch] = (frame + gr + ch) % 6;
                    for (int b = 0; b < 32; b++) {
                        values[ch * 32 + b] = coefficient(frame, gr, ch, b, row);
                        values[64 + ch * 32 + b] = attenuation(frame, gr, ch, b, row);
                    }
                }
            }
            /* Opposite gate values inside the frame must not change the latch. */
            for (int j = 0; j < width; j++)
                values[133 + j] = p ? 0.0 : gate(frame, j);
            hold_capture(&h, 2, all, &bus, in, 0, &limited);
            if (!(p % 32))
                for (int ch = 0; ch < 2; ch++) {
                    Fields *f = &hold_output(&h)[gr][ch];
                    assert(f->gain == 127 + meta + ch + gr && f->window == (meta + gr + ch) % 6);
                    for (int b = 0; b < 32; b++) {
                        int j = b * 18 + row;
                        assert(f->q[j] == coefficient(from[ch][b], gr, ch, b, row));
                        assert(f->attenuation[j] == attenuation(from[ch][b], gr, ch, b, row));
                    }
                }
            hold_advance(&h, granules, 2);
        }
    }
    assert(!limited);
}
static void level_hold(int ng) {
    FieldHold h = {.request_level = 1};
    FieldBus bus = {.width = {32, 1, 1, 1, 1, 32},
                    .offset = {0, 32, 33, 34, 35, 36},
                    .connected = {1, 1, 1, 0, 0, 1}};
    double value[68] = {0}, *in[68];
    for (int i = 0; i < 68; i++)
        in[i] = value + i;
    unsigned limited = 0;
    for (int fr = 0; fr < 7; fr++) {
        for (int pos = 0; pos < ng * 576; pos++) {
            int gr = pos / 576, row = pos % 576 / 32;
            int gain = (int[]){20, 48, 255, 20, 90, 20, 30}[fr] + (fr == 2 ? 0 : gr);
            value[33] = gain;
            value[0] = fr ? 160 : 32 + row + gr;
            value[1] = 10 + fr;
            value[32] = 8;
            value[36] = fr >= 1 && fr <= 5;
            hold_capture(&h, 1, fr == 4, &bus, in, 0, &limited);
            Fields *f = &hold_output(&h)[gr][0];
            if (fr == 1 || fr == 3 || fr == 4 || fr == 5) {
                int held_gain = 20 + gr;
                assert(f->q[row] == 32 + row + gr);
                assert(f->gain - (int)f->attenuation[row] == held_gain - 8);
            }
            if (fr != 4) {
                assert(f->q[18 + row] == 10 + fr);
                assert(f->gain - (int)f->attenuation[18 + row] == gain - 8);
            }
            if (fr == 6)
                assert(f->q[row] == 160 && f->attenuation[row] == 8);
            hold_advance(&h, ng, 1);
        }
    }
    assert(!limited);
}
int main(void) {
    level_hold(1);
    level_hold(2);
    const int widths[] = {1, 3, 32, 33, 47, 64};
    for (int ng = 1; ng <= 2; ng++)
        for (int w = 0; w < 6; w++) {
            exercise(ng, widths[w], 0);
            exercise(ng, widths[w], 1);
        }
    puts("Per-band gates, stereo addressing, both granule counts and whole-frame hold passed.");
}
