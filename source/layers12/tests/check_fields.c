#include "../max_common.h"
#include "../pcm.h"
#include <assert.h>

static void capture(Capture *cap, Bus *bus, double **in, Format f, int all, int level,
                    uint32_t left, uint32_t right) {
    uint32_t mask[2] = {left, right};
    capture_begin(cap, f, all, level, mask);
    for (int row = 0; row < sb_rows(LAYER); row++)
        assert(capture_row(cap, bus, in, row, 0, 0) == (row + 1 == sb_rows(LAYER)));
}

static void band_controls(void) {
    Bus bus = {.width = {64, 1, 1}, .offset = {0, 64, 65}, .connected = {1, 1, 1}};
    double values[66], *in[66];
    for (int i = 0; i < 66; i++)
        in[i] = values + i;
    for (int i = 0; i < 64; i++)
        values[i] = i % 2 ? -1e100 : 1e100;
    values[1] = NAN;
    values[2] = INFINITY;
    values[64] = 1e100;
    values[65] = 1e100;
    Format f = live_format(48000, 2);
    Capture cap = {0};
    capture(&cap, &bus, in, f, 0, 0, 0, 0);
    int high = LAYER == 1 ? 14 : 17, center = sb_levels(LAYER, high) / 2;
    assert(cap.frame.q[0][0][0] == center && cap.frame.q[0][1][3] == -center);
    assert(!cap.frame.q[0][0][1] && !cap.frame.q[0][0][2]);
    assert(cap.frame.allocation[1][0] == high && cap.frame.scale[0][1][0] == 62);
    Packet p, roundtrip;
    Padding pad = {0};
    assert(sb_pack(&cap.frame, 32, 0, &pad, &p) && sb_unpack(p.data, p.length, &roundtrip));
    for (int i = 0; i < 64; i++)
        values[i] = 4;
    values[64] = 5;
    values[65] = 20;
    capture(&cap, &bus, in, f, 0, 0, 1, 8);
    assert(cap.frame.q[0][0][0] == center && cap.frame.q[0][1][3] == -center);
    assert(cap.frame.q[0][1][0] == 4 && cap.frame.q[0][0][3] == 4);
    assert(cap.frame.scale[0][0][0] == 20);
    values[65] = 11;
    capture(&cap, &bus, in, f, 0, 1, 1, 8);
    assert(cap.frame.scale[0][0][0] == 20 && cap.frame.scale[0][1][0] == 11);
    BandFrame frozen = cap.frame;
    values[64] = 0;
    values[65] = 42;
    capture(&cap, &bus, in, f, 1, 0, 0, 0);
    assert(!memcmp(&frozen, &cap.frame, sizeof(frozen)));
    capture(&cap, &bus, in, f, 0, 0, 0, 0);
    assert(cap.frame.allocation[0][0] == 0 && cap.frame.q[0][1][3] == 0);
}

static void host_rates(void) {
    const double hosts[] = {8000, 22050, 44100, 44117.25, 48000, 96000, 192000};
    for (unsigned i = 0; i < sizeof hosts / sizeof hosts[0]; i++) {
        double host = hosts[i];
        int native = downsample_hz(host, 1);
        Timing t;
        timing_setup(&t, host, 1);
        int starts = 0;
        int n = (int)ceil(host * .5);
        for (int j = 0; j < n; j++)
            starts += timing_mark(&t) == 1;
        assert(abs(starts - (int)round((double)n * native / (host * 32 * sb_rows(LAYER)))) <= 1);
        Playback *p = calloc(1, sizeof(*p));
        assert(p && pcm_setup(&p->bank, host, 0));
        BandFrame fields;
        Analysis analysis = {0};
        BandAudio input;
        double re = 0, im = 0, energy = 0;
        int used = 0, frame = 0, rows = sb_rows(LAYER);
        for (int j = 0; j < n; j++) {
            if (j >= (int)ceil(frame * rows * 32 * host / native)) {
                for (int row = 0; row < rows; row++) {
                    float pcm[32];
                    for (int k = 0; k < 32; k++)
                        pcm[k] = .12 * sin(2 * acos(-1.) * 431 * ((frame * rows + row) * 32 + k) /
                                           native);
                    filter_analyze(&analysis, pcm, input.x[row][0]);
                }
                sb_quantize(&input, live_format(native, 1), &fields);
                playback_frame(p, &fields);
                frame++;
            }
            float out[2];
            playback_sample(p, out);
            if (j > n / 2) {
                double angle = 2 * acos(-1.) * 431 * j / host;
                re += out[0] * cos(angle);
                im += out[0] * sin(angle);
                energy += out[0] * out[0];
                used++;
                assert(isfinite(out[0]) && out[0] == out[1]);
            }
        }
        double gain = 2 * hypot(re, im) / used;
        assert(gain > .11 && gain < .13 && gain / sqrt(energy / used) > 1.36);
        pcm_free(&p->bank);
        free(p);
    }
}
int main(void) {
    sb_tables();
    filter_init();
    band_controls();
    host_rates();
    return 0;
}
