#include "worker.h"
#include <dispatch/dispatch.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
enum { QUEUE = 32 };
typedef struct {
    union {
        BandFrame fields;
        BandAudio audio;
    } data;
    Format fmt;
    int from_audio, dest, kbps, keep, listen;
    unsigned epoch;
} Job;
typedef struct {
    Packet p;
    unsigned epoch;
} Reply;
struct Worker {
    Job jobs[QUEUE];
    Reply results[QUEUE];
    _Atomic unsigned in_read, in_write, out_read, out_write, drops;
    _Atomic int stop;
    _Atomic(const char *) error;
    dispatch_semaphore_t wake;
    pthread_t thread;
    FILE *file;
    char *path, *temporary;
    Converter convert;
    Format source;
    BandFrame analyzed, converted;
    Packet packed;
    Padding padding;
    unsigned epoch;
    int dest, kbps, listen, group_pos, rate_fifo[16];
    unsigned rate_in, rate_out;
    Result stats;
};
static void deliver(Worker *w, const BandFrame *f, int kbps, int keep) {
    Packet *p = &w->packed;
    if (!sb_pack(f, kbps, keep, &w->padding, p)) {
        atomic_store(&w->error, "cannot fit frame at this format");
        return;
    }
    if (w->file && fwrite(p->data, 1, p->length, w->file) != (size_t)p->length) {
        atomic_store(&w->error, "could not write recording");
        return;
    }
    w->stats.frames++;
    w->stats.fitted += p->fitted;
    w->stats.hz = f->fmt.hz;
    w->stats.channels = f->fmt.channels;
    w->stats.kbps = p->fields.fmt.kbps;
    if (w->listen) {
        unsigned wr = atomic_load(&w->out_write);
        if (wr - atomic_load(&w->out_read) < QUEUE) {
            Reply *reply = &w->results[wr % QUEUE];
            reply->p = *p;
            reply->epoch = w->epoch;
            atomic_store(&w->out_write, wr + 1);
        } else
            atomic_fetch_add(&w->drops, 1);
    }
}
/* Only the worker mutates the padding accumulator, converter and file.
   A generation change discards pending preview state at a format boundary. */
static void consume(Worker *w, const Job *j) {
    Format f = j->fmt;
    int reset = w->epoch != j->epoch || w->source.hz != f.hz || w->source.channels != f.channels ||
                w->dest != j->dest;
    if (reset) {
        if (w->file && w->stats.frames) {
            atomic_store(&w->error, "recording format changed");
            return;
        }
        w->padding = (Padding){0};
        w->epoch = j->epoch;
        w->source = f;
        w->dest = j->dest;
        convert_start(&w->convert, f, j->dest);
        w->rate_in = w->rate_out = 0;
        w->group_pos = 0;
    }
    w->listen = j->listen;
    w->kbps = j->kbps;
    const BandFrame *fields;
    if (j->from_audio) {
        sb_quantize(&j->data.audio, f, &w->analyzed);
        fields = &w->analyzed;
    } else
        fields = &j->data.fields;
    if (j->dest == f.hz) {
        deliver(w, fields, j->kbps, j->keep);
        return;
    }
    if (!w->group_pos)
        w->rate_fifo[w->rate_in++ % 16] = j->kbps;
    if (++w->group_pos == w->convert.factor)
        w->group_pos = 0;
    if (convert_frame(&w->convert, fields, &w->converted))
        deliver(w, &w->converted, w->rate_fifo[w->rate_out++ % 16], 0);
}
static void *run(void *arg) {
    Worker *w = arg;
    for (;;) {
        dispatch_semaphore_wait(w->wake, DISPATCH_TIME_FOREVER);
        unsigned r = atomic_load(&w->in_read);
        while (r != atomic_load(&w->in_write)) {
            if (!atomic_load(&w->error))
                consume(w, &w->jobs[r % QUEUE]);
            atomic_store(&w->in_read, ++r);
        }
        if (atomic_load(&w->stop) && r == atomic_load(&w->in_write))
            break;
    }
    while (!atomic_load(&w->error) && convert_finish(&w->convert, &w->converted))
        deliver(w, &w->converted, w->rate_fifo[w->rate_out++ % 16], 0);
    return NULL;
}
static void dispose(Worker *w) {
    if (w->file)
        fclose(w->file);
    if (w->temporary)
        unlink(w->temporary);
    free(w->path);
    free(w->temporary);
    if (w->wake)
        dispatch_release(w->wake);
    free(w);
}
Worker *worker_new(const char *path) {
    Worker *w = calloc(1, sizeof(*w));
    if (!w)
        return NULL;
    w->wake = dispatch_semaphore_create(0);
    if (path) {
        w->path = strdup(path);
        size_t n = strlen(path) + 16;
        w->temporary = malloc(n);
        if (!w->path || !w->temporary) {
            free(w->temporary);
            w->temporary = NULL;
            dispose(w);
            return NULL;
        }
        snprintf(w->temporary, n, "%s.XXXXXX", path);
        int fd = mkstemp(w->temporary);
        if (fd < 0) {
            free(w->temporary);
            w->temporary = NULL;
            dispose(w);
            return NULL;
        }
        if (!(w->file = fdopen(fd, "wb"))) {
            close(fd);
            dispose(w);
            return NULL;
        }
    }
    if (!w->wake || pthread_create(&w->thread, NULL, run, w)) {
        dispose(w);
        return NULL;
    }
    return w;
}
static Job *offer(Worker *w, unsigned *next) {
    if (!w || atomic_load(&w->error))
        return NULL;
    *next = atomic_load(&w->in_write);
    if (*next - atomic_load(&w->in_read) < QUEUE)
        return &w->jobs[*next % QUEUE];
    atomic_fetch_add(&w->drops, 1);
    if (w->file)
        atomic_store(&w->error, "writer queue exhausted");
    return NULL;
}
static void post(Worker *w, unsigned n) {
    atomic_store(&w->in_write, n + 1);
    dispatch_semaphore_signal(w->wake);
}
void worker_fields(Worker *w, const BandFrame *f, int dest, int kbps, int keep, unsigned epoch,
                   int listen) {
    unsigned n;
    Job *j = offer(w, &n);
    if (!j)
        return;
    j->data.fields = *f;
    j->fmt = f->fmt;
    j->dest = dest;
    j->kbps = kbps;
    j->keep = keep;
    j->epoch = epoch;
    j->listen = listen;
    j->from_audio = 0;
    post(w, n);
}
void worker_audio(Worker *w, const BandAudio *a, Format f, unsigned epoch) {
    unsigned n;
    Job *j = offer(w, &n);
    if (!j)
        return;
    j->data.audio = *a;
    j->fmt = f;
    j->dest = f.hz;
    j->kbps = f.kbps;
    j->keep = 0;
    j->epoch = epoch;
    j->listen = 1;
    j->from_audio = 1;
    post(w, n);
}
int worker_pop(Worker *w, Packet *p, unsigned *epoch) {
    if (!w)
        return 0;
    unsigned rd = atomic_load(&w->out_read);
    if (rd == atomic_load(&w->out_write))
        return 0;
    Reply *r = &w->results[rd % QUEUE];
    *p = r->p;
    *epoch = r->epoch;
    atomic_store(&w->out_read, rd + 1);
    return 1;
}
const char *worker_error(Worker *w) {
    return w ? atomic_load(&w->error) : NULL;
}
unsigned worker_drops(Worker *w) {
    return w ? atomic_load(&w->drops) : 0;
}
Result worker_close(Worker *w) {
    if (!w)
        return (Result){0};
    atomic_store(&w->stop, 1);
    dispatch_semaphore_signal(w->wake);
    pthread_join(w->thread, NULL);
    Result result = w->stats;
    result.dropped = atomic_load(&w->drops);
    result.error = atomic_load(&w->error);
    if (w->file) {
        if (fclose(w->file))
            result.error = "could not close recording";
        w->file = NULL;
        if (!result.error && !result.frames)
            result.error = "no complete frames recorded";
        if (!result.error && rename(w->temporary, w->path))
            result.error = "could not finish recording";
    }
    dispose(w);
    return result;
}
