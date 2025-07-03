/**
 * @file     main.c
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2020/07/15
 * @history
 * @note     ble_peripheral cp main file
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

#include "cmsis_os2.h"
#include "rtx_lib.h"

/**
 * @brief  Local variables
 */
static osRtxThread_t m_appMainCB __attribute__((aligned(4), section(".bss.os.thread.cb")));
static uint64_t      m_u64AppMainStack[128];

/**
 * @brief  Extern functions
 * @param  argument
 */
extern void app_main(void *argument);

/**
 * @brief  main function
 * @return
 */
int main(void)
{
    const osThreadAttr_t attr = {
        "app_main",
        osThreadJoinable,
        &m_appMainCB,
        sizeof(m_appMainCB),
        &m_u64AppMainStack[0],
        sizeof(m_u64AppMainStack),
        osPriorityNormal,
        0,
    };

    osKernelInitialize();               // Initialize CMSIS-RTOS
    osThreadNew(app_main, NULL, &attr); // Create application main thread
    osKernelStart();                    // Start thread execution

    for (;;);

    return 0;
}
