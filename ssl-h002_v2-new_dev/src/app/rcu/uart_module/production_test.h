/**
*********************************************************************************************************
*               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     bct3236_driver.h
* @brief    
* @details
* @author   lihuashe
* @date     2024-12-18
* @version  v1.0
*********************************************************************************************************
*/
#ifndef _PRODUCTION_TEST_H_
#define _PRODUCTION_TEST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "board.h"
#include "stdio.h"
#include "app_section.h"
#include "uart_driver.h"
#include "app_msg.h"
#include "app_task.h"
#include "buzzer.h"
#include "voltage_driver.h"
#include "flash_device.h"
#include "buzzer.h"
#include "swtimer.h"
#include "board.h"

#define FIRMWARE_VERSION                   0x01, 0x02
#define FIRMWARE_STR_VERSION               "CY_V1.0.2_OTA"


#define HV_1800V_AGING_HOUR                0x10
#define HV_1800V_AGING_MIN                 0x12
#define LV_24V_AGING_HOUR                  0x20
#define LV_24V_AGING_MIN                   0x22
#define LV_14V_AGING_HOUR                  0x30
#define LV_14V_AGING_MIN                   0x32

#define AGING_HOUR                    60 * 60 * 1000
#define AGING_MIN                     10 * 60 * 1000 //10分钟停止

#define AGING_HOUR_STOP               5 * 60 * 1000
#define AGING_MIN_STOP                3 * 60 * 1000

typedef void (*Production_Test_Func)(uint8_t *);

typedef enum
{
    NO_AGING_TEST = 0,
    AGING_TEST_STOP = 0,
    AGING_TESTING = 1, 
    AGING_TEST_END = 2,

}ENUM_TEST_HV_LV_STATUS_DEF;

typedef struct 
{
    uint8_t uart_cmd[8];
    Production_Test_Func test_cmd_do;

}Production_Test;

typedef struct 
{
    uint16_t aging_loop_count;
    uint16_t now_aging_loop_count;
    uint8_t volatile_mode;
    uint8_t test_cmd;
    uint32_t test_time;
    ENUM_TEST_HV_LV_STATUS_DEF test_status;

}Production_Hv_Lv_Test;

typedef struct 
{
    uint16_t aging_loop_count;
    uint8_t test_cmd;

}Production_Aging_Test;

// 宏定义
#define INIT_PRODUCTION_HV_LV_TEST() { \
    .aging_loop_count = 0, \
    .now_aging_loop_count = 0, \
    .volatile_mode = 0, \
    .test_cmd = 0, \
    .test_time = 0, \
    .test_status = NO_AGING_TEST, \
}

extern Production_Aging_Test save_aging;

extern volatile bool continue_aging;

static Production_Hv_Lv_Test voltage_aging;

static bool is_stop_work;

extern void production_test_uart_msg(T_IO_MSG production_test_msg);

extern void production_aging_test_timer_cb(TimerHandle_t pxTimer);

extern void product_test_task_init(void);

extern void seq_on_check_continue_aging(void);

void get_production_aging_test_result(uint8_t *cmd);
void exit_production_aging_test(uint8_t *cmd);
void production_aging_test(uint8_t *cmd);
void beauty_red_led_on_off(uint8_t *cmd);
void test_all_voltage_on_off(uint8_t *cmd);
void test_buzzer_off(uint8_t *cmd);
void test_buzzer_set_on(uint8_t *cmd);
void get_firmware_version(uint8_t *cmd);
void get_chg_status(uint8_t *cmd);
void get_bat_dianliang(uint8_t *cmd);
void set_mode_gear(uint8_t *cmd);
void set_sn_num(uint8_t *cmd);
void get_sn_num(uint8_t *cmd);
void key_backlit_led(uint8_t *cmd);
void gear_ctrl_led(uint8_t *cmd);
void mode_ctrl_led(uint8_t *cmd);
void test_chg_red_led(uint8_t *cmd);
void test_power_off(uint8_t *cmd);
void test_single_tone(uint8_t *cmd);


static const Production_Test test_cmd[] = {
    
    {{0xAA, 0x55, 0x00, 0x0E}, &set_sn_num},//写SN
    {{0xAA, 0x55, 0x02, 0x0F, 0x00, 0x00, 0xFE}, &get_sn_num},//读SN
    
    {{0xAA, 0x55, 0x01, 0x08, 0x09, 0xFE}, &test_power_off},//关机
    {{0xAA, 0x55, 0x01, 0x09, 0x0A, 0xFE}, &get_chg_status},//获取充电状态
    {{0xAA, 0x55, 0x01, 0x0B, 0x0C, 0xFE}, &get_bat_dianliang},//获取电量
    {{0xAA, 0x55, 0x01, 0x0C, 0x0D, 0xFE}, &get_firmware_version},//获取固件版本号
    
    {{0xAA, 0x55, 0x01, 0x13, 0x14, 0xFE}, &get_production_aging_test_result},//读高压测试结果命令
    {{0xAA, 0x55, 0x01, 0x11, 0x12, 0xFE}, &exit_production_aging_test},//退出高压老化测试,关机

    {{0xAA, 0x55, 0x01, 0x23, 0x24, 0xFE}, &get_production_aging_test_result},//读中压测试结果命令
    {{0xAA, 0x55, 0x01, 0x21, 0x22, 0xFE}, &exit_production_aging_test},//退出中压老化测试,关机

    {{0xAA, 0x55, 0x01, 0x33, 0x34, 0xFE}, &get_production_aging_test_result},//读低压测试结果命令
    {{0xAA, 0x55, 0x01, 0x31, 0x32, 0xFE}, &exit_production_aging_test},//退出低压老化测试,关机
//-------------------------------------------------------------------------------------------
    {{0xAA, 0x55, 0x02, 0x01, 0x00, 0x03, 0xFE}, &beauty_red_led_on_off},//美容灯关
    {{0xAA, 0x55, 0x02, 0x01, 0x00, 0x04, 0xFE}, &beauty_red_led_on_off},//美容灯开

    {{0xAA, 0x55, 0x02, 0x02, 0x00, 0x04, 0xFE}, &key_backlit_led},//按键背光灯关
    {{0xAA, 0x55, 0x02, 0x02, 0x00, 0x05, 0xFE}, &key_backlit_led},//按键背光灯开

    {{0xAA, 0x55, 0x02, 0x03, 0x00, 0x05, 0xFE}, &test_chg_red_led},//充电指示灯关
    {{0xAA, 0x55, 0x02, 0x03, 0x00, 0x06, 0xFE}, &test_chg_red_led},//充电指示灯开

    {{0xAA, 0x55, 0x02, 0x04, 0x00, 0x06, 0xFE}, &gear_ctrl_led},//挡位指示灯关
    {{0xAA, 0x55, 0x02, 0x04, 0x00, 0x07, 0xFE}, &gear_ctrl_led},//挡位指示灯开

    {{0xAA, 0x55, 0x02, 0x05, 0x00, 0x07, 0xFE}, &mode_ctrl_led},//模式指示灯关
    {{0xAA, 0x55, 0x02, 0x05, 0x01, 0x08, 0xFE}, &mode_ctrl_led},//模式指示灯开

    {{0xAA, 0x55, 0x02, 0x0A, 0x00, 0x0C, 0xFE}, &test_all_voltage_on_off},//高压开关关
    {{0xAA, 0x55, 0x02, 0x0A, 0x01, 0x0D, 0xFE}, &test_all_voltage_on_off},//高压开关开

    {{0xAA, 0x55, 0x02, 0x0B, 0x00, 0x0D, 0xFE}, &test_all_voltage_on_off},//中压开关关
    {{0xAA, 0x55, 0x02, 0x0B, 0x01, 0x0E, 0xFE}, &test_all_voltage_on_off},//中压开关开

    {{0xAA, 0x55, 0x02, 0x0C, 0x00, 0x0E, 0xFE}, &test_all_voltage_on_off},//低压开关关
    {{0xAA, 0x55, 0x02, 0x0C, 0x01, 0x0F, 0xFE}, &test_all_voltage_on_off},//低压开关开

    {{0xAA, 0x55, 0x02, 0x0D, 0x00, 0x00, 0xFE}, &test_single_tone},//设置单载波
    
    {{0xAA, 0x55, 0x02, 0x10, 0x00, 0x00, 0xFE}, &production_aging_test},//进入高压小时老化测试
    {{0xAA, 0x55, 0x02, 0x12, 0x00, 0x00, 0xFE}, &production_aging_test},//进入高压分钟老化测试

    {{0xAA, 0x55, 0x02, 0x20, 0x00, 0x00, 0xFE}, &production_aging_test},//进入中压小时老化测
    {{0xAA, 0x55, 0x02, 0x22, 0x00, 0x00, 0xFE}, &production_aging_test},//进入中压分钟老化测试

    {{0xAA, 0x55, 0x02, 0x30, 0x00, 0x00, 0xFE}, &production_aging_test},//进入低压小时老化测
    {{0xAA, 0x55, 0x02, 0x32, 0x00, 0x00, 0xFE}, &production_aging_test},//进入低压分钟老化测试

    {{0xAA, 0x55, 0x02, 0x06, 0x03, 0x0B, 0xFE}, &test_buzzer_off},//蜂鸣器关
//-------------------------------------------------------------------------------------------
    {{0xAA, 0x55, 0x03, 0x06, 0x00, 0x00, 0x00, 0xFE}, &test_buzzer_set_on},//蜂鸣器开频率

    {{0xAA, 0x55, 0x03, 0x07, 0x00, 0x00, 0x00, 0xFE}, &set_mode_gear},//设置模式档位
};




#ifdef __cplusplus
}
#endif

#endif

/******************* (C) COPYRIGHT 2024 Realtek Semiconductor Corporation *****END OF FILE****/
