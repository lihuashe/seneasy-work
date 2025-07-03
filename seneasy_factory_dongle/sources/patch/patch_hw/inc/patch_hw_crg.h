/**
 * @file     patch_hw_crg.h
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

#ifndef __PATCH_HW_CRG_H__
#define __PATCH_HW_CRG_H__

/**
 * @brief  Select GTIM System Clock (GTIM_SCLK) division with SYS_CLK.
 * @param  enDiv: The divisor of system clock, @ref EN_GTIM_SYS_CLK_DIV_T.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T patch_hw_crg_set_gtim_sys_clk_divisor(EN_GTIM_SYS_CLK_DIV_T enDiv);

/**
 * @brief  Take time(us) transform to cycles of RC32K.
 * @param  u32Time: Time, unit: us
 * @return cycles of RC32K.
 */
extern uint32_t patch_hw_crg_time_to_32k_cycles(uint32_t u32TimeUs);

#endif /* __PATCH_HW_CRG_H__ */
