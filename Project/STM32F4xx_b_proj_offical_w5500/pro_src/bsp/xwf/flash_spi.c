#include <stdio.h>
#include <string.h>
#include "main.h"
#include "def.h"
#include "global_data.h"
#include "function.h"



DMA_HandleTypeDef hdma_spi3_rx;
DMA_HandleTypeDef hdma_spi3_tx;

//DMA_HandleTypeDef hdma_spi1_tx;  

 
void SpiFlashDma_init()
{
  spi_flash.Instance               = SPI3;
  spi_flash.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  spi_flash.Init.Direction         = SPI_DIRECTION_2LINES;
  spi_flash.Init.CLKPhase          = SPI_PHASE_1EDGE;
  spi_flash.Init.CLKPolarity       = SPI_POLARITY_HIGH;
  spi_flash.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  spi_flash.Init.CRCPolynomial     = 7;
  spi_flash.Init.DataSize          = SPI_DATASIZE_8BIT;  //SPI_DATASIZE_16BIT
  spi_flash.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  spi_flash.Init.NSS               = SPI_NSS_SOFT;
  spi_flash.Init.TIMode            = SPI_TIMODE_DISABLE;
  
  spi_flash.Init.Mode = SPI_MODE_MASTER;

  if(HAL_SPI_Init(&spi_flash) != HAL_OK)
  {
    /* Initialization Error */
   // Error_Handler();
  }
  else
  flash_init();
  
}

void HAL_SPI3_MspInit(SPI_HandleTypeDef *hspi)
{
GPIO_InitTypeDef  GPIO_InitStruct;
	
 __HAL_RCC_SPI3_CLK_ENABLE();
 __HAL_RCC_DMA1_CLK_ENABLE();
  
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);   
    free_spi_cs(mx_cs);
	free_spi_cs(sram_cs);
      
    hdma_spi3_rx.Instance = DMA1_Stream3;
    hdma_spi3_rx.Init.Channel= DMA_CHANNEL_0;
    hdma_spi3_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_spi3_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi3_rx.Init.MemInc    = DMA_MINC_ENABLE;
    hdma_spi3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi3_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_spi3_rx.Init.Mode                = DMA_NORMAL;
    hdma_spi3_rx.Init.Priority            = DMA_PRIORITY_HIGH;
    hdma_spi3_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;         
    hdma_spi3_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    hdma_spi3_rx.Init.MemBurst            = DMA_MBURST_INC4;
    hdma_spi3_rx.Init.PeriphBurst         = DMA_PBURST_INC4; 

    HAL_DMA_Init(&hdma_spi3_rx);
    __HAL_LINKDMA(hspi,hdmarx,hdma_spi3_rx);
    
    hdma_spi3_tx.Instance = DMA1_Stream4;
    hdma_spi3_tx.Init.Channel             = DMA_CHANNEL_0;
    hdma_spi3_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_spi3_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_spi3_tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_spi3_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi3_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_spi3_tx.Init.Mode                = DMA_NORMAL;
    hdma_spi3_tx.Init.Priority            = DMA_PRIORITY_LOW;
    hdma_spi3_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;         
    hdma_spi3_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    hdma_spi3_tx.Init.MemBurst            = DMA_MBURST_INC4;
    hdma_spi3_tx.Init.PeriphBurst         = DMA_PBURST_INC4;
  
    HAL_DMA_Init(&hdma_spi3_tx);   
  
  /* Associate the initialized DMA handle to the the SPI handle */
   __HAL_LINKDMA(hspi, hdmatx, hdma_spi3_tx);
  
  
 
  /*##-4- Configure the NVIC for DMA #########################################*/ 
  /* NVIC configuration for DMA transfer complete interrupt (SPI3_TX) */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
    
  /* NVIC configuration for DMA transfer complete interrupt (SPI3_RX) */
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);   
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
  
  /*##-5- Configure the NVIC for SPI #########################################*/
//  HAL_NVIC_SetPriority(SPI3_IRQn, 0, 2);
//  HAL_NVIC_EnableIRQ(SPI3_IRQn);
}

