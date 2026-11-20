#include "pack.h"
#include "field_bus.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
static unsigned rng = 25;
static unsigned next(void) {
    rng = 1664525 * rng + 1013904223;
    return rng;
}
int main(int ac, char **av) {
    if (ac != 5)
        return 2;
    int hz = atoi(av[1]), ch = atoi(av[2]), br = atoi(av[3]);
    Pack p;
    if (!pack_init(&p, hz, ch, br))
        return 3;
    FILE *fp = fopen(av[4], "wb");
    Fields fields[2][2];
    unsigned limited = 0;
    unsigned char bytes[L3_FRAME_MAX];
    int prev[2] = {0};
    for (int step = 0; step < 26; step++) {
        for (int g = 0; g < (hz >= 32000 ? 2 : 1); g++)
            for (int c = 0; c < ch; c++) {
                Fields *f = &fields[g][c];
                f->gain = bus_limit((double[]){NAN, 999., -1e30, 220., 170., 255.}[step % 6], 0,
                                    255, &limited);
                f->window = bus_limit((int[]){0, 1, 2, 2, 4, 5, 3, 0, 5, 8, -1, 1, 4}[step % 13], 0,
                                      5, &limited);
                for (int i = 0; i < 576; i++) {
                    unsigned r = next();
                    double in = (double[]){NAN,  INFINITY, -INFINITY,
                                           1e20, -1e20,    0.,
                                           -1.,  1.,       (int)(r % 16413) - 8206}[step % 9];
                    double att = (step % 4 == 0)   ? 0
                                 : (step % 4 == 1) ? 1000
                                 : (step % 4 == 2) ? r % 211
                                                   : -INFINITY;
                    f->q[i] = bus_limit(in, -8206, 8206, &limited);
                    f->attenuation[i] = bus_limit(att, 0, 210, &limited);
                }
            }
        int n = pack_frame(&p, fields, bytes);
        if (!n || n != layer3_frame_length(bytes) || fwrite(bytes, 1, n, fp) != (size_t)n)
            return 4;
        EncodedFrame f;
        if (!layer3_unpack(&f, bytes, n))
            return 5;
        for (int g = 0; g < f.granules; g++)
            for (int c = 0; c < ch; c++) {
                int w = f.fields[g][c].window, old = prev[c];
                if (w >= 4)
                    w = 2;
                if ((old == 1 && w != 2) || (old == 2 && w != 2 && w != 3) ||
                    ((old == 0 || old == 3) && w != 0 && w != 1))
                    return 6;
                prev[c] = w;
            }
    }
    if (!limited || !p.fitted || !p.windows)
        return 7;
    fclose(fp);
    return 0;
}
