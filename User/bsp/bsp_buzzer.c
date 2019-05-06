/**
  ******************************************************************************
  * @file:      bsp_buzzer.c
  * @author:    Cat£¨Ëï¹ØÆ½£©
  * @version:   V1.0
  * @date:      2019-1-1
  * @brief:
  * @attention:
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "./bsp_buzzer.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


void buzzer_ctrl(uint8_t state)
{
    if (DISABLE == state) {
        GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN);
    } else {
        GPIO_SetBits(BUZZER_PORT, BUZZER_PIN);
    }
}


void buzzer_gpio_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(BUZZER_CLK, ENABLE);

    /* RESET */
    GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);
    buzzer_ctrl(DISABLE);
}


/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

