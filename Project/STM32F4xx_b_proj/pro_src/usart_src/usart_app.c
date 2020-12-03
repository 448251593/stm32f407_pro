/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : RECDATADEALTASK.c
* Author             : 毕工
* Version            : V1.0.0
* Date               : 20130607
* Description        : 串口接收数据处理任务
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x.h"
#include "usart_app.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "usart_driver.h"
#include "spi_adc_app.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// #define   UART_SEDNTASK_STK_SIZE   128
// #define   UART_RECTASK_STK_SIZE    128


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// CPU_STK  UartSendTaskStk[UART_SEDNTASK_STK_SIZE];
// OS_TCB   UartSendTaskTCB;
// void     UartSendTask(void *p_arg);

// CPU_STK  UartRecTaskStk[UART_RECTASK_STK_SIZE];/*串口接收处理任务堆栈和控制块*/
// OS_TCB   UartRecTaskTCB;
// void     UartRecTask(void *p_arg);


#if  UART1EN > 0
FT_FIFO              Uart1Fifo;
unsigned char        Uart1SndBuf[UART1_BUF_SIZE];
FT_FIFO              Usart3RxFifo;
unsigned char        Usart3RxBuf[USART1_REC_BUF_SIZE];
#endif





/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



unsigned int Uart4DataParse(FT_FIFO *Ptrfifo);

/*******************************************************************************
* Function Name  : UartFifoInit
* Description    : fifoinit
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void  UartFifoInit(void)
{
#if  UART1EN > 0
    ft_fifo_init(&Uart1Fifo, Uart1SndBuf, UART1_BUF_SIZE);
    ft_fifo_init(&Usart3RxFifo, Usart3RxBuf, USART1_REC_BUF_SIZE);
#endif

}


/*******************************************************************************
* Function Name  : Uart1SendData.
* Description    : 串口1发送
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/


#if  UART1EN > 0
void   Usart3SendData(char *p, uint16_t len)
{

    unsigned  int  templen;
    // CPU_SR_ALLOC();
    // CPU_CRITICAL_ENTER();


    templen = ft_fifo_getlenth(&Uart1Fifo);

    if (len + templen <= UART1_BUF_SIZE - 1)
    {
        ft_fifo_put(&Uart1Fifo,(unsigned char *) p, len);
    }
    // CPU_CRITICAL_EXIT();

    if (ft_fifo_getlenth(&Uart1Fifo) > 0)
    {
        // USART_ITConfig(USART1, USART_IT_TXE, ENABLE); //开启中断发送 使能
        usart3_driver_send_enable();
    }
}
uint8_t usart3_get_fifo(uint8_t *pdata)
{
    unsigned char   tempchar;
    if (ft_fifo_getlenth(&Uart1Fifo) > 0) //缓冲区有数据
    {
        ft_fifo_get(&Uart1Fifo, &tempchar, 0, 1);
        *pdata = tempchar;
        // USART_SendData(USART1, tempchar);
        return 1;
    }
    else
    {
        // USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
        return 0;
    }
}
uint8_t  usart3_put_fifo(uint8_t pdt)
{
    ft_fifo_put(&Usart3RxFifo,(unsigned char *) &pdt, 1);
    return 1;
}
uint8_t  isdigit_check(char *str)
{
    int i, len;
    len = strlen(str);
    for (i = 0;  i < len; i++) {
        if (str[i]<'0' || str[i]>'9') {
            return 0;
        }
    }
    return 1;
}
uint8_t   cmdbuf[128];
uint32_t  param = 0;
uint8_t   usart3_recv_idle = 0;
uint8_t  usart3_set_idle(void)
{
    usart3_recv_idle = 1;
}
uint8_t  usart3_parse_cmd(void)
{
//    int iRet = 0;
    int iBuffLen = 0;
    FT_FIFO *Ptrfifo = &Usart3RxFifo;
    iBuffLen = ft_fifo_getlenth(Ptrfifo);
    char *p1,*p2;
    uint8_t  tmp_buf[100];
//    uint16_t  val;

    if (iBuffLen  > 0 ) //usart3_recv_idle = 1 &&
    {
        usart3_recv_idle = 0;
        memset(cmdbuf, 0, sizeof(cmdbuf));
        if (iBuffLen <= sizeof(cmdbuf))
        {
            ft_fifo_get(Ptrfifo, (fifo_u8 *)cmdbuf, 0, iBuffLen);
        }
        else
        {
            ft_fifo_get(Ptrfifo, (fifo_u8 *)cmdbuf, 0, sizeof(cmdbuf));
        }

        p1 = strstr((const char *)cmdbuf, "set");
        if(p1)
        {
            p2 =  strstr(p1+3, "f=");
            if(p2)
            {
                if(isdigit_check(p2+2))
                {
                    param = atoi(p2+2);
                    sprintf(( char *)tmp_buf, "%d,", param);
                    Usart3SendData((char *)tmp_buf, strlen((const char *)tmp_buf));
                }
            }
             p2 =  strstr((p1+3),"start");
            if(p2)
            {
                extern void  adc_read_start(void);
                adc_read_start();
                printf("start=%d\n", get_global_tick());
            }
            p2 =  strstr((p1+3),"read");
            if(p2)
            {
                print_adc_data();
            }
            // val = spi_adc_read();
            // sprintf(tmp_buf, "%d,", val);
            // Usart3SendData(tmp_buf, strlen(tmp_buf));
        }
    }
    return 0;
}
#endif


/*******************************************************************************
* Function Name  : fputc.
* Description    : 重定向printf的输出
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int fputc(int ch, FILE *f)
{
    char temp;
    //unsigned  int  templen;

    temp = (char) ch;

    // templen = ft_fifo_getlenth(&Uart2Fifo);

    // if (templen + 1 <= UART2_BUF_SIZE - 1)
    // {
    //     ft_fifo_put(&Uart2Fifo,(unsigned char*) &temp, 1);
    // }
    Usart3SendData(&temp, 1);
    return ch;
}
#if 0
/*******************************************************************************
* Function Name  : fgetc.
* Description    : 重定向scanf的输出 没用途
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int fgetc(FILE *f)
{
    return  0;
}
#endif

