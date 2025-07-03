#ifndef __FUNCTION__H__
#define __FUNCTION__H__

// 头文件声明
#include "driver.h"

// 软件号版本选择
#define SRC_1116_6234           0x6234
#define SRC_1116_6242           0x6242
#define SRC_1116_6254           0x6254
#define NOW_SOFT_VERSION        SRC_1116_6254 

// 进行版本选择
#define JUMOO_VERSION           0x01
#define HAIER_VERSION           0x00
#define NOW_VERSION JUMOO_VERSION

// 选择横款射频或竖款射频
#define HORIZONTAL_PLATE        0x01    // 横版设备
#define VERTICAL_PLATE          0x00    // 竖版设备
#define NOW_PLATE               HORIZONTAL_PLATE

// 设备配对版本号
#define DEVICE_SOFT_RSV_1       0x11
#define DEVICE_SOFT_RSV_2       0x16
#define DEVICE_PAIR_RSV_1       ((NOW_SOFT_VERSION & 0xFF00) >> 8)
#define DEVICE_PAIR_RSV_2       (NOW_SOFT_VERSION & 0xFF)
#define NOW_DEVICE_VERSION      ((DEVICE_SOFT_RSV_1 << 8) | DEVICE_SOFT_RSV_2)


#if (NOW_SOFT_VERSION == SRC_1116_6242)

// 配对按键停止声明
#define PAIR_STOP_KEY           0x01

// 隐藏功能键配置
#define FILTERE_ELEMENT_STOP    PAIR_STOP_KEY
#define FILTERE_ELEMENT_ADD     0x08
#define FILTERE_ELEMENT_DEC     0x09

// 配对射频信号放大键声明
#define PAIR_RF_INCREASE_KEY    0x02

// 上电默认档位参数初始化宏定义
#define WATRE_TEMPERATURE       0x03 // 水温
#define SEAT_TEMPERATURE        0x03 // 座温
#define AIR_TEMPERATURE         0x02 // 风温
#define SEAT_POSOTION           0x05 // 喷位
#define WATRE_PRESSURE          0x03 // 水压

// 配对恢复功能上电标志位
#define AUTO_FLUSH_VALUE        0x01  // 自动冲刷
#define FOOT_SENSATION_VALUE    0x00  // 脚感
#define SET_FUN_AOTU_VALUE      0x00  // 自动翻盖
#define SET_FUNPOWER_VALUE      0x00  // 节电
#define SET_INTELLIGENT_VALUE   0x01  // 夜灯开关
#define PRE_WETTING_VALUE       0x01  // 预湿润
#define AUTOMATIC_FOAM_VALUE    0x00  // 自动发泡（跑沫盾）
#define MUTE_CLOSE_VALUE        0x01  // 静音开关
#define SET_SENSOR_LIGHT_VALUE  0x00  // 光感开关

#elif(NOW_SOFT_VERSION == SRC_1116_6254)

// 配对按键停止声明
#define PAIR_STOP_KEY           0x01

// 隐藏功能键配置
#define FILTERE_ELEMENT_STOP    PAIR_STOP_KEY
#define FILTERE_ELEMENT_ADD     0x0B
#define FILTERE_ELEMENT_DEC     0x0A

// 配对射频信号放大键声明
#define PAIR_RF_INCREASE_KEY    0x03

// 上电默认档位参数初始化宏定义
#define WATRE_TEMPERATURE       0x03 // 水温
#define SEAT_TEMPERATURE        0x03 // 座温
#define AIR_TEMPERATURE         0x02 // 风温
#define SEAT_POSOTION           0x03 // 喷位
#define WATRE_PRESSURE          0x03 // 水压

// 配对恢复功能上电标志位
#define AUTO_FLUSH_VALUE        0x01  // 自动冲刷
#define FOOT_SENSATION_VALUE    0x01  // 脚感
#define SET_FUN_AOTU_VALUE      0x00  // 自动翻盖
#define SET_FUNPOWER_VALUE      0x00  // 节电
#define SET_INTELLIGENT_VALUE   0x01  // 夜灯开关
#define PRE_WETTING_VALUE       0x01  // 预湿润
#define AUTOMATIC_FOAM_VALUE    0x01  // 自动发泡（跑沫盾）
#define MUTE_CLOSE_VALUE        0x00  // 静音开关
#define FOOT_FEEL_FOAM_VALUE    0x00  // 脚感发泡
#define SET_SENSOR_LIGHT_VALUE  0x00  // 光感开关
#define SET_GESTURE_VALUE       0x00  // 手势开关


#endif 

// 不同款休眠时间选择
#if (NOW_VERSION == JUMOO_VERSION)
#define SLEEP_TIME_VALUE 3
#else
#define SLEEP_TIME_VALUE 5
#endif

// 选择不同按键的产测按键和发送MAC_SOFTWARE的键值
#if (NOW_DEVICE_VERSION == 0x1116)
#define FACTORY_KEY_1 0x02
#define FACTORY_KEY_2 0x05
#define SEND_MAC_KEY_1 0x01
#define SEND_MAC_KEY_2 0x04
#endif
#if (NOW_DEVICE_VERSION == 0x1625)
#define FACTORY_KEY_1 0x02
#define FACTORY_KEY_2 0x05
#define SEND_MAC_KEY_1 0x02
#define SEND_MAC_KEY_2 0x07
#endif


#define FUNCTION_MODE    4 // 四种模式
#define WASH_MODE        4
#define OFF              0X0
#define ON               0X1

typedef struct
{
    u8   gear;
    bool fz_flag;
}Gear_fz;

typedef struct
{
    Gear_fz air_temperature;
    Gear_fz seat_position;
    Gear_fz seat_temperature;
    Gear_fz water_temperature;
    Gear_fz water_pressure;
}Wash_event;

typedef struct
{
    u8 mute;                    // 静音标志位
    u8 seat_foaming;            // 自动发泡
    u8 foot_sensation;          // 脚感开关
    u8 Foot_feel_foam;          // 脚感发泡开关
    u8 night_light;             // 夜灯开关
    u8 pre_wetting;             // 预湿润开关
    u8 sensor_light;            // 光感
    u8 power_save;              // 省电模式
    u8 auto_flip_cover;         // 自动翻盖
    u8 auto_flush;              // 自动冲刷
    u8 switch_gesture;          // 手势开关
}Function_on_off;

//模式标志
extern _attribute_data_retention_ u8 now_mode;
extern _attribute_data_retention_ u8 normal_wash;
extern _attribute_data_retention_ u8 child_wash;
extern _attribute_data_retention_ u8 women_wash;
extern _attribute_data_retention_ u8 strong_wash;

//模式和功能结构体
extern _attribute_data_retention_ Wash_event g_wash[WASH_MODE];
extern _attribute_data_retention_ Function_on_off g_function;

extern bool Rf_increase_flag;
extern bool Key_dis_pairing_flag;
extern bool set_fun_filter_element_flag;
extern _attribute_data_retention_ bool factory_test_key_falg;

extern volatile u32 interrupted_time;
extern volatile u32 air_temperature_lights_up_3s_tick;


// led 灯熄灭时间
extern volatile u32 led_pwm_lighting_off_time;
extern volatile u32 set_fun_filter_element_time;

// 功能按键声明
extern bool Key_function_release_switch_flag;
extern bool Key_function_switch_three_runed_flag;

extern void fun_init();
extern void Function_data_init();
u8 Key_function_switch_event(u8 key_code_val);

// 功能函数声明
extern void Jomoo_pairing_successfully_restores_default_values();
extern void Jomoo_restore_child_washing_initialization_parameters();
extern void pair_addr_write_flash();
extern void Jomoo_Divece_pairing_event();
extern void Jomoo_instant_stop_event(u8 key_code_val);
extern void Jomoo_water_scour_event(u8 key_code_val);
extern void Jomoo_aotu_scour_open_event(u8 key_code_val);
extern void Jomoo_aotu_scour_off_event(u8 key_code_val);
extern void Jomoo_water_sml_event(u8 key_code_val);
extern void Jomoo_instant_auto_stop_event(u8 key_code_val);
extern void Jomoo_pair_event(u8 key_code_val);
extern void Jomoo_hip_washing_movement_event(u8 key_code_val);
extern void Jomoo_Strong_washing_event(u8 key_code_val);
extern void Jomoo_women_washing_movement_event(u8 key_code_val);
extern void Jomoo_child_washing_event(u8 key_code_val);
extern void Jomoo_foam_shield_event(u8 key_code_val);
extern void Jomoo_seat_foaming_event(u8 key_code_val);
extern void Jomoo_water_temperature_event(u8 key_code_val);
extern void Jomoo_energy_conservation_off_event(u8 key_code_val);
extern void Jomoo_energy_conservation_open_event(u8 key_code_val);
extern void Jomoo_seat_temperature_event(u8 key_code_val);
extern void Jomoo_intelligent_night_light_event(u8 key_code_val);
extern void Jomoo_warm_air_air_temperature_event(u8 key_code_val);
extern void Jomoo_foot_sensation_event(u8 key_code_val);
extern void Jomoo_move_forward_event(u8 key_code_val);
extern void Jomoo_nozzle_clean_event(u8 key_code_val);
extern void Jomoo_move_back_event(u8 key_code_val);
extern void Jomoo_device_mute_event(u8 key_code_val);
extern void Jomoo_water_pressure_event(u8 key_code_val);
extern void Jomoo_pre_wetting_event(u8 key_code_val);
extern void Jomoo_seat_temperature_dec_event(u8 key_code_val);
extern void Jomoo_seat_temperature_add_event(u8 key_code_val);
extern void Jomoo_Toilet_flap_event(u8 key_code_val);
extern void Jomoo_Toilet_roll_event(u8 key_code_val);
extern void Jomoo_Facilitating_stool_event(u8 key_code_val);
extern void Jomoo_seation_event(u8 key_code_val);
extern void Jomoo_warm_air_event(u8 key_code_val);
extern void Jomoo_air_temperature_event(u8 key_code_val);
extern void Jomoo_water_pressure_add_event(u8 key_code_val);
extern void Jomoo_water_pressure_dec_event(u8 key_code_val);
extern void Jomoo_null_event(u8 key_code_val);
extern void Jomoo_aotu_scour_open_off_event(u8 key_code_val);
extern void Jomoo_energy_open_off_event(u8 key_code_val);
// 组合键
extern void Jomoo_combination_forced_foaming_event(u8 key_code_val);
extern void Jomoo_combination_filter_reset_event(u8 key_code_val);
extern void Jomoo_combination_filter_stop_event(u8 key_code_val);
extern void Jomoo_combination_filter_time_event(u8 key_code_val);
extern void powered_on_press_enter_production_testing();

// RAM_CODE_SEC 函数声明
extern _attribute_ram_code_sec_ void Jomoo_pair_success_event(void);
#endif