/**
  ******************************************************************************
  * @file:      bsp_spi.h
  * @author:    Cat（孙关平）
  * @version:   V1.0
  * @date:      2018-1-26
  * @brief:
  * @attention:
  ******************************************************************************
  */


#ifndef __BSP_SPI_H
#define __BSP_SPI_H

#include "global.h"
#include <stdio.h>

/* ------------------------------------------------------------------- IO map */
/*
 *  SPI1 用于驱动w5500
 *  SPI_CS      --> PA4
 *  SPI_SCK     --> PA5
 *  SPI_MISO    --> PA6
 *  SPI_MOSI    --> PA7
 *  INT         --> PE3 中断引脚
 *  RESET       --> PE4 复位引脚
 *
 *******************************
 *
 *  SPI2 用于驱动spi flash
 *  SPI_CS      --> PB12
 *  SPI_SCK     --> PB13
 *  SPI_MISO    --> PB14
 *  SPI_MOSI    --> PB15
**/

/* --------------------------------------------------------------------- SPI1 */
#if 0
    #define W5500_SPIx                  SPI1
    #define W5500_SPI_CLK               RCC_APB2Periph_SPI1

    #define W5500_SPI_CS_PIN            GPIO_Pin_4                  /* PA.4 */
    #define W5500_SPI_CS_PORT           GPIOA
    #define W5500_SPI_CS_CLK            RCC_APB2Periph_GPIOA

    #define W5500_SPI_SCK_PIN           GPIO_Pin_5                  /* PA.5 */
    #define W5500_SPI_SCK_PORT          GPIOA
    #define W5500_SPI_SCK_CLK           RCC_APB2Periph_GPIOA

    #define W5500_SPI_MISO_PIN          GPIO_Pin_6                  /* PA.6 */
    #define W5500_SPI_MISO_PORT         GPIOA
    #define W5500_SPI_MISO_CLK          RCC_APB2Periph_GPIOA

    #define W5500_SPI_MOSI_PIN          GPIO_Pin_7                  /* PA.7 */
    #define W5500_SPI_MOSI_PORT         GPIOA
    #define W5500_SPI_MOSI_CLK          RCC_APB2Periph_GPIOA

    #define W5500_INT_PIN               GPIO_Pin_3                  /* PE.3 */
    #define W5500_INT_PORT              GPIOE
    #define W5500_INT_CLK               RCC_APB2Periph_GPIOE

    #define W5500_RST_PIN               GPIO_Pin_4                  /* PE.4 */
    #define W5500_RST_PORT              GPIOE
    #define W5500_RST_CLK               RCC_APB2Periph_GPIOE

    #define W5500_DUMMY_BYTE            0xFF

    extern void spi_cs_ctrl_w5500(FunctionalState cmd);
    extern void rst_ctrl_w5500(FunctionalState cmd);
    extern uint8_t spi_send_byte_w5500(uint8_t byte);
    extern void gpio_config_w5500(void);
#endif
/* --------------------------------------------------------------------- SPI2 */
//#define  sFLASH_ID                  0xEF3015    //W25X16
//#define  sFLASH_ID                  0xEF4015    //W25Q16
//#define  sFLASH_ID                  0XEF4017    //W25Q64
#define  sFLASH_ID                  0XEF4018    //W25Q128

#define SPI_FLASH_PageSize          256
#define SPI_FLASH_PerWritePageSize  256
#define SPI_FLASH_SectorSize        4096
#define SPI_FLASH_BlockSize         65536

/*
    共16Mbyte的SPI_FLASH
    第一部分：1Mbyte用于存储用户数据
    第二部分：1Mbyte用于存储升级固件的备份
    第三部分：14Mbyte用于fatfs文件系统
*/
#define DATABASE_SECTOR_SIZE        SPI_FLASH_SectorSize
#define DATABASE_SECTOR_START       0
#define DATABASE_SECTOR_COUNT       256                         //1Mbyte

#define FM_BACKUP_SECTOR_SIZE       SPI_FLASH_SectorSize
#define FM_BACKUP_SECTOR_START      (DATABASE_SECTOR_START + DATABASE_SECTOR_COUNT)
#define FM_BACKUP_SECTOR_COUNT      256                         //1Mbyte

#define FATFS_SECTOR_SIZE           SPI_FLASH_SectorSize
#define FATFS_SECTOR_COUNT          (3584)                      //14Mbyte
#define FATFS_SECTOR_START          (DATABASE_SECTOR_START + DATABASE_SECTOR_COUNT + FM_BACKUP_SECTOR_COUNT)
#define FATFS_ADDR_START            (FATFS_SECTOR_START * SPI_FLASH_SectorSize)

/*命令定义-开头*******************************/
#define W25X_WriteEnable            0x06
#define W25X_WriteDisable           0x04
#define W25X_ReadStatusReg          0x05
#define W25X_WriteStatusReg         0x01
#define W25X_ReadData               0x03
#define W25X_FastReadData           0x0B
#define W25X_FastReadDual           0x3B
#define W25X_PageProgram            0x02
#define W25X_BlockErase             0xD8
#define W25X_SectorErase            0x20
#define W25X_ChipErase              0xC7
#define W25X_PowerDown              0xB9
#define W25X_ReleasePowerDown       0xAB
#define W25X_DeviceID               0xAB
#define W25X_ManufactDeviceID       0x90
#define W25X_JedecDeviceID          0x9F

/* WIP(busy)标志，FLASH内部正在写入 */
#define WIP_Flag                    0x01
#define Dummy_Byte                  0xFF
/*命令定义-结尾*******************************/


/*SPI接口定义-开头****************************/
#define FLASH_SPIx                  SPI1
#define FLASH_SPI_CLK               RCC_APB2Periph_SPI1

/* CS(NSS)引脚，普通GPIO即可 */
#define FLASH_SPI_CS_PIN            GPIO_Pin_4                  /* PA.4 */
#define FLASH_SPI_CS_PORT           GPIOA
#define FLASH_SPI_CS_CLK            RCC_APB2Periph_GPIOA

/* SCK引脚 */
#define FLASH_SPI_SCK_PIN           GPIO_Pin_5                  /* PA.5 */
#define FLASH_SPI_SCK_PORT          GPIOA
#define FLASH_SPI_SCK_CLK           RCC_APB2Periph_GPIOA

/* MISO引脚 */
#define FLASH_SPI_MISO_PIN          GPIO_Pin_6                  /* PA.6 */
#define FLASH_SPI_MISO_PORT         GPIOA
#define FLASH_SPI_MISO_CLK          RCC_APB2Periph_GPIOA

/* MOSI引脚 */
#define FLASH_SPI_MOSI_PIN          GPIO_Pin_7                  /* PA.7 */
#define FLASH_SPI_MOSI_PORT         GPIOA
#define FLASH_SPI_MOSI_CLK          RCC_APB2Periph_GPIOA

#define SPI_FLASH_CS_LOW()      IO_LOW(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN)
#define SPI_FLASH_CS_HIGH()     IO_HIGH(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN)

/*SPI接口定义-结尾****************************/

/*等待超时时间*/
#define SPIT_FLAG_TIMEOUT           ((uint32_t)0x1000)
#define SPIT_LONG_TIMEOUT           ((uint32_t)(10 * SPIT_FLAG_TIMEOUT))

void spi_flash_init(void);
void spi_flash_sector_erase(uint32_t SectorAddr);
void spi_flash_block_erase(uint32_t BlockAddr);
void spi_flash_addr_erase(int addr, int size);
void spi_flash_chip_erase(void);
void spi_flash_page_write(uint8_t* pBuffer,
                          uint32_t WriteAddr,
                          uint16_t NumByteToWrite);
void spi_flash_buffer_write(uint8_t* pBuffer,
                            uint32_t WriteAddr,
                            uint16_t NumByteToWrite);
void spi_flash_buffer_read(uint8_t* pBuffer,
                           uint32_t ReadAddr,
                           uint16_t NumByteToRead);
uint32_t spi_flash_read_id(void);
uint32_t spi_flash_read_device_id(void);
uint8_t spi_flash_send_byte(uint8_t byte);
void spi_flash_write_enable(void);
void spi_flash_wait_for_write_end(void);
void spi_flash_power_down(void);
void spi_flash_wakeup(void);


#endif /* __BSP_SPI_H */

