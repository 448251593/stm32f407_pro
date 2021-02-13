#include <stdio.h>
#include <string.h>
#include "main.h"
#include "def.h"
#include "global_data.h"
#include "function.h"

void HAL_SPI2_MspInit(SPI_HandleTypeDef *hspi);	
void HAL_SPI3_MspInit(SPI_HandleTypeDef *hspi);	


void SpiAdcDma_init(void)
{
  spi_adc.Instance               = SPI1;
  spi_adc.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  spi_adc.Init.Direction         = SPI_DIRECTION_2LINES;
  spi_adc.Init.CLKPhase          = SPI_PHASE_1EDGE;
  spi_adc.Init.CLKPolarity       = SPI_POLARITY_HIGH;
  spi_adc.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  spi_adc.Init.CRCPolynomial     = 7;
  spi_adc.Init.DataSize          = SPI_DATASIZE_16BIT;  //SPI_DATASIZE_16BIT
  spi_adc.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  spi_adc.Init.NSS               = SPI_NSS_SOFT;
  spi_adc.Init.TIMode            = SPI_TIMODE_DISABLE;
  
  spi_adc.Init.Mode = SPI_MODE_MASTER;

  if(HAL_SPI_Init(&spi_adc) != HAL_OK)
  {
    /* Initialization Error */
   // Error_Handler();
  }
  
}

void HAL_SPI1_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef  GPIO_InitStruct;	
 
   GPIO_InitStruct.Pin       = adc_spiclk|adc_spimiso;
   GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull      = GPIO_NOPULL;
   GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
   GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
   HAL_GPIO_Init(adc_spiport, &GPIO_InitStruct);
   __HAL_RCC_SPI1_CLK_ENABLE();     	
	
}


void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{

	
   if (hspi->Instance == SPI1)  //adc
   {
  /* Enable DMA1 clock */
   //__HAL_RCC_DMA1_CLK_ENABLE();   
   HAL_SPI1_MspInit(hspi);
   }
   if (hspi->Instance == SPI2)  //net
	{
	//HAL_SPI2_MspInit(hspi);	
	}
   
    if (hspi->Instance == SPI3)  //flash & ram
	{
	HAL_SPI3_MspInit(hspi);	
	}
}

void ad7276_sample(void)
{
	volatile SPI_TypeDef * pSpi = SPI1;

	uint16_t timeout = 200;
     static uint16_t tmp=0;
     	
	if(((pSpi->CR1) & (SPI_CR1_SPE)) != (SPI_CR1_SPE))
	{
		pSpi->CR1 |= SPI_CR1_SPE;
	}	
	while((((pSpi->SR) & (SPI_SR_TXE)) != (SPI_SR_TXE))&&(timeout>0))
	{
		timeout--;
	}
	if(((pSpi->SR) & (SPI_SR_TXE)) == (SPI_SR_TXE))
	{
		pSpi->DR = 0xffff;
		timeout= 2000;
	}
	while((((pSpi->SR) & (SPI_SR_TXE)) != (SPI_SR_TXE))&&(timeout>0))
	{
		timeout--;
	}
	timeout = 200;
	while((((pSpi->SR) & (SPI_SR_RXNE)) != (SPI_SR_RXNE))&&(timeout>0))
	{
		timeout --;
	}
	if(((pSpi->SR) & (SPI_SR_RXNE)) == (SPI_SR_RXNE))
	{
	    //return(pSpi->DR);
        /* if(sample%3==0)tmp=0x01+sample;
         else if(sample%3==1)tmp=0x2+sample;
         else if(sample%3==2)tmp=0x3+sample;*/
         tmp=pSpi->DR;
        // tmp=swapword(tmp);   
         ad_data[sample++]=tmp;//pSpi->DR;
         //sample++;
	}
    else
         ad_data[sample++]=0x7777;
}

