#ifndef MP3PATCH_ENCODER_H
#define MP3PATCH_ENCODER_H
#include "layer3.h"
typedef struct Encoder Encoder;
Encoder *encoder_new(int rate, int channels, int bitrate);
void encoder_free(Encoder *e);
void encoder_set_bitrate(Encoder *e, int kbps);
void encoder_feed(Encoder *e, float left, float right);
const EncodedFrame *encoder_next(Encoder *e);
unsigned encoder_dropouts(Encoder *e);
#endif
