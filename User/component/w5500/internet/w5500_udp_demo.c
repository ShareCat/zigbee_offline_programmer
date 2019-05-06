/**
******************************************************************************
* @file         udp_demo.c
* @author       WIZnet Software Team
* @version      V1.0
* @date         2015-02-14
* @brief        UDP��ʾ����
******************************************************************************
**/

#include "w5500_udp_demo.h"
#include "./../w5500_conf.h"
#include "./../w5500_socket.h"
#include "./../w5500_utility.h"

#include "./../../../sys/sys_command_line.h"


#define DEBUG_W5500_UDP_DEMO FALSE  /* ���Կ��� */

static uint8_t udp_buff[2048];      /* ����һ��2KB�Ļ��� */
static uint16_t udp_rx_len = 0;

/**
*@brief     UDP���Գ���
*@param     ��
*@return    ��
*/
void do_udp(void)
{
    switch(getSn_SR(SOCK_UDPS)) {   /* ��ȡsocket��״̬ */
        case SOCK_CLOSED:           /* socket���ڹر�״̬ */
            socket(SOCK_UDPS, Sn_MR_UDP, w5500_config.lport, 0);    /* ��ʼ��socket */
            break;

        case SOCK_UDP:              /* socket��ʼ����� */
            if(getSn_IR(SOCK_UDPS) & Sn_IR_RECV) {
                setSn_IR(SOCK_UDPS, Sn_IR_RECV);            /* ������ж� */
            }

            if((udp_rx_len = getSn_RX_RSR(SOCK_UDPS)) > 0) {    /* ���յ����� */
                recvfrom(SOCK_UDPS, udp_buff, udp_rx_len, w5500_config.rip, &w5500_config.rport);   /* W5500���ռ���������������� */

                if(udp_rx_len <= 2048 + 8) {    /* ȷ��û��Խ�� */
#if DEBUG_W5500_UDP_DEMO
                    udp_buff[udp_rx_len - 8] = 0x00;    /* ����ַ��������� */
                    PRINTF("udp: %s\r\n", udp_buff);    /* ��ӡ���ջ��� */
                    sendto(SOCK_UDPS, udp_buff, udp_rx_len - 8, w5500_config.rip, w5500_config.rport);  /* W5500�ѽ��յ������ݷ��͸�Remote */
#endif
                }
            }

            break;
    }

}

