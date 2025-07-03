/**
 * @file     patch_version.c
 * @author   Maxscend SoC Software Team
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

#include <stdint.h>
#include <stdio.h>

#include "err_def.h"
#include "version.h"

#include "patch_version.h"

/**
 * @brief  Get patch lib version.
 * @param  pstVersion_t: Pointer to the version struct, @ref stVersion_t.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T patch_get_version(stVersion_t *pstVersion_t)
{
    if (NULL == pstVersion_t)
    {
        return ERR_PARA_ERR;
    }

    pstVersion_t->u8RomVer        = ROM_VERSION;
    pstVersion_t->u8Boot2Ver      = BOOT2_VERSION;
    pstVersion_t->u32SDKVer       = SDK_VERSION;
    pstVersion_t->u16UserPatchVer = USER_PATCH_VERSION;

    return ERR_STA_OK;
}
