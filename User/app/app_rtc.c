/**
  ******************************************************************************
  * @file:      app_rtc.c
  * @author:    Cat?????
  * @version:   V1.0
  * @date:      2019-1-1
  * @brief:     
  * @attention:
  ******************************************************************************
  */


#include "./app_rtc.h"

#include "./../sys/sys_scheduler.h"
#include "./../sys/sys_command_line.h"
#include "./../sys/sys_queue.h"



/* ??????????2019-01-01 00:00:00 */
struct rtc_time systmtime = {
    0, 0, 0, 1, 1, 2019, 0
};


void rtc_set(struct rtc_time *set_time)
{
    /* ??RTC */
    time_adjust(set_time);

    /* ?????????? */
    BKP_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}


/**
  * @brief  RTC???
  * @param  None
  * @retval None
  */
void rtc_init(void)
{
    RTC_NVIC_Config();
    RTC_CheckAndConfig(&systmtime);
}


/**
  * @brief  ??RTC??
  * @param  None
  * @retval None
  */
void rtc_task(void)
{
    time_update(&systmtime);
}


/*********************************************END OF FILE**********************/