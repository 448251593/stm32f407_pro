/*
*
@file		socket.h
@brief	define function of socket API 
*
*/

#ifndef	_SOCKET_H_
#define	_SOCKET_H_

typedef uint8_t 			SOCKET;
extern uint8_t socket(SOCKET s, uint8_t protocol, uint16_t port, uint8_t flag); // Opens a socket(TCP or UDP or IP_RAW mode)
extern void close(SOCKET s); // Close socket
extern uint8_t connect(SOCKET s, uint8_t * addr, uint16_t port); // Establish TCP connection (Active connection)
extern void disconnect(SOCKET s); // disconnect the connection
extern uint8_t listen(SOCKET s);	// Establish TCP connection (Passive connection)
extern uint16_t send(SOCKET s, const uint8_t * buf, uint16_t len); // Send data (TCP)
extern uint16_t recv(SOCKET s, uint8_t * buf, uint16_t len);	// Receive data (TCP)
extern uint16_t sendto(SOCKET s, const uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t port); // Send data (UDP/IP RAW)
extern uint16_t recvfrom(SOCKET s, uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t  *port); // Receive data (UDP/IP RAW)

#ifdef __MACRAW__
void macraw_open(void);
uint16_t macraw_send( const uint8_t * buf, uint16_t len ); //Send data (MACRAW)
uint16_t macraw_recv( uint8_t * buf, uint16_t len ); //Recv data (MACRAW)
#endif


enum socket_connect_flag
 {
  Disconnect,
  FirstConnect,
  Connect,
  Connect_none,
 };	 
 #ifndef DATA_BUF_SIZE
	#define DATA_BUF_SIZE			(1024*2)//SPI_NET_DMA_BUFFER_SIZE //(1024*6)
#endif
#define   NET_SEND_BUF_SIZE      1000*114//
extern uint16_t   socket_state;
extern uint16_t   heart_beat_time;
extern uint16_t   protocal_overtime;
extern uint16_t   rx_tail;
void NetLoop(void);
void   w5500_send_put(char *p, uint32_t len);
void      w5500_fifo_init(void);
unsigned short send_for_dma(SOCKET s, const uint8_t * buf, unsigned short len);
#endif
/* _SOCKET_H_ */
