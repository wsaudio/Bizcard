/*
STM32F030 Based drum machine

Peripheral configuration:
TIM1: Audio buffer generation, up counter 1024 * 256 (183.1Hz)
TIM3: LED/Button Cycle, up counter to 48000 (1kHz)
TIM14: Audio output PWM, up counter to 1024 (187.5kHz)

Pins configuration:
PA7: Audio output (TIM1 PWM1)
PA0, PA1, PA2, PA3, PA4: LED/Button MUX Out
PA5, PA6, PA9, PA10: LED MUX In
PA13, PA14, PF0, PF1: Button MUX In
*/

#ifndef GLOBAL_VARS
#define GLOBAL_VARS

//Audio
#define fs 23437.5
#define outputBufferSize 512
#define samplesPerChunk 256

//Fixed point conversion
#define fixedPointPos 20
#define fp(x) ((x) << fixedPointPos) //Convert to fixed point
#define ufp(x) ((x) >> fixedPointPos) //Convert to integer

//Ranges ---------------------------------------
//Master Audio
#define volumeSteps 8
//Sequencer
#define tempoMin 20
#define tempoMax 220
#define swingMin 0
#define swingMax 7
//Kick
#define kickPitchMin - 24
#define kickPitchMax 24
#define kickDecMin 1
#define kickDecMax 10
//Clap
#define clapDecMin 1
#define clapDecMax 10
//Toms
#define tomPitchMin - 24
#define tomPitchMax 24
#define tomDecMin 1
#define tomDecMax 10
//HH
#define hhClosedMin 1
#define hhClosedMax 10
#define hhOpenMin 1
#define hhOpenMax 10

//Defaults--------------------------------------
//Master Audio
#define defaultVol 8
#define readPointerDefault 384
#define writePointerDefault 0
//Sequencer
#define defaultTempo 90
#define defaultSwing 0
//Kick
#define defaultKickPitchOffset 0
#define defaultKickDecay 5
//Clap
#define defaultClapDecay 5
//Tom
#define defaultTomPitchOffset 0
#define defaultTomDecay 5
//HH
#define defaultHHCloseDecay 5
#define defaultHHOpenDecay 5

//Generators
#define noOfGenerators 6
#define kickNo 0
#define clapNo 1
#define tomNo 2
#define hhNo 3

//Registers
#define tim1Pwm1CCR (uint32_t*)0x40002034

#endif
