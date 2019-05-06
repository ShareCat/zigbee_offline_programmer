#ifndef __USART_H
#define __USART_H


#include <stdio.h>
#include <stdint.h>
#include "stm32f10x.h"


/**
  * 串口宏定义，不同的串口挂载的总线和IO不一样，移植时需要修改这几个宏
    * 1-修改总线时钟的宏，uart1挂载到apb2总线，其他uart挂载到apb1总线
    * 2-修改GPIO的宏
  */

/* -------------------------------------------------------------------- UART1 */

#define  DOWNLOAD_USARTx                    USART1
#define  DOWNLOAD_USART_CLK                 RCC_APB2Periph_USART1
#define  DOWNLOAD_USART_APBxClkCmd          RCC_APB2PeriphClockCmd
#define  DOWNLOAD_USART_BAUDRATE_1          38400   /* NXP516x复位刚进boot时候波特率 */
#define  DOWNLOAD_USART_BAUDRATE_2          1000000 /* 下载时候的波特率 */

// USART GPIO 引脚宏定义
#define  DOWNLOAD_USART_GPIO_CLK            (RCC_APB2Periph_GPIOA)
#define  DOWNLOAD_USART_GPIO_APBxClkCmd     RCC_APB2PeriphClockCmd

#define  DOWNLOAD_USART_TX_GPIO_PORT        GPIOA
#define  DOWNLOAD_USART_TX_GPIO_PIN         GPIO_Pin_9
#define  DOWNLOAD_USART_RX_GPIO_PORT        GPIOA
#define  DOWNLOAD_USART_RX_GPIO_PIN         GPIO_Pin_10

#define  DOWNLOAD_USART_IRQ                 USART1_IRQn
#define  DOWNLOAD_USART_IRQHandler          USART1_IRQHandler

void download_usart_tx(uint8_t *buff, uint16_t len);
void download_usart_tx_handle(void);
void download_usart_rx_handle(void);
void download_uart_init(uint32_t band);

#define DOWNLOAD_USART_DMA_RX
#define DOWNLOAD_USART_DMA_TX

#define DOWNLOAD_USART_DMA_RX_BUFF_MAX   256
#define DOWNLOAD_USART_DMA_TX_BUFF_MAX   256

void download_usart_dma_tx(uint8_t *buff, uint16_t len);


/* -------------------------------------------------------------------- UART2 */

#define  DEBUG_USARTx                       USART2
#define  DEBUG_USART_CLK                    RCC_APB1Periph_USART2
#define  DEBUG_USART_APBxClkCmd             RCC_APB1PeriphClockCmd
//#define  DEBUG_USART_BAUDRATE               768000
#define  DEBUG_USART_BAUDRATE               2000000U    /* 宏定义不起作用 */

// USART GPIO 引脚宏定义
#define  DEBUG_USART_GPIO_CLK               (RCC_APB2Periph_GPIOA)
#define  DEBUG_USART_GPIO_APBxClkCmd        RCC_APB2PeriphClockCmd
//
#define  DEBUG_USART_TX_GPIO_PORT           GPIOA
#define  DEBUG_USART_TX_GPIO_PIN            GPIO_Pin_2
#define  DEBUG_USART_RX_GPIO_PORT           GPIOA
#define  DEBUG_USART_RX_GPIO_PIN            GPIO_Pin_3

#define  DEBUG_USART_IRQ                    USART2_IRQn
#define  DEBUG_USART_IRQHandler             USART2_IRQHandler

void debug_usart_tx_handle(void);
void debug_usart_rx_handle(void);
void debug_usart_init(uint32_t baud);

/* -------------------------------------------------------------------- UART3 */
#if 0
    #define  PSTN_WIFI_USARTx                   USART3
    #define  PSTN_WIFI_USART_CLK                RCC_APB1Periph_USART3
    #define  PSTN_WIFI_USART_APBxClkCmd         RCC_APB1PeriphClockCmd
    #define  PSTN_WIFI_USART_BAUDRATE           115200

    // USART GPIO 引脚宏定义
    #define  PSTN_WIFI_USART_GPIO_CLK           (RCC_APB2Periph_GPIOB)
    #define  PSTN_WIFI_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd

    #define  PSTN_WIFI_USART_TX_GPIO_PORT       GPIOB
    #define  PSTN_WIFI_USART_TX_GPIO_PIN        GPIO_Pin_10
    #define  PSTN_WIFI_USART_RX_GPIO_PORT       GPIOB
    #define  PSTN_WIFI_USART_RX_GPIO_PIN        GPIO_Pin_11

    #define  PSTN_WIFI_USART_IRQ                USART3_IRQn
    #define  PSTN_WIFI_USART_IRQHandler         USART3_IRQHandler

    void pstn_wifi_usart_tx_handle(void);
    void pstn_wifi_usart_rx_handle(void);
    void pstn_wifi_uart_init(void);
#endif
/* -------------------------------------------------------------------- UART4 */
#if 0
    #define  RF_USARTx                          UART4
    #define  RF_USART_CLK                       RCC_APB1Periph_UART4
    #define  RF_USART_APBxClkCmd                RCC_APB1PeriphClockCmd
    #define  RF_USART_BAUDRATE                  9600

    // USART GPIO 引脚宏定义
    #define  RF_USART_GPIO_CLK                  (RCC_APB2Periph_GPIOC)
    #define  RF_USART_GPIO_APBxClkCmd           RCC_APB2PeriphClockCmd

    #define  RF_USART_TX_GPIO_PORT              GPIOC
    #define  RF_USART_TX_GPIO_PIN               GPIO_Pin_10
    #define  RF_USART_RX_GPIO_PORT              GPIOC
    #define  RF_USART_RX_GPIO_PIN               GPIO_Pin_11

    #define  RF_USART_IRQ                       UART4_IRQn
    #define  RF_USART_IRQHandler                UART4_IRQHandler

    void rf_usart_tx_handle(void);
    void rf_usart_rx_handle(void);
    void rf_uart_init(void);
#endif
/* -------------------------------------------------------------------- UART5 */
#if 0
    #define  DEBUG_USARTx                       USART1
    #define  DEBUG_USART_CLK                    RCC_APB2Periph_USART1
    #define  DEBUG_USART_APBxClkCmd             RCC_APB2PeriphClockCmd
    #define  DEBUG_USART_BAUDRATE               115200

    // USART GPIO 引脚宏定义
    #define  DEBUG_USART_GPIO_CLK               (RCC_APB2Periph_GPIOA)
    #define  DEBUG_USART_GPIO_APBxClkCmd        RCC_APB2PeriphClockCmd

    #define  DEBUG_USART_TX_GPIO_PORT           GPIOA
    #define  DEBUG_USART_TX_GPIO_PIN            GPIO_Pin_9
    #define  DEBUG_USART_RX_GPIO_PORT           GPIOA
    #define  DEBUG_USART_RX_GPIO_PIN            GPIO_Pin_10

    #define  DEBUG_USART_IRQ                    USART1_IRQn
    #define  DEBUG_USART_IRQHandler             USART1_IRQHandler

    void debug_usart_tx_handle(void);
    void debug_usart_rx_handle(void);
    void debug_usart_init(uint32_t baud);
#endif


void usart_send_byte(USART_TypeDef * pUSARTx, uint8_t ch);
void usart_send_array(USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num);
void usart_send_string(USART_TypeDef * pUSARTx, char *str);
void usart_send_half_word(USART_TypeDef * pUSARTx, uint16_t ch);

#endif /* __BSP_USART_H */
