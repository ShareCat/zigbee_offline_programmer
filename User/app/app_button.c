/**
  ******************************************************************************
  * @file:      app_button.c
  * @author:    Cat（孙关平）
  * @version:   V1.0
  * @date:      2018-3-3
  * @brief:     按键处理
  * @attention:
  ******************************************************************************
  */


#include "./app_button.h"

#include "./../bsp/bsp_button.h"
#include "./../component/button/button.h"

#include "./app_nxp.h"
#include "./app_buzzer.h"


QUEUE8_S button_queue;


struct Button btn_ena;

/**
  * @brief  ena按键单击服务函数
  * @param  null
  * @retval null
  */
void ena_button_single_click_handler(void* btn)
{
    PRINTF("ena single click\r\n");

    uint8_t event = E_BUZZER_SHORT_SOUND;
    QUEUE_IN(buzzer_queue, event);
}

/**
  * @brief  ena按键双击服务函数
  * @param  null
  * @retval null
  */
void ena_button_double_click_handler(void* btn)
{
    PRINTF("ena double click\r\n");

    uint8_t event = E_BUZZER_LONG_SOUND;
    QUEUE_IN(buzzer_queue, event);
}

/**
  * @brief  ena按键长按服务函数
  * @param  null
  * @retval null
  */
void ena_button_long_press_handler(void* btn)
{
    PRINTF("ena long press\r\n");
}


struct Button btn_enb;

/**
  * @brief  enb按键单击服务函数
  * @param  null
  * @retval null
  */
void enb_button_single_click_handler(void* btn)
{
    PRINTF("enb single click\r\n");

#ifdef NXP_TEST_MODE
    extern void nxp_test_mode_set();
    nxp_test_mode_set();
#endif

    uint8_t event = E_BUZZER_SHORT_SOUND;
    QUEUE_IN(buzzer_queue, event);
    QUEUE_IN(buzzer_queue, event);
}

/**
  * @brief  enb按键双击服务函数
  * @param  null
  * @retval null
  */
void enb_button_double_click_handler(void* btn)
{
    PRINTF("enb double click\r\n");

    uint8_t event = E_BUZZER_LONG_SOUND;
    QUEUE_IN(buzzer_queue, event);
    QUEUE_IN(buzzer_queue, event);
}

/**
  * @brief  enb按键长按服务函数
  * @param  null
  * @retval null
  */
void enb_button_long_press_handler(void* btn)
{
    PRINTF("enb long press\r\n");
}


struct Button btn_sw;

/**
  * @brief  sw按键单击服务函数
  * @param  null
  * @retval null
  */
void sw_button_single_click_handler(void* btn)
{
    uint8_t event = ENB_SINGLE_CLICK;
    QUEUE_IN(button_queue, event);

    PRINTF("sw single click\r\n");
}

/**
  * @brief  sw按键双击服务函数
  * @param  null
  * @retval null
  */
void sw_button_double_click_handler(void* btn)
{
    PRINTF("sw double click\r\n");

    extern uint8_t msd_run_get(void);
    if (FALSE == msd_run_get()) {   /* 由于fm_buff缓存共用所以做个互锁 */
        /* 读取并解析config配置文件 */
        extern void config_file_handle(void);
        config_file_handle();
    }
}

/**
  * @brief  sw按键长按服务函数
  * @param  null
  * @retval null
  */
void sw_button_long_press_handler(void* btn)
{
    PRINTF("sw long press\r\n");

    extern uint8_t msd_run_get(void);
    if (FALSE == msd_run_get()) {   /* 由于fm_buff缓存共用所以做个互锁 */
        if (E_NXP_WAITING == nxp_state_get()) { /* 正在下载，就不处理 */
            /* 将系统当前的配置信息，写到system.txt文件中 */
            extern void system_file_creat(void);
            system_file_creat();

            extern void buzzer_notice_config_ok(void);
            buzzer_notice_config_ok();
        }
    }
}


/**
  * @brief  按键任务初始化
  * @param  null
  * @retval null
  */
void button_scan_init(void)
{
    QUEUE_INIT(button_queue);

    /* ena按键的IO口初始化 */
    ena_button_gpio_config();
    button_init(&btn_ena, ena_button_read, ENA_BUTTON_PRESSED);
    button_attach(&btn_ena, SINGLE_CLICK, ena_button_single_click_handler);
    button_attach(&btn_ena, DOUBLE_CLICK, ena_button_double_click_handler);
    button_attach(&btn_ena, LONG_PRESS_START, ena_button_long_press_handler);
    button_start(&btn_ena);

    /* enb按键的IO口初始化 */
    enb_button_gpio_config();
    button_init(&btn_enb, enb_button_read, ENB_BUTTON_PRESSED);
    button_attach(&btn_enb, SINGLE_CLICK, enb_button_single_click_handler);
    button_attach(&btn_enb, DOUBLE_CLICK, enb_button_double_click_handler);
    button_attach(&btn_enb, LONG_PRESS_START, enb_button_long_press_handler);
    button_start(&btn_enb);

    /* sw按键的IO口初始化 */
    sw_button_gpio_config();
    button_init(&btn_sw, sw_button_read, SW_BUTTON_PRESSED);
    button_attach(&btn_sw, SINGLE_CLICK, sw_button_single_click_handler);
    button_attach(&btn_sw, DOUBLE_CLICK, sw_button_double_click_handler);
    button_attach(&btn_sw, LONG_PRESS_START, sw_button_long_press_handler);
    button_start(&btn_sw);

}


/**
  * @brief  按键扫描的任务，由系统20ms调度一次
  * @param  null
  * @retval null
  */
void button_scan_task(void)
{
    button_ticks();
}


/*********************************************END OF FILE*********************/
