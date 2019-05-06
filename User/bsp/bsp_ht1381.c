

/* Includes ------------------------------------------------------------------*/
#include "bsp_ht1381.h"
#include "./../sys/sys_command_line.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


#define IsBCD(dat) ((((dat) >> 4) <= 0x09) && (((dat)&0x0F) <= 0x09)) ? 1 : 0


const uint8_t week_of_first_day[] = {
    /* 00~99年第一天为周几,0为周日 */
    6, 1, 2, 3, 4, 6, 0, 1, 2, 4,   /* 00~09年 */
    5, 6, 0, 2, 3, 4, 5, 0, 1, 2,   /* 10~19年 */
    3, 5, 6, 0, 1, 3, 4, 5, 6, 1,   /* 20~29年 */
    2, 3, 4, 6, 0, 1, 2, 4, 5, 6,   /* 30~39年 */
    0, 2, 3, 4, 5, 0, 1, 2, 3, 5,   /* 40~49年 */
    6, 0, 1, 3, 4, 5, 6, 1, 2, 3,   /* 50~59年 */
    4, 6, 0, 1, 2, 4, 5, 6, 0, 2,   /* 60~69年 */
    3, 4, 5, 0, 1, 2, 3, 5, 6, 0,   /* 70~79年 */
    1, 3, 4, 5, 6, 1, 2, 3, 4, 6,   /* 80~89年 */
    0, 1, 2, 4, 5, 6, 0, 2, 3, 4    /* 90~99年 */
};


const uint8_t days_of_a_month[12] = {
    /* 一年12个月，每个月对应天数 */
    0x31, 0x28, 0x31, 0x30, 0x31, 0x30,
    0x31, 0x31, 0x30, 0x31, 0x30, 0x31
};


const u8 LeapYear[] = {
    /* 闰年 */
    0x00, 0x04, 0x08, 0x12, 0x16, 0x20, 0x24, 0x28, 0x32,
    0x36, 0x40, 0x44, 0x48, 0x52, 0x56, 0x60, 0x64, 0x68,
    0x72, 0x76, 0x80, 0x84, 0x88, 0x92, 0x96
};



uint8_t ht1381_read_time(uint8_t ucRegAdd);
void ht1381_write_time(uint8_t ucRegAdd, uint8_t ucData);


static void ht1381_delay(void)
{
    uint8_t t = 200;

    while(t--);
}


uint8_t bcd_hex(uint8_t num)
{
    if(IsBCD(num)) {
        return (uint8_t)((num >> 4) * 10 + (num & 0x0F));
    } else {
        return 0;
    }
}


uint8_t hex_bcd(uint8_t num)
{
    if(99 >= num) {
        return (uint8_t)(num + 6 * (num / 10));
    } else {
        return 0;
    }
}


/**
  * @brief  判断是否是闰年
  * @param  无
  * @retval 无
  */
uint32_t IsLeapYear(uint8_t ucYear)
{
    uint32_t i;

    for(i = 0; i < 25; i++) {  // 25跟建的表有关
        if(ucYear == LeapYear[i])
            return 1;
    }

    return 0;
}


/**
  * @brief  验证日期的合法性
  * @param  无
  * @retval 无
  */
uint8_t CheckDay(uint8_t ucYear, uint8_t ucMonth, uint8_t ucDay)
{
    if((ucYear > MAX_YEAR) || (0 == ucMonth) || (ucMonth > MAX_MONTH) || (0 == ucDay) ||
            (ucDay > MAX_DATE))
        return 0;

    if((ucDay > 0) && (ucDay <= (days_of_a_month[bcd_hex(ucMonth) - 1]))) {
        return 1;
    } else if(((0x29 == ucDay) &&
               (0x02 == ucMonth)) // 2月29号为闰年则该日期正常
              && IsLeapYear(ucYear)) {
        return 1;
    } else {
        return 0;
    }
}


/**
  * @brief  验证时间的合法性
  * @param  无
  * @retval 无
  */
uint8_t CheckTime(uint8_t ucHour, uint8_t ucMinute, uint8_t ucSecond)
{
    if((ucHour <= MAX_HOUR) && (ucMinute <= MAX_MIN) && (ucSecond <= MAX_SEC)) {
        return 1;
    }

    return 0;
}


/**
  * @brief  HT1381的IO口设置为输入
  * @param  无
  * @retval 无
  */
static void ht1381_io_in(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = HT1381_IO_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HT1381_IO_GPIO_PORT, &GPIO_InitStructure);
}


/**
  * @brief  HT1381的IO口设置为输出
  * @param  无
  * @retval 无
  */
static void ht1381_io_out(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = HT1381_IO_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HT1381_IO_GPIO_PORT, &GPIO_InitStructure);
}


/**
  * @brief  HT1381芯片初始化
  * @param  无
  * @retval 无
  */
void ht1381_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    uint8_t temp;

    RCC_APB2PeriphClockCmd(HT1381_CLK_GPIO_CLK
                           | HT1381_IO_GPIO_CLK
                           | HT1381_RST_GPIO_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = HT1381_CLK_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HT1381_CLK_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = HT1381_IO_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HT1381_IO_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = HT1381_RST_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HT1381_RST_GPIO_PORT, &GPIO_InitStructure);


    //写允许晶振启动,秒钟寄存器最高位清零
    temp = ht1381_read_time(REG_SEC);

    if(0 != (temp & 0x80)) {  //已经允许,不再重写
        ht1381_write_time(REG_SEC, temp & 0x7F);
    }

    // WriteTime(REG_SEC, ReadTime(REG_SEC) & 0x7F);
    temp = ht1381_read_time(REG_HR);

    if(0 != (temp & 0x80)) {  // 0 表示使用24小时制
        ht1381_write_time(REG_HR, temp & 0x7F);
    }
}


/**
  * @brief  起始信号
  * @param  无
  * @retval 无
  */
static void ht1381_start(void)
{
    GPIO_ResetBits(HT1381_RST_GPIO_PORT, HT1381_RST_GPIO_PIN);
    GPIO_ResetBits(HT1381_CLK_GPIO_PORT, HT1381_CLK_GPIO_PIN);
    GPIO_ResetBits(HT1381_IO_GPIO_PORT, HT1381_IO_GPIO_PIN);
    GPIO_SetBits(HT1381_RST_GPIO_PORT, HT1381_RST_GPIO_PIN);
    ht1381_delay();
}


/**
  * @brief  结束信号
  * @param  无
  * @retval 无
  */
static void ht1381_stop(void)
{
    GPIO_ResetBits(HT1381_CLK_GPIO_PORT, HT1381_CLK_GPIO_PIN);
    GPIO_ResetBits(HT1381_RST_GPIO_PORT, HT1381_RST_GPIO_PIN);
    ht1381_delay();
}


/**
  * @brief  发送一个字节
  * @param  无
  * @retval 无
  */
static void ht1381_send_byte(uint8_t ucData)
{
    volatile uint8_t i;

    for(i = 8; i != 0; i--) {
        GPIO_ResetBits(HT1381_CLK_GPIO_PORT, HT1381_CLK_GPIO_PIN);

        if(ucData & 0x01)
            GPIO_SetBits(HT1381_IO_GPIO_PORT, HT1381_IO_GPIO_PIN);
        else
            GPIO_ResetBits(HT1381_IO_GPIO_PORT, HT1381_IO_GPIO_PIN);

        ucData >>= 1;
        GPIO_SetBits(HT1381_CLK_GPIO_PORT, HT1381_CLK_GPIO_PIN);
        ht1381_delay();
    }

    GPIO_ResetBits(HT1381_CLK_GPIO_PORT, HT1381_CLK_GPIO_PIN);
}


/**
  * @brief  接收一个字节
  * @param  无
  * @retval 无
  */
static uint8_t ht1381_reveive_byte(void)
{
    uint8_t i;
    uint8_t ucDataRecv = 0;

    ht1381_io_in();
    ht1381_delay();

    for(i = 8; i > 0; i--) {
        ucDataRecv >>= 1;

        if(1 == GPIO_ReadInputDataBit(HT1381_IO_GPIO_PORT, HT1381_IO_GPIO_PIN)) {
            ucDataRecv |= 0x80;
        }

        GPIO_SetBits(HT1381_CLK_GPIO_PORT, HT1381_CLK_GPIO_PIN);
        ht1381_delay();
        GPIO_ResetBits(HT1381_CLK_GPIO_PORT, HT1381_CLK_GPIO_PIN);
    }

    ht1381_io_out();
    ht1381_delay();
    return (ucDataRecv);
}


/**
  * @brief  设置HT1381寄存器
  * @param  无
  * @retval 无
  */
static void ht1381_write_reg(uint8_t ucRegAdd, uint8_t ucData)
{
    ht1381_start();
    ht1381_send_byte(ucRegAdd);
    ht1381_send_byte(ucData);
    ht1381_stop();
}


/**
  * @brief  从HT1381读取时间，读取到的都是BCD码
  * @param  无
  * @retval 无
  */
static uint8_t ht1381_read_time(uint8_t ucRegAdd)
{
    uint8_t ucData;

    ucRegAdd <<= 1;
    ucRegAdd &= 0x0E;
    ucRegAdd += 0x81;
    ht1381_start();
    ht1381_send_byte(ucRegAdd); //需要读取地址
    ucData = ht1381_reveive_byte();
    ht1381_stop();
    return ucData;
}


/**
  * @brief  设置HT1381的时间，BCD码
  * @param  无
  * @retval 无
  */
static void ht1381_write_time(uint8_t ucRegAdd, uint8_t ucData)
{
    //去除写保护
    ht1381_write_reg(CMD_WRT_WP, 0x00);
    //写数据
    ucRegAdd <<= 1;
    ucRegAdd &= 0x0E;
    ucRegAdd |= 0x80;
    ht1381_write_reg(ucRegAdd, ucData);
    //开写保护
    ht1381_write_reg(CMD_WRT_WP, 0x80);
}


/**
  * @brief  从HT1381读取时间，读取到的都是BCD码
  * @param  无
  * @retval 无
  */
void get_rtc_time(SYS_TIME_TypeDef *SetTime)
{
    SetTime->year = ht1381_read_time(REG_YR);
    SetTime->month = ht1381_read_time(REG_MONTH);
    SetTime->day = ht1381_read_time(REG_DATE);
    SetTime->hour = ht1381_read_time(REG_HR);
    SetTime->minite = ht1381_read_time(REG_MIN);
    SetTime->second = ht1381_read_time(REG_SEC);
    SetTime->week = ht1381_read_time(REG_WEEK);
}


/**
  * @brief  设置HT1381的时间，BCD码
  * @param  无
  * @retval 无
  */
void set_rtc_time(SYS_TIME_TypeDef *SetTime)
{
    ht1381_write_time(REG_YR, SetTime->year);
    ht1381_write_time(REG_MONTH, SetTime->month);
    ht1381_write_time(REG_DATE, SetTime->day);
    ht1381_write_time(REG_HR, SetTime->hour);
    ht1381_write_time(REG_MIN, SetTime->minite);
    ht1381_write_time(REG_SEC, SetTime->second);
    ht1381_write_time(REG_WEEK, SetTime->week);
}


void set_week(SYS_TIME_TypeDef *SetTime)
{
    uint16_t uiDayNum = 0;

    if(!(CheckDay(SetTime->year, SetTime->month,
                  SetTime->day))) { //时间有效性判断
        return;
    }

    switch(SetTime->month) {  // BCD格式
        case 0x12:
            uiDayNum += 30;

        case 0x11:
            uiDayNum += 31;

        case 0x10:
            uiDayNum += 30;

        case 0x09:
            uiDayNum += 31;

        case 0x08:
            uiDayNum += 31;

        case 0x07:
            uiDayNum += 30;

        case 0x06:
            uiDayNum += 31;

        case 0x05:
            uiDayNum += 30;

        case 0x04:
            uiDayNum += 31;

        case 0x03:
            uiDayNum += (IsLeapYear(SetTime->year)) ? 29 : 28; //闰年2月多1天

        case 0x02:
            uiDayNum += 31;

        case 0x01:
            uiDayNum += bcd_hex(SetTime->day);
            break;

        default:
            break;
    }

    SetTime->day =
        (week_of_first_day[bcd_hex(SetTime->year)] + (uiDayNum - 1)) % 7;
}


/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

