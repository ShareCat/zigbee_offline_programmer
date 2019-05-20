/**
  ******************************************************************************
  * @file:      sys_command_line.c
  * @author:    Cat(���ƽ)
  * @version:   V1.0
  * @date:      2018-1-18
  * @brief:     ����������
  * @attention:
  ******************************************************************************
  */


#include "sys_command_line.h"
#include "./../bsp/bsp_usart.h"

#include "./../app/app_nxp.h"
#include "./../app/app_fatfs.h"



static uint8_t cli_help(void *para, uint8_t len);
static uint8_t cli_clear(void *para, uint8_t len);
static uint8_t cli_echo(void *para, uint8_t len);
static uint8_t cli_reboot(void *para, uint8_t len);


__packed typedef struct {
#define HANDLE_LEN 128

    uint8_t buff[HANDLE_LEN];
    uint8_t len;
} HANDLE_TYPE_S;



uint8_t cli_echo_flag = DISABLE; /* ����Ĭ�Ϲر� */

RX_BUFF_TYPE cli_rx_buff; /* 128�ֽڶ��У����洮���жϽ��յ����� */



const char CLI_Cmd_Help[] =
    "\r\n"
    "[help]\r\n"
    " * show commands\r\n"
    "\r\n";

const char CLI_Cmd_Clear[] =
    "[cls]\r\n"
    " * clear the screen\r\n"
    "\r\n";

const char CLI_Cmd_Echo[] =
    "[echo]\r\n"
    " * echo 1: echo on\r\n"
    " * echo 0: echo off\r\n"
    "\r\n";

const char CLI_Cmd_Reboot[] =
    "[reboot]\r\n"
    " * reboot MCU\r\n"
    "\r\n";


/**
  * ����ṹ����������ִ�Сд
  */
const COMMAND_S CLI_Cmd[] = {
    /* ����               �������                ��ʼ������          ������ */
    {"help",            CLI_Cmd_Help,       NULL,           cli_help},
    {"cls",             CLI_Cmd_Clear,      NULL,           cli_clear},
    {"echo",            CLI_Cmd_Echo,       NULL,           cli_echo},
    {"reboot",          CLI_Cmd_Reboot,     NULL,           cli_reboot},

    /* ������ϵͳ������¿�ʼ���й�Ӧ�õ������� */
#ifdef NXP_CLI_DEBUG
    {"nxp",             NXP_CMD,            NULL,           cli_nxp},
#endif

#ifdef FATFS_CLI_DEBUG
    {"fatfs",           FATFS_CMD,          NULL,           cli_fatfs},
#endif
};


/**
  * @brief          ��ӡÿ������İ�����Ϣ
  * @param  para:   ������ַ
  * @param  len:    ��������
  * @retval         ����True��ʾ��ȷ
  */
static uint8_t cli_help(void *para, uint8_t len)
{
    uint8_t i;

    for(i = 0; i < sizeof(CLI_Cmd) / sizeof(COMMAND_S); i++) {
        if (NULL != CLI_Cmd[i].pHelp) {
            PRINTF(CLI_Cmd[i].pHelp);
        }
    }

    return TRUE;
}


/**
  * @brief          ��������
  * @param  para:   ������ַ
  * @param  len:    ��������
  * @retval         ����True��ʾ��ȷ
  */
static uint8_t cli_clear(void *para, uint8_t len)
{
    TERMINAL_BACK_BLACK(); /* �����ն���ʾ����Ϊ��ɫ */
    TERMINAL_FONT_GREEN(); /* �����ն���ʾ����Ϊ��ɫ */

    /* This prints the clear screen and move cursor to top-left corner control
     * characters for VT100 terminals. This means it will not work on
     * non-VT100 compliant terminals, namely Windows' cmd.exe, but should
     * work on anything unix-y. */
    TERMINAL_RESET_CURSOR();
    TERMINAL_DISPLAY_CLEAR(); /* secureCRT�յ�����ַ��������� */

    return TRUE;
}


/**
  * @brief          ��ӡÿ������İ�����Ϣ
  * @param  para:   ������ַ
  * @param  len:    ��������
  * @retval         ����True��ʾ��ȷ
  */
static uint8_t cli_echo(void *para, uint8_t len)
{
    uint8_t *pTemp;
    pTemp = (uint8_t *)para;

    if((0 < len) && (NULL != pTemp)) {
        pTemp++; /* ����һ���ո� */

        if('1' == *pTemp) {
            /* �򿪻��� */
            cli_echo_flag = ENABLE;
            PRINTF("echo on\r\n");
        } else if('0' == *pTemp) {
            /* �رջ��� */
            cli_echo_flag = DISABLE;
            PRINTF("echo off\r\n");
        } else {
            /* �����д� */
            return FALSE;
        }
    }

    /* ��ȷ���� */
    return TRUE;
}


#if 0
/**
  * @brief  ��ȡMCU��Ϣ
  * @param  ������ַ�Ͳ�������
  * @retval ����True��ʾ��ȷ
  */
static uint8_t cli_mcu_info(void *para, uint8_t len)
{
    uint16_t flash_size;
    uint32_t MAC_buff[3];

    /* ��ȡFLASH��С */
    flash_size = *(__IO uint16_t *)(0x1FFF7A22);
    PRINTF("Flash size: %d KB\r\n", flash_size);

    /* ��ȡID */
    MAC_buff[0] = *(__IO uint32_t*)(0x1FFF7A10);
    MAC_buff[1] = *(__IO uint32_t*)(0x1FFF7A14);
    MAC_buff[2] = *(__IO uint32_t*)(0x1FFF7A18);
    PRINTF("UID(hex): %02X-%02X-%02X\r\n", MAC_buff[0], MAC_buff[1], MAC_buff[2]);

    return TRUE;
}
#endif


/**
  * @brief          ����MCU
  * @param  para:   ������ַ
  * @param  len:    ��������
  * @retval         ����True��ʾ��ȷ
  */
static uint8_t cli_reboot(void *para, uint8_t len)
{
    extern void Delay(__IO uint32_t nCount);
    PRINTF("\r\n[END]: System Rebooting");
    PRINTF(".");
    Delay(0xFFFFF);
    PRINTF(".");
    Delay(0xFFFFF);

    SYSTEM_REBOOT();

    return TRUE;
}


#if CLI_HISTORY

__packed typedef struct {
    char cmd[HISTORY_MAX][HANDLE_LEN];
    uint8_t count;
    uint8_t latest;
    uint8_t show;
}HISTORY_S;

static HISTORY_S history;


/**
  * @brief          ���һ����ʷ��¼
  * @param  buff:   ��ʷ��¼
  * @retval         ��
  */
static void cli_history_add(char* buff)
{
    uint16_t len;

    if (NULL == buff) return;

    len = strlen((const char *)buff);
    if (len < HANDLE_LEN) {
        memset((void *)history.cmd[history.latest], 0x00, HANDLE_LEN);
        memcpy((void *)history.cmd[history.latest], (const void *)buff, len);
        if (history.count < HISTORY_MAX) {
            history.count++;
        }

        history.latest++;
        if (history.latest >= HISTORY_MAX) {
            history.latest = 0;
        }

        history.show = 0;
    }
}


/**
  * @brief          �鿴��ʷ��¼
  * @param  mode:   TRUE�鿴��һ����FALSE�鿴��һ��
  * @retval         ��ʷ��¼
  */
static char* cli_history_show(uint8_t mode)
{
    uint8_t num;
    uint8_t index;

    if (0 == history.count) return 0;

    if (TRUE == mode) {
        /* ��һ����ʷ���� */
        if (history.show < history.count) {
            history.show++;
        }
    } else {
        /* ��һ����ʷ���� */
        if (1 < history.show) {
            history.show--;
        }
    }

    num = history.show;
    index = history.latest;
    while (num) {
        if (0 != index) {
            index--;
        } else {
            index = HISTORY_MAX - 1;
        }
        num--;
    }

    //PRINTF("history: %s \r\n", history.cmd[index]);

    return history.cmd[index];
}

#endif


/**
  * @brief  �����г�ʼ��
  * @param  ���ڲ�����
  * @retval null
  */
void cli_init(uint32_t baud)
{
    uint8_t i;

    memset((uint8_t *)&cli_rx_buff, 0, sizeof(RX_BUFF_TYPE));

#if CLI_HISTORY
    memset((uint8_t *)&history, 0, sizeof(history));
#endif

    USART_INIT(baud);

    /* ��ÿ��������г�ʼ�� */
    for(i = 0; i < sizeof(CLI_Cmd) / sizeof(COMMAND_S); i++) {
        /* �������ʼ�������ǿ� */
        if(NULL != CLI_Cmd[i].pInit) {
            if(FALSE == CLI_Cmd[i].pInit()) {
                /* ִ�г�ʼ���������ش���Ҫ��ʾ */
                PRINTF("\r\n-> FUN[%d] INIT WRONG\r\n", i);
            }
        }
    }

    PRINTF(" \r\n");
    TERMINAL_BACK_BLACK(); /* �����ն���ʾ����Ϊ��ɫ */
    TERMINAL_FONT_GREEN(); /* �����ն���ʾ����Ϊ��ɫ */
    TERMINAL_DISPLAY_CLEAR();
    TERMINAL_RESET_CURSOR();

    PRINTF_COLOR(E_FONT_YELLOW, "-------------------------------\r\n\r\n");
    TERMINAL_HIGH_LIGHT();
    PRINTF("    CLI version: V0.6          \r\n\r\n");
    PRINTF("    coder: Cat                 \r\n\r\n");
    PRINTF("    Email: 843553493@qq.com    \r\n\r\n");
    TERMINAL_UN_HIGH_LIGHT();
    PRINTF_COLOR(E_FONT_YELLOW, "-------------------------------\r\n\r\n");
}


/**
  * @brief              ������յ�������
  * @param  rx_buff:    �����յ�������
  * @retval             null
  */
static void cli_rx_handle(RX_BUFF_TYPE *rx_buff)
{
    static HANDLE_TYPE_S Handle = {.len = 0};
    uint8_t i;
    uint8_t ParaLen;
    uint8_t *ParaAddr;
    uint8_t cmd_match = FALSE;

    /*  ---------------------------------------
        ��һ���������ڽ��յ������ݱ�������
        ---------------------------------------
     */
    while(1) {

        if(Handle.len < HANDLE_LEN) {  /* ����û�� */

            /* �����н��յ������ݣ����Ƶ�Handle.buff����� */
            if(TRUE == QUEUE_OUT((*rx_buff), Handle.buff[Handle.len])) {
                /* '\b'����ɾ�������һ���ַ� */
                if ('\b' == Handle.buff[Handle.len]) {
                    /* ���л�����ַ���ɾ�����һ�� */
                    if (0 < Handle.len) {
                        /* ʵ����secrueCRT��Ҳɾ����������һ���ַ� */
                        TERMINAL_MOVE_LEFT(1);
                        TERMINAL_CLEAR_END();
                        /* ����һ���ַ� */
                        Handle.len -= 1;
                    }

                } else {
                    /* ���յ����ַ����ͳ�ȥ���ն˻��� */
                    if ((KEY_ESCAPE != Handle.buff[Handle.len])
                        && (KEY_LEFT_SQUARE != Handle.buff[Handle.len])) {

                        USART_SendData(DEBUG_USARTx, Handle.buff[Handle.len]);
                    }
                    //PRINTF("%02x ", Handle.buff[Handle.len]);

                    /* �������ַ�������ɾ���� */
                    Handle.len++;

#if CLI_HISTORY
                    char *p_hist_cmd = 0;
                    if (Handle.len > 2) {
                        if ((KEY_ESCAPE == Handle.buff[Handle.len - 3])
                            && (KEY_LEFT_SQUARE == Handle.buff[Handle.len - 2])
                            && (KEY_BIG_A == Handle.buff[Handle.len - 1])) {
                            /* [�Ϸ���]�������� */
                            TERMINAL_MOVE_LEFT(Handle.len);
                            TERMINAL_CLEAR_END();
                            p_hist_cmd = cli_history_show(TRUE);
                        } else if ((KEY_ESCAPE == Handle.buff[Handle.len - 3])
                            && (KEY_LEFT_SQUARE == Handle.buff[Handle.len - 2])
                            && (KEY_BIG_B == Handle.buff[Handle.len - 1])) {
                            /* [�·���]�������� */
                            TERMINAL_MOVE_LEFT(Handle.len);
                            TERMINAL_CLEAR_END();
                            p_hist_cmd = cli_history_show(FALSE);
                        }

                        if (0 != p_hist_cmd) {
                            memcpy(Handle.buff, p_hist_cmd, strlen(p_hist_cmd));
                            Handle.len = strlen(p_hist_cmd);
                            Handle.buff[Handle.len] = '\0';
                            PRINTF("%s", Handle.buff);  /* ��ʾ��ѯ������ */
                        }
                    }
#endif
                }

            } else {
                /* ��ȫ�����Ƶ�Handle.buff�� */
                break;
            }

        } else {
            /* �������� */
            break;
        }
    }

    /*  ---------------------------------------
        �ڶ�������������
        ---------------------------------------
     */
    if((1 == Handle.len) && (KEY_ENTER == Handle.buff[Handle.len - 1])) {
        /* ����Ӧ������"\r"������̨���س����Ƿ��͵�"\r"��MCU */
        Handle.len = 0;
    } else if(1 < Handle.len) {  /* ��������������������Ϊ�������"\r"����1�ֽ��� */
        /* ���������"\r"��β */
        if(KEY_ENTER == Handle.buff[Handle.len - 1]) {
            Handle.buff[Handle.len - 1] = '\0';

            /* ѭ����Ѱ��ƥ������� */
            for(i = 0; i < sizeof(CLI_Cmd) / sizeof(COMMAND_S); i++) {
                if(0 == strncmp((const char *)Handle.buff,
                                (void *)CLI_Cmd[i].pCmd,
                                strlen(CLI_Cmd[i].pCmd))) {
                    cmd_match = TRUE;
                    ParaLen = Handle.len - strlen(CLI_Cmd[i].pCmd);   /* ��������ĳ��� */
                    ParaAddr = &Handle.buff[strlen(CLI_Cmd[i].pCmd)]; /* ��������ĵ�ַ */

                    if(NULL != CLI_Cmd[i].pFun) {
                        /* ִ�������Ӧ�ĺ��� */
                        if(CLI_Cmd[i].pFun(ParaAddr, ParaLen)) {
                            /* ����ִ����ȷ */
                            PRINTF("\r\n-> OK\r\n");

#if CLI_HISTORY
                            cli_history_add((char *)Handle.buff);
#endif

                            /* �����˻��ԣ��ʹ�ӡ�յ������� */
                            if(ENABLE == cli_echo_flag) {
                                Handle.buff[Handle.len] = '\0';
                                PRINTF("[echo]: %s\r\n", (const char*)Handle.buff);
                            }
                        } else {
                            /* ����ִ�г��� */
                            PRINTF("\r\n-> PARA. ERR\r\n");
                            /* ����������ʾ������ʹ�ð��� */
                            PRINTF(CLI_Cmd[i].pHelp);
                        }
                    } else {
                        /* �ǿպ�������ʾ���� */
                        PRINTF("\r\n-> FUNC. ERR\r\n");
                    }
                }
            }

            if(FALSE == cmd_match) {
                /* û��ƥ�䵽��Ч�����ʾ������� */
                PRINTF("\r\n-> CMD ERR, try: help\r\n\r\n");
            }

            Handle.len = 0;

        }

    }


    if(Handle.len >= HANDLE_LEN) {
        /* ���ˣ���ռ��� */
        Handle.len = 0;
    }
}


/**
  * @brief  ����Ҫ���͵�����
  * @param  null
  * @retval null
  */
static void cli_tx_handle(void)
{
    /* Ԥ�� */
}


/**
  * @brief  �������������ݣ�һ��50ms����һ�ξ͹���
  * @param  null
  * @retval null
  */
void cli_task(void)
{
    cli_rx_handle(&cli_rx_buff);
    cli_tx_handle();
}


