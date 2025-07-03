/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     backlight_handle.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef BACKLIGHT_HANDLE_H_
#define BACKLIGHT_HANDLE_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include  "../app_config.h"
#include "../../sdk/tl_common.h"
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
#define BCT3236_DEVICE_ADDR			(0x78)  /* AD0——>GND */

extern u8 tick_key_up_home;
extern u8 tick_key_power;
extern u8 tick_key_ambilight;
extern u8 tick_key_ott_3;
extern u8 tick_key_ott_4;
extern u8 key_level_flag;
extern u8 tick_backlight_normal;
extern u8 tick_backlight_hidden;
extern u8 tick_backlight_all;
extern u8 br_val_level[20];
extern _attribute_data_retention_ bool led_test_sw_flag;
extern _attribute_data_retention_ bool backlight_normal_Flag; 
extern _attribute_data_retention_ bool backlight_hidden_Flag; 
extern _attribute_data_retention_ u32 app_backlight_normal_timer; 
extern _attribute_data_retention_ u32 app_backlight_hidden_timer; 
extern _attribute_data_retention_ u32 app_backlight_all_timer; 
extern _attribute_data_retention_ u8 key_123_press_flag; 
extern _attribute_data_retention_ u8 key_press_quick_timer; 
extern _attribute_data_retention_ u32 LedDisplayTick;
extern _attribute_data_retention_ int backlight_task;

#if (TEST_SW_LED)
extern u8 led_test_sleep;
#endif

#define LED_NORMAL_ON2S                 0x01
#define LED_NORMAL_ON5S                 0x02
#define LED_NORMAL_ON2S_HIDDEN_ON2S     0x03
#define LED_NORMAL_ON5S_HIDDEN_ON5S     0x04
#define LED_NORMAL_ON2S_HIDDEN_OFF      0x05
/*============================================================================*
 *                          Functions
 *============================================================================*/
extern void app_backlight_task(void);
extern void app_backlight_model_function(u8 light_model);
extern void app_backlight_NormalKey_Display(u8 led_flag, u32 led_timer);
extern void aw21036_backlight_NormalKey(u8 led_flag);
extern void aw21036_backlight_HiddenKey(u8 led_flag);
extern void aw21036_backlight_AllKey(u8 led_flag);
extern void LED_Test_SW_Function(u8 key0);
extern void LED_Test_SW_Change(void);
extern void LED_Test_SW_Toggle_ALL(void);
extern void LED_Test_SW_Increase_Decrease_BR(u8 BR_Flag);
extern void pressed_power_key(void);
extern void pressed_power_key_release(void);
extern void pressed_ambilight_key(void);
extern void pressed_ambilight_key_release(void);
extern void pressed_ott_3_key(void);
extern void pressed_ott_3_key_release(void);
extern void pressed_ott_4_key(void);
extern void pressed_ott_4_key_release(void);
extern void LED_Test_SW_Duty(u8 duty_level);
extern void LED_Test_SW_Duty_EachLed(u8 duty_level, u8 each_led);
extern void LED_Test_SW_Duty_GroupLed(u8 duty_level, u8 group_led);

extern void bct3236_backlight_NormalKey(u8 led_flag);
extern void bct3236_backlight_HiddenKey(u8 led_flag);
extern void bct3236_backlight_AllKey(u8 led_flag);
#endif /* BACKLIGHT_HANDLE_H_ */
