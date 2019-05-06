/*
**************************************************************************************************
* @file         w5500_conf.c
* @author       WIZnet Software Team
* @version      V1.0
* @date         2015-02-14
* @brief        ����MCU����ֲW5500������Ҫ�޸ĵ��ļ�������W5500��MAC��IP��ַ
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


#define DEBUG_W5500_CONFIG TRUE             /* ���Կ��� */

W5500_CONFIG_S w5500_config;                /* ���ýṹ�� */

const W5500_CONFIG_S w5500_config_default = {
    {0x00, 0x08, 0xdc, 0x11, 0x11, 0x11},   /* mac��ַ��������W5500���������
                                                    ��ͬһ�ֳ���������ʹ�ò�ͬ
                                                    ��MAC��ַ*/
    {192, 168, 1, 6},                       /* local_ip */
    {LOCAL_PORT},                           /* local_port */
    {255, 255, 255, 0},                     /* subnet */
    {192, 168, 1, 1},                       /* gateway */
    {8, 8, 8, 8},                           /* �ȸ��DNS������ */
    //{223, 5, 5, 5},                           /* Alibaba��DNS������1 */
    //{223, 6, 6, 6},                           /* Alibaba��DNS������2 */

    {192, 168, 1, 8},                       /* remote_ip */
    {REMOTE_PORT},                          /* remote_port */
    {FW_VER_HIGH, FW_VER_LOW},              /* sw_ver */
    {IP_FROM_DHCP}                          /* Ĭ��ʹ��DHCP��ʽ��ȡIP */
};


uint8_t  dhcp_ok = 0;                       /* dhcp�ɹ���ȡIP */
uint32_t ms = 0;                            /* ������� */
uint32_t dhcp_time = 0;                     /* DHCP���м�����ʱ1���һ�� */
uint8_t ntptimer = 0;                       /* NPT����� */

/**
*@brief     ����W5500��IP��ַ
*@param     ��
*@return    ��
*/
void set_w5500_ip(void)
{
    uint8_t temp_ip[4] = {0, 0, 0, 0};
    uint8_t temp_subnet[4] = {0, 0, 0, 0};
    uint8_t temp_gateway[4] = {0, 0, 0, 0};

#ifndef W5500_CONFIG_FROM_DATABASE
    /* ���������ַ��6�ֽ�uint8_t����,�Լ����壬����Ψһ��ʶ�����豸�������ֵַ
            �õ�ֵַ��Ҫ��IEEE���룬����OUI�Ĺ涨��ǰ3���ֽ�Ϊ���̴��룬��������
            ��Ϊ��Ʒ�������Լ����������ַ��ע���һ���ֽڱ���Ϊż�� */
    memcpy(w5500_config.mac, w5500_config_default.mac, 6);
    /* ����IP�����뱾��IP����ͬһ�����������򱾻����޷��ҵ����� */
    memcpy(w5500_config.lip, w5500_config_default.lip, 4);
    /* �������������������� */
    memcpy(w5500_config.sub, w5500_config_default.sub, 4);
    /* ʹ�����ؿ���ʹͨ��ͻ�������ľ��ޣ�ͨ�����ؿ��Է��ʵ��������������Internet */
    memcpy(w5500_config.gw, w5500_config_default.gw, 4);
    /* DNS��������ַ */
    memcpy(w5500_config.dns, w5500_config_default.dns, 4);
    /* ͨѶԶ��IP��ַ */
    memcpy(w5500_config.rip, w5500_config_default.rip, 4);
    /* ͨѶ���ض˿� */
    w5500_config.lport = w5500_config_default.lport;
    /* ͨѶԶ�˶˿� */
    w5500_config.rport = w5500_config_default.rport;

    /* ����汾��Ϣ��������Ҫѡ�� */
    memcpy(w5500_config.sw_ver, w5500_config_default.sw_ver, 2);
#endif

#ifndef MAC_DEBUG
    /*
        ��SPI_FLASH��ȡ��ȷ��MAC��ַ
    */
    
#endif

    if(w5500_config.ip_from == IP_FROM_DEFINE) {
#if DEBUG_W5500_CONFIG
        LOG("W5500 IP from define");
#endif
    } else if(w5500_config.ip_from == IP_FROM_DHCP) {    /*ʹ��DHCP��ȡIP�����������DHCP�Ӻ���*/
        if(dhcp_ok == 1) {  /* DHCP�Ѿ��ɹ���ȡ��IP */
#if DEBUG_W5500_CONFIG
            LOG("W5500 IP from DHCP ok");
#endif
            /* ����DHCP��ȡ��������Ϣ�����ýṹ�� */
            memcpy(w5500_config.lip, DHCP_GET.lip, 4);
            memcpy(w5500_config.sub, DHCP_GET.sub, 4);
            memcpy(w5500_config.gw, DHCP_GET.gw, 4);
            memcpy(w5500_config.dns, DHCP_GET.gw, 4);   /* ʹ������IP��ΪDNS��������Ҳ������·������æ�������� */
            //memcpy(w5500_config.dns, DHCP_GET.dns, 4);/* ��ʱ��ʹ�û�ȡ����DNS */
        } else {
#if DEBUG_W5500_CONFIG
            LOG("W5500 DHCP fail\r\n");
#endif
        }
    }


    /* ��IP������Ϣд��W5500��Ӧ�Ĵ��� */
    setSUBR(w5500_config.sub);
    setGAR(w5500_config.gw);
    setSIPR(w5500_config.lip);

    /* ��������ʱ�䣬Ĭ��Ϊ2000(200ms) ÿһ��λ��ֵΪ100΢��,��ʼ��ʱֵ��
            Ϊ2000(0x07D0),����200���� */
    setRTR(0x07D0);

    /*�������Դ�����Ĭ��Ϊ8������ط��Ĵ��������趨ֵ,�������ʱ�ж�(���
            �Ķ˿��жϼĴ����е�Sn_IR ��ʱλ(TIMEOUT)�á�1��) */
    setRCR(0x08);

#if DEBUG_W5500_CONFIG
    /* �鿴��ӡ�Ƿ�ɹ�����IP�Ȳ��� */
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
*@brief     ����W5500��MAC��ַ
*@param     ��
*@return    ��
*/
void set_w5500_mac(void)
{
#ifndef W5500_CONFIG_FROM_DATABASE
    memcpy(w5500_config.mac, w5500_config_default.mac, 6);
#endif
    setSHAR(w5500_config.mac);  /* ����mac��ַ */
    memcpy(DHCP_GET.mac, w5500_config.mac, 6);
}

/**
*@brief     W5500��λ���ú���
*@param     ��
*@return    ��
*/
void reset_w5500(void)
{
    //W5500_RST_LOW();
    //delay_ms(10);
    //W5500_RST_HIGH();
    //delay_ms(900);
}


/**
*@brief     д��һ��8λ���ݵ�W5500
*@param     addrbsb: д�����ݵĵ�ַ
*@param   data��д���8λ����
*@return    ��
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
*@brief     ��W5500����һ��8λ����
*@param     addrbsb: д�����ݵĵ�ַ
*@param   data����д��ĵ�ַ����ȡ����8λ����
*@return    ��
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
*@brief     ��W5500д��len�ֽ�����
*@param     addrbsb: д�����ݵĵ�ַ
*@param   buf��д���ַ���
*@param   len���ַ�������
*@return    len�������ַ�������
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
*@brief     ��W5500����len�ֽ�����
*@param     addrbsb: ��ȡ���ݵĵ�ַ
*@param     buf����Ŷ�ȡ����
*@param     len���ַ�������
*@return    len�������ַ�������
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
*@brief     STM32��ʱ��2��ʼ��
*@param     ��
*@return    ��
*/
void timer2_init(void)
{
    //TIM2_Configuration();             /* TIM2 ��ʱ���� */
    //TIM2_NVIC_Configuration();        /* ��ʱ�����ж����ȼ� */
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  /* TIM2 ���¿�ʱ�ӣ���ʼ��ʱ */
}

/**
*@brief     dhcp�õ��Ķ�ʱ����ʼ��
*@param     ��
*@return    ��
*/
void dhcp_timer_init(void)
{
    //timer2_init();
}

/**
*@brief     ntp�õ��Ķ�ʱ����ʼ��
*@param     ��
*@return    ��
*/
void ntp_timer_init(void)
{
    //timer2_init();
}

/**
*@brief     ��ʱ��2�жϺ���
*@param     ��
*@return    ��
*/
void timer2_isr(void)
{
    ms++;

    if(ms >= 100) {
        ms = 0;
        dhcp_time++;        /* DHCP��ʱ��1S */
#ifndef __NTP_H__
        ntptimer++;         /* NTP����ʱ���1S */
#endif
    }

}

typedef  void (*pFunction)(void);

/**
*@brief     STM32ϵͳ��λ����
*@param     ��
*@return    ��
*/
void reboot(void)
{
    pFunction Jump_To_Application;
    uint32 JumpAddress;
    printf(" ϵͳ�����С���\r\n");
    JumpAddress = *(vu32*)(0x00000004);
    Jump_To_Application = (pFunction) JumpAddress;
    Jump_To_Application();
}
#endif

