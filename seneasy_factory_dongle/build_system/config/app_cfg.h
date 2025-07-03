/**
 * @file     app_cfg.h
 * @version  V1.0
 * @date     2021/02/06
 * @history
 * @note     ble_central app configuration.
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2023, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 */

#ifndef __APP_CFG_H__
#define __APP_CFG_H__


// MXD2670 Memory Map
#include "..\..\sources\mem_cfg\mxd2670_mem_cfg_r2.h"


//=====================================================================================================================
// MEMOARY MAP DEFINE
//=====================================================================================================================
//=============================================================================
// RAM(0x2000_2000 ~ 0x0003_FFFF)
//=============================================================================
// CP RAM Code & Data In SDK
#define RAM_CP_ADDR                        (RAM_2ND_BOOT_ADDR) // 0x2000_2000
#define RAM_CP_SIZE_MAX                    ((56 +64)* KBYTE)

// CP STACK and HEAP in SDK
#define RAM_CP_STACK_SIZE_MAX              (4 * KBYTE) // @ref startup_ARMCM0Plus.s
#define RAM_CP_HEAP_SIZE_MAX               (0 * KBYTE) // @ref startup_ARMCM0Plus.s
#define RAM_CP_STACK_HEAP_SIZE_MAX         (RAM_CP_STACK_SIZE_MAX + RAM_CP_HEAP_SIZE_MAX)
#define RAM_CP_STACK_HEAP_ADDR             (RAM_CP_ADDR + RAM_CP_SIZE_MAX - RAM_CP_STACK_HEAP_SIZE_MAX)

// MP NEW_VECTOR In SDK
#define RAM_MP_VECTOR_ADDR                 (RAM_CP_ADDR + RAM_CP_SIZE_MAX)
#define RAM_MP_VECTOR_SIZE_MAX             (512)

// MP RAM Code In SDK
#define RAM_MP_CODE_ADDR                   (RAM_MP_VECTOR_ADDR + RAM_MP_VECTOR_SIZE_MAX)
#define RAM_MP_CODE_SIZE_MAX               (32 * KBYTE - RAM_MP_VECTOR_SIZE_MAX)

// MP RAM Data In SDK
#define RAM_MP_DATA_ADDR                   (RAM_MP_CODE_ADDR + RAM_MP_CODE_SIZE_MAX)
#define RAM_MP_DATA_SIZE_MAX               (32 * KBYTE)

// MP STACK and HEAP in SDK
#define RAM_MP_STACK_SIZE_MAX              (4 * KBYTE) // @ref startup_ARMCM33.s
#define RAM_MP_HEAP_SIZE_MAX               (0 * KBYTE) // @ref startup_ARMCM33.s
#define RAM_MP_STACK_HEAP_SIZE_MAX         (RAM_MP_STACK_SIZE_MAX + RAM_MP_HEAP_SIZE_MAX)
#define RAM_MP_STACK_HEAP_ADDR             (RAM_MP_DATA_ADDR + RAM_MP_DATA_SIZE_MAX - RAM_MP_STACK_HEAP_SIZE_MAX)

//=============================================================================
// Flash(0x1000_0000 ~ 0x11FF_FFFF)
//=============================================================================
// APP Code
#define FLASH_MP_APP_CODE_ADDR             (FLASH_BASE_ADDR + FLASH_BOOT_RESERVED_SIZE_MAX)
#define FLASH_MP_APP_CODE_SIZE_MAX         (FLASH_SIZE_MAX - FLASH_BOOT_RESERVED_SIZE_MAX)


//=====================================================================================================================
// PERIPHERAL DEFINE
//=====================================================================================================================
//=============================================================================
// GPIO
//=============================================================================
// mp app_debug
#define GPIO_PORT_MP_APP_DEBUG_TX          (GPIO_PORT_UART0_TX)
#define GPIO_PIN_MP_APP_DEBUG_TX           (GPIO_PIN_UART0_TX)

// cp app_debug
#define GPIO_PORT_CP_APP_DEBUG_TX          (GPIO_PORT_UART1_TX)
#define GPIO_PIN_CP_APP_DEBUG_TX           (GPIO_PIN_UART1_TX)

// cp mlog
#define GPIO_PORT_CP_MLOG_TX               (GPIO_PORT_UART1_TX)
#define GPIO_PIN_CP_MLOG_TX                (GPIO_PIN_UART1_TX)

//=============================================================================
// UART
//=============================================================================
// mp app_debug
#define UART_HANDLE_MP_APP_DEBUG           (UART0)
#define UART_BPS_MP_APP_DEBUG              (921600)

// cp app_debug
#define UART_HANDLE_CP_APP_DEBUG           (UART1)
#define UART_BPS_CP_APP_DEBUG              (921600)

// cp mlog
#define UART_HANDLE_CP_MLOG                (UART1)
#define UART_BPS_CP_MLOG                   (921600)

//=============================================================================
// BLE
//=============================================================================
#define BLE_SCAN_CHANNEL                   (GAP_ADV_CHANNEL_MAP_37|GAP_ADV_CHANNEL_MAP_38|GAP_ADV_CHANNEL_MAP_39)
#define BLE_SCAN_INTERVAL                  MSEC_TO_UNITS(200, UNIT_0_625_MS)
#define BLE_SCAN_WINDOW                    MSEC_TO_UNITS(100, UNIT_0_625_MS)
#define BLE_SCAN_PEER_MAC                  {0x55, 0x40, 0x4D, 0x03, 0x03, 0xBA}
#define BLE_SCAN_PEER_LOCAL_NAME           "MXD2670_RCU_DEMO"

#define BLE_CONNECT_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)
#define BLE_CONNECT_TIMEOUT                MSEC_TO_UNITS(1000, UNIT_10_MS)

//=====================================================================================================================
// APP Version
//=====================================================================================================================
#define APP_VERSION                       ( 0x0001 )//0.1
#define PATCH_VERSION                     ( 0x020A ) //2.10 // SDK version
#define MODULE_VERSION                    ( 0x0100 ) //1.0 // Module version
#define FW_VERSION                        ( (PATCH_VERSION << 16) | MODULE_VERSION )

//=====================================================================================================================
// APP Configuration
//=====================================================================================================================
//=============================================================================
// DEFAULT CFG
//=============================================================================
#define RC_HCLK_TUNE_DEFAUT_VAL            (0x56)
#define RC_LCLK_TUNE_DEFAUT_VAL            (0xA9)

//=============================================================================
// SYSTEM CLOCK
//=============================================================================
#define DCXO_HCLK_STABLE_TIME_2500US       (2500)
#define DCXO_HCLK_STABLE_TIME_2000US       (2000)
#define DCXO_HCLK_STABLE_TIME              (DCXO_HCLK_STABLE_TIME_2500US)
#define SYSTEM_CLOCK_PLL64M_ENABLED        (0)

//=============================================================================
// LOG CFG
//=============================================================================
#if defined(__MAIN_PROCESSOR)
    #define APP_DEBUG_ENABLED              (1)
    #define MLOG_DEBUG_ENABLED             (0)
#else
    #define APP_DEBUG_ENABLED              (1)
    #define MLOG_DEBUG_ENABLED             (0)
#endif

#if APP_DEBUG_ENABLED
    #define PRINTF(fmt, ...)           printf(fmt, ##__VA_ARGS__)
#else
    #define PRINTF(fmt, ...)
#endif


//=====================================================================================================================
// SYSTEM CONTROL COMMON REGISTER DEFINE
//=====================================================================================================================
#define SYS_CTRL_COM_REG_MP_CTRL_CP        (3)


//=====================================================================================================================
//Wakeup LUT Index
//=====================================================================================================================
#define LUT_INDEX_CP_LLC                   (15)


/*============================================================================*
 *                              MY Configuration
 *============================================================================*/

#define USB_HID_EN 0

#endif /* __APP_CFG_H__ */


