/**
  ******************************************************************************
  * @file:      bsp_spi.c
  * @author:    Cat�����ƽ��
  * @version:   V1.0
  * @date:      2018-1-26
  * @brief:
  * @attention:
  ******************************************************************************
  */


#include "./bsp_spi.h"

#include "./../sys/sys_scheduler.h"
#include "./../sys/sys_command_line.h"


/**
  * @brief  �ȴ���ʱ�ص�����
  * @param  None.
  * @retval None.
  */
static uint16_t spi_timeout_user_callback(uint8_t errorCode)
{
    /* �ȴ���ʱ��Ĵ���,���������Ϣ */
    ERRA("SPI time out! errorCode = %d", errorCode);
    return 0;
}

/* --------------------------------------------------------------------- SPI1 */
#if 0
void spi_init_w5500(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    /* ʹ��SPIʱ�� */
    RCC_APB2PeriphClockCmd(W5500_SPI_CLK, ENABLE);

    /* ʹ��SPI������ص�ʱ�� */
    RCC_APB2PeriphClockCmd(W5500_SPI_SCK_CLK |
                           W5500_SPI_MISO_CLK |
                           W5500_SPI_MISO_CLK |
                           W5500_SPI_MOSI_CLK, ENABLE);

    /*!< Configure W5500_SPI pin: SCK */
    GPIO_InitStructure.GPIO_Pin = W5500_SPI_SCK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(W5500_SPI_SCK_PORT, &GPIO_InitStructure);

    /*!< Configure W5500_SPI pin: MISO */
    GPIO_InitStructure.GPIO_Pin = W5500_SPI_MISO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(W5500_SPI_MISO_PORT, &GPIO_InitStructure);

    /*!< Configure W5500_SPI pin: MOSI */
    GPIO_InitStructure.GPIO_Pin = W5500_SPI_MOSI_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(W5500_SPI_MOSI_PORT, &GPIO_InitStructure);

    /*!< Configure W5500_SPI pin: CS */
    GPIO_InitStructure.GPIO_Pin = W5500_SPI_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(W5500_SPI_CS_PORT, &GPIO_InitStructure);

    /*!< SPI configuration */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;

    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(W5500_SPIx, &SPI_InitStructure);

    /*!< Enable the W5500_SPI  */
    SPI_Cmd(W5500_SPIx, ENABLE);
}


void gpio_config_w5500(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(W5500_RST_CLK | W5500_INT_CLK, ENABLE);

    /*����RESET����*/
    GPIO_InitStructure.GPIO_Pin = W5500_RST_PIN;        /*ѡ��Ҫ���Ƶ�GPIO����*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   /*������������Ϊ50MHz */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    /*��������ģʽΪͨ���������*/
    GPIO_Init(W5500_RST_PORT, &GPIO_InitStructure);     /*���ÿ⺯������ʼ��GPIO*/
    GPIO_SetBits(W5500_RST_PORT, W5500_RST_PIN);

    /*����INT����*/
    GPIO_InitStructure.GPIO_Pin = W5500_INT_PIN;        /*ѡ��Ҫ���Ƶ�GPIO����*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   /*������������Ϊ50MHz*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       /*��������ģʽΪͨ������ģ����������*/
    GPIO_Init(W5500_INT_PORT, &GPIO_InitStructure);     /*���ÿ⺯������ʼ��GPIO*/

    spi_init_w5500();                                   /*��ʼ������W5500��SPI�ӿ�*/
}


/**
 * @brief  SPIƬѡ����
 * @param  null
 * @retval null
 */
void spi_cs_ctrl_w5500(FunctionalState cmd)
{
    if(ENABLE == cmd) {
        IO_HIGH(W5500_SPI_CS_PORT, W5500_SPI_CS_PIN);
    } else {
        IO_LOW(W5500_SPI_CS_PORT, W5500_SPI_CS_PIN);
    }
}

/**
 * @brief  RSTӲ����λ����
 * @param  null
 * @retval null
 */
void rst_ctrl_w5500(FunctionalState cmd)
{
    if(ENABLE == cmd) {
        IO_HIGH(W5500_RST_PORT, W5500_RST_PIN);
    } else {
        IO_LOW(W5500_RST_PORT, W5500_RST_PIN);
    }
}


/**
 * @brief  ʹ��SPI����һ���ֽڵ�����
 * @param  byte��Ҫ���͵�����
 * @retval ���ؽ��յ�������
 */
uint8_t spi_send_byte_w5500(uint8_t byte)
{
    uint32_t SPITimeout = SPIT_FLAG_TIMEOUT;

    /* �ȴ����ͻ�����Ϊ�գ�TXE�¼� */
    while(SPI_I2S_GetFlagStatus(W5500_SPIx, SPI_I2S_FLAG_TXE) == RESET) {
        if((SPITimeout--) == 0) return spi_timeout_user_callback(0);
    }

    /* д�����ݼĴ�������Ҫд�������д�뷢�ͻ����� */
    SPI_I2S_SendData(W5500_SPIx, byte);

    SPITimeout = SPIT_FLAG_TIMEOUT;

    /* �ȴ����ջ������ǿգ�RXNE�¼� */
    while(SPI_I2S_GetFlagStatus(W5500_SPIx, SPI_I2S_FLAG_RXNE) == RESET) {
        if((SPITimeout--) == 0) return spi_timeout_user_callback(1);
    }

    /* ��ȡ���ݼĴ�������ȡ���ջ��������� */
    return SPI_I2S_ReceiveData(W5500_SPIx);
}
#endif




/* --------------------------------------------------------------------- SPI2 */

/**
  * @brief  SPI FLASH��ʼ��
  * @param  ��
  * @retval ��
  */
void spi_flash_init(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* ʹ��SPIʱ�� */
    RCC_APB2PeriphClockCmd(FLASH_SPI_CLK, ENABLE);

    /* ʹ��SPI������ص�ʱ�� */
    RCC_APB2PeriphClockCmd(FLASH_SPI_CS_CLK |
                           FLASH_SPI_SCK_CLK |
                           FLASH_SPI_MISO_CLK |
                           FLASH_SPI_MOSI_CLK, ENABLE);

    /* ����SPI�� CS���ţ���ͨIO���� */
    GPIO_InitStructure.GPIO_Pin = FLASH_SPI_CS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(FLASH_SPI_CS_PORT, &GPIO_InitStructure);

    /* ����SPI�� SCK����*/
    GPIO_InitStructure.GPIO_Pin = FLASH_SPI_SCK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(FLASH_SPI_SCK_PORT, &GPIO_InitStructure);

    /* ����SPI�� MISO����*/
    GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MISO_PIN;
    GPIO_Init(FLASH_SPI_MISO_PORT, &GPIO_InitStructure);

    /* ����SPI�� MOSI����*/
    GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MOSI_PIN;
    GPIO_Init(FLASH_SPI_MOSI_PORT, &GPIO_InitStructure);

    /* ֹͣ�ź� FLASH: CS���Ÿߵ�ƽ*/
    SPI_FLASH_CS_HIGH();

    /* SPI ģʽ���� */
    // FLASHоƬ ֧��SPIģʽ0��ģʽ3���ݴ�����CPOL CPHA
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(FLASH_SPIx, &SPI_InitStructure);

    /* ʹ�� SPI  */
    SPI_Cmd(FLASH_SPIx, ENABLE);
}

/**
 * @brief  ����FLASH����
 * @param  SectorAddr��Ҫ���������������ַ�������������
 * @retval ��
 */
void spi_flash_sector_erase(uint32_t SectorAddr)
{
    /* ����FLASHдʹ������ */
    spi_flash_write_enable();
    spi_flash_wait_for_write_end();
    /* �������� */
    /* ѡ��FLASH: CS�͵�ƽ */
    SPI_FLASH_CS_LOW();

    /* ������������ָ��*/
    spi_flash_send_byte(W25X_SectorErase);
    /*���Ͳ���������ַ�ĸ�λ*/
    spi_flash_send_byte((SectorAddr & 0xFF0000) >> 16);
    /* ���Ͳ���������ַ����λ */
    spi_flash_send_byte((SectorAddr & 0xFF00) >> 8);
    /* ���Ͳ���������ַ�ĵ�λ */
    spi_flash_send_byte(SectorAddr & 0xFF);
    /* ֹͣ�ź� FLASH: CS �ߵ�ƽ */
    SPI_FLASH_CS_HIGH();
    /* �ȴ��������*/
    spi_flash_wait_for_write_end();
}

/**
 * @brief  ����FLASH��
 * @param  SectorAddr��Ҫ�����Ŀ��ַ
 * @retval ��
 */
void spi_flash_block_erase(uint32_t BlockAddr)
{
    /* ����FLASHдʹ������ */
    spi_flash_write_enable();
    spi_flash_wait_for_write_end();
    /* �������� */
    /* ѡ��FLASH: CS�͵�ƽ */
    SPI_FLASH_CS_LOW();

    /* ������������ָ��*/
    spi_flash_send_byte(W25X_BlockErase);
    /*���Ͳ���������ַ�ĸ�λ*/
    spi_flash_send_byte((BlockAddr & 0xFF0000) >> 16);
    /* ���Ͳ���������ַ����λ */
    spi_flash_send_byte((BlockAddr & 0xFF00) >> 8);
    /* ���Ͳ���������ַ�ĵ�λ */
    spi_flash_send_byte(BlockAddr & 0xFF);
    /* ֹͣ�ź� FLASH: CS �ߵ�ƽ */
    SPI_FLASH_CS_HIGH();
    /* �ȴ��������*/
    spi_flash_wait_for_write_end();
}

/**
* @brief   �����ṩ��Ҫ������ַ�Ͳ�����С���������
* @param   ��
* @retval  FLASH ID
*/
void spi_flash_addr_erase(int addr, int size)
{
    uint32_t block_pos;
    uint32_t block_addr;
    uint32_t i;
    uint32_t sec_num;                           /* Ҫ��������� */

    i = size % SPI_FLASH_BlockSize;
    sec_num = size / SPI_FLASH_BlockSize;

    if(i) sec_num++;

    block_pos = addr / SPI_FLASH_BlockSize;     /* ���ַ (��������ĵ�ַ���ڵڼ�����) */

    for(i = 0; i < sec_num; i++) {
        block_addr = block_pos * SPI_FLASH_BlockSize;   /* ����������������ʼ��ַ */
        spi_flash_block_erase(block_addr);
        block_pos++;
    }
}

/**
 * @brief  ��������SPI FLASH
 * @param  ��
 * @retval ��
 */
void spi_flash_chip_erase(void)
{
    /* ����FLASHдʹ������ */
    spi_flash_write_enable();

    /* ���� Erase */
    /* ѡ��FLASH: CS�͵�ƽ */
    SPI_FLASH_CS_LOW();
    /* �����������ָ��*/
    spi_flash_send_byte(W25X_ChipErase);
    /* ֹͣ�ź� FLASH: CS �ߵ�ƽ */
    SPI_FLASH_CS_HIGH();

    /* �ȴ��������*/
    spi_flash_wait_for_write_end();
}

/**
 * @brief   ��FLASH��ҳд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
 * @param   pBuffer��Ҫд�����ݵ�ָ��
 * @param   WriteAddr��д���ַ
 * @param   NumByteToWrite��д�����ݳ��ȣ�����С�ڵ���SPI_FLASH_PerWritePageSize
 * @retval  ��
 */
void spi_flash_page_write(uint8_t* pBuffer,
                          uint32_t WriteAddr,
                          uint16_t NumByteToWrite)
{
    /* ����FLASHдʹ������ */
    spi_flash_write_enable();

    /* ѡ��FLASH: CS�͵�ƽ */
    SPI_FLASH_CS_LOW();
    /* дҳдָ��*/
    spi_flash_send_byte(W25X_PageProgram);
    /*����д��ַ�ĸ�λ*/
    spi_flash_send_byte((WriteAddr & 0xFF0000) >> 16);
    /*����д��ַ����λ*/
    spi_flash_send_byte((WriteAddr & 0xFF00) >> 8);
    /*����д��ַ�ĵ�λ*/
    spi_flash_send_byte(WriteAddr & 0xFF);

    if(NumByteToWrite > SPI_FLASH_PerWritePageSize) {
        NumByteToWrite = SPI_FLASH_PerWritePageSize;
        ERR("SPI_FLASH_PageWrite too large!");
    }

    /* д������*/
    while(NumByteToWrite--) {
        /* ���͵�ǰҪд����ֽ����� */
        spi_flash_send_byte(*pBuffer);
        /* ָ����һ�ֽ����� */
        pBuffer++;
    }

    /* ֹͣ�ź� FLASH: CS �ߵ�ƽ */
    SPI_FLASH_CS_HIGH();

    /* �ȴ�д�����*/
    spi_flash_wait_for_write_end();
}

/**
 * @brief   ��FLASHд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
 * @param   pBuffer��Ҫд�����ݵ�ָ��
 * @param   WriteAddr��д���ַ
 * @param   NumByteToWrite��д�����ݳ���
 * @retval  ��
 */
void spi_flash_buffer_write(uint8_t* pBuffer,
                            uint32_t WriteAddr,
                            uint16_t NumByteToWrite)
{
    uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

    /*mod�������࣬��writeAddr��SPI_FLASH_PageSize��������������AddrֵΪ0*/
    Addr = WriteAddr % SPI_FLASH_PageSize;

    /*��count������ֵ���պÿ��Զ��뵽ҳ��ַ*/
    count = SPI_FLASH_PageSize - Addr;
    /*�����Ҫд��������ҳ*/
    NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
    /*mod�������࣬�����ʣ�಻��һҳ���ֽ���*/
    NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

    /* Addr=0,��WriteAddr �պð�ҳ���� aligned  */
    if(Addr == 0) {
        /* NumByteToWrite < SPI_FLASH_PageSize */
        if(NumOfPage == 0) {
            spi_flash_page_write(pBuffer, WriteAddr, NumByteToWrite);
        } else { /* NumByteToWrite > SPI_FLASH_PageSize */
            /*�Ȱ�����ҳ��д��*/
            while(NumOfPage--) {
                spi_flash_page_write(pBuffer, WriteAddr, SPI_FLASH_PageSize);
                WriteAddr +=  SPI_FLASH_PageSize;
                pBuffer += SPI_FLASH_PageSize;
            }

            /*���ж���Ĳ���һҳ�����ݣ�����д��*/
            spi_flash_page_write(pBuffer, WriteAddr, NumOfSingle);
        }
    }
    /* ����ַ�� SPI_FLASH_PageSize ������  */
    else {
        /* NumByteToWrite < SPI_FLASH_PageSize */
        if(NumOfPage == 0) {
            /*��ǰҳʣ���count��λ�ñ�NumOfSingleС��һҳд����*/
            if(NumOfSingle > count) {
                temp = NumOfSingle - count;
                /*��д����ǰҳ*/
                spi_flash_page_write(pBuffer, WriteAddr, count);

                WriteAddr +=  count;
                pBuffer += count;
                /*��дʣ�������*/
                spi_flash_page_write(pBuffer, WriteAddr, temp);
            } else { /*��ǰҳʣ���count��λ����д��NumOfSingle������*/
                spi_flash_page_write(pBuffer, WriteAddr, NumByteToWrite);
            }
        } else { /* NumByteToWrite > SPI_FLASH_PageSize */
            /*��ַ����������count�ֿ������������������*/
            NumByteToWrite -= count;
            NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
            NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

            /* ��д��count�����ݣ�Ϊ��������һ��Ҫд�ĵ�ַ���� */
            spi_flash_page_write(pBuffer, WriteAddr, count);

            /* ���������ظ���ַ�������� */
            WriteAddr +=  count;
            pBuffer += count;

            /*������ҳ��д��*/
            while(NumOfPage--) {
                spi_flash_page_write(pBuffer, WriteAddr, SPI_FLASH_PageSize);
                WriteAddr +=  SPI_FLASH_PageSize;
                pBuffer += SPI_FLASH_PageSize;
            }

            /*���ж���Ĳ���һҳ�����ݣ�����д��*/
            if(NumOfSingle != 0) {
                spi_flash_page_write(pBuffer, WriteAddr, NumOfSingle);
            }
        }
    }
}

/**
 * @brief   ��ȡFLASH����
 * @param   pBuffer���洢�������ݵ�ָ��
 * @param   ReadAddr����ȡ��ַ
 * @param   NumByteToRead����ȡ���ݳ���
 * @retval  ��
 */
void spi_flash_buffer_read(uint8_t* pBuffer,
                           uint32_t ReadAddr,
                           uint16_t NumByteToRead)
{
    /* ѡ��FLASH: CS�͵�ƽ */
    SPI_FLASH_CS_LOW();

    /* ���� �� ָ�� */
    spi_flash_send_byte(W25X_ReadData);

    /* ���� �� ��ַ��λ */
    spi_flash_send_byte((ReadAddr & 0xFF0000) >> 16);
    /* ���� �� ��ַ��λ */
    spi_flash_send_byte((ReadAddr & 0xFF00) >> 8);
    /* ���� �� ��ַ��λ */
    spi_flash_send_byte(ReadAddr & 0xFF);

    /* ��ȡ���� */
    while(NumByteToRead--) {  /* while there is data to be read */
        /* ��ȡһ���ֽ�*/
        *pBuffer = spi_flash_send_byte(Dummy_Byte);
        /* ָ����һ���ֽڻ����� */
        pBuffer++;
    }

    /* ֹͣ�ź� FLASH: CS �ߵ�ƽ */
    SPI_FLASH_CS_HIGH();
}

/**
 * @brief   ��ȡFLASH ID
 * @param   ��
 * @retval  FLASH ID
 */
uint32_t spi_flash_read_id(void)
{
    uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

    /* ��ʼͨѶ��CS�͵�ƽ */
    SPI_FLASH_CS_LOW();

    /* ����JEDECָ���ȡID */
    spi_flash_send_byte(W25X_JedecDeviceID);

    /* ��ȡһ���ֽ����� */
    Temp0 = spi_flash_send_byte(Dummy_Byte);

    /* ��ȡһ���ֽ����� */
    Temp1 = spi_flash_send_byte(Dummy_Byte);

    /* ��ȡһ���ֽ����� */
    Temp2 = spi_flash_send_byte(Dummy_Byte);

    /* ֹͣͨѶ��CS�ߵ�ƽ */
    SPI_FLASH_CS_HIGH();

    /*�����������������Ϊ�����ķ���ֵ*/
    Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

    return Temp;
}

/**
 * @brief   ��ȡFLASH Device ID
 * @param   ��
 * @retval  FLASH Device ID
 */
uint32_t spi_flash_read_device_id(void)
{
    uint32_t Temp = 0;

    /* Select the FLASH: Chip Select low */
    SPI_FLASH_CS_LOW();

    /* Send "RDID " instruction */
    spi_flash_send_byte(W25X_DeviceID);
    spi_flash_send_byte(Dummy_Byte);
    spi_flash_send_byte(Dummy_Byte);
    spi_flash_send_byte(Dummy_Byte);

    /* Read a byte from the FLASH */
    Temp = spi_flash_send_byte(Dummy_Byte);

    /* Deselect the FLASH: Chip Select high */
    SPI_FLASH_CS_HIGH();

    return Temp;
}

/**
 * @brief  ʹ��SPI����һ���ֽڵ�����
 * @param  byte��Ҫ���͵�����
 * @retval ���ؽ��յ�������
 */
uint8_t spi_flash_send_byte(uint8_t byte)
{
    uint32_t SPITimeout = SPIT_FLAG_TIMEOUT;

    /* �ȴ����ͻ�����Ϊ�գ�TXE�¼� */
    while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_TXE) == RESET) {
        if((SPITimeout--) == 0) return spi_timeout_user_callback(0);
    }

    /* д�����ݼĴ�������Ҫд�������д�뷢�ͻ����� */
    SPI_I2S_SendData(FLASH_SPIx, byte);

    SPITimeout = SPIT_FLAG_TIMEOUT;

    /* �ȴ����ջ������ǿգ�RXNE�¼� */
    while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_RXNE) == RESET) {
        if((SPITimeout--) == 0) return spi_timeout_user_callback(1);
    }

    /* ��ȡ���ݼĴ�������ȡ���ջ��������� */
    return SPI_I2S_ReceiveData(FLASH_SPIx);
}

/**
 * @brief  ��FLASH���� дʹ�� ����
 * @param  none
 * @retval none
 */
void spi_flash_write_enable(void)
{
    /* ͨѶ��ʼ��CS�� */
    SPI_FLASH_CS_LOW();

    /* ����дʹ������*/
    spi_flash_send_byte(W25X_WriteEnable);

    /*ͨѶ������CS�� */
    SPI_FLASH_CS_HIGH();
}

/**
 * @brief  �ȴ�WIP(BUSY)��־����0�����ȴ���FLASH�ڲ�����д�����
 * @param  none
 * @retval none
 */
void spi_flash_wait_for_write_end(void)
{
    uint8_t FLASH_Status = 0;

    /* ѡ�� FLASH: CS �� */
    SPI_FLASH_CS_LOW();

    /* ���� ��״̬�Ĵ��� ���� */
    spi_flash_send_byte(W25X_ReadStatusReg);

    /* ��FLASHæµ����ȴ� */
    do {
        /* ��ȡFLASHоƬ��״̬�Ĵ��� */
        FLASH_Status = spi_flash_send_byte(Dummy_Byte);
        WATCH_DOG_FEED();   /* ��ֹSPI_FLASH��Ƭ������ʱ��������¿��Ź���λ */
    } while((FLASH_Status & WIP_Flag) == SET);  /* ����д���־ */

    /* ֹͣ�ź�  FLASH: CS �� */
    SPI_FLASH_CS_HIGH();
}


/**
 * @brief  SPI_FLASH�������ģʽ
 * @param  none
 * @retval none
 */
void spi_flash_power_down(void)
{
    /* ͨѶ��ʼ��CS�� */
    SPI_FLASH_CS_LOW();

    /* ���� ���� ���� */
    spi_flash_send_byte(W25X_PowerDown);

    /*ͨѶ������CS�� */
    SPI_FLASH_CS_HIGH();
}

/**
 * @brief  ����SPI_FLASH
 * @param  none
 * @retval none
 */
void spi_flash_wakeup(void)
{
    /*ѡ�� FLASH: CS �� */
    SPI_FLASH_CS_LOW();

    /* ���� �ϵ� ���� */
    spi_flash_send_byte(W25X_ReleasePowerDown);

    /* ֹͣ�ź� FLASH: CS �� */
    SPI_FLASH_CS_HIGH();
}


/*********************************************END OF FILE**********************/
