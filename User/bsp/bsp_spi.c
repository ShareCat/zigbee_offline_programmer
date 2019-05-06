/**
  ******************************************************************************
  * @file:      bsp_spi.c
  * @author:    Cat（孙关平）
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
  * @brief  等待超时回调函数
  * @param  None.
  * @retval None.
  */
static uint16_t spi_timeout_user_callback(uint8_t errorCode)
{
    /* 等待超时后的处理,输出错误信息 */
    ERRA("SPI time out! errorCode = %d", errorCode);
    return 0;
}

/* --------------------------------------------------------------------- SPI1 */
#if 0
void spi_init_w5500(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    /* 使能SPI时钟 */
    RCC_APB2PeriphClockCmd(W5500_SPI_CLK, ENABLE);

    /* 使能SPI引脚相关的时钟 */
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

    /*定义RESET引脚*/
    GPIO_InitStructure.GPIO_Pin = W5500_RST_PIN;        /*选择要控制的GPIO引脚*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   /*设置引脚速率为50MHz */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    /*设置引脚模式为通用推挽输出*/
    GPIO_Init(W5500_RST_PORT, &GPIO_InitStructure);     /*调用库函数，初始化GPIO*/
    GPIO_SetBits(W5500_RST_PORT, W5500_RST_PIN);

    /*定义INT引脚*/
    GPIO_InitStructure.GPIO_Pin = W5500_INT_PIN;        /*选择要控制的GPIO引脚*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   /*设置引脚速率为50MHz*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       /*设置引脚模式为通用推挽模拟上拉输入*/
    GPIO_Init(W5500_INT_PORT, &GPIO_InitStructure);     /*调用库函数，初始化GPIO*/

    spi_init_w5500();                                   /*初始化驱动W5500的SPI接口*/
}


/**
 * @brief  SPI片选控制
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
 * @brief  RST硬件复位控制
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
 * @brief  使用SPI发送一个字节的数据
 * @param  byte：要发送的数据
 * @retval 返回接收到的数据
 */
uint8_t spi_send_byte_w5500(uint8_t byte)
{
    uint32_t SPITimeout = SPIT_FLAG_TIMEOUT;

    /* 等待发送缓冲区为空，TXE事件 */
    while(SPI_I2S_GetFlagStatus(W5500_SPIx, SPI_I2S_FLAG_TXE) == RESET) {
        if((SPITimeout--) == 0) return spi_timeout_user_callback(0);
    }

    /* 写入数据寄存器，把要写入的数据写入发送缓冲区 */
    SPI_I2S_SendData(W5500_SPIx, byte);

    SPITimeout = SPIT_FLAG_TIMEOUT;

    /* 等待接收缓冲区非空，RXNE事件 */
    while(SPI_I2S_GetFlagStatus(W5500_SPIx, SPI_I2S_FLAG_RXNE) == RESET) {
        if((SPITimeout--) == 0) return spi_timeout_user_callback(1);
    }

    /* 读取数据寄存器，获取接收缓冲区数据 */
    return SPI_I2S_ReceiveData(W5500_SPIx);
}
#endif




/* --------------------------------------------------------------------- SPI2 */

/**
  * @brief  SPI FLASH初始化
  * @param  无
  * @retval 无
  */
void spi_flash_init(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 使能SPI时钟 */
    RCC_APB2PeriphClockCmd(FLASH_SPI_CLK, ENABLE);

    /* 使能SPI引脚相关的时钟 */
    RCC_APB2PeriphClockCmd(FLASH_SPI_CS_CLK |
                           FLASH_SPI_SCK_CLK |
                           FLASH_SPI_MISO_CLK |
                           FLASH_SPI_MOSI_CLK, ENABLE);

    /* 配置SPI的 CS引脚，普通IO即可 */
    GPIO_InitStructure.GPIO_Pin = FLASH_SPI_CS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(FLASH_SPI_CS_PORT, &GPIO_InitStructure);

    /* 配置SPI的 SCK引脚*/
    GPIO_InitStructure.GPIO_Pin = FLASH_SPI_SCK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(FLASH_SPI_SCK_PORT, &GPIO_InitStructure);

    /* 配置SPI的 MISO引脚*/
    GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MISO_PIN;
    GPIO_Init(FLASH_SPI_MISO_PORT, &GPIO_InitStructure);

    /* 配置SPI的 MOSI引脚*/
    GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MOSI_PIN;
    GPIO_Init(FLASH_SPI_MOSI_PORT, &GPIO_InitStructure);

    /* 停止信号 FLASH: CS引脚高电平*/
    SPI_FLASH_CS_HIGH();

    /* SPI 模式配置 */
    // FLASH芯片 支持SPI模式0及模式3，据此设置CPOL CPHA
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

    /* 使能 SPI  */
    SPI_Cmd(FLASH_SPIx, ENABLE);
}

/**
 * @brief  擦除FLASH扇区
 * @param  SectorAddr：要擦除的扇区物理地址，不是扇区编号
 * @retval 无
 */
void spi_flash_sector_erase(uint32_t SectorAddr)
{
    /* 发送FLASH写使能命令 */
    spi_flash_write_enable();
    spi_flash_wait_for_write_end();
    /* 擦除扇区 */
    /* 选择FLASH: CS低电平 */
    SPI_FLASH_CS_LOW();

    /* 发送扇区擦除指令*/
    spi_flash_send_byte(W25X_SectorErase);
    /*发送擦除扇区地址的高位*/
    spi_flash_send_byte((SectorAddr & 0xFF0000) >> 16);
    /* 发送擦除扇区地址的中位 */
    spi_flash_send_byte((SectorAddr & 0xFF00) >> 8);
    /* 发送擦除扇区地址的低位 */
    spi_flash_send_byte(SectorAddr & 0xFF);
    /* 停止信号 FLASH: CS 高电平 */
    SPI_FLASH_CS_HIGH();
    /* 等待擦除完毕*/
    spi_flash_wait_for_write_end();
}

/**
 * @brief  擦除FLASH块
 * @param  SectorAddr：要擦除的块地址
 * @retval 无
 */
void spi_flash_block_erase(uint32_t BlockAddr)
{
    /* 发送FLASH写使能命令 */
    spi_flash_write_enable();
    spi_flash_wait_for_write_end();
    /* 擦除扇区 */
    /* 选择FLASH: CS低电平 */
    SPI_FLASH_CS_LOW();

    /* 发送扇区擦除指令*/
    spi_flash_send_byte(W25X_BlockErase);
    /*发送擦除扇区地址的高位*/
    spi_flash_send_byte((BlockAddr & 0xFF0000) >> 16);
    /* 发送擦除扇区地址的中位 */
    spi_flash_send_byte((BlockAddr & 0xFF00) >> 8);
    /* 发送擦除扇区地址的低位 */
    spi_flash_send_byte(BlockAddr & 0xFF);
    /* 停止信号 FLASH: CS 高电平 */
    SPI_FLASH_CS_HIGH();
    /* 等待擦除完毕*/
    spi_flash_wait_for_write_end();
}

/**
* @brief   根据提供的要擦除地址和擦除大小，按块擦除
* @param   无
* @retval  FLASH ID
*/
void spi_flash_addr_erase(int addr, int size)
{
    uint32_t block_pos;
    uint32_t block_addr;
    uint32_t i;
    uint32_t sec_num;                           /* 要擦除块个数 */

    i = size % SPI_FLASH_BlockSize;
    sec_num = size / SPI_FLASH_BlockSize;

    if(i) sec_num++;

    block_pos = addr / SPI_FLASH_BlockSize;     /* 块地址 (计算给定的地址处于第几个块) */

    for(i = 0; i < sec_num; i++) {
        block_addr = block_pos * SPI_FLASH_BlockSize;   /* 根据扇区计算块的起始地址 */
        spi_flash_block_erase(block_addr);
        block_pos++;
    }
}

/**
 * @brief  擦除整个SPI FLASH
 * @param  无
 * @retval 无
 */
void spi_flash_chip_erase(void)
{
    /* 发送FLASH写使能命令 */
    spi_flash_write_enable();

    /* 整块 Erase */
    /* 选择FLASH: CS低电平 */
    SPI_FLASH_CS_LOW();
    /* 发送整块擦除指令*/
    spi_flash_send_byte(W25X_ChipErase);
    /* 停止信号 FLASH: CS 高电平 */
    SPI_FLASH_CS_HIGH();

    /* 等待擦除完毕*/
    spi_flash_wait_for_write_end();
}

/**
 * @brief   对FLASH按页写入数据，调用本函数写入数据前需要先擦除扇区
 * @param   pBuffer，要写入数据的指针
 * @param   WriteAddr，写入地址
 * @param   NumByteToWrite，写入数据长度，必须小于等于SPI_FLASH_PerWritePageSize
 * @retval  无
 */
void spi_flash_page_write(uint8_t* pBuffer,
                          uint32_t WriteAddr,
                          uint16_t NumByteToWrite)
{
    /* 发送FLASH写使能命令 */
    spi_flash_write_enable();

    /* 选择FLASH: CS低电平 */
    SPI_FLASH_CS_LOW();
    /* 写页写指令*/
    spi_flash_send_byte(W25X_PageProgram);
    /*发送写地址的高位*/
    spi_flash_send_byte((WriteAddr & 0xFF0000) >> 16);
    /*发送写地址的中位*/
    spi_flash_send_byte((WriteAddr & 0xFF00) >> 8);
    /*发送写地址的低位*/
    spi_flash_send_byte(WriteAddr & 0xFF);

    if(NumByteToWrite > SPI_FLASH_PerWritePageSize) {
        NumByteToWrite = SPI_FLASH_PerWritePageSize;
        ERR("SPI_FLASH_PageWrite too large!");
    }

    /* 写入数据*/
    while(NumByteToWrite--) {
        /* 发送当前要写入的字节数据 */
        spi_flash_send_byte(*pBuffer);
        /* 指向下一字节数据 */
        pBuffer++;
    }

    /* 停止信号 FLASH: CS 高电平 */
    SPI_FLASH_CS_HIGH();

    /* 等待写入完毕*/
    spi_flash_wait_for_write_end();
}

/**
 * @brief   对FLASH写入数据，调用本函数写入数据前需要先擦除扇区
 * @param   pBuffer，要写入数据的指针
 * @param   WriteAddr，写入地址
 * @param   NumByteToWrite，写入数据长度
 * @retval  无
 */
void spi_flash_buffer_write(uint8_t* pBuffer,
                            uint32_t WriteAddr,
                            uint16_t NumByteToWrite)
{
    uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

    /*mod运算求余，若writeAddr是SPI_FLASH_PageSize整数倍，运算结果Addr值为0*/
    Addr = WriteAddr % SPI_FLASH_PageSize;

    /*差count个数据值，刚好可以对齐到页地址*/
    count = SPI_FLASH_PageSize - Addr;
    /*计算出要写多少整数页*/
    NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
    /*mod运算求余，计算出剩余不满一页的字节数*/
    NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

    /* Addr=0,则WriteAddr 刚好按页对齐 aligned  */
    if(Addr == 0) {
        /* NumByteToWrite < SPI_FLASH_PageSize */
        if(NumOfPage == 0) {
            spi_flash_page_write(pBuffer, WriteAddr, NumByteToWrite);
        } else { /* NumByteToWrite > SPI_FLASH_PageSize */
            /*先把整数页都写了*/
            while(NumOfPage--) {
                spi_flash_page_write(pBuffer, WriteAddr, SPI_FLASH_PageSize);
                WriteAddr +=  SPI_FLASH_PageSize;
                pBuffer += SPI_FLASH_PageSize;
            }

            /*若有多余的不满一页的数据，把它写完*/
            spi_flash_page_write(pBuffer, WriteAddr, NumOfSingle);
        }
    }
    /* 若地址与 SPI_FLASH_PageSize 不对齐  */
    else {
        /* NumByteToWrite < SPI_FLASH_PageSize */
        if(NumOfPage == 0) {
            /*当前页剩余的count个位置比NumOfSingle小，一页写不完*/
            if(NumOfSingle > count) {
                temp = NumOfSingle - count;
                /*先写满当前页*/
                spi_flash_page_write(pBuffer, WriteAddr, count);

                WriteAddr +=  count;
                pBuffer += count;
                /*再写剩余的数据*/
                spi_flash_page_write(pBuffer, WriteAddr, temp);
            } else { /*当前页剩余的count个位置能写完NumOfSingle个数据*/
                spi_flash_page_write(pBuffer, WriteAddr, NumByteToWrite);
            }
        } else { /* NumByteToWrite > SPI_FLASH_PageSize */
            /*地址不对齐多出的count分开处理，不加入这个运算*/
            NumByteToWrite -= count;
            NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
            NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

            /* 先写完count个数据，为的是让下一次要写的地址对齐 */
            spi_flash_page_write(pBuffer, WriteAddr, count);

            /* 接下来就重复地址对齐的情况 */
            WriteAddr +=  count;
            pBuffer += count;

            /*把整数页都写了*/
            while(NumOfPage--) {
                spi_flash_page_write(pBuffer, WriteAddr, SPI_FLASH_PageSize);
                WriteAddr +=  SPI_FLASH_PageSize;
                pBuffer += SPI_FLASH_PageSize;
            }

            /*若有多余的不满一页的数据，把它写完*/
            if(NumOfSingle != 0) {
                spi_flash_page_write(pBuffer, WriteAddr, NumOfSingle);
            }
        }
    }
}

/**
 * @brief   读取FLASH数据
 * @param   pBuffer，存储读出数据的指针
 * @param   ReadAddr，读取地址
 * @param   NumByteToRead，读取数据长度
 * @retval  无
 */
void spi_flash_buffer_read(uint8_t* pBuffer,
                           uint32_t ReadAddr,
                           uint16_t NumByteToRead)
{
    /* 选择FLASH: CS低电平 */
    SPI_FLASH_CS_LOW();

    /* 发送 读 指令 */
    spi_flash_send_byte(W25X_ReadData);

    /* 发送 读 地址高位 */
    spi_flash_send_byte((ReadAddr & 0xFF0000) >> 16);
    /* 发送 读 地址中位 */
    spi_flash_send_byte((ReadAddr & 0xFF00) >> 8);
    /* 发送 读 地址低位 */
    spi_flash_send_byte(ReadAddr & 0xFF);

    /* 读取数据 */
    while(NumByteToRead--) {  /* while there is data to be read */
        /* 读取一个字节*/
        *pBuffer = spi_flash_send_byte(Dummy_Byte);
        /* 指向下一个字节缓冲区 */
        pBuffer++;
    }

    /* 停止信号 FLASH: CS 高电平 */
    SPI_FLASH_CS_HIGH();
}

/**
 * @brief   读取FLASH ID
 * @param   无
 * @retval  FLASH ID
 */
uint32_t spi_flash_read_id(void)
{
    uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

    /* 开始通讯：CS低电平 */
    SPI_FLASH_CS_LOW();

    /* 发送JEDEC指令，读取ID */
    spi_flash_send_byte(W25X_JedecDeviceID);

    /* 读取一个字节数据 */
    Temp0 = spi_flash_send_byte(Dummy_Byte);

    /* 读取一个字节数据 */
    Temp1 = spi_flash_send_byte(Dummy_Byte);

    /* 读取一个字节数据 */
    Temp2 = spi_flash_send_byte(Dummy_Byte);

    /* 停止通讯：CS高电平 */
    SPI_FLASH_CS_HIGH();

    /*把数据组合起来，作为函数的返回值*/
    Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

    return Temp;
}

/**
 * @brief   读取FLASH Device ID
 * @param   无
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
 * @brief  使用SPI发送一个字节的数据
 * @param  byte：要发送的数据
 * @retval 返回接收到的数据
 */
uint8_t spi_flash_send_byte(uint8_t byte)
{
    uint32_t SPITimeout = SPIT_FLAG_TIMEOUT;

    /* 等待发送缓冲区为空，TXE事件 */
    while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_TXE) == RESET) {
        if((SPITimeout--) == 0) return spi_timeout_user_callback(0);
    }

    /* 写入数据寄存器，把要写入的数据写入发送缓冲区 */
    SPI_I2S_SendData(FLASH_SPIx, byte);

    SPITimeout = SPIT_FLAG_TIMEOUT;

    /* 等待接收缓冲区非空，RXNE事件 */
    while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_RXNE) == RESET) {
        if((SPITimeout--) == 0) return spi_timeout_user_callback(1);
    }

    /* 读取数据寄存器，获取接收缓冲区数据 */
    return SPI_I2S_ReceiveData(FLASH_SPIx);
}

/**
 * @brief  向FLASH发送 写使能 命令
 * @param  none
 * @retval none
 */
void spi_flash_write_enable(void)
{
    /* 通讯开始：CS低 */
    SPI_FLASH_CS_LOW();

    /* 发送写使能命令*/
    spi_flash_send_byte(W25X_WriteEnable);

    /*通讯结束：CS高 */
    SPI_FLASH_CS_HIGH();
}

/**
 * @brief  等待WIP(BUSY)标志被置0，即等待到FLASH内部数据写入完毕
 * @param  none
 * @retval none
 */
void spi_flash_wait_for_write_end(void)
{
    uint8_t FLASH_Status = 0;

    /* 选择 FLASH: CS 低 */
    SPI_FLASH_CS_LOW();

    /* 发送 读状态寄存器 命令 */
    spi_flash_send_byte(W25X_ReadStatusReg);

    /* 若FLASH忙碌，则等待 */
    do {
        /* 读取FLASH芯片的状态寄存器 */
        FLASH_Status = spi_flash_send_byte(Dummy_Byte);
        WATCH_DOG_FEED();   /* 防止SPI_FLASH整片擦除的时间过长导致看门狗复位 */
    } while((FLASH_Status & WIP_Flag) == SET);  /* 正在写入标志 */

    /* 停止信号  FLASH: CS 高 */
    SPI_FLASH_CS_HIGH();
}


/**
 * @brief  SPI_FLASH进入掉电模式
 * @param  none
 * @retval none
 */
void spi_flash_power_down(void)
{
    /* 通讯开始：CS低 */
    SPI_FLASH_CS_LOW();

    /* 发送 掉电 命令 */
    spi_flash_send_byte(W25X_PowerDown);

    /*通讯结束：CS高 */
    SPI_FLASH_CS_HIGH();
}

/**
 * @brief  唤醒SPI_FLASH
 * @param  none
 * @retval none
 */
void spi_flash_wakeup(void)
{
    /*选择 FLASH: CS 低 */
    SPI_FLASH_CS_LOW();

    /* 发送 上电 命令 */
    spi_flash_send_byte(W25X_ReleasePowerDown);

    /* 停止信号 FLASH: CS 高 */
    SPI_FLASH_CS_HIGH();
}


/*********************************************END OF FILE**********************/
