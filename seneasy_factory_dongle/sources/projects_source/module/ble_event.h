/**
 * @file     ble_event.h
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2022/02/13
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

#ifndef __BLE_EVENT_H__
#define __BLE_EVENT_H__

typedef void (* ble_event_callback_t )( uint8_t u8Event, uint8_t* pu8Data, uint16_t u16DataLen);

/**
 * @brief  ble event init.
 */
extern void ble_event_init(ble_event_callback_t ble_event_callback_cb);
uint16_t ble_get_connect_handle(void);
#endif /* __BLE_EVENT_H__ */
