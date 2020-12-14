/**
  ******************************************************************************
  * @file    USART/USART_TwoBoards/USART_DataExchangeInterrupt/main.c
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
#include "xwf_pin_map.h"
#include "string.h"
/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */
/** @addtogroup USART_DataExchangeInterrupt
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t aTxBuffer[BUFFERSIZE] ;
__IO uint16_t aTxBuffer_datalen = 0x00;

  DMA_InitTypeDef  DMA_InitStructure;
// uint8_t aRxBuffer [BUFFERSIZE];
// __IO uint8_t ubRxIndex = 0x00;
// __IO uint8_t ubTxIndex = 0x00;
__IO uint32_t TimeOut = 0x00;  
void usart3_dma_start(void);
/* Private function prototypes -----------------------------------------------*/
static void USART_Config(void);
// static void SysTickConfig(void);
#ifdef USART_RECEIVER
static TestStatus Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength);
#endif
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int usart3_init(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       files (startup_stm32f40_41xxx.s/startup_stm32f427_437xx.s/startup_stm32f429_439xx.s)
       before to branch to application main. 
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */
  UartFifoInit();
  /* USART configuration -----------------------------------------------------*/
  USART_Config();
  
  /* SysTick configuration ---------------------------------------------------*/
  // SysTickConfig();
  
  /* LEDs configuration ------------------------------------------------------*/
  // STM_EVAL_LEDInit(LED1);
  // STM_EVAL_LEDInit(LED2);
  // STM_EVAL_LEDInit(LED3);
  
// #ifdef USART_TRANSMITTER
  
  /* Tamper Button Configuration ---------------------------------------------*/
  // STM_EVAL_PBInit(BUTTON_TAMPER,BUTTON_MODE_GPIO);
  
  // /* Wait until Tamper Button is pressed */
  // while (STM_EVAL_PBGetState(BUTTON_TAMPER));  
  
  #if  USART_DMA_TX_ENABEL
  #else
  /* Enable the Tx buffer empty interrupt */
  USART_ITConfig(USARTx, USART_IT_TXE, ENABLE);
  #endif
  
// #endif /* USART_TRANSMITTER */
  
// #ifdef USART_RECEIVER
  
  /* Enable the Tx buffer empty interrupt */
  USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
//   USART_ITConfig(USARTx, USART_IT_IDLE, ENABLE);


}

/**
  * @brief  Configures the USART Peripheral.
  * @param  None
  * @retval None
  */

static void USART_Config(void)
{
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(USARTx_TX_GPIO_CLK | USARTx_RX_GPIO_CLK, ENABLE);
  
  /* Enable USART clock */
  USARTx_CLK_INIT(USARTx_CLK, ENABLE);
  
  /* Connect USART pins to AF7 */
  GPIO_PinAFConfig(USARTx_TX_GPIO_PORT, USARTx_TX_SOURCE, USARTx_TX_AF);
  GPIO_PinAFConfig(USARTx_RX_GPIO_PORT, USARTx_RX_SOURCE, USARTx_RX_AF);
  
  /* Configure USART Tx and Rx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = USARTx_TX_PIN;
  GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = USARTx_RX_PIN;
  GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure);

  /* Enable the USART OverSampling by 8 */
  USART_OverSampling8Cmd(USARTx, ENABLE);  

  /* USARTx configuration ----------------------------------------------------*/
  /* USARTx configured as follows:
        - BaudRate = 5250000 baud
		   - Maximum BaudRate that can be achieved when using the Oversampling by 8
		     is: (USART APB Clock / 8) 
			 Example: 
			    - (USART3 APB1 Clock / 8) = (42 MHz / 8) = 5250000 baud
			    - (USART1 APB2 Clock / 8) = (84 MHz / 8) = 10500000 baud
		   - Maximum BaudRate that can be achieved when using the Oversampling by 16
		     is: (USART APB Clock / 16) 
			 Example: (USART3 APB1 Clock / 16) = (42 MHz / 16) = 2625000 baud
			 Example: (USART1 APB2 Clock / 16) = (84 MHz / 16) = 5250000 baud
        - Word Length = 8 Bits
        - one Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */ 
  USART_InitStructure.USART_BaudRate = 5250000;//921600;//115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USARTx, &USART_InitStructure);


  #if  USART_DMA_TX_ENABEL

  /* Enable the DMA clock */
  RCC_AHB1PeriphClockCmd(USARTx_DMAx_CLK, ENABLE);

  /* Configure DMA Initialization Structure */
  DMA_InitStructure.DMA_BufferSize = BUFFERSIZE ;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(USARTx->DR)) ;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  /* Configure TX DMA */
  DMA_InitStructure.DMA_Channel = USARTx_TX_DMA_CHANNEL;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)aTxBuffer;
  DMA_Init(USARTx_TX_DMA_STREAM, &DMA_InitStructure);

  // /* Configure RX DMA */
  // DMA_InitStructure.DMA_Channel = USARTx_RX_DMA_CHANNEL ;
  // DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;
  // DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)aRxBuffer ;
  // DMA_Init(USARTx_RX_DMA_STREAM,&DMA_InitStructure);
   /* Enable DMA USART TX Stream */
    /* Enable USART */
  

    /* Configure the Priority Group to 2 bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USARTx_DMA_TX_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  DMA_ITConfig(USARTx_TX_DMA_STREAM, DMA_IT_TC, ENABLE);
   
  USART_Cmd(USARTx, ENABLE);


//   usart3_dma_start();

#else

  /* NVIC configuration */
  /* Configure the Priority Group to 2 bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USARTx_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable USART */
  USART_Cmd(USARTx, ENABLE);
  #endif

}
  #if  USART_DMA_TX_ENABEL
void usart3_dma_start(void)
{
  /* Enable DMA USART TX Stream */
  DMA_Cmd(USARTx_TX_DMA_STREAM,ENABLE);
  /* Enable USART DMA TX Requsts */
  USART_DMACmd(USARTx, USART_DMAReq_Tx, ENABLE);

  // /* Waiting the end of Data transfer */
  // while (USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET);    
  // while (DMA_GetFlagStatus(USARTx_TX_DMA_STREAM,USARTx_TX_DMA_FLAG_TCIF)==RESET);
  
  // /* Clear DMA Transfer Complete Flags */
  // DMA_ClearFlag(USARTx_TX_DMA_STREAM,USARTx_TX_DMA_FLAG_TCIF);
  // /* Clear USART Transfer Complete Flags */
  // USART_ClearFlag(USARTx,USART_FLAG_TC);  
}
//add by bcg,2020-12-14 23:54:12 dma usart3 发送完成中断处理,如果没有新的数据就不用再发送
void DMA1_Stream3_IRQHandler(void)
{
  uint16_t   tmp_len;
  if (DMA_GetITStatus(USARTx_TX_DMA_STREAM, DMA_IT_TCIF3) == SET)
	{
    DMA_ClearITPendingBit(USARTx_TX_DMA_STREAM, DMA_IT_TCIF3);
		// if (usart3_get_fifo(&aTxBuffer[0]))
    tmp_len = usart3_dma_get_fifo_data(aTxBuffer, BUFFERSIZE);
		if (tmp_len > 0)
		{
       DMA_InitStructure.DMA_BufferSize = tmp_len ;
       DMA_Init(USARTx_TX_DMA_STREAM, &DMA_InitStructure);
			/* Send Transaction data */
			// USART_SendData(USARTx, tmp);
			//  USART_DMACmd(USARTx, USART_DMAReq_Tx, ENABLE);
       usart3_dma_start();
		}
		else
		{
			/* Disable the Tx buffer empty interrupt */
			//  USART_DMACmd(USARTx, USART_DMAReq_Tx, DISABLE);
		}
	}
}
#endif
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void usart3_SysTick_Handler(void)
{
  /* Decrement the timeout value */
  if (TimeOut != 0x0)
  {
    TimeOut--;
  }
  
  // if (ubCounter < 10)
  // {
  //   ubCounter++;
  // }
  // else
  // {
  //   ubCounter = 0x00;
  //   // STM_EVAL_LEDToggle(LED1);
  // }
}

/**
* @brief  This function handles USRAT interrupt request.
* @param  None
* @retval None
*/
void USART3_IRQHandler_deal(void)
{
	uint8_t   tmp;
  /* USART in Receiver mode */
	if (USART_GetITStatus(USARTx, USART_IT_RXNE) == SET)
	{
		tmp = USART_ReceiveData(USARTx);
		usart3_put_fifo(tmp);
		// if (ubRxIndex < BUFFERSIZE)
		// {
		// 	/* Receive Transaction data */
		// 	aRxBuffer[ubRxIndex++] = USART_ReceiveData(USARTx);
		// }
		// else
		// {
		// 	TimeOut++;
		// 	/* Disable the Rx buffer not empty interrupt */
		// 	//   USART_ITConfig(USARTx, USART_IT_RXNE, DISABLE);
		// }
	}
#if USART_DMA_TX_ENABEL
#else
  /* USART in Transmitter mode */
  if (USART_GetITStatus(USARTx, USART_IT_TXE) == SET)
  {
    if (usart3_get_fifo(&tmp))
    {
      /* Send Transaction data */
      USART_SendData(USARTx, tmp);
    }
    else
    {
      /* Disable the Tx buffer empty interrupt */
      USART_ITConfig(USARTx, USART_IT_TXE, DISABLE);
    }
  }
#endif

  if (USART_GetITStatus(USARTx, USART_IT_IDLE) == SET)
	{
		//空闲中断
		usart3_set_idle();
		USART3->DR;                                       //读取数据。注意：这句必须要，否则不能够清除中断标志位
		USART_ClearITPendingBit(USARTx, USART_IT_IDLE);
	}
}
// }

void usart3_driver_send_enable(void)
{
#if USART_DMA_TX_ENABEL
	usart3_dma_start();
#else
	USART_ITConfig(USARTx, USART_IT_TXE, ENABLE);
#endif
}




/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
