/**
  ******************************************************************************
  * @file:
  * @author:    Cat
  * @version:   V1.0
  * @date:      2018-3-23
  * @brief:     �������߷��������߾��š�����PGM������
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



/* ADC1��DMA���� ------------------------------------------------------ begin */

volatile uint16_t adc_value[E_ADC_CHANNEL_MAX]; /* ADC�ɼ�������棬�Ҷ��� */
volatile uint16_t hcf4051_ch_value[E_HCF4051_CHANNEL_MAX];
uint8_t hcf4051_ch = E_HCF4051_ADC17;

void adc_hcf4051_gpio_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /*����GPIO����ʱ��*/
    RCC_APB2PeriphClockCmd(HCF4051_S0_CLK |
                           HCF4051_S1_CLK |
                           HCF4051_S2_CLK, ENABLE);

    /* HCF4051_S0 */
    GPIO_InitStructure.GPIO_Pin = HCF4051_S0_PIN;       /* ѡ��Ҫ���Ƶ�GPIO���� */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    /* ģʽΪͨ��������� */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   /* ����Ϊ50MHz */
    GPIO_Init(HCF4051_S0_PORT, &GPIO_InitStructure);    /* ���⺯������ʼ��GPIO */
    GPIO_ResetBits(HCF4051_S0_PORT, HCF4051_S0_PIN);

    /* HCF4051_S1 */
    GPIO_InitStructure.GPIO_Pin = HCF4051_S1_PIN;       /* ѡ��Ҫ���Ƶ�GPIO���� */
    GPIO_Init(HCF4051_S1_PORT, &GPIO_InitStructure);    /* ���⺯������ʼ��GPIO */
    GPIO_ResetBits(HCF4051_S1_PORT, HCF4051_S1_PIN);

    /* HCF4051_S2 */
    GPIO_InitStructure.GPIO_Pin = HCF4051_S2_PIN;       /* ѡ��Ҫ���Ƶ�GPIO���� */
    GPIO_Init(HCF4051_S2_PORT, &GPIO_InitStructure);    /* ���⺯������ʼ��GPIO */
    GPIO_ResetBits(HCF4051_S2_PORT, HCF4051_S2_PIN);

    adc_hcf4051_ctrl(E_HCF4051_ADC17);                  /* �ϵ�Ĭ����ѡ��һ��ͨ�� */
}


/**
  * @brief  ����HCF4051��ѡ��E_HCF4051_ADC15��E_HCF4051_ADC22�ӵ�ADC_Channel_14�ɼ�
  * @param  E_HCF4051_ADC15��E_HCF4051_ADC22
  * @retval ��
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
  * @brief  ���߷�����DC��AC���IO��ADC��ʼ��
  * @param  ��
  * @retval ��
  */
void adc_gpio_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* �� ADC IO�˿�ʱ�� */
    ADC_GPIO_APBxClock_FUN(ADC1_8_ADC_GPIO_CLK |
                           ADC9_10_ADC_GPIO_CLK |
                           ADC11_16_ADC_GPIO_CLK, ENABLE);

    /* ���� ADC IO ����ģʽ */
    GPIO_InitStructure.GPIO_Pin = ADC1_ADC_PIN | ADC2_ADC_PIN |
                                  ADC3_ADC_PIN | ADC4_ADC_PIN |
                                  ADC5_ADC_PIN | ADC6_ADC_PIN |
                                  ADC7_ADC_PIN | ADC8_ADC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(ADC1_8_ADC_PORT, &GPIO_InitStructure);    /* ��ʼ�� ADC IO */

    /* ���� ADC IO ����ģʽ */
    GPIO_InitStructure.GPIO_Pin = ADC9_ADC_PIN | ADC10_ADC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(ADC9_10_ADC_PORT, &GPIO_InitStructure);   /* ��ʼ�� ADC IO */

    /* ���� ADC IO ����ģʽ */
    GPIO_InitStructure.GPIO_Pin = ADC11_ADC_PIN | ADC12_ADC_PIN |
                                  ADC13_ADC_PIN | ADC14_ADC_PIN |
                                  ADC15_ADC_PIN | ADC16_ADC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(ADC11_16_ADC_PORT, &GPIO_InitStructure);  /* ��ʼ�� ADC IO */
}


/**
  * @brief  ����ADC1��DMA������������ת��ģʽ
  * @param  ��
  * @retval ��
  */
void adc_dma_config(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    /* ��DMAʱ�� */
    RCC_AHBPeriphClockCmd(ADC_DMA_CLK, ENABLE);
    /* ��ADCʱ�� */
    ADC_APBxClock_FUN(ADC_CLK, ENABLE);

    /* ��λDMA������ */
    DMA_DeInit(ADC_DMA_CHANNEL);

    /* ���� DMA ��ʼ���ṹ�� */
    /* �����ַΪ��ADC ���ݼĴ�����ַ */
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC_x->DR));

    /* ���ñ���ADCת������ĵ�ַ */
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&adc_value;

    /* ����Դ�������� */
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;

    /* ��������С��Ӧ�õ�������Ŀ�ĵصĴ�С */
    DMA_InitStructure.DMA_BufferSize = E_ADC_CHANNEL_MAX;

    /* ����Ĵ���ֻ��һ������ַ���õ��� */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

    /* �洢����ַ���� */
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;

    /* �������ݴ�СΪ���֣��������ֽ� */
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;

    /* �ڴ����ݴ�СҲΪ���֣����������ݴ�С��ͬ */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;

    /* ѭ������ģʽ */
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;

    /* DMA ����ͨ�����ȼ�Ϊ�ߣ���ʹ��һ��DMAͨ��ʱ�����ȼ����ò�Ӱ�� */
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;

    /* ��ֹ�洢�����洢��ģʽ����Ϊ�Ǵ����赽�洢�� */
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    /* ��ʼ��DMA */
    DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);

    /* ʹ�� DMA ͨ�� */
    DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);

    /* ADC ģʽ���� */
    /* ֻʹ��һ��ADC�����ڵ�ģʽ */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;

    /* ɨ��ģʽ */
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;

    /* ����ת��ģʽ */
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;

    /* �����ⲿ����ת��������������� */
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;

    /* ת������Ҷ��� */
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;

    /* ת��ͨ������ */
    ADC_InitStructure.ADC_NbrOfChannel = E_ADC_CHANNEL_MAX;

    /* ��ʼ��ADC */
    ADC_Init(ADC_x, &ADC_InitStructure);

    /* ����ADCʱ�ӣΪPCLK2��8��Ƶ����9MHz */
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    /* ����ADC ͨ����ת��˳��Ͳ���ʱ�� */
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

    /* ʹ��ADC DMA ���� */
    ADC_DMACmd(ADC_x, ENABLE);

    /* ����ADC ������ʼת�� */
    ADC_Cmd(ADC_x, ENABLE);

    /* ��ʼ��ADC У׼�Ĵ��� */
    ADC_ResetCalibration(ADC_x);

    /* �ȴ�У׼�Ĵ�����ʼ����� */
    while(ADC_GetResetCalibrationStatus(ADC_x));

    /* ADC��ʼУ׼ */
    ADC_StartCalibration(ADC_x);

    /* �ȴ�У׼��� */
    while(ADC_GetCalibrationStatus(ADC_x));

    ADC_Cmd(ADC_x, DISABLE);
    ADC_Cmd(ADC_x, ENABLE);

    /* ����û�в����ⲿ����������ʹ���������ADCת�� */
    ADC_SoftwareStartConvCmd(ADC_x, ENABLE);
}

/* ADC1��DMA���� -------------------------------------------------------- end */


/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

