/**
*********************************************************************************************************
*               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     pwm_handle.h
* @brief    Related functions of battery management
* @details
* @author   lizefeng
* @date     2024-02-03
* @version  v1.0
*********************************************************************************************************
*/
#ifndef _PWM_HANDLE_H_
#define _PWM_HANDLE_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/

#include "driver.h"
#include "AAA_app_config.h"
#include "function.h"

// 5个led_pwm模式切换  1为pwm工作电压下均衡状态，2为led_io
#define led_model 1

// 0为没有处理微闪   1为处理微闪
#define microflash 0

#define NUM_LEVELS 18 // 档位数量
#define MIN_VOLTAGE 1700 // 最小电压（mV）
#define MAX_VOLTAGE 3500 // 最大电压（mV）
#define PWM_CLCOK_OFF PWM_CLOCK
#define PWM_CLOCK (unsigned short)(1000 * CLOCK_SYS_CLOCK_1US)

extern int voltage_level;

// 时间状态位
extern u32 led_pwm_time;
extern u32 led_pwm_all_blink_time; // 所有led闪烁时间戳

// 时间标志位
extern u8 led_pwm_blink_flag;

// led_pwm状态位
extern volatile u8 led_pwm_status;
extern bool case_5_led_pwm_init;
extern bool case_14_led_pwm_init;
#if (led_model == 1)
extern void set_led_brightness();
#endif
extern void led_pwm_init_led(u8 i);
extern void led_pwm_init_all_led();

extern void led_pwm_luminosity(u8 led_port_chose, u8 led_state); // 亮度设置了10档
extern void led_pwm_all_open();

extern void led_pwm_function(u8 led_1, u8 led_2, u8 led_3, u8 led_4, u8 led_5);

extern void led_pwm_all_off();
extern void led_pwm_status_open(u8 led_port_chose);
extern void led_pwm_paired_successfully_all_open_3s(void);

extern void led_pwm_power_on(); // 上电灯效
extern void led_pwm_135_blink();
extern void led_pwm_pair_init();
extern void led_pwm_single_open(u8 led_port_chose);
extern void led_pwm_single_off(u8 led_port_chose);
extern void led_pwm_control(u8 led_port_chose);
extern void led_pwm_factory_control(u8 led_port_chose, bool turn_on);

extern void led_port_map(u8 led_port_chose, u8 led_state);
#endif