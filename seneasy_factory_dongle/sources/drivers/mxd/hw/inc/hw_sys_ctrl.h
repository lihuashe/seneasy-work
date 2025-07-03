/**********************************************************************************************************************
 * @file     hw_sys_ctrl.h
 * @version  V1.1
 * @date     2022/01/17
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

#ifndef __HW_SYS_CTRL_H__
#define __HW_SYS_CTRL_H__


#ifdef __cplusplus
extern "C" {
#endif




/**
 * @brief  Enable cp(cm0p) remap function.
 * @param  u32Addr: The real address of cp in the bus. Align to KB.
 * @param  u16Size: The size of remap code. Align to 4 bytes.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_sys_ctrl_enable_cp_remap(uint32_t u32Addr, uint16_t u16Size);


/**
 * @brief  Disable cp remap function.
 */
extern void rom_hw_sys_ctrl_disable_cp_remap(void);


/**
 * @brief  Write The Common Register which Used for MP and CP.
 * @param  enReg: The common register index, @ref EN_SYS_CTRL_COM_REG_T.
 * @param  u32Val: The Value which will write to the common register.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_sys_ctrl_write_com_reg(EN_SYS_CTRL_COM_REG_T enReg, uint32_t u32Val);


/**
 * @brief  Read The Common Register which Used for MP and CP.
 * @param  enReg: The common register index, @ref EN_SYS_CTRL_COM_REG_T.
 * @param  pu32Val: The Value which read from the common register.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_sys_ctrl_read_com_reg(EN_SYS_CTRL_COM_REG_T enReg, uint32_t* pu32Val);


/**
 * @brief  Lock device.
 * @param  u8LockId: Lock id, @ref EN_SYS_CTRL_LOCK_ID_T.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_sys_ctrl_lock_device(uint8_t u8LockId);


/**
 * @brief  Release device lock.
 * @param  u8LockId: Lock id, @ref EN_SYS_CTRL_LOCK_ID_T.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_sys_ctrl_release_device(uint8_t u8LockId);


/**
 * @brief  Enable CP or MP indicated Peripheral interrupt.
 * @param  enCore: CP or MP, @ref EN_SYS_CTRL_CORE_SEL_T.
 * @param  u8Irq: Indicate which interrupt will be enable, @ref IRQn_Type.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_sys_ctrl_enable_peri_int(EN_SYS_CTRL_CORE_SEL_T enCore, uint8_t u8Irq);


/**
 * @brief  Disable CP or MP indicated Peripheral interrupt.
 * @param  enCore: CP or MP, @ref EN_SYS_CTRL_CORE_SEL_T.
 * @param  u8Irq: Indicate which interrupt will be disable, @ref IRQn_Type.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_sys_ctrl_disable_peri_int(EN_SYS_CTRL_CORE_SEL_T enCore, uint8_t u8Irq);


/**
 * @brief  Enable or disable CP or MP indicated Peripheral interrupt, each bit corresponding a peripheral.
 * @param  enCore: CP or MP, @ref EN_SYS_CTRL_CORE_SEL_T.
 * @param  u32Reg0: Indicate which interrupt will be enable or disable, @ref IRQn_Type.
 * @param  u32Reg1: Indicate which interrupt will be enable or disable, @ref IRQn_Type.
 *         0: Disable xx peripheral interrupt.
 *         1: Enable xx peripheral interrupt.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_sys_ctrl_peri_int_ctrl(EN_SYS_CTRL_CORE_SEL_T enCore, uint32_t u32Reg0, uint32_t u32Reg1);


/**
 * @brief  Get CP or MP indicated SoftWare interrupt flag(status).
 * @param  enCore: CP or MP, @ref EN_SYS_CTRL_CORE_SEL_T.
 * @param  pu8Msk: Indicate which interrupt flag will be read.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_sys_ctrl_get_sw_int_flag(EN_SYS_CTRL_CORE_SEL_T enCore, uint8_t* pu8Msk);


/**
 * @brief  Clear CP or MP indicated SoftWare interrupt flag(status).
 * @param  enCore: CP or MP, @ref EN_SYS_CTRL_CORE_SEL_T.
 * @param  u8Msk: Indicate which flag will be cleared.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_sys_ctrl_clear_sw_int_flag(EN_SYS_CTRL_CORE_SEL_T enCore, uint8_t u8Msk);


/**
 * @brief  Trigger CP or MP indicated SoftWare interrupt.
 * @param  enCore: CP or MP, @ref EN_SYS_CTRL_CORE_SEL_T.
 * @param  u8IntEn: Indicate which interrupt will be enable,
 *                  bit-1 means enable
 *                  bit-0 means no impact
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_sys_ctrl_trigger_sw_int(EN_SYS_CTRL_CORE_SEL_T enCore, uint8_t u8IntEn);


/**
 * @brief  Control gptimer Gtimer start to work or stop working synchronously.
 * @param  u16SyncCtrl: Start or stop timer. @ref EN_SYS_CTRL_TIMER_SYNC_CTRL_T.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_sys_ctrl_gtim_sync_ctrl(uint16_t u16SyncCtrl);


/**
 * @brief  Get USB phy interrupt flag.
 * @param  pu32Flag: Interrupt flag.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_sys_ctrl_get_usb_phy_int_flag(uint32_t* pu32Flag);


/**
 * @brief  Clear USB phy interrupt flag.
 * @param  u32Flag: interrupt flag.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_sys_ctrl_clear_usb_phy_int_flag(uint32_t u32Flag);


/**
 * @brief  Enable USB phy interrupt.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_sys_ctrl_enable_usb_phy_pd_int(void);


/**
 * @brief  Disable USB phy interrupt.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_sys_ctrl_disable_usb_phy_pd_int(void);


/**
 * @brief  Get USB phy por event interrupt.
 * @param  pu32Flag: Interrupt flag.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_sys_ctrl_get_usb_phy_por_event_int_flag(uint32_t* pu32Flag);


/**
 * @brief  Enable wdt1 wakeup interrupt.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_sys_ctrl_enable_wdt_wakeup_int(void);


/**
 * @brief  Disable wdt1 wakeup interrupt.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_sys_ctrl_disable_wdt_wakeup_int(void);


/**
 * @brief  Get wdt1 wakeup interrupt flag.
 * @param  pu32Flag: Interrupt flag.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_sys_ctrl_get_wdt_wakeup_int_flag(uint32_t* pu32Flag);


/**
 * @brief  Clear wdt1 wakeup interrupt flag.
 * @param  u32Flag: interrupt flag.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_sys_ctrl_clear_wdt_wakeup_int_flag(uint32_t u32Flag);




#ifdef __cplusplus
}
#endif

#endif /* __HW_SYS_CTRL_H__ */


