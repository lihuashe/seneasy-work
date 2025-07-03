/**
 * @file     idle_task.c
 * @author   Maxscend SoC Software Team
 * @version  V1.1
 * @date     2022/02/10
 * @history
 * @note     idle task.
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2022, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "RegMXD2670.h"
#include "err_def.h"

#include "app_cfg.h"
#include "utility.h"
#include "lpwr_ctrl.h"
#include "rom_callback.h"

#if APP_DEBUG_ENABLED
    #include "debug.h"
#endif

#if MLOG_DEBUG_ENABLED
    #include "mlog_transport_uart.h"
#endif

/**
 * @brief  System idle process function
 */
static void idle_task_sys_idle_process(void)
{
#if APP_DEBUG_ENABLED
    debug_printf();
    rom_delay_us(150);
#endif

#if MLOG_DEBUG_ENABLED
    mlog_transport_uart_print();
    rom_delay_us(150);
#endif

    //lpwr_ctrl_goto_sleep();
}

/**
 * @brief  idle task init.
 */
void idle_task_init(void)
{
    g_libIdleTaskCallback = idle_task_sys_idle_process;
}
