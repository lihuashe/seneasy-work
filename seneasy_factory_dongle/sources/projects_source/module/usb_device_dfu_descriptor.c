/**********************************************************************************************************************
 * @file     usbd_cdc_descriptor.c
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2022/12/20
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

#include <stdint.h>

#include "utility.h"

#include "usb_device_descriptor.h"
#include "usb_device_standard_request.h"
#include "usb_device_dfu_descriptor.h"




//=====================================================================================================================
// GLOBAL VARIABLE
//=====================================================================================================================
/* USB Device Descriptor */
uint8_t g_au8UsbDeviceDescriptor[] =
{
    0x12,                                  /* bLength */
    0x01,                                  /* bDescriptorType */
    0x10, 0x01,                            /* bcdUSB */
    0x00,                                  /* bDeviceClass */
    0x00,                                  /* bDeviceSubClass */
    0x00,                                  /* bDeviceProtocol */
    0x40,                                  /* bMaxPacketSize0 */
    LO_UINT16(MAXSCEND_VID),
    HI_UINT16(MAXSCEND_VID),               /* idVendor */
    LO_UINT16(CDC_PID),
    HI_UINT16(CDC_PID),                    /* idProduct */
    0x00, 0x01,                            /* bcdDevice */
    0x01,                                  /* iManufacturer */
    0x02,                                  /* iProduct */
    0x03,                                  /* iSerialNumber */
    0x01,                                  /* bNumConfigurations */
};

/* USB Configuration Descriptor */
uint8_t g_au8UsbConfigurationDescriptor[] =
{
    /* Configuration Descriptor */
    0x09,                                  /* bLength: Configuration Descriptor size */
    USB_DESCRIPTOR_TYPE_CONFIGURATION,     /* bDescriptorType: Configuration */
    9 + 8 + 9 + 5 + 5 + 4 + 5 + 9 + 7 + 7, /* wTotalLength:no of returned bytes */
    0x00,
    0x02,                                  /* bNumInterfaces: Number of interfaces supported */
    0x01,                                  /* bConfigurationValue: Configuration value */
    0x00,                                  /* iConfiguration: Index of string descriptor describing the configuration */
    0x80,                                  /* bmAttributes: usb Vbus powered */
    0xFA,                                  /* MaxPower: x * 2mA */

    /* Interface Association Descriptor(IAD) for CDC */
    0x08,                                  /* bLength */
    0x0B,                                  /* bDescriptorType: INTERFACE_ASSOCIATION (0x0B) */
    CDC_CTRL_INTERFACE_NUM,                /* bFirstInterface */
    0x02,                                  /* bInterfaceCount */
    0x02,                                  /* bFunctionClass: Communication Interface Class */
    0x02,                                  /* bFunctionSubClass: Abstract Control Model */
    0x00,                                  /* bFunctionProtocol: No class specific protocol required */
    0x00,                                  /* iFunction */

    /* 0.CDC Ctrl Interface Descriptor */
    0x09,                                  /* bLength: Interface Descriptor size */
    USB_DESCRIPTOR_TYPE_INTERFACE,         /* bDescriptorType: Interface */
    CDC_CTRL_INTERFACE_NUM,                /* bInterfaceNumber: Number(index) of CDC Ctrl Interface */
    0x00,                                  /* bAlternateSetting: Alternate setting */
    0x00,                                  /* bNumEndpoints: One endpoints used */
    0x02,                                  /* bInterfaceClass: Communications and CDC Control (0x02) */
    0x02,                                  /* bInterfaceSubClass: Abstract Control Model */
    0x01,                                  /* bInterfaceProtocol: Common AT commands */
    0x00,                                  /* iInterface: */

    /* Header Functional Descriptor */
    0x05,                                  /* bLength: Endpoint Descriptor size */
    0x24,                                  /* bDescriptorType: CS_INTERFACE */
    0x00,                                  /* bDescriptorSubtype: Header Func Desc */
    0x10,                                  /* bcdCDC: spec release number */
    0x01,

    /* Call Management Functional Descriptor */
    0x05,                                  /* bFunctionLength */
    0x24,                                  /* bDescriptorType: CS_INTERFACE */
    0x01,                                  /* bDescriptorSubtype: Call Management Func Desc */
    0x00,                                  /* bmCapabilities: D0+D1 */
    0x01,                                  /* bDataInterface: 1 */

    /* ACM Functional Descriptor */
    0x04,                                  /* bFunctionLength */
    0x24,                                  /* bDescriptorType: CS_INTERFACE */
    0x02,                                  /* bDescriptorSubtype: Abstract Control Management desc */
    0x02,                                  /* bmCapabilities */

    /* Union Functional Descriptor */
    0x05,                                  /* bFunctionLength */
    0x24,                                  /* bDescriptorType: CS_INTERFACE */
    0x06,                                  /* bDescriptorSubtype: Union func desc */
    0x00,                                  /* bMasterInterface: Communication class interface */
    0x01,                                  /* bSlaveInterface0: Data Class Interface */

    /* 1.CDC Data class interface descriptor */
    0x09,                                  /* bLength: Endpoint Descriptor size */
    USB_DESCRIPTOR_TYPE_INTERFACE,         /* bDescriptorType: */
    CDC_DATA_INTERFACE_NUM,                /* bInterfaceNumber: Number(index) of CDC Data interface */
    0x00,                                  /* bAlternateSetting: Alternate setting */
    0x02,                                  /* bNumEndpoints: Two endpoints used */
    0x0A,                                  /* bInterfaceClass: CDC Data (0x0a) */
    0x00,                                  /* bInterfaceSubClass: Unknown (0x00) */
    0x00,                                  /* bInterfaceProtocol: No class specific protocol required (0x00) */
    0x00,                                  /* iInterface: */

    /* Endpoint OUT Descriptor */
    0x07,                                  /* bLength: Endpoint Descriptor size */
    USB_DESCRIPTOR_TYPE_ENDPOINT,          /* bDescriptorType: Endpoint */
    EP_ADDR_CDC_BULK_OUT,                  /* bEndpointAddress */
    EP_ATTR_CDC_OUT,                       /* bmAttributes: Bulk */
    CDC_USB_DATA_PACKET_SIZE,              /* wMaxPacketSize: */
    0,
    0x00,                                  /* bInterval: ignore for Bulk transfer */

    /* Endpoint IN Descriptor */
    0x07,                                  /* bLength: Endpoint Descriptor size */
    USB_DESCRIPTOR_TYPE_ENDPOINT,          /* bDescriptorType: Endpoint */
    EP_ADDR_CDC_BULK_IN,                   /* bEndpointAddress */
    EP_ATTR_CDC_IN,                        /* bmAttributes: Bulk */
    CDC_USB_DATA_PACKET_SIZE,              /* wMaxPacketSize: */
    0,
    0x00,                                  /* bInterval: ignore for Bulk transfer */
};

/* USB String Descriptor - LangID */
uint8_t g_au8UsbStringDescriptorLangID[] = {0x04, 0x03, 0x09, 0x04};

/* USB Device Qualifier Descriptor */
uint8_t g_au8DeviceQualifierDescriptor[10] =
{
    10,
    6,
    0x10,
    0x01,
    0,
    0,
    0,
    64,
    1,
    0
};


