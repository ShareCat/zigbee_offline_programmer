

#include "./../sys/sys_scheduler.h"
#include "bsp_systick.h"


/**
  * @brief  ����ϵͳ�δ�ʱ�� SysTick
  * @param  ��
  * @retval ��
    * Setup SysTick Timer for 1 msec interrupts.
     ------------------------------------------
    1. The SysTick_Config() function is a CMSIS function which configure:
       - The SysTick Reload register with value passed as function parameter.
       - Configure the SysTick IRQ priority to the lowest value (0x0F).
       - Reset the SysTick Counter register.
       - Configure the SysTick Counter clock source to be Core Clock Source (HCLK).
       - Enable the SysTick Interrupt.
       - Start the SysTick Counter.

    2. You can change the SysTick Clock source to be HCLK_Div8 by calling the
       SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8) just after the
       SysTick_Config() function call. The SysTick_CLKSourceConfig() is defined
       inside the misc.c file.

    3. You can change the SysTick IRQ priority by calling the
       NVIC_SetPriority(SysTick_IRQn,...) just after the SysTick_Config() function
       call. The NVIC_SetPriority() is defined inside the core_cm0.h file.

    4. To adjust the SysTick time base, use the following formula:

         Reload Value = SysTick Counter Clock (Hz) x  Desired Time base (s)

       - Reload Value is the parameter to be passed for SysTick_Config() function
       - Reload Value should not exceed 0xFFFFFF
   *********************************************************************************/
void systick_init(uint8_t ticket_ms)
{
    /* SystemFrequency / 1000    1ms�ж�һ��
     * SystemFrequency / 100000  10us�ж�һ��
     * SystemFrequency / 1000000 1us�ж�һ��
     */

    uint32_t temp;

    temp = 1000 / ticket_ms;

    if(SysTick_Config(SystemCoreClock / temp)) {
        /* Capture error */
        while(1);
    }
}


/**
  * @brief  ��ȡ���ĳ���
  * @param  ��
  * @retval ��
  * @attention  �� SysTick �жϺ��� SysTick_Handler()����
  */
void systick_irq(void)
{
    os_interrupt_handle(os_task);
}


/*********************************************END OF FILE**********************/
