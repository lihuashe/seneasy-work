/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     SMARTRCU_service.h
  * @brief    Head file for using SMARTRCU Service Remote Controller.
  * @details  SMARTRCU service data structs and external functions declaration.
  * @author   Chenjie Jin
  * @date     2018-5-7
  * @version  v1.1
  * *************************************************************************************
  */

#ifndef _SMARTRCU_SERVICE_H_
#define _SMARTRCU_SERVICE_H_

#include "profile_server.h"

#ifdef __cplusplus
extern "C"  {
#endif      /* __cplusplus */


/** @brief  Demo Profile service related UUIDs. */

#define GATT_UUID_CHAR_SMARTRCU_READ_DATA                        0xB001
#define GATT_UUID_CHAR_SMARTRCU_WRITE_DATA                       0xB002

/** @brief  Index of each characteristic in Demo Profile service database. */
#define BLE_SERVICE_CHAR_SMARTRCU_READ_DATA_INDEX                0x02
#define BLE_SERVICE_CHAR_SMARTRCU_WRITE_DATA_INDEX               0x04
#define GATT_SVC_SMARTRCU_WRITE_CHAR_CCCD_INDEX                  0x05

#define SMARTRCU_WRITE_DATA  1
#define SMARTRCU_WRITE_DATA_CCCD  2

#define SMARTRCU_NOTIFY_ENABLE  1
#define SMARTRCU_NOTIFY_DISABLE  2

#define SMARTRCU_READ_DATA  1
/** Message content */
typedef union
{
    struct
    {
        uint8_t len;
        uint8_t *report;
    } report_data;
} T_SMARTRCU_WRITE_PARAMETER;

/** @struct _TATVV_WRITE_MSG
  * @brief write message
  */
typedef struct
{
    uint8_t write_type; /**< ref: @ref SMARTRCU_Write_Info */
    T_SMARTRCU_WRITE_PARAMETER write_parameter;
} T_SMARTRCU_WRITE_MSG;

typedef struct
{
    uint8_t read_type; /**< ref: @ref Vendor_Write_Info */
    uint8_t read_data;
} T_SMARTRCU_READ_MSG;

typedef union _TSMARTRCU_UPSTREAM_MSG_DATA
{
    uint8_t notification_indification_index;
    T_SMARTRCU_WRITE_MSG write_msg;
    T_SMARTRCU_READ_MSG read_msg;
} TSMARTRCU_UPSTREAM_MSG_DATA;

/** SMARTRCU service data to inform application */
typedef struct _T_SMARTRCU_CALLBACK_DATA
{
    T_SERVICE_CALLBACK_TYPE     msg_type;
    TSMARTRCU_UPSTREAM_MSG_DATA    msg_data;
} T_SMARTRCU_CALLBACK_DATA;

uint8_t smartrcu_svc_add_service(void *pFunc);

#ifdef __cplusplus
}
#endif

#endif
