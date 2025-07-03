/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     profile_init.c
* @brief    .
* @details
* @author   Ken_mei
* @date     2015-03-27
* @version  v0.2
**********************************************************************************************************/
#include "rtl876x.h"
#include "gap.h"
#include "gap_adv.h"
#include "rcu_gap.h"
#include "board.h"
#include "gap_storage_le.h"
#include "gap_conn_le.h"
#include "string.h"
#include <trace.h>
#include "mem_config.h"
#include "rcu_application.h"
#include <gap_bond_le.h>
#include "os_timer.h"
#include "swtimer.h"
#include "led_driver.h"
#include "mp_test.h"
#if FEATURE_SUPPORT_PRIVACY
#include "privacy_mgnt.h"
#endif
#if FEATURE_SUPPORT_RECOVER_PAIR_INFO
#include "os_mem.h"
#include "ftl.h"
#endif

/*============================================================================*
 *                         Macros
 *============================================================================*/
/* What is the advertising interval when device is discoverable (units of 625us, 160=100ms) */
#define DEFAULT_ADVERTISING_INTERVAL_MIN 320 /* 20ms */
#define DEFAULT_ADVERTISING_INTERVAL_MAX 320 /* 30ms */

/*============================================================================*
 *                              Local Variables
 *============================================================================*/
#if FEATURE_SUPPORT_RECOVER_PAIR_INFO
bool is_need_delete_last_bond_info = false;
bool is_first_check_bond_info = true;
#endif

/* GAP - SCAN RSP data (max size = 31 bytes) */
static uint8_t app_scan_rsp_data[] =
{
    0x03,  /* length */
    0x19,  /* type="Appearance" */
    0x80, 0x01,  /* Remote Control */

    C_DEVICE_NAME_LEN,  /* length */
    0x09,  /* type="Complete local name" */
    C_DEVICE_NAME,  
};

/* Pairing advertisement data (max size = 31 bytes, though this is
 best kept short to conserve power while advertisting) */
static uint8_t app_pairing_adv_data[] =
{
    0x02,  /* length */
    0x01,  /* type="Flags" */
    0x05,  /* LE Limited Discoverable Mode, BR/EDR not supported */

    C_DEVICE_NAME_LEN,  /* length */
    0x09,  /* type="Complete local name" */
    C_DEVICE_NAME,  

    0x03,  /* length */
    0x19,  /* type="More 16-bit UUIDs available" */
    0x80, 0x01,  /* HID Service */

    0x03,  /* length */
    0x02,  /* type="Appearance" */
    0x12, 0x18,  /* Generic Remote Control */

    0x04,  /* length */
    0x0d,  /* type="Appearance" */
    0x04, 0x05, 0x00,  /* Generic Remote Control */
};
/*注意：此处时产测模式广播出去的数据，
*最后一行格式为，
*10Byte记录按键是否按下过，
*1Byte记录当前按下的按键。（从1开始）
*1Byte记录语音测试是否通过。
*
*/

#define FIRST_VERSION  0x00
#define SECOND_VERSION 0x00
#define THIRD_VERSION  0x01
#define FOURTH_VERSION  0x00
#define FIFTH_VERSION  0x04
uint8_t app_syseneasy_test_adv_data[24]= 
{
    0x18,  /* length */
    0x23,  /* Type: Manufacturer Specific (0xff) */
    0x03, // Company ID: Texas Instruments Inc. (0x000d)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,// Data: 383802000001,19:信噪比，18：頻率，16-17：音量
    FIRST_VERSION, SECOND_VERSION, THIRD_VERSION, FOURTH_VERSION, FIFTH_VERSION  // Software version 预留5位作为软件版本，如果软件版本是1.0.11，那这5个字节就是 0x00，0x00，0x01，0x00，0x0B
};
// {
//     0x0e,  /* length */
//     0x23,  /* Type: Manufacturer Specific (0xff) */
//     0x03, // Company ID: Texas Instruments Inc. (0x000d)
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // Data: 383802000001
// };

static uint8_t app_undirect_reconnect_adv_data[] =
{
    0x02,  /* length */
    0x01,  /* type="Flags" */
    0x05,  /* LE Limited Discoverable Mode, BR/EDR not supported */

    0x03,  /* length */
    0x03,  /* type="More 16-bit UUIDs available" */
    0x12, 0x18,  /* HID Service */

    0x03,  /* length */
    0x19,  /* type="Appearance" */
    0x80, 0x01,  /* Generic Remote Control */
};
/* Prompt advertisement data (max size = 31 bytes, though this is
 best kept short to conserve power while advertisting) */
static uint8_t app_prompt_adv_data[] =
{
    0x12,           /* length */
    0xff,           /* type="Manufacturer Specific" */
    0x2f,0x07,      /*Company ID = OnePlus Electronics(Shenzhen) */  
    0x43,0x52,0x2B,0x31,0x3C,0x00,0x07,0x00,0xF4,0x30,0x8B,0x05,0x5e,0xa2,0x3a,  /*3a a2 5e 05 8b 30 f4 00 07 00 3c 31 2b 52 43*/

    0x02,  /* length */
    0x01,  /* type="Flags" */
    0x04,  /* No Discoverable Mode, BR/EDR not supported */
};

/* GAP - Advertisement data (max size = 31 bytes, though this is
 best kept short to conserve power while advertisting) */
uint8_t app_power_adv_data[] =
{
    0x12,  /* length     */
    0xFF,  /* vendor information */
    0x2F, 0x07,/* OnePlus Electroics(Shenzhen) information */
    0x00,  /* Seq Number */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* TV address */

    0x00, 0x07, 0x00, 0x3d,/* Power */

    0x31, 0x2B, 0x52, 0x43,/* Short Local Name */ 

    0x02,  /* length */
    0x01,  /* type="Flags" */
    0x04,  /* No Discoverable Mode, BR/EDR not supported */
};

/******************************************************************
 * @fn          Initial gap parameters
 * @brief      Initialize peripheral and gap bond manager related parameters
 *
 * @return     void
 */
void rcu_le_gap_init(void)
{
    uint8_t  device_name[GAP_DEVICE_NAME_LEN] = {C_DEVICE_NAME};
    uint16_t appearance = GAP_GATT_APPEARANCE_GENERIC_REMOTE_CONTROL;

    //advertising parameters
    uint8_t  adv_evt_type = GAP_ADTYPE_ADV_IND;
    uint8_t  adv_direct_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    uint8_t  adv_direct_addr[GAP_BD_ADDR_LEN] = {0};
    uint8_t  adv_chann_map = GAP_ADVCHAN_ALL;
    uint8_t  adv_filter_policy = GAP_ADV_FILTER_ANY;
    uint16_t adv_int_min = DEFAULT_ADVERTISING_INTERVAL_MIN;
    uint16_t adv_int_max = DEFAULT_ADVERTISING_INTERVAL_MIN;

    //GAP Bond Manager parameters
    uint8_t  auth_pair_mode = GAP_PAIRING_MODE_PAIRABLE;
    uint16_t auth_flags = GAP_AUTHEN_BIT_BONDING_FLAG;
    uint8_t  auth_io_cap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
    uint8_t  auth_oob = false;
    uint8_t  auth_use_fix_passkey = false;
    uint32_t auth_fix_passkey = 0;
    uint8_t  auth_sec_req_enalbe = false;
    uint16_t auth_sec_req_flags = GAP_AUTHEN_BIT_BONDING_FLAG;
    uint8_t  slave_init_mtu_req = true;
    le_set_gap_param(GAP_PARAM_SLAVE_INIT_GATT_MTU_REQ, sizeof(slave_init_mtu_req),
                     &slave_init_mtu_req);

    //Register gap callback
    le_register_app_cb(app_gap_callback);

#if FEATURE_SUPPORT_PRIVACY
    privacy_init(app_privacy_callback, true);
#endif

    //Set device name and device appearance
    le_set_gap_param(GAP_PARAM_DEVICE_NAME, GAP_DEVICE_NAME_LEN, device_name);
    le_set_gap_param(GAP_PARAM_APPEARANCE, sizeof(appearance), &appearance);

    //Set advertising parameters
    le_adv_set_param(GAP_PARAM_ADV_EVENT_TYPE, sizeof(adv_evt_type), &adv_evt_type);
    le_adv_set_param(GAP_PARAM_ADV_DIRECT_ADDR_TYPE, sizeof(adv_direct_type), &adv_direct_type);
    le_adv_set_param(GAP_PARAM_ADV_DIRECT_ADDR, sizeof(adv_direct_addr), adv_direct_addr);
    le_adv_set_param(GAP_PARAM_ADV_CHANNEL_MAP, sizeof(adv_chann_map), &adv_chann_map);
    le_adv_set_param(GAP_PARAM_ADV_FILTER_POLICY, sizeof(adv_filter_policy), &adv_filter_policy);
    le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MIN, sizeof(adv_int_min), &adv_int_min);
    le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MAX, sizeof(adv_int_max), &adv_int_max);
    le_adv_set_param(GAP_PARAM_ADV_DATA, sizeof(app_pairing_adv_data), app_pairing_adv_data);
    le_adv_set_param(GAP_PARAM_SCAN_RSP_DATA, sizeof(app_scan_rsp_data), app_scan_rsp_data);

    // Setup the GAP Bond Manager
    gap_set_param(GAP_PARAM_BOND_PAIRING_MODE, sizeof(auth_pair_mode), &auth_pair_mode);
    gap_set_param(GAP_PARAM_BOND_AUTHEN_REQUIREMENTS_FLAGS, sizeof(auth_flags), &auth_flags);
    gap_set_param(GAP_PARAM_BOND_IO_CAPABILITIES, sizeof(auth_io_cap), &auth_io_cap);
    gap_set_param(GAP_PARAM_BOND_OOB_ENABLED, sizeof(auth_oob), &auth_oob);
    le_bond_set_param(GAP_PARAM_BOND_FIXED_PASSKEY, sizeof(auth_fix_passkey), &auth_fix_passkey);
    le_bond_set_param(GAP_PARAM_BOND_FIXED_PASSKEY_ENABLE, sizeof(auth_use_fix_passkey),
                      &auth_use_fix_passkey);
    le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_ENABLE, sizeof(auth_sec_req_enalbe), &auth_sec_req_enalbe);
    le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_REQUIREMENT, sizeof(auth_sec_req_flags),
                      &auth_sec_req_flags);

}

/**
 * @brief start advertising
 * @param unsigned char flg - indicate to send which type of adv
 * @return true or false
 * @retval void
 */
bool rcu_start_adv(T_ADV_TYPE adv_type)
{
    bool result = false;
    uint8_t adv_event_type;
    uint8_t adv_channel_map;
    uint8_t adv_filter_policy;
    uint16_t adv_interval_min;
    uint16_t adv_interval_max;
    T_LE_KEY_ENTRY *p_le_key_entry = le_get_high_priority_bond();

    /* check current status, only allow to start adv if RCU_STATUS_IDLE */
    if (app_global_data.rcu_status != RCU_STATUS_IDLE)
    {
        APP_PRINT_WARN1("rcu_status %d is not allowed to start adv again!", app_global_data.rcu_status);
        return false;
    }

    switch (adv_type)
    {
    case ADV_DIRECT_HDC:
        {
            adv_event_type = GAP_ADTYPE_ADV_HDC_DIRECT_IND;
            adv_filter_policy = GAP_ADV_FILTER_ANY;
            adv_channel_map = GAP_ADVCHAN_ALL;

            le_adv_set_param(GAP_PARAM_ADV_EVENT_TYPE, sizeof(adv_event_type), &adv_event_type);
            le_adv_set_param(GAP_PARAM_ADV_FILTER_POLICY, sizeof(adv_filter_policy), &adv_filter_policy);
            le_adv_set_param(GAP_PARAM_ADV_CHANNEL_MAP, sizeof(adv_channel_map), &adv_channel_map);

#if FEATURE_SUPPORT_PRIVACY
            if (p_le_key_entry->remote_bd.remote_bd_type == GAP_REMOTE_ADDR_LE_RANDOM)
            {
                if (app_privacy_resolution_state == PRIVACY_ADDR_RESOLUTION_DISABLED)
                {
                    privacy_set_addr_resolution(true);
                }

                le_adv_set_param(GAP_PARAM_ADV_DIRECT_ADDR_TYPE,
                                 sizeof(p_le_key_entry->resolved_remote_bd.remote_bd_type),
                                 &(p_le_key_entry->resolved_remote_bd.remote_bd_type));
                le_adv_set_param(GAP_PARAM_ADV_DIRECT_ADDR, sizeof(p_le_key_entry->resolved_remote_bd.addr),
                                 p_le_key_entry->resolved_remote_bd.addr);
            }
            else
            {
                le_adv_set_param(GAP_PARAM_ADV_DIRECT_ADDR_TYPE, sizeof(p_le_key_entry->remote_bd.remote_bd_type),
                                 &(p_le_key_entry->remote_bd.remote_bd_type));
                le_adv_set_param(GAP_PARAM_ADV_DIRECT_ADDR, sizeof(p_le_key_entry->remote_bd.addr),
                                 p_le_key_entry->remote_bd.addr);
            }
#else
            le_adv_set_param(GAP_PARAM_ADV_DIRECT_ADDR_TYPE, sizeof(p_le_key_entry->remote_bd.remote_bd_type),
                             &(p_le_key_entry->remote_bd.remote_bd_type));
            le_adv_set_param(GAP_PARAM_ADV_DIRECT_ADDR, sizeof(p_le_key_entry->remote_bd.addr),
                             p_le_key_entry->remote_bd.addr);
#endif

            if (GAP_CAUSE_SUCCESS == le_adv_start())
            {
                APP_PRINT_INFO0("rcu start ADV_DIRECT_HDC success!");
                app_global_data.rcu_status = RCU_STATUS_ADVERTISING;
                app_global_data.adv_type = ADV_DIRECT_HDC;
                result = true;
            }
            else
            {
                APP_PRINT_WARN0("rcu start ADV_DIRECT_HDC fail!");
            }
        }
        break;

    case ADV_UNDIRECT_RECONNECT:
        {
#if FEATURE_SUPPORT_PRIVACY
            if (app_privacy_resolution_state == PRIVACY_ADDR_RESOLUTION_DISABLED)
            {

                privacy_set_addr_resolution(true);
            }
#endif
            adv_event_type = GAP_ADTYPE_ADV_IND;
            adv_channel_map = GAP_ADVCHAN_ALL;
            adv_filter_policy = GAP_ADV_FILTER_WHITE_LIST_ALL;
            adv_interval_min = 0x20;  /* 20ms */
            adv_interval_max = 0x20;  /* 20ms */

            le_adv_set_param(GAP_PARAM_ADV_EVENT_TYPE, sizeof(adv_event_type), &adv_event_type);
            le_adv_set_param(GAP_PARAM_ADV_CHANNEL_MAP, sizeof(adv_channel_map), &adv_channel_map);
            le_adv_set_param(GAP_PARAM_ADV_FILTER_POLICY, sizeof(adv_filter_policy), &adv_filter_policy);
            le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MIN, sizeof(adv_interval_min), &adv_interval_min);
            le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MAX, sizeof(adv_interval_max), &adv_interval_max);
            le_adv_set_param(GAP_PARAM_ADV_DATA, sizeof(app_undirect_reconnect_adv_data), app_undirect_reconnect_adv_data);
            le_adv_set_param(GAP_PARAM_SCAN_RSP_DATA, sizeof(app_scan_rsp_data), app_scan_rsp_data);

            if (GAP_CAUSE_SUCCESS == le_adv_start())
            {
                APP_PRINT_INFO0("rcu start ADV_UNDIRECT_RECONNECT success!");
                app_global_data.rcu_status = RCU_STATUS_ADVERTISING;
                app_global_data.adv_type = ADV_UNDIRECT_RECONNECT;
                if(app_global_data.abnormal_discon_state == 1){
                    app_global_data.abnormal_discon_state = 0;
                    os_timer_restart(&adv_timer, ADV_UNDIRECT_DISCON_RECONNECT_TIMEOUT);
                }
                else{
                    os_timer_restart(&adv_timer, ADV_UNDIRECT_RECONNECT_TIMEOUT);
                }                
                result = true;
            }
            else
            {
                APP_PRINT_WARN0("rcu start ADV_UNDIRECT_RECONNECT fail!");
            }
        }
        break;

    case ADV_UNDIRECT_POWER:
        {
            PROFILE_PRINT_INFO0("ADV_UNDIRECT_POWER for power on master!");
#if FEATURE_SUPPORT_PRIVACY
            if (app_privacy_resolution_state == PRIVACY_ADDR_RESOLUTION_DISABLED)
            {

                privacy_set_addr_resolution(true);
            }
#endif
            adv_event_type = GAP_ADTYPE_ADV_IND;
            adv_channel_map = GAP_ADVCHAN_ALL;
            adv_filter_policy = GAP_ADV_FILTER_WHITE_LIST_ALL;
            adv_interval_min = 0x20;  /* 20ms */
            adv_interval_max = 0x30;  /* 30ms */
#if FEATURE_SUPPORT_PRIVACY
            for (int i = 0; i < 6; i++)
            {
                app_power_adv_data[sizeof(app_power_adv_data) - 14 + i] = p_le_key_entry->resolved_remote_bd.addr[i];
            }
#else
            for (int i = 0; i < 6; i++)
            {
                app_power_adv_data[sizeof(app_power_adv_data) - 14 + i] = p_le_key_entry->remote_bd.addr[i];
            }
#endif      
            app_power_adv_data[4] ++;
            le_adv_set_param(GAP_PARAM_ADV_EVENT_TYPE, sizeof(adv_event_type), &adv_event_type);
            le_adv_set_param(GAP_PARAM_ADV_CHANNEL_MAP, sizeof(adv_channel_map), &adv_channel_map);
            le_adv_set_param(GAP_PARAM_ADV_FILTER_POLICY, sizeof(adv_filter_policy), &adv_filter_policy);
            le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MIN, sizeof(adv_interval_min), &adv_interval_min);
            le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MAX, sizeof(adv_interval_max), &adv_interval_max);
            le_adv_set_param(GAP_PARAM_ADV_DATA, sizeof(app_power_adv_data), app_power_adv_data);
            le_adv_set_param(GAP_PARAM_SCAN_RSP_DATA, sizeof(app_scan_rsp_data), app_scan_rsp_data);

            if (GAP_CAUSE_SUCCESS == le_adv_start())
            {
                APP_PRINT_INFO0("rcu start ADV_UNDIRECT_POWER success!");
                app_global_data.rcu_status = RCU_STATUS_ADVERTISING;
                app_global_data.adv_type = ADV_UNDIRECT_POWER;
                os_timer_restart(&adv_timer, ADV_UNDIRECT_POWER_TIMEOUT);
                result = true;
            }
            else
            {
                APP_PRINT_WARN0("rcu start ADV_UNDIRECT_POWER fail!");
            }
        }
        break;

    case ADV_UNDIRECT_PAIRING:
        {
            PROFILE_PRINT_INFO0("ADV_UNDIRECT_PAIRING for pairing!");
#if FEATURE_SUPPORT_REMOVE_LINK_KEY_BEFORE_PAIRING
            if (app_global_data.is_link_key_existed)
            {
#if FEATURE_SUPPORT_RECOVER_PAIR_INFO
                get_bond_info();
#endif
                app_global_data.is_link_key_existed = false;
                le_bond_clear_all_keys();
            }
#endif
#if FEATURE_SUPPORT_PRIVACY
            if (app_privacy_resolution_state == PRIVACY_ADDR_RESOLUTION_ENABLED)
            {
                privacy_set_addr_resolution(false);
            }
#endif
            adv_event_type = GAP_ADTYPE_ADV_IND;
            adv_channel_map = GAP_ADVCHAN_ALL;
            adv_filter_policy = GAP_ADV_FILTER_ANY;
            adv_interval_min = 0x20;  /* 20ms */
            adv_interval_max = 0x30;  /* 30ms */

            le_adv_set_param(GAP_PARAM_ADV_EVENT_TYPE, sizeof(adv_event_type), &adv_event_type);
            le_adv_set_param(GAP_PARAM_ADV_CHANNEL_MAP, sizeof(adv_channel_map), &adv_channel_map);
            le_adv_set_param(GAP_PARAM_ADV_FILTER_POLICY, sizeof(adv_filter_policy), &adv_filter_policy);
            le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MIN, sizeof(adv_interval_min), &adv_interval_min);
            le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MAX, sizeof(adv_interval_max), &adv_interval_max);
            le_adv_set_param(GAP_PARAM_ADV_DATA, sizeof(app_pairing_adv_data), app_pairing_adv_data);
            le_adv_set_param(GAP_PARAM_SCAN_RSP_DATA, sizeof(app_scan_rsp_data), app_scan_rsp_data);

            if (GAP_CAUSE_SUCCESS == le_adv_start())
            {
                APP_PRINT_INFO0("rcu start ADV_UNDIRECT_PAIRING success!");
                app_global_data.rcu_status = RCU_STATUS_ADVERTISING;
                app_global_data.adv_type = ADV_UNDIRECT_PAIRING;
                os_timer_restart(&adv_timer, ADV_UNDIRECT_PAIRING_TIMEOUT);
                if(single_tone_test_flag == true){
                }
                else{
                    LED_BLINK(LED_1, LED_TYPE_BLINK_PAIR_ADV, 0);
                }
                result = true;
            }
            else
            {
                APP_PRINT_WARN0("rcu start ADV_UNDIRECT_PAIRING fail!");
            }
        }
        break;

    case ADV_UNDIRECT_PROMPT:
        PROFILE_PRINT_INFO0("ADV_UNDIRECT_PROMPT for prompt!");
#if FEATURE_SUPPORT_PRIVACY
        if (app_privacy_resolution_state == PRIVACY_ADDR_RESOLUTION_DISABLED)
        {

            privacy_set_addr_resolution(true);
        }
#endif
        adv_event_type = GAP_ADTYPE_ADV_NONCONN_IND;
        adv_channel_map = GAP_ADVCHAN_ALL;
        adv_filter_policy = GAP_ADV_FILTER_WHITE_LIST_ALL;
        adv_interval_min = 0x20;
        adv_interval_max = 0x30;

        le_adv_set_param(GAP_PARAM_ADV_EVENT_TYPE, sizeof(adv_event_type), &adv_event_type);
        le_adv_set_param(GAP_PARAM_ADV_CHANNEL_MAP, sizeof(adv_channel_map), &adv_channel_map);
        le_adv_set_param(GAP_PARAM_ADV_FILTER_POLICY, sizeof(adv_filter_policy), &adv_filter_policy);
        le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MIN, sizeof(adv_interval_min), &adv_interval_min);
        le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MAX, sizeof(adv_interval_max), &adv_interval_max);
        le_adv_set_param(GAP_PARAM_ADV_DATA, sizeof(app_prompt_adv_data), app_prompt_adv_data);
        le_adv_set_param(GAP_PARAM_SCAN_RSP_DATA,  sizeof(app_scan_rsp_data), app_scan_rsp_data);

        if (GAP_CAUSE_SUCCESS == le_adv_start())
        {
            APP_PRINT_INFO0("rcu start ADV_UNDIRECT_PROMPT success!");
            app_global_data.rcu_status = RCU_STATUS_ADVERTISING;
            app_global_data.adv_type = ADV_UNDIRECT_PROMPT;
            os_timer_restart(&adv_timer, ADV_UNDIRECT_PROMPT_TIMEOUT);
            result = true;
        }
        else
        {
            APP_PRINT_WARN0("rcu start ADV_UNDIRECT_PROMPT fail!");
        }
        break;
        
    default:
        /* invalid adv type */
        APP_PRINT_WARN1("[rcu_start_adv] unknown adv type: %d", adv_type);
        break;
    }

    return result;
}

/**
 * @brief stop advertising
 * @param stop_adv_reason - indicate to the reason to stop adv
 * @return true or false
 * @retval void
 */
bool rcu_stop_adv(T_STOP_ADV_REASON stop_adv_reason)
{
    bool result = false;
    app_global_data.stop_adv_reason = stop_adv_reason;

    if (app_global_data.rcu_status == RCU_STATUS_ADVERTISING)
    {
        if (le_adv_stop() == GAP_CAUSE_SUCCESS)
        {
            APP_PRINT_INFO0("[rcu_stop_adv] stop adv command sent successfully");
            app_global_data.rcu_status = RCU_STATUS_STOP_ADVERTISING;
            app_global_data.adv_type = ADV_IDLE;
            result = true;
        }
        else
        {
            APP_PRINT_WARN0("[rcu_stop_adv] stop adv command sent failed");
        }
    }
    else
    {
        APP_PRINT_WARN1("[rcu_stop_adv] Invalid RCU status: %d", app_global_data.rcu_status);
    }

    return result;
}

/**
 * @brief terminate connection
 */
bool rcu_terminate_connection(T_DISCONN_REASON disconn_reason)
{
    bool result = false;
    app_global_data.disconn_reason = disconn_reason;

    if ((app_global_data.rcu_status == RCU_STATUS_CONNECTED)
        || (app_global_data.rcu_status == RCU_STATUS_PAIRED))
    {
        if (le_disconnect(0) == GAP_CAUSE_SUCCESS)
        {
            APP_PRINT_INFO0("[rcu_terminate_connection] terminate command sent successfully");
            result = true;
        }
        else
        {
            APP_PRINT_WARN0("[rcu_terminate_connection] terminate command sent failed");
        }
    }
    else
    {
        APP_PRINT_WARN1("[rcu_terminate_connection] Invalid RCU status: %d", app_global_data.rcu_status);
    }

    return result;

}



/**
 * @brief update peripheral connection parameters
 * @param interval - connection interval; latency - peripheral connection latency
 * @return none
 */
void rcu_update_conn_params(uint16_t interval, uint16_t latency, uint16_t timeout)
{
    if (GAP_CAUSE_SUCCESS != le_update_conn_param(0, interval, interval, latency, timeout / 10,
                                                  interval * 2 - 2,
                                                  interval * 2 - 2))
    {
        if (app_global_data.updt_conn_params_retry_cnt < MAX_UPDATE_CONN_PARAMS_RETRY_CNT)
        {
            app_global_data.updt_conn_params_retry_cnt++;
            APP_PRINT_WARN0("[rcu_update_conn_params] send HCI command retry");
            os_timer_restart(&update_conn_params_timer, UPDATE_CONN_PARAMS_TIMEOUT);
        }
        else
        {
            APP_PRINT_WARN0("[rcu_update_conn_params] send HCI command failed");
        }
    }
}

#if FEATURE_SUPPORT_RECOVER_PAIR_INFO
/**
 * @brief get bond information
 * @param none
 * @return none
 */
void get_bond_info(void)
{
    uint8_t idx = 0;
    T_LE_KEY_ENTRY *p_entry = NULL;

    APP_PRINT_WARN0("[bond info] get_bond_info");

    if (app_global_data.is_link_key_existed)
    {
        app_global_data.bond_info_length = le_get_dev_bond_info_len();//FTL_PAIRED_INFO_LEN
        app_global_data.p_last_bond_info = os_mem_alloc(RAM_TYPE_DATA_ON, app_global_data.bond_info_length);

        p_entry = le_find_key_entry_by_idx(idx);
        if ((app_global_data.p_last_bond_info != NULL) && (p_entry != NULL))
        {
            bool result = le_get_dev_bond_info(p_entry, app_global_data.p_last_bond_info);
            if (result)
            {
                /* get bond information successfully, and delete bond information */
                is_need_delete_last_bond_info = true;
                ftl_save(app_global_data.p_last_bond_info, FTL_PAIRED_INFO_OFFSET, FTL_PAIRED_INFO_LEN);
            }
            else
            {
                os_mem_free(app_global_data.p_last_bond_info);
            }
        }
    }
}

/**
 * @brief delete last bond information
 * @param none
 * @return none
 */
void delete_last_bond_info(void)
{
    APP_PRINT_WARN0("[bond info] delete_last_bond_info");

    if (is_need_delete_last_bond_info)
    {
        is_need_delete_last_bond_info = false;
        os_mem_free(app_global_data.p_last_bond_info);
        memset(app_global_data.p_last_bond_info, 0, FTL_PAIRED_INFO_LEN);
        ftl_save(app_global_data.p_last_bond_info, FTL_PAIRED_INFO_OFFSET, FTL_PAIRED_INFO_LEN);
    }
}

/**
 * @brief set bond information
 * @param none
 * @return none
 */
void set_bond_info(void)
{
    bool exist = false;
    T_LE_KEY_ENTRY *p_entry = NULL;

    if (is_first_check_bond_info)
    {
        APP_PRINT_WARN0("[bond info] is_first_check_bond_info");
        is_first_check_bond_info = false;
        app_global_data.p_last_bond_info = os_mem_alloc(RAM_TYPE_DATA_ON, FTL_PAIRED_INFO_LEN);
        ftl_load(app_global_data.p_last_bond_info, FTL_PAIRED_INFO_OFFSET, FTL_PAIRED_INFO_LEN);
        T_LE_KEY_ENTRY *p_entry1 = (T_LE_KEY_ENTRY *)app_global_data.p_last_bond_info;
        if (p_entry1->is_used != 0 || p_entry1->idx != 0 || p_entry1->flags != 0 ||
            p_entry1->local_bd_type != 0
            || p_entry1->app_data != 0 || p_entry1->remote_bd.bond_flags != 0 ||
            p_entry1->remote_bd.remote_bd_type != 0
            || p_entry1->resolved_remote_bd.bond_flags != 0 || p_entry1->resolved_remote_bd.remote_bd_type != 0)
        {
            is_need_delete_last_bond_info = true;
        }
        else
        {
            os_mem_free(app_global_data.p_last_bond_info);
        }
    }
    if (is_need_delete_last_bond_info)
    {
        APP_PRINT_WARN0("[bond info] set_bond_info");
        p_entry = le_set_dev_bond_info(FTL_PAIRED_INFO_LEN, app_global_data.p_last_bond_info,
                                       &exist);
        if ((p_entry != NULL) && (!exist))
        {
#if FEATURE_SUPPORT_PRIVACY
            /* set bond information successfully */
            privacy_handle_bond_modify_msg(LE_BOND_ADD, p_entry, true);
#else
            app_handle_bond_modify_msg(LE_BOND_ADD, p_entry);
#endif
            app_global_data.is_link_key_existed = true;
        }
        delete_last_bond_info();
    }
}
#endif

