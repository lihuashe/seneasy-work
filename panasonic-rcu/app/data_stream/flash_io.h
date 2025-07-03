/**
 * @file flash_io.h
 * @author likeliang (likeliang@seneasy.com)
 * @brief 
 * @version 0.1
 * @date 2025-03-25
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef APP_DATA_STREAM_FLASH_IO_H_
#define APP_DATA_STREAM_FLASH_IO_H_

#ifdef __cplusplus
extern "C" {
#endif
#define FLASH_FACTORY_MAC_START_ADDR 0x08007C00 // 31,744
#define FLASH_USER_START_ADDR 0x08007D00		// 32,000
void flash_save();
void flash_load();
#ifdef __cplusplus
}
#endif

#endif  // APP_DATA_STREAM_FLASH_IO_H_
