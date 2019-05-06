#ifndef __BSP_HT1381_H
#define __BSP_HT1381_H


#include "global.h"


/* HT1381ʱ��оƬCLK���� */
#define HT1381_CLK_GPIO_CLK     RCC_APB2Periph_GPIOC
#define HT1381_CLK_GPIO_PORT    GPIOC
#define HT1381_CLK_GPIO_PIN     GPIO_Pin_4

/* HT1381ʱ��оƬIO���� */
#define HT1381_IO_GPIO_CLK      RCC_APB2Periph_GPIOC
#define HT1381_IO_GPIO_PORT     GPIOC
#define HT1381_IO_GPIO_PIN      GPIO_Pin_5

/* HT1381ʱ��оƬRST���� */
#define HT1381_RST_GPIO_CLK     RCC_APB2Periph_GPIOB
#define HT1381_RST_GPIO_PORT    GPIOB
#define HT1381_RST_GPIO_PIN     GPIO_Pin_0


typedef struct sys_tm {   /* ȫ��ΪBCD�� */
    unsigned char year;     /* ��2000�꿪ʼ������ */
    unsigned char month;    /* 1�����·���[1,12] */
    unsigned char day;      /* 1���µ�����[1,31] */
    unsigned char hour;     /* Сʱ��[0,23] */
    unsigned char minite;   /* ������[0,59] */
    unsigned char second;   /* ����[0,59] */
    unsigned char week;     /* 1���ڵ�ĳ��[0,6](������=0) */
} SYS_TIME_TypeDef;


enum {
    DEFAULT_YEAR = 0x00,    /* BCD�룬Ĭ��2000�� */
    DEFAULT_MONTH = 0x01,   /* BCD�룬Ĭ��1�� */
    DEFAULT_DATE = 0x01,    /* BCD�룬Ĭ��1�� */
    DEFAULT_HOUR = 0x00,    /* BCD�룬Ĭ��0ʱ */
    DEFAULT_MIN = 0x00,     /* BCD�룬Ĭ��0�� */
    DEFAULT_SEC = 0x00,     /* BCD�룬Ĭ��0�� */
    DEFAULT_WEEK = 0x06,    /* BCD�룬2000��1��1���������� */

    MAX_YEAR = 0x38,        /* BCD�룬���2038�꣬��Ϊunixʱ���ʹ��32bit��ʱ��ʵ����HT1381�ܼ��㵽2099�� */
    MAX_MONTH = 0x12,       /* BCD�룬һ�����12���� */
    MAX_DATE = 0x31,        /* BCD�룬һ�������31�� */
    MAX_HOUR = 0x23,        /* BCD�룬Сʱ�����ֵ23 */
    MAX_MIN = 0x59,         /* BCD�룬���ӣ����ֵ59 */
    MAX_SEC = 0x59,         /* BCD�룬���ӣ����ֵ59 */
    MAX_WEEK = 0x06,        /* BCD�룬���ڣ�0��ʾ�����죬1~6��ʾ����һ�������� */
};


#define REG_SEC                 0
#define REG_MIN                 1
#define REG_HR                  2
#define REG_DATE                3
#define REG_MONTH               4
#define REG_WEEK                5
#define REG_YR                  6
#define REG_WP                  7


#define CMD_WRT                 0x80
#define CMD_READ                0x81
#define CMD_WRT_SEC             0x80
#define CMD_READ_SEC            0x81
#define CMD_WRT_MINUT           0x82
#define CMD_READ_MINUT          0x83
#define CMD_WRT_HR              0x84
#define CMD_READ_HR             0x85
#define CMD_WRT_DATE            0x86
#define CMD_READ_DATA           0x87
#define CMD_WRT_MON             0x88
#define CMD_READ_MON            0x89
#define CMD_WRT_DAY             0x8A
#define CMD_READ_DAY            0x8B
#define CMD_WRT_YR              0x8C
#define CMD_READ_YR             0x8D
#define CMD_WRT_WP              0x8E
#define CMD_READ_WP             0x8F

#define CMD_READ_H              0x85
#define CMD_WRT_HR              0x84



extern void ht1381_init(void);
extern void get_rtc_time(SYS_TIME_TypeDef *SetTime);
extern void set_rtc_time(SYS_TIME_TypeDef *SetTime);
extern void set_week(SYS_TIME_TypeDef *SetTime);
extern uint8_t bcd_hex(uint8_t num);
extern uint8_t hex_bcd(uint8_t num);


#endif /* __BSP_HT1381_H */
