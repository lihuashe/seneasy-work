/**
 * @file     err_def.h
 * @version  V1.0
 * @date     2020/01/03
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

#ifndef __ERR_DEF_H__
#define __ERR_DEF_H__

#ifdef __cplusplus
extern "C"
{
#endif

//=====================================================================================================================
// TYPEDEF ENUM
//=====================================================================================================================
typedef enum
{
    ERR_STA_OK      = 0x00,
    ERR_STA_ERROR   = 0x01,
    ERR_STA_BUSY    = 0x02,
    ERR_STA_TIMEOUT = 0x03,
    ERR_STA_BREAK   = 0x04,

    ERR_PARA_ERR    = 0x10,

    ERR_DATA_BUSY   = 0x20, // Data is not ready
    ERR_NOT_ALLOWED = 0x21,

} EN_ERR_STA_T;

#define ERR_RETURN_IF_ERROR(enRet) \
    do                             \
    {                              \
        if (ERR_STA_OK != (enRet)) \
        {                          \
            return (enRet);        \
        }                          \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif /* __ERR_DEF_H__ */
