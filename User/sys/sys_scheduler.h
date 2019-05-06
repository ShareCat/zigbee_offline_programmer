/**
  ******************************************************************************
  * @file:      sys_scheduler.h
  * @author:    Cat(孙关平)
  * @version:   V1.0
  * @date:      2018-1-18
  * @brief:     调度器
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
#define OS_WATCH_DOG        TRUE    /* 是否使用看门狗 */
#define OS_SLEEP            FALSE   /* 是否使用休眠 */
#define OS_TASK_TIMER       TRUE    /* 是否给每个任务分配一个定时器 */
#define OS_MAX_TASKS        TEN     /* 调度器最大任务个数 */
#define OS_SYS_TICK_MS      T_10MS  /* 调度器心跳时间10ms */
#define OS_DEBUG            FALSE   /* 是否开始调度器调试 */
#define OS_INFO             TRUE    /* 上电是否打印调度器信息 */



/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

#define OS_TIMER_INIT(t_ms) systick_init(t_ms) /* 调度器的心跳定时器初始化，t_ms
                                                    为调度器心跳时间，单位ms */
#define OS_TIMER_RUN        ;
#define OS_TIMER_STOP       ;

#if OS_WATCH_DOG
#define WATCH_DOG_INIT()    watch_dog_init()
#define WATCH_DOG_FEED()    watch_dog_feed()
#else
#define WATCH_DOG_INIT()    ;
#define WATCH_DOG_FEED()    ;
#endif /* OS_WATCH_DOG */

#define ENTER_SLEEP()       ;               /* 由底层提供MCU进入休眠方法 */


typedef enum {
    TASK_NULL =      0,     /* 任务不存在 */
    TASK_UNSUSPEND = 1,     /* 任务没有被挂起 */
    TASK_SUSPEND =   2,     /* 任务被挂起 */
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


/* 任务控制块结构 */
__packed typedef struct {
    uint8_t exist_flag: 1;      /* 任务存在标记，1表示这个任务存在 */
    uint8_t ready_flag: 1;       /* 任务就绪标记，可以运行，1表示可以运行（等待
                                        调度调用运行） */
    uint8_t suspend_flag: 1;     /* 任务挂起标记，1表示挂起，挂起后就不再对任务
                                        就绪标记进行置位 */
    uint8_t sleep_flag: 1;       /* 任务可以睡眠就绪标记，1表示这个任务就绪，但
                                        需要所有任务都是休眠就绪状态才让MCU进入
                                        休眠 */
    uint8_t wakeup_flag: 1;     /* MCU睡眠后被唤醒，就会置1 */
    uint16_t tick_backup;       /* 任务周期时间备份 */
    uint16_t tick_counter;      /* 任务周期时间计算 */
    void (*func)(void);         /* 任务入口函数 */


#if OS_TASK_TIMER
    uint16_t timer;             /* 每个任务分配一个定时器，最小定时单位为
                                        OS_SYS_TICK_MS的倍数 */
#endif /* OS_TASK_TIMER */
} OS_TASK_S;

/* 定义任务序列 */
extern volatile OS_TASK_S os_task[OS_MAX_TASKS];
/* 当前正在执行的任务ID */
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
