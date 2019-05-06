/**
******************************************************************************
* @file         udp_demo.c
* @author       WIZnet Software Team
* @version      V1.0
* @date         2015-02-14
* @brief        UDP演示函数
******************************************************************************
**/

#include "w5500_udp_demo.h"
#include "./../w5500_conf.h"
#include "./../w5500_socket.h"
#include "./../w5500_utility.h"

#include "./../../../sys/sys_command_line.h"


#define DEBUG_W5500_UDP_DEMO FALSE  /* 调试开关 */

static uint8_t udp_buff[2048];      /* 定义一个2KB的缓存 */
static uint16_t udp_rx_len = 0;

/**
*@brief     UDP测试程序
*@param     无
*@return    无
*/
void do_udp(void)
{
    switch(getSn_SR(SOCK_UDPS)) {   /* 获取socket的状态 */
        case SOCK_CLOSED:           /* socket处于关闭状态 */
            socket(SOCK_UDPS, Sn_MR_UDP, w5500_config.lport, 0);    /* 初始化socket */
            break;

        case SOCK_UDP:              /* socket初始化完成 */
            if(getSn_IR(SOCK_UDPS) & Sn_IR_RECV) {
                setSn_IR(SOCK_UDPS, Sn_IR_RECV);            /* 清接收中断 */
            }

            if((udp_rx_len = getSn_RX_RSR(SOCK_UDPS)) > 0) {    /* 接收到数据 */
                recvfrom(SOCK_UDPS, udp_buff, udp_rx_len, w5500_config.rip, &w5500_config.rport);   /* W5500接收计算机发送来的数据 */

                if(udp_rx_len <= 2048 + 8) {    /* 确定没有越界 */
#if DEBUG_W5500_UDP_DEMO
                    udp_buff[udp_rx_len - 8] = 0x00;    /* 添加字符串结束符 */
                    PRINTF("udp: %s\r\n", udp_buff);    /* 打印接收缓存 */
                    sendto(SOCK_UDPS, udp_buff, udp_rx_len - 8, w5500_config.rip, w5500_config.rport);  /* W5500把接收到的数据发送给Remote */
#endif
                }
            }

            break;
    }

}

