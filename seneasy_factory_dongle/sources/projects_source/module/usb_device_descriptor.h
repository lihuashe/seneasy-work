/**********************************************************************************************************************
 * @file     usb_device_descriptor.h
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
 #ifndef __USB_DEVICE_DESCRIPTOR_H__
#define __USB_DEVICE_DESCRIPTOR_H__
 
#ifdef __cplusplus
 extern "C" {
#endif

//=====================================================================================================================
// USB ID DEFINE
//=====================================================================================================================
// VID and PID
#define MAXSCEND_VID                   0xBA06
#define HID_KEYBOARD_PID               0x0101
#define HID_MOUSE_PID                  0x0201
#define HID_KEYBORD_MOUSE_PID          0x0301
#define CDC_PID                        0x0102
#define AUDIO_MIC_PID                  0x0103
#define AUDIO_SPEEAKER_PID             0x0203
#define AUDIO_MIC_SPEEAKER_PID         0x0303
#define HID_AUDIO_CDC_PID              0x0403
#define DONGLE_HID_AUDIO_PID           0x0502

#define USB_DEVICE_VID                 (MAXSCEND_VID)
#define USB_DEVICE_PID                 (DONGLE_HID_AUDIO_PID)
#define USB_VID_MSB                    (HI_UINT16(USB_DEVICE_VID))
#define USB_VID_LSB                    (LO_UINT16(USB_DEVICE_VID))
#define USB_PID_MSB                    (HI_UINT16(USB_DEVICE_PID))
#define USB_PID_LSB                    (LO_UINT16(USB_DEVICE_PID))

//=====================================================================================================================
// USB DEVICE DEFINE
//=====================================================================================================================
#define USB_DT_DEVICE                    1
#define USB_DT_CONFIG                    2
#define USB_DT_STRING                    3
#define USB_DT_INTERFACE                 4
#define USB_DT_ENDPOINT                  5
#define USB_DT_DEVICE_QUALIFIER          6
#define USB_DT_HID                       0x21
#define USB_DT_HID_REPORT                0x22

#define USB_REQ_GET_STATUS               0
#define USB_REQ_CLEAR_FEATURE            1
#define USB_REQ_SET_FEATURE              3
#define USB_REQ_SET_ADDRESS              5
#define USB_REQ_GET_DESCRIPTOR           6
#define USB_REQ_SET_DESCRIPTOR           7
#define USB_REQ_GET_CONFIGURATION        8
#define USB_REQ_SET_CONFIGURATION        9
#define USB_REQ_GET_INTERFACE            10
#define USB_REQ_SET_INTERFACE            11
#define USB_REQ_SYNCH_FRAME              12

// Configuration Descriptor Type
#define USB_DESCRIPTOR_TYPE_DEVICE                    0x01U
#define USB_DESCRIPTOR_TYPE_CONFIGURATION             0x02U
#define USB_DESCRIPTOR_TYPE_STRING                    0x03U
#define USB_DESCRIPTOR_TYPE_INTERFACE                 0x04U
#define USB_DESCRIPTOR_TYPE_ENDPOINT                  0x05U
#define USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER          0x06U
#define USB_DESCRIPTOR_TYPE_OTHER_SPEED_CONFIGURATION 0x07U
#define USB_DESCRIPTOR_TYPE_BOS                       0x0FU

//=====================================================================================================================
// USB INTERFACE DEFINE
//=====================================================================================================================
#define USB_DEVICE_MAX_INTERFACE           4
// CDC interface serial number define
#define CDC_CTRL_INTERFACE_NUM             0
#define CDC_DATA_INTERFACE_NUM             1
// Interface Num define
#define AUDIO_ATL_INTERFACE_NUM            2
#define AUDIO_SRM_IN_INTERFACE_NUM         3

//=====================================================================================================================
// HID DEFINE
//=====================================================================================================================
// Endpoint transfer packet size
#define HID_USB_PACKET_SIZE                   64

// HID report ID define
#define HID_MOUSE_REPORT_ID                   (0x01)
#define HID_KEYBOARD_REPORT_ID                (0x02)
#define HID_CONSUMER_REPORT_ID                (0x03)

//=====================================================================================================================
// CDC DEFINE
//=====================================================================================================================
// Endpoint transfer packet size
#define CDC_USB_DATA_PACKET_SIZE           64
#define CDC_USB_CTRL_PACKET_SIZE           16



/* USB String Descriptor - Manufacturer */
#define USB_STRING_DESCRIPTOR_MANUFACTURER     "Maxscend"

/* USB String Descriptor - Product */
#define USB_STRING_DESCRIPTOR_PRODUCT          "Maxscend CDC"

/* USB String Descriptor - Serial Number */
#define USB_STRING_DESCRIPTOR_SERIAL_NUMBER    "20221221"

#define CDC_CMD_GET_LINE_CODING            0xA121
#define CDC_CMD_SET_LINE_CODING            0x2120
#define CDC_CMD_SET_CONTROL_LINE_STATE     0x2122

//=====================================================================================================================
// AUDIO DEFINE
//=====================================================================================================================
// Endpoint transfer packet size
#define AUDIO_USB_PACKET_SIZE                64

// Audio device define
#define AUDIO_SAMPLING_RATE                  16000
#define AUDIO_MIC_CH_NUM                     1
#define AUDIO_SPEAKER_CH_NUM                 1

// AUDIO Control module number
#define AUDIO_SPEAKER_IT_ID            1
#define AUDIO_SPEAKER_FU_ID            2 // MUTE/VOLUME
#define AUDIO_SPEAKER_OT_ID            3
#define AUDIO_MIC_IT_ID                4
#define AUDIO_MIC_FU_ID                5
#define AUDIO_MIC_SL_ID                6
#define AUDIO_MIC_OT_ID                7

// Audio Command define
#define USB_AUDIO_SET_CUR              0x2101
#define USB_AUDIO_SET_IDLE             0x210A
#define USB_AUDIO_GET_CUR              0xA181
#define USB_AUDIO_GET_MIN              0xA182
#define USB_AUDIO_GET_MAX              0xA183
#define USB_AUDIO_GET_RES              0xA184
#define USB_AUDIO_SET_CUR_EP           0x2201
#define USB_AUDIO_GET_CUR_EP           0xA281

#define USB_AUDIO_MIN_VOLUME           ( 0x00000 )
#define USB_AUDIO_MAX_VOLUME           ( 0x3FFFF )
#define USB_AUDIO_RES_VOLUME           ( 0x00001 )



//=====================================================================================================================
// ENDPOINT DEFINE
//=====================================================================================================================
//Endpoint Descriptor Address(USB2.0 SPEC Table 9-13. Standard Endpoint Descriptor)
//Endpoint Address(Support 6 endpoint)
#define MASK_EP_ADDR_CTRL                    0
#define MASK_EP_ADDR_1                       1
#define MASK_EP_ADDR_2                       2
#define MASK_EP_ADDR_3                       3
#define MASK_EP_ADDR_4                       4
#define MASK_EP_ADDR_5                       5
#define MASK_EP_ADDR_6                       6
//Endpoint Direction
#define MASK_EP_DIR_OUT                      0x00
#define MASK_EP_DIR_IN                       0x80

//Endpoint Descriptor Attributes(USB2.0 SPEC Table 9-13. Standard Endpoint Descriptor)
//Transfer Type
#define  MASK_TRANS_TYPE_CTRL                0x00
#define  MASK_TRANS_TYPE_ISO                 0x01
#define  MASK_TRANS_TYPE_BULK                0x02
#define  MASK_TRANS_TYPE_INT                 0x03
//Synchronization Type
#define  MASK_SYNC_TYPE_NO_SYNCHRONIZATION   0x00
#define  MASK_SYNC_TYPE_ASYNCHRONIZATION     0x04
#define  MASK_SYNC_TYPE_ADAPTIVE             0x08
#define  MASK_SYNC_TYPE_SYNCHRONOUS          0x0C
//Usage Type
#define  MASK_USAGE_TYPE_DATA_EP             0x00
#define  MASK_USAGE_TYPE_FB_EP               0x10
#define  MASK_USAGE_TYPE_FB_DATA_EP          0x20

//USER Endpoint Attributes
#define  EP_ATTR_CTRL                       (MASK_TRANS_TYPE_CTRL)
#define  EP_ATTR_AUDIO_IN                   (MASK_TRANS_TYPE_ISO|MASK_SYNC_TYPE_ADAPTIVE|MASK_USAGE_TYPE_DATA_EP)
#define  EP_ATTR_AUDIO_OUT                  (MASK_TRANS_TYPE_ISO|MASK_SYNC_TYPE_ADAPTIVE|MASK_USAGE_TYPE_DATA_EP)
#define  EP_ATTR_HID_IN                     (MASK_TRANS_TYPE_INT|MASK_SYNC_TYPE_NO_SYNCHRONIZATION|MASK_USAGE_TYPE_DATA_EP)
#define  EP_ATTR_HID_OUT                    (MASK_TRANS_TYPE_INT|MASK_SYNC_TYPE_NO_SYNCHRONIZATION|MASK_USAGE_TYPE_DATA_EP)
#define  EP_ATTR_CDC_IN                     (MASK_TRANS_TYPE_BULK|MASK_SYNC_TYPE_NO_SYNCHRONIZATION|MASK_USAGE_TYPE_DATA_EP)
#define  EP_ATTR_CDC_OUT                    (MASK_TRANS_TYPE_BULK|MASK_SYNC_TYPE_NO_SYNCHRONIZATION|MASK_USAGE_TYPE_DATA_EP)

//USER Endpoint Address
#define EP_ADDR_CONTROL                     (MASK_EP_ADDR_CTRL)
#define EP_ADDR_HID_INT_IN                  (MASK_EP_DIR_IN|MASK_EP_ADDR_4)
#define EP_ADDR_HID_INT_OUT                 (MASK_EP_DIR_OUT|MASK_EP_ADDR_3)
#define EP_ADDR_CDC_BULK_IN                 (MASK_EP_DIR_IN|MASK_EP_ADDR_1)
#define EP_ADDR_CDC_BULK_OUT                (MASK_EP_DIR_OUT|MASK_EP_ADDR_2)
#define EP_ADDR_CDC_INT_IN                  (MASK_EP_DIR_IN|MASK_EP_ADDR_6)
#define EP_ADDR_AUDIO_ISO_IN                (MASK_EP_DIR_IN|MASK_EP_ADDR_5)
#define EP_ADDR_AUDIO_ISO_OUT               (MASK_EP_DIR_OUT|MASK_EP_ADDR_6)


extern uint8_t gHidDataReportDescriptor[146];
extern uint8_t gUsbDeviceDescriptor[];

extern const uint8_t gUsbConfgDesAudioCDC[];
extern const uint8_t gDeviceProductString[];



#ifdef __cplusplus
}
#endif

#endif /*__USB_DEVICE_DESCRIPTOR_H__*/
