/**********************************************************************************************************************
 * @file     hal_wdt.h
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


#ifndef __HAL_WDT_H__
#define __HAL_WDT_H__


//=====================================================================================================================
// DEFINE
//=====================================================================================================================
#define WDT_MAX_TIME_MS                    ( 131082000 )
#define WDT_DELAY_RESET_MAX_TIME_MS        ( 2147 )




/**
 * @brief  Configure watchdog monitor time, delay time of reset and enable wdt.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @param  u32TimeMs: Monitor time, unit: ms. Max time 131082S.
 * @param  u16DelayResetTimeMs: Time of delay reset, unit: ms.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hal_wdt_init(stWDT_Handle_t *pstWDT, uint32_t u32TimeMs, uint16_t u16DelayResetTimeMs);


/**
 * @brief  Deinitializes the WDT Module and reset the registers.
 * @param  pstWDT: WDT handle, Should be WDT0 / WDT1.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hal_wdt_deinit(stWDT_Handle_t* pstWDT);




#endif /* __HAL_WDT_H__ */


