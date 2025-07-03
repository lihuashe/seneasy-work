/**
 * @file     ble_init.c
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2023/5/26
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

#include "ble_config.h"

#include "RegMXD2670.h"
#include "err_def.h"
#include "hw_pmu.h"
#include "hw_crg.h"
#include "hw_sys_ctrl.h"
#include "hal_rf.h"

#include "llc_llp_interface.h"
#include "llp_hw.h"
#include "llp.h"
#include "llp_sleep.h"

#include "ble_impl.h"

#include "type_def.h"
#include "app_cfg.h"

#include "rf_init.h"
#include "patch_ble.h"

/**
 * @brief  GLOBAL FUNCTIONS
 */
extern void ble_interface_init(void);
extern bool ble_controller_init(void);
extern bool ble_host_init(void);

/**
 * @brief  Link Layer Controller Interrupt Handler.
 */
void LLC_IRQ_Handler(void)
{
    rom_llp_hw_int_irq_handler();
}

/**
 * @brief  ble init.
 */
EN_ERR_STA_T ble_init(void)
{
    // rf init
    rf_init();

    // llc interrupt init
    rom_hw_sys_ctrl_enable_peri_int(SYS_CTRL_CP, LLC_IRQ);
    NVIC_ClearPendingIRQ(LLC_IRQ);
    NVIC_SetPriority(LLC_IRQ, 0);
    NVIC_EnableIRQ(LLC_IRQ);

    // llc wakeup init
    rom_hw_pmu_set_wakeup_source(LUT_INDEX_CP_LLC, LUT_TRIG_ID_OTHER, LUT_TRIG_ID_LLC,
                                 LUT_ACT_PD_SYS_ON | LUT_ACT_DCXO_HCLK_ON | LUT_ACT_CP_IRQ_EN);

    // ble interface init
    ble_interface_init();

    // ble controller init
    if (!ble_controller_init())
    {
        PRINTF("ble_controller_init error.\n");
        return ERR_STA_ERROR;
    }

    // host init
    if (!ble_host_init())
    {
        PRINTF("ble_host_init error.\n");
        return ERR_STA_ERROR;
    }

    // patch ble init.
    patch_ble_init();

#if ((EXT_ADV_NUM) > 0) || (EXT_SCAN_ENABLE) || (EXT_INITIATOR_ENABLE)
    ble_extended_init();
#endif

    return ERR_STA_OK;
}

bool ble_init_is_system_sleep_allowed(void)
{
    return rom_llp_is_system_sleep_allowed();
}

void ble_init_on_system_wakeup_by_llc(void)
{
    rom_llp_sleep_on_system_wakeup_by_llc();
}
