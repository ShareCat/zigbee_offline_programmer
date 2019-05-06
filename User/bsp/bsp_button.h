/**
  ******************************************************************************
  * @file:      bsp_button.h
  * @author:    Cat
  * @version:   V1.0
  * @date:      2018-3-3
  * @brief:     按键驱动
  * @attention:
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_BUTTON_H
#define __BSP_BUTTON_H
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "global.h"


/* ENA键 */
#define ENA_BUTTON_GPIO_PORT        GPIOB
#define ENA_BUTTON_GPIO_CLK         RCC_APB2Periph_GPIOB
#define ENA_BUTTON_GPIO_PIN         GPIO_Pin_8
#define ENA_BUTTON_PRESSED          0           /* 按键被按下时候的电平 */

/* ENB键 */
#define ENB_BUTTON_GPIO_PORT        GPIOB
#define ENB_BUTTON_GPIO_CLK         RCC_APB2Periph_GPIOB
#define ENB_BUTTON_GPIO_PIN         GPIO_Pin_9
#define ENB_BUTTON_PRESSED          0           /* 按键被按下时候的电平 */

/* SW键 */
#define SW_BUTTON_GPIO_PORT         GPIOB
#define SW_BUTTON_GPIO_CLK          RCC_APB2Periph_GPIOB
#define SW_BUTTON_GPIO_PIN          GPIO_Pin_10
#define SW_BUTTON_PRESSED           0           /* 按键被按下时候的电平 */


extern void ena_button_gpio_config(void);
extern uint8_t ena_button_read(void);
extern void enb_button_gpio_config(void);
extern uint8_t enb_button_read(void);
extern void sw_button_gpio_config(void);
extern uint8_t sw_button_read(void);



/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/




#endif /* __BSP_BUTTON_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
