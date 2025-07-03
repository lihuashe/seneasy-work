/**
 * @file key_handle.c
 * @author likeliang (likeliang@seneasy.com)
 * @brief 按键事件处理
 * @version 0.1
 * @date 2025-03-24
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "key_handle.h"
#include "key_evt_handle.h"
#include "control_state.h"
#include "pan216_ext.h"
#include "my_log.h"
#include "utility.h"
#include "led.h"
#include "app.h"
#include "time_utils.h"
/*============================================================================*
 *                              Local Variables
 *============================================================================*/

// Key Bindings
#define POWER_K 1
#define TIME_K 4
#define HEAT_K 5
#define VENT_K 6
#define WIND_DIR_K 7
#define HOT_DRY_K 8
#define COOL_DRY_K 9

static void heat_mode_key_cb(u8 key);
static void hot_dry_mode_key_cb(u8 key);
static void cool_dry_mode_key_cb(u8 key);
static void vent_mode_key_cb(u8 key);
static void baby_mode_key_cb(u8 key);
static void wind_direction_key_cb(u8 key);
static void wind_type_key_cb(u8 key);
static void light_key_cb(u8 key);
static void wifi_key_cb(u8 key);
static void pwoer_key_cb(u8 key);
static void reset_key_cb(u8 key);
static void network_config_key_cb(u8 key);
static void smell_key_cb(u8 key);

static void pair_key_cb(u8 key0, u8 key1);

static void change_chipMode_key_cb(u8 key0, u8 key1);

const static KeyEvent click_key_cb_arr[] = {
    KEY_CLICK_EVENT_ITEM(POWER_K, pwoer_key_cb),
    KEY_CLICK_EVENT_ITEM(HEAT_K, heat_mode_key_cb),
    KEY_CLICK_EVENT_ITEM(VENT_K, vent_mode_key_cb),
    KEY_CLICK_EVENT_ITEM(WIND_DIR_K, wind_direction_key_cb),
    KEY_CLICK_EVENT_ITEM(HOT_DRY_K, hot_dry_mode_key_cb),
    KEY_CLICK_EVENT_ITEM(COOL_DRY_K, cool_dry_mode_key_cb),
    KEY_LONG_CLICK_EVENT_ITEM(POWER_K, 3000, reset_key_cb),
    KEY_COMB_LONG_CLICK_EVENT_ITEM(HEAT_K, VENT_K, 3000, pair_key_cb),
    KEY_COMB_LONG_CLICK_EVENT_ITEM(4, 7, 3000, change_chipMode_key_cb),
};

static void power_key_release_cb(u8 key);
static void vent_mode_key_release_cb(u8 key);
static void heat_mode_key_release_cb(u8 key);
static void cool_dry_mode_key_release_cb(u8 key);
static void time_key_release_cb(u8 key);

const static KeyEvent key_release_cb_arr[] = {
    KEY_RELEASE_EVENT_ITEM(POWER_K, power_key_release_cb),
    KEY_RELEASE_EVENT_ITEM(TIME_K, time_key_release_cb),
    KEY_RELEASE_EVENT_ITEM(HEAT_K, heat_mode_key_release_cb),
    KEY_RELEASE_EVENT_ITEM(VENT_K, vent_mode_key_release_cb),
};

static void time_key_cb(u32 time_ms, u8 key);

const static KeyEvent key_repeat_cb_arr[] = {
    KEY_REPEAT_EVENT_ITEM(TIME_K, time_key_cb),
};

/*============================================================================*
 *                              Global Variables
 *============================================================================*/

/*============================================================================*
 *                              Local Functions
 *============================================================================*/

static void vent_mode_key_cb(u8 key)
{
#if __DEBUG__ >= __DEBUG_DEV__
    my_log(">D%d[%s] key:%d\n", __LINE__, __FUNCTION__, key);
#endif
    change_mode(MODE_VENT);
    led_bar_light_single(&g_ledBarMode, 2, LED_LIGHT_TIME); // 点亮换气按键led
}
static void heat_mode_key_cb(u8 key)
{
#if __DEBUG__ >= __DEBUG_DEV__
    my_log(">D%d[%s] key:%d\n", __LINE__, __FUNCTION__, key);
#endif
    change_mode(MODE_HEAT);
    led_bar_light_single(&g_ledBarMode, 1, LED_LIGHT_TIME);
}
static void cool_dry_mode_key_cb(u8 key)
{
#if __DEBUG__ >= __DEBUG_DEV__
    my_log(">D%d[%s] key:%d\n", __LINE__, __FUNCTION__, key);
#endif
    change_mode(MODE_COOL_DRY);
    led_bar_light_single(&g_ledBarMode, 4, LED_LIGHT_TIME);
    send_ctrl_data(RF_SEND_MAIN_FUN);
}
static void hot_dry_mode_key_cb(u8 key)
{
#if __DEBUG__ >= __DEBUG_DEV__
    my_log(">D%d[%s] key:%d\n", __LINE__, __FUNCTION__, key);
#endif
    change_mode(MODE_HOT_DRY);
    led_bar_light_single(&g_ledBarMode, 3, LED_LIGHT_TIME);
    send_ctrl_data(RF_SEND_MAIN_FUN);
}

static void baby_mode_key_cb(u8 key)
{
#if __DEBUG__ >= __DEBUG_DEV__
    my_log(">D%d[%s] key:%d\n", __LINE__, __FUNCTION__, key);
#endif
    change_mode(MODE_BABY);
    led_bar_light_single(&g_ledBarMode, 5, LED_LIGHT_TIME);
    send_ctrl_data(RF_SEND_BABY_FUN);
}

static void smell_key_cb(u8 key)
{
#if __DEBUG__ >= __DEBUG_DEV__
    my_log(">D%d[%s] key:%d\n", __LINE__, __FUNCTION__, key);
#endif
    send_ctrl_data(RF_SEND_SMELL);
}

static void time_key_cb(u32 time_ms, u8 key)
{
#if __DEBUG__ >= __DEBUG_DEV__
    my_log(">D%d[%s] key:%d\n", __LINE__, __FUNCTION__, key);
#endif
    if (g_current_ctrl.main_mode == MODE_STOP ||
        g_current_ctrl.main_mode == MODE_BABY)
        return;
    static uint32_t last_time_ms;
    if (time_ms == 0)
    {
        last_time_ms = 0;
        timing_change(false);
    }
    else if (time_ms >= 1000 && time_ms - last_time_ms > TIME_SET_REPEAT_INTERVAL_MS)
    {
        timing_change(true);
        last_time_ms = time_ms;
    }
}

static void wind_direction_key_cb(u8 key)
{
#if __DEBUG__ >= __DEBUG_DEV__
    my_log(">D%d[%s] key:%d\n", __LINE__, __FUNCTION__, key);
#endif
    if (g_current_ctrl.main_mode == MODE_STOP ||
        g_current_ctrl.main_mode == MODE_VENT ||
        g_current_ctrl.main_mode == MODE_BABY)
        return;
    air_dir_change();
    send_ctrl_data(RF_SEND_AIR_SET);
}

static void wind_type_key_cb(u8 key)
{
#if __DEBUG__ >= __DEBUG_DEV__
    my_log(">D%d[%s] key:%d\n", __LINE__, __FUNCTION__, key);
#endif
    if (g_current_ctrl.main_mode == MODE_STOP ||
        g_current_ctrl.main_mode == MODE_VENT ||
        g_current_ctrl.main_mode == MODE_BABY)
        return;
    air_type_change();
    send_ctrl_data(RF_SEND_AIR_SET);
}

static void light_key_cb(u8 key)
{
#if __DEBUG__ >= __DEBUG_DEV__
    my_log(">D%d[%s] key:%d\n", __LINE__, __FUNCTION__, key);
#endif
    send_ctrl_data(RF_SEND_LIGHT);
}

static void wifi_key_cb(u8 key)
{
#if __DEBUG__ >= __DEBUG_DEV__
    my_log(">D%d[%s] key:%d\n", __LINE__, __FUNCTION__, key);
#endif
    send_ctrl_data(RF_SEND_WIFI);
}

static void pwoer_key_cb(u8 key)
{
#if __DEBUG__ >= __DEBUG_DEV__
    my_log(">D%d[%s] key:%d\n", __LINE__, __FUNCTION__, key);
#endif
    change_mode(MODE_STOP);
}

static void pair_key_cb(u8 key0, u8 key1)
{
#if __DEBUG__ >= __DEBUG_DEV__
    my_log(">D%d[%s]\n", __LINE__, __FUNCTION__, key0, key1);
#endif
    send_pair_data();
}

static void change_chipMode_key_cb(u8 key0, u8 key1)
{
    if (app.chipMode == A7535)
    {
        app_change_chipMode(XN297L);
        led_all_off();
        led_bar_all_on(&g_ledBarTime, 1000);
    }
    else if (app.chipMode == XN297L)
    {
        app_change_chipMode(A7535);
        led_all_off();
        led_bar_light_l2r(&g_ledBarTime, 3, 1000);
    }
    else
    {
        return;
    }

    HAL_Delay(1000);
    HAL_NVIC_SystemReset();
    while (1)
        ;
}

// 长按电源清除记忆
static void reset_key_cb(u8 key)
{
#if __DEBUG__ >= __DEBUG_DEV__
    my_log(">D%d[%s]\n", __LINE__, __FUNCTION__, key);
#endif
    reset_memory();
    if (led_bar_is_light(&g_ledBarTime))
    {
        led_bar_light_single(&g_ledBarTime, 1, LED_LIGHT_TIME);
    }
    if (led_bar_is_light(&g_ledBarFanSpeed))
    {
        led_bar_light_single(&g_ledBarFanSpeed, 1, LED_LIGHT_TIME);
    }
}

// 开启配网
static void network_config_key_cb(u8 key)
{
#if __DEBUG__ >= __DEBUG_DEV__
    my_log(">D%d[%s]\n", __LINE__, __FUNCTION__, key);
#endif
    send_ctrl_data(RF_SEND_NETWORK_CONFIG);
}

// release事件

// 待机
static void power_key_release_cb(u8 key)
{
    led_all_off();
    send_ctrl_data(RF_SEND_POWER_STOP);
}
// 松开发码
static void vent_mode_key_release_cb(u8 key)
{
    send_ctrl_data(RF_SEND_MAIN_FUN);
}

static void heat_mode_key_release_cb(u8 key)
{
    send_ctrl_data(RF_SEND_MAIN_FUN);
}

static void cool_dry_mode_key_release_cb(u8 key)
{
    send_ctrl_data(RF_SEND_MAIN_FUN);
}

static void time_key_release_cb(u8 key)
{
#if __DEBUG__ >= __DEBUG_DEV__
    my_log(">D%d[%s] key:%d\n", __LINE__, __FUNCTION__, key);
#endif
    if (g_current_ctrl.main_mode == MODE_STOP ||
        g_current_ctrl.main_mode == MODE_BABY)
        return;
    send_ctrl_data(RF_SEND_MAIN_FUN);
}
/*============================================================================*
 *                              Global Functions
 *============================================================================*/
void key_handle_init()
{
    key_evt_reset_all();
    set_click_key_map(click_key_cb_arr, ARRAY_SIZE(click_key_cb_arr));
    set_release_key_map(key_release_cb_arr, ARRAY_SIZE(key_release_cb_arr));
    set_repeat_key_map(key_repeat_cb_arr, ARRAY_SIZE(key_repeat_cb_arr));
    u8 joined_key_arr[] = {1, 2, 3};
    kb_set_joined_key(joined_key_arr, sizeof(joined_key_arr));
}