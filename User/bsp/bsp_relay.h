#ifndef __BSP_RELAY_H
#define __BSP_RELAY_H

#include "global.h"



enum {
    E_RELAY_1,
    E_RELAY_2,
    E_RELAY_3,
    E_RELAY_4,
    E_RELAY_5,
    E_RELAY_6,
    E_RELAY_7,
    E_RELAY_8,
    E_RELAY_9,
    E_RELAY_10,
    E_RELAY_11,
    E_RELAY_12,
    E_RELAY_13,
    E_RELAY_14,
    E_RELAY_15,
    E_RELAY_16,
    E_RELAY_17,
    E_RELAY_18,
    E_RELAY_19,
    E_RELAY_SW,
    E_RELAY_KEY,
    E_RELAY_IR,
    E_RELAY_SMOKE,
    E_S2000_SEL1,
    E_S2000_SEL2,
    E_S2000_SEL3,
    E_S2000_SEL4,
    E_S2000_SEL5,
    E_S2000_SEL6,
    E_S2000_SEL7,
    E_S2000_SEL8
};


//Òý½Å¶¨Òå
/************************************************************/

/* RELAY_1 */
#define RELAY_1_PIN         GPIO_Pin_13             /* PD13 */
#define RELAY_1_PORT        GPIOD
#define RELAY_1_CLK         RCC_APB2Periph_GPIOD

/* RELAY_2 */
#define RELAY_2_PIN         GPIO_Pin_14             /* PD14 */
#define RELAY_2_PORT        GPIOD
#define RELAY_2_CLK         RCC_APB2Periph_GPIOD

/* RELAY_3 */
#define RELAY_3_PIN         GPIO_Pin_15             /* PD15 */
#define RELAY_3_PORT        GPIOD
#define RELAY_3_CLK         RCC_APB2Periph_GPIOD

/* RELAY_4 */
#define RELAY_4_PIN         GPIO_Pin_11             /* PD11 */
#define RELAY_4_PORT        GPIOD
#define RELAY_4_CLK         RCC_APB2Periph_GPIOD

/* RELAY_5 */
#define RELAY_5_PIN         GPIO_Pin_10             /* PD10 */
#define RELAY_5_PORT        GPIOD
#define RELAY_5_CLK         RCC_APB2Periph_GPIOD

/* RELAY_6 */
#define RELAY_6_PIN         GPIO_Pin_9              /* PD9 */
#define RELAY_6_PORT        GPIOD
#define RELAY_6_CLK         RCC_APB2Periph_GPIOD

/* RELAY_7 */
#define RELAY_7_PIN         GPIO_Pin_8              /* PD8 */
#define RELAY_7_PORT        GPIOD
#define RELAY_7_CLK         RCC_APB2Periph_GPIOD

/* RELAY_8 */
#define RELAY_8_PIN         GPIO_Pin_15             /* PB15 */
#define RELAY_8_PORT        GPIOB
#define RELAY_8_CLK         RCC_APB2Periph_GPIOB

/* RELAY_9 */
#define RELAY_9_PIN         GPIO_Pin_14             /* PB14 */
#define RELAY_9_PORT        GPIOB
#define RELAY_9_CLK         RCC_APB2Periph_GPIOB

/* RELAY_10 */
#define RELAY_10_PIN        GPIO_Pin_13             /* PB13 */
#define RELAY_10_PORT       GPIOB
#define RELAY_10_CLK        RCC_APB2Periph_GPIOB

/* RELAY_11 */
#define RELAY_11_PIN        GPIO_Pin_15             /* PE15 */
#define RELAY_11_PORT       GPIOE
#define RELAY_11_CLK        RCC_APB2Periph_GPIOE

/* RELAY_12 */
#define RELAY_12_PIN        GPIO_Pin_6              /* PC6 */
#define RELAY_12_PORT       GPIOC
#define RELAY_12_CLK        RCC_APB2Periph_GPIOC

/* RELAY_13 */
#define RELAY_13_PIN        GPIO_Pin_7              /* PC7 */
#define RELAY_13_PORT       GPIOC
#define RELAY_13_CLK        RCC_APB2Periph_GPIOC

/* RELAY_14 */
#define RELAY_14_PIN        GPIO_Pin_8              /* PC8 */
#define RELAY_14_PORT       GPIOC
#define RELAY_14_CLK        RCC_APB2Periph_GPIOC

/* RELAY_15 */
#define RELAY_15_PIN        GPIO_Pin_9              /* PC9 */
#define RELAY_15_PORT       GPIOC
#define RELAY_15_CLK        RCC_APB2Periph_GPIOC

/* RELAY_16 */
#define RELAY_16_PIN        GPIO_Pin_8              /* PA8 */
#define RELAY_16_PORT       GPIOA
#define RELAY_16_CLK        RCC_APB2Periph_GPIOA

/* RELAY_17 */
#define RELAY_17_PIN        GPIO_Pin_11            /* PA11 */
#define RELAY_17_PORT       GPIOA
#define RELAY_17_CLK        RCC_APB2Periph_GPIOA

/* RELAY_18 */
#define RELAY_18_PIN        GPIO_Pin_12             /* PA12 */
#define RELAY_18_PORT       GPIOA
#define RELAY_18_CLK        RCC_APB2Periph_GPIOA

/* RELAY_19 */
#define RELAY_19_PIN        GPIO_Pin_15             /* PA15 */
#define RELAY_19_PORT       GPIOA
#define RELAY_19_CLK        RCC_APB2Periph_GPIOA

/* RELAY_SW */
#define RELAY_SW_PIN        GPIO_Pin_12             /* PD12 */
#define RELAY_SW_PORT       GPIOD
#define RELAY_SW_CLK        RCC_APB2Periph_GPIOD

/* RELAY_KEY */
#define RELAY_KEY_PIN       GPIO_Pin_0              /* PD0 */
#define RELAY_KEY_PORT      GPIOD
#define RELAY_KEY_CLK       RCC_APB2Periph_GPIOD

/* RELAY_IR */
#define RELAY_IR_PIN        GPIO_Pin_1              /* PD1 */
#define RELAY_IR_PORT       GPIOD
#define RELAY_IR_CLK        RCC_APB2Periph_GPIOD

/* RELAY_SMOKE */
#define RELAY_SMOKE_PIN     GPIO_Pin_2              /* PD2 */
#define RELAY_SMOKE_PORT    GPIOD
#define RELAY_SMOKE_CLK     RCC_APB2Periph_GPIOD

/* S2000_SEL1 */
#define S2000_SEL1_PIN      GPIO_Pin_3              /* PD3 */
#define S2000_SEL1_PORT     GPIOD
#define S2000_SEL1_CLK      RCC_APB2Periph_GPIOD

/* S2000_SEL2 */
#define S2000_SEL2_PIN      GPIO_Pin_4              /* PD4 */
#define S2000_SEL2_PORT     GPIOD
#define S2000_SEL2_CLK      RCC_APB2Periph_GPIOD

/* S2000_SEL3 */
#define S2000_SEL3_PIN      GPIO_Pin_7              /* PD7 */
#define S2000_SEL3_PORT     GPIOD
#define S2000_SEL3_CLK      RCC_APB2Periph_GPIOD

/* S2000_SEL4 */
#define S2000_SEL4_PIN      GPIO_Pin_3              /* PB3 */
#define S2000_SEL4_PORT     GPIOB
#define S2000_SEL4_CLK      RCC_APB2Periph_GPIOB

/* S2000_SEL5 */
#define S2000_SEL5_PIN      GPIO_Pin_4              /* PB4 */
#define S2000_SEL5_PORT     GPIOB
#define S2000_SEL5_CLK      RCC_APB2Periph_GPIOB

/* S2000_SEL6 */
#define S2000_SEL6_PIN      GPIO_Pin_5              /* PB5 */
#define S2000_SEL6_PORT     GPIOB
#define S2000_SEL6_CLK      RCC_APB2Periph_GPIOB

/* S2000_SEL7 */
#define S2000_SEL7_PIN      GPIO_Pin_6              /* PB6 */
#define S2000_SEL7_PORT     GPIOB
#define S2000_SEL7_CLK      RCC_APB2Periph_GPIOB

/* S2000_SEL8 */
#define S2000_SEL8_PIN      GPIO_Pin_7              /* PB7 */
#define S2000_SEL8_PORT     GPIOB
#define S2000_SEL8_CLK      RCC_APB2Periph_GPIOB


extern void relay_gpio_init(void);
extern void relay_ctrl(uint8_t relay, FunctionalState cmd);


#endif /* __BSP_RELAY_H */
