/**
  ******************************************************************************
  * @file:      sys_scheduler.c
  * @author:    Cat(���ƽ)
  * @version:   V1.0
  * @date:      2018-1-18
  * @brief:     ������
  * @attention:
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "./../bsp/bsp_systick.h"
#include "./sys_command_line.h"
#include "./sys_scheduler.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


/* ������������ */
volatile OS_TASK_S os_task[OS_MAX_TASKS];
/* ��ǰ����ִ�е�����ID */
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
  * @brief                      ΪӦ�ó����ṩһ��������50ms��ʱ���˺���Ϊ__weak
  * @param                      null
  * @retval                     None
  */
__weak void normal_timer_50ms(void)
{
    /* 
        ע�������������������"__weak"�����Ӧ�ó�����Ҫ50ms��ʱ�ӿڣ��������û�
        �����ж���һ������һ���ĺ���normal_timer_50ms�����������Զ����벻������
        ��"__weak"�ĺ������������ĺô����û�������Ҫ��ʱ50ms�ӿڻ��ǲ���Ҫ������
        ��Ҫ���޸ı��ļ���
    */
}

/**
  * @brief                      ΪӦ�ó����ṩһ��������500ms��ʱ���˺���Ϊ__weak
  * @param                      null
  * @retval                     None
  */
__weak void normal_timer_500ms(void)
{
    /* 
        ע�������������������"__weak"�����Ӧ�ó�����Ҫ500ms��ʱ�ӿڣ��������û�
        �����ж���һ������һ���ĺ���normal_timer_500ms�����������Զ����벻������
        ��"__weak"�ĺ������������ĺô����û�������Ҫ��ʱ500ms�ӿڻ��ǲ���Ҫ������
        ��Ҫ���޸ı��ļ���
    */
}

/**
  * @brief                      ΪӦ�ó����ṩһ��������50ms��ʱ
  * @param  NORMAL_TIMER_S*:    ��ʱ�ṹ��ָ��
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
  * @brief                      ΪӦ�ó����ṩһ��������500ms��ʱ
  * @param  NORMAL_TIMER_S*:    ��ʱ�ṹ��ָ��
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
  * @brief          ������������ʱ
  * @param  time:   ������ʱʱ�䣬������OS_SYS_TICK_MS��������
  * @retval         None
  */
void os_timer_set(uint16_t time)
{
    os_task[os_running_task_id].timer = time / OS_SYS_TICK_MS;
}

/**
  * @brief  ������������ʱ
  * @param  None
  * @retval ����ʱ����ǰֵ
  */
uint16_t os_timer_check(void)
{
    return os_task[os_running_task_id].timer;
}
#endif

/**
  * @brief              ��������������
  * @param  OS_TASK_S*: ������ƿ�ָ��
  * @param  func:       ������ָ��
  * @param  perid:      ����ִ������
  * @retval             None
  */
void os_task_creat(volatile OS_TASK_S *ptask, void (*func)(void), uint16_t perid)
{
    uint8_t i;

    for(i = 0; i < OS_MAX_TASKS; i++) {
        if(FALSE == ptask[i].exist_flag) {  /* �ҵ���һ��û��ʹ�õ�������ƿ� */
            os_task[i].func = func;
            os_task[i].tick_backup = perid / OS_SYS_TICK_MS;  /* ������Ҫ���ٸ�tick */
            os_task[i].tick_counter = os_task[i].tick_backup;
            os_task[i].exist_flag = TRUE;
            os_task[i].ready_flag = TRUE;
            os_task[i].suspend_flag = FALSE;
            os_task[i].sleep_flag = FALSE;

#if OS_DEBUG
            PRINTF("creat task[%d] ok\r\n", i);
#endif /* OS_DEBUG */

            return; /* ���񴴽��ɹ������� */
        }
    }

    /* ���е�����˵��û�ж����������ˣ���������ʧ�� */
    DIE("creat task err!");
}


/**
  * @brief                  �������
  * @param  *task_func:     ������ָ�룬Ҳ����Ҫ���������
  * @param  suspend_state:  ENABLE��ʾִ���������
  * @retval TASK_NULL:      ��ʾ���񲻴���
            TASK_SUSPEND:   ��ʾ�������ɹ�
            TASK_UNSUSPEND: ��ʾ����ȡ������ɹ�
  */
OS_TASK_STATE_E os_task_suspend(void *task_func, uint8_t suspend_state)
{
    uint8_t i;
    OS_TASK_STATE_E state = TASK_NULL;

    for(i = 0; i < OS_MAX_TASKS; i++) {
        if(os_task[i].func == task_func) {
            if(ENABLE == suspend_state) {  /* ���� */
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
  * @brief                  ��������ѯ
  * @param  *task_func:     ������ָ��
  * @retval TASK_NULL:      ��ʾ���񲻴���
            TASK_SUSPEND:   ��ʾ���񱻹���
            TASK_UNSUSPEND: ��ʾ����û�б�����
  */
OS_TASK_STATE_E os_task_suspend_check(void *task_func)
{
    uint8_t i;
    OS_TASK_STATE_E state = TASK_NULL;

    for(i = 0; i < OS_MAX_TASKS; i++) {
        if(os_task[i].func == task_func) {
            if(TRUE == os_task[i].suspend_flag) {  /* ���� */
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
  * @brief                  MCU�����ѣ��͸�ÿ��������ƿ黽�ѱ�־
  * @param  OS_TASK_S*:     ������ƿ�ָ��
  * @param  suspend_state:  ENABLE��ʾִ���������
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
  * @brief              MCU����
  * @param  OS_TASK_S*: ������ƿ�ָ��
  * @retval             None
  */
static void os_sleep(volatile OS_TASK_S *ptask)
{
    uint8_t i;
    uint8_t ready = TRUE;

    /* ��ѯ�Ƿ�ÿ�����񶼴���˯�߾���״̬ */
    for(i = 0; i < OS_MAX_TASKS; i++) {
        if(TRUE == ptask[i].exist_flag) {
            /* ������һ������û����˯�߾���״̬��ϵͳ��Ҫ�ȴ��������˯�߾��� */
            if(FALSE == ptask[i].sleep_flag) {
                ready = FALSE; /* ��ѯ��һ��û��׼���ã����˳� */
                PRINTF("not ready %d", i);
                break;
            }
        }
    }

    if(TRUE == ready) {

#if OS_DEBUG
        PRINTF("sys sleep\r\n");
#endif /* OS_DEBUG */

        /* ���õײ㣬��MCU�������� */
        ENTER_SLEEP();

        /* ��������˵��MCU�������� */
        os_wakeup(ptask);
    }
}
#endif


/**
  * @brief              ��������ʼ����
  * @param  OS_TASK_S*: ������ƿ�ָ��
  * @retval             None
  */
void os_start(volatile OS_TASK_S *ptask)
{
    uint8_t i;

#if OS_DEBUG
    PRINTF("OS running\r\n");
#endif /* OS_DEBUG */

    /* ��ʼ�����Ź� */
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
        /* �߼�ι�� */
        os_watch_dog_cnt = 0;
#endif

#if OS_SLEEP
        os_sleep(ptask);
#endif /* OS_SLEEP */
    }
}


/**
  * @brief              ����������Ҫ�Ķ�ʱ�͸����������״̬��������Ҫ�ж�ʱ��
                            ÿOS_SYS_TICK_MSʱ�䱻��ʱ������һ��
  * @param  OS_TASK_S*: ������ƿ�ָ��
  * @retval             None
  */
void os_interrupt_handle(volatile OS_TASK_S *ptask)
{
    uint8_t i;

    /* ��ͨ��ʱ */
    if(++normal_timers.cnt_50ms >= 5) {         /* 50ms���� */

#if OS_WATCH_DOG
        if(os_watch_dog_cnt++ > 200) {          /* ����10S */
            ERR("watch dog err");
        } else {
            WATCH_DOG_FEED();                   /* 50msιһ�ι� */
        }

#endif
        normal_timers.cnt_50ms = 0;
        normal_timers.flag_50ms = TRUE;

        if(++normal_timers.cnt_500ms >= 10) {   /* 500ms���� */
            normal_timers.cnt_500ms = 0;
            normal_timers.flag_500ms = TRUE;
        }
    }

    /* ϵͳ������� */
    for(i = 0; i < OS_MAX_TASKS; i++) {

#if OS_TASK_TIMER

        /**********************
         * ����ʱ����ʱ����
         **********************/
        if(ptask[i].timer) {
            ptask[i].timer--;
        }

#endif /* OS_TASK_TIMER */

        /********************************
         * ��������Ƿ���Խ������״̬
         ********************************/
        if(FALSE == ptask[i].suspend_flag) {    /* ����û�б����� */
            if(ptask[i].tick_backup) {
                if(FALSE == ptask[i].sleep_flag) {  /* ���������߾���״̬ */
                    if(!--ptask[i].tick_counter) {
                        ptask[i].tick_counter = ptask[i].tick_backup;
                        ptask[i].ready_flag = TRUE;
                    }
                }
            }
        } else {
            /* ���񱻹��� */
            ptask[i].ready_flag = FALSE;
        }
    }
}


/**
  * @brief              ��������ʼ��
  * @param  OS_TASK_S*: ������ƿ�ָ��
  * @retval             None
  */
void os_init(volatile OS_TASK_S *ptask)
{
    extern const uint8_t LHD8006_TYPE;      /* �������ͣ�������main.c�ļ� */

    memset((void *)ptask, 0x00, OS_MAX_TASKS * sizeof(OS_TASK_S));
    OS_TIMER_INIT(OS_SYS_TICK_MS);

    /*��ʼ��CommandLine ���� 768000 8-N-1 */
    CLI_INIT(CLI_BAUDRATE);

#if OS_INFO

    /* ��ӡ�����汾 */
    PRINTF("panel: \r\n");

    /* ��ӡ����ʱ�� */
    PRINTF("build: %s %s\r\n\r\n", __DATE__, __TIME__);
#endif /* OS_INFO */
}


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
