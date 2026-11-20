/* Build with the Max SDK using check-packed-monitor.py. Drive the production
   writer and the packer's real monitor path independently of the Max scheduler. */
#include "../mp3pack.c"
#include <time.h>

static int setup_test_rate(External *x, double host, int ch) {
    x->channels = ch;
    if (!rate_setup(&x->playback, host))
        return 0;
    return x->playback.factor >= 0 || fractional_setup(&x->converter, x->playback.hz, host);
}
static void serve_reference(External *x, FILE *src, FILE *dst, int n) {
    Rate *r = &x->playback;
    for (int i = 0; i < n; i++) {
        int ticks = codec_ticks(&r->clock);
        float pair[2] = {0};
        for (int k = 0; k < ticks; k++) {
            if (fread(pair, sizeof(float), x->channels, src) != (size_t)x->channels)
                pair[0] = pair[1] = 0;
            if (x->channels == 1)
                pair[1] = pair[0];
            if (r->factor < 0)
                fractional_push(&x->converter, pair[0], pair[1]);
        }
        if (r->factor > 1)
            rate_push(r, pair[0], pair[1]);
        for (int ch = 0; ch < x->channels; ch++) {
            float v = r->factor == 1 ? pair[ch]
                      : r->factor > 1
                          ? r->factor * rate_filter(r, ch)
                          : fractional_read(&x->converter, ch, -r->clock.phase / r->clock.host);
            fwrite(&v, sizeof(v), 1, dst);
        }
    }
}
int main(int ac, char **av) {
    if (ac != 7)
        return 2;
    External x = {0};
    int ch = atoi(av[3]), skip = atoi(av[4]);
    double host = atof(av[2]);
    if (!setup_test_rate(&x, host, ch))
        return 3;
    int hz = x.playback.hz, ng = hz >= 32000 ? 2 : 1, hop = ng * 576;
    int samples = (int)ceil((18 * hop + 128.) * host / hz);
    FILE *out = fopen(av[6], "wb");
    if (!out)
        return 3;
    if (!strcmp(av[1], "reference")) {
        FILE *src = fopen(av[5], "rb");
        if (!src)
            return 4;
        serve_reference(&x, src, out, samples);
        fclose(src);
    } else {
        PackMonitor *m = x.monitor = calloc(1, sizeof(*m));
        PackWriter *w = writer_open(av[5], 0, 1);
        if (!m || !w)
            return 5;
        layer3_tables();
        writer_attach_monitor(w, m);
        Fields f[2][2] = {0};
        for (int i = 0; i < 12; i++) {
            if (i == skip)
                monitor_enable(m, 1);
            for (int gr = 0; gr < ng; gr++)
                for (int c = 0; c < ch; c++) {
                    Fields *v = &f[gr][c];
                    v->gain = 165;
                    v->q[c * 36] = 10 + 3 * i + gr;
                    /* Leave a short block open to exercise frames generated at stop. */
                    v->window = i == 10 && !gr ? 1 : i >= 10 ? 2 : 0;
                }
            writer_offer(w, hz, ch, f, 0);
            struct timespec nap = {0, 1000000};
            for (int t = 0; t < 2000 && atomic_load(&m->written) < (unsigned)(i + 1); t++)
                nanosleep(&nap, NULL);
            if (atomic_load(&m->written) != (unsigned)(i + 1))
                return 6;
        }
        PackResult r = writer_close(w);
        if (r.error || r.format.frames <= 12 || atomic_load(&m->written) != r.format.frames ||
            atomic_load(&m->drops) || atomic_load(&m->fitted))
            return 7;
        double pair[2], *outs[] = {pair, pair + 1};
        for (int i = 0; i < samples; i++) {
            audition(&x, atomic_load(&m->epoch), outs, 0);
            for (int c = 0; c < ch; c++) {
                float v = (float)pair[c];
                fwrite(&v, sizeof(v), 1, out);
            }
        }
        if (atomic_load(&m->rd) != atomic_load(&m->wr))
            return 8;
        free(m);
    }
    fractional_free(&x.converter);
    return fclose(out);
}
