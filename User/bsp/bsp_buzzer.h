/**
  ******************************************************************************
  * @file:      bsp_buzzer.h
  * @author:    Cat£¨Ëï¹ØÆ½£©
  * @version:   V1.0
  * @date:      2019-1-1
  * @brief:
  * @attention:
  ******************************************************************************
  */


#ifndef __BSP_BUZZER_H
#define __BSP_BUZZER_H


#include "global.h"


/* BUZZER */
#define BUZZER_PIN              GPIO_Pin_2              /* PB2 */
#define BUZZER_PORT             GPIOB
#define BUZZER_CLK              RCC_APB2Periph_GPIOB


void buzzer_gpio_config(void);
void buzzer_ctrl(uint8_t state);


#endif /* __BSP_BUZZER_H */

