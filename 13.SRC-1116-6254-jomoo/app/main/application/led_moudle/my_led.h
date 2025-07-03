#ifndef __MY_LED__H__
#define __MY_LED__H__

#include "driver.h"
#include "pwm_handle.h"
#include "help_tools.h"
#include <types.h>


//我设置的代码


#define LEDS_NUM 5  // 定义 LED 的数量


typedef struct 
{
    uint8_t id;                     // LED ID
    uint16_t brightness;            // LED 亮度 (0-1000)
    bool is_on;                     // LED 状态
    volatile uint32_t on_time;        // LED 亮灯时间（单位：毫秒）
    uint32_t off_time;              // LED 亮灯间隔时间（单位：毫秒）
    volatile uint32_t start_delay;    // LED 亮灯时间（单位：毫秒）
    uint32_t end_delay;             // LED 亮灯间隔时间（单位：毫秒）
    uint8_t  blink_count;           // LED 亮灯次数
}Led_one;

typedef struct 
{
    u8      all_on_off;
    Led_one status[5];   
}Led_status; // 使用 volatile 关键字

extern Led_status g_leds;

// 初始化 LED
void led_init(); 
extern void led_init_set(uint8_t id);

// 切换 LED 状态
extern void led_toggle(uint8_t id);

//单独或者每个灯的状态设置
extern void led_set_every(u8 led_1, u8 led_2, u8 led_3, u8 led_4, u8 led_5);

//灯的控制事件
extern void led_contral_on_off(void);

//单独或者每个灯的状态设置
extern void led_set_brightness_time(char *leds, u16 brightness, char *time, char *time_interval, u8 count);

#endif