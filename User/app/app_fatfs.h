/**
  ******************************************************************************
  * @file:      app_fatfs.h
  * @author:    Cat?????
  * @version:   V1.0
  * @date:      2018-4-16
  * @brief:     fatfs????
  * @attention: 
  ******************************************************************************
  */


#ifndef __APP_FATFS_H
#define __APP_FATFS_H

#include "global.h"



#ifdef FATFS_CLI_DEBUG
/* 命令行的帮助信息 */
#define FATFS_CMD \
    "[fatfs]\r\n"\
    " * fatfs config info: configuration information\r\n"\
    " * fatfs fm info: firmware information\r\n"\
    " * fatfs dbg 1: fatfs debug on\r\n"\
    " * fatfs dbg 0: fatfs debug off\r\n"\
    "\r\n"

extern uint8_t cli_fatfs(void *para, uint8_t len);
#endif


extern uint8_t check_program_ok(void);
extern uint32_t get_config_info_file_size(void);
extern uint8_t get_config_info_state(void);
extern uint8_t get_config_info_mcu_run_after_program(void);
extern uint8_t get_config_info_auto_program_control(void);
extern uint8_t get_config_info_auto_program_time(void);
extern void config_info_max_program_handle_after_program(void);
extern void fatfs_init(void);
extern void fatfs_task(void);


#endif /* __APP_FATFS_H */
