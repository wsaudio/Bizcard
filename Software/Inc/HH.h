#ifndef HH
#define HH

#include "stm32f0xx_hal.h"
#include "LUTs.h"
#include "Global Vars.h"
#include "math.h"
#include "String.h"

#define noOfHHs 2
#define ClosedHHMaxDecayTime 0.03 //0.1 Secs
#define OpenHHMaxDecayTime 0.1 //0.1 Secs
#define HHLUTSize 1010

void initHHParams(void);
void changeHHDecayTime(uint8_t instrument, uint8_t newTime);
void calcHHDecayTim(uint8_t instrument);
uint8_t getCurrentHHAmp(uint8_t instrument);
void incHHLUTPos(uint8_t instrument);
void setHHNoteOn(uint8_t instrument);
void HHGenerateBuffer(uint8_t instrument, int8_t *buffer, uint16_t nFrames);

#endif
