

#ifndef _W5500_CONF_H_
#define _W5500_CONF_H_


#include <stdint.h>
#include <stdio.h>
#include "stm32f10x.h"


#define __GNUC__


/* -------------------------------------------------------- ����IO�ײ�---��ʼ */

/**
*@brief     ����W5500��Ƭѡ�˿�SCSnΪ��
*@param     ��
*@return    ��
*/
#define IINCHIP_CS_OFF()            do {                            \
                                        spi_cs_ctrl_w5500(DISABLE); \
                                    }while(0)
/**
*@brief     ����W5500��Ƭѡ�˿�SCSnΪ��
*@param     ��
*@return    ��
*/
#define IINCHIP_CS_ON()             do {                            \
                                        spi_cs_ctrl_w5500(ENABLE);  \
                                    }while(0)

/**
*@brief     W5500 SPI��дһ���ֽ�����
*@param     dat��д���һ���ֽ�����
*@return    ����һ���ֽ�����
*/
#define IINCHIP_SPI_SEND_BYTE(dat)  spi_send_byte_w5500(dat)

/**
*@brief     W5500 IO��ʼ��������SPI��RESET���š�INT����
*@param     ��
*@return    ��
*/
#define W5500_GPIO_CONFIG()         gpio_config_w5500()


#define W5500_RST_LOW()             do {                            \
                                        rst_ctrl_w5500(DISABLE);    \
                                    }while(0)

#define W5500_RST_HIGH()            do {                            \
                                        rst_ctrl_w5500(ENABLE);     \
                                    }while(0)

/* -------------------------------------------------------- ����IO�ײ�---���� */


#define MAX_BUF_SIZE        1460            /* ����ÿ�����ݰ��Ĵ�С */
#define KEEP_ALIVE_TIME     30              // 30sec
#define TX_RX_MAX_BUF_SIZE  2048


#pragma pack(1)
/* �˽ṹ�嶨����W5500�ɹ����õ���Ҫ���� */
typedef struct {
#define FW_VER_HIGH     1
#define FW_VER_LOW      0
#define IP_FROM_DEFINE  0       /* ʹ�ó�ʼ���õ�IP */
#define IP_FROM_DHCP    1       /* ʹ��DHCP��ȡIP */
#define LOCAL_PORT      27106   /* ���ض˿� */
#define REMOTE_PORT     27109   /* Զ�̶˿ڣ�ƽ̨�Ķ˿� */

    uint8_t  mac[6];            /* MAC��ַ */
    uint8_t  lip[4];            /* local IP����IP��ַ */
    uint16_t lport;             /* local port���ض˿� */
    uint8_t  sub[4];            /* �������� */
    uint8_t  gw[4];             /* ���� */
    uint8_t  dns[4];            /* DNS��������ַ */
    uint8_t  rip[4];            /* remote IPԶ��IP��ַ */
    uint16_t rport;             /* remote portԶ�̶˿� */
    uint8_t  sw_ver[2];         /* ����汾�� */
    uint8_t  ip_from;           /* IP��DHCP��ȡ������ʹ�����õľ�̬IP */
} W5500_CONFIG_S;
#pragma pack()

extern W5500_CONFIG_S w5500_config;
extern const W5500_CONFIG_S w5500_config_default;

extern uint8_t dhcp_ok;           /* DHCP��ȡ�ɹ� */
extern uint32_t dhcp_time;        /* DHCP���м��� */
extern uint8_t ntptimer;          /* NPT����� */

/* MCU������غ��� */
void timer2_init(void);                     /* STM32��ʱ��2��ʼ�� */
void timer2_isr(void);                      /* ��ʱ���ж�ִ�еĺ��� */
void reboot(void);                          /* STM32��λ */

/* W5500SPI��غ��� */
void IINCHIP_WRITE(uint32_t addrbsb, uint8_t data);     /* д��һ��8λ���ݵ�W5500 */
uint8_t IINCHIP_READ(uint32_t addrbsb);                     /* ��W5500����һ��8λ���� */
uint16_t wiz_write_buf(uint32_t addrbsb, uint8_t* buf, uint16_t len);   /* ��W5500д��len�ֽ����� */
uint16_t wiz_read_buf(uint32_t addrbsb, uint8_t* buf, uint16_t len);    /* ��W5500����len�ֽ����� */

/* W5500����������غ��� */
void reset_w5500(void);                     /* Ӳ��λW5500 */
void set_w5500_mac(void);                   /* ����W5500��MAC��ַ */
void set_w5500_ip(void);                    /* ����W5500��IP��ַ */

/*��Ҫ�ö�ʱ�ĵ�Ӧ�ú���*/
void dhcp_timer_init(void);                 /* dhcp�õ��Ķ�ʱ����ʼ�� */
void ntp_timer_init(void);                  /* npt�õ��Ķ�ʱ����ʼ�� */


#endif /* _W5500_CONF_H_ */

