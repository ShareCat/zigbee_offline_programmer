/**
  ******************************************************************************
  * @file    Project/STM32F0xx_StdPeriph_Templates/main.c
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    05-December-2014
  * @brief   Main program body
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


#include "bsp_usart.h"
#include "../sys/sys_queue.h"
#include "../sys/sys_command_line.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void usart_send_byte(USART_TypeDef * pUSARTx, uint8_t ch);


/* -------------------------------------------------------------------- UART1 */

#if 0
void download_usart_tx(uint8_t *buff, uint16_t len)
{
    uint16_t i = 0;

    for(i = 0; i < len; i++) {
        usart_send_byte(DOWNLOAD_USARTx, buff[i]);
    }
}

/**
 * @brief  ���ڷ����жϷ���������stm32f0xx_it.c�����ж��е���
 * @param  ��
 * @retval ��
 */
void download_usart_tx_handle(void)
{
    /* Ԥ�� */
}

/**
 * @brief  ���ڽ����жϷ���������stm32f0xx_it.c�����ж��е���
 * @param  ��
 * @retval ��
 */
void download_usart_rx_handle(void)
{
    uint8_t ucTemp;
    extern QUEUE256_S nxp_rx_queue;

    if(USART_GetITStatus(DOWNLOAD_USARTx, USART_IT_RXNE) != RESET) {
        ucTemp = USART_ReceiveData(DOWNLOAD_USARTx);
        QUEUE_IN(nxp_rx_queue, ucTemp);
        //usart_send_byte(DOWNLOAD_USARTx, ucTemp);
        //PRINTF("%02x ", ucTemp);
    }
}
#endif
/**
 * @brief  ����Ƕ�������жϿ�����NVIC
 * @param  ��
 * @retval ��
 */
static void download_uart_nvic_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Ƕ�������жϿ�������ѡ�� */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* ����USARTΪ�ж�Դ */
    NVIC_InitStructure.NVIC_IRQChannel = DOWNLOAD_USART_IRQ;
    /* �������ȼ�*/
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    /* �����ȼ� */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    /* ʹ���ж� */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    /* ��ʼ������NVIC */
    NVIC_Init(&NVIC_InitStructure);

#if 0   /* USART_DMA�������ⳤ�ȵ����ݣ�����ʹ��USART_RX���жϣ���ΪDMA_RX���ж�ֻ�ܽ��չ̶����ȵ����ݲŻ����жϲ��� */
    //DMA�����ж�����
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif

    //DMA�����ж�����
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/* USART_DMA���ջ��� */
uint8_t download_usart_dma_rx_buff[DOWNLOAD_USART_DMA_RX_BUFF_MAX];

/* USART_DMA���ͻ��� */
uint8_t download_usart_dma_tx_buff[DOWNLOAD_USART_DMA_TX_BUFF_MAX];

void download_usart_dma_rx_handle(void)
{
    if(USART_GetITStatus(DOWNLOAD_USARTx, USART_IT_IDLE) != RESET) {
        uint32_t temp = 0;
        uint16_t len;

        temp = temp;    /* ������������� */
        temp = DOWNLOAD_USARTx->SR;
        temp = DOWNLOAD_USARTx->DR; //��USART_IT_IDLE��־
        DMA_Cmd(DMA1_Channel5, DISABLE);

        /* ��ȡUSART_DMA�յ��ֽ��� */
        len = DOWNLOAD_USART_DMA_RX_BUFF_MAX - DMA_GetCurrDataCounter(DMA1_Channel5);

        extern void nxp_handle_rx_copy(uint8_t *buff, uint16_t len);
        nxp_handle_rx_copy(download_usart_dma_rx_buff, len);

#ifdef NXP_TEST_MODE
        extern uint8_t nxp_test_mode;
        if (TRUE == nxp_test_mode) {
            download_usart_dma_rx_buff[len] = 0;
            PRINTF("RX: %s \r\n", download_usart_dma_rx_buff);
        }
#endif

        //�������ô������ݳ���
        DMA_SetCurrDataCounter(DMA1_Channel5, DOWNLOAD_USART_DMA_RX_BUFF_MAX);
        //�򿪽���DMAͨ�������½���
        DMA_Cmd(DMA1_Channel5, ENABLE);
#if 0
        uint16_t i = 0;
        PRINTF("ram data from nxp516x: ");

        for(i = 0; i < len; i++) {
            PRINTF("%02x ", download_usart_dma_rx_buff[i]);
        }

        NL1();
#endif
    }
}

void download_usart_dma_tx_handle(void)
{
    if(DMA_GetITStatus(DMA1_FLAG_TC4) == SET) {
        DMA_ClearFlag(DMA1_FLAG_GL4);
        DMA_Cmd(DMA1_Channel4, DISABLE);
        //PRINTF("download_usart_dma_tx_handle \r\n");
    }
}

void download_usart_dma_tx(uint8_t *buff, uint16_t len)
{
    //while (DMA_GetCurrDataCounter(DMA1_Channel4));  // ���DMA����ͨ�����Ƿ�������
    if(len) {
        if(len > DOWNLOAD_USART_DMA_TX_BUFF_MAX) {
            len = DOWNLOAD_USART_DMA_TX_BUFF_MAX;
            /* ������Χ */
        }

        memcpy(download_usart_dma_tx_buff, buff, len);
        //DMA��������-Ҫ�ȹ� ���÷��ͳ��� ����DMA
        DMA_Cmd(DMA1_Channel4, DISABLE);
        DMA1_Channel4->CNDTR = len;     // ���÷��ͳ���
        DMA_Cmd(DMA1_Channel4, ENABLE); // ����DMA
        USART_DMACmd(DOWNLOAD_USARTx, USART_DMAReq_Tx, ENABLE); // ����DMA����
    }
}

/**
 * @brief  USART GPIO ����,������������
 * @param  ��
 * @retval ��
 */
void download_uart_init(uint32_t band)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    // �򿪴���GPIO��ʱ��
    DOWNLOAD_USART_APBxClkCmd(DOWNLOAD_USART_GPIO_CLK, ENABLE);

    // �򿪴��������ʱ��
    DOWNLOAD_USART_APBxClkCmd(DOWNLOAD_USART_CLK, ENABLE);

    // ��USART Tx��GPIO����Ϊ���츴��ģʽ
    GPIO_InitStructure.GPIO_Pin = DOWNLOAD_USART_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DOWNLOAD_USART_TX_GPIO_PORT, &GPIO_InitStructure);

    // ��USART Rx��GPIO����Ϊ��������ģʽ
    GPIO_InitStructure.GPIO_Pin = DOWNLOAD_USART_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(DOWNLOAD_USART_RX_GPIO_PORT, &GPIO_InitStructure);

    // ���ô��ڵĹ�������
    // ���ò�����
    USART_InitStructure.USART_BaudRate = band;
    // ���� �������ֳ�
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    // ����ֹͣλ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    // ����У��λ
    USART_InitStructure.USART_Parity = USART_Parity_No;
    // ����Ӳ��������
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    // ���ù���ģʽ���շ�һ��
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    // ��ɴ��ڵĳ�ʼ������
    USART_Init(DOWNLOAD_USARTx, &USART_InitStructure);

    // �����ж����ȼ�����
    download_uart_nvic_config();

    // ʹ�ܴ���
    USART_Cmd(DOWNLOAD_USARTx, ENABLE);

/* -------------------------------------------------------------- UART DMA RX */
    /*
        ������DMA����
    */
    DMA_InitTypeDef DMA_InitStructure;

    //����DMAʱ��
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    //DMA1ͨ��5����
    DMA_DeInit(DMA1_Channel5);
    //�����ַ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);
    //�ڴ��ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)download_usart_dma_rx_buff;
    //dma���䷽����
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    //����DMA�ڴ���ʱ�������ĳ���
    DMA_InitStructure.DMA_BufferSize = DOWNLOAD_USART_DMA_RX_BUFF_MAX;
    //����DMA���������ģʽ��һ������
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    //����DMA���ڴ����ģʽ
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    //���������ֳ�
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    //�ڴ������ֳ�
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    //����DMA�Ĵ���ģʽ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    //����DMA�����ȼ���
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    //����DMA��2��memory�еı����������
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);

    //�ж�����
    USART_ITConfig(DOWNLOAD_USARTx, USART_IT_TC, DISABLE);
    USART_ITConfig(DOWNLOAD_USARTx, USART_IT_RXNE, DISABLE);
    USART_ITConfig(DOWNLOAD_USARTx, USART_IT_IDLE, ENABLE);
    //����DMA��ʽ����
    USART_DMACmd(DOWNLOAD_USARTx, USART_DMAReq_Rx, ENABLE);
    //ʹ��ͨ��5
    DMA_Cmd(DMA1_Channel5, ENABLE);
/* -------------------------------------------------------------- UART DMA RX */

/* -------------------------------------------------------------- UART DMA TX */
    /*
        ���ڷ�DMA����
    */
    //DMA1ͨ��4����
    DMA_DeInit(DMA1_Channel4);
    //�����ַ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);
    //�ڴ��ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)download_usart_dma_tx_buff;
    //dma���䷽����
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    //����DMA�ڴ���ʱ�������ĳ���
    DMA_InitStructure.DMA_BufferSize = DOWNLOAD_USART_DMA_TX_BUFF_MAX;
    //����DMA���������ģʽ��һ������
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    //����DMA���ڴ����ģʽ
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    //���������ֳ�
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    //�ڴ������ֳ�
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
    //����DMA�Ĵ���ģʽ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    //����DMA�����ȼ���
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    //����DMA��2��memory�еı����������
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);
    DMA_ClearFlag(DMA1_FLAG_GL4);//���DMA���б�־
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);    //��������DMAͨ���ж�
    DMA_Cmd(DMA1_Channel4, DISABLE);                   //�ر�DMA
/* -------------------------------------------------------------- UART DMA TX */
}

/* -------------------------------------------------------------------- UART2 */
#if 0
/**
 * @brief  ���ڷ����жϷ���������stm32f0xx_it.c�����ж��е���
 * @param  ��
 * @retval ��
 */
void gsm_wifi_usart_tx_handle(void)
{
    /* Ԥ�� */
}

/**
 * @brief  ���ڽ����жϷ���������stm32f0xx_it.c�����ж��е���
 * @param  ��
 * @retval ��
 */
void gsm_wifi_usart_rx_handle(void)
{
    uint8_t ucTemp;

    if(USART_GetITStatus(GSM_WIFI_USARTx, USART_IT_RXNE) != RESET) {
        ucTemp = USART_ReceiveData(GSM_WIFI_USARTx);
        //usart_send_byte(GSM_WIFI_USARTx, ucTemp);
    }
}

/**
 * @brief  ����Ƕ�������жϿ�����NVIC
 * @param  ��
 * @retval ��
 */
static void gsm_wifi_uart_nvic_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Ƕ�������жϿ�������ѡ�� */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* ����USARTΪ�ж�Դ */
    NVIC_InitStructure.NVIC_IRQChannel = GSM_WIFI_USART_IRQ;
    /* �������ȼ�*/
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    /* �����ȼ� */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    /* ʹ���ж� */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    /* ��ʼ������NVIC */
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief  USART GPIO ����,������������
 * @param  ��
 * @retval ��
 */
void gsm_wifi_uart_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    // �򿪴���GPIO��ʱ��
    DEBUG_USART_GPIO_APBxClkCmd(GSM_WIFI_USART_GPIO_CLK, ENABLE);

    // �򿪴��������ʱ��
    DEBUG_USART_APBxClkCmd(GSM_WIFI_USART_CLK, ENABLE);

    // ��USART Tx��GPIO����Ϊ���츴��ģʽ
    GPIO_InitStructure.GPIO_Pin = GSM_WIFI_USART_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GSM_WIFI_USART_TX_GPIO_PORT, &GPIO_InitStructure);

    // ��USART Rx��GPIO����Ϊ��������ģʽ
    GPIO_InitStructure.GPIO_Pin = GSM_WIFI_USART_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GSM_WIFI_USART_RX_GPIO_PORT, &GPIO_InitStructure);

    // ���ô��ڵĹ�������
    // ���ò�����
    USART_InitStructure.USART_BaudRate = GSM_WIFI_USART_BAUDRATE;
    // ���� �������ֳ�
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    // ����ֹͣλ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    // ����У��λ
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    // ����Ӳ��������
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    // ���ù���ģʽ���շ�һ��
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    // ��ɴ��ڵĳ�ʼ������
    USART_Init(GSM_WIFI_USARTx, &USART_InitStructure);

    // �����ж����ȼ�����
    gsm_wifi_uart_nvic_config();

    // ʹ�ܴ��ڽ����ж�
    USART_ITConfig(GSM_WIFI_USARTx, USART_IT_RXNE, ENABLE);

    // ʹ�ܴ���
    USART_Cmd(GSM_WIFI_USARTx, ENABLE);

    // ���������ɱ�־
    //USART_ClearFlag(GSM_WIFI_USARTx, USART_FLAG_TC);
}
#endif
/* -------------------------------------------------------------------- UART3 */
#if 0
/**
 * @brief  ���ڷ����жϷ���������stm32f0xx_it.c�����ж��е���
 * @param  ��
 * @retval ��
 */
void pstn_wifi_usart_tx_handle(void)
{
    /* Ԥ�� */
}

/**
 * @brief  ���ڽ����жϷ���������stm32f0xx_it.c�����ж��е���
 * @param  ��
 * @retval ��
 */
void pstn_wifi_usart_rx_handle(void)
{
    uint8_t ucTemp;

    if(USART_GetITStatus(PSTN_WIFI_USARTx, USART_IT_RXNE) != RESET) {
        ucTemp = USART_ReceiveData(PSTN_WIFI_USARTx);
        usart_send_byte(PSTN_WIFI_USARTx, ucTemp);
    }
}

/**
 * @brief  ����Ƕ�������жϿ�����NVIC
 * @param  ��
 * @retval ��
 */
static void pstn_wifi_uart_nvic_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Ƕ�������жϿ�������ѡ�� */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* ����USARTΪ�ж�Դ */
    NVIC_InitStructure.NVIC_IRQChannel = PSTN_WIFI_USART_IRQ;
    /* �������ȼ�*/
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    /* �����ȼ� */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    /* ʹ���ж� */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    /* ��ʼ������NVIC */
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief  USART GPIO ����,������������
 * @param  ��
 * @retval ��
 */
void pstn_wifi_uart_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    // �򿪴���GPIO��ʱ��
    DEBUG_USART_GPIO_APBxClkCmd(PSTN_WIFI_USART_GPIO_CLK, ENABLE);

    // �򿪴��������ʱ��
    DEBUG_USART_APBxClkCmd(PSTN_WIFI_USART_CLK, ENABLE);

    // ��USART Tx��GPIO����Ϊ���츴��ģʽ
    GPIO_InitStructure.GPIO_Pin = PSTN_WIFI_USART_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PSTN_WIFI_USART_TX_GPIO_PORT, &GPIO_InitStructure);

    // ��USART Rx��GPIO����Ϊ��������ģʽ
    GPIO_InitStructure.GPIO_Pin = PSTN_WIFI_USART_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(PSTN_WIFI_USART_RX_GPIO_PORT, &GPIO_InitStructure);

    // ���ô��ڵĹ�������
    // ���ò�����
    USART_InitStructure.USART_BaudRate = PSTN_WIFI_USART_BAUDRATE;
    // ���� �������ֳ�
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    // ����ֹͣλ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    // ����У��λ
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    // ����Ӳ��������
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    // ���ù���ģʽ���շ�һ��
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    // ��ɴ��ڵĳ�ʼ������
    USART_Init(PSTN_WIFI_USARTx, &USART_InitStructure);

    // �����ж����ȼ�����
    pstn_wifi_uart_nvic_config();

    // ʹ�ܴ��ڽ����ж�
    USART_ITConfig(PSTN_WIFI_USARTx, USART_IT_RXNE, ENABLE);

    // ʹ�ܴ���
    USART_Cmd(PSTN_WIFI_USARTx, ENABLE);

    // ���������ɱ�־
    //USART_ClearFlag(PSTN_WIFI_USARTx, USART_FLAG_TC);
}
#endif
/* -------------------------------------------------------------------- UART4 */
#if 0
/**
 * @brief  ���ڷ����жϷ���������stm32f0xx_it.c�����ж��е���
 * @param  ��
 * @retval ��
 */
void rf_usart_tx_handle(void)
{
}

/**
 * @brief  ���ڽ����жϷ���������stm32f0xx_it.c�����ж��е���
 * @param  ��
 * @retval ��
 */
void rf_usart_rx_handle(void)
{
    uint8_t byTemp;

    if(USART_GetITStatus(RF_USARTx, USART_IT_RXNE) != RESET) {
        USART_ClearITPendingBit(RF_USARTx, USART_IT_RXNE);
        byTemp = USART_ReceiveData(RF_USARTx);
        usart_send_byte(RF_USARTx, byTemp);
    }
}

void rf_usart_tx_int_enable(void)
{
    USART_ITConfig(RF_USARTx, USART_IT_TXE, ENABLE);
}

void rf_usart_tx_int_disable(void)
{
    USART_ITConfig(RF_USARTx, USART_IT_TXE, DISABLE);
}

/**
 * @brief  USART GPIO ����,������������
 * @param  ��
 * @retval ��
 */
void rf_uart_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RF_USART_GPIO_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RF_USART_CLK, ENABLE);

    /* Configure UART4 Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = RF_USART_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(RF_USART_RX_GPIO_PORT, &GPIO_InitStructure);

    /* Configure UART4 Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = RF_USART_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(RF_USART_TX_GPIO_PORT, &GPIO_InitStructure);

    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = RF_USART_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* UART4 configuration ---------------------------------------------------*/
    /* UART4configured as follow:
          - BaudRate = 9600 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - No parity
          - Hardware flow control disabled (RTS and CTS signals)
          - Receive and transmit enabled
    */
    USART_InitStructure.USART_BaudRate = RF_USART_BAUDRATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    /* Configure UART4 */
    USART_Init(RF_USARTx, &USART_InitStructure);

    /* Enable UART4 Receive and Transmit interrupts */
    USART_ITConfig(RF_USARTx, USART_IT_RXNE, ENABLE);

    /* Enable the UART4 */
    USART_Cmd(RF_USARTx, ENABLE);
}

/**
 * @brief  ���ڷ���һ���ֽ�
 * @param  byData ��������
 * @retval ��
 */
void rf_uart_send_byte(uint8_t byData)
{
    usart_send_byte(RF_USARTx, byData);
}
#endif
/* -------------------------------------------------------------------- UART5 */
/**
 * @brief  ���ڷ����жϷ���������stm32f0xx_it.c�����ж��е���
 * @param  ��
 * @retval ��
 */
void debug_usart_tx_handle(void)
{
    /* Ԥ�� */
}

/**
 * @brief  ���ڽ����жϷ���������stm32f0xx_it.c�����ж��е���
 * @param  ��
 * @retval ��
 */
void debug_usart_rx_handle(void)
{
    uint8_t ucTemp;

    if(USART_GetITStatus(DEBUG_USARTx, USART_IT_RXNE) != RESET) {
        ucTemp = USART_ReceiveData(DEBUG_USARTx);
        QUEUE_IN(cli_rx_buff, ucTemp);
        //printf("%02x", ucTemp);
    }
}

/**
 * @brief  ����Ƕ�������жϿ�����NVIC
 * @param  ��
 * @retval ��
 */
static void debug_usart_nvic_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Ƕ�������жϿ�������ѡ�� */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* ����USARTΪ�ж�Դ */
    NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;
    /* �������ȼ�*/
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    /* �����ȼ� */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    /* ʹ���ж� */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    /* ��ʼ������NVIC */
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief  USART GPIO ����,������������
 * @param  ��
 * @retval ��
 */
void debug_usart_init(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    // �򿪴���GPIO��ʱ��
    DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);

    // �򿪴��������ʱ��
    DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);

    // ��USART Tx��GPIO����Ϊ���츴��ģʽ
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

    // ��USART Rx��GPIO����Ϊ��������ģʽ
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);

    // ���ô��ڵĹ�������
    // ���ò�����
    USART_InitStructure.USART_BaudRate = 2000000;
    // ���� �������ֳ�
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    // ����ֹͣλ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    // ����У��λ
    USART_InitStructure.USART_Parity = USART_Parity_No;
    // ����Ӳ��������
    USART_InitStructure.USART_HardwareFlowControl =
        USART_HardwareFlowControl_None;
    // ���ù���ģʽ���շ�һ��
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    // ��ɴ��ڵĳ�ʼ������
    USART_Init(DEBUG_USARTx, &USART_InitStructure);

    // �����ж����ȼ�����
    debug_usart_nvic_config();

    // ʹ�ܴ��ڽ����ж�
    USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE);

    // ʹ�ܴ���
    USART_Cmd(DEBUG_USARTx, ENABLE);
    USART_ClearFlag(DEBUG_USARTx, USART_FLAG_TC);
}


/**
 * @brief  ����һ���ֽ�
 * @param  ��
 * @retval ��
 */
void usart_send_byte(USART_TypeDef * pUSARTx, uint8_t ch)
{
    /* ����һ���ֽ����ݵ�USART */
    USART_SendData(pUSARTx, ch);

    /* �ȴ��������ݼĴ���Ϊ�� */
    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

/**
 * @brief  ����8λ������
 * @param  ��
 * @retval ��
 */
void usart_send_array(USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
    uint8_t i;

    for(i = 0; i < num; i++) {
        /* ����һ���ֽ����ݵ�USART */
        usart_send_byte(pUSARTx, array[i]);

    }

    /* �ȴ�������� */
    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET);
}

/**
 * @brief  �����ַ���
 * @param  ��
 * @retval ��
 */
void usart_send_string(USART_TypeDef * pUSARTx, char *str)
{
    unsigned int k = 0;

    do {
        usart_send_byte(pUSARTx, *(str + k));
        k++;
    } while(*(str + k) != '\0');

    /* �ȴ�������� */
    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET)
    {}
}

/**
 * @brief  ����һ��16λ��
 * @param  ��
 * @retval ��
 */
void Usart_SendHalfWord(USART_TypeDef * pUSARTx, uint16_t ch)
{
    uint8_t temp_h, temp_l;

    /* ȡ���߰�λ */
    temp_h = (ch & 0XFF00) >> 8;
    /* ȡ���Ͱ�λ */
    temp_l = ch & 0XFF;

    /* ���͸߰�λ */
    USART_SendData(pUSARTx, temp_h);

    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);

    /* ���͵Ͱ�λ */
    USART_SendData(pUSARTx, temp_l);

    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

/**
 * @brief  �ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
 * @param  ��
 * @retval ��
 */
int fputc(int ch, FILE *f)
{
    /* ����һ���ֽ����ݵ����� */
    USART_SendData(DEBUG_USARTx, (uint8_t) ch);

    /* �ȴ�������� */
    while(USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);

    return (ch);
}

/**
 * @brief  �ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
 * @param  ��
 * @retval ��
 */
int fgetc(FILE *f)
{
    /* �ȴ������������� */
    while(USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);

    return (int)USART_ReceiveData(DEBUG_USARTx);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

