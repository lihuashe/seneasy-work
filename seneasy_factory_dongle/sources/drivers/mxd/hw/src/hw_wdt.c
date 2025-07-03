/**********************************************************************************************************************
 * @file     hw_wdt.c
 * @author   Maxscend SoC Software Team
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

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "RegMXD2670.h"
#include "err_def.h"

#include "type_def.h"
#include "utility.h"

#include "hw_wdt.h"

//=====================================================================================================================
// DEFINE
//=====================================================================================================================
#define WDT_CFG_VALID_TIMEOUT_US           (120)




/**
 * @brief  Trigger timer configuration valid.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @param  enCh: Indicate which timer will be cfg, @ref EN_TIMER_CH_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_wdt_trig_cfg_valid(stWDT_Handle_t *pstWDT, uint32_t u32Cfg)
{
    uint32_t u32Count = 0;

    if ((!(IS_VALID_WDT_HANDLE(pstWDT))) || (WDT_CFG_VALID_FLAG_MASK < u32Cfg))
    {
        return ERR_PARA_ERR;
    }

    while (u32Cfg != (pstWDT->WDT_CFG_VALID_FLAG & u32Cfg))
    {
        if (WDT_CFG_VALID_TIMEOUT_US < u32Count)
        {
            pstWDT->WDT_CFG_VALID_CLR = u32Cfg;
            return ERR_STA_TIMEOUT;
        }

        u32Count++;
        rom_delay_us(1);
    }

    pstWDT->WDT_CFG_VALID_CLR = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Deinitializes the WDT Module and reset the registers.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_wdt_deinit(stWDT_Handle_t *pstWDT)
{
    EN_ERR_STA_T enRet;

    /* 1. Set wdt compare counter to default */
    pstWDT->WDT_COMPARE = (0xFF & WDT_COMPARE_CNT_MSK) << WDT_COMPARE_CNT_SHIFT;
    pstWDT->WDT_TRIG_CFG = WDT_TRIG_COMPARE_VALID;

    /* 2. Set wdt delay reset to default. */
    pstWDT->WDT_RESET_DELAY_TIME = (0x10 & WDT_RESET_DELAY_TIME_MSK) << WDT_RESET_DELAY_TIME_SHIFT;

    /* 3. Disable wdt delay reset. */
    pstWDT->WDT_RESET_EN = WDT_RESET_DISABLE << WDT_RESET_EN_SHIFT;
    pstWDT->WDT_TRIG_CFG = WDT_TRIG_RESET_DELAY_VALID;

    /* 4. Wait wdt cfg valid. */
    enRet = rom_hw_wdt_trig_cfg_valid(pstWDT, WDT_COMPARE_CNT_VALID | WDT_RESET_DELAY_VALID);
    ERR_RETURN_IF_ERROR(enRet);

    return ERR_STA_OK;
}

/**
 * @brief  Enable watchdog timer.
 *         This function needs 3 cycles of 32KHz to be valid.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_wdt_enable(stWDT_Handle_t *pstWDT)
{
    if (!(IS_VALID_WDT_HANDLE(pstWDT)))
    {
        return ERR_PARA_ERR;
    }

    pstWDT->WDT_EN = WDT_ENABLE;
    pstWDT->WDT_TRIG_CFG = WDT_TRIG_ENABLE_VALID;

    return rom_hw_wdt_trig_cfg_valid(pstWDT, WDT_ENABLE_VALID);
}

/**
 * @brief  Disable watchdog timer.
 *         This function needs 3 cycles of 32KHz to be valid.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_wdt_disable(stWDT_Handle_t *pstWDT)
{
    if (!(IS_VALID_WDT_HANDLE(pstWDT)))
    {
        return ERR_PARA_ERR;
    }

    pstWDT->WDT_EN = WDT_DISABLE;
    pstWDT->WDT_TRIG_CFG = WDT_TRIG_ENABLE_VALID;

    return rom_hw_wdt_trig_cfg_valid(pstWDT, WDT_ENABLE_VALID);
}

/**
 * @brief  Configure watchdog compare value(monitor time).
 *         This function needs 3 cycles of 32KHz to be valid.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @param  u32Compare: WDT compare counter, the unit is a cycle of 32KHz.
 * @return status: @ref EN_ERR_STA_T.
 **/
EN_ERR_STA_T rom_hw_wdt_set_compare(stWDT_Handle_t *pstWDT, uint32_t u32Compare)
{
    if (!(IS_VALID_WDT_HANDLE(pstWDT)))
    {
        return ERR_PARA_ERR;
    }

    pstWDT->WDT_COMPARE = (u32Compare & WDT_COMPARE_CNT_MSK) << WDT_COMPARE_CNT_SHIFT;
    pstWDT->WDT_TRIG_CFG = WDT_TRIG_COMPARE_VALID;

    return ERR_STA_OK;
}

/**
 * @brief  Get watchdog timer counter.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_wdt_get_counter(stWDT_Handle_t *pstWDT, uint32_t *pu32Counter)
{
    if (!(IS_VALID_WDT_HANDLE(pstWDT)) || (NULL == pu32Counter))
    {
        return ERR_PARA_ERR;
    }

    *pu32Counter = (pstWDT->WDT_COUNTER >> WDT_COUNTER_SHIFT) & WDT_COUNTER_MSK;

    return ERR_STA_OK;
}

/**
 * @brief  Enable watchdog reset system after wdt counter reach compare counter. (default)
 *         This function needs 3 cycles of 32KHz to be valid.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_wdt_enable_reset(stWDT_Handle_t *pstWDT)
{
    if (!(IS_VALID_WDT_HANDLE(pstWDT)))
    {
        return ERR_PARA_ERR;
    }

    pstWDT->WDT_RESET_EN = WDT_RESET_ENABLE << WDT_RESET_EN_SHIFT;
    pstWDT->WDT_TRIG_CFG = WDT_TRIG_RESET_DELAY_VALID;

    return ERR_STA_OK;
}

/**
 * @brief  Disable watchdog reset system after wdt counter reach compare counter.
 *         This function needs 3 cycles of 32KHz to be valid.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_wdt_disable_reset(stWDT_Handle_t *pstWDT)
{
    if (!(IS_VALID_WDT_HANDLE(pstWDT)))
    {
        return ERR_PARA_ERR;
    }

    pstWDT->WDT_RESET_EN = WDT_RESET_DISABLE << WDT_RESET_EN_SHIFT;
    pstWDT->WDT_TRIG_CFG = WDT_TRIG_RESET_DELAY_VALID;

    return ERR_STA_OK;
}

/**
 * @brief  Configure delay reset time.
 *         This function needs 3 cycles of 32KHz to be valid.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @param  u16Time: Time of delay reset, the unit is a cycle of 32KHz.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_wdt_set_delay_reset(stWDT_Handle_t *pstWDT, uint16_t u16Time)
{
    if (!(IS_VALID_WDT_HANDLE(pstWDT)))
    {
        return ERR_PARA_ERR;
    }

    pstWDT->WDT_RESET_DELAY_TIME = (u16Time & WDT_RESET_DELAY_TIME_MSK) << WDT_RESET_DELAY_TIME_SHIFT;
    pstWDT->WDT_TRIG_CFG = WDT_TRIG_RESET_DELAY_VALID;

    return ERR_STA_OK;
}

/**
 * @brief  Clear watchdog timer counter.
 *         This function needs 3 cycles of 32KHz to be valid.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_wdt_feed(stWDT_Handle_t *pstWDT)
{
    if (!(IS_VALID_WDT_HANDLE(pstWDT)))
    {
        return ERR_PARA_ERR;
    }

    pstWDT->WDT_TRIG_CFG = WDT_TRIG_CLEAR_COUNT_VALID;

    return ERR_STA_OK;
}

/**
 * @brief  Get indicated WDT interrupt flag(status) by read the WDT_INT_FLAG register.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @param  pu8IntMsk: Indicate which interrupt flag will be read.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_wdt_get_interrupt_flag(stWDT_Handle_t *pstWDT, uint8_t *pu8IntMsk)
{
    if (!(IS_VALID_WDT_HANDLE(pstWDT)) || (NULL == pu8IntMsk))
    {
        return ERR_PARA_ERR;
    }

    *pu8IntMsk = pstWDT->WDT_INT_FLAG & WDT_INT_MASK;

    return ERR_STA_OK;
}

/**
 * @brief  Clear indicated WDT interrupt flag(status) by write the WDT_INT_CLR register.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @param  u8Msk: Indicate which flag will be cleared.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_wdt_clear_interrupt_flag(stWDT_Handle_t *pstWDT, uint8_t u8Msk)
{
    if (!(IS_VALID_WDT_HANDLE(pstWDT)) || (WDT_INT_MASK < u8Msk))
    {
        return ERR_PARA_ERR;
    }

    pstWDT->WDT_INT_CLR = u8Msk;

    return ERR_STA_OK;
}

/**
 * @brief  Enable indicated WDT interrupt byt write WDT_INT_EN register with indicated bits.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @param  u8IntEn: Indicate which interrupt will be enable, 
 *                  bit-1 means enable
 *                  bit-0 means no impact.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_wdt_enable_interrupt(stWDT_Handle_t *pstWDT, uint8_t u8IntEn)
{
    if (!(IS_VALID_WDT_HANDLE(pstWDT)) || (WDT_INT_MASK < u8IntEn))
    {
        return ERR_PARA_ERR;
    }

    pstWDT->WDT_INT_CLR = u8IntEn;
    pstWDT->WDT_INT_EN |= (u8IntEn & WDT_INT_MASK);

    return ERR_STA_OK;
}

/**
 * @brief  Disable indicated WDT interrupt byt write WDT_INT_EN register with indicated bits.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @param  u8IntDis: Indicate which interrupt will be disable, 
 *                   bit-1 means disable.
 *                   bit-0 means no impact.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_wdt_disable_interrupt(stWDT_Handle_t *pstWDT, uint8_t u8IntDis)
{
    if (!(IS_VALID_WDT_HANDLE(pstWDT)) || (WDT_INT_MASK < u8IntDis))
    {
        return ERR_PARA_ERR;
    }

    pstWDT->WDT_INT_EN &= ~u8IntDis;
    pstWDT->WDT_INT_CLR = u8IntDis;

    return ERR_STA_OK;
}
