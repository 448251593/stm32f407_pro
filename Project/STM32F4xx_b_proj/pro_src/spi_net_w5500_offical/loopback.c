#include <stdio.h>
#include "loopback.h"
#include "socket.h"
#include "wizchip_conf.h"
#include "fifobuffer.h"
#include "spi_net_driver.h"
#if LOOPBACK_MODE == LOOPBACK_MAIN_NOBLCOK

wiz_NetInfo gWIZNETINFO=
{
   .mac={12,33,44,55,66,77},
   .ip={192,168,0,100},
   .sn={255,255,255,0},
   .gw={192,168,0,1},
   .dns={192,168,0,1},
   .dhcp=1,
};
#define   NET_SEND_BUF_SIZE      1024*80//
FT_FIFO              spi_net_send_Fifo;
unsigned char        spi_net_send_Buf[NET_SEND_BUF_SIZE];

void    W5500_net_init(void)
{
   sNET_Init();
   /* Critical section callback */
   //  reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);   

/* Chip selection call back */
    reg_wizchip_cs_cbfunc(SPI_set_slave_select, SPI_clear_slave_select);
 
    /* SPI Read & Write callback function */
   reg_wizchip_spi_cbfunc(sNET_ReadByte, sNET_SendByte); 

   ctlnetwork(CN_SET_NETINFO, &gWIZNETINFO);
   memset(&gWIZNETINFO, 0, sizeof(gWIZNETINFO));
   gWIZNETINFO.dhcp = 1;
   ctlnetwork(CN_GET_NETINFO, &gWIZNETINFO);
   printf("mac=%d:%d:%d:%d:%d:%d\n", gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],
   gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
   printf("ip=%d.%d.%d.%d\n", gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],
   gWIZNETINFO.ip[3]);
   printf("sn=%d.%d.%d.%d\n", gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],
   gWIZNETINFO.sn[3]);
   printf("gw=%d.%d.%d.%d\n", gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],
   gWIZNETINFO.gw[3]);

   printf("w5500---version=%d\n",getVERSIONR());
   ft_fifo_init(&spi_net_send_Fifo, spi_net_send_Buf, NET_SEND_BUF_SIZE);
}
uint8_t rx_buf[DATA_BUF_SIZE];
void   NetLoop(void)
{
   const uint8_t destip[4] = {192, 168, 0, 112};
   const uint16_t destport = 6800;
   loopback_tcpc(0, rx_buf, (uint8_t*)destip, destport);

}

uint16_t  w5500_send( void)
{
    unsigned int len_t;
    unsigned int ret;
   len_t = ft_fifo_getlenth(&spi_net_send_Fifo);
   if (len_t > 0)
   {
      ft_fifo_seek(&spi_net_send_Fifo, rx_buf, 0, len_t);
      ret = send(0, rx_buf , len_t);
      if (ret > 0)
      {
         ft_fifo_setoffset(&spi_net_send_Fifo, ret);
      }
   }
}

void   w5500_send_put(char *p, uint32_t len)
{

    unsigned  int  templen;
    // CPU_SR_ALLOC();
    // CPU_CRITICAL_ENTER();
    __disable_irq() ; //关闭总中断


    templen = ft_fifo_getlenth(&spi_net_send_Fifo);

    if (len + templen <= NET_SEND_BUF_SIZE - 1)
    {
        ft_fifo_put(&spi_net_send_Fifo,(unsigned char *) p, len);
    }
    // CPU_CRITICAL_EXIT();
    __enable_irq() ; //打开总中断

}

int32_t loopback_tcpc(uint8_t sn, uint8_t* buf, uint8_t* destip, uint16_t destport)
{
   int32_t ret; // return value for SOCK_ERRORs
   uint16_t size = 0, sentsize=0;

   // Destination (TCP Server) IP info (will be connected)
   // >> loopback_tcpc() function parameter
   // >> Ex)
   //	uint8_t destip[4] = 	{192, 168, 0, 214};
   //	uint16_t destport = 	5000;

   // Port number for TCP client (will be increased)
   uint16_t any_port = 	50000;

   // Socket Status Transitions
   // Check the W5500 Socket n status register (Sn_SR, The 'Sn_SR' controlled by Sn_CR command or Packet send/recv status)
   uint8_t  sts= 0;
   sts = getSn_SR(sn);
   switch(sts)
   {
      case SOCK_ESTABLISHED :
         if(getSn_IR(sn) & Sn_IR_CON)	// Socket n interrupt register mask; TCP CON interrupt = connection with peer is successful
         {
#ifdef _LOOPBACK_DEBUG_
			printf("%d:Connected to - %d.%d.%d.%d : %d\r\n",sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
			setSn_IR(sn, Sn_IR_CON);  // this interrupt should be write the bit cleared to '1'
         }

         //////////////////////////////////////////////////////////////////////////////////////////////
         // Data Transaction Parts; Handle the [data receive and send] process
         //////////////////////////////////////////////////////////////////////////////////////////////
         if ((size = getSn_RX_RSR(sn)) > 0) // Sn_RX_RSR: Socket n Received Size Register, Receiving data length
         {
            if (size > DATA_BUF_SIZE)
               size = DATA_BUF_SIZE;   // DATA_BUF_SIZE means user defined buffer size (array)
            ret = recv(sn, buf, size); // Data Receive process (H/W Rx socket buffer -> User's buffer)

            if (ret <= 0)
               return ret; // If the received data length <= 0, receive failed and process end
            sentsize = 0;

            // Data sentsize control
            while (size != sentsize)
            {
               ret = send(sn, buf + sentsize, size - sentsize); // Data send process (User's buffer -> Destination through H/W Tx socket buffer)
               if (ret < 0)                                     // Send Error occurred (sent data length < 0)
               {
                  close(sn); // socket close
                  return ret;
               }
               sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            }
               w5500_send_put("helloworld",  10);
         }
         w5500_send();
       //////////////////////////////////////////////////////////////////////////////////////////////
         break;

      case SOCK_CLOSE_WAIT :
#ifdef _LOOPBACK_DEBUG_
         //printf("%d:CloseWait\r\n",sn);
#endif
         if((ret=disconnect(sn)) != SOCK_OK) return ret;
#ifdef _LOOPBACK_DEBUG_
         printf("%d:Socket Closed\r\n", sn);
#endif
         break;

      case SOCK_INIT :
#ifdef _LOOPBACK_DEBUG_
    	//  printf("%d:Try to connect to the %d.%d.%d.%d : %d\r\n", sn, destip[0], destip[1], destip[2], destip[3], destport);
       printf("sn=%d,",sn );
    	 printf("Try to connect to the %d.%d.%d.%d : %d\r\n",  destip[0], destip[1], destip[2], destip[3], destport);
#endif
    	 if( (ret = connect(sn, destip, destport)) != SOCK_OK) return ret;	//	Try to TCP connect to the TCP server (destination)
         break;

      case SOCK_CLOSED:
    	  close(sn);
    	  if((ret=socket(sn, Sn_MR_TCP, any_port++, 0x00)) != sn) return ret; // TCP socket open with 'any_port' port number
#ifdef _LOOPBACK_DEBUG_
    	 //printf("%d:TCP client loopback start\r\n",sn);
         //printf("%d:Socket opened\r\n",sn);
#endif
         break;
      default:
         break;
   }
   return 1;
}


int32_t loopback_udps(uint8_t sn, uint8_t* buf, uint16_t port)
{
   int32_t  ret;
   uint16_t size, sentsize;
   uint8_t  destip[4];
   uint16_t destport;

   switch(getSn_SR(sn))
   {
      case SOCK_UDP :
         if((size = getSn_RX_RSR(sn)) > 0)
         {
            if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
            ret = recvfrom(sn, buf, size, destip, (uint16_t*)&destport);
            if(ret <= 0)
            {
#ifdef _LOOPBACK_DEBUG_
               printf("%d: recvfrom error. %ld\r\n",sn,ret);
#endif
               return ret;
            }
            size = (uint16_t) ret;
            sentsize = 0;
            while(sentsize != size)
            {
               ret = sendto(sn, buf+sentsize, size-sentsize, destip, destport);
               if(ret < 0)
               {
#ifdef _LOOPBACK_DEBUG_
                  printf("%d: sendto error. %ld\r\n",sn,ret);
#endif
                  return ret;
               }
               sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            }
         }
         break;
      case SOCK_CLOSED:
#ifdef _LOOPBACK_DEBUG_
         //printf("%d:UDP loopback start\r\n",sn);
#endif
         if((ret = socket(sn, Sn_MR_UDP, port, 0x00)) != sn)
            return ret;
#ifdef _LOOPBACK_DEBUG_
         printf("%d:Opened, UDP loopback, port [%d]\r\n", sn, port);
#endif
         break;
      default :
         break;
   }
   return 1;
}

#endif
