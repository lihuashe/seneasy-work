/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     app_rcu.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef APP_RCU_H_
#define APP_RCU_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include  "app_config.h"
#include "../sdk/tl_common.h"
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
#define     ADV_INTERNAL_SWITCH_TIME            30   //30s
#define     ADV_IDLE_ENTER_DEEP_TIME            60   //120
#define     ADV_RECONN_ENTER_DEEP_COUNT         1
#define     ADV_IDLE_ENTER_DEEP_COUNT           3//5    //5
#define     CONN_IDLE_ENTER_DEEP_TIME           60   //60 s
#define     MY_DIRECT_ADV_TMIE                  3*1000*1000 //3s

#define     MY_APP_ADV_CHANNEL                  BLT_ENABLE_ADV_ALL
#define     MY_ADV_INTERVAL_MIN                 ADV_INTERVAL_10MS
#define     MY_ADV_INTERVAL_MAX                 ADV_INTERVAL_20MS

#if (MCU_CORE_TYPE == MCU_CORE_8278)
    #define        MY_RF_POWER_INDEX            RF_POWER_P3p50dBm
#else
    #define        MY_RF_POWER_INDEX            RF_POWER_P3p01dBm
#endif
#define        BLE_DEVICE_ADDRESS_TYPE          BLE_DEVICE_ADDRESS_PUBLIC

extern u8 user_unpair_wait_terminate;
extern u8 tick_key_source;
extern u8 tick_key_options;
extern u8 tick_key_guide;
extern u8 tick_key_subtitle;
extern u8 tick_key_green;
extern u8 tick_key_voice;
extern u8 tick_key_back_home;
extern _attribute_data_retention_ u8 app_rcu_pairing_flag;
extern _attribute_data_retention_ u32 ftick_voice_reconn_send_code;
extern _attribute_data_retention_ u32 app_voice_key_reconn_timer;
extern _attribute_data_retention_ u32 app_enter_single_tone_timer;
extern _attribute_data_retention_ u32 app_backlight_ic_close_timer;
extern _attribute_data_retention_ u32 app_hidden_close_timer; 
extern _attribute_data_retention_ u32 hidden_LongPressTick;
extern _attribute_data_retention_ u32 br_test_close_led_timer;
extern _attribute_data_retention_ u32 br_test_close_led_settime;
extern _attribute_data_retention_ u32 br_test_led_duty_timer; 
extern _attribute_data_retention_ u32 br_test_led_duty_settime;
extern _attribute_data_retention_ u32 ltr_sensor_timer;
extern _attribute_data_retention_ u32 sensor_init_timer;
extern _attribute_data_retention_ u32 led_display_timer;
extern _attribute_data_retention_ u8 Flash_Power_Flag;
extern _attribute_data_retention_ u8 led_display_flag;
extern _attribute_data_retention_ u8 led_display_keycode;
extern _attribute_data_retention_ u8 sensor_init_error_cnt;
extern _attribute_data_retention_ u32 ASL_SENSOR_INIT_timer;
extern _attribute_data_retention_ u32 app_audio_delay_close_timer;
extern _attribute_data_retention_ u32 led_bct3236_normalKey_00_timer;
extern _attribute_data_retention_ u32 led_bct3236_normalKey_01_timer;
extern _attribute_data_retention_ u32 led_bct3236_hiddenKey_timer;
extern _attribute_data_retention_ u32 led_bct3236_allKey_00_timer;
extern _attribute_data_retention_ u32 led_bct3236_allKey_01_timer;
extern const u8 tbl_advData_TPV[31];
extern const u8 tbl_scanRsp_TPV[7];

extern bool app_hidden_key_mode;
extern bool app_hidden_close_flag;
/*============================================================================*
 *                          Functions
 *============================================================================*/
void app_rcu_timer_task(void);
void app_dcdc_enable(void);
void app_dcdc_delay_disable(void);
void app_rcu_erase_pair(void);
int  app_rcu_repairing(void);
void app_rcu_unpair_wait_terminate(void);
void app_rcu_enter_single_tone(void);
void pressed_voice_key(void);
void pressed_voice_key_release(void);
void app_rcu_send_voice_ble(void);
void pressed_source_key(void);
void pressed_source_key_release(void);
void pressed_options_key(void);
void pressed_options_key_release(void);
void pressed_guide_key(void);
void pressed_guide_key_release(void);
void pressed_subtitle_key(void);
void pressed_subtitle_key_release(void);
void pressed_green_key(void);
void pressed_green_key_release(void);
void combkey_back_ok_timeout(void);
void combkey_back_down_timeout(void);
void pressed_back_home_key_release(void);
void combkey_back_home_timeout(void);
void process_batt_poor(void);
void user_direct_adv(int n);
void app_backlight_key_handle(u8 key_code);
void app_backlight_key_release_handle(void);
void app_rcu_flash_lock_init(void);
void app_rcu_flash_unlock_init(void);
void pressed_123_key(void);
#endif /* APP_RCU_H_ */
