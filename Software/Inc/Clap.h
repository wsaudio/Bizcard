#ifndef CLAP
#define CLAP

#include "stm32f0xx_hal.h"
#include "LUTs.h"
#include "Global Vars.h"
#include "Math.h"
#include "String.h"

#define clapMaxDecayTime 0.2
#define clapCnt 2
#define preClapLength 284

void initClapParams(void);
void changeClapDecayTime(uint8_t newTime);
void calcClapDecayTim(void);
uint8_t getCurrentClapAmp(void);
void setClapNoteOn(void);
void clapGenerateBuffer(int8_t *buffer, uint16_t nFrames);

#endif
