/**
  ******************************************************************************
  * @file:      bsp_timer.h
  * @author:    Cat�����ƽ��
  * @version:   V1.0
  * @date:      2018-3-23
  * @brief:     ��ʱ������
  * @attention:
  ******************************************************************************
  */


#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H


#include "stm32f10x_tim.h"

#include "global.h"



void timer_62us_init(uint16_t arr, uint16_t psc);



#endif /* __BSP_TIMER_H */

