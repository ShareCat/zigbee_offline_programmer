/**
  ******************************************************************************
  * @file:      app_buzzer.c
  * @author:    Cat（孙关平）
  * @version:   V1.0
  * @date:      2018-3-3
  * @brief:     按键处理
  * @attention:
  ******************************************************************************
  */


#include "./app_buzzer.h"

#include "./../bsp/bsp_buzzer.h"


uint8_t buzzer_state = E_BUZZER_NULL;
QUEUE8_S buzzer_queue;


/**
  * @brief  蜂鸣器初始化
  * @param  null
  * @retval null
  */
void buzzer_init(void)
{
    QUEUE_INIT(buzzer_queue);
    buzzer_gpio_config();
}


/**
  * @brief  蜂鸣器短一声，提示下载成功
  * @param  null
  * @retval null
  */
void buzzer_notice_ok(void)
{
    uint8_t event = E_BUZZER_SHORT_SOUND;

    QUEUE_IN(buzzer_queue, event);
}


/**
  * @brief  蜂鸣器短二声，提示下载失败
  * @param  null
  * @retval null
  */
void buzzer_notice_fail(void)
{
    uint8_t event = E_BUZZER_SHORT_SOUND;

    QUEUE_IN(buzzer_queue, event);
    QUEUE_IN(buzzer_queue, event);
}


/**
  * @brief  蜂鸣器长一声，提示配置成功
  * @param  null
  * @retval null
  */
void buzzer_notice_config_ok(void)
{
    uint8_t event = E_BUZZER_LONG_SOUND;

    QUEUE_IN(buzzer_queue, event);
}


/**
  * @brief  蜂鸣器长二声，提示配置失败
  * @param  null
  * @retval null
  */
void buzzer_notice_config_fail(void)
{
    uint8_t event = E_BUZZER_LONG_SOUND;

    QUEUE_IN(buzzer_queue, event);
    QUEUE_IN(buzzer_queue, event);
}


/**
  * @brief  蜂鸣器任务
  * @param  null
  * @retval null
  */
void buzzer_task(void)
{
    switch (buzzer_state) {
        case E_BUZZER_NULL: {
            uint8_t event;
            if (TRUE == QUEUE_OUT(buzzer_queue, event)) {
                if ((E_BUZZER_SHORT_SOUND == event)
                    || (E_BUZZER_LONG_SOUND == event)) {
                    buzzer_state = event;
                }
            }
        }
        break;

        case E_BUZZER_SHORT_SOUND: {
            buzzer_ctrl(ENABLE);
            os_timer_set(T_60MS);
            buzzer_state = E_BUZZER_SHORT_HOLD;
        }
        break;

        case E_BUZZER_SHORT_HOLD: {
            if (!os_timer_check()) {
                buzzer_ctrl(DISABLE);
                os_timer_set(T_80MS);
                buzzer_state = E_BUZZER_HOLD_SILENCE;
            }
        }
        break;

        case E_BUZZER_LONG_SOUND: {
            buzzer_ctrl(ENABLE);
            os_timer_set(T_2S);
            buzzer_state = E_BUZZER_LONG_HOLD;
        }
        break;

        case E_BUZZER_LONG_HOLD: {
            if (!os_timer_check()) {
                buzzer_ctrl(DISABLE);
                os_timer_set(T_300MS);
                buzzer_state = E_BUZZER_HOLD_SILENCE;
            }
        }
        break;

        case E_BUZZER_HOLD_SILENCE: {
            if (!os_timer_check()) {
                buzzer_state = E_BUZZER_NULL;
            }
        }
        break;

        default: {
            buzzer_state = E_BUZZER_NULL;
        }
        break;
    }
}


/*********************************************END OF FILE**********************/
