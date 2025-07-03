/**
*********************************************************************************************************
*               Copyright(c) 2018, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     key_handle.c
* @brief    This is the entry of user code which the key handle module resides in.
* @details
* @author   chenjie jin
* @date     2018-05-03
* @version  v1.1
*********************************************************************************************************
*/

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "board.h"
#include "key_handle.h"
#include <trace.h>
#include "profile_server.h"
#include "hids_rmc.h"
#include "rcu_application.h"
#include "os_timer.h"
#include "swtimer.h"
#include "voice.h"
#include "voice_driver.h"
#include "rcu_gap.h"
#include "gap_conn_le.h"
#include "gap_storage_le.h"
#include "rtl876x_wdg.h"
#include "rtl876x_keyscan.h"
#include "gap_bond_le.h"
#include "battery_driver.h"
#if IR_FUN
#include "ir_app_config.h"
#endif
#include "led_driver.h"
#if FEATURE_SUPPORT_MP_TEST_MODE
#include "mp_test.h"
#endif
#if (VOICE_FLOW_SEL == ATV_GOOGLE_VOICE_FLOW)
#include "atvv_service.h"
#endif
#include <app_section.h>

/*============================================================================*
 *                              Local Variables
 *============================================================================*/
/* Key Mapping Table Definiton */
#if (RCU_HD_PLATFORM_SEL == R_DEMO_RCU)
static const T_KEY_INDEX_DEF KEY_MAPPING_TABLE[KEYPAD_ROW_SIZE][KEYPAD_COLUMN_SIZE] =
{
    {VK_TV_POWER,      VK_EXIT,         VK_ENTER,    VK_MOUSE_EN},
    {VK_POWER,         VK_VOLUME_UP,    VK_DOWN,     VK_RIGHT},
    {VK_TV_SIGNAL,     VK_VOLUME_DOWN,  VK_VOICE,    VK_MENU},
    {VK_UP,            VK_PAGE_DOWN,    VK_HOME,     VK_PAGE_UP},
    {VK_LEFT,          VK_NC,           VK_NC,       VK_NC},
};
#elif (RCU_HD_PLATFORM_SEL == H_DEMO_RCU)
static const T_KEY_INDEX_DEF KEY_MAPPING_TABLE[KEYPAD_ROW_SIZE][KEYPAD_COLUMN_SIZE] =
{
    {VK_POWER,      VK_VOICE,       VK_HOME,    VK_MENU},
    {VK_VOLUME_UP,  VK_VOLUME_DOWN, VK_ENTER,   VK_EXIT},
    {VK_LEFT,       VK_RIGHT,       VK_UP,      VK_DOWN},
};
#elif (RCU_HD_PLATFORM_SEL == G_MIN_DEMO_RCU)
static const T_KEY_INDEX_DEF KEY_MAPPING_TABLE[KEYPAD_ROW_SIZE][KEYPAD_COLUMN_SIZE] =
{
    {VK_POWER,           MM_AC_Back,         MM_DPadCenter,  MM_Dashboard},
    {MM_Mute,            VK_VOLUME_UP,       MM_DPadDown,    MM_DPadRight},
    {MM_AC_Bookmarks,    VK_VOLUME_DOWN,     MM_AC_Search,   MM_Guide},
    {MM_DPadUp,          VK_YOUTUBE,         MM_AC_Home,     MM_Live},
    {MM_DPadLeft,        VK_NETFLIX,         VK_APP04,       VK_NC},
};
#endif

/*============================================================================*
 *                              Global Variables
 *============================================================================*/
/* BLE HID code table definition */
const uint16_t BLE_KEY_CODE_TABLE[BLE_KEY_CODE_TABLE_SIZE] =
{
#if (RCU_HD_PLATFORM_SEL != G_MIN_DEMO_RCU)
    0x00,  /* VK_NC */
    0x66,  /* VK_POWER */
    0x4B,  /* VK_PAGE_UP */
    0x4E,  /* VK_PAGE_DOWN */
    0x76,  /* VK_MENU */
    0x4A,  /* VK_HOME */
    0x3E,  /* VK_VOICE */
    0x28,  /* VK_ENTER */
    0x29,  /* VK_EXIT */
    0x50,  /* VK_LEFT */
    0x4F,  /* VK_RIGHT */
    0x52,  /* VK_UP */
    0x51,  /* VK_DOWN */
    0x00,  /* VK_MOUSE_EN */
    0x7F,  /* VK_VOLUME_MUTE */
    0x80,  /* VK_VOLUME_UP */
    0x81,  /* VK_VOLUME_DOWN */
    0x3F,  /* VK_VOICE_STOP */
    0x00,  /* VK_TV_POWER */
    0x00,  /* VK_TV_SIGNAL */
#if FEATURE_SUPPORT_MULTIMEDIA_KEYBOARD
    0xb5,    /* MM_ScanNext */
    0xb6,    /* MM_ScanPrevious */
    0xb7,    /* MM_Stop */
    0xcd,    /* MM_Play_Pause */
    0xe2,    /* MM_Mute */
    0xe5,    /* MM_BassBoost */
    0xe7,    /* MM_Loudness */
    0xe9,    /* MM_VolumeIncrement */
    0xea,    /* MM_VolumeDecrement */
    0x0152,  /* MM_BassIncrement */
    0x0153,  /* MM_BassDecrement */
    0x0154,  /* MM_TrebleIncrement */
    0x0155,  /* MM_TrebleDecrement */
    0x0183,  /* MM_AL_ConsumerControl */
    0x018a,  /* MM_AL_EmailReader */
    0x0192,  /* MM_AL_Calculator */
    0x0194,  /* MM_AL_LocalMachineBrowser */
    0x0221,  /* MM_AC_Search */
    0x0223,  /* MM_AC_Home */
    0x0224,  /* MM_AC_Back */
    0x0225,  /* MM_AC_Forward */
    0x0226,  /* MM_AC_Stop */
    0x0227,  /* MM_AC_Refresh */
    0x022a,  /* MM_AC_Bookmarks */
#endif
#else
    0x00,  /* VK_NC */
    0x66,  /* VK_POWER */
    0x4B,  /* VK_PAGE_UP */
    0x4E,  /* VK_PAGE_DOWN */
    0x76,  /* VK_MENU */
    0x4A,  /* VK_HOME */
    0x3E,  /* VK_VOICE */
    0x28,  /* VK_ENTER */
    0x29,  /* VK_EXIT */
    0x50,  /* VK_LEFT */
    0x4F,  /* VK_RIGHT */
    0x52,  /* VK_UP */
    0x51,  /* VK_DOWN */
    0x00,  /* VK_MOUSE_EN */
    0x7F,  /* VK_VOLUME_MUTE */
    0x80,  /* VK_VOLUME_UP */
    0x81,  /* VK_VOLUME_DOWN */
    0x3F,  /* VK_VOICE_STOP */
    0x00,  /* VK_TV_POWER */
    0x00,  /* VK_TV_SIGNAL */
    0x6c,  /* VK_NETFLIX */
    0x6d,  /* VK_YOUTUBE */
    0x6e,  /* VK_APP04 */
#if FEATURE_SUPPORT_MULTIMEDIA_KEYBOARD
    0xb5,    /* MM_ScanNext */
    0xb6,    /* MM_ScanPrevious */
    0xb7,    /* MM_Stop */
    0xcd,    /* MM_Play_Pause */
    0xe2,    /* MM_Mute */
    0xe5,    /* MM_BassBoost */
    0xe7,    /* MM_Loudness */
    0xe9,    /* MM_VolumeIncrement */
    0xea,    /* MM_VolumeDecrement */
    0x0152,  /* MM_BassIncrement */
    0x0153,  /* MM_BassDecrement */
    0x0154,  /* MM_TrebleIncrement */
    0x0155,  /* MM_TrebleDecrement */
    0x0183,  /* MM_AL_ConsumerControl */
    0x018a,  /* MM_AL_EmailReader */
    0x0192,  /* MM_AL_Calculator */
    0x0194,  /* MM_AL_LocalMachineBrowser */
    0x0221,  /* MM_AC_Search */
    0x0223,  /* MM_AC_Home */
    0x0224,  /* MM_AC_Back */
    0x0225,  /* MM_AC_Forward */
    0x0226,  /* MM_AC_Stop */
    0x0227,  /* MM_AC_Refresh */
    0x022a,  /* MM_AC_Bookmarks */
    0x01a2,  /* MM_Dashboard */
    0x42,    /* MM_DPadUp */
    0x43,    /* MM_DPadDown */
    0x44,    /* MM_DPadLeft */
    0x45,    /* MM_DPadRight */
    0x41,    /* MM_DPadCenter */
    0x8d,    /* MM_Guide */
    0x89,    /* MM_Live */
    0x00,    /* MM_BugReport */
#endif
#endif
};

T_KEY_HANDLE_GLOBAL_DATA key_handle_global_data;  /* Value to indicate the reconnection key data */
TimerHandle_t combine_keys_detection_timer;
TimerHandle_t notify_key_data_after_reconn_timer;

/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/**
 * @brief handle key event in BLE idle state
 * @param key_index - pressed key index
 * @return none
 * @retval void
 */
static void key_handle_one_key_in_idle_status(T_KEY_INDEX_DEF key_index)
{
#if IR_LEARN_MODE
    if (ir_learn_press_handle(key_index, VK_NC, 1))
    {
        /* in IR learning mode, return */
        return;
    }
#endif

    if (app_global_data.is_link_key_existed)
    {
        key_handle_global_data.reconn_key_index =  key_index;
#if FEATURE_SUPPORT_POWER_ON_ADV
        if (key_index == VK_POWER)
        {
            rcu_start_adv(ADV_UNDIRECT_POWER);
        }
        else
#endif
        {
#if FEATURE_SUPPORT_PRIVACY
            rcu_start_adv(ADV_UNDIRECT_RECONNECT);
#else
            rcu_start_adv(ADV_DIRECT_HDC);
            app_global_data.direct_adv_cnt = 1;
#endif
        }
    }
#if FEATURE_SUPPORT_ANY_KEY_TRIG_PAIRING_ADV
    else
    {
        app_global_data.pair_failed_retry_cnt = 0;
        rcu_start_adv(ADV_UNDIRECT_PAIRING);
    }
#endif

#if IR_FUN
#if (RCU_HD_PLATFORM_SEL == G_MIN_DEMO_RCU)
    if ((key_index != MM_AC_Search) || (app_global_data.is_link_key_existed == false))
#else
    if ((key_index != VK_VOICE) || (app_global_data.is_link_key_existed == false))
#endif
    {
        ir_send_key_press_handle(key_index, VK_NC, 1);  /*send IR wave, period send if long press*/
        key_handle_global_data.reconn_key_index =
            VK_NC;  /* reset reconn_key_index to avoid duplicate key data */
    }
#endif
}

/**
 * @brief handle key event in BLE adv state
 * @param key_index - pressed key index
 * @return none
 * @retval void
 */
static void key_handle_one_key_in_adv_status(T_KEY_INDEX_DEF key_index)
{
#if IR_LEARN_MODE
    if (ir_learn_press_handle(key_index, VK_NC, 1))
    {
        /* in IR learning mode, return */
        return;
    }
#endif

#if FEATURE_SUPPORT_POWER_ON_ADV
    if (key_index == VK_POWER)
    {
        switch (app_global_data.adv_type)
        {
        case ADV_DIRECT_HDC:
            {
                app_global_data.stop_adv_reason = STOP_ADV_REASON_POWERKEY;
            }
            break;
        case ADV_UNDIRECT_PROMPT:
        case ADV_UNDIRECT_RECONNECT:
            {
                os_timer_stop(&adv_timer);
                rcu_stop_adv(STOP_ADV_REASON_POWERKEY);
            }
            break;
        default:
            {
                /* do nothing */
            }
            break;
        }
    }
    else
#endif
    {
        if ((app_global_data.adv_type == ADV_DIRECT_HDC)
            || (app_global_data.adv_type == ADV_UNDIRECT_RECONNECT))
        {
            /* update reconn_key_index */
            key_handle_global_data.reconn_key_index = key_index;
        }
    }

#if IR_FUN
#if (RCU_HD_PLATFORM_SEL == G_MIN_DEMO_RCU)
    if ((key_index != MM_AC_Search) || (app_global_data.is_link_key_existed == false))
#else
    if ((key_index != VK_VOICE) || (app_global_data.is_link_key_existed == false))
#endif
    {
        ir_send_key_press_handle(key_index, VK_NC, 1);  /*send IR wave, period send if long press*/
        key_handle_global_data.reconn_key_index =
            VK_NC;  /* reset reconn_key_index to avoid duplicate key data */
    }
#endif
}

/**
 * @brief handle one key pressed scenario
 * @param key_index - pressed key index
 * @return none
 * @retval void
 */
static void key_handle_one_key_scenario(T_KEY_INDEX_DEF key_index)
{
    APP_PRINT_INFO1("[key_handle_one_key_scenario] rcu status is %d", app_global_data.rcu_status);
    switch (app_global_data.rcu_status)
    {
    case RCU_STATUS_IDLE:
        {
            key_handle_one_key_in_idle_status(key_index);
        }
        break;
    case RCU_STATUS_ADVERTISING:
        {
            key_handle_one_key_in_adv_status(key_index);
        }
        break;
    case RCU_STATUS_PAIRED:
        {
#if IR_LEARN_MODE
            if (ir_learn_press_handle(key_index, VK_NC, 1))
            {
                /* in IR learning mode, return */
                return;
            }
#endif
#if RCU_VOICE_EN
#if (RCU_HD_PLATFORM_SEL == G_MIN_DEMO_RCU)
            if (key_index == MM_AC_Search)
#else
            if (key_index == VK_VOICE)
#endif
            {
                if ((app_global_data.is_keyboard_notify_en) && (app_global_data.is_voice_notify_en))
                {
                    voice_handle_mic_key_pressed();
                }
                else
                {
                    APP_PRINT_INFO2("[key_handle_one_key_scenario] not allow to start voice recording %d %d",
                                    app_global_data.is_keyboard_notify_en, app_global_data.is_voice_notify_en);
                }
            }
            else
#endif
            {
                key_handle_notfiy_key_data(key_index);
            }
        }
        break;
    default:
        {
#if IR_LEARN_MODE
            if (ir_learn_press_handle(key_index, VK_NC, 1))
            {
                /* in IR learning mode, return */
                return;
            }
#endif
#if IR_FUN
            ir_send_key_press_handle(key_index, VK_NC, 1);  /*send IR wave, period send if long press*/
#endif
        }
        break;
    }
}

/******************************************************************
 * @brief  key handle process factory reset event
 * @param  none
 * @return none
 */
void key_handle_process_factory_reset_event(void)
{
    /* reset FTL section */
    if (app_global_data.is_link_key_existed)
    {
        app_global_data.is_link_key_existed = false;
        le_bond_clear_all_keys();
    }

#if FEATURE_SUPPORT_MP_TEST_MODE
    if (true == mp_test_is_test_mode_flag_en())
    {
        mp_test_disable_test_mode_flag();
    }
#endif

#if IR_LEARN_MODE
    ir_learn_reset_ftl_learning_data();
#endif

    WDG_SystemReset(RESET_ALL_EXCEPT_AON, RESET_REASON_FACTORY_RESET);
}

#if (RCU_HD_PLATFORM_SEL == G_MIN_DEMO_RCU)
/******************************************************************
 * @brief  key handle process bug report event
 * @param  none
 * @return none
 */
void key_handle_process_bug_report_event()
{
#if IR_FUN
    if (app_global_data.rcu_status != RCU_STATUS_PAIRED ||
        app_global_data.rcu_status != RCU_STATUS_CONNECTED)
    {
        ir_send_key_press_handle(MM_BugReport, VK_NC, 1);
    }
#endif
}
#endif

/**
 * @brief handle two keys pressed scenario
 * @param key_index - pressed key index
 * @return none
 * @retval void
 */
static void key_handle_two_keys_scenario(T_KEY_INDEX_DEF key_index_1, T_KEY_INDEX_DEF key_index_2)
{
    uint32_t timeout_ms = COMBINE_KEYS_DETECT_TIMEOUT;

#if (RCU_HD_PLATFORM_SEL == G_MIN_DEMO_RCU)
    if (((key_index_1 == MM_AC_Home && key_index_2 == MM_AC_Back) ||
         (key_index_2 == MM_AC_Home && key_index_1 == MM_AC_Back)))
    {
        key_handle_global_data.combine_keys_status = PAIRING_COMBINE_KEYS_BIT_MASK;
    }
    else if (((key_index_1 == MM_AC_Search && key_index_2 == MM_Mute) ||
              (key_index_2 == MM_AC_Search && key_index_1 == MM_Mute)))
    {
        key_handle_global_data.combine_keys_status = FACTORY_RESET_COMBINE_KEYS_BIT_MASK;

        /* update timeout period */
        timeout_ms = FACTORY_RESET_KEYS_DETECT_TIMEOUT;
    }
    else if (((key_index_1 == MM_DPadCenter && key_index_2 == MM_AC_Back) ||
              (key_index_2 == MM_DPadCenter && key_index_1 == MM_AC_Back)))
    {
        key_handle_global_data.combine_keys_status = BUG_REPORT_COMBINE_KEYS_BIT_MASK;

        /* update timeout period */
        timeout_ms = BUG_REPOERT_DETECT_TIMEOUT;
#if IR_FUN
        if (is_ir_send_working())
        {
            ir_send_key_release_handle();
        }
#endif
    }
#else
    if (((key_index_1 == VK_ENTER && key_index_2 == VK_VOLUME_UP) ||
         (key_index_2 == VK_ENTER && key_index_1 == VK_VOLUME_UP)))
    {
        key_handle_global_data.combine_keys_status = PAIRING_COMBINE_KEYS_BIT_MASK;

    }
#endif
#if IR_LEARN_MODE
    else if (((key_index_1 == VK_ENTER && key_index_2 == VK_LEFT) ||
              (key_index_2 == VK_ENTER && key_index_1 == VK_LEFT)))
    {
        key_handle_global_data.combine_keys_status = IR_LEARNING_COMBINE_KEYS_BIT_MASK;
    }
#endif

#if FEATURE_SUPPORT_MP_TEST_MODE
    if (key_handle_global_data.combine_keys_status == INVALID_COMBINE_KEYS_BIT_MASK)
    {
        key_handle_global_data.combine_keys_status = mp_test_handle_two_keys_scenario(key_index_1,
                                                                                      key_index_2);
    }
#endif

    APP_PRINT_INFO1("[key_handle_two_keys_scenario] combine_keys_status is 0x%04X",
                    key_handle_global_data.combine_keys_status);
    if (key_handle_global_data.combine_keys_status != INVALID_COMBINE_KEYS_BIT_MASK)
    {
        /* start combine keys dectecion timer */
        os_timer_restart(&combine_keys_detection_timer, timeout_ms);
    }
}

/**
 * @brief    key handle combine keys detected timer callback
 * @param    p_timer - point of timer
 * @return   none
 * @retval   void
 * Caution   do NOT excute time consumption functions in timer callback
 */
static void key_handle_comb_keys_timer_cb(TimerHandle_t p_timer) DATA_RAM_FUNCTION;
static void key_handle_comb_keys_timer_cb(TimerHandle_t p_timer)
{
    /* check combine keys status */
    APP_PRINT_INFO1("[key_handle_comb_keys_timer_cb] combine_keys_status is 0x%04X",
                    key_handle_global_data.combine_keys_status);

    if (key_handle_global_data.combine_keys_status == PAIRING_COMBINE_KEYS_BIT_MASK)
    {
        app_global_data.pair_failed_retry_cnt = 0;
        switch (app_global_data.rcu_status)
        {
        case RCU_STATUS_IDLE:
            {
                rcu_start_adv(ADV_UNDIRECT_PAIRING);
            }
            break;
        case RCU_STATUS_ADVERTISING:
            {
                if (app_global_data.adv_type == ADV_UNDIRECT_PAIRING)
                {
                    os_timer_restart(&adv_timer, ADV_UNDIRECT_PAIRING_TIMEOUT);
                }
                else
                {
                    if (app_global_data.adv_type != ADV_DIRECT_HDC)
                    {
                        os_timer_stop(&adv_timer);
                    }
                    rcu_stop_adv(STOP_ADV_REASON_PAIRING);
                }
            }
            break;
        case RCU_STATUS_PAIRED:
            {
                rcu_terminate_connection(DISCONN_REASON_PAIRING);
            }
            break;
        default:
            /* do nothing */
            break;
        }
    }
#if IR_LEARN_MODE
    else if (key_handle_global_data.combine_keys_status == IR_LEARNING_COMBINE_KEYS_BIT_MASK)
    {
        if (is_ir_learn_working())
        {
            ir_learn_exit();
        }
        else
        {
            /* ir learn module init and start */
            ir_learn_module_init();
        }
    }
#endif
#if (RCU_HD_PLATFORM_SEL == G_MIN_DEMO_RCU)
    else if (key_handle_global_data.combine_keys_status == FACTORY_RESET_COMBINE_KEYS_BIT_MASK)
    {
        key_handle_process_factory_reset_event();
    }
    else if (key_handle_global_data.combine_keys_status == BUG_REPORT_COMBINE_KEYS_BIT_MASK)
    {
        key_handle_process_bug_report_event();
    }
#endif
#if FEATURE_SUPPORT_MP_TEST_MODE
    mp_test_handle_comb_keys_timer_cb(key_handle_global_data.combine_keys_status);
#endif
}

/**
 * @brief    notify key data after reconn timer callback
 * @param    p_timer - point of timer
 * @return   none
 * @retval   void
 * Caution   do NOT excute time consumption functions in timer callback
 */
static void notify_key_data_timer_cb(TimerHandle_t p_timer) DATA_RAM_FUNCTION;
static void notify_key_data_timer_cb(TimerHandle_t p_timer)
{
    APP_PRINT_INFO3("[notify_key_data_timer_cb] rcu_status is %d, reconn_key_index is %d, last_pressed_key_index is %d",
                    app_global_data.rcu_status, key_handle_global_data.reconn_key_index,
                    key_handle_global_data.last_pressed_key_index);

    if ((app_global_data.rcu_status == RCU_STATUS_PAIRED) &&
        (key_handle_global_data.reconn_key_index != VK_NC))
    {
        if (key_handle_global_data.last_pressed_key_index == VK_NC)
        {
            /* reconnection key is released, need to reissue press and release event */
#if RCU_VOICE_EN
#if (RCU_HD_PLATFORM_SEL == G_MIN_DEMO_RCU)
            if (key_handle_global_data.reconn_key_index == MM_AC_Search)
#else
            if (key_handle_global_data.reconn_key_index == VK_VOICE)
#endif
            {
#if (VOICE_FLOW_SEL == ATV_GOOGLE_VOICE_FLOW)
                if (ATV_ASSISTANT_INTERACTION_MODEL_HOLD_TO_TALK != atvv_global_data.assistant_interaction_model)
                {
                    /* reissue voice key event even key released before reconnection for ATV Voice flow */
                    voice_handle_mic_key_pressed();
                }
#endif
            }
            else
#endif
            {
                APP_PRINT_INFO0("[notify_key_data_timer_cb] Reissue press and release event");
                key_handle_notfiy_key_data(key_handle_global_data.reconn_key_index);
                key_handle_global_data.last_pressed_key_index = key_handle_global_data.reconn_key_index;
                key_handle_notfiy_key_data(VK_NC);
            }
        }
        else
        {
            /* reconnection key is NOT released, need to reissue press event */
#if RCU_VOICE_EN
#if (RCU_HD_PLATFORM_SEL == G_MIN_DEMO_RCU)
            if (key_handle_global_data.reconn_key_index == MM_AC_Search)
#else
            if (key_handle_global_data.reconn_key_index == VK_VOICE)
#endif
            {
                voice_handle_mic_key_pressed();
            }
            else
#endif
            {
                APP_PRINT_INFO0("[notify_key_data_timer_cb] Reissue press event");
                key_handle_notfiy_key_data(key_handle_global_data.reconn_key_index);
            }
        }

        key_handle_global_data.reconn_key_index = VK_NC;
    }
}

/*============================================================================*
 *                              Global Functions
 *============================================================================*/
/**
* @brief  Initialize key handle data
*/
void key_handle_init_data(void)
{
    APP_PRINT_INFO0("[key_handle_init_data] init data");
    memset(&key_handle_global_data, 0, sizeof(key_handle_global_data));
}

/**
 * @brief    key handler init timer
 */
void key_handle_init_timer(void)
{
    APP_PRINT_INFO0("[key_handle_init_timer] init timer");
    /* combine_keys_detection_timer is used to detect combine keys after timeout */
    if (false == os_timer_create(&combine_keys_detection_timer, "combine_keys_detection_timer",  1, \
                                 COMBINE_KEYS_DETECT_TIMEOUT, false, key_handle_comb_keys_timer_cb))
    {
        APP_PRINT_ERROR0("[key_handle_init_timer] combine_keys_detection_timer creat failed!");
    }

    /* notify_key_data_after_reconn_timer is used to notify key data after timeout */
    if (false == os_timer_create(&notify_key_data_after_reconn_timer,
                                 "notify_key_data_after_reconn_timer",  1, \
                                 NOTIFY_KEY_DATA_TIMEOUT, false, notify_key_data_timer_cb))
    {
        APP_PRINT_ERROR0("[key_handle_init_timer] notify_key_data_after_reconn_timer creat failed!");
    }
}

/**
* @brief   notify key data
* @param   key_index - index of key
* @return  true or false
*/
bool key_handle_notfiy_key_data(T_KEY_INDEX_DEF key_index)
{
    bool result = false;
    uint16_t key_data = BLE_KEY_CODE_TABLE[key_index];
    T_KEY_INDEX_DEF pre_key_index = key_handle_global_data.last_pressed_key_index;

    APP_PRINT_INFO3("[key_handle_notfiy_key_data] key_index %d, key_code 0x%x, pre_key_index = %d",
                    key_index, key_data, pre_key_index);

    if (((key_index != VK_NC) && (key_index < BLE_MM_START_KEY_INDEX))
        || ((key_index == VK_NC) && (pre_key_index < BLE_MM_START_KEY_INDEX)))
    {
        /* normal key code */
        if (app_global_data.is_keyboard_notify_en)
        {
            uint8_t notfiy_key_data[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
            notfiy_key_data[2] = (uint8_t) key_data;

            result = server_send_data(0, app_global_data.hid_srv_id, GATT_SRV_HID_KB_INPUT_INDEX, \
                                      notfiy_key_data, sizeof(notfiy_key_data), GATT_PDU_TYPE_NOTIFICATION);
            if (result == false)
            {
                APP_PRINT_WARN0("[key_handle_notfiy_key_data] server_send_data failed!");
            }
        }
        else
        {
            APP_PRINT_WARN0("[key_handle_notfiy_key_data] is_keyboard_notify_en is disbaled!");
            result = false;
        }
    }
#if FEATURE_SUPPORT_MULTIMEDIA_KEYBOARD
    else
    {
        /* multimedia key code */
        if (app_global_data.is_mm_keyboard_notify_en)
        {
            uint8_t mm_key_data[4] = {0x00, 0x00, 0x00, 0x00};

            mm_key_data[0] = (uint8_t)(key_data & 0xff);
            mm_key_data[1] = (uint8_t)((key_data >> 8) & 0xff);

            result = server_send_data(0, app_global_data.hid_srv_id, GATT_SRV_HID_MM_KB_INPUT_INDEX, \
                                      mm_key_data, sizeof(mm_key_data), GATT_PDU_TYPE_NOTIFICATION);
            if (result == false)
            {
                APP_PRINT_INFO0("[key_handle_notfiy_key_data] server_send_data failed!");
            }
        }
        else
        {
            APP_PRINT_WARN0("[key_handle_notfiy_key_data] is_mm_keyboard_notify_en is disbaled!");
            result = false;
        }
    }
#endif
#if (VOICE_FLOW_SEL == ATV_GOOGLE_VOICE_FLOW)
    if (result == true && atvv_global_data.app_support_version == ATVV_VERSION_0_4)
    {
#if (RCU_HD_PLATFORM_SEL == G_MIN_DEMO_RCU)
        if (key_index == MM_DPadCenter)
#else
        if (key_index == VK_ENTER)
#endif
        {
            bool notify_result = false;
            memset(atvv_global_data.char_ctl_data_buff, 0, ATVV_CHAR_CTL_DATA_LEN);
            atvv_global_data.char_ctl_data_buff[0] = ATV_CTL_OPCODE_DPAD_SELECT;

            notify_result = server_send_data(0, app_global_data.atvv_srv_id, GATT_SVC_ATVV_CHAR_CTL_VALUE_INDEX,
                                             \
                                             atvv_global_data.char_ctl_data_buff, sizeof(atvv_global_data.char_ctl_data_buff),
                                             GATT_PDU_TYPE_NOTIFICATION);
            if (notify_result == false)
            {
                APP_PRINT_WARN0("[key_handle_notfiy_key_data] ATV_CTL_OPCODE_DPAD_SELECT notify failed!");
            }
            else
            {
                APP_PRINT_INFO0("[key_handle_notfiy_key_data] ATV_CTL_OPCODE_DPAD_SELECT notify success!");
            }
        }
    }
#endif

    return result;
}

/**
* @brief   handle key pressed event
* @param   p_keyscan_fifo_data - point of keyscan FIFO data
* @return  void
*/
void key_handle_pressed_event(T_KEYSCAN_FIFO_DATA *p_keyscan_fifo_data)
{
    T_KEYSCAN_FIFO_DATA keyscan_fifo_data;
    memcpy(&keyscan_fifo_data, p_keyscan_fifo_data, sizeof(T_KEYSCAN_FIFO_DATA));
    APP_PRINT_INFO1("[key_handle_pressed_event] keyscan FIFO length is %d", keyscan_fifo_data.len);

    LED_ON(LED_1);

    for (uint8_t index = 0; index < (keyscan_fifo_data.len); index++)
    {
        APP_PRINT_INFO4("[key_handle_pressed_event] keyscan data[%d]: row - %d, column - %d, value - %d", \
                        index, keyscan_fifo_data.key[index].row, keyscan_fifo_data.key[index].column,
                        KEY_MAPPING_TABLE[keyscan_fifo_data.key[index].row][keyscan_fifo_data.key[index].column]);
    }

    T_KEY_INDEX_DEF key_index_1 =
        KEY_MAPPING_TABLE[keyscan_fifo_data.key[0].row][keyscan_fifo_data.key[0].column];
    T_KEY_INDEX_DEF key_index_2 =
        KEY_MAPPING_TABLE[keyscan_fifo_data.key[1].row][keyscan_fifo_data.key[1].column];

    /* check if need to reissue key release event */
    if ((key_handle_global_data.last_pressed_key_index != VK_NC) &&
        ((keyscan_fifo_data.len != 1) || (key_handle_global_data.last_pressed_key_index != key_index_1)))
    {
        key_handle_release_event();
    }

#if BAT_EN
    /*check battery module by key triggled*/
    {
        rcu_bat_handle(IO_MSG_TYPE_BAT_DETECT);
        if (RCU_STATUS_LOW_POWER == app_global_data.rcu_status)
        {
            return;
        }
    }
#endif

    if (keyscan_fifo_data.len == 0)
    {
        APP_PRINT_WARN0("[key_handle_pressed_event] FIFO length is 0!");
    }
    else if (keyscan_fifo_data.len == 1)
    {
        key_handle_global_data.last_pressed_key_index = key_index_1;
        key_handle_one_key_scenario(key_index_1);
    }
    else if (keyscan_fifo_data.len == 2)
    {
        key_handle_two_keys_scenario(key_index_1, key_index_2);
    }
    else
    {
        /* more than two keys are pressed, just ignore this scenario.
           If need to use three or more keys as combined keys, need
           to caution ghost keys!
        */
    }
}

/**
* @brief   handle key release event
* @param   none
* @return  void
*/
void key_handle_release_event(void)
{
    LED_OFF(LED_1);

    if (key_handle_global_data.last_pressed_key_index != VK_NC)
    {
        APP_PRINT_INFO0("[key_handle_release_event] key release event");

#if IR_FUN
        if (is_ir_send_working())
        {
            ir_send_key_release_handle();
        }
        else
#endif
#if RCU_VOICE_EN
            if (voice_driver_global_data.is_voice_driver_working == true) /*voice working or not*/
            {
                voice_handle_mic_key_released();
            }
            else
#endif
                if (app_global_data.rcu_status == RCU_STATUS_PAIRED)
                {
                    key_handle_notfiy_key_data(VK_NC);
                }

#if IR_LEARN_MODE
        ir_learn_key_release_handle();
#endif
        key_handle_global_data.last_pressed_key_index = VK_NC;
    }
#if (RCU_HD_PLATFORM_SEL == G_MIN_DEMO_RCU)
#if IR_FUN
    if (is_ir_send_working())
    {
        ir_send_key_release_handle();
    }
#endif
#endif
}
