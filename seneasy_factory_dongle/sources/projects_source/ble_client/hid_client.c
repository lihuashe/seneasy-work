#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "utility.h"
#include "app_cfg.h"


#include "gatt_uuid_def.h"
#include "gatt_client_api.h"
#include "ble_central.h"

#include "hid_client.h"

#define HID_REPORT_CHAR_NUM     (2)

static uint16_t gu16HidServiceStartHandle = 0;
static uint16_t gu16HidServiceEndHandle = 0;

static uint16_t gu16HidReportCharValueHandle = 0;
static uint16_t gu16HidReportCharDesCfgHandle = 0;
static uint16_t gu16HidReportCharDesHandle = 0;

static uint16_t gu16KeyReportCharValueHandle = 0;
static uint16_t gu16KeyReportCharDesCfgHandle = 0;
static uint16_t gu16KeyReportCharDesHandle = 0;

static uint16_t gu16ConsumerReportCharValueHandle = 0;
static uint16_t gu16ConsumerReportCharDesCfgHandle = 0;
static uint16_t gu16ConsumerReportCharDesHandle = 0;

static uint8_t gu8HidReportCharFindIdx = 0;

hid_client_event_callback_t hid_client_event_callback;

static void hid_client_parse_char_read_result(uint8_t *pu8Data, uint16_t u16DataLen)
{
    if((NULL == pu8Data) || (0 == u16DataLen))
    {
        return;
    }

    if((u16DataLen == 2) && (0x02 == pu8Data[0]) && (0x01 == pu8Data[1]))
    {
        gu16KeyReportCharValueHandle = gu16HidReportCharValueHandle;
        gu16KeyReportCharDesCfgHandle = gu16HidReportCharDesCfgHandle;
        gu16KeyReportCharDesHandle = gu16HidReportCharDesHandle;
        PRINTF("hid key report char found char handle %d, des cfg handle %d, char des handle %d\n", gu16KeyReportCharValueHandle, gu16KeyReportCharDesCfgHandle, gu16KeyReportCharDesHandle);
    }
    else if((u16DataLen == 2) && (0x03 == pu8Data[0]) && (0x01 == pu8Data[1]))
    {
        gu16ConsumerReportCharValueHandle = gu16HidReportCharValueHandle;
        gu16ConsumerReportCharDesCfgHandle = gu16HidReportCharDesCfgHandle;
        gu16ConsumerReportCharDesHandle = gu16HidReportCharDesHandle;
        PRINTF("hid consumer report char found char handle %d, des cfg handle %d, char des handle %d\n", gu16ConsumerReportCharValueHandle, gu16ConsumerReportCharDesCfgHandle, gu16ConsumerReportCharDesHandle);
    }

    if(gu8HidReportCharFindIdx < HID_REPORT_CHAR_NUM)
    {
        PRINTF("find next report char %d\n", gu8HidReportCharFindIdx);
        hid_client_get_char_handle();
    }
    else
    {
        PRINTF("hid report char found finish\n");
        gu8HidReportCharFindIdx = 0;
        if(hid_client_event_callback)
        {
            hid_client_event_callback(EN_HID_CLIENT_EVENT_CHAR_FIND_FINISH, NULL, 0);
        }
    }
}

static void hid_client_parse_consumer_char_des_cfg_write_result(uint8_t u8Result);
static void hid_client_parse_key_char_des_cfg_write_result(uint8_t u8Result)
{
    if(0 == u8Result)
    {
        PRINTF("hid key report char des cfg write ok\n");
    }
    uint8_t u8Data[2] = {1, 0};
    if(0 != gu16ConsumerReportCharDesCfgHandle)
    {
        if(!ble_central_write_char_value(ble_central_get_conn_handle(), gu16ConsumerReportCharDesCfgHandle, u8Data, sizeof(u8Data), hid_client_parse_consumer_char_des_cfg_write_result))
        {
            PRINTF("hid report char des cfg write fail\n");
        }
    }
}
static void hid_client_parse_consumer_char_des_cfg_write_result(uint8_t u8Result)
{
    if(0 == u8Result)
    {
        PRINTF("hid consumer report char des cfg write ok\n");
        if(hid_client_event_callback)
        {
            hid_client_event_callback(EN_HID_CLIENT_EVENT_CHAR_EN_NTF_FINISH, NULL, 0);
        }
    }
}

void hid_client_get_char_handle(void)
{
    if(ble_central_get_char_value_handle_by_uuid(0, REPORT_UUID, NULL, &gu16HidReportCharValueHandle, gu8HidReportCharFindIdx))
    {
        gu8HidReportCharFindIdx++;
        if(!ble_central_get_char_des_by_char_handle(gu16HidReportCharValueHandle, GATT_CLIENT_CHAR_CFG_UUID, &gu16HidReportCharDesCfgHandle))
        {
            PRINTF("hid report char des cfg not found %d\n", gu8HidReportCharFindIdx);
            return;
        }
        if(!ble_central_get_char_des_by_char_handle(gu16HidReportCharValueHandle, GATT_REPORT_REF_UUID, &gu16HidReportCharDesHandle))
        {
            PRINTF("hid report char des not found %d\n", gu8HidReportCharFindIdx);
            return;
        }
        if(!ble_central_read_char_value(ble_central_get_conn_handle(), gu16HidReportCharDesHandle, hid_client_parse_char_read_result))
        {
            PRINTF("hid report char des read fail %d\n", gu8HidReportCharFindIdx);
            return;
        }
        PRINTF("wait read result charhandle%d->%d\n", gu16HidReportCharValueHandle, gu16HidReportCharDesHandle);
    }
    else
    {
        PRINTF("hid report char handle not found\n");
    }
}

void hid_client_enable_notify(void)
{
    uint8_t u8Data[2] = {1, 0};
    if(0 != gu16KeyReportCharDesCfgHandle)
    {
        if(!ble_central_write_char_value(ble_central_get_conn_handle(), gu16KeyReportCharDesCfgHandle, u8Data, sizeof(u8Data), hid_client_parse_key_char_des_cfg_write_result))
        {
            PRINTF("hid report char des cfg write fail\n");
        }
    }
    else
    {
        PRINTF("hid_client_enable_notify fail Handle = %d\n", gu16KeyReportCharDesCfgHandle);
    }
}

void hid_client_get_srvice_handle(uint16_t *pu16StartHandle, uint16_t *pu16EndHandle)
{
    ble_central_get_service_handle_by_uuid(0, HID_SERV_UUID, NULL, &gu16HidServiceStartHandle, &gu16HidServiceEndHandle);
    *pu16StartHandle = gu16HidServiceStartHandle;
    *pu16EndHandle = gu16HidServiceEndHandle;
}

void hid_client_process_notify_data(uint8_t *pu8Data, uint16_t u16DataLen)
{
    uint16_t u16CharValueHandle = rom_little_endian_read_16(pu8Data, 0);
    if(u16CharValueHandle == gu16KeyReportCharValueHandle)
    {
        if(NULL != hid_client_event_callback)
        {
            hid_client_event_callback(EN_HID_CLIENT_EVENT_KEY_CHAR_DATA, pu8Data+2, u16DataLen-2);
        }
    }
    else if(u16CharValueHandle == gu16ConsumerReportCharValueHandle)
    {
        if(NULL != hid_client_event_callback)
        {
            hid_client_event_callback(EN_HID_CLIENT_EVENT_CONSUMER_CHAR_DATA, pu8Data+2, u16DataLen-2);
        }
    }
}

void hid_client_event_callback_regiest(hid_client_event_callback_t cb)
{
    if(NULL != cb)
    {
        hid_client_event_callback = cb;
    }
}