/**
  ******************************************************************************
  * @file:      sys_command_line.h
  * @author:    Cat(���ƽ)
  * @version:   V1.0
  * @date:      2018-1-18
  * @brief:     ����������
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
#define CLI_ENABLE          TRUE                    /* �Ƿ�ʹ�������� */
#define CLI_HISTORY         TRUE                    /* �Ƿ�ʹ����ʷ��¼ */
#define HISTORY_MAX         10                      /* ��ʷ��¼���� */
#define CLI_PRINTF          TRUE                    /* ���ڴ�ӡ���� */
#define CLI_BAUDRATE        ((uint32_t)768000)      /* ���ڲ����� */
#define USART_INIT(baud)    debug_usart_init(baud)  /* ���ڳ�ʼ������ */
#define SYSTEM_REBOOT()     NVIC_SystemReset()      /* MCU���� */

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




/* ����-----------------------------------------------------------------BEGIN */

/* #val��ʹ��#�Ѻ������Ϊһ���ַ���,��##�����������������һ�� */
#define ASSERT(val)                                                     \
if (!(val))                                                             \
{                                                                       \
    PRINTF("(%s) has assert failed at %s.\n", #val, __FUNCTION__);      \
    while (1);                                                          \
}

/* ����-------------------------------------------------------------------END */




/* ��ӡ����-------------------------------------------------------------BEGIN */

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

/* ��ӡ����---------------------------------------------------------------END */




/* �����ն���ʾ---------------------------------------------------------BEGIN */

/*
�ο�����:       http://blog.csdn.net/yangguihao/article/details/47734349
            http://blog.csdn.net/kevinshq/article/details/8179252


    @ʵ���ն˽����й��Ķ�λ����������:
        \033[0m     �ر���������
        \033[1m     ���ø�����
        \03[4m      �»���
        \033[5m     ��˸
        \033[7m     ����
        \033[8m     ����
        \033[30m    --  \033[37m  ����ǰ��ɫ
        \033[40m    --  \033[47m  ���ñ���ɫ
        \033[nA     �������n��
        \033[nB     �������n��
        \033[nC     �������n��
        \033[nD     �������n��
        \033[y;xH   ���ù��λ��
        \033[2J     ����
        \033[K      ����ӹ�굽��β������
        \033[s      ������λ��
        \033[u      �ָ����λ��
        \033[?25l   ���ع��
        \33[?25h    ��ʾ���


    @������ɫ��Χ: 40--49                 ����ɫ: 30--39
     40: BLACK                          30: ��
     41: RED                            31: ��
     42: GREEN                          32: ��
     43: YELLOW                         33: ��
     44: BLUE                           34: ��
     45: PURPLE                         35: ��
     46: CYAN                           36: ����
     47: WHITE                          37: ��ɫ
*/

/* ������ɫ */
#define TERMINAL_FONT_BLACK()       PRINTF("\033[1;30m")
#define TERMINAL_FONT_L_RED()       PRINTF("\033[0;31m")    /* ǳ��ɫ */
#define TERMINAL_FONT_RED()         PRINTF("\033[1;31m")    /* ��ɫ */
#define TERMINAL_FONT_GREEN()       PRINTF("\033[1;32m")
#define TERMINAL_FONT_YELLOW()      PRINTF("\033[1;33m")
#define TERMINAL_FONT_BLUE()        PRINTF("\033[1;34m")
#define TERMINAL_FONT_PURPLE()      PRINTF("\033[1;35m")
#define TERMINAL_FONT_CYAN()        PRINTF("\033[1;36m")
#define TERMINAL_FONT_WHITE()       PRINTF("\033[1;37m")

/* ������ɫ */
#define TERMINAL_BACK_BLACK()       PRINTF("\033[1;40m")
#define TERMINAL_BACK_L_RED()       PRINTF("\033[0;41m")    /* ǳ��ɫ */
#define TERMINAL_BACK_RED()         PRINTF("\033[1;41m")    /* ��ɫ */
#define TERMINAL_BACK_GREEN()       PRINTF("\033[1;42m")
#define TERMINAL_BACK_YELLOW()      PRINTF("\033[1;43m")
#define TERMINAL_BACK_BLUE()        PRINTF("\033[1;44m")
#define TERMINAL_BACK_PURPLE()      PRINTF("\033[1;45m")
#define TERMINAL_BACK_CYAN()        PRINTF("\033[1;46m")
#define TERMINAL_BACK_WHITE()       PRINTF("\033[1;47m")

/* ����ӹ�굽��β������ */
#define TERMINAL_CLEAR_END()        PRINTF("\033[K")

/* �����Ļ */
#define TERMINAL_DISPLAY_CLEAR()    PRINTF("\033[2J")

/* ���ƹ�� */
#define TERMINAL_MOVE_UP(x)         PRINTF("\033[%dA", (x))

/* ���ƹ�� */
#define TERMINAL_MOVE_DOWN(x)       PRINTF("\033[%dB", (x))

/* ���ƹ�� */
#define TERMINAL_MOVE_LEFT(y)       PRINTF("\033[%dD", (y))

/* ���ƹ�� */
#define TERMINAL_MOVE_RIGHT(y)      PRINTF("\033[%dC",(y))

/* ��λ��� */
#define TERMINAL_MOVE_TO(x, y)      PRINTF("\033[%d;%dH", (x), (y))

/* ��긴λ */
#define TERMINAL_RESET_CURSOR()     PRINTF("\033[H")

/* ���ع�� */
#define TERMINAL_HIDE_CURSOR()      PRINTF("\033[?25l")

/* ��ʾ��� */
#define TERMINAL_SHOW_CURSOR()      PRINTF("\033[?25h")

/* ���� */
#define TERMINAL_HIGH_LIGHT()       PRINTF("\033[7m")
#define TERMINAL_UN_HIGH_LIGHT()    PRINTF("\033[27m")

/* �����ն���ʾ-----------------------------------------------------------END */


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

