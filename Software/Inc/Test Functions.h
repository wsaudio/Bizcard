#ifndef TEST_FUNCTIONS
#define TEST_FUNCTIONS

#include "stm32f0xx_hal.h"
#include "Kick.h"
#include "Sequencer.h"
#include "Master Audio.h"

//Kick
extern int8_t kickPitchOffset;
extern uint8_t kickDecayTime;

uint8_t testKickFunctions(void);
uint8_t testSeqFunctions(void);

#endif
