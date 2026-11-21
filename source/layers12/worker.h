#ifndef MPEG_SUBBAND_WORKER_H
#define MPEG_SUBBAND_WORKER_H
#include "convert.h"
typedef struct Worker Worker;
typedef struct {
    unsigned long frames, fitted, dropped;
    int hz, channels, kbps;
    const char *error;
} Result;
/* Each worker has one DSP producer. Disk writes and bit allocation run on its thread. */
Worker *worker_new(const char *destination);
void worker_fields(Worker *w, const BandFrame *f, int dst, int kbps, int keep, unsigned epoch,
                   int listen);
void worker_audio(Worker *w, const BandAudio *a, Format fmt, unsigned epoch);
int worker_pop(Worker *w, Packet *p, unsigned *epoch);
Result worker_close(Worker *w);
const char *worker_error(Worker *w);
unsigned worker_drops(Worker *w);
#endif
