#ifndef __BSP_SYSTICK_H
#define __BSP_SYSTICK_H


#include "stm32f10x_tim.h"

#include "global.h"

extern void systick_init(uint8_t ticket_ms);
extern void systick_irq(void);



#endif /* __BSP_SYSTICK_H */
