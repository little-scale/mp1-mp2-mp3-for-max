#include "pack_writer.h"
#include "bitrate.h"
#include "spectral_rate.h"
#include <dispatch/dispatch.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
enum { SLOTS = 64, WRITER_OVERFLOW = 1, FORMAT, IO, EMPTY, CHANGED, CONVERSION };
typedef struct {
    Fields fields[2][2];
    int hz, target_hz, channels, bitrate, file_windows;
    unsigned limited;
} Job;
typedef struct {
    unsigned char data[L3_FRAME_MAX];
    int len;
} Packet;
struct PackWriter {
    PackMonitor *monitor;
    Synthesis *synth;
    SpectralFrame spectral;
    MonitorFrame scratch;
    Packet prior[2];
    unsigned history_write, monitor_epoch;
    int history_count;
    Job ring[SLOTS];
    SpectralRate *conversion;
    Fields converted[2][2];
    int source_hz, group_pos, pending_rates[8];
    unsigned rates_in, rates_out;
    _Atomic unsigned rd, wr;
    _Atomic int stop, error;
    FILE *file;
    char *destination, *temporary;
    dispatch_semaphore_t wake;
    pthread_t thread;
    PackResult result;
    unsigned long id;
    int bitrate;
};
const char *writer_error(int code) {
    static const char *message[] = {"ok",
                                    "writer queue exhausted",
                                    "bitrate unavailable at this MP3 rate",
                                    "could not finish writing the file",
                                    "no complete frames recorded",
                                    "stream format changed",
                                    "could not initialize spectral rate conversion"};
    return message[code];
}
void monitor_enable(PackMonitor *m, int enable) {
    unsigned e = atomic_load(&m->epoch);
    while ((e & 1u) != (unsigned)(enable != 0) &&
           !atomic_compare_exchange_weak(&m->epoch, &e, e + 1)) {
    }
}
void monitor_tick(PackMonitor *m, unsigned epoch, int hz, int ch, float pair[2]) {
    pair[0] = pair[1] = 0;
    unsigned rd = atomic_load(&m->rd), wr = atomic_load(&m->wr);
    if (m->seen != epoch) {
        m->seen = epoch;
        m->pos = m->wait = m->playing = 0;
    }
    while (rd != wr) {
        MonitorFrame *f = &m->frames[rd % MONITOR_SLOTS];
        if (f->epoch == epoch && (epoch & 1u) && f->hz == hz && f->channels == ch) {
            if (!m->playing) {
                if (++m->wait <= f->count)
                    return;
                m->playing = 1;
            }
            pair[0] = f->pcm[0][m->pos];
            pair[1] = f->pcm[ch - 1][m->pos];
            if (++m->pos == f->count) {
                m->pos = 0;
                atomic_store(&m->rd, rd + 1);
            }
            return;
        }
        m->pos = m->wait = m->playing = 0;
        atomic_store(&m->rd, ++rd);
    }
    m->pos = m->wait = m->playing = 0;
}
static int render_packet(PackWriter *w, MonitorFrame *out, const unsigned char *bytes, int n) {
    SpectralFrame *f = &w->spectral;
    if (!layer3_read_spectrum(w->synth, bytes, n, w->result.format.rate, f))
        return 0;
    for (int g = 0; g < f->granules; ++g)
        for (int c = 0; c < f->channels; ++c)
            layer3_render_spectrum(w->synth, &f->gr[g][c], c, out->pcm[c] + 576 * g);
    out->count = 576 * f->granules;
    out->channels = f->channels;
    out->hz = w->result.format.rate;
    return 1;
}
static void publish(PackWriter *w, const unsigned char *bytes, int n) {
    PackMonitor *m = w->monitor;
    if (!m)
        return;
    Pack *p = &w->result.format;
    atomic_store(&m->fitted, p->fitted);
    atomic_store(&m->windows, p->windows);
    atomic_store(&m->written, p->frames);
    unsigned epoch = atomic_load(&m->epoch);
    if (epoch & 1u) {
        if (w->monitor_epoch != epoch) {
            layer3_clear(w->synth);
            /* Independent packets: two prior frames recover both filter histories
               even at the one-granule MPEG-2/2.5 rates. */
            for (int k = w->history_count; k > 0; --k) {
                Packet *old = &w->prior[(w->history_write - k) % 2];
                render_packet(w, &w->scratch, old->data, old->len);
            }
            w->monitor_epoch = epoch;
        }
        unsigned wr = atomic_load(&m->wr);
        int room = wr - atomic_load(&m->rd) < MONITOR_SLOTS;
        MonitorFrame *f = room ? &m->frames[wr % MONITOR_SLOTS] : &w->scratch;
        if (render_packet(w, f, bytes, n) && room) {
            f->epoch = epoch;
            atomic_store(&m->wr, wr + 1);
        } else
            atomic_fetch_add(&m->drops, 1);
    }
    Packet *next = &w->prior[w->history_write++ % 2];
    memcpy(next->data, bytes, n);
    next->len = n;
    if (w->history_count < 2)
        ++w->history_count;
}
static void write_frame(PackWriter *w, const Fields fields[2][2], int kbps, int file_windows) {
    Pack *p = &w->result.format;
    if (kbps && kbps != p->bitrate)
        p->bitrate = pack_bitrate(p->rate, kbps);
    unsigned char bytes[L3_FRAME_MAX];
    int n = file_windows ? pack_file_frame(p, fields, bytes) : pack_frame(p, fields, bytes);
    if (!n || (w->file && fwrite(bytes, 1, n, w->file) != (size_t)n))
        atomic_store(&w->error, IO);
    else
        publish(w, bytes, n);
}
static void converted_frame(PackWriter *w) {
    /* Conversion looks ahead by less than four output granules. Keep each
       frame's bitrate aligned with the start of its original source interval. */
    write_frame(w, w->converted, w->pending_rates[w->rates_out++ % 8], 0);
}
static void consume(PackWriter *w, const Job *j) {
    Pack *p = &w->result.format;
    if (!p->rate) {
        if (!pack_init(p, j->target_hz, j->channels, j->bitrate)) {
            atomic_store(&w->error, FORMAT);
            return;
        }
        w->source_hz = j->hz;
        if (j->hz != j->target_hz) {
            w->conversion = spectral_rate_new(j->hz, j->target_hz, j->channels);
            if (!w->conversion) {
                atomic_store(&w->error, CONVERSION);
                return;
            }
        }
    } else if (w->source_hz != j->hz || j->target_hz != p->rate || j->channels != p->channels) {
        atomic_store(&w->error, CHANGED);
        return;
    }
    w->result.limited += j->limited;
    if (!w->conversion) {
        write_frame(w, j->fields, j->bitrate, j->file_windows);
        return;
    }
    int ng = j->hz >= 32000 ? 2 : 1, divisor = j->hz / j->target_hz;
    for (int g = 0; g < ng; ++g) {
        if (!w->group_pos)
            w->pending_rates[w->rates_in++ % 8] = j->bitrate;
        if (++w->group_pos == divisor)
            w->group_pos = 0;
        if (spectral_rate_fields(w->conversion, j->fields[g], w->converted[0]))
            converted_frame(w);
    }
}
static void *drain(void *arg) {
    PackWriter *w = arg;
    for (;;) {
        dispatch_semaphore_wait(w->wake, DISPATCH_TIME_FOREVER);
        unsigned r = atomic_load(&w->rd);
        while (r != atomic_load(&w->wr)) {
            Job *j = &w->ring[r % SLOTS];
            if (!atomic_load(&w->error))
                consume(w, j);
            atomic_store(&w->rd, ++r);
        }
        if (atomic_load(&w->stop) && r == atomic_load(&w->wr))
            break;
    }
    while (w->conversion && !atomic_load(&w->error) &&
           spectral_rate_finish(w->conversion, w->converted[0]))
        converted_frame(w);
    return NULL;
}
static void dispose(PackWriter *w) {
    if (w->file)
        fclose(w->file);
    if (w->temporary)
        unlink(w->temporary);
    if (w->wake)
        dispatch_release(w->wake);
    spectral_rate_free(w->conversion);
    layer3_free(w->synth);
    free(w->temporary);
    free(w->destination);
    free(w);
}
static PackWriter *start_worker(PackWriter *w) {
    w->synth = layer3_new();
    w->wake = dispatch_semaphore_create(0);
    if (!w->synth || !w->wake || pthread_create(&w->thread, NULL, drain, w)) {
        dispose(w);
        return NULL;
    }
    return w;
}
PackWriter *writer_preview(PackMonitor *monitor) {
    PackWriter *w = calloc(1, sizeof(*w));
    if (!w)
        return NULL;
    w->monitor = monitor;
    return start_worker(w);
}
PackWriter *writer_open(const char *path, int bitrate, unsigned long id) {
    PackWriter *w = calloc(1, sizeof(*w));
    if (!w)
        return NULL;
    w->destination = strdup(path);
    w->temporary = malloc(strlen(path) + 16);
    if (!w->temporary || !w->destination) {
        free(w->temporary);
        w->temporary = NULL;
        dispose(w);
        return NULL;
    }
    sprintf(w->temporary, "%s.XXXXXX", path);
    int fd = mkstemp(w->temporary);
    if (fd < 0) {
        free(w->temporary);
        w->temporary = NULL;
        dispose(w);
        return NULL;
    }
    w->file = fdopen(fd, "wb");
    if (!w->file) {
        close(fd);
        dispose(w);
        return NULL;
    }
    w->bitrate = bitrate;
    w->id = id;
    return start_worker(w);
}
void writer_attach_monitor(PackWriter *w, PackMonitor *m) {
    w->monitor = m;
    atomic_store(&m->fitted, 0);
    atomic_store(&m->written, 0);
    atomic_store(&m->windows, 0);
    atomic_store(&m->drops, 0);
}
int writer_has_input(PackWriter *w) {
    return w && atomic_load(&w->wr) != 0;
}
unsigned long writer_id(PackWriter *w) { return w ? w->id : 0; }
void writer_offer(PackWriter *w, int hz, int ch, const Fields fields[2][2], unsigned limited) {
    writer_offer_at(w, hz, ch, fields, limited, w->bitrate, 0);
}
void writer_offer_at(PackWriter *w, int hz, int ch, const Fields fields[2][2], unsigned limited,
                     int kbps, int file_windows) {
    writer_offer_source(w, hz, hz, ch, fields, limited, kbps, file_windows);
}
void writer_offer_source(PackWriter *w, int hz, int target_hz, int ch, const Fields fields[2][2],
                         unsigned limited, int kbps, int file_windows) {
    if (atomic_load(&w->error))
        return;
    unsigned next = atomic_load(&w->wr);
    if (next - atomic_load(&w->rd) >= SLOTS) {
        if (w->file)
            atomic_store(&w->error, WRITER_OVERFLOW);
        else
            atomic_fetch_add(&w->monitor->drops, 1);
        return;
    }
    Job *j = &w->ring[next % SLOTS];
    j->bitrate = kbps;
    j->file_windows = file_windows;
    j->hz = hz;
    j->target_hz = target_hz;
    j->channels = ch;
    j->limited = limited;
    int ng = hz >= 32000 ? 2 : 1;
    for (int g = 0; g < ng; g++)
        memcpy(j->fields[g], fields[g], ch * sizeof(Fields));
    atomic_store(&w->wr, next + 1);
    dispatch_semaphore_signal(w->wake);
}
PackResult writer_close(PackWriter *w) {
    atomic_store(&w->stop, 1);
    dispatch_semaphore_signal(w->wake);
    pthread_join(w->thread, NULL);
    PackResult r = w->result;
    r.error = atomic_load(&w->error);
    if (!w->file) {
        dispose(w);
        return r;
    }
    /* Finish an open short-window sequence with legal silent transitions. */
    static const Fields silence[2][2] = {0};
    while (!r.error && (r.format.previous[0] || r.format.previous[1])) {
        unsigned char bytes[L3_FRAME_MAX];
        int n = pack_frame(&w->result.format, silence, bytes);
        r.format = w->result.format;
        if (!n || fwrite(bytes, 1, n, w->file) != (size_t)n)
            r.error = IO;
        else
            publish(w, bytes, n);
    }
    if (fclose(w->file))
        r.error = IO;
    w->file = NULL;
    if (!r.error && !r.format.frames)
        r.error = EMPTY;
    if (!r.error && rename(w->temporary, w->destination))
        r.error = IO;
    dispose(w);
    return r;
}
