/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ledӦ�ú����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F407 ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "bsp_relay.h"



/**
 * @brief  RELAY����
 * @param  relay:   �̵���
 * @param  ENABLE:  ����
 * @param  DISABLE: �ر�
 * @retval ��
 */
void relay_ctrl(uint8_t relay, FunctionalState cmd)
{
    uint8_t err = FALSE;
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;

    switch(relay) {
        case E_RELAY_1: {
            GPIOx = RELAY_1_PORT;
            GPIO_Pin = RELAY_1_PIN;
        }
        break;

        case E_RELAY_2: {
            GPIOx = RELAY_2_PORT;
            GPIO_Pin = RELAY_2_PIN;
        }
        break;

        case E_RELAY_3: {
            GPIOx = RELAY_3_PORT;
            GPIO_Pin = RELAY_3_PIN;
        }
        break;

        case E_RELAY_4: {
            GPIOx = RELAY_4_PORT;
            GPIO_Pin = RELAY_4_PIN;
        }
        break;

        case E_RELAY_5: {
            GPIOx = RELAY_5_PORT;
            GPIO_Pin = RELAY_5_PIN;
        }
        break;

        case E_RELAY_6: {
            GPIOx = RELAY_6_PORT;
            GPIO_Pin = RELAY_6_PIN;
        }
        break;

        case E_RELAY_7: {
            GPIOx = RELAY_7_PORT;
            GPIO_Pin = RELAY_7_PIN;
        }
        break;

        case E_RELAY_8: {
            GPIOx = RELAY_8_PORT;
            GPIO_Pin = RELAY_8_PIN;
        }
        break;

        case E_RELAY_9: {
            GPIOx = RELAY_9_PORT;
            GPIO_Pin = RELAY_9_PIN;
        }
        break;

        case E_RELAY_10: {
            GPIOx = RELAY_10_PORT;
            GPIO_Pin = RELAY_10_PIN;
        }
        break;

        case E_RELAY_11: {
            GPIOx = RELAY_11_PORT;
            GPIO_Pin = RELAY_11_PIN;
        }
        break;

        case E_RELAY_12: {
            GPIOx = RELAY_12_PORT;
            GPIO_Pin = RELAY_12_PIN;
        }
        break;

        case E_RELAY_13: {
            GPIOx = RELAY_13_PORT;
            GPIO_Pin = RELAY_13_PIN;
        }
        break;

        case E_RELAY_14: {
            GPIOx = RELAY_14_PORT;
            GPIO_Pin = RELAY_14_PIN;
        }
        break;

        case E_RELAY_15: {
            GPIOx = RELAY_15_PORT;
            GPIO_Pin = RELAY_15_PIN;
        }
        break;

        case E_RELAY_16: {
            GPIOx = RELAY_16_PORT;
            GPIO_Pin = RELAY_16_PIN;
        }
        break;

        case E_RELAY_17: {
            GPIOx = RELAY_17_PORT;
            GPIO_Pin = RELAY_17_PIN;
        }
        break;

        case E_RELAY_18: {
            GPIOx = RELAY_18_PORT;
            GPIO_Pin = RELAY_18_PIN;
        }
        break;

        case E_RELAY_19: {
            GPIOx = RELAY_19_PORT;
            GPIO_Pin = RELAY_19_PIN;
        }
        break;

        case E_RELAY_SW: {
            GPIOx = RELAY_SW_PORT;
            GPIO_Pin = RELAY_SW_PIN;
        }
        break;

        case E_RELAY_KEY: {
            GPIOx = RELAY_KEY_PORT;
            GPIO_Pin = RELAY_KEY_PIN;
        }
        break;

        case E_RELAY_IR: {
            GPIOx = RELAY_IR_PORT;
            GPIO_Pin = RELAY_IR_PIN;
        }
        break;

        case E_RELAY_SMOKE: {
            GPIOx = RELAY_SMOKE_PORT;
            GPIO_Pin = RELAY_SMOKE_PIN;
        }
        break;

        case E_S2000_SEL1: {
            GPIOx = S2000_SEL1_PORT;
            GPIO_Pin = S2000_SEL1_PIN;
        }
        break;

        case E_S2000_SEL2: {
            GPIOx = S2000_SEL2_PORT;
            GPIO_Pin = S2000_SEL2_PIN;
        }
        break;

        case E_S2000_SEL3: {
            GPIOx = S2000_SEL3_PORT;
            GPIO_Pin = S2000_SEL3_PIN;
        }
        break;

        case E_S2000_SEL4: {
            GPIOx = S2000_SEL4_PORT;
            GPIO_Pin = S2000_SEL4_PIN;
        }
        break;

        case E_S2000_SEL5: {
            GPIOx = S2000_SEL5_PORT;
            GPIO_Pin = S2000_SEL5_PIN;
        }
        break;

        case E_S2000_SEL6: {
            GPIOx = S2000_SEL6_PORT;
            GPIO_Pin = S2000_SEL6_PIN;
        }
        break;

        case E_S2000_SEL7: {
            GPIOx = S2000_SEL7_PORT;
            GPIO_Pin = S2000_SEL7_PIN;
        }
        break;

        case E_S2000_SEL8: {
            GPIOx = S2000_SEL8_PORT;
            GPIO_Pin = S2000_SEL8_PIN;
        }
        break;

        default: {
            err = TRUE;
            break;
        }
    }

    if(FALSE == err) {
        if(ENABLE == cmd) {
            /* �����̵��� */
            IO_HIGH(GPIOx, GPIO_Pin);
        } else {
            /* �رռ̵��� */
            IO_LOW(GPIOx, GPIO_Pin);
        }
    }
}


/**
 * @brief  RELAY_KEY
 * @param  ENABLE:  ����
 * @param  DISABLE: �ر�
 * @retval ��
 */
void relay_key_ctrl(FunctionalState cmd)
{
    if(ENABLE == cmd) {
        IO_HIGH(RELAY_KEY_PORT, RELAY_KEY_PIN);
    } else {
        IO_LOW(RELAY_KEY_PORT, RELAY_KEY_PIN);
    }
}


/**
 * @brief  RELAY_IR
 * @param  ENABLE:  ����
 * @param  DISABLE: �ر�
 * @retval ��
 */
void relay_ir_ctrl(FunctionalState cmd)
{
    if(ENABLE == cmd) {
        IO_HIGH(RELAY_IR_PORT, RELAY_IR_PIN);
    } else {
        IO_LOW(RELAY_IR_PORT, RELAY_IR_PIN);
    }
}


/**
 * @brief  RELAY_SMOKE
 * @param  ENABLE:  ����
 * @param  DISABLE: �ر�
 * @retval ��
 */
void relay_smoke_ctrl(FunctionalState cmd)
{
    if(ENABLE == cmd) {
        IO_HIGH(RELAY_SMOKE_PORT, RELAY_SMOKE_PIN);
    } else {
        IO_LOW(RELAY_SMOKE_PORT, RELAY_SMOKE_PIN);
    }
}


/**
 * @brief  ��ʼ������RELAY��IO
 * @param  ��
 * @retval ��
 */
void relay_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /*����LED��ص�GPIO����ʱ��*/
    RCC_APB2PeriphClockCmd(RELAY_1_CLK |
                           RELAY_2_CLK |
                           RELAY_3_CLK |
                           RELAY_4_CLK |
                           RELAY_5_CLK |
                           RELAY_6_CLK |
                           RELAY_7_CLK |
                           RELAY_8_CLK |
                           RELAY_9_CLK |
                           RELAY_10_CLK |
                           RELAY_11_CLK |
                           RELAY_12_CLK |
                           RELAY_13_CLK |
                           RELAY_14_CLK |
                           RELAY_15_CLK |
                           RELAY_16_CLK |
                           RELAY_17_CLK |
                           RELAY_18_CLK |
                           RELAY_19_CLK |
                           RELAY_SW_CLK |
                           RELAY_KEY_CLK |
                           RELAY_IR_CLK |
                           RELAY_SMOKE_CLK |
                           S2000_SEL1_CLK |
                           S2000_SEL2_CLK |
                           S2000_SEL3_CLK |
                           S2000_SEL4_CLK |
                           S2000_SEL5_CLK |
                           S2000_SEL6_CLK |
                           S2000_SEL7_CLK |
                           S2000_SEL8_CLK, ENABLE);

    /* RELAY_1 */
    GPIO_InitStructure.GPIO_Pin = RELAY_1_PIN;          /* ѡ��Ҫ���Ƶ�GPIO���� */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    /* ģʽΪͨ��������� */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   /* ����Ϊ50MHz */
    GPIO_Init(RELAY_1_PORT, &GPIO_InitStructure);       /* ���⺯������ʼ��GPIO */
    GPIO_ResetBits(RELAY_1_PORT, RELAY_1_PIN);          /* �ϵ��ȹرռ̵��� */

    /* RELAY_2 */
    GPIO_InitStructure.GPIO_Pin = RELAY_2_PIN;
    GPIO_Init(RELAY_2_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(RELAY_2_PORT, RELAY_2_PIN);          /* �ϵ��ȹرռ̵��� */

    /* RELAY_3 */
    GPIO_InitStructure.GPIO_Pin = RELAY_3_PIN;
    GPIO_Init(RELAY_3_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(RELAY_3_PORT, RELAY_3_PIN);          /* �ϵ��ȹرռ̵��� */

    /* RELAY_4 */
    GPIO_InitStructure.GPIO_Pin = RELAY_4_PIN;
    GPIO_Init(RELAY_4_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(RELAY_4_PORT, RELAY_4_PIN);          /* �ϵ��ȹرռ̵��� */

    /* RELAY_5 */
    GPIO_InitStructure.GPIO_Pin = RELAY_5_PIN;
    GPIO_Init(RELAY_5_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(RELAY_5_PORT, RELAY_5_PIN);          /* �ϵ��ȹرռ̵��� */

    /* RELAY_6 */
    GPIO_InitStructure.GPIO_Pin = RELAY_6_PIN;
    GPIO_Init(RELAY_6_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(RELAY_6_PORT, RELAY_6_PIN);          /* �ϵ��ȹرռ̵��� */

    /* RELAY_7 */
    GPIO_InitStructure.GPIO_Pin = RELAY_7_PIN;
    GPIO_Init(RELAY_7_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(RELAY_7_PORT, RELAY_7_PIN);          /* �ϵ��ȹرռ̵��� */

    /* RELAY_8 */
    GPIO_InitStructure.GPIO_Pin = RELAY_8_PIN;
    GPIO_Init(RELAY_8_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(RELAY_8_PORT, RELAY_8_PIN);          /* �ϵ��ȹرռ̵��� */

    /* RELAY_9 */
    GPIO_InitStructure.GPIO_Pin = RELAY_9_PIN;
    GPIO_Init(RELAY_9_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(RELAY_9_PORT, RELAY_9_PIN);          /* �ϵ��ȹرռ̵��� */

    /* RELAY_10 */
    GPIO_InitStructure.GPIO_Pin = RELAY_10_PIN;
    GPIO_Init(RELAY_10_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(RELAY_10_PORT, RELAY_10_PIN);        /* �ϵ��ȹرռ̵��� */

    /* RELAY_11 */
    GPIO_InitStructure.GPIO_Pin = RELAY_11_PIN;
    GPIO_Init(RELAY_11_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(RELAY_11_PORT, RELAY_11_PIN);        /* �ϵ��ȹرռ̵��� */

    /* RELAY_12 */
    GPIO_InitStructure.GPIO_Pin = RELAY_12_PIN;
    GPIO_Init(RELAY_12_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(RELAY_12_PORT, RELAY_12_PIN);        /* �ϵ��ȹرռ̵��� */

    /* RELAY_13 */
    GPIO_InitStructure.GPIO_Pin = RELAY_13_PIN;
    GPIO_Init(RELAY_13_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(RELAY_13_PORT, RELAY_13_PIN);        /* �ϵ��ȹرռ̵��� */

    /* RELAY_14 */
    GPIO_InitStructure.GPIO_Pin = RELAY_14_PIN;
    GPIO_Init(RELAY_14_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(RELAY_14_PORT, RELAY_14_PIN);        /* �ϵ��ȹرռ̵��� */

    /* RELAY_15 */
    GPIO_InitStructure.GPIO_Pin = RELAY_15_PIN;
    GPIO_Init(RELAY_15_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(RELAY_15_PORT, RELAY_15_PIN);        /* �ϵ��ȹرռ̵��� */

    /* RELAY_16 */
    GPIO_InitStructure.GPIO_Pin = RELAY_16_PIN;
    GPIO_Init(RELAY_16_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(RELAY_16_PORT, RELAY_16_PIN);        /* �ϵ��ȹرռ̵��� */

    /* RELAY_17 */
    GPIO_InitStructure.GPIO_Pin = RELAY_17_PIN;
    GPIO_Init(RELAY_17_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(RELAY_17_PORT, RELAY_17_PIN);        /* �ϵ��ȹرռ̵��� */

    /* RELAY_18 */
    GPIO_InitStructure.GPIO_Pin = RELAY_18_PIN;
    GPIO_Init(RELAY_18_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(RELAY_18_PORT, RELAY_18_PIN);        /* �ϵ��ȹرռ̵��� */

    /* RELAY_19 */
    GPIO_InitStructure.GPIO_Pin = RELAY_19_PIN;
    GPIO_Init(RELAY_19_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(RELAY_19_PORT, RELAY_19_PIN);        /* �ϵ��ȹرռ̵��� */

    /* RELAY_SW */
    GPIO_InitStructure.GPIO_Pin = RELAY_SW_PIN;
    GPIO_Init(RELAY_SW_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(RELAY_SW_PORT, RELAY_SW_PIN);        /* �ϵ��ȹرռ̵��� */

    /* RELAY_KEY */
    GPIO_InitStructure.GPIO_Pin = RELAY_KEY_PIN;
    GPIO_Init(RELAY_KEY_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(RELAY_KEY_PORT, RELAY_KEY_PIN);      /* �ϵ��ȹرռ̵��� */

    /* RELAY_IR */
    GPIO_InitStructure.GPIO_Pin = RELAY_IR_PIN;
    GPIO_Init(RELAY_IR_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(RELAY_IR_PORT, RELAY_IR_PIN);        /* �ϵ��ȹرռ̵��� */

    /* RELAY_SMOKE */
    GPIO_InitStructure.GPIO_Pin = RELAY_SMOKE_PIN;
    GPIO_Init(RELAY_SMOKE_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(RELAY_SMOKE_PORT, RELAY_SMOKE_PIN);  /* �ϵ��ȹرռ̵��� */

    /* S2000_SEL1 */
    GPIO_InitStructure.GPIO_Pin = S2000_SEL1_PIN;
    GPIO_Init(S2000_SEL1_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(S2000_SEL1_PORT, S2000_SEL1_PIN);    /* �ϵ��ȹرռ̵��� */

    /* S2000_SEL2 */
    GPIO_InitStructure.GPIO_Pin = S2000_SEL2_PIN;
    GPIO_Init(S2000_SEL2_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(S2000_SEL2_PORT, S2000_SEL2_PIN);    /* �ϵ��ȹرռ̵��� */

    /* S2000_SEL3 */
    GPIO_InitStructure.GPIO_Pin = S2000_SEL3_PIN;
    GPIO_Init(S2000_SEL3_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(S2000_SEL3_PORT, S2000_SEL3_PIN);    /* �ϵ��ȹرռ̵��� */

    /* S2000_SEL4 */
    GPIO_InitStructure.GPIO_Pin = S2000_SEL4_PIN;
    GPIO_Init(S2000_SEL4_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(S2000_SEL4_PORT, S2000_SEL4_PIN);    /* �ϵ��ȹرռ̵��� */

    /* S2000_SEL5 */
    GPIO_InitStructure.GPIO_Pin = S2000_SEL5_PIN;
    GPIO_Init(S2000_SEL5_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(S2000_SEL5_PORT, S2000_SEL5_PIN);    /* �ϵ��ȹرռ̵��� */

    /* S2000_SEL6 */
    GPIO_InitStructure.GPIO_Pin = S2000_SEL6_PIN;
    GPIO_Init(S2000_SEL6_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(S2000_SEL6_PORT, S2000_SEL6_PIN);    /* �ϵ��ȹرռ̵��� */

    /* S2000_SEL7 */
    GPIO_InitStructure.GPIO_Pin = S2000_SEL7_PIN;
    GPIO_Init(S2000_SEL7_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(S2000_SEL7_PORT, S2000_SEL7_PIN);    /* �ϵ��ȹرռ̵��� */

    /* S2000_SEL8 */
    GPIO_InitStructure.GPIO_Pin = S2000_SEL8_PIN;
    GPIO_Init(S2000_SEL8_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(S2000_SEL8_PORT, S2000_SEL8_PIN);    /* �ϵ��ȹرռ̵��� */
}


/*********************************************END OF FILE**********************/
