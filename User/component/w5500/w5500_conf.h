

#ifndef _W5500_CONF_H_
#define _W5500_CONF_H_


#include <stdint.h>
#include <stdio.h>
#include "stm32f10x.h"


#define __GNUC__


/* -------------------------------------------------------- 调用IO底层---开始 */

/**
*@brief     设置W5500的片选端口SCSn为低
*@param     无
*@return    无
*/
#define IINCHIP_CS_OFF()            do {                            \
                                        spi_cs_ctrl_w5500(DISABLE); \
                                    }while(0)
/**
*@brief     设置W5500的片选端口SCSn为高
*@param     无
*@return    无
*/
#define IINCHIP_CS_ON()             do {                            \
                                        spi_cs_ctrl_w5500(ENABLE);  \
                                    }while(0)

/**
*@brief     W5500 SPI读写一个字节数据
*@param     dat：写入的一个字节数据
*@return    返回一个字节数据
*/
#define IINCHIP_SPI_SEND_BYTE(dat)  spi_send_byte_w5500(dat)

/**
*@brief     W5500 IO初始化，包括SPI和RESET引脚、INT引脚
*@param     无
*@return    无
*/
#define W5500_GPIO_CONFIG()         gpio_config_w5500()


#define W5500_RST_LOW()             do {                            \
                                        rst_ctrl_w5500(DISABLE);    \
                                    }while(0)

#define W5500_RST_HIGH()            do {                            \
                                        rst_ctrl_w5500(ENABLE);     \
                                    }while(0)

/* -------------------------------------------------------- 调用IO底层---结束 */


#define MAX_BUF_SIZE        1460            /* 定义每个数据包的大小 */
#define KEEP_ALIVE_TIME     30              // 30sec
#define TX_RX_MAX_BUF_SIZE  2048


#pragma pack(1)
/* 此结构体定义了W5500可供配置的主要参数 */
typedef struct {
#define FW_VER_HIGH     1
#define FW_VER_LOW      0
#define IP_FROM_DEFINE  0       /* 使用初始设置的IP */
#define IP_FROM_DHCP    1       /* 使用DHCP获取IP */
#define LOCAL_PORT      27106   /* 本地端口 */
#define REMOTE_PORT     27109   /* 远程端口，平台的端口 */

    uint8_t  mac[6];            /* MAC地址 */
    uint8_t  lip[4];            /* local IP本地IP地址 */
    uint16_t lport;             /* local port本地端口 */
    uint8_t  sub[4];            /* 子网掩码 */
    uint8_t  gw[4];             /* 网关 */
    uint8_t  dns[4];            /* DNS服务器地址 */
    uint8_t  rip[4];            /* remote IP远程IP地址 */
    uint16_t rport;             /* remote port远程端口 */
    uint8_t  sw_ver[2];         /* 软件版本号 */
    uint8_t  ip_from;           /* IP从DHCP获取，还是使用设置的静态IP */
} W5500_CONFIG_S;
#pragma pack()

extern W5500_CONFIG_S w5500_config;
extern const W5500_CONFIG_S w5500_config_default;

extern uint8_t dhcp_ok;           /* DHCP获取成功 */
extern uint32_t dhcp_time;        /* DHCP运行计数 */
extern uint8_t ntptimer;          /* NPT秒计数 */

/* MCU配置相关函数 */
void timer2_init(void);                     /* STM32定时器2初始化 */
void timer2_isr(void);                      /* 定时器中断执行的函数 */
void reboot(void);                          /* STM32软复位 */

/* W5500SPI相关函数 */
void IINCHIP_WRITE(uint32_t addrbsb, uint8_t data);     /* 写入一个8位数据到W5500 */
uint8_t IINCHIP_READ(uint32_t addrbsb);                     /* 从W5500读出一个8位数据 */
uint16_t wiz_write_buf(uint32_t addrbsb, uint8_t* buf, uint16_t len);   /* 向W5500写入len字节数据 */
uint16_t wiz_read_buf(uint32_t addrbsb, uint8_t* buf, uint16_t len);    /* 从W5500读出len字节数据 */

/* W5500基本配置相关函数 */
void reset_w5500(void);                     /* 硬复位W5500 */
void set_w5500_mac(void);                   /* 配置W5500的MAC地址 */
void set_w5500_ip(void);                    /* 配置W5500的IP地址 */

/*需要用定时的的应用函数*/
void dhcp_timer_init(void);                 /* dhcp用到的定时器初始化 */
void ntp_timer_init(void);                  /* npt用到的定时器初始化 */


#endif /* _W5500_CONF_H_ */

