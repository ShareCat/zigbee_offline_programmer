/**
  ******************************************************************************
  * @file:      sys_queue.h
  * @author:    Cat(���ƽ)
  * @version:   V1.0
  * @date:      2018-1-18
  * @brief:     ����
  * @attention:
  ******************************************************************************
  */


#ifndef __SYS_QUEUE_H
#define __SYS_QUEUE_H

#include <stdint.h>
#include "global.h"


/*
STM32��ʹ��ʱ��ʱ��Ҫ����ȫ���жϣ�����MCU������������������ⲿ�жϣ���ֹ�����������ⲿ�жϴ�����������ʧ�ܡ�
ARM MDK���ṩ�����������ӿ������úͿ������жϣ�
__disable_irq();   // �ر����ж�
__enable_irq();    // �������ж�
�����Է�������һ�����⣬�ڹر����жϺ�������жϴ�������Ȼ��ʱ���������жϣ�
���ڵ���__enable_irq()�������жϺ�MCU����������֮ǰ�������жϡ���˵��__disable_irq()ֻ��
��ֹCPUȥ��Ӧ�жϣ�û��������ȥ�����жϵĴ������жϷ�������Ӧ�ļĴ����Ὣ�жϱ�־��λ��
��__enable_irq()�����жϺ�������Ӧ���жϱ�־û����գ�������ᴥ���жϡ�����Ҫ���ֹ����
�жϣ���������ģ����жϽ���Disable����������ÿ��ģ���ж�Դ�кܶ࣬������ж�Disable�Ļ���
�ϸ��ӣ���Ϊ�򵥵ķ�����ͨ��XXX_ClearITPendingBit()����жϱ�־����ֱ��ͨ��XXX_DeInit()����
���Ĵ�����״̬��������__enable_irq()�������жϺ�MCU�Ͳ�����Ӧ֮ǰ�������ж��ˡ�
*/

#define ENABLE_ALL_IRQ()    __enable_irq()     /* �ر����ж� */
#define DISABLE_ALL_IRQ()   __disable_irq()    /* �����ж� */



extern uint8_t queue_init(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len);
extern uint8_t queue_full(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len);
extern uint8_t queue_empty(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len);
extern uint8_t queue_in(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len, uint8_t *PData);
extern uint8_t queue_out(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len, uint8_t *PData);

//��ʼ��q����
#define QUEUE_INIT(q)       queue_init((uint16_t *) &(q.Front),(uint16_t *) &(q.Rear),(uint8_t *) &(q.PBase[0]),(uint16_t) (sizeof(q.PBase)))
//�ж�q�����Ƿ�����
#define QUEUE_FULL(q)       queue_full((uint16_t *) &(q.Front),(uint16_t *) &(q.Rear),(uint8_t *) &(q.PBase[0]),(uint16_t) (sizeof(q.PBase)))
//�ж�q�����Ƿ��
#define QUEUE_EMPTY(q)      queue_empty((uint16_t *) &(q.Front),(uint16_t *) &(q.Rear),(uint8_t *) &(q.PBase[0]),(uint16_t) (sizeof(q.PBase)))
//��pdata�ŵ�q����
#define QUEUE_IN(q, pdata)  queue_in((uint16_t *) &(q.Front),(uint16_t *) &(q.Rear),(uint8_t *) &(q.PBase[0]),(uint16_t) (sizeof(q.PBase)),(uint8_t *) &pdata)
//��q�����ó�һ��Ԫ�ص�pdata
#define QUEUE_OUT(q, pdata) queue_out((uint16_t *) &(q.Front),(uint16_t *) &(q.Rear),(uint8_t *) &(q.PBase[0]),(uint16_t) (sizeof(q.PBase)),(uint8_t *) &pdata)

//8 Bytes Queue Struct
__packed typedef struct queue8 {
    uint16_t    Front;
    uint16_t    Rear;
    uint8_t     PBase[8 + 1];
} QUEUE8_S;

//16 Bytes Queue Struct
__packed typedef struct queue16 {
    uint16_t    Front;
    uint16_t    Rear;
    uint8_t     PBase[16 + 1];
} QUEUE16_S;

//32 Bytes Queue Struct
__packed typedef struct queue32 {
    uint16_t    Front;
    uint16_t    Rear;
    uint8_t     PBase[32 + 1];
} QUEUE32_S;

//64 Bytes Queue Struct
__packed typedef struct queue64 {
    uint16_t    Front;
    uint16_t    Rear;
    uint8_t     PBase[64 + 1];
} QUEUE64_S;

//128 Bytes Queue Struct
__packed typedef struct queue128 {
    uint16_t    Front;
    uint16_t    Rear;
    uint8_t     PBase[128 + 1];
} QUEUE128_S;

//256 Bytes Queue Struct
__packed typedef struct queue256 {
    uint16_t    Front;
    uint16_t    Rear;
    uint8_t     PBase[256 + 1];
} QUEUE256_S;

__packed typedef struct queue1024 {
    uint16_t    Front;
    uint16_t    Rear;
    uint8_t     PBase[1024 + 1];
} QUEUE1024_S;

__packed typedef struct queue2048 {
    uint16_t    Front;
    uint16_t    Rear;
    uint8_t     PBase[2048 + 1];
} QUEUE2048_S;


#endif /* __SYS_QUEUE_H */

