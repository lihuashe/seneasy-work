/**********************************************************************************************************************
 * @file     usb_device.h
 * @version  V1.0
 * @date     2023/01/11
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
#ifndef __USB_DEVICE_H__
#define __USB_DEVICE_H__

typedef void (*usb_cdc_recive_cb_t)(uint8_t *, uint8_t );
void usb_cdc_recive_callback_register(usb_cdc_recive_cb_t cb);

extern void usb_device_init ( void );

#if USB_HID_EN
/**********************************************************************************************************************
 * @brief  device hid data recive
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
extern EN_USB_DEVICE_ERR_CODE usb_device_hid_data_recive ( uint8_t *pu8ReceiveBuf, uint32_t *pu32Len );

/**********************************************************************************************************************
 * @brief  device hid data send
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
extern EN_USB_DEVICE_ERR_CODE usb_device_hid_data_send ( uint8_t *pu8SendBuf, uint32_t u32Len );

/**********************************************************************************************************************
 * @brief  usb device hid keboard data report
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
extern EN_USB_DEVICE_ERR_CODE usb_device_hid_keyboard_data_report ( uint8_t *pu8SendBuf, uint32_t u32Len );

/**********************************************************************************************************************
 * @brief  usb device hid consumer data report
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
extern EN_USB_DEVICE_ERR_CODE usb_device_hid_consumer_data_report ( uint8_t *pu8SendBuf, uint32_t u32Len );
#endif

/**********************************************************************************************************************
 * @brief  usb device send audio mic data
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
extern EN_USB_DEVICE_ERR_CODE usb_device_send_mic_data(uint8_t *pu8Buf, uint32_t u32Len);

/**
 * @brief  USB device cdc recive data.
 * @param  pu8Buf: Pointer to save received data.
 * @param  pu32Len: Pointer to received data length.
 * @return Error code, @ref USB_DEVICE_ERR_CODE.
 */
extern EN_USB_DEVICE_ERR_CODE usb_device_recive_cdc_data(uint8_t *pu8Buf, uint32_t *pu32Len);

/**
 * @brief  USB device cdc send data.
 * @param  pu8Buf: Pointer to send buffer.
 * @param  u32Len: Send data buffer length.
 * @return Error code, @ref USB_DEVICE_ERR_CODE.
 */
extern EN_USB_DEVICE_ERR_CODE usb_device_send_cdc_data(uint8_t *pu8Buf, uint32_t u32Len);

#endif /*__USB_DEVICE_H__*/