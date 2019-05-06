#ifndef __BSP_HT1381_H
#define __BSP_HT1381_H


#include "global.h"


/* HT1381时钟芯片CLK引脚 */
#define HT1381_CLK_GPIO_CLK     RCC_APB2Periph_GPIOC
#define HT1381_CLK_GPIO_PORT    GPIOC
#define HT1381_CLK_GPIO_PIN     GPIO_Pin_4

/* HT1381时钟芯片IO引脚 */
#define HT1381_IO_GPIO_CLK      RCC_APB2Periph_GPIOC
#define HT1381_IO_GPIO_PORT     GPIOC
#define HT1381_IO_GPIO_PIN      GPIO_Pin_5

/* HT1381时钟芯片RST引脚 */
#define HT1381_RST_GPIO_CLK     RCC_APB2Periph_GPIOB
#define HT1381_RST_GPIO_PORT    GPIOB
#define HT1381_RST_GPIO_PIN     GPIO_Pin_0


typedef struct sys_tm {   /* 全部为BCD码 */
    unsigned char year;     /* 从2000年开始的年数 */
    unsigned char month;    /* 1年中月份数[1,12] */
    unsigned char day;      /* 1个月的天数[1,31] */
    unsigned char hour;     /* 小时数[0,23] */
    unsigned char minite;   /* 分钟数[0,59] */
    unsigned char second;   /* 秒数[0,59] */
    unsigned char week;     /* 1星期的某天[0,6](星期天=0) */
} SYS_TIME_TypeDef;


enum {
    DEFAULT_YEAR = 0x00,    /* BCD码，默认2000年 */
    DEFAULT_MONTH = 0x01,   /* BCD码，默认1月 */
    DEFAULT_DATE = 0x01,    /* BCD码，默认1号 */
    DEFAULT_HOUR = 0x00,    /* BCD码，默认0时 */
    DEFAULT_MIN = 0x00,     /* BCD码，默认0分 */
    DEFAULT_SEC = 0x00,     /* BCD码，默认0秒 */
    DEFAULT_WEEK = 0x06,    /* BCD码，2000年1月1号是星期六 */

    MAX_YEAR = 0x38,        /* BCD码，最大到2038年，因为unix时间戳使用32bit计时，实际上HT1381能计算到2099年 */
    MAX_MONTH = 0x12,       /* BCD码，一年最多12个月 */
    MAX_DATE = 0x31,        /* BCD码，一个月最多31天 */
    MAX_HOUR = 0x23,        /* BCD码，小时，最大值23 */
    MAX_MIN = 0x59,         /* BCD码，分钟，最大值59 */
    MAX_SEC = 0x59,         /* BCD码，秒钟，最大值59 */
    MAX_WEEK = 0x06,        /* BCD码，星期，0表示星期天，1~6表示星期一到星期六 */
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
