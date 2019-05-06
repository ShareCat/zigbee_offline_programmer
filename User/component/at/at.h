/**
  ******************************************************************************
  * @file:      at.h
  * @author:    Cat�����ƽ��
  * @version:   V1.0
  * @date:      2018-5-9
  * @brief:     AT�����������
  * @attention:
  ******************************************************************************
  */


#ifndef __AT_H
#define __AT_H


#include "stdint.h"
#include "./../../sys/sys_scheduler.h"
#include "./../../sys/sys_command_line.h"
#include "./../../sys/sys_queue.h"


enum {
    AT_STAT_IDLE,                       /* ����AT֡ͷ��״̬ */
    AT_STAT_RECEIVE,                    /* ���ձ�������״̬ */
    AT_STAT_PROCESS                     /* �����ݸ���Ӧ���������״̬ */
};


typedef struct {
    char* cmd_name;                     /* ���� */
    uint8_t cmd_len;                    /* ����� */
    void (*setup_func)(char *pPara);    /* ����������'='��'0'��'9' */
    void (*exe_func)(void);             /* ����������'\r' */
    void (*query_func)(void);           /* ����������'?\r' */
    void (*test_func)(void);            /* ����������'=?\r' */
} AT_CMD_S;


typedef struct AT_S {
#define CMD_MAX_LEN     32              /* ����ʱ����ʱ���������buff��С��Ҳ
                                                ����֧��������ֽ��� */

    QUEUE32_S rx_buff;                  /* ���ն��л��� */
    uint8_t sta : 7;                    /* ����״̬ */
    uint8_t receive_flag : 1;           /* ���ջ����״̬ */
    uint8_t* p_cmd;                     /* ����ʱ���õ���ָ�� */
    const AT_CMD_S* cmd_list;           /* ָ������ṹ */
    uint8_t cmd_max;                    /* ����ṹ��������� */
    void (*send_char)(uint8_t c);       /* ����һ���ַ��ӿ� */
    struct AT_S* next;
} AT_S;


extern void at_get_char(AT_S* p_at, uint8_t c);
extern void at_task(void);
extern uint8_t at_init(AT_S* p_at,
                       const AT_CMD_S* p_at_cmd_list,
                       uint8_t cmd_num,
                       void(*p_send_char)(uint8_t c));


#endif /* __AT_H */
