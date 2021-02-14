#include <stdio.h>
#include <string.h>
#include "loopback.h"
#include "socket.h"
#include "wizchip_conf.h"
#include "fifobuffer.h"
#include "spi_net_driver.h"
#include "spi_adc_app.h"
#include "dhcp.h"
#if LOOPBACK_MODE == LOOPBACK_MAIN_NOBLCOK

wiz_NetInfo gWIZNETINFO=
{
//    .mac={0x12,0x33,0x44,0x55,0x66,0x77},
   .mac={0xe0,0xdc,0xff,0xca,0x42,0xff},
   .ip={192,168,0,100},
   .sn={255,255,255,0},
   .gw={192,168,0,1},
   .dns={192,168,0,1},
   .dhcp=2,
};

uint32_t  state_update_count= 0 ;
uint16_t   socket_state = 0;

#define   NET_SEND_BUF_SIZE      1024*80//
FT_FIFO              spi_net_send_Fifo;
unsigned char        spi_net_send_Buf[NET_SEND_BUF_SIZE];
uint16_t w5500_send(void);
char ptmp[800] = {0};

void Reset_W5500(void)
{
   //GPIO_ResetBits(GPIOA, WIZ_RESET);
   SPI_net_rst_set(0);
   Delay(5);
   //GPIO_SetBits(GPIOA, WIZ_RESET);
   SPI_net_rst_set(1);
   Delay(1600);
}

void    W5500_net_init(void)
{
   sNET_Init();
   /* Critical section callback */
   //  reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);   
   Reset_W5500();
/* Chip selection call back */
   reg_wizchip_cs_cbfunc(SPI_set_slave_select, SPI_clear_slave_select);
 
    /* SPI Read & Write callback function */
   reg_wizchip_spi_cbfunc(sNET_ReadByte, sNET_SendByte);


   // ctlwizchip(CW_RESET_WIZCHIP, 0);
   char tx_buf_size[8] = {2,2,2,2,2,2,2,2};
   char rx_buf_size[8] = {2,2,2,2,2,2,2,2};
   // ctlwizchip(CW_INIT_WIZCHIP, tx_rx_buf_size);
   wizchip_init(tx_buf_size, rx_buf_size);
   wizchip_setnetmode(NM_FORCEARP);
#define  DHCP_ENABEL 0
#if DHCP_ENABEL
   setSHAR(gWIZNETINFO.mac);
   DHCP_init(1, ptmp);
   while (1)
   {
	   uint8_t dhcp_ret = DHCP_run();
	   if(dhcp_ret != DHCP_IP_LEASED )
	   {
		   Delay(500);
		// 	 printf("try dhcp again\n");
	   }
	   else
	   {
		   break;
	   }
   }
#endif




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

   printf("w5500--version=%d\n",getVERSIONR());

   ft_fifo_init(&spi_net_send_Fifo, spi_net_send_Buf, NET_SEND_BUF_SIZE);
}
uint8_t rx_buf[DATA_BUF_SIZE+3];
const uint8_t destip[4] = {192, 168, 0, 112};
const uint16_t destport = 6800;

uint8_t     w5500_send_flush_flag = 0;

void     w5500_send_flush(void)
{
   w5500_send_flush_flag = 1;
   return;
}

uint16_t  w5500_send( void)
{
   unsigned int len_t;
   int ret;
   uint8_t *p_tmp = 0;

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
#if   SPI_NET_DMA_ENABEL
      ret = send_for_dma(0, p_tmp, len_t+3);
#else
      ret = send(0, p_tmp + 3, len_t);
#endif
      // extern unsigned char const default_server_ip[];    // = {192, 168, 2, 112};
      // extern unsigned short const default_server_port[]; //= {6800};
      // ret = sendto_for_dma(0, p_tmp, len_t + 3,(uint8_t*) default_server_ip, (unsigned short)default_server_port);
      if (ret > 0)
      {
         ft_fifo_setoffset(&spi_net_send_Fifo, ret);
         // w5500_send_flush_flag = 0;
      }
      // else if (ret < 0)
      // {
      //    close(0);
      //    socket_state = Disconnect;
      // }

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
#if   SPI_NET_DMA_ENABEL
      ret = send_for_dma(0, p_tmp, len_t + 3);
#else
      ret = send(0, p_tmp + 3, len_t);
#endif

         // extern unsigned char const default_server_ip[];    // = {192, 168, 2, 112};
         // extern unsigned short const default_server_port[]; //= {6800};
         // ret = sendto_for_dma(0, p_tmp, len_t + 3,(uint8_t*) default_server_ip, (unsigned short)default_server_port);
         if (ret > 0)
         {
            ft_fifo_setoffset(&spi_net_send_Fifo, ret);
            // w5500_send_flush_flag = 0;
         }
         // else if (ret < 0)
         // {
         //    close(0);
         //    socket_state = Disconnect;
         // }
         printf("net send end=%d\n", get_global_tick());
      }
      
   }
	  return 0;
}
extern uint32_t sample_nums_count;
uint16_t  w5500_send_test(void)
{
    unsigned int len_t;
    unsigned int ret;
   if (run_status_g.status_s == 0)
   {
      return 0;
   }
   len_t = DATA_BUF_SIZE;
   ret = send(0, rx_buf , len_t);
   if (ret > 0)
   {
      
      sample_nums_count = sample_nums_count + ret;
   }
	 return 0;
}

void w5500_send_put(char *p, uint32_t len)
{

    unsigned  int  templen;
   // CPU_SR_ALLOC();
   // CPU_CRITICAL_ENTER();
   __disable_irq(); //关闭总中断

   //  templen = ft_fifo_getlenth(&spi_net_send_Fifo);

   //  if (len + templen <= NET_SEND_BUF_SIZE - 1)
   //  {
   templen = ft_fifo_put(&spi_net_send_Fifo, (unsigned char *)p, len);
   if (templen > 0)
   {
      sample_nums_count++;
   }
   
   //  }
   // CPU_CRITICAL_EXIT();
   __enable_irq(); //打开总中断
}
uint16_t any_port = 	50000;
#define   LOOP_SN_0   0
// int32_t loopback_tcpc(uint8_t LOOP_SN_0, uint8_t* buf, uint8_t* destip, uint16_t destport)
// int32_t loopback_tcpc(void)
int32_t NetLoop(void)
{
   int32_t ret; // return value for SOCK_ERRORs
   uint16_t size = 0;//, sentsize=0;

   // Destination (TCP Server) IP info (will be connected)
   // >> loopback_tcpc() function parameter
   // >> Ex)


   // Socket Status Transitions
   // Check the W5500 Socket n status register (Sn_SR, The 'Sn_SR' controlled by Sn_CR command or Packet send/recv status)
   uint8_t  sts= 0;
  
   sts = getSn_SR(LOOP_SN_0);
   switch(sts)
   {
      case SOCK_ESTABLISHED :
         if (state_update_count++ > 20000)
         {
            state_update_count = 0;
         }
         else
         {
            w5500_send();
            return 1;
         }
         if(getSn_IR(LOOP_SN_0) & Sn_IR_CON)	// Socket n interrupt register mask; TCP CON interrupt = connection with peer is successful
         {
#ifdef _LOOPBACK_DEBUG_
			printf("%d:Connected to - %d.%d.%d.%d : %d\r\n",LOOP_SN_0, destip[0], destip[1], destip[2], destip[3], destport);
#endif
			setSn_IR(LOOP_SN_0, Sn_IR_CON);  // this interrupt should be write the bit cleared to '1'
         }
         
         //////////////////////////////////////////////////////////////////////////////////////////////
         // Data Transaction Parts; Handle the [data receive and send] process
         //////////////////////////////////////////////////////////////////////////////////////////////
         if ((size = getSn_RX_RSR(LOOP_SN_0)) > 0) // Sn_RX_RSR: Socket n Received Size Register, Receiving data length
         {
            memset(rx_buf, 0, 100);
            if (size > DATA_BUF_SIZE)
               size = DATA_BUF_SIZE;   // DATA_BUF_SIZE means user defined buffer size (array)
            ret = recv(LOOP_SN_0, rx_buf, size); // Data Receive process (H/W Rx socket buffer -> User's buffer)

            if (ret <= 0)
               return ret; // If the received data length <= 0, receive failed and process end
             parse_data_handle(rx_buf, ret);
            // sentsize = 0;

         }
         
         // w5500_send();
       //////////////////////////////////////////////////////////////////////////////////////////////
         break;

      case SOCK_CLOSE_WAIT :
#ifdef _LOOPBACK_DEBUG_
         //printf("%d:CloseWait\r\n",LOOP_SN_0);
#endif
         if((ret=disconnect(LOOP_SN_0)) != SOCK_OK) return ret;
#ifdef _LOOPBACK_DEBUG_
         printf("%d:Socket Closed\r\n", LOOP_SN_0);
#endif
         break;

      case SOCK_INIT :
#ifdef _LOOPBACK_DEBUG_
    	//  printf("%d:Try to connect to the %d.%d.%d.%d : %d\r\n", LOOP_SN_0, destip[0], destip[1], destip[2], destip[3], destport);
      //  printf("LOOP_SN_0=%d,",LOOP_SN_0 );
    	//  printf("Try to connect to the %d.%d.%d.%d : %d\r\n",  destip[0], destip[1], destip[2], destip[3], destport);
#endif
    	 if( (ret = connect(LOOP_SN_0, (uint8_t *)destip, destport)) != SOCK_OK) return ret;	//	Try to TCP connect to the TCP server (destination)
         break;
      case SOCK_CLOSED:
    	  close(LOOP_SN_0);
    	  if((ret=socket(LOOP_SN_0, Sn_MR_TCP, any_port++, 0x00)) != LOOP_SN_0) return ret; // TCP socket open with 'any_port' port number
#ifdef _LOOPBACK_DEBUG_
    	 //printf("%d:TCP client loopback start\r\n",LOOP_SN_0);
         //printf("%d:Socket opened\r\n",LOOP_SN_0);
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