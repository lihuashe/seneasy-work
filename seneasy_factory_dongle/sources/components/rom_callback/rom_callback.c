/*
 * @file     rom_callback.c
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2021/05/27
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
#include <stdbool.h>

#include "rom_callback.h"

/**
 * @brief  Local function callback.
 */
func_lib_assert g_libAssertCallback = NULL;
func_lib_idle_task g_libIdleTaskCallback = NULL;
func_lib_os_error g_libOsErrorCallback = NULL;
func_lib_system_error g_libSystemErrorCallback = NULL;

void system_assert(uint16_t u16Addr, bool bCondition, uint32_t u32Info)
{
    if ((NULL != g_libAssertCallback) && !bCondition)
    {
        g_libAssertCallback(u16Addr, u32Info);
    }
}

void system_error(uint16_t u16ErrModule, uint16_t u16ErrLine, uint32_t u32ErrInfo1, uint32_t u32ErrInfo2)
{
    if (NULL != g_libSystemErrorCallback)
    {
        g_libSystemErrorCallback(u16ErrModule, u16ErrLine, u32ErrInfo1, u32ErrInfo2);
    }
}

/**
 * @brief  Idle task processor callback for cp and mp.
 */
void idle_task_processor(void)
{
    if (NULL != g_libIdleTaskCallback)
    {
        g_libIdleTaskCallback();
    }
}

/**
 * @brief  os error processor callback for cp and mp.
 */
uint8_t os_error_processor(uint32_t u32Code, void* pObjectId)
{
    if (NULL != g_libOsErrorCallback)
    {
        g_libOsErrorCallback(u32Code, pObjectId);
    }
    else
    {
        return 0;
    }

    return 1;
}

