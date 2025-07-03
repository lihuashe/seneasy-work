/**
 * @file features.h
 * @brief 
 * @date Sat 14 Feb 2015 09:55:02 AM CST
 * @author XIHAO
 *
 * @addtogroup 
 * @ingroup 
 * @details 
 *
 * @{
 */

#ifndef __FEATURES_H__
#define __FEATURES_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

#ifdef CONFIG_AUTOCONF_VERSION
#include "autoconf_version.h"
#endif

// XH98xxD A1
#if defined(CONFIG_XH98xxDA1_RELEASE)
#if defined(CONFIG_BLE_ALL)
#include "autoconf_release_xh98xxd_all_a1.h"
#else
#include "autoconf_release_xh98xxd_a1.h"
#endif

// xh98xx A1
#elif defined(CONFIG_XH98xxFA1_RELEASE)
#include "autoconf_release_xh98xxf_a1.h"

// xh98xx A1
#elif defined(CONFIG_XH98xxEA1_RELEASE)
#include "autoconf_release_xh98xx_a1.h"

#else
#include "autoconf.h"
#endif

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#ifdef CONFIG_USE_BASE_COMPONENT_SYMBOL
#include "bc.h"
#endif

/*********************************************************************
 * MACROS
 */
#ifndef AUTOCONF_INCLUDED
#define AUTOCONF_INCLUDED
#define BLE_VERSION_4_0 (6)
#define BLE_VERSION_4_1 (7)
#define BLE_VERSION_4_2 (8)
#define BLE_VERSION_5_0 (9)
#endif

#if defined(CONFIG_XH98xxD)
#define CONFIG_ROM_ID (0xxh98xxD000 | (CONFIG_HARDWARE_VERSION << 8) | (CONFIG_MINOR_VERSION << 0))
#elif defined(CONFIG_XH98xxF)
#define CONFIG_ROM_ID (0xxh98xxDF00 | (CONFIG_HARDWARE_VERSION << 4) | (CONFIG_MINOR_VERSION << 0))
#elif defined(CONFIG_XH98xx)
#define CONFIG_ROM_ID (0xxh98xxE000 | (CONFIG_HARDWARE_VERSION << 8) | (CONFIG_MINOR_VERSION << 0))
#else
#error Unknown Device
#endif

#ifdef CONFIG_XH98xx
#define CONFIG_SPI_ABSENT
#define CONFIG_UART2_ABSENT
#define CONFIG_OTP_ABSENT
#define CONFIG_CHARGE_ABSENT
#endif

#ifdef CONFIG_XH98xxD
#define CONFIG_UART0_ABSENT
#define CONFIG_UART2_ABSENT
#define CONFIG_DCDC_ABSENT
#define CONFIG_EFUSE_ABSENT
#endif

#ifdef CONFIG_XH98xxF
#define CONFIG_DCDC_ABSENT
#define CONFIG_EFUSE_ABSENT
#endif

/*********************************************************************
 * REDEFINE MACROS
 */


/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */


#ifdef __cplusplus
}
#endif

#endif

/** @} */

