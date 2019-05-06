
#ifndef __W5500_SOCKET_H_
#define __W5500_SOCKET_H_

#include <stdint.h>
#include "w5500.h"

#include "stm32f10x.h"


/* Socket 端口选择，可按自己的习惯定义 */
#define SOCK_TCPS               0
#define SOCK_HUMTEM             0
#define SOCK_PING               0
#define SOCK_TCPC               1
#define SOCK_UDPS               2
#define SOCK_WEIBO              2
#define SOCK_DHCP               3
#define SOCK_HTTPS              4
#define SOCK_DNS                5
#define SOCK_SMTP               6
#define SOCK_NTP                7
#define SOCK_HTTPC              7
//#define NETBIOS_SOCK    6 //在netbios.c已定义

extern uint8_t socket(SOCKET s, uint8_t protocol, uint16_t port, uint8_t flag); // Opens a socket(TCP or UDP or IP_RAW mode)
extern void close(SOCKET s); // Close socket
extern uint8_t connect_check(SOCKET s);
extern uint8_t connect_no_check(SOCKET s, uint8_t* addr, uint16_t port);
extern uint8_t connect(SOCKET s, uint8_t* addr, uint16_t port); // Establish TCP connection (Active connection)
extern void disconnect(SOCKET s); // disconnect the connection
extern uint8_t listen(SOCKET s);    // Establish TCP connection (Passive connection)
extern uint16_t send(SOCKET s, const uint8_t* buf, uint16_t len); // Send data (TCP)
extern uint16_t recv(SOCKET s, uint8_t* buf, uint16_t len); // Receive data (TCP)
extern uint8_t sendto_check(SOCKET s);
extern uint16_t sendto_no_check(SOCKET s, const uint8_t* buf, uint16_t len, uint8_t* addr, uint16_t port); // Send data (UDP/IP RAW)
extern uint16_t sendto(SOCKET s, const uint8_t* buf, uint16_t len, uint8_t* addr, uint16_t port); // Send data (UDP/IP RAW)
extern uint16_t recvfrom(SOCKET s, uint8_t* buf, uint16_t len, uint8_t* addr, uint16_t *port); // Receive data (UDP/IP RAW)

#ifdef __MACRAW__
    void macraw_open(void);
    uint16_t macraw_send(const uint8_t* buf, uint16_t len); //Send data (MACRAW)
    uint16_t macraw_recv(uint8_t * buf, uint16_t len); //Recv data (MACRAW)
#endif

#endif
/* __W5500_SOCKET_H_ */

