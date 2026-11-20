#ifndef MP3PATCH_DOWNSAMPLE_H
#define MP3PATCH_DOWNSAMPLE_H
#include "ext.h"
#include "codec_clock.h"
#include <stdatomic.h>
#include <sched.h>
static inline int downsample_value(long argc, const t_atom *argv) {
    if (argc != 1 || (atom_gettype(argv) != A_LONG && atom_gettype(argv) != A_FLOAT))
        return -1;
    return codec_factor(atom_getfloat(argv));
}
/* Strip the optional trailing creation setting, leaving original positional arguments intact. */
static inline int downsample_args(long *argc, t_atom *argv) {
    if (*argc >= 2 && atom_gettype(argv + *argc - 2) == A_SYM &&
        atom_getsym(argv + *argc - 2) == gensym("@downsample")) {
        int factor = downsample_value(1, argv + *argc - 1);
        *argc -= 2;
        if (factor < 0)
            error("@downsample expects a nonnegative whole-number divisor");
        return factor;
    }
    return 1;
}
static inline int downsample_message(t_object *x, long argc, t_atom *argv) {
    int factor = downsample_value(argc, argv);
    if (factor < 0)
        object_error(x, "downsample expects a nonnegative whole-number divisor");
    return factor;
}
static inline int downsample_hz(double host, int factor) {
    CodecClock clock;
    return codec_clock_setup_divided(&clock, host, factor) ? clock.hz : 0;
}
static inline void downsample_quiesce(_Atomic int *change, _Atomic unsigned *readers) {
    atomic_store(change, 1);
    while (atomic_load(readers))
        sched_yield();
}
static inline void downsample_status(void *out, double host, int native, int channels) {
    t_atom a[3];
    atom_setfloat(a, host);
    atom_setlong(a + 1, native);
    atom_setlong(a + 2, channels);
    outlet_anything(out, gensym("rate"), 3, a);
}
#endif
