#include "band_input.h"
#include "layer1.h"
#include <stdio.h>
#include <math.h>

#define OBJECT_NAME "mp1bands~"

typedef struct {
    t_pxobject object;
    Layer1 decoder;
    Bus bus;
    int raw;
    double words[32], allocation[32], scale[32];
    uint32_t valid;
} DecoderExternal;

static t_class *decoder_class;

static void capture_slot(DecoderExternal *x, double **ins, long sample) {
    if (x->decoder.slot == 0) {
        for (int band = 0; band < 32; ++band) {
            x->allocation[band] = lane(&x->bus, ins, 2, 0, band, sample, 7);
            x->scale[band] = lane(&x->bus, ins, 3, 0, band, sample, 18);
        }
    }
    x->valid = route_band_words(&x->bus, ins, (int)sample, x->words);
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
    bus_setup(&x->bus, x, chain, connected, 4);
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
