/**********************************************************************************************************************
 * @file     usb_utility.h
 * @version  V1.0
 * @date     2022/12/20
 * @history
 * @note
 **********************************************************************************************************************/

#ifndef __USBD_UTILITY_H__
#define __USBD_UTILITY_H__


#define Le16ToCpu(Val)      (Val)
#define Le32ToCpu(Val)      (Val)
#define CpuToLe16(Val)      (Val)
#define CpuToLe32(Val)      (Val)

#define Be16ToCpu(Val)      (((Val) << 8) | ((Val) >> 8))
#define Be32ToCpu(Val)      (((Val) << 24) | (((Val) << 8) & 0x00FF0000) | (((Val) >> 8) & 0x0000FF00) | ((Val) >> 24))
#define CpuToBe16(Val)      (((Val) << 8) | ((Val) >> 8))
#define CpuToBe32(Val)      (((Val) << 24) | (((Val) << 8) & 0x00FF0000) | (((Val) >> 8) & 0x0000FF00) | ((Val) >> 24))


#endif /* __USB_UTILITY_H__ */


