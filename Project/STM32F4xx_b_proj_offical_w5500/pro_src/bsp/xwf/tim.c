#include "main.h"
#include <stdio.h>
#include <string.h>
#include "hua_def.h"
#include "hua_data.h"
#include "user_function.h"

TIM_HandleTypeDef Tim3Handle;
TIM_OC_InitTypeDef sConfig;

/* TIM3 init function */
void MX_TIM3_Init(void)
{
  
  
  Tim3Handle.Instance = TIM3;    

  Tim3Handle.Init.Prescaler         = 4;  //16M
  Tim3Handle.Init.Period            = (16000*10/128)-1;  //12.8K;
  Tim3Handle.Init.ClockDivision     = 0;
  Tim3Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
//  Tim3Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  Tim3Handle.Init.RepetitionCounter = 0;
  if (HAL_TIM_PWM_Init(&Tim3Handle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
    /* Common configuration for all channels */
  sConfig.OCMode       = TIM_OCMODE_PWM1;
  sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
  sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
  sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
  sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

  sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

  /* Set the pulse value for channel 1 */
  sConfig.Pulse =(Tim3Handle.Init.Period)/2;  //50%
  if (HAL_TIM_PWM_ConfigChannel(&Tim3Handle, &sConfig, TIM_CHANNEL_4) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler();
  }
}


void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
  GPIO_InitTypeDef   GPIO_InitStruct;
 
  __HAL_RCC_TIM3_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
 
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}


