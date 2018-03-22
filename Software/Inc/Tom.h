#ifndef TOM
#define TOM

#include "stm32f0xx_hal.h"
#include "LUTs.h"
#include "Global Vars.h"
#include "math.h"
#include "String.h"

//Defines
#define noOfToms 2
#define tomAmpEndPos fp(ampDecLUTSize)
#define tomPitchDecayPlaybackSpeed (0.1 * pow(2, fixedPointPos))
#define tomMaxDecayTime 0.23 //0.23s
#define tomLowBasePitch 110 //110Hz
#define tomHighBasepitch 220 //220Hz

extern uint8_t pitchDecLUTData[pitchDecLUTSize];

//Function prototypes
void initTomParams(void);
void changeTomPitch(uint8_t instrument, uint8_t newPitch);
void changeTomDecayTime(uint8_t newTime);
void calcTomPitch(uint8_t instrument);
void calcTomDecay(void);
void setTomNoteOn(uint8_t instrument);
uint32_t getTomSinPos(uint8_t instrument);
uint8_t getCurrentTomAmp(uint8_t instrument);
void tomGenerateBuffer(uint8_t instrument, int8_t *buffer, uint16_t nFrames);

#endif
