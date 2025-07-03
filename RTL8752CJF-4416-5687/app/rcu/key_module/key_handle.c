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
#include "os_sched.h"
#if IR_FUN
#include "ir_app_config.h"
#endif
#include "led_driver.h"
#if FEATURE_SUPPORT_MP_TEST_MODE
#include "mp_test.h"
#include "single_tone.h"
#endif
#if (VOICE_FLOW_SEL == ATV_GOOGLE_VOICE_FLOW)
#include "atvv_service.h"
#endif
#include <app_section.h>
#include "bas.h"
#include "ftl.h"
#include "smartrcu_handle.h"
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
#elif (RCU_HD_PLATFORM_SEL == SRC_4416_5302)
static const T_KEY_INDEX_DEF KEY_MAPPING_TABLE[KEYPAD_ROW_SIZE][KEYPAD_COLUMN_SIZE] =
{
    {MM_PREVIOUS,        MM_Play_Pause,  MM_FAST_FORWARD,MM_VolumeIncrement,MM_STOP,         MM_P_Increment,         VK_NC},
    {MM_VolumeDecrement, MM_Mute,        MM_P_Decrement, MM_AC_Search,      MM_DPadUp,       MM_AC_Home,             VK_NC},
    {MM_DPadLeft,        MM_DPadCenter,  MM_DPadRight,   MM_AC_Back,        MM_DPadDown,     MM_Setting,             VK_NC},
    {MM_Bookmark,        MM_INFO,        MM_Guide,       MM_RED,            MM_GREEN,        MM_YELLOW,              MM_BLUE},
    {MM_YouTube,         MM_PrimeVideo,  MM_FAVORITES,   MM_NETFLIX,        MM_Disney,       MM_Google_Play_Games,   VK_NC},
    {MM_NUMBER1,         MM_NUMBER2,     MM_NUMBER3,     MM_NUMBER4,        MM_NUMBER5,      MM_NUMBER6,             MM_POWER},
    {MM_NUMBER7,         MM_NUMBER8,     MM_NUMBER9,     MM_REWIND,         MM_NUMBER0,      MM_NEXT,                VK_NC},
};
#elif (RCU_HD_PLATFORM_SEL == SRC_4416_5291)
static const T_KEY_INDEX_DEF KEY_MAPPING_TABLE[KEYPAD_ROW_SIZE][KEYPAD_COLUMN_SIZE] =
{
    {MM_RW,              MM_Play_Pause,  MM_FF,          MM_VolumeIncrement,MM_REC,      MM_P_Increment,VK_NC},
    {MM_VolumeDecrement, MM_Mute,        MM_P_Decrement, MM_DPadMenu,       MM_DPadUp,   MM_EXIT,       VK_NC},
    {MM_DPadLeft,        MM_DPadCenter,  MM_DPadRight,   MM_AC_Back,        MM_DPadDown, MM_AC_Home,    VK_NC},
    {MM_FAVORITES,       MM_AC_Search,   MM_INFO,        MM_RED,            MM_GREEN,    MM_YELLOW,     MM_BLUE},
    {MM_Guide,           MM_YouTube,     MM_PrimeVideo,  MM_Source,         MM_NETFLIX,  MM_DISNEY,     VK_NC},
    {MM_NUMBER1,         MM_NUMBER2,     MM_NUMBER3,     MM_NUMBER4,        MM_NUMBER5,  MM_NUMBER6,    MM_POWER},
    {MM_NUMBER7,         MM_NUMBER8,     MM_NUMBER9,     MM_TEXT,           MM_NUMBER0,  MM_SUB,        VK_NC},
};
#elif (RCU_HD_PLATFORM_SEL == SRC_3925_5879)
static const T_KEY_INDEX_DEF KEY_MAPPING_TABLE[KEYPAD_ROW_SIZE][KEYPAD_COLUMN_SIZE] =
{
    {VK_NUMBER1,         VK_NUMBER2,     VK_NUMBER3,     VK_NUMBER4,        VK_NUMBER5,      VK_NUMBER6},
    {VK_NUMBER7,         VK_NUMBER8,     VK_NUMBER9,     MM_INFO,           VK_NUMBER0,      MM_SWITCH},
    {VK_VolumeIncrement, MM_Source,      MM_P_Increment, VK_VolumeDecrement,MM_MENU,         MM_P_Decrement},
    {MM_CONTACT,         MM_AC_Search,   MM_Setting,     MM_DPadLeft,       MM_DPadCenter,   MM_DPadRight},
    {MM_DPadUp,          MM_DPadDown,    MM_NETFLIX,     MM_AC_Back,        MM_AC_Home,      MM_TV},
    {MM_YouTube,         MM_PrimeVideo,  MM_YouTubeMusic,MM_RED,            MM_GREEN,        MM_YELLOW},
    {MM_POWER,           MM_Mute,        VK_NC,          VK_NC,             VK_NC,           MM_BLUE},
};
#endif

/*============================================================================*
 *                            Left  Global Variables
 *============================================================================*/
/* BLE HID code table definition */
const uint16_t BLE_KEY_CODE_TABLE[BLE_KEY_CODE_TABLE_SIZE] =
{
#if (RCU_HD_PLATFORM_SEL == SRC_4416_5302)
    0x00,  /* VK_NC */
    0x00,  /* VK_POWER */
    0x4B,  /* VK_PAGE_UP */
    0x4E,  /* VK_PAGE_DOWN */
    0x4A,  /* VK_HOME */
    0x86,  /* VK_MENU */
    0x3E,  /* VK_VOICE */
    0x28,  /* VK_ENTER */
#if FEATURE_SUPPORT_MULTIMEDIA_KEYBOARD
    0xb5,    /* MM_ScanNext */
    0x0030,    /* MM_POWER */
    0x0037,    /* MM_NUMBER1 */
    0x0038,    /* MM_NUMBER2 */
    0x0039,    /* MM_NUMBER3 */
    0x003a,    /* MM_NUMBER4 */
    0x003b,    /* MM_NUMBER5 */
    0x003c,    /* MM_NUMBER6 */
    0x003d,    /* MM_NUMBER7 */
    0x003e,    /* MM_NUMBER8 */
    0x003f,    /* MM_NUMBER9 */
    0x00D6,    /* MM_REWIND */
    0x000f,    /* MM_NUMBER0 */
    0x00b3,    /* MM_FAST_FORWARD */
    0x00B4,    /* MM_PREVIOUS */
    0x00cd,    /* MM_Play_Pause */
    0x00d5,    /* MM_NEXT */
    0x00E9,    /* MM_VolumeIncrement */
    0x00EA,    /* MM_VolumeDecrement */
    0x00B2,    /* MM_STOP */
    0x00E2,    /* MM_Mute */
    0x009C,    /* MM_P_Increment */
    0x009D,    /* MM_P_Decrement */
    0x0221,    /* MM_AC_Search */
    0x0223,    /* MM_AC_Home */
    0x0042,    /* MM_DPadUp */
    0x0044,    /* MM_DPadLeft */
    0x0041,    /* MM_DPadCenter */
    0x0045,    /* MM_DPadRight */
    0x0043,    /* MM_DPadDown */
    0x0224,    /* MM_AC_Back */
    0x0007,    /* MM_Setting */
    0x00cf,    /* MM_Bookmark */
    0X0040,    /* MM_INFO */
    0x01a2,    /* MM_Guide */
    0x000c,    /* MM_YouTube */
    0x000a,    /* MM_NETFLIX */
    0x000b,    /* MM_PrimeVideo */
    0x0013,    /* MM_Disney */
    0x002c,    /* MM_FAVORITES */
    0X0015,    /* MM_Google_Play_Games */
    0x00a5,    /* MM_RED */
    0x00a6,    /* MM_GREEN */
    0X00a7,    /* MM_YELLOW */
    0x00a8,    /* MM_BLUE */
    0x00B7,    /* MM_REC_THREE_SEC */
    0x00,      /* MM_AC_Search_Noconnect */ 
    0x00,      /* MM_BugReport */
#endif
#elif (RCU_HD_PLATFORM_SEL == SRC_4416_5291)
    0x00,  /* VK_NC */
    0x00,  /* VK_POWER */
    0x4B,  /* VK_PAGE_UP */
    0x4E,  /* VK_PAGE_DOWN */
    0x4A,  /* VK_HOME */
    0x86,  /* VK_MENU */
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
    0x27,  /* VK_NUMBER0 */
    0x1E,  /* VK_NUMBER1 */
    0x1F,  /* VK_NUMBER2 */
    0x20,  /* VK_NUMBER3 */
    0x21,  /* VK_NUMBER4 */
    0x22,  /* VK_NUMBER5 */
    0x23,  /* VK_NUMBER6 */
    0x24,  /* VK_NUMBER7 */
    0x25,  /* VK_NUMBER8 */
    0x26,  /* VK_NUMBER9 */
    0x69,  /* VK_RED */
    0x6A,  /* VK_GREEN */
    0x6B,  /* VK_YELLOW */
    0X61,  /* VK_SUB */
    0x6C,  /* VK_BLUE */
#if FEATURE_SUPPORT_MULTIMEDIA_KEYBOARD
    0xb5,    /* MM_ScanNext */
    0x0030,    /* MM_POWER */
    0x0073,    /* MM_NUMBER1 */
    0x0074,    /* MM_NUMBER2 */
    0x0075,    /* MM_NUMBER3 */
    0x0082,    /* MM_NUMBER4 */
    0x0042,    /* MM_NUMBER5 */
    0x0043,    /* MM_NUMBER6 */
    0x0044,    /* MM_NUMBER7 */
    0x0045,    /* MM_NUMBER8 */
    0x0090,    /* MM_NUMBER9 */
    0x021A,    /* MM_TEST */
    0x0094,    /* MM_NUMBER0 */
    0x0061,    /* MM_SUB */
    0x00B4,    /* MM_RW */
    0x0064,    /* MM_PLAY_PAUSE */
    0x00B3,    /* MM_FF */
    0x00E9,    /* MM_VOL_UP */
    0x00EA,    /* MM_VOL_DOWN */
    0x00B2,    /* MM_REC */
    0x00E2,    /* MM_MUTE */
    0x009C,    /* MM_P_UP */
    0x009D,    /* MM_P_DOWN */
    0x0096,    /* MM_MENU */
    0x0204,    /* MM_EXIT */
    0x0088,    /* MM_UP */
    0x008E,    /* MM_LEFT */
    0x0041,    /* MM_OK */
    0x008F,    /* MM_RIGHT */
    0x008B,    /* MM_DOWN */
    0x0098,    /* MM_BACK */
    0x0223,    /* MM_HOME */
    0x00A1,    /* MM_FAVORITES */
    0X00CF,    /* MM_VOICE */
    0x0060,    /* MM_INFO */
    0x0095,    /* MM_GUIDE */
    0x0097,    /* MM_SOURSE */
    0x021F,    /* MM_NETFLIX */
    0x0209,    /* MM_YOUTUBE */
    0x0062,    /* MM_PRIME_VIDEO */
    0X00A5,    /* MM_DISNEY */
    0x0069,    /* MM_RED */
    0x006A,    /* MM_GREEN */
    0X006B,    /* MM_YELLOW */
    0x006C,    /* MM_BLUE */
    0x00B7,    /* MM_REC_THREE_SEC */
    0x00,      /* MM_AC_Search_Noconnect */ 
    0x00,      /* MM_BugReport */
#endif
#elif (RCU_HD_PLATFORM_SEL == SRC_3925_5879)
    0x00,      /* VK_NC */
    0x1E,      /* VK_NUMBER1 */
    0x1F,      /* VK_NUMBER2 */
    0x20,      /* VK_NUMBER3 */
    0x21,      /* VK_NUMBER4 */
    0x22,      /* VK_NUMBER5 */
    0x23,      /* VK_NUMBER6 */
    0x24,      /* VK_NUMBER7 */
    0x25,      /* VK_NUMBER8 */
    0x26,      /* VK_NUMBER9 */
    0x27,      /* VK_NUMBER0 */
    0x80,      /* VK_VolumeIncrement */
    0x81,      /* VK_VolumeDecrement */
#if FEATURE_SUPPORT_MULTIMEDIA_KEYBOARD
    0x00,      /* MM_ScanNext */
    0x00,      /* MM_POWER */
    0x00E2,    /* MM_Mute */
    0x00,      /* MM_NUMBER1 */
    0x00,      /* MM_NUMBER2 */
    0x00,      /* MM_NUMBER3 */
    0x00,      /* MM_NUMBER4 */
    0x00,      /* MM_NUMBER5 */
    0x00,      /* MM_NUMBER6 */
    0x00,      /* MM_NUMBER7 */
    0x00,      /* MM_NUMBER8 */
    0x00,      /* MM_NUMBER9 */
    0x01BD,    /* MM_INFO */
    0x00,      /* MM_NUMBER0 */
    0x0090,    /* MM_SWITCH */
    0x00,      /* MM_VolumeIncrement */
    0x01BB,    /* MM_Source */
    0x009C,    /* MM_P_Increment */
    0x00,      /* MM_VolumeDecrement */
    0x0040,    /* MM_MENU */
    0x009D,    /* MM_P_Decrement */
    0x019C,    /* MM_CONTACT */
    0x0221,    /* MM_AC_Search */
    0x009F,    /* MM_Setting */
    0x0042,    /* MM_DPadUp */
    0x0044,    /* MM_DPadLeft */
    0x0041,    /* MM_DPadCenter */
    0x0045,    /* MM_DPadRight */
    0x0043,    /* MM_DPadDown */
    0x0224,    /* MM_AC_Back */
    0x0223,    /* MM_AC_Home */
    0x008D,    /* MM_TV */
    0x00,      /* MM_NETFLIX */
    0x00,      /* MM_YouTube */
    0X0076,    /* MM_PrimeVideo */
    0x01D2,    /* MM_YouTubeMusic */
    0x0069,    /* MM_RED */
    0x006A,    /* MM_GREEN */
    0x006C,    /* MM_YELLOW */
    0x006B,    /* MM_BLUE */

    0x00,      /* MM_BugReport */
    0x00,      /* MM_TalkBack */
    0x00,      /* MM_POWER_INCERSE */
    0x00,      /* MM_NETFLIX_INCERSE */
    0x00,      /* MM_YouTube_INCERSE */
    0x00,      /* MM_AC_Search_Noconnect */
#endif
#endif
};

T_KEY_HANDLE_GLOBAL_DATA key_handle_global_data;  /* Value to indicate the reconnection key data */
TimerHandle_t combine_keys_detection_timer;
TimerHandle_t notify_key_data_after_reconn_timer;
TimerHandle_t key_MM_DPadCenter_pressed_timer;
TimerHandle_t ambilight_timer;
TimerHandle_t MM_REC_timer;
TimerHandle_t combine_keys_release_timer;
bool special_two_key_press_flag = false;
extern uint8_t app_power_adv_data[];
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
#if (RCU_HD_PLATFORM_SEL == SRC_4416_5302)
    // if(key_index != MM_REC){
        ir_send_key_press_handle(key_index, VK_NC, 1);  /*send IR wave, period send if long press*/
    // }
#elif (RCU_HD_PLATFORM_SEL == SRC_4416_5291)
    ir_send_key_press_handle(key_index, VK_NC, 1);  /*send IR wave, period send if long press*/
#elif (RCU_HD_PLATFORM_SEL == SRC_3925_5879)
    ir_send_key_press_handle(key_index, VK_NC, 1);  /*send IR wave, period send if long press*/
#endif
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
#if (RCU_HD_PLATFORM_SEL == SRC_4416_5302)
    // if ((key_index != MM_AC_Search) || (app_global_data.is_link_key_existed == false))
#elif (RCU_HD_PLATFORM_SEL == SRC_4416_5291)
    // if ((key_index != MM_AC_Search) || (app_global_data.is_link_key_existed == false))
#elif (RCU_HD_PLATFORM_SEL == SRC_3925_5879)
    // if ((key_index != MM_AC_Search) || (app_global_data.is_link_key_existed == false))
#else
    if ((key_index != VK_VOICE) || (app_global_data.is_link_key_existed == false))
#endif
    {
#if (RCU_HD_PLATFORM_SEL == SRC_4416_5302)
        // if(key_index != MM_REC){
            ir_send_key_press_handle(key_index, VK_NC, 1);  /*send IR wave, period send if long press*/
        // }  
#elif (RCU_HD_PLATFORM_SEL == SRC_4416_5291)
        ir_send_key_press_handle(key_index, VK_NC, 1);
#elif (RCU_HD_PLATFORM_SEL == SRC_3925_5879)
        ir_send_key_press_handle(key_index, VK_NC, 1);
#endif        
        key_handle_global_data.reconn_key_index = VK_NC;  /* reset reconn_key_index to avoid duplicate key data */
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
#if (RCU_HD_PLATFORM_SEL == SRC_4416_5302)
    if (app_global_data.is_link_key_existed && app_global_data.rcu_status != RCU_STATUS_PAIRED){
        LED_BLINK(LED_1,LED_TYPE_BLINK_KEY_PRESS_MODE,6);
    }
#endif
    APP_PRINT_INFO1("[key_handle_one_key_scenario] rcu status is %d", app_global_data.rcu_status);
    switch (app_global_data.rcu_status)
    {
    case RCU_STATUS_IDLE:
        if (special_two_key_press_flag == false)
        {
            key_handle_one_key_in_idle_status(key_index);
        }
        break;
    case RCU_STATUS_ADVERTISING:
        if (special_two_key_press_flag == false)
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
#if (RCU_HD_PLATFORM_SEL == SRC_4416_5302)
            if (key_index == MM_AC_Search)
#elif (RCU_HD_PLATFORM_SEL == SRC_4416_5291)
            if (key_index == MM_AC_Search)
#elif (RCU_HD_PLATFORM_SEL == SRC_3925_5879)
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
                if (key_index != MM_POWER && key_index != MM_NETFLIX && key_index != MM_YouTube) {
                    key_handle_notfiy_key_data(key_index);  
                }           
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
if (special_two_key_press_flag == false)
    {
        ir_send_key_press_handle(key_index, VK_NC, 1);  /*send IR wave, period send if long press*/
    }
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

#if (RCU_HD_PLATFORM_SEL == SRC_4416_5302 || RCU_HD_PLATFORM_SEL == SRC_4416_5291 || RCU_HD_PLATFORM_SEL == SRC_3925_5879)
/******************************************************************
 * @brief  key handle process bug report event
 * @param  none
 * @return none
 */
void key_ok_back_bug_report_event()
{
#if IR_FUN
    os_timer_restart(&combine_keys_release_timer, COMBINE_KEYS_RELEASE_TIMEOUT);
    key_handle_global_data.combine_keys_release_status = 1;
    ir_send_key_press_handle(MM_BugReport, VK_NC, 1);
#endif
}

void key_back_down_bug_report_event()
{
#if IR_FUN
    os_timer_restart(&combine_keys_release_timer, COMBINE_KEYS_RELEASE_TIMEOUT);
    key_handle_global_data.combine_keys_release_status = 1;
    ir_send_key_press_handle(MM_TalkBack, VK_NC, 1);
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

#if (RCU_HD_PLATFORM_SEL == SRC_3925_5879)
    if (((key_index_1 == MM_AC_Home && key_index_2 == MM_AC_Back) ||
         (key_index_2 == MM_AC_Home && key_index_1 == MM_AC_Back)))
    {
        key_handle_global_data.combine_keys_status = PAIRING_COMBINE_KEYS_BIT_MASK;
        special_two_key_press_flag = true;
    }
    else if (((key_index_1 == MM_DPadCenter && key_index_2 == MM_AC_Back) ||
              (key_index_2 == MM_DPadCenter && key_index_1 == MM_AC_Back)))
    {
        if(app_global_data.rcu_status != RCU_STATUS_PAIRED || app_global_data.rcu_status != RCU_STATUS_CONNECTED){
            key_handle_global_data.combine_keys_status = OK_BACK_COMBINE_KEYS_BIT_MASK;
            timeout_ms = COMBINE_KEYS_RELEASE_TIMEOUT;
            special_two_key_press_flag = true;
        }
        
    }
    else if (((key_index_1 == MM_DPadDown && key_index_2 == MM_AC_Back) ||
              (key_index_2 == MM_DPadDown && key_index_1 == MM_AC_Back)))
    {
        if(app_global_data.rcu_status != RCU_STATUS_PAIRED || app_global_data.rcu_status != RCU_STATUS_CONNECTED){
            key_handle_global_data.combine_keys_status = BACK_DOWN_COMBINE_KEYS_BIT_MASK;
            timeout_ms = COMBINE_KEYS_RELEASE_TIMEOUT;
            special_two_key_press_flag = true;
        }
    }
#endif
#if IR_LEARN_MODE
    else if (((key_index_1 == MM_VolumeIncrement && key_index_2 == MM_VolumeDecrement) ||
              (key_index_2 == MM_VolumeIncrement && key_index_1 == MM_VolumeDecrement)))
    {
        key_handle_global_data.combine_keys_status = SMARTRCU_DATA_CLEAN_KEYS_BIT_MASK;
        timeout_ms = SMARTRCU_DATA_CLEAN_TIMEOUT;
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
    else if (key_handle_global_data.combine_keys_status == SMARTRCU_DATA_CLEAN_KEYS_BIT_MASK)
    {
        if(app_global_data.rcu_status == RCU_STATUS_PAIRED){
            rcu_terminate_connection(DISCONN_REASON_IDLE);
        }
        LED_BLINK(LED_1,LED_TYPE_BLINK_KEY_PRESS_MODE,3);
        smartrcu_data_reset();
    }
#endif
#if (RCU_HD_PLATFORM_SEL == SRC_3925_5879)
        else if (key_handle_global_data.combine_keys_status == OK_BACK_COMBINE_KEYS_BIT_MASK)
    {
        ir_send_key_release_handle();
        key_ok_back_bug_report_event();
    }
    else if (key_handle_global_data.combine_keys_status == BACK_DOWN_COMBINE_KEYS_BIT_MASK)
    {
        ir_send_key_release_handle();
        key_back_down_bug_report_event();
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
#if (RCU_HD_PLATFORM_SEL == SRC_4416_5302)
            if (key_handle_global_data.reconn_key_index == MM_AC_Search)
#elif(RCU_HD_PLATFORM_SEL == SRC_4416_5291)
            if (key_handle_global_data.reconn_key_index == MM_AC_Search)
#elif(RCU_HD_PLATFORM_SEL == SRC_3925_5879)
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
#if (RCU_HD_PLATFORM_SEL == SRC_4416_5302)
            if (key_handle_global_data.reconn_key_index == MM_AC_Search)
#elif(RCU_HD_PLATFORM_SEL == SRC_4416_5291)
            if (key_handle_global_data.reconn_key_index == MM_AC_Search)
#elif(RCU_HD_PLATFORM_SEL == SRC_3925_5879)
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
static void combine_keys_release_timer_cb(TimerHandle_t p_timer) DATA_RAM_FUNCTION;
static void combine_keys_release_timer_cb(TimerHandle_t p_timer)
{
    APP_PRINT_INFO0("[combine_keys_release_timer_cb] key_handle_global_data.combine_keys_release_status");
    ir_send_exit();
}
/**
 * @brief    key_MM_DPadCenter_pressed_timer_cb
 * @param    p_timer - point of timer
 * @return   none
 * @retval   void
 * Caution   OK键按下计数定时器回调函数
 */
static void key_MM_DPadCenter_pressed_timer_cb(TimerHandle_t p_timer) DATA_RAM_FUNCTION;
static void key_MM_DPadCenter_pressed_timer_cb(TimerHandle_t p_timer)
{
    static uint16_t key_MM_DPadCenter_pressed_count = 0;
    key_MM_DPadCenter_pressed_count ++;
    if(key_MM_DPadCenter_pressed_count < 100 && key_handle_global_data.last_pressed_key_index == VK_NC){
        key_MM_DPadCenter_pressed_count = 0;
        os_timer_stop(&key_MM_DPadCenter_pressed_timer);        
    }
    if(key_MM_DPadCenter_pressed_count == 100 && key_handle_global_data.last_pressed_key_index == MM_DPadCenter){
        key_MM_DPadCenter_pressed_count = 0;
        os_timer_stop(&key_MM_DPadCenter_pressed_timer);        
    }
   
}
/**
 * @brief    ambilight_timer_cb
 * @param    p_timer - point of timer
 * @return   none
 * @retval   void
 * Caution   背光灯关闭定时器回调函数
 */
static void ambilight_timer_cb(TimerHandle_t p_timer) DATA_RAM_FUNCTION;
static void ambilight_timer_cb(TimerHandle_t p_timer)
{
   LED_AMBILIGHT_OFF();
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

    if (false == os_timer_create(&key_MM_DPadCenter_pressed_timer,
                                 "key_MM_DPadCenter_pressed_timer",  1, \
                                 KEY_MM_DPadCenter_PRESSED_TIMEOUT, true, key_MM_DPadCenter_pressed_timer_cb))
    {
        APP_PRINT_ERROR0("[key_handle_init_timer] key_MM_DPadCenter_pressed_timer creat failed!");
    }
    
    if (false == os_timer_create(&combine_keys_release_timer,
                                 "combine_keys_release_timer",  1, \
                                 COMBINE_KEYS_RELEASE_TIMEOUT, false, combine_keys_release_timer_cb))
    {
        APP_PRINT_ERROR0("[key_handle_init_timer] combine_keys_release_timer creat failed!");
    }

    if (false == os_timer_create(&ambilight_timer,
                                 "ambilight_timer",  1, \
                                 AMBILIGHT_TIMEOUT, false, ambilight_timer_cb))
    {
        APP_PRINT_ERROR0("[key_handle_init_timer] ambilight_timer creat failed!");
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
            uint8_t mm_key_data[2] = {0x00, 0x00};

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
#if (RCU_HD_PLATFORM_SEL == SRC_4416_5302)
        if (key_index == MM_DPadCenter)
#elif(RCU_HD_PLATFORM_SEL == SRC_3925_5879)
        if (key_index == MM_DPadCenter)
#elif(RCU_HD_PLATFORM_SEL == SRC_4416_5291)
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
    if(single_tone_test_flag == true){
        APP_PRINT_INFO0("single_tone_test_flag = true!");
        rcu_start_adv(ADV_UNDIRECT_PAIRING);
        os_timer_restart(&single_pair_adv_time_out, SINGLE_PAIR_ADV_TIMEOUT);
        return;                       
    }
    T_KEYSCAN_FIFO_DATA keyscan_fifo_data;
    memcpy(&keyscan_fifo_data, p_keyscan_fifo_data, sizeof(T_KEYSCAN_FIFO_DATA));
    APP_PRINT_INFO1("[key_handle_pressed_event] keyscan FIFO length is %d", keyscan_fifo_data.len);

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
        if(key_index_1 == MM_DPadCenter){
            APP_PRINT_WARN0("[key_handle_pressed_event] MM_DPadCenter is press!");
            os_timer_restart(&key_MM_DPadCenter_pressed_timer, KEY_MM_DPadCenter_PRESSED_TIMEOUT);
        }
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
    if (keyscan_global_data.is_all_key_released == true)
    {
        special_two_key_press_flag = false;
    }
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
            else{
#endif
                if (app_global_data.rcu_status == RCU_STATUS_PAIRED)
                {   
                    if(key_handle_global_data.last_pressed_key_index == MM_AC_Search){
                        return;
                    }     
#if (RCU_HD_PLATFORM_SEL == SRC_4416_5302)      
                    // if(key_handle_global_data.last_pressed_key_index != MM_REC){
                        key_handle_notfiy_key_data(VK_NC);   
                    // }    
#elif(RCU_HD_PLATFORM_SEL == SRC_4416_5291)  
                        key_handle_notfiy_key_data(VK_NC);
#elif(RCU_HD_PLATFORM_SEL == SRC_3925_5879)  
                    key_handle_notfiy_key_data(VK_NC);     
#endif                                                               
                }
            }

#if IR_LEARN_MODE
        ir_learn_key_release_handle();
#endif
        key_handle_global_data.last_pressed_key_index = VK_NC;
    }
#if (RCU_HD_PLATFORM_SEL == SRC_4416_5302)
#if IR_FUN
    if (is_ir_send_working())
    {
        // if(key_handle_global_data.last_pressed_key_index != MM_REC){
            ir_send_key_release_handle();
        // }
    }
#endif
#endif
#if (RCU_HD_PLATFORM_SEL == SRC_4416_5291)
#if IR_FUN
    if (is_ir_send_working())
    {
        ir_send_key_release_handle();
    }
#endif
#endif
#if (RCU_HD_PLATFORM_SEL == SRC_3925_5879)
#if IR_FUN
    if (is_ir_send_working())
    {
        ir_send_key_release_handle();
    }
#endif
#endif
}
void smartrcu_con_state_handle(T_KEY_INDEX_DEF key_index)
{
    APP_PRINT_INFO1("[smartrcu_con_state_handle] key_index = %d", key_index);
    memset(&smartrcu_global_data, 0, sizeof(smartrcu_global_data));
    if(ftl_load(&smartrcu_global_data,4,12) == 0){  
        APP_PRINT_INFO1("[smartrcu_con_state_handle] ftl_load smartrcu_global_data.send_smartrcu_ir_state = %d",smartrcu_global_data.send_smartrcu_ir_state);       
        if(smartrcu_global_data.send_smartrcu_ir_state == 1){
            switch (key_index)
            {
            case MM_POWER:
                smartrcu_key_handle(MM_POWER,INDEX_POWER);
                break;
            case MM_NETFLIX:
                smartrcu_key_handle(MM_NETFLIX,INDEX_NETFLIX);
                break;
            case MM_YouTube:
                smartrcu_key_handle(MM_YouTube,INDEX_YouTube);
                break;
            
            default:
                break;
            }
        }
        else{
            key_handle_notfiy_key_data(key_index);
        } 
    } 
    else{
        APP_PRINT_INFO0("[smartrcu_con_state_handle] ftl_load fail");
    }
}
void smartrcu_key_handle(T_KEY_INDEX_DEF key_index,uint8_t smartrcu_key_index)
{
    APP_PRINT_INFO3("[smartrcu_key_handle] key_index = %d,smartrcu_key_index = %d,key_code_type =%d",key_index,smartrcu_key_index,smartrcu_global_data.key_send_data[smartrcu_key_index].key_code_type);
    if(smartrcu_global_data.key_send_data[smartrcu_key_index].key_code_type == SEND_IR_KEY){
        smartrcu_ir_send_handle(key_index);
    }
    else if(smartrcu_global_data.key_send_data[smartrcu_key_index].key_code_type == SEND_BLE_KEY){
        key_handle_notfiy_key_data(key_index); 
    }
    else if(smartrcu_global_data.key_send_data[smartrcu_key_index].key_code_type == SEND_ALL_KEY){
        smartrcu_ir_send_handle(key_index);
        key_handle_notfiy_key_data(key_index); 
    }
}

void smartrcu_ir_send_handle(uint8_t key_index)
{
    static bool inve_bit = 0;
    uint8_t key_code = 0;
    if(smartrcu_global_data.inve_code_flag != 0){
        if(inve_bit == 0){
            inve_bit = 1;
            key_code = key_index;
        }
        else if(inve_bit == 1){
            inve_bit = 0;
            switch (key_index)
            {
            case MM_POWER:
                key_code = MM_POWER_INCERSE;
                break;
            case MM_NETFLIX:
                key_code = MM_NETFLIX_INCERSE;
                break;
            case MM_YouTube:
                key_code = MM_YouTube_INCERSE;
                break;   
            default:
                break;
            }
        }
    }
    else{
        key_code = key_index;
    }
    APP_PRINT_INFO1("[smartrcu_ir_send_handle] key_code = %d",key_code);
    ir_send_key_press_handle(key_code, VK_NC, 1);
}

