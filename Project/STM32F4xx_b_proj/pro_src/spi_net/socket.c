///*
//*
//@file    socket.c
//@brief   setting chip register for socket
#if 1
// #include "app/fpga.h"
// #include "app/def.h"
// #include "app/data.h"
// #include "app/function.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "w5500.h"

#include "socket.h"
#include "protocol.h"
#include "fifobuffer.h"




extern void Delay( uint32_t nTime);
unsigned short anyport=9013;

#define ETH_RX_BUF_SIZE 2048 //以太网帧最大接收长度
#define ETH_TX_BUF_SIZE 2048 //以太网帧最大发送长度

#define HEART_BEAT_CNT_MAX (1*60*1000ul)  //
#define MAKE_WORD(h,l) (uint16_t)((h<<8)+l)

static unsigned short local_port;


uint16_t   socket_state;
uint16_t   heart_beat_time;
uint16_t   protocal_overtime;
uint16_t   rx_tail;
uint8_t    recv_buf[128];
///**
//@brief   This Socket function initialize the channel in perticular mode, and set the port and wait for W5200 done it.
//@return  1 for sucess else 0.
//*/
uint8_t socket(SOCKET s, uint8_t protocol, unsigned short port, uint8_t flag)
{
   uint8_t ret;
   if (
        ((protocol&0x0F) == Sn_MR_TCP)    ||
        ((protocol&0x0F) == Sn_MR_UDP)    ||
        ((protocol&0x0F) == Sn_MR_IPRAW)  ||
        ((protocol&0x0F) == Sn_MR_MACRAW) ||
        ((protocol&0x0F) == Sn_MR_PPPOE)
      )
   {
      close(s);
      IINCHIP_WRITE(Sn_MR(s) ,protocol | flag);
      if (port != 0) {
         IINCHIP_WRITE( Sn_PORT0(s) ,(uint8_t)((port & 0xff00) >> 8));
         IINCHIP_WRITE( Sn_PORT1(s) ,(uint8_t)(port & 0x00ff));
      } else {
         local_port++; // if don't set the source port, set local_port number.
         IINCHIP_WRITE(Sn_PORT0(s) ,(uint8_t)((local_port & 0xff00) >> 8));
         IINCHIP_WRITE(Sn_PORT1(s) ,(uint8_t)(local_port & 0x00ff));
      }
      IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_OPEN); // run sockinit Sn_CR

      /* wait to process the command... */
      while( IINCHIP_READ(Sn_CR(s)) )
         ;
      /* ------- */
      ret = 1;
   }
   else
   {
      ret = 0;
   }
   return ret;
}


///**
//@brief   This function close the socket and parameter is "s" which represent the socket number
//*/
void close(SOCKET s)
{

   IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_CLOSE);

   /* wait to process the command... */
   while( IINCHIP_READ(Sn_CR(s) ) )
      ;
   /* ------- */
        /* all clear */
   IINCHIP_WRITE( Sn_IR(s) , 0xFF);
   socket_state=Disconnect; //add wzj
   RJ45_config.enable=0;
}


///**
//@brief   This function established  the connection for the channel in passive (server) mode. This function waits for the request from the peer.
//@return  1 for success else 0.
//*/
uint8_t listen(SOCKET s)
{
   uint8_t ret;
   if (IINCHIP_READ( Sn_SR(s) ) == SOCK_INIT)
   {
      IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_LISTEN);
      /* wait to process the command... */
      while( IINCHIP_READ(Sn_CR(s) ) )
         ;
      /* ------- */
      ret = 1;
   }
   else
   {
      ret = 0;
   }
   return ret;
}


///**
//@brief   This function established  the connection for the channel in Active (client) mode.
//      This function waits for the untill the connection is established.

//@return  1 for success else 0.
//*/
uint8_t connect(SOCKET s, uint8_t * addr, unsigned short port)
{
    uint8_t ret;
    if
        (
            ((addr[0] == 0xFF) && (addr[1] == 0xFF) && (addr[2] == 0xFF) && (addr[3] == 0xFF)) ||
            ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
            (port == 0x00)
        )
    {
      ret = 0;
    }
    else
    {
        ret = 1;
        // set destination IP
//		IINCHIP_WRITE(0x000100,0x22);
//		ret=IINCHIP_READ(0x000100);

        IINCHIP_WRITE( Sn_DIPR0(s), addr[0]);
	    ret=IINCHIP_READ(Sn_DIPR0(s));
        IINCHIP_WRITE( Sn_DIPR1(s), addr[1]);
		ret=IINCHIP_READ(Sn_DIPR1(s));
        IINCHIP_WRITE( Sn_DIPR2(s), addr[2]);
		ret=IINCHIP_READ(Sn_DIPR2(s));
        IINCHIP_WRITE( Sn_DIPR3(s), addr[3]);
        ret=IINCHIP_READ(Sn_DIPR3(s));
        IINCHIP_WRITE( Sn_DPORT0(s), (uint8_t)((port & 0xff00) >> 8));
		ret=IINCHIP_READ(Sn_DPORT0(s));
        IINCHIP_WRITE( Sn_DPORT1(s), (uint8_t)(port & 0x00ff));
		ret=IINCHIP_READ(Sn_DPORT1(s));
        IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_CONNECT);
		ret=IINCHIP_READ(Sn_CR(s));
       Delay(100);
		/* wait for completion */
	   while ( IINCHIP_READ(Sn_CR(s) ) ){ };

//         time_1ms=0;
        while ( IINCHIP_READ(Sn_SR(s)) != SOCK_SYNSENT )
        {
            if(IINCHIP_READ(Sn_SR(s)) == SOCK_ESTABLISHED)
            {
                break;
            }
            if (getSn_IR(s) & Sn_IR_TIMEOUT)
            {
                IINCHIP_WRITE(Sn_IR(s), (Sn_IR_TIMEOUT));  // clear TIMEOUT Interrupt
                ret = 0;
                break;
            }
          //  if(time_1ms>1000)break;
        }
    }

   return ret;
}



///**
//@brief   This function used for disconnect the socket and parameter is "s" which represent the socket number
//@return  1 for success else 0.
//*/
void disconnect(SOCKET s)
{
   IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_DISCON);

   /* wait to process the command... */
   while( IINCHIP_READ(Sn_CR(s) ) )
      ;
   socket_state=Disconnect; //add wzj
   RJ45_config.enable=0;
   /* ------- */
}

unsigned short send_for_dma(SOCKET s, const uint8_t * buf, unsigned short len)
{
  uint8_t status=0;
  unsigned short ret=0;
  unsigned short freesize=0;
   //add by bcg,2021-01-10 16:23:13 先去除cmd 长度
  len = len - 3;

  if (len > getIINCHIP_TxMAX(s)) ret = getIINCHIP_TxMAX(s); // check size not to exceed MAX size.
  else ret = len;

  // if freebuf is available, start.
  do
  {
    freesize = getSn_TX_FSR(s);
    status = IINCHIP_READ(Sn_SR(s));
    if ((status != SOCK_ESTABLISHED) && (status != SOCK_CLOSE_WAIT))
    {
      ret = 0;
      break;
    }
  } while (freesize < ret);


  // copy data
  send_data_processing_for_dma(s, (uint8_t *)buf, ret);
  IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_SEND);

  /* wait to process the command... */
  while( IINCHIP_READ(Sn_CR(s) ) );

  while ( (IINCHIP_READ(Sn_IR(s) ) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK )
  {
    status = IINCHIP_READ(Sn_SR(s));
    if ((status != SOCK_ESTABLISHED) && (status != SOCK_CLOSE_WAIT) )
    {
   //   LOG_INFO("SEND_OK Problem!!\r\n");
      close(s);
      return 0;
    }
  }
  IINCHIP_WRITE( Sn_IR(s) , Sn_IR_SEND_OK);

#ifdef __DEF_IINCHIP_INT__
  // putISR(s, getISR(s) & (~Sn_IR_SEND_OK));
#else
   IINCHIP_WRITE( Sn_IR(s) , Sn_IR_SEND_OK);
#endif

   return ret;
}

///**
//@brief   This function used to send the data in TCP mode
//@return  1 for success else 0.
//*/
unsigned short send(SOCKET s, const uint8_t * buf, unsigned short len)
{
  uint8_t status=0;
  unsigned short ret=0;
  unsigned short freesize=0;

  if (len > getIINCHIP_TxMAX(s)) ret = getIINCHIP_TxMAX(s); // check size not to exceed MAX size.
  else ret = len;

  // if freebuf is available, start.
  do
  {
    freesize = getSn_TX_FSR(s);
    status = IINCHIP_READ(Sn_SR(s));
    if ((status != SOCK_ESTABLISHED) && (status != SOCK_CLOSE_WAIT))
    {
      ret = 0;
      break;
    }
  } while (freesize < ret);


  // copy data
  send_data_processing(s, (uint8_t *)buf, ret);
  IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_SEND);

  /* wait to process the command... */
  while( IINCHIP_READ(Sn_CR(s) ) );

  while ( (IINCHIP_READ(Sn_IR(s) ) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK )
  {
    status = IINCHIP_READ(Sn_SR(s));
    if ((status != SOCK_ESTABLISHED) && (status != SOCK_CLOSE_WAIT) )
    {
   //   LOG_INFO("SEND_OK Problem!!\r\n");
      close(s);
      return 0;
    }
  }
  IINCHIP_WRITE( Sn_IR(s) , Sn_IR_SEND_OK);

#ifdef __DEF_IINCHIP_INT__
  // putISR(s, getISR(s) & (~Sn_IR_SEND_OK));
#else
   IINCHIP_WRITE( Sn_IR(s) , Sn_IR_SEND_OK);
#endif

   return ret;
}



///**
//@brief   This function is an application I/F function which is used to receive the data in TCP mode.
//      It continues to wait for data as much as the application wants to receive.

//@return  received data size for success else -1.
//*/
unsigned short recv(SOCKET s, uint8_t * buf, unsigned short len)
{
   unsigned short ret=0;
   if ( len > 0 )
   {
      recv_data_processing(s, buf, len);
      IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_RECV);
      /* wait to process the command... */
      while( IINCHIP_READ(Sn_CR(s) ));
      /* ------- */
      ret = len;
   }
   return ret;
}


///**
//@brief   This function is an application I/F function which is used to send the data for other then TCP mode.
//      Unlike TCP transmission, The peer's destination address and the port is needed.

//@return  This function return send data size for success else -1.
//*/
unsigned short sendto(SOCKET s, const uint8_t * buf, unsigned short len, uint8_t * addr, unsigned short port)
{
   unsigned short ret=0;

   if (len > getIINCHIP_TxMAX(s)) ret = getIINCHIP_TxMAX(s); // check size not to exceed MAX size.
   else ret = len;

   if( ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) || ((port == 0x00)) )//||(ret == 0) )
   {
      /* added return value */
      ret = 0;
   }
   else
   {
      IINCHIP_WRITE( Sn_DIPR0(s), addr[0]);
      IINCHIP_WRITE( Sn_DIPR1(s), addr[1]);
      IINCHIP_WRITE( Sn_DIPR2(s), addr[2]);
      IINCHIP_WRITE( Sn_DIPR3(s), addr[3]);
      IINCHIP_WRITE( Sn_DPORT0(s),(uint8_t)((port & 0xff00) >> 8));
      IINCHIP_WRITE( Sn_DPORT1(s),(uint8_t)(port & 0x00ff));
      // copy data
      send_data_processing(s, (uint8_t *)buf, ret);
      IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_SEND);
      /* wait to process the command... */
      while( IINCHIP_READ( Sn_CR(s) ) )
         ;
      /* ------- */

      while( (IINCHIP_READ( Sn_IR(s) ) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK )
      {
         if (IINCHIP_READ( Sn_IR(s) ) & Sn_IR_TIMEOUT)
         {
            /* clear interrupt */
            IINCHIP_WRITE( Sn_IR(s) , (Sn_IR_SEND_OK | Sn_IR_TIMEOUT)); /* clear SEND_OK & TIMEOUT */
            return 0;
         }
      }
      IINCHIP_WRITE( Sn_IR(s) , Sn_IR_SEND_OK);
   }
   return ret;
}


///**
//@brief   This function is an application I/F function which is used to receive the data in other then
//   TCP mode. This function is used to receive UDP, IP_RAW and MAC_RAW mode, and handle the header as well.

//@return  This function return received data size for success else -1.
//*/
unsigned short recvfrom(SOCKET s, uint8_t * buf, unsigned short len, uint8_t * addr, unsigned short *port)
{
   uint8_t head[8];
   unsigned short data_len=0;
   unsigned short ptr=0;
   uint32_t addrbsb =0;
   if ( len > 0 )
   {
      ptr     = IINCHIP_READ(Sn_RX_RD0(s) );
      ptr     = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_RX_RD1(s));
      addrbsb = (uint32_t)(ptr<<8) +  (s<<5) + 0x18;

      switch (IINCHIP_READ(Sn_MR(s) ) & 0x07)
      {
      case Sn_MR_UDP :
        wiz_read_buf(addrbsb, head, 0x08);
        ptr += 8;
        // read peer's IP address, port number.
        addr[0]  = head[0];
        addr[1]  = head[1];
        addr[2]  = head[2];
        addr[3]  = head[3];
        *port    = head[4];
        *port    = (*port << 8) + head[5];
        data_len = head[6];
        data_len = (data_len << 8) + head[7];

        addrbsb = (uint32_t)(ptr<<8) +  (s<<5) + 0x18;
        wiz_read_buf(addrbsb, buf, data_len);
        ptr += data_len;

        IINCHIP_WRITE( Sn_RX_RD0(s), (uint8_t)((ptr & 0xff00) >> 8));
        IINCHIP_WRITE( Sn_RX_RD1(s), (uint8_t)(ptr & 0x00ff));
        break;

      case Sn_MR_IPRAW :
        wiz_read_buf(addrbsb, head, 0x06);
        ptr += 6;
        addr[0]  = head[0];
        addr[1]  = head[1];
        addr[2]  = head[2];
        addr[3]  = head[3];
        data_len = head[4];
        data_len = (data_len << 8) + head[5];

        addrbsb  = (uint32_t)(ptr<<8) +  (s<<5) + 0x18;
        wiz_read_buf(addrbsb, buf, data_len);
        ptr += data_len;

        IINCHIP_WRITE( Sn_RX_RD0(s), (uint8_t)((ptr & 0xff00) >> 8));
        IINCHIP_WRITE( Sn_RX_RD1(s), (uint8_t)(ptr & 0x00ff));
        break;

      case Sn_MR_MACRAW :
        wiz_read_buf(addrbsb, head, 0x02);
        ptr+=2;
        data_len = head[0];
        data_len = (data_len<<8) + head[1] - 2;
        if(data_len > 1514)
        {
//           LOG_INFO("data_len over 1514\r\n");
           while(1);
        }

        addrbsb  = (uint32_t)(ptr<<8) +  (s<<5) + 0x18;
        wiz_read_buf(addrbsb, buf, data_len);
        ptr += data_len;

        IINCHIP_WRITE( Sn_RX_RD0(s), (uint8_t)((ptr & 0xff00) >> 8));
        IINCHIP_WRITE( Sn_RX_RD1(s), (uint8_t)(ptr & 0x00ff));
        break;

      default :
            break;
      }
      IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_RECV);

      /* wait to process the command... */
      while( IINCHIP_READ( Sn_CR(s)) ) ;
      /* ------- */
   }
   return data_len;
}

///*
//void macraw_open(void)
//{
//  uint8_t sock_num;
//  unsigned short dummyPort = 0;
//  uint8_t mFlag = 0;
//  sock_num = 0;


//  close(sock_num); // Close the 0-th socket
//  socket(sock_num, Sn_MR_MACRAW, dummyPort,mFlag);  // OPen the 0-th socket with MACRAW mode
//}


//unsigned short macraw_send( const uint8_t * buf, unsigned short len )
//{
//   unsigned short ret=0;
//   uint8_t sock_num;
//   sock_num =0;


//   if (len > getIINCHIP_TxMAX(sock_num)) ret = getIINCHIP_TxMAX(sock_num); // check size not to exceed MAX size.
//   else ret = len;

//   send_data_processing(sock_num, (uint8_t *)buf, len);

//   //W5500 SEND COMMAND
//   IINCHIP_WRITE(Sn_CR(sock_num),Sn_CR_SEND);
//   while( IINCHIP_READ(Sn_CR(sock_num)) );
//   while ( (IINCHIP_READ(Sn_IR(sock_num)) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK );
//   IINCHIP_WRITE(Sn_IR(sock_num), Sn_IR_SEND_OK);

//   return ret;
//}

//unsigned short macraw_recv( uint8_t * buf, unsigned short len )
//{
//   uint8 sock_num;
//   unsigned short data_len=0;
//   unsigned short dummyPort = 0;
//   unsigned short ptr = 0;
//   uint8 mFlag = 0;
//   sock_num = 0;

//   if ( len > 0 )
//   {

//      data_len = 0;

//      ptr = IINCHIP_READ(Sn_RX_RD0(sock_num));
//      ptr = (unsigned short)((ptr & 0x00ff) << 8) + IINCHIP_READ( Sn_RX_RD1(sock_num) );
//      //-- read_data(s, (uint8 *)ptr, data, len); // read data
//      data_len = IINCHIP_READ_RXBUF(0, ptr);
//      ptr++;
//      data_len = ((data_len<<8) + IINCHIP_READ_RXBUF(0, ptr)) - 2;
//      ptr++;

//      if(data_len > 1514)
//      {
////         LOG_INFO("data_len over 1514\r\n");
////         LOG_INFO("\r\nptr: %X, data_len: %X", ptr, data_len);
//         //while(1);
//          recommand : close and open **/
// /*        close(sock_num); // Close the 0-th socket
//         socket(sock_num, Sn_MR_MACRAW, dummyPort,mFlag);  // OPen the 0-th socket with MACRAW mode
//         return 0;
//      }

//      IINCHIP_READ_RXBUF_BURST(sock_num, ptr, data_len, (uint8*)(buf));
//      ptr += data_len;

//      IINCHIP_WRITE(Sn_RX_RD0(sock_num),(uint8)((ptr & 0xff00) >> 8));
//      IINCHIP_WRITE(Sn_RX_RD1(sock_num),(uint8)(ptr & 0x00ff));
//      IINCHIP_WRITE(Sn_CR(sock_num), Sn_CR_RECV);
//      while( IINCHIP_READ(Sn_CR(sock_num)) ) ;
//   }

//   return data_len;
//}
//   */




uint8_t SendSocketData(SOCKET s, uint8_t * buf, uint16_t len)
{
	uint16_t tx_len;
	uint16_t tx_cnt=0;
	if(	socket_state==Disconnect)
	{
		return 1;
	}

	while(1)
	{
		tx_len=send(s,buf,len);
		if(tx_len==len)break;
		tx_cnt++;
        if(tx_cnt>=5)
		         {
			         close(s);
//				//	 NetInit();
		             return 1;
				 }

	}
	heart_beat_time=0;
	return 0;
}

FT_FIFO              spi_net_send_Fifo;
unsigned char        spi_net_send_Buf[NET_SEND_BUF_SIZE];
uint8_t     w5500_send_flush_flag = 0;

uint8_t rx_buf[DATA_BUF_SIZE+3];
void      w5500_fifo_init(void)
{
      ft_fifo_init(&spi_net_send_Fifo, spi_net_send_Buf, NET_SEND_BUF_SIZE);
}
void     w5500_send_flush(void)
{
   w5500_send_flush_flag = 1;
   // printf("start net_send=%d\n", get_global_tick());
   return;
}
uint16_t  w5500_send( void)
{
   unsigned int len_t;
   int ret;
   uint8_t *p_tmp = 0;

  if(w5500_send_flush_flag == 0)
  {
     return 0;
  }
   // len_t = ft_fifo_getlenth(&spi_net_send_Fifo);
   // len_t = spi_net_send_Fifo.cnt;
   if (spi_net_send_Fifo.cnt >= DATA_BUF_SIZE)
   {
      // if (spi_net_send_Fifo.cnt >= DATA_BUF_SIZE)
         len_t = DATA_BUF_SIZE;
      

      __disable_irq(); //关闭总中断
      p_tmp = rx_buf;
      ft_fifo_seek(&spi_net_send_Fifo, p_tmp + 3, 0, len_t);
      __enable_irq(); //打开总中断

      ret = send_for_dma(0, p_tmp, len_t+3);

      // extern unsigned char const default_server_ip[];    // = {192, 168, 2, 112};
      // extern unsigned short const default_server_port[]; //= {6800};
      // ret = sendto_for_dma(0, p_tmp, len_t + 3,(uint8_t*) default_server_ip, (unsigned short)default_server_port);
      if (ret > 0)
      {
         ft_fifo_setoffset(&spi_net_send_Fifo, ret);
         // w5500_send_flush_flag = 0;
      }
      else if (ret < 0)
      {
         close(0);
         socket_state = Disconnect;
      }

   }
   else
   {
      if (w5500_send_flush_flag == 1)
      {
         w5500_send_flush_flag = 0;
         len_t = spi_net_send_Fifo.cnt;

         __disable_irq(); //关闭总中断
         p_tmp = rx_buf;
         ft_fifo_seek(&spi_net_send_Fifo, p_tmp + 3, 0, len_t);
         __enable_irq(); //打开总中断

         ret = send_for_dma(0, p_tmp, len_t + 3);

         // extern unsigned char const default_server_ip[];    // = {192, 168, 2, 112};
         // extern unsigned short const default_server_port[]; //= {6800};
         // ret = sendto_for_dma(0, p_tmp, len_t + 3,(uint8_t*) default_server_ip, (unsigned short)default_server_port);
         if (ret > 0)
         {
            ft_fifo_setoffset(&spi_net_send_Fifo, ret);
            // w5500_send_flush_flag = 0;
         }
         else if (ret < 0)
         {
            close(0);
            socket_state = Disconnect;
         }
         printf("net send end=%d\n", get_global_tick());
      }
      
   }
	  return 0;
}

void   w5500_send_put(char *p, uint32_t len)
{

   //  unsigned  int  templen;
    // CPU_SR_ALLOC();
    // CPU_CRITICAL_ENTER();
    __disable_irq() ; //关闭总中断


   //  templen = ft_fifo_getlenth(&spi_net_send_Fifo);

   //  if (len + templen <= NET_SEND_BUF_SIZE - 1)
   //  {
        ft_fifo_put(&spi_net_send_Fifo,(unsigned char *) p, len);
   //  }
    // CPU_CRITICAL_EXIT();
    __enable_irq() ; //打开总中断

}
uint32_t  state_update_count= 0 ;
const uint8_t message[] = "Hello World";


int32_t loopback_udps(uint8_t sn, uint8_t* buf, uint16_t port)
{
   int32_t  ret;
   uint16_t size, sentsize;
   uint8_t  destip[4]={192, 168, 0, 112};
   uint16_t destport = 6800;

   switch(getSn_SR(sn))
   {
      case SOCK_UDP :
         sendto(sn, buf, 5, destip, destport);
//          if((size = getSn_RX_RSR(sn)) > 0)
//          {
//             if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
//             ret = recvfrom(sn, buf, size, destip, (uint16_t*)&destport);
//             if(ret <= 0)
//             {
// #ifdef 1
//                printf("%d: recvfrom error. %ld\r\n",sn,ret);
// #endif
//                return ret;
//             }
//             size = (uint16_t) ret;
//             sentsize = 0;
//             while(sentsize != size)
//             {
//                ret = sendto(sn, buf+sentsize, size-sentsize, destip, destport);
//                if(ret < 0)
//                {
// #ifdef 1
//                   printf("%d: sendto error. %ld\r\n",sn,ret);
// #endif
//                   return ret;
//                }
//                sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
//             }
//          }
         break;
      case SOCK_CLOSED:
#if 1
         printf("%d:UDP loopback start\r\n",sn);
#endif
         if((ret = socket(sn, Sn_MR_UDP, port, 0x00)) != sn)
            return ret;
#if 1
         printf("%d:Opened, UDP loopback, port [%d]\r\n", sn, port);
#endif
         break;
      default :
         break;
   }
   return 1;
}
void start_send_udp_msg(void)
{
   loopback_udps(1, "hello", 5);
}
void NetLoop(void)
{
	unsigned char state,phy_cfgr;
	unsigned short erx_len; //,etx_len;
	//unsigned char i,j=0,num=0;
	     

   if(state_update_count++ > 20000)
   {
      phy_cfgr = getPHYCFGR();
      if (!(phy_cfgr & 0x1))
      {
         disconnect(0);
         socket_state = Connect_none;
         return;
      }
      state = getSn_SR(0);
      state_update_count = 0;
   }
   else
   {
      if(socket_state == Connect)
      {
         w5500_send();
      }
   }
	//	debug("net",state);
	switch (state) /*获取socket0的状态*/
	{
   case SOCK_INIT: /*socket初始化完成*/
      Delay(50);//Delay(50);
      connect(0, (uint8_t *)RJ45_config.server_ip, RJ45_config.server_port[0]); /*在TCP模式下向服务器发送连接请求*/
      printf("SI\n");
      break;

   case SOCK_ESTABLISHED: /*socket连接建立*/
		if (getSn_IR(0) & Sn_IR_CON)
		{
			setSn_IR(0, Sn_IR_CON); /*Sn_IR的第0位置1而清0*/
		}
		switch (socket_state)
		{
		case Disconnect:
			socket_state = Connect;
			SendSocketData(0, (uint8_t *)message, sizeof(message)); //向Server发送数据
			break;

		case Connect:
			erx_len = getSn_RX_RSR(0); //len为已接收数据的大小
			if (erx_len > 0)
			{
            memset(recv_buf, 0, sizeof(recv_buf));
				recv(0, recv_buf, erx_len); //W5500接收来自Sever的数据
            parse_data_handle(recv_buf, erx_len);
				// rx_tail = rx_tail + erx_len;
				// protocal_overtime = 0;
				// recv_buf[rx_tail++] = 0x00;
				// RJ45_config.enable = 1;
			}
			// if (heart_beat_time >= HEART_BEAT_CNT_MAX)
			// {
			// 	SendSocketData(0, (uint8_t *)message, sizeof(message)); //向Server发送数据
			// }
         // w5500_send();
			break;
		default:
			break;
		}
		break;

	case SOCK_CLOSE_WAIT: /*socket等待关闭状态*/
		disconnect(0);
		break;

	case SOCK_CLOSED: /*socket关闭*/
		socket_state = Disconnect;
		socket(0, Sn_MR_TCP, anyport++, Sn_MR_ND); /*打开socket0的一个端口*/
		break;
	default:
		//printf("SOCK_DEFAULT %d\n", getSn_SR(0));
		break;
	}
}

//unsigned short process_erx_buf(unsigned short len)
//{
//static unsigned char erx_len;
//unsigned char i;
//unsigned short cmd,stmp;
//static unsigned char *  addr;
//if(erx_buf[0]!=0xfb || erx_buf[1]!=0xfb)return 0; //帧头检测
//stmp=PC_NID;
//if(memcmp(erx_buf+2,&stmp,2)!=0 ) return 0;
//if(memcmp(erx_buf+4,&dev_comm_addr,2)!=0 ) return 0;
//erx_len=MAKE_WORD(erx_buf[10],erx_buf[11]);

//if(CheckCRC(erx_buf,erx_len)==1)
//    {
//      //printf("CRC error!\n");
//  //    return;
//    }
//    //printf("CRC ok!\n");
//cmd=MAKE_WORD(erx_buf[8],erx_buf[9]);
//if(cmd&UP_CMD)
//    {
//      //printf("error direction\n");
//      return 0;
//    }
//if(cmd&ERR_CMD)return 0;
//if(cmd&ANS_CMD){ packet_respond=1;return 0;}
//
//cmd=cmd&0xff;
//    switch(cmd)
//    {
//    case 0x2d:
//      //rtn=CmdAddSensor(rx_buf+12+route_len,rx_data_len);
//      break;
//    case 0x2e:
//      //rtn=CmdDelSensor(rx_buf+12+route_len,rx_data_len);
//      break;
//    case 0x2F:
//      //rtn=CmdModifySensor(rx_buf+12+route_len,rx_data_len);
//      break;
//    case 0x6B:
//      //rtn=CmdQuerySensor(rx_buf+12+route_len,rx_data_len,tx_buf+12+route_len,&tx_data_len);
//      break;
//    case 0xA1:
//    //  rtn=CmdStartSample(rx_buf+12+route_len,rx_data_len);
//      break;
//    case 0xA2:
//    //  rtn=CmdStopSample(rx_buf+12+route_len,rx_data_len);
//      break;
//    case 0x30:
//	    addr=(unsigned char *)&RJ45_config;
//	    for(i=0;i<96;i++)
//	         {
//			 *addr++=erx_buf[10+i];
//			 }
//         i=0xAA;
//    	 spi_flash_write(dev_config_addr,&i,1);
//       break;
//    default:
//       break;
//    }
//
//return 0;
//}


#endif
