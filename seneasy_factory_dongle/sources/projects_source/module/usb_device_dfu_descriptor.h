/**********************************************************************************************************************
 * @file     usb_device_dfu_descriptor.h
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

#ifndef __USB_DEVICE_DFU_DESCRIPTOR_H__
#define __USB_DEVICE_DFU_DESCRIPTOR_H__

#ifdef __cplusplus
 extern "C" {
#endif


//=====================================================================================================================
// DEFINE
//=====================================================================================================================
// Endpoint transfer packet size
#define CDC_USB_DATA_PACKET_SIZE           64
#define CDC_USB_CTRL_PACKET_SIZE           16

// CDC interface serial number define
#define CDC_CTRL_INTERFACE_NUM             0
#define CDC_DATA_INTERFACE_NUM             1

/* USB String Descriptor - Manufacturer */
#define USB_STRING_DESCRIPTOR_MANUFACTURER     "Maxscend"

/* USB String Descriptor - Product */
#define USB_STRING_DESCRIPTOR_PRODUCT          "Maxscend CDC"

/* USB String Descriptor - Serial Number */
#define USB_STRING_DESCRIPTOR_SERIAL_NUMBER    "20221221"


//=====================================================================================================================
// GLOBAL VARIABLE
//=====================================================================================================================
/* USB Device Descriptor */
extern uint8_t g_au8UsbDeviceDescriptor[];


/* USB Configuration Descriptor */
extern uint8_t g_au8UsbConfigurationDescriptor[];


/* USB String Descriptor - LangID */
extern uint8_t g_au8UsbStringDescriptorLangID[];


/* USB Device Qualifier Descriptor */
extern uint8_t g_au8DeviceQualifierDescriptor[10];


#ifdef __cplusplus
}
#endif

#endif /*__USB_DEVICE_DFU_DESCRIPTOR_H__*/


