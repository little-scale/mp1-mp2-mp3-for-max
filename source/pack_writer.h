#ifndef MP3PATCH_WRITER_H
#define MP3PATCH_WRITER_H
#include "pack.h"
#include "pack_monitor.h"
typedef struct PackWriter PackWriter;
typedef struct {
    Pack format;
    unsigned long limited;
    int error;
} PackResult;
PackWriter *writer_open(const char *path, int bitrate, unsigned long id);
PackWriter *writer_preview(PackMonitor *monitor);
void writer_attach_monitor(PackWriter *w, PackMonitor *m);
int writer_has_input(PackWriter *w);
unsigned long writer_id(PackWriter *w);
void writer_offer(PackWriter *w, int hz, int ch, const Fields fields[2][2], unsigned limited);
void writer_offer_at(PackWriter *w, int hz, int ch, const Fields fields[2][2], unsigned limited,
                     int kbps, int file_windows);
void writer_offer_source(PackWriter *w, int source_hz, int target_hz, int ch,
                         const Fields fields[2][2], unsigned limited, int kbps, int file_windows);
PackResult writer_close(PackWriter *w);
const char *writer_error(int code);
#endif
