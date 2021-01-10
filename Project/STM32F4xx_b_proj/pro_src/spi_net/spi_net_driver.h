/**
  ******************************************************************************
  * @file    SPI/SPI_NET/spi_NET.h
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   This file contains all the functions prototypes for the spi_NET
  *          firmware driver.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_NET_DRIVER_H
#define __SPI_NET_DRIVER_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* M25P SPI NET supported commands */
#define sNET_CMD_WRITE          0x02  /* Write to Memory instruction */
#define sNET_CMD_WRSR           0x01  /* Write Status Register instruction */
#define sNET_CMD_WREN           0x06  /* Write enable instruction */
#define sNET_CMD_READ           0x03  /* Read from Memory instruction */
#define sNET_CMD_RDSR           0x05  /* Read Status Register instruction  */
#define sNET_CMD_RDID           0x9F  /* Read identification */
#define sNET_CMD_SE             0xD8  /* Sector Erase instruction */
#define sNET_CMD_BE             0xC7  /* Bulk Erase instruction */

#define sNET_WIP_FLAG           0x01  /* Write In Progress (WIP) flag */

#define sNET_DUMMY_BYTE         0xA5a5
#define sNET_SPI_PAGESIZE       0x100

#define sNET_M25P128_ID         0x202018
#define sNET_M25P64_ID          0x202017

/* M25P NET SPI Interface pins  */
#define sNET_SPI                           SPI2
#define sNET_SPI_CLK                       RCC_APB1Periph_SPI2
#define sNET_SPI_CLK_INIT                  RCC_APB1PeriphClockCmd

#define sNET_SPI_SCK_PIN                   GPIO_Pin_1
#define sNET_SPI_SCK_GPIO_PORT             GPIOI
#define sNET_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOI
#define sNET_SPI_SCK_SOURCE                GPIO_PinSource1
#define sNET_SPI_SCK_AF                    GPIO_AF_SPI2

#define sNET_SPI_MISO_PIN                  GPIO_Pin_2
#define sNET_SPI_MISO_GPIO_PORT            GPIOI
#define sNET_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOI
#define sNET_SPI_MISO_SOURCE               GPIO_PinSource2
#define sNET_SPI_MISO_AF                   GPIO_AF_SPI2

#define sNET_SPI_MOSI_PIN                  GPIO_Pin_3
#define sNET_SPI_MOSI_GPIO_PORT            GPIOI
#define sNET_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOI
#define sNET_SPI_MOSI_SOURCE               GPIO_PinSource3
#define sNET_SPI_MOSI_AF                   GPIO_AF_SPI2

#define sNET_CS_PIN                        GPIO_Pin_0
#define sNET_CS_GPIO_PORT                  GPIOI
#define sNET_CS_GPIO_CLK                   RCC_AHB1Periph_GPIOI

#define sNET_DMA                            DMA1
#define sNET_DMA_CLK                        RCC_AHB1Periph_DMA1
#define sNET_TX_DMA_CHANNEL                 DMA_Channel_0
#define sNET_TX_DMA_STREAM                  DMA1_Stream4
#define sNET_TX_DMA_FLAG_TCIF               DMA_FLAG_TCIF4
#define sNET_RX_DMA_CHANNEL                 DMA_Channel_0
#define sNET_RX_DMA_STREAM                  DMA1_Stream3
#define sNET_RX_DMA_FLAG_TCIF               DMA_FLAG_TCIF3
/* Exported macro ------------------------------------------------------------*/
/* Select sNET: Chip Select pin low */
#define sNET_CS_LOW()       GPIO_ResetBits(sNET_CS_GPIO_PORT, sNET_CS_PIN)
/* Deselect sNET: Chip Select pin high */
#define sNET_CS_HIGH()      GPIO_SetBits(sNET_CS_GPIO_PORT, sNET_CS_PIN)


#define SPI_NET_DMA_ENABEL                       1
#define SPI_NET_DMA_BUFFER_SIZE                  (1024*6)
/* Exported functions ------------------------------------------------------- */

/* High layer functions  */
void sNET_DeInit(void);
void sNET_Init(void);
void sNET_EraseSector(uint32_t SectorAddr);
void sNET_EraseBulk(void);
void sNET_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void sNET_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void sNET_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint32_t sNET_ReadID(void);
void sNET_StartReadSequence(uint32_t ReadAddr);

/* Low layer functions */
uint8_t sNET_ReadByte(void);
uint8_t sNET_SendByte(uint8_t byte);
uint16_t sNET_SendHalfWord(uint16_t HalfWord);
void sNET_WriteEnable(void);
void sNET_WaitForWriteEnd(void);
void SPI_net_transfer_block(uint8_t *pBuffer_tx, uint16_t NumByteToTx,uint8_t *pBuffer_rx, uint16_t NumByteToRx);
void SPI_set_slave_select(void);
void SPI_clear_slave_select(void);
void  spi_dma_data_buf_clear(void);
void  spi_dma_put_data_buf(uint8_t *pdt, uint16_t len);
void  spi_dma_write(void);
void spi_dma_read(uint8_t *pRxBuf, uint16_t Rx_size);
uint8_t* get_spi_dma_data_entry(void);
#ifdef __cplusplus
}
#endif

#endif /* __SPI_NET_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
