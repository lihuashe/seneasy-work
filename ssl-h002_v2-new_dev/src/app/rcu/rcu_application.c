/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rcu_application.c
* @brief     rcu application implementation
* @details   including callbacks of handle functions for users to implement customization requirements
* @author    barry_bian
* @date      2020-02-24
* @version   v1.0
* *********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <trace.h>
#include <string.h>
#include <gap.h>
#include <gap_adv.h>
#include <gap_bond_le.h>
#include "rcu_gap.h"
#include <profile_server.h>
#include <gap_msg.h>
#include "board.h"
#include <app_task.h>
#include <app_msg.h>
#include <rcu_application.h>
#include "hids_rmc.h"
#include "bas.h"
#include "dis.h"
#include "key_handle.h"
#include "os_timer.h"
#include "swtimer.h"
#include "gap_conn_le.h"
#include "voice_service.h"
#include "vendor_service.h"
#include "ota_service.h"
#include "rtl876x_wdg.h"
#include "rtl876x_aon_wdg.h"
#include "dfu_api.h"
#include "mp_test.h"
#include "hids_rmc.h"
#include "dfu_service.h"
#include "dfu_flash.h"
#include "patch_header_check.h"
#include "gatt_builtin_services.h"
#include "led_config.h"
#include "battery_driver.h"
#include "app_section.h"
#include "mem_types.h"
#include "voltage_driver.h"
#include "production_test.h"
#include "key_handle.h"
#include "ftl.h"
#if SUPPORT_IR_TX_FEATURE
#include "ir_send_handle.h"
#if SUPPORT_IR_LEARN_FEATURE
#include "ir_learn_handle.h"
#endif
#endif
#if FEATURE_SUPPORT_MP_TEST_MODE
#include "data_uart_test.h"
#endif
#if (VOICE_FLOW_SEL == ATV_GOOGLE_VOICE_FLOW)
#include "atvv_service.h"
#elif (VOICE_FLOW_SEL == RTK_GATT_VOICE_FLOW)
#include "voice_service.h"
#endif
#if SUPPORT_SW_EQ
#include "sw_equalizer.h"
#endif

/*============================================================================*
 *                              Functions Declaration
 *============================================================================*/
void app_handle_io_msg(T_IO_MSG io_driver_msg_recv) DATA_RAM_FUNCTION;

/*============================================================================*
 *                              Global Variables
 *============================================================================*/
bool search_device_flag = 0;    //寻找遥控器功能开启标志位
T_APP_GLOBAL_DATA app_global_data;
#if FEATURE_SUPPORT_PRIVACY
T_PRIVACY_STATE app_privacy_state = PRIVACY_STATE_INIT;
T_PRIVACY_ADDR_RESOLUTION_STATE app_privacy_resolution_state = PRIVACY_ADDR_RESOLUTION_DISABLED;
#endif
/*============================================================================*
 *                              External Variables
 *============================================================================*/

/*============================================================================*
 *                              Loacal Functions
 *============================================================================*/
/******************************************************************
 * @brief   handler stop adverting reason
 * @param   T_STOP_ADV_REASON reason.
 * @return  none
 * @retval  void
 */
static void app_stop_adv_reason_handler(T_STOP_ADV_REASON reason)
{
    switch (reason)
    {
    case STOP_ADV_REASON_POWERKEY:
        APP_PRINT_INFO0("STOP_ADV_REASON_POWERKEY!");
        rcu_start_adv(ADV_UNDIRECT_POWER);
        break;
    case STOP_ADV_REASON_PAIRING:
        APP_PRINT_INFO0("STOP_ADV_REASON_PAIRING!");
        rcu_start_adv(ADV_UNDIRECT_PAIRING);
        break;
    case STOP_ADV_REASON_TIMEOUT:
        APP_PRINT_INFO0("STOP_ADV_REASON_TIMEOUT!");
        break;
    case STOP_ADV_REASON_LOWPOWER:
        APP_PRINT_INFO0("STOP_ADV_REASON_LOWPOWER!");
        app_global_data.rcu_status = RCU_STATUS_LOW_POWER;
        break;
    case STOP_ADV_REASON_UART_CMD:
        APP_PRINT_INFO0("STOP_ADV_REASON_UART_CMD!");
        break;
    case STOP_ADV_REASON_IDLE:
        APP_PRINT_INFO0("STOP_ADV_REASON_IDLE!");
        if (app_global_data.adv_type == ADV_DIRECT_HDC)
        {
            APP_PRINT_INFO1("[STOP_ADV_REASON_IDLE] retry cnt is %d", app_global_data.direct_adv_cnt);
            if (app_global_data.direct_adv_cnt < MAX_DIRECT_ADV_RETRY_CNT)
            {
                rcu_start_adv(ADV_DIRECT_HDC);
                app_global_data.direct_adv_cnt++;
                return;
            }
            else
            {
                app_global_data.direct_adv_cnt = 0;  /* reset direct_adv_cnt to 0 */

//                 if (key_handle_global_data.pending_keyscan_fifo_data.len > 0)
//                 {
//                     /* other keys */
// #if FEATURE_SUPPORT_UNDIRECT_PROMPT_ADV
//                     rcu_start_adv(ADV_UNDIRECT_PROMPT);
// #endif
//                 }
            }
        }
        break;
    default:
        APP_PRINT_WARN1("Unknown stop_adv_reason: %d", app_global_data.stop_adv_reason);
        break;
    }

    //key_handle_reset_pending_keyscan_fifo_data();
}

/******************************************************************
 * @brief   handler disconnect reason
 * @param   T_DISCONN_REASON reason.
 * @return  none
 * @retval  void
 */
static void app_disconn_reason_handler(T_DISCONN_REASON reason)
{
    switch (reason)
    {
    case DISCONN_REASON_PAIRING:
        APP_PRINT_INFO0("[GAP_CONN_STATE_DISCONNECTED] DISCONN_REASON_PAIRING");
        rcu_start_adv(ADV_UNDIRECT_PAIRING);
        break;
    case DISCONN_REASON_TIMEOUT:
        APP_PRINT_INFO0("[GAP_CONN_STATE_DISCONNECTED] DISCONN_REASON_TIMEOUT");
        break;
    case DISCONN_REASON_PAIR_FAILED:
        APP_PRINT_INFO1("[GAP_CONN_STATE_DISCONNECTED] is_link_key_existed = %d",
                        app_global_data.is_link_key_existed);
        if (false == app_global_data.is_link_key_existed)
        {
            APP_PRINT_INFO1("[GAP_CONN_STATE_DISCONNECTED] DISCONN_REASON_PAIR_FAILED, retry cnt is %d",
                            app_global_data.pair_failed_retry_cnt);
            if (app_global_data.pair_failed_retry_cnt < MAX_PAIR_FAILED_RETRY_CNT)
            {
                app_global_data.pair_failed_retry_cnt++;
                rcu_start_adv(ADV_UNDIRECT_PAIRING);
            }
        }
        break;
    case DISCONN_REASON_LOW_POWER:
        APP_PRINT_INFO0("[GAP_CONN_STATE_DISCONNECTED] DISCONN_REASON_LOW_POWER");
        app_global_data.rcu_status = RCU_STATUS_LOW_POWER;
        break;
    case DISCONN_REASON_UART_CMD:
        APP_PRINT_INFO0("[GAP_CONN_STATE_DISCONNECTED] DISCONN_REASON_UART_CMD");
        break;
#if SUPPORT_NORMAL_OTA
    case DISCONN_REASON_OTA:
        APP_PRINT_INFO0("[GAP_CONN_STATE_DISCONNECTED] DISCONN_REASON_OTA");
        /* switch to OTA and reboot */
        dfu_switch_to_ota_mode();
        break;
#endif
#if SUPPORT_SILENT_OTA
    case DISCONN_REASON_SILENT_OTA:
        dfu_fw_reboot(true);
        break;
#endif
    default:
        APP_PRINT_WARN1("[GAP_CONN_STATE_DISCONNECTED] unknown reason: %d", app_global_data.disconn_reason);
        break;
    }
}

/******************************************************************
 * @fn        app_general_srv_cb
 * @brief     General service callbacks are handled in this function.
 * @param     p_data  - pointer to callback data
 * @return    cb_result
 * @retval    T_APP_RESULT
 */
static T_APP_RESULT app_general_srv_cb(T_SERVER_APP_CB_DATA *p_data)
{
    T_APP_RESULT cb_result = APP_RESULT_SUCCESS;

    switch (p_data->eventId)
    {
    case PROFILE_EVT_SRV_REG_COMPLETE:
        APP_PRINT_INFO1("PROFILE_EVT_SRV_REG_COMPLETE: result %d",
                        p_data->event_data.service_reg_result);
        break;

    case PROFILE_EVT_SEND_DATA_COMPLETE:
        APP_PRINT_INFO5("PROFILE_EVT_SEND_DATA_COMPLETE: conn_id %d, cause 0x%x, service_id %d, attrib_idx 0x%x, credits = %d",
                        p_data->event_data.send_data_result.conn_id,
                        p_data->event_data.send_data_result.cause,
                        p_data->event_data.send_data_result.service_id,
                        p_data->event_data.send_data_result.attrib_idx,
                        p_data->event_data.send_data_result.credits);
        if (p_data->event_data.send_data_result.cause == GAP_SUCCESS)
        {
            APP_PRINT_INFO0("PROFILE_EVT_SEND_DATA_COMPLETE success");
#if SUPPORT_VOICE_FEATURE
            if (voice_driver_global_data.is_voice_driver_working)
            {
                voice_handle_messages(VOICE_MSG_BT_SEND_COMPLETE, NULL);
            }
#endif
        }
        else
        {
            APP_PRINT_ERROR0("PROFILE_EVT_SEND_DATA_COMPLETE failed");
        }
        break;

    default:
        break;
    }

    return cb_result;
}

/******************************************************************
 * @fn       app_bas_srv_cb
 * @brief    BAS service callbacks are handled in this function.
 * @param    p_data  - pointer to callback data
 * @return   cb_result
 * @retval   T_APP_RESULT
 */
static T_APP_RESULT app_bas_srv_cb(T_BAS_CALLBACK_DATA *p_data)
{
    T_APP_RESULT cb_result = APP_RESULT_SUCCESS;

    switch (p_data->msg_type)
    {
    case SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION:
        {
            if (p_data->msg_data.notification_indification_index == BAS_NOTIFY_BATTERY_LEVEL_ENABLE)
            {
                APP_PRINT_INFO0("[app_bas_srv_cb] Battery level notification enable");
            }
            else if (p_data->msg_data.notification_indification_index ==
                     BAS_NOTIFY_BATTERY_LEVEL_DISABLE)
            {
                APP_PRINT_INFO0("[app_bas_srv_cb] Battery level notification disable");
            }
        }
        break;
    case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
        {
            uint8_t bat_level = 100;
#if SUPPORT_BAT_DETECT_FEATURE
            if (BAT_STATUS_SUCCESS == bat_update_battery_info())
            {
                bat_level = bat_get_current_voltage_level();
            }
            else
            {
                cb_result = APP_RESULT_APP_ERR;
            }
#endif
            bas_set_parameter(BAS_PARAM_BATTERY_LEVEL, 1, &bat_level);
        }
        break;
    default:
        break;
    }

    return cb_result;
}

/******************************************************************
 * @fn       app_dis_srv_cb
 * @brief    DIS service callbacks are handled in this function.
 * @param    p_data  - pointer to callback data
 * @return   cb_result
 * @retval   T_APP_RESULT
 */
static T_APP_RESULT app_dis_srv_cb(T_DIS_CALLBACK_DATA *p_data)
{
    T_APP_RESULT cb_result = APP_RESULT_SUCCESS;

    switch (p_data->msg_type)
    {
    case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
        {
            if (p_data->msg_data.read_value_index == DIS_READ_MANU_NAME_INDEX)
            {
                const uint8_t DISManufacturerName[] = "Seneasy";
                dis_set_parameter(DIS_PARAM_MANUFACTURER_NAME,
                                  sizeof(DISManufacturerName) - 1,
                                  (void *)DISManufacturerName);
            }
            else if (p_data->msg_data.read_value_index == DIS_READ_MODEL_NUM_INDEX)
            {
                const uint8_t DISModelNumber[] = "RTL8762EJF-H002-6568";
                dis_set_parameter(DIS_PARAM_MODEL_NUMBER,
                                  sizeof(DISModelNumber) - 1,
                                  (void *)DISModelNumber);
            }
            else if (p_data->msg_data.read_value_index == DIS_READ_SERIAL_NUM_INDEX)
            {
                uint8_t save_sn[34];
                ftl_load(save_sn, FTL_DEVICE_SN_ADDR, FTL_DEVICE_SN_LEN);
                
                if (save_sn[0] <= 16 || save_sn[0] >= 30)
                {
                    memset(save_sn, 0, sizeof(save_sn));
                    save_sn[0] = 24;
                }
                
                dis_set_parameter(DIS_PARAM_SERIAL_NUMBER,
                                  save_sn[0] - 1,
                                  (void *)&save_sn[2]);
            }
            else if (p_data->msg_data.read_value_index == DIS_READ_HARDWARE_REV_INDEX)
            {
                const uint8_t DISHardwareRev[] = "H002A4751 & H002B4752";
                dis_set_parameter(DIS_PARAM_HARDWARE_REVISION,
                                  sizeof(DISHardwareRev) - 1,
                                  (void *)DISHardwareRev);
            }
            else if (p_data->msg_data.read_value_index == DIS_READ_FIRMWARE_REV_INDEX)
            {
                const uint8_t DISFirmwareRev[] = FIRMWARE_STR_VERSION;
                dis_set_parameter(DIS_PARAM_FIRMWARE_REVISION,
                                  sizeof(DISFirmwareRev) - 1,
                                  (void *)DISFirmwareRev);
            }
            else if (p_data->msg_data.read_value_index == DIS_READ_SOFTWARE_REV_INDEX)
            {
                const uint8_t DISSoftwareRev[] = FIRMWARE_STR_VERSION;
                dis_set_parameter(DIS_PARAM_SOFTWARE_REVISION,
                                  sizeof(DISSoftwareRev) - 1,
                                  (void *)DISSoftwareRev);
            }
            else if (p_data->msg_data.read_value_index == DIS_READ_SYSTEM_ID_INDEX)
            {
                const uint8_t DISSystemID[DIS_SYSTEM_ID_LENGTH] = {0, 1, 2, 0, 0, 3, 4, 5};
                dis_set_parameter(DIS_PARAM_SYSTEM_ID,
                                  sizeof(DISSystemID),
                                  (void *)DISSystemID);
            }
            else if (p_data->msg_data.read_value_index == DIS_READ_IEEE_CERT_STR_INDEX)
            {
                const uint8_t DISIEEEDataList[] = "RTKBeeIEEEDatalist";
                dis_set_parameter(DIS_PARAM_IEEE_DATA_LIST,
                                  sizeof(DISIEEEDataList) - 1,
                                  (void *)DISIEEEDataList);
            }
            else if (p_data->msg_data.read_value_index == DIS_READ_PNP_ID_INDEX)
            {
                /* TODO: need to conform this info with Androind team */
                uint16_t version = 0x01; //VERSION_BUILD;

                uint8_t DISPnpID[DIS_PNP_ID_LENGTH] = {0x01, (uint8_t)VID, (uint8_t)(VID >> 8), (uint8_t)PID, (uint8_t)(PID >> 8), (uint8_t)version, (uint8_t)(version >> 8)};

                dis_set_parameter(DIS_PARAM_PNP_ID,
                                  sizeof(DISPnpID),
                                  DISPnpID);
            }
        }
        break;
    default:
        break;
    }

    return cb_result;
}

/******************************************************************
 * @fn       app_hid_srv_cb
 * @brief    HID service callbacks are handled in this function.
 * @param    p_data  - pointer to callback data
 * @return   cb_result
 * @retval   T_APP_RESULT
 */
static T_APP_RESULT app_hid_srv_cb(T_HID_CALLBACK_DATA *p_data)
{
    T_APP_RESULT cb_result = APP_RESULT_SUCCESS;

    APP_PRINT_INFO1("[app_hid_srv_cb] msg_type %d", p_data->msg_type);
    switch (p_data->msg_type)
    {
    case SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION:
        {
            switch (p_data->msg_data.notification_indification_index)
            {
            case HID_NOTIFY_INDICATE_KB_ENABLE:
                APP_PRINT_INFO0("[app_hid_srv_cb] HID_NOTIFY_INDICATE_KB_ENABLE");
                break;
            case HID_NOTIFY_INDICATE_KB_DISABLE:
                APP_PRINT_INFO0("[app_hid_srv_cb] HID_NOTIFY_INDICATE_KB_DISABLE");
                break;
#if SUPPORT_VOICE_FEATURE
            case HID_NOTIFY_INDICATE_VOICE_ENABLE:
                APP_PRINT_INFO0("[app_hid_srv_cb] HID_NOTIFY_INDICATE_VOICE_ENABLE");
                break;
            case HID_NOTIFY_INDICATE_VOICE_DISABLE:
                APP_PRINT_INFO0("[app_hid_srv_cb] HID_NOTIFY_INDICATE_VOICE_DISABLE");
                break;
#endif
#if FEATURE_SUPPORT_MULTIMEDIA_KEYBOARD
            case HID_NOTIFY_INDICATE_MM_KB_ENABLE:
                APP_PRINT_INFO0("[app_hid_srv_cb] HID_NOTIFY_INDICATE_MM_KB_ENABLE");
                break;
            case HID_NOTIFY_INDICATE_MM_KB_DISABLE:
                APP_PRINT_INFO0("[app_hid_srv_cb] HID_NOTIFY_INDICATE_MM_KB_DISABLE");
                break;
#endif
            default:
                break;
            }
        }
        break;

    case SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE:
        {
            APP_PRINT_INFO0("SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE\n");
#if SUPPORT_VOICE_FEATURE
            if (p_data->msg_data.write.write_type == HID_WRITE_VIOCE_CMD)
            {
                uint8_t OutCmd = p_data->msg_data.write.write_parameter.voice_enable;
                voice_handle_messages(VOICE_MSG_BT_WRITE_CMD, &OutCmd);
            }
#endif
        }
        break;
    default:
        break;
    }

    return cb_result;
}

/******************************************************************
 * @fn       app_vendor_srv_cb
 * @brief    Vendor service callbacks are handled in this function.
 * @param    p_data  - pointer to callback data
 * @return   cb_result
 * @retval   T_APP_RESULT
 */
static T_APP_RESULT app_vendor_srv_cb(T_VENDOR_CALLBACK_DATA *p_data)
{
    T_APP_RESULT cb_result = APP_RESULT_SUCCESS;

    APP_PRINT_INFO1("[app_vendor_srv_cb] msg_type %d", p_data->msg_type);
    switch (p_data->msg_type)
    {
    case SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION:
        {
            if (p_data->msg_data.notification_indification_index == VENDOR_NOTIFY_ENABLE)
            {
                APP_PRINT_INFO0("[app_vendor_srv_cb] VENDOR_NOTIFY_ENABLE");
                server_send_data(0, app_global_data.vendor_srv_id,
                                 BLE_SERVICE_CHAR_VENDOR_HANDSHAKE_INDEX,
                                 (uint8_t *)vendor_svc_handshake_values,
                                 16,
                                 GATT_PDU_TYPE_NOTIFICATION);
            }
            else if (p_data->msg_data.notification_indification_index == VENDOR_NOTIFY_DISABLE)
            {
                APP_PRINT_INFO0("[app_vendor_srv_cb] VENDOR_NOTIFY_DISABLE");
            }
        }
        break;
    case SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE:
        {
            if (p_data->msg_data.write_msg.write_type == VENDOR_WRITE_HANDSHAKE)
            {
                APP_PRINT_INFO0("[app_vendor_srv_cb] SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE");
                memcpy(vendor_svc_handshake_values, p_data->msg_data.write_msg.write_parameter.report_data.report,
                       16);
                if (!BTIF_VendorGetResponse(vendor_svc_handshake_values, 16))
                {
                    APP_PRINT_WARN0("[app_vendor_srv_cb] blueAPI_HandShakeGetResponse failed!");
                    cb_result = APP_RESULT_APP_ERR;
                }
            }
#if FEATURE_SUPPORT_MP_TEST_MODE
            else if (p_data->msg_data.write_msg.write_type == VENDOR_WRITE_TEST_MODE)
            {
                mp_test_clear_bond_info();

                if (mp_test_disable_test_mode_flag())
                {
                    APP_PRINT_INFO0("[app_vendor_srv_cb] disable test mode flag success!");

                    /* notify 8-0x00 HID key notification */
                    uint8_t notify_buffer[8];
                    memset(notify_buffer, 0, sizeof(notify_buffer));
                    server_send_data(0, app_global_data.hid_srv_id, GATT_SRV_HID_KB_INPUT_INDEX,
                                     notify_buffer, sizeof(notify_buffer), GATT_PDU_TYPE_NOTIFICATION);
                }
                else
                {
                    APP_PRINT_WARN0("[app_vendor_srv_cb] disable test mode flag failed!");
                    cb_result = APP_RESULT_APP_ERR;
                }
            }
#endif
            else
            {
                cb_result = APP_RESULT_APP_ERR;
            }
        }
        break;
    default:
        break;
    }

    return cb_result;
}

/******************************************************************
 * @fn       app_ota_srv_cb
 * @brief    OTA service callbacks are handled in this function.
 * @param    p_data  - pointer to callback data
 * @return   cb_result
 * @retval   T_APP_RESULT
 */
static T_APP_RESULT app_ota_srv_cb(T_OTA_CALLBACK_DATA *p_data)
{
    T_APP_RESULT cb_result = APP_RESULT_SUCCESS;

    switch (p_data->msg_type)
    {
    case SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE:
        if (OTA_WRITE_CHAR_VAL == p_data->msg_data.write.opcode &&
            OTA_VALUE_ENTER == p_data->msg_data.write.u.value)
        {
#if SUPPORT_NORMAL_OTA
#if SUPPORT_BAT_DETECT_FEATURE
            bat_update_battery_info();
            uint16_t bat_value = bat_get_current_voltage_value();
            if (bat_value < BAT_ENTER_OTA_MODE_THRESHOLD)
            {
                APP_PRINT_INFO1("[app_ota_srv_cb] Cannot switch into OTA mode, battery voltage is too low %d",
                                bat_value);
                cb_result = APP_RESULT_APP_ERR;
            }
            else    
#endif
            {
                APP_PRINT_INFO0("[app_ota_srv_cb] Preparing switch into OTA mode");
                /* disconnect and prepare to enter OTA mode */
                //rcu_terminate_connection(DISCONN_REASON_OTA);
            }
#endif
        }
#if FEATURE_SUPPORT_MP_TEST_MODE
        else if (OTA_WRITE_TEST_MODE_CHAR_VAL == p_data->msg_data.write.opcode)
        {
            APP_PRINT_INFO1("[MP] Test mode clear operation action: (%d)\n",
                            p_data->msg_data.write.u.value);
            if (true == mp_test_clear_bond_info())
            {
                APP_PRINT_INFO0("[MP] Exit MP test mode.");
                uint8_t exitTestModeData[8] =  {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
                exitTestModeData[2] = 0xFF;
                server_send_data(0, app_global_data.hid_srv_id, GATT_SRV_HID_KB_INPUT_INDEX,
                                 exitTestModeData, sizeof(exitTestModeData), GATT_PDU_TYPE_NOTIFICATION);
            }
        }
#endif
        break;

    default:
        break;
    }

    return cb_result;
}

/******************************************************************
 * @fn       app_dfu_srv_cb
 * @brief    DFU service callbacks are handled in this function.
 * @param    p_data  - pointer to callback data
 * @return   cb_result
 * @retval   T_APP_RESULT
 */
#if SUPPORT_SILENT_OTA
static T_APP_RESULT app_dfu_srv_cb(T_DFU_CALLBACK_DATA *p_data)
{
    T_APP_RESULT cb_result = APP_RESULT_SUCCESS;

    switch (p_data->msg_type)
    {
    case SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE:
        {
            if (p_data->msg_data.write.opcode == DFU_WRITE_START)
            {
                if (p_data->msg_data.write.write_attrib_index == INDEX_DFU_CONTROL_POINT_CHAR_VALUE)
                {
#if SUPPORT_BAT_DETECT_FEATURE
                    /* update battery after stack ready */
                    bat_update_battery_info();
#endif
                    app_set_latency_status(LATENCY_DFU_PROC_BIT, LANTENCY_OFF);
                }
            }
            else if (p_data->msg_data.write.opcode == DFU_WRITE_ATTR_EXIT)
            {
                if (p_data->msg_data.write.write_attrib_index == INDEX_DFU_CONTROL_POINT_CHAR_VALUE)
                {
                    uint8_t control_point_opcode = *p_data->msg_data.write.p_value;
                    switch (control_point_opcode)
                    {
                    case DFU_OPCODE_VALID_FW:
                        {
                            T_IO_MSG dfu_valid_fw_msg;
                            dfu_valid_fw_msg.type = IO_MSG_TYPE_DFU_VALID_FW;
                            dfu_valid_fw_msg.u.param = p_data->conn_id;
                            if (app_send_msg_to_apptask(&dfu_valid_fw_msg) == false)
                            {
                                DBG_DIRECT("DFU send Valid FW msg fail!");
                            }
                        }
                        break;
                    case DFU_OPCODE_ACTIVE_IMAGE_RESET:
                        {
                            rcu_terminate_connection(DISCONN_REASON_SILENT_OTA);
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
            else if (p_data->msg_data.write.opcode == DFU_WRITE_ATTR_ENTER)
            {
#if FEATURE_SUPPORT_NO_ACTION_DISCONN
                os_timer_restart(&no_act_disconn_timer, NO_ACTION_DISCON_TIMEOUT);
#endif
                /*check rcu state*/
                uint8_t notif_data[9] = {0};
                uint8_t control_point_opcode = *p_data->msg_data.write.p_value;
                if (RCU_STATUS_PAIRED != app_global_data.rcu_status)
                {
                    APP_PRINT_INFO0("[app_dfu_srv_cb] err: rcu is not paired.");

                    notif_data[0] = DFU_OPCODE_NOTIFICATION;
                    notif_data[1] = control_point_opcode;
                    notif_data[2] = DFU_ARV_FAIL_SYS_VERSION_ERROR;
                    server_send_data(0, app_global_data.dfu_srv_id, INDEX_DFU_CONTROL_POINT_CHAR_VALUE, \
                                     notif_data, 3, GATT_PDU_TYPE_NOTIFICATION);
                    cb_result = APP_RESULT_REJECT;
                }
            }
        }
        break;
    case SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION:
        {
            if (p_data->msg_data.notification_indification_index == DFU_NOTIFY_ENABLE)
            {
                APP_PRINT_INFO0("dfu notification enable");
            }
            else if (p_data->msg_data.notification_indification_index ==
                     DFU_NOTIFY_DISABLE)
            {
                APP_PRINT_INFO0("dfu notification disable");
            }
        }
        break;
    default:
        break;

    }
    return cb_result;
}
#endif

#if (SUPPORT_VOICE_FEATURE  && (VOICE_FLOW_SEL == ATV_GOOGLE_VOICE_FLOW))
/******************************************************************
 * @fn       app_atvv_srv_cb
 * @brief    ATV service callbacks are handled in this function.
 * @param    p_data  - pointer to callback data
 * @return   cb_result
 * @retval   T_APP_RESULT
 */
static T_APP_RESULT app_atvv_srv_cb(T_ATVV_CALLBACK_DATA *p_data)
{
    bool notify_result = false;
    T_APP_RESULT cb_result = APP_RESULT_SUCCESS;

    switch (p_data->msg_type)
    {
    case SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION:
        {
            /* add user code here */
            switch (p_data->msg_data.notification_indification_index)
            {
            case ATVV_CHAR_RX_NOTIFY_ENABLE:
                APP_PRINT_INFO0("[app_atvv_srv_cb] ATVV_CHAR_RX_NOTIFY_ENABLE");
                app_global_data.is_atv_voice_notify_en = true;
                break;
            case ATVV_CHAR_RX_NOTIFY_DISABLE:
                APP_PRINT_INFO0("[app_atvv_srv_cb] ATVV_CHAR_RX_NOTIFY_DISABLE");
                app_global_data.is_atv_voice_notify_en = false;
                if (ATVV_VERSION_1_0 == atvv_global_data.app_support_version)
                {
                    if (voice_driver_global_data.is_voice_driver_working == true)
                    {
                        voice_handle_atvv_audio_stop(ATV_STOP_REASON_ATVV_CHAR_AUDIO_DISABLE);
                    }
                }
                break;
            default:
                break;
            }
        }
        break;

    case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
        {
            /* add user code here */
        }
        break;

    case SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE:
        {
            uint8_t report_len = p_data->msg_data.write.write_parameter.report_data.len;
            uint8_t *report_data_ptr = p_data->msg_data.write.write_parameter.report_data.report;

            APP_PRINT_INFO1("[app_atvv_srv_cb] SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE report_len = %d",
                            report_len);

            if ((report_len > 0) && (report_len <= ATVV_CHAR_WRITE_DATA_LEN))
            {
                APP_PRINT_INFO1("[app_atvv_srv_cb] report data = %b", TRACE_BINARY(report_len, report_data_ptr));

                switch (report_data_ptr[0])
                {
                case ATV_TX_OPCODE_GET_CAPS:
                    {
                        /* parse recieved data */
                        uint8_t parsed_version_major = report_data_ptr[1];
                        uint8_t parsed_version_minor = report_data_ptr[2];
                        uint16_t parsed_codecs = ((uint16_t)report_data_ptr[3] << 8) + report_data_ptr[4];

                        APP_PRINT_INFO3("[app_atvv_srv_cb] ATV_TX_OPCODE_GET_CAPS V%d.%d codec = %d",
                                        parsed_version_major, parsed_version_minor, parsed_codecs);

                        /* choose atv version */
                        uint8_t app_support_major_version = (uint8_t)(atvv_global_data.app_support_version >> 8);
                        if (app_support_major_version > parsed_version_major)
                        {
                            app_support_major_version = parsed_version_major;
                            if (app_support_major_version == 0)
                            {
                                atvv_global_data.app_support_version = ATVV_VERSION_0_4;
                            }
                            else
                            {
                                atvv_global_data.app_support_version = ATVV_VERSION_1_0;
                            }
                        }
                        APP_PRINT_INFO3("[app_atvv_srv_cb] APP use ATV version V%d.%d codec = %d",
                                        (uint8_t)(atvv_global_data.app_support_version >> 8), (uint8_t)atvv_global_data.app_support_version,
                                        atvv_global_data.codec_support);

                        /* send response */
                        if (ATVV_VERSION_1_0 == atvv_global_data.app_support_version)
                        {
#if (ATV_VOICE_INTERACTION_MODEL == ATV_INTERACTION_MODEL_ON_REQUEST)
                            atvv_global_data.assistant_interaction_model = ATV_ASSISTANT_INTERACTION_MODEL_ON_REQUEST;
#elif (ATV_VOICE_INTERACTION_MODEL == ATV_INTERACTION_MODEL_PRESS_TO_TALK)
                            atvv_global_data.assistant_interaction_model = ATV_ASSISTANT_INTERACTION_MODEL_PRESS_TO_TALK;
#elif (ATV_VOICE_INTERACTION_MODEL == ATV_INTERACTION_MODEL_HOLD_TO_TALK)
                            atvv_global_data.assistant_interaction_model = ATV_ASSISTANT_INTERACTION_MODEL_HOLD_TO_TALK;
#endif

                            atvv_global_data.codec_used = ATTV_CODEC_MASK_16K_ADPCM;
                            uint8_t mast_support_assistant_interaction_model = report_data_ptr[5];
                            uint16_t audio_frame_size = VOICE_FRAME_SIZE_AFTER_ENC * VOICE_PCM_FRAME_CNT;

                            if (atvv_global_data.assistant_interaction_model > mast_support_assistant_interaction_model)
                            {
                                atvv_global_data.assistant_interaction_model = mast_support_assistant_interaction_model;
                            }
                            APP_PRINT_INFO2("[app_atvv_srv_cb] assistant_interaction_model-- master support: %d, rcu use: %d",
                                            mast_support_assistant_interaction_model, atvv_global_data.assistant_interaction_model);

                            memset(atvv_global_data.char_ctl_data_buff, 0, ATVV_CHAR_CTL_DATA_LEN);
                            atvv_global_data.char_ctl_data_buff[0] = ATV_CTL_OPCODE_GET_CAPS_RESP;
                            atvv_global_data.char_ctl_data_buff[1] = (uint8_t)(atvv_global_data.app_support_version >> 8);
                            atvv_global_data.char_ctl_data_buff[2] = (uint8_t)atvv_global_data.app_support_version;
                            atvv_global_data.char_ctl_data_buff[3] = (uint8_t)
                                                                     atvv_global_data.codec_support; /* codecs supported */
                            atvv_global_data.char_ctl_data_buff[4] =
                                atvv_global_data.assistant_interaction_model; /*assistant interaction model*/
                            atvv_global_data.char_ctl_data_buff[5] = (uint8_t)(audio_frame_size >> 8);
                            atvv_global_data.char_ctl_data_buff[6] = (uint8_t) audio_frame_size;  /* bytes/audio frame size*/
                            atvv_global_data.char_ctl_data_buff[7] = ATVV_DLE_AND_MTU_UPDATE_DISABLE; /* ectra configuration */
                            atvv_global_data.char_ctl_data_buff[8] = 0x00;  /* reserved*/
                        }
                        else if (ATVV_VERSION_0_4 == atvv_global_data.app_support_version)
                        {
                            atvv_global_data.assistant_interaction_model = ATV_ASSISTANT_INTERACTION_MODEL_ON_REQUEST;

                            memset(atvv_global_data.char_ctl_data_buff, 0, ATVV_CHAR_CTL_DATA_LEN);
                            atvv_global_data.char_ctl_data_buff[0] = ATV_CTL_OPCODE_GET_CAPS_RESP;
                            atvv_global_data.char_ctl_data_buff[1] = (uint8_t)(atvv_global_data.app_support_version >> 8);
                            atvv_global_data.char_ctl_data_buff[2] = (uint8_t)atvv_global_data.app_support_version;
                            atvv_global_data.char_ctl_data_buff[3] = (uint8_t)(atvv_global_data.codec_support >> 8);
                            atvv_global_data.char_ctl_data_buff[4] = (uint8_t)
                                                                     atvv_global_data.codec_support;  /* codec_support */
                            atvv_global_data.char_ctl_data_buff[5] = 0x00;
                            atvv_global_data.char_ctl_data_buff[6] = (uint8_t)VOICE_REPORT_FRAME_SIZE;  /* bytes/frame */
                            atvv_global_data.char_ctl_data_buff[7] = 0x00;
                            atvv_global_data.char_ctl_data_buff[8] = 0x14;  /* bytes/char */
                        }

                        notify_result = server_send_data(0, app_global_data.atvv_srv_id, GATT_SVC_ATVV_CHAR_CTL_VALUE_INDEX,
                                                         atvv_global_data.char_ctl_data_buff, 9, GATT_PDU_TYPE_NOTIFICATION);
                        if (notify_result == false)
                        {
                            APP_PRINT_WARN0("[app_atvv_srv_cb] ATV_TX_OPCODE_GET_CAPS notify failed!");
                        }
                    }
                    break;

                case ATV_TX_OPCODE_MIC_OPEN:
                    {
                        APP_PRINT_INFO0("[app_atvv_srv_cb] ATV_TX_OPCODE_MIC_OPEN");
                        if (ATVV_VERSION_1_0 == atvv_global_data.app_support_version)
                        {
                            if ((atvv_global_data.atv_start_reason != ATV_START_REASON_PTT)
                                && (atvv_global_data.atv_start_reason != ATV_START_REASON_HTT))
                            {
                                if (app_global_data.is_atv_voice_notify_en == true)
                                {
                                    atvv_global_data.audio_consumption_mode = report_data_ptr[1];
                                    if (voice_driver_global_data.is_voice_driver_working == true)
                                    {
                                        voice_handle_atvv_audio_stop(ATV_STOP_REASON_AUDIO_START_COMMAND);
                                    }
                                    voice_handle_atvv_audio_start(ATV_START_REASON_MIC_OPEN_REQUEST);
                                }
                                else
                                {
                                    voice_handle_atvv_mic_open_error(ATV_MIC_OPEN_ERROR_ATVV_CHAR_AUDIO_IS_DISABLE);
                                }
                            }
                            else
                            {
                                APP_PRINT_WARN0("[app_atvv_srv_cb] atv assistant_interaction_model is not ATV_ASSISTANT_INTERACTION_MODEL_ON_REQUEST");
                                voice_handle_atvv_mic_open_error(ATV_MIC_OPEN_ERROR_PTT_HTT_IS_IN_PROGRESS);
                            }
                        }
                        else if (ATVV_VERSION_0_4 == atvv_global_data.app_support_version)
                        {
                            uint16_t parsed_codecs = ((uint16_t)report_data_ptr[1] << 8) + report_data_ptr[2];
                            APP_PRINT_INFO1("[app_atvv_srv_cb] ATV_TX_OPCODE_MIC_OPEN parsed_codecs = %d", parsed_codecs);

                            if ((parsed_codecs & atvv_global_data.codec_support) && app_global_data.is_atv_voice_notify_en)
                            {
                                atvv_global_data.codec_used = parsed_codecs;
                                if (parsed_codecs == ATTV_CODEC_MASK_8K_ADPCM)
                                {
                                    voice_driver_codec_params.codec_sample_rate = SAMPLE_RATE_8KHz;
                                }
                                else if (parsed_codecs == ATTV_CODEC_MASK_16K_ADPCM)
                                {
                                    voice_driver_codec_params.codec_sample_rate = SAMPLE_RATE_16KHz;
                                }
                                voice_handle_atvv_audio_start(ATV_START_REASON_MIC_OPEN_REQUEST);
                            }
                            else
                            {
                                voice_handle_atvv_mic_open_error(ATV_MIC_OPEN_ERROR_RESERVED);
                            }
                        }
                    }
                    break;

                case ATV_TX_OPCODE_MIC_CLOSE:
                    {
                        APP_PRINT_INFO0("[app_atvv_srv_cb] ATV_TX_OPCODE_MIC_CLOSE");
                        APP_PRINT_INFO2("[app_atvv_srv_cb] stream_id-- master: 0x%x, rcu: 0x%x", report_data_ptr[1],
                                        atvv_global_data.stream_id);
                        if (ATVV_VERSION_1_0 == atvv_global_data.app_support_version)
                        {
                            if (voice_driver_global_data.is_voice_driver_working == true)
                            {
                                if ((report_data_ptr[1] == 0xff)
                                    || ((report_data_ptr[1] == 0)
                                        && (atvv_global_data.atv_start_reason == ATV_START_REASON_MIC_OPEN_REQUEST))
                                    || ((report_data_ptr[1] == atvv_global_data.stream_id)
                                        && (atvv_global_data.atv_start_reason != ATV_START_REASON_MIC_OPEN_REQUEST)))
                                {
                                    voice_handle_atvv_audio_stop(ATV_STOP_REASON_MIC_CLOSE_MESSAGE);
                                }
                            }
                        }
                        else if (ATVV_VERSION_0_4 == atvv_global_data.app_support_version)
                        {
                            voice_handle_atvv_audio_stop(ATV_STOP_REASON_MIC_CLOSE_MESSAGE);
                        }
                    }
                    break;

                case ATV_TX_OPCODE_EXTEND:
                    {
                        APP_PRINT_INFO0("[app_atvv_srv_cb] ATV_TX_OPCODE_EXTEND");
                        APP_PRINT_INFO2("[app_atvv_srv_cb] stream_id-- master: 0x%x, rcu: 0x%x", report_data_ptr[1],
                                        atvv_global_data.stream_id);
                        if (ATVV_VERSION_1_0 == atvv_global_data.app_support_version)
                        {
                            if ((report_data_ptr[1] == 0xff || report_data_ptr[1] == atvv_global_data.stream_id) &&
                                voice_driver_global_data.is_voice_driver_working == true)
                            {
                                APP_PRINT_INFO0("[voice_handle_atvv_audio_stop] ATV_TX_OPCODE_EXTEND");
                                os_timer_restart(&voice_exception_timer, VOICE_TRANSFER_TIMEOUT);
                            }
                        }
                    }
                    break;

                default:
                    break;
                }
            }
        }
        break;

    default:
        break;
    }

    return cb_result;
}
#endif

#if (SUPPORT_VOICE_FEATURE  && (VOICE_FLOW_SEL == RTK_GATT_VOICE_FLOW))
/******************************************************************
 * @fn       app_voice_srv_cb
 * @brief    voice service callbacks are handled in this function.
 * @param    p_data  - pointer to callback data
 * @return   cb_result
 * @retval   T_APP_RESULT
 */
static T_APP_RESULT app_voice_srv_cb(T_VOICE_CALLBACK_DATA *p_data)
{
    T_APP_RESULT cb_result = APP_RESULT_SUCCESS;

    switch (p_data->msg_type)
    {
    case SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION:
        {
            /* add user code here */
        }
        break;

    case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
        {
            /* add user code here */
        }
        break;

    case SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE:
        {
            /* add user code here */
        }
        break;

    default:
        break;
    }

    return cb_result;
}
#endif

#if IS_PRINT_REMAINING_RAM_SIZE
/******************************************************************
 * @fn       app_print_remaining_ram_size
 * @brief    Print the remaining ram size.
 *
 * @param    void
 * @return   none
 * @retval   void
 */
static void app_print_remaining_ram_size(void)
{
    //uint32_t data_ram_size = 0, buffer_ram_size = 0;
    //data_ram_size = os_mem_peek(RAM_TYPE_DATA_ON);//the remainig data ram size
    //buffer_ram_size = os_mem_peek(RAM_TYPE_BUFFER_ON);//the remainig buffer ram size
    //APP_PRINT_INFO2("[app_print_remaining_ram_size] data_ram_size = %d, buffer_ram_size = %d",
    //                data_ram_size, buffer_ram_size);
}
#endif

/******************************************************************
 * @fn       peripheral_HandleBtDevStateChangeEvt
 * @brief    All the gaprole_States_t events are pre-handled in this function.
 *           Then the event handling function shall be called according to the newState.
 *
 * @param    new_state - new gap state
 * @param    uint16_t cause
 * @return   none
 * @retval   void
 */
static void periph_handle_dev_state_evt(T_GAP_DEV_STATE new_state, uint16_t cause)
{
    APP_PRINT_INFO4("periph_handle_dev_state_evt: init state %d, adv state %d, conn state %d, cause 0x%x",
                    new_state.gap_init_state, new_state.gap_adv_state,
                    new_state.gap_conn_state, cause);

#if FEATURE_SUPPORT_PRIVACY
    if ((new_state.gap_init_state == GAP_INIT_STATE_STACK_READY)
        && (new_state.gap_adv_state == GAP_ADV_STATE_IDLE)
        && (new_state.gap_conn_state == GAP_CONN_DEV_STATE_IDLE))
    {
        privacy_handle_resolv_list();
    }
#endif

    if (app_global_data.gap_dev_state.gap_init_state != new_state.gap_init_state)
    {
        if (new_state.gap_init_state == GAP_INIT_STATE_STACK_READY)
        {
            APP_PRINT_INFO0("GAP stack ready");


#if IS_PRINT_REMAINING_RAM_SIZE
            app_print_remaining_ram_size();
#endif

            /* set PPCP, otherwise may cause connection update failed case */
            gaps_set_peripheral_preferred_conn_param(RCU_CONNECT_INTERVAL, RCU_CONNECT_INTERVAL,
                                                     RCU_CONNECT_LATENCY, RCU_SUPERVISION_TIMEOUT / 10);

            app_global_data.rcu_status = RCU_STATUS_IDLE;


#if FEATURE_SUPPORT_DEEP_SLEEP_STATE
            if (false == app_global_data.wake_up_from_dss)
#endif
            {
#if SUPPORT_BAT_DETECT_FEATURE
                /* update battery after stack ready */
                
#endif
            }

#if (FEATURE_SUPPORT_REMOVE_LINK_KEY_BEFORE_PAIRING && FEATURE_SUPPORT_RECOVER_PAIR_INFO)
            set_bond_info();
#endif

            T_LE_KEY_ENTRY *p_le_key_entry;
            p_le_key_entry = le_get_high_priority_bond();
            if ((p_le_key_entry != NULL) && (p_le_key_entry->is_used))
            {
                app_global_data.is_link_key_existed = true;

                /* attempt to reconnect with master */
#if FEATURE_SUPPORT_PRIVACY
                /* The privacy module automatically manages the whitelist when the privacy_whitelist is set to false.*/
#if SUPPORT_BAT_DETECT_FEATURE
                if (BAT_MODE_LOW_POWER != bat_get_current_mode())
#endif
                {
#if FEATURE_SUPPORT_DEEP_SLEEP_STATE
                    if (false == app_global_data.wake_up_from_dss)
#endif
                    {
                        rcu_start_adv(ADV_UNDIRECT_RECONNECT);
                        os_timer_restart(&adv_timer, ADV_UNDIRECT_RECONNECT_FIRST_TIMEOUT);
                    }
                }
#else
                /* add paired device in white list */
                le_modify_white_list(GAP_WHITE_LIST_OP_ADD, p_le_key_entry->remote_bd.addr,
                                     (T_GAP_REMOTE_ADDR_TYPE)(p_le_key_entry->remote_bd.remote_bd_type));
                {
#if FEATURE_SUPPORT_DEEP_SLEEP_STATE
                    if (false == app_global_data.wake_up_from_dss)
#endif
                    {
                        app_global_data.direct_adv_cnt = MAX_DIRECT_ADV_RETRY_CNT;
                        rcu_start_adv(ADV_DIRECT_HDC);
                    }
                }
#endif
            }
            else
            {
                app_global_data.is_link_key_existed = false;
            }

            app_nvic_config();  /* enable peripheral NVIC after GAP stack ready */
        }
    }

    if (app_global_data.gap_dev_state.gap_adv_state != new_state.gap_adv_state)
    {
        if (new_state.gap_adv_state == GAP_ADV_STATE_IDLE)
        {

            /* sync rcu status to idle */
            app_global_data.rcu_status = RCU_STATUS_IDLE;
            if (app_global_data.adv_type != ADV_DIRECT_HDC)
            {
                os_timer_stop(&adv_timer);
            }

            if (new_state.gap_adv_sub_state == GAP_ADV_TO_IDLE_CAUSE_STOP)
            {
                APP_PRINT_INFO0("Advertisng is stopped by user stop or timeouot");
#if (FEATURE_SUPPORT_REMOVE_LINK_KEY_BEFORE_PAIRING && FEATURE_SUPPORT_RECOVER_PAIR_INFO)
                if (app_global_data.adv_type == ADV_UNDIRECT_PAIRING)
                {
                    set_bond_info();
                }
#endif
                /* check adv stop reason */
                app_stop_adv_reason_handler(app_global_data.stop_adv_reason);
                app_global_data.stop_adv_reason = STOP_ADV_REASON_IDLE;  /* reset stop_adv_reason */
#if FEATURE_SUPPORT_DEEP_SLEEP_STATE
                os_timer_restart(&enter_dss_timer, ENTER_DSS_TIMEOUT);
                APP_PRINT_INFO0("[periph_handle_dev_state_evt] restart enter_dss_timer");
#endif
            }
            else if (new_state.gap_adv_sub_state == GAP_ADV_TO_IDLE_CAUSE_CONN)
            {
                APP_PRINT_INFO0("Advertisng is stopped for link establishment");
            }
        }
        else if (new_state.gap_adv_state == GAP_ADV_STATE_ADVERTISING)
        {
            APP_PRINT_INFO0("GAP adv start");
            app_global_data.rcu_status = RCU_STATUS_ADVERTISING;
        }
    }

    if (app_global_data.gap_dev_state.gap_conn_state != new_state.gap_conn_state)
    {
        APP_PRINT_INFO2("conn state: %d -> %d",
                        app_global_data.gap_dev_state.gap_conn_state,
                        new_state.gap_conn_state);
    }
    app_global_data.gap_dev_state = new_state;
}

/******************************************************************
 * @fn       peripheral_HandleBtDevStateChangeEvt
 * @brief    Handle connection status change event.
 *
 * @param    uint8_t conn_id
 * @param    T_GAP_CONN_STATE new_state
 * @param    uint16_t disc_cause
 * @return   none
 * @retval   void
 */
static void periph_handle_conn_state_evt(uint8_t conn_id, T_GAP_CONN_STATE new_state,
                                         uint16_t disc_cause)
{
    APP_PRINT_INFO3("periph_handle_conn_state_evt: conn_id = %d old_state = %d new_state = %d",
                    conn_id, app_global_data.gap_conn_state, new_state);
    switch (new_state)
    {
    case GAP_CONN_STATE_DISCONNECTED:
        {
            app_global_data.rcu_status = RCU_STATUS_IDLE;
#if (SUPPORT_VOICE_FEATURE  && (VOICE_FLOW_SEL == ATV_GOOGLE_VOICE_FLOW))
            app_global_data.is_atv_voice_notify_en = false;
#endif
#if FEATURE_SUPPORT_NO_ACTION_DISCONN
            /* stop no_act_disconn_timer if disconnection detected */
            os_timer_stop(&no_act_disconn_timer);
#endif
            os_timer_stop(&next_state_check_timer);  /* ensure pair_fail_disconn_timer is stopped */

#if FEATURE_SUPPORT_DEEP_SLEEP_STATE
            os_timer_restart(&enter_dss_timer, ENTER_DSS_TIMEOUT);
            APP_PRINT_INFO0("[periph_handle_conn_state_evt] restart enter_dss_timer");
#endif

#if SUPPORT_VOICE_FEATURE
            if (voice_driver_global_data.is_voice_driver_working == true)
            {
                voice_global_data.is_allowed_to_notify_voice_data = false;
                voice_handle_stop_mic();
            }
#endif
#if (FEATURE_SUPPORT_REMOVE_LINK_KEY_BEFORE_PAIRING && FEATURE_SUPPORT_RECOVER_PAIR_INFO)
            if (disc_cause == (HCI_ERR | HCI_ERR_REMOTE_USER_TERMINATE))
            {
                APP_PRINT_INFO0("[GAP_CONN_STATE_DISCONNECTED] REMOTE_USER_TERMINATE");

                if (app_global_data.is_link_key_existed == false &&
                    app_global_data.pair_failed_retry_cnt < MAX_PAIR_FAILED_RETRY_CNT)
                {
                    app_global_data.pair_failed_retry_cnt++;
                    rcu_start_adv(ADV_UNDIRECT_PAIRING);
                }
                else
                {
                    set_bond_info();
                }
            }
            else if (disc_cause == (HCI_ERR | HCI_ERR_LOCAL_HOST_TERMINATE))
            {
                if (app_global_data.disconn_reason == DISCONN_REASON_PAIRING ||
                    (app_global_data.disconn_reason == DISCONN_REASON_PAIR_FAILED &&
                     app_global_data.pair_failed_retry_cnt < MAX_PAIR_FAILED_RETRY_CNT))
                {
                    /* do nothing */
                }
                else
                {
                    set_bond_info();
                }
                /* handle APP disconection reason */
                app_disconn_reason_handler(app_global_data.disconn_reason);
            }
            else
            {
                set_bond_info();
                APP_PRINT_WARN1("[GAP_CONN_STATE_DISCONNECTED] Connection lost: cause 0x%x", disc_cause);
            }
#else
            if (disc_cause == (HCI_ERR | HCI_ERR_REMOTE_USER_TERMINATE))
            {
                APP_PRINT_INFO0("[GAP_CONN_STATE_DISCONNECTED] REMOTE_USER_TERMINATE");

                if (app_global_data.is_link_key_existed == false)
                {
                    if (app_global_data.pair_failed_retry_cnt < MAX_PAIR_FAILED_RETRY_CNT)
                    {
                        app_global_data.pair_failed_retry_cnt++;
                        rcu_start_adv(ADV_UNDIRECT_PAIRING);
                    }
                }
            }
            else if (disc_cause == (HCI_ERR | HCI_ERR_LOCAL_HOST_TERMINATE))
            {
                /* handle APP disconection reason */
                app_disconn_reason_handler(app_global_data.disconn_reason);
            }
            else
            {
                APP_PRINT_WARN1("[GAP_CONN_STATE_DISCONNECTED] Connection lost: cause 0x%x", disc_cause);
            }
#endif

            app_global_data.disconn_reason = DISCONN_REASON_IDLE;  /* reset disconn_reason */
        }
        break;

    case GAP_CONN_STATE_CONNECTED:
        {
            app_global_data.rcu_status = RCU_STATUS_CONNECTED;
            os_timer_restart(&next_state_check_timer, PAIRING_EXCEPTION_TIMEOUT);
#if FEATURE_SUPPORT_DEEP_SLEEP_STATE
            os_timer_stop(&enter_dss_timer);
            APP_PRINT_INFO0("[periph_handle_conn_state_evt] stop enter_dss_timer");
#endif

            /* reset latency_status to default after connect */
            memset(&app_global_data.latency_status, 0, sizeof(app_global_data.latency_status));
            /* turn off slave latency to speed up pairing process, have to turn on latency later */
            app_set_latency_status(LATENCY_SYS_UPDATE_BIT, LANTENCY_OFF);
#if FEATURE_SUPPORT_DATA_LENGTH_EXTENSION
            uint16_t gap_param_data_len_tx_oct = 251;
            uint16_t gap_param_data_len_tx_time = 2120; /* tx max time: 2.12ms */
            le_set_data_len(conn_id, gap_param_data_len_tx_oct, gap_param_data_len_tx_time);
#endif
            /* print remote device information */
            uint16_t conn_interval;
            uint16_t conn_latency;
            uint16_t conn_supervision_timeout;
            uint8_t  remote_bd[6];
            uint8_t remote_bd_type;
            le_get_conn_param(GAP_PARAM_CONN_INTERVAL, &conn_interval, conn_id);
            le_get_conn_param(GAP_PARAM_CONN_LATENCY, &conn_latency, conn_id);
            le_get_conn_param(GAP_PARAM_CONN_TIMEOUT, &conn_supervision_timeout, conn_id);
            le_get_conn_addr(conn_id, remote_bd, &remote_bd_type);
            APP_PRINT_INFO5("GAP_CONN_STATE_CONNECTED:remote_bd %s, remote_addr_type %d, conn_interval 0x%x, conn_latency 0x%x, conn_supervision_timeout 0x%x",
                            TRACE_BDADDR(remote_bd), remote_bd_type,
                            conn_interval, conn_latency, conn_supervision_timeout);
        }
        break;

    default:
        break;
    }
    app_global_data.gap_conn_state = new_state;
}

/******************************************************************
 * @fn       peripheral_HandleBtGapAuthenStateChangeEvt
 * @brief    All the bonding state change  events are pre-handled in this function.
 *           Then the event handling function shall be called according to the newState.
 *
 * @param    uint8_t conn_id
 * @param    T_GAP_CONN_STATE new_state
 * @param    uint16_t cause
 * @return   none
 * @retval   void
 */

static void periph_handle_authen_state_evt(uint8_t conn_id, uint8_t new_state, uint16_t cause)
{
    APP_PRINT_INFO1("periph_handle_authen_state_evt:conn_id %d", conn_id);

    switch (new_state)
    {
    case GAP_AUTHEN_STATE_STARTED:
        {
            APP_PRINT_INFO0("GAP_AUTHEN_STATE_STARTED");
            os_timer_restart(&next_state_check_timer, PAIRING_EXCEPTION_TIMEOUT);
        }
        break;

    case GAP_AUTHEN_STATE_COMPLETE:
        {
            if (cause == GAP_CAUSE_SUCCESS)
            {
                APP_PRINT_INFO0("[GAP_AUTHEN_STATE_COMPLETE] pairing success");
                app_global_data.is_link_key_existed = true;
                app_global_data.rcu_status = RCU_STATUS_PAIRED;
                app_global_data.pair_failed_retry_cnt = 0;
                app_global_data.updt_conn_params_retry_cnt = 0;  /* reset retry cnt */
                os_timer_stop(&next_state_check_timer);
#if FEATURE_SUPPORT_NO_ACTION_DISCONN
                os_timer_restart(&no_act_disconn_timer, NO_ACTION_DISCON_TIMEOUT);
#endif

                if ((app_global_data.adv_type == ADV_DIRECT_HDC)
                    || (app_global_data.adv_type == ADV_UNDIRECT_RECONNECT))
                {
        
        
                    /* update connection parameter 4s delay */
                    os_timer_restart(&update_conn_params_timer, UPDATE_CONN_PARAMS_SHORT_TIMEOUT);
                    
                }
                else
                {
                    /* start timer to update connection parameter, postpone with timer to accelate GATT process */
                    os_timer_restart(&update_conn_params_timer, UPDATE_CONN_PARAMS_TIMEOUT);
                }
#if (FEATURE_SUPPORT_REMOVE_LINK_KEY_BEFORE_PAIRING && FEATURE_SUPPORT_RECOVER_PAIR_INFO)
                delete_last_bond_info();
#endif
            app_global_data.adv_type = ADV_IDLE;
            }
            else
            {
                if (cause == (SM_ERR | SM_ERR_KEY_SAVE_FAILED))
                {
                    le_bond_clear_all_keys();
                }

                APP_PRINT_INFO0("[GAP_AUTHEN_STATE_COMPLETE] pair failed");

                os_timer_restart(&next_state_check_timer, PAIR_FAIL_DISCONN_TIMEOUT);
            }
        }
        break;

    default:
        {
            APP_PRINT_INFO1("GAP_MSG_LE_AUTHEN_STATE_CHANGE:(unknown newstate: %d)", new_state);
        }
        break;
    }
}
/******************************************************************
 * @fn       peripheral_HandleBtGapConnParaChangeEvt
 * @brief    All the connection parameter update change  events are pre-handled in this function.
 *           Then the event handling function shall be called according to the status.
 *
 * @param    uint8_t conn_id
 * @param    uint8_t status - connection parameter result, 0 - success, otherwise fail.
 * @param    uint16_t cause
 * @return   none
 * @retval   void
 */
static void periph_conn_param_update_evt(uint8_t conn_id, uint8_t status, uint16_t cause)
{
    switch (status)
    {
    case GAP_CONN_PARAM_UPDATE_STATUS_SUCCESS:
        {
            /* reset updt_conn_params_retry_cnt */
            app_global_data.updt_conn_params_retry_cnt = 0;

            le_get_conn_param(GAP_PARAM_CONN_INTERVAL, &app_global_data.conn_interval, conn_id);
            le_get_conn_param(GAP_PARAM_CONN_LATENCY, &app_global_data.conn_latency, conn_id);
            le_get_conn_param(GAP_PARAM_CONN_TIMEOUT, &app_global_data.conn_supervision_timeout, conn_id);

            APP_PRINT_INFO3("GAP_MSG_LE_CONN_PARAM_UPDATE update success:conn_interval 0x%x, conn_slave_latency 0x%x, conn_supervision_timeout 0x%x",
                            app_global_data.conn_interval, app_global_data.conn_latency,
                            app_global_data.conn_supervision_timeout);
        }
        break;

    case GAP_CONN_PARAM_UPDATE_STATUS_FAIL:
        {
            APP_PRINT_ERROR1("GAP_MSG_LE_CONN_PARAM_UPDATE failed: cause 0x%x", cause);
            if (app_global_data.updt_conn_params_retry_cnt < MAX_UPDATE_CONN_PARAMS_RETRY_CNT)
            {
                app_global_data.updt_conn_params_retry_cnt++;
                APP_PRINT_WARN0("[rcu_update_conn_params] GAP_CONN_PARAM_UPDATE_STATUS_FAIL retry");
                os_timer_restart(&update_conn_params_timer, UPDATE_CONN_PARAMS_TIMEOUT);
            }
            else
            {
                APP_PRINT_WARN0("[rcu_update_conn_params] GAP_CONN_PARAM_UPDATE_STATUS_FAIL failed");
            }
        }
        break;

    case GAP_CONN_PARAM_UPDATE_STATUS_PENDING:
        {
            APP_PRINT_INFO0("GAP_CONN_PARAM_UPDATE_STATUS_PENDING");
        }
        break;

    default:
        break;
    }
}

/******************************************************************
 * @fn       peripheral_HandleBtGapMessage
 * @brief    All the bt gap msg  events are pre-handled in this function.
 *           Then the event handling function shall be called according to the subType
 *           of BEE_IO_MSG.
 *
 * @param    p_gap_msg  - pointer to bee io msg
 * @return   none
 * @retval   void
 */
static void periph_handle_gap_msg(T_IO_MSG *p_gap_msg)
{
    T_LE_GAP_MSG gap_msg;
    uint8_t conn_id;
    memcpy(&gap_msg, &p_gap_msg->u.param, sizeof(p_gap_msg->u.param));

    APP_PRINT_TRACE1("periph_handle_gap_msg subType = %d", p_gap_msg->subtype);
    switch (p_gap_msg->subtype)
    {
    case GAP_MSG_LE_DEV_STATE_CHANGE:
        {
            periph_handle_dev_state_evt(gap_msg.msg_data.gap_dev_state_change.new_state,
                                        gap_msg.msg_data.gap_dev_state_change.cause);
        }
        break;

    case GAP_MSG_LE_CONN_STATE_CHANGE:
        {
            periph_handle_conn_state_evt(gap_msg.msg_data.gap_conn_state_change.conn_id,
                                         (T_GAP_CONN_STATE)gap_msg.msg_data.gap_conn_state_change.new_state,
                                         gap_msg.msg_data.gap_conn_state_change.disc_cause);
        }
        break;

    case GAP_MSG_LE_AUTHEN_STATE_CHANGE:
        {
            periph_handle_authen_state_evt(gap_msg.msg_data.gap_authen_state.conn_id,
                                           gap_msg.msg_data.gap_authen_state.new_state,
                                           gap_msg.msg_data.gap_authen_state.status);
        }
        break;

    case GAP_MSG_LE_BOND_JUST_WORK:
        {
            conn_id = gap_msg.msg_data.gap_bond_just_work_conf.conn_id;
            le_bond_just_work_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
            APP_PRINT_INFO0("GAP_MSG_LE_BOND_JUST_WORK");
        }
        break;

    case GAP_MSG_LE_BOND_PASSKEY_DISPLAY:
        {
            uint32_t display_value = 0;
            conn_id = gap_msg.msg_data.gap_bond_passkey_display.conn_id;
            le_bond_get_display_key(conn_id, &display_value);
            APP_PRINT_INFO1("GAP_MSG_LE_BOND_PASSKEY_DISPLAY:passkey %d", display_value);
        }
        break;

    case GAP_MSG_LE_BOND_USER_CONFIRMATION:
        {
            uint32_t display_value = 0;
            conn_id = gap_msg.msg_data.gap_bond_user_conf.conn_id;
            le_bond_get_display_key(conn_id, &display_value);
            APP_PRINT_INFO1("GAP_MSG_LE_BOND_USER_CONFIRMATION: passkey %d", display_value);
            le_bond_user_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
        }
        break;

    case GAP_MSG_LE_BOND_PASSKEY_INPUT:
        {
            uint32_t passkey = 888888;
            conn_id = gap_msg.msg_data.gap_bond_passkey_input.conn_id;
            APP_PRINT_INFO1("GAP_MSG_LE_BOND_PASSKEY_INPUT: conn_id %d", conn_id);
            le_bond_passkey_input_confirm(conn_id, passkey, GAP_CFM_CAUSE_ACCEPT);
        }
        break;

    case GAP_MSG_LE_BOND_OOB_INPUT:
        {
            uint8_t oob_data[GAP_OOB_LEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            conn_id = gap_msg.msg_data.gap_bond_oob_input.conn_id;
            APP_PRINT_INFO0("GAP_MSG_LE_BOND_OOB_INPUT");
            le_bond_set_param(GAP_PARAM_BOND_OOB_DATA, GAP_OOB_LEN, oob_data);
            le_bond_oob_input_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
        }
        break;

    case GAP_MSG_LE_CONN_PARAM_UPDATE:
        {
            periph_conn_param_update_evt(gap_msg.msg_data.gap_conn_param_update.conn_id,
                                         gap_msg.msg_data.gap_conn_param_update.status,
                                         gap_msg.msg_data.gap_conn_param_update.cause);
        }
        break;
    case GAP_MSG_LE_CONN_MTU_INFO:
        {
            uint8_t mtu_size = 0;
            le_get_conn_param(GAP_PARAM_CONN_MTU_SIZE, &mtu_size,
                              gap_msg.msg_data.gap_conn_mtu_info.conn_id);

            if (mtu_size >= 23)
            {
                /* update mtu size */
                app_global_data.mtu_size = mtu_size;
                APP_PRINT_INFO1("[periph_handle_gap_msg] update mtu size to %d", mtu_size);
            }
        }
        break;

    default:
        APP_PRINT_ERROR1("periph_handle_gap_msg: unknown subtype %d", p_gap_msg->subtype);
        break;
    }
}

/*============================================================================*
 *                              Global Functions
 *============================================================================*/
#if !FEATURE_SUPPORT_PRIVACY
/******************************************************************
 * @brief    handle bond modify message.
 * @param    T_LE_BOND_MODIFY_TYPE type
 * @param    T_LE_KEY_ENTRY *p_entry
 * @return   none
 * @retval   void
 */
void app_handle_bond_modify_msg(T_LE_BOND_MODIFY_TYPE type, T_LE_KEY_ENTRY *p_entry)
{
    APP_PRINT_INFO1("app_handle_bond_modify_msg  GAP_MSG_LE_BOND_MODIFY_INFO:type=0x%x",
                    type);

    switch (type)
    {
    case LE_BOND_DELETE:
        {
            le_modify_white_list(GAP_WHITE_LIST_OP_REMOVE, p_entry->remote_bd.addr,
                                 (T_GAP_REMOTE_ADDR_TYPE)(p_entry->remote_bd.remote_bd_type));
        }
        break;
    case LE_BOND_ADD:
        {
            le_modify_white_list(GAP_WHITE_LIST_OP_ADD, p_entry->remote_bd.addr,
                                 (T_GAP_REMOTE_ADDR_TYPE)(p_entry->remote_bd.remote_bd_type));
        }
        break;
    case LE_BOND_CLEAR:
        {
            le_modify_white_list(GAP_WHITE_LIST_OP_CLEAR, p_entry->remote_bd.addr,
                                 (T_GAP_REMOTE_ADDR_TYPE)(p_entry->remote_bd.remote_bd_type));
        }
        break;
    default:
        break;
    }
}
#endif

/******************************************************************
 * @brief    set or get rcu app latency status.
 * @param    index_bit - module index
 * @param    new_status - LANTENCY_ON or LANTENCY_OFF
 * @return   none
 * @retval   void
 */
void app_set_latency_status(T_APP_LATENCY_MASK index_bit, T_LATENCY_STATE new_state)
{
    uint8_t retry_cnt = 0;
    bool latency_upate_value = true;

    APP_PRINT_INFO2("[app_set_latency_status] module index = %d, new_state is %d", index_bit,
                    new_state);

    /*0. check new state valid or not */
    if ((new_state != LANTENCY_ON) && (new_state != LANTENCY_OFF))
    {
        APP_PRINT_WARN0("[app_set_latency_status] new_state is invalid");
        return;
    }

    /*1. check latency state is updating */
    if (app_global_data.latency_status.is_updating == true)
    {
        APP_PRINT_INFO0("[app_set_latency_status] latency_status is pending!");
        /* update pending latency status */
        app_global_data.latency_status.is_pending = true;
        app_global_data.latency_status.pending_status = new_state;
        app_global_data.latency_status.pending_index_bit = index_bit;
        return ;
    }

    /*2. update latency data value */
    if (new_state == LANTENCY_OFF)
    {
        app_global_data.latency_status.latency_bits_map |= (1 << index_bit);
    }
    else
    {
        app_global_data.latency_status.latency_bits_map &= ~(1 << index_bit);
    }

    /*3. check whether need update according to latency value*/
    if (0 == app_global_data.latency_status.latency_bits_map)
    {
        /* need enable latency*/
        if (LANTENCY_ON == app_global_data.latency_status.cur_status)
        {
            APP_PRINT_INFO0("[app_set_latency_status] latency_status is already enabled!");
            return;
        }
        latency_upate_value = false;
    }
    else
    {
        /* need disable latency */
        if (LANTENCY_OFF == app_global_data.latency_status.cur_status)
        {
            APP_PRINT_INFO0("[app_set_latency_status] latency_status is already disabled!");
            return;
        }
        latency_upate_value = true;
    }

    /*4. process latency update*/
    while (retry_cnt < MAX_ONOFF_LATENCY_SEND_RETRY_CNT)
    {
        if (le_disable_slave_latency(0, latency_upate_value) == GAP_CAUSE_SUCCESS)
        {
            APP_PRINT_INFO1("[app_set_latency_status] call le_disable_slave_latency successfully, latency bit map: %#x",
                            app_global_data.latency_status.latency_bits_map);
            app_global_data.latency_status.is_pending = false;
            app_global_data.latency_status.is_updating = true;
            app_global_data.latency_status.cur_status = new_state;
            app_global_data.latency_status.cur_index_bit = index_bit;
            return;
        }
        retry_cnt++;
    }

    /*5. check update success or not*/
    if (retry_cnt >= MAX_ONOFF_LATENCY_SEND_RETRY_CNT)
    {
        /* all retry failed */
        APP_PRINT_WARN0("[app_set_latency_status] all retry failed");
        /* TBD: how to handle this scenario */
    }
}

/******************************************************************
 * @fn       app_init_global_data
 * @brief    Initialize global APP data
 * @param    void
 * @return   none
 * @retval   void
 */
void app_init_global_data(void)
{
    memset(&app_global_data, 0, sizeof(app_global_data));
    app_global_data.mtu_size = 23;
#if FEATURE_SUPPORT_DEEP_SLEEP_STATE
    app_global_data.wake_up_from_dss = false;
#endif
}

/******************************************************************
 * @fn       app_handle_io_msg
 * @brief    All the application events are pre-handled in this function.
 *           All the IO MSGs are sent to this function, Then the event handling function
 *           shall be called according to the MSG type.
 *
 * @param    io_msg - bee io msg data
 * @return   none
 * @retval   void
 */
void app_handle_io_msg(T_IO_MSG io_msg)
{
    uint16_t msg_type = io_msg.type;

    switch (msg_type)
    {
    case IO_MSG_TYPE_BT_STATUS:
        {
            periph_handle_gap_msg(&io_msg);
        }
        break;
    case IO_MSG_TYPE_KEYSCAN:
        {

            if (io_msg.subtype == IO_MSG_KEYSCAN_RX_PKT)
            {
                key_handle_pressed_event(io_msg.u.buf);
            }
        }
        break;
    case IO_MSG_TYPE_TIMER:
        {
            if (io_msg.subtype == IO_MSG_SUBTYPE_VOLTAGE)
            {
                voltage_mode_gear_do();
            }
        }
        break;
    case IO_MSG_TYPE_GPIO:
        {
            if (io_msg.subtype == IO_MSG_SUBTYPE_USB_CHG)
            {
                usb_chg_task_init();
            }
        }
        break;
#if (FEATURE_SUPPORT_MP_TEST_MODE && MP_TEST_MODE_SUPPORT_DATA_UART_TEST)
    case IO_MSG_TYPE_UART:
        {
            uart_test_handle_uart_msg(io_msg);
        }
        break;
#endif
#if SUPPORT_BAT_DETECT_FEATURE
    case IO_MSG_TYPE_BAT_DETECT:
        {
            bat_msg_handle(io_msg.subtype);
        }
        break;
#endif
    case IO_MSG_TYPE_RESET_WDG_TIMER:
        {
            APP_PRINT_INFO0("[WDG] Watch Dog Rset Timer");
            WDG_Restart();
        }
        break;
#if (AON_WDG_ENABLE == 1)
    case IO_MSG_TYPE_RESET_AON_WDG_TIMER:
        {
            APP_PRINT_INFO0("[WDG] AON Watch Dog Rset Timer");
            AON_WDG_Restart();
        }
        break;
#endif
#if SUPPORT_SILENT_OTA
    case IO_MSG_TYPE_DFU_VALID_FW:
        {
            dfu_service_handle_valid_fw(io_msg.u.param);
        }
        break;
#endif
    default:
        break;
    }
}

/******************************************************************
 * @fn       app_gap_callback
 * @brief    All the application gap events are pre-handled in this function.
 *
 * @param    cb_type - event type
 * @param    p_cb_data - point to datas
 * @return   result
 * @retval   T_APP_RESULT
 */
T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
{
    T_APP_RESULT result = APP_RESULT_SUCCESS;
    T_LE_CB_DATA *p_data = (T_LE_CB_DATA *)p_cb_data;

    switch (cb_type)
    {
    case GAP_MSG_LE_DATA_LEN_CHANGE_INFO:
        APP_PRINT_INFO3("GAP_MSG_LE_DATA_LEN_CHANGE_INFO: conn_id %d, tx octets 0x%x, max_tx_time 0x%x",
                        p_data->p_le_data_len_change_info->conn_id,
                        p_data->p_le_data_len_change_info->max_tx_octets,
                        p_data->p_le_data_len_change_info->max_tx_time);
        break;

    case GAP_MSG_LE_BOND_MODIFY_INFO:
        APP_PRINT_INFO1("GAP_MSG_LE_BOND_MODIFY_INFO: type 0x%x",
                        p_data->p_le_bond_modify_info->type);
#if FEATURE_SUPPORT_PRIVACY
        privacy_handle_bond_modify_msg(p_data->p_le_bond_modify_info->type,
                                       p_data->p_le_bond_modify_info->p_entry, true);
#else
        app_handle_bond_modify_msg(p_data->p_le_bond_modify_info->type,
                                   p_data->p_le_bond_modify_info->p_entry);
#endif
        break;

    case GAP_MSG_LE_MODIFY_WHITE_LIST:
        APP_PRINT_INFO2("GAP_MSG_LE_MODIFY_WHITE_LIST: operation %d, cause 0x%x",
                        p_data->p_le_modify_white_list_rsp->operation,
                        p_data->p_le_modify_white_list_rsp->cause);
        break;

    case GAP_MSG_LE_DISABLE_SLAVE_LATENCY:
        APP_PRINT_INFO1("GAP_MSG_LE_DISABLE_SLAVE_LATENCY: cause 0x%x",
                        p_data->p_le_disable_slave_latency_rsp->cause);
        /* restore is_updating to false */
        app_global_data.latency_status.is_updating = false;
        if (p_data->p_le_disable_slave_latency_rsp->cause == GAP_CAUSE_SUCCESS)
        {
            app_global_data.latency_status.retry_cnt = 0;
            /* check if has any pending status */
            if (app_global_data.latency_status.is_pending == true)
            {
                APP_PRINT_INFO0("[app_gap_callback] handle onoff latency pending status");
                app_set_latency_status(app_global_data.latency_status.pending_index_bit,
                                       app_global_data.latency_status.pending_status);
            }
        }
        else
        {
            if (app_global_data.latency_status.retry_cnt < MAX_ONOFF_LATENCY_FAILED_RETRY_CNT)
            {
                APP_PRINT_WARN0("[app_gap_callback] retry onoff latency");
                app_global_data.latency_status.retry_cnt++;
                app_set_latency_status(app_global_data.latency_status.cur_index_bit,
                                       app_global_data.latency_status.cur_status);
            }
            else
            {
                APP_PRINT_ERROR0("[app_gap_callback] OnOff latency failed!");
                /* TBD: how to handle this scenario */
            }
        }
        break;

    default:
        APP_PRINT_INFO1("app_gap_callback: unhandled cb_type 0x%x", cb_type);
        break;
    }
    return result;
}

#if FEATURE_SUPPORT_PRIVACY
/******************************************************************
 * @fn       app_privacy_callback
 * @brief    All the privacy events are pre-handled in this function.
 *
 * @param    type - event type
 * @param    cb_data - point to datas
 * @return   none
 * @retval   void
 */
void app_privacy_callback(T_PRIVACY_CB_TYPE type, T_PRIVACY_CB_DATA cb_data)
{
    APP_PRINT_INFO1("app_privacy_callback: type %d", type);
    switch (type)
    {
    case PRIVACY_STATE_MSGTYPE:
        app_privacy_state = cb_data.privacy_state;
        break;

    case PRIVACY_RESOLUTION_STATUS_MSGTYPE:
        app_privacy_resolution_state = cb_data.resolution_state;
        break;

    default:
        break;
    }
}
#endif

/******************************************************************
 * @fn       app_profile_callback
 * @brief    All the bt profile callbacks are handled in this function.
 *           Then the event handling function shall be called according to the serviceID
 *           of BEE_IO_MSG.
 *
 * @param    service_id  -  service id of profile
 * @param    p_data  - pointer to callback data
 * @return   app_result
 * @retval   T_APP_RESULT
 */
T_APP_RESULT app_profile_callback(T_SERVER_ID service_id, void *p_data)
{
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;
    if (service_id == SERVICE_PROFILE_GENERAL_ID)
    {
        app_general_srv_cb((T_SERVER_APP_CB_DATA *)p_data);
    }
    else if (service_id == app_global_data.bas_srv_id)
    {
        app_result = app_bas_srv_cb((T_BAS_CALLBACK_DATA *)p_data);
    }
    else if (service_id == app_global_data.dis_srv_id)
    {
        app_result = app_dis_srv_cb((T_DIS_CALLBACK_DATA *)p_data);
    }
    else if (service_id == app_global_data.hid_srv_id)
    {
        app_result = app_hid_srv_cb((T_HID_CALLBACK_DATA *)p_data);
    }
    else if (service_id == app_global_data.vendor_srv_id)
    {
        app_result = app_vendor_srv_cb((T_VENDOR_CALLBACK_DATA *)p_data);
    }
    else if (service_id == app_global_data.ota_srv_id)
    {
        app_result = app_ota_srv_cb((T_OTA_CALLBACK_DATA *)p_data);
    }
#if SUPPORT_SILENT_OTA
    else if (service_id == app_global_data.dfu_srv_id)
    {
        app_result = app_dfu_srv_cb((T_DFU_CALLBACK_DATA *)p_data);
    }
#endif
#if SUPPORT_VOICE_FEATURE
#if (VOICE_FLOW_SEL == ATV_GOOGLE_VOICE_FLOW)
    else if (service_id == app_global_data.atvv_srv_id)
    {
        app_result = app_atvv_srv_cb((T_ATVV_CALLBACK_DATA *)p_data);
    }
#elif (VOICE_FLOW_SEL == RTK_GATT_VOICE_FLOW)
    else if (service_id == app_global_data.voice_srv_id)
    {
        app_result = app_voice_srv_cb((T_VOICE_CALLBACK_DATA *)p_data);
    }
#endif
#endif
    return app_result;
}

/******************* (C) COPYRIGHT 2020 Realtek Semiconductor Corporation *****END OF FILE****/
