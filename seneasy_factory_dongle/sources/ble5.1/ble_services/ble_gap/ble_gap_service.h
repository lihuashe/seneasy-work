/**
 * @file     ble_gap_service.h
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2021/12/5
 * @history
 * @note     ble gap service
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

#ifndef __BLE_GAP_SERVICE_H__
#define __BLE_GAP_SERVICE_H__

/**
 * @brief  Initializing the ble gap service.
 * @param  u16Apperance: Pointer to function for handling received data.
 * @param  pu8DevName: Pointer to device name.
 * @param  u8DevNameLen: Device name length.
 * @return uint32_t: Return status.
 */
extern uint32_t ble_gap_service_init(uint8_t *pu8DevName, uint8_t u8DevNameLen, uint16_t u16Apperance);

/**
 * @brief  Set the Maxscend UART Service.
 * @param  pu8DevName: Pointer to device name.
 * @param  u8DevNameLen: Device name length.
 */
extern void ble_gap_service_set_device_name(uint8_t *pu8DevName, uint16_t u8DevNameLen);

#endif /* __BLE_GAP_SERVICE_H__ */
