/**
  ******************************************************************************
  * @file    TIM/TIM_TIM9OCToggle/main.c
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "xwf_pin_map.h"
#include "spi_adc_app.h"
#include "main.h"
#include "tim9_driver.h"
/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup TIM_TIM9OCToggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t capture = 0;
__IO uint16_t CCR1_Val = 15;//add by bcg,2020-12-02 16:11:08  f = 15000000 / CCR2_Val /2 ; 15 = 500kHz
__IO uint16_t CCR2_Val = 20480;
uint16_t PrescalerValue = 0;

/* Private function prototypes -----------------------------------------------*/
void TIM_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int timer9_driver_init(void)
{
	/*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       files (startup_stm32f40_41xxx.s/startup_stm32f427_437xx.s/startup_stm32f429_439xx.s)
       before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	/* TIM Configuration */
	TIM_Config();

	/* ---------------------------------------------------------------------------
     TIM9 Configuration: Output Compare Toggle Mode:

    In this example TIM9 input clock (TIM9CLK) is set to 2 * APB2 clock (PCLK2),
    since APB2 prescaler is different from 1.
      TIM9CLK = 2 * PCLK2
      PCLK2 = HCLK / 2
      => TIM9CLK = HCLK = SystemCoreClock

    To get TIM9 counter clock at 15 MHz, the prescaler is computed as follows:
       Prescaler = (TIM9CLK / TIM9 counter clock) - 1
       Prescaler = (SystemCoreClock /15 MHz) - 1

    CC1 update rate = TIM9 counter clock / CCR1_Val = 366.2 Hz
    ==> So the TIM9 Channel 1 generates a periodic signal with
	      a frequency equal to 183.1 Hz

    CC2 update rate = TIM9 counter clock / CCR2_Val = 732.4 Hz
    ==> So the TIM9 channel 2 generates a periodic signal with
	      a frequency equal to 366.3 Hz

    Note:
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f4xx.c file.
     Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
     function to update SystemCoreClock variable value. Otherwise, any configuration
     based on this variable will be incorrect.
  --------------------------------------------------------------------------- */

	/* Compute the prescaler value */
	PrescalerValue = (uint16_t)(SystemCoreClock / 15000000) - 1;

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 65535;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);

	/* Output Compare Toggle Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC1Init(TIM9, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Disable);

	/* Output Compare Toggle Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;//TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR2_Val;

	TIM_OC2Init(TIM9, &TIM_OCInitStructure);

	TIM_OC2PreloadConfig(TIM9, TIM_OCPreload_Disable);

	/* TIM enable counter */
	TIM_Cmd(TIM9, ENABLE);

	/* TIM IT enable */
	// TIM_ITConfig(TIM9, TIM_IT_CC1 | TIM_IT_CC2, ENABLE);
	// TIM_ITConfig(TIM9, TIM_IT_CC1, ENABLE);

	// while (1)
	// {}
	return  0;
}

/**
  * @brief  Configure the TIM9 Pins.
  * @param  None
  * @retval None
  */
void TIM_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* TIM9 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);

  /* GPIOA clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);

  /* GPIOA Configuration: TIM9 CH1 (PA2) and TIM9 CH2 (PA3) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Connect TIM pins to AF3 */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_TIM9);
//   GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_TIM9);

  /* Enable the TIM9 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f40xx.s/startup_stm32f427x.s).                         */
/******************************************************************************/

/**
  * @brief  This function handles TIM9 global interrupt request.
  * @param  None
  * @retval None
  */
static __IO uint32_t timer_tick_ms_g = 0;
// uint32_t   count_test = 0;


void TIM9_IRQHandler_s(void)
{
  /* TIM9_CH1 toggling with frequency = 500khz;//183.1 Hz */
  if (TIM_GetITStatus(TIM9, TIM_IT_CC1) != RESET)
  {
    TIM_ClearITPendingBit(TIM9, TIM_IT_CC1);
    capture = TIM_GetCapture1(TIM9);
    TIM_SetCompare1(TIM9, capture + CCR1_Val);
	// count_test++;
	// if(count_test== 2*500)//add by bcg,2020-12-02 21:19:57 1000
	// {
	// 	count_test = 0;
	// 	// led_toggle(LED_Y);
	// 	// get_adc_data();

	// }
  
  	timer_tick_ms_g++;
  }

  /* TIM9_CH2 toggling with frequency = 366.2 Hz */
  if (TIM_GetITStatus(TIM9, TIM_IT_CC2) != RESET)
  {
    TIM_ClearITPendingBit(TIM9, TIM_IT_CC2);
    capture = TIM_GetCapture2(TIM9);
    TIM_SetCompare2(TIM9, capture + CCR2_Val);
  }

}
inline uint32_t get_global_tick(void)
{
  // return  timer_tick_ms_g;
  return  run_status_g.time_tick_ms;

}
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  while (1)
  {}
}
#endif
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
