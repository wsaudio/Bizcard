/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "stm32f0xx.h"
#include "stm32f0xx_it.h"
#include "Master Audio.h"
#include "User Interface.h"

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim14;

/******************************************************************************/
/*            Cortex-M0 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
}

/*TIM1_BRK_UP_TRG_COM_IRQHandler
Audio buffer generation
*/
void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{	
	masterAudioGenBuffer();
	
	__HAL_TIM_CLEAR_IT(&htim1, TIM_IT_UPDATE);
}

/*TIM14_IRQHandler
Outputs current sample to PWM
*/
void TIM14_IRQHandler(void)
{	
	sendOutputSample();
	
	__HAL_TIM_CLEAR_IT(&htim14, TIM_IT_UPDATE);
}

/*TIM3_IRQHandler
Handles user interface multiplexing
*/
void TIM3_IRQHandler(void)
{
	uiMuxInterrupt();

	__HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);
}
