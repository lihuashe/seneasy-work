/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     hid_profile.c
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "hid_profile.h"
#include "stack/ble/ble.h"
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
const u16 extReportRefUUID         = GATT_UUID_EXT_REPORT_REF;
const u16 reportRefUUID            = GATT_UUID_REPORT_REF;
const u16 my_hidServiceUUID        = SERVICE_UUID_HUMAN_INTERFACE_DEVICE;
const u16 hidServiceUUID           = SERVICE_UUID_HUMAN_INTERFACE_DEVICE;
const u16 hidProtocolModeUUID      = CHARACTERISTIC_UUID_HID_PROTOCOL_MODE;
const u16 hidReportUUID            = CHARACTERISTIC_UUID_HID_REPORT;
const u16 hidReportMapUUID         = CHARACTERISTIC_UUID_HID_REPORT_MAP;
const u16 hidbootKeyInReportUUID   = CHARACTERISTIC_UUID_HID_BOOT_KEY_INPUT;
const u16 hidbootKeyOutReportUUID  = CHARACTERISTIC_UUID_HID_BOOT_KEY_OUTPUT;
const u16 hidbootMouseInReportUUID = CHARACTERISTIC_UUID_HID_BOOT_MOUSE_INPUT;
const u16 hidinformationUUID       = CHARACTERISTIC_UUID_HID_INFORMATION;
const u16 hidCtrlPointUUID         = CHARACTERISTIC_UUID_HID_CONTROL_POINT;
const u16 hidIncludeUUID           = GATT_UUID_INCLUDE;
const u8 protocolMode              = DFLT_HID_PROTOCOL_MODE;

// Key in Report characteristic variables
u8 reportKeyIn[8];
u8 reportKeyInCCC[2];

// HID Report Reference characteristic descriptor, key input
const u8 reportRefKeyIn[2] = { 
    HID_REPORT_ID_KEYBOARD_INPUT, 
    HID_REPORT_TYPE_INPUT 
};

// Key out Report characteristic variables
u8 reportKeyOut[1];
const u8 reportRefKeyOut[2] = { 
    HID_REPORT_ID_KEYBOARD_INPUT, 
    HID_REPORT_TYPE_OUTPUT 
};

// Consumer Control input Report
u8 reportConsumerControlIn[2];
u8 reportConsumerControlInCCC[2];
const u8 reportRefConsumerControlIn[2] = {
    HID_REPORT_ID_CONSUME_CONTROL_INPUT, 
    HID_REPORT_TYPE_INPUT 
};

// Boot Keyboard Input Report
u8 bootKeyInReport;
u8 bootKeyInReportCCC[2];

// Boot Keyboard Output Report
u8 bootKeyOutReport;

// HID Information characteristic
const u8 hidInformation[4] = {
    U16_LO(0x0111), U16_HI(0x0111),   // bcdHID (USB HID version)
    0x00,                             // bCountryCode
    0x01                              // Flags
};

// HID Control Point characteristic
u8 controlPoint;

// HID Report Map characteristic
// Keyboard report descriptor (using format for Boot interface descriptor)
const u8 reportMap[78] = // use GB project report map
{
    //keyboard report in
    0x05, 0x01,         // Usage Pg (Generic Desktop)
    0x09, 0x06,         // Usage (Keyboard)
    0xA1, 0x01,         // Collection: (Application)
    0x85, HID_REPORT_ID_KEYBOARD_INPUT, // Report Id (keyboard)
    0x05, 0x07,         // Usage Pg (Key Codes)
    0x19, 0xE0,         // Usage Min (224)  VK_CTRL:0xe0
    0x29, 0xE7,         // Usage Max (231)  VK_RWIN:0xe7
    0x15, 0x00,         // Log Min (0)
    0x25, 0x01,         // Log Max (1)
    0x75, 0x01,         // Report Size (1)   1 bit * 8
    0x95, 0x08,         // Report Count (8)
    0x81, 0x02,         // Input: (Data, Variable, Absolute)
    0x95, 0x01,         // Report Count (1)
    0x75, 0x08,         // Report Size (8)
    0x81, 0x01,         // Input: (Constant)
    //keyboard output
    //5 bit led ctrl: NumLock CapsLock ScrollLock Compose kana
    0x75, 0x08,         // Report Size (8)
    0x95, 0x06,         // Report Count (6)
    0x15, 0x00,         // Log Min (0)
    0x25, 0xFF,         // Log Max (255)
    0x05, 0x07,         // Usage Pg (Key Codes)
    0x19, 0x00,         // Usage Min (0)
    0x29, 0x13,         // Usage Max (19)
    0x09, 0x00,         // Usage
    0x19, 0x15,         // Usage Min (21)
    0x29, 0xFF,         // Usage Max (255)
    0x81, 0x00,         // Input: (Data, Array)
    0xC0,               // End Collection
    //consumer report in
    0x05, 0x0C,         // Usage Page (Consumer)
    0x09, 0x01,         // Usage (Consumer Control)
    0xA1, 0x01,         // Collection (Application)
    0x85, HID_REPORT_ID_CONSUME_CONTROL_INPUT,  // Report Id
    0x95, 0x01,         // global, report count 1
    0x75, 0x10,         // global, report size 16 bits
    0x15, 0x00,         // global, min  0x01
    0x26, 0xff, 0x03,   // global, max  0x28c
    0x19, 0x00,         // local,  min  0x01
    0x2a, 0xff, 0x03,   // local,  max  0x28c
    0x81, 0x00,         // main,  input data variable, absolute
    0xc0,               // main, end collection
};

// HID External Report Reference Descriptor for report map
u16 extServiceUUID;
/*============================================================================*
 *                         HID attribute values
 *============================================================================*/
const u8 my_hidProtocolModeCharVal[5] = {
    CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP,
    U16_LO(HID_PROTOCOL_MODE_DP_H), U16_HI(HID_PROTOCOL_MODE_DP_H),
    U16_LO(CHARACTERISTIC_UUID_HID_PROTOCOL_MODE), U16_HI(CHARACTERISTIC_UUID_HID_PROTOCOL_MODE)
};
const u8 my_hidbootKeyInReporCharVal[5] = {
    CHAR_PROP_READ | CHAR_PROP_NOTIFY,
    U16_LO(HID_BOOT_KB_REPORT_INPUT_DP_H), U16_HI(HID_BOOT_KB_REPORT_INPUT_DP_H),
    U16_LO(CHARACTERISTIC_UUID_HID_BOOT_KEY_INPUT), U16_HI(CHARACTERISTIC_UUID_HID_BOOT_KEY_INPUT)
};
const u8 my_hidbootKeyOutReporCharVal[5] = {
    CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RSP,
    U16_LO(HID_BOOT_KB_REPORT_OUTPUT_DP_H), U16_HI(HID_BOOT_KB_REPORT_OUTPUT_DP_H),
    U16_LO(CHARACTERISTIC_UUID_HID_BOOT_KEY_OUTPUT), U16_HI(CHARACTERISTIC_UUID_HID_BOOT_KEY_OUTPUT)
};
const u8 my_hidReportCCinCharVal[5] = {
    CHAR_PROP_READ | CHAR_PROP_NOTIFY,
    U16_LO(HID_CONSUME_REPORT_INPUT_DP_H), U16_HI(HID_CONSUME_REPORT_INPUT_DP_H),
    U16_LO(CHARACTERISTIC_UUID_HID_REPORT), U16_HI(CHARACTERISTIC_UUID_HID_REPORT)
};
const u8 my_hidReportKEYinCharVal[5] = {
    CHAR_PROP_READ | CHAR_PROP_NOTIFY,
    U16_LO(HID_NORMAL_KB_REPORT_INPUT_DP_H), U16_HI(HID_NORMAL_KB_REPORT_INPUT_DP_H),
    U16_LO(CHARACTERISTIC_UUID_HID_REPORT), U16_HI(CHARACTERISTIC_UUID_HID_REPORT)
};
const u8 my_hidReportKEYoutCharVal[5] = {
    CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RSP,
    U16_LO(HID_NORMAL_KB_REPORT_OUTPUT_DP_H), U16_HI(HID_NORMAL_KB_REPORT_OUTPUT_DP_H),
    U16_LO(CHARACTERISTIC_UUID_HID_REPORT), U16_HI(CHARACTERISTIC_UUID_HID_REPORT)
};
const u8 my_hidReportMapCharVal[5] = {
    CHAR_PROP_READ,
    U16_LO(HID_REPORT_MAP_DP_H), U16_HI(HID_REPORT_MAP_DP_H),
    U16_LO(CHARACTERISTIC_UUID_HID_REPORT_MAP), U16_HI(CHARACTERISTIC_UUID_HID_REPORT_MAP)
};
const u8 my_hidinformationCharVal[5] = {
    CHAR_PROP_READ,
    U16_LO(HID_INFORMATION_DP_H), U16_HI(HID_INFORMATION_DP_H),
    U16_LO(CHARACTERISTIC_UUID_HID_INFORMATION), U16_HI(CHARACTERISTIC_UUID_HID_INFORMATION)
};
const u8 my_hidCtrlPointCharVal[5] = {
    CHAR_PROP_WRITE_WITHOUT_RSP,
    U16_LO(HID_CONTROL_POINT_DP_H), U16_HI(HID_CONTROL_POINT_DP_H),
    U16_LO(CHARACTERISTIC_UUID_HID_CONTROL_POINT), U16_HI(CHARACTERISTIC_UUID_HID_CONTROL_POINT)
};

/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
