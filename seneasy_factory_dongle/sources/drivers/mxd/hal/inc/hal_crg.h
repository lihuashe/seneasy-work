/**********************************************************************************************************************
 * @file     hal_crg.h
 * @version  V1.1
 * @date     2022/01/11
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


#ifndef __HAL_CRG_H__
#define __HAL_CRG_H__

#ifdef __cplusplus
extern "C"{
#endif




/**
 * @brief  Take time(us) transform to cycles of RC32K.
 * @param  u32Time: Time, unit: us.
 * @return cycles of RC32K.
 */
// extern uint32_t rom_hal_crg_time_to_32k_cycles(uint32_t u32Time);


/**
 * @brief  Take cycles of RC32K transform to time(us).
 * @param  u32Cycles: Cycles of RC32K.
 * @return Time, unit: us.
 */
extern uint32_t rom_hal_crg_32k_cycles_to_time(uint32_t u32Cycles);


#ifdef __cplusplus
}
#endif

#endif /* __HAL_CRG_H__ */

