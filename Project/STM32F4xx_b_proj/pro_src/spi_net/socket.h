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

#endif
/* _SOCKET_H_ */
