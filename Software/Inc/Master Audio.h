#ifndef MASTER_AUDIO
#define MASTER_AUDIO

#include "stm32f0xx_hal.h"
#include "Global Vars.h"
#include "Sequencer.h"
#include "Kick.h"
#include "Clap.h"
#include "Tom.h"
#include "HH.h"
#include "LUTs.h"

//Function prototypes
void initAudio(void);
void initVol(void);
void setGeneratorVol(uint8_t instNo, uint8_t newVol);
void startAudioPlayback(void);
void checkRWAlignment(void);
void masterAudioGenBuffer(void);
void resetBuffer(uint16_t nFrames);
void addToBuffer(int8_t *buffer, uint16_t nFrames, uint8_t instrument);
void generateAndMixInstruments(uint16_t nFrames);
void triggerInstruments(void);
void sendOutputSample(void);

#endif
