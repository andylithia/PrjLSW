/**
  ******************************************************************************
  * @file    stm32l0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "stm32l0xx.h"
#include "stm32l0xx_it.h"

/* USER CODE BEGIN 0 */
#include "HCMS29xx.h"
#include "HCMS29xx_Util.h"
#include "menu_logic.h"
#include "main.h"

extern uint8_t testBuffer[4*5];
extern uint8_t gVirtualBuffer[4*9];
extern menu_t* currentMenu;

extern uint8_t gSleep;
extern uint8_t peakCurrentControl;
extern uint8_t gPWMControl;
extern uint8_t gSleepDelay;
int gSleepDelayActual=500;

extern int gShadeEnable;
extern uint8_t MFA_allowInterrupt;
uint8_t testFSM=0;

uint8_t keyStateMap=0;
int counter=0;
int dimoutPWMControl=0;
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim21;
extern TIM_HandleTypeDef htim22;

/******************************************************************************/
/*            Cortex-M0+ Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN HardFault_IRQn 1 */

  /* USER CODE END HardFault_IRQn 1 */
}

/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
* @brief This function handles Pendable request for system service.
*/
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32L0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l0xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles EXTI line 0 and line 1 interrupts.
*/
void EXTI0_1_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_1_IRQn 0 */

  /* USER CODE END EXTI0_1_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
  /* USER CODE BEGIN EXTI0_1_IRQn 1 */

  /* USER CODE END EXTI0_1_IRQn 1 */
}

/**
* @brief This function handles EXTI line 2 and line 3 interrupts.
*/
void EXTI2_3_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI2_3_IRQn 0 */

  /* USER CODE END EXTI2_3_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
  /* USER CODE BEGIN EXTI2_3_IRQn 1 */

  /* USER CODE END EXTI2_3_IRQn 1 */
}

/**
* @brief This function handles EXTI line 4 to 15 interrupts.
*/
void EXTI4_15_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_15_IRQn 0 */

  /* USER CODE END EXTI4_15_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
  /* USER CODE BEGIN EXTI4_15_IRQn 1 */

  /* USER CODE END EXTI4_15_IRQn 1 */
}

/**
* @brief This function handles TIM21 global interrupt.
*/
void TIM21_IRQHandler(void){

  /* USER CODE BEGIN TIM21_IRQn 0 */
	if(counter>gSleepDelayActual){
		if(!gSleep){
			gSleep=1;
			dimoutPWMControl=gPWMControl;
		}
		else {		
			if(!(counter&0x03)){
				if(dimoutPWMControl>0){
					// dim out display
					HCMS29xx_SendCWR((0x1<<6)|(peakCurrentControl<<4)|dimoutPWMControl);
					dimoutPWMControl--;
				}
				else {
					HCMS29xx_SendCWR(0x00);
					HAL_TIM_Base_Stop_IT(&htim21);
					__HAL_RCC_GPIOA_CLK_SLEEP_ENABLE();
					__HAL_RCC_GPIOC_CLK_SLEEP_ENABLE();
				}
			}
		}
	}
	counter++;
	
	if(!testFSM){
		HCMS29xx_VirtualBuffer_2Physical(gVirtualBuffer, testBuffer, 0, 0);
		// Key Scan
		if((BTN3_GPIO_Port->IDR & BTN3_Pin)){
			if(!(keyStateMap&(1<<3))){
				if(MFA_allowInterrupt)
					MFA_CALL(currentMenu,3,0);
				keyStateMap|=(1<<3);
			}
		}
		else
			keyStateMap&=~(1<<3);
		
		if((BTN4_GPIO_Port->IDR & BTN4_Pin)){
			if(!(keyStateMap&(1<<4))){
				if(MFA_allowInterrupt)
					MFA_CALL(currentMenu,4,0);
				keyStateMap|=(1<<4);
			}
		}
		else
			keyStateMap&=~(1<<4);

		if((BTN5_GPIO_Port->IDR & BTN5_Pin)){
			if(!(keyStateMap&(1<<5))){
				if(MFA_allowInterrupt)
					MFA_CALL(currentMenu,5,0);
				keyStateMap|=(1<<5);
			}
		}
		else
			keyStateMap&=~(1<<5);

		if((BTN6_GPIO_Port->IDR & BTN6_Pin)){
			if(!(keyStateMap&(1<<6))){
				if(MFA_allowInterrupt)
					MFA_CALL(currentMenu,6,0);
				keyStateMap|=(1<<6);
			}
		}
		else
			keyStateMap&=~(1<<6);
		
		HCMS29xx_SendMultiple(testBuffer, 20);
		
		if(gShadeEnable){
			testFSM=!testFSM;
			htim21.Instance->ARR=5;
		}
		else {
			htim21.Instance->ARR=20;
		}
	}
	else{
		if(gShadeEnable){
			testFSM=!testFSM;
			htim21.Instance->ARR=10;
			HCMS29xx_VirtualBuffer_2Physical(gVirtualBuffer, testBuffer, 0, 3);
			HCMS29xx_SendMultiple(testBuffer, 20);
		}
		else {
			testFSM=0;
			htim21.Instance->ARR=20;
			HCMS29xx_VirtualBuffer_2Physical(gVirtualBuffer, testBuffer, 0, 0);
			HCMS29xx_SendMultiple(testBuffer, 20);
		}
	}
  /* USER CODE END TIM21_IRQn 0 */
  HAL_TIM_IRQHandler(&htim21);
  /* USER CODE BEGIN TIM21_IRQn 1 */

  /* USER CODE END TIM21_IRQn 1 */
}

/**
* @brief This function handles TIM22 global interrupt.
*/
void TIM22_IRQHandler(void)
{
  /* USER CODE BEGIN TIM22_IRQn 0 */
	
  /* USER CODE END TIM22_IRQn 0 */
  HAL_TIM_IRQHandler(&htim22);
  /* USER CODE BEGIN TIM22_IRQn 1 */

  /* USER CODE END TIM22_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	counter=0;
	if(gSleep){
		// Restore screen
		__HAL_RCC_GPIOA_CLK_SLEEP_DISABLE();
		__HAL_RCC_GPIOC_CLK_SLEEP_DISABLE();
		HCMS29xx_SendCWR((0x1<<6)|(peakCurrentControl<<4)|gPWMControl);
		// Restore timer
		gSleep=0;
		htim21.Instance->ARR=20;
		HAL_TIM_Base_Start_IT(&htim21);
		
		// Dummy Key Scan
		if((BTN3_GPIO_Port->IDR & BTN3_Pin))
			keyStateMap|=(1<<3);
		if((BTN4_GPIO_Port->IDR & BTN4_Pin))
			keyStateMap|=(1<<4);
		if((BTN5_GPIO_Port->IDR & BTN5_Pin))
			keyStateMap|=(1<<5);
		if((BTN6_GPIO_Port->IDR & BTN6_Pin))
			keyStateMap|=(1<<6);
		
	}
		
}
/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
