

#include "bsp_iwdg.h"


/*
 * ���� IWDG �ĳ�ʱʱ��
 * Tout = prv/40 * rlv (s)
 *      prv������[4,8,16,32,64,128,256]
 * prv:Ԥ��Ƶ��ֵ��ȡֵ���£�
 *     @arg IWDG_Prescaler_4: IWDG prescaler set to 4
 *     @arg IWDG_Prescaler_8: IWDG prescaler set to 8
 *     @arg IWDG_Prescaler_16: IWDG prescaler set to 16
 *     @arg IWDG_Prescaler_32: IWDG prescaler set to 32
 *     @arg IWDG_Prescaler_64: IWDG prescaler set to 64
 *     @arg IWDG_Prescaler_128: IWDG prescaler set to 128
 *     @arg IWDG_Prescaler_256: IWDG prescaler set to 256
 *
 * rlv:Ԥ��Ƶ��ֵ��ȡֵ��ΧΪ��0-0XFFF
 * �������þ�����
 * IWDG_Config(IWDG_Prescaler_64 ,625);  // IWDG 1s ��ʱ���
 */

void watch_dog_init(void)
{
    /* 1Sι�� */
    uint8_t prv = IWDG_Prescaler_64;
    uint16_t rlv = 625;

    // ʹ�� Ԥ��Ƶ�Ĵ���PR����װ�ؼĴ���RLR��д
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    // ����Ԥ��Ƶ��ֵ
    IWDG_SetPrescaler(prv);

    // ������װ�ؼĴ���ֵ
    IWDG_SetReload(rlv);

    // ����װ�ؼĴ�����ֵ�ŵ���������
    IWDG_ReloadCounter();

    // ʹ�� IWDG
    IWDG_Enable();
}


/**
  * @brief  ι��
  * @param  null
  * @retval null
  */
void watch_dog_feed(void)
{
    // ����װ�ؼĴ�����ֵ�ŵ��������У�ι������ֹIWDG��λ
    // ����������ֵ����0��ʱ������ϵͳ��λ
    IWDG_ReloadCounter();
}


/*********************************************END OF FILE**********************/
