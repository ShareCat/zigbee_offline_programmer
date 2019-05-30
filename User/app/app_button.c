/**
  ******************************************************************************
  * @file:      app_button.c
  * @author:    Cat�����ƽ��
  * @version:   V1.0
  * @date:      2018-3-3
  * @brief:     ��������
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
  * @brief  ena��������������
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
  * @brief  ena����˫��������
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
  * @brief  ena��������������
  * @param  null
  * @retval null
  */
void ena_button_long_press_handler(void* btn)
{
    PRINTF("ena long press\r\n");
}


struct Button btn_enb;

/**
  * @brief  enb��������������
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
  * @brief  enb����˫��������
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
  * @brief  enb��������������
  * @param  null
  * @retval null
  */
void enb_button_long_press_handler(void* btn)
{
    PRINTF("enb long press\r\n");
}


struct Button btn_sw;

/**
  * @brief  sw��������������
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
  * @brief  sw����˫��������
  * @param  null
  * @retval null
  */
void sw_button_double_click_handler(void* btn)
{
    PRINTF("sw double click\r\n");

    extern uint8_t msd_run_get(void);
    if (FALSE == msd_run_get()) {   /* ����fm_buff���湲�������������� */
        /* ��ȡ������config�����ļ� */
        extern void config_file_handle(void);
        config_file_handle();
    }
}

/**
  * @brief  sw��������������
  * @param  null
  * @retval null
  */
void sw_button_long_press_handler(void* btn)
{
    PRINTF("sw long press\r\n");

    extern uint8_t msd_run_get(void);
    if (FALSE == msd_run_get()) {   /* ����fm_buff���湲�������������� */
        if (E_NXP_WAITING == nxp_state_get()) { /* �������أ��Ͳ����� */
            /* ��ϵͳ��ǰ��������Ϣ��д��system.txt�ļ��� */
            extern void system_file_creat(void);
            system_file_creat();

            extern void buzzer_notice_config_ok(void);
            buzzer_notice_config_ok();
        }
    }
}


/**
  * @brief  ���������ʼ��
  * @param  null
  * @retval null
  */
void button_scan_init(void)
{
    QUEUE_INIT(button_queue);

    /* ena������IO�ڳ�ʼ�� */
    ena_button_gpio_config();
    button_init(&btn_ena, ena_button_read, ENA_BUTTON_PRESSED);
    button_attach(&btn_ena, SINGLE_CLICK, ena_button_single_click_handler);
    button_attach(&btn_ena, DOUBLE_CLICK, ena_button_double_click_handler);
    button_attach(&btn_ena, LONG_PRESS_START, ena_button_long_press_handler);
    button_start(&btn_ena);

    /* enb������IO�ڳ�ʼ�� */
    enb_button_gpio_config();
    button_init(&btn_enb, enb_button_read, ENB_BUTTON_PRESSED);
    button_attach(&btn_enb, SINGLE_CLICK, enb_button_single_click_handler);
    button_attach(&btn_enb, DOUBLE_CLICK, enb_button_double_click_handler);
    button_attach(&btn_enb, LONG_PRESS_START, enb_button_long_press_handler);
    button_start(&btn_enb);

    /* sw������IO�ڳ�ʼ�� */
    sw_button_gpio_config();
    button_init(&btn_sw, sw_button_read, SW_BUTTON_PRESSED);
    button_attach(&btn_sw, SINGLE_CLICK, sw_button_single_click_handler);
    button_attach(&btn_sw, DOUBLE_CLICK, sw_button_double_click_handler);
    button_attach(&btn_sw, LONG_PRESS_START, sw_button_long_press_handler);
    button_start(&btn_sw);

}


/**
  * @brief  ����ɨ���������ϵͳ20ms����һ��
  * @param  null
  * @retval null
  */
void button_scan_task(void)
{
    button_ticks();
}


/*********************************************END OF FILE*********************/
