/**
  ******************************************************************************
  * @file:      app_buzzer.h
  * @author:    Cat£¨Ëï¹ØÆ½£©
  * @version:   V1.0
  * @date:      2018-3-3
  * @brief:     ·äÃùÆ÷
  * @attention:
  ******************************************************************************
  */


#ifndef __APP_BUZZER_H
#define __APP_BUZZER_H


#include "global.h"

#include "./../sys/sys_scheduler.h"
#include "./../sys/sys_command_line.h"
#include "./../sys/sys_queue.h"



enum {
    E_BUZZER_NULL,
    E_BUZZER_SHORT_SOUND,
    E_BUZZER_SHORT_HOLD,
    E_BUZZER_LONG_SOUND,
    E_BUZZER_LONG_HOLD,
    E_BUZZER_HOLD_SILENCE,
};


extern QUEUE8_S buzzer_queue;


extern void buzzer_notice_ok(void);
extern void buzzer_notice_fail(void);
extern void buzzer_notice_invalid_config(void);
extern void buzzer_notice_config_ok(void);
extern void buzzer_notice_config_fail(void);
extern void buzzer_init(void);
extern void buzzer_task(void);


#endif /* __APP_BUTTON_H */
