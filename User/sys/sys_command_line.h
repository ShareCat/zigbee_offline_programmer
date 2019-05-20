/**
  ******************************************************************************
  * @file:      sys_command_line.h
  * @author:    Cat(孙关平)
  * @version:   V1.0
  * @date:      2018-1-18
  * @brief:     调试命令行
  * @attention:
  ******************************************************************************
  */


#ifndef __SYS_COMMAND_LINE_H
#define __SYS_COMMAND_LINE_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "sys_queue.h"

/* Macro config --------------------------------------------------------------*/
#define CLI_ENABLE          TRUE                    /* 是否使用命令行 */
#define CLI_HISTORY         TRUE                    /* 是否使用历史记录 */
#define HISTORY_MAX         10                      /* 历史记录个数 */
#define CLI_PRINTF          TRUE                    /* 串口打印调试 */
#define CLI_BAUDRATE        ((uint32_t)768000)      /* 串口波特率 */
#define USART_INIT(baud)    debug_usart_init(baud)  /* 串口初始化函数 */
#define SYSTEM_REBOOT()     NVIC_SystemReset()      /* MCU重启 */

#define KEY_ESCAPE          0x1b
#define KEY_LEFT_SQUARE     0x5b
#define KEY_BIG_A           0x41
#define KEY_BIG_B           0x42
#define KEY_ENTER           '\r'


#if CLI_ENABLE
    #define CLI_INIT(...)       cli_init(__VA_ARGS__)
    #define CLI_RUN(...)        cli_run(__VA_ARGS__)
#else
    #define CLI_INIT(...)       ;
    #define CLI_RUN(...)        ;
#endif /* CLI_ENABLE */


#if CLI_PRINTF
    #define PRINTF(...)         printf(__VA_ARGS__)
    #define SPRINTF(...)        sprintf(__VA_ARGS__)

enum {
    E_FONT_BLACK,
    E_FONT_L_RED,
    E_FONT_RED,
    E_FONT_GREEN,
    E_FONT_YELLOW,
    E_FONT_BLUE,
    E_FONT_PURPLE,
    E_FONT_CYAN,
    E_FONT_WHITE,
};

    #define PRINTF_COLOR(c, ...)    do {                            \
                                        switch (c) {                \
                                            case E_FONT_BLACK:      \
                                            TERMINAL_FONT_BLACK();  \
                                            break;                  \
                                            case E_FONT_L_RED:      \
                                            TERMINAL_FONT_L_RED();  \
                                            break;                  \
                                            case E_FONT_RED:        \
                                            TERMINAL_FONT_RED();    \
                                            break;                  \
                                            case E_FONT_GREEN:      \
                                            TERMINAL_FONT_GREEN();  \
                                            break;                  \
                                            case E_FONT_YELLOW:     \
                                            TERMINAL_FONT_YELLOW(); \
                                            break;                  \
                                            case E_FONT_BLUE:       \
                                            TERMINAL_FONT_BLUE();   \
                                            break;                  \
                                            case E_FONT_PURPLE:     \
                                            TERMINAL_FONT_PURPLE(); \
                                            break;                  \
                                            case E_FONT_CYAN:       \
                                            TERMINAL_FONT_CYAN();   \
                                            break;                  \
                                            case E_FONT_WHITE:      \
                                            TERMINAL_FONT_WHITE();  \
                                            break;                  \
                                        }                           \
                                        printf(__VA_ARGS__);        \
                                        TERMINAL_FONT_GREEN();      \
                                    } while(0)
#else
    #define PRINTF(...)         ;
    #define SPRINTF(...)        ;
    #define PRINTF_COLOR(c, ...);
#endif /* CLI_PRINTF */




/* 断言-----------------------------------------------------------------BEGIN */

/* #val：使用#把宏参数变为一个字符串,用##把两个宏参数贴合在一起 */
#define ASSERT(val)                                                     \
if (!(val))                                                             \
{                                                                       \
    PRINTF("(%s) has assert failed at %s.\n", #val, __FUNCTION__);      \
    while (1);                                                          \
}

/* 断言-------------------------------------------------------------------END */




/* 打印调试-------------------------------------------------------------BEGIN */

#define ERR(fmt)        do {                                            \
                            TERMINAL_FONT_RED();                        \
                            PRINTF("### ERROR ###: "fmt"\r\n");         \
                            TERMINAL_FONT_GREEN();                      \
                        } while(0)
#define ERRA(fmt, ...)  do {                                            \
                            TERMINAL_FONT_RED();                        \
                            PRINTF("### ERROR ### %s(%d): "fmt"\r\n",   \
                                __FUNCTION__, __LINE__, __VA_ARGS__);   \
                            TERMINAL_FONT_GREEN();                      \
                        }while(0)
#define LOG(fmt)        do {                                            \
                            TERMINAL_FONT_CYAN();                       \
                            PRINTF("[Log]: "fmt"\r\n");                 \
                            TERMINAL_FONT_GREEN();                      \
                        } while(0)
#define LOGA(fmt, ...)  do {                                            \
                            TERMINAL_FONT_CYAN();                       \
                            PRINTF("[Log]: "fmt"\r\n", __VA_ARGS__);    \
                            TERMINAL_FONT_GREEN();                      \
                        } while(0)
#define DBG(fmt)        do {                                            \
                            TERMINAL_FONT_YELLOW();                     \
                            PRINTF("[Debug] %s(%d): "fmt"\r\n",         \
                                __FUNCTION__, __LINE__);                \
                            TERMINAL_FONT_GREEN();                      \
                        } while(0)
#define DBGA(fmt, ...)  do {                                            \
                            TERMINAL_FONT_YELLOW();                     \
                            PRINTF("[Debug] %s(%d): "fmt"\r\n",         \
                                __FUNCTION__, __LINE__, __VA_ARGS__);   \
                            TERMINAL_FONT_GREEN();                      \
                        } while(0)
#define DIE(fmt)        do {                                            \
                            TERMINAL_FONT_RED();                        \
                            TERMINAL_HIGH_LIGHT();                      \
                            PRINTF("### DIE ### %s(%d): "fmt"\r\n",     \
                                __FUNCTION__, __LINE__);                \
                        } while(1) /* infinite loop */
#define NL1()           do { PRINTF("\r\n"); } while(0)
#define NL2()           do { PRINTF("\r\n\r\n"); } while(0)
#define NL3()           do { PRINTF("\r\n\r\n\r\n"); } while(0)

/* 打印调试---------------------------------------------------------------END */




/* 控制终端显示---------------------------------------------------------BEGIN */

/*
参考链接:       http://blog.csdn.net/yangguihao/article/details/47734349
            http://blog.csdn.net/kevinshq/article/details/8179252


    @实现终端界面中光标的定位和清屏操作:
        \033[0m     关闭所有属性
        \033[1m     设置高亮度
        \03[4m      下划线
        \033[5m     闪烁
        \033[7m     反显
        \033[8m     消隐
        \033[30m    --  \033[37m  设置前景色
        \033[40m    --  \033[47m  设置背景色
        \033[nA     光标上移n行
        \033[nB     光标下移n行
        \033[nC     光标右移n行
        \033[nD     光标左移n行
        \033[y;xH   设置光标位置
        \033[2J     清屏
        \033[K      清除从光标到行尾的内容
        \033[s      保存光标位置
        \033[u      恢复光标位置
        \033[?25l   隐藏光标
        \33[?25h    显示光标


    @背景颜色范围: 40--49                 字颜色: 30--39
     40: BLACK                          30: 黑
     41: RED                            31: 红
     42: GREEN                          32: 绿
     43: YELLOW                         33: 黄
     44: BLUE                           34: 蓝
     45: PURPLE                         35: 紫
     46: CYAN                           36: 深绿
     47: WHITE                          37: 白色
*/

/* 字体颜色 */
#define TERMINAL_FONT_BLACK()       PRINTF("\033[1;30m")
#define TERMINAL_FONT_L_RED()       PRINTF("\033[0;31m")    /* 浅红色 */
#define TERMINAL_FONT_RED()         PRINTF("\033[1;31m")    /* 红色 */
#define TERMINAL_FONT_GREEN()       PRINTF("\033[1;32m")
#define TERMINAL_FONT_YELLOW()      PRINTF("\033[1;33m")
#define TERMINAL_FONT_BLUE()        PRINTF("\033[1;34m")
#define TERMINAL_FONT_PURPLE()      PRINTF("\033[1;35m")
#define TERMINAL_FONT_CYAN()        PRINTF("\033[1;36m")
#define TERMINAL_FONT_WHITE()       PRINTF("\033[1;37m")

/* 背景颜色 */
#define TERMINAL_BACK_BLACK()       PRINTF("\033[1;40m")
#define TERMINAL_BACK_L_RED()       PRINTF("\033[0;41m")    /* 浅红色 */
#define TERMINAL_BACK_RED()         PRINTF("\033[1;41m")    /* 红色 */
#define TERMINAL_BACK_GREEN()       PRINTF("\033[1;42m")
#define TERMINAL_BACK_YELLOW()      PRINTF("\033[1;43m")
#define TERMINAL_BACK_BLUE()        PRINTF("\033[1;44m")
#define TERMINAL_BACK_PURPLE()      PRINTF("\033[1;45m")
#define TERMINAL_BACK_CYAN()        PRINTF("\033[1;46m")
#define TERMINAL_BACK_WHITE()       PRINTF("\033[1;47m")

/* 清除从光标到行尾的内容 */
#define TERMINAL_CLEAR_END()        PRINTF("\033[K")

/* 清除屏幕 */
#define TERMINAL_DISPLAY_CLEAR()    PRINTF("\033[2J")

/* 上移光标 */
#define TERMINAL_MOVE_UP(x)         PRINTF("\033[%dA", (x))

/* 下移光标 */
#define TERMINAL_MOVE_DOWN(x)       PRINTF("\033[%dB", (x))

/* 左移光标 */
#define TERMINAL_MOVE_LEFT(y)       PRINTF("\033[%dD", (y))

/* 右移光标 */
#define TERMINAL_MOVE_RIGHT(y)      PRINTF("\033[%dC",(y))

/* 定位光标 */
#define TERMINAL_MOVE_TO(x, y)      PRINTF("\033[%d;%dH", (x), (y))

/* 光标复位 */
#define TERMINAL_RESET_CURSOR()     PRINTF("\033[H")

/* 隐藏光标 */
#define TERMINAL_HIDE_CURSOR()      PRINTF("\033[?25l")

/* 显示光标 */
#define TERMINAL_SHOW_CURSOR()      PRINTF("\033[?25h")

/* 反显 */
#define TERMINAL_HIGH_LIGHT()       PRINTF("\033[7m")
#define TERMINAL_UN_HIGH_LIGHT()    PRINTF("\033[27m")

/* 控制终端显示-----------------------------------------------------------END */


#define RX_BUFF_TYPE        QUEUE64_S
extern RX_BUFF_TYPE         cli_rx_buff;


__packed typedef struct {
    const char *pCmd;
    const char *pHelp;
    uint8_t (*pInit)(void);
    uint8_t (*pFun)(void *, uint8_t);
} COMMAND_S;



extern void cli_init(uint32_t baud);
extern void cli_task(void);


#endif /* __SYS_COMMAND_LINE_H */

