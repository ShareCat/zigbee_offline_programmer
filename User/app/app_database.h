/**
  ******************************************************************************
  * @file:      app_database.h
  * @author:    Cat（孙关平）
  * @version:   V1.0
  * @date:      2018-3-3
  * @brief:     数据库，用于保存数据，使用LittleFS，支持写平衡，掉电保护
  * @attention:
  ******************************************************************************
  */


#ifndef __APP_DATABASE_H
#define __APP_DATABASE_H


#include "global.h"


extern void database_init(void);
extern void database_scan_task(void);


#endif /* __APP_DATABASE_H */
