#ifndef MPEG_BAND_INPUT_H
#define MPEG_BAND_INPUT_H
#include "max_common.h"

/* Addresses are per source lane; allocation and scale are per destination.
   Ascending traversal makes the last lane win an address collision. */
static inline uint32_t route_band_words(const Bus *bus, double **in, int at, double words[32]) {
    if (!bus->connected[0])
        return 0;
    int lanes = bus->width[0];
    if (lanes == 1 || lanes > 32)
        lanes = 32;
    uint32_t written = 0;
    for (int i = 0; i < lanes; ++i) {
        double address = lane(bus, in, 1, 0, i, at, -1);
        int band = i;
        if (isfinite(address) && address >= 0) {
            int index = address >= 31 ? 31 : (int)address;
            band = bus->width[1] == 1 ? (i + index) % 32 : index;
        }
        words[band] = lane(bus, in, 0, 0, i, at, 0);
        written |= UINT32_C(1) << band;
    }
    return written;
}
#endif
