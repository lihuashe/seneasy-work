/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     keyscan_driver.c
* @brief    keyscan module driver
* @details
* @author   barry_bian
* @date     2020-02-25
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <board.h>
#include <string.h>
#include <trace.h>
#include <os_msg.h>
#include <app_msg.h>
#include <os_timer.h>
#include <keyscan_basic_driver.h>
#include <keyscan_driver.h>
#include <rtl876x_rcc.h>
#include <rtl876x_keyscan.h>
#include <swtimer.h>
#include <rtl876x_pinmux.h>
#include <rtl876x_nvic.h>
#include <app_task.h>
#include <app_section.h>

/*============================================================================*
 *                              Local Variables
 *============================================================================*/
TimerHandle_t keyscan_timer;

/*============================================================================*
 *                              Global Variables
 *============================================================================*/
#if FEATURE_SUPPORT_DEEP_SLEEP_STATE
bool allow_to_enter_dss_flag = false;
#endif

/*============================================================================*
 *                              Functions Declaration
 *============================================================================*/
#define keyscan_interrupt_handler Keyscan_Handler



/*============================================================================*
 *                              Local Functions
 *============================================================================*/


#if FEATURE_SUPPORT_DEEP_SLEEP_STATE

static void keyscan_enable_wakeup_io_config(uint8_t is_debounce)
{
    /* @note: no key is pressed, use PAD wake up function with debounce,
    but pad debunce time should be smaller than ble connect interval */
#ifdef KEY_IO0
    System_WakeUpPinEnable(KEY_IO0, PAD_WAKEUP_POL_LOW, is_debounce, 0x08);//PAD_WK_DEBOUNCE_ENABLE
#endif
#ifdef KEY_IO1
    System_WakeUpPinEnable(KEY_IO1, PAD_WAKEUP_POL_LOW, is_debounce, 0x08);
#endif
#ifdef KEY_IO2
    System_WakeUpPinEnable(KEY_IO2, PAD_WAKEUP_POL_LOW, is_debounce, 0x08);
#endif
#ifdef KEY_IO3
    System_WakeUpPinEnable(KEY_IO3, PAD_WAKEUP_POL_LOW, is_debounce, 0x08);
#endif

}

static void keyscan_disable_wakeup_io_config(void)
{
    /* @note: no key is pressed, use PAD wake up function with debounce,
    but pad debunce time should be smaller than ble connect interval */
#ifdef KEY_IO0
    System_WakeUpPinDisable(KEY_IO0);
#endif
#ifdef KEY_IO1
    System_WakeUpPinDisable(KEY_IO1);
#endif
#ifdef KEY_IO2
    System_WakeUpPinDisable(KEY_IO2);
#endif
#ifdef KEY_IO3
    System_WakeUpPinDisable(KEY_IO3);
#endif
}

#endif

/*============================================================================*
 *                              Global Functions
 *============================================================================*/


#if FEATURE_SUPPORT_KEY_LONG_PRESS_PROTECT
/******************************************************************
 * @brief    key long pressed handler
 * @param    none
 * @return   none
 * @retval   void
 */
void keyscan_long_press_handler(uint8_t long_pressed_key_row_pin)
{
    os_timer_stop(&keyscan_timer);
    KeyScan_DeInit(KEYSCAN);
    keyscan_global_data.is_key_long_pressed = true;
    keyscan_global_data.long_pressed_key_row_pin = long_pressed_key_row_pin;
}
#endif

