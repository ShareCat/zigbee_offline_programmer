#ifndef __BSP_WIRE_DEVICE_H
#define __BSP_WIRE_DEVICE_H


#include "global.h"



/* ADC1��DMA���� ------------------------------------------------------ begin */

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


/* ע�⣺����ADC�ɼ���IO����û�и��ã�����ɼ���ѹ����Ӱ�� */
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
    E_HARDWARE_ADC15,               /* ADC_Channel_14�����ͨ��ͨ��HCF4051���ɼ����ADC */
    E_HARDWARE_ADC16,               /* ADC_Channel_15����������ɼ� */
    E_ADC_CHANNEL_MAX,              /* Ӳ���ܹ�ת��ͨ������ */

    /* ������ͨ��HCF4051��ѡ�ӵ�ADC_Channel_14�ɼ���Ҳ����˵��ͬһʱ��ֻ�ܲɼ�һ· */
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

#define ADC1_ADC_PIN                GPIO_Pin_0      /* PA0��ӦADC_CH0 */
#define ADC1_ADC_CHANNEL            ADC_Channel_0

#define ADC2_ADC_PIN                GPIO_Pin_1      /* PA1��ӦADC_CH1 */
#define ADC2_ADC_CHANNEL            ADC_Channel_1

#define ADC3_ADC_PIN                GPIO_Pin_2      /* PA2��ӦADC_CH2 */
#define ADC3_ADC_CHANNEL            ADC_Channel_2

#define ADC4_ADC_PIN                GPIO_Pin_3      /* PA3��ӦADC_CH3 */
#define ADC4_ADC_CHANNEL            ADC_Channel_3

#define ADC5_ADC_PIN                GPIO_Pin_4      /* PA4��ӦADC_CH4 */
#define ADC5_ADC_CHANNEL            ADC_Channel_4

#define ADC6_ADC_PIN                GPIO_Pin_5      /* PA5��ӦADC_CH5 */
#define ADC6_ADC_CHANNEL            ADC_Channel_5

#define ADC7_ADC_PIN                GPIO_Pin_6      /* PA6��ӦADC_CH6 */
#define ADC7_ADC_CHANNEL            ADC_Channel_6

#define ADC8_ADC_PIN                GPIO_Pin_7      /* PA7��ӦADC_CH7 */
#define ADC8_ADC_CHANNEL            ADC_Channel_7

#define ADC9_ADC_PIN                GPIO_Pin_0      /* PB0��ӦADC_CH8 */
#define ADC9_ADC_CHANNEL            ADC_Channel_8

#define ADC10_ADC_PIN               GPIO_Pin_1      /* PB1��ӦADC_CH9 */
#define ADC10_ADC_CHANNEL           ADC_Channel_9

#define ADC11_ADC_PIN               GPIO_Pin_0      /* PC0��ӦADC_CH10 */
#define ADC11_ADC_CHANNEL           ADC_Channel_10

#define ADC12_ADC_PIN               GPIO_Pin_1      /* PC1��ӦADC_CH11 */
#define ADC12_ADC_CHANNEL           ADC_Channel_11

#define ADC13_ADC_PIN               GPIO_Pin_2      /* PC2��ӦADC_CH12 */
#define ADC13_ADC_CHANNEL           ADC_Channel_12

#define ADC14_ADC_PIN               GPIO_Pin_3      /* PC3��ӦADC_CH13 */
#define ADC14_ADC_CHANNEL           ADC_Channel_13

#define ADC15_ADC_PIN               GPIO_Pin_4      /* PC4��ӦADC_CH14 */
#define ADC15_ADC_CHANNEL           ADC_Channel_14

#define ADC16_ADC_PIN               GPIO_Pin_5      /* PC5��ӦADC_CH15 */
#define ADC16_ADC_CHANNEL           ADC_Channel_15


/* ADC1 ��Ӧ DMA1ͨ��1��ADC3��ӦDMA2ͨ��5��ADC2û��DMA���� */
#define ADC_x                       ADC1            /* ADCͨ�� */
#define ADC_DMA_CHANNEL             DMA1_Channel1   /* DMAͨ�� */
#define ADC_DMA_CLK                 RCC_AHBPeriph_DMA1

extern volatile uint16_t adc_value[E_ADC_CHANNEL_MAX];
extern volatile uint16_t hcf4051_ch_value[E_HCF4051_CHANNEL_MAX];
extern uint8_t hcf4051_ch;

extern void adc_hcf4051_gpio_config(void);
extern void adc_hcf4051_ctrl(uint8_t ch);
extern void adc_gpio_config(void);
extern void adc_dma_config(void);
/* ADC1��DMA���� -------------------------------------------------------- end */


#endif /* __BSP_WIRE_DEVICE_H */

