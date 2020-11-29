#include <stdio.h>
#include <string.h>
#include "main.h"
#include "def.h"
#include "global_data.h"
#include "function.h"

UART_HandleTypeDef UartHandle;
//__IO ITStatus UartReady = RESET;

void HAL_UART_MspInit(UART_HandleTypeDef *huart)   //usart3
{
  static DMA_HandleTypeDef hdma_tx;
 
  if(huart->Instance !=USART3)return;
  
  GPIO_InitTypeDef  GPIO_InitStruct;
  __HAL_RCC_GPIOB_CLK_ENABLE(); //gpio PB10 PB11
  __HAL_RCC_USART3_CLK_ENABLE();
  
  /* UART TX RX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  hdma_tx.Instance                 = DMA1_Stream6;
  
  hdma_tx.Init.Channel             = DMA_CHANNEL_4;
  hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_tx.Init.Mode                = DMA_NORMAL;
  hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
  hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
  hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  hdma_tx.Init.MemBurst            = DMA_MBURST_INC4;
  hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC4;
  
  HAL_DMA_Init(&hdma_tx);   
  
  /* Associate the initialized DMA handle to the the UART handle */
  __HAL_LINKDMA(huart, hdmatx, hdma_tx);  
  
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
    
  SET_BIT(huart->Instance->CR1,USART_CR1_RXNEIE);
  HAL_NVIC_SetPriority(USART3_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(USART3_IRQn);
}





void uart3_init(void)
{

  UartHandle.Instance = USART3;
  UartHandle.Init.BaudRate = 115200;
  UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits     = UART_STOPBITS_1;
  UartHandle.Init.Parity       = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode         = UART_MODE_TX_RX;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&UartHandle) != HAL_OK)
  {
   // Error_Handler();
  }
  else
  {
   debug("hello xwf",NODATA);  
  }
}



void UART_Send(uint8_t* pdata, uint32_t size)
{
  while(UartHandle.gState != HAL_UART_STATE_READY);
  HAL_UART_Transmit_DMA(&UartHandle,pdata,size);
  //HAL_Delay(50);
}



void debug0( const char *string,uint16_t data)
{
  uint8_t tail=0,i,buf[10];
  buf[0]='\r';
  buf[1]='\n';
  UART_Send(buf,2);
  UART_Send((void *)string,strlen(string));
  if(data==0x55AA)return;
  
  UART_Send((void *)"=",1);
  i=data/10000;  
  if(i)buf[tail++]=i+0x30;
  data=data%10000;  //5321
  i=data/1000;  
  if(i||tail)buf[tail++]=i+0x30;
  data=data%1000;  //321
  i=data/100;
  if(i||tail)buf[tail++]=i+0x30;
  data=data%100;
  i=data/10;
  if(i||tail)buf[tail++]=i+0x30;
  data=data%10;
  i=data;
  buf[tail++]=i+0x30; 
  UART_Send(buf,tail);
}

void stop_uart3_rx(void)
{
CLEAR_BIT(USART3->CR1,USART_CR1_RE);
CLEAR_BIT(USART3->CR1,USART_CR1_RXNEIE);
}

void en_uart3_rx(void)
{
SET_BIT(USART3->CR1,USART_CR1_RXNEIE);  
SET_BIT(USART3->CR1,USART_CR1_RE);
rx_tail=0;
protocol_overTime=0;
}


