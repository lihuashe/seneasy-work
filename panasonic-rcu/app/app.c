/**
 * @file app.c
 * @author likeliang (likeliang@seneasy.com)
 * @brief app_mian
 * @version 0.1
 * @date 2025-03-25
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "app.h"
#include "pm.h"
#include "key_handle.h"
#include "pan216_port.h"
#include "key_driver.h"
#include "time_utils.h"
#include "2_4g_io.h"
#include "led.h"
#include "flash_io.h"
/*============================================================================*
 *                              Local Variables
 *============================================================================*/
static uint32_t sleep_clock_t = 0;
#define NO_OPERATION2SLEEP_TM 1000
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
const app_t app_default = APP_DEFAULT_INFO;
app_t app;

void APP_ErrorHandler();
/*============================================================================*
 *                              Local Functions
 *============================================================================*/

/*============================================================================*
 *                              Global Functions
 *============================================================================*/
void app_init()
{
    // app init
    flash_load();
    key_handle_init();
    rf_2_4g_init();

    sleep_clock_t = clock_time() + NO_OPERATION2SLEEP_TM;
}

void app_try2sleep()
{
    if (key_get_pressed_num() || rf_2_4g_is_busy())
        sleep_clock_t = clock_time();

    if (clock_time_exceed(sleep_clock_t, NO_OPERATION2SLEEP_TM))
    {
        led_all_off();
        pm_entry_sleep();
    }
}

void app_proc()
{
    rf_2_4g_proc();
    app_try2sleep();
}

/**
 * @brief 切换主机需要重启
 *
 * @param ht
 */
void app_change_chipMode(ChipMode cm)
{
    switch (cm)
    {
    case A7535:
        break;
    case XN297L:
        break;

    default:
        return;
    }
    app.chipMode = cm;
    flash_save();
}