/**
  ******************************************************************************
  * @file    SPI/SPI_NET/spi_NET.c
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   This file provides a set of functions needed to manage the SPI M25Pxxx
  *          NET memory.
  *
  *          ===================================================================
  *          Notes:
  *           - There is no SPI NET memory available in STM324xG-EVAL board,
  *             to use this driver you have to build your own hardware.
  *          ===================================================================
  *
  *          It implements a high level communication layer for read and write
  *          from/to this memory. The needed STM32 hardware resources (SPI and
  *          GPIO) are defined in spi_NET.h file, and the initialization is
  *          performed in sADC_LowLevel_Init() function.
  *
  *          You can easily tailor this driver to any development board, by just
  *          adapting the defines for hardware resources and sADC_LowLevel_Init()
  *          function.
  *
  *          +-----------------------------------------------------------+
  *          |                     Pin assignment                        |
  *          +-----------------------------+---------------+-------------+
  *          |  STM32 SPI Pins             |     sADC    |    Pin      |
  *          +-----------------------------+---------------+-------------+
  *          | sADC_CS_PIN               | ChipSelect(/S)|    1        |
  *          | sADC_SPI_MISO_PIN / MISO  |   DataOut(Q)  |    2        |
  *          |                             |   VCC         |    3 (3.3 V)|
  *          |                             |   GND         |    4 (0 V)  |
  *          | sADC_SPI_MOSI_PIN / MOSI  |   DataIn(D)   |    5        |
  *          | sADC_SPI_SCK_PIN / SCK    |   Clock(C)    |    6        |
  *          |                             |    VCC        |    7 (3.3 V)|
  *          |                             |    VCC        |    8 (3.3 V)|
  *          +-----------------------------+---------------+-------------+
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
#include "spi_adc_driver.h"

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup SPI_NET
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void sADC_LowLevel_DeInit(void);
void sADC_LowLevel_Init(void);
void s_adc_gain_pin_init(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  DeInitializes the peripherals used by the SPI NET driver.
  * @param  None
  * @retval None
  */
void sADC_DeInit(void)
{
  sADC_LowLevel_DeInit();
}

/**
  * @brief  Initializes the peripherals used by the SPI NET driver.
  * @param  None
  * @retval None
  */
void sADC_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;

  sADC_LowLevel_Init();

  /*!< Deselect the NET: Chip Select high */
  sADC_CS_HIGH();

  /*!< SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//SPI_Direction_1Line_Rx;//add by bcg,2020-09-02 15:44:41 only read
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//SPI_FirstBit_LSB;//SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(sADC_SPI, &SPI_InitStructure);

#if SPI_ADC_DMA_ENABLE

#else
/*!< Enable the sADC_SPI  */
	SPI_Cmd(sADC_SPI, ENABLE);
#endif


  s_adc_gain_pin_init();

}
#if SPI_ADC_DMA_ENABLE
void sADC_dma_start(void)
{
	/* Enable DMA SPI TX Stream */
	DMA_Cmd(sADC_SPI_TX_DMA_STREAM, ENABLE);

	/* Enable DMA SPI RX Stream */
	DMA_Cmd(sADC_SPI_RX_DMA_STREAM, ENABLE);

	/* Enable SPI DMA TX Requsts */
	SPI_I2S_DMACmd(sADC_SPI, SPI_I2S_DMAReq_Tx, ENABLE);

	/* Enable SPI DMA RX Requsts */
	SPI_I2S_DMACmd(sADC_SPI, SPI_I2S_DMAReq_Rx, ENABLE);

	// /* Enable the SPI peripheral */
	// SPI_Cmd(sADC_SPI, ENABLE);

	/*!< Enable the sADC_SPI  */
	SPI_Cmd(sADC_SPI, ENABLE);
}
void sADC_dma_stop(void)
{
	/* Enable DMA SPI TX Stream */
	DMA_Cmd(sADC_SPI_TX_DMA_STREAM, DISABLE);

	/* Enable DMA SPI RX Stream */
	DMA_Cmd(sADC_SPI_RX_DMA_STREAM, DISABLE);

	/* Enable SPI DMA TX Requsts */
	SPI_I2S_DMACmd(sADC_SPI, SPI_I2S_DMAReq_Tx, DISABLE);

	/* Enable SPI DMA RX Requsts */
	SPI_I2S_DMACmd(sADC_SPI, SPI_I2S_DMAReq_Rx, DISABLE);

	// /* Enable the SPI peripheral */
	// SPI_Cmd(sADC_SPI, ENABLE);

	/*!< Enable the sADC_SPI  */
	SPI_Cmd(sADC_SPI, DISABLE);
}


uint8_t  sADC_dma_check_finish(void)
{
	/* Waiting the end of Data transfer */
	//   while (DMA_GetFlagStatus(sADC_SPI_TX_DMA_STREAM,sADC_SPI_TX_DMA_FLAG_TCIF)==RESET);
	//   while (DMA_GetFlagStatus(sADC_SPI_RX_DMA_STREAM,sADC_SPI_RX_DMA_FLAG_TCIF)==RESET);
	/* Clear DMA Transfer Complete Flags */
	// DMA_ClearFlag(sADC_SPI_TX_DMA_STREAM, sADC_SPI_TX_DMA_FLAG_TCIF);
	// DMA_ClearFlag(sADC_SPI_RX_DMA_STREAM, sADC_SPI_RX_DMA_FLAG_TCIF);
	uint8_t  ret = 0;
	if (DMA_GetFlagStatus(sADC_SPI_TX_DMA_STREAM, sADC_SPI_TX_DMA_FLAG_TCIF) == RESET)
	{
		ret = ret | 0x01;
		DMA_ClearFlag(sADC_SPI_TX_DMA_STREAM, sADC_SPI_TX_DMA_FLAG_TCIF);
	}
	if (DMA_GetFlagStatus(sADC_SPI_RX_DMA_STREAM, sADC_SPI_RX_DMA_FLAG_TCIF) == RESET)
	{
		ret = ret | 0x02;
		DMA_ClearFlag(sADC_SPI_RX_DMA_STREAM, sADC_SPI_RX_DMA_FLAG_TCIF);
	}
	return ret;

}
#endif
/**
  * @brief  Reads a byte from the SPI NET.
  * @note   This function must be used only if the Start_Read_Sequence function
  *         has been previously called.
  * @param  None
  * @retval Byte Read from the SPI NET.--ok
  */
uint16_t sADC_ReadByte(void)
{
	uint16_t t = 0;
	sADC_CS_LOW();
	t = sADC_SendByte(sADC_DUMMY_BYTE);
	sADC_CS_HIGH();
	// sADC_CS_HIGH();

	return t;
}

/**
  * @brief  Sends a byte through the SPI interface and return the byte received
  *         from the SPI bus.
  * @param  byte: byte to send.
  * @retval The value of the received byte.--ok
  */
uint16_t sADC_SendByte(uint16_t byte)
{

  /*!< Loop while DR register in not empty */
  while (SPI_I2S_GetFlagStatus(sADC_SPI, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(sADC_SPI, byte);

  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(sADC_SPI, SPI_I2S_FLAG_RXNE) == RESET);

  /*!< Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(sADC_SPI);
}
//add by bcg,2020-09-02 10:22:41

// HAL_SPI_Transmit(&SpiHandle[1], (uint8_t *)&(fptr->tx),fptr->txnum,1000);
// HAL_SPI_Receive(&SpiHandle[1], fptr->rx, fptr->rxnum,1000);
void SPI_adc_transfer_block(uint8_t *pBuffer_tx, uint16_t NumByteToTx,uint8_t *pBuffer_rx, uint16_t NumByteToRx)
{
	// while (NumByteToRead--) /*!< while there is data to be read */
	// {
	// 	/*!< Read a byte from the NET */
	// 	*pBuffer = sADC_SendByte(sADC_DUMMY_BYTE);
	// 	/*!< Point to the next location where the byte read will be saved */
	// 	pBuffer++;
	// }
	while (NumByteToTx--)
	{
		sADC_SendByte(*pBuffer_tx++);
	}

	while (NumByteToRx--)
	{
		*pBuffer_rx = sADC_SendByte(sADC_DUMMY_BYTE);
		/*!< Point to the next location where the byte read will be saved */
		pBuffer_rx++;
	}
}
#if SPI_ADC_DMA_ENABLE
#define BUFFERSIZE_ADC   1024
uint16_t adc_dma_RxBuffer[BUFFERSIZE_ADC];
#endif

/**
  * @brief  Initializes the peripherals used by the SPI NET driver.
  * @param  None
  * @retval None
  */
void sADC_LowLevel_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /*!< Enable the SPI clock */
  sADC_SPI_CLK_INIT(sADC_SPI_CLK, ENABLE);

  /*!< Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(sADC_SPI_SCK_GPIO_CLK | sADC_SPI_MISO_GPIO_CLK |
                         sADC_SPI_MOSI_GPIO_CLK | sADC_CS_GPIO_CLK, ENABLE);

  /*!< SPI pins configuration *************************************************/

  /*!< Connect SPI pins to AF5 */
  GPIO_PinAFConfig(sADC_SPI_SCK_GPIO_PORT, sADC_SPI_SCK_SOURCE, sADC_SPI_SCK_AF);
  GPIO_PinAFConfig(sADC_SPI_MISO_GPIO_PORT, sADC_SPI_MISO_SOURCE, sADC_SPI_MISO_AF);
  // GPIO_PinAFConfig(sADC_SPI_MOSI_GPIO_PORT, sADC_SPI_MOSI_SOURCE, sADC_SPI_MOSI_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

  /*!< SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = sADC_SPI_SCK_PIN;
  GPIO_Init(sADC_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< SPI MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  sADC_SPI_MOSI_PIN;
  GPIO_Init(sADC_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< SPI MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin =  sADC_SPI_MISO_PIN;
  GPIO_Init(sADC_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure sADC Card CS pin in output pushpull mode ********************/
  GPIO_InitStructure.GPIO_Pin = sADC_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(sADC_CS_GPIO_PORT, &GPIO_InitStructure);
//----------------------------------------------------------------------------
#if SPI_ADC_DMA_ENABLE
  DMA_InitTypeDef DMA_InitStructure;

  /* Enable DMA clock */
  RCC_AHB1PeriphClockCmd(sADC_SPI_DMA_CLK, ENABLE);
  /* DMA configuration -------------------------------------------------------*/
  /* Deinitialize DMA Streams */
  DMA_DeInit(sADC_SPI_TX_DMA_STREAM);
  DMA_DeInit(sADC_SPI_RX_DMA_STREAM);

  /* Configure DMA Initialization Structure */
  DMA_InitStructure.DMA_BufferSize = BUFFERSIZE_ADC ;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(sADC_SPI->DR)) ;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  /* Configure TX DMA */
  DMA_InitStructure.DMA_Channel = sADC_SPI_TX_DMA_CHANNEL ;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;
  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)adc_dma_RxBuffer ;//发送发送数据随便
  DMA_Init(sADC_SPI_TX_DMA_STREAM, &DMA_InitStructure);
  /* Configure RX DMA */
  DMA_InitStructure.DMA_Channel = sADC_SPI_RX_DMA_CHANNEL ;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;
  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)adc_dma_RxBuffer ;
  DMA_Init(sADC_SPI_RX_DMA_STREAM, &DMA_InitStructure);

  #endif

}

/**
  * @brief  DeInitializes the peripherals used by the SPI NET driver.
  * @param  None
  * @retval None
  */
void sADC_LowLevel_DeInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /*!< Disable the sADC_SPI  ************************************************/
  SPI_Cmd(sADC_SPI, DISABLE);

  /*!< DeInitializes the sADC_SPI *******************************************/
  SPI_I2S_DeInit(sADC_SPI);

  /*!< sADC_SPI Periph clock disable ****************************************/
  sADC_SPI_CLK_INIT(sADC_SPI_CLK, DISABLE);

  /*!< Configure all pins used by the SPI as input floating *******************/
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_InitStructure.GPIO_Pin = sADC_SPI_SCK_PIN;
  GPIO_Init(sADC_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = sADC_SPI_MISO_PIN;
  GPIO_Init(sADC_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = sADC_SPI_MOSI_PIN;
  GPIO_Init(sADC_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = sADC_CS_PIN;
  GPIO_Init(sADC_CS_GPIO_PORT, &GPIO_InitStructure);
}

#if 1
#define sADC_GAIN1_PIN                        GPIO_Pin_1
#define sADC_GAIN1_GPIO_PORT                  GPIOB
#define sADC_GAIN1_GPIO_CLK                   RCC_AHB1Periph_GPIOB

#define sADC_GAIN0_PIN                        GPIO_Pin_0
#define sADC_GAIN0_GPIO_PORT                  GPIOB
#define sADC_GAIN0_GPIO_CLK                   RCC_AHB1Periph_GPIOB


void s_adc_gain_pin_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/*!< Enable GPIO clocks */
	  RCC_AHB1PeriphClockCmd(sADC_GAIN1_GPIO_CLK| sADC_GAIN0_GPIO_CLK, ENABLE);

	/*!< Configure sADC Card CS pin in output pushpull mode ********************/
	GPIO_InitStructure.GPIO_Pin = sADC_GAIN1_PIN|sADC_GAIN0_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(sADC_GAIN1_GPIO_PORT, &GPIO_InitStructure);
  s_adc_gain_set(ADC_GAIN_0);
}

void s_adc_gain_set(ADC_GAIN_ENUM id)
{
	switch (id)
  {
  case ADC_GAIN_0:
    GPIO_ResetBits(sADC_GAIN0_GPIO_PORT, sADC_GAIN0_PIN);
    GPIO_ResetBits(sADC_GAIN1_GPIO_PORT, sADC_GAIN1_PIN);
    break;
  case ADC_GAIN_2:
    GPIO_ResetBits(sADC_GAIN0_GPIO_PORT, sADC_GAIN0_PIN);
    GPIO_SetBits(sADC_GAIN1_GPIO_PORT, sADC_GAIN1_PIN);
    break;
  case ADC_GAIN_1:
    GPIO_SetBits(sADC_GAIN0_GPIO_PORT, sADC_GAIN0_PIN);
    GPIO_ResetBits(sADC_GAIN1_GPIO_PORT, sADC_GAIN1_PIN);
    break;
  case ADC_GAIN_3:
    GPIO_SetBits(sADC_GAIN0_GPIO_PORT, sADC_GAIN0_PIN);
    GPIO_SetBits(sADC_GAIN1_GPIO_PORT, sADC_GAIN1_PIN);
    break;

  default:
		break;
  }
}

#endif
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
