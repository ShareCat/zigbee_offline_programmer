/**
  *****************************************************************************
  * @file:      app_nxp.c
  * @author:    Cat�����ƽ��
  * @version:   V1.0
  * @date:      2019-1-1
  * @brief:     
  * @attention:
  *****************************************************************************
  */


#include "./app_nxp.h"

#include "./../sys/sys_scheduler.h"
#include "./../sys/sys_command_line.h"
#include "./../sys/sys_queue.h"

#include "./../bsp/bsp_nxp.h"
#include "./../bsp/bsp_usart.h"
#include "./../bsp/bsp_spi.h"

#include "./app_button.h"
#include "./app_buzzer.h"
#include "./app_fatfs.h"



#define FLASH_HEADER_ADDR   0x00000000
#define FLASH_HEADER_LEN    0x0040


static uint8_t nxp_state = E_NXP_NULL;
static uint8_t nxp_dl_fail = FALSE;


enum {
    ST_M25P05_A     = 0x04,     /* NXP_zigbee����SPI_FLASH */
    ST_M25P10_A     = 0x00,     /* NXP_zigbee����SPI_FLASH */
    ST_M25P20_A     = 0x05,     /* NXP_zigbee����SPI_FLASH */
    ST_M25P40       = 0x03,     /* NXP_zigbee����SPI_FLASH */
    SST_25VF010A    = 0x01,     /* NXP_zigbee����SPI_FLASH */
    ATMEL_25F512    = 0x02,     /* NXP_zigbee����SPI_FLASH */
    INTERNAL_FLASH  = 0x08,     /* NXP_zigbee�ڲ�SPI_FLASH */
};


enum {
    NXP_BAND_1M     = 0x01,     /* ���ò����ʣ�1000000 bps */
    NXP_BAND_500K   = 0x02,     /* ���ò����ʣ�500000 bps */
    NXP_BAND_115200 = 0x09,     /* ���ò����ʣ�115200 bps */
    NXP_BAND_38400  = 0x26,     /* ���ò����ʣ�38400 bps */
};


const NXP_CHIP_S *nxp_chip_now;

const NXP_CHIP_S nxp_chip[NXP_CHIP_MAX] = {   /* оƬID������0x32��ѯ */
    {
        {0x60, 0x00, 0xb6, 0x86, 0x00, 0x0b, 0x00, 0x02},
        0x08,
        NXP_JN5169,
        512*1024
    },

    {
        {0x10, 0x40, 0x46, 0x86},
        0x04,
        NXP_JN5148,
        512*1024    /* ��֪�����٣���д��512kbyte */
    },

    {
        {0x00, 0x00, 0x56, 0x86},
        0x04,
        NXP_JN5142,
        512*1024    /* ��֪�����٣���д��512kbyte */
    },

    {
        {0x10, 0x00, 0x20, 0x00},
        0x04,
        NXP_JN5139,
        512*1024    /* ��֪�����٣���д��512kbyte */
    }

};


NXP_PROTOCOL_S nxp_protocol;


HANDLE_TX_S handle_tx;
HANDLE_RX_S handle_rx;


__packed typedef struct {
#define DOWNLOAD_START_ADDR         0x00
#define DOWNLOAD_ONE_PACK_MAX_SIZE  0x80    /* һ�������������128byte�Ĺ̼� */
    uint32_t chip_flash_size;       /* оƬflash��С */
    uint32_t fm_size;               /* Ҫ���صĹ̼��ֽ��� */
    uint32_t fm_dl_count;           /* �������ֽ��� */
    uint32_t fm_src_addr;           /* Ҫ���ع̼��Ĵ�ŵ�ַ����SPI_FLASH */
    uint32_t fm_dst_addr;           /* �̼����ص�NXP��flash��ַ */

    /* �����Ƿ������ؼ�¼�õ��ı�����ÿ�δ�SPI_FLASH��ȡһ��sector�Ĺ̼����ֶ�����ص�NXP_ZIGBEE�� */
    uint8_t *p_fm_buff;             /* ָ���SPI_FLASH��ȡ��һ���ֹ̼����� */
    uint16_t fm_buff_remian;        /* �̼������л�ʣ�¶���û������ */
    uint16_t fm_buff_dl_count;      /* �̼��������������ֽ��� */
    uint16_t fm_buff_dl_pack_size;  /* �������ع̼����ֽ��� */

#define DL_ACK_TIMER_MAX    10      /* �ظ�ʱ�䲻�ܳ���500ms */
    uint8_t dl_ack_timer;           /* ����һ�������ȴ��ظ���ʱ */
}NXP_DOWNLOAD_S;

NXP_DOWNLOAD_S nxp_dl;

#define FM_BUFF_SIZE SPI_FLASH_SectorSize
uint8_t fm_buff[FM_BUFF_SIZE];



uint8_t nxp_dbg_en = FALSE;
#ifdef NXP_CLI_DEBUG

/**
  * @brief  NXP��Ϣ�鿴
  * @param  None
  * @retval None
  */
static void cli_nxp_info(void)
{
    PRINTF("nxp_state = %d \r\n", nxp_state);
}


/**
  * @brief  NXP����������
  * @param  None
  * @retval None
  */
uint8_t cli_nxp(void *para, uint8_t len)
{
    uint8_t *pTemp = (uint8_t *)para;

    if ((0 < len) && (NULL != pTemp)) {
        pTemp++; /* ����һ���ո� */

        if (0 == strncmp((const char *)pTemp, "info", strlen("info"))) {
            /* nxp info. */
            cli_nxp_info();
        } else if (0 == strncmp((const char *)pTemp, "dbg 1", strlen("dbg 1"))) {
            /* nxp debug on */
            nxp_dbg_en = TRUE;
            NL1();
            DBG("nxp debug on\r\n");
        } else if (0 == strncmp((const char *)pTemp, "dbg 0", strlen("dbg 0"))) {
            /* nxp debug off */
            nxp_dbg_en = FALSE;
            NL1();
            DBG("nxp debug off\r\n");
        } else {
            /* �����д� */
            return FALSE;
        }
    }

    /* ��ȷ���� */
    return TRUE;
}

#endif


/**
  * @brief  ��ȡNXP��������״̬
  * @param  None
  * @retval None
  */
uint8_t nxp_state_get(void)
{
    return nxp_state;
}


/**
  * @brief  ����Э�飬����Ч��
  * @param  None
  * @retval None
  */
uint8_t calc_check_sum(NXP_PROTOCOL_S *p_nxp)
{
    uint8_t i = 0;
    uint8_t sum = 0;

    sum = sum ^ *(p_nxp->len);
    sum = sum ^ *(p_nxp->message_type);

    if (*(p_nxp->len) > 2) {
        for (i = 0; i < (*(p_nxp->len) - 2); i++) {
            sum = sum ^ p_nxp->buff[i];
        }
    }

    return sum;
}

/**
  * @brief  ��������֮ǰ������Ч��
  * @param  None
  * @retval None
  */
void handle_tx_get_check_sum(HANDLE_TX_S *p_handle_tx)
{
    nxp_protocol.len = &p_handle_tx->buff[0];
    nxp_protocol.message_type = &p_handle_tx->buff[1];
    nxp_protocol.buff = &p_handle_tx->buff[2];

    p_handle_tx->buff[p_handle_tx->len] = calc_check_sum(&nxp_protocol);
}


/**
  * @brief  ��ʼ��
  * @param  None
  * @retval None
  */
void nxp_init(void)
{
    memset(&nxp_protocol, 0x00, sizeof(nxp_protocol));
    memset(&handle_rx, 0x00, sizeof(handle_rx));
    nxp_gpio_config();
    download_uart_init(DOWNLOAD_USART_BAUDRATE_1);
}


void nxp_handle_rx_copy(uint8_t *buff, uint16_t len)
{
    if ((len > 0) && (len < RX_BUFF_MAX)) {
        memcpy(handle_rx.buff, buff, len);
        handle_rx.len = len;
    }
}


/**
  * @brief  ����NXP_ZIGBEE�Ļظ���
  * @param  None
  * @retval None
  */
uint8_t nxp_rx_handle(void)
{
//#define NXP_RX_HANDLE_DEBUG

    uint8_t err = TRUE;
    uint8_t *data = handle_rx.buff;
    uint16_t data_len = handle_rx.len;

    if (data_len > 0) {
        /* ��ʼ���� */
        uint8_t temp_check_sum;
        nxp_protocol.len = &data[0];
        nxp_protocol.message_type = &data[1];
        nxp_protocol.buff = &data[2];
        nxp_protocol.check_sum = &data[data_len - 1];
        temp_check_sum = data[data_len - 1];
        if (temp_check_sum == calc_check_sum(&nxp_protocol)) {
            err = FALSE;

#ifdef NXP_RX_HANDLE_DEBUG
            DBG("--------------------begin");
            PRINTF("len = %02x \r\n", *nxp_protocol.len);
            PRINTF("message_type = %02x \r\n", *nxp_protocol.message_type);
            if (*nxp_protocol.len > 2) {
                uint8_t i;
                PRINTF("buff: ");
                for (i = 0; i < *nxp_protocol.len - 2; i++) {
                    PRINTF("%02x ", nxp_protocol.buff[i]);
                }
                NL1();
            }
            PRINTF("check_sum = %02x \r\n", *nxp_protocol.check_sum);
            DBG("--------------------end \r\n");
#endif
        } else {
//#ifdef NXP_RX_HANDLE_DEBUG
            ERR("check_sum error \r\n");
//#endif
        }
    }

    return err;
}


/**
  * @brief  �ظ���ʱʱ���ʱ
  * @param  None
  * @retval None
  */
void download_ack_time_run(void)
{
    nxp_dl.dl_ack_timer++;
}


/**
  * @brief  ��ͨ��ʱ����50ms������������һ��
  * @param  None
  * @retval None
  */
void normal_timer_50ms(void)
{
    download_ack_time_run();
}


/**
  * @brief  ��λ״̬
  * @param  None
  * @retval None
  */
void nxp_state_reset_from_err(void)
{
    memset(&nxp_dl, 0x00, sizeof(nxp_dl));
    memset(&handle_rx, 0x00, sizeof(handle_rx));
    nxp_state = E_NXP_NULL;
    buzzer_notice_fail();
}


/**
  * @brief  �ȴ�NXP_ZIGBEE�Ļظ�����ʱ���
  * @param  None
  * @retval TRUE��ʾ��ʱ
  */
uint8_t download_ack_time_check(void)
{
    uint8_t err = FALSE;

    if (nxp_dl.dl_ack_timer > DL_ACK_TIMER_MAX) {
        /* ����ʧ�ܣ�ԭ����û�лظ� */
        if (TRUE == nxp_dbg_en) {
            ERR("download_ack_time_check no ack \r\n");
        }
        err = TRUE;
    }

    return err;
}


/**
  * @brief  �ȴ�NXP_ZIGBEE�Ļظ���
  * @param  None
  * @retval None
  */
uint8_t wait_pack_from_nxp(void)
{
    uint8_t err = TRUE;

    while(0 == handle_rx.len) {
        /* ��ʱ */
        if (TRUE == download_ack_time_check()) {
            nxp_dl_fail = TRUE;
            break;  /* ����while */
        }
    }

    if (handle_rx.len) {
        //PRINTF("handle_rx.len = %d \r\n", handle_rx.len);
        if (FALSE == nxp_rx_handle()) {
            handle_tx.err_count = 0;
            err = FALSE;
        } else {
            if (++handle_tx.err_count > TX_ERR_COUNT_MAX) {
                /* �������̫�ദ�� */

            }
        }
        handle_rx.len = 0;
    }

    return err;
}


/**
  * @brief  �������ݰ���NXP_ZIGBEE
  * @param  None
  * @retval None
  */
uint8_t send_pack_to_nxp(uint8_t wait)
{
    uint8_t err = TRUE;

    download_usart_dma_tx(handle_tx.buff, handle_tx.len + 1);
    nxp_dl.dl_ack_timer = 0;

    if (FALSE != wait) {    /* ��Ҫ�ȴ���� */
        err = wait_pack_from_nxp();
    }

    return err;
}


/**
  * @brief  ����CHIP_IDƥ��оƬ�ͺ�
  * @param  None
  * @retval None
  */
static const NXP_CHIP_S * find_chip_id(uint8_t *buff)
{
    const NXP_CHIP_S *p_nxp_chip = 0;
    uint8_t i;

    for (i = 0; i < NXP_CHIP_MAX; i++) {
        if (0 == memcmp(nxp_chip[i].id, buff, nxp_chip[i].id_len)) {
            p_nxp_chip = &nxp_chip[i];
            break;
        }
    }

    return (const NXP_CHIP_S *)p_nxp_chip;
}


/**
  * @brief  ���flash_header�Ƿ�Ϊ��
  * @param  None
  * @retval FALSE��ʾΪ�գ�TRUE��ʾ��Ϊ��
  */
static uint8_t flash_header_check_null(uint8_t *p_data)
{
    uint8_t err = FALSE;
    uint8_t i;

    for (i = 0; i < FLASH_HEADER_LEN; i++) {
        if (0xff != p_data[i]) {
            err = FALSE;
            break;
        }
    }

    return err;
}


/**
  * @brief  ��SPI_FLASH�ж�ȡһ��sector�Ĺ̼������棬4096�ֽ�
  * @param  None
  * @retval None
  */
uint16_t get_one_sector_firmware_from_spi_flash(void)
{
    uint16_t size = FM_BUFF_SIZE;   /* ÿ�ι̶���ȡһ��sector */

    memset(fm_buff, 0x00, sizeof(fm_buff));
    spi_flash_buffer_read(fm_buff, nxp_dl.fm_src_addr, size);
    nxp_dl.fm_src_addr += size;

    #if 0
    LOGA("get_one_sector_firmware_from_spi_flash, addr = 0x%08x, size = %d",
        nxp_dl.fm_src_addr, size);
    #endif

    return size;
}


/**
  * @brief  ��ѯfm_buff���Ƿ񱾴λ���Ĺ̼�������ɣ��Ƿ���Ҫ��SPI_FLASH��ȡ�����̼�����
  * @param  None
  * @retval TRUE��ʾ��û��������ɣ�FALSE��ʾ�������
  */
uint8_t check_firmware_buff(void)
{
    if (nxp_dl.fm_dl_count >= nxp_dl.fm_size) return FALSE;

    if (0 == nxp_dl.fm_buff_remian) {
        /* ����������ɣ���ʼ��ȡ�����Ĺ̼������棬�������� */
        nxp_dl.fm_buff_remian = get_one_sector_firmware_from_spi_flash();
        nxp_dl.p_fm_buff = fm_buff;
        nxp_dl.fm_buff_dl_count = 0;
        nxp_dl.fm_buff_dl_pack_size = 0;
    }

    return TRUE;
}


/**
  * @brief  NXP_ZIGBEE���ص�һ��
  * @param  None
  * @retval None
  */
void nxp_download_first_pack(void)
{
    memset(&nxp_dl, 0x00, sizeof(nxp_dl));
    nxp_dl.fm_size = get_config_info_file_size();
    nxp_dl.fm_src_addr = FM_BACKUP_SECTOR_START * FM_BACKUP_SECTOR_SIZE;

    memset(&handle_tx, 0x00, sizeof(handle_tx));
    handle_tx.len = 0;
    handle_tx.buff[handle_tx.len++] = 0x00; /* ���ȷ������ */
    handle_tx.buff[handle_tx.len++] = E_NXP_FLASH_PROGRAM_REQUEST;
    handle_tx.buff[handle_tx.len++] = 0x00;
    handle_tx.buff[handle_tx.len++] = 0x00;
    handle_tx.buff[handle_tx.len++] = 0x00;
    handle_tx.buff[handle_tx.len++] = 0x00;

    check_firmware_buff();

    if ((nxp_dl.fm_size - nxp_dl.fm_dl_count) > DOWNLOAD_ONE_PACK_MAX_SIZE) {
        nxp_dl.fm_buff_dl_pack_size = DOWNLOAD_ONE_PACK_MAX_SIZE;
    } else {
        nxp_dl.fm_buff_dl_pack_size = nxp_dl.fm_size - nxp_dl.fm_dl_count;
    }
    memcpy(&handle_tx.buff[handle_tx.len],
            &fm_buff[nxp_dl.fm_buff_dl_count],
            nxp_dl.fm_buff_dl_pack_size);
    handle_tx.len += nxp_dl.fm_buff_dl_pack_size;
    
    handle_tx.buff[0] = handle_tx.len;      /* ȷ������ */
    handle_tx_get_check_sum(&handle_tx);

    send_pack_to_nxp(FALSE);
}


/**
  * @brief  NXP_ZIGBEE����ʣ��İ���ʹ���жϴ��� + ����DMA���ͷ�ʽ
  * @param  None
  * @retval None
  */
void nxp_download_other_pack(void)
{
    uint8_t err = FALSE;

    nxp_dl.dl_ack_timer = 0;

    if (nxp_state != E_NXP_DOWNLOAD_OTHER_PACK) return;

    if (FALSE == wait_pack_from_nxp()) {
        uint8_t response = nxp_protocol.buff[0];

        if (0 == response) {    /* 0x00��ʾ�ɹ� */
            nxp_dl.fm_dl_count += nxp_dl.fm_buff_dl_pack_size;
            nxp_dl.fm_buff_remian -= nxp_dl.fm_buff_dl_pack_size;
            nxp_dl.fm_buff_dl_count += nxp_dl.fm_buff_dl_pack_size;
            nxp_dl.fm_dst_addr += nxp_dl.fm_buff_dl_pack_size;
            if (TRUE == nxp_dbg_en) {
                PRINTF("downloaded[%d] ok \r\n", nxp_dl.fm_dl_count);
            }

            if (TRUE == check_firmware_buff()) {
                /* ��ʼ����������һ���� */
                memset(&handle_tx, 0x00, sizeof(handle_tx));
                handle_tx.len = 0;
                handle_tx.buff[handle_tx.len++] = 0x00; /* ���ȷ������ */
                handle_tx.buff[handle_tx.len++] = E_NXP_FLASH_PROGRAM_REQUEST;
                handle_tx.buff[handle_tx.len++] = (uint8_t)(nxp_dl.fm_dst_addr >> 0);
                handle_tx.buff[handle_tx.len++] = (uint8_t)(nxp_dl.fm_dst_addr >> 8);
                handle_tx.buff[handle_tx.len++] = (uint8_t)(nxp_dl.fm_dst_addr >> 16);
                handle_tx.buff[handle_tx.len++] = (uint8_t)(nxp_dl.fm_dst_addr >> 24);

                if ((nxp_dl.fm_size - nxp_dl.fm_dl_count) > DOWNLOAD_ONE_PACK_MAX_SIZE) {
                    nxp_dl.fm_buff_dl_pack_size = DOWNLOAD_ONE_PACK_MAX_SIZE;
                } else {
                    nxp_dl.fm_buff_dl_pack_size = nxp_dl.fm_size - nxp_dl.fm_dl_count;
                }
                memcpy(&handle_tx.buff[handle_tx.len], &fm_buff[nxp_dl.fm_buff_dl_count], nxp_dl.fm_buff_dl_pack_size);
                handle_tx.len += nxp_dl.fm_buff_dl_pack_size;

                handle_tx.buff[0] = handle_tx.len;      /* ȷ������ */
                handle_tx_get_check_sum(&handle_tx);
                send_pack_to_nxp(FALSE);

                if (TRUE == nxp_dbg_en) {
                    PRINTF("%d bytes downloading..., nxp_addr = 0x%08x \r\n",
                        nxp_dl.fm_buff_dl_pack_size,
                        nxp_dl.fm_dst_addr);
                }
            }
        } else {
            /* ��ǳ��� */
            err = TRUE;
        }
    } else {
        /* ��ǳ��� */
        err = TRUE;
    }

    if (TRUE == err) {
        /* �������Ƿ��ط�������� */
        ERRA("pack[%d] download fail \r\n", nxp_dl.fm_buff_dl_count);
        nxp_dl_fail = TRUE;
    }
}


/**
  * @brief  ��ӡ��ǰ���ؽ���
  * @param  None
  * @retval None
  */
static void nxp_download_process_view(void)
{
    float dl_percent;

    dl_percent = (float)nxp_dl.fm_dl_count / (float)nxp_dl.fm_size;
    dl_percent = dl_percent * 100;
    LOGA("download progress: %0.2f %% ", dl_percent);
}


/**
  * @brief  ����NXP_ZIGBEE����
  * @param  None
  * @retval None
  */
static void nxp_set_mcu_run(void)
{
    memset(&handle_tx, 0x00, sizeof(handle_tx));
    handle_tx.len = 0;
    handle_tx.buff[handle_tx.len++] = 0x00; /* ���ȷ������ */
    handle_tx.buff[handle_tx.len++] = E_NXP_RUN_REQUEST;
    handle_tx.buff[handle_tx.len++] = 0x00; /* 4byte��jump��ַ��LSB��ǰ */
    handle_tx.buff[handle_tx.len++] = 0x00;
    handle_tx.buff[handle_tx.len++] = 0x00;
    handle_tx.buff[handle_tx.len++] = 0x00;
    handle_tx.buff[0] = handle_tx.len;      /* ȷ������ */
    handle_tx_get_check_sum(&handle_tx);

    if (FALSE == send_pack_to_nxp(TRUE)) {
        uint8_t response = nxp_protocol.buff[0];

        if (0 == response) {    /* 0x00��ʾ�ɹ� */
            if (TRUE == nxp_dbg_en) {
                PRINTF("nxp_set_mcu_run \r\n");
            }
        } else {
            if (TRUE == nxp_dbg_en) {
                ERR("nxp_set_mcu_run fail \r\n");
            }
        }
    }
}


__packed typedef struct {
    uint8_t enable :1;
    uint8_t download_ok :1;
    uint16_t timer;
}NXP_AUTO_S;

NXP_AUTO_S nxp_auto;

/**
  * @brief  ��ѯ�Ƿ���Ҫ���أ������������棺�������»��Ƕ�ʱ�Զ�����
  * @param  None
  * @retval None
  */
uint8_t nxp_wait_for_command(void)
{
    uint8_t rtn = FALSE;
    uint8_t event;

    if (FALSE == get_config_info_auto_program_control()) {
        /* �ֶ�����ģʽ */
        if (TRUE == QUEUE_OUT(button_queue, event)) {
            /* �а����������� */
            if (ENB_SINGLE_CLICK == event) {
                if (E_NXP_WAITING == nxp_state) {
                    /* �����������ʱ�򣬰����ذ��������ʱ��������ΰ�����
                        ֻ�п���ʱ���°����Ż�������� */
                    rtn = TRUE;
                } else {
                    //PRINTF("download busy... \r\n");
                }
            }
        }
    } else {
        /* �Զ���ʱ����ģʽ */
        if (TRUE == QUEUE_OUT(button_queue, event)) {
            /* ���������Զ�����ģʽ�������ǹر� */
            if (ENB_SINGLE_CLICK == event) {
                if (TRUE == nxp_auto.enable) {
                    nxp_auto.enable = FALSE;
                } else {
                    nxp_auto.enable = TRUE;
                }
            }
        }

        if (TRUE == nxp_auto.enable) {
            /* ʱ�䵽�˾����� */
            if (nxp_auto.timer) {
                nxp_auto.timer--;   /* 10ms��1 */
            }

            if ((nxp_auto.timer == 0) && (E_NXP_NULL == nxp_state)) {
                /* ��ȡ�Զ���̼��ʱ�䣬��λ�� */
                nxp_auto.timer = get_config_info_auto_program_time();
                /* �����ж��ٸ�10ms */
                nxp_auto.timer = nxp_auto.timer * 100;
            }

            if ((nxp_auto.timer == 0) && (E_NXP_WAITING == nxp_state)) {
                nxp_auto.download_ok = FALSE;
                rtn = TRUE;
            }
        }
    }

    if (TRUE == rtn) {
        if (FALSE == check_program_condition()) {
            /* ����������������ʾû����Ч�����ã�����ʧ�� */
            buzzer_notice_invalid_config();
            if (TRUE == nxp_auto.enable) {
                /* �Զ���ʱ����ģʽ�£����ش��������ˣ���ʾ��󣬹رտ��� */
                nxp_auto.enable = FALSE;
            }
            rtn = FALSE;
            if (TRUE == nxp_dbg_en) {
                ERR("check_program_ok fail \r\n");
            }
        }
    }

    return rtn;
}


/**
  * @brief  NXP_ZIGBEE��������
  * @param  None
  * @retval None
  */
void nxp_task(void)
{
    uint8_t cmd = nxp_wait_for_command();

    switch (nxp_state) {
        case E_NXP_NULL: {
            nxp_state = E_NXP_WAITING;
        }
        //break;    /* ����Ͳ���break�ˣ�ֱ�ӽ�����һ��״̬����ʡ20ms */

        case E_NXP_WAITING: {
            if (TRUE == cmd) {
                memset(&nxp_dl, 0x00, sizeof(nxp_dl));
                memset(&handle_rx, 0x00, sizeof(handle_rx));
                download_uart_init(DOWNLOAD_USART_BAUDRATE_1);
                nxp_state = E_NXP_RESET_ING;
            }
        }
        break;

        case E_NXP_RESET_ING: {
            /* ����RESET���и�λ��ͬʱ����DO1�� */
            nxp_reset_ctrl(DISABLE);
            nxp_do1_ctrl(DISABLE);
            os_timer_set(T_1S);
            nxp_state = E_NXP_RESET_OK;
            //PRINTF("E_NXP_RESET_ING \r\n");
        }
        break;

        case E_NXP_RESET_OK: {
            if (!os_timer_check()) {
                /* ����RESET��ɸ�λ��DO1�ű��ֵ͵�ƽ������bootloader */
                nxp_reset_ctrl(ENABLE);
                nxp_do1_ctrl(DISABLE);
                os_timer_set(T_200MS);  /* ����200ms */
                nxp_state = E_NXP_GET_CHIP_ID;
                if (TRUE == nxp_dbg_en) {
                    PRINTF("E_NXP_RESET_OK \r\n");
                }
            }
        }
        break;

        case E_NXP_GET_CHIP_ID: {
            if (!os_timer_check()) {
                memset(&handle_tx, 0x00, sizeof(handle_tx));
                handle_tx.len = 0;
                handle_tx.buff[handle_tx.len++] = 0x00; /* ���ȷ������ */
                handle_tx.buff[handle_tx.len++] = E_NXP_GET_CHIP_ID_REQUEST;
                handle_tx.buff[0] = handle_tx.len;      /* ȷ������ */
                handle_tx_get_check_sum(&handle_tx);

                if (FALSE == send_pack_to_nxp(TRUE)) {
                    uint8_t response = nxp_protocol.buff[0];
                    uint8_t *chip_id_addr = nxp_protocol.buff + 1;

                    if (0 == response) {    /* 0x00��ʾ�ɹ� */
                        nxp_chip_now = find_chip_id(chip_id_addr);
                        if (0 != nxp_chip_now) {
                            if (TRUE == nxp_dbg_en) {
                                PRINTF("GET_CHIP_ID: %s \r\n",
                                    nxp_chip_now->name);
                            }
                            nxp_state = E_NXP_SELECT_INTERNAL_FLASH;
                        } else {
                            /* ʧ�ܴ��� */
                            nxp_dl_fail = TRUE;
                            if (TRUE == nxp_dbg_en) {
                                ERR("GET_CHIP_ID dont match \r\n");
                            }
                        }
                    } else {
                        /* ʧ�ܴ��� */
                        nxp_dl_fail = TRUE;
                        if (TRUE == nxp_dbg_en) {
                            ERR("GET_CHIP_ID no response \r\n");
                        }
                    }
                } else {
                    /* ʧ�ܴ��� */
                    nxp_dl_fail = TRUE;
                    if (TRUE == nxp_dbg_en) {
                        ERR("GET_CHIP_ID send no response \r\n");
                    }
                }
            }
        }
        break;

        case E_NXP_SELECT_INTERNAL_FLASH: {
            memset(&handle_tx, 0x00, sizeof(handle_tx));
            handle_tx.len = 0;
            handle_tx.buff[handle_tx.len++] = 0x00; /* ���ȷ������ */
            handle_tx.buff[handle_tx.len++] = E_NXP_SET_FLASH_TYPE_REQUEST;
            handle_tx.buff[handle_tx.len++] = INTERNAL_FLASH;
            handle_tx.buff[handle_tx.len++] = 0x00;
            handle_tx.buff[handle_tx.len++] = 0x00;
            handle_tx.buff[handle_tx.len++] = 0x00;
            handle_tx.buff[handle_tx.len++] = 0x00;
            handle_tx.buff[0] = handle_tx.len;      /* ȷ������ */
            handle_tx_get_check_sum(&handle_tx);

            if (FALSE == send_pack_to_nxp(TRUE)) {
                uint8_t response = nxp_protocol.buff[0];

                if (0 == response) {    /* 0x00��ʾ�ɹ� */
                    nxp_state = E_NXP_SET_BAND_1M;
                    if (TRUE == nxp_dbg_en) {
                        PRINTF("E_NXP_SELECT_INTERNAL_FLASH \r\n");
                    }
                } else {
                    /* ʧ�ܴ��� */
                    nxp_dl_fail = TRUE;
                    if (TRUE == nxp_dbg_en) {
                        ERR("E_NXP_SELECT_INTERNAL_FLASH fail \r\n");
                    }
                }
            } else {
                /* ʧ�ܴ��� */
                nxp_dl_fail = TRUE;
                if (TRUE == nxp_dbg_en) {
                    ERR("E_NXP_SELECT_INTERNAL_FLASH send fail \r\n");
                }
            }
        }
        break;

        case E_NXP_SET_BAND_1M: {
            memset(&handle_tx, 0x00, sizeof(handle_tx));
            handle_tx.len = 0;
            handle_tx.buff[handle_tx.len++] = 0x00; /* ���ȷ������ */
            handle_tx.buff[handle_tx.len++] = E_NXP_CHANGE_BAND_RATE_REQUEST;
            handle_tx.buff[handle_tx.len++] = NXP_BAND_1M;
            handle_tx.buff[0] = handle_tx.len;      /* ȷ������ */
            handle_tx_get_check_sum(&handle_tx);

            if (FALSE == send_pack_to_nxp(TRUE)) {
                uint8_t response = nxp_protocol.buff[0];

                if (0 == response) {    /* 0x00��ʾ�ɹ� */
                    nxp_state = E_NXP_FLASH_ERASE_ALL;
                    /* �޸��Լ��Ĵ��ڲ����� */
                    download_uart_init(DOWNLOAD_USART_BAUDRATE_2);
                    os_timer_set(T_20MS);   /* NXP_ZIGBEE�޸Ĳ����ʺ�ȴ�20ms */
                    if (TRUE == nxp_dbg_en) {
                        PRINTF("E_NXP_SET_BAND_1M \r\n");
                    }
                } else {
                    /* ʧ�ܴ��� */
                    nxp_dl_fail = TRUE;
                    if (TRUE == nxp_dbg_en) {
                        ERR("E_NXP_SET_BAND_1M fail \r\n");
                    }
                }
            } else {
                /* ʧ�ܴ��� */
                nxp_dl_fail = TRUE;
                if (TRUE == nxp_dbg_en) {
                    ERR("E_NXP_SET_BAND_1M send fail \r\n");
                }
            }
        }
        break;

        case E_NXP_FLASH_ERASE_ALL: {
            if (!os_timer_check()) {
                memset(&handle_tx, 0x00, sizeof(handle_tx));
                handle_tx.len = 0;
                handle_tx.buff[handle_tx.len++] = 0x00; /* ���ȷ������ */
                handle_tx.buff[handle_tx.len++] = E_NXP_FLASH_ERASE_ALL_REQUEST;
                handle_tx.buff[0] = handle_tx.len;      /* ȷ������ */
                handle_tx_get_check_sum(&handle_tx);

                send_pack_to_nxp(FALSE);
                os_timer_set(T_100MS);  /* ʵ�����NXP_5169��Ҫ100ms */

                nxp_state = E_NXP_FLASH_ERASE_ALL_ACK;
            }
        }
        break;

        case E_NXP_FLASH_ERASE_ALL_ACK: {
            if (!os_timer_check()) {
                if (FALSE == wait_pack_from_nxp()) {
                    uint8_t response = nxp_protocol.buff[0];

                    if ((0 == response) && (E_NXP_FLASH_ERASE_ALL_RESPONSE == *nxp_protocol.message_type)) {    /* 0x00��ʾ�ɹ� */
                        nxp_state = E_NXP_FLASH_HEADER_CHECK;
                        if (TRUE == nxp_dbg_en) {
                            PRINTF("E_NXP_FLASH_ERASE_ALL \r\n");
                        }
                    }
                } else {
                    /* ����ʧ�ܴ��� */
                    nxp_dl_fail = TRUE;
                    if (TRUE == nxp_dbg_en) {
                        ERR("E_NXP_FLASH_ERASE_ALL_ACK send fail \r\n");
                    }
                }
            }
        }
        break;

        case E_NXP_FLASH_HEADER_CHECK: {
            memset(&handle_tx, 0x00, sizeof(handle_tx));
            handle_tx.len = 0;
            handle_tx.buff[handle_tx.len++] = 0x00; /* ���ȷ������ */
            handle_tx.buff[handle_tx.len++] = E_NXP_FLASH_READ_REQUEST;
            handle_tx.buff[handle_tx.len++] = (uint8_t)(FLASH_HEADER_ADDR >> 0);
            handle_tx.buff[handle_tx.len++] = (uint8_t)(FLASH_HEADER_ADDR >> 8);
            handle_tx.buff[handle_tx.len++] = (uint8_t)(FLASH_HEADER_ADDR >> 16)
            ;
            handle_tx.buff[handle_tx.len++] = (uint8_t)(FLASH_HEADER_ADDR >> 24);
            handle_tx.buff[handle_tx.len++] = (uint8_t)(FLASH_HEADER_LEN >> 0);
            handle_tx.buff[handle_tx.len++] = (uint8_t)(FLASH_HEADER_LEN >> 8);
            handle_tx.buff[0] = handle_tx.len;      /* ȷ������ */
            handle_tx_get_check_sum(&handle_tx);

            if (FALSE == send_pack_to_nxp(TRUE)) {
                uint8_t response = nxp_protocol.buff[0];

                if ((0 == response) && (FALSE == flash_header_check_null(&nxp_protocol.buff[1]))) {     /* 0x00��ʾ�ɹ� */
                    memset(&nxp_dl, 0x00, sizeof(nxp_dl));
                    nxp_state = E_NXP_DOWNLOAD_FIRST_PACK;
                    if (TRUE == nxp_dbg_en) {
                        PRINTF("E_NXP_FLASH_HEADER_CHECK \r\n");
                    }
                } else {
                    /* ʧ�ܴ��� */
                    nxp_dl_fail = TRUE;
                    if (TRUE == nxp_dbg_en) {
                        ERR("E_NXP_FLASH_HEADER_CHECK fail \r\n");
                    }
                }
            } else {
                /* ʧ�ܴ��� */
                nxp_dl_fail = TRUE;
                if (TRUE == nxp_dbg_en) {
                    ERR("E_NXP_FLASH_HEADER_CHECK send fail \r\n");
                }
            }
        }
        break;

        case E_NXP_DOWNLOAD_FIRST_PACK: {
            nxp_download_first_pack();
            nxp_state = E_NXP_DOWNLOAD_OTHER_PACK;
        }
        break;

        case E_NXP_DOWNLOAD_OTHER_PACK: {
            if (nxp_dl.fm_size == nxp_dl.fm_dl_count) {
                /* ������� */
                nxp_download_process_view();
                nxp_state = E_NXP_DOWNLOAD_OK;
            } else {
                if (TRUE == download_ack_time_check()) {
                    nxp_dl_fail = TRUE;
                }
            }
        }
        break;

        case E_NXP_DOWNLOAD_OK: {
            if (TRUE == get_config_info_mcu_run_after_program()) {
                /* ������ɺ�����MCU���� */
                nxp_set_mcu_run();
            }
            /* ����ʣ����¼���� */
            config_info_max_program_handle_after_program();
            /* ��������һ������ʾ���سɹ� */
            buzzer_notice_ok();

            nxp_state = E_NXP_NULL;
        }
        break;

        default: {
            nxp_state = E_NXP_NULL;
        }
        break;
    }

    if ((E_NXP_NULL != nxp_state) && (E_NXP_WAITING != nxp_state)) {
        /* ��ӡ���ؽ��� */
        static uint8_t temp_timer = 0;

        if (++temp_timer > 10) {    /* 100ms��ӡһ�� */
            nxp_download_process_view();
            temp_timer = 0;
        }
    }

    if (TRUE == nxp_dl_fail) {
        nxp_dl_fail = FALSE;
        nxp_state_reset_from_err();
    }
}


/*********************************************END OF FILE*********************/
