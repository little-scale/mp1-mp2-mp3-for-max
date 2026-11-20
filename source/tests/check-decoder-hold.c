/* Drive the Max decoder's byte inlet with a real packet stream and signal-rate gates.
   Write a separately assembled MP3 with the spectra expected at each playback boundary. */
#include "../mp3decode.c"
#include "../pack.h"

enum { LIVE = 32, FRAMES = 42, VECTOR = 1152 };
typedef struct { unsigned char bytes[L3_FRAME_MAX]; int len; } Packet;

static int window_for(int granule, int mixed) {
    static const int cycle[] = {0, 1, 2, 4, 2, 3, 0, 0};
    int w = granule < 24 ? cycle[granule % 8] : 0;
    return w == 4 && !mixed ? 2 : w;
}
static int held_band(int frame, int band) {
    return (frame >= 2 && frame < 23 && (band + frame / 3) % 4 == 0) ||
           (frame >= LIVE - 1 && frame < LIVE + 4 && (band == 0 || band == 15));
}
static int whole(int frame, int channels) {
    return channels == 1 && frame >= LIVE - 1 && frame < LIVE + 4;
}

int main(int argc, char **argv) {
    if (argc != 6) return 2;
    int hz = atoi(argv[1]), channels = atoi(argv[2]), mixed = atoi(argv[3]);
    int ng = hz >= 32000 ? 2 : 1, hop = ng * 576;
    FILE *reference = fopen(argv[4], "wb"), *audio = fopen(argv[5], "wb");
    Pack input, expected;
    int kb = ng == 2 ? 128 : 64;
    if (!reference || !audio || !pack_init(&input, hz, channels, kb) ||
        !pack_init(&expected, hz, channels, kb)) return 3;

    Packet packets[LIVE];
    Fields source[2][2] = {0}, output[2][2] = {0};
    unsigned seed = 421;
    for (int f = 0; f < FRAMES; ++f) {
        for (int gr = 0; gr < ng; ++gr) {
            for (int ch = 0; ch < channels; ++ch) {
                Fields *s = &source[gr][ch], *out = &output[gr][ch];
                memset(s, 0, sizeof(*s));
                s->gain = 170;
                s->window = window_for(f * ng + gr, mixed);
                if (f < LIVE)
                    for (int band = 0; band < 32; ++band) {
                        seed = seed * 1664525u + 1013904223u;
                        s->q[18 * band + (seed >> 8) % 18] = seed & 1 ? -2 : 3;
                    }
                if (whole(f, channels)) continue;
                out->gain = s->gain;
                out->window = s->window;
                for (int band = 0; band < 32; ++band)
                    if (!held_band(f, band))
                        memcpy(out->q + 18 * band, s->q + 18 * band, 18 * sizeof(float));
            }
        }
        if (f < LIVE) {
            Packet *p = &packets[f];
            p->len = pack_frame(&input, source, p->bytes);
            if (p->len < 4 || p->len > hop) return 4;
        }
        unsigned char buf[L3_FRAME_MAX];
        int n = pack_frame(&expected, output, buf);
        if (!n || fwrite(buf, 1, n, reference) != (size_t)n) return 4;
    }
    if (input.fitted || input.windows || expected.fitted || expected.windows) return 5;

    External x = {0};
    x.channels = 2;
    x.byte_mode = 1;
    x.bus.width[0] = x.bus.offset[1] = 1;
    x.bus.width[1] = 32;
    x.bus.connected[0] = x.bus.connected[1] = 1;
    x.s = calloc(1, sizeof(*x.s));
    x.synth = layer3_new();
    layer3_tables();
    x.byte_rates = byte_rates_new(hz, hz, 1);
    if (!x.byte_rates || !x.s || !x.synth || !rate_setup(&x.s->rate, hz)) return 6;
    int delay = hop + packets[0].len - 1;
    double data[33][VECTOR], *in[33], pcm[2][VECTOR], *out[] = {pcm[0], pcm[1]};
    for (int i = 0; i < 33; ++i) in[i] = data[i];
    for (int t = 0; t < delay + FRAMES * hop;) {
        int frame = t < delay ? -1 : (t - delay) / hop;
        int left = frame < 0 ? delay - t : hop - (t - delay) % hop;
        int n = left > 127 ? 127 : left;
        atomic_store(&x.freeze, whole(frame, channels));
        for (int i = 0; i < n; ++i) {
            int f = (t + i) / hop, pos = (t + i) % hop;
            data[0][i] = f < LIVE && pos < packets[f].len ? packets[f].bytes[pos] : -1;
            for (int band = 0; band < 32; ++band)
                data[1 + band][i] = held_band(frame, band);
        }
        perform(&x, NULL, in, 33, out, 2, n, 0, NULL);
        if (t >= delay)
            for (int i = 0; i < n; ++i) {
                float pair[2] = {(float)pcm[0][i], (float)pcm[1][i]};
                if (fwrite(pair, sizeof(float), channels, audio) != (size_t)channels) return 7;
            }
        t += n;
    }
    byte_rates_free(x.byte_rates);
    layer3_free(x.synth);
    free(x.s);
    return fclose(reference) || fclose(audio);
}
