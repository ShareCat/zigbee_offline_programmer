/**
  ******************************************************************************
  * @file:      app_mass_storage.c
  * @author:    Cat（孙关平）
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
  * @brief  通过控制USB的2个通讯引脚DP-和DP+拉低来实现和电脑的连接断开，拉低后需要延时1S保证连接顺利断开
  * @param  null
  * @retval null
  */
static void usb_gpio_disconnect(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* USB clock刚开始关闭 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, DISABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* USB的2个通讯引脚DP-和DP+，刚开始都拉低来断开USB连接 */
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
  * @brief  关闭USB
  * @param  无
  * @retval 无
  */
void disable_usb(void)
{
    usb_gpio_disconnect();
    USB_Cable_Config(DISABLE);
}


/**
  * @brief  
  * @param  无
  * @retval 无
  */
void mass_storage_init(void)
{
    /* 初始化MAL */
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
                /* 设置USB时钟为48M */
                Set_USBClock();

                /* 配置USB中断(包括SDIO中断) */
                USB_Interrupts_Config();

                /* USB初始化 */
                USB_Init();

                usb_state = USB_CHECK;
            }
        }
        break;

        case USB_CHECK: {
            static uint8_t timer = 0;

            if (bDeviceState != CONFIGURED) {
                if (++timer > 100) {    /* 10s后，USB还没有枚举完成 */
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
