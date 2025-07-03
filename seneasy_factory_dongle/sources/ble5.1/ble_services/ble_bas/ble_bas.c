/**
 * @file     ble_battery_service.c
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2022/11/11
 * @history
 * @note     ble battery service
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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "bluetooth.h"
#include "err_def.h"

#include "gatt_uuid_def.h"
#include "gatt_service_api.h"
#include "stack_utility.h"
#include "ble_cb_event_def.h"
#include "patch_gatt_service_api.h"

#include "app_cfg.h"
#include "ble_bas.h"

/**
 * @brief  DEFINE
 */
#define BLE_SERVICE_RETURN_IF_ERROR(handle) \
    do                                      \
    {                                       \
        if (handle == 0)                    \
        {                                   \
            return ERR_STA_ERROR;           \
        }                                   \
    } while (0)

/**
 * @brief  BLE Handle
 */
typedef enum
{
    BLE_BATT_LEVEL_VALUE_IDX            = 0,
    BLE_BATT_LEVEL_VALUE_CLIENT_CFG_IDX = 1,

} enBleBasHandle_t;

/**
 * @brief  LOCAL VARIABLES
 */
static battery_service_event_callback_t battery_service_event_callback;

uint8_t  m_u8BatteryLevelClientCharCfg;
uint16_t m_au16BatteryServerHandle[2];

// ble battery services callbacks.
static uint16_t ble_battery_get_attrlenCB(uint16_t u16ConnHandle, uint16_t u16AttrHandle);
static uint16_t ble_battery_read_attrCB(uint16_t u16ConnHandle, uint16_t u16AttrHandle, uint16_t u16Offset,
                                        uint8_t *pu8Data, uint16_t u16DataLen);
static uint32_t ble_battery_write_attrCB(uint16_t u16ConnHandle, uint16_t u16AttrHandle, uint16_t transaction_mode,
                                         uint16_t u16Offset, uint8_t *pu8Data, uint16_t u16DataLen);

static const gatt_serviceCBs_t batteryCBs = {
    ble_battery_get_attrlenCB,
    ble_battery_read_attrCB,  // Read callback function pointer
    ble_battery_write_attrCB, // Write callback function pointer
};

/**
 * @brief  Initializes the battery service by registering GATT
 *          attributes with the GATT server.
 */
uint32_t battery_service_init(battery_service_event_callback_t battery_service_event_cb)
{
    uint16_t u16Handle = 0;
    if (battery_service_event_cb == NULL)
    {
        return RTN_FAIL;
    }
    battery_service_event_callback = battery_service_event_cb;

    u16Handle = patch_gatts_api_add_service_start(0, BATT_SERV_UUID, NULL, (gatt_serviceCBs_t *)&batteryCBs);
    BLE_SERVICE_RETURN_IF_ERROR(u16Handle);

    u16Handle = patch_gatts_api_add_char(0, BATT_LEVEL_UUID, NULL,
                                         ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC, NULL, 0);
    BLE_SERVICE_RETURN_IF_ERROR(u16Handle);
    m_au16BatteryServerHandle[BLE_BATT_LEVEL_VALUE_IDX] = u16Handle;

    u16Handle = rom_gatts_api_add_char_descrip_client_config();
    BLE_SERVICE_RETURN_IF_ERROR(u16Handle);
    m_au16BatteryServerHandle[BLE_BATT_LEVEL_VALUE_CLIENT_CFG_IDX] = u16Handle;

    u16Handle = rom_gatts_api_add_char_descrip(GATT_REPORT_REF_UUID, ATT_PROPERTY_READ, NULL, 0);
    BLE_SERVICE_RETURN_IF_ERROR(u16Handle);
    patch_gatts_api_add_service_end();

    return 0;
}

/**
 * @brief  Report the battery level.
 * @param  u16ConnHandle: Connect handle.
 * @param  u8BatteryLevel: battery level.
 * @return status: Return status.
 */
uint8_t ble_battery_notify_level(uint16_t u16ConnHandle, uint8_t u8BatteryLevel)
{
    uint8_t mu8BatteryLevel = u8BatteryLevel;
    if (m_u8BatteryLevelClientCharCfg & GATT_CLIENT_CFG_NOTIFY)
    {
        rom_gatts_api_send_notify(u16ConnHandle, m_au16BatteryServerHandle[BLE_BATT_LEVEL_VALUE_IDX], &mu8BatteryLevel,
                                  1);
    }
    return 0;
}

static uint16_t ble_battery_get_attrlenCB(uint16_t u16ConnHandle, uint16_t u16AttrHandle)
{
    if (m_au16BatteryServerHandle[BLE_BATT_LEVEL_VALUE_IDX] == u16AttrHandle)
    {
        return 1;
    }
    else if (m_au16BatteryServerHandle[BLE_BATT_LEVEL_VALUE_CLIENT_CFG_IDX] == u16AttrHandle)
    {
        return 2;
    }

    return 0;
}

static uint16_t ble_battery_read_attrCB(uint16_t u16ConnHandle, uint16_t u16AttrHandle, uint16_t u16Offset,
                                        uint8_t *pu8Data, uint16_t u16DataLen)
{
    if (m_au16BatteryServerHandle[BLE_BATT_LEVEL_VALUE_IDX] == u16AttrHandle)
    {
        if (pu8Data)
        {
            battery_service_event_callback(EN_BATTERY_EVENT_READ_LEVEL, pu8Data, 1);
        }
        return 1;
    }
    else if (m_au16BatteryServerHandle[BLE_BATT_LEVEL_VALUE_CLIENT_CFG_IDX] == u16AttrHandle)
    {
        if (pu8Data)
        {
            pu8Data[0] = m_u8BatteryLevelClientCharCfg;
            pu8Data[1] = 0;
        }
        return 2;
    }
    return 0;
}

static uint32_t ble_battery_write_attrCB(uint16_t u16ConnHandle, uint16_t u16AttrHandle, uint16_t transaction_mode,
                                         uint16_t u16Offset, uint8_t *pu8Data, uint16_t u16DataLen)
{
    if (m_au16BatteryServerHandle[BLE_BATT_LEVEL_VALUE_CLIENT_CFG_IDX] == u16AttrHandle)
    {
        m_u8BatteryLevelClientCharCfg = pu8Data[0];
    }
    return 0;
}
