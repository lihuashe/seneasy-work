/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2018 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _KEY_HANDLE_H_
#define _KEY_HANDLE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <keyscan_driver.h>

/*============================================================================*
 *                              Macro Definitions
 *============================================================================*/
/* define the bit mask of combine keys */
#define INVALID_COMBINE_KEYS_BIT_MASK               0x0000
#define PAIRING_COMBINE_KEYS_BIT_MASK               0x0001
#define SMARTRCU_DATA_CLEAN_KEYS_BIT_MASK           0x0002
#define HCI_UART_TEST_COMBINE_KEYS_BIT_MASK         0x0004
#define DATA_UART_TEST_COMBINE_KEYS_BIT_MASK        0x0008
#define SINGLE_TONE_TEST_COMBINE_KEYS_BIT_MASK      0x0010
#define FAST_PAIR_1_COMBINE_KEYS_BIT_MASK           0x0020
#define FAST_PAIR_2_COMBINE_KEYS_BIT_MASK           0x0040
#define FAST_PAIR_3_COMBINE_KEYS_BIT_MASK           0x0080
#define FAST_PAIR_4_COMBINE_KEYS_BIT_MASK           0x0100
#define FAST_PAIR_5_COMBINE_KEYS_BIT_MASK           0x0200
#if (RCU_HD_PLATFORM_SEL == SRC_4416_5302)
#define BUG_REPORT_COMBINE_KEYS_BIT_MASK            0x0400
#elif (RCU_HD_PLATFORM_SEL == SRC_3925_5879)
#define OK_BACK_COMBINE_KEYS_BIT_MASK               0x0400
#define BACK_DOWN_COMBINE_KEYS_BIT_MASK             0x0800
#endif
#define FACTORY_RESET_COMBINE_KEYS_BIT_MASK         0x8000

#if (RCU_HD_PLATFORM_SEL == SRC_4416_5302)
#define COMBINE_KEYS_DETECT_TIMEOUT         3000  /* 3 sec */
#define FACTORY_RESET_KEYS_DETECT_TIMEOUT   4000  /* 4 sec */
#define BUG_REPOERT_DETECT_TIMEOUT          1000  /* 1 sec */
#define SMARTRCU_DATA_CLEAN_TIMEOUT         10000  /* 10 sec */   
#elif (RCU_HD_PLATFORM_SEL == SRC_3925_5879)
#define COMBINE_KEYS_DETECT_TIMEOUT         3000  /* 3 sec */
#define FACTORY_RESET_KEYS_DETECT_TIMEOUT   4000  /* 4 sec */
#define BUG_REPOERT_DETECT_TIMEOUT          1000  /* 1 sec */
#define SMARTRCU_DATA_CLEAN_TIMEOUT         10000  /* 10 sec */  
#else
#define COMBINE_KEYS_DETECT_TIMEOUT 2000  /* 2 sec */
#endif

#define NOTIFY_KEY_DATA_TIMEOUT           300  /* 300 ms */
#define COMBINE_KEYS_RELEASE_TIMEOUT      100  /* 100 ms */
#define KEY_MM_DPadCenter_PRESSED_TIMEOUT 50   /* 50 ms */
#define AMBILIGHT_TIMEOUT                 5000 /* 5 sec */
#define KEY_MM_REC_PRESSED_TIMEOUT        50   /* 50 ms */

/*============================================================================*
 *                         Types
 *============================================================================*/
typedef enum _RECON_KEY_STATUS
{
    RECON_KEY_IDLE = 0,
    RECON_KEY_PRESS = 1,
    RECON_KEY_REALSE_IMM = 2,
    RECON_KEY_SEND_REALSE_BY_TIM = 3,
    RECON_KEY_SEND_REALSE_NORMAL = 4,
} RECON_KEY_STATUS;

typedef enum
{
#if (RCU_HD_PLATFORM_SEL == SRC_4416_5302)
    VK_NC        = 0x00,
    VK_POWER,
    VK_PAGE_UP,
    VK_PAGE_DOWN,
    VK_HOME,
    VK_MENU,
    VK_VOICE,
    VK_ENTER,
#if FEATURE_SUPPORT_MULTIMEDIA_KEYBOARD
    MM_ScanNext,
    MM_POWER,
    MM_NUMBER1,
    MM_NUMBER2,
    MM_NUMBER3,
    MM_NUMBER4,
    MM_NUMBER5,
    MM_NUMBER6,
    MM_NUMBER7,
    MM_NUMBER8,
    MM_NUMBER9,
    MM_REWIND,
    MM_NUMBER0,
    MM_FAST_FORWARD,
    MM_PREVIOUS,
    MM_Play_Pause,
    MM_NEXT,
    MM_VolumeIncrement,
    MM_VolumeDecrement,
    MM_STOP,
    MM_Mute,
    MM_P_Increment,
    MM_P_Decrement,
    MM_AC_Search,
    MM_AC_Home,
    MM_DPadUp,
    MM_DPadLeft,
    MM_DPadCenter,
    MM_DPadRight,
    MM_DPadDown,
    MM_AC_Back,
    MM_Setting,
    MM_Bookmark,
    MM_INFO,
    MM_Guide,
    MM_YouTube,
    MM_NETFLIX, 
    MM_PrimeVideo,
    MM_Disney,
    MM_FAVORITES, 
    MM_Google_Play_Games,
    MM_RED,
    MM_GREEN,
    MM_YELLOW,
    MM_BLUE, 
    MM_REC_THREE_SEC,
    MM_AC_Search_Noconnect,
    MM_BugReport,
    MM_Combine,
    MM_POWER_INCERSE,
    MM_VolumeIncrement_INCERSE,
    MM_VolumeDecrement_INCERSE,
    MM_Mute_INCERSE,
    KEY_INDEX_ENUM_GUAID
#endif
#elif (RCU_HD_PLATFORM_SEL == SRC_4416_5291)
    VK_NC        = 0x00,
    VK_POWER,
    VK_PAGE_UP,
    VK_PAGE_DOWN,
    VK_HOME,
    VK_MENU,
    VK_VOICE,
    VK_ENTER,
    VK_EXIT,
    VK_LEFT,
    VK_RIGHT,
    VK_UP,
    VK_DOWN,
    VK_MOUSE_EN,
    VK_VOLUME_MUTE,
    VK_VOLUME_UP,
    VK_VOLUME_DOWN,
    VK_VOICE_STOP,
    VK_TV_POWER,
    VK_TV_SIGNAL,
    VK_NUMBER0,
    VK_NUMBER1,
    VK_NUMBER2,
    VK_NUMBER3,
    VK_NUMBER4,
    VK_NUMBER5,
    VK_NUMBER6,
    VK_NUMBER7,
    VK_NUMBER8,
    VK_NUMBER9,
    VK_RED,
    VK_GREEN,
    VK_YELLOW,
    VK_SUB,
    VK_BLUE, 
#if FEATURE_SUPPORT_MULTIMEDIA_KEYBOARD
    MM_ScanNext,
    MM_POWER,
    MM_NUMBER1,
    MM_NUMBER2,
    MM_NUMBER3,
    MM_NUMBER4,
    MM_NUMBER5,
    MM_NUMBER6,
    MM_NUMBER7,
    MM_NUMBER8,
    MM_NUMBER9,
    MM_TEXT,
    MM_NUMBER0,
    MM_SUB,
    MM_RW,
    MM_Play_Pause,
    MM_FF,
    MM_VolumeIncrement,
    MM_VolumeDecrement,
    MM_REC,
    MM_Mute,
    MM_P_Increment,
    MM_P_Decrement,
    MM_DPadMenu,
    MM_EXIT,
    MM_DPadUp,
    MM_DPadLeft,
    MM_DPadCenter,
    MM_DPadRight,
    MM_DPadDown,
    MM_AC_Back,
    MM_AC_Home,
    MM_FAVORITES,    
    MM_AC_Search,
    MM_INFO,
    MM_Guide,
    MM_Source,
    MM_NETFLIX, 
    MM_YouTube,
    MM_PrimeVideo,
    MM_DISNEY,
    MM_RED,
    MM_GREEN,
    MM_YELLOW,
    MM_BLUE, 
    MM_REC_THREE_SEC,
    MM_AC_Search_Noconnect,
    MM_BugReport,
    KEY_INDEX_ENUM_GUAID
#endif
#elif (RCU_HD_PLATFORM_SEL == SRC_3925_5879)
    VK_NC        = 0x00,
    VK_NUMBER1,
    VK_NUMBER2,
    VK_NUMBER3,
    VK_NUMBER4,
    VK_NUMBER5,
    VK_NUMBER6,
    VK_NUMBER7,
    VK_NUMBER8,
    VK_NUMBER9,
    VK_NUMBER0,
    VK_VolumeIncrement,
    VK_VolumeDecrement,
#if FEATURE_SUPPORT_MULTIMEDIA_KEYBOARD
    MM_ScanNext,
    MM_POWER,
    MM_Mute,
    MM_NUMBER1,
    MM_NUMBER2,
    MM_NUMBER3,
    MM_NUMBER4,
    MM_NUMBER5,
    MM_NUMBER6,
    MM_NUMBER7,
    MM_NUMBER8,
    MM_NUMBER9,
    MM_INFO,
    MM_NUMBER0,
    MM_SWITCH,
    MM_VolumeIncrement,
    MM_Source,
    MM_P_Increment,
    MM_VolumeDecrement,
    MM_MENU,
    MM_P_Decrement,
    MM_CONTACT,
    MM_AC_Search,
    MM_Setting,
    MM_DPadUp,
    MM_DPadLeft,
    MM_DPadCenter,
    MM_DPadRight,
    MM_DPadDown,
    MM_AC_Back,
    MM_AC_Home,
    MM_TV,
    MM_NETFLIX,
    MM_YouTube,
    MM_PrimeVideo,
    MM_YouTubeMusic,
    MM_RED,
    MM_GREEN, 
    MM_YELLOW,
    MM_BLUE,

    MM_BugReport,
    MM_TalkBack,
    MM_POWER_INCERSE,
    MM_NETFLIX_INCERSE,
    MM_YouTube_INCERSE,
    MM_AC_Search_Noconnect,
    KEY_INDEX_ENUM_GUAID
#endif
#endif

} T_KEY_INDEX_DEF;

/* define the key code table size, the value should modify according to BLE_KEY_CODE_TABLE */
#define BLE_KEY_CODE_TABLE_SIZE KEY_INDEX_ENUM_GUAID

/* define the start key index for multimedia keys */
#define BLE_MM_START_KEY_INDEX MM_ScanNext

/* Key global parameters' struct */
typedef struct
{
    T_KEY_INDEX_DEF reconn_key_index;  /* to indicate the reconnection key index */
    T_KEY_INDEX_DEF last_pressed_key_index;  /* to indicate the last pressed key index */
    uint32_t combine_keys_status;  /* to indicate the status of combined keys */
    bool combine_keys_release_status;
} T_KEY_HANDLE_GLOBAL_DATA;

/*============================================================================*
*                        Export Global Variables
*============================================================================*/
extern const uint16_t BLE_KEY_CODE_TABLE[BLE_KEY_CODE_TABLE_SIZE];
extern T_KEY_HANDLE_GLOBAL_DATA key_handle_global_data;
extern TimerHandle_t combine_keys_detection_timer;
extern TimerHandle_t notify_key_data_after_reconn_timer;

/*============================================================================*
 *                         Functions
 *============================================================================*/
void key_handle_init_data(void);
void key_handle_pressed_event(T_KEYSCAN_FIFO_DATA *pKey_Data);
void key_handle_release_event(void);
bool key_handle_notfiy_key_data(T_KEY_INDEX_DEF key_index);
void key_handle_init_timer(void);
void smartrcu_con_state_handle(T_KEY_INDEX_DEF key_index);
void smartrcu_key_handle(T_KEY_INDEX_DEF key_index,uint8_t smartrcu_key_index);
void smartrcu_ir_send_handle(uint8_t key_index);
#ifdef __cplusplus
}
#endif

#endif
