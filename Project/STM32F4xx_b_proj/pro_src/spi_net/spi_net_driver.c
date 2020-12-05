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
  *          performed in sNET_LowLevel_Init() function.
  *
  *          You can easily tailor this driver to any development board, by just
  *          adapting the defines for hardware resources and sNET_LowLevel_Init()
  *          function.
  *
  *          +-----------------------------------------------------------+
  *          |                     Pin assignment                        |
  *          +-----------------------------+---------------+-------------+
  *          |  STM32 SPI Pins             |     sNET    |    Pin      |
  *          +-----------------------------+---------------+-------------+
  *          | sNET_CS_PIN               | ChipSelect(/S)|    1        |
  *          | sNET_SPI_MISO_PIN / MISO  |   DataOut(Q)  |    2        |
  *          |                             |   VCC         |    3 (3.3 V)|
  *          |                             |   GND         |    4 (0 V)  |
  *          | sNET_SPI_MOSI_PIN / MOSI  |   DataIn(D)   |    5        |
  *          | sNET_SPI_SCK_PIN / SCK    |   Clock(C)    |    6        |
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
#include "spi_net_driver.h"

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
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(sNET_SPI, &SPI_InitStructure);

  /*!< Enable the sNET_SPI  */
  SPI_Cmd(sNET_SPI, ENABLE);
}
#if 0
/**
  * @brief  Erases the specified NET sector.
  * @param  SectorAddr: address of the sector to erase.
  * @retval None
  */
void sNET_EraseSector(uint32_t SectorAddr)
{
  /*!< Send write enable instruction */
  sNET_WriteEnable();

  /*!< Sector Erase */
  /*!< Select the NET: Chip Select low */
  sNET_CS_LOW();
  /*!< Send Sector Erase instruction */
  sNET_SendByte(sNET_CMD_SE);
  /*!< Send SectorAddr high nibble address byte */
  sNET_SendByte((SectorAddr & 0xFF0000) >> 16);
  /*!< Send SectorAddr medium nibble address byte */
  sNET_SendByte((SectorAddr & 0xFF00) >> 8);
  /*!< Send SectorAddr low nibble address byte */
  sNET_SendByte(SectorAddr & 0xFF);
  /*!< Deselect the NET: Chip Select high */
  sNET_CS_HIGH();

  /*!< Wait the end of NET writing */
  sNET_WaitForWriteEnd();
}

/**
  * @brief  Erases the entire NET.
  * @param  None
  * @retval None
  */
void sNET_EraseBulk(void)
{
  /*!< Send write enable instruction */
  sNET_WriteEnable();

  /*!< Bulk Erase */
  /*!< Select the NET: Chip Select low */
  sNET_CS_LOW();
  /*!< Send Bulk Erase instruction  */
  sNET_SendByte(sNET_CMD_BE);
  /*!< Deselect the NET: Chip Select high */
  sNET_CS_HIGH();

  /*!< Wait the end of NET writing */
  sNET_WaitForWriteEnd();
}

/**
  * @brief  Writes more than one byte to the NET with a single WRITE cycle
  *         (Page WRITE sequence).
  * @note   The number of byte can't exceed the NET page size.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the NET.
  * @param  WriteAddr: NET's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the NET, must be equal
  *         or less than "sNET_PAGESIZE" value.
  * @retval None
  */
void sNET_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  /*!< Enable the write access to the NET */
  sNET_WriteEnable();

  /*!< Select the NET: Chip Select low */
  sNET_CS_LOW();
  /*!< Send "Write to Memory " instruction */
  sNET_SendByte(sNET_CMD_WRITE);
  /*!< Send WriteAddr high nibble address byte to write to */
  sNET_SendByte((WriteAddr & 0xFF0000) >> 16);
  /*!< Send WriteAddr medium nibble address byte to write to */
  sNET_SendByte((WriteAddr & 0xFF00) >> 8);
  /*!< Send WriteAddr low nibble address byte to write to */
  sNET_SendByte(WriteAddr & 0xFF);

  /*!< while there is data to be written on the NET */
  while (NumByteToWrite--)
  {
    /*!< Send the current byte */
    sNET_SendByte(*pBuffer);
    /*!< Point on the next byte to be written */
    pBuffer++;
  }

  /*!< Deselect the NET: Chip Select high */
  sNET_CS_HIGH();

  /*!< Wait the end of NET writing */
  sNET_WaitForWriteEnd();
}

/**
  * @brief  Writes block of data to the NET. In this function, the number of
  *         WRITE cycles are reduced, using Page WRITE sequence.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the NET.
  * @param  WriteAddr: NET's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the NET.
  * @retval None
  */
void sNET_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % sNET_SPI_PAGESIZE;
  count = sNET_SPI_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / sNET_SPI_PAGESIZE;
  NumOfSingle = NumByteToWrite % sNET_SPI_PAGESIZE;

  if (Addr == 0) /*!< WriteAddr is sNET_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sNET_PAGESIZE */
    {
      sNET_WritePage(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /*!< NumByteToWrite > sNET_PAGESIZE */
    {
      while (NumOfPage--)
      {
        sNET_WritePage(pBuffer, WriteAddr, sNET_SPI_PAGESIZE);
        WriteAddr +=  sNET_SPI_PAGESIZE;
        pBuffer += sNET_SPI_PAGESIZE;
      }

      sNET_WritePage(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /*!< WriteAddr is not sNET_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sNET_PAGESIZE */
    {
      if (NumOfSingle > count) /*!< (NumByteToWrite + WriteAddr) > sNET_PAGESIZE */
      {
        temp = NumOfSingle - count;

        sNET_WritePage(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        sNET_WritePage(pBuffer, WriteAddr, temp);
      }
      else
      {
        sNET_WritePage(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /*!< NumByteToWrite > sNET_PAGESIZE */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / sNET_SPI_PAGESIZE;
      NumOfSingle = NumByteToWrite % sNET_SPI_PAGESIZE;

      sNET_WritePage(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        sNET_WritePage(pBuffer, WriteAddr, sNET_SPI_PAGESIZE);
        WriteAddr +=  sNET_SPI_PAGESIZE;
        pBuffer += sNET_SPI_PAGESIZE;
      }

      if (NumOfSingle != 0)
      {
        sNET_WritePage(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

/**
  * @brief  Reads a block of data from the NET.
  * @param  pBuffer: pointer to the buffer that receives the data read from the NET.
  * @param  ReadAddr: NET's internal address to read from.
  * @param  NumByteToRead: number of bytes to read from the NET.--ok
  * @retval None
  */
void sNET_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  /*!< Select the NET: Chip Select low */
  sNET_CS_LOW();

  /*!< Send "Read from Memory " instruction */
  sNET_SendByte(sNET_CMD_READ);

  /*!< Send ReadAddr high nibble address byte to read from */
  sNET_SendByte((ReadAddr & 0xFF0000) >> 16);
  /*!< Send ReadAddr medium nibble address byte to read from */
  sNET_SendByte((ReadAddr& 0xFF00) >> 8);
  /*!< Send ReadAddr low nibble address byte to read from */
  sNET_SendByte(ReadAddr & 0xFF);

  while (NumByteToRead--) /*!< while there is data to be read */
  {
    /*!< Read a byte from the NET */
    *pBuffer = sNET_SendByte(sNET_DUMMY_BYTE);
    /*!< Point to the next location where the byte read will be saved */
    pBuffer++;
  }

  /*!< Deselect the NET: Chip Select high */
  sNET_CS_HIGH();
}

/**
  * @brief  Reads NET identification.
  * @param  None
  * @retval NET identification
  */
uint32_t sNET_ReadID(void)
{
  uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /*!< Select the NET: Chip Select low */
  sNET_CS_LOW();

  /*!< Send "RDID " instruction */
  sNET_SendByte(0x9F);

  /*!< Read a byte from the NET */
  Temp0 = sNET_SendByte(sNET_DUMMY_BYTE);

  /*!< Read a byte from the NET */
  Temp1 = sNET_SendByte(sNET_DUMMY_BYTE);

  /*!< Read a byte from the NET */
  Temp2 = sNET_SendByte(sNET_DUMMY_BYTE);

  /*!< Deselect the NET: Chip Select high */
  sNET_CS_HIGH();

  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;
}

/**
  * @brief  Initiates a read data byte (READ) sequence from the NET.
  *   This is done by driving the /CS line low to select the device, then the READ
  *   instruction is transmitted followed by 3 bytes address. This function exit
  *   and keep the /CS line low, so the NET still being selected. With this
  *   technique the whole content of the NET is read with a single READ instruction.
  * @param  ReadAddr: NET's internal address to read from.
  * @retval None
  */
void sNET_StartReadSequence(uint32_t ReadAddr)
{
  /*!< Select the NET: Chip Select low */
  sNET_CS_LOW();

  /*!< Send "Read from Memory " instruction */
  sNET_SendByte(sNET_CMD_READ);

  /*!< Send the 24-bit address of the address to read from -------------------*/
  /*!< Send ReadAddr high nibble address byte */
  sNET_SendByte((ReadAddr & 0xFF0000) >> 16);
  /*!< Send ReadAddr medium nibble address byte */
  sNET_SendByte((ReadAddr& 0xFF00) >> 8);
  /*!< Send ReadAddr low nibble address byte */
  sNET_SendByte(ReadAddr & 0xFF);
}


/**
  * @brief  Sends a Half Word through the SPI interface and return the Half Word
  *         received from the SPI bus.
  * @param  HalfWord: Half Word to send.
  * @retval The value of the received Half Word.
  */
uint16_t sNET_SendHalfWord(uint16_t HalfWord)
{
  /*!< Loop while DR register in not empty */
  while (SPI_I2S_GetFlagStatus(sNET_SPI, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send Half Word through the sNET peripheral */
  SPI_I2S_SendData(sNET_SPI, HalfWord);

  /*!< Wait to receive a Half Word */
  while (SPI_I2S_GetFlagStatus(sNET_SPI, SPI_I2S_FLAG_RXNE) == RESET);

  /*!< Return the Half Word read from the SPI bus */
  return SPI_I2S_ReceiveData(sNET_SPI);
}

/**
  * @brief  Enables the write access to the NET.
  * @param  None
  * @retval None
  */
void sNET_WriteEnable(void)
{
  /*!< Select the NET: Chip Select low */
  sNET_CS_LOW();

  /*!< Send "Write Enable" instruction */
  sNET_SendByte(sNET_CMD_WREN);

  /*!< Deselect the NET: Chip Select high */
  sNET_CS_HIGH();
}

/**
  * @brief  Polls the status of the Write In Progress (WIP) flag in the NET's
  *         status register and loop until write operation has completed.
  * @param  None
  * @retval None
  */
void sNET_WaitForWriteEnd(void)
{
  uint8_t NETstatus = 0;

  /*!< Select the NET: Chip Select low */
  sNET_CS_LOW();

  /*!< Send "Read Status Register" instruction */
  sNET_SendByte(sNET_CMD_RDSR);

  /*!< Loop as long as the memory is busy with a write cycle */
  do
  {
    /*!< Send a dummy byte to generate the clock needed by the NET
    and put the value of the status register in NET_Status variable */
    NETstatus = sNET_SendByte(sNET_DUMMY_BYTE);

  }
  while ((NETstatus & sNET_WIP_FLAG) == SET); /* Write in progress */

  /*!< Deselect the NET: Chip Select high */
  sNET_CS_HIGH();
}
#endif


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
}

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

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
