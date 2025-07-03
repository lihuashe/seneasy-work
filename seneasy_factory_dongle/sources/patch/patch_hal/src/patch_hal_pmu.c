/**
 * @file     hal_pmu.c
 * @author   Maxscend SoC Software Team
 * @version  V1.1
 * @date     2023/7/1
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
#include <stdbool.h>

#include "RegMXD2670.h"
#include "err_def.h"
#include "hw_crg.h"
#include "hw_pmu.h"
#include "hal_pmu.h"

#include "type_def.h"
#include "utility.h"

#include "mxd2670_mem_cfg_r2.h"

/**
 * @brief  Macro definition
 */
#define SYS_CLK_SETUP_TIME_US (16)
#define SRAM_BLOCK_SIZE_KB    (64)

/**
 * @brief  Set part of RAM_SIZE_KBYTE_MAX KB ram or all of them to retention.
 * @param  u16RamSize: Size of ram which will keep retention, unit: KB.
 *         u16RamSize >= 0 && u16RamSize <= 256.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T patch_hal_pmu_set_sram_ret(uint16_t u16RamSizeKB)
{
    uint8_t  u8Tmp  = 0;
    uint32_t u32Tmp = 0;

    if (u16RamSizeKB > RAM_SIZE_KBYTE_MAX)
    {
        return ERR_PARA_ERR;
    }

    if (u16RamSizeKB % SRAM_BLOCK_SIZE_KB)
    {
        u8Tmp = 1;
    }

    u32Tmp = (0x01 << ((u16RamSizeKB / SRAM_BLOCK_SIZE_KB) + u8Tmp)) - 1;

    rom_hw_pmu_clr_sram_block_ret(SRAM_BLOCK_EN_MSK);
    rom_hw_pmu_set_sram_block_ret(u32Tmp);

    return ERR_STA_OK;
}

/**
 * @brief  Select System Clock(SYS_HCLK) Source.
 * @param  enSrc: System clock source selection, @ref EN_SYS_CLK_SRC_SEL_T.
 * @param  enFreq: The crystal frequency, @ref EN_DCXO_HCLK_FREQ_T.
 * @param  u16TimeUs: It means the stabilization time of crystal. Unit: us.
 *                    If system clock is @EN_SYS_CLK_RC16M, it means no impact.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T patch_hal_pmu_set_sys_clk_src(EN_SYS_CLK_SRC_SEL_T enSrc, EN_DCXO_HCLK_FREQ_T enFreq, uint16_t u16TimeUs)
{
    EN_ERR_STA_T enRet;
    uint32_t     u32SysClkSrc;

    if (!IS_VALID_SYS_CLK_SRC_SEL(enSrc))
    {
        return ERR_PARA_ERR;
    }

    if ((EN_SYS_CLK_RC16M == enSrc) || (EN_DCXO_HCLK_FREQ_16M == enFreq))
    {
        rom_hal_pmu_set_sys_clk_src(enSrc, u16TimeUs);
    }
    else
    {
        rom_hw_crg_get_sys_clk_src(&u32SysClkSrc);

        switch (enSrc)
        {
            case EN_SYS_CLK_DCXO16M:
            {
                if (SYS_CLK_SRC_DCXO_HCLK_DIV2 == u32SysClkSrc)
                {
                    return ERR_STA_OK;
                }

                // Turn on DCXO_HCLK power and enable DCXO_HCLK clock out.
                enRet = rom_hal_pmu_enable_dcxo_hclk(enFreq, u16TimeUs);
                ERR_RETURN_IF_ERROR(enRet);

                // set PMU_HCLK frequency to 1MHz
                rom_hw_crg_set_pmu_hclk_divisor(EN_PMU_HCLK_DIV_16);

                // set CM33 APB frequency to 16MHz
                rom_hw_crg_set_apb_clk_divisor(EN_APB_CLK_DIV_1);

                // set CM0 AHB frequency to 16MHz
                rom_hw_crg_set_cp_ahb_clk_divisor(EN_AHB_CLK_DIV_1);

                // set SYS_CLK clock source to DCXO_HCLK(16MHz)
                rom_hw_crg_set_sys_clk_src(SYS_CLK_DCXO_HCLK_DIV2);

                // set SYS_CLK global variable to 16000000,
                // means system clock is 16MHz
                #if defined(__MAIN_PROCESSOR)
                    SystemCoreSetClock(SYS_CLOCK_16M_HZ);
                #endif
                break;
            }
            case EN_SYS_CLK_PLL64M:
            {
                if (SYS_CLK_SRC_PLL_CLK == u32SysClkSrc)
                {
                    return ERR_STA_OK;
                }

                // Turn on pll power and enable pll clock out.
                enRet = rom_hal_pmu_enable_pll_clk(enFreq, u16TimeUs);
                ERR_RETURN_IF_ERROR(enRet);

                // Select pll128m clock (PLL128M_CLK) Division.
                // Set SYS_CLK frequency to 64MHz
                rom_hw_crg_set_pll128m_clk_divisor(EN_PLL128M_CLK_DIV_2);

                // Select QSPI0 clock source to SYS_CLK
                rom_hw_crg_set_qspi_clk_src(CRG_QSPI0, QSPI_CLK_SYS_CLK);

                // set PMU_HCLK frequency to 1MHz
                rom_hw_crg_set_pmu_hclk_divisor(EN_PMU_HCLK_DIV_64);

                // set CM33 APB frequency to 16MHz
                rom_hw_crg_set_apb_clk_divisor(EN_APB_CLK_DIV_4);

                // set CM0 AHB frequency to 16MHz
                rom_hw_crg_set_cp_ahb_clk_divisor(EN_AHB_CLK_DIV_4);

                // set SYS_CLK clock source to PLL_CLK(64MHz)
                rom_hw_crg_set_sys_clk_src(SYS_CLK_PLL_CLK);

                // set SYS_CLK global variable to 64000000,
                // means system clock is 64MHz
                #if defined(__MAIN_PROCESSOR)
                    SystemCoreSetClock(SYS_CLOCK_64M_HZ);
                #endif
                break;
            }
            default: break;
        }
    }

    rom_delay_us(SYS_CLK_SETUP_TIME_US);

    return ERR_STA_OK;
}
