#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"
#include "layer1.h"
#include <stdio.h>
#include <math.h>

#define OBJECT_NAME "mp1bands~"

typedef struct {
    t_pxobject object;
    Layer1 decoder;
    short connected[4];
    int raw;
    long channels[4], offset[4];
    double words[32], allocation[32], scale[32];
    uint32_t valid;
} DecoderExternal;

static t_class *decoder_class;

/* A mono field broadcasts. Short multichannel fields have explicit defaults. */
static double lane_value(const DecoderExternal *x, double **ins, int port, int lane, long sample,
                         double fallback) {
    if (!x->connected[port])
        return fallback;
    if (x->channels[port] == 1)
        lane = 0;
    if (lane >= x->channels[port])
        return fallback;
    return ins[x->offset[port] + lane][sample];
}

static void capture_slot(DecoderExternal *x, double **ins, long sample) {
    if (x->decoder.slot == 0) {
        for (int band = 0; band < 32; ++band) {
            x->allocation[band] = lane_value(x, ins, 2, band, sample, 7);
            x->scale[band] = lane_value(x, ins, 3, band, sample, 18);
        }
    }
    x->valid = 0;
    if (!x->connected[0])
        return;
    const int lanes = x->channels[0] == 1 || x->channels[0] >= 32 ? 32 : (int)x->channels[0];
    for (int lane = 0; lane < lanes; ++lane) {
        double address = lane_value(x, ins, 1, lane, sample, -1);
        int band = lane;
        if (isfinite(address) && address >= 0) {
            int index = address >= 31 ? 31 : (int)address;
            band = x->channels[1] == 1 ? (lane + index) % 32 : index;
        }
        x->words[band] = lane_value(x, ins, 0, lane, sample, 0);
        x->valid |= UINT32_C(1) << band;
    }
}

static void perform64(DecoderExternal *x, t_object *chain, double **ins, long nin, double **outs,
                      long nout, long count, long flags, void *user) {
    for (long i = 0; i < count; ++i) {
        int col = x->decoder.column;
        outs[1][i] = col;
        outs[2][i] = col == 0 && x->decoder.slot == 0;
        if (col == 0)
            capture_slot(x, ins, i);
        int written = (x->valid >> col) & 1;
        outs[0][i] = layer1_tick(&x->decoder, written ? x->words[col] : 0, -1, x->allocation[col],
                                 x->scale[col], x->raw, written);
    }
}

static void dsp64(DecoderExternal *x, t_object *chain, short *connected, double rate,
                  long max_vector, long flags) {
    for (int i = 0; i < 4; ++i)
        x->connected[i] = connected[i];
    long offset = 0;
    for (long i = 0; i < 4; ++i) {
        long channels = (long)object_method(chain, gensym("getnuminputchannels"), x, i);
        x->channels[i] = channels > 0 ? channels : 1;
        x->offset[i] = offset;
        offset += x->channels[i];
    }
    layer1_reset(&x->decoder);
    object_method(chain, gensym("dsp_add64"), x, perform64, 0, NULL);
}

static void assist(DecoderExternal *x, void *box, long kind, long port, char *text) {
    static const char *inputs[] = {
        "MC data: channels 1..32 feed bands 0..31. Mono broadcasts. Optional raw codewords.",
        "MC band addresses (0..31). Mono rotates all lanes. Disconnected = identity.",
        "MC allocation per destination band: 0 mute, 1..14 = 2..15 bits. Mono broadcasts.",
        "MC scale index per destination band: 0..62, higher = quieter. Mono broadcasts."};
    static const char *outputs[] = {"Decoded mono audio", "Scan column (0..31)",
                                    "Frame start (one-sample pulse)"};
    snprintf(text, 512, "(signal) %s", kind == ASSIST_INLET ? inputs[port] : outputs[port]);
}

static void *create(t_symbol *name, long argc, t_atom *argv) {
    if (argc && (argc != 1 || atom_gettype(argv) != A_SYM || atom_getsym(argv) != gensym("raw"))) {
        error(OBJECT_NAME ": optional argument is raw");
        return NULL;
    }
    DecoderExternal *x = (DecoderExternal *)object_alloc(decoder_class);
    if (!x)
        return NULL;
    x->raw = argc == 1;
    dsp_setup(&x->object, 4);
    x->object.z_misc |= Z_NO_INPLACE;
    x->object.z_misc |= Z_MC_INLETS;
    for (int i = 0; i < 3; ++i)
        outlet_new((t_object *)x, "signal");
    return x;
}

void C74_EXPORT ext_main(void *module) {
    layer1_init_tables();
    t_class *c = class_new(OBJECT_NAME, (method)create, (method)dsp_free, sizeof(DecoderExternal),
                           NULL, A_GIMME, 0);
    class_addmethod(c, (method)dsp64, "dsp64", A_CANT, 0);
    class_addmethod(c, (method)assist, "assist", A_CANT, 0);
    class_dspinit(c);
    class_register(CLASS_BOX, c);
    decoder_class = c;
}
