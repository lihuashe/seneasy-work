#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "utility.h"
#include "app_cfg.h"
#include "err_def.h"

#include "gatt_uuid_def.h"
#include "ble_central.h"
#include "adpcm.h"
#include "crc.h"
#include "fw_upgrade_process.h"
#include "ota_client.h"


//=====================================================================================================================
// LOCAL VARIABLES
//=====================================================================================================================
static uint8_t u8OtaBuff[OTA_DATA_SIZE];
static uint16_t u16OtaWriteCharHandle = 0;
static uint16_t u16OtaNotifyCharHandle = 0;
static uint16_t u16OtaNotifyCharDesCfgHandle = 0;

ota_client_event_callback_t ota_client_event_callback;

/**********************************************************************************************************************
 * @brief  ota client get service handle
 * @param  pu16StartHandle: start handle
           pu16EndHandle: end handle
 * @output
 * @return None.
 *********************************************************************************************************************/
void ota_client_get_srvice_handle(uint16_t *pu16StartHandle, uint16_t *pu16EndHandle)
{
    ble_central_get_service_handle_by_uuid(0, MXD_OTA_SERVICE_UUID, NULL, pu16StartHandle, pu16EndHandle);
}

/**********************************************************************************************************************
 * @brief  ota client get characteristic handle
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
void ota_client_get_char_handle(void)
{
    if(!ble_central_get_char_value_handle_by_uuid(0, MXD_OTA_WRITE_UUID, NULL, &u16OtaWriteCharHandle, 0))
    {
        PRINTF("ota write char not found\n");
        return;
    }
    if(!ble_central_get_char_value_handle_by_uuid(0, MXD_OTA_NOTIFY_UUID, NULL, &u16OtaNotifyCharHandle, 0))
    {
        PRINTF("ota notify char not found\n");
        return;
    }
    if(!ble_central_get_char_des_by_char_handle(u16OtaNotifyCharHandle, GATT_CLIENT_CHAR_CFG_UUID, &u16OtaNotifyCharDesCfgHandle))
    {
        PRINTF("ota notify char cfg des not found\n");
        return;
    }
 
    if(ota_client_event_callback)
    {
        ota_client_event_callback(EN_OTA_CLIENT_EVENT_CHAR_FIND_FINISH, NULL, 0);
    }
}

/**********************************************************************************************************************
 * @brief  ota client parse notify characteristic description config write result
 * @param  u8Result: 0 - write ok, 1 - write fail
 * @output
 * @return None.
 *********************************************************************************************************************/
void ota_client_parse_notify_char_des_cfg_write_result(uint8_t u8Result)
{
    if(0 == u8Result)
    {
        PRINTF("ota ctrl char des cfg write ok\n");
        if(ota_client_event_callback)
        {
            ota_client_event_callback(EN_OTA_CLIENT_EVENT_CHAR_EN_NTF_FINISH, NULL, 0);
        }
    }
}

/**********************************************************************************************************************
 * @brief  ota client parse write characteristic write result
 * @param  u8Result: 0 - write ok, 1 - write fail
 * @output
 * @return None.
 *********************************************************************************************************************/
static void ota_client_parse_write_char_write_result(uint8_t u8Result)
{
    if(0 == u8Result)
    {
        PRINTF("ota client write data ok\n");
    }
    else
    {
        PRINTF("ota client write data result %d\n", u8Result);
    }
}

/**********************************************************************************************************************
 * @brief  ota client enable notify
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
void ota_client_enable_notify(void)
{
    uint8_t u8Data[2] = {1, 0};
    if(0 != u16OtaNotifyCharDesCfgHandle)
    {
        if(!ble_central_write_char_value(ble_central_get_conn_handle(), u16OtaNotifyCharDesCfgHandle, u8Data, sizeof(u8Data), ota_client_parse_notify_char_des_cfg_write_result))
        {
            PRINTF("ota notify char des cfg fail\n");
        }
    }
}

/**********************************************************************************************************************
 * @brief  ota client process notify data
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
void ota_client_process_notify_data(uint8_t *pu8Data, uint16_t u16DataLen)
{
    uint16_t u16CharValueHandle = rom_little_endian_read_16(pu8Data, 0);
    u16DataLen -= 2;
    memcpy(pu8Data, pu8Data + 2, u16DataLen);
    if(u16CharValueHandle == u16OtaNotifyCharHandle)
    {
        ota_client_event_callback(EN_OTA_CLIENT_EVENT_DATA, pu8Data, u16DataLen);
    }
}

/**********************************************************************************************************************
 * @brief  ota client send data
 * @param  uint8_t *pu8OtaBuff: OTA data buff
           uint16_t u16Len: OTA data length
 * @output
 * @return None.
 *********************************************************************************************************************/
bool ota_client_send_data(uint8_t *pu8OtaBuff, uint16_t u16Len)
{
    if( u16Len > OTA_DATA_SIZE )
    {
        PRINTF("ota client send data is too large\n");
        return false;
    }
    
//    memset( u8OtaBuff, 0, OTA_DATA_SIZE );
//    memcpy( u8OtaBuff, pu8OtaBuff, u16Len );
    
    if(!ble_central_write_char_value(ble_central_get_conn_handle(), u16OtaWriteCharHandle, pu8OtaBuff, u16Len, ota_client_parse_write_char_write_result))
    {
        PRINTF("ota client send data fail\n");
    }

    return true;
}

/**********************************************************************************************************************
 * @brief  ota client event callback regisest
 * @param  ota_client_event_callback_t: callback
 * @output
 * @return None.
 *********************************************************************************************************************/
void ota_client_event_callback_regiest(ota_client_event_callback_t cb)
{
    if(cb)
    {
        ota_client_event_callback = cb;
    }
}
