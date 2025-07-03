/**
 * @file     hw_usb_ctrl.h
 * @author   Maxscend SoC Software Team
 * @version  V1.1
 * @date     2022/1/14
 * @history
 * @note
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

#ifndef __HW_USB_CTRL_H__
#define __HW_USB_CTRL_H__

#ifdef __cplusplus
 extern "C" {
#endif /*__cplusplus*/


//=====================================================================================================================
// DEFINE
//=====================================================================================================================
#define USB_MASK_EP_INVAILD_INT            (0x7F7F)
#define USB_REG_FIELD_BIT                  (8)
#define USB_ENDPOINT_MAX                   (7)
#define USB_INVAILD_EPCB                   ((struct usb_fp_callback_t *)(0xFFFFFFFF))

//=====================================================================================================================
// TYPEDEF
//=====================================================================================================================
typedef enum
{
    EN_USB_HOOK_UNKNOWN         = 0,
    EN_USB_HOOK_DEV_PHY         = 1,
    EN_USB_HOOK_HOST_PHY        = 2,
    EN_USB_HOOK_DEV_CTRL        = 3,
    EN_USB_HOOK_HOST_CTRL       = 4,
    EN_USB_HOOK_UNFIFO          = 5,
    EN_USB_HOOK_FIFO            = 6,
    EN_USB_HOOK_MAX,

} EN_USB_CALLBACK_HOOK_T;

typedef void (*FPCALLBACK)(uint8_t u8Type, uint8_t u8EpNum);

struct usb_fp_callback_t
{
    uint8_t u8EpNum;
    FPCALLBACK fpcb;
};

typedef int (*USBCallBackHook_t)(int, void*, void*);


//=====================================================================================================================
// EXTERN VARIABLES
//=====================================================================================================================
extern USBCallBackHook_t USBCallBackHook[EN_USB_HOOK_MAX];





/**
 * @brief  A send buffer that load data from a buffer into an endpoint.
 * @param  u32EndPointNum: EpNum endpoint.
 * @param  pu8Buf: Point to data buf.
 * @param  u32Len: buf data lenght.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_usb_load_fifo_data(uint32_t u32EndPointNum, uint8_t* pu8Buf, uint32_t u32Len);


/**
 * @brief  A send buffer that unload data from a buffer into an endpoint.
 * @param  u32EndPointNum: EpNum endpoint.
 * @param  pu8Buf: Point to data buf.
 * @param  u32Len: buf data lenght.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_usb_unload_fifo_data(uint32_t u32EndPointNum, uint8_t* pu8Buf, uint32_t u32Len);


/**
 * @brief  Usb get endpoint interrupt flag.
 * @param  pu8EpIndex: Endpoint that generates the interrupt.
 * @param  pu16Intrx: Endpoint interrupt flag.
 * @param  pu8IntrUSB: Bus abnormal interrupt flag.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_usb_ctrl_get_interrupt_flag(uint8_t* pu8EpIndex, uint16_t* pu16Intrx, uint8_t* pu8IntrUSB);


/**
 * @brief  The Device modeler enable an endpoint to interrupt.
 * @param  u8EpNum: EpNum endpoint.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_usb_ctrl_endpoint_interrupt_enable(uint8_t u8EpNum);


/**
 * @brief  The Device modeler disable an endpoint to interrupt.
 * @param  u8EpNum: EpNum endpoint.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_usb_ctrl_endpoint_interrupt_disable(uint8_t u8EpNum);


/**
 * @brief  The Device modeler init an endpoint to interrupt.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_usb_ctrl_endpoint_interrupt_init(void);




#ifdef  __cplusplus
}
#endif /*__cplusplus*/

#endif /*__HW_USB_CTRL_H__*/


