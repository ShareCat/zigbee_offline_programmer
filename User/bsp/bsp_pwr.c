/**
  ******************************************************************************
  * @file    stm32f0xx_pwr.c
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    05-December-2014
  * @brief   This file provides firmware functions to manage the following
  *          functionalities of the Power Controller (PWR) peripheral:
  *           + Backup Domain Access
  *           + PVD configuration
  *           + WakeUp pins configuration
  *           + Low Power modes configuration
  *           + Flags management
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x_pwr.h"
//#include "stm32f10x_rcc.h"
#include "bsp_pwr.h"
#include "./../sys/sys_command_line.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* ------------------ PWR registers bit mask ------------------------ */



/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/**
  * @brief  ��������ʱ��
  * @param  None
  * @retval None
  */
void pwr_reset_clock(void)
{
#if 0
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

    /* SYSCLK, HCLK, PCLK configuration ----------------------------------------*/
    /* Enable HSE */
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);

    /* Wait till HSE is ready and if Time out is reached exit */
    do {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        StartUpCounter++;
    } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if((RCC->CR & RCC_CR_HSERDY) != RESET) {
        HSEStatus = (uint32_t)0x01;
    } else {
        HSEStatus = (uint32_t)0x00;
    }

    if(HSEStatus == (uint32_t)0x01) {
        /* Enable Prefetch Buffer and set Flash Latency */
        FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;

        /* HCLK = SYSCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

        /* PCLK = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE_DIV1;

        /* PLL configuration = HSE * 6 = 48 MHz */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));

#ifdef STM32F0_CPU_72M /* ��Ƶ��72M */
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_PREDIV1 | RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLMULL9);
#else //48M
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_PREDIV1 | RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLMULL6);
#endif

        /* Enable PLL */
        RCC->CR |= RCC_CR_PLLON;

        /* Wait till PLL is ready */
        while((RCC->CR & RCC_CR_PLLRDY) == 0) {
        }

        /* Select PLL as system clock source */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

        /* Wait till PLL is used as system clock source */
        while((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL) {
        }
    } else {
        /* If HSE fails to start-up, the application will have wrong clock
             configuration. User can add here some code to deal with this error */
    }

#endif
}


void pwr_init(void)
{
#if 0
    GPIO_InitTypeDef  GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); /* ʹ�ܵ�Դ����ʱ�� */
    PWR_BackupAccessCmd(ENABLE);
    PWR_WakeUpPinCmd(PWR_WakeUpPin_1, ENABLE);
    PWR_ClearFlag(PWR_FLAG_WU);
    //PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
    //PWR_EnterSTANDBYMode();

    /* PA0���� */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//ѡ��Ҫ�õ�GPIO����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //��������ģʽΪ��������ģʽ
    GPIO_Init(GPIOA, &GPIO_InitStructure); //���ÿ⺯������ʼ��GPIO

    EXTI_InitStructure.EXTI_Line = EXTI_Line0;  //���ð������е��ⲿ��·
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;         //�����ⲿ�ж�ģʽ:EXTI��·Ϊ�ж�����
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //�����ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure); // ��ʼ���ⲿ�ж�

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_1_IRQn; //ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPriority = 2; //��ռ���ȼ�2��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure); //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
#endif
}


void pwr_ctrl(uint8_t mode)
{
#if 0
#if 0
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6 | RCC_APB2Periph_USART7
                           | RCC_APB2Periph_USART8 | RCC_APB2Periph_ADC1
                           | RCC_APB2Periph_TIM1 | RCC_APB2Periph_SPI1
                           | RCC_APB2Periph_TIM15 | RCC_APB2Periph_TIM16
                           | RCC_APB2Periph_TIM17 | RCC_APB2Periph_DBGMCU, DISABLE);

    GPIOA->MODER = 0x55555555;
    GPIOB->MODER = 0x55555555;
    GPIOC->MODER = 0x55555555;
    GPIOD->MODER = 0x55555555;
    GPIOF->MODER = 0x55555555;
    //����������Ҳ������
    GPIOA->PUPDR = 0x00000000;
    GPIOB->PUPDR = 0x00000000;
    GPIOC->PUPDR = 0x00000000;
    GPIOD->PUPDR = 0x00000000;
    GPIOF->PUPDR = 0x00000000;
    //���0
    GPIOA->ODR = 0x00000000;
    GPIOB->ODR = 0x00000000;
    GPIOC->ODR = 0x00000000;
    GPIOD->ODR = 0x00000000;
    GPIOF->ODR = 0x00000000;

#endif

    if(SLEEP_MODE == mode) {

#ifdef BSP_PWR_DEBUG
        PRINTF("ENTER SLEEP_MODE");
#endif
        PWR_EnterSleepMode(PWR_SLEEPEntry_WFI); /* Request Wait For Interrupt */
        //PWR_EnterSleepMode(PWR_SLEEPEntry_WFE); /* Request Wait For Event */

    } else if(STOP_MODE == mode) {

#ifdef BSP_PWR_DEBUG
        PRINTF("ENTER STOP_MODE");
#endif
        PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_SLEEPEntry_WFI);
        /*  ��ֹͣģʽ�£�����ڽ����ģʽǰADC��DACû�б��رգ���ô��Щ������Ȼ����
            ������ͨ�����üĴ���ADC_CR2��ADONλ�ͼĴ���DAC_CR��ENxλΪ0�ɹر���2������
         */

    } else if(STANDBY_MODE == mode) {

#ifdef BSP_PWR_DEBUG
        PRINTF("ENTER STANDBY_MODE");
#endif
        PWR_EnterSTANDBYMode();
        /*  �Ӵ���ģʽ���Ѻ�Ĵ���ִ�е�ͬ�ڸ�λ���ִ��(��������ģʽ���š���ȡ��λ
            ������)����Դ����/״̬�Ĵ���(PWR_CSR)����ָʾ�ں��ɴ���״̬�˳�
         */
    }

#endif
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
