#ifndef __KEY_DRIVER_H__
#define __KEY_DRIVER_H__

/*********************** head file ********************/
#include "types.h"
#include "key_config.h"

#define KEY_HIGH_V  1
#define KEY_LOW_V   0

// 键盘结构体定义
typedef struct {
    u8 cnt;
    u8 keycode[MAX_DETECT_KEY_NUM];
} keyscan_data_t;

typedef struct {
    u8 counts:7;  // 每个按键的消抖计数器
    u8 stable_state:1; // 稳定后的状态
} key_debounce_t;

typedef enum _KEYBOARD_STATUS
{
    KEYBOARD_STATUS_NULL = 0x00,    // 无动作
    KEYBOARD_STATUS_RELEASE = 0x01, // 按键抬起
    KEYBOARD_STATUS_PRESS = 0x02,   // 按键按下
} KeyboardStatus;

typedef enum _KEY_EVENT_TYPE
{
    KEY_EVENT_TYPE_NULL = 0x00,       // 无动作
    KEY_EVENT_TYPE_RELEASE = 0x01,    // 按键抬起
    KEY_EVENT_TYPE_PRESS = 0x02,      // 按键按下瞬间
    KEY_EVENT_TYPE_PRESSED = 0x04,    // 按键已经按下
} KEY_EVENT_TYPE;

struct key_state_machine
{
    KeyboardStatus current_status;
    u16 keep_time_ms;
    keyscan_data_t last_kb_data;
    keyscan_data_t current_kb_data;
};

struct key_event
{
    u8 cnt; // type类型对应的按键数量
    KEY_EVENT_TYPE type;
    u8 key_code[4];   // 发生type类型的按键值
    u16 keep_time_ms; // 按键按下时间
};

typedef void (*key_handle)(struct key_event *key_evt);
extern const uint16_t row_pins[KB_ROW_NUM];
extern const uint16_t col_pins[KB_COL_NUM];
/*============================================================================*
 *                          Functions
 *============================================================================*/
bool key_code_is_exist(u8 *k_arr, u8 len, u8 key);
void kb_init(void);
void kb_fast_scan(key_handle callback);
void key_loop(key_handle callback);
void kb_get_key_event(struct key_event *k_evt);
void kb_set_joined_key(u8 *arr, u8 num);
void key_config_wakeupPin();
void key_reset_wakeupPin();
u8 key_get_pressed_num();
#if STUCK_KEY_ENABLE
u8 key_get_stuck_key_num();
#endif

#endif
