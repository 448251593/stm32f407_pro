/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : RECDATADEALTASK.c
* Author             : �Ϲ�
* Version            : V1.0.0
* Date               : 20130607
* Description        : ���ڽ������ݴ�������
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x.h"
#include "usart_app.h"
#include <stdint.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// #define   UART_SEDNTASK_STK_SIZE   128
// #define   UART_RECTASK_STK_SIZE    128


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// CPU_STK  UartSendTaskStk[UART_SEDNTASK_STK_SIZE];
// OS_TCB   UartSendTaskTCB;
// void     UartSendTask(void *p_arg);

// CPU_STK  UartRecTaskStk[UART_RECTASK_STK_SIZE];/*���ڽ��մ��������ջ�Ϳ��ƿ�*/
// OS_TCB   UartRecTaskTCB;
// void     UartRecTask(void *p_arg);


#if  UART1EN > 0
FT_FIFO              Uart1Fifo;
unsigned char        Uart1SndBuf[UART1_BUF_SIZE];
#endif

#if UART2EN > 0
FT_FIFO              Uart2Fifo;
unsigned char        Uart2SndBuf[UART2_BUF_SIZE];
#endif

#if UART3EN > 0
FT_FIFO              Uart3Fifo;
unsigned char        Uart3SndBuf[UART3_BUF_SIZE];
FT_FIFO              Uart3RxFifo;
unsigned char        Uart3RxBuf[UART3_RXBUF_SIZE];
#endif

#if UART4EN > 0
FT_FIFO              Uart4Fifo;
unsigned char        Uart4SndBuf[UART4_BUF_SIZE];
FT_FIFO              Uart4RxFifo;
unsigned char        Uart4RxBuf[UART4_RXBUF_SIZE];
#endif



/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#if  UART1EN > 0
static void   Uart1SendData(char *p, uint16_t len);
#endif
#if  UART2EN > 0
static void   Uart2SendData(char *p, uint16_t len);
#endif
#if  UART3EN > 0
static void   Uart3SendData(char *p, uint16_t len);
#endif
#if  UART4EN > 0
static void   Uart4SendData(char *p, uint16_t len);
#endif

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
#endif
#if UART2EN > 0
    ft_fifo_init(&Uart2Fifo, Uart2SndBuf, UART2_BUF_SIZE);
#endif

#if UART3EN > 0
    ft_fifo_init(&Uart3Fifo, Uart3SndBuf, UART3_BUF_SIZE);
    ft_fifo_init(&Uart3RxFifo, Uart3RxBuf, UART3_RXBUF_SIZE);
#endif

#if UART4EN > 0
    ft_fifo_init(&Uart4Fifo, Uart4SndBuf, UART4_BUF_SIZE);
    ft_fifo_init(&Uart4RxFifo, Uart4RxBuf, UART4_RXBUF_SIZE);
#endif
}


/*******************************************************************************
* Function Name  : Uart1SendData.
* Description    : ����1����
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
        // USART_ITConfig(USART1, USART_IT_TXE, ENABLE); //�����жϷ��� ʹ��
        usart3_driver_send_enable();
    }
}
uint8_t usart3_get_fifo(uint8_t *pdata)
{
    unsigned char   tempchar;
    if (ft_fifo_getlenth(&Uart1Fifo) > 0) //������������
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

#endif

/*******************************************************************************
* Function Name  : Uart2SendData.
* Description    : ����2����
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
#if UART2EN > 0
void   Uart2SendData(char *p, uint16_t len)
{
    unsigned  int  templen;

    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();

    templen = ft_fifo_getlenth(&Uart2Fifo);

    if (len + templen <= UART2_BUF_SIZE - 1)
    {
        ft_fifo_put(&Uart2Fifo, (unsigned char *)p, len);
    }

    CPU_CRITICAL_EXIT();
}
#endif

#if UART3EN > 0

/*******************************************************************************
* Function Name  : Uart3SendData.
* Description    : ����3����
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void   Uart3SendData(char *p, uint16_t len)
{
    unsigned  int  templen;

    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();

    templen = ft_fifo_getlenth(&Uart3Fifo);

    if (len + templen <= UART3_BUF_SIZE - 1)
    {
        ft_fifo_put(&Uart3Fifo,(unsigned char *) p, len);
    }
    CPU_CRITICAL_EXIT();
}
#endif

#if UART4EN > 0

/*******************************************************************************
* Function Name  : Uart4SendData.
* Description    : ����4����
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void   Uart4SendData(char *p, uint16_t len)
{
    unsigned  int  templen;

    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();
    templen = ft_fifo_getlenth(&Uart4Fifo);

    if (len + templen <= UART4_BUF_SIZE - 1)
    {
        ft_fifo_put(&Uart4Fifo,(unsigned char *) p, len);
    }

    CPU_CRITICAL_EXIT();
}
#endif

#if 0
/*******************************************************************************
* Function Name  : Uart_Config.
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void  Uart_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
#if UART2EN > 0
    /*-------------��������ʱ�Ӻ�����gpioʱ ��  ----------------*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);//����ӳ��ܽ�
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);//ʹ��uart2ʱ��

    USART_DeInit(USART2);//�崮��

    /*ioģʽ����----����ӳ��---pd5-pd6*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /*��������*/
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;


    USART_Init(USART2, &USART_InitStructure);

    /* Enable USART2 Receive  interrupts ʹ�ܽ����ж� */
//    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    /* Enable the USART2 */
    USART_Cmd(USART2, ENABLE);
#endif

#if  UART1EN > 0

    /***************************����1*********************************/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//ʹ��uart1ʱ��

    USART_DeInit(USART1);//�崮��1

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOA , &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*��������*/
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &USART_InitStructure);

    /* Enable USART1 Receive  interrupts ʹ�ܽ����ж� */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    /* Enable the USAR1 */
    USART_Cmd(USART1, ENABLE);
#endif

#if UART3EN > 0
    /*------------------------------����3---------------------------------*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);//ʹ��uart3ʱ��

    USART_DeInit(USART3);//�崮��3

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOB , &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /*��������*/
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART3, &USART_InitStructure);

    /* Enable USART1 Receive  interrupts ʹ�ܽ����ж� */
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    /* Enable the USAR1 */
    USART_Cmd(USART3, ENABLE);
#endif

#if UART4EN > 0
    /*------------------------------����4--------------------------------*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);//ʹ��uart3ʱ��

    USART_DeInit(UART4);//�崮��4

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /*��������*/
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(UART4, &USART_InitStructure);

    /* Enable USART1 Receive  interrupts ʹ�ܽ����ж� */
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
    /* Enable the USAR1 */
    USART_Cmd(UART4, ENABLE);
#endif
    UartFifoInit();/*���ڻ����ʼ��*/
#if DEBUG_EN > 0
    printf("uart init \r\n");
#endif
}

#if UART4EN > 0
/*******************************************************************************
* Function Name  : UART4_IRQHandler.
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void UART4_IRQHandler(void)
{

    unsigned char   tempchar;
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();
    if ((UART4->SR & (USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE)) != 0)//�������
    {
        USART_ReceiveData(UART4);
    }

    if(USART_GetITStatus(UART4, USART_IT_TXE) != RESET)//�鿴���ͱ�־
    {
        if(ft_fifo_getlenth(&Uart4Fifo) > 0)//������������
        {
            ft_fifo_get(&Uart4Fifo, &tempchar, 0, 1 );
            USART_SendData(UART4, tempchar);
        }
        else
        {
            USART_ITConfig(UART4, USART_IT_TXE, DISABLE);
        }
    }

    if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)//�鿴����
    {
        tempchar = USART_ReceiveData(UART4);
        ft_fifo_put(&Uart4RxFifo,(unsigned char *) &tempchar, 1);
    }


    CPU_CRITICAL_EXIT();
    OSIntExit();            /* Tell uC/OS-III that we are leaving the ISR             */
}
#endif

#if UART3EN > 0
/*******************************************************************************
* Function Name  : USART3_IRQHandler.
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USART3_IRQHandler(void)
{
    unsigned char   tempchar;
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();
    if ((USART3->SR & (USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE)) != 0)//�������
    {
        USART_ReceiveData(USART3);
    }

    if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)//�鿴���ͱ�־
    {
        if(ft_fifo_getlenth(&Uart3Fifo) > 0)//������������
        {
            ft_fifo_get(&Uart3Fifo, &tempchar, 0, 1 );
            USART_SendData(USART3, tempchar);
        }
        else
        {
            USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
        }
    }

    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//�鿴����
    {
        tempchar = USART_ReceiveData(USART3);
        ft_fifo_put(&Uart3RxFifo,(unsigned char *) &tempchar, 1);
        printf("%c",(unsigned char)tempchar);/*���͵�uart2*/
    }

    CPU_CRITICAL_EXIT();
    OSIntExit();            /* Tell uC/OS-III that we are leaving the ISR             */


}
#endif

/*******************************************************************************
* Function Name  : USART2_IRQHandler.
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
#if UART2EN > 0
void USART2_IRQHandler(void)
{
    unsigned char   tempchar;
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();

    if ((USART2->SR & (USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE)) != 0)//�������
    {
        USART_ReceiveData(USART2);
    }


    if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)//�鿴���ͱ�־
    {
        if(ft_fifo_getlenth(&Uart2Fifo) > 0)//������������
        {
            ft_fifo_get(&Uart2Fifo, &tempchar, 0, 1 );
            USART_SendData(USART2, tempchar);
        }
        else
        {
            USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
        }
    }

//    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//�鿴����
//    {
//        tempchar = USART_ReceiveData(USART2);
//        ft_fifo_put(&Uart3Fifo,(unsigned char *) &tempchar, 1);
//    }
    CPU_CRITICAL_EXIT();
    OSIntExit();            /* Tell uC/OS-III that we are leaving the ISR             */

}
#endif


/*******************************************************************************
* Function Name  : USART1_IRQHandler.
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/

#if  UART1EN > 0
void USART1_IRQHandler(void)
{
    unsigned char   tempchar;
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();

    if ((USART1->SR & (USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE)) != 0)//�������
    {
        USART_ReceiveData(USART1);
    }

    if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)//�鿴���ͱ�־
    {
        if(ft_fifo_getlenth(&Uart1Fifo) > 0)//������������
        {
            ft_fifo_get(&Uart1Fifo, &tempchar, 0, 1 );
            USART_SendData(USART1, tempchar);
        }
        else
        {
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
        }
    }
    CPU_CRITICAL_EXIT();
    OSIntExit();            /* Tell uC/OS-III that we are leaving the ISR             */

}
#endif

/*******************************************************************************
* Function Name  : fputc.
* Description    : �ض���printf�����
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int fputc(int ch, FILE *f)
{
    char temp;
    unsigned  int  templen;

    temp = (char) ch;

    templen = ft_fifo_getlenth(&Uart2Fifo);

    if (templen + 1 <= UART2_BUF_SIZE - 1)
    {
        ft_fifo_put(&Uart2Fifo,(unsigned char*) &temp, 1);
    }

    return ch;
}

/*******************************************************************************
* Function Name  : fgetc.
* Description    : �ض���scanf����� û��;
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int fgetc(FILE *f)
{
    return  0;
}
#endif
