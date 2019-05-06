/**
  ******************************************************************************
  * @file:      app_database.c
  * @author:    Cat（孙关平）
  * @version:   V1.0
  * @date:      2018-3-3
  * @brief:     数据库，用于保存数据，使用LittleFS，支持写平衡，掉电保护
  * @attention:
  ******************************************************************************
  */


#include "./app_database.h"

#include "./../sys/sys_scheduler.h"
#include "./../sys/sys_command_line.h"
#include "./../sys/sys_queue.h"

#include "./../bsp/bsp_spi.h"
#include "./../component/littlefs/lfs.h"


int user_provided_block_device_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size) {
    spi_flash_buffer_read((uint8_t*)buffer, (block * c->block_size + off), size);
    return 0;
}

int user_provided_block_device_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size) {
    spi_flash_buffer_write((uint8_t*)buffer, (block * c->block_size + off), size);
    return 0;
}

int user_provided_block_device_erase(const struct lfs_config *c, lfs_block_t block) {
    spi_flash_sector_erase(block * c->block_size);
    return 0;
}

int user_provided_block_device_sync(const struct lfs_config *c) {
    return 0;
}


// variables used by the filesystem
lfs_t lfs;
lfs_file_t file;

// configuration of the filesystem is provided by this struct
const struct lfs_config cfg = {
    // block device operations
    .read  = user_provided_block_device_read,
    .prog  = user_provided_block_device_prog,
    .erase = user_provided_block_device_erase,
    .sync  = user_provided_block_device_sync,

    // block device configuration
    .read_size = 64,
    .prog_size = 64,
    .block_size = DATABASE_SECTOR_SIZE,
    .block_count = FM_BACKUP_SECTOR_COUNT,
    .cache_size = 64,
    .lookahead_size = 64,
#if 0
	.read_buffer = lfs_read_buf,
	.prog_buffer = lfs_prog_buf,
	.lookahead_buffer = lfs_lookahead_buf,
	//.file_buffer = lfs_file_buf,
#endif
};


/**
  * @brief  按键任务初始化
  * @param  null
  * @retval null
  */
void database_init(void)
{
    spi_flash_init();

    // mount the filesystem
    int err = lfs_mount(&lfs, &cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        lfs_format(&lfs, &cfg);
        lfs_mount(&lfs, &cfg);
    }

#if 1   /* 用于测试，记录上电次数 */
    // read current count
    uint32_t boot_count = 0;
    lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));

    // update boot count
    boot_count += 1;
    lfs_file_rewind(&lfs, &file);
    lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&lfs, &file);

    // print the boot count
    printf("boot_count: %d \r\n\r\n", boot_count);
#endif

    // release any resources we were using
    lfs_unmount(&lfs);

    void check_firmware_backup_and_config_info(void);
    check_firmware_backup_and_config_info();
}


/**
  * @brief  保存一个文件到数据库
  * @param  null
  * @retval null
  */
void database_save(char* file_name, uint8_t *file_buff, uint32_t file_len)
{
    // mount the filesystem
    int err = lfs_mount(&lfs, &cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        lfs_format(&lfs, &cfg);
        lfs_mount(&lfs, &cfg);
    }

    lfs_file_open(&lfs, &file, file_name, LFS_O_RDWR | LFS_O_CREAT);

    lfs_file_rewind(&lfs, &file);
    lfs_file_write(&lfs, &file, file_buff, file_len);

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&lfs, &file);

    // release any resources we were using
    lfs_unmount(&lfs);
}


/**
  * @brief  从数据库读取一个文件
  * @param  null
  * @retval null
  */
void database_read(char* file_name, uint8_t *file_buff, uint32_t file_len)
{

    // mount the filesystem
    int err = lfs_mount(&lfs, &cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        lfs_format(&lfs, &cfg);
        lfs_mount(&lfs, &cfg);
    }

    lfs_file_open(&lfs, &file, file_name, LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, file_buff, file_len);

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&lfs, &file);

    // release any resources we were using
    lfs_unmount(&lfs);
}


/**
  * @brief  数据库任务，由系统20ms调度一次
  * @param  null
  * @retval null
  */
void database_task(void)
{

}


/*********************************************END OF FILE**********************/
