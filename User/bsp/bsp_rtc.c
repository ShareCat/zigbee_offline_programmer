/**
  ******************************************************************************
  * @file    bsp_rtc.c
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   stm32 RTC ����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:���� F103-�Ե� STM32 ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */



#include "./bsp_rtc.h"

#include "./../sys/sys_command_line.h"



/*���ڣ���Ф������ASCII��*/
char const *WEEK_STR[] = {"��", "һ", "��", "��", "��", "��", "��"};
char const *zodiac_sign[] = {"��", "��", "ţ", "��", "��", "��", "��", "��", "��", "��", "��", "��"};

/*Ӣ�ģ����ڣ���Ф������ASCII��*/
char const *en_WEEK_STR[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char const *en_zodiac_sign[] = {"Pig", "Rat", "Ox", "Tiger", "Rabbit", "Dragon", "Snake", "Horse", "Goat", "Monkey", "Rooster", "Dog"};


/*
 * ��������NVIC_Configuration
 * ����  ������RTC���жϵ����ж����ȼ�Ϊ1�������ȼ�Ϊ0
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
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
 * ��������RTC_CheckAndConfig
 * ����  ����鲢����RTC
 * ����  �����ڶ�ȡRTCʱ��Ľṹ��ָ��
 * ���  ����
 * ����  ���ⲿ����
 */
void RTC_CheckAndConfig(struct rtc_time *tm)
{
    /*������ʱ��鱸�ݼĴ���BKP_DR1��������ݲ���0xA5A5,
      ������������ʱ�䲢ѯ���û�����ʱ��*/
    if (BKP_ReadBackupRegister(RTC_BKP_DRX) != RTC_BKP_DATA) {
        PRINTF("\r\n\r\n RTC not yet configured....");
        PRINTF("\r\n\r\n RTC configured....");

        /* ʹ��tm��ʱ������RTC�Ĵ��� */
        time_adjust(tm);

        /*��BKP_DR1�Ĵ���д���־��˵��RTC��������*/
        BKP_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
    } else {

        /* ʹ�� PWR �� Backup ʱ�� */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

        /* ������� Backup ���� */
        PWR_BackupAccessCmd(ENABLE);

        /*LSE��������������ʱ��*/

#ifdef RTC_CLOCK_SOURCE_LSI
        /* ʹ�� LSI */
        RCC_LSICmd(ENABLE);

        /* �ȴ� LSI ׼���� */
        while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
        {}

#endif

        if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET) {
            /*����Ƿ��������*/
            PRINTF("\r\n\r\n Power On Reset occurred....");
        } else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET) {
            /*����Ƿ�Reset��λ*/
            PRINTF("\r\n\r\n External Reset occurred....");
        }

        PRINTF("\r\n No need to configure RTC....");

        /*�ȴ��Ĵ���ͬ��*/
        RTC_WaitForSynchro();

        /*����RTC���ж�*/
        RTC_ITConfig(RTC_IT_SEC, ENABLE);

        /*�ȴ��ϴ�RTC�Ĵ���д�������*/
        RTC_WaitForLastTask();
    }

    /* ������ʱ������꣬������У��ʱ�������PC13 */
#ifdef RTCClockOutput_Enable
    /* ʹ�� PWR �� Backup ʱ�� */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* ������� Backup ���� */
    PWR_BackupAccessCmd(ENABLE);

    /* ��ֹ Tamper ���� */
    /* Ҫ��� RTCCLK/64 �� Tamper ����,  tamper ���ܱ����ֹ */
    BKP_TamperPinCmd(DISABLE);

    /* ʹ�� RTC ʱ������� Tamper ���� */
    BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
#endif

    /* �����λ��־ flags */
    RCC_ClearFlag();

}



/*
 * ��������RTC_Configuration
 * ����  ������RTC
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void RTC_Configuration(void)
{
    /* ʹ�� PWR �� Backup ʱ�� */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* ������� Backup ���� */
    PWR_BackupAccessCmd(ENABLE);

    /* ��λ Backup ���� */
    BKP_DeInit();

//ʹ���ⲿʱ�ӻ����ڲ�ʱ�ӣ���bsp_rtc.h�ļ����壩
//ʹ���ⲿʱ��ʱ������Щ����¾�������
//������Ʒ��ʱ�򣬺����׳����ⲿ����������������̫�ɿ�
#ifdef  RTC_CLOCK_SOURCE_LSE
    /* ʹ�� LSE */
    RCC_LSEConfig(RCC_LSE_ON);

    /* �ȴ� LSE ׼���� */
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {}

    /* ѡ�� LSE ��Ϊ RTC ʱ��Դ */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    /* ʹ�� RTC ʱ�� */
    RCC_RTCCLKCmd(ENABLE);

    /* �ȴ� RTC �Ĵ��� ͬ��
     * ��ΪRTCʱ���ǵ��ٵģ��ڻ�ʱ���Ǹ��ٵģ�����Ҫͬ��
     */
    RTC_WaitForSynchro();

    /* ȷ����һ�� RTC �Ĳ������ */
    RTC_WaitForLastTask();

    /* ʹ�� RTC ���ж� */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);

    /* ȷ����һ�� RTC �Ĳ������ */
    RTC_WaitForLastTask();

    /* ���� RTC ��Ƶ: ʹ RTC ����Ϊ1s  */
    /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) = 1HZ */
    RTC_SetPrescaler(32767);

    /* ȷ����һ�� RTC �Ĳ������ */
    RTC_WaitForLastTask();

#else

    /* ʹ�� LSI */
    RCC_LSICmd(ENABLE);

    /* �ȴ� LSI ׼���� */
    while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
    {}

    /* ѡ�� LSI ��Ϊ RTC ʱ��Դ */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

    /* ʹ�� RTC ʱ�� */
    RCC_RTCCLKCmd(ENABLE);

    /* �ȴ� RTC �Ĵ��� ͬ��
     * ��ΪRTCʱ���ǵ��ٵģ��ڻ�ʱ���Ǹ��ٵģ�����Ҫͬ��
     */
    RTC_WaitForSynchro();

    /* ȷ����һ�� RTC �Ĳ������ */
    RTC_WaitForLastTask();

    /* ʹ�� RTC ���ж� */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);

    /* ȷ����һ�� RTC �Ĳ������ */
    RTC_WaitForLastTask();

    /* ���� RTC ��Ƶ: ʹ RTC ����Ϊ1s ,LSIԼΪ40KHz */
    /* RTC period = RTCCLK/RTC_PR = (40 KHz)/(40000-1+1) = 1HZ */
    RTC_SetPrescaler(40000 - 1);

    /* ȷ����һ�� RTC �Ĳ������ */
    RTC_WaitForLastTask();
#endif

}



/*
 * ��������Time_Regulate_Get
 * ����  �������û�ʹ�ô������õ�ʱ�䣬
 *         �Ա����ת����ʱ����洢��RTC �����Ĵ����С�
 * ����  �����ڶ�ȡRTCʱ��Ľṹ��ָ��
 * ע��  ���ڴ��ڵ�����������ʱ������������Ҫ�ӻس�
 */
void Time_Regulate_Get(struct rtc_time *tm)
{
    uint32_t temp_num = 0;
    uint8_t day_max = 0 ;

    PRINTF("\r\n=========================����ʱ��==================");

    do {
        PRINTF("\r\n  ���������(Please Set Years),��Χ[1970~2038]�������ַ�����ӻس�:");
        scanf("%d", &temp_num);

        if (temp_num < 1970 || temp_num > 2038) {
            PRINTF("\r\n �������������:%d��������Ҫ��", temp_num);

        } else {
            PRINTF("\n\r  ��ݱ�����Ϊ: %d\n\r", temp_num);

            tm->tm_year = temp_num;
            break;
        }
    } while (1);


    do {
        PRINTF("\r\n  �������·�(Please Set Months):��Χ[1~12]�������ַ�����ӻس�:");
        scanf("%d", &temp_num);

        if (temp_num < 1 || temp_num > 12) {
            PRINTF("\r\n �������������:%d��������Ҫ��", temp_num);

        } else {
            PRINTF("\n\r  �·ݱ�����Ϊ: %d\n\r", temp_num);

            tm->tm_mon = temp_num;
            break;
        }
    } while (1);

    /*�����·ݼ����������*/
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

            /*��������*/
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
        PRINTF("\r\n  ����������(Please Set Months),��Χ[1~%d]�������ַ�����ӻس�:", day_max);
        scanf("%d", &temp_num);

        if (temp_num < 1 || temp_num > day_max) {
            PRINTF("\r\n �������������:%d��������Ҫ��", temp_num);
        } else {
            PRINTF("\n\r  ���ڱ�����Ϊ: %d\n\r", temp_num);

            tm->tm_mday = temp_num;
            break;
        }
    } while (1);

    do {
        PRINTF("\r\n  ������ʱ��(Please Set Hours),��Χ[0~23]�������ַ�����ӻس�:");
        scanf("%d", &temp_num);

        if (temp_num > 23) {
            PRINTF("\r\n �������������:%d��������Ҫ��", temp_num);
        } else {
            PRINTF("\n\r  ʱ�ӱ�����Ϊ: %d\n\r", temp_num);

            tm->tm_hour = temp_num;
            break;
        }
    } while (1);

    do {
        PRINTF("\r\n  ���������(Please Set Minutes),��Χ[0~59]�������ַ�����ӻس�:");
        scanf("%d", &temp_num);

        if (temp_num > 59) {
            PRINTF("\r\n �������������:%d��������Ҫ��", temp_num);
        } else {
            PRINTF("\n\r  ���ӱ�����Ϊ: %d\n\r", temp_num);

            tm->tm_min = temp_num;
            break;
        }
    } while (1);

    do {
        PRINTF("\r\n  ����������(Please Set Seconds),��Χ[0~59]�������ַ�����ӻس�:");
        scanf("%d", &temp_num);

        if (temp_num > 59) {
            PRINTF("\r\n �������������:%d��������Ҫ��", temp_num);
        } else {
            PRINTF("\n\r  ���ӱ�����Ϊ: %d\n\r", temp_num);

            tm->tm_sec = temp_num;
            break;
        }
    } while (1);

}


/*
 * ��������Time_Adjust
 * ����  ��ʱ�����
 * ����  �����ڶ�ȡRTCʱ��Ľṹ��ָ�루����ʱ�䣩
 * ���  ����
 * ����  ���ⲿ����
 */
void time_adjust(struct rtc_time *tm)
{

    /* RTC ���� */
    RTC_Configuration();

    /* �ȴ�ȷ����һ�β������ */
    RTC_WaitForLastTask();

    /* �������� */
    GregorianDay(tm);

    /* �����ڼ���ʱ�����д�뵽RTC�����Ĵ��� */
    RTC_SetCounter(mktimev(tm) - TIME_ZOOM);

    /* �ȴ�ȷ����һ�β������ */
    RTC_WaitForLastTask();
}

/*
 * ��������calendar_test
 * ����  �����Թ�����ũ������ʮ�Ľ���
 */
static void calendar_test(struct rtc_time *tm)
{
    uint8_t str[200];

    GetChinaCalendar((u16)tm->tm_year, (u8)tm->tm_mon, (u8)tm->tm_mday, str);
    PRINTF("Solar calendar: %0.2d%0.2d,%0.2d,%0.2d\r\n", str[0], str[1], str[2],  str[3]);

    GetChinaCalendarStr((u16)tm->tm_year, (u8)tm->tm_mon, (u8)tm->tm_mday, str);
    PRINTF("lunar calendar: %s\r\n", str);

    if (GetJieQiStr((u16)tm->tm_year, (u8)tm->tm_mon, (u8)tm->tm_mday, str))
        PRINTF("lunar calendar: %s\r\n", str);
}

/*
 * ��������time_update
 * ����  ����ȡ����ʱ��ֵ
 * ����  ��-TimeVar RTC����ֵ����λΪ s
 * ���  ����
 * ����  ���ڲ�����
 */
void time_update(struct rtc_time *tm)
{
    uint32_t BJ_TimeVar;
    uint32_t TimeVar;

    TimeVar = RTC_GetCounter();

    /*  �ѱ�׼ʱ��ת��Ϊ����ʱ��*/
    BJ_TimeVar = TimeVar + TIME_ZOOM;

    to_tm(BJ_TimeVar, tm);  /* �Ѷ�ʱ����ֵת��Ϊ����ʱ�� */

    /* ���ʱ���������ʱ�� */
    PRINTF("UNIX value = %d \r\n", TimeVar);

    PRINTF("Time: %d-%d-%d %0.2d:%0.2d:%0.2d (Zodiac Sign: %s, Weekday: %s) \r\n",    tm->tm_year, tm->tm_mon, tm->tm_mday, 
        tm->tm_hour, tm->tm_min, tm->tm_sec, 
        en_zodiac_sign[(tm->tm_year - 3) % 12], en_WEEK_STR[tm->tm_wday]);

}


/************************END OF FILE***************************************/
