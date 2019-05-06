/**
  ******************************************************************************
  * @file:      sys_scheduler.c
  * @author:    Cat(孙关平)
  * @version:   V1.0
  * @date:      2018-1-18
  * @brief:     调度器
  * @attention:
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "./../bsp/bsp_systick.h"
#include "./sys_command_line.h"
#include "./sys_scheduler.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


/* 定义任务序列 */
volatile OS_TASK_S os_task[OS_MAX_TASKS];
/* 当前正在执行的任务ID */
volatile uint8_t os_running_task_id = 0;

#if OS_WATCH_DOG
    volatile uint16_t os_watch_dog_cnt;
#endif

typedef struct {
    uint8_t flag_50ms: 1;
    uint8_t cnt_50ms: 7;
    uint8_t flag_500ms: 1;
    uint8_t cnt_500ms: 7;
} NORMAL_TIMER_S;

NORMAL_TIMER_S normal_timers = {0, 0, 0, 0};


/**
  * @brief                      为应用程序提供一个基础的50ms定时，此函数为__weak
  * @param                      null
  * @retval                     None
  */
__weak void normal_timer_50ms(void)
{
    /* 
        注意这个函数加了弱符号"__weak"，如果应用程序需要50ms定时接口，可以在用户
        程序中定义一个名字一样的函数normal_timer_50ms，编译器会自动编译不带弱符
        号"__weak"的函数。这样做的好处是用户不管需要定时50ms接口还是不需要，都不
        需要来修改本文件。
    */
}

/**
  * @brief                      为应用程序提供一个基础的500ms定时，此函数为__weak
  * @param                      null
  * @retval                     None
  */
__weak void normal_timer_500ms(void)
{
    /* 
        注意这个函数加了弱符号"__weak"，如果应用程序需要500ms定时接口，可以在用户
        程序中定义一个名字一样的函数normal_timer_500ms，编译器会自动编译不带弱符
        号"__weak"的函数。这样做的好处是用户不管需要定时500ms接口还是不需要，都不
        需要来修改本文件。
    */
}

/**
  * @brief                      为应用程序提供一个基础的50ms定时
  * @param  NORMAL_TIMER_S*:    定时结构体指针
  * @retval                     None
  */
void normal_timer_50ms_handle(NORMAL_TIMER_S* t)
{
    if(TRUE == t->flag_50ms) {
        t->flag_50ms = FALSE;
        //PRINTF("normal timer 50ms\r\n");
        normal_timer_50ms();
    }
}

/**
  * @brief                      为应用程序提供一个基础的500ms定时
  * @param  NORMAL_TIMER_S*:    定时结构体指针
  * @retval                     None
  */
void normal_timer_500ms_handle(NORMAL_TIMER_S* t)
{
    if(TRUE == t->flag_500ms) {
        t->flag_500ms = FALSE;
        //PRINTF("normal timer 500ms\r\n");
        normal_timer_500ms();
    }
}


#if OS_TASK_TIMER
/**
  * @brief          调度器任务延时
  * @param  time:   任务延时时间，必须是OS_SYS_TICK_MS的整数倍
  * @retval         None
  */
void os_timer_set(uint16_t time)
{
    os_task[os_running_task_id].timer = time / OS_SYS_TICK_MS;
}

/**
  * @brief  调度器任务延时
  * @param  None
  * @retval 任务定时器当前值
  */
uint16_t os_timer_check(void)
{
    return os_task[os_running_task_id].timer;
}
#endif

/**
  * @brief              调度器创建任务
  * @param  OS_TASK_S*: 任务控制块指针
  * @param  func:       任务函数指针
  * @param  perid:      任务执行周期
  * @retval             None
  */
void os_task_creat(volatile OS_TASK_S *ptask, void (*func)(void), uint16_t perid)
{
    uint8_t i;

    for(i = 0; i < OS_MAX_TASKS; i++) {
        if(FALSE == ptask[i].exist_flag) {  /* 找到了一个没有使用的任务控制块 */
            os_task[i].func = func;
            os_task[i].tick_backup = perid / OS_SYS_TICK_MS;  /* 计算需要多少个tick */
            os_task[i].tick_counter = os_task[i].tick_backup;
            os_task[i].exist_flag = TRUE;
            os_task[i].ready_flag = TRUE;
            os_task[i].suspend_flag = FALSE;
            os_task[i].sleep_flag = FALSE;

#if OS_DEBUG
            PRINTF("creat task[%d] ok\r\n", i);
#endif /* OS_DEBUG */

            return; /* 任务创建成功，返回 */
        }
    }

    /* 运行到这里说明没有多余的任务块了，创建任务失败 */
    DIE("creat task err!");
}


/**
  * @brief                  任务挂起
  * @param  *task_func:     任务函数指针，也就是要挂起的任务
  * @param  suspend_state:  ENABLE表示执行任务挂起
  * @retval TASK_NULL:      表示任务不存在
            TASK_SUSPEND:   表示任务挂起成功
            TASK_UNSUSPEND: 表示任务取消挂起成功
  */
OS_TASK_STATE_E os_task_suspend(void *task_func, uint8_t suspend_state)
{
    uint8_t i;
    OS_TASK_STATE_E state = TASK_NULL;

    for(i = 0; i < OS_MAX_TASKS; i++) {
        if(os_task[i].func == task_func) {
            if(ENABLE == suspend_state) {  /* 挂起 */
                os_task[i].suspend_flag = TRUE;
                state = TASK_SUSPEND;
#if OS_DEBUG
                PRINTF("task[%d] suspend\r\n", i);
#endif /* OS_DEBUG */

            } else {
                os_task[i].suspend_flag = FALSE;
                state = TASK_UNSUSPEND;
#if OS_DEBUG
                PRINTF("task[%d] unsuspend\r\n", i);
#endif /* OS_DEBUG */

            }

            break;
        }
    }

    return state;
}


/**
  * @brief                  任务挂起查询
  * @param  *task_func:     任务函数指针
  * @retval TASK_NULL:      表示任务不存在
            TASK_SUSPEND:   表示任务被挂起
            TASK_UNSUSPEND: 表示任务没有被挂起
  */
OS_TASK_STATE_E os_task_suspend_check(void *task_func)
{
    uint8_t i;
    OS_TASK_STATE_E state = TASK_NULL;

    for(i = 0; i < OS_MAX_TASKS; i++) {
        if(os_task[i].func == task_func) {
            if(TRUE == os_task[i].suspend_flag) {  /* 挂起 */
                state = TASK_SUSPEND;
            } else {
                state = TASK_UNSUSPEND;
            }

            break;
        }
    }

    return state;
}


#if OS_SLEEP
/**
  * @brief                  MCU被唤醒，就给每个任务控制块唤醒标志
  * @param  OS_TASK_S*:     任务控制块指针
  * @param  suspend_state:  ENABLE表示执行任务挂起
  * @retval                 None
  */
static void os_wakeup(volatile OS_TASK_S *ptask)
{
    uint8_t i;

    for(i = 0; i < OS_MAX_TASKS; i++) {
        if(TRUE == ptask[i].exist_flag) {
            ptask[i].sleep_flag = FALSE;
            ptask[i].wakeup_flag = TRUE;
        }
    }

#if OS_DEBUG
    PRINTF("sys wakeup\r\n");
#endif /* OS_DEBUG */

}


/**
  * @brief              MCU休眠
  * @param  OS_TASK_S*: 任务控制块指针
  * @retval             None
  */
static void os_sleep(volatile OS_TASK_S *ptask)
{
    uint8_t i;
    uint8_t ready = TRUE;

    /* 查询是否每个任务都处于睡眠就绪状态 */
    for(i = 0; i < OS_MAX_TASKS; i++) {
        if(TRUE == ptask[i].exist_flag) {
            /* 有任意一个任务没有在睡眠就绪状态，系统就要等待这个任务睡眠就绪 */
            if(FALSE == ptask[i].sleep_flag) {
                ready = FALSE; /* 查询到一个没有准备好，就退出 */
                PRINTF("not ready %d", i);
                break;
            }
        }
    }

    if(TRUE == ready) {

#if OS_DEBUG
        PRINTF("sys sleep\r\n");
#endif /* OS_DEBUG */

        /* 调用底层，让MCU进入休眠 */
        ENTER_SLEEP();

        /* 运行这里说明MCU被唤醒了 */
        os_wakeup(ptask);
    }
}
#endif


/**
  * @brief              调度器开始运行
  * @param  OS_TASK_S*: 任务控制块指针
  * @retval             None
  */
void os_start(volatile OS_TASK_S *ptask)
{
    uint8_t i;

#if OS_DEBUG
    PRINTF("OS running\r\n");
#endif /* OS_DEBUG */

    /* 初始化看门狗 */
    WATCH_DOG_INIT();

    while(1) {
        normal_timer_50ms_handle(&normal_timers);
        normal_timer_500ms_handle(&normal_timers);

        for(i = 0; i < OS_MAX_TASKS; i++) {
            if(TRUE == ptask[i].ready_flag) {
                ptask[i].ready_flag = FALSE;

                if((TRUE == ptask[i].exist_flag) && (NULL != ptask[i].func)) {
                    os_running_task_id = i;
                    // TASK RUN..
                    (*(ptask[i].func))();
                }
            }
        }

#if OS_WATCH_DOG
        /* 逻辑喂狗 */
        os_watch_dog_cnt = 0;
#endif

#if OS_SLEEP
        os_sleep(ptask);
#endif /* OS_SLEEP */
    }
}


/**
  * @brief              调度器所需要的定时和更新任务就绪状态函数，需要有定时器
                            每OS_SYS_TICK_MS时间被定时器调度一次
  * @param  OS_TASK_S*: 任务控制块指针
  * @retval             None
  */
void os_interrupt_handle(volatile OS_TASK_S *ptask)
{
    uint8_t i;

    /* 普通定时 */
    if(++normal_timers.cnt_50ms >= 5) {         /* 50ms到了 */

#if OS_WATCH_DOG
        if(os_watch_dog_cnt++ > 200) {          /* 超过10S */
            ERR("watch dog err");
        } else {
            WATCH_DOG_FEED();                   /* 50ms喂一次狗 */
        }

#endif
        normal_timers.cnt_50ms = 0;
        normal_timers.flag_50ms = TRUE;

        if(++normal_timers.cnt_500ms >= 10) {   /* 500ms到了 */
            normal_timers.cnt_500ms = 0;
            normal_timers.flag_500ms = TRUE;
        }
    }

    /* 系统任务调度 */
    for(i = 0; i < OS_MAX_TASKS; i++) {

#if OS_TASK_TIMER

        /**********************
         * 任务定时器计时处理
         **********************/
        if(ptask[i].timer) {
            ptask[i].timer--;
        }

#endif /* OS_TASK_TIMER */

        /********************************
         * 检测任务是否可以进入就绪状态
         ********************************/
        if(FALSE == ptask[i].suspend_flag) {    /* 任务没有被挂起 */
            if(ptask[i].tick_backup) {
                if(FALSE == ptask[i].sleep_flag) {  /* 任务不是休眠就绪状态 */
                    if(!--ptask[i].tick_counter) {
                        ptask[i].tick_counter = ptask[i].tick_backup;
                        ptask[i].ready_flag = TRUE;
                    }
                }
            }
        } else {
            /* 任务被挂起 */
            ptask[i].ready_flag = FALSE;
        }
    }
}


/**
  * @brief              调度器初始化
  * @param  OS_TASK_S*: 任务控制块指针
  * @retval             None
  */
void os_init(volatile OS_TASK_S *ptask)
{
    extern const uint8_t LHD8006_TYPE;      /* 主机类型，定义在main.c文件 */

    memset((void *)ptask, 0x00, OS_MAX_TASKS * sizeof(OS_TASK_S));
    OS_TIMER_INIT(OS_SYS_TICK_MS);

    /*初始化CommandLine 串口 768000 8-N-1 */
    CLI_INIT(CLI_BAUDRATE);

#if OS_INFO

    /* 打印主机版本 */
    PRINTF("panel: \r\n");

    /* 打印编译时间 */
    PRINTF("build: %s %s\r\n\r\n", __DATE__, __TIME__);
#endif /* OS_INFO */
}


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
