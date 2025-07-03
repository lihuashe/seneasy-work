/**
 * @file     hid_service.c
 * @author
 * @version  V1.0
 * @date     2021/09/04
 * @history
 * @note     Human Interface Device Service
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
#include "patch_gatt_service_api.h"

#include "stack_utility.h"
#include "ble_cb_event_def.h"

#include "app_cfg.h"

#include "ble_hid_service.h"

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

/* HID information flags */
#define HID_FLAGS_REMOTE_WAKE          (0x01) /* RemoteWake */
#define HID_FLAGS_NORMALLY_CONNECTABLE (0x02) /* NormallyConnectable */

#define HID_KEY_REPORT_ID              (0x01)
#define HID_CONSUMER_REPORT_ID         (0x02)
#define HID_VOICE_REPORT_ID            (0x5A)

/* HID Report type */
#define HID_REPORT_TYPE_INPUT          (1)

/**
 * @brief  LOCAL VARIABLES
 */

/* HID Information characteristic value */
static const uint8_t m_au8HidInfo[] = {
    0x11, 0x01,                                            /* bcdHID (USB HID version) */
    0x00,                                                  /* bCountryCode */
    HID_FLAGS_REMOTE_WAKE | HID_FLAGS_NORMALLY_CONNECTABLE /* Flags */
};

static uint16_t m_u16HandleHidInfo         = 0;
static uint16_t m_u16HandleHidControlPoint = 0;
static uint16_t m_u16HandleHidMap          = 0;

static uint16_t m_u16HandleReportKeyBord       = 0;
static uint16_t m_u16HandleReportKeyBordConfig = 0;
static uint16_t m_u16HandleKeyBordReportRef    = 0;

static const uint8_t    m_au8KbdReportRef[]              = {2, HID_REPORT_TYPE_INPUT};
static volatile uint8_t u8KeyborddReport[KBD_REPORT_NUM] = {0};
static uint16_t         m_u16KbdConfig                   = 0;

static uint16_t m_u16HandleReportConsumer       = 0;
static uint16_t m_u16HandleReportConsumerConfig = 0;
static uint16_t m_u16HandleConsumerReportRef    = 0;

static const uint8_t     m_au8ConsumerReportRef[]                  = {3, HID_REPORT_TYPE_INPUT};
static volatile uint16_t m_au16ConsumerReport[CONSUMER_REPORT_NUM] = {0};
static uint16_t          m_u16ConsumerConfig                       = 0;

/* define the HID report map */
static const uint8_t m_au8HidReportMap[] = {
    0x05, 0x01, // Usage Page (Generic Desktop)
    0x09, 0x02, // Usage (Mouse)
    0xA1, 0x01, // Collection (Application)
    0x09, 0x01, //   Usage (Pointer)
    0xA1, 0x00, //   Collection (Physical)
    0x85, 0x01, // Report Id (1)
    0x05, 0x09, //  Usage Page (Buttons)
    0x19, 0x01, //  Usage Minimum (01) - Button 1
    0x29, 0x03, //  Usage Maximum (03) - Button 3
    0x15, 0x00, //  Logical Minimum (0)
    0x25, 0x01, //  Logical Maximum (1)
    0x75, 0x01, //  Report Size (1)
    0x95, 0x03, //  Report Count (3)
    0x81, 0x02, //  Input (Data, Variable, Absolute) - Button states
    0x75, 0x05, //  Report Size (5)
    0x95, 0x01, //  Report Count (1)
    0x81, 0x01, //  Input (Constant) - Padding or Reserved bits
    0x05, 0x01, //  Usage Page (Generic Desktop)
    0x09, 0x30, //  Usage (X)
    0x09, 0x31, //  Usage (Y)
    0x09, 0x38, //  Usage (Wheel)
    0x15, 0x81, //  Logical Minimum (-127)
    0x25, 0x7F, //  Logical Maximum (127)
    0x75, 0x08, //  Report Size (8)
    0x95, 0x03, //  Report Count (3)
    0x81, 0x06, //  Input (Data, Variable, Relative) - X & Y coordinate`
    0xC0,       //   End Collection
    0xC0,       // End Collection

    0x05, 0x01, /* Usage Pg (Generic Desktop) */
    0x09, 0x06, /* Usage (Keyboard) */
    0xA1, 0x01, /* Collection: (Application) */
    0x85, 0x02, /* Report ID (2) */
    0x05, 0x07, /* Usage Pg (Key Codes) */
    0x19, 0xe0, /* Usage Min (224) */
    0x29, 0xe7, /* Usage Max (231) */
    0x15, 0x00, /* Log Min (0) */
    0x25, 0x01, /* Log Max (1 */
    0x75, 0x01, /* report Size (1) */
    0x95, 0x08, /* Report Count (8) */
    0x81, 0x02, /* Input: (Data, Array) */
    0x95, 0x01, /* Report Count (1) */
    0x75, 0x08, /* report Size (8) */
    0x81, 0x01, /* Input: constant */
    0x95, 0x05, /* report count(5) */
    0x75, 0x01, /* report size(1) */
    0x05, 0x08, /* usage pg(leds) */
    0x19, 0x01, /* usage min(1) */
    0x29, 0x05, /* usage max(5) */
    0x91, 0x02, /* output:(data, variable, absolute) */
    0x95, 0x01, /* report count(1) */
    0x75, 0x03, /* report size(3) */
    0x91, 0x01, /* output: constant */
    0x95, 0x06, /* report count(6) */
    0x75, 0x08, /* report size(8) */
    0x15, 0x00, /* log min(0) */
    0x25, 0xA5, /* log max(165) */
    0x05, 0x07, /* usage pg(key codes) */
    0x19, 0x00, /* usage min(0) */
    0x29, 0xA5, /* usage max(165) */
    0x81, 0x00, /* input:(data, array) */
    0xC0,       /* End Collection */

    0x05, 0x0C,       /* Usage Page(Consumer) */
    0x09, 0x01,       /* Usage(Consumer Control) */
    0xA1, 0x01,       /* Collection(Application) */
    0x85, 0x03,       /* Report ID (3)   */
    0x19, 0x00,       /* Usage Min (0x00) */
    0x2a, 0x9c, 0x02, /* Usage Max (0x029c) */
    0x15, 0x00,       /* Logical Min (0x00) */
    0x26, 0x9c, 0x02, /* Logical Max (0x029c) */
    0x75, 0x10,       /* Report Size (0x10) */
    0x95, 0x08,       /* Report Count (0x08) */
    0x81, 0x00,       /* Input (Data,Ary,Abs) */
    0xC0,             /* End Collection(Application) */
};

/**
 * @brief  LOCAL VARIABLES
 */
static uint16_t ble_hid_service_get_attribute_len_callback(uint16_t u16ConnHandle, uint16_t u16AttrHandle);
static uint16_t ble_hid_service_read_attr_callback(uint16_t u16ConnHandle, uint16_t u16AttrHandle, uint16_t u16Offset,
                                                   uint8_t *pu8Data, uint16_t u16DataLen);
static uint32_t ble_hid_service_write_attr_callback(uint16_t u16ConnHandle, uint16_t u16AttrHandle,
                                                    uint16_t u16TransactionMode, uint16_t u16Offset, uint8_t *pu8Data,
                                                    uint16_t u16DataLen);

static const gatt_serviceCBs_t m_stHidServiceCallBacks = {
    ble_hid_service_get_attribute_len_callback,
    ble_hid_service_read_attr_callback,
    ble_hid_service_write_attr_callback,
};

/**
 * @brief  Initializes the ble Hid service.
 * @return uint32_t: Return status.
 */
uint32_t ble_hid_service_init(void)
{
    uint16_t u16Handle = 0;

    u16Handle =
        patch_gatts_api_add_service_start(0, HID_SERV_UUID, NULL, (gatt_serviceCBs_t *)&m_stHidServiceCallBacks);
    BLE_SERVICE_RETURN_IF_ERROR(u16Handle);

    /* add char: hid infomation */
    u16Handle = 0;
    u16Handle = patch_gatts_api_add_char(0, HID_INFORMATION_UUID, NULL, ATT_PROPERTY_READ, (uint8_t *)m_au8HidInfo,
                                         sizeof(m_au8HidInfo));
    BLE_SERVICE_RETURN_IF_ERROR(u16Handle);
    m_u16HandleHidInfo = u16Handle;
    PRINTF("m_u16HandleHidInfo:%04X\r\n", m_u16HandleHidInfo);

    /* add char: hid control point */
    u16Handle = 0;
    u16Handle = patch_gatts_api_add_char(0, HID_CTRL_PT_UUID, NULL,
                                         ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC, NULL, 0);
    BLE_SERVICE_RETURN_IF_ERROR(u16Handle);
    m_u16HandleHidControlPoint = u16Handle;
    PRINTF("m_u16HandleHidControlPoint:%04X\r\n", m_u16HandleHidControlPoint);

    /* add char: hid pReport map */ // ATT_PROPERTY_AUTHENTICATION_REQUIRED
    u16Handle = 0;
    u16Handle =
        patch_gatts_api_add_char(0, REPORT_MAP_UUID, NULL, ATT_PROPERTY_READ | ATT_PROPERTY_AUTHENTICATION_REQUIRED,
                                 (uint8_t *)m_au8HidReportMap, sizeof(m_au8HidReportMap));
    BLE_SERVICE_RETURN_IF_ERROR(u16Handle);
    m_u16HandleHidMap = u16Handle;
    PRINTF("m_u16HandleHidMap:%04X,size:%d\r\n", m_u16HandleHidMap, sizeof(m_au8HidReportMap));

    /* add char: HID Report characteristic,keyboard input */
    u16Handle = 0;
    u16Handle = patch_gatts_api_add_char(
        0, REPORT_UUID, NULL,
        ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC | ATT_PROPERTY_AUTHENTICATION_REQUIRED, NULL, 0);
    BLE_SERVICE_RETURN_IF_ERROR(u16Handle);
    m_u16HandleReportKeyBord = u16Handle;
    PRINTF("m_u16HandleReportKeyBord:%04X\r\n", m_u16HandleReportKeyBord);

    /* HID Report characteristic client characteristic configuration */
    u16Handle = 0;
    u16Handle = rom_gatts_api_add_char_descrip_client_config();
    BLE_SERVICE_RETURN_IF_ERROR(u16Handle);
    m_u16HandleReportKeyBordConfig = u16Handle;
    PRINTF("m_u16HandleReportKeyBordConfig:%04X\r\n", m_u16HandleReportKeyBordConfig);

    /* HID Report Reference characteristic descriptor, keyboard input */
    u16Handle = 0;
    u16Handle = rom_gatts_api_add_char_descrip(GATT_REPORT_REF_UUID, ATT_PROPERTY_READ, (uint8_t *)m_au8KbdReportRef,
                                               sizeof(m_au8KbdReportRef));
    BLE_SERVICE_RETURN_IF_ERROR(u16Handle);
    m_u16HandleKeyBordReportRef = u16Handle;
    PRINTF("m_u16HandleKeyBordReportRef:%04X\r\n", m_u16HandleKeyBordReportRef);
    /* add char:   HID Report characteristic,consumer control input */
    u16Handle = 0;
    u16Handle = patch_gatts_api_add_char(
        0, REPORT_UUID, NULL,
        ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC | ATT_PROPERTY_AUTHENTICATION_REQUIRED, NULL, 0);
    BLE_SERVICE_RETURN_IF_ERROR(u16Handle);
    m_u16HandleReportConsumer = u16Handle;
    PRINTF("m_u16HandleReportConsumer:%04X\r\n", m_u16HandleReportConsumer);

    /* HID Report characteristic client characteristic configuration */
    u16Handle = 0;
    u16Handle = rom_gatts_api_add_char_descrip_client_config();
    BLE_SERVICE_RETURN_IF_ERROR(u16Handle);
    m_u16HandleReportConsumerConfig = u16Handle;
    PRINTF("m_u16HandleReportConsumerConfig:%04X\r\n", m_u16HandleReportConsumerConfig);

    /* HID Report Reference characteristic descriptor, keyboard input */
    u16Handle = 0;
    u16Handle = rom_gatts_api_add_char_descrip(GATT_REPORT_REF_UUID, ATT_PROPERTY_READ,
                                               (uint8_t *)m_au8ConsumerReportRef, sizeof(m_au8ConsumerReportRef));
    BLE_SERVICE_RETURN_IF_ERROR(u16Handle);
    m_u16HandleConsumerReportRef = u16Handle;
    PRINTF("m_u16HandleConsumerReportRef:%04X\r\n", m_u16HandleConsumerReportRef);
    patch_gatts_api_add_service_end();

    PRINTF("hid servive init done.\r\n");
    return 0;
}

/**
 * @brief  Report hid value (type:0x07).
 * @param  u16ConnHandle: Connect handle.
 * @param  pReport: Pointer to the value you want to report.
 * @param  u8DateLen: The length.
 * @return status: Return status.
 */
bool ble_hid_kbd_report(uint16_t u16ConnHandle, uint8_t *pReport, uint8_t u8DateLen)
{
    uint8_t u8Ret = 0;
    if ((NULL == pReport) || (0 == u8DateLen) || (KBD_REPORT_NUM < u8DateLen))
    {
        return false;
    }

    u8Ret = rom_gatts_api_send_notify(u16ConnHandle, m_u16HandleReportKeyBord, pReport, u8DateLen);
    if (u8Ret)
    {
        PRINTF("hid base send fail : %d\r\n", u8Ret);
        return false;
    }
    PRINTF("rcu.notify.HID(07): 0x");
    for (int i = 0; i < u8DateLen; i++)
    {
        PRINTF("%02X ", pReport[i]);
    }
    PRINTF("\n");
    return true;
}

/**
 * @brief  Report hid value (type:0x0C).
 * @param  u16ConnHandle: Connect handle.
 * @param  pReport: Pointer to the value you want to report.
 * @param  u8DateLen: The length.
 * @return status: Return status.
 */
bool ble_hid_consumer_report(uint16_t u16ConnHandle, uint16_t *pReport, uint8_t u8DateLen)
{
    uint8_t u8Ret = 0;
    if ((NULL == pReport) || (0 == u8DateLen) || ((CONSUMER_REPORT_NUM << 1) < u8DateLen))
    {
        return false;
    }

    u8Ret = rom_gatts_api_send_notify(u16ConnHandle, m_u16HandleReportConsumer, (uint8_t *)pReport, u8DateLen);
    if (u8Ret)
    {
        PRINTF("hid consumer send fail : %d\r\n", u8Ret);
        return false;
    }
    PRINTF("rcu.notify.HID(0C): 0x");
    for (int i = 0; i < u8DateLen; i++)
    {
        PRINTF("%04X ", pReport[i]);
    }
    PRINTF("\n");
    return true;
}

static uint16_t ble_hid_service_get_attribute_len_callback(uint16_t u16ConnHandle, uint16_t u16AttrHandle)
{
    PRINTF("hid get len handle = %04X\r\n", u16AttrHandle);
    if (u16AttrHandle == m_u16HandleHidInfo)
    {
        PRINTF("m_au8HidInfo = %04X\r\n", sizeof(m_au8HidInfo));
        return sizeof(m_au8HidInfo);
    }
    else if (u16AttrHandle == m_u16HandleHidMap)
    {
        PRINTF("m_au8HidReportMap= %04X\r\n", sizeof(m_au8HidReportMap));
        return sizeof(m_au8HidReportMap);
    }

    else if (u16AttrHandle == m_u16HandleReportKeyBord)
    {
        PRINTF("u8KeyborddReport= %04X\r\n", sizeof(u8KeyborddReport));
        return sizeof(u8KeyborddReport);
    }
    else if (u16AttrHandle == m_u16HandleReportKeyBordConfig)
    {
        PRINTF("m_u16KbdConfig= %04X\r\n", sizeof(m_u16KbdConfig));
        return sizeof(m_u16KbdConfig);
    }

    else if (u16AttrHandle == m_u16HandleReportConsumer)
    {
        PRINTF("m_au16ConsumerReport= %04X\r\n", sizeof(m_au16ConsumerReport));
        return sizeof(m_au16ConsumerReport);
    }

    else if (u16AttrHandle == m_u16HandleReportConsumerConfig)
    {
        PRINTF("m_u16HandleReportConsumerConfig = %04X\r\n", 2);
        return 2;
    }
    else if (u16AttrHandle == m_u16HandleKeyBordReportRef)
    {
        PRINTF("m_u16HandleKeyBordReportRef = %04X\r\n", m_u16HandleKeyBordReportRef);
        return sizeof(m_au8KbdReportRef);
    }
    else if (u16AttrHandle == m_u16HandleConsumerReportRef)
    {
        PRINTF("m_u16HandleConsumerReportRef = %04X\r\n", m_u16HandleConsumerReportRef);
        return sizeof(m_au8ConsumerReportRef);
    }
    return 0;
}

static uint16_t ble_hid_service_read_attr_callback(uint16_t u16ConnHandle, uint16_t u16AttrHandle, uint16_t u16Offset,
                                                   uint8_t *pu8Data, uint16_t u16DataLen)
{
    PRINTF("hid read handle = %04X,offset = %d,buff size:%d\r\n", u16AttrHandle, u16Offset, u16DataLen);
    if (u16AttrHandle == m_u16HandleHidInfo)
    {
        memcpy(pu8Data, (uint8_t *)m_au8HidInfo + u16Offset, u16DataLen);
        return u16DataLen;
    }
    else if (u16AttrHandle == m_u16HandleHidMap)
    {
        memcpy(pu8Data, m_au8HidReportMap + u16Offset, u16DataLen);
        return sizeof(u16DataLen);
    }

    else if (u16AttrHandle == m_u16HandleReportKeyBord)
    {
        memcpy(pu8Data, (uint8_t *)u8KeyborddReport + u16Offset, u16DataLen);
        return u16DataLen;
    }
    else if (u16AttrHandle == m_u16HandleReportKeyBordConfig)
    {
        memcpy(pu8Data, (uint8_t *)&m_u16KbdConfig, u16DataLen);
        return sizeof(m_u16KbdConfig);
    }

    else if (u16AttrHandle == m_u16HandleReportConsumer)
    {
        memcpy(pu8Data, (uint8_t *)m_au16ConsumerReport + u16Offset, u16DataLen);
        return u16DataLen;
    }

    else if (u16AttrHandle == m_u16HandleReportConsumerConfig)
    {
        memcpy(pu8Data, (uint8_t *)&m_u16ConsumerConfig, u16DataLen);
        return sizeof(u16DataLen);
    }
    else if (u16AttrHandle == m_u16HandleKeyBordReportRef)
    {
        PRINTF("m_u16HandleKeyBordReportRef = %04X\r\n", m_u16HandleKeyBordReportRef);
        memcpy(pu8Data, (uint8_t *)m_au8KbdReportRef, u16DataLen);
        return sizeof(u16DataLen);
    }
    else if (u16AttrHandle == m_u16HandleConsumerReportRef)
    {
        PRINTF("m_u16HandleConsumerReportRef = %04X\r\n", m_u16HandleConsumerReportRef);
        memcpy(pu8Data, (uint8_t *)m_au8ConsumerReportRef, u16DataLen);
        return sizeof(u16DataLen);
    }

    return 0;
}

static uint32_t ble_hid_service_write_attr_callback(uint16_t u16ConnHandle, uint16_t u16AttrHandle,
                                                    uint16_t u16TransactionMode, uint16_t u16Offset, uint8_t *pu8Data,
                                                    uint16_t u16DataLen)
{
    PRINTF("hid write handle = %04X,offset = %d\r\n", u16AttrHandle, u16Offset);
    if (u16AttrHandle == m_u16HandleHidControlPoint)
    {
    }
    else if (u16AttrHandle == m_u16HandleReportKeyBordConfig)
    {
        m_u16KbdConfig = pu8Data[0] + (pu8Data[1] << 8);
    }
    else if (u16AttrHandle == m_u16HandleReportConsumerConfig)
    {
        m_u16HandleReportConsumerConfig = pu8Data[0] + (pu8Data[1] << 8);
    }
    return 0;
}