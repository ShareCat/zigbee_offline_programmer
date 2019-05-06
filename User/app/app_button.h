/**
  ******************************************************************************
  * @file:      app_button.h
  * @author:    Cat（孙关平）
  * @version:   V1.0
  * @date:      2018-3-3
  * @brief:     按键处理
  * @attention:
  ******************************************************************************
  */


#ifndef __APP_BUTTON_H
#define __APP_BUTTON_H


#include "global.h"

#include "./../sys/sys_scheduler.h"
#include "./../sys/sys_command_line.h"
#include "./../sys/sys_queue.h"


enum {
    ENB_SINGLE_CLICK,
    ENB_DOUBLE_CLICK,
    ENB_LONG_PRESS,
};

extern QUEUE8_S button_queue;


extern void button_scan_init(void);
extern void button_scan_task(void);


#endif /* __APP_BUTTON_H */
