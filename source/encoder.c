#include "encoder.h"
#include "vendor/lame.h"
#include <dispatch/dispatch.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

enum { QUEUE = 16 };
typedef struct {
    float l[1152], r[1152];
    int bitrate;
} Input;
struct Encoder {
    lame_t lame;
    pthread_t thread;
    dispatch_semaphore_t wake;
    _Atomic int running;
    _Atomic unsigned iw, ir, ow, orr, dropouts;
    Input in[QUEUE];
    EncodedFrame out[QUEUE];
    unsigned char pending[16384];
    int count, samples, pos, playing, primed, block_reserved;
    int next_bitrate, worker_bitrate;
};
static void collect(Encoder *e, unsigned char *buf, int n) {
    if (n < 0 || n > (int)sizeof(e->pending) - e->count) {
        atomic_fetch_add(&e->dropouts, 1);
        e->count = 0;
        return;
    }
    memcpy(e->pending + e->count, buf, n);
    e->count += n;
    int pos = 0;
    while (e->count - pos >= 4) {
        int len = layer3_frame_length(e->pending + pos);
        if (!len) {
            ++pos;
            continue;
        }
        if (e->count - pos < len)
            break;
        unsigned w = atomic_load(&e->ow);
        if (w - atomic_load(&e->orr) < QUEUE &&
            layer3_unpack(&e->out[w % QUEUE], e->pending + pos, len))
            atomic_store(&e->ow, w + 1);
        else
            atomic_fetch_add(&e->dropouts, 1);
        pos += len;
    }
    if (pos) {
        e->count -= pos;
        memmove(e->pending, e->pending + pos, e->count);
    }
}
static void *encode_loop(void *arg) {
    Encoder *e = arg;
    unsigned char bytes[8640];
    while (atomic_load(&e->running)) {
        dispatch_semaphore_wait(e->wake, DISPATCH_TIME_FOREVER);
        unsigned r = atomic_load(&e->ir);
        while (atomic_load(&e->running) && r != atomic_load(&e->iw)) {
            Input *p = &e->in[r % QUEUE];
            if (p->bitrate != e->worker_bitrate) {
                if (lame_set_frame_bitrate(e->lame, p->bitrate) == 0)
                    e->worker_bitrate = p->bitrate;
                else
                    atomic_fetch_add(&e->dropouts, 1);
            }
            int n = lame_encode_buffer_ieee_float(e->lame, p->l, p->r, e->samples, bytes,
                                                  sizeof(bytes));
            collect(e, bytes, n);
            atomic_store(&e->ir, ++r);
        }
    }
    return NULL;
}
Encoder *encoder_new(int rate, int channels, int bitrate) {
    Encoder *e = calloc(1, sizeof(*e));
    if (!e)
        return NULL;
    e->lame = lame_init();
    if (!e->lame) {
        free(e);
        return NULL;
    }
    lame_t l = e->lame;
    lame_set_num_channels(l, channels);
    lame_set_mode(l, channels == 1 ? MONO : STEREO);
    lame_set_in_samplerate(l, rate);
    lame_set_out_samplerate(l, rate);
    lame_set_brate(l, bitrate);
    lame_set_VBR(l, vbr_off);
    lame_set_quality(l, 3);
    lame_set_disable_reservoir(l, 1);
    lame_set_bWriteVbrTag(l, 0);
    lame_set_write_id3tag_automatic(l, 0);
    if (lame_init_params(l) < 0 || lame_get_out_samplerate(l) != rate ||
        lame_get_brate(l) != bitrate || bitrate * 1000 > rate * 8) {
        lame_close(l);
        free(e);
        return NULL;
    }
    e->worker_bitrate = e->next_bitrate = bitrate;
    e->samples = lame_get_framesize(l);
    if (e->samples != 576 && e->samples != 1152) {
        lame_close(l);
        free(e);
        return NULL;
    }
    e->wake = dispatch_semaphore_create(0);
    if (!e->wake) {
        lame_close(l);
        free(e);
        return NULL;
    }
    atomic_store(&e->running, 1);
    if (pthread_create(&e->thread, NULL, encode_loop, e)) {
        dispatch_release(e->wake);
        lame_close(l);
        free(e);
        return NULL;
    }
    return e;
}
void encoder_free(Encoder *e) {
    if (!e)
        return;
    atomic_store(&e->running, 0);
    dispatch_semaphore_signal(e->wake);
    pthread_join(e->thread, NULL);
    dispatch_release(e->wake);
    lame_close(e->lame);
    free(e);
}
void encoder_set_bitrate(Encoder *e, int kbps) { e->next_bitrate = kbps; }
void encoder_feed(Encoder *e, float left, float right) {
    unsigned w = atomic_load(&e->iw);
    if (!e->pos) {
        e->block_reserved = (w - atomic_load(&e->ir) < QUEUE);
        if (e->block_reserved)
            e->in[w % QUEUE].bitrate = e->next_bitrate;
    }
    if (e->block_reserved) {
        e->in[w % QUEUE].l[e->pos] = left;
        e->in[w % QUEUE].r[e->pos] = right;
    }
    if (++e->pos == e->samples) {
        e->pos = 0;
        if (e->block_reserved) {
            atomic_store(&e->iw, w + 1);
            dispatch_semaphore_signal(e->wake);
        } else
            atomic_fetch_add(&e->dropouts, 1);
    }
}
const EncodedFrame *encoder_next(Encoder *e) {
    unsigned r = atomic_load(&e->orr);
    if (e->playing) {
        atomic_store(&e->orr, ++r);
        e->playing = 0;
    }
    unsigned available = atomic_load(&e->ow) - r;
    if (e->primed && !available) {
        e->primed = 0;
        atomic_fetch_add(&e->dropouts, 1);
    }
    if (!e->primed && available < 2)
        return NULL;
    e->primed = e->playing = 1;
    return &e->out[r % QUEUE];
}
unsigned encoder_dropouts(Encoder *e) { return e ? atomic_load(&e->dropouts) : 0; }
