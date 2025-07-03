/**
 * @file     patch_version.h
 * @version  V1.0
 * @date     2023/9/4
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

#ifndef __PATCH_VERSION_H__
#define __PATCH_VERSION_H__

/**
 * @brief  TYPEDEF.
 */
typedef struct
{
    uint8_t  u8RomVer;
    uint8_t  u8Boot2Ver;
    uint32_t u32SDKVer;
    uint16_t u16UserPatchVer;

} stVersion_t;

/**
 * @brief  Get patch lib version.
 * @param  pstVersion_t: Pointer to the version struct, @ref stVersion_t.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T patch_get_version(stVersion_t *pstVersion_t);

#endif /* __PATCH_VERSION_H__ */
