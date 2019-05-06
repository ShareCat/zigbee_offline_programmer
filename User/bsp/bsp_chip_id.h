#ifndef __BSP_CHIP_ID_H
#define __BSP_CHIP_ID_H


#include "global.h"

#ifdef STM32F0
    #define CHIP_ID_ADDR    0x1FFFF7AC
#else
    #define CHIP_ID_ADDR    0x1FFFF7E8
#endif

extern void get_chip_id(uint8_t chip_id[12]);

#endif /* __BSP_CHIP_ID_H */

