/**
  ******************************************************************************
  * @file    SPI/SPI_TwoBoards/SPI_DataExchangeInterrupt/main.c
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
#include "spi_driver.h"
#include "main.h"
/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup SPI_DataExchangeInterrupt
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
    uint8_t spi_aTxBuffer[BUFFERSIZE] = "SPI Master/Slave : Communication between two SPI using Interrupts";
__IO uint8_t spi_aRxBuffer [BUFFERSIZE];
__IO uint8_t spi_ubRxIndex = 0;
__IO uint8_t spi_ubTxIndex = 0;
__IO uint32_t spi_TimeOut = 0;

// extern  __IO uint8_t spi_aRxBuffer[];
// extern __IO uint8_t spi_ubRxIndex;
// extern __IO uint8_t spi_ubTxIndex;
// extern uint8_t spi_aTxBuffer[];
// extern __IO uint32_t spi_TimeOut;
__IO uint8_t spi_ubCounter = 0;
SPI_InitTypeDef  SPI_InitStructure;
void  spi_driver_select_gpio_init(void);
/* Private function prototypes -----------------------------------------------*/
static void SPI_Config(void);
void    spix_recv_data_end_event(uint8_t *pdt, uint16_t  len);
//static TestStatus Buffercmp(uint8_t* pBuffer1, __IO uint8_t* pBuffer2, uint16_t BufferLength);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int spix_driver_init(void)
{
  /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       files (startup_stm32f40_41xxx.s/startup_stm32f427_437xx.s/startup_stm32f429_439xx.s)
       before to branch to application main.
     */

  /* SPI configuration */
  	SPI_Config();
	spi_driver_select_gpio_init();
  /* SysTick configuration */
//   SysTickConfig();

//   /* LEDs configuration */
//   STM_EVAL_LEDInit(LED1);
//   STM_EVAL_LEDInit(LED2);
//   STM_EVAL_LEDInit(LED3);
//   STM_EVAL_LEDInit(LED4);

#ifdef SPI_MASTER
  /* Master board configuration */
  /* Initializes the SPI communication */
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_Init(SPIx, &SPI_InitStructure);

  /* The Data transfer is performed in the SPI interrupt routine */
  /* Configure the Tamper Button */
//   STM_EVAL_PBInit(BUTTON_TAMPER,BUTTON_MODE_GPIO);

  /* Wait until Tamper Button is pressed */
//   while (STM_EVAL_PBGetState(BUTTON_TAMPER));

  /* Enable the SPI peripheral */
  SPI_Cmd(SPIx, ENABLE);

  /* Initialize Buffer counters */
  spi_ubTxIndex = 0;
  spi_ubRxIndex = 0;

  /* Enable the Rx buffer not empty interrupt */
  SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, ENABLE);

  /* Enable the Tx buffer empty interrupt */
  SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, ENABLE);

#endif /* SPI_MASTER */

#ifdef SPI_SLAVE
  /* Slave board configuration */
  /* Initializes the SPI communication */
  SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
  SPI_Init(SPIx, &SPI_InitStructure);

  /* The Data transfer is performed in the SPI interrupt routine */
  /* Initialize Buffer counters */
  spi_ubTxIndex = 0;
  spi_ubRxIndex = 0;

  /* Enable the Rx buffer not empty interrupt */
  SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, ENABLE);

  /* Enable the Tx empty interrupt */
  SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, ENABLE);

  /* Enable the SPI peripheral */
  SPI_Cmd(SPIx, ENABLE);

#endif /* SPI_SLAVE */
#if 0
  /* Waiting the end of Data transfer */
  while ((spi_ubTxIndex < BUFFERSIZE) && (spi_ubRxIndex < BUFFERSIZE))
  {
  }

  /* Disable the Rx buffer not empty interrupt */
  SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, DISABLE);

  /* Disable the Tx empty interrupt */
  SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, DISABLE);

  /* Disable the SPI peripheral */
  SPI_Cmd(SPIx, DISABLE);

  if (Buffercmp(spi_aTxBuffer, spi_aRxBuffer, BUFFERSIZE) != FAILED)
  {
    /* Turn ON LED1 and LED3 */
    // STM_EVAL_LEDOn(LED1);
    // STM_EVAL_LEDOn(LED3);
    // /* Turn OFF LED2 and LED4 */
    // STM_EVAL_LEDOff(LED2);
    // STM_EVAL_LEDOff(LED4);
  }
  else
  {
    /* Turn OFF LED1 and LED3 */
    // STM_EVAL_LEDOff(LED1);
    // STM_EVAL_LEDOff(LED3);
    // /* Turn ON LED2 and LED4 */
    // STM_EVAL_LEDOn(LED2);
    // STM_EVAL_LEDOn(LED4);
  }

  /* Infinite Loop */
  while (1)
  {
  }
#endif
  return 0;
}

/**
  * @brief  Configures the SPI Peripheral.
  * @param  None
  * @retval None
  */
static void SPI_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Peripheral Clock Enable -------------------------------------------------*/
  /* Enable the SPI clock */
  SPIx_CLK_INIT(SPIx_CLK, ENABLE);

  /* Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(SPIx_SCK_GPIO_CLK | SPIx_MISO_GPIO_CLK | SPIx_MOSI_GPIO_CLK, ENABLE);

  /* SPI GPIO Configuration --------------------------------------------------*/
  /* GPIO Deinitialisation */
  GPIO_DeInit(SPIx_SCK_GPIO_PORT);
  GPIO_DeInit(SPIx_MISO_GPIO_PORT);
  GPIO_DeInit(SPIx_MOSI_GPIO_PORT);

  /* Connect SPI pins to AF5 */
  GPIO_PinAFConfig(SPIx_SCK_GPIO_PORT, SPIx_SCK_SOURCE, SPIx_SCK_AF);
  GPIO_PinAFConfig(SPIx_MISO_GPIO_PORT, SPIx_MISO_SOURCE, SPIx_MISO_AF);
  GPIO_PinAFConfig(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_SOURCE, SPIx_MOSI_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

  /* SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = SPIx_SCK_PIN;
  GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStructure);

  /* SPI  MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin =  SPIx_MISO_PIN;
  GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStructure);

  /* SPI  MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  SPIx_MOSI_PIN;
  GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /* SPI configuration -------------------------------------------------------*/
  SPI_I2S_DeInit(SPIx);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;

  /* Configure the Priority Group to 1 bit */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  /* Configure the SPI interrupt priority */
  NVIC_InitStructure.NVIC_IRQChannel = SPIx_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}



/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */
 TestStatus Buffercmp(uint8_t* pBuffer1, __IO uint8_t* pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }
    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;
}
//add by bcg,2020-09-01 10:52:41
void spix_decrement(void)
{
	/* Decrement the spi_TimeOut value */
  if (spi_TimeOut != 0)
  {
    spi_TimeOut--;
  }

  if (spi_ubCounter < 10)
  {
    spi_ubCounter++;
  }
  else
  {
    spi_ubCounter = 0;
    // STM_EVAL_LEDToggle(LED1);
  }
}

//add by bcg,2020-09-01 10:50:27
void spix_driver_irqhandler(void)
{
  //add by bcg,2020-09-01 19:26:42 --spi3
	/* SPI in Receiver mode */
	if (SPI_I2S_GetITStatus(SPIx, SPI_I2S_IT_RXNE) == SET)
	{
		if (spi_ubRxIndex < BUFFERSIZE)
		{
			/* Receive Transaction data */
			spi_aRxBuffer[spi_ubRxIndex++] = SPI_I2S_ReceiveData(SPIx);
			//spix_recv_data_end_event(spi_aRxBuffer, spi_ubRxIndex);
		}
		else
		{
			/* Disable the Rx buffer not empty interrupt */
			SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, DISABLE);
			//spix_recv_data_end_event(spi_aRxBuffer, spi_ubRxIndex);
			spi_ubRxIndex = 0;
			SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, ENABLE);
		}
	}
	/* SPI in Transmitter mode */
	if (SPI_I2S_GetITStatus(SPIx, SPI_I2S_IT_TXE) == SET)
	{
		if (spi_ubTxIndex < BUFFERSIZE)
		{
			/* Send Transaction data */
			SPI_I2S_SendData(SPIx, spi_aTxBuffer[spi_ubTxIndex++]);
		}
		else
		{
			/* Disable the Tx buffer empty interrupt */
			SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, DISABLE);
		}
	}
}

//add by bcg,2020-09-01 11:54:53 spi_片选接口
void   spi_driver_select(uint8_t spi_id)
{
	if (spi_id == SPI_SELECT_1)
	{
		GPIO_SetBits(SPI3_SS1_PIN_GROUP, SPI3_SS1_PIN);
		GPIO_ResetBits(SPI3_SS_PIN_GROUP, SPI3_SS_PIN);
	}
	else if (spi_id == SPI_SELECT_2)
	{
		GPIO_SetBits(SPI3_SS_PIN_GROUP, SPI3_SS_PIN);
		GPIO_ResetBits(SPI3_SS1_PIN_GROUP, SPI3_SS1_PIN);

	}
}
//add by bcg,2020-09-01 19:42:49 spi 片选 gpio init
void  spi_driver_select_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* GPIOG Peripheral clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	/* GPIOG Peripheral clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* Configure PG6 and PG8 in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = SPI3_SS_PIN | SPI3_SS1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
}



void    spix_send_data(uint8_t *pdt, uint16_t  len)
{
	memcpy(spi_aTxBuffer, pdt, BUFFERSIZE < len ? BUFFERSIZE : len);
	spi_ubTxIndex = 0;
	SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, ENABLE);
}
//add by bcg,2020-09-01 19:52:00  接收spix数据处理
void    spix_recv_data_end_event(uint8_t *pdt, uint16_t  len)
{

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

  /* Infinite loop */
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
