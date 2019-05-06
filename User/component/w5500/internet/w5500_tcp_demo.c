/**
********************************************************************************
* @file         tcp_demo.c
* @author       WIZnet Software Team
* @version      V1.0
* @date         2015-02-14
* @brief        TCP ��ʾ����
* @attention
********************************************************************************
**/

#include "w5500_tcp_demo.h"
#include "./../w5500_conf.h"
#include "./../w5500_socket.h"

#include "./../../../sys/sys_command_line.h"


#define DEBUG_W5500_TCP_DEMO FALSE      /* ���Կ��� */

static uint8_t tcp_buff[2048];          /* ����һ��2KB�Ļ��� */
static uint16_t tcp_rx_len = 0;

/**
*@brief     TCP Server�ػ���ʾ������
*@param     ��
*@return    ��
*/
void do_tcp_server(void)
{
    switch(getSn_SR(SOCK_TCPS)) {       /* ��ȡsocket��״̬ */
        case SOCK_CLOSED:               /* socket���ڹر�״̬ */
            socket(SOCK_TCPS, Sn_MR_TCP, w5500_config.lport, Sn_MR_ND); /* ��socket */
            break;

        case SOCK_INIT:                 /* socket�ѳ�ʼ��״̬ */
            listen(SOCK_TCPS);          /* socket�������� */
            break;

        case SOCK_ESTABLISHED:          /* socket�������ӽ���״̬ */

            if(getSn_IR(SOCK_TCPS) & Sn_IR_CON) {
                setSn_IR(SOCK_TCPS, Sn_IR_CON);     /* ��������жϱ�־λ */
            }

            tcp_rx_len = getSn_RX_RSR(SOCK_TCPS);   /* ����lenΪ�ѽ������ݵĳ��� */

            if(tcp_rx_len > 0) {
                recv(SOCK_TCPS, tcp_buff, tcp_rx_len);  /* ��������Client������ */
                tcp_buff[tcp_rx_len] = 0x00;            /* ����ַ��������� */
#if DEBUG_W5500_TCP_DEMO
                PRINTF("%s\r\n", tcp_buff);
#endif
                send(SOCK_TCPS, tcp_buff, tcp_rx_len);  /* ��Client�������� */
            }

            break;

        case SOCK_CLOSE_WAIT:                   /* socket���ڵȴ��ر�״̬ */
            close(SOCK_TCPS);
            break;
    }
}

/**
*@brief     TCP Client�ػ���ʾ������
*@param     ��
*@return    ��
*/
void do_tcp_client(void)
{
    uint16_t index;

    switch(getSn_SR(SOCK_TCPC)) {       /* ��ȡsocket��״̬ */
        case SOCK_CLOSED:               /* socket���ڹر�״̬ */
            socket(SOCK_TCPC, Sn_MR_TCP, w5500_config.lport++, Sn_MR_ND);
            break;

        case SOCK_INIT:                 /* socket���ڳ�ʼ��״̬ */
            connect_no_check(SOCK_TCPC, w5500_config.rip, w5500_config.rport); /* socket���ӷ����� */
            break;

        case SOCK_ESTABLISHED:          /* socket�������ӽ���״̬ */
            if(getSn_IR(SOCK_TCPC) & Sn_IR_CON) {
                setSn_IR(SOCK_TCPC, Sn_IR_CON);     /* ��������жϱ�־λ */
            }

            len = getSn_RX_RSR(SOCK_TCPC);      /* ����lenΪ�ѽ������ݵĳ��� */

            if((len > 0) && (len < 2048)) {
                recv(SOCK_TCPC, tcp_buff, len);     /* ��������Server������ */
                tcp_buff[len] = 0x00;               /* ����ַ��������� */
#if DEBUG_W5500_TCP_DEMO
                PRINTF("%s\r\n", tcp_buff);
                send(SOCK_TCPC, tcp_buff, len);     /* ��Server�������� */
#endif

                extern volatile QUEUE2048_S w5500_rx_queue;

                for(index = 0; index < len; index++) {
                    QUEUE_IN(w5500_rx_queue, tcp_buff[index]);  /* �յ����������� */
                }
            }

            break;

        case SOCK_CLOSE_WAIT:                   /* socket���ڵȴ��ر�״̬ */
            close(SOCK_TCPC);
            break;

    }
}

