/**
*****************************************************************************************
*     Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    otp_config.h
  * @brief   Update Configuration in APP
  * @date    2020.03.10
  * @version v1.1
  * *************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2020 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef OTP_CONFIG_H
#define OTP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rtl876x_wdg.h"
/*============================================================================*
 *                        debug configuration
 *============================================================================*/
/** @brief just for OTA Demo test */
#define OTA_TEST                                   0
/** @brief just for OTA Demo test */
#define OTA_TEST_BANK_SWITCH                       0
#if (OTA_TEST_BANK_SWITCH == 0)
#define OTA_TEST_IMAGE_VERSION                     0
#endif

/** @brief just for debug */
#define SYSTEM_TRACE_ENABLE                        0


/*============================================================================*
 *                        flash configuration
 *============================================================================*/
/** @brief support for puran flash*/
#define FTL_APP_CALLBACK_ENABLE                    0
/** @brief modify ftl logic space size to adjust the RAM footprint of the ftl Mapping Table
    * PAGE_ELEMENT_DATA_NUM = ((FMC_PAGE_SIZE / 8) - 1).
    * MAX_LOGICAL_ADDR_SIZE = (((PAGE_ELEMENT_DATA_NUM * (g_page_num - 1)) - 1) << 2)
    * if sector size is 4KB, PAGE_ELEMENT_DATA_NUM equal 511.
    * g_page_num = ftl physical size / FMC_PAGE_SIZE. so if ftl size is 16KB, g_page_num is 4.
    * example: default ftl size in flash layout is 16KB, MAX_LOGICAL_ADDR_SIZE = 0x17F0 = 6128
    * FTL_REAL_LOGIC_ADDR_SIZE must be less or equal MAX_LOGICAL_ADDR_SIZE, otherwise will init ftl fail.
*/
#define FTL_REAL_LOGIC_ADDR_SIZE                   (4 * 1024)
/** @brief enable BP, set lock level depend on flash layout and selected flash id */
#define FLASH_BLOCK_PROTECT_ENABLE                 1
/** @brief modify delay time for wakeup flash from power down mode to standby mode*/
#define AFTER_TOGGLE_CS_DELAY                      6


/*============================================================================*
 *                        platform configuration
 *============================================================================*/
/** @brief default enable swd pinmux */
#define SWD_PINMUX_ENABLE                          0
/** @brief enable aon wdg which continue work in dlps state */
#define AON_WDG_ENABLE                             0
/** @brief set aon wdg timeout period in seconds, max value is 65s */
#define AON_WDG_TIME_OUT_PERIOD_SECOND             10
/** @brief default disable watch dog in rom */
#define ROM_WATCH_DOG_ENABLE                       1
/** @brief set wdg mode, default reset all */
#define ROM_WATCH_DOG_MODE                         RESET_ALL
/** @brief Watch Dog Timer Config, default 4s timeout
   * div_factor: 16Bit: 32.768k/(1+divfactor).
   * cnt_limit: 2^(cnt_limit+1) - 1 ; max 11~15 = 0xFFF.
   * wdg_mode:
   *            1: RESET_ALL_EXCEPT_AON
   *            3: RESET_ALL
**/
#define ROM_WATCH_DOG_CFG_DIV_FACTOR               31
#define ROM_WATCH_DOG_CFG_CNT_LIMIT                15

/*before wdg system reset, write reset reason to specific flash addr if enable*/
#define WRITE_REASON_TO_FLASH_BEFORE_RESET_ENABLE     0
#if (WRITE_REASON_TO_FLASH_BEFORE_RESET_ENABLE > 0)
/*write reset reason to specific flash address*/
#define REBOOT_REASON_RECORD_ADDRESS                  0x8cb000
/*max number of reboot record (2^n), one reset reason need 4 bytes, occupy flash size equal 2^(n+2)*/
#define REBOOT_REASON_RECORD_LIMIT_POWERT2            10  //reserve 4K
#endif


/*============================================================================*
 *                        upperstack configuration
 *============================================================================*/
#define BT_STACK_CONFIG_ENABLE

#ifdef BT_STACK_CONFIG_ENABLE
void bt_stack_config_init(void);
#endif
/*============================================================================*
*                        OTA configuration
*============================================================================*/
/*If support unsafe single bank ota user data, must define the following macros */
#define SUPPORT_SINGLE_BANK_OTA_USER_DATA
#ifdef SUPPORT_SINGLE_BANK_OTA_USER_DATA
#define USER_DATA_START_ADDR                       0x00840000  //back 8M(0x1000000) or 0x00840000
#define USER_DATA_MAX_SIZE                         (1025 * 1024)  //1M+1K
#endif

/*normal ota timeout settings*/
#define OTA_TIMEOUT_TOTAL                          240
#define OTA_TIMEOUT_WAIT4_CONN                     60
#define OTA_TIMEOUT_WAIT4_IMAGE_TRANS              200
#define OTA_TIMEOUT_CTITTV                         0xFF


/*============================================================================*
 *                        app configuration
 *============================================================================*/
//add more here



#ifdef __cplusplus
}
#endif


/** @} */ /* End of group OTP_CONFIG */
#endif

/******************* (C) COPYRIGHT 2020 Realtek Semiconductor Corporation *****END OF FILE****/
