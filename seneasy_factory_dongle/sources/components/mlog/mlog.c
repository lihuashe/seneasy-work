/**
 * @file     mlog.c
 * @version  V1.0
 * @date     2023/7/11
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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "mlog.h"

callback_mlog_0 g_mlogCallback0 = NULL;

callback_mlog_16 g_mlogCallback16 = NULL;
callback_mlog_32 g_mlogCallback32 = NULL;
callback_mlog_64 g_mlogCallback64 = NULL;

callback_mlog_88   g_mlogCallback88   = NULL;
callback_mlog_1616 g_mlogCallback1616 = NULL;
callback_mlog_3216 g_mlogCallback3216 = NULL;
callback_mlog_3232 g_mlogCallback3232 = NULL;

callback_mlog_1688   g_mlogCallback1688   = NULL;
callback_mlog_161616 g_mlogCallback161616 = NULL;
callback_mlog_3288   g_mlogCallback3288   = NULL;
callback_mlog_321616 g_mlogCallback321616 = NULL;

callback_mlog_x g_mlogCallbackX = NULL;

void mlog_0(uint16_t u16LogId)
{
    if (NULL != g_mlogCallback0)
    {
        g_mlogCallback0(u16LogId);
    }
}

void mlog_16(uint16_t u16LogId, uint16_t u16Param1)
{
    if (NULL != g_mlogCallback16)
    {
        g_mlogCallback16(u16LogId, u16Param1);
    }
}

void mlog_32(uint16_t u16LogId, uint32_t u32Param1)
{
    if (NULL != g_mlogCallback32)
    {
        g_mlogCallback32(u16LogId, u32Param1);
    }
}

void mlog_64(uint16_t u16LogId, uint64_t u64Param1)
{
    if (NULL != g_mlogCallback64)
    {
        g_mlogCallback64(u16LogId, u64Param1);
    }
}

void mlog_88(uint16_t u16LogId, uint8_t u8Param1, uint8_t u8Param2)
{
    if (NULL != g_mlogCallback88)
    {
        g_mlogCallback88(u16LogId, u8Param1, u8Param2);
    }
}

void mlog_1616(uint16_t u16LogId, uint16_t u16Param1, uint16_t u16Param2)
{
    if (NULL != g_mlogCallback1616)
    {
        g_mlogCallback1616(u16LogId, u16Param1, u16Param2);
    }
}

void mlog_3216(uint16_t u16LogId, uint32_t u32Param1, uint16_t u16Param2)
{
    if (NULL != g_mlogCallback3216)
    {
        g_mlogCallback3216(u16LogId, u32Param1, u16Param2);
    }
}

void mlog_3232(uint16_t u16LogId, uint32_t u32Param1, uint32_t u32Param2)
{
    if (NULL != g_mlogCallback3232)
    {
        g_mlogCallback3232(u16LogId, u32Param1, u32Param2);
    }
}

void mlog_1688(uint16_t u16LogId, uint16_t u16Param1, uint8_t u8Param2, uint8_t u8Param3)
{
    if (NULL != g_mlogCallback1688)
    {
        g_mlogCallback1688(u16LogId, u16Param1, u8Param2, u8Param3);
    }
}

void mlog_161616(uint16_t u16LogId, uint16_t u16Param1, uint16_t u16Param2, uint16_t u16Param3)
{
    if (NULL != g_mlogCallback161616)
    {
        g_mlogCallback161616(u16LogId, u16Param1, u16Param2, u16Param3);
    }
}

void mlog_3288(uint16_t u16LogId, uint32_t u32Param1, uint8_t u8Param2, uint8_t u8Param3)
{
    if (NULL != g_mlogCallback3288)
    {
        g_mlogCallback3288(u16LogId, u32Param1, u8Param2, u8Param3);
    }
}

void mlog_321616(uint16_t u16LogId, uint32_t u32Param1, uint16_t u16Param2, uint16_t u16Param3)
{
    if (NULL != g_mlogCallback321616)
    {
        g_mlogCallback321616(u16LogId, u32Param1, u16Param2, u16Param3);
    }
}

void mlog_x(uint16_t u16LogId, uint8_t *pu8Data, uint8_t u8DataSize)
{
    if (NULL != g_mlogCallbackX)
    {
        g_mlogCallbackX(u16LogId, pu8Data, u8DataSize);
    }
}
