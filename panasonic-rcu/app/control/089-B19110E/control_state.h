/**
 * @file control_state.h
 * @author likeliang (likeliang@seneasy.com)
 * @brief
 * @version 0.1
 * @date 2025-03-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef APP_CONTROL_CONTROL_STATE_H_
#define APP_CONTROL_CONTROL_STATE_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include "control_protocol.h"
#include "py32f0xx_hal.h"
#include "led_bar.h"
#include <stdbool.h>

    /* 系统基础配置 ---------------------------------------------------*/

    // 可调节档
    struct adjustable_value
    {
        uint8_t val : 7;
        uint8_t add_sub_flag : 1; // true加，加减标志
    };

    typedef enum
    {
        TIME_00_15_INDEX, // 定时15分钟
        TIME_00_30_INDEX,
        TIME_01_00_INDEX,
        TIME_03_00_INDEX,
        TIME_06_00_INDEX,
        TIME_FF_FF_INDEX, // 连续
    } TimeIndex;
    typedef enum
    {
        AIR_DIR_FIXED_INDEX,
        AIR_DIR_FIXED_90_INDEX,
        AIR_DIR_AUTO_INDEX,
        AIR_DIR_NO_SETTING_INDEX,
    } AirDirIndex;
    typedef enum
    {
        AIR_TYPE_FOCUS_INDEX,
        AIR_TYPE_DIFFUSE_INDEX,
        AIR_TYPE_NO_SETTING_INDEX,
    } AirTypeIndex;

    /* 运行记忆结构体 -----------------------------------------------*/
    typedef struct
    {
        uint8_t duration_index;
        uint8_t air_dir_index;
        uint8_t air_type_index;
    } ModeMemory;
    typedef struct
    {
        ModeMemory heat_l;   // 取暖弱模式记忆
        ModeMemory heat_h;   // 取暖强模式记忆
        ModeMemory hot_dry;  // 热干燥记忆
        ModeMemory cool_dry; // 凉干燥记忆
        ModeMemory vent;     // 换气模式记忆
    } OperationMemory;

/* 默认运行参数 -------------------------------------------------*/
#define MODEL_TYPE MODEL_20GBY1

// 模式初始参数
#define DEFAULT_MODE MODE_HEAT
#define DEFAULT_FAN_SPEED FAN_SPEED_HIGH
#define DEFAULT_AIR_DIR AIR_DIR_FIXED
#define DEFAULT_AIR_TYPE AIR_TYPE_FOCUS

// 各模式初始定时时间（单位：分钟）
#define DEFAULT_HEAT_DURATION TIME_00_15_INDEX     // 取暖模式
#define DEFAULT_HOT_DRY_DURATION TIME_00_15_INDEX  // 热干燥
#define DEFAULT_COOL_DRY_DURATION TIME_00_15_INDEX // 凉干燥
#define DEFAULT_VENT_DURATION TIME_00_15_INDEX     // 换气

// 最大允许运行时间（单位：分钟）
#define MAX_HEAT_DURATION (3 * 60)    // 取暖3小时
#define MAX_HOT_DRY_DURATION (6 * 60) // 热干燥6小时

/* 模式特性配置 -------------------------------------------------*/
// 取暖模式特殊配置
#define HEAT_DEFAULT_AIR_DIR AIR_DIR_FIXED_90_INDEX
#define HEAT_DEFAULT_AIR_TYPE AIR_TYPE_FOCUS_INDEX

// 干燥模式公共配置
#define DRY_DEFAULT_AIR_DIR AIR_DIR_NO_SETTING_INDEX
#define DRY_DEFAULT_AIR_TYPE AIR_TYPE_NO_SETTING_INDEX

// 换气模式风向、风种
#define VENT_DEFAULT_AIR_DIR AIR_DIR_NO_SETTING_INDEX
#define VENT_DEFAULT_AIR_TYPE AIR_TYPE_NO_SETTING_INDEX

// 宝宝模式运行时间、风向、风种
#define BABY_DEFAULT_DURATION TIME_FF_FF_INDEX
#define BABY_DEFAULT_AIR_DIR AIR_DIR_NO_SETTING_INDEX
#define BABY_DEFAULT_AIR_TYPE AIR_TYPE_NO_SETTING_INDEX

// 定时长按重复触发间隔
#define TIME_SET_REPEAT_INTERVAL_MS 400

/* 全局默认配置 -------------------------------------------------*/
#define DEFAULT_CONFIG {     \
    .main_mode = MODE_STOP,  \
    .fan_speed = 0,          \
    .air_direction = 0xFF,   \
    .air_type = 0xFF,        \
    .time_setting = 0,       \
    .remain_hours = 0,       \
    .remain_minutes = 0,     \
    .light_ctrl = 0xFF,      \
    .wifi_ctrl = 0xFF,       \
    .voice_ctrl = 0xFF,      \
    .model_type = 0xFF,      \
    .smell = 0xFF,           \
    .bluetooth_music = 0xFF, \
    .filter = 0xFF,          \
    .reserved = {0xFF, 0xFF, 0xFF, 0xFF}};

#define DEFAULT_MEMORY                                             \
    {                                                              \
        .heat_l = {.air_dir_index = HEAT_DEFAULT_AIR_DIR,          \
                   .air_type_index = HEAT_DEFAULT_AIR_TYPE,        \
                   .duration_index = DEFAULT_HEAT_DURATION},       \
        .heat_h = {.air_dir_index = HEAT_DEFAULT_AIR_DIR,          \
                   .air_type_index = HEAT_DEFAULT_AIR_TYPE,        \
                   .duration_index = DEFAULT_HEAT_DURATION},       \
        .hot_dry = {.air_dir_index = DRY_DEFAULT_AIR_DIR,          \
                    .air_type_index = DRY_DEFAULT_AIR_TYPE,        \
                    .duration_index = DEFAULT_HOT_DRY_DURATION},   \
        .cool_dry = {.air_dir_index = DRY_DEFAULT_AIR_DIR,         \
                     .air_type_index = DRY_DEFAULT_AIR_TYPE,       \
                     .duration_index = DEFAULT_COOL_DRY_DURATION}, \
        .vent = {.air_dir_index = VENT_DEFAULT_AIR_DIR,            \
                 .air_type_index = VENT_DEFAULT_AIR_TYPE,          \
                 .duration_index = DEFAULT_VENT_DURATION }         \
    }

    extern RemoteControlData g_current_ctrl;
    extern const RemoteControlData g_ctrl_stop;

    void send_ctrl_data(RfSendType t);
    void change_mode(uint8_t new_mode);
    void reset_memory();
    void air_dir_change();
    void air_type_change();
    void timing_add_or_sub(bool isAdd, bool isRepeat);
    // void wifi_switch();
    // void light_switch();

#ifdef __cplusplus
}
#endif

#endif // APP_CONTROL_CONTROL_STATE_H_
