#ifndef LUTS
#define LUTS

#include "stm32f0xx_hal.h"
#include "Global Vars.h"

//LUT sizes
#define sinLUTSize 768
#define pitchDecLUTSize 94
#define ampDecLUTSize 256

//LUT limits
#define sinLUTLimit fp(sinLUTSize)
#define pitchDecLUTLimit fp(pitchDecLUTSize)
#define ampDecLUTLimit fp(ampDecLUTSize)

//Function prototypes
int8_t sinLUT(uint32_t pos);
uint8_t pitchDecLUT(uint32_t pos);
uint8_t ampDecLUT(uint32_t pos);
uint8_t rnd(void);

#endif
