/**********************************************************************************************************************
 * @file     usb_device_standard_request.h
 * @version  V1.0
 * @date     2021/02/25
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

#ifndef __USB_DEVICE_STANDARD_REQUEST_H__
#define    __USB_DEVICE_STANDARD_REQUEST_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

//user device type enum
typedef enum
{
    USB_DEVICE_UNKONWN = 0x00,
    USB_DEVICE_CDC = 0x02,
    USB_DEVICE_DONGLE_CDC_AUDIO = 0x06,
}USB_DEVICE_TYPE_T;


struct std_device_desp_t
{
    uint8_t u8Lenght;
    uint8_t u8DesriptorTpye;
    uint16_t u16BcdUsb;
    uint8_t u8DevClass;
    uint8_t u8DevSubClass;
    uint8_t u8DevPortocol;
    uint8_t u8MaxPackSize0;
    uint16_t u16IdVendor;
    uint16_t u16IdProduct;
    uint16_t u16BcdDevice;
    uint8_t u8Manufact;
    uint8_t u8IProduct;
    uint8_t u8SerialNum;
    uint8_t u8NumConfig;
};


struct usb_std_fifo_t
{
    uint16_t u16Length;
    uint8_t *pu8Buff;
};


struct usb_std_desp_t
{
    uint8_t u8Mode;
    uint8_t *pu8DevDesp;
    struct usb_std_fifo_t pstConfigDesp;
    struct usb_std_fifo_t pstInterfaceDesp;
    struct usb_std_fifo_t pstEndpointDesp;
    struct usb_std_fifo_t pstDevStringLangeID;
    struct usb_std_fifo_t pstDevStringManu;
    struct usb_std_fifo_t pstDevProductString;
    struct usb_std_fifo_t pstgDevStringSerialNum;
    struct usb_std_fifo_t pstDeviceQualifier;
    struct usb_std_fifo_t pstHIDReportDesp;
    struct usb_std_fifo_t pstDevStringIndex;
};


/**********************************************************************************************************************
 * @brief  USB model choose.
 * @param  u8Mode: usb word model
 * @param  u16UsbVid: 
 * @param  u16UsbPid: 
 * @output 
 * @return None.
 *********************************************************************************************************************/
extern void usb_device_model_select(uint8_t u8Mode,uint16_t u16UsbVid,uint16_t u16UsbPid);
    

/**********************************************************************************************************************
 * @brief  Sends the reply data to control the transmission command.
 * @param  u16Resp: reply data
 * @param  u8Len: reply len,1 or 2
 * @output 
 * @return None.
 *********************************************************************************************************************/
extern void usb_device_send_resp(uint16_t u16Resp, uint8_t u8Len);

/**********************************************************************************************************************
 * @brief  Process the get descriptor command.
 * @param  None.
 * @output 
 * @return None.
 *********************************************************************************************************************/
extern void usb_device_descriptor_get(void);


/**********************************************************************************************************************
 * @brief  USB device standard request.
 * @param  None.
 * @output 
 * @return None.
 *********************************************************************************************************************/
extern void usb_device_standard_request(void);
    
/**********************************************************************************************************************
 * @brief  USB device class request.
 * @param  None.
 * @output 
 * @return None.
 *********************************************************************************************************************/
extern void usb_device_class_request(uint8_t *pu8Setup);
    
/**********************************************************************************************************************
 * @brief  USB device manufacturer request.
 * @param  None.
 * @output 
 * @return None.
 *********************************************************************************************************************/
extern void usb_device_manufactur_request(void);
    
/**********************************************************************************************************************
 * @brief  USB device unkown request.
 * @param  None.
 * @output 
 * @return None.
 *********************************************************************************************************************/
extern void usb_device_other_request(void);
    

/**********************************************************************************************************************
 * @brief  start uo grate.
 * @param  None.
 * @output 
 * @return None.
 *********************************************************************************************************************/
extern void usb_start_up_grate(uint32_t u32UpdateResource);
    

/**********************************************************************************************************************
 * @brief  Handle control commands from PC
 * @param  None.
 * @output 
 * @return None.
 *********************************************************************************************************************/
extern void usb_device_request_process(void);


/**********************************************************************************************************************
 * @brief  Set USB descriptor mode
 * @param  u8Type: USB_DEVICE_TYPE_T
 * @output
 * @return None.
 *********************************************************************************************************************/
extern uint8_t usb_device_set_standard_descriptor(uint8_t u8Type);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__USB_DEVICE_STANDARD_REQUEST_H__*/

