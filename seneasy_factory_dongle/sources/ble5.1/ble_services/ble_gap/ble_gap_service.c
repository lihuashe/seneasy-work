/**
 * @file     ble_gap_service.c
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2021/12/5
 * @history
 * @note     ble gap service.
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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "err_def.h"
#include "gatt_uuid_def.h"
#include "gatt_service_api.h"
#include "patch_gatt_service_api.h"

#include "utility.h"

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

#define DEVICE_NAME_MAX_LEN         32

// Central Address Resolution Characteristic.
#define BLE_UUID_CAR_CHARACTERISTIC 0x2AA6

/**
 * @brief  BLE Handle
 */
typedef enum
{
    BLE_GAPS_HANDLE_DEVICE_NAME,
    BLE_GAPS_HANDLE_APPEARANCE,
    BLE_GAPS_HANDLE_CAR, // CENTRAL_ADDRESS_RESOLUTION
    BLE_GAPS_HANDLE_MAX

} enBleMusHandle_t;

/**
 * @brief  LOCAL VARIABLES
 */
static uint16_t m_u16BleGapServiceHandle[BLE_GAPS_HANDLE_MAX];
static uint8_t  m_au8DevName[DEVICE_NAME_MAX_LEN];
static uint8_t  m_au8Apperance[2];
static uint8_t  m_u8SupportAddressResolution = 0;

// ble gap services callbacks.
static uint16_t ble_gap_get_attr_len_cb(uint16_t u16ConnHandle, uint16_t u16AttrHandle);
static uint16_t ble_gap_read_attr_cb(uint16_t u16ConnHandle, uint16_t u16AttrHandle, uint16_t u16Offset,
                                     uint8_t *pu8Data, uint16_t u16DataLen);
static uint32_t ble_gap_write_attr_cb(uint16_t u16ConnHandle, uint16_t u16AttrHandle, uint16_t u16TransactionMode,
                                      uint16_t u16Offset, uint8_t *pu8Data, uint16_t u16DataLen);

static const gatt_serviceCBs_t m_stGattCBs = {
    ble_gap_get_attr_len_cb,
    ble_gap_read_attr_cb,  // Read callback function pointer
    ble_gap_write_attr_cb, // Write callback function pointer
};

/**
 * @brief  Initializing the ble gap service.
 * @param  u16Apperance: Pointer to function for handling received data.
 * @param  pu8DevName: Pointer to device name.
 * @param  u8DevNameLen: Device name length.
 * @return uint32_t: Return status.
 */
uint32_t ble_gap_service_init(uint8_t *pu8DevName, uint8_t u8DevNameLen, uint16_t u16Apperance)
{
    uint16_t u16Handle = 0;

    // Device name
    u8DevNameLen = (u8DevNameLen > DEVICE_NAME_MAX_LEN) ? DEVICE_NAME_MAX_LEN : u8DevNameLen;
    memcpy(m_au8DevName, pu8DevName, u8DevNameLen);

    // Apperance
    m_au8Apperance[0] = LO_UINT16(u16Apperance);
    m_au8Apperance[1] = HI_UINT16(u16Apperance);

    // Add the service.
    u16Handle = patch_gatts_api_add_service_start(0, GAP_SERVICE_UUID, NULL, (gatt_serviceCBs_t *)&m_stGattCBs);
    BLE_SERVICE_RETURN_IF_ERROR(u16Handle);

    // Add the device name Characteristic.
    u16Handle = patch_gatts_api_add_char(0, DEVICE_NAME_UUID, NULL, ATT_PROPERTY_READ, m_au8DevName, u8DevNameLen);
    BLE_SERVICE_RETURN_IF_ERROR(u16Handle);
    m_u16BleGapServiceHandle[BLE_GAPS_HANDLE_DEVICE_NAME] = u16Handle;

    // Add the appearance Characteristic.
    u16Handle =
        patch_gatts_api_add_char(0, APPEARANCE_UUID, NULL, ATT_PROPERTY_READ, m_au8Apperance, sizeof(m_au8Apperance));
    BLE_SERVICE_RETURN_IF_ERROR(u16Handle);
    m_u16BleGapServiceHandle[BLE_GAPS_HANDLE_APPEARANCE] = u16Handle;

    // Add the Central Address Resolution Characteristic.
    u16Handle = patch_gatts_api_add_char(0, BLE_UUID_CAR_CHARACTERISTIC, NULL,
                                         (ATT_PROPERTY_READ | ATT_PROPERTY_DYNAMIC), NULL, 0);
    BLE_SERVICE_RETURN_IF_ERROR(u16Handle);
    m_u16BleGapServiceHandle[BLE_GAPS_HANDLE_CAR] = u16Handle;

    patch_gatts_api_add_service_end();

    return ERR_STA_OK;
}

static uint16_t ble_gap_get_attr_len_cb(uint16_t u16ConnHandle, uint16_t u16AttrHandle)
{
    if (m_u16BleGapServiceHandle[BLE_GAPS_HANDLE_CAR] == u16AttrHandle)
    {
        return 1;
    }

    return 0;
}

static uint16_t ble_gap_read_attr_cb(uint16_t u16ConnHandle, uint16_t u16AttrHandle, uint16_t u16Offset,
                                     uint8_t *pu8Data, uint16_t u16DataLen)
{
    if (m_u16BleGapServiceHandle[BLE_GAPS_HANDLE_CAR] == u16AttrHandle)
    {
        if (pu8Data)
        {
            pu8Data[0] = m_u8SupportAddressResolution;
        }

        return 1;
    }

    return 0;
}

uint32_t ble_gap_write_attr_cb(uint16_t u16ConnHandle, uint16_t u16AttrHandle, uint16_t u16TransactionMode,
                               uint16_t u16Offset, uint8_t *pu8Data, uint16_t u16DataLen)
{
    return 0;
}

/**
 * @brief  Set the Maxscend UART Service.
 * @param  pu8DevName: Pointer to device name.
 * @param  u8DevNameLen: Device name length.
 */
void ble_gap_service_set_device_name(uint8_t *pu8DevName, uint16_t u8DevNameLen)
{
    memcpy(m_au8DevName, pu8DevName, u8DevNameLen);
}
