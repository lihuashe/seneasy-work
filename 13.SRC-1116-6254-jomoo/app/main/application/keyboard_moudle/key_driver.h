#ifndef __KEY_DRUVER_H__
#define __KEY_DRUVER_H__

/*********************** head file ********************/
#include "driver.h"

// 定义键盘矩阵的行列数量
#define ROW_CNT 4
#define COL_CNT 4

#define KEY_SCAN_NUM_1 (ROW_CNT * COL_CNT + 1)
#define KEY_SCAN_NUM_2 KEY_SCAN_NUM_1 * 2
#define KEY_SCAN_NUM_4 KEY_SCAN_NUM_2 * 2

#define KEY_SCAN_TIME KEY_SCAN_NUM_1
#define STUCK_SCAN_TIME KEY_SCAN_NUM_4

#define Row0 GPIO_PB6
#define Row1 GPIO_PB7
#define Row2 GPIO_PC0
#define Row3 GPIO_PC1

#define Col1 GPIO_PD4
#define Col2 GPIO_PB2
#define Col3 GPIO_PB4
#define Col4 GPIO_PB5

#define KEY_COMBINE_ENABLE 1

// 按键引脚端口配置
extern unsigned char g_ks_row[];
extern unsigned char g_ks_col[];

// 定义32K时钟源类型
#define SOURCE_32K_RC 0

// 定义按键对应值
#define KB_CODE_S1 1
#define KB_CODE_S2 2
#define KB_CODE_S3 3
#define KB_CODE_S4 4
#define KB_CODE_S5 5
#define KB_CODE_S6 6
#define KB_CODE_S7 7
#define KB_CODE_S8 8
#define KB_CODE_S9 9
#define KB_CODE_S10 10
#define KB_CODE_S11 11
#define KB_CODE_S12 12
#define KB_CODE_S13 13
#define KB_CODE_S14 14
#define KB_CODE_S15 15
#define KB_CODE_S16 16
#define KB_NULL     100
#define NUM_KEYS 16
#define KEY_COMFIG (NUM_KEYS + 1)


enum KEY_CURRENT_TYPE
{
    NO_ACTION = 0x00,
    SHORT_PRESS = 0x01,
    LONG_PRESS_THREE_S = 0x02,
    LONG_PRESS_AIGHT_S = 0x03,
    INSTANT_HANDLE_PRESS = 0x04,
};
typedef enum KEY_CURRENT_TYPE Key_current_type;
extern Key_current_type key_cur_type;

enum KEY_CURRENT_STATUS
{
    KEY_RELEASED = 0x00,
    KEY_PRESSED = 0x01,
};
typedef enum KEY_CURRENT_STATUS Key_current_status;
// extern Key_current_status;
Key_current_status extern key_cur_status;

// 档位
extern u8 water_temperature;
extern u8 seat_temperature;
extern u8 air_temperature;

// 0为加档 1为减档  后续需要使用深度睡眠寄存器
bool water_temperature_add_subtract_gear;
bool seat_temperature_add_subtract_gear;

// 妇洗处理
extern u8 tick_women_wash_fx; // 妇洗标志位
extern u8 water_pressure_fx;
extern u8 seat_position_fX; // 位置档位-妇洗

// 童洗处理
extern u8 tick_child_wash_tx; // 童洗标志位
extern u8 water_temperature_tx;
extern u8 air_temperature_tx;
extern u8 water_pressure_tx;
extern u8 seat_position_tX; // 位置档位-童洗

extern u8 water_pressure;
extern u8 seat_position;
extern u8 intelligent_night_light;
volatile extern u32 led_pwm_lighting_duration_3s_tick;
extern _attribute_data_retention_ bool wakeup_src_value;
extern u32 tick_factory_test_check;

// 产测变量
extern u8 factory_test_key_values;
extern _attribute_data_retention_ bool production_testing_power_on_led_off;

// 卡键标志位
extern _attribute_data_retention_ bool stuck_flag;

// 按键处理事件
extern bool long_press_3s_s2_pair;
extern bool pair_addrass_write_flash;
extern bool RELEASE_HANDLE;
// 时间标志位
extern volatile u32 pair_led_start_time;
extern volatile u32 pair_led_succuss_time;
extern volatile u32 stuck_key_time_flag;

// 按键存储变量声明
extern u8 key_static[2];

// 设备休眠状态
extern bool key_wake_static_flag;

// 按键存储变量声明
extern u8 key_static[2];
extern void key_user_init();
extern void keyhandler_event(void);
extern void Scan_key_release(void);
extern void Stuck_sleep_wakeup_event();
void Instant_trigger_event();
extern void key_event_loop();
extern void key_change_proc_old();
void Combine_key_handle_event();

int kb_status_ret();
void time_rcu_event_handler();
void key_handle_pressed_1_event();
void Key_release_data_init();
void key_handle_pressed_2_event();

void led_pwm_paired_successfully_all_open_3s();
void powered_on_press_enter_production_testing();
bool Combine_key_lose_event(u8 keycode_1, u8 keycode_2, u8 chose_code_1, u8 chose_code_2);
extern _attribute_ram_code_sec_noinline_ void key_irq_handler(void);

#if (UI_KEYBOARD_ENABLE)
void key_change_proc(void);
void proc_keyboard_Test (u8 e, u8 *p, int n);
extern void Stuck_key_release_event();
#endif

#endif