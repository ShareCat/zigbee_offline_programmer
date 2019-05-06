/**
  ******************************************************************************
  * @file:      sys_scheduler.h
  * @author:    Cat(���ƽ)
  * @version:   V1.0
  * @date:      2018-1-18
  * @brief:     ������
  * @attention:
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SYS_SCHEDULER_H
#define __SYS_SCHEDULER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "global.h"
#include "./../bsp/bsp_iwdg.h"


/* Macro config --------------------------------------------------------------*/
#define OS_WATCH_DOG        TRUE    /* �Ƿ�ʹ�ÿ��Ź� */
#define OS_SLEEP            FALSE   /* �Ƿ�ʹ������ */
#define OS_TASK_TIMER       TRUE    /* �Ƿ��ÿ���������һ����ʱ�� */
#define OS_MAX_TASKS        TEN     /* ���������������� */
#define OS_SYS_TICK_MS      T_10MS  /* ����������ʱ��10ms */
#define OS_DEBUG            FALSE   /* �Ƿ�ʼ���������� */
#define OS_INFO             TRUE    /* �ϵ��Ƿ��ӡ��������Ϣ */



/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

#define OS_TIMER_INIT(t_ms) systick_init(t_ms) /* ��������������ʱ����ʼ����t_ms
                                                    Ϊ����������ʱ�䣬��λms */
#define OS_TIMER_RUN        ;
#define OS_TIMER_STOP       ;

#if OS_WATCH_DOG
#define WATCH_DOG_INIT()    watch_dog_init()
#define WATCH_DOG_FEED()    watch_dog_feed()
#else
#define WATCH_DOG_INIT()    ;
#define WATCH_DOG_FEED()    ;
#endif /* OS_WATCH_DOG */

#define ENTER_SLEEP()       ;               /* �ɵײ��ṩMCU�������߷��� */


typedef enum {
    TASK_NULL =      0,     /* ���񲻴��� */
    TASK_UNSUSPEND = 1,     /* ����û�б����� */
    TASK_SUSPEND =   2,     /* ���񱻹��� */
} OS_TASK_STATE_E;


enum {
    T_1MS   = 1,        T_2MS   = 2,        T_3MS   = 3,
    T_4MS   = 4,        T_5MS   = 5,        T_6MS   = 6,
    T_7MS   = 7,        T_8MS   = 8,        T_9MS   = 9,
    T_10MS  = 10,       T_20MS  = 20,       T_30MS  = 30,
    T_40MS  = 40,       T_50MS  = 50,       T_60MS  = 60,
    T_70MS  = 70,       T_80MS  = 80,       T_90MS  = 90,
    T_100MS = 100,      T_200MS = 200,      T_300MS = 300,
    T_400MS = 400,      T_500MS = 500,      T_600MS = 600,
    T_700MS = 700,      T_800MS = 800,      T_900MS = 900,
    T_1S    = 1000,     T_2S    = 2000,     T_3S    = 3000,
    T_4S    = 4000,     T_5S    = 5000,     T_6S    = 6000,
    T_7S    = 7000,     T_8S    = 8000,     T_9S    = 9000,
    T_10S   = 10000,    T_15S   = 15000,    T_20S   = 20000,
    T_25S   = 25000,    T_30S   = 30000,    T_35S   = 35000,
    T_40S   = 40000,    T_45S   = 45000,    T_50S   = 50000,
    T_55S   = 55000,    T_60S   = 60000,
};


/* ������ƿ�ṹ */
__packed typedef struct {
    uint8_t exist_flag: 1;      /* ������ڱ�ǣ�1��ʾ���������� */
    uint8_t ready_flag: 1;       /* ���������ǣ��������У�1��ʾ�������У��ȴ�
                                        ���ȵ������У� */
    uint8_t suspend_flag: 1;     /* ��������ǣ�1��ʾ���𣬹����Ͳ��ٶ�����
                                        ������ǽ�����λ */
    uint8_t sleep_flag: 1;       /* �������˯�߾�����ǣ�1��ʾ��������������
                                        ��Ҫ�������������߾���״̬����MCU����
                                        ���� */
    uint8_t wakeup_flag: 1;     /* MCU˯�ߺ󱻻��ѣ��ͻ���1 */
    uint16_t tick_backup;       /* ��������ʱ�䱸�� */
    uint16_t tick_counter;      /* ��������ʱ����� */
    void (*func)(void);         /* ������ں��� */


#if OS_TASK_TIMER
    uint16_t timer;             /* ÿ���������һ����ʱ������С��ʱ��λΪ
                                        OS_SYS_TICK_MS�ı��� */
#endif /* OS_TASK_TIMER */
} OS_TASK_S;

/* ������������ */
extern volatile OS_TASK_S os_task[OS_MAX_TASKS];
/* ��ǰ����ִ�е�����ID */
extern volatile uint8_t os_running_task_id;


#if OS_TASK_TIMER
extern void os_timer_set(uint16_t t);
extern uint16_t os_timer_check(void);
#endif
extern void os_init(volatile OS_TASK_S *ptask);
extern void os_timer_run(void);
extern void os_timer_stop(void);
extern void os_task_creat(volatile OS_TASK_S *ptask, void (*func)(void), uint16_t perid);
extern void os_start(volatile OS_TASK_S *ptask);
extern void os_interrupt_handle(volatile OS_TASK_S *ptask);
extern OS_TASK_STATE_E os_task_suspend(void *task_func, uint8_t suspend_state);
extern OS_TASK_STATE_E os_task_suspend_check(void *task_func);





#endif /* __SYS_SCHEDULER_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
