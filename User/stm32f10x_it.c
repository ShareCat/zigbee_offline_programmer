/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTI
  
  AL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "./bsp/bsp_systick.h"
#include "./bsp/bsp_usart.h"

#include "usb_lib.h"
#include "usb_istr.h"

#include "./project_conf.h"


/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

#ifndef USING_CM_BACKTRACE  /* 如果使用cm_backtrace，则HardFault_Handler函数会在cmb_fault.S汇编中实现 */
/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
__weak void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
#endif

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    systick_irq();
}

/* 串口中断服务函数 */
void DOWNLOAD_USART_IRQHandler(void)
{
    //download_usart_rx_handle();
    //download_usart_tx_handle();

    extern void download_usart_dma_rx_handle(void);
    download_usart_dma_rx_handle();

    void nxp_download_other_pack(void);
    nxp_download_other_pack();
}

void DMA1_Channel4_IRQHandler(void)
{
    extern void download_usart_dma_tx_handle(void);
    download_usart_dma_tx_handle();
}

#if 0/* USART_DMA接收任意长度的数据，必须使用USART_RX的中断，因为DMA_RX的中断只能接收固定长度的数据才会有中断产生 */
void DMA1_Channel5_IRQHandler(void)
{
    extern void download_usart_dma_rx_handle(void);
    download_usart_dma_rx_handle();
}
#endif

/* 串口中断服务函数 */
void GSM_WIFI_USART_IRQHandler(void)
{
#if 0
    gsm_wifi_usart_rx_handle();
    gsm_wifi_usart_tx_handle();
#endif
}

/* 串口中断服务函数 */
void PSTN_WIFI_USART_IRQHandler(void)
{
#if 0
    pstn_wifi_usart_rx_handle();
    pstn_wifi_usart_tx_handle();
#endif
}

/* 串口中断服务函数 */
void RF_USART_IRQHandler(void)
{
#if 0
    rf_usart_rx_handle();
    rf_usart_tx_handle();
#endif
}

/* 串口中断服务函数 */
void DEBUG_USART_IRQHandler(void)
{
    debug_usart_rx_handle();
    debug_usart_tx_handle();
}

/* 定时器中断服务函数 */
void BASIC_TIM_IRQHandler(void)
{
#if 0
    extern soft_uart VoiUart;

    if (TIM_GetITStatus(S_UART_TIMER, TIM_IT_Update) != RESET) {
        S_UART_Timer_Handler(&VoiUart);
        TIM_ClearITPendingBit(S_UART_TIMER, TIM_FLAG_Update);
    }
#endif
}

/*
 * 函数名：USB_LP_CAN1_RX0_IRQHandler
 * 描述  ：USB低优先级中断请求
 * 输入  ：无
 * 输出  ：无
 */
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    USB_Istr();
}

/*
 * 函数名：USB_HP_CAN1_TX_IRQHandler
 * 描述  ：USB高优先级中断请求 或发送中断
 * 输入  ：无
 * 输出  ：无
 */
void USB_HP_CAN1_TX_IRQHandler(void)
{
    CTR_HP();
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
