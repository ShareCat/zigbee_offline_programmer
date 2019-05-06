

/* Includes ------------------------------------------------------------------*/
#include "bsp_chip_id.h"
#include "./../sys/sys_command_line.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  获取stm32芯片ID
  * @param  chip_id 接收保存ID
  * @retval None
  */
void get_chip_id(uint8_t chip_id[12])
{
    uint8_t index;

    //PRINTF("UID: ");
    for(index = 0; index < 12; index++) {
        chip_id[index] = *(volatile unsigned char*)(CHIP_ID_ADDR + index);
        //PRINTF("%02x ", chip_id[index]);
    }
}


/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

