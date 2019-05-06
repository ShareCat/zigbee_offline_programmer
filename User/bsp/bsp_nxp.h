/**
  ******************************************************************************
  * @file:      bsp_nxp.h
  * @author:    Cat£¨Ëï¹ØÆ½£©
  * @version:   V1.0
  * @date:      2019-1-1
  * @brief:
  * @attention:
  ******************************************************************************
  */


#ifndef __BSP_NXP_H
#define __BSP_NXP_H


#include "global.h"


/* RELAY_1 */
#define NXP_RESET_PIN               GPIO_Pin_0              /* PB0 */
#define NXP_RESET_PORT              GPIOB
#define NXP_RESET_CLK               RCC_APB2Periph_GPIOB

#define NXP_DO1_PIN                 GPIO_Pin_1              /* PB1 */
#define NXP_DO1_PORT                GPIOB
#define NXP_DO1_CLK                 RCC_APB2Periph_GPIOB


void nxp_gpio_config(void);
void nxp_reset_ctrl(uint8_t state);
void nxp_do1_ctrl(uint8_t state);


#endif /* __BSP_NXP_H */

