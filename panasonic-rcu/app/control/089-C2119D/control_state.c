/**
 * @file control_state.c
 * @author likeliang (likeliang@seneasy.com)
 * @brief
 * @version 0.1
 * @date 2025-03-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "control_state.h"
#include "control_protocol.h"
#include "pan216_ext.h"
#include "flash_io.h"
#include <string.h>
#include "led.h"
#include "app.h"
/*============================================================================*
 *                              Local Variables
 *============================================================================*/
const static OperationMemory runtime_memory_default = DEFAULT_MEMORY;
static OperationMemory runtime_memory = DEFAULT_MEMORY;

static uint8_t *duration_index;
static uint8_t *air_dir_index;
static uint8_t *air_type_index;

const static uint8_t duration_map[] = {
    TIME_SET_MINUTE(15),
    TIME_SET_MINUTE(30),
    TIME_SET(1, 0),
    TIME_SET(3, 0),
    TIME_SET(6, 0),
    TIME_CONTINUOUS,
};
const static uint8_t air_dir_map[] = {
    AIR_DIR_FIXED,
    AIR_DIR_FIXED_90,
    AIR_DIR_AUTO,
    AIR_NO_SETTING,
};
const static uint8_t air_type_map[] = {
    AIR_TYPE_FOCUS,
    AIR_TYPE_DIFFUSE,
    AIR_NO_SETTING,
};
/*============================================================================*
 *                              Global Variables
 *============================================================================*/

/* 当前设备状态 */
const RemoteControlData g_ctrl_stop = DEFAULT_CONFIG; // 待机
RemoteControlData g_current_ctrl = DEFAULT_CONFIG;

/*============================================================================*
 *                              Local Functions
 *============================================================================*/

/**
 * @brief
 *
 * @param dest
 * @param[out] h
 * @param[out] m
 */
static void time_setting_decode(uint8_t dest, uint8_t *h, uint8_t *m)
{
    if (dest == TIME_CONTINUOUS)
    {
        *h = 0x0D;
        *m = 0;
    }
    else if (dest < TIME_SET_MINUTE(60))
    {
        *m = dest * 5;
        *h = 0;
    }
    else
    {
        uint8_t half_hour_cnt = dest - TIME_SET_MINUTE(60); // 得到half_hour_cnt个30分钟
        *h = half_hour_cnt / 2 + 1;
        *m = half_hour_cnt % 2;
    }
}
/**
 * @brief
 *
 * @param[out] dest
 * @param h 小时范围0-12，分钟需要是30的倍数
 * @param m 分钟范围0-60，分钟需要是5的倍数
 */
static void time_setting_encode(uint8_t *dest, uint8_t h, uint8_t m)
{
    if (h == 0)
    {
        *dest = TIME_SET_MINUTE(m);
    }
    else
    {
        *dest = TIME_SET(h, m);
    }
}
/*============================================================================*
 *                              Global Functions
 *============================================================================*/

/* 模式切换处理 */
void change_mode(uint8_t new_mode)
{
    switch (new_mode)
    {
    case MODE_STOP:
        g_current_ctrl.main_mode = new_mode;
        return;
    case MODE_HEAT:
        if (g_current_ctrl.main_mode != MODE_HEAT || g_current_ctrl.fan_speed == FAN_SPEED_LOW)
        {
            led_bar_light_single(&g_ledBarFanSpeed, 1, LED_LIGHT_TIME);
            g_current_ctrl.fan_speed = FAN_SPEED_HIGH;
            duration_index = &runtime_memory.heat_h.duration_index;
            air_dir_index = &runtime_memory.heat_h.air_dir_index;
            air_type_index = &runtime_memory.heat_h.air_type_index;
        }
        else if (g_current_ctrl.fan_speed == FAN_SPEED_HIGH)
        {
            led_bar_light_single(&g_ledBarFanSpeed, 2, LED_LIGHT_TIME);
            g_current_ctrl.fan_speed = FAN_SPEED_LOW;
            duration_index = &runtime_memory.heat_l.duration_index;
            air_dir_index = &runtime_memory.heat_l.air_dir_index;
            air_type_index = &runtime_memory.heat_l.air_type_index;
        }
        break;

    case MODE_HOT_DRY:
        duration_index = &runtime_memory.hot_dry.duration_index;
        air_dir_index = &runtime_memory.hot_dry.air_dir_index;
        air_type_index = &runtime_memory.hot_dry.air_type_index;
        break;

    case MODE_COOL_DRY:
        duration_index = &runtime_memory.cool_dry.duration_index;
        air_dir_index = &runtime_memory.cool_dry.air_dir_index;
        air_type_index = &runtime_memory.cool_dry.air_type_index;
        break;

    case MODE_VENT:
        if (g_current_ctrl.main_mode != MODE_VENT || g_current_ctrl.fan_speed == FAN_SPEED_LOW)
        {
            duration_index = &runtime_memory.vent_h.duration_index;
            air_dir_index = &runtime_memory.vent_h.air_dir_index;
            air_type_index = &runtime_memory.vent_h.air_type_index;
            led_bar_light_single(&g_ledBarFanSpeed, 1, LED_LIGHT_TIME);
            g_current_ctrl.fan_speed = FAN_SPEED_HIGH;
        }
        else if (g_current_ctrl.fan_speed == FAN_SPEED_HIGH)
        {
            duration_index = &runtime_memory.vent_l.duration_index;
            air_dir_index = &runtime_memory.vent_l.air_dir_index;
            air_type_index = &runtime_memory.vent_l.air_type_index;
            led_bar_light_single(&g_ledBarFanSpeed, 2, LED_LIGHT_TIME);
            g_current_ctrl.fan_speed = FAN_SPEED_LOW;
        }
        break;
    case MODE_BABY:
        led_bar_all_off(&g_ledBarTime);
        led_bar_all_off(&g_ledBarFanSpeed);
        g_current_ctrl.main_mode = new_mode;
        return;
    default:
        return;
    }
    g_current_ctrl.main_mode = new_mode;
    g_current_ctrl.air_direction = air_dir_map[*air_dir_index];
    g_current_ctrl.air_type = air_type_map[*air_type_index];
    g_current_ctrl.time_setting = duration_map[*duration_index];

    if (g_current_ctrl.main_mode != MODE_HEAT && g_current_ctrl.main_mode != MODE_VENT)
    {
        g_current_ctrl.fan_speed = NO_SETTING;
        led_bar_all_off(&g_ledBarFanSpeed);
    }

    time_setting_decode(g_current_ctrl.time_setting, &g_current_ctrl.remain_hours, &g_current_ctrl.remain_minutes);

    led_bar_light_single(&g_ledBarTime, (*duration_index) + 1, LED_LIGHT_TIME);
}

void timing_add_or_sub(bool isAdd, bool isRepeat)
{
    uint8_t index;
    switch (g_current_ctrl.main_mode)
    {
    case MODE_HEAT:
        if (g_current_ctrl.fan_speed == FAN_SPEED_HIGH)
        {
            led_bar_light_single(&g_ledBarFanSpeed, 1, LED_LIGHT_TIME);
        }
        else if (g_current_ctrl.fan_speed == FAN_SPEED_LOW)
        {
            led_bar_light_single(&g_ledBarFanSpeed, 2, LED_LIGHT_TIME);
        }

        led_bar_light_single(&g_ledBarMode, 1, LED_LIGHT_TIME);
        index = TIME_03_00_INDEX;
        break;

    case MODE_HOT_DRY:
        led_bar_light_single(&g_ledBarMode, 3, LED_LIGHT_TIME);
        index = TIME_06_00_INDEX;
        break;

    case MODE_COOL_DRY:
        led_bar_light_single(&g_ledBarMode, 4, LED_LIGHT_TIME);
        index = TIME_FF_FF_INDEX;
        break;

    case MODE_VENT:
        if (g_current_ctrl.fan_speed == FAN_SPEED_HIGH)
        {
            led_bar_light_single(&g_ledBarFanSpeed, 1, LED_LIGHT_TIME);
        }
        else if (g_current_ctrl.fan_speed == FAN_SPEED_LOW)
        {
            led_bar_light_single(&g_ledBarFanSpeed, 2, LED_LIGHT_TIME);
        }
        led_bar_light_single(&g_ledBarMode, 1, LED_LIGHT_TIME);
        index = TIME_FF_FF_INDEX;
        break;
    }

    if (isAdd)
    {
        if (isRepeat)
        {
            if ((*duration_index) < index)
            {
                (*duration_index)++;
            }
        }
        else
        {
            (*duration_index)++;
            (*duration_index) %= index + 1;
        }
    }
    else
    {
        if (isRepeat)
        {
            if ((*duration_index) > 0)
            {
                (*duration_index)--;
            }
        }
        else
        {
            if ((*duration_index) == 0)
                (*duration_index) = index;
            else
                (*duration_index)--;
        }
    }

    g_current_ctrl.time_setting = duration_map[*duration_index];

    time_setting_decode(g_current_ctrl.time_setting, &g_current_ctrl.remain_hours, &g_current_ctrl.remain_minutes);
    led_bar_light_single(&g_ledBarTime, (*duration_index) + 1, LED_LIGHT_TIME);
}

/* 定时设置处理 */
void timing_change(bool repeat_f)
{
    uint8_t index;
    switch (g_current_ctrl.main_mode)
    {
    case MODE_HEAT:
        index = TIME_03_00_INDEX;
        break;

    case MODE_HOT_DRY:
        index = TIME_06_00_INDEX;
        break;

    case MODE_COOL_DRY:
        index = TIME_FF_FF_INDEX;
        break;

    case MODE_VENT:
        index = TIME_FF_FF_INDEX;
        break;
    }
    if (repeat_f && (*duration_index) < index)
    {
        (*duration_index)++;
    }
    else if (!repeat_f)
    {
        (*duration_index)++;
        (*duration_index) %= (index + 1);
    }

    g_current_ctrl.time_setting = duration_map[*duration_index];

    time_setting_decode(g_current_ctrl.time_setting, &g_current_ctrl.remain_hours, &g_current_ctrl.remain_minutes);
    led_bar_light_single(&g_ledBarTime, (*duration_index) + 1, LED_LIGHT_TIME);
}

/* 记忆功能复位 */
void reset_memory()
{
    runtime_memory = runtime_memory_default;
    g_current_ctrl.time_setting = duration_map[*duration_index];
    g_current_ctrl.air_direction = air_dir_map[*air_dir_index];
    g_current_ctrl.air_type = air_type_map[*air_type_index];
}
/**
 * @brief 风向切换
 *
 */
void air_dir_change()
{
    // *air_dir_index = (*air_dir_index + 1) % 3;
    // 根据样机修改只在AIR_DIR_FIXED和AIR_DIR_AUTO之间切换
    if (*air_dir_index == AIR_DIR_FIXED_INDEX || *air_dir_index == AIR_DIR_FIXED_90_INDEX)
        *air_dir_index = AIR_DIR_AUTO_INDEX;
    else if (*air_dir_index == AIR_DIR_AUTO_INDEX)
        *air_dir_index = AIR_DIR_FIXED_INDEX;
    g_current_ctrl.air_direction = air_dir_map[*air_dir_index];
}
/**
 * @brief 风种切换
 *
 */
void air_type_change()
{
    *air_type_index = (*air_type_index + 1) % 2;
    g_current_ctrl.air_type = air_type_map[*air_type_index];
}
