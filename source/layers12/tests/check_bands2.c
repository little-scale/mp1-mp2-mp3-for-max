#include "../bands2.h"
#include <assert.h>

static Bus make_bus(int words, int addresses, int allocation, int scale) {
    int widths[] = {words, addresses, allocation, scale};
    Bus b = {0};
    int offset = 0;
    for (int i = 0; i < 4; i++) {
        b.connected[i] = widths[i] > 0;
        b.width[i] = widths[i] ? widths[i] : 1;
        b.offset[i] = offset;
        offset += b.width[i];
    }
    return b;
}

static void check_routing(void) {
    double samples[40] = {3, 6, 9, 12}, words[32], *in[40];
    for (int i = 0; i < 40; i++)
        in[i] = samples + i;
    Bus b = make_bus(4, 0, 0, 0);
    assert(route_band_words(&b, in, 0, words) == 15 && words[0] == 3 && words[3] == 12);
    b = make_bus(4, 1, 0, 0);
    samples[4] = 31.8;
    assert(route_band_words(&b, in, 0, words) == UINT32_C(0x80000007));
    assert(words[31] == 3 && words[0] == 6 && words[1] == 9 && words[2] == 12);
    b = make_bus(4, 4, 0, 0);
    samples[4] = NAN; samples[5] = -1; samples[6] = 1.9; samples[7] = 1000;
    assert(route_band_words(&b, in, 0, words) == UINT32_C(0x80000003));
    assert(words[0] == 3 && words[1] == 9 && words[31] == 12);
    b = make_bus(1, 0, 0, 0);
    assert(route_band_words(&b, in, 0, words) == UINT32_MAX);
    for (int i = 0; i < 32; ++i)
        assert(words[i] == 3);
    b = make_bus(0, 0, 0, 0);
    assert(route_band_words(&b, NULL, 0, words) == 0);
}

/* Reference is mp2decode~ mc's field capture, with an explicit permutation
   into its fixed MC lanes. Change controls between latch points as well. */
static void check_slots(BandWordMode mode) {
    Layer2Bands s = {0};
    Capture reference = {0};
    Synthesis synthesis = {0};
    Bus decoded = make_bus(32, 32, 32, 0);
    s.inputs = make_bus(32, 32, 32, 32);
    s.fields.fmt = live_format(48000, 1);
    double fields[128], ordered[96], *in[128], *ref[96];
    for (int i = 0; i < 128; i++)
        in[i] = fields + i;
    for (int i = 0; i < 96; i++)
        ref[i] = ordered + i;
    for (int frame = 0; frame < 2; frame++) {
        capture_begin(&reference, s.fields.fmt, 0, 0, (uint32_t[2]){0, 0});
        for (int row = 0; row < 36; row++) {
            for (int lane = 0; lane < 32; lane++) {
                int dest = (lane + row * 3 + frame) % 32;
                int allocation = (dest + frame) % 18;
                int center = sb_levels(2, allocation) / 2;
                int word = (lane * 19 + row * 37) % (2 * center + 1) - center;
                double value = mode == BAND_NORMALIZED && center ? (double)word / center : word;
                fields[lane] = mode == BAND_RAW ? word + center : value;
                fields[32 + lane] = dest;
                fields[64 + dest] = row == 0 ? allocation : 0;
                fields[96 + dest] = row % 12 == 0 ? (row + dest + frame) % 63 : 62;
                ordered[dest] = value;
                ordered[32 + dest] = fields[64 + dest];
                ordered[64 + dest] = fields[96 + dest];
            }
            capture_row(&reference, &decoded, ref, row, 0, mode == BAND_NORMALIZED);
            bands2_slot(&s, in, 0, row, mode);
            float bands[32], pcm[32];
            for (int i = 0; i < 32; i++) {
                assert(s.fields.q[row][0][i] == reference.frame.q[row][0][i]);
                assert(s.fields.allocation[0][i] == reference.frame.allocation[0][i]);
                assert(s.fields.scale[row/12][0][i] == reference.frame.scale[row/12][0][i]);
                bands[i] = sb_amplitude(&reference.frame, row, 0, i);
            }
            filter_synthesize(&synthesis, bands, pcm);
            assert(memcmp(pcm, s.pcm, sizeof(pcm)) == 0);
        }
    }
}

static void check_limits(void) {
    Layer2Bands s = {0};
    s.inputs = make_bus(2, 2, 1, 1);
    s.fields.fmt = live_format(48000, 1);
    double v[] = {-100, 100, 1, 31, 100, -100}, *in[6];
    for (int i = 0; i < 6; i++)
        in[i] = v + i;
    bands2_slot(&s, in, 0, 0, BAND_RAW);
    assert(s.fields.allocation[0][0] == 17 && s.fields.scale[0][0][0] == 0);
    assert(s.fields.q[0][0][0] == 0 && s.fields.q[0][0][1] == -32767);
    v[4] = 2; v[5] = 100;
    bands2_slot(&s, in, 0, 0, BAND_RAW);
    assert(s.fields.q[0][0][0] == 0 && s.fields.q[0][0][1] == -2 && s.fields.q[0][0][31] == 2);
    assert(s.fields.scale[0][0][0] == 62);
    v[0] = NAN; v[1] = INFINITY;
    bands2_slot(&s, in, 0, 1, BAND_RAW);
    assert(s.fields.q[1][0][1] == 0 && s.fields.q[1][0][31] == 0);
    assert(band_word(-2., 2, BAND_NORMALIZED) == -2 && band_word(2., 2, BAND_NORMALIZED) == 2);
    assert(band_word(-1.5, 2, BAND_SIGNED) == -2 && band_word(1.5, 2, BAND_SIGNED) == 2);
}

static void check_clock(double host) {
    Layer2Bands s = {0};
    assert(pcm_setup(&s.bank, host, 0));
    const int divisors[] = {1, 2, 1};
    enum { BLOCK = 63 };
    double audio[BLOCK], column[BLOCK], pulse[BLOCK], *out[] = {audio, column, pulse};
    double tone[BLOCK], zero[BLOCK] = {0}, *in[] = {tone, zero};
    for (int j = 0; j < BLOCK; j++)
        tone[j] = .75;
    for (int d = 0; d < 3; d++) {
        int hz = downsample_hz(host, divisors[d]);
        if (d && hz == downsample_hz(host, divisors[d-1]))
            continue;
        int frames = 0;
        double power = 0;
        int count = (int)ceil(4.5 * 1152 * host / hz);
        for (int i = 0; i < count; i += BLOCK) {
            int n = count - i < BLOCK ? count - i : BLOCK;
            int active = i >= count / 2;
            s.inputs = make_bus(active ? 2 : 0, 0, 0, 0);
            bands2_render(&s, hz, BAND_NORMALIZED, in, out, n);
            for (int j = 0; j < n; j++) {
                int at = i + j;
                int native = (int)floor(at * (double)hz / host);
                assert(column[j] == native % 32);
                assert(isfinite(audio[j]) && (active || audio[j] == 0));
                power += audio[j] * audio[j];
                if (pulse[j]) {
                    assert(pulse[j] == 1 && at == (int)ceil(frames * 1152. * host / hz));
                    frames++;
                }
            }
        }
        assert(frames == 5 && power > 1.e-6 * count);
    }
    pcm_free(&s.bank);
}

int main(void) {
    sb_tables();
    filter_init();
    check_routing();
    check_limits();
    check_slots(BAND_NORMALIZED);
    check_slots(BAND_SIGNED);
    check_slots(BAND_RAW);
    const double hosts[] = {8000, 32000, 44100, 44117.25, 48000, 96000, 192000};
    for (unsigned i = 0; i < sizeof(hosts) / sizeof(hosts[0]); ++i)
        check_clock(hosts[i]);
    return 0;
}
