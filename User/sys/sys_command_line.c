/**
  ******************************************************************************
  * @file:      sys_command_line.c
  * @author:    Cat(孙关平)
  * @version:   V1.0
  * @date:      2018-1-18
  * @brief:     调试命令行
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



uint8_t cli_echo_flag = DISABLE; /* 回显默认关闭 */

RX_BUFF_TYPE cli_rx_buff; /* 128字节队列，保存串口中断接收的命令 */



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
  * 命令结构体表，命令区分大小写
  */
const COMMAND_S CLI_Cmd[] = {
    /* 命令               命令帮助                初始化函数          处理函数 */
    {"help",            CLI_Cmd_Help,       NULL,           cli_help},
    {"cls",             CLI_Cmd_Clear,      NULL,           cli_clear},
    {"echo",            CLI_Cmd_Echo,       NULL,           cli_echo},
    {"reboot",          CLI_Cmd_Reboot,     NULL,           cli_reboot},

    /* 上面是系统命令，以下开始是有关应用的命令了 */
#ifdef NXP_CLI_DEBUG
    {"nxp",             NXP_CMD,            NULL,           cli_nxp},
#endif

#ifdef FATFS_CLI_DEBUG
    {"fatfs",           FATFS_CMD,          NULL,           cli_fatfs},
#endif
};


/**
  * @brief          打印每个命令的帮助信息
  * @param  para:   参数地址
  * @param  len:    参数长度
  * @retval         返回True表示正确
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
  * @brief          清屏命令
  * @param  para:   参数地址
  * @param  len:    参数长度
  * @retval         返回True表示正确
  */
static uint8_t cli_clear(void *para, uint8_t len)
{
    TERMINAL_BACK_BLACK(); /* 设置终端显示背景为黑色 */
    TERMINAL_FONT_GREEN(); /* 设置终端显示字体为绿色 */

    /* This prints the clear screen and move cursor to top-left corner control
     * characters for VT100 terminals. This means it will not work on
     * non-VT100 compliant terminals, namely Windows' cmd.exe, but should
     * work on anything unix-y. */
    TERMINAL_RESET_CURSOR();
    TERMINAL_DISPLAY_CLEAR(); /* secureCRT收到这个字符串就清屏 */

    return TRUE;
}


/**
  * @brief          打印每个命令的帮助信息
  * @param  para:   参数地址
  * @param  len:    参数长度
  * @retval         返回True表示正确
  */
static uint8_t cli_echo(void *para, uint8_t len)
{
    uint8_t *pTemp;
    pTemp = (uint8_t *)para;

    if((0 < len) && (NULL != pTemp)) {
        pTemp++; /* 跳过一个空格 */

        if('1' == *pTemp) {
            /* 打开回显 */
            cli_echo_flag = ENABLE;
            PRINTF("echo on\r\n");
        } else if('0' == *pTemp) {
            /* 关闭回显 */
            cli_echo_flag = DISABLE;
            PRINTF("echo off\r\n");
        } else {
            /* 参数有错 */
            return FALSE;
        }
    }

    /* 正确返回 */
    return TRUE;
}


#if 0
/**
  * @brief  获取MCU信息
  * @param  参数地址和参数长度
  * @retval 返回True表示正确
  */
static uint8_t cli_mcu_info(void *para, uint8_t len)
{
    uint16_t flash_size;
    uint32_t MAC_buff[3];

    /* 获取FLASH大小 */
    flash_size = *(__IO uint16_t *)(0x1FFF7A22);
    PRINTF("Flash size: %d KB\r\n", flash_size);

    /* 获取ID */
    MAC_buff[0] = *(__IO uint32_t*)(0x1FFF7A10);
    MAC_buff[1] = *(__IO uint32_t*)(0x1FFF7A14);
    MAC_buff[2] = *(__IO uint32_t*)(0x1FFF7A18);
    PRINTF("UID(hex): %02X-%02X-%02X\r\n", MAC_buff[0], MAC_buff[1], MAC_buff[2]);

    return TRUE;
}
#endif


/**
  * @brief          重启MCU
  * @param  para:   参数地址
  * @param  len:    参数长度
  * @retval         返回True表示正确
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
  * @brief          添加一个历史记录
  * @param  buff:   历史记录
  * @retval         无
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
  * @brief          查看历史记录
  * @param  mode:   TRUE查看上一个，FALSE查看下一个
  * @retval         历史记录
  */
static char* cli_history_show(uint8_t mode)
{
    uint8_t num;
    uint8_t index;

    if (0 == history.count) return 0;

    if (TRUE == mode) {
        /* 上一个历史命令 */
        if (history.show < history.count) {
            history.show++;
        }
    } else {
        /* 下一个历史命令 */
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
  * @brief  命令行初始化
  * @param  串口波特率
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

    /* 对每个命令进行初始化 */
    for(i = 0; i < sizeof(CLI_Cmd) / sizeof(COMMAND_S); i++) {
        /* 该命令初始化函数非空 */
        if(NULL != CLI_Cmd[i].pInit) {
            if(FALSE == CLI_Cmd[i].pInit()) {
                /* 执行初始化函数返回错误，要提示 */
                PRINTF("\r\n-> FUN[%d] INIT WRONG\r\n", i);
            }
        }
    }

    PRINTF(" \r\n");
    TERMINAL_BACK_BLACK(); /* 设置终端显示背景为黑色 */
    TERMINAL_FONT_GREEN(); /* 设置终端显示字体为绿色 */
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
  * @brief              处理接收到的数据
  * @param  rx_buff:    串口收到的数据
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
        第一步，将串口接收到的数据保存起来
        ---------------------------------------
     */
    while(1) {

        if(Handle.len < HANDLE_LEN) {  /* 缓存没满 */

            /* 串口有接收到新数据，复制到Handle.buff后解析 */
            if(TRUE == QUEUE_OUT((*rx_buff), Handle.buff[Handle.len])) {
                /* '\b'用于删除最近的一个字符 */
                if ('\b' == Handle.buff[Handle.len]) {
                    /* 还有缓存的字符就删除最近一个 */
                    if (0 < Handle.len) {
                        /* 实现在secrueCRT上也删除最近输入的一个字符 */
                        TERMINAL_MOVE_LEFT(1);
                        TERMINAL_CLEAR_END();
                        /* 回退一个字符 */
                        Handle.len -= 1;
                    }

                } else {
                    /* 将收到的字符发送出去，终端回显 */
                    if ((KEY_ESCAPE != Handle.buff[Handle.len])
                        && (KEY_LEFT_SQUARE != Handle.buff[Handle.len])) {

                        USART_SendData(DEBUG_USARTx, Handle.buff[Handle.len]);
                    }
                    //PRINTF("%02x ", Handle.buff[Handle.len]);

                    /* 是正常字符，不是删除键 */
                    Handle.len++;

#if CLI_HISTORY
                    char *p_hist_cmd = 0;
                    if (Handle.len > 2) {
                        if ((KEY_ESCAPE == Handle.buff[Handle.len - 3])
                            && (KEY_LEFT_SQUARE == Handle.buff[Handle.len - 2])
                            && (KEY_BIG_A == Handle.buff[Handle.len - 1])) {
                            /* [上方向]键被按下 */
                            TERMINAL_MOVE_LEFT(Handle.len);
                            TERMINAL_CLEAR_END();
                            p_hist_cmd = cli_history_show(TRUE);
                        } else if ((KEY_ESCAPE == Handle.buff[Handle.len - 3])
                            && (KEY_LEFT_SQUARE == Handle.buff[Handle.len - 2])
                            && (KEY_BIG_B == Handle.buff[Handle.len - 1])) {
                            /* [下方向]键被按下 */
                            TERMINAL_MOVE_LEFT(Handle.len);
                            TERMINAL_CLEAR_END();
                            p_hist_cmd = cli_history_show(FALSE);
                        }

                        if (0 != p_hist_cmd) {
                            memcpy(Handle.buff, p_hist_cmd, strlen(p_hist_cmd));
                            Handle.len = strlen(p_hist_cmd);
                            Handle.buff[Handle.len] = '\0';
                            PRINTF("%s", Handle.buff);  /* 显示查询的命令 */
                        }
                    }
#endif
                }

            } else {
                /* 已全部复制到Handle.buff了 */
                break;
            }

        } else {
            /* 缓存满了 */
            break;
        }
    }

    /*  ---------------------------------------
        第二步，解析命令
        ---------------------------------------
     */
    if((1 == Handle.len) && (KEY_ENTER == Handle.buff[Handle.len - 1])) {
        /* 不响应单独的"\r"，控制台按回车就是发送的"\r"给MCU */
        Handle.len = 0;
    } else if(1 < Handle.len) {  /* 命令长度满足解析条件，因为命令结束"\r"就是1字节了 */
        /* 命令必须以"\r"结尾 */
        if(KEY_ENTER == Handle.buff[Handle.len - 1]) {
            Handle.buff[Handle.len - 1] = '\0';

            /* 循环，寻找匹配的命令 */
            for(i = 0; i < sizeof(CLI_Cmd) / sizeof(COMMAND_S); i++) {
                if(0 == strncmp((const char *)Handle.buff,
                                (void *)CLI_Cmd[i].pCmd,
                                strlen(CLI_Cmd[i].pCmd))) {
                    cmd_match = TRUE;
                    ParaLen = Handle.len - strlen(CLI_Cmd[i].pCmd);   /* 命令参数的长度 */
                    ParaAddr = &Handle.buff[strlen(CLI_Cmd[i].pCmd)]; /* 命令参数的地址 */

                    if(NULL != CLI_Cmd[i].pFun) {
                        /* 执行命令对应的函数 */
                        if(CLI_Cmd[i].pFun(ParaAddr, ParaLen)) {
                            /* 命令执行正确 */
                            PRINTF("\r\n-> OK\r\n");

#if CLI_HISTORY
                            cli_history_add((char *)Handle.buff);
#endif

                            /* 开启了回显，就打印收到的命令 */
                            if(ENABLE == cli_echo_flag) {
                                Handle.buff[Handle.len] = '\0';
                                PRINTF("[echo]: %s\r\n", (const char*)Handle.buff);
                            }
                        } else {
                            /* 命令执行出错 */
                            PRINTF("\r\n-> PARA. ERR\r\n");
                            /* 参数出错提示该命令使用帮助 */
                            PRINTF(CLI_Cmd[i].pHelp);
                        }
                    } else {
                        /* 是空函数，提示错误 */
                        PRINTF("\r\n-> FUNC. ERR\r\n");
                    }
                }
            }

            if(FALSE == cmd_match) {
                /* 没有匹配到有效命令，提示命令错误 */
                PRINTF("\r\n-> CMD ERR, try: help\r\n\r\n");
            }

            Handle.len = 0;

        }

    }


    if(Handle.len >= HANDLE_LEN) {
        /* 满了，清空计数 */
        Handle.len = 0;
    }
}


/**
  * @brief  处理要发送的数据
  * @param  null
  * @retval null
  */
static void cli_tx_handle(void)
{
    /* 预留 */
}


/**
  * @brief  处理命令行数据，一般50ms运行一次就够了
  * @param  null
  * @retval null
  */
void cli_task(void)
{
    cli_rx_handle(&cli_rx_buff);
    cli_tx_handle();
}


