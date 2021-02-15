/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.c
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
#include "main.h"
#include "usart_driver.h"
#include "spi_net_driver.h"
#include "xwf_pin_map.h"
#include "spi_adc_driver.h"
#include "usart_app.h"
#include "spi_adc_app.h"
#include "tim9_driver.h"
#include "stdio.h"
#include "w5500.h"
#include "loopback.h"

/** @addtogroup Template_Project
  * @{
  */
extern void gpio_init(void);
extern void led_on(uint16_t led_num);
extern void led_off(uint16_t led_num);
extern void led_toggle(uint16_t led_num);
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t uwTimingDelay;


RCC_ClocksTypeDef RCC_Clocks;

/* Private function prototypes -----------------------------------------------*/


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	//  GPIO_InitTypeDef GPIO_InitStructure;
	//int i = 0;
	/*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       files before to branch to application main.
       To reconfigure the default setting of SystemInit() function,
       refer to system_stm32f4xx.c file */
	uint32_t tmp_count = 0;
	gpio_init();
	usart3_init();
	printf("version info = %d.%d.%d\n", VERSION_MAIN_NUM, VERSION_SUB1_NUM, VERSION_SUB2_NUM);
	const char* time = __TIME__ " "__DATE__ ;
	printf("build time = %s\n", time);
	usart3send_flush();
	sADC_Init();
	// timer9_driver_init();
	/* SysTick end of count event each 10ms */
	RCC_GetClocksFreq(&RCC_Clocks);
	//SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);

	/* Add your application code here */
	/* Insert 50 ms delay */
	Delay(50);

	W5500_net_init();
	/* Infinite loop */
	//led_off(LED_Y);
	led_off(LED_O);
	// adc_read_start();
	spi_adc_read();//add by bcg,2020-12-05 11:14:05第一次读取0值去掉
	// usart3_send_data("1234567", 7);
	while (1)
	{
		// tmp_count = get_global_tick();
		// led_toggle(LED_Y);
		// led_toggle(LED_O);
		// Delay(1);

		// spi_adc_read();
		// adc_read_deal();
		// get_adc_data();
		get_adc_data_200khz();
		NetLoop();
		w5500_send();
		// if (tmp_count % (1000) == 0)
		// {
		// 	led_toggle(LED_Y);
		// 	led_toggle(LED_O);
		// 	// tmp_count = get_global_tick();
		// 	// spi_adc_read();
		// 	// printf("tmp_count=%d",tmp_count);
		// 	// usart3send_flush();
		// }
	}
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay( uint32_t nTime)
{
	uwTimingDelay = nTime;

	while (uwTimingDelay != 0)
		;
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
	run_status_g.time_tick_ms++;
	if (uwTimingDelay != 0x00)
	{
		uwTimingDelay--;
	}

	usart3_parse_cmd();
	if (run_status_g.time_tick_ms % (1000) == 0)
	{
		//led_toggle(LED_Y);
		led_toggle(LED_O);
		DHCP_time_handler();
	}
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
