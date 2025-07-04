/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    mem_config.h
  * @brief   Memory Configuration
  * @date    2017.6.6
  * @version v1.0
  * *************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef MEM_CONFIG_H
#define MEM_CONFIG_H

#include "flash_map.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Code configuration
 *============================================================================*/
/** @brief set app bank to support OTA: 1 is ota bank1, 0 is ota bank0 */
#define APP_BANK                                  0

/** @brief ram code configuration: 1 is ram code, 0 is flash code */
#define FEATURE_RAM_CODE                          0

/** @brief encrypt app or not */
#define FEATURE_ENCRYPTION                        0


/*============================================================================*
 *                        data ram layout configuration
 *============================================================================*/
/* Data RAM layout:                    88K
example:
   1) reserved for rom:                14K (fixed)
   2) Patch:                           15K (fixed)
   3) upperstack:                       2K (adjustable, depend on used upperstack version)
   4) app global + ram code:           24K (adjustable, config APP_GLOBAL_SIZE)
   5) Heap ON:                         33K (adjustable, config HEAP_DATA_ON_SIZE)
*/

/** @brief data ram size for upperstack global variables and code */
#define UPPERSTACK_GLOBAL_SIZE         (2 * 1024)

/** @brief data ram size for app global variables and code, could be changed */
#define APP_GLOBAL_SIZE                (20 * 1024)

/** @brief data ram size for heap, could be changed, but (UPPERSTACK_GLOBAL_SIZE + APP_GLOBAL_SIZE + HEAP_DATA_ON_SIZE) must be 59k  */
#define HEAP_DATA_ON_SIZE              (59 * 1024 - APP_GLOBAL_SIZE - UPPERSTACK_GLOBAL_SIZE)

/** @brief shared cache ram size (adjustable, config SHARE_CACHE_RAM_SIZE: 0/4KB/8KB) */
#define SHARE_CACHE_RAM_SIZE           (0 * 1024)
/*****************************************************/


/***************The following macros can't be modified***************/
#define DATA_RAM_START_ADDR            0x00200000
#define DATA_RAM_TOTAL_SIZE            (88 * 1024)
#define DATA_RAM_RESERVED_SIZE         (29 * 1024)

#define APP_GLOBAL_ADDR                (DATA_RAM_START_ADDR + DATA_RAM_RESERVED_SIZE + UPPERSTACK_GLOBAL_SIZE)
#define SHARE_CACHE_RAM_ADDR           (DATA_RAM_START_ADDR + DATA_RAM_TOTAL_SIZE)


#ifdef __cplusplus
}
#endif


/** @} */ /* End of group MEM_CONFIG */
#endif
