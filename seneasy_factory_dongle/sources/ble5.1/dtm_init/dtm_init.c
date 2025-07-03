/**
 * @file     dtm_init.c
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

#include "RegMXD2670.h"
#include "err_def.h"
#include "hw_pmu.h"
#include "hw_crg.h"
#include "hw_sys_ctrl.h"
#include "hal_rf.h"

#include "llc_llp_interface.h"
#include "llp_hw.h"
#include "llp_dtm.h"
#include "llp.h"
#include "llc_dtm.h"

#include "ble_impl.h"

#include "app_cfg.h"

#include "rf_init.h"
#include "patch_ble.h"

/**
 * @brief  LOCAL VARIABLES
 */
static uint8_t m_au8DtmBuf[256] __attribute__((aligned(4)));

/**
 * @brief  Link Layer Controller Interrupt Handler.
 */
#ifdef DTM_PROJECT
void LLC_IRQ_Handler(void)
{
    rom_llp_hw_int_irq_handler();
}
#endif

/**
 * @brief  dtm init.
 */
EN_ERR_STA_T dtm_init(void)
{
    EN_LLP_ERR_T enErr;

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

    // llp & llc interface init
    rom_llc_dtm_set_start_tx_fragment_callback(rom_llp_dtm_start_tx_fragment);
    rom_llc_dtm_set_start_rx_fragment_callback(rom_llp_dtm_start_rx_fragment);
    rom_llc_dtm_set_stop_fragment_callback(rom_llp_dtm_stop_fragment);
    rom_llp_dtm_set_tx_end_report_callback(rom_llc_dtm_on_llp_tx_end);
    rom_llp_dtm_set_rx_end_report_callback(rom_llc_dtm_on_llp_rx_end);
    rom_llp_set_rf_tx_calib_callback(rom_hal_rf_set_modgain_cail_correction);
    rom_llp_set_rf_rx_calib_callback(rom_hal_rf_rx_dc_cal_offset_update);

    // patch ble init.
    patch_ble_init();

    // llc_dtm init
    enErr = rom_llc_dtm_init(m_au8DtmBuf, sizeof(m_au8DtmBuf));
    if (LLP_ERR_NONE != enErr)
    {
        return ERR_STA_ERROR;
    }

    // llp_dtm init
    rom_llp_dtm_init();

    return ERR_STA_OK;
}
