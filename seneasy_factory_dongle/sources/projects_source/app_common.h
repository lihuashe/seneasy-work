/**
 * @file app_common.h
 * @author likeliang (likeliang@seneasy.com)
 * @brief 
 * @version 0.1
 * @date 2025-06-27
 * 
 * @copyright Copyright (c) 2025
 * 
*/


#ifndef SOURCES_PROJECTS_SOURCE_APP_COMMON_H_
#define SOURCES_PROJECTS_SOURCE_APP_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "app_cfg.h"

typedef enum
{
    EN_MSG_MODULE_BLE            = 0x00,
    EN_MSG_MODULE_BLE_CENTRAL    = 0x01,
    EN_MSG_MODULE_HID_CLIENT     = 0x05,
    EN_MSG_MODULE_ATVV_CLIENT    = 0x06,
    EN_MSG_MODULE_BATTERY_CLIENT = 0x07,
    EN_MSG_MODULE_OTA_CLIENT     = 0x08,
    EN_MSG_MODULE_POWER_MANAGE   = 0x09,
    EN_MSG_MODULE_LED            = 0x0A,
    EN_MSG_MODULE_IPC            = 0x0B,
    EN_MSG_MODULE_USB            = 0x0C,
    
}EN_MSG_MODULE_T;


#ifdef __cplusplus
}
#endif

#endif  // SOURCES_PROJECTS_SOURCE_APP_COMMON_H_
