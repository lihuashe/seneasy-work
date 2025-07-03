/**********************************************************************************************************************
 * @file     fw_upgrade_Interface_ota.c
 * @version  V3.0
 * @date     2022/07/29
 * @history
 * @note     OTA Service
 **********************************************************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2022, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 *********************************************************************************************************************/


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "gatt_service_api.h"

#include "fw_upgrade_interface_ota.h"


//=====================================================================================================================
// TYPEDEF
//=====================================================================================================================
#define MXD_ERR_SUCCESS      0
#define MXD_ERR_FAILURE      1

#define PRINTF(fmt, args...)

#define BLE_SERVICE_RETURN_IF_ERROR(u16Handle)            \
        if (u16Handle == 0)                               \
        {                                                 \
            PRINTF("ble ota service init failure.\n");    \
            return MXD_ERR_FAILURE;                       \
        }                                                 \


//=====================================================================================================================
// TYPEDEF
//=====================================================================================================================
typedef enum
{
    BLE_OTA_HANDLE_WRITE      = 0,
    BLE_OTA_HANDLE_NOTIFY     = 1,
    BLE_OTA_HANDLE_NOTIFY_CFG = 2,
    BLE_OTA_HANDLE_MAX        = 3

} enBleOtaHandle_t;


//=====================================================================================================================
// LOCAL VARIABLES
//=====================================================================================================================
static uint16_t m_u16BleOtaServiceHandle[BLE_OTA_HANDLE_MAX];

static uint16_t u16BleClientCfg; // Notify enable or disable flag.

static ble_ota_service_receive_data_callback_t m_pfnRcvData = NULL;


static uint16_t ble_ota_service_get_attr_len_cb(uint16_t u16ConnHandle, uint16_t u16AttrHandle);
static uint16_t ble_ota_service_read_cb(uint16_t u16ConnHandle, uint16_t u16AttrHandle,
                                     uint16_t u16Offset, uint8_t *pu8Data, uint16_t u16DataLen);
static uint32_t ble_ota_service_write_cb(uint16_t u16ConnHandle, uint16_t u16AttrHandle, uint16_t u16TransactionMode,
                                      uint16_t u16Offset, uint8_t *pu8Data, uint16_t u16DataLen);
static const gatt_serviceCBs_t m_stBleTtaServiceCBs =
{
    ble_ota_service_get_attr_len_cb,
    ble_ota_service_read_cb,
    ble_ota_service_write_cb,
};


/**
 * @brief  ble ota service init.
 * @return status, @ref
 */
uint32_t ble_ota_service_init(ble_ota_service_receive_data_callback_t pfnCallback)
{
    uint16_t u16Handle = 0;

    m_pfnRcvData = pfnCallback;

    u16Handle = rom_gatts_api_add_service_start(0, MXD_BLE_OTA_SERVICE_UUID, NULL, (gatt_serviceCBs_t *)&m_stBleTtaServiceCBs);
    BLE_SERVICE_RETURN_IF_ERROR(u16Handle);

    u16Handle = rom_gatts_api_add_char(0, MXD_BLE_OTA_WRITE_UUID, NULL,
                               (ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC),
                               NULL, 0);
    BLE_SERVICE_RETURN_IF_ERROR(u16Handle);
    m_u16BleOtaServiceHandle[BLE_OTA_HANDLE_WRITE] = u16Handle;

    u16Handle = rom_gatts_api_add_char(0, MXD_BLE_OTA_NOTIFY_UUID, NULL,
                               (ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC),
                               NULL, 0);
    BLE_SERVICE_RETURN_IF_ERROR(u16Handle);
    m_u16BleOtaServiceHandle[BLE_OTA_HANDLE_NOTIFY] = u16Handle;

    u16Handle = rom_gatts_api_add_char_descrip_client_config();
    BLE_SERVICE_RETURN_IF_ERROR(u16Handle);
    m_u16BleOtaServiceHandle[BLE_OTA_HANDLE_NOTIFY_CFG] = u16Handle;

    rom_gatts_api_add_service_end();

    return MXD_ERR_SUCCESS;
}

/**
 * @brief  Get ble attribute length.
 * @param  u16Handle: Ble connected handle.
 * @return u16Length: Length.
 */
static uint16_t ble_ota_service_get_attr_len_cb(uint16_t u16ConnHandle, uint16_t u16AttrHandle)
{
    if (m_u16BleOtaServiceHandle[BLE_OTA_HANDLE_NOTIFY_CFG] == u16AttrHandle)
    {
        return 2;
    }

    return 0;
}

/**
 * @brief  Ble read.
 * @param  u16Handle: Ble connected handle.
 * @param  u16Offset: Offset.
 * @param  pu8Buf: Pointer to read buffer.
 * @param  u16Len: Buffer length.
 * @return u16Length: Length.
 */
static uint16_t ble_ota_service_read_cb(uint16_t u16ConnHandle, uint16_t u16AttrHandle,
                                     uint16_t u16Offset, uint8_t *pu8Data, uint16_t u16DataLen)
{
    if (m_u16BleOtaServiceHandle[BLE_OTA_HANDLE_NOTIFY_CFG] == u16AttrHandle)
    {
        if (pu8Data)
        {
            pu8Data[0] = u16BleClientCfg;
            pu8Data[1] = 0;
        }

        return 2;
    }

    return 0;
}

/**
 * @brief  Enable ble notify.
 * @param  pu8Buf: Pointer to read buffer.
 * @param  u16Len: Buffer length.
 */
static void ble_ota_service_write_cccd(uint8_t *pu8Data, uint16_t u16DataLen)
{
    if (u16DataLen > 0)
    {
        u16BleClientCfg = pu8Data[0];

        if (u16BleClientCfg & GATT_CLIENT_CFG_NOTIFY)
        {
        }
    }
}

/**
 * @brief  Ble write data to SoC.
 * @param  u16Handle: Ble connected handle.
 * @param  u16Mode: Mode.
 * @param  u16Offset: Offset.
 * @param  pu8Buf: Pointer to read buffer.
 * @param  u16Len: Buffer length.
 * @return status.
 */
static uint32_t ble_ota_service_write_cb(uint16_t u16ConnHandle, uint16_t u16AttrHandle, uint16_t u16TransactionMode,
                                      uint16_t u16Offset, uint8_t *pu8Data, uint16_t u16DataLen)
{
    if (m_u16BleOtaServiceHandle[BLE_OTA_HANDLE_WRITE] == u16AttrHandle)
    {
        //ble_ota_service_write_data(pu8Buf, u16Len);
        m_pfnRcvData(pu8Data, u16DataLen);
    }
    else if (m_u16BleOtaServiceHandle[BLE_OTA_HANDLE_NOTIFY_CFG] == u16AttrHandle)
    {
        ble_ota_service_write_cccd(pu8Data, u16DataLen);
    }

    return MXD_ERR_SUCCESS;
}

/**
 * @brief  Send notification data.
 * @param  u16Handle: Ble connected handle.
 * @param  pu8Buf: Pointer to write buffer.
 * @param  u16Len: Buffer length.
 * @return status.
 */
uint32_t ble_ota_service_send_notify(uint16_t u16ConnHandle, uint8_t *pu8Data, uint16_t u16DataLen)
{
    return rom_gatts_api_send_notify(u16ConnHandle, m_u16BleOtaServiceHandle[BLE_OTA_HANDLE_NOTIFY], pu8Data, u16DataLen);
}

