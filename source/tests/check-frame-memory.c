/* Compile with the Max SDK include paths and -Wl,-dead_strip,-undefined,dynamic_lookup. */
#include "../mp3frames.c"
#include <assert.h>
static void check_memory(int ng) {
    Memory x = {0};
    x.capacity = 3;
    x.channels = 2;
    x.ring = calloc(4, sizeof(Frame));
    atomic_store(&x.record, 1);
    FieldBus bus = {.width = {64, 64, 2, 2, 1, 64},
                    .offset = {0, 64, 128, 130, 132, 133},
                    .connected = {1, 1, 1, 1, 0, 1}};
    x.bus = bus;
    enum { INS = 197, OUTS = 135, V = 32 };
    double iv[INS][V] = {0}, ov[OUTS][V], *in[INS], *out[OUTS];
    for (int i = 0; i < INS; i++)
        in[i] = iv[i];
    for (int i = 0; i < OUTS; i++)
        out[i] = ov[i];
    codec_clock_setup(&x.clock, ng == 2 ? 48000 : 24000);
    int saved[3] = {0}, used = 0;
    for (int f = 0; f < 22; f++) {
        int writing = f < 9 || f >= 15;
        atomic_store(&x.record, writing);
        if (f == 14) {
            clear(&x);
            used = 0;
        }
        int picked[64];
        for (int lane = 0; lane < 64; lane++) {
            double age = f < 4       ? 0
                         : f == 6    ? 2
                         : f == 7    ? 200
                         : f == 8    ? -1
                         : lane == 0 ? NAN
                                     : lane % 4 + 0.1;
            in[133 + lane][0] = age;
            int a = isfinite(age) ? (int)round(fmin(used, fmax(0, age))) : 0;
            picked[lane] = a ? saved[a - 1] : f;
        }
        for (int g = 0; g < ng; g++)
            for (int row = 0; row < 18; row++) {
                for (int i = 0; i < V; i++) {
                    for (int lane = 0; lane < 64; lane++) {
                        in[lane][i] = 3 + f * 40 + g + lane + row;
                        in[64 + lane][i] = 2 * f + g + lane % 3;
                        if (i || row || g)
                            in[133 + lane][i] = 100;
                    }
                    for (int c = 0; c < 2; c++) {
                        in[128 + c][i] = 140 + f + g + c;
                        in[130 + c][i] = (f + g + c) % 5;
                    }
                }
                perform(&x, NULL, in, INS, out, OUTS, V, 0, NULL);
                for (int lane = 0; lane < 64; lane++) {
                    int c = lane / 32, src = picked[lane], uniform = 1;
                    for (int b = 0; b < 32; b++)
                        uniform &= picked[c * 32 + b] == picked[c * 32];
                    assert(out[lane][0] == 3 + src * 40 + g + lane + row);
                    assert(out[128 + c][0] - out[64 + lane][0] ==
                           140 + src + g + c - (2 * src + g + lane % 3));
                    assert(out[130 + c][0] == ((uniform ? picked[c * 32] : f) + g + c) % 5);
                }
                for (int i = 0; i < V; i++) {
                    assert(out[132][i] == (!i && !row));
                    assert(out[133][i] == row);
                    assert(out[134][i] == (!i && !row && !g));
                }
            }
        if (writing) {
            for (int i = 2; i > 0; i--)
                saved[i] = saved[i - 1];
            saved[0] = f;
            if (used < 3)
                ++used;
        }
    }
    free(x.ring);
}
int main(void) {
    check_memory(1);
    check_memory(2);
    return 0;
}
