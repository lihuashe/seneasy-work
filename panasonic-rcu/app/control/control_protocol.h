/**
 * @file control_protocol.h
 * @author likeliang (likeliang@seneasy.com)
 * @brief 遥控通信协议
 * @version 0.1
 * @date 2025-03-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef APP_CONTROL_CONTROL_PROTOCOL_H_
#define APP_CONTROL_CONTROL_PROTOCOL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "py32f0xx_hal.h"

/* 固定前导码 */
#define PREAMBLE_FIXED_BITS 0xAA, 0xAA, 0xAA, 0xBB, 0xCC, 0xDD
#define NO_SETTING 0xFF

/* 运行模式设定 */
#define MODE_STOP 0x00
#define MODE_FAST_HEAT 0x02
#define MODE_GLOBAL_HEAT 0x03
#define MODE_HEAT 0x04
#define MODE_VENT 0x06
#define MODE_COOL_DRY 0x08
#define MODE_HOT_DRY 0x0A
#define MODE_STERILIZATION 0x0D // 杀菌
#define MODE_NANOE_X 0x0D       // 离子水
#define MODE_BABY 0x0E

/* 风量设定 */
#define FAN_SPEED_HIGH 0x00
#define FAN_SPEED_LOW 0x01

/* 风向设定 */
#define AIR_DIR_FIXED 0x00
#define AIR_DIR_FIXED_90 0x03
#define AIR_DIR_AUTO 0x06

/* 风种设定 */
#define AIR_TYPE_FOCUS 0x01
#define AIR_TYPE_DIFFUSE 0x02
#define AIR_NO_SETTING 0xFF

/* 时间设定 */
#define TIME_CONTINUOUS 0x23

/* 剩余时间（时）*/
#define REMAIN_HOUR_CONTINUOUS 0x0D

/* 照明设定 */
#define LIGHT_OFF 0x00
#define LIGHT_WARM_ON 0x01
#define LIGHT_COLD_ON 0x02
#define LIGHT_NIGHT_ON 0x03
#define LIGHT_TOGGLE 0x04
#define LIGHT_NIGHT_TOGGLE 0x05

/* WIFI控制 */
#define WIFI_OFF 0x00
#define WIFI_ON 0x01
#define WIFI_RESET_AP 0x02
#define WIFI_RESET_FASTCONNECT 0x03
#define WIFI_RESET_TMALL 0x04
#define WIFI_RESET_BLUETOOTH 0x05

/* 语音控制 */
#define VOICE_OFF 0x00
#define VOICE_ON 0x01

/* 气味感应 */
#define SMELL_TOGGLE 0x00

/* 蓝牙音箱 */
#define BLUETOOTH_MUSIC_TOGGLE 0x00
#define BLUETOOTH_MUSIC_RESET 0x01

/* 过滤网 */
#define FILTER 0x00

/* 机型代码 */
#define MODEL_54BET1C 0x01
#define MODEL_54BE1C 0x02
#define MODEL_20GBY1 0x03
#define MODEL_TB30KL1_JDBNKL1 0x04
#define MODEL_30BKL1C_20GBKL1_MDB20L1 0x05
#define MODEL_23BWN2H 0x06
#define MODEL_23BW2H 0x07
#define MODEL_45BH1C 0x09
#define MODEL_20CSE1C_20GCSE1 0x10
#define MODEL_TB30U2_JDBJU2 0x11
#define MODEL_30BU4C_16GBU4_MDB162 0x12
#define MODEL_JDBQVL2C_TB54VL2C 0x13
#define MODEL_26GBVL2C_54BVL2C 0x14
#define MODEL_26GBV2C_54BVL2C 0x15
#define MODEL_85BH1C 0x1B
#define MODEL_RESERVED 0xFF

// 小于等于1小时，分钟范围0-60，分钟需要是5的倍数
#define TIME_SET_MINUTE(m) ((m / 5) & 0xFF)
// 大于1小时，小时范围1-12，分钟需要是30的倍数
#define TIME_SET(h, m) ((h * 2 + m / 30 + TIME_SET_MINUTE(60) - 2) & 0xFF)

    /* 发送类型 -----------------------------------------------*/
    typedef enum
    {
        RF_SEND_POWER_STOP,
        RF_SEND_MAIN_FUN,
        RF_SEND_BABY_FUN,
        RF_SEND_NANOE_X_FUN,
        RF_SEND_SUB_FUN,
        RF_SEND_AIR_SET,
        RF_SEND_NETWORK_CONFIG,
        RF_SEND_LIGHT,
        RF_SEND_WIFI,
        RF_SEND_WIFI_RESET_AP,
        RF_SEND_VOICE,
        RF_SEND_SMELL,
        RF_SEND_BLUETOOTH_MUSIC,
        RF_SEND_FILTER,
    } RfSendType;

    /* 遥控数据结构体 -----------------------------------------------*/
    typedef struct
    {
        /* 主功能组 */
        uint8_t main_mode;      // 运行模式
        uint8_t fan_speed;      // 风量
        uint8_t air_direction;  // 风向
        uint8_t air_type;       // 风种
        uint8_t time_setting;   // 时间设定
        uint8_t remain_hours;   // 剩余小时
        uint8_t remain_minutes; // 剩余分钟

        /* 辅助功能组 */
        uint8_t light_ctrl; // 照明控制
        uint8_t wifi_ctrl;  // WiFi控制
        uint8_t voice_ctrl; // 语音控制

        /* 系统参数 */
        uint8_t model_type; // 机型代码
        uint8_t reserved;   // 保留
    } __attribute__((packed)) RemoteControlData_A7535;

    typedef struct
    {
        /* 主功能组 */
        uint8_t main_mode;      // 运行模式
        uint8_t fan_speed;      // 风量
        uint8_t air_direction;  // 风向
        uint8_t air_type;       // 风种
        uint8_t time_setting;   // 时间设定
        uint8_t remain_hours;   // 剩余小时
        uint8_t remain_minutes; // 剩余分钟

        /* 辅助功能组 */
        uint8_t light_ctrl; // 照明控制
        uint8_t wifi_ctrl;  // WiFi控制
        uint8_t voice_ctrl; // 语音控制

        /* 系统参数 */
        uint8_t model_type;      // 机型代码
        uint8_t smell;           // 气味
        uint8_t bluetooth_music; // 蓝牙音响
        uint8_t filter;          // 蓝牙音响
        uint8_t reserved[4];     // 保留
    } __attribute__((packed)) RemoteControlData_XN297L, RemoteControlData;
    /* 配对结构体 -----------------------------------------------*/
    typedef struct
    {
        uint8_t preamble_fixed[6];
        uint8_t id[4];          // id
        uint8_t unique_id[4];   // unique id
        uint8_t placeholder[6]; // 占位符

        uint8_t model_type; // 机型代码
        uint8_t reserved;   // 保留
        uint8_t checksum;   // 校验和
    } __attribute__((packed)) PairPayload_A7535;
    typedef struct
    {
        uint8_t preamble_fixed[6];
        uint8_t id[4];          // id
        uint8_t unique_id[4];   // unique id
        uint8_t placeholder[6]; // 占位符

        uint8_t model_type;  // 机型代码
        uint8_t reserved[7]; // 保留
        uint8_t checksum;    // 校验和
    } __attribute__((packed)) PairPayload_XN297L, PairPayload;
    /* 遥控结构体 -----------------------------------------------*/

    typedef struct
    {
        uint8_t preamble_fixed[6];
        uint8_t unique_id[4]; // unique id
        RemoteControlData_A7535 ctrl_data;
        uint8_t checksum; // 校验和
    } __attribute__((packed)) CtrlPayload_A7535;

    typedef struct
    {
        uint8_t preamble_fixed[6];
        uint8_t unique_id[4]; // unique id
        RemoteControlData_XN297L ctrl_data;
        uint8_t checksum;
    } __attribute__((packed)) CtrlPayload_XN297L, CtrlPayload;

    void send_ctrl_data(RfSendType t);
    void send_pair_data();

#ifdef __cplusplus
}
#endif

#endif // APP_CONTROL_CONTROL_PROTOCOL_H_
