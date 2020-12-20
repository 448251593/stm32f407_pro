#ifndef   _USART_APP_H_
#define   _USART_APP_H_


// #include "stm32f10x.h"
#include "fifobuffer.h"
#include "stdint.h"
#define  UART1EN    1//串口使能开关
#define  UART2EN    0
#define  UART3EN    0
#define  UART4EN    0



#define   REC_TIME_OUT        10//
#define   UART_BUF_SIZE       100

#define   UART1_BUF_SIZE      1024*80//
#define   USART1_REC_BUF_SIZE      128//

#define   UART2_BUF_SIZE      500//

#define   UART3_BUF_SIZE      512//
#define   UART3_RXBUF_SIZE    60//

#define   UART4_BUF_SIZE      60//
#define   UART4_RXBUF_SIZE    200//

extern FT_FIFO              Uart3RxFifo;
extern unsigned char        Uart3RxBuf[UART3_RXBUF_SIZE];

// extern FT_FIFO              Uart4RxFifo;
// extern unsigned char        Uart4RxBuf[UART4_RXBUF_SIZE];


void   Uart_Config(void);
void   UartSendData( char Uart_num, char *p, uint16_t len);

void UartpollSend(void);

void CreatUartTask(void);
uint8_t usart_get_fifo(uint8_t *pdata);
void Usart3SendData(char *p, uint16_t len);
uint8_t usart3_put_fifo(uint8_t pdt);
uint8_t usart3_parse_cmd(void);
uint32_t  usart3_dma_get_fifo_data(uint8_t *pout, uint32_t  size);
void  usart3send_flush(void);
#endif
