/**
 * @file     ble_init.h
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2023/5/26
 * @history
 * @note
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

#ifndef __BLE_INIT_H__
#define __BLE_INIT_H__

/**
 * @brief  ble init.
 */
extern EN_ERR_STA_T ble_init(void);

/**
 * @brief  check if system can go to sleep
 * @return false(0) - system can not goto sleep.
 * @return true(1)  - system can goto sleep.
 */
bool ble_init_is_system_sleep_allowed(void);

/**
 * @brief when system wakeup by llc, invoke this func
 */
void ble_init_on_system_wakeup_by_llc(void);

#endif /* __BLE_INIT_H__ */
