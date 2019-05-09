/**
  *****************************************************************************
  * @file:      app_fatfs.c
  * @author:    Cat�����ƽ��
  * @version:   V1.0
  * @date:      2018-4-16
  * @brief:     fatfs�ļ�ϵͳ
  * @attention: 
  *****************************************************************************
  */


#include "./app_fatfs.h"

#include "./../sys/sys_scheduler.h"
#include "./../sys/sys_command_line.h"
#include "./../sys/sys_queue.h"

#include "./../bsp/bsp_spi.h"

#include "./../component/fatfs/ff.h"

#include "./app_nxp.h"



#define APP_FATFS_DEBUG

#define CONFIG_FILE_PATH    "1:config.txt"  /* �����ļ�·�� */
#define ROOT_PATH           "1:"            /* ��Ŀ¼ */


FATFS fs;                                   /* FatFs�ļ�ϵͳ���� */
FIL fnew;                                   /* �ļ����� */

char ReadBuffer[256] = {0};                 /* �������� */


__packed typedef struct {
#define CHIP_NAME_MAX   32
#define FILE_NAME_MAX   64
#define CONFIG_YES      'Y'
#define CONFIG_NO       'N'

    uint8_t enable;         /* �Ƿ����� */
    char chip_name[CHIP_NAME_MAX];
    char file_name[FILE_NAME_MAX];
    uint32_t file_size;
    uint32_t file_check_sum;
    /* max_program: 0��ʾ���޴���¼��1��ʾ��¼�������꣬����1��ʾ��¼ʣ����� */
    uint32_t max_program;
    uint8_t verify_after_program :1;
    uint8_t read_out_protection :1;
    uint8_t mcu_run_after_program :1;
    uint8_t auto_program_control :1;
    uint8_t auto_program_time;

    /* ���ڱ�ǣ��Ƿ������ù� */
    uint8_t chip_name_set_flag :1;
    uint8_t file_name_set_flag :1;
    uint8_t max_program_set_flag :1;
    uint8_t verify_after_program_set_flag :1;
    uint8_t read_out_protection_set_flag :1;
    uint8_t mcu_run_after_program_set_flag :1;
    uint8_t auto_program_control_set_flag :1;
    uint8_t auto_program_time_set_flag :1;

    uint8_t check_sum;      /* Ч��� */
}CONFIG_INFO_S;

CONFIG_INFO_S config_info;


static void fatfs_mount(void);
static void fatfs_unmount(void);
static void chip_name_handle(char* str, CONFIG_INFO_S *p_cfg);
static void file_name_handle(char* str, CONFIG_INFO_S *p_cfg);
static void max_program_handle(char* str, CONFIG_INFO_S *p_cfg);
static void verify_after_program_handle(char* str, CONFIG_INFO_S *p_cfg);
static void read_out_protection_handle(char* str, CONFIG_INFO_S *p_cfg);
static void mcu_run_after_program_handle(char* str, CONFIG_INFO_S *p_cfg);
static void auto_program_control_handle(char* str, CONFIG_INFO_S *p_cfg);
static void auto_program_time_handle(char* str, CONFIG_INFO_S *p_cfg);
static void print_config_file_from_database(void);
static void print_firmware_backup(void);



uint8_t fatfs_dbg_en = FALSE;
#ifdef FATFS_CLI_DEBUG

/**
  * @brief  FATFS��Ϣ�鿴
  * @param  None
  * @retval None
  */
static void cli_fatfs_config_info(void)
{
    /* �����ݿ��ȡ�����config�����ļ�������ӡ���� */
    print_config_file_from_database();
}


/**
  * @brief  FATFS��Ϣ�鿴
  * @param  None
  * @retval None
  */
static void cli_fatfs_firmware_info(void)
{
    if (E_NXP_WAITING == nxp_state_get()) {/* ����fm_buff���湲������������� */
        print_firmware_backup();
    }
}


/**
  * @brief  FATFS����������
  * @param  None
  * @retval None
  */
uint8_t cli_fatfs(void *para, uint8_t len)
{
    uint8_t *pTemp = (uint8_t *)para;

    if ((0 < len) && (NULL != pTemp)) {
        pTemp++; /* ����һ���ո� */

        if (0 == strncmp((const char *)pTemp, "config info", strlen("config info"))) {
            /* fatfs config info. */
            cli_fatfs_config_info();
        } else if (0 == strncmp((const char *)pTemp, "fm info", strlen("fm info"))) {
            /* fatfs firmware info. */
            cli_fatfs_firmware_info();
        } else if (0 == strncmp((const char *)pTemp, "dbg 1", strlen("dbg 1"))) {
            /* fatfs debug on */
            fatfs_dbg_en = TRUE;
            NL1();
            DBG("fatfs debug on\r\n");
        } else if (0 == strncmp((const char *)pTemp, "dbg 0", strlen("dbg 0"))) {
            /* fatfs debug off */
            fatfs_dbg_en = FALSE;
            NL1();
            DBG("fatfs debug off\r\n");
        } else {
            /* �����д� */
            return FALSE;
        }
    }

    /* ��ȷ���� */
    return TRUE;
}

#endif


#if 0
/**
  * @brief  fatfs�ļ�ϵͳ��д����
  * @param  ��
  * @retval ��
  */
static void fatfs_rd_wr_test(void)
{
#define WRITE_BUFF  "hello world"

    FRESULT res_flash;  /* �ļ�������� */
    UINT fnum;          /* �ļ��ɹ���д���� */

    /*----------------------- �ļ�ϵͳ���ԣ�д���� -------------------*/
    /* ���ļ���ÿ�ζ����½�����ʽ�򿪣�����Ϊ��д */
    PRINTF("file write test \r\n");
    res_flash = f_open(&fnew, "1:test.txt", FA_CREATE_ALWAYS | FA_WRITE);

    if (res_flash == FR_OK) {
        PRINTF("open file ok \r\n");
        /* ��ָ���洢������д�뵽�ļ��� */
        res_flash = f_write(&fnew, WRITE_BUFF, strlen(WRITE_BUFF), &fnum);

        if (res_flash == FR_OK) {
            printf("file write bytes = %d \r\n", fnum);
            printf("file write content: %s \r\n", WRITE_BUFF);
        } else {
            printf("file write fail��%d \r\n", res_flash);
        }
    } else {
        ERR("open/creat file fail \r\n");
    }
    /* ���ٶ�д���ر��ļ� */
    f_close(&fnew);
    NL2();

    /*------------------- �ļ�ϵͳ���ԣ������� --------------------------*/
    PRINTF("file read test \r\n");
    res_flash = f_open(&fnew, "1:test.txt", FA_OPEN_EXISTING | FA_READ);

    if (res_flash == FR_OK) {
        PRINTF("file open ok \r\n");
        res_flash = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);

        if (res_flash == FR_OK) {
            PRINTF("file read bytes = %d \r\n", fnum);
            PRINTF("file read content: %s \r\n", ReadBuffer);
        } else {
            ERRA("file read fail %d \r\n", res_flash);
        }
    } else {
        ERR("open file fail \r\n");
    }

    /* ���ٶ�д���ر��ļ� */
    f_close(&fnew);
    NL2();
}
#endif


/**
  * @brief  ��ȡconfig_info.file_size
  * @param  ��
  * @retval ��
  */
uint32_t get_config_info_file_size(void)
{
    return config_info.file_size;
}


/**
  * @brief  ��ȡconfig_info.enable
  * @param  ��
  * @retval ��
  */
uint8_t get_config_info_state(void)
{
    return config_info.enable;
}


/**
  * @brief  ��ȡconfig_info.mcu_run_after_program
  * @param  ��
  * @retval ��
  */
uint8_t get_config_info_mcu_run_after_program(void)
{
    return config_info.mcu_run_after_program;
}


/**
  * @brief  ��ȡget_config_info_auto_program_control
  * @param  ��
  * @retval ��
  */
uint8_t get_config_info_auto_program_control(void)
{
    return config_info.auto_program_control;
}


/**
  * @brief  ��ȡget_config_info_auto_program_time
  * @param  ��
  * @retval ��
  */
uint8_t get_config_info_auto_program_time(void)
{
    return config_info.auto_program_time;
}


/**
  * @brief  ��ȡconfig_info.mcu_run_after_program
  * @param  ��
  * @retval ��
  */
uint8_t config_info_max_program_check()
{
    uint8_t rtn = FALSE;

    /* max_program: 0��ʾ���޴���¼��1��ʾ��¼�������꣬����1��ʾ��¼ʣ����� */
    if (1 != config_info.max_program) {
        rtn = TRUE;
    }

    return rtn;
}


/**
  * @brief  �鿴�Ƿ�������¼������
  * @param  ��
  * @retval TRUE�������������Խ�����¼
  */
uint8_t check_program_condition(void)
{
    uint8_t temp1 = FALSE;
    uint8_t temp2 = FALSE;

    temp1 = get_config_info_state();
    temp2 = config_info_max_program_check();

    if ((TRUE == temp1) && (TRUE == temp2)) {
        /* �������������Խ�����¼ */
        return TRUE;
    } else {
        return FALSE;
    }
}


/**
  * @brief  ��ȡĳ���ļ�����Ϣ
  * @param  ��
  * @retval ��
  */
static FRESULT fatfs_file_check(const char *fpath, FILINFO *fno)
{
    FRESULT res_flash;  /* �ļ�������� */

    /* ��ȡ�ļ���Ϣ */
    res_flash = f_stat(fpath, fno);

    if (res_flash == FR_OK) {
        DBG("--------------------begin");
        PRINTF("file path: %s \r\n", fpath);
        PRINTF("file size: %ld bytes \r\n", fno->fsize);
        PRINTF("time: %u/%02u/%02u, %02u:%02u \r\n",
               (fno->fdate >> 9) + 1980, fno->fdate >> 5 & 15, fno->fdate & 31, fno->ftime >> 11, fno->ftime >> 5 & 63);
        PRINTF("state: %c%c%c%c%c \r\n",
               (fno->fattrib & AM_DIR) ? 'D' : '-',      // ��һ��Ŀ¼
               (fno->fattrib & AM_RDO) ? 'R' : '-',      // ֻ���ļ�
               (fno->fattrib & AM_HID) ? 'H' : '-',      // �����ļ�
               (fno->fattrib & AM_SYS) ? 'S' : '-',      // ϵͳ�ļ�
               (fno->fattrib & AM_ARC) ? 'A' : '-');     // �����ļ�
        DBG("--------------------end \r\n");
    } else {
        ERR("fatfs_file_check fail \r\n");
    }

    return res_flash;
}


/**
  * @brief  �ݹ�ɨ��FatFs�ڵ��ļ�
  * @param  path:��ʼɨ��·��
  * @retval result:�ļ�ϵͳ�ķ���ֵ
  */
static FRESULT scan_files(char* path)
{
    FRESULT res;      //�����ڵݹ���̱��޸ĵı���������ȫ�ֱ���
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;        // �ļ���

#if _USE_LFN
    /* ���ļ���֧�� */
    /* ����������Ҫ2���ֽڱ���һ�����֡�*/
    static char lfn[_MAX_LFN * 2 + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
#endif
    //��Ŀ¼
    res = f_opendir(&dir, path);

    if (res == FR_OK) {
        i = strlen(path);

        for (;;) {
            //��ȡĿ¼�µ����ݣ��ٶ����Զ�����һ���ļ�
            res = f_readdir(&dir, &fno);

            //Ϊ��ʱ��ʾ������Ŀ��ȡ��ϣ�����
            if (res != FR_OK || fno.fname[0] == 0) break;

#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif

            //���ʾ��ǰĿ¼������
            if (*fn == '.') continue;

            //Ŀ¼���ݹ��ȡ
            if (fno.fattrib & AM_DIR) {
                //�ϳ�����Ŀ¼��
                sprintf(&path[i], "/%s", fn);
                //�ݹ����
                res = scan_files(path);
                path[i] = 0;

                //��ʧ�ܣ�����ѭ��
                if (res != FR_OK)
                    break;
            } else {
                printf("%s/%s\r\n", path, fn);  //����ļ���
                /* ������������ȡ�ض���ʽ���ļ�·�� */
            }//else
        } //for
    }

    return res;
}


/**
  * @brief  �ݹ�ɨ��FatFs�ڵ��ļ�
  * @param  path:��ʼɨ��·��
  * @retval result:�ļ�ϵͳ�ķ���ֵ
  */
static FRESULT fatfs_scan_files(char* path)
{
    FRESULT res;      //�����ڵݹ���̱��޸ĵı���������ȫ�ֱ���

    DBG("--------------------begin");
    strcpy((char*)ReadBuffer, path);
    res = scan_files((char*)ReadBuffer);
    DBG("--------------------end \r\n");

    return res;
}


__packed typedef struct {
#define KW_CONFIG_INFO              "CONFIG_INFO"
#define KW_CHIP_NAME                "CHIP_NAME"
#define KW_FILE_NAME                "FILE_NAME"
#define KW_FILE_SIZE                "FILE_SIZE"
#define KW_MAX_PROGRAM              "MAX_PROGRAM"
#define KW_VERIFY_AFTER_PROGRAM     "VERIFY_AFTER_PROGRAM"
#define KW_READ_OUT_PROTECTION      "READ_OUT_PROTECTION"
#define KW_MCU_RUN_AFTER_PROGRAM    "MCU_RUN_AFTER_PROGRAM"
#define KW_AUTO_PROGRAM_CONTROL     "AUTO_PROGRAM_CONTROL"
#define KW_AUTO_PROGRAM_TIME        "AUTO_PROGRAM_TIME"

    char *key_word;
    void (*func)(char* str, CONFIG_INFO_S *p_cfg);
    //char *err_code;
}KEY_WORD_MAP_S;

const KEY_WORD_MAP_S key_word_map[] = {
    /* оƬ�ͺţ��ַ�����ʽ */
    {KW_CHIP_NAME,                  chip_name_handle},
    /* �����̼����ļ������ַ�����ʽ */
    {KW_FILE_NAME,                  file_name_handle},
    /* �����¼������0Ϊ���ޣ�(1->FFFF FFFF)�� */
    {KW_MAX_PROGRAM,                max_program_handle},
    /* ��¼����Ƿ�Ч�飬"Y"Ч�飬"N"��Ч�� */
    {KW_VERIFY_AFTER_PROGRAM,       verify_after_program_handle},
    /* ����������ֹ�Ƿ���ȡ�̼�,��"Y"������"N"�ر� */
    {KW_READ_OUT_PROTECTION,        read_out_protection_handle},
    /* �����ɺ���MCU���У�"Y"������"N"�ر� */
    {KW_MCU_RUN_AFTER_PROGRAM,      mcu_run_after_program_handle},
    /* �Զ���̿��ƣ�"Y"�Զ���"N"�ֶ� */
    {KW_AUTO_PROGRAM_CONTROL,       auto_program_control_handle},
    /* �ȴ�������ʱ����ȥ�����һ��MCU��(1->FF)�� */
    {KW_AUTO_PROGRAM_TIME,          auto_program_time_handle},
};


/**
  * @brief  ����Ŀ��MCU��������
  * @param  str�����ַ�������
  * @retval null
  */
static void chip_name_handle(char* str, CONFIG_INFO_S *p_cfg)
{
    char *addr_1;
    char *addr_2;
    uint8_t len;

    PRINTF("chip_name_handle: %s \r\n", str);

    addr_1 = strstr((const char *)str, "\"");

    if (NULL != addr_1) {
        addr_1 += 1;  /* ָ�����õĵ�һ���ַ� */
        addr_2 = strstr((const char *)addr_1, "\"");

        if (NULL != addr_2) {
            len = addr_2 - addr_1;
            if (len < CHIP_NAME_MAX) {
                memset(p_cfg->chip_name, 0x00, CHIP_NAME_MAX);
                memcpy(p_cfg->chip_name, addr_1, len);
                p_cfg->chip_name_set_flag = TRUE;
            } else {
                /* ����̫�� */
                p_cfg->chip_name_set_flag = FALSE;
                ERR("chip_name len overflow \r\n");
            }
        } else {
            /* ���� */
            p_cfg->chip_name_set_flag = FALSE;
        }
    } else {
        /* ���� */
        p_cfg->chip_name_set_flag = FALSE;
    }
}


/**
  * @brief  ������¼�ļ���������
  * @param  str�����ַ�������
  * @retval null
  */
static void file_name_handle(char* str, CONFIG_INFO_S *p_cfg)
{
    char *addr_1;
    char *addr_2;
    uint8_t len;

    PRINTF("file_name_handle: %s \r\n", str);

    addr_1 = strstr((const char *)str, "\"");

    if (NULL != addr_1) {
        addr_1 += 1;  /* ָ�����õĵ�һ���ַ� */
        addr_2 = strstr((const char *)addr_1, "\"");

        if (NULL != addr_2) {
            len = addr_2 - addr_1;
            if (len < FILE_NAME_MAX) {
                memset(p_cfg->file_name, 0x00, FILE_NAME_MAX);
                memcpy(p_cfg->file_name, addr_1, len);
                p_cfg->file_name_set_flag = TRUE;
            } else {
                /* ����̫�� */
                p_cfg->file_name_set_flag = FALSE;
                ERR("file_name len overflow \r\n");
            }
        } else {
            /* ���� */
            p_cfg->file_name_set_flag = FALSE;
        }
    } else {
        /* ���� */
        p_cfg->file_name_set_flag = FALSE;
    }
}


/**
  * @brief  ������¼��������
  * @param  str�����ַ�������
  * @retval null
  */
static void max_program_handle(char* str, CONFIG_INFO_S *p_cfg)
{
    char *addr_1;
    char *addr_2;

    PRINTF("max_program_handle: %s \r\n", str);

    addr_1 = strstr((const char *)str, "\"");

    if (NULL != addr_1) {
        addr_1 += 1;  /* ָ�����õĵ�һ���ַ� */
        addr_2 = strstr((const char *)addr_1, "\"");

        if (NULL != addr_2) {
            addr_2[0] = 0x00;       /* �ַ����������� */

            extern int my_atoi(char *str);
            p_cfg->max_program = my_atoi(addr_1);   /* �ַ���ת���� */
            if ((0 != p_cfg->max_program) && (0xffffffff != p_cfg->max_program)) {
                /* ����Ϊʲô+1����max_program����ʱ���ע�ͣ�max_program����Ϊ0xffffffff������+1����� */
                p_cfg->max_program += 1;
            }
            p_cfg->max_program_set_flag = TRUE;
            //PRINTF("p_cfg->max_program = %d \r\n", p_cfg->max_program);
        } else {
            /* ���� */
            p_cfg->max_program_set_flag = FALSE;
        }
    } else {
        /* ���� */
        p_cfg->max_program_set_flag = FALSE;
    }
}


/**
  * @brief  ������¼��ɺ��Ƿ�Ч������
  * @param  str�����ַ�������
  * @retval null
  */
static void verify_after_program_handle(char* str, CONFIG_INFO_S *p_cfg)
{
    char *addr_1;

    PRINTF("verify_after_program_handle: %s \r\n", str);

    addr_1 = strstr((const char *)str, "\"");

    if (NULL != addr_1) {
        addr_1 += 1;  /* ָ�����õĵ�һ���ַ� */
        if (CONFIG_YES == addr_1[0]) {
            p_cfg->verify_after_program = TRUE;
            p_cfg->verify_after_program_set_flag = TRUE;
        } else if (NULL != addr_1[0]) {
            p_cfg->verify_after_program = FALSE;
            p_cfg->verify_after_program_set_flag = TRUE;
        } else {
            /* �������� */
            p_cfg->verify_after_program_set_flag = FALSE;
        }

    } else {
        /* ���� */
        p_cfg->verify_after_program_set_flag = FALSE;
    }
}


/**
  * @brief  ������������������
  * @param  str�����ַ�������
  * @retval null
  */
static void read_out_protection_handle(char* str, CONFIG_INFO_S *p_cfg)
{
    char *addr_1;

    PRINTF("read_out_protection_handle: %s \r\n", str);

    addr_1 = strstr((const char *)str, "\"");

    if (NULL != addr_1) {
        addr_1 += 1;  /* ָ�����õĵ�һ���ַ� */
        if (CONFIG_YES == addr_1[0]) {
            p_cfg->read_out_protection = TRUE;
            p_cfg->read_out_protection_set_flag = TRUE;
        } else if (NULL != addr_1[0]) {
            p_cfg->read_out_protection = FALSE;
            p_cfg->read_out_protection_set_flag = TRUE;
        } else {
            /* �������� */
            p_cfg->read_out_protection_set_flag = FALSE;
        }

    } else {
        /* ���� */
        p_cfg->read_out_protection_set_flag = FALSE;
    }
}


/**
  * @brief  ������¼��ɺ�MCU�Ƿ���������
  * @param  str�����ַ�������
  * @retval null
  */
static void mcu_run_after_program_handle(char* str, CONFIG_INFO_S *p_cfg)
{
    char *addr_1;

    PRINTF("mcu_run_after_program_handle: %s \r\n", str);

    addr_1 = strstr((const char *)str, "\"");

    if (NULL != addr_1) {
        addr_1 += 1;  /* ָ�����õĵ�һ���ַ� */
        if (CONFIG_YES == addr_1[0]) {
            p_cfg->mcu_run_after_program = TRUE;
            p_cfg->mcu_run_after_program_set_flag = TRUE;
        } else if (NULL != addr_1[0]) {
            p_cfg->mcu_run_after_program = FALSE;
            p_cfg->mcu_run_after_program_set_flag = TRUE;
        } else {
            /* �������� */
            p_cfg->mcu_run_after_program_set_flag = FALSE;
        }

    } else {
        /* ���� */
        p_cfg->mcu_run_after_program_set_flag = FALSE;
    }
}


/**
  * @brief  �����Զ���̿�������
  * @param  str�����ַ�������
  * @retval null
  */
static void auto_program_control_handle(char* str, CONFIG_INFO_S *p_cfg)
{
    char *addr_1;

    PRINTF("auto_program_control_handle: %s \r\n", str);

    addr_1 = strstr((const char *)str, "\"");

    if (NULL != addr_1) {
        addr_1 += 1;  /* ָ�����õĵ�һ���ַ� */
        if (CONFIG_YES == addr_1[0]) {
            p_cfg->auto_program_control = TRUE;
            p_cfg->auto_program_control_set_flag = TRUE;
        } else if (NULL != addr_1[0]) {
            p_cfg->auto_program_control = FALSE;
            p_cfg->auto_program_control_set_flag = TRUE;
        } else {
            /* �������� */
            p_cfg->auto_program_control_set_flag = FALSE;
        }

    } else {
        /* ���� */
        p_cfg->auto_program_control_set_flag = FALSE;
    }
}


/**
  * @brief  �����Զ������ʱʱ������
  * @param  str�����ַ�������
  * @retval null
  */
static void auto_program_time_handle(char* str, CONFIG_INFO_S *p_cfg)
{
    char *addr_1;
    char *addr_2;

    PRINTF("auto_program_time_handle: %s \r\n", str);

    addr_1 = strstr((const char *)str, "\"");

    if (NULL != addr_1) {
        addr_1 += 1;  /* ָ�����õĵ�һ���ַ� */
        addr_2 = strstr((const char *)addr_1, "\"");

        if (NULL != addr_2) {
            addr_2[0] = 0x00;       /* �ַ����������� */

            extern int my_atoi(char *str);
            p_cfg->auto_program_time = my_atoi(addr_1);  /* �ַ���ת���� */
            p_cfg->auto_program_time_set_flag = TRUE;
            //PRINTF("p_cfg->auto_program_time = %d \r\n", p_cfg->auto_program_time);
        } else {
            /* ���� */
            p_cfg->auto_program_time_set_flag = FALSE;
        }
    } else {
        /* ���� */
        p_cfg->auto_program_time_set_flag = FALSE;
    }
}


/**
  * @brief  ����config�����ļ�
  * @param  path:config�����ļ�·��
  * @retval null
  */
static void match_config_key_word(char *line, CONFIG_INFO_S *p_cfg)
{
    uint8_t i = 0;
    uint8_t num = sizeof(key_word_map) / sizeof(key_word_map[0]);

    for (i = 0; i < num; i++) {
        if (NULL != strstr(line, key_word_map[i].key_word)) {
            //PRINTF("match_config_key_word = %d \r\n", i);
            key_word_map[i].func(line, p_cfg);
            break;
        }
    }
}


/**
  * @brief  �ж�һ���ַ��Ƿ���ע�ͣ������ע�;Ͳ���Ҫ����
  * @param  lineһ���ַ�
  * @retval ����0��ʾ��һ����ע�ͣ�����Ҫ����
  */
static char *check_comment_line(char *line)
{
    /* �жϵ�һ���ַ��ǲ���ע�ͷ��ţ�"#" */
    if ('#' == line[0]) {
        //PRINTF("comment line here: %s \r\n", line);
        /* �����ע�Ϳ�ͷ����ô�ͷ���һ��0����ʾ���в���Ҫ������ */
        line = 0;
    }

    return line;
}


/**
  * @brief  ����config_info��Ч���
  * @param  null
  * @retval null
  */
static uint8_t calc_config_info_check_sum(CONFIG_INFO_S *p_cfg)
{
    uint16_t i;
    uint8_t *addr = (uint8_t *)p_cfg;
    uint8_t check_sum = 0;

    for (i = 0; i < (sizeof(CONFIG_INFO_S) - 1); i++) {
        check_sum += addr[i];
    }

    return check_sum;
}


/**
  * @brief  ���ԣ���ӡ�鿴config_info
  * @param  null
  * @retval null
  */
static void config_info_print(CONFIG_INFO_S *p_cfg)
{
    NL1();
    DBG("--------------------begin");
    PRINTF("config_info.enable = %d \r\n", p_cfg->enable);
    PRINTF("config_info.chip_name = %s \r\n", p_cfg->chip_name);
    PRINTF("config_info.file_name = %s \r\n", p_cfg->file_name);
    PRINTF("config_info.file_size = %d \r\n", p_cfg->file_size);
    PRINTF("config_info.file_check_sum = %d \r\n", p_cfg->file_check_sum);
    if (0 == p_cfg->max_program) {
        PRINTF("config_info.max_program = infinity \r\n");
    } else {
        PRINTF("config_info.max_program = %d \r\n", p_cfg->max_program - 1);
    }
    PRINTF("config_info.verify_after_program = %d \r\n", p_cfg->verify_after_program);
    PRINTF("config_info.read_out_protection = %d \r\n", p_cfg->read_out_protection);
    PRINTF("config_info.mcu_run_after_program = %d \r\n", p_cfg->mcu_run_after_program);
    PRINTF("config_info.auto_program_control = %d \r\n", p_cfg->auto_program_control);
    PRINTF("config_info.auto_program_time = %d \r\n", p_cfg->auto_program_time);
    PRINTF("config_info.check_sum = %d \r\n", p_cfg->check_sum);
    DBG("--------------------end");
    NL1();
}


/**
  * @brief  �����ݿ��ȡ�����config�����ļ�
  * @param  ��
  * @retval ��
  */
static uint8_t config_info_update_from_database(void)
{
    uint8_t err = FALSE;
    uint8_t check_sum = 0;

    extern void database_read(char* file_name, uint8_t *file_buff, uint32_t file_len);
    database_read(KW_CONFIG_INFO, (uint8_t *)&config_info, sizeof(CONFIG_INFO_S));

    check_sum = calc_config_info_check_sum(&config_info);
    if (check_sum != config_info.check_sum) {
        ERR("config_info.check_sum ERR \r\n");
        err = TRUE;
    }

    return err;
}


/**
  * @brief  �����ݿ��ȡ�����config�����ļ�������ӡ����
  * @param  ��
  * @retval ��
  */
static void print_config_file_from_database(void)
{
    config_info_update_from_database();

    config_info_print(&config_info);
}


/**
  * @brief  ��config_info�浽���ݿ⣬��fatfs�е������̼����Ƶ��̼�������
  * @param  null
  * @retval null
  */
static void config_info_save_to_database(CONFIG_INFO_S *p_cfg)
{
    extern void database_save(char *file_name, uint8_t *file_buff, uint32_t file_len);
    database_save(KW_CONFIG_INFO, (uint8_t *)p_cfg, sizeof(CONFIG_INFO_S));

    config_info_update_from_database();
}


/**
  * @brief  ÿ����¼�ɹ��󣬴�����¼������¼
  * @param  null
  * @retval null
  */
void config_info_max_program_handle_after_program(void)
{
    if (0 != config_info.max_program) {
        if (config_info.max_program > 1) {
            config_info.max_program--;
            config_info.check_sum = calc_config_info_check_sum(&config_info);
        }
        config_info_save_to_database(&config_info);
    }
}


#define SPRINTF_BUFF_LEN    1024
uint16_t sprintf_index = 0;
/* ���ڵ��ԣ�����ʱ��ӡ������Ϣ����ȷ�򲻴�ӡ */
char sprintf_buff[SPRINTF_BUFF_LEN];
char *config_err_code[] = {
    "chip name not set or the length of name > 32  \r\n",
    "chip name not support \r\n",
    "firmware file not found or the length of name > 64 \r\n",
    "firmware file oversize \r\n",
    "firmware file name not set \r\n",
    "config.txt file not found \r\n",
};


/**
  * @brief  sprintf_buff��ʼ��
  * @param  null
  * @retval null
  */
static void sprintf_buff_init(void)
{
    sprintf_index = 0;
    memset(sprintf_buff, 0x00, SPRINTF_BUFF_LEN);
}


/**
  * @brief  ���Ӵ�����Ϣ��sprintf_buff
  * @param  null
  * @retval null
  */
static void sprintf_buff_add(char *str)
{
    if (NULL == str) return;

    if ((SPRINTF_BUFF_LEN - sprintf_index) > strlen(str)) {
        sprintf_index += SPRINTF(sprintf_buff + sprintf_index, str);
    } else {
        ERR("sprintf_buff not enough \r\n");
    }
}


/**
  * @brief  ��ʾsprintf_buff����Ĵ�����Ϣ
  * @param  null
  * @retval null
  */
static void sprintf_buff_show(void)
{
    ERR("--------------------begin");
    /* sprintf_buff�Ƿ��������ʱû����ж� */
    if (sprintf_index) {
        PRINTF("%s", sprintf_buff);
    }
    sprintf_index = 0;
    ERR("--------------------end \r\n");
}


/**
  * @brief  �ж������ļ����������Ƿ�Ϸ�
  * @param  null
  * @retval null
  */
static uint8_t config_info_check(CONFIG_INFO_S *p_cfg)
{
    uint8_t len;
    uint8_t err;
    FILINFO fno;
    char *temp_dir = ReadBuffer;

    memset(temp_dir, 0x00, sizeof(ReadBuffer));

    /*
        ����оƬ�ͺ�����
    */
    err = TRUE;
    if (TRUE == p_cfg->chip_name_set_flag) {
        /* �ж�оƬ�ͺ��Ƿ�Ϸ� */
        for (len = 0; len < NXP_CHIP_MAX; len++) {
            if (0 == memcmp(p_cfg->chip_name, nxp_chip[len].name, strlen(nxp_chip[len].name))) {
                /* оƬ�ͺźϷ� */
                err = FALSE;
                /* ��¼��оƬFLASH��С */
                p_cfg->file_size = nxp_chip[len].flash_size;
                break;
            }
        }
        if (TRUE == err) {
            /* оƬ�ͺŲ��Ϸ� */
            sprintf_buff_add(config_err_code[1]);
            p_cfg->chip_name_set_flag = FALSE;
        }
    } else {
        /* оƬ�ͺű������ã���ʾ���� */
        sprintf_buff_add(config_err_code[0]);
    }

    /*
        ���������̼��ļ�������
    */
    if (TRUE == p_cfg->file_name_set_flag) {
        /* �ж������̼��Ƿ���� */
        len = (uint8_t)strlen(p_cfg->file_name);
        if (len > FILE_NAME_MAX) {
            len = FILE_NAME_MAX;
        }
        memcpy(temp_dir, ROOT_PATH, 2);
        memcpy(&temp_dir[2], p_cfg->file_name, len);
        if (FR_OK == fatfs_file_check((const char *)temp_dir, &fno)) {
            /* �����̼����ڣ�����Ҫ�жϹ̼���С�Ƿ񳬳�оƬFLASH��Χ */
            if (p_cfg->file_size < fno.fsize) {
                /* �����̼�̫�󣬳���оƬ�ڲ�FLASH��С */
                if (TRUE == p_cfg->chip_name_set_flag) {
                    /* ���оƬ�ͺŴ�����ôp_cfg->file_size�ͻ���0 */
                    sprintf_buff_add(config_err_code[3]);
                }
                p_cfg->file_name_set_flag = FALSE;
            } else {
                p_cfg->file_size = fno.fsize;
            }
        } else {
            /* �����̼������� */
            sprintf_buff_add(config_err_code[2]);
            p_cfg->file_name_set_flag = FALSE;
        }
    } else {
        /* �����̼��������ã���ʾ���� */
        sprintf_buff_add(config_err_code[4]);
    }

    /*
        ��������̴�������
    */
    if (TRUE == p_cfg->max_program_set_flag) {
        /* ����̴��� */
        
    } else {
        /* ����̴���û�����õĻ���Ĭ�����ó�0��0��ʾ���ߴ�����¼ */
        p_cfg->max_program = 0;
        p_cfg->max_program_set_flag = TRUE;
    }

    /*
        ������¼���Ч������
    */
    if (TRUE == p_cfg->verify_after_program_set_flag) {
        /* ��¼���Ч�� */
        
    } else {
        /* ��¼���Ч��û�����õĻ���Ĭ�����ó�FALSE */
        p_cfg->verify_after_program = FALSE;
        p_cfg->verify_after_program_set_flag = TRUE;
    }

    /*
        �������������
    */
    if (TRUE == p_cfg->read_out_protection_set_flag) {
        /* ������ */
        
    } else {
        /* ������û�����õĻ���Ĭ�����ó�FALSE */
        p_cfg->read_out_protection = FALSE;
        p_cfg->read_out_protection_set_flag = TRUE;
    }

    /*
        ������¼���MCUֱ����������
    */
    if (TRUE == p_cfg->mcu_run_after_program_set_flag) {
        /* ��¼���MCUֱ������ */
        
    } else {
        /* ��¼���MCUֱ������û�����õĻ���Ĭ�����ó�TRUE */
        p_cfg->mcu_run_after_program = TRUE;
        p_cfg->mcu_run_after_program_set_flag = TRUE;
    }

    /*
        �����Զ��������
    */
    if (TRUE == p_cfg->auto_program_control_set_flag) {
        /* �Զ���� */
        
    } else {
        /* �Զ����û�����õĻ���Ĭ�����ó�FALSE */
        p_cfg->auto_program_control = FALSE;
        p_cfg->auto_program_control_set_flag = TRUE;
    }

    /*
        �����Զ����ʱ������
    */
    if (TRUE == p_cfg->auto_program_time_set_flag) {
        /* �Զ���� */
        
    } else {
        /* �Զ����ʱ��û�����õĻ���Ĭ�����ó�5s */
        p_cfg->auto_program_time = 0x05;
        p_cfg->auto_program_time_set_flag = TRUE;
    }


    if ((TRUE == p_cfg->chip_name_set_flag)
        && (TRUE == p_cfg->file_name_set_flag)
        && (TRUE == p_cfg->max_program_set_flag)
        && (TRUE == p_cfg->verify_after_program_set_flag)
        && (TRUE == p_cfg->read_out_protection_set_flag)
        && (TRUE == p_cfg->mcu_run_after_program_set_flag)
        && (TRUE == p_cfg->auto_program_control_set_flag)
        && (TRUE == p_cfg->auto_program_time_set_flag)) {

        p_cfg->enable = TRUE;
        p_cfg->check_sum = calc_config_info_check_sum(p_cfg);
        //config_info_print(p_cfg);   /* ���ԣ���ӡ�鿴config_info */
        err = FALSE;
    } else {
        err = TRUE;
    }

    return err;
}


/**
  * @brief  ���ϵ磬�̼��������Ĺ̼�����Ч��
  * @param  null
  * @retval null
  */
static uint8_t firmware_backup_check(void)
{
    uint8_t err = FALSE;
    uint16_t i = 0;
    uint32_t check_sum = 0;
    unsigned int rd_count = 0;
    unsigned int rd_once = 0;
    uint32_t fm_backup_addr = FM_BACKUP_SECTOR_START * FM_BACKUP_SECTOR_SIZE;

    extern uint8_t fm_buff[];
    char *read_buff = (char *)fm_buff;   /* ����stack��ѹ�� */
    uint16_t read_buff_max = 4096;

    if ((config_info.file_size - rd_count) < read_buff_max) {
        rd_once = config_info.file_size - rd_count;
    } else {
        rd_once = read_buff_max;
    }

    while (rd_count < config_info.file_size) {
        spi_flash_buffer_read((uint8_t *)read_buff, fm_backup_addr, rd_once);

        for (i = 0; i < rd_once; i++) {
            check_sum += read_buff[i];
        }

        fm_backup_addr += rd_once;
        rd_count += rd_once;
        if ((config_info.file_size - rd_count) < read_buff_max) {
            rd_once = config_info.file_size - rd_count;
        }
    }

    if (check_sum != config_info.file_check_sum) {
        ERR("config_info.file_check_sum ERR \r\n");
        err = TRUE;
    }

    return err;
}


/**
  * @brief  ��ӡ�̼��������Ĺ̼�
  * @param  null
  * @retval null
  */
static void print_firmware_backup(void)
{
    unsigned int rd_count = 0;
    unsigned int rd_once = 0;
    uint32_t fm_backup_addr = FM_BACKUP_SECTOR_START * FM_BACKUP_SECTOR_SIZE;

    extern uint8_t fm_buff[];
    char *read_buff = (char *)fm_buff;   /* ����stack��ѹ�� */
    uint16_t read_buff_max = 4096;

    if ((config_info.file_size - rd_count) < read_buff_max) {
        rd_once = config_info.file_size - rd_count;
    } else {
        rd_once = read_buff_max;
    }
    while (rd_count < config_info.file_size) {
        spi_flash_buffer_read((uint8_t *)read_buff, fm_backup_addr, rd_once);
        
        //if (TRUE == fatfs_dbg_en) {
            extern volatile uint16_t os_watch_dog_cnt;
            /* ������Դ�ӡ�Ứ�Ѻܶ�ʱ�䣬����10s�������Ź���λ */
            os_watch_dog_cnt = 0;

            PRINTF("read: 0x%08x \r\n", rd_count);
            uint16_t i = 0;
            for (i = 0; i < rd_once; i++) {
                PRINTF("%02x ", read_buff[i]);
                if ((i + 1) % 32 == 0) {
                    NL1();
                }
            }
            NL1();
        //}

        fm_backup_addr += rd_once;
        rd_count += rd_once;
        if ((config_info.file_size - rd_count) < read_buff_max) {
            rd_once = config_info.file_size - rd_count;
        }
    }
}


/**
  * @brief  ���ϵ磬�̼��������Ĺ̼�����Ч�飬�����ݿ��ȡcomfig_info
  * @param  null
  * @retval null
  */
void check_firmware_backup_and_config_info(void)
{
    uint8_t err1 = FALSE;
    uint8_t err2 = FALSE;

    err1 = config_info_update_from_database();
    err2 = firmware_backup_check();

    if ((err1 == TRUE) || (err2 == TRUE)) {
        memset(&config_info, 0x00, sizeof(CONFIG_INFO_S));
    }
}


/**
  * @brief  ��fatfs�ļ�ϵͳ��ȡ�����̼������Ƶ��̼�������
  * @param  null
  * @retval null
  */
static uint8_t copy_firmware_to_backup(CONFIG_INFO_S *p_cfg)
{
    uint8_t err = FALSE;
    FRESULT res_flash;  /* �ļ�������� */
    uint16_t i = 0;
    char *temp_dir = ReadBuffer;
    unsigned int rd_count = 0;
    unsigned int rd_once = 0;
    uint32_t fm_backup_addr = FM_BACKUP_SECTOR_START * FM_BACKUP_SECTOR_SIZE;

    extern uint8_t fm_buff[];
    char *read_buff = (char *)fm_buff;   /* ����stack��ѹ�� */
    uint16_t read_buff_max = 4096;

    memset(temp_dir, 0x00, sizeof(ReadBuffer));
    memcpy(temp_dir, ROOT_PATH, 2);
    memcpy(&temp_dir[2], p_cfg->file_name, strlen(p_cfg->file_name));

    p_cfg->file_check_sum = 0;

    res_flash = f_open(&fnew, temp_dir, FA_OPEN_EXISTING | FA_READ);

    if (res_flash == FR_OK) {
        while (rd_count < p_cfg->file_size) {
            /*  �ļ���λ��rd_countλ�� */
            res_flash = f_lseek(&fnew, rd_count);
            /* ��ȡ�ļ��������ݵ������� */
            res_flash = f_read(&fnew, read_buff, read_buff_max, &rd_once);

            /* ����̼���Ч��� */
            for (i = 0; i < rd_once; i++) {
                p_cfg->file_check_sum += read_buff[i];
            }

            if (TRUE == fatfs_dbg_en) {
                extern volatile uint16_t os_watch_dog_cnt;
                /* ������Դ�ӡ�Ứ�Ѻܶ�ʱ�䣬����10s�������Ź���λ */
                os_watch_dog_cnt = 0;

                PRINTF("read: 0x%08x \r\n", rd_count);
                for (i = 0; i < rd_once; i++) {
                    PRINTF("%02x ", read_buff[i]);
                    if ((i + 1) % 32 == 0) {
                        NL1();
                    }
                }
                NL1();
            }

            if (FR_OK == res_flash) {
                spi_flash_sector_erase(fm_backup_addr);
                spi_flash_buffer_write((uint8_t *)read_buff, fm_backup_addr, rd_once);
                fm_backup_addr += rd_once;
                rd_count += rd_once;
            }
        }
        /* ���������file_check_sum��������¼���config_info��Ч��� */
        p_cfg->check_sum = calc_config_info_check_sum(p_cfg);
        LOG("copy_firmware_to_backup ok \r\n");
    } else {
        sprintf_buff_add(config_err_code[2]);

        /* û�м�������̼�����ʾ���� */
        err = TRUE;
    }

    /* ���ٶ�д���ر��ļ� */
    f_close(&fnew);

    return err;
}


/**
  * @brief  ��ȡconfig�����ļ�������ȡ������Ϣ����
  * @param  path:config�����ļ�·��
  * @retval null
  */
void config_file_handle(void)
{
//#define CONFIG_FILE_HANDLE_DENUG

    uint8_t err = FALSE;
    CONFIG_INFO_S temp_config_info;
    FRESULT res_flash;  /* �ļ�������� */

#if 0   /* ���ԣ��鿴config�����ļ����ļ���Ϣ */
    //FILINFO fno;
    //fatfs_file_check(CONFIG_FILE_PATH, &fno);
#endif

    sprintf_buff_init();
    memset(&temp_config_info, 0x00, sizeof(temp_config_info));

    fatfs_mount();

    res_flash = f_open(&fnew, CONFIG_FILE_PATH, FA_OPEN_EXISTING | FA_READ);

    if (res_flash == FR_OK) {
        //PRINTF("config file open ok \r\n");
        /* f_gets��ȡ����'\n'��'\0'��ֹͣ��������ÿ�ζ�ȡһ���ַ�������һ������ */
        while (0 != f_gets((TCHAR *)ReadBuffer, sizeof(ReadBuffer), &fnew)) {
            #ifdef CONFIG_FILE_HANDLE_DENUG
            PRINTF("config file read line(%d): %s \r\n", strlen(ReadBuffer), ReadBuffer);
            #endif

            /* ȥ�����еĿո��ַ� */
            void my_str_remove_blank(char *str);
            my_str_remove_blank(ReadBuffer);

            /* ������һ������ */
            char *p_line;
            p_line = check_comment_line(ReadBuffer);
            if (0 != p_line) {
                match_config_key_word(p_line, &temp_config_info);
            }
        }
    } else {
        sprintf_buff_add(config_err_code[5]);

        /* û�м�⵽�����ļ�����ʾ���� */
        err = TRUE;
    }

    /* ���ٶ�д���ر��ļ� */
    f_close(&fnew);

    if (FALSE == err) {
        /* �����ļ����浽дƽ�����򣬲��������ļ����Ƶ������� */
        if (FALSE == config_info_check(&temp_config_info)) {
            /* ��fatfs�е������̼����Ƶ��̼������� */
            if (FALSE == copy_firmware_to_backup(&temp_config_info)) {
                /* ��config_info�浽���ݿ� */
                config_info_save_to_database(&temp_config_info);
            } else {
                err = TRUE;
            }
        } else {
            err = TRUE;
        }
    }

    if (TRUE == err) {
        /* ��������ʾ���� */
        extern void buzzer_notice_config_fail(void);
        buzzer_notice_config_fail();
        /* ��ʾ�������� */
        sprintf_buff_show();
    } else {
        extern void buzzer_notice_config_ok(void);
        buzzer_notice_config_ok();
    }

    /* ����ʹ���ļ�ϵͳ��ȡ�������ļ�ϵͳ */
    fatfs_unmount();
}


/**
  * @brief  �����ļ�ϵͳ�����û���ļ�ϵͳ�����ʽ�������¹����ļ�ϵͳ
  * @param  ��
  * @retval ��
  */
static void fatfs_mount(void)
{
    FRESULT res_flash;  /* �ļ�������� */

    /* ���ⲿSPI Flash�����ļ�ϵͳ���ļ�ϵͳ����ʱ���SPI�豸��ʼ������ʼ����������
        ��������:f_mount()->find_volume()->disk_initialize->spi_flash_Init() */
    res_flash = f_mount(&fs, "1:", 1);

    /* ���û���ļ�ϵͳ�͸�ʽ���������ļ�ϵͳ */
    if (res_flash == FR_NO_FILESYSTEM) {
        LOG("fatfs dont esist \r\n");
        /* ��ʽ�� */
        res_flash = f_mkfs("1:", 0, 0);

        if (res_flash == FR_OK) {
            PRINTF("fatfs format ok \r\n");
            /* ��ʽ������ȡ������ */
            res_flash = f_mount(NULL, "1:", 1);
            /* ���¹���   */
            res_flash = f_mount(&fs, "1:", 1);
        } else {
            /* ��ʽ��SPI_FLASH�������ļ�ϵͳʧ�� */
            ERR("fatfs format fail \r\n");
            while (1);
        }
    } else if (res_flash != FR_OK) {
        /* �ⲿFlash�����ļ�ϵͳʧ�ܣ�����ԭ��SPI Flash��ʼ�����ɹ� */
        ERRA("fatfs init fail = %d \r\n", res_flash);
        while (1);
    } else {
        /* ����FATFS�ļ�ϵͳ�ɹ� */
        //PRINTF("fatfs ok \r\n");
    }
}


/**
  * @brief  ȡ�������ļ�ϵͳ
  * @param  ��
  * @retval ��
  */
static void fatfs_unmount(void)
{
    /* ����ʹ���ļ�ϵͳ��ȡ�������ļ�ϵͳ */
    f_mount(NULL, "1:", 1);
}


/**
  * @brief  fatfs�ļ�ϵͳ��ʼ��
  * @param  ��
  * @retval ��
  */
void fatfs_init(void)
{
#ifdef APP_FATFS_DEBUG
    PRINTF("16Mbyte SPI_FLASH memory map: \r\n");
    PRINTF("+------------------------------+  --> 0x%08x start_addr \r\n", DATABASE_SECTOR_START * DATABASE_SECTOR_SIZE);
    PRINTF("| database              : %dMB  | \r\n", (DATABASE_SECTOR_COUNT * DATABASE_SECTOR_SIZE) / (1024 * 1024));
    PRINTF("+------------------------------+  --> 0x%08x \r\n", FM_BACKUP_SECTOR_START * FM_BACKUP_SECTOR_SIZE);
    PRINTF("| firmware_backup       : %dMB  | \r\n", (FM_BACKUP_SECTOR_COUNT * FM_BACKUP_SECTOR_SIZE) / (1024 * 1024));
    PRINTF("+------------------------------+  --> 0x%08x \r\n", FATFS_SECTOR_START * FATFS_SECTOR_SIZE);
    PRINTF("| fatfs file system     : %dMB | \r\n", (FATFS_SECTOR_COUNT * FATFS_SECTOR_SIZE) / (1024 * 1024));
    PRINTF("+------------------------------+  --> 0x%08x end_addr \r\n\r\n", (DATABASE_SECTOR_COUNT + FM_BACKUP_SECTOR_COUNT + FATFS_SECTOR_COUNT) * SPI_FLASH_SectorSize);
#endif

    fatfs_mount();

    /* ������ӡfatfs�������ļ���·�� */
    fatfs_scan_files(ROOT_PATH);

    fatfs_unmount();
}


/**
  * @brief  fatfs�ļ�ϵͳ
  * @param  null
  * @retval null
  */
void fatfs_task(void)
{
    /* ��ʱ���� */
}


/*********************************************END OF FILE*********************/
