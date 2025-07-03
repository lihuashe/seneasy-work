
/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2023 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef _KEYSCAN_BASIC_DRIVER_H_
#define _KEYSCAN_BASIC_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <rtl876x.h>
#include <rtl876x_pinmux.h>
#include <key_config.h>

/*============================================================================*
 *                         Macros
 *============================================================================*/
//#define KEYSCAN_PRINT_LOG  /* Enable print log or not */

#ifdef KEYSCAN_PRINT_LOG
#define KEYSCAN_DBG_BUFFER(MODULE, LEVEL, fmt, para_num,...) DBG_BUFFER_##LEVEL(TYPE_BEE2, SUBTYPE_FORMAT, MODULE, fmt, para_num, ##__VA_ARGS__)
#else
#define KEYSCAN_DBG_BUFFER(MODULE, LEVEL, fmt, para_num,...) ((void)0)
#endif

/*============================================================================*
 *                         Types
 *============================================================================*/
/**
 * @brief  KeyScan FIFO data struct definition.
 */
typedef struct
{
    uint32_t len;               /**< Keyscan state register        */
    struct
    {
        uint16_t column: 5;      /**< Keyscan raw buffer data       */
        uint16_t row: 4;         /**< Keyscan raw buffer data       */
        uint16_t reserved: 7;
    } key[8];
} T_KEYSCAN_FIFO_DATA;

/**
 * @brief  KeyScan global data struct definition.
 */
typedef struct
{
    bool is_allowed_to_repeat_report;  /* to indicate whether to allow to report repeat keyscan data event or not */
    bool is_allowed_to_enter_dlps;  /* to indicate whether to allow to enter dlps or not */
    bool is_all_key_released;  /* to indicate whether all keys are released or not */
    bool is_pinmux_setted; /* to indicate whether pinmux is setted */
    bool is_key_long_pressed; /* to indicate whether key is long pressed */
    uint8_t long_pressed_key_row_pin; /* row pin of long pressed key */
    T_KEYSCAN_FIFO_DATA pre_fifo_data;  /* to indicate the previous keyscan FIFO data */
    T_KEYSCAN_FIFO_DATA cur_fifo_data;  /* to indicate the current keyscan FIFO data */
} T_KEYSCAN_GLOBAL_DATA;


typedef enum
{
    KEY_RELEASE  = 0x00,
    KEY_PRESS,
    KEY_STUCK
}ENUM_KEY;

/**
 * @brief  KeyScan FIFO data struct definition.
 */
typedef struct
{
    uint8_t key_press_num; 
    uint8_t key_status[4];   
    uint8_t key[4];
} KeyScan_Data;
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
extern T_KEYSCAN_GLOBAL_DATA keyscan_global_data;
extern volatile KeyScan_Data g_keyscan_io_data;
/*============================================================================*
 *                         Functions
 *============================================================================*/

void keyscan_init_pad_config(void);
void keyscan_init_pad_config(void);
void keyscan_io_init_pad_config(void);
void keyscan_io_init_driver(uint32_t is_debounce);
void power_key_check(void);
#ifdef __cplusplus
}
#endif

#endif

