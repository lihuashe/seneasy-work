/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     dfu_service.h
* @brief
* @details
* @author   ken_mei
* @date     02-09-2016
* @version  v1.0.0
******************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2015 Realtek Semiconductor Corporation</center></h2>
******************************************************************************
*/

#ifndef _DFU_SERVICE_H_
#define _DFU_SERVICE_H_

#ifdef __cplusplus
extern "C"  {
#endif      /* __cplusplus */

#include "profile_server.h"
#include "flash_device.h"
#include "patch_header_check.h"


#define GATT_UUID128_DFU_SERVICE        0x12, 0xA2, 0x4D, 0x2E, 0xFE, 0x14, 0x48, 0x8e, 0x93, 0xD2, 0x17, 0x3C, 0x87, 0x62, 0x00, 0x00
#define GATT_UUID128_DFU_PACKET         0x12, 0xA2, 0x4D, 0x2E, 0xFE, 0x14, 0x48, 0x8e, 0x93, 0xD2, 0x17, 0x3C, 0x87, 0x63, 0x00, 0x00
#define GATT_UUID128_DFU_CONTROL_POINT  0x12, 0xA2, 0x4D, 0x2E, 0xFE, 0x14, 0x48, 0x8e, 0x93, 0xD2, 0x17, 0x3C, 0x87, 0x64, 0x00, 0x00

/*attribut index*/
#define INDEX_DFU_PACKET_VALUE                  0x02
#define INDEX_DFU_CONTROL_POINT_CHAR_VALUE      0x04
#define INDEX_DFU_CHAR_CCCD_INDEX               0x05

/*length of each control point procedure*/
#define DFU_LENGTH_START_DFU                    (1+12+4)/*4 bytes is padding for encrypt*/
#define DFU_LENGTH_RECEIVE_FW_IMAGE_INFO        (1+2+4) //img_id + cur_offset
#define DFU_LENGTH_VALID_FW                     (1+2)   //img_id
#define DFU_LENGTH_ACTIVE_IMAGE_RESET           (1)
#define DFU_LENGTH_SYSTEM_RESET                 (1)
#define DFU_LENGTH_REPORT_TARGET_INFO           (1+2) //img_id
#define DFU_LENGTH_CONN_PARA_TO_UPDATE_REQ      (1+2+2+2+2) //conn_interval_min,conn_interval_max,conn_latency,superv_tout
#define DFU_LENGTH_BUFFER_CHECK_EN              (1)
#define DFU_LENGTH_REPORT_BUFFER_CRC            (1+2+2) //buf_size, buf_crc

/*length of notification*/
#define DFU_NOTIFY_LENGTH_ARV                   3  //others opcode notification length
#define DFU_NOTIFY_LENGTH_START_DFU             (DFU_NOTIFY_LENGTH_ARV)
#define DFU_NOTIFY_LENGTH_VALID_FW              (DFU_NOTIFY_LENGTH_ARV)
#define DFU_NOTIFY_LENGTH_CONN_PARA_UPDATE_REQ  (DFU_NOTIFY_LENGTH_ARV)
#define DFU_NOTIFY_LENGTH_REPORT_TARGET_INFO    (DFU_NOTIFY_LENGTH_ARV+4+4)  //img_ver, cur_offset
#define DFU_NOTIFY_LENGTH_BUFFER_CHECK_EN       (DFU_NOTIFY_LENGTH_ARV+2+2)  //buf_size, mtu_size
#define DFU_NOTIFY_LENGTH_REPORT_BUFFER_CRC     (DFU_NOTIFY_LENGTH_ARV+4)   //cur_offset
#define DFU_NOTIFY_LENGTH_RECEIVE_IC_TYPE       (DFU_NOTIFY_LENGTH_ARV+1)   //ic_type
/*max length*/
#define DFU_NOTIFY_LENGTH_MAX                   DFU_NOTIFY_LENGTH_REPORT_TARGET_INFO


/*============================================================================*
  *                                Types
  *============================================================================*/
/** @defgroup DFU_Client_Exported_Types Exported Types
  * @{
  */
/*each control point procedure,can't modify exist value*/
typedef enum
{
    DFU_OPCODE_MIN                          = 0x00, /*control point opcode min*/
    DFU_OPCODE_START_DFU                    = 0x01,
    DFU_OPCODE_RECEIVE_FW_IMAGE_INFO        = 0x02,
    DFU_OPCODE_VALID_FW                     = 0x03,
    DFU_OPCODE_ACTIVE_IMAGE_RESET           = 0x04,
    DFU_OPCODE_SYSTEM_RESET                 = 0x05,
    DFU_OPCODE_REPORT_TARGET_INFO           = 0x06,
    DFU_OPCODE_CONN_PARA_TO_UPDATE_REQ      = 0x07,
    DFU_OPCODE_PKT_RX_NOTIFICATION_VOICE    = 0x08,
    DFU_OPCODE_BUFFER_CHECK_EN              = 0x09,
    DFU_OPCODE_REPORT_BUFFER_CRC            = 0x0a,
    DFU_OPCODE_RECEIVE_IC_TYPE              = 0x0b,
    DFU_OPCODE_COPY_IMG                     = 0x0c,
    DFU_OPCODE_MAX                          = 0x0d, /*control point opcode max*/
    DFU_OPCODE_NOTIFICATION                 = 0x10, /*notification opcode,fixed*/
} T_DFU_CP_OPCODE;

/*dfu error code*/
typedef enum
{
    DFU_ARV_SUCCESS                        = 0x01,
    DFU_ARV_FAIL_INVALID_PARAMETER         = 0x02,
    DFU_ARV_FAIL_OPERATION                 = 0x03,
    DFU_ARV_FAIL_DATA_SIZE_EXCEEDS_LIMIT   = 0x04,
    DFU_ARV_FAIL_CRC_ERROR                 = 0x05,
    DFU_ARV_FAIL_LENGTH_ERROR              = 0x06,
    DFU_ARV_FAIL_PROG_ERROR                = 0x07,
    DFU_ARV_FAIL_ERASE_ERROR               = 0x08,
} T_DFU_ARV_ERROR_CODE;

typedef void (*P_FUN_DFU_OPCODE_CB)(uint8_t conn_id, uint8_t opcode);

typedef struct _T_DFU_CTRL_POINT
{
    uint8_t opcode;
    T_IMG_CTRL_HEADER_FORMAT start_dfu; /* start dfu trans first 12 bytes ctrl header*/
} T_DFU_CTRL_POINT, * P_DFU_CTRL_POINT;

typedef struct
{
    T_IMG_CTRL_HEADER_FORMAT ctrl_header;
    uint32_t image_total_length;
    uint32_t origin_image_version;
    uint32_t cur_offset;
    uint8_t mtu_size;
    bool dfu_conn_para_upd_in_progress;
} T_DFU_PARA;

typedef struct
{
    T_IMG_ID image_id;
    uint32_t image_size;
    uint32_t image_offset;
} T_TEMP_IMAGE_INFO;

typedef enum
{
    DFU_NOTIFY_ENABLE  = 1,
    DFU_NOTIFY_DISABLE = 2,
} T_DFU_NOTIF_IND_IDX;

typedef union _TDFU_UPSTREAM_MSG_DATA
{
    T_DFU_NOTIF_IND_IDX notification_indification_index;
    uint8_t write_value_index;
} TDFU_UPSTREAM_MSG_DATA;

/** Dfu service data to inform application */
typedef struct _TDFU_CALLBACK_DATA
{
    uint8_t                     conn_id;
    T_SERVICE_CALLBACK_TYPE     msg_type;
    TDFU_UPSTREAM_MSG_DATA      msg_data;
} TDFU_CALLBACK_DATA;

/*============================================================================*
  *                               external variables
  *============================================================================*/
extern T_DFU_PARA g_dfu_para;
extern uint8_t *p_ota_temp_buffer_head;
extern uint16_t g_ota_tmp_buf_used_size;
extern uint32_t g_sil_dfu_resend_offset;

/*============================================================================*
  *                                Functions
  *============================================================================*/
void dfu_notify_conn_para_update_req(uint8_t conn_id, T_DFU_ARV_ERROR_CODE error_code);

void dfu_service_handle_valid_fw(uint8_t conn_id);

uint8_t dfu_add_service(void *pFunc);

#ifdef __cplusplus
}
#endif

#endif
