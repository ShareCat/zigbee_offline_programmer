/**
  ******************************************************************************
  * @file:      app_nxp.h
  * @author:    Cat�����ƽ��
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
/* �����еİ�����Ϣ */
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
#define NXP_MAC_ADDR        0x01001580  /* mac address�ĵ�ַ */
#define NXP_MAC_LEN         8           /* mac address���ֽ��� */

    E_NXP_NULL,                 /* ����״̬ */
    E_NXP_WAITING,              /* �ȴ����ذ������£������Զ����ص���ʱ */
    E_NXP_RESET_ING,            /* ��λNXP_ZIGBEE */
    E_NXP_RESET_OK,             /* ��λ�ɹ� */
    //E_NXP_READ_CUSTOM_MAC,      /* For OTP via Flash Programmer */
    E_NXP_READ_MAC,             /* ��ȡmac address */
    E_NXP_GET_CHIP_ID,          /* ��λ��ɺ��100ms��zigbee������38400bsp */
    E_NXP_SELECT_INTERNAL_FLASH,/* ѡ���ڲ�FLASH���� */
    E_NXP_SET_BAND_1M,          /* �����������ó�1M��׼������ */
    E_NXP_FLASH_ERASE_ALL,      /* ����ȫ��FLASH */
    E_NXP_FLASH_ERASE_ALL_ACK,  /* �ȴ�����ȫ��FLASH�ɹ��Ļظ� */
    E_NXP_FLASH_HEADER_CHECK,   /* ���FLASH_HEADER���жϲ���ȫ��FLASH�Ƿ�ɹ� */
    E_NXP_DOWNLOAD_FIRST_PACK,  /* ��ʼ���ع̼���һ���� */
    E_NXP_DOWNLOAD_OTHER_PACK,  /* ���ع̼�ʣ�µİ� */
    E_NXP_CODE_READ_PROTECT,    /* ���ù̼������� */
    E_NXP_SET_MCU_RUN,          /* ����MCU���� */
    E_NXP_DOWNLOAD_OK,          /* ���ع̼��ɹ� */
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
    uint8_t *len;       /* len�����������ݳ��ȣ����Ϊ255�����buff���253 */
    uint8_t *message_type;
    uint8_t *buff;
    uint8_t *check_sum; /* �������ݵ���� */
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
