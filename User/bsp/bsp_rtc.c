/**
  ******************************************************************************
  * @file    bsp_rtc.c
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   stm32 RTC 驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火 F103-霸道 STM32 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */



#include "./bsp_rtc.h"

#include "./../sys/sys_command_line.h"



/*星期，生肖用文字ASCII码*/
char const *WEEK_STR[] = {"日", "一", "二", "三", "四", "五", "六"};
char const *zodiac_sign[] = {"猪", "鼠", "牛", "虎", "兔", "龙", "蛇", "马", "羊", "猴", "鸡", "狗"};

/*英文，星期，生肖用文字ASCII码*/
char const *en_WEEK_STR[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char const *en_zodiac_sign[] = {"Pig", "Rat", "Ox", "Tiger", "Rabbit", "Dragon", "Snake", "Horse", "Goat", "Monkey", "Rooster", "Dog"};


/*
 * 函数名：NVIC_Configuration
 * 描述  ：配置RTC秒中断的主中断优先级为1，次优先级为0
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void RTC_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Configure one bit for preemption priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* Enable the RTC Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/*
 * 函数名：RTC_CheckAndConfig
 * 描述  ：检查并配置RTC
 * 输入  ：用于读取RTC时间的结构体指针
 * 输出  ：无
 * 调用  ：外部调用
 */
void RTC_CheckAndConfig(struct rtc_time *tm)
{
    /*在启动时检查备份寄存器BKP_DR1，如果内容不是0xA5A5,
      则需重新配置时间并询问用户调整时间*/
    if (BKP_ReadBackupRegister(RTC_BKP_DRX) != RTC_BKP_DATA) {
        PRINTF("\r\n\r\n RTC not yet configured....");
        PRINTF("\r\n\r\n RTC configured....");

        /* 使用tm的时间配置RTC寄存器 */
        time_adjust(tm);

        /*向BKP_DR1寄存器写入标志，说明RTC已在运行*/
        BKP_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
    } else {

        /* 使能 PWR 和 Backup 时钟 */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

        /* 允许访问 Backup 区域 */
        PWR_BackupAccessCmd(ENABLE);

        /*LSE启动无需设置新时钟*/

#ifdef RTC_CLOCK_SOURCE_LSI
        /* 使能 LSI */
        RCC_LSICmd(ENABLE);

        /* 等待 LSI 准备好 */
        while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
        {}

#endif

        if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET) {
            /*检查是否掉电重启*/
            PRINTF("\r\n\r\n Power On Reset occurred....");
        } else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET) {
            /*检查是否Reset复位*/
            PRINTF("\r\n\r\n External Reset occurred....");
        }

        PRINTF("\r\n No need to configure RTC....");

        /*等待寄存器同步*/
        RTC_WaitForSynchro();

        /*允许RTC秒中断*/
        RTC_ITConfig(RTC_IT_SEC, ENABLE);

        /*等待上次RTC寄存器写操作完成*/
        RTC_WaitForLastTask();
    }

    /* 定义了时钟输出宏，则配置校正时钟输出到PC13 */
#ifdef RTCClockOutput_Enable
    /* 使能 PWR 和 Backup 时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* 允许访问 Backup 区域 */
    PWR_BackupAccessCmd(ENABLE);

    /* 禁止 Tamper 引脚 */
    /* 要输出 RTCCLK/64 到 Tamper 引脚,  tamper 功能必须禁止 */
    BKP_TamperPinCmd(DISABLE);

    /* 使能 RTC 时钟输出到 Tamper 引脚 */
    BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
#endif

    /* 清除复位标志 flags */
    RCC_ClearFlag();

}



/*
 * 函数名：RTC_Configuration
 * 描述  ：配置RTC
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void RTC_Configuration(void)
{
    /* 使能 PWR 和 Backup 时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* 允许访问 Backup 区域 */
    PWR_BackupAccessCmd(ENABLE);

    /* 复位 Backup 区域 */
    BKP_DeInit();

//使用外部时钟还是内部时钟（在bsp_rtc.h文件定义）
//使用外部时钟时，在有些情况下晶振不起振
//批量产品的时候，很容易出现外部晶振不起振的情况，不太可靠
#ifdef  RTC_CLOCK_SOURCE_LSE
    /* 使能 LSE */
    RCC_LSEConfig(RCC_LSE_ON);

    /* 等待 LSE 准备好 */
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {}

    /* 选择 LSE 作为 RTC 时钟源 */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    /* 使能 RTC 时钟 */
    RCC_RTCCLKCmd(ENABLE);

    /* 等待 RTC 寄存器 同步
     * 因为RTC时钟是低速的，内环时钟是高速的，所以要同步
     */
    RTC_WaitForSynchro();

    /* 确保上一次 RTC 的操作完成 */
    RTC_WaitForLastTask();

    /* 使能 RTC 秒中断 */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);

    /* 确保上一次 RTC 的操作完成 */
    RTC_WaitForLastTask();

    /* 设置 RTC 分频: 使 RTC 周期为1s  */
    /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) = 1HZ */
    RTC_SetPrescaler(32767);

    /* 确保上一次 RTC 的操作完成 */
    RTC_WaitForLastTask();

#else

    /* 使能 LSI */
    RCC_LSICmd(ENABLE);

    /* 等待 LSI 准备好 */
    while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
    {}

    /* 选择 LSI 作为 RTC 时钟源 */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

    /* 使能 RTC 时钟 */
    RCC_RTCCLKCmd(ENABLE);

    /* 等待 RTC 寄存器 同步
     * 因为RTC时钟是低速的，内环时钟是高速的，所以要同步
     */
    RTC_WaitForSynchro();

    /* 确保上一次 RTC 的操作完成 */
    RTC_WaitForLastTask();

    /* 使能 RTC 秒中断 */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);

    /* 确保上一次 RTC 的操作完成 */
    RTC_WaitForLastTask();

    /* 设置 RTC 分频: 使 RTC 周期为1s ,LSI约为40KHz */
    /* RTC period = RTCCLK/RTC_PR = (40 KHz)/(40000-1+1) = 1HZ */
    RTC_SetPrescaler(40000 - 1);

    /* 确保上一次 RTC 的操作完成 */
    RTC_WaitForLastTask();
#endif

}



/*
 * 函数名：Time_Regulate_Get
 * 描述  ：保存用户使用串口设置的时间，
 *         以便后面转化成时间戳存储到RTC 计数寄存器中。
 * 输入  ：用于读取RTC时间的结构体指针
 * 注意  ：在串口调试助手输入时，输入完数字要加回车
 */
void Time_Regulate_Get(struct rtc_time *tm)
{
    uint32_t temp_num = 0;
    uint8_t day_max = 0 ;

    PRINTF("\r\n=========================设置时间==================");

    do {
        PRINTF("\r\n  请输入年份(Please Set Years),范围[1970~2038]，输入字符后请加回车:");
        scanf("%d", &temp_num);

        if (temp_num < 1970 || temp_num > 2038) {
            PRINTF("\r\n 您输入的数字是:%d，不符合要求", temp_num);

        } else {
            PRINTF("\n\r  年份被设置为: %d\n\r", temp_num);

            tm->tm_year = temp_num;
            break;
        }
    } while (1);


    do {
        PRINTF("\r\n  请输入月份(Please Set Months):范围[1~12]，输入字符后请加回车:");
        scanf("%d", &temp_num);

        if (temp_num < 1 || temp_num > 12) {
            PRINTF("\r\n 您输入的数字是:%d，不符合要求", temp_num);

        } else {
            PRINTF("\n\r  月份被设置为: %d\n\r", temp_num);

            tm->tm_mon = temp_num;
            break;
        }
    } while (1);

    /*根据月份计算最大日期*/
    switch (tm->tm_mon) {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            day_max = 31;
            break;

        case 4:
        case 6:
        case 9:
        case 11:
            day_max = 30;
            break;

        case 2:

            /*计算闰年*/
            if ((tm->tm_year % 4 == 0) &&
                    ((tm->tm_year % 100 != 0) || (tm->tm_year % 400 == 0)) &&
                    (tm->tm_mon > 2)) {
                day_max = 29;
            } else {
                day_max = 28;
            }

            break;
    }

    do {
        PRINTF("\r\n  请输入日期(Please Set Months),范围[1~%d]，输入字符后请加回车:", day_max);
        scanf("%d", &temp_num);

        if (temp_num < 1 || temp_num > day_max) {
            PRINTF("\r\n 您输入的数字是:%d，不符合要求", temp_num);
        } else {
            PRINTF("\n\r  日期被设置为: %d\n\r", temp_num);

            tm->tm_mday = temp_num;
            break;
        }
    } while (1);

    do {
        PRINTF("\r\n  请输入时钟(Please Set Hours),范围[0~23]，输入字符后请加回车:");
        scanf("%d", &temp_num);

        if (temp_num > 23) {
            PRINTF("\r\n 您输入的数字是:%d，不符合要求", temp_num);
        } else {
            PRINTF("\n\r  时钟被设置为: %d\n\r", temp_num);

            tm->tm_hour = temp_num;
            break;
        }
    } while (1);

    do {
        PRINTF("\r\n  请输入分钟(Please Set Minutes),范围[0~59]，输入字符后请加回车:");
        scanf("%d", &temp_num);

        if (temp_num > 59) {
            PRINTF("\r\n 您输入的数字是:%d，不符合要求", temp_num);
        } else {
            PRINTF("\n\r  分钟被设置为: %d\n\r", temp_num);

            tm->tm_min = temp_num;
            break;
        }
    } while (1);

    do {
        PRINTF("\r\n  请输入秒钟(Please Set Seconds),范围[0~59]，输入字符后请加回车:");
        scanf("%d", &temp_num);

        if (temp_num > 59) {
            PRINTF("\r\n 您输入的数字是:%d，不符合要求", temp_num);
        } else {
            PRINTF("\n\r  秒钟被设置为: %d\n\r", temp_num);

            tm->tm_sec = temp_num;
            break;
        }
    } while (1);

}


/*
 * 函数名：Time_Adjust
 * 描述  ：时间调节
 * 输入  ：用于读取RTC时间的结构体指针（北京时间）
 * 输出  ：无
 * 调用  ：外部调用
 */
void time_adjust(struct rtc_time *tm)
{

    /* RTC 配置 */
    RTC_Configuration();

    /* 等待确保上一次操作完成 */
    RTC_WaitForLastTask();

    /* 计算星期 */
    GregorianDay(tm);

    /* 由日期计算时间戳并写入到RTC计数寄存器 */
    RTC_SetCounter(mktimev(tm) - TIME_ZOOM);

    /* 等待确保上一次操作完成 */
    RTC_WaitForLastTask();
}

/*
 * 函数名：calendar_test
 * 描述  ：测试公历、农历、二十四节气
 */
static void calendar_test(struct rtc_time *tm)
{
    uint8_t str[200];

    GetChinaCalendar((u16)tm->tm_year, (u8)tm->tm_mon, (u8)tm->tm_mday, str);
    PRINTF("Solar calendar: %0.2d%0.2d,%0.2d,%0.2d\r\n", str[0], str[1], str[2], str[3]);

    GetChinaCalendarStr((u16)tm->tm_year, (u8)tm->tm_mon, (u8)tm->tm_mday, str);
    PRINTF("lunar calendar: %s\r\n", str);

    if (GetJieQiStr((u16)tm->tm_year, (u8)tm->tm_mon, (u8)tm->tm_mday, str))
        PRINTF("lunar calendar: %s\r\n", str);
}

/*
 * 函数名：time_update
 * 描述  ：读取更新时间值
 * 输入  ：-TimeVar RTC计数值，单位为 s
 * 输出  ：无
 * 调用  ：内部调用
 */
void time_update(struct rtc_time *tm)
{
    uint32_t BJ_TimeVar;
    uint32_t TimeVar;

    TimeVar = RTC_GetCounter();

    BJ_TimeVar = TimeVar + TIME_ZOOM;   /* 把标准时间转换为北京时间 */

    to_tm(BJ_TimeVar, tm);              /* 把定时器的值转换为北京时间 */

    /* 打印时间戳 */
    PRINTF("UNIX value = %d \r\n", TimeVar);

    /* 打印公历时间 */
    PRINTF("Time: %d-%d-%d %0.2d:%0.2d:%0.2d (Zodiac Sign: %s, Weekday: %s) \r\n",    tm->tm_year, tm->tm_mon, tm->tm_mday, 
        tm->tm_hour, tm->tm_min, tm->tm_sec, 
        en_zodiac_sign[(tm->tm_year - 3) % 12], en_WEEK_STR[tm->tm_wday]);

}


/************************END OF FILE***************************************/
