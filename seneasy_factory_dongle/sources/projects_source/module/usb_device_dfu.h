/**********************************************************************************************************************
 * @file     usb_device_dfu.h
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

#ifndef __USB_DEVICE_DFU_H__
#define __USB_DEVICE_DFU_H__

#ifdef __cplusplus
 extern "C" {
#endif

//=====================================================================================================================
// DEFINE
//=====================================================================================================================
#define CDC_CMD_GET_LINE_CODING            0xA121
#define CDC_CMD_SET_LINE_CODING            0x2120
#define CDC_CMD_SET_CONTROL_LINE_STATE     0x2122




/**
 * @brief  USB device endpiont reset.
 */
extern void usb_device_reset_endpoint(void);


/**
 * @brief  USB device cdc class request.
 * @param  pu8Setup: Pointer to setup buffer.
 */
extern void usb_device_cdc_class_request(uint8_t *pu8Setup);


/**
 * @brief  Get USB device cdc status.
 * @return USB CDC is open or not.
 */
extern uint8_t usb_device_get_cdc_status(void);


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




#ifdef __cplusplus
}
#endif

#endif /*__USB_DEVICE_DFU_H__*/


