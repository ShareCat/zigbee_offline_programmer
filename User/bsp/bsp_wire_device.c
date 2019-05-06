/**
  ******************************************************************************
  * @file:
  * @author:    Cat
  * @version:   V1.0
  * @date:      2018-3-23
  * @brief:     包含有线防区、有线警号、有线PGM的驱动
  * @attention:
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "bsp_wire_device.h"
#include "./../sys/sys_command_line.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/* ADC1和DMA配置 ------------------------------------------------------ begin */

volatile uint16_t adc_value[E_ADC_CHANNEL_MAX]; /* ADC采集结果缓存，右对齐 */
volatile uint16_t hcf4051_ch_value[E_HCF4051_CHANNEL_MAX];
uint8_t hcf4051_ch = E_HCF4051_ADC17;

void adc_hcf4051_gpio_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /*开启GPIO外设时钟*/
    RCC_APB2PeriphClockCmd(HCF4051_S0_CLK |
                           HCF4051_S1_CLK |
                           HCF4051_S2_CLK, ENABLE);

    /* HCF4051_S0 */
    GPIO_InitStructure.GPIO_Pin = HCF4051_S0_PIN;       /* 选择要控制的GPIO引脚 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    /* 模式为通用推挽输出 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   /* 速率为50MHz */
    GPIO_Init(HCF4051_S0_PORT, &GPIO_InitStructure);    /* 调库函数，初始化GPIO */
    GPIO_ResetBits(HCF4051_S0_PORT, HCF4051_S0_PIN);

    /* HCF4051_S1 */
    GPIO_InitStructure.GPIO_Pin = HCF4051_S1_PIN;       /* 选择要控制的GPIO引脚 */
    GPIO_Init(HCF4051_S1_PORT, &GPIO_InitStructure);    /* 调库函数，初始化GPIO */
    GPIO_ResetBits(HCF4051_S1_PORT, HCF4051_S1_PIN);

    /* HCF4051_S2 */
    GPIO_InitStructure.GPIO_Pin = HCF4051_S2_PIN;       /* 选择要控制的GPIO引脚 */
    GPIO_Init(HCF4051_S2_PORT, &GPIO_InitStructure);    /* 调库函数，初始化GPIO */
    GPIO_ResetBits(HCF4051_S2_PORT, HCF4051_S2_PIN);

    adc_hcf4051_ctrl(E_HCF4051_ADC17);                  /* 上电默认先选择一个通道 */
}


/**
  * @brief  控制HCF4051来选择E_HCF4051_ADC15到E_HCF4051_ADC22接到ADC_Channel_14采集
  * @param  E_HCF4051_ADC15到E_HCF4051_ADC22
  * @retval 无
  */
void adc_hcf4051_ctrl(uint8_t ch)
{
    hcf4051_ch = ch;

    switch(hcf4051_ch) {
        case E_HCF4051_ADC17: {
            GPIO_ResetBits(HCF4051_S0_PORT, HCF4051_S0_PIN);
            GPIO_ResetBits(HCF4051_S1_PORT, HCF4051_S1_PIN);
            GPIO_ResetBits(HCF4051_S2_PORT, HCF4051_S2_PIN);
        }
        break;

        case E_HCF4051_ADC18: {
            GPIO_SetBits(HCF4051_S0_PORT, HCF4051_S0_PIN);
            GPIO_ResetBits(HCF4051_S1_PORT, HCF4051_S1_PIN);
            GPIO_ResetBits(HCF4051_S2_PORT, HCF4051_S2_PIN);
        }
        break;

        case E_HCF4051_ADC19: {
            GPIO_ResetBits(HCF4051_S0_PORT, HCF4051_S0_PIN);
            GPIO_SetBits(HCF4051_S1_PORT, HCF4051_S1_PIN);
            GPIO_ResetBits(HCF4051_S2_PORT, HCF4051_S2_PIN);
        }
        break;

        case E_HCF4051_ADC20: {
            GPIO_SetBits(HCF4051_S0_PORT, HCF4051_S0_PIN);
            GPIO_SetBits(HCF4051_S1_PORT, HCF4051_S1_PIN);
            GPIO_ResetBits(HCF4051_S2_PORT, HCF4051_S2_PIN);
        }
        break;

        case E_HCF4051_ADC21: {
            GPIO_ResetBits(HCF4051_S0_PORT, HCF4051_S0_PIN);
            GPIO_ResetBits(HCF4051_S1_PORT, HCF4051_S1_PIN);
            GPIO_SetBits(HCF4051_S2_PORT, HCF4051_S2_PIN);
        }
        break;

        case E_HCF4051_ADC22: {
            GPIO_SetBits(HCF4051_S0_PORT, HCF4051_S0_PIN);
            GPIO_ResetBits(HCF4051_S1_PORT, HCF4051_S1_PIN);
            GPIO_SetBits(HCF4051_S2_PORT, HCF4051_S2_PIN);
        }
        break;

        case E_HCF4051_ADC23: {
            GPIO_ResetBits(HCF4051_S0_PORT, HCF4051_S0_PIN);
            GPIO_SetBits(HCF4051_S1_PORT, HCF4051_S1_PIN);
            GPIO_SetBits(HCF4051_S2_PORT, HCF4051_S2_PIN);
        }
        break;

        case E_HCF4051_ADC24: {
            GPIO_SetBits(HCF4051_S0_PORT, HCF4051_S0_PIN);
            GPIO_SetBits(HCF4051_S1_PORT, HCF4051_S1_PIN);
            GPIO_SetBits(HCF4051_S2_PORT, HCF4051_S2_PIN);
        }
        break;

        default: {
            GPIO_ResetBits(HCF4051_S0_PORT, HCF4051_S0_PIN);
            GPIO_ResetBits(HCF4051_S1_PORT, HCF4051_S1_PIN);
            GPIO_ResetBits(HCF4051_S2_PORT, HCF4051_S2_PIN);
            hcf4051_ch = E_HCF4051_ADC17;
        }
        break;
    }
}


/**
  * @brief  有线防区和DC、AC检测IO口ADC初始化
  * @param  无
  * @retval 无
  */
void adc_gpio_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 打开 ADC IO端口时钟 */
    ADC_GPIO_APBxClock_FUN(ADC1_8_ADC_GPIO_CLK |
                           ADC9_10_ADC_GPIO_CLK |
                           ADC11_16_ADC_GPIO_CLK, ENABLE);

    /* 配置 ADC IO 引脚模式 */
    GPIO_InitStructure.GPIO_Pin = ADC1_ADC_PIN | ADC2_ADC_PIN |
                                  ADC3_ADC_PIN | ADC4_ADC_PIN |
                                  ADC5_ADC_PIN | ADC6_ADC_PIN |
                                  ADC7_ADC_PIN | ADC8_ADC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(ADC1_8_ADC_PORT, &GPIO_InitStructure);    /* 初始化 ADC IO */

    /* 配置 ADC IO 引脚模式 */
    GPIO_InitStructure.GPIO_Pin = ADC9_ADC_PIN | ADC10_ADC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(ADC9_10_ADC_PORT, &GPIO_InitStructure);   /* 初始化 ADC IO */

    /* 配置 ADC IO 引脚模式 */
    GPIO_InitStructure.GPIO_Pin = ADC11_ADC_PIN | ADC12_ADC_PIN |
                                  ADC13_ADC_PIN | ADC14_ADC_PIN |
                                  ADC15_ADC_PIN | ADC16_ADC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(ADC11_16_ADC_PORT, &GPIO_InitStructure);  /* 初始化 ADC IO */
}


/**
  * @brief  配置ADC1和DMA，并开启连续转换模式
  * @param  无
  * @retval 无
  */
void adc_dma_config(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    /* 打开DMA时钟 */
    RCC_AHBPeriphClockCmd(ADC_DMA_CLK, ENABLE);
    /* 打开ADC时钟 */
    ADC_APBxClock_FUN(ADC_CLK, ENABLE);

    /* 复位DMA控制器 */
    DMA_DeInit(ADC_DMA_CHANNEL);

    /* 配置 DMA 初始化结构体 */
    /* 外设基址为：ADC 数据寄存器地址 */
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC_x->DR));

    /* 设置保存ADC转换结果的地址 */
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&adc_value;

    /* 数据源来自外设 */
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;

    /* 缓冲区大小，应该等于数据目的地的大小 */
    DMA_InitStructure.DMA_BufferSize = E_ADC_CHANNEL_MAX;

    /* 外设寄存器只有一个，地址不用递增 */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

    /* 存储器地址递增 */
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;

    /* 外设数据大小为半字，即两个字节 */
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;

    /* 内存数据大小也为半字，跟外设数据大小相同 */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;

    /* 循环传输模式 */
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;

    /* DMA 传输通道优先级为高，当使用一个DMA通道时，优先级设置不影响 */
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;

    /* 禁止存储器到存储器模式，因为是从外设到存储器 */
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    /* 初始化DMA */
    DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);

    /* 使能 DMA 通道 */
    DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);

    /* ADC 模式配置 */
    /* 只使用一个ADC，属于单模式 */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;

    /* 扫描模式 */
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;

    /* 连续转换模式 */
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;

    /* 不用外部触发转换，软件开启即可 */
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;

    /* 转换结果右对齐 */
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;

    /* 转换通道个数 */
    ADC_InitStructure.ADC_NbrOfChannel = E_ADC_CHANNEL_MAX;

    /* 初始化ADC */
    ADC_Init(ADC_x, &ADC_InitStructure);

    /* 配置ADC时钟Ｎ狿CLK2的8分频，即9MHz */
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    /* 配置ADC 通道的转换顺序和采样时间 */
    ADC_RegularChannelConfig(ADC_x, ADC1_ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC_x, ADC2_ADC_CHANNEL, 2, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC_x, ADC3_ADC_CHANNEL, 3, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC_x, ADC4_ADC_CHANNEL, 4, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC_x, ADC5_ADC_CHANNEL, 5, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC_x, ADC6_ADC_CHANNEL, 6, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC_x, ADC7_ADC_CHANNEL, 7, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC_x, ADC8_ADC_CHANNEL, 8, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC_x, ADC9_ADC_CHANNEL, 9, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC_x, ADC10_ADC_CHANNEL, 10, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC_x, ADC11_ADC_CHANNEL, 11, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC_x, ADC12_ADC_CHANNEL, 12, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC_x, ADC13_ADC_CHANNEL, 13, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC_x, ADC14_ADC_CHANNEL, 14, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC_x, ADC15_ADC_CHANNEL, 15, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC_x, ADC16_ADC_CHANNEL, 16, ADC_SampleTime_55Cycles5);

    /* 使能ADC DMA 请求 */
    ADC_DMACmd(ADC_x, ENABLE);

    /* 开启ADC ，并开始转换 */
    ADC_Cmd(ADC_x, ENABLE);

    /* 初始化ADC 校准寄存器 */
    ADC_ResetCalibration(ADC_x);

    /* 等待校准寄存器初始化完成 */
    while(ADC_GetResetCalibrationStatus(ADC_x));

    /* ADC开始校准 */
    ADC_StartCalibration(ADC_x);

    /* 等待校准完成 */
    while(ADC_GetCalibrationStatus(ADC_x));

    ADC_Cmd(ADC_x, DISABLE);
    ADC_Cmd(ADC_x, ENABLE);

    /* 由于没有采用外部触发，所以使用软件触发ADC转换 */
    ADC_SoftwareStartConvCmd(ADC_x, ENABLE);
}

/* ADC1和DMA配置 -------------------------------------------------------- end */


/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

