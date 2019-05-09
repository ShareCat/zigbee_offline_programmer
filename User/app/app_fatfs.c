/**
  *****************************************************************************
  * @file:      app_fatfs.c
  * @author:    Cat（孙关平）
  * @version:   V1.0
  * @date:      2018-4-16
  * @brief:     fatfs文件系统
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

#define CONFIG_FILE_PATH    "1:config.txt"  /* 配置文件路径 */
#define ROOT_PATH           "1:"            /* 根目录 */


FATFS fs;                                   /* FatFs文件系统对象 */
FIL fnew;                                   /* 文件对象 */

char ReadBuffer[256] = {0};                 /* 读缓冲区 */


__packed typedef struct {
#define CHIP_NAME_MAX   32
#define FILE_NAME_MAX   64
#define CONFIG_YES      'Y'
#define CONFIG_NO       'N'

    uint8_t enable;         /* 是否启用 */
    char chip_name[CHIP_NAME_MAX];
    char file_name[FILE_NAME_MAX];
    uint32_t file_size;
    uint32_t file_check_sum;
    /* max_program: 0表示无限次烧录，1表示烧录次数用完，大于1表示烧录剩余次数 */
    uint32_t max_program;
    uint8_t verify_after_program :1;
    uint8_t read_out_protection :1;
    uint8_t mcu_run_after_program :1;
    uint8_t auto_program_control :1;
    uint8_t auto_program_time;

    /* 用于标记，是否有配置过 */
    uint8_t chip_name_set_flag :1;
    uint8_t file_name_set_flag :1;
    uint8_t max_program_set_flag :1;
    uint8_t verify_after_program_set_flag :1;
    uint8_t read_out_protection_set_flag :1;
    uint8_t mcu_run_after_program_set_flag :1;
    uint8_t auto_program_control_set_flag :1;
    uint8_t auto_program_time_set_flag :1;

    uint8_t check_sum;      /* 效验和 */
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
  * @brief  FATFS信息查看
  * @param  None
  * @retval None
  */
static void cli_fatfs_config_info(void)
{
    /* 从数据库读取保存的config配置文件，并打印出来 */
    print_config_file_from_database();
}


/**
  * @brief  FATFS信息查看
  * @param  None
  * @retval None
  */
static void cli_fatfs_firmware_info(void)
{
    if (E_NXP_WAITING == nxp_state_get()) {/* 由于fm_buff缓存共用因此做个互锁 */
        print_firmware_backup();
    }
}


/**
  * @brief  FATFS调试命令行
  * @param  None
  * @retval None
  */
uint8_t cli_fatfs(void *para, uint8_t len)
{
    uint8_t *pTemp = (uint8_t *)para;

    if ((0 < len) && (NULL != pTemp)) {
        pTemp++; /* 跳过一个空格 */

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
            /* 参数有错 */
            return FALSE;
        }
    }

    /* 正确返回 */
    return TRUE;
}

#endif


#if 0
/**
  * @brief  fatfs文件系统读写测试
  * @param  无
  * @retval 无
  */
static void fatfs_rd_wr_test(void)
{
#define WRITE_BUFF  "hello world"

    FRESULT res_flash;  /* 文件操作结果 */
    UINT fnum;          /* 文件成功读写数量 */

    /*----------------------- 文件系统测试：写测试 -------------------*/
    /* 打开文件，每次都以新建的形式打开，属性为可写 */
    PRINTF("file write test \r\n");
    res_flash = f_open(&fnew, "1:test.txt", FA_CREATE_ALWAYS | FA_WRITE);

    if (res_flash == FR_OK) {
        PRINTF("open file ok \r\n");
        /* 将指定存储区内容写入到文件内 */
        res_flash = f_write(&fnew, WRITE_BUFF, strlen(WRITE_BUFF), &fnum);

        if (res_flash == FR_OK) {
            printf("file write bytes = %d \r\n", fnum);
            printf("file write content: %s \r\n", WRITE_BUFF);
        } else {
            printf("file write fail：%d \r\n", res_flash);
        }
    } else {
        ERR("open/creat file fail \r\n");
    }
    /* 不再读写，关闭文件 */
    f_close(&fnew);
    NL2();

    /*------------------- 文件系统测试：读测试 --------------------------*/
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

    /* 不再读写，关闭文件 */
    f_close(&fnew);
    NL2();
}
#endif


/**
  * @brief  获取config_info.file_size
  * @param  无
  * @retval 无
  */
uint32_t get_config_info_file_size(void)
{
    return config_info.file_size;
}


/**
  * @brief  获取config_info.enable
  * @param  无
  * @retval 无
  */
uint8_t get_config_info_state(void)
{
    return config_info.enable;
}


/**
  * @brief  获取config_info.mcu_run_after_program
  * @param  无
  * @retval 无
  */
uint8_t get_config_info_mcu_run_after_program(void)
{
    return config_info.mcu_run_after_program;
}


/**
  * @brief  获取get_config_info_auto_program_control
  * @param  无
  * @retval 无
  */
uint8_t get_config_info_auto_program_control(void)
{
    return config_info.auto_program_control;
}


/**
  * @brief  获取get_config_info_auto_program_time
  * @param  无
  * @retval 无
  */
uint8_t get_config_info_auto_program_time(void)
{
    return config_info.auto_program_time;
}


/**
  * @brief  获取config_info.mcu_run_after_program
  * @param  无
  * @retval 无
  */
uint8_t config_info_max_program_check()
{
    uint8_t rtn = FALSE;

    /* max_program: 0表示无限次烧录，1表示烧录次数用完，大于1表示烧录剩余次数 */
    if (1 != config_info.max_program) {
        rtn = TRUE;
    }

    return rtn;
}


/**
  * @brief  查看是否满足烧录的条件
  * @param  无
  * @retval TRUE满足条件，可以进行烧录
  */
uint8_t check_program_condition(void)
{
    uint8_t temp1 = FALSE;
    uint8_t temp2 = FALSE;

    temp1 = get_config_info_state();
    temp2 = config_info_max_program_check();

    if ((TRUE == temp1) && (TRUE == temp2)) {
        /* 满足条件，可以进行烧录 */
        return TRUE;
    } else {
        return FALSE;
    }
}


/**
  * @brief  获取某个文件的信息
  * @param  无
  * @retval 无
  */
static FRESULT fatfs_file_check(const char *fpath, FILINFO *fno)
{
    FRESULT res_flash;  /* 文件操作结果 */

    /* 获取文件信息 */
    res_flash = f_stat(fpath, fno);

    if (res_flash == FR_OK) {
        DBG("--------------------begin");
        PRINTF("file path: %s \r\n", fpath);
        PRINTF("file size: %ld bytes \r\n", fno->fsize);
        PRINTF("time: %u/%02u/%02u, %02u:%02u \r\n",
               (fno->fdate >> 9) + 1980, fno->fdate >> 5 & 15, fno->fdate & 31, fno->ftime >> 11, fno->ftime >> 5 & 63);
        PRINTF("state: %c%c%c%c%c \r\n",
               (fno->fattrib & AM_DIR) ? 'D' : '-',      // 是一个目录
               (fno->fattrib & AM_RDO) ? 'R' : '-',      // 只读文件
               (fno->fattrib & AM_HID) ? 'H' : '-',      // 隐藏文件
               (fno->fattrib & AM_SYS) ? 'S' : '-',      // 系统文件
               (fno->fattrib & AM_ARC) ? 'A' : '-');     // 档案文件
        DBG("--------------------end \r\n");
    } else {
        ERR("fatfs_file_check fail \r\n");
    }

    return res_flash;
}


/**
  * @brief  递归扫描FatFs内的文件
  * @param  path:初始扫描路径
  * @retval result:文件系统的返回值
  */
static FRESULT scan_files(char* path)
{
    FRESULT res;      //部分在递归过程被修改的变量，不用全局变量
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;        // 文件名

#if _USE_LFN
    /* 长文件名支持 */
    /* 简体中文需要2个字节保存一个“字”*/
    static char lfn[_MAX_LFN * 2 + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
#endif
    //打开目录
    res = f_opendir(&dir, path);

    if (res == FR_OK) {
        i = strlen(path);

        for (;;) {
            //读取目录下的内容，再读会自动读下一个文件
            res = f_readdir(&dir, &fno);

            //为空时表示所有项目读取完毕，跳出
            if (res != FR_OK || fno.fname[0] == 0) break;

#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif

            //点表示当前目录，跳过
            if (*fn == '.') continue;

            //目录，递归读取
            if (fno.fattrib & AM_DIR) {
                //合成完整目录名
                sprintf(&path[i], "/%s", fn);
                //递归遍历
                res = scan_files(path);
                path[i] = 0;

                //打开失败，跳出循环
                if (res != FR_OK)
                    break;
            } else {
                printf("%s/%s\r\n", path, fn);  //输出文件名
                /* 可以在这里提取特定格式的文件路径 */
            }//else
        } //for
    }

    return res;
}


/**
  * @brief  递归扫描FatFs内的文件
  * @param  path:初始扫描路径
  * @retval result:文件系统的返回值
  */
static FRESULT fatfs_scan_files(char* path)
{
    FRESULT res;      //部分在递归过程被修改的变量，不用全局变量

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
    /* 芯片型号，字符串形式 */
    {KW_CHIP_NAME,                  chip_name_handle},
    /* 升级固件的文件名，字符串形式 */
    {KW_FILE_NAME,                  file_name_handle},
    /* 最大烧录次数，0为无限，(1->FFFF FFFF)次 */
    {KW_MAX_PROGRAM,                max_program_handle},
    /* 烧录完成是否效验，"Y"效验，"N"不效验 */
    {KW_VERIFY_AFTER_PROGRAM,       verify_after_program_handle},
    /* 读保护，防止非法读取固件,，"Y"开启，"N"关闭 */
    {KW_READ_OUT_PROTECTION,        read_out_protection_handle},
    /* 编程完成后让MCU运行，"Y"开启，"N"关闭 */
    {KW_MCU_RUN_AFTER_PROGRAM,      mcu_run_after_program_handle},
    /* 自动编程控制，"Y"自动，"N"手动 */
    {KW_AUTO_PROGRAM_CONTROL,       auto_program_control_handle},
    /* 等待多少秒时间再去编程下一个MCU，(1->FF)秒 */
    {KW_AUTO_PROGRAM_TIME,          auto_program_time_handle},
};


/**
  * @brief  解析目标MCU类型设置
  * @param  str配置字符串内容
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
        addr_1 += 1;  /* 指向配置的第一个字符 */
        addr_2 = strstr((const char *)addr_1, "\"");

        if (NULL != addr_2) {
            len = addr_2 - addr_1;
            if (len < CHIP_NAME_MAX) {
                memset(p_cfg->chip_name, 0x00, CHIP_NAME_MAX);
                memcpy(p_cfg->chip_name, addr_1, len);
                p_cfg->chip_name_set_flag = TRUE;
            } else {
                /* 名字太长 */
                p_cfg->chip_name_set_flag = FALSE;
                ERR("chip_name len overflow \r\n");
            }
        } else {
            /* 错误 */
            p_cfg->chip_name_set_flag = FALSE;
        }
    } else {
        /* 错误 */
        p_cfg->chip_name_set_flag = FALSE;
    }
}


/**
  * @brief  解析烧录文件名字设置
  * @param  str配置字符串内容
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
        addr_1 += 1;  /* 指向配置的第一个字符 */
        addr_2 = strstr((const char *)addr_1, "\"");

        if (NULL != addr_2) {
            len = addr_2 - addr_1;
            if (len < FILE_NAME_MAX) {
                memset(p_cfg->file_name, 0x00, FILE_NAME_MAX);
                memcpy(p_cfg->file_name, addr_1, len);
                p_cfg->file_name_set_flag = TRUE;
            } else {
                /* 名字太长 */
                p_cfg->file_name_set_flag = FALSE;
                ERR("file_name len overflow \r\n");
            }
        } else {
            /* 错误 */
            p_cfg->file_name_set_flag = FALSE;
        }
    } else {
        /* 错误 */
        p_cfg->file_name_set_flag = FALSE;
    }
}


/**
  * @brief  解析烧录次数设置
  * @param  str配置字符串内容
  * @retval null
  */
static void max_program_handle(char* str, CONFIG_INFO_S *p_cfg)
{
    char *addr_1;
    char *addr_2;

    PRINTF("max_program_handle: %s \r\n", str);

    addr_1 = strstr((const char *)str, "\"");

    if (NULL != addr_1) {
        addr_1 += 1;  /* 指向配置的第一个字符 */
        addr_2 = strstr((const char *)addr_1, "\"");

        if (NULL != addr_2) {
            addr_2[0] = 0x00;       /* 字符串结束符号 */

            extern int my_atoi(char *str);
            p_cfg->max_program = my_atoi(addr_1);   /* 字符串转整形 */
            if ((0 != p_cfg->max_program) && (0xffffffff != p_cfg->max_program)) {
                /* 这里为什么+1，看max_program定义时候的注释，max_program不能为0xffffffff，否则+1后溢出 */
                p_cfg->max_program += 1;
            }
            p_cfg->max_program_set_flag = TRUE;
            //PRINTF("p_cfg->max_program = %d \r\n", p_cfg->max_program);
        } else {
            /* 错误 */
            p_cfg->max_program_set_flag = FALSE;
        }
    } else {
        /* 错误 */
        p_cfg->max_program_set_flag = FALSE;
    }
}


/**
  * @brief  解析烧录完成后是否效验设置
  * @param  str配置字符串内容
  * @retval null
  */
static void verify_after_program_handle(char* str, CONFIG_INFO_S *p_cfg)
{
    char *addr_1;

    PRINTF("verify_after_program_handle: %s \r\n", str);

    addr_1 = strstr((const char *)str, "\"");

    if (NULL != addr_1) {
        addr_1 += 1;  /* 指向配置的第一个字符 */
        if (CONFIG_YES == addr_1[0]) {
            p_cfg->verify_after_program = TRUE;
            p_cfg->verify_after_program_set_flag = TRUE;
        } else if (NULL != addr_1[0]) {
            p_cfg->verify_after_program = FALSE;
            p_cfg->verify_after_program_set_flag = TRUE;
        } else {
            /* 参数错误 */
            p_cfg->verify_after_program_set_flag = FALSE;
        }

    } else {
        /* 错误 */
        p_cfg->verify_after_program_set_flag = FALSE;
    }
}


/**
  * @brief  解析读保护功能设置
  * @param  str配置字符串内容
  * @retval null
  */
static void read_out_protection_handle(char* str, CONFIG_INFO_S *p_cfg)
{
    char *addr_1;

    PRINTF("read_out_protection_handle: %s \r\n", str);

    addr_1 = strstr((const char *)str, "\"");

    if (NULL != addr_1) {
        addr_1 += 1;  /* 指向配置的第一个字符 */
        if (CONFIG_YES == addr_1[0]) {
            p_cfg->read_out_protection = TRUE;
            p_cfg->read_out_protection_set_flag = TRUE;
        } else if (NULL != addr_1[0]) {
            p_cfg->read_out_protection = FALSE;
            p_cfg->read_out_protection_set_flag = TRUE;
        } else {
            /* 参数错误 */
            p_cfg->read_out_protection_set_flag = FALSE;
        }

    } else {
        /* 错误 */
        p_cfg->read_out_protection_set_flag = FALSE;
    }
}


/**
  * @brief  解析烧录完成后MCU是否运行设置
  * @param  str配置字符串内容
  * @retval null
  */
static void mcu_run_after_program_handle(char* str, CONFIG_INFO_S *p_cfg)
{
    char *addr_1;

    PRINTF("mcu_run_after_program_handle: %s \r\n", str);

    addr_1 = strstr((const char *)str, "\"");

    if (NULL != addr_1) {
        addr_1 += 1;  /* 指向配置的第一个字符 */
        if (CONFIG_YES == addr_1[0]) {
            p_cfg->mcu_run_after_program = TRUE;
            p_cfg->mcu_run_after_program_set_flag = TRUE;
        } else if (NULL != addr_1[0]) {
            p_cfg->mcu_run_after_program = FALSE;
            p_cfg->mcu_run_after_program_set_flag = TRUE;
        } else {
            /* 参数错误 */
            p_cfg->mcu_run_after_program_set_flag = FALSE;
        }

    } else {
        /* 错误 */
        p_cfg->mcu_run_after_program_set_flag = FALSE;
    }
}


/**
  * @brief  解析自动编程控制设置
  * @param  str配置字符串内容
  * @retval null
  */
static void auto_program_control_handle(char* str, CONFIG_INFO_S *p_cfg)
{
    char *addr_1;

    PRINTF("auto_program_control_handle: %s \r\n", str);

    addr_1 = strstr((const char *)str, "\"");

    if (NULL != addr_1) {
        addr_1 += 1;  /* 指向配置的第一个字符 */
        if (CONFIG_YES == addr_1[0]) {
            p_cfg->auto_program_control = TRUE;
            p_cfg->auto_program_control_set_flag = TRUE;
        } else if (NULL != addr_1[0]) {
            p_cfg->auto_program_control = FALSE;
            p_cfg->auto_program_control_set_flag = TRUE;
        } else {
            /* 参数错误 */
            p_cfg->auto_program_control_set_flag = FALSE;
        }

    } else {
        /* 错误 */
        p_cfg->auto_program_control_set_flag = FALSE;
    }
}


/**
  * @brief  解析自动编程延时时间设置
  * @param  str配置字符串内容
  * @retval null
  */
static void auto_program_time_handle(char* str, CONFIG_INFO_S *p_cfg)
{
    char *addr_1;
    char *addr_2;

    PRINTF("auto_program_time_handle: %s \r\n", str);

    addr_1 = strstr((const char *)str, "\"");

    if (NULL != addr_1) {
        addr_1 += 1;  /* 指向配置的第一个字符 */
        addr_2 = strstr((const char *)addr_1, "\"");

        if (NULL != addr_2) {
            addr_2[0] = 0x00;       /* 字符串结束符号 */

            extern int my_atoi(char *str);
            p_cfg->auto_program_time = my_atoi(addr_1);  /* 字符串转整形 */
            p_cfg->auto_program_time_set_flag = TRUE;
            //PRINTF("p_cfg->auto_program_time = %d \r\n", p_cfg->auto_program_time);
        } else {
            /* 错误 */
            p_cfg->auto_program_time_set_flag = FALSE;
        }
    } else {
        /* 错误 */
        p_cfg->auto_program_time_set_flag = FALSE;
    }
}


/**
  * @brief  处理config配置文件
  * @param  path:config配置文件路径
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
  * @brief  判断一行字符是否是注释，如果是注释就不需要解析
  * @param  line一行字符
  * @retval 返回0表示这一行是注释，不需要解析
  */
static char *check_comment_line(char *line)
{
    /* 判断第一个字符是不是注释符号："#" */
    if ('#' == line[0]) {
        //PRINTF("comment line here: %s \r\n", line);
        /* 如果是注释开头，那么就返回一个0，表示这行不需要解析了 */
        line = 0;
    }

    return line;
}


/**
  * @brief  计算config_info的效验和
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
  * @brief  调试，打印查看config_info
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
  * @brief  从数据库读取保存的config配置文件
  * @param  无
  * @retval 无
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
  * @brief  从数据库读取保存的config配置文件，并打印出来
  * @param  无
  * @retval 无
  */
static void print_config_file_from_database(void)
{
    config_info_update_from_database();

    config_info_print(&config_info);
}


/**
  * @brief  将config_info存到数据库，把fatfs中的升级固件复制到固件备份区
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
  * @brief  每次烧录成功后，处理烧录次数记录
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
/* 用于调试，出错时打印出错信息，正确则不打印 */
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
  * @brief  sprintf_buff初始化
  * @param  null
  * @retval null
  */
static void sprintf_buff_init(void)
{
    sprintf_index = 0;
    memset(sprintf_buff, 0x00, SPRINTF_BUFF_LEN);
}


/**
  * @brief  增加错误信息到sprintf_buff
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
  * @brief  显示sprintf_buff里面的错误信息
  * @param  null
  * @retval null
  */
static void sprintf_buff_show(void)
{
    ERR("--------------------begin");
    /* sprintf_buff是否溢出，暂时没添加判断 */
    if (sprintf_index) {
        PRINTF("%s", sprintf_buff);
    }
    sprintf_index = 0;
    ERR("--------------------end \r\n");
}


/**
  * @brief  判断配置文件各个参数是否合法
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
        处理芯片型号设置
    */
    err = TRUE;
    if (TRUE == p_cfg->chip_name_set_flag) {
        /* 判断芯片型号是否合法 */
        for (len = 0; len < NXP_CHIP_MAX; len++) {
            if (0 == memcmp(p_cfg->chip_name, nxp_chip[len].name, strlen(nxp_chip[len].name))) {
                /* 芯片型号合法 */
                err = FALSE;
                /* 记录该芯片FLASH大小 */
                p_cfg->file_size = nxp_chip[len].flash_size;
                break;
            }
        }
        if (TRUE == err) {
            /* 芯片型号不合法 */
            sprintf_buff_add(config_err_code[1]);
            p_cfg->chip_name_set_flag = FALSE;
        }
    } else {
        /* 芯片型号必须设置，提示错误 */
        sprintf_buff_add(config_err_code[0]);
    }

    /*
        处理升级固件文件名设置
    */
    if (TRUE == p_cfg->file_name_set_flag) {
        /* 判断升级固件是否存在 */
        len = (uint8_t)strlen(p_cfg->file_name);
        if (len > FILE_NAME_MAX) {
            len = FILE_NAME_MAX;
        }
        memcpy(temp_dir, ROOT_PATH, 2);
        memcpy(&temp_dir[2], p_cfg->file_name, len);
        if (FR_OK == fatfs_file_check((const char *)temp_dir, &fno)) {
            /* 升级固件存在，还需要判断固件大小是否超出芯片FLASH范围 */
            if (p_cfg->file_size < fno.fsize) {
                /* 升级固件太大，超出芯片内部FLASH大小 */
                if (TRUE == p_cfg->chip_name_set_flag) {
                    /* 如果芯片型号错误，那么p_cfg->file_size就会是0 */
                    sprintf_buff_add(config_err_code[3]);
                }
                p_cfg->file_name_set_flag = FALSE;
            } else {
                p_cfg->file_size = fno.fsize;
            }
        } else {
            /* 升级固件不存在 */
            sprintf_buff_add(config_err_code[2]);
            p_cfg->file_name_set_flag = FALSE;
        }
    } else {
        /* 升级固件必须设置，提示错误 */
        sprintf_buff_add(config_err_code[4]);
    }

    /*
        处理最大编程次数设置
    */
    if (TRUE == p_cfg->max_program_set_flag) {
        /* 最大编程次数 */
        
    } else {
        /* 最大编程次数没有设置的话，默认设置成0，0表示无线次数烧录 */
        p_cfg->max_program = 0;
        p_cfg->max_program_set_flag = TRUE;
    }

    /*
        处理烧录完成效验设置
    */
    if (TRUE == p_cfg->verify_after_program_set_flag) {
        /* 烧录完成效验 */
        
    } else {
        /* 烧录完成效验没有设置的话，默认设置成FALSE */
        p_cfg->verify_after_program = FALSE;
        p_cfg->verify_after_program_set_flag = TRUE;
    }

    /*
        处理读保护设置
    */
    if (TRUE == p_cfg->read_out_protection_set_flag) {
        /* 读保护 */
        
    } else {
        /* 读保护没有设置的话，默认设置成FALSE */
        p_cfg->read_out_protection = FALSE;
        p_cfg->read_out_protection_set_flag = TRUE;
    }

    /*
        处理烧录完成MCU直接运行设置
    */
    if (TRUE == p_cfg->mcu_run_after_program_set_flag) {
        /* 烧录完成MCU直接运行 */
        
    } else {
        /* 烧录完成MCU直接运行没有设置的话，默认设置成TRUE */
        p_cfg->mcu_run_after_program = TRUE;
        p_cfg->mcu_run_after_program_set_flag = TRUE;
    }

    /*
        处理自动编程设置
    */
    if (TRUE == p_cfg->auto_program_control_set_flag) {
        /* 自动编程 */
        
    } else {
        /* 自动编程没有设置的话，默认设置成FALSE */
        p_cfg->auto_program_control = FALSE;
        p_cfg->auto_program_control_set_flag = TRUE;
    }

    /*
        处理自动编程时间设置
    */
    if (TRUE == p_cfg->auto_program_time_set_flag) {
        /* 自动编程 */
        
    } else {
        /* 自动编程时间没有设置的话，默认设置成5s */
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
        //config_info_print(p_cfg);   /* 调试，打印查看config_info */
        err = FALSE;
    } else {
        err = TRUE;
    }

    return err;
}


/**
  * @brief  刚上电，固件备份区的固件进行效验
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
    char *read_buff = (char *)fm_buff;   /* 减少stack的压力 */
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
  * @brief  打印固件备份区的固件
  * @param  null
  * @retval null
  */
static void print_firmware_backup(void)
{
    unsigned int rd_count = 0;
    unsigned int rd_once = 0;
    uint32_t fm_backup_addr = FM_BACKUP_SECTOR_START * FM_BACKUP_SECTOR_SIZE;

    extern uint8_t fm_buff[];
    char *read_buff = (char *)fm_buff;   /* 减少stack的压力 */
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
            /* 这里调试打印会花费很多时间，超过10s引发看门狗复位 */
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
  * @brief  刚上电，固件备份区的固件进行效验，从数据库读取comfig_info
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
  * @brief  从fatfs文件系统读取升级固件并复制到固件备份区
  * @param  null
  * @retval null
  */
static uint8_t copy_firmware_to_backup(CONFIG_INFO_S *p_cfg)
{
    uint8_t err = FALSE;
    FRESULT res_flash;  /* 文件操作结果 */
    uint16_t i = 0;
    char *temp_dir = ReadBuffer;
    unsigned int rd_count = 0;
    unsigned int rd_once = 0;
    uint32_t fm_backup_addr = FM_BACKUP_SECTOR_START * FM_BACKUP_SECTOR_SIZE;

    extern uint8_t fm_buff[];
    char *read_buff = (char *)fm_buff;   /* 减少stack的压力 */
    uint16_t read_buff_max = 4096;

    memset(temp_dir, 0x00, sizeof(ReadBuffer));
    memcpy(temp_dir, ROOT_PATH, 2);
    memcpy(&temp_dir[2], p_cfg->file_name, strlen(p_cfg->file_name));

    p_cfg->file_check_sum = 0;

    res_flash = f_open(&fnew, temp_dir, FA_OPEN_EXISTING | FA_READ);

    if (res_flash == FR_OK) {
        while (rd_count < p_cfg->file_size) {
            /*  文件定位到rd_count位置 */
            res_flash = f_lseek(&fnew, rd_count);
            /* 读取文件所有内容到缓存区 */
            res_flash = f_read(&fnew, read_buff, read_buff_max, &rd_once);

            /* 计算固件的效验和 */
            for (i = 0; i < rd_once; i++) {
                p_cfg->file_check_sum += read_buff[i];
            }

            if (TRUE == fatfs_dbg_en) {
                extern volatile uint16_t os_watch_dog_cnt;
                /* 这里调试打印会花费很多时间，超过10s引发看门狗复位 */
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
        /* 上面更新了file_check_sum，因此重新计算config_info的效验和 */
        p_cfg->check_sum = calc_config_info_check_sum(p_cfg);
        LOG("copy_firmware_to_backup ok \r\n");
    } else {
        sprintf_buff_add(config_err_code[2]);

        /* 没有检测升级固件，提示错误 */
        err = TRUE;
    }

    /* 不再读写，关闭文件 */
    f_close(&fnew);

    return err;
}


/**
  * @brief  读取config配置文件，并提取配置信息保存
  * @param  path:config配置文件路径
  * @retval null
  */
void config_file_handle(void)
{
//#define CONFIG_FILE_HANDLE_DENUG

    uint8_t err = FALSE;
    CONFIG_INFO_S temp_config_info;
    FRESULT res_flash;  /* 文件操作结果 */

#if 0   /* 调试，查看config配置文件的文件信息 */
    //FILINFO fno;
    //fatfs_file_check(CONFIG_FILE_PATH, &fno);
#endif

    sprintf_buff_init();
    memset(&temp_config_info, 0x00, sizeof(temp_config_info));

    fatfs_mount();

    res_flash = f_open(&fnew, CONFIG_FILE_PATH, FA_OPEN_EXISTING | FA_READ);

    if (res_flash == FR_OK) {
        //PRINTF("config file open ok \r\n");
        /* f_gets读取碰到'\n'或'\0'就停止，可用于每次读取一个字符串或是一行数据 */
        while (0 != f_gets((TCHAR *)ReadBuffer, sizeof(ReadBuffer), &fnew)) {
            #ifdef CONFIG_FILE_HANDLE_DENUG
            PRINTF("config file read line(%d): %s \r\n", strlen(ReadBuffer), ReadBuffer);
            #endif

            /* 去除所有的空格字符 */
            void my_str_remove_blank(char *str);
            my_str_remove_blank(ReadBuffer);

            /* 解析这一行数据 */
            char *p_line;
            p_line = check_comment_line(ReadBuffer);
            if (0 != p_line) {
                match_config_key_word(p_line, &temp_config_info);
            }
        }
    } else {
        sprintf_buff_add(config_err_code[5]);

        /* 没有检测到配置文件，提示错误 */
        err = TRUE;
    }

    /* 不再读写，关闭文件 */
    f_close(&fnew);

    if (FALSE == err) {
        /* 配置文件保存到写平衡区域，并把升级文件复制到备份区 */
        if (FALSE == config_info_check(&temp_config_info)) {
            /* 把fatfs中的升级固件复制到固件备份区 */
            if (FALSE == copy_firmware_to_backup(&temp_config_info)) {
                /* 将config_info存到数据库 */
                config_info_save_to_database(&temp_config_info);
            } else {
                err = TRUE;
            }
        } else {
            err = TRUE;
        }
    }

    if (TRUE == err) {
        /* 蜂鸣器提示错误 */
        extern void buzzer_notice_config_fail(void);
        buzzer_notice_config_fail();
        /* 提示错误类型 */
        sprintf_buff_show();
    } else {
        extern void buzzer_notice_config_ok(void);
        buzzer_notice_config_ok();
    }

    /* 不再使用文件系统，取消挂载文件系统 */
    fatfs_unmount();
}


/**
  * @brief  挂载文件系统，如果没有文件系统，则格式化后重新挂载文件系统
  * @param  无
  * @retval 无
  */
static void fatfs_mount(void)
{
    FRESULT res_flash;  /* 文件操作结果 */

    /* 在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化，初始化函数调用
        流程如下:f_mount()->find_volume()->disk_initialize->spi_flash_Init() */
    res_flash = f_mount(&fs, "1:", 1);

    /* 如果没有文件系统就格式化并创建文件系统 */
    if (res_flash == FR_NO_FILESYSTEM) {
        LOG("fatfs dont esist \r\n");
        /* 格式化 */
        res_flash = f_mkfs("1:", 0, 0);

        if (res_flash == FR_OK) {
            PRINTF("fatfs format ok \r\n");
            /* 格式化后，先取消挂载 */
            res_flash = f_mount(NULL, "1:", 1);
            /* 重新挂载   */
            res_flash = f_mount(&fs, "1:", 1);
        } else {
            /* 格式化SPI_FLASH并创建文件系统失败 */
            ERR("fatfs format fail \r\n");
            while (1);
        }
    } else if (res_flash != FR_OK) {
        /* 外部Flash挂载文件系统失败，可能原因：SPI Flash初始化不成功 */
        ERRA("fatfs init fail = %d \r\n", res_flash);
        while (1);
    } else {
        /* 挂载FATFS文件系统成功 */
        //PRINTF("fatfs ok \r\n");
    }
}


/**
  * @brief  取消挂载文件系统
  * @param  无
  * @retval 无
  */
static void fatfs_unmount(void)
{
    /* 不再使用文件系统，取消挂载文件系统 */
    f_mount(NULL, "1:", 1);
}


/**
  * @brief  fatfs文件系统初始化
  * @param  无
  * @retval 无
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

    /* 遍历打印fatfs中所有文件的路径 */
    fatfs_scan_files(ROOT_PATH);

    fatfs_unmount();
}


/**
  * @brief  fatfs文件系统
  * @param  null
  * @retval null
  */
void fatfs_task(void)
{
    /* 暂时不用 */
}


/*********************************************END OF FILE*********************/
