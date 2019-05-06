/**
  ******************************************************************************
  * @file    stm32f0xx_pwr.h
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    05-December-2014
  * @brief   This file contains all the functions prototypes for the PWR firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_PWR_H
#define __BSP_PWR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "global.h"


#define BSP_PWR_DEBUG

enum {
    SLEEP_MODE,     /* 功耗还是比较高 */
    STOP_MODE,      /* 手册说20uA左右 */
    STANDBY_MODE    /* 手册说2uA左右 */
};


/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

extern void pwr_reset_clock(void);
extern void pwr_init(void);
extern void pwr_ctrl(uint8_t mode);



#endif /* __BSP_PWR_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
