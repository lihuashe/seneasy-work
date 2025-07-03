/**
 * @file     ble_battery_service.h
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2022/11/11
 * @history
 * @note     ble battrey service
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2021, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 */

#ifndef BLE_BATTERY_SERVICE_H
#define BLE_BATTERY_SERVICE_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        EN_BATTERY_EVENT_READ_LEVEL = 0x00,

    } enBatteryEvent_t;

    typedef void (*battery_service_event_callback_t)(uint8_t u8Event, uint8_t *pu8Data, uint16_t u16DataLen);

    uint32_t ble_battery_service_init(battery_service_event_callback_t battery_service_event_cb);

    uint8_t ble_battery_notify_level(uint16_t u16ConnHandle, uint8_t u8BatteryLevel);

#ifdef __cplusplus
}
#endif

#endif /* BATTSERVICE_H */
