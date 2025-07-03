/**
 * @file led.c
 * @author likeliang (likeliang@seneasy.com)
 * @brief 定义各种状态对应的led闪烁事件
 * @version 0.1
 * @date 2025-03-25
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "led.h"
#include "led_evt.h"
#include "led_bar.h"
/*============================================================================*
 *                              Local Variables
 *============================================================================*/

/*============================================================================*
 *                              Global Variables
 *============================================================================*/

const LedConfig LED_EVT_LIGHT_FOREVER = LED_EVT_ITEM(1000, 0, LED_EVT_FOREVER, 0);
const LedConfig LED_EVT_BLINK_1Hz_FOREVER = LED_EVT_ITEM(500, 500, LED_EVT_FOREVER, 0);
const LedConfig LED_EVT_LIGHT_3S = LED_EVT_ITEM(3000, 0, 1, 0);

LedBar g_ledBarTime = {
    .led = {4, 1, 11, 9, 5, 2},
    .num = 6,
};

LedBar g_ledBarMode = {
    .led = {15, 16, 17, 18},
    .num = 4,
};

LedBar g_ledBarFanSpeed = {
    .led = {8, 12},
    .num = 2,
};

/*============================================================================*
 *                              Local Functions
 *============================================================================*/

/*============================================================================*
 *                              Global Functions
 *============================================================================*/

void led_all_off()
{
    led_bar_all_off(&g_ledBarMode);
    led_bar_all_off(&g_ledBarTime);
    led_bar_all_off(&g_ledBarFanSpeed);
}