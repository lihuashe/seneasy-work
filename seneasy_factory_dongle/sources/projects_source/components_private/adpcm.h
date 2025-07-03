#ifndef ADPCM_H
#define ADPCM_H

typedef struct
{
    int valprev;
    int index;
    
}stCodecState_t;

void adpcm_encode(stCodecState_t* state, int16_t* input, int numSamples, uint8_t* output);

void adpcm_decode(stCodecState_t* state, uint8_t* input, int numSamples, int16_t* output);

#endif
