#ifndef __W5500_UDP_DEMO_H
#define __W5500_UDP_DEMO_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "./../w5500.h"

#include "stm32f10x.h"


extern uint16_t udp_port;     /* ����UDP��һ���˿ڲ���ʼ�� */
void do_udp(void);

#endif /* __W5500_UDP_DEMO_H */

