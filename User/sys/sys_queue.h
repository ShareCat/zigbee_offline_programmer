/**
  ******************************************************************************
  * @file:      sys_queue.h
  * @author:    Cat(孙关平)
  * @version:   V1.0
  * @date:      2018-1-18
  * @brief:     队列
  * @attention:
  ******************************************************************************
  */


#ifndef __SYS_QUEUE_H
#define __SYS_QUEUE_H

#include <stdint.h>
#include "global.h"


/*
STM32在使用时有时需要禁用全局中断，比如MCU在升级过程中需禁用外部中断，防止升级过程中外部中断触发导致升级失败。
ARM MDK中提供了如下两个接口来禁用和开启总中断：
__disable_irq();   // 关闭总中断
__enable_irq();    // 开启总中断
但测试发现这样一个问题，在关闭总中断后，如果有中断触发，虽然此时不会引发中断，
但在调用__enable_irq()开启总中断后，MCU会立即处理之前触发的中断。这说明__disable_irq()只是
禁止CPU去响应中断，没有真正的去屏蔽中断的触发，中断发生后，相应的寄存器会将中断标志置位，
在__enable_irq()开启中断后，由于相应的中断标志没有清空，因而还会触发中断。所以要想禁止所有
中断，必须对逐个模块的中断进行Disable操作，由于每个模块中断源有很多，对逐个中断Disable的话比
较复杂，较为简单的方法是通过XXX_ClearITPendingBit()清除中断标志或者直接通过XXX_DeInit()来清
除寄存器的状态。这样在__enable_irq()开启总中断后，MCU就不会响应之前触发的中断了。
*/

#define ENABLE_ALL_IRQ()    __enable_irq()     /* 关闭总中断 */
#define DISABLE_ALL_IRQ()   __disable_irq()    /* 打开总中断 */



extern uint8_t queue_init(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len);
extern uint8_t queue_full(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len);
extern uint8_t queue_empty(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len);
extern uint8_t queue_in(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len, uint8_t *PData);
extern uint8_t queue_out(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len, uint8_t *PData);

//初始化q队列
#define QUEUE_INIT(q)       queue_init((uint16_t *) &(q.Front),(uint16_t *) &(q.Rear),(uint8_t *) &(q.PBase[0]),(uint16_t) (sizeof(q.PBase)))
//判断q队列是否已满
#define QUEUE_FULL(q)       queue_full((uint16_t *) &(q.Front),(uint16_t *) &(q.Rear),(uint8_t *) &(q.PBase[0]),(uint16_t) (sizeof(q.PBase)))
//判断q队列是否空
#define QUEUE_EMPTY(q)      queue_empty((uint16_t *) &(q.Front),(uint16_t *) &(q.Rear),(uint8_t *) &(q.PBase[0]),(uint16_t) (sizeof(q.PBase)))
//将pdata放到q队列
#define QUEUE_IN(q, pdata)  queue_in((uint16_t *) &(q.Front),(uint16_t *) &(q.Rear),(uint8_t *) &(q.PBase[0]),(uint16_t) (sizeof(q.PBase)),(uint8_t *) &pdata)
//从q队列拿出一个元素到pdata
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

