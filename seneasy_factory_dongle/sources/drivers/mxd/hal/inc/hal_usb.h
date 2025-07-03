/**********************************************************************************************************************
 * @file     hal_usb.h
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

#ifndef __HAL_USB_H__
#define __HAL_USB_H__

#ifdef __cplusplus
extern "C"{
#endif




/**
 * @brief  Enable or disable usb clock gate.
 * @param  u8Enable: 0 - disable, others - enable.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hal_usb_clock_enable(uint8_t u8Enable);


/**
 * @brief  Reset usb module by software reset.
 */
extern void rom_hal_usb_clock_reset(void);


/**
 * @brief  Device software connect.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hal_usb_device_software_connect(void);


/**
 * @brief  Device software disconnect.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hal_usb_device_software_disconnect(void);


/**
 * @brief  usb phy device init.
 * @param  Initialization process of usb Device PHY.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hal_usb_phy_device_init(void);


/**
 * @brief  usb phy host init.
 * @param  Initialization process of usb Host PHY.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hal_usb_phy_host_init(void);


/**
 * @brief  usb device init.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hal_usb_device_init(void);


/**
 * @brief  usb phy init.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hal_usb_host_init(void);


/**
 * @brief  usb phy test init.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hal_usb_phy_test_mode_init(void);




#ifdef __cplusplus
}
#endif

#endif /*__HAL_USB_H__*/

