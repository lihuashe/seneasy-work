/**
 * @file     app_main.c
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2022/02/13
 * @history
 * @note     ble_peripheral cp app main file
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
#include <string.h>
#include <stdbool.h>

#include "RegMXD2670.h"
#include "err_def.h"
#include "hw_crg.h"
#include "hw_gpio.h"
#include "hw_pmu.h"
#include "hw_uart.h"
#include "hw_sys_ctrl.h"
#include "hw_trng.h"
#include "hal_pmu.h"
#include "hal_rf.h"

#include "app_cfg.h"
#include "boards.h"

#include "rand.h"
#include "utility.h"
#include "lpwr_ctrl.h"
#include "sw_timer.h"

#include "cmsis_os2.h"
#include "rtx_lib.h"

#include "rom_callback.h"
#include "err_debug.h"
#include "debug.h"
#include "mlog.h"
#include "mlog_transport_uart.h"

#include "ble_init.h"
#include "app_dongle.h"

/**
 * @brief  LOCAL VARIABLES
 */
#if APP_DEBUG_ENABLED
static uint8_t      m_u8DebugBuf[4096] __attribute__((aligned(4)));
static stDebugBuf_t m_stDebugBuf __attribute__((aligned(4)));
#endif

/**
 * @brief  Extern functions.
 */
extern void idle_task_init(void);

/**
 * @brief  System clock init, Control clock gate.
 */
static void system_clock_init(void)
{
    rom_hw_crg_enable_clk_gate(CRG_AHB4_CLK_GATE);
    rom_hw_crg_enable_clk_gate(CRG_FD_CLK_GATE);
    rom_hw_crg_enable_clk_gate(CRG_LO_CLK_GATE);
    rom_hw_crg_enable_clk_gate(CRG_SDM_CLK_GATE);
    rom_hw_crg_enable_clk_gate(CRG_MDM_CLK_GATE);
    rom_hw_crg_enable_clk_gate(CRG_LLC_HCLK_GATE);
    rom_hw_crg_enable_clk_gate(CRG_LLC_LCLK_GATE);
    rom_hw_crg_enable_clk_gate(CRG_CPTO0_CLK_GATE);
    rom_hw_crg_enable_clk_gate(CRG_CPTO1_CLK_GATE);
    rom_hw_crg_enable_clk_gate(CRG_CALI_CLK_GATE);
    rom_hw_crg_enable_clk_gate(CRG_TRNG_CLK_GATE);
    rom_hw_crg_enable_clk_gate(CRG_USB_CLK_GATE);
}

/**
 * @brief  All peripheral related please init here
 */
static void peripheral_init(void)
{
    /* Disable CP all Peripheral interrupt. */
    rom_hw_sys_ctrl_peri_int_ctrl(SYS_CTRL_CP, 0, 0);

    // Rand init
    rom_rand_init(rom_hw_trng_gen_32bit());

    // App timer init
    sw_timer_init();

#if APP_DEBUG_ENABLED
    m_stDebugBuf.pu8buf    = m_u8DebugBuf;
    m_stDebugBuf.u16MaxLen = sizeof(m_u8DebugBuf);
    debug_init(GPIO_PORT_CP_APP_DEBUG_TX, GPIO_PIN_CP_APP_DEBUG_TX, UART_HANDLE_CP_APP_DEBUG, UART_BPS_CP_APP_DEBUG,
                   &m_stDebugBuf);
    PRINTF("[%s T %s]ble peripheral project(cp) start to work.\n", __DATE__, __TIME__);
#endif

#if MLOG_DEBUG_ENABLED
    mlog_transport_uart_init(UART_HANDLE_CP_MLOG, GPIO_PORT_CP_MLOG_TX, GPIO_PIN_CP_MLOG_TX, UART_BPS_CP_MLOG);

    MLOG0D(0x80c0, "MLOG0D test");
    MLOGI(0x80c1, "MLOGI test : 0x%04x", 0x1234);
    MLOG32I(0x80c2, "MLOG32I test : 0x%08x", 0x12345678);
    MLOG88W(0x80c3, "MLOG88W test : 0x%02x : 0x%02x", 0x12, 0x34);
    MLOG1616W(0x80c4, "MLOG1616W test : 0x%04x : 0x%04x", 0x1234, 0x5678);
    MLOG3216W(0x80c5, "MLOG3216W test : 0x%08x : 0x%04x", 0x12345678, 0x4321);
    MLOG3232W(0x80c6, "MLOG3232W test : 0x%08x : 0x%08x", 0x12345678, 0x87654321);
    MLOG1688E(0x80c7, "MLOG1688E test : 0x%04x : 0x%02x : 0x%02x", 0x1234, 0x56, 0x78);
    MLOG161616E(0x80c8, "MLOG161616E test : 0x%04x : 0x%04x : 0x%04x", 0x1234, 0x5678, 0xabcd);
    MLOG3288E(0x80c9, "MLOG3288E test : 0x%08x : 0x%02x : 0x%02x", 0x12345678, 0xab, 0xcd);
    MLOG321616E(0x80ca, "MLOG321616E test : 0x%08x : 0x%04x : 0x%04x", 0x12345678, 0xaabb, 0xccdd);

    uint8_t pu8Data[] = {0x12, 0x34, 0x56, 0x78, 0x90};
    MLOGXD(0x80cb, "MLOGXD test:", pu8Data, sizeof(pu8Data));
#endif

    err_debug_init();
}

/**
 * @brief  ble service init.
 * @param  pu8Data: Pointer to received data from ble.
 * @param  u16DataLen: Data length.
 */
static void ble_mus_data_handler(uint16_t u16ConnectHandle, uint8_t *pu8Data, uint16_t u16DataLen)
{
    uint16_t i;

    PRINTF("[BLE_RX][%d]: ", u16DataLen);
    for (i = 0; i < u16DataLen; i++)
    {
        PRINTF("%02X ", pu8Data[i]);
    }
    PRINTF("\n");
}


/**
 * @brief  Check if the system can get into sleep
 * @return false(0) - System can not get into sleep.
 * @return true(1)  - System can get into sleep.
 */
static bool lpwr_before_sleep(void)
{
    // Check ble.
    if (!ble_init_is_system_sleep_allowed())
    {
        return false;
    }

// Check app debug(uart).
#if APP_DEBUG_ENABLED
    if (!debug_print_complete())
    {
        return false;
    }

    // Set uart pin to impedance
    debug_deinit();
#endif

// Check mlog(uart).
#if MLOG_DEBUG_ENABLED
    /* Set uart pin to impedance */
    mlog_transport_uart_deinit();
#endif

    OS_Tick_Disable();

    return true;
}

/**
 * @brief  CPU after wakeup from sleep func
 */
static void lpwr_after_wakeup(void)
{
#if APP_DEBUG_ENABLED
    debug_reinit();
    // PRINTF("A");
#endif

#if MLOG_DEBUG_ENABLED
    // Re-init mlog
    mlog_transport_uart_reinit();
#endif

    OS_Tick_Enable();
}

/**
 * @brief  PMU IRQ Handler.
 */
void PMU_IRQ_Handler(void)
{
    uint32_t u32IntFlag;

    rom_hw_pmu_get_interrupt_flag(&u32IntFlag);
    rom_hw_pmu_clear_interrupt_flag(u32IntFlag);

    if (u32IntFlag & (1 << LUT_INDEX_CP_LLC))
    {
        ble_init_on_system_wakeup_by_llc();
    }
}

/**
 * @brief  Low power init
 * @param  enMode: System work mode, @ref EN_LPWR_MODE_SEL_T.
 */
static void system_lpwr_init(EN_LPWR_MODE_SEL_T enMode)
{
    // Enable pmu interrupt. S40M1 Needed.
    rom_hw_sys_ctrl_enable_peri_int(SYS_CTRL_CP, PMU_IRQ);
    NVIC_ClearPendingIRQ(PMU_IRQ);
    NVIC_SetPriority(PMU_IRQ, 0);
    NVIC_EnableIRQ(PMU_IRQ);

    // Low power init.
    lpwr_ctrl_init(enMode, lpwr_before_sleep, lpwr_after_wakeup);
}

/**
 * @brief  cp app main task.
 * @param  argument
 */
void app_main(void *argument)
{
    system_clock_init();
    peripheral_init();
    
    app_dongle_init();

    idle_task_init();
    system_lpwr_init(LPWR_MODE_ACTIVE);

    osThreadTerminate(osThreadGetId());
}
