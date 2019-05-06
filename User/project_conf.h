/**
  ******************************************************************************
  * @file:      LHD8006_app_conf.h
  * @author:    Cat  
  * @version:   V1.0  
  * @date:      2018-3-1
  * @brief:     APP��������������ƣ������й��������Ի��Ƿ�֧����������
                �����е�����ļ�������ͳһ���ù���
  * @attention: ����ǰһ��Ҫȷ�������Ƿ���ȷ
  ******************************************************************************
  */


#include <stdint.h>

  
/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __LHD8006_APP_CONF_H
#define __LHD8006_APP_CONF_H



/* ------------------------------------------------ APP�궨�壬�����ͱ��붨�� */

/* 0x08008000��APP��ʼ��ַ��0x08000000��0x08008000��bootloaderռ32KB */
#define APP_ADDR    ((uint32_t)0x08008000)

/* ���屾����ΪAPP������Ҫ�����ж�����ƫ�ƣ�Keil�������ҲҪ�޸�Flash��ʼ��ַΪFLASH_APP_ADDR */
//#define APP_PROJECT


/* cm_backtrace(��� ARM Cortex-M ϵ�� MCU �Ĵ�������Զ�׷�١���λ��
    ����ԭ���Զ������Ŀ�Դ�⣬���ӣ�
    https://github.com/armink/CmBacktrace)�����ʹ������ */
#define USING_CM_BACKTRACE


/* --------------------------------------------------------- �����е��Ժ궨�� */

#define NXP_CLI_DEBUG               /* NXP�����е��Ժ궨�忪�� */
#define FATFS_CLI_DEBUG             /* FATFS�����е��Ժ궨�忪�� */


/* ----------------------------------------------------------- ģ����Ժ궨�� */

//#define APP_W5500_DEBUG             /* app_w5500�ļ����� */
//#define APP_OTA_DEBUG               /* OTA���� */



#endif /* __LHD8006_APP_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
