#include "../reader.h"
#include "../worker.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static uint32_t random_state = 0x24531085;
static unsigned next_random(void) {
    random_state ^= random_state << 13;
    random_state ^= random_state >> 17;
    random_state ^= random_state << 5;
    return random_state;
}
static void make_fields(BandFrame *f, int sequence) {
    int rows = sb_rows(f->fmt.layer);
    for (int c = 0; c < f->fmt.channels; c++) {
        for (int b = 0; b < 32; b++) {
            int sparse = sequence % 4 < 2;
            int active = !sparse || ((b + sequence * 3) % 17 == 0);
            int a = active ? 1 + (int)(next_random() % (f->fmt.layer == 1 ? 14 : 17)) : 0;
            int shared = c == 1 && f->fmt.mode == 1 && b >= f->fmt.bound;
            if (shared)
                a = f->allocation[0][b];
            f->allocation[c][b] = a;
            for (int g = 0; g < rows / 12; g++) {
                int sf = 8 + (b + sequence + c) % 19;
                if (sequence % 4 == 1)
                    sf = g == 2 ? 14 : 9;
                if (sequence % 4 == 2)
                    sf = g == 0 ? 8 : 12;
                if (sequence % 4 == 3)
                    sf += g;
                f->scale[g][c][b] = sf;
            }
            int levels = sb_levels(f->fmt.layer, a);
            for (int j = 0; j < rows; j++) {
                int q = levels ? (int)(next_random() % (unsigned)levels) - levels / 2 : 0;
                if (j == 0)
                    q = levels / 2;
                if (j == 1)
                    q = -levels / 2;
                f->q[j][c][b] = shared ? f->q[j][0][b] : q;
            }
        }
    }
}
static int matrix(FILE *file, int layer, int hz, int ch) {
    BandFrame f = {0};
    Packet written, parsed;
    Padding padding = {0};
    int prior = 0, sequence = 0;
    for (int requested = 1; requested <= 448; requested++) {
        int kb = sb_bitrate(layer, hz, ch, requested, 1000);
        if (kb == prior)
            continue;
        prior = kb;
        for (int k = 0; k < 6; k++, sequence++) {
            /* A stable first pair lets general audio demuxers identify the stream
               before the tests start changing channel modes between frames. */
            int mode = ch == 1 ? 3 : sequence < 4 ? 1 : k == 1 ? 0 : k == 2 ? 2 : 1;
            f.fmt = (Format){layer, hz, kb, ch, mode, sequence < 4 ? 4 : 4 + 4 * (k % 4), -1, hz};
            make_fields(&f, sequence);
            if (!sb_pack(&f, kb, 0, &padding, &written) ||
                !sb_unpack(written.data, written.length, &parsed))
                return 0;
            for (int c = 0; c < ch; c++)
                for (int b = 0; b < 32; b++)
                    for (int row = 0; row < sb_rows(layer); row++) {
                        if (written.fields.q[row][c][b] != parsed.fields.q[row][c][b] ||
                            sb_amplitude(&written.fields, row, c, b) !=
                                sb_amplitude(&parsed.fields, row, c, b))
                            return 0;
                    }
            if (fwrite(written.data, 1, written.length, file) != (size_t)written.length)
                return 0;
        }
    }
    return 1;
}
static int transport(int layer, int reader_div, int writer_div, const char *source,
                     const char *destination) {
    const char *err = NULL;
    Reader *r = reader_open(source, layer, reader_div, &err);
    if (!r) {
        fprintf(stderr, "%s\n", err);
        return 0;
    }
    if (!reader_rewind(r, &err)) {
        reader_close(r);
        return 0;
    }
    Worker *w = worker_new(destination);
    if (!w) {
        reader_close(r);
        return 0;
    }
    int terminal = 0;
    while (!terminal && !worker_error(w)) {
        const BandFrame *f = reader_peek(r, &terminal);
        if (f) {
            int target = convert_target(f->fmt, writer_div);
            worker_fields(w, f, target,
                          sb_bitrate(layer, target, f->fmt.channels, f->fmt.kbps, 1000), 1, 1, 0);
            reader_advance(r);
        }
        usleep(1500);
    }
    if (terminal < 0)
        fprintf(stderr, "%s\n", reader_error(r));
    reader_close(r);
    Result result = worker_close(w);
    if (result.error)
        fprintf(stderr, "%s\n", result.error);
    return terminal == 1 && !result.error && !result.dropped;
}
int main(int argc, char **argv) {
    sb_tables();
    filter_init();
    if (!convert_tables())
        return 2;
    if (argc == 6 && !strcmp(argv[1], "matrix")) {
        FILE *f = fopen(argv[5], "wb");
        if (!f)
            return 2;
        int pass = matrix(f, atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
        return fclose(f) || !pass;
    }
    if (argc == 7 && !strcmp(argv[1], "transport"))
        return !transport(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), argv[5], argv[6]);
    return 2;
}
