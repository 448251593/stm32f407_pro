/**
  
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "spi_net_driver.h"
#include "string.h"

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
#if SPI_NET_DMA_ENABEL
uint8_t pTmpBuf1[SPI_NET_DMA_BUFFER_SIZE];
uint8_t pTmpBufread[128];
uint32_t pTmpBuf1_size = 0;
DMA_InitTypeDef DMA_InitStructure;
#endif
/* Private function prototypes -----------------------------------------------*/
void sNET_LowLevel_DeInit(void);
void sNET_LowLevel_Init(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  DeInitializes the peripherals used by the SPI NET driver.
  * @param  None
  * @retval None
  */
void sNET_DeInit(void)
{
  sNET_LowLevel_DeInit();
}

/**
  * @brief  Initializes the peripherals used by the SPI NET driver.
  * @param  None
  * @retval None
  */
void sNET_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;

  sNET_LowLevel_Init();

  /*!< Deselect the NET: Chip Select high */
  sNET_CS_HIGH();



  /*!< SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  #if  SPI_NET_DMA_ENABEL  //add by bcg,2021-01-06 21:32:05 dma 和轮询读写 初始化方式不一样.具体不知道为什么
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;//SPI_CPHA_2Edge;//SPI_CPHA_1Edge;
  #else
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//SPI_CPHA_1Edge;

  #endif
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(sNET_SPI, &SPI_InitStructure);

  #if  SPI_NET_DMA_ENABEL
  #else
  /*!< Enable the sNET_SPI  */
  SPI_Cmd(sNET_SPI, ENABLE);
  #endif
}



/**
  * @brief  Reads a byte from the SPI NET.
  * @note   This function must be used only if the Start_Read_Sequence function
  *         has been previously called.
  * @param  None
  * @retval Byte Read from the SPI NET.--ok
  */
uint8_t sNET_ReadByte(void)
{
 return (sNET_SendByte(sNET_DUMMY_BYTE));
}

/**
  * @brief  Sends a byte through the SPI interface and return the byte received
  *         from the SPI bus.
  * @param  byte: byte to send.
  * @retval The value of the received byte.--ok
  */
uint8_t sNET_SendByte(uint8_t byte)
{
  /*!< Loop while DR register in not empty */
  while (SPI_I2S_GetFlagStatus(sNET_SPI, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(sNET_SPI, byte);

  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(sNET_SPI, SPI_I2S_FLAG_RXNE) == RESET);

  /*!< Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(sNET_SPI);
}
//add by bcg,2020-09-02 10:22:41

// HAL_SPI_Transmit(&SpiHandle[1], (uint8_t *)&(fptr->tx),fptr->txnum,1000);
// HAL_SPI_Receive(&SpiHandle[1], fptr->rx, fptr->rxnum,1000);
void SPI_net_transfer_block(uint8_t *pBuffer_tx, uint16_t NumByteToTx,uint8_t *pBuffer_rx, uint16_t NumByteToRx)
{
	// while (NumByteToRead--) /*!< while there is data to be read */
	// {
	// 	/*!< Read a byte from the NET */
	// 	*pBuffer = sNET_SendByte(sNET_DUMMY_BYTE);
	// 	/*!< Point to the next location where the byte read will be saved */
	// 	pBuffer++;
	// }
#if SPI_NET_DMA_ENABEL
	
	if (NumByteToRx > 0)//add by bcg,2021-01-06 21:07:06 写就是读
	{
		spi_dma_data_buf_clear();
		spi_dma_put_data_buf(pBuffer_tx, NumByteToTx);
		spi_dma_read(pBuffer_rx, NumByteToRx);
	}
	if (NumByteToTx > 0)
	{
		spi_dma_data_buf_clear();
		spi_dma_put_data_buf(pBuffer_tx, NumByteToTx);
		spi_dma_write();
	}
#else

	while (NumByteToTx--)
	{
		sNET_SendByte(*pBuffer_tx++);
	}

	while (NumByteToRx--)
	{
		*pBuffer_rx = sNET_SendByte(sNET_DUMMY_BYTE);
		/*!< Point to the next location where the byte read will be saved */
		pBuffer_rx++;
	}
#endif
}
/**
  * @brief  Initializes the peripherals used by the SPI NET driver.
  * @param  None
  * @retval None
  */

void sNET_LowLevel_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /*!< Enable the SPI clock */
  sNET_SPI_CLK_INIT(sNET_SPI_CLK, ENABLE);

  /*!< Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(sNET_SPI_SCK_GPIO_CLK | sNET_SPI_MISO_GPIO_CLK |
                         sNET_SPI_MOSI_GPIO_CLK | sNET_CS_GPIO_CLK, ENABLE);

  /*!< SPI pins configuration *************************************************/

  /*!< Connect SPI pins to AF5 */
  GPIO_PinAFConfig(sNET_SPI_SCK_GPIO_PORT, sNET_SPI_SCK_SOURCE, sNET_SPI_SCK_AF);
  GPIO_PinAFConfig(sNET_SPI_MISO_GPIO_PORT, sNET_SPI_MISO_SOURCE, sNET_SPI_MISO_AF);
  GPIO_PinAFConfig(sNET_SPI_MOSI_GPIO_PORT, sNET_SPI_MOSI_SOURCE, sNET_SPI_MOSI_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

  /*!< SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = sNET_SPI_SCK_PIN;
  GPIO_Init(sNET_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< SPI MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  sNET_SPI_MOSI_PIN;
  GPIO_Init(sNET_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< SPI MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin =  sNET_SPI_MISO_PIN;
  GPIO_Init(sNET_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure sNET Card CS pin in output pushpull mode ********************/
  GPIO_InitStructure.GPIO_Pin = sNET_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(sNET_CS_GPIO_PORT, &GPIO_InitStructure);


  #if  SPI_NET_DMA_ENABEL

  /* Peripheral Clock Enable -------------------------------------------------*/

  /* Enable DMA clock */
  RCC_AHB1PeriphClockCmd(sNET_DMA_CLK, ENABLE);

   /* Deinitialize DMA Streams */
  DMA_DeInit(sNET_TX_DMA_STREAM);
  DMA_DeInit(sNET_RX_DMA_STREAM);
  
  /* Configure DMA Initialization Structure */

  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(sNET_SPI->DR)) ;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;

  /* Configure TX DMA */
  DMA_InitStructure.DMA_BufferSize = 0 ;
  DMA_InitStructure.DMA_Channel = sNET_TX_DMA_CHANNEL ;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;
  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)0 ;
  DMA_Init(sNET_TX_DMA_STREAM, &DMA_InitStructure);

  /* Configure RX DMA */
  DMA_InitStructure.DMA_BufferSize = 0 ;
  DMA_InitStructure.DMA_Channel = sNET_RX_DMA_CHANNEL ;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;
  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)0 ; 
  DMA_Init(sNET_RX_DMA_STREAM, &DMA_InitStructure);
  #endif
}
  #if  SPI_NET_DMA_ENABEL
void  spi_dma_data_buf_clear(void)
{
	pTmpBuf1_size = 0;
}
void  spi_dma_put_data_buf(uint8_t *pdt, uint16_t len)
{
	if (SPI_NET_DMA_BUFFER_SIZE - pTmpBuf1_size >= len)
	{
		memcpy(&pTmpBuf1[pTmpBuf1_size], pdt, len);
		pTmpBuf1_size += len;
	}
}
void spi_dma_start(void)
{
	/* Enable DMA SPI TX Stream */
	DMA_Cmd(sNET_TX_DMA_STREAM, ENABLE);
	/* Enable DMA SPI RX Stream */
	DMA_Cmd(sNET_RX_DMA_STREAM, ENABLE);
	/* Enable SPI DMA TX Requsts */
	SPI_I2S_DMACmd(sNET_SPI, SPI_I2S_DMAReq_Tx, ENABLE);
	/* Enable SPI DMA RX Requsts */
	SPI_I2S_DMACmd(sNET_SPI, SPI_I2S_DMAReq_Rx, ENABLE);
	/* Enable the SPI peripheral */
	SPI_Cmd(sNET_SPI, ENABLE);

	/* Waiting the end of Data transfer */
	while (DMA_GetFlagStatus(sNET_TX_DMA_STREAM, sNET_TX_DMA_FLAG_TCIF) == RESET)
	{
		// get_adc_data_200khz();
	}
	while (DMA_GetFlagStatus(sNET_RX_DMA_STREAM, sNET_RX_DMA_FLAG_TCIF) == RESET)
	{
		// get_adc_data_200khz();
	}

	/* Clear DMA Transfer Complete Flags */
	DMA_ClearFlag(sNET_TX_DMA_STREAM, sNET_TX_DMA_FLAG_TCIF);
	DMA_ClearFlag(sNET_RX_DMA_STREAM, sNET_RX_DMA_FLAG_TCIF);

	/* Disable DMA SPI TX Stream */
	DMA_Cmd(sNET_TX_DMA_STREAM, DISABLE);

	/* Disable DMA SPI RX Stream */
	DMA_Cmd(sNET_RX_DMA_STREAM, DISABLE);

	/* Disable SPI DMA TX Requsts */
	SPI_I2S_DMACmd(sNET_SPI, SPI_I2S_DMAReq_Tx, DISABLE);

	/* Disable SPI DMA RX Requsts */
	SPI_I2S_DMACmd(sNET_SPI, SPI_I2S_DMAReq_Rx, DISABLE);

	/* Disable the SPI peripheral */
	SPI_Cmd(sNET_SPI, DISABLE);

}
void  spi_dma_write_for_send(uint8_t *pbuf, uint32_t data_size)
{
	DMA_InitStructure.DMA_BufferSize = data_size;
	/* Configure TX DMA */
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Channel = sNET_TX_DMA_CHANNEL;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)pbuf;
	DMA_Init(sNET_TX_DMA_STREAM, &DMA_InitStructure);

	/* Configure RX DMA */
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStructure.DMA_Channel = sNET_RX_DMA_CHANNEL;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)pTmpBufread;
	DMA_Init(sNET_RX_DMA_STREAM, &DMA_InitStructure);

	spi_dma_start();
}
void  spi_dma_write(void)
{
	DMA_InitStructure.DMA_BufferSize = pTmpBuf1_size;
	/* Configure TX DMA */
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Channel = sNET_TX_DMA_CHANNEL;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)pTmpBuf1;
	DMA_Init(sNET_TX_DMA_STREAM, &DMA_InitStructure);

	/* Configure RX DMA */
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStructure.DMA_Channel = sNET_RX_DMA_CHANNEL;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)pTmpBufread;
	DMA_Init(sNET_RX_DMA_STREAM, &DMA_InitStructure);

	spi_dma_start();
}

void spi_dma_read(uint8_t *pRxBuf, uint16_t Rx_size)
{
	// memset(pTmpBuf1, 0, SPI_NET_DMA_BUFFER_SIZE);
	// memcpy(pTmpBuf1, pTxBuf, tx_len);

	DMA_InitStructure.DMA_BufferSize = Rx_size + pTmpBuf1_size;
	/* Configure TX DMA */
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Channel = sNET_TX_DMA_CHANNEL;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)pTmpBuf1;
	DMA_Init(sNET_TX_DMA_STREAM, &DMA_InitStructure);

	/* Configure RX DMA */
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Channel = sNET_RX_DMA_CHANNEL;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)pTmpBufread;
	DMA_Init(sNET_RX_DMA_STREAM, &DMA_InitStructure);
	spi_dma_start();
	memcpy(pRxBuf, pTmpBufread+pTmpBuf1_size, Rx_size);
}
#endif
/**
  * @brief  DeInitializes the peripherals used by the SPI NET driver.
  * @param  None
  * @retval None
  */
void sNET_LowLevel_DeInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /*!< Disable the sNET_SPI  ************************************************/
  SPI_Cmd(sNET_SPI, DISABLE);

  /*!< DeInitializes the sNET_SPI *******************************************/
  SPI_I2S_DeInit(sNET_SPI);

  /*!< sNET_SPI Periph clock disable ****************************************/
  sNET_SPI_CLK_INIT(sNET_SPI_CLK, DISABLE);

  /*!< Configure all pins used by the SPI as input floating *******************/
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_InitStructure.GPIO_Pin = sNET_SPI_SCK_PIN;
  GPIO_Init(sNET_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = sNET_SPI_MISO_PIN;
  GPIO_Init(sNET_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = sNET_SPI_MOSI_PIN;
  GPIO_Init(sNET_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = sNET_CS_PIN;
  GPIO_Init(sNET_CS_GPIO_PORT, &GPIO_InitStructure);
}
//add by bcg,2020-09-02 14:13:54---
inline void SPI_set_slave_select(void)
{
	sNET_CS_LOW();
}
inline void SPI_clear_slave_select(void)
{
	sNET_CS_HIGH();
}
void SPI_net_rst_set(uint8_t f)
{
	if (f == 0)
	{
		// GPIO_ResetBits(sNET_RESET_GPIO_PORT, sNET_CS_PIN);
		led_on(GPIO_Pin_15);
	}
	else
	{
		// GPIO_SetBits(sNET_RESET_GPIO_PORT, sNET_CS_PIN);
		led_off(GPIO_Pin_15);
	}
}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
