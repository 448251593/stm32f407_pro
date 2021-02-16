#ifndef _LOOPBACK_H_
#define _LOOPBACK_H_

#include <stdint.h>
#include "fifobuffer.h"
#include "app_fifo.h"


/* Loopback test debug message printout enable */
#define	_LOOPBACK_DEBUG_
#define   FIFO_SELECT       1
/* DATA_BUF_SIZE define for Loopback example */
#ifndef DATA_BUF_SIZE
	#define DATA_BUF_SIZE			(1024*4)//(1460*5)
#endif

/************************/
/* Select LOOPBACK_MODE */
/************************/
#define LOOPBACK_MAIN_NOBLOCK    0
#define LOOPBACK_MODE   LOOPBACK_MAIN_NOBLOCK

extern FT_FIFO              spi_net_send_Fifo;
extern app_fifo_t           spi_net_send_ff;
/* TCP server Loopback test example */
int32_t loopback_tcps(uint8_t sn, uint8_t* buf, uint16_t port);

/* TCP client Loopback test example */
// int32_t loopback_tcpc(uint8_t sn, uint8_t* buf, uint8_t* destip, uint16_t destport);
int32_t loopback_tcpc(void);
/* UDP Loopback test example */
int32_t loopback_udps(uint8_t sn, uint8_t* buf, uint16_t port);
void   w5500_send_put(char *p, uint32_t len);
void     w5500_send_flush(void);
void  NetLoop(void);
void  w5500_send( void);
void  w5500_send_ack( uint8_t *pdat_t, uint16_t  len);
#endif
