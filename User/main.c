/**
  ******************************************************************************
  * @file:      main.c
  * @author:    Cat�����ƽ��
  * @version:   V1.0  
  * @date:      2018-1-26
  * @brief:     LHD8006����APPԴ��
  * @attention: 
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "./sys/sys_scheduler.h"
#include "./sys/sys_command_line.h"

#include "./app/app_button.h"
#include "./app/app_buzzer.h"
#include "./app/app_database.h"
#include "./app/app_fatfs.h"
#include "./app/app_mass_storage.h"
#include "./app/app_nxp.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


static uint8_t msd_run = FALSE;


void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
    for(; nCount != 0; nCount--);
}


/*******************************************************************************
*  Function: Driver_OpenEA                                      
*******************************************************************************/
void Driver_OpenEA(void)
{
    __set_PRIMASK(0);
}

/*******************************************************************************
*  Function: Driver_CloseEA                                 
*******************************************************************************/
void Driver_CloseEA(void)
{
    __set_PRIMASK(1);
}


/**
  * @brief  �ر�JTAG��SWD����
  * @param  null
  * @retval null
  */
static void disable_jtag_and_swd(void)
{
#define STM32_SWD_ONLY  /* ֻ����SWD���ԣ��ر�JTAG */

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); //����AFIOʱ��

#ifdef STM32_SWD_ONLY
    /* �ر�JTAG���ܣ�ʹ��SWD */
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
#else
    /* �ر�JTAG��SWD���� */
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
#endif
#undef STM32_SWD_ONLY

#ifdef USING_CM_BACKTRACE
    #define CM_BACKTRACE_HW_VERSION     "V1.0.0"    /* �������汾û�й�ϵ����ȫ��Ϊcm_backtrace��Ҫ */
    #define CM_BACKTRACE_SW_VERSION     "V0.1.0"    /* �������汾û�й�ϵ����ȫ��Ϊcm_backtrace��Ҫ */
    extern void cm_backtrace_init(const char *firmware_name, const char *hardware_ver, const char *software_ver);
    cm_backtrace_init("TestBoard", CM_BACKTRACE_HW_VERSION, CM_BACKTRACE_SW_VERSION);
#endif
}


uint8_t msd_run_get(void)
{
    return msd_run;
}


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    /*!< At this stage the microcontroller clock setting is already configured,
         this is done through SystemInit() function which is called from startup
         file (startup_stm32f10x.s) before to branch to application main.
         To reconfigure the default setting of SystemInit() function, refer to
         system_stm32f10x.c file
       */
    #ifdef APP_PROJECT  /* ���屾����ΪAPP���򣬾���Ҫ�����ж�����ƫ�ƣ�ͬʱKeil�������ҲҪ�޸�Flash��ʼ��ַΪFLASH_APP_ADDR */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, APP_ADDR - NVIC_VectTab_FLASH);
    #endif

    disable_jtag_and_swd();

    /* Add your application code here */
    os_init(os_task);

    /* ���������н�������20ms����һ�� */
    os_task_creat(os_task, cli_task, T_20MS);

    /* ����������20ms����һ�� */
    buzzer_init();
    os_task_creat(os_task, buzzer_task, T_20MS);

    /* ����ɨ������20ms����һ�� */
	button_scan_init();
    os_task_creat(os_task, button_scan_task, T_20MS);

    disable_usb();

    extern uint8_t sw_button_read(void);
    if (0 == sw_button_read()) {
        /* USB MSD�豸��ʼ�� */
        mass_storage_init();
        os_task_creat(os_task, mass_storage_task, T_100MS);
        msd_run = TRUE;
    } else {
        /* ���ݿ��ʼ�� */
        database_init();
        //os_task_creat(os_task, database_task, T_100MS);

        /* FATFS�ļ�ϵͳ��ʼ�� */
        fatfs_init();
        //os_task_creat(os_task, fatfs_task, T_100MS);

        /* NXP_ZIGBEE�������� */
        nxp_init();
        os_task_creat(os_task, nxp_task, T_10MS);
    }

    /* ���������� */
    os_start(os_task);
    
    /* �������е����� */
    while(1);
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1) {
    }
}
#endif

/*********************************************END OF FILE**********************/
