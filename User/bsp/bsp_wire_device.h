#ifndef __BSP_WIRE_DEVICE_H
#define __BSP_WIRE_DEVICE_H


#include "global.h"



/* ADC1和DMA配置 ------------------------------------------------------ begin */

/* RELAY_1 */
#define HCF4051_S0_PIN              GPIO_Pin_7              /* PE7 */
#define HCF4051_S0_PORT             GPIOE
#define HCF4051_S0_CLK              RCC_APB2Periph_GPIOE

#define HCF4051_S1_PIN              GPIO_Pin_8              /* PE8 */
#define HCF4051_S1_PORT             GPIOE
#define HCF4051_S1_CLK              RCC_APB2Periph_GPIOE

#define HCF4051_S2_PIN              GPIO_Pin_9              /* PE9 */
#define HCF4051_S2_PORT             GPIOE
#define HCF4051_S2_CLK              RCC_APB2Periph_GPIOE


/* 注意：用作ADC采集的IO必须没有复用，否则采集电压会有影响 */
#define ADC_APBxClock_FUN           RCC_APB2PeriphClockCmd
#define ADC_CLK                     RCC_APB2Periph_ADC1

#define ADC_GPIO_APBxClock_FUN      RCC_APB2PeriphClockCmd
#define ADC1_8_ADC_GPIO_CLK         RCC_APB2Periph_GPIOA
#define ADC1_8_ADC_PORT             GPIOA

#define ADC9_10_ADC_GPIO_CLK        RCC_APB2Periph_GPIOB
#define ADC9_10_ADC_PORT            GPIOB

#define ADC11_16_ADC_GPIO_CLK       RCC_APB2Periph_GPIOC
#define ADC11_16_ADC_PORT           GPIOC


enum {
    E_HARDWARE_ADC1,                /* ADC_Channel_0 */
    E_HARDWARE_ADC2,                /* ADC_Channel_1 */
    E_HARDWARE_ADC3,                /* ADC_Channel_2 */
    E_HARDWARE_ADC4,                /* ADC_Channel_3 */
    E_HARDWARE_ADC5,                /* ADC_Channel_4 */
    E_HARDWARE_ADC6,                /* ADC_Channel_5 */
    E_HARDWARE_ADC7,                /* ADC_Channel_6 */
    E_HARDWARE_ADC8,                /* ADC_Channel_7 */
    E_HARDWARE_ADC9,                /* ADC_Channel_8 */
    E_HARDWARE_ADC10,               /* ADC_Channel_9 */
    E_HARDWARE_ADC11,               /* ADC_Channel_10 */
    E_HARDWARE_ADC12,               /* ADC_Channel_11 */
    E_HARDWARE_ADC13,               /* ADC_Channel_12 */
    E_HARDWARE_ADC14,               /* ADC_Channel_13 */
    E_HARDWARE_ADC15,               /* ADC_Channel_14，这个通道通过HCF4051来采集多个ADC */
    E_HARDWARE_ADC16,               /* ADC_Channel_15，热敏电阻采集 */
    E_ADC_CHANNEL_MAX,              /* 硬件总共转换通道个数 */

    /* 以下是通过HCF4051单选接到ADC_Channel_14采集，也就是说，同一时间只能采集一路 */
    E_HCF4051_ADC17,
    E_HCF4051_ADC18,
    E_HCF4051_ADC19,
    E_HCF4051_ADC20,
    E_HCF4051_ADC21,
    E_HCF4051_ADC22,
    E_HCF4051_ADC23,
    E_HCF4051_ADC24,
    E_HCF4051_CHANNEL_MAX = 8
};

#define ADC1_ADC_PIN                GPIO_Pin_0      /* PA0对应ADC_CH0 */
#define ADC1_ADC_CHANNEL            ADC_Channel_0

#define ADC2_ADC_PIN                GPIO_Pin_1      /* PA1对应ADC_CH1 */
#define ADC2_ADC_CHANNEL            ADC_Channel_1

#define ADC3_ADC_PIN                GPIO_Pin_2      /* PA2对应ADC_CH2 */
#define ADC3_ADC_CHANNEL            ADC_Channel_2

#define ADC4_ADC_PIN                GPIO_Pin_3      /* PA3对应ADC_CH3 */
#define ADC4_ADC_CHANNEL            ADC_Channel_3

#define ADC5_ADC_PIN                GPIO_Pin_4      /* PA4对应ADC_CH4 */
#define ADC5_ADC_CHANNEL            ADC_Channel_4

#define ADC6_ADC_PIN                GPIO_Pin_5      /* PA5对应ADC_CH5 */
#define ADC6_ADC_CHANNEL            ADC_Channel_5

#define ADC7_ADC_PIN                GPIO_Pin_6      /* PA6对应ADC_CH6 */
#define ADC7_ADC_CHANNEL            ADC_Channel_6

#define ADC8_ADC_PIN                GPIO_Pin_7      /* PA7对应ADC_CH7 */
#define ADC8_ADC_CHANNEL            ADC_Channel_7

#define ADC9_ADC_PIN                GPIO_Pin_0      /* PB0对应ADC_CH8 */
#define ADC9_ADC_CHANNEL            ADC_Channel_8

#define ADC10_ADC_PIN               GPIO_Pin_1      /* PB1对应ADC_CH9 */
#define ADC10_ADC_CHANNEL           ADC_Channel_9

#define ADC11_ADC_PIN               GPIO_Pin_0      /* PC0对应ADC_CH10 */
#define ADC11_ADC_CHANNEL           ADC_Channel_10

#define ADC12_ADC_PIN               GPIO_Pin_1      /* PC1对应ADC_CH11 */
#define ADC12_ADC_CHANNEL           ADC_Channel_11

#define ADC13_ADC_PIN               GPIO_Pin_2      /* PC2对应ADC_CH12 */
#define ADC13_ADC_CHANNEL           ADC_Channel_12

#define ADC14_ADC_PIN               GPIO_Pin_3      /* PC3对应ADC_CH13 */
#define ADC14_ADC_CHANNEL           ADC_Channel_13

#define ADC15_ADC_PIN               GPIO_Pin_4      /* PC4对应ADC_CH14 */
#define ADC15_ADC_CHANNEL           ADC_Channel_14

#define ADC16_ADC_PIN               GPIO_Pin_5      /* PC5对应ADC_CH15 */
#define ADC16_ADC_CHANNEL           ADC_Channel_15


/* ADC1 对应 DMA1通道1，ADC3对应DMA2通道5，ADC2没有DMA功能 */
#define ADC_x                       ADC1            /* ADC通道 */
#define ADC_DMA_CHANNEL             DMA1_Channel1   /* DMA通道 */
#define ADC_DMA_CLK                 RCC_AHBPeriph_DMA1

extern volatile uint16_t adc_value[E_ADC_CHANNEL_MAX];
extern volatile uint16_t hcf4051_ch_value[E_HCF4051_CHANNEL_MAX];
extern uint8_t hcf4051_ch;

extern void adc_hcf4051_gpio_config(void);
extern void adc_hcf4051_ctrl(uint8_t ch);
extern void adc_gpio_config(void);
extern void adc_dma_config(void);
/* ADC1和DMA配置 -------------------------------------------------------- end */


#endif /* __BSP_WIRE_DEVICE_H */

