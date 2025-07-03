/**
 * @file     patch_hw_crg.c
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2023/5/24
 * @history
 * @note
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2023, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "RegMXD2670.h"
#include "err_def.h"
#include "hw_crg.h"

/**
 * @brief  Select GTIM System Clock (GTIM_SCLK) division with SYS_CLK.
 * @param  enDiv: The divisor of system clock, @ref EN_GTIM_SYS_CLK_DIV_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T patch_hw_crg_set_gtim_sys_clk_divisor(EN_GTIM_SYS_CLK_DIV_T enDiv)
{
    uint32_t u32Cfg = 0;

    if (!IS_GTIM_SYS_CLK_DIV_VALID(enDiv))
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = CRG->CRG_CLK_DIV_CFG0;
    u32Cfg &= ~(CRG_GTIM_SYS_CLK_DIV_MSK << CRG_GTIM_SYS_CLK_DIV_SHIFT);
    u32Cfg |= (enDiv & CRG_GTIM_SYS_CLK_DIV_MSK) << CRG_GTIM_SYS_CLK_DIV_SHIFT;
    CRG->CRG_CLK_DIV_CFG0 = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Take time(us) transform to cycles of RC32K.
 * @param  u32Time: Time, unit: us
 * @return cycles of RC32K.
 */
uint32_t patch_hw_crg_time_to_32k_cycles(uint32_t u32TimeUs)
{
    uint16_t u16Ratio = rom_hw_crg_get_32k_cycles_of_dcxo_hclk();

    if (u16Ratio)
    {
        return (((uint64_t)(u32TimeUs)) << 10) / (u16Ratio);
    }

    return 0;
}
