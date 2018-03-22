#ifndef SEQUENCER
#define SEQUENCER

#include "stm32f0xx_hal.h"
#include "Global Vars.h"

#define noOfSequencerSteps 16
#define swingRange 0.5

extern uint8_t sequence[noOfGenerators][noOfSequencerSteps];
extern uint8_t currentPlaybackStep;

void initSequencerParams(void);
void calcTimeToNextStep(void);
void changeSequencerTempo(uint8_t newVal);
void changeSequencerSwing(uint8_t newVal);
void addOrRemoveSequencerStep(uint8_t instrument, uint8_t stepNo);
void cancelConflictingNotes(uint8_t instrument, uint8_t stepNo);
uint8_t getStepValue(uint8_t instrument);
uint16_t getSamplesToGenerate(uint16_t remainingSamples);

#endif
