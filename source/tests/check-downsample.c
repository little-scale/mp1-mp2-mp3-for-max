#include "../byte_rate.h"
#include "../field_file.h"
#include "../pack_writer.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>

static void check_clocks(void) {
    const int cases[][3] = {{48000, 1, 48000}, {48000, 2, 24000}, {48000, 3, 16000},
                            {48000, 4, 12000}, {48000, 6, 8000},  {44100, 2, 22050},
                            {44100, 3, 22050}, {44100, 4, 11025}, {32000, 4, 8000},
                            {96000, 2, 24000}, {88200, 2, 22050}, {192000, 6, 8000},
                            {50000, 2, 24000}, {8000, 6, 8000},   {1000, 2, 8000}};
    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        CodecClock clock;
        assert(codec_clock_setup_divided(&clock, cases[i][0], cases[i][1]));
        assert(clock.hz == cases[i][2]);
        int ticks = 0;
        for (int n = 0; n <= cases[i][0]; ++n)
            ticks += codec_ticks(&clock);
        assert(ticks == 1 + clock.hz);
    }
    assert(codec_factor(0) == 1 && codec_factor(1) == 1 && codec_factor(100) == 6);
    assert(codec_factor(-1) < 0 && codec_factor(1.5) < 0 && codec_factor(NAN) < 0);
    assert(byte_target(24000, 24000, 2) == 24000);
    assert(byte_target(44100, 24000, 2) == 22050);
    assert(byte_target(48000, 24000, 1) == 48000);
}

static long copy_frames(FieldFile *file, const char *path, int output, int kbps) {
    int native = field_file_rate(file);
    PackWriter *w = writer_open(path, kbps, 1);
    assert(w);
    long granules = 0;
    struct timespec nap = {.tv_nsec = 2000000};
    int status;
    for (;;) {
        const EncodedFrame *f = field_file_peek(file, &status);
        if (f) {
            assert(f->rate == native && f->channels == field_file_channels(file));
            granules += f->granules;
            writer_offer_source(w, native, output, f->channels, f->fields, 0, kbps, 1);
            field_file_advance(file);
        } else if (status)
            break;
        nanosleep(&nap, NULL);
    }
    PackResult result = writer_close(w);
    assert(status == 1 && !result.error && result.format.rate == output);
    assert(result.format.frames == (granules + native / output - 1) / (native / output));
    return result.format.frames;
}
int main(int argc, char **argv) {
    if (argc != 7)
        return 2;
    check_clocks();
    layer3_tables();
    const char *reason = NULL;
    int factor = atoi(argv[2]), kbps = atoi(argv[3]);
    FieldFile *source = field_file_open(argv[1], &reason);
    FieldFile *converted = field_file_open_at(argv[1], factor, &reason);
    if (!source || !converted) {
        fprintf(stderr, "%s\n", reason);
        return 1;
    }
    int dest = field_file_rate(converted);
    assert(field_file_rate(source) == field_file_source_rate(converted));
    long frames = copy_frames(source, argv[4], dest, kbps);
    assert(copy_frames(converted, argv[5], dest, kbps) == frames);
    assert(field_file_rewind(converted, &reason));
    assert(copy_frames(converted, argv[6], dest, kbps) == frames);
    field_file_close(source);
    field_file_close(converted);
    return 0;
}
