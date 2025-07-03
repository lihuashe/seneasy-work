/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     led_config.h
* @brief    
* @details
* @author   lihuashe
* @date     2024-12-20
* @version  v1.0
*********************************************************************************************************
*/

#ifndef LED_CONFIG_H_
#define LED_CONFIG_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "bct3236_driver.h"
#include "aw21036_driver.h"
#include "app_function.h"
#include "os_timer.h"
#include "common_macros.h"
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
#define  TIMER_LED_SET_TIME   20/* 循环RED_LED运行时长10ms */


typedef enum
{
    LED_NORMAL_WORK = 0x00,
    LED_CHGING,             //充电呼吸灯灯效
    LED_CHG_DONE,           //充电完成白灯灯效
    LED_LOW_POWER,          //低电量灯效
    LED_PAIRING_START,      //开始配对等效
    LED_PAIRED_OK,          //配对成功灯效
    LED_OTA_START,          //开始OTA灯效
    LED_BRIGHT_STOP

}ENUM_LED_SYSTEM_STATUS_DEF;

typedef enum
{
    LED_BLINK_MODE    = 0x01,
    LED_BREATH_MODE,
    LED_BLINK_AND_BREATH_MODE

}ENUM_VOLTAGE_LED_MODE_DEF;


typedef enum
{
    LED_NORMAL = 0x00,
    LED_RESET,
    LED_DELETE_LOOP,//loop次数归零
    LED_DELETE_COUNT//blink次数归零

}ENUM_LED_BLINK_SET_DEF;

typedef struct
{
    uint8_t  brightness;//亮度
    uint8_t  blink_count;//次数
    uint8_t  loop_count;//多少次循环
    uint16_t keep_time;//亮灯时间
    uint16_t wait_time;//熄灯时间
    uint16_t loop_time;//下次循环时间

}Blink_Led_Config; 


typedef struct
{
    u8_Func     led_on;
    void_Func   led_off;
    u32_Func    blink_timer_t;//默认ms定时器
    void_Func   blink_stop_timer_t;

}Blink_Led_Handler; 


typedef struct
{
    uint8_t  current_brightness;
    uint8_t  start_brightness;
    uint8_t  end_brightness;
    uint8_t  step_brightness;
    uint16_t led_breath_time;//ms,led的呼吸间隔
    uint16_t led_end_keep_time;
    bool     reverse_led_flag;

}Breath_Led_Config;


typedef struct
{
    u8_Func     led_on;
    u32_Func    breath_timer_t;

}Breath_Led_Handler; 

typedef struct
{
    uint8_t  mode;
    uint8_t  gear;
    uint8_t  led_mode;
    Blink_Led_Config blink;
}Voltage_Led_Map; 

static Voltage_Led_Map v_led_map[] = {

    {SCALP_MODE, 1, LED_BLINK_MODE, {0xfe, 1, 0, 200, 1000}},
    {SCALP_MODE, 2, LED_BLINK_MODE, {0xfe, 1, 0, 200, 800}},
    {SCALP_MODE, 3, LED_BLINK_MODE, {0xfe, 1, 0, 200, 600}},
    {SCALP_MODE, 4, LED_BLINK_MODE, {0xfe, 1, 0, 200, 400}},
    {SCALP_MODE, 5, LED_BLINK_MODE, {0xfe, 1, 0, 200, 200}},

    {FACE_MODE, 1, LED_BLINK_MODE, {0xfe, 2, 0, 100, 100, 1000}},
    {FACE_MODE, 2, LED_BLINK_MODE, {0xfe, 2, 0, 100, 100, 800}},
    {FACE_MODE, 3, LED_BLINK_MODE, {0xfe, 2, 0, 100, 100, 600}}, 
    {FACE_MODE, 4, LED_BLINK_MODE, {0xfe, 2, 0, 100, 100, 400}},
    {FACE_MODE, 5, LED_BLINK_MODE, {0xfe, 2, 0, 100, 100, 200}},
  
    {FACE_MESO_MODE, 1, LED_BLINK_AND_BREATH_MODE, {0xfe, 2, 0, 100, 100, 1000}},
    {FACE_MESO_MODE, 2, LED_BLINK_AND_BREATH_MODE, {0xfe, 2, 0, 100, 100, 800}},
    {FACE_MESO_MODE, 3, LED_BLINK_AND_BREATH_MODE, {0xfe, 2, 0, 100, 100, 600}}, 
    {FACE_MESO_MODE, 4, LED_BLINK_AND_BREATH_MODE, {0xfe, 2, 0, 100, 100, 400}},
    {FACE_MESO_MODE, 5, LED_BLINK_AND_BREATH_MODE, {0xfe, 2, 0, 100, 100, 200}},

    {SCALP_MESO_MODE, 1, LED_BLINK_AND_BREATH_MODE, {0xfe, 1, 0, 200, 1000, 10}},
    {SCALP_MESO_MODE, 2, LED_BLINK_AND_BREATH_MODE, {0xfe, 1, 0, 200, 800, 10}},
    {SCALP_MESO_MODE, 3, LED_BLINK_AND_BREATH_MODE, {0xfe, 1, 0, 200, 600, 10}},
    {SCALP_MESO_MODE, 4, LED_BLINK_AND_BREATH_MODE, {0xfe, 1, 0, 200, 400, 10}},
    {SCALP_MESO_MODE, 5, LED_BLINK_AND_BREATH_MODE, {0xfe, 1, 0, 200, 200, 10}},

};

static Voltage_Led_Map low_power_map = {0, 0, LED_BLINK_MODE, {0xff, 9, 1, 200, 200, 1000}};
static Voltage_Led_Map led_ble_pair_map = {0, 0, LED_BLINK_MODE, {0xff, 30, 1, 500, 500, 10}};
static Voltage_Led_Map led_ble_paired_map = {0, 0, LED_BLINK_MODE, {0xff, 1, 1, 3000, 10, 10}};

static Voltage_Led_Map now_led_map;
static uint8_t led_max_bright;
static uint8_t led_imax;
static uint8_t led_mode;
static Breath_Led_Config chg_red_led = {240, 230, 0x00, 3, 30, 500, false};
static Breath_Led_Config hv_lv_red_led = {240, 230, 0x00, 2, 26, 500, false};
static Breath_Led_Config led_ble_ota_map = {240, 230, 0x00, 5, 26, 500, false};
static Breath_Led_Config now_breath = {240, 230, 0x00, 40, 300, 500, false};
static Blink_Led_Handler  led_blink_handler;
static Breath_Led_Handler led_breath_handler;

extern uint8_t g_led_sys_status;
/*============================================================================*
 *                          Functions
 *============================================================================*/
//基础控制函数
void ctrl_gear_key_backlit_off(void);
void ctrl_gear_key_backlit_on(void);
void ctrl_mode_key_backlit_off(void);
void ctrl_mode_key_backlit_on(void);

void ctrl_gear_led_all_off(void);
void ctrl_gear_led(uint8_t gear);
void ctrl_mode_led_all_off(void);
void ctrl_mode_led_all_on(void);
void ctrl_mode_led(uint8_t mode);
void ctrl_mode_gear_led(uint8_t mode, uint8_t gear);

void ctrl_red_led_all_off(void);
void ctrl_red_led(uint8_t brightness);
void ctrl_chg_red_led_off(void);
void ctrl_chg_red_led_set(uint8_t brightness);

//亮灯控制开始
void ctrl_ota_led_start(void);
void ctrl_led_ble_pair_start(void);
void ctrl_chg_red_start(void);
void ctrl_low_power_red_led_start(void);

//特殊函数
void led_event_timer_t_set(uint32_t time_ms);
void led_event_stop_timer_t(void);
bool led_blink_ctrl_event(Blink_Led_Config *led, Blink_Led_Handler *led_handler, uint8_t set_mode) DATA_RAM_FUNCTION;
bool led_breath_ctrl_event(Breath_Led_Config *led, Breath_Led_Handler *led_handler) DATA_RAM_FUNCTION;
void led_ctrl_timer_cb(TimerHandle_t pxTimer);
void ctrl_all_led_timer_stop(void) DATA_RAM_FUNCTION;
void led_system_ctrl(uint8_t led_status) DATA_RAM_FUNCTION;

//方便调换引脚的led宏
#define WHITE_LED_GEAR_1    10
#define WHITE_LED_GEAR_2    12
#define WHITE_LED_GEAR_3    14
#define WHITE_LED_GEAR_4    16
#define WHITE_LED_GEAR_5    18

#define RED_LED_CHG_1       11
#define RED_LED_CHG_2       13
#define RED_LED_CHG_3       15
#define RED_LED_CHG_4       17
#define RED_LED_CHG_5       19




#endif /* LED_CONFIG_H_ */
