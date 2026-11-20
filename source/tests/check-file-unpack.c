/* Offline reference for the file -> MC fields -> pack path. PCM is an optional
   independent measurement tap in this test executable. */
#include "field_file.h"
#include "pack.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "usage: check-file-unpack input.mp3 output.mp3 fields.f32\n");
        return 2;
    }
    layer3_tables();
    const char *error = NULL;
    FieldFile *file = field_file_open(argv[1], &error);
    if (!file) {
        fprintf(stderr, "%s\n", error);
        return 1;
    }
    int hz = field_file_rate(file), channels = field_file_channels(file);
    FILE *out = fopen(argv[2], "wb"), *pcm = fopen(argv[3], "wb");
    Synthesis *monitor = layer3_new();
    Pack pack;
    if (!out || !pcm || !monitor || !pack_init(&pack, hz, channels, 0))
        return 2;
    unsigned char bytes[L3_FRAME_MAX];
    unsigned windows[6] = {0}, joint = 0, borrowed = 0;
    for (;;) {
        int status = 0;
        const EncodedFrame *frame = field_file_peek(file, &status);
        if (!frame) {
            if (status < 0) {
                fprintf(stderr, "%s\n", field_file_error(file));
                return 1;
            }
            if (status)
                break;
            struct timespec delay = {0, 500000};
            nanosleep(&delay, NULL);
            continue;
        }
        int n = pack_file_frame(&pack, frame->fields, bytes);
        if (n <= 0 || fwrite(bytes, 1, n, out) != (size_t)n)
            return 3;
        joint += ((frame->bytes[3] >> 6) == 1 && (frame->bytes[3] & 48) != 0);
        int side = 4 + ((frame->bytes[1] & 1) ? 0 : 2);
        borrowed +=
            (frame->bytes[side] != 0 || (frame->granules == 2 && (frame->bytes[side + 1] & 128)));
        for (int g = 0; g < frame->granules; g++) {
            float band[2][576], pair[1152];
            for (int ch = 0; ch < channels; ch++) {
                Fields fields = frame->fields[g][ch];
                if (fields.gain < 0 || fields.gain > 255 || fields.window < 0 || fields.window > 5)
                    return 4;
                windows[fields.window]++;
                for (int i = 0; i < 576; i++)
                    if (!isfinite(fields.q[i]) || fabs(fields.q[i]) > 8206 ||
                        !isfinite(fields.attenuation[i]) || fields.attenuation[i] < 0 ||
                        fields.attenuation[i] > 210)
                        return 4;
                layer3_render(monitor, &fields, ch, band[ch]);
            }
            for (int i = 0; i < 576; i++)
                for (int ch = 0; ch < channels; ch++)
                    pair[i * channels + ch] = band[ch][i];
            if (fwrite(pair, sizeof(float), 576 * channels, pcm) != (size_t)(576 * channels))
                return 3;
        }
        field_file_advance(file);
    }
    printf(
        "%d Hz, %d channels, %lu frames, %u joint, %u reservoir, %lu fitted, %u short, %u mixed\n",
        hz, channels, pack.frames, joint, borrowed, pack.fitted, windows[2],
        windows[4] + windows[5]);
    fclose(out);
    fclose(pcm);
    layer3_free(monitor);
    field_file_close(file);
    return 0;
}
