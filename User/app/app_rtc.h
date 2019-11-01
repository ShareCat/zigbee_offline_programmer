/**
  ******************************************************************************
  * @file:      app_rtc.h
  * @author:    Cat?????
  * @version:   V1.0
  * @date:      2019-1-1
  * @brief:     
  * @attention:
  ******************************************************************************
  */


#ifndef __APP_RTC_H
#define	__APP_RTC_H

#include "global.h"

#include "./../bsp/bsp_rtc.h"
#include "./../bsp/bsp_date.h"
#include "./../bsp/bsp_calendar.h"


void rtc_init(void);
void rtc_task(void);


#endif /* __APP_RTC_H */
