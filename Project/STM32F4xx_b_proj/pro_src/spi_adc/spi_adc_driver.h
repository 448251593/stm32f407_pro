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
#ifndef __SPI_ADC_DRIVER_H
#define __SPI_ADC_DRIVER_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

typedef enum{
	ADC_GAIN_0 = 0,
	ADC_GAIN_1 ,
	ADC_GAIN_2,
	ADC_GAIN_3,
}ADC_GAIN_ENUM;
#define     SPI_ADC_DMA_ENABLE    0
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* M25P SPI NET supported commands */
#define sADC_CMD_WRITE          0x02  /* Write to Memory instruction */
#define sADC_CMD_WRSR           0x01  /* Write Status Register instruction */
#define sADC_CMD_WREN           0x06  /* Write enable instruction */
#define sADC_CMD_READ           0x03  /* Read from Memory instruction */
#define sADC_CMD_RDSR           0x05  /* Read Status Register instruction  */
#define sADC_CMD_RDID           0x9F  /* Read identification */
#define sADC_CMD_SE             0xD8  /* Sector Erase instruction */
#define sADC_CMD_BE             0xC7  /* Bulk Erase instruction */

#define sADC_WIP_FLAG           0x01  /* Write In Progress (WIP) flag */

#define sADC_DUMMY_BYTE         0xA5A5
#define sADC_SPI_PAGESIZE       0x100

#define sADC_M25P128_ID         0x202018
#define sADC_M25P64_ID          0x202017

#define sADC_GPIOA_PORT_CAT(PARAM)         PARAM_ ## GPIOA

/* M25P NET SPI Interface pins  */
#define sADC_SPI                           SPI1
#define sADC_SPI_CLK                       RCC_APB2Periph_SPI1//RCC_APB1Periph_SPI2
#define sADC_SPI_CLK_INIT                  RCC_APB2PeriphClockCmd//RCC_APB1PeriphClockCmd

#define sADC_SPI_SCK_PIN                   GPIO_Pin_5
#define sADC_SPI_SCK_GPIO_PORT             GPIOA
#define sADC_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOA
#define sADC_SPI_SCK_SOURCE                GPIO_PinSource5
#define sADC_SPI_SCK_AF                    GPIO_AF_SPI1

#define sADC_SPI_MISO_PIN                  GPIO_Pin_6
#define sADC_SPI_MISO_GPIO_PORT            GPIOA
#define sADC_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define sADC_SPI_MISO_SOURCE               GPIO_PinSource6
#define sADC_SPI_MISO_AF                   GPIO_AF_SPI1

#define sADC_SPI_MOSI_PIN                  GPIO_Pin_3
#define sADC_SPI_MOSI_GPIO_PORT            GPIOA
#define sADC_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOI
#define sADC_SPI_MOSI_SOURCE               GPIO_PinSource3
#define sADC_SPI_MOSI_AF                   GPIO_AF_SPI2

#define sADC_CS_PIN                        GPIO_Pin_4
#define sADC_CS_GPIO_PORT                  GPIOA
#define sADC_CS_GPIO_CLK                   RCC_AHB1Periph_GPIOA
//---------------------------dma-----------------------------------------------
#define sADC_SPI_DMA                       DMA2
#define sADC_SPI_DMA_CLK                   RCC_AHB1Periph_DMA2
#define sADC_SPI_TX_DMA_CHANNEL            DMA_Channel_3
#define sADC_SPI_TX_DMA_STREAM             DMA1_Stream3
#define sADC_SPI_TX_DMA_FLAG_TCIF          DMA_FLAG_TCIF4
#define sADC_SPI_RX_DMA_CHANNEL            DMA_Channel_3
#define sADC_SPI_RX_DMA_STREAM             DMA1_Stream2
#define sADC_SPI_RX_DMA_FLAG_TCIF          DMA_FLAG_TCIF3

/* Exported macro ------------------------------------------------------------*/
/* Select sADC: Chip Select pin low */
#define sADC_CS_LOW()       GPIO_ResetBits(sADC_CS_GPIO_PORT, sADC_CS_PIN)
/* Deselect sADC: Chip Select pin high */
#define sADC_CS_HIGH()      GPIO_SetBits(sADC_CS_GPIO_PORT, sADC_CS_PIN)

/* Exported functions ------------------------------------------------------- */

/* High layer functions  */
void sADC_DeInit(void);
void sADC_Init(void);
void sADC_EraseSector(uint32_t SectorAddr);
void sADC_EraseBulk(void);
void sADC_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void sADC_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void sADC_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint32_t sADC_ReadID(void);
void sADC_StartReadSequence(uint32_t ReadAddr);

/* Low layer functions */
uint16_t sADC_ReadByte(void);
uint16_t sADC_SendByte(uint16_t byte);
uint16_t sADC_SendHalfWord(uint16_t HalfWord);
void sADC_WriteEnable(void);
void sADC_WaitForWriteEnd(void);
void SPI_adc_transfer_block(uint8_t *pBuffer_tx, uint16_t NumByteToTx,uint8_t *pBuffer_rx, uint16_t NumByteToRx);
void s_adc_gain_set(ADC_GAIN_ENUM id);
#ifdef __cplusplus
}
#endif

#endif /* __SPI_NET_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
