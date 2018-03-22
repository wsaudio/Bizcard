#ifndef KICK
#define KICK

#include "stm32f0xx_hal.h"
#include "LUTs.h"
#include "Global Vars.h"
#include "math.h"
#include "String.h"

//Defines
#define kickAmpEndPos fp(ampDecLUTSize)
#define kickPitchDecayPlaybackSpeed (0.02 * pow(2, fixedPointPos))
#define kickMaxDecayTime 1.0 //2.3 Secs
#define kickBasePitch 55 //55Hz

extern uint8_t pitchDecLUTData[pitchDecLUTSize];

//Function prototypes
void initKickParams(void);
void changeKickPitch(uint8_t newPitch);
void changeKickDecayTime(uint8_t newTime);
void calcKickPitch(void);
void calcKickDecayTime(void);
void setKickNoteOn(void);
uint32_t getKickSinPos(void);
int8_t getCurrentKickAmp(void);
void kickGenerateBuffer(int8_t *buffer, uint16_t nFrames);

#endif
