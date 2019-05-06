/**
  ******************************************************************************
  * @file:      bsp_timer.h
  * @author:    Cat（孙关平）
  * @version:   V1.0
  * @date:      2018-3-23
  * @brief:     定时器驱动
  * @attention:
  ******************************************************************************
  */


#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H


#include "stm32f10x_tim.h"

#include "global.h"



void timer_62us_init(uint16_t arr, uint16_t psc);



#endif /* __BSP_TIMER_H */

