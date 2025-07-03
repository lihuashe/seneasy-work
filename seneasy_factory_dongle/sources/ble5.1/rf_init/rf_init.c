/**
 * @file     rf_init.c
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

#include <stdint.h>
#include <stdbool.h>

#include "RegMXD2670.h"
#include "err_def.h"
#include "hw_rf.h"
#include "hal_rf.h"

/**
 * @brief  rf init.
 */
EN_ERR_STA_T rf_init(void)
{
    EN_ERR_STA_T enRet;

    // rf init
    rom_hal_rf_cali_init();
    rom_hal_rf_tx_cali();

    enRet = rom_hal_rf_rx_cali();
    ERR_RETURN_IF_ERROR(enRet);

    rom_hal_rf_rx_dc_cal_sweep();
    rom_hal_rf_tx_power_table_init((uint8_t *)g_pu8TxGainDefaultTable);
    rom_hal_rf_tx_power_set(TX_0_DBM);
    rom_hw_rf_tx_pa_h2_config(0x0B);

    return ERR_STA_OK;
}
