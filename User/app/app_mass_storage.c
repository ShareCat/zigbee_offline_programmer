/**
  ******************************************************************************
  * @file:      app_mass_storage.c
  * @author:    Cat�����ƽ��
  * @version:   V1.0
  * @date:      2018-4-16
  * @brief:     mass_storage
  * @attention: 
  ******************************************************************************
  */


#include "./app_mass_storage.h"

#include "./../sys/sys_scheduler.h"
#include "./../sys/sys_command_line.h"
#include "./../sys/sys_queue.h"

#include "./../../Libraries/Mass_Storage/inc/hw_config.h"
#include "./../../Libraries/Mass_Storage/inc/usb_pwr.h"
#include "./../../Libraries/STM32_USB-FS-Device_Driver/inc/usb_init.h"


#define APP_MASS_STORAGE_DEBUG


enum {
    USB_DISCONNECT,
    USB_CONNECT,
    USB_INIT,
    USB_CHECK,
};

uint8_t usb_state = USB_DISCONNECT;


/**
  * @brief  ͨ������USB��2��ͨѶ����DP-��DP+������ʵ�ֺ͵��Ե����ӶϿ������ͺ���Ҫ��ʱ1S��֤����˳���Ͽ�
  * @param  null
  * @retval null
  */
static void usb_gpio_disconnect(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* USB clock�տ�ʼ�ر� */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, DISABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* USB��2��ͨѶ����DP-��DP+���տ�ʼ���������Ͽ�USB���� */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, GPIO_Pin_11);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, GPIO_Pin_12);
}


/**
  * @brief  �ر�USB
  * @param  ��
  * @retval ��
  */
void disable_usb(void)
{
    usb_gpio_disconnect();
    USB_Cable_Config(DISABLE);
}


/**
  * @brief  
  * @param  ��
  * @retval ��
  */
void mass_storage_init(void)
{
    /* ��ʼ��MAL */
    Set_System();
}


/**
  * @brief  
  * @param  null
  * @retval null
  */
void mass_storage_task(void)
{
    switch (usb_state) {
        case USB_DISCONNECT: {
            disable_usb();
            usb_state = USB_CONNECT;
            os_timer_set(T_1S);
        }
        break;

        case USB_CONNECT: {
            if (!os_timer_check()) {
                USB_Cable_Config(ENABLE);
                usb_state = USB_INIT;
                os_timer_set(T_200MS);
            }
        }
        break;

        case USB_INIT: {
            if (!os_timer_check()) {
                /* ����USBʱ��Ϊ48M */
                Set_USBClock();

                /* ����USB�ж�(����SDIO�ж�) */
                USB_Interrupts_Config();

                /* USB��ʼ�� */
                USB_Init();

                usb_state = USB_CHECK;
            }
        }
        break;

        case USB_CHECK: {
            static uint8_t timer = 0;

            if (bDeviceState != CONFIGURED) {
                if (++timer > 100) {    /* 10s��USB��û��ö����� */
                    usb_state = USB_DISCONNECT;
                    timer = 0;
                }
            } else {
                timer = 0;
            }
        }
        break;

        default: {
            usb_state = USB_DISCONNECT;
        }
        break;
    }
}


/*********************************************END OF FILE**********************/
