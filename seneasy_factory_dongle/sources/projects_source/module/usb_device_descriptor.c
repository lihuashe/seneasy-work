/**********************************************************************************************************************
 * @file     usb_hid_device_descriptor.c
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2021/03/17
 * @history
 * @note
 **********************************************************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2021, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 *********************************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "err_def.h"
#include "RegMXD2670.h"
#include "utility.h"

#include "hw_usb_phy.h"
#include "hw_usb_ctrl.h"
#include "hw_usb_timeout.h"
#include "hw_usb_dcd.h"
#include "hw_usb_hcd.h"
#include "hal_usb.h"

#include "usb_device_descriptor.h"

const uint8_t gDeviceProductString[] = "Maxscend S40 Dongle";        //max length: 32bytes

/*USB HID device descriptor*/
uint8_t gUsbDeviceDescriptor[] =
{
    0x12,         /* bLength */
    0x01,         /* bDescriptorType */
    0x10, 0x01,   /* bcdUSB */
    0x00,         /* bDeviceClass */
    0x00,         /* bDeviceSubClass */
    0x00,         /* bDeviceProtocol */
    0x40,         /* bMaxPacketSize0 */
    USB_VID_LSB, USB_VID_MSB,   /* idVendor */
    USB_PID_LSB, USB_PID_MSB,   /* idProduct */
    0x00, 0x01,   /* bcdDevice */
    0x01,         /* iManufacturer */
    0x02,         /* iProduct */
    0x03,         /* iSerialNumber */
    0x01,         /* bNumConfigurations */
};

/*USB device configuration descriptor*/
const uint8_t gUsbConfgDesAudioCDC[] =
{
    /*Configuration Descriptor*/
    0x09,                                 /* bLength: Configuration Descriptor size */
    USB_DESCRIPTOR_TYPE_CONFIGURATION,    /* bDescriptorType: Configuration */
    9+ 8 + 9 + 5 + 5 + 4 + 5 + 9 + 7 + 7+9+9+12+9+7+9+9+9+7+11+9+7,  /* wTotalLength:no of returned bytes */
    0x00,
    USB_DEVICE_MAX_INTERFACE,             /* bNumInterfaces:3 interface */
    0x01,                                 /* bConfigurationValue: Configuration value */
    0x00,                                 /* iConfiguration: Index of string descriptor describing the configuration */
    0x80,                                 /* bmAttributes: usb Vbus powered */
    0xFA,                                 /* MaxPower: x*2mA: 500mA */

    /* Interface Association Descriptor(IAD) for CDC */
    0x08,                   /* bLength */
    0x0B,                   /* bDescriptorType: INTERFACE_ASSOCIATION (0x0B) */
    CDC_CTRL_INTERFACE_NUM, /* bFirstInterface */
    0x02,                   /* bInterfaceCount */
    0x02,                   /* bFunctionClass: Communication Interface Class */
    0x02,                   /* bFunctionSubClass: Abstract Control Model */
    0x00,                   /* bFunctionProtocol: No class specific protocol required */
    0x00,                   /* iFunction */

    /* 0.CDC Ctrl Interface Descriptor */
    0x09,                          /* bLength: Interface Descriptor size */
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType: Interface */
    CDC_CTRL_INTERFACE_NUM,        /* bInterfaceNumber: Number(index) of CDC Ctrl Interface */
    0x00,                          /* bAlternateSetting: Alternate setting */
    0x00,                          /* bNumEndpoints: One endpoints used */
    0x02,                          /* bInterfaceClass: Communications and CDC Control (0x02) */
    0x02,                          /* bInterfaceSubClass: Abstract Control Model */
    0x01,                          /* bInterfaceProtocol: Common AT commands */
    0x00,                          /* iInterface: */

    /* Header Functional Descriptor */
    0x05, /* bLength: Endpoint Descriptor size */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x00, /* bDescriptorSubtype: Header Func Desc */
    0x10, /* bcdCDC: spec release number */
    0x01,

    /* Call Management Functional Descriptor */
    0x05, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x01, /* bDescriptorSubtype: Call Management Func Desc */
    0x00, /* bmCapabilities: D0+D1 */
    0x01, /* bDataInterface: 1 */

    /* ACM Functional Descriptor */
    0x04, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x02, /* bDescriptorSubtype: Abstract Control Management desc */
    0x02, /* bmCapabilities */

    /* Union Functional Descriptor */
    0x05, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x06, /* bDescriptorSubtype: Union func desc */
    0x00, /* bMasterInterface: Communication class interface */
    0x01, /* bSlaveInterface0: Data Class Interface */

    /* 1.CDC Data class interface descriptor */
    0x09,                          /* bLength: Endpoint Descriptor size */
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType: */
    CDC_DATA_INTERFACE_NUM,        /* bInterfaceNumber: Number(index) of CDC Data interface */
    0x00,                          /* bAlternateSetting: Alternate setting */
    0x02,                          /* bNumEndpoints: Two endpoints used */
    0x0A,                          /* bInterfaceClass: CDC Data (0x0a) */
    0x00,                          /* bInterfaceSubClass: Unknown (0x00) */
    0x00,                          /* bInterfaceProtocol: No class specific protocol required (0x00) */
    0x00,                          /* iInterface: */

    /* Endpoint OUT Descriptor */
    0x07,                         /* bLength: Endpoint Descriptor size */
    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
    EP_ADDR_CDC_BULK_OUT,         /* bEndpointAddress */
    EP_ATTR_CDC_OUT,              /* bmAttributes: Bulk */
    CDC_USB_DATA_PACKET_SIZE,     /* wMaxPacketSize: */
    0, 0x00,                      /* bInterval: ignore for Bulk transfer */

    /* Endpoint IN Descriptor */
    0x07,                         /* bLength: Endpoint Descriptor size */
    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
    EP_ADDR_CDC_BULK_IN,          /* bEndpointAddress */
    EP_ATTR_CDC_IN,               /* bmAttributes: Bulk */
    CDC_USB_DATA_PACKET_SIZE,     /* wMaxPacketSize: */
    0, 0x00,                      /* bInterval: ignore for Bulk transfer */

    /* 0. Audio Control Interface */
    0x09, 0x04, AUDIO_ATL_INTERFACE_NUM, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
    0x09, 0x24, 0x01, 0x00, 0x01, 0x2E, 0x00, 0x01, 0x01,
    0x0C, 0x24, 0x02, 0x02, 0x01, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x09, 0x24, 0x03, 0x12, 0x01, 0x01, 0x02, 0x21, 0x00,
    0x07, 0x24, 0x05, 0x21, 0x01, 0x05, 0x00,
    0x09, 0x24, 0x06, 0x05, 0x02, 0x01, 0x03, 0x00, 0x00,

    // 1. Audio Stream Interface, Microphone
    0x09, 0x04, AUDIO_SRM_IN_INTERFACE_NUM, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00,
    0x09, 0x04, AUDIO_SRM_IN_INTERFACE_NUM, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00,
    0x07, 0x24, 0x01, 0x12, 0x01, 0x01, 0x00,

    0x0B,
    0x24,
    0x02,
    0x01,
    AUDIO_MIC_CH_NUM,
    0x02,
    0x10,
    0x01,
    BREAK_UINT32(AUDIO_SAMPLING_RATE, 0),
    BREAK_UINT32(AUDIO_SAMPLING_RATE, 1),
    BREAK_UINT32(AUDIO_SAMPLING_RATE, 2),

    0x09, 0x05, EP_ADDR_AUDIO_ISO_IN, EP_ATTR_AUDIO_IN, AUDIO_USB_PACKET_SIZE, 0x00, 0x01, 0x00, 0x00,
    0x07, 0x25, 0x01, 0x01, 0x00, 0x00, 0x00,
};

/*USB HID report descriptor*/
uint8_t gHidDataReportDescriptor[146] =
{
    0x05, 0x01,     /* Usage Page (Generic Desktop) */
    0x09, 0x02,     /* Usage (Mouse) */
    0xA1, 0x01,     /* Collection (Application) */
    0x09, 0x01,     /* Usage (Pointer) */
    0xA1, 0x00,     /* Collection (Physical) */
    0x85, HID_MOUSE_REPORT_ID,     /* Report Id (1) */
    0x05, 0x09,     /* Usage Page (Buttons) */
    0x19, 0x01,     /* Usage Minimum (01) - Button 1 */
    0x29, 0x03,     /* Usage Maximum (03) - Button 3 */
    0x15, 0x00,     /* Logical Minimum (0) */
    0x25, 0x01,     /* Logical Maximum (1) */
    0x75, 0x01,     /* Report Size (1) */
    0x95, 0x03,     /* Report Count (3) */
    0x81, 0x02,     /* Input (Data, Variable, Absolute) - Button states */
    0x75, 0x05,     /* Report Size (5) */
    0x95, 0x01,     /* Report Count (1) */
    0x81, 0x01,     /* Input (Constant) - Padding or Reserved bits */
    0x05, 0x01,     /* Usage Page (Generic Desktop) */
    0x09, 0x30,     /* Usage (X) */
    0x09, 0x31,     /* Usage (Y) */
    0x09, 0x38,     /* Usage (Wheel) */
    0x15, 0x81,     /* Logical Minimum (-127) */
    0x25, 0x7F,     /* Logical Maximum (127) */
    0x75, 0x08,     /* Report Size (8) */
    0x95, 0x03,     /* Report Count (3) */
    0x81, 0x06,     /* Input (Data, Variable, Relative) - X & Y coordinate */
    0xC0,           /* End Collection */
    0xC0,           /* End Collection */
    
    0x05, 0x01,     /* USAGE_PAGE(Generic Desktop) */
    0x09, 0x06,     /* USAGE(Keyboard) */
    0xa1, 0x01,     /* COLLECTION(Application) */
    0x85, HID_KEYBOARD_REPORT_ID,     /* Report Id (2) */
    0x05, 0x07,     /* USAGE_PAGE(Keyboard/Keypad) */
    0x19, 0xe0,     /* USAGE_MINIMUM(Keyboard LeftControl) */
    0x29, 0xe7,     /* USAGE_MAXIMUM(Keyboard Right GUI) */
    0x15, 0x00,     /* LOGICAL_MINIMUM(0) */
    0x25, 0x01,     /* LOGICAL_MAXIMUM(1) */
    0x95, 0x08,     /* REPORT_COUNT(8) */
    0x75, 0x01,     /* REPORT_SIZE(1) */
    0x81, 0x02,     /* INPUT(Data,Var,Abs) */
    0x95, 0x01,     /* REPORT_COUNT(1) */
    0x75, 0x08,     /* REPORT_SIZE(1) */
    0x81, 0x03,     /* INPUT(Cnst,Var,Abs) */
    0x95, 0x06,     /* REPORT_COUNT(6) */
    0x75, 0x08,     /* REPORT_SIZE(8) */
    0x15, 0x00,     /* LOGICAL_MINIMUM(0) */
    0x25, 0xff,     /* LOGICAL_MAXIMUM(255) */
    0x05, 0x07,     /* USAGE_PAGE(Keyboard/Keypad) */
    0x19, 0x00,     /* USAGE_MINIMUM(Reserved) */
    0x29, 0x65,     /* USAGE_MAXIMUM(Keyboard Application) */
    0x81, 0x00,     /* INPUT(Data,Var,Abs) */
    0x25, 0x01,     /* USAGE_MAXIMUM(1) */
    0x95, 0x05,     /* REPORT_COUNT(5) */
    0x75, 0x01,     /* REPORT_SIZE(1) */
    0x05, 0x08,     /* USAGE_PAGE(LEDs) */
    0x19, 0x01,     /* USAGE_MINIMUM(Num Lock) */
    0x29, 0x05,     /* USAGE_MAXIMUM(Kana) */
    0x91, 0x02,     /* OUTPUT(Data,Var,Abs) */
    0x95, 0x01,     /* REPORT_COUNT(1) */
    0x75, 0x03,     /* REPORT_SIZE(3) */
    0x91, 0x03,     /* OUTPUT(Cnst,Var,Abs) */
    0xc0,           /* END_COLLECTION */

    0x05, 0x0C,     /* Usage Page(Consumer) */
    0x09, 0x01,     /* Usage(Consumer Control) */
    0xA1, 0x01,     /* Collection(Application) */
    0x85, HID_CONSUMER_REPORT_ID,     /* Report ID (3)   */
    0x19, 0x00,     /* Usage Min (0x00) */
    0x2a, 0x9c, 0x02, /* Usage Max (0x029c) */
    0x15, 0x00,     /* Logical Min (0x00) */
    0x26, 0x9c, 0x02, /* Logical Max (0x029c) */
    0x75, 0x10,     /* Report Size (0x10) */
    0x95, 0x08,     /* Report Count (0x08) */
    0x81, 0x00,     /* Input (Data,Ary,Abs) */
    0xC0,           /* End Collection(Application) */
};
