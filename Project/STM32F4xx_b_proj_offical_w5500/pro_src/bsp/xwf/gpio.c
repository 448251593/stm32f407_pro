
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "def.h"
#include "global_data.h"
#include "function.h"
#include "xwf_pin_map.h"
#if 0



void free_spi_cs(uint32_t Pin_cs)
{
switch(Pin_cs)
  {
   case mx_cs:
   HAL_GPIO_WritePin(mx_cs_port,mx_cs,GPIO_PIN_SET);  
   break;
   case sram_cs:
   HAL_GPIO_WritePin(sram_cs_port,sram_cs,GPIO_PIN_SET);
   break;
   case eth_cs:
   HAL_GPIO_WritePin(eth_cs_port,eth_cs,GPIO_PIN_SET);
   break;
   case ad_cs:
   HAL_GPIO_WritePin(ad_cs_port,ad_cs,GPIO_PIN_SET);
   break;      
  }	
}

void selected_spi_cs(uint32_t Pin_cs)
{
switch(Pin_cs)
  {
   case mx_cs:
   HAL_GPIO_WritePin(mx_cs_port,mx_cs,GPIO_PIN_RESET);  
   break;
   case sram_cs:
   HAL_GPIO_WritePin(sram_cs_port,sram_cs,GPIO_PIN_RESET);
   break;
   case eth_cs:
   HAL_GPIO_WritePin(eth_cs_port,eth_cs,GPIO_PIN_RESET);
   break;
   case ad_cs:
   HAL_GPIO_WritePin(ad_cs_port,ad_cs,GPIO_PIN_RESET);
   break;      
  }		
}
#endif
void gpio_out_init(void)
{
	/*
	uint32_t GPIO_Pin;                                          
  GPIOMode_TypeDef GPIO_Mode;     
  GPIOSpeed_TypeDef GPIO_Speed;                             
  GPIOOType_TypeDef GPIO_OType;                    
  GPIOPuPd_TypeDef GPIO_PuPd;     
                                  
	*/
//  GPIO_InitTypeDef GPIO_InitStruct = {0};
//  GPIO_InitStruct.Pin = LED_Y|LED_O;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);
//	
//  led_off(LED_Y|LED_O);  
    GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.GPIO_Pin = LED_Y|LED_O;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(LED_PORT, &GPIO_InitStruct);

		
//	GPIO_InitStructure.GPIO_Pin = LED_Y|LED_O;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
  // GPIO_InitStruct.Pin =mx_cs;
  // HAL_GPIO_Init(mx_cs_port, &GPIO_InitStruct); 
  // free_spi_cs(mx_cs);
  // GPIO_InitStruct.Pin =mx_hold;
  // HAL_GPIO_Init(mx_hold_port, &GPIO_InitStruct); 
  // HAL_GPIO_WritePin(mx_hold_port,mx_hold,GPIO_PIN_SET);

  // GPIO_InitStruct.Pin =eth_cs;
  // HAL_GPIO_Init(eth_cs_port, &GPIO_InitStruct); 
  // free_spi_cs(eth_cs);
  // GPIO_InitStruct.Pin =eth_rst;
  // HAL_GPIO_Init(eth_rst_port, &GPIO_InitStruct); 
  // HAL_GPIO_WritePin(eth_rst_port,eth_rst,GPIO_PIN_SET);
  
  // GPIO_InitStruct.Pin =sram_cs;
  // HAL_GPIO_Init(sram_cs_port, &GPIO_InitStruct); 
  // free_spi_cs(sram_cs);
  
  // GPIO_InitStruct.Pin =ad_cs;
  // HAL_GPIO_Init(ad_cs_port, &GPIO_InitStruct); 
  // free_spi_cs(ad_cs);
}

/*  Bit_RESET = 0,
  Bit_SET*/
void led_on(uint16_t led_num)
{
//HAL_GPIO_WritePin(LED_PORT,led_num,GPIO_PIN_RESET); 
GPIO_WriteBit(LED_PORT,led_num,Bit_RESET)	;
}

void led_off(uint16_t led_num)
{
//HAL_GPIO_WritePin(LED_PORT, led_num,GPIO_PIN_SET);    
	GPIO_WriteBit(LED_PORT,led_num,Bit_SET)	;
}
void led_toggle(uint16_t led_num)
{
//HAL_GPIO_TogglePin(LED_PORT,led_num);
	GPIO_ToggleBits(LED_PORT,led_num);
}
void gpio_input_init(void)
{
 
  

 
}


void gpio_init(void)
{
//  __HAL_RCC_GPIOC_CLK_ENABLE();
//  __HAL_RCC_GPIOH_CLK_ENABLE();
//  __HAL_RCC_GPIOA_CLK_ENABLE();
//  __HAL_RCC_GPIOB_CLK_ENABLE();
//  __HAL_RCC_GPIOG_CLK_ENABLE();
//  __HAL_RCC_GPIOD_CLK_ENABLE();
//  __HAL_RCC_GPIOE_CLK_ENABLE();
//  __HAL_RCC_GPIOF_CLK_ENABLE();
//  __HAL_RCC_GPIOI_CLK_ENABLE();
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
  gpio_out_init();
  gpio_input_init();
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
