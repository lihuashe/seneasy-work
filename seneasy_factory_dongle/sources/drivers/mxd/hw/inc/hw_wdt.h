/**********************************************************************************************************************
 * @file     hw_wdt.h
 * @version  V1.0
 * @date     2021/6/8
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

#ifndef __HW_WDT_H__
#define __HW_WDT_H__

#ifdef __cplusplus
    extern "C" {
#endif 




/**
 * @brief  Trigger timer configuration valid.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @param  enCh: Indicate which timer will be cfg, @ref EN_TIMER_CH_T.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_wdt_trig_cfg_valid(stWDT_Handle_t* pstWDT, uint32_t u32Cfg);


/**
 * @brief  Deinitializes the WDT Module and reset the registers.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_wdt_deinit(stWDT_Handle_t *pstWDT);


/**
 * @brief  Enable watchdog timer.
 *         This function needs 3 cycles of 32KHz to be valid.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_wdt_enable(stWDT_Handle_t* pstWDT);


/**
 * @brief  Disable watchdog timer.
 *         This function needs 3 cycles of 32KHz to be valid.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_wdt_disable(stWDT_Handle_t* pstWDT);


/**
 * @brief  Configure watchdog compare value(monitor time).
 *         This function needs 3 cycles of 32KHz to be valid.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @param  u32Compare: WDT compare counter, the unit is a cycle of 32KHz.
 * @return status: @ref EN_ERR_STA_T.
 **/
extern EN_ERR_STA_T rom_hw_wdt_set_compare(stWDT_Handle_t *pstWDT, uint32_t u32Compare);


/**
 * @brief  Get watchdog timer counter.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_wdt_get_counter(stWDT_Handle_t* pstWDT, uint32_t *pu32Counter);


/**
 * @brief  Enable watchdog reset system after wdt counter reach compare counter. (default)
 *         This function needs 3 cycles of 32KHz to be valid.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_wdt_enable_reset(stWDT_Handle_t* pstWDT);


/**
 * @brief  Disable watchdog reset system after wdt counter reach compare counter.
 *         This function needs 3 cycles of 32KHz to be valid.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_wdt_disable_reset(stWDT_Handle_t* pstWDT);


/**
 * @brief  Configure delay reset time.
 *         This function needs 3 cycles of 32KHz to be valid.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @param  u16Time: Time of delay reset, the unit is a cycle of 32KHz.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_wdt_set_delay_reset(stWDT_Handle_t* pstWDT, uint16_t u16Time);


/**
 * @brief  Clear watchdog timer counter.
 *         This function needs 3 cycles of 32KHz to be valid.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_wdt_feed(stWDT_Handle_t* pstWDT);


/**
 * @brief  Get indicated WDT interrupt flag(status) by read the WDT_INT_FLAG register.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @param  pu8IntMsk: Indicate which interrupt flag will be read.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_wdt_get_interrupt_flag(stWDT_Handle_t* pstWDT, uint8_t* pu8IntMsk);


/**
 * @brief  Clear indicated WDT interrupt flag(status) by write the WDT_INT_CLR register.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @param  u8Msk: Indicate which flag will be cleared.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_wdt_clear_interrupt_flag(stWDT_Handle_t* pstWDT, uint8_t u8Msk);


/**
 * @brief  Enable indicated WDT interrupt byt write WDT_INT_EN register with indicated bits.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @param  u8IntEn: Indicate which interrupt will be enable, 
 *                  bit-1 means enable
 *                  bit-0 means no impact.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_wdt_enable_interrupt(stWDT_Handle_t* pstWDT, uint8_t u8IntEn);


/**
 * @brief  Disable indicated WDT interrupt byt write WDT_INT_EN register with indicated bits.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @param  u8IntDis: Indicate which interrupt will be disable, 
 *                   bit-1 means disable.
 *                   bit-0 means no impact.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_wdt_disable_interrupt(stWDT_Handle_t* pstWDT, uint8_t u8IntDis);


#ifdef __cplusplus
}
#endif

#endif /* _HW_WDT_H_ */


