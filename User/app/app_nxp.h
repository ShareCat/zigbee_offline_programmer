/**
  ******************************************************************************
  * @file:      app_nxp.h
  * @author:    Cat（孙关平）
  * @version:   V1.0
  * @date:      2019-1-1
  * @brief:     
  * @attention:
  ******************************************************************************
  */


#ifndef __APP_NXP_H
#define	__APP_NXP_H

#include "global.h"



#ifdef NXP_CLI_DEBUG
/* 命令行的帮助信息 */
#define NXP_CMD \
    "[nxp]\r\n"\
    " * nxp info: nxp information\r\n"\
    " * nxp dbg 1: nxp debug on\r\n"\
    " * nxp dbg 0: nxp debug off\r\n"\
    "\r\n"

extern uint8_t cli_nxp(void *para, uint8_t len);
#endif


#define NXP_JN5168 "NXP_JN5168"
#define NXP_JN5169 "NXP_JN5169"
#define NXP_JN5148 "NXP_JN5148"
#define NXP_JN5142 "NXP_JN5142"
#define NXP_JN5139 "NXP_JN5139"


enum {
#define NXP_CUSTOM_MAC_ADDR 0x01001570  /* For OTP via Flash Programmer */
#define NXP_MAC_ADDR        0x01001580  /* mac address的地址 */
#define NXP_MAC_LEN         8           /* mac address的字节数 */

    E_NXP_NULL,                 /* 空闲状态 */
    E_NXP_WAITING,              /* 等待下载按键按下，或是自动下载倒计时 */
    E_NXP_RESET_ING,            /* 复位NXP_ZIGBEE */
    E_NXP_RESET_OK,             /* 复位成功 */
    //E_NXP_READ_CUSTOM_MAC,      /* For OTP via Flash Programmer */
    E_NXP_READ_MAC,             /* 读取mac address */
    E_NXP_GET_CHIP_ID,          /* 复位完成后过100ms，zigbee串口是38400bsp */
    E_NXP_SELECT_INTERNAL_FLASH,/* 选择内部FLASH下载 */
    E_NXP_SET_BAND_1M,          /* 将波特率设置成1M，准备下载 */
    E_NXP_FLASH_ERASE_ALL,      /* 擦除全部FLASH */
    E_NXP_FLASH_ERASE_ALL_ACK,  /* 等待擦除全部FLASH成功的回复 */
    E_NXP_FLASH_HEADER_CHECK,   /* 检测FLASH_HEADER来判断擦除全部FLASH是否成功 */
    E_NXP_DOWNLOAD_FIRST_PACK,  /* 开始下载固件第一个包 */
    E_NXP_DOWNLOAD_OTHER_PACK,  /* 下载固件剩下的包 */
    E_NXP_CODE_READ_PROTECT,    /* 设置固件读保护 */
    E_NXP_SET_MCU_RUN,          /* 设置MCU运行 */
    E_NXP_DOWNLOAD_OK,          /* 下载固件成功 */
};


enum {  /* message_type */
    E_NXP_FLASH_ERASE_ALL_REQUEST   = 0x07,
    E_NXP_FLASH_ERASE_ALL_RESPONSE  = 0x08,

    E_NXP_FLASH_PROGRAM_REQUEST     = 0x09,
    E_NXP_FLASH_PROGRAM_RESPONSE    = 0x0A,

    E_NXP_FLASH_READ_REQUEST        = 0x0B,
    E_NXP_FLASH_READ_RESPONSE       = 0x0C,

    E_NXP_SECTOR_ERASE_REQUEST      = 0x0D,
    E_NXP_SECTOR_ERASE_RESPONSE     = 0x0E,

    E_NXP_WRITE_SR_REQUEST          = 0x0F,
    E_NXP_WRITE_SR_RESPONSE         = 0x10,

    E_NXP_SET_RESET_REQUEST         = 0x14,
    E_NXP_SET_RESET_RESPONSE        = 0x15,

    E_NXP_RAM_WRITE_REQUEST         = 0x1D,
    E_NXP_RAM_WRITE_RESPONSE        = 0x1E,

    E_NXP_RAM_READ_REQUEST          = 0x1F,
    E_NXP_RAM_READ_RESPONSE         = 0x20,

    E_NXP_RUN_REQUEST               = 0x21,
    E_NXP_RUN_RESPONSE              = 0x22,

    E_NXP_READ_FLASH_ID_REQUEST     = 0x25,
    E_NXP_READ_FLASH_ID_RESPONSE    = 0x26,

    E_NXP_CHANGE_BAND_RATE_REQUEST  = 0x27,
    E_NXP_CHANGE_BAND_RATE_RESPONSE = 0x28,

    E_NXP_SET_FLASH_TYPE_REQUEST    = 0x2C,
    E_NXP_SET_FLASH_TYPE_RESPONSE   = 0x2D,

    E_NXP_GET_CHIP_ID_REQUEST       = 0x32,
    E_NXP_GET_CHIP_ID_RESPONSE      = 0x33,
};


__packed typedef struct {
#define NXP_CHIP_MAX        5
#define NXP_CHIP_ID_LEN_MAX 9
    uint8_t id[NXP_CHIP_ID_LEN_MAX];
    uint8_t id_len;
    char* name;
    uint32_t flash_size;
}NXP_CHIP_S;

extern const NXP_CHIP_S nxp_chip[NXP_CHIP_MAX];


__packed typedef struct {
#define NXP_MAX_BUFF_SIZE       253
    uint8_t *len;       /* len后面所有数据长度，最大为255，因此buff最大253 */
    uint8_t *message_type;
    uint8_t *buff;
    uint8_t *check_sum; /* 所有数据的异或 */
}NXP_PROTOCOL_S;


__packed typedef struct {
#define TX_ERR_COUNT_MAX    3
#define TX_BUFF_MAX         256
    uint8_t err_count;
    uint16_t len;
    uint8_t buff[TX_BUFF_MAX];
}HANDLE_TX_S;


__packed typedef struct {
#define RX_BUFF_MAX         256
    uint16_t len;
    uint8_t buff[RX_BUFF_MAX];
}HANDLE_RX_S;


extern uint8_t nxp_state_get(void);
extern void nxp_init(void);
extern void nxp_task(void);


#endif /* __APP_NXP_H */
