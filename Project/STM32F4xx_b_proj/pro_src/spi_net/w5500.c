///*
// * (c)COPYRIGHT
// * ALL RIGHT RESERVED
// *
// * FileName : w5500.c
//  * -----------------------------------------------------------------
// */
#if 1

#define SPI_NET_OLD_CODE_ENALBE 0
#include "spi_net_driver.h"
// #include "app/fpga.h"
// #include "app/def.h"
// #include "app/data.h"
// #include "app/function.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
// #include "app/net/socket.h"
#include "w5500.h"
#include "protocol.h"
#include "main.h"
typedef struct 
{
	char local_ip[16];

	char mac[6];
	short server_port[1];

	char gateway[16];
	char mask[16];
	char server_ip[16];
}RJ45_config_struct;
RJ45_config_struct  RJ45_config;
// #include "app/uart/debug_def.h"

//spi_instance_t _net_core_spi;

unsigned char const default_server_ip[] = {192, 168, 2, 112};
unsigned char const default_client_ip[] = {192, 168, 2, 100};
unsigned short const default_server_port[] = {6800};
unsigned char const default_mask[] = {255, 255, 255, 0};
unsigned char const default_gateway[4] = {192, 168, 2, 254};
unsigned char const default_mac[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};

static uint8_t I_STATUS[8];
static uint16_t SSIZE[8]; /**< Max Tx buffer size by each channel */
static uint16_t RSIZE[8]; /**< Max Rx buffer size by each channel */

uint8_t txsize[8] = {4, 2, 2, 2, 2, 1, 1, 2};
uint8_t rxsize[8] = {4, 2, 2, 2, 2, 1, 1, 2};

void W5500_net_init(void)
{

	//   MSS_GPIO_set_output(rst_W5500,0);
	//   delay_ms(100);
	//   MSS_GPIO_set_output(rst_W5500,1);
	//   delay_ms(20);

#if SPI_NET_OLD_CODE_ENALBE
	SPI_init(net_core_spi, CORESPI_net_ADDR, 32);
	SPI_configure_master_mode(net_core_spi);
#else
	sNET_Init();
#endif

	// socket_state = Disconnect;
	// RJ45_config.enable = 0;
	// heart_beat_time = 0x00;
	NetInit();
}

void set_netpara_default()
{
	uint8_t i;
#if  1
	RJ45_config.server_port[0] = default_server_port[0];

	for (i = 0; i < 6; i++)
		RJ45_config.mac[i] = default_mac[i];

	for (i = 0; i < 4; i++)
	{
		RJ45_config.local_ip[i] = default_client_ip[i];
		RJ45_config.gateway[i] = default_gateway[i];
		RJ45_config.mask[i] = default_mask[i];
		RJ45_config.server_ip[i] = default_server_ip[i];
	}
	#endif
}
//add by bcg,2020-09-02 14:38:54--
uint8_t  ascii_2_uint(uint8_t *pd,uint8_t n)
{

	return *pd - '0';
}
void get_int_from_string(uint8_t *uint, uint8_t *config, uint8_t num)
{
	uint8_t i, j, cdata[5];
	uint32_t data = 0;
	for (i = 0; i < num; i++)
	{
		for (j = 0; j < 5; j++)
		{
			cdata[j] = *config++;
			if (cdata[j] == '.' || cdata[j] == '}' || cdata[j] == ')')
				break;
			if (cdata[j] > '9' || cdata[j] < '0')
				return;
			//  config++;
		}
		*uint++ = ascii_2_uint(cdata, j);
	}
}

void get_int_from_string2(uint16_t *uint, uint8_t *config, uint8_t num)
{
	uint8_t i, j, cdata[5];
	uint32_t data = 0;
	for (i = 0; i < num; i++)
	{
		for (j = 0; j < 5; j++)
		{
			cdata[j] = *config++;
			if (cdata[j] == '.' || cdata[j] == '}' || cdata[j] == ')')
				break;
			if (cdata[j] > '9' || cdata[j] < '0')
				return;
			//  config++;
		}
		*uint++ = ascii_2_uint(cdata, j);
	}
}

uint8_t ascii_2_hex(uint8_t *cdata, uint8_t num) //֧��2�ֽ�ASCII��
{
	uint8_t A, B, a, b;
	A = cdata[0];
	B = cdata[1];
	if (A >= '0' && A <= '9')
		a = A - '0';
	else if ((A >= 'A' && A <= 'F'))
		a = A - 'A' + 10;
	else if ((A >= 'a' && A <= 'f'))
		a = A - 'a' + 10;

	if (B >= '0' && B <= '9')
		b = B - '0';
	else if ((B >= 'A' && B <= 'F'))
		b = B - 'A' + 10;
	else if ((B >= 'a' && B <= 'f'))
		b = B - 'a' + 10;

	return (a << 4) + b;
}

void get_hex_from_string(uint8_t *uint, uint8_t *config, uint8_t num)
{
	uint8_t i, j, cdata[5];
	//uint32_t data = 0;
	for (i = 0; i < num; i++)
	{
		for (j = 0; j < 5; j++)
		{
			cdata[j] = *config++;
			if (cdata[j] == '.' || cdata[j] == '}' || cdata[j] == ')' || cdata[j] == ',')
				break;
			//  config++;
		}
		*uint++ = ascii_2_hex(cdata, j);
	}
}

void set_rj45_config(uint8_t *config)
{
	#if 0
	uint32_t i = 0, cmd_eth_type;
	uint8_t *bak;
	if (!strncmp("dev_para", config, strlen("dev_para")))
	{
		config = config + strlen("dev_para");
		bak = config;
		for (cmd_eth_type = 0; cmd_eth_type < sub_ETH_end; cmd_eth_type++)
		{
			while (strncmp(sub_ETH[cmd_eth_type], (char *)config, strlen(sub_ETH[cmd_eth_type])))
			{
				if (*config++ == '}')
					return;
			}
			//��⵽һ���ؼ���
			config = config + strlen(sub_ETH[cmd_eth_type]);
			config++;
			switch (cmd_eth_type)
			{
			case server_ip:
				get_int_from_string(RJ45_config.server_ip, config, 4);
				break;

			case local_ip:
				get_int_from_string(RJ45_config.local_ip, config, 4);
				break;

			case server_port:
				get_int_from_string2(RJ45_config.server_port, config, 1);
				break;

			case mac_addr:
				get_hex_from_string(RJ45_config.mac, config, 6);
				break;

			case gate_way:
				get_int_from_string(RJ45_config.gateway, config, 4);
				break;
			}
			config = bak;
		}
	}
	else
	#endif
		set_netpara_default();
}

void NetInit(void)
{
	uint8_t tmp_data[6], i;
	//	set_netpara_default();
	i = getVersion();
	//	debug("netVer",i);
	#if 1
	for (i = 0; i < 6; i++)
		tmp_data[i] = RJ45_config.mac[i];
	setSHAR(tmp_data); /*����Mac��ַ*/

	for (i = 0; i < 4; i++)
		tmp_data[i] = RJ45_config.local_ip[i];
	setSIPR(tmp_data); /*����Ip��ַ*/

	for (i = 0; i < 4; i++)
		tmp_data[i] = RJ45_config.mask[i];
	setSUBR(tmp_data); /*������������*/

	for (i = 0; i < 4; i++)
		tmp_data[i] = RJ45_config.gateway[i];
#endif
	setGAR(tmp_data); /*����Ĭ������*/

	getSHAR(tmp_data);
	getSIPR(tmp_data);
	getSUBR(tmp_data);
	getGAR(tmp_data);

	sysinit(txsize, rxsize); /*��ʼ��8��socket*/

	setRTR(2000); /*�������ʱ��ֵ*/
	setRCR(3);	  /*����������·��ʹ���*/
}

uint8_t getISR(uint8_t s)
{
	return I_STATUS[s];
}
void putISR(uint8_t s, uint8_t val)
{
	I_STATUS[s] = val;
}

uint16_t getIINCHIP_RxMAX(uint8_t s)
{
	return RSIZE[s];
}
uint16_t getIINCHIP_TxMAX(uint8_t s)
{
	return SSIZE[s];
}

//add by bcg,2020-09-02 14:13:54---
void SPI_set_slave_select(void)
{
	sNET_CS_LOW();
}
void SPI_clear_slave_select(void)
{
	sNET_CS_HIGH();
}

void IINCHIP_WRITE(uint32_t addrbsb, uint8_t data)
{
	unsigned char cmd_buffer[4];
	//IINCHIP_ISR_DISABLE();                // Interrupt Service Routine Disable
	//   W5500_CS_LOW;                              // CS=0, SPI start
	SPI_set_slave_select();
	cmd_buffer[0] = (addrbsb & 0x00FF0000) >> 16; // Address byte 1
	cmd_buffer[1] = (addrbsb & 0x0000FF00) >> 8;
	cmd_buffer[2] = (addrbsb & 0x000000F8) + 4;
	cmd_buffer[3] = data;
	//  wait_ready();
	//    SPI_transfer_block(net_core_spi, cmd_buffer, 4, 0, 0);
	SPI_net_transfer_block( cmd_buffer, 4, 0, 0);
	//  wait_ready();
	SPI_clear_slave_select();

	// Data write (write 1byte data)
	//   W5500_CS_HIGH;                               // CS=1,  SPI end
	//  IINCHIP_ISR_ENABLE();                         // Interrupt Service Routine Enable
}

uint8_t IINCHIP_READ(uint32_t addrbsb)
{
	uint8_t data[1];
	unsigned char cmd_buffer[4];
	//	IINCHIP_ISR_DISABLE();                        // Interrupt Service Routine Disable
	SPI_set_slave_select();
	cmd_buffer[0] = (addrbsb & 0x00FF0000) >> 16; // Address byte 1
	cmd_buffer[1] = (addrbsb & 0x0000FF00) >> 8;
	cmd_buffer[2] = (addrbsb & 0x000000F8);

	//  wait_ready();

	//    SPI_transfer_block(net_core_spi, cmd_buffer, 3, data, 1);
	SPI_net_transfer_block( cmd_buffer, 3, data, 1);
	//  wait_ready();
	SPI_clear_slave_select();
	// if(addrbsb==Sn_CR(0))return 0;  //debug
	LOG_INFO("chip read=0x%02x\n", data[0]);
	return data[0];
}

uint16_t wiz_write_buf(uint32_t addrbsb, uint8_t *buf, uint16_t len)
{
	uint16_t idx = 0;
	unsigned char cmd_buffer[2048];
	// CS=0, SPI start
	SPI_set_slave_select();
	cmd_buffer[0] = (addrbsb & 0x00FF0000) >> 16; // Address byte 1
	cmd_buffer[1] = (addrbsb & 0x0000FF00) >> 8;
	cmd_buffer[2] = (addrbsb & 0x000000F8) + 4;

	for (idx = 0; idx < len; idx++) // Write data in loop
	{
		cmd_buffer[idx + 3] = buf[idx];
	}
	//    SPI_transfer_block(net_core_spi, cmd_buffer, (idx+3), 0, 0);
	   SPI_net_transfer_block( cmd_buffer, (idx+3), 0, 0);
	//  wait_ready();
	SPI_clear_slave_select();

	//  IINCHIP_ISR_ENABLE();                         // Interrupt Service Routine Enable

	return len;
}

uint16_t wiz_read_buf(uint32_t addrbsb, uint8_t *buf, uint16_t len)
{
	unsigned char cmd_buffer[3];
	//unsigned char rx_buffer[100];
	//  IINCHIP_ISR_DISABLE();
	SPI_set_slave_select();
	cmd_buffer[0] = (addrbsb & 0x00FF0000) >> 16; // Address byte 1
	cmd_buffer[1] = (addrbsb & 0x0000FF00) >> 8;
	cmd_buffer[2] = (addrbsb & 0x000000F8);
//	SPI_transfer_block(net_core_spi, cmd_buffer, 3, buf, len);	   //������
	SPI_net_transfer_block( cmd_buffer, 3, buf, len); //������

	SPI_clear_slave_select();

	return len;
}

///**
//@brief  This function is for resetting of the iinchip. Initializes the iinchip to work in whether DIRECT or INDIRECT mode
//*/
void iinchip_init(void)
{
	setMR(MR_RST);
}

///**
//@brief  This function set the transmit & receive buffer size as per the channels is used
//Note for TMSR and RMSR bits are as follows\n
//bit 1-0 : memory size of channel #0 \n
//bit 3-2 : memory size of channel #1 \n
//bit 5-4 : memory size of channel #2 \n
//bit 7-6 : memory size of channel #3 \n
//bit 9-8 : memory size of channel #4 \n
//bit 11-10 : memory size of channel #5 \n
//bit 12-12 : memory size of channel #6 \n
//bit 15-14 : memory size of channel #7 \n
//Maximum memory size for Tx, Rx in the W5500 is 16K Bytes,\n
//In the range of 16KBytes, the memory size could be allocated dynamically by each channel.\n
//Be attentive to sum of memory size shouldn't exceed 8Kbytes\n
//and to data transmission and receiption from non-allocated channel may cause some problems.\n
//If the 16KBytes memory is already  assigned to centain channel, \n
//other 3 channels couldn't be used, for there's no available memory.\n
//If two 4KBytes memory are assigned to two each channels, \n
//other 2 channels couldn't be used, for there's no available memory.\n
//*/
void sysinit(uint8_t *tx_size, uint8_t *rx_size)
{
	int16_t i;
	int16_t ssum, rsum;
	ssum = 0;
	rsum = 0;

	for (i = 0; i < 8; i++) // Set the size, masking and base address of Tx & Rx memory by each channel
	{
		IINCHIP_WRITE((Sn_TXMEM_SIZE(i)), tx_size[i]);
		IINCHIP_WRITE((Sn_RXMEM_SIZE(i)), rx_size[i]);

		SSIZE[i] = (int16_t)(0);
		RSIZE[i] = (int16_t)(0);

		if (ssum <= 16384)
		{
			switch (tx_size[i])
			{
			case 1:
				SSIZE[i] = (int16_t)(1024);
				break;
			case 2:
				SSIZE[i] = (int16_t)(2048);
				break;
			case 4:
				SSIZE[i] = (int16_t)(4096);
				break;
			case 8:
				SSIZE[i] = (int16_t)(8192);
				break;
			case 16:
				SSIZE[i] = (int16_t)(16384);
				break;
			default:
				RSIZE[i] = (int16_t)(2048);
				break;
			}
		}

		if (rsum <= 16384)
		{
			switch (rx_size[i])
			{
			case 1:
				RSIZE[i] = (int16_t)(1024);
				break;
			case 2:
				RSIZE[i] = (int16_t)(2048);
				break;
			case 4:
				RSIZE[i] = (int16_t)(4096);
				break;
			case 8:
				RSIZE[i] = (int16_t)(8192);
				break;
			case 16:
				RSIZE[i] = (int16_t)(16384);
				break;
			default:
				RSIZE[i] = (int16_t)(2048);
				break;
			}
		}
		ssum += SSIZE[i];
		rsum += RSIZE[i];
	}
}

//// added

///**
//@brief  This function sets up gateway IP address.
//*/
void setGAR(
	uint8_t *addr /**< a pointer to a 4 -byte array responsible to set the Gateway IP address. */
)
{
	wiz_write_buf(GAR0, addr, 4);
}
void getGWIP(uint8_t *addr)
{
	wiz_read_buf(GAR0, addr, 4);
}

/**
@brief  It sets up SubnetMask address
*/
void setSUBR(uint8_t *addr)
{
	wiz_write_buf(SUBR0, addr, 4);
}
/**
@brief  This function sets up MAC address.
*/
void setSHAR(
	uint8_t *addr /**< a pointer to a 6 -byte array responsible to set the MAC address. */
)
{
	wiz_write_buf(SHAR0, addr, 6);
}

/**
@brief  This function sets up Source IP address.
*/
void setSIPR(
	uint8_t *addr /**< a pointer to a 4 -byte array responsible to set the Source IP address. */
)
{
	wiz_write_buf(SIPR0, addr, 4);
}

/**
@brief  This function sets up Source IP address.
*/
void getGAR(uint8_t *addr)
{
	wiz_read_buf(GAR0, addr, 4);
}
void getSUBR(uint8_t *addr)
{
	wiz_read_buf(SUBR0, addr, 4);
}
void getSHAR(uint8_t *addr)
{
	wiz_read_buf(SHAR0, addr, 6);
}
void getSIPR(uint8_t *addr)
{
	wiz_read_buf(SIPR0, addr, 4);
}

void setMR(uint8_t val)
{
	IINCHIP_WRITE(MR, val);
}

/**
@brief  This function gets Interrupt register in common register.
 */
uint8_t getIR(void)
{
	return IINCHIP_READ(IR);
}

///**
//@brief  This function sets up Retransmission time.

//If there is no response from the peer or delay in response then retransmission
//will be there as per RTR (Retry Time-value Register)setting
//*/
void setRTR(uint16_t timeout)
{
	IINCHIP_WRITE(RTR0, (uint8_t)((timeout & 0xff00) >> 8));
	IINCHIP_WRITE(RTR1, (uint8_t)(timeout & 0x00ff));
}

///**
//@brief  This function set the number of Retransmission.

//If there is no response from the peer or delay in response then recorded time
//as per RTR & RCR register seeting then time out will occur.
//*/
void setRCR(uint8_t retry)
{
	IINCHIP_WRITE(WIZ_RCR, retry);
}

///**
//@brief  This function set the interrupt mask Enable/Disable appropriate Interrupt. ('1' : interrupt enable)

//If any bit in IMR is set as '0' then there is not interrupt signal though the bit is
//set in IR register.
//*/
void clearIR(uint8_t mask)
{
	IINCHIP_WRITE(IR, ~mask | getIR()); // must be setted 0x10.
}

///**
//@brief  This sets the maximum segment size of TCP in Active Mode), while in Passive Mode this is set by peer
//*/
void setSn_MSS(SOCKET s, uint16_t Sn_MSSR)
{
	IINCHIP_WRITE(Sn_MSSR0(s), (uint8_t)((Sn_MSSR & 0xff00) >> 8));
	IINCHIP_WRITE(Sn_MSSR1(s), (uint8_t)(Sn_MSSR & 0x00ff));
}

void setSn_TTL(SOCKET s, uint8_t ttl)
{
	IINCHIP_WRITE(Sn_TTL(s), ttl);
}

/**
@brief  get socket interrupt status

These below functions are used to read the Interrupt & Soket Status register
*/
uint8_t getSn_IR(SOCKET s)
{
	return IINCHIP_READ(Sn_IR(s));
}

/**
@brief   get socket status
*/
uint8_t getSn_SR(SOCKET s)
{
	return IINCHIP_READ(Sn_SR(s));
}

uint8_t getPHYCFGR(void)
{
	return IINCHIP_READ(PHYCFGR);
}

uint8_t getVersion(void)
{
	return IINCHIP_READ(VERSIONR);
}
///**
//@brief  get socket TX free buf size

//This gives free buffer size of transmit buffer. This is the data size that user can transmit.
//User shuold check this value first and control the size of transmitting data
//*/
uint16_t getSn_TX_FSR(SOCKET s)
{
	uint16_t val = 0, val1 = 0;
	do
	{
		val1 = IINCHIP_READ(Sn_TX_FSR0(s));
		val1 = (val1 << 8) + IINCHIP_READ(Sn_TX_FSR1(s));
		if (val1 != 0)
		{
			val = IINCHIP_READ(Sn_TX_FSR0(s));
			val = (val << 8) + IINCHIP_READ(Sn_TX_FSR1(s));
		}
	} while (val != val1);
	return val;
}

///**
//@brief   get socket RX recv buf size

//This gives size of received data in receive buffer.
//*/
uint16_t getSn_RX_RSR(SOCKET s)
{
	uint16_t val = 0, val1 = 0;
	do
	{
		val1 = IINCHIP_READ(Sn_RX_RSR0(s));
		val1 = (val1 << 8) + IINCHIP_READ(Sn_RX_RSR1(s));
		if (val1 != 0)
		{
			val = IINCHIP_READ(Sn_RX_RSR0(s));
			val = (val << 8) + IINCHIP_READ(Sn_RX_RSR1(s));
		}
	} while (val != val1);
	return val;
}

///**
//@brief   This function is being called by send() and sendto() function also.

//This function read the Tx write pointer register and after copy the data in buffer update the Tx write pointer
//register. User should read upper byte first and lower byte later to get proper value.
//*/
void send_data_processing(SOCKET s, uint8_t *data, uint16_t len)
{
	uint16_t ptr = 0;
	uint32_t addrbsb = 0;
	if (len == 0)
	{
		//DBG("CH: %d Unexpected1 length 0\r\n", s);
		return;
	}

	ptr = IINCHIP_READ(Sn_TX_WR0(s));
	ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_TX_WR1(s));

	addrbsb = ((uint32_t)ptr << 8) + (s << 5) + 0x10;
	wiz_write_buf(addrbsb, data, len);

	ptr += len;
	IINCHIP_WRITE(Sn_TX_WR0(s), (uint8_t)((ptr & 0xff00) >> 8));
	IINCHIP_WRITE(Sn_TX_WR1(s), (uint8_t)(ptr & 0x00ff));
}

///**
//@brief  This function is being called by recv() also.

//This function read the Rx read pointer register
//and after copy the data from receive buffer update the Rx write pointer register.
//User should read upper byte first and lower byte later to get proper value.
//*/
void recv_data_processing(SOCKET s, uint8_t *data, uint16_t len)
{
	uint16_t ptr = 0;
	uint32_t addrbsb = 0;

	if (len == 0)
	{
		//DBG("CH: %d Unexpected2 length 0\r\n", s);
		return;
	}

	ptr = IINCHIP_READ(Sn_RX_RD0(s));
	ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_RX_RD1(s));

	addrbsb = ((uint32_t)ptr << 8) + (s << 5) + 0x18;
	wiz_read_buf(addrbsb, data, len);
	ptr += len;

	IINCHIP_WRITE(Sn_RX_RD0(s), (uint8_t)((ptr & 0xff00) >> 8));
	IINCHIP_WRITE(Sn_RX_RD1(s), (uint8_t)(ptr & 0x00ff));
}

void setSn_IR(uint8_t s, uint8_t val)
{
	IINCHIP_WRITE(Sn_IR(s), val);
}

#endif
