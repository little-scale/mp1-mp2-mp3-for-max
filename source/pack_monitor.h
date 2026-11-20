#ifndef MP3PATCH_PACK_MONITOR_H
#define MP3PATCH_PACK_MONITOR_H
#include "layer3.h"
#include <stdatomic.h>
enum { MONITOR_SLOTS = 32 };
typedef struct {
    float pcm[2][1152];
    int hz, channels, count;
    unsigned epoch;
} MonitorFrame;
/* The file worker produces frames. MSP alone advances rd, pos and wait. */
typedef struct {
    MonitorFrame frames[MONITOR_SLOTS];
    _Atomic unsigned rd, wr, epoch;
    _Atomic unsigned long written, fitted, windows, drops;
    int pos, wait, playing;
    unsigned seen;
} PackMonitor;
void monitor_enable(PackMonitor *m, int enable);
void monitor_tick(PackMonitor *m, unsigned epoch, int hz, int ch, float pair[2]);
#endif
