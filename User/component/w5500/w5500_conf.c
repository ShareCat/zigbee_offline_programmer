/*
**************************************************************************************************
* @file         w5500_conf.c
* @author       WIZnet Software Team
* @version      V1.0
* @date         2015-02-14
* @brief        配置MCU，移植W5500程序需要修改的文件，配置W5500的MAC和IP地址
**************************************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include "./../../sys/sys_command_line.h"

#include "w5500_conf.h"
#include "./../../bsp/bsp_spi.h"
#include "w5500_utility.h"
#include "w5500.h"
#include "./internet/w5500_dhcp.h"


#define DEBUG_W5500_CONFIG TRUE             /* 调试开关 */

W5500_CONFIG_S w5500_config;                /* 配置结构体 */

const W5500_CONFIG_S w5500_config_default = {
    {0x00, 0x08, 0xdc, 0x11, 0x11, 0x11},   /* mac地址，如果多块W5500网络适配板
                                                    在同一现场工作，请使用不同
                                                    的MAC地址*/
    {192, 168, 1, 6},                       /* local_ip */
    {LOCAL_PORT},                           /* local_port */
    {255, 255, 255, 0},                     /* subnet */
    {192, 168, 1, 1},                       /* gateway */
    {8, 8, 8, 8},                           /* 谷歌的DNS服务器 */
    //{223, 5, 5, 5},                           /* Alibaba的DNS服务器1 */
    //{223, 6, 6, 6},                           /* Alibaba的DNS服务器2 */

    {192, 168, 1, 8},                       /* remote_ip */
    {REMOTE_PORT},                          /* remote_port */
    {FW_VER_HIGH, FW_VER_LOW},              /* sw_ver */
    {IP_FROM_DHCP}                          /* 默认使用DHCP方式获取IP */
};


uint8_t  dhcp_ok = 0;                       /* dhcp成功获取IP */
uint32_t ms = 0;                            /* 毫秒计数 */
uint32_t dhcp_time = 0;                     /* DHCP运行计数定时1秒加一次 */
uint8_t ntptimer = 0;                       /* NPT秒计数 */

/**
*@brief     配置W5500的IP地址
*@param     无
*@return    无
*/
void set_w5500_ip(void)
{
    uint8_t temp_ip[4] = {0, 0, 0, 0};
    uint8_t temp_subnet[4] = {0, 0, 0, 0};
    uint8_t temp_gateway[4] = {0, 0, 0, 0};

#ifndef W5500_CONFIG_FROM_DATABASE
    /* 设置物理地址，6字节uint8_t数组,自己定义，用于唯一标识网络设备的物理地址值
            该地址值需要到IEEE申请，按照OUI的规定，前3个字节为厂商代码，后三个字
            节为产品序号如果自己定义物理地址，注意第一个字节必须为偶数 */
    memcpy(w5500_config.mac, w5500_config_default.mac, 6);
    /* 网关IP必须与本机IP属于同一个子网，否则本机将无法找到网关 */
    memcpy(w5500_config.lip, w5500_config_default.lip, 4);
    /* 子网掩码用于子网运算 */
    memcpy(w5500_config.sub, w5500_config_default.sub, 4);
    /* 使用网关可以使通信突破子网的局限，通过网关可以访问到其它子网或进入Internet */
    memcpy(w5500_config.gw, w5500_config_default.gw, 4);
    /* DNS服务器地址 */
    memcpy(w5500_config.dns, w5500_config_default.dns, 4);
    /* 通讯远端IP地址 */
    memcpy(w5500_config.rip, w5500_config_default.rip, 4);
    /* 通讯本地端口 */
    w5500_config.lport = w5500_config_default.lport;
    /* 通讯远端端口 */
    w5500_config.rport = w5500_config_default.rport;

    /* 软件版本信息，根据需要选用 */
    memcpy(w5500_config.sw_ver, w5500_config_default.sw_ver, 2);
#endif

#ifndef MAC_DEBUG
    /*
        从SPI_FLASH读取正确的MAC地址
    */
    
#endif

    if(w5500_config.ip_from == IP_FROM_DEFINE) {
#if DEBUG_W5500_CONFIG
        LOG("W5500 IP from define");
#endif
    } else if(w5500_config.ip_from == IP_FROM_DHCP) {    /*使用DHCP获取IP参数，需调用DHCP子函数*/
        if(dhcp_ok == 1) {  /* DHCP已经成功获取到IP */
#if DEBUG_W5500_CONFIG
            LOG("W5500 IP from DHCP ok");
#endif
            /* 复制DHCP获取的配置信息到配置结构体 */
            memcpy(w5500_config.lip, DHCP_GET.lip, 4);
            memcpy(w5500_config.sub, DHCP_GET.sub, 4);
            memcpy(w5500_config.gw, DHCP_GET.gw, 4);
            memcpy(w5500_config.dns, DHCP_GET.gw, 4);   /* 使用网关IP作为DNS服务器，也就是由路由器帮忙解析域名 */
            //memcpy(w5500_config.dns, DHCP_GET.dns, 4);/* 暂时不使用获取到的DNS */
        } else {
#if DEBUG_W5500_CONFIG
            LOG("W5500 DHCP fail\r\n");
#endif
        }
    }


    /* 将IP配置信息写入W5500相应寄存器 */
    setSUBR(w5500_config.sub);
    setGAR(w5500_config.gw);
    setSIPR(w5500_config.lip);

    /* 设置重试时间，默认为2000(200ms) 每一单位数值为100微秒,初始化时值设
            为2000(0x07D0),等于200毫秒 */
    setRTR(0x07D0);

    /*设置重试次数，默认为8次如果重发的次数超过设定值,则产生超时中断(相关
            的端口中断寄存器中的Sn_IR 超时位(TIMEOUT)置“1”) */
    setRCR(0x08);

#if DEBUG_W5500_CONFIG
    /* 查看打印是否成功设置IP等参数 */
    getSIPR(temp_ip);
    getSUBR(temp_subnet);
    getGAR(temp_gateway);
    LOGA("W5500 IP: %d.%d.%d.%d  Subnet: %d.%d.%d.%d  Gateway: %d.%d.%d.%d  DNS: %d.%d.%d.%d",
         temp_ip[0], temp_ip[1], temp_ip[2], temp_ip[3],
         temp_subnet[0], temp_subnet[1], temp_subnet[2], temp_subnet[3],
         temp_gateway[0], temp_gateway[1], temp_gateway[2], temp_gateway[3],
         w5500_config.dns[0], w5500_config.dns[1], w5500_config.dns[2], w5500_config.dns[3]);
#endif
}

/**
*@brief     配置W5500的MAC地址
*@param     无
*@return    无
*/
void set_w5500_mac(void)
{
#ifndef W5500_CONFIG_FROM_DATABASE
    memcpy(w5500_config.mac, w5500_config_default.mac, 6);
#endif
    setSHAR(w5500_config.mac);  /* 设置mac地址 */
    memcpy(DHCP_GET.mac, w5500_config.mac, 6);
}

/**
*@brief     W5500复位设置函数
*@param     无
*@return    无
*/
void reset_w5500(void)
{
    //W5500_RST_LOW();
    //delay_ms(10);
    //W5500_RST_HIGH();
    //delay_ms(900);
}


/**
*@brief     写入一个8位数据到W5500
*@param     addrbsb: 写入数据的地址
*@param   data：写入的8位数据
*@return    无
*/
void IINCHIP_WRITE(uint32_t addrbsb, uint8_t data)
{
    IINCHIP_CS_OFF();
    IINCHIP_SPI_SEND_BYTE((addrbsb & 0x00FF0000) >> 16);
    IINCHIP_SPI_SEND_BYTE((addrbsb & 0x0000FF00) >> 8);
    IINCHIP_SPI_SEND_BYTE((addrbsb & 0x000000F8) + 4);
    IINCHIP_SPI_SEND_BYTE(data);
    IINCHIP_CS_ON();
}

/**
*@brief     从W5500读出一个8位数据
*@param     addrbsb: 写入数据的地址
*@param   data：从写入的地址处读取到的8位数据
*@return    无
*/
uint8_t IINCHIP_READ(uint32_t addrbsb)
{
    uint8_t data = 0;

    IINCHIP_CS_OFF();
    IINCHIP_SPI_SEND_BYTE((addrbsb & 0x00FF0000) >> 16);
    IINCHIP_SPI_SEND_BYTE((addrbsb & 0x0000FF00) >> 8);
    IINCHIP_SPI_SEND_BYTE((addrbsb & 0x000000F8))    ;
    data = IINCHIP_SPI_SEND_BYTE(0x00);
    IINCHIP_CS_ON();

    return data;
}

/**
*@brief     向W5500写入len字节数据
*@param     addrbsb: 写入数据的地址
*@param   buf：写入字符串
*@param   len：字符串长度
*@return    len：返回字符串长度
*/
uint16_t wiz_write_buf(uint32_t addrbsb, uint8_t* buf, uint16_t len)
{
    uint16_t idx = 0;

    if(len == 0) {
        //PRINTF("Unexpected2 length 0\r\n");
        return len;
    }

    IINCHIP_CS_OFF();
    IINCHIP_SPI_SEND_BYTE((addrbsb & 0x00FF0000) >> 16);
    IINCHIP_SPI_SEND_BYTE((addrbsb & 0x0000FF00) >> 8);
    IINCHIP_SPI_SEND_BYTE((addrbsb & 0x000000F8) + 4);

    for(idx = 0; idx < len; idx++) {
        IINCHIP_SPI_SEND_BYTE(buf[idx]);
    }

    IINCHIP_CS_ON();
    return len;
}

/**
*@brief     从W5500读出len字节数据
*@param     addrbsb: 读取数据的地址
*@param     buf：存放读取数据
*@param     len：字符串长度
*@return    len：返回字符串长度
*/
uint16_t wiz_read_buf(uint32_t addrbsb, uint8_t* buf, uint16_t len)
{
    uint16_t idx = 0;

    if(len == 0) {
        //PRINTF("Unexpected2 length 0\r\n");
        return len;
    }

    IINCHIP_CS_OFF();
    IINCHIP_SPI_SEND_BYTE((addrbsb & 0x00FF0000) >> 16);
    IINCHIP_SPI_SEND_BYTE((addrbsb & 0x0000FF00) >> 8);
    IINCHIP_SPI_SEND_BYTE((addrbsb & 0x000000F8));

    for(idx = 0; idx < len; idx++) {
        buf[idx] = IINCHIP_SPI_SEND_BYTE(0x00);
    }

    IINCHIP_CS_ON();
    return len;
}

#if 0
/**
*@brief     STM32定时器2初始化
*@param     无
*@return    无
*/
void timer2_init(void)
{
    //TIM2_Configuration();             /* TIM2 定时配置 */
    //TIM2_NVIC_Configuration();        /* 定时器的中断优先级 */
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  /* TIM2 重新开时钟，开始计时 */
}

/**
*@brief     dhcp用到的定时器初始化
*@param     无
*@return    无
*/
void dhcp_timer_init(void)
{
    //timer2_init();
}

/**
*@brief     ntp用到的定时器初始化
*@param     无
*@return    无
*/
void ntp_timer_init(void)
{
    //timer2_init();
}

/**
*@brief     定时器2中断函数
*@param     无
*@return    无
*/
void timer2_isr(void)
{
    ms++;

    if(ms >= 100) {
        ms = 0;
        dhcp_time++;        /* DHCP定时加1S */
#ifndef __NTP_H__
        ntptimer++;         /* NTP重试时间加1S */
#endif
    }

}

typedef  void (*pFunction)(void);

/**
*@brief     STM32系统软复位函数
*@param     无
*@return    无
*/
void reboot(void)
{
    pFunction Jump_To_Application;
    uint32 JumpAddress;
    printf(" 系统重启中……\r\n");
    JumpAddress = *(vu32*)(0x00000004);
    Jump_To_Application = (pFunction) JumpAddress;
    Jump_To_Application();
}
#endif

