/*****************************************************************************************
*     Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    key_handle.h
  * @brief   This file contains all the constants and functions prototypes for key handle.
  * @details
  * @author  barry_bian
  * @date    2020-02-25
  * @version v1.0
  * *************************************************************************************
  */

#ifndef _KEY_HANDLE_H_
#define _KEY_HANDLE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <keyscan_driver.h>
#include <key_config.h>
#include <app_section.h>
#include <keyscan_basic_driver.h>


/*============================================================================*
 *                              Macro Definitions
 *============================================================================*/
/* define the bit mask of combine keys */
#define INVALID_COMBINE_KEYS_BIT_MASK               0x0000
#define PAIRING_COMBINE_KEYS_BIT_MASK               0x0001
#define IR_LEARNING_COMBINE_KEYS_BIT_MASK           0x0002
#define HCI_UART_TEST_COMBINE_KEYS_BIT_MASK         0x0004
#define DATA_UART_TEST_COMBINE_KEYS_BIT_MASK        0x0008
#define SINGLE_TONE_TEST_COMBINE_KEYS_BIT_MASK      0x0010
#define FAST_PAIR_1_COMBINE_KEYS_BIT_MASK           0x0020
#define FAST_PAIR_2_COMBINE_KEYS_BIT_MASK           0x0040
#define FAST_PAIR_3_COMBINE_KEYS_BIT_MASK           0x0080
#define FAST_PAIR_4_COMBINE_KEYS_BIT_MASK           0x0100
#define FAST_PAIR_5_COMBINE_KEYS_BIT_MASK           0x0200
#if (RCU_HD_PLATFORM_SEL == G_MIN_DEMO_RCU)
#define BUG_REPORT_COMBINE_KEYS_BIT_MASK            0x0400
#endif
#define FACTORY_RESET_COMBINE_KEYS_BIT_MASK         0x8000

#if (RCU_HD_PLATFORM_SEL == G_MIN_DEMO_RCU)
#define COMBINE_KEYS_DETECT_TIMEOUT         4000  /* 4 sec */
#define FACTORY_RESET_KEYS_DETECT_TIMEOUT   4000  /* 4 sec */
#define BUG_REPOERT_DETECT_TIMEOUT          1000  /* 1 sec */
#else
#define COMBINE_KEYS_DETECT_TIMEOUT         2000  /* 2 sec */
#define FACTORY_RESET_KEYS_DETECT_TIMEOUT   8000  /* 8 sec */
#endif
#define NOTIFY_KEY_DATA_TIMEOUT             300   /* 300 ms */
#if FEATURE_SUPPORT_KEY_LONG_PRESS_PROTECT
#define LONG_PRESS_X_S_KEY_DETECT_TIMEOUT    1000 /* 1 sec */
#define LONG_PRESS_KEY_DETECT_TIMEOUT       10000 /* 10 sec */
#endif

#define MAX_HID_KEYBOARD_USAGE_CNT      8
#define MAX_HID_CONSUMER_USAGE_CNT      3

/*============================================================================*
 *                              Types
 *============================================================================*/


/* define the key types */
typedef enum
{
    KEY_TYPE_NONE       = 0x00,  /* none key type */
    KEY_TYPE_BLE_ONLY   = 0x01,  /* only BLE key type */
    KEY_TYPE_IR_ONLY    = 0x02,  /* only IR key type */
    KEY_TYPE_BLE_OR_IR  = 0x03,  /* BLE or IR key type */
} T_KEY_TYPE_DEF;

/* define the HID usage pages */
typedef enum
{
    HID_UNDEFINED_PAGE  = 0x00,
    HID_KEYBOARD_PAGE   = 0x07,
    HID_CONSUMER_PAGE   = 0x0C,
} T_HID_USAGE_PAGES_DEF;

/* define the key code table size, the value should modify according to KEY_CODE_TABLE */
#define KEY_CODE_TABLE_SIZE KEY_INDEX_ENUM_GUAID

/* define the struct of key code */
typedef struct
{
    T_KEY_TYPE_DEF key_type;
    uint8_t ir_key_code;
    uint8_t hid_usage_page;
    uint16_t hid_usage_id;
} T_KEY_CODE_DEF;

typedef struct
{
    uint8_t keyboard_usage_cnt;
    uint8_t keyboard_usage_buffer[MAX_HID_KEYBOARD_USAGE_CNT];
    uint8_t consumer_usage_cnt;
    uint16_t consumer_usage_buffer[MAX_HID_CONSUMER_USAGE_CNT];
} T_KEY_HID_USAGES_BUFFER;


typedef struct
{
    struct
    {
        uint32_t press_time;      /**按压时间 */

	    uint16_t key_status;
        uint8_t  key;         /**键值*/
    } key[6];
} T_KEY_STATUS_DATA;


/*============================================================================*
*                           Export Global Variables
*============================================================================*/

extern T_KEY_STATUS_DATA g_key_data;
extern TimerHandle_t notify_key_data_after_reconn_timer;

/*============================================================================*
 *                          Functions
 *============================================================================*/

extern void key_handle_process_repairing_event(void);
extern void key_press_time_dis_event(void) DATA_RAM_FUNCTION;
extern void key_handle_release_event(void) DATA_RAM_FUNCTION;
extern void key_handle_pressed_event(void *p_keyscan_fifo_data);
extern void key_disable(void);

#ifdef __cplusplus
}
#endif

#endif
