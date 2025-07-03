/**
 * @file     hw_cache.h
 * @version  V1.0
 * @date     2020/06/16
 * @history
 * @note
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2021, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 */

#ifndef __HW_CACHE_H__
#define __HW_CACHE_H__

#ifdef __cplusplus
extern "C"{
#endif


/**
 * @brief  Initialize cache.
 * @param  enEn: Enable cache or bypass, @ref EN_CACHE_ENABLE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_cache_init(EN_CACHE_ENABLE_T enEn);


/**
 * @brief  Configure cache read mode.
 * @param  EN_FLASH_BUS_MODE_T: Read mode, @ref EN_FLASH_BUS_MODE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T rom_hw_cache_set_mode(EN_FLASH_BUS_MODE_T enMode);



#ifdef __cplusplus
}
#endif

#endif /* __HW_CACHE_H__ */

