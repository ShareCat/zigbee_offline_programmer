/**
  ******************************************************************************
  * @file:      bsp_nxp.c
  * @author:    Cat£¨Ëï¹ØÆ½£©
  * @version:   V1.0
  * @date:      2019-1-1
  * @brief:
  * @attention:
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "./bsp_nxp.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void nxp_gpio_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(NXP_RESET_CLK |
                           NXP_DO1_CLK, ENABLE);

    /* RESET */
    GPIO_InitStructure.GPIO_Pin = NXP_RESET_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(NXP_RESET_PORT, &GPIO_InitStructure);
    GPIO_SetBits(NXP_RESET_PORT, NXP_RESET_PIN);

    /* DO1 */
    GPIO_InitStructure.GPIO_Pin = NXP_DO1_PIN;
    GPIO_Init(NXP_DO1_PORT, &GPIO_InitStructure);
    GPIO_SetBits(NXP_DO1_PORT, NXP_DO1_PIN);
}


void nxp_reset_ctrl(uint8_t state)
{
    if(DISABLE == state) {
        GPIO_ResetBits(NXP_RESET_PORT, NXP_RESET_PIN);
    } else {
        GPIO_SetBits(NXP_RESET_PORT, NXP_RESET_PIN);
    }
}


void nxp_do1_ctrl(uint8_t state)
{
    if(DISABLE == state) {
        GPIO_ResetBits(NXP_DO1_PORT, NXP_DO1_PIN);
    } else {
        GPIO_SetBits(NXP_DO1_PORT, NXP_DO1_PIN);
    }
}


/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

