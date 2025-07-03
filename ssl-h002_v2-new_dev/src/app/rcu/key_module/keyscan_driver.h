
/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2018 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef _KEYSCAN_DRIVER_H_
#define _KEYSCAN_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <swtimer.h>
#include <rtl876x.h>

/*============================================================================*
 *                         Macros
 *============================================================================*/
#define KEYSCAN_SW_INTERVAL 50  /* 50ms */

/*============================================================================*
 *                         Types
 *============================================================================*/


/*============================================================================*
*                        Export Global Variables
*============================================================================*/
extern TimerHandle_t keyscan_timer;
#if FEATURE_SUPPORT_DEEP_SLEEP_STATE
extern bool allow_to_enter_dss_flag;
#endif

/*============================================================================*
 *                         Functions
 *============================================================================*/
static void keyscan_enable_wakeup_io_config(uint8_t is_debounce);
static void keyscan_disable_wakeup_io_config(void);

#ifdef __cplusplus
}
#endif

#endif

