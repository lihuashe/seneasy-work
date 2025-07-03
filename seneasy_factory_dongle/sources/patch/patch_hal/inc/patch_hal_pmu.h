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

#ifndef __PATCH_HAL_PMU_H__
#define __PATCH_HAL_PMU_H__

/**
 * @brief  Set part of RAM_SIZE_KBYTE_MAX KB ram or all of them to retention.
 * @param  u16RamSize: Size of ram which will keep retention, unit: KB.
 *         u16RamSize >= 0 && u16RamSize <= 256.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T patch_hal_pmu_set_sram_ret(uint16_t u16RamSizeKB);

/**
 * @brief  Select System Clock(SYS_HCLK) Source.
 * @param  enSrc: System clock source selection, @ref EN_SYS_CLK_SRC_SEL_T.
 * @param  enFreq: The crystal frequency, @ref EN_DCXO_HCLK_FREQ_T.
 * @param  u16TimeUs: It means the stabilization time of crystal. Unit: us.
 *                    If system clock is @EN_SYS_CLK_RC16M, it means no impact.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T patch_hal_pmu_set_sys_clk_src(EN_SYS_CLK_SRC_SEL_T enSrc, EN_DCXO_HCLK_FREQ_T enFreq,
                                                  uint16_t u16TimeUs);

#endif /* __PATCH_HAL_PMU_H__ */
