#include "../filter.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int test_encode(int layer, int hz, int kb, int ch, FILE *out, double high_hz) {
    Analysis analysis[2] = {0};
    BandAudio input;
    BandFrame frame;
    Packet packet;
    Padding pad = {0};
    Format f = {layer, hz, kb, ch, ch == 1 ? 3 : 0, 32, 0, hz};
    long t = 0;
    for (int k = 0; k < 60; k++) {
        for (int j = 0; j < sb_rows(layer); j++) {
            for (int c = 0; c < ch; c++) {
                float pcm[32];
                for (int i = 0; i < 32; i++)
                    pcm[i] = .16 * sin(2 * acos(-1.) * (c ? 1231 : 431) * (t + i) / hz) +
                             .14 * sin(2 * acos(-1.) * high_hz * (t + i) / hz);
                filter_analyze(analysis + c, pcm, input.x[j][c]);
            }
            t += 32;
        }
        sb_quantize(&input, f, &frame);
        if (!sb_pack(&frame, kb, 0, &pad, &packet) ||
            fwrite(packet.data, 1, packet.length, out) != (size_t)packet.length)
            return 0;
    }
    return 1;
}
int main(int argc, char **argv) {
    sb_tables();
    filter_init();
    if ((argc == 7 || argc == 8) && !strcmp(argv[1], "encode")) {
        FILE *f = fopen(argv[6], "wb");
        if (!f)
            return 2;
        int good = test_encode(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), f,
                               argc == 8 ? atof(argv[7]) : 0);
        return fclose(f) || !good;
    }
    if (argc != 5 || strcmp(argv[1], "compare"))
        return 2;
    FILE *src = fopen(argv[2], "rb"), *pcm = fopen(argv[3], "wb"), *dup = fopen(argv[4], "wb");
    if (!src || !pcm || !dup)
        return 2;
    Synthesis synth[2] = {0};
    Padding pad = {0};
    Packet p, copy;
    uint8_t data[SB_PACKET];
    int count = 0;
    for (;;) {
        size_t n = fread(data, 1, 4, src);
        if (!n && feof(src))
            break;
        Format f;
        int len = n == 4 ? sb_header(data, &f) : 0;
        if (len <= 4 || len > SB_PACKET || fread(data + 4, 1, len - 4, src) != (size_t)len - 4 ||
            !sb_unpack(data, len, &p))
            return 3;
        if (!sb_pack(&p.fields, 0, 1, &pad, &copy) ||
            fwrite(copy.data, 1, copy.length, dup) != (size_t)copy.length)
            return 4;
        for (int j = 0; j < sb_rows(f.layer); j++) {
            float buf[2][32];
            for (int c = 0; c < f.channels; c++) {
                float band[32];
                for (int b = 0; b < 32; b++)
                    band[b] = sb_amplitude(&p.fields, j, c, b);
                filter_synthesize(synth + c, band, buf[c]);
            }
            for (int i = 0; i < 32; i++)
                for (int c = 0; c < f.channels; c++)
                    fwrite(&buf[c][i], sizeof(float), 1, pcm);
        }
        count++;
    }
    fclose(src);
    fclose(pcm);
    fclose(dup);
    return !count;
}
