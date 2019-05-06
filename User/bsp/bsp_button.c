/**
  ******************************************************************************
  * @file:      bsp_button.c
  * @author:    Cat
  * @version:   V1.0
  * @date:      2018-3-3
  * @brief:     ��������
  * @attention:
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "bsp_button.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


void ena_button_gpio_config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* ʹ��GPIO������ʱ�� */
    RCC_APB2PeriphClockCmd(ENA_BUTTON_GPIO_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = ENA_BUTTON_GPIO_PIN;  /* ѡ��Ҫ���Ƶ�GPIO���� */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   /* �������� */
    GPIO_Init(ENA_BUTTON_GPIO_PORT, &GPIO_InitStructure);   /* ���⺯������ʼ��GPIO */
}


uint8_t ena_button_read(void)
{
    return GPIO_ReadInputDataBit(ENA_BUTTON_GPIO_PORT, ENA_BUTTON_GPIO_PIN);
}


void enb_button_gpio_config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* ʹ��GPIO������ʱ�� */
    RCC_APB2PeriphClockCmd(ENB_BUTTON_GPIO_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = ENB_BUTTON_GPIO_PIN;  /* ѡ��Ҫ���Ƶ�GPIO���� */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   /* �������� */
    GPIO_Init(ENB_BUTTON_GPIO_PORT, &GPIO_InitStructure);   /* ���⺯������ʼ��GPIO */
}


uint8_t enb_button_read(void)
{
    return GPIO_ReadInputDataBit(ENB_BUTTON_GPIO_PORT, ENB_BUTTON_GPIO_PIN);
}


void sw_button_gpio_config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* ʹ��GPIO������ʱ�� */
    RCC_APB2PeriphClockCmd(SW_BUTTON_GPIO_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = SW_BUTTON_GPIO_PIN;  /* ѡ��Ҫ���Ƶ�GPIO���� */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   /* �������� */
    GPIO_Init(SW_BUTTON_GPIO_PORT, &GPIO_InitStructure);    /* ���⺯������ʼ��GPIO */
}


uint8_t sw_button_read(void)
{
    return GPIO_ReadInputDataBit(SW_BUTTON_GPIO_PORT, SW_BUTTON_GPIO_PIN);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
