/**
********************************************************************************
* @file         tcp_demo.c
* @author       WIZnet Software Team
* @version      V1.0
* @date         2015-02-14
* @brief        TCP 演示函数
* @attention
********************************************************************************
**/

#include "w5500_tcp_demo.h"
#include "./../w5500_conf.h"
#include "./../w5500_socket.h"

#include "./../../../sys/sys_command_line.h"


#define DEBUG_W5500_TCP_DEMO FALSE      /* 调试开关 */

static uint8_t tcp_buff[2048];          /* 定义一个2KB的缓存 */
static uint16_t tcp_rx_len = 0;

/**
*@brief     TCP Server回环演示函数。
*@param     无
*@return    无
*/
void do_tcp_server(void)
{
    switch(getSn_SR(SOCK_TCPS)) {       /* 获取socket的状态 */
        case SOCK_CLOSED:               /* socket处于关闭状态 */
            socket(SOCK_TCPS, Sn_MR_TCP, w5500_config.lport, Sn_MR_ND); /* 打开socket */
            break;

        case SOCK_INIT:                 /* socket已初始化状态 */
            listen(SOCK_TCPS);          /* socket建立监听 */
            break;

        case SOCK_ESTABLISHED:          /* socket处于连接建立状态 */

            if(getSn_IR(SOCK_TCPS) & Sn_IR_CON) {
                setSn_IR(SOCK_TCPS, Sn_IR_CON);     /* 清除接收中断标志位 */
            }

            tcp_rx_len = getSn_RX_RSR(SOCK_TCPS);   /* 定义len为已接收数据的长度 */

            if(tcp_rx_len > 0) {
                recv(SOCK_TCPS, tcp_buff, tcp_rx_len);  /* 接收来自Client的数据 */
                tcp_buff[tcp_rx_len] = 0x00;            /* 添加字符串结束符 */
#if DEBUG_W5500_TCP_DEMO
                PRINTF("%s\r\n", tcp_buff);
#endif
                send(SOCK_TCPS, tcp_buff, tcp_rx_len);  /* 向Client发送数据 */
            }

            break;

        case SOCK_CLOSE_WAIT:                   /* socket处于等待关闭状态 */
            close(SOCK_TCPS);
            break;
    }
}

/**
*@brief     TCP Client回环演示函数。
*@param     无
*@return    无
*/
void do_tcp_client(void)
{
    uint16_t index;

    switch(getSn_SR(SOCK_TCPC)) {       /* 获取socket的状态 */
        case SOCK_CLOSED:               /* socket处于关闭状态 */
            socket(SOCK_TCPC, Sn_MR_TCP, w5500_config.lport++, Sn_MR_ND);
            break;

        case SOCK_INIT:                 /* socket处于初始化状态 */
            connect_no_check(SOCK_TCPC, w5500_config.rip, w5500_config.rport); /* socket连接服务器 */
            break;

        case SOCK_ESTABLISHED:          /* socket处于连接建立状态 */
            if(getSn_IR(SOCK_TCPC) & Sn_IR_CON) {
                setSn_IR(SOCK_TCPC, Sn_IR_CON);     /* 清除接收中断标志位 */
            }

            len = getSn_RX_RSR(SOCK_TCPC);      /* 定义len为已接收数据的长度 */

            if((len > 0) && (len < 2048)) {
                recv(SOCK_TCPC, tcp_buff, len);     /* 接收来自Server的数据 */
                tcp_buff[len] = 0x00;               /* 添加字符串结束符 */
#if DEBUG_W5500_TCP_DEMO
                PRINTF("%s\r\n", tcp_buff);
                send(SOCK_TCPC, tcp_buff, len);     /* 向Server发送数据 */
#endif

                extern volatile QUEUE2048_S w5500_rx_queue;

                for(index = 0; index < len; index++) {
                    QUEUE_IN(w5500_rx_queue, tcp_buff[index]);  /* 收到的数据入列 */
                }
            }

            break;

        case SOCK_CLOSE_WAIT:                   /* socket处于等待关闭状态 */
            close(SOCK_TCPC);
            break;

    }
}

