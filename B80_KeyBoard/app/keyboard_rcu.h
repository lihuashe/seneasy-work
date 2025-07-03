/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     keyboard_rcu.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef KEYBOARD_RCU_H_
#define KEYBOARD_RCU_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "app_config.h"
#include "../tl_common.h"
#include "rf_24g_module/rf_24g_driver.h"
#include "rf_24g_module/rf_24g_handle.h"

/*============================================================================*
*                        Export Global Variables
*============================================================================*/
//HID USAGE
#define HID_USAGE_KEYBOARD			0x07
#define HID_USAGE_CONSUMER			0x0C
#define HID_USAGE_MOUSE				0x0E

//55 KEY HID
#define HID_NONE                 	0x00
#define HID_KB_ESCAPE               0x29
#define HID_KB_TAB                 	0x2B
#define HID_KB_CAPS_LOCK           	0x39
#define HID_KB_SHIFT_LEFT           0xE1
#define HID_KB_CONTROL_LEFT         0xE0
#define HID_KB_FN                  	0x00
#define HID_KB_GUI_LEFT           	0xE3
#define HID_KB_ALT_LEFT             0xE2
#define HID_KB_SPACE               	0x2C
#define HID_KB_ARROW_LEFT           0x50
#define HID_KB_ARROW_RIGHT          0x4F
#define HID_KB_ARROW_UP             0x52
#define HID_KB_ARROW_DOWN           0x51
#define HID_KB_SHIFT_RIGHT          0xE5
#define HID_KB_ENTER               	0x28
#define HID_KB_BACKSPACE            0x2A
#define HID_KB_DELETE            	0x4C
#define HID_KB_EQUAL          		0x2E	//=
#define HID_KB_PERIOD         		0x37	//.
#define HID_KB_SLASH          		0x38	///
#define HID_KB_1                   	0x1E
#define HID_KB_2                   	0x1F
#define HID_KB_3                   	0x20
#define HID_KB_4                   	0x21
#define HID_KB_5                   	0x22
#define HID_KB_6                   	0x23
#define HID_KB_7                   	0x24
#define HID_KB_8                   	0x25
#define HID_KB_9                   	0x26
#define HID_KB_0                   	0x27
#define HID_KB_Q                   	0x14
#define HID_KB_W                   	0x1A
#define HID_KB_E                   	0x08
#define HID_KB_R                   	0x15
#define HID_KB_T                   	0x17
#define HID_KB_Y                   	0x1C
#define HID_KB_U                   	0x18
#define HID_KB_I                   	0x0C
#define HID_KB_O                   	0x12
#define HID_KB_P                   	0x13
#define HID_KB_A                   	0x04
#define HID_KB_S                   	0x16
#define HID_KB_D                   	0x07
#define HID_KB_F                   	0x09
#define HID_KB_G                   	0x0A
#define HID_KB_H                   	0x0B
#define HID_KB_J                   	0x0D
#define HID_KB_K                   	0x0E
#define HID_KB_L                   	0x0F
#define HID_KB_Z                   	0x1D
#define HID_KB_X                   	0x1B
#define HID_KB_C                   	0x06
#define HID_KB_V                   	0x19
#define HID_KB_B                   	0x05
#define HID_KB_N                   	0x11
#define HID_KB_M                   	0x10

//FN
#define HID_KB_END					0x4D
#define HID_KB_GRAVE				0x35
#define HID_KB_APOSTROPHE			0x34
#define HID_KB_PAGE_UP				0x4B
#define HID_KB_PAGE_DOWN			0x4E
#define HID_KB_HOME					0x4A
#define HID_KB_BACKSLASH			0x31 
#define HID_KB_SEMICOLON			0x33 
#define HID_KB_COMMA				0x36
#define HID_KB_BRACKET_RIGHT		0x30 
#define HID_KB_BRACKET_LEFT			0x2F
#define HID_KB_MINUS				0x2D 

extern const unsigned char KB_Map_RCU_HID_KB[KB_SCAN_NUM * KB_DRIVE_NUM];
extern const unsigned char KB_Map_RCU_HID_FN[KB_SCAN_NUM * KB_DRIVE_NUM];

typedef enum
{
	HAS_MOUSE_REPORT=BIT(0),
	HAS_KEYBOARD_REPORT=BIT(1),
	HAS_CONSUMER_REPORT=BIT(2),
	HAS_SYSYTEM_REPORT=BIT(3),
	HAS_JOYSTIC_REPORT=BIT(4),
}HAS_REPORT_TYPE_AAA;

extern my_fifo_t fifo_km;


typedef enum
{
	FN_PRESS_AAA=BIT(0),
	T_BIND_PRESS_AAA=BIT(1),
	MOUSE_KEY_PRESS_AAA=BIT(2),
	T_ESC_PRESS_AAA=BIT(3),
	T_Q_PRESS_AAA=BIT(4),
	T_W_PRESS_AAA=BIT(5),
	T_E_PRESS_AAA=BIT(6),
	T_R_PRESS_AAA=BIT(7),
	T_T_PRESS_AAA=BIT(8),
	T_Y_PRESS_AAA=BIT(9),
	T_U_PRESS_AAA=BIT(10),
	T_I_PRESS_AAA=BIT(11),
	T_O_PRESS_AAA=BIT(12),
	T_F1_PRESS_AAA=BIT(13),
	T_SPACE_PRESS_AAA=BIT(14),
	T_KB_1_PRESS_AAA=BIT(15),
	T_KB_2_PRESS_AAA=BIT(16),
	T_KB_3_PRESS_AAA=BIT(17),
	T_KB_4_PRESS_AAA=BIT(18),
	T_LCTRL_PRESS_AAA=BIT(19),
}SPECIAL_KEY_PRESS_FLAG_AAA;

#define MAX_BTN_CNT_AAA 6
typedef struct{
	u8 cnt;
	u8 keycode[MAX_BTN_CNT_AAA]; 
	u8 press_cnt;
	u32 special_key_press_f;
}kb_data_t_aaa;
extern kb_data_t_aaa key_buf_aaa;
extern _attribute_data_retention_ volatile int		device_ack_received;
extern volatile u32 start_rf_tick;
extern _attribute_data_retention_ u8 pair_success_flag;
extern _attribute_data_retention_ u8 connect_ok;
extern _attribute_data_retention_ u8 keyboard_send_need_f;
extern _attribute_data_retention_ u32 rf_24G_deep_tick;
extern _attribute_data_retention_ u8 app_func_mode;
extern _attribute_data_retention_ u32 app_rcu_main_init_timer;
extern _attribute_data_retention_ u8  app_rcu_main_init_flag;
extern _attribute_data_retention_ u32 app_rcu_kb_pressed_timer;
extern _attribute_data_retention_ u8  app_rcu_kb_pressed_flag;
extern _attribute_data_retention_ u32 app_led_blink_timer;
extern _attribute_data_retention_ u16 app_led_blink_time_ms;
extern _attribute_data_retention_ u32 app_led_on_timer;
extern _attribute_data_retention_ u16 app_led_on_time_ms;
extern _attribute_data_retention_ u8 app_notify_dongleOTA;
extern _attribute_data_retention_ u32 app_rcu_24g_pair_timer;
extern _attribute_data_retention_ u32 lowBattDet_tick;
extern _attribute_data_retention_ u32 app_led_2_blink_timer;
extern _attribute_data_retention_ u16 app_led_2_blink_time_ms;
extern _attribute_data_retention_ u32 app_ota_dfu_wait_timer;
extern _attribute_data_retention_ u32 app_ota_dfu_download_timer;
extern _attribute_data_retention_ u32 app_heartbeat_delay_timer;
extern _attribute_data_retention_ u32 app_factory_mode_timer;
extern _attribute_data_retention_ u8 keyboard_buf_aaa[8];
extern _attribute_data_retention_ u32 rf_24G_deep_time_ms;
extern _attribute_data_retention_ u32 app_tx_send_data_timer;
extern _attribute_data_retention_ u16 app_tx_send_data_timer_ms;
extern u8 rcu_wakeup_touch;
extern u8 notify_dongleOTA_flag;
extern u8 app_ship_mode_led;
extern u16 app_rcu_batt_mv;
extern u8 report_release_flag;
extern u8 report_gesture_flag;
extern u8 report_mouse_L_flag;
extern u8 report_mouse_R_flag;
extern u8 rcu_ota_dfu_flag;
#define VBAT_VOL_GOOD                       4200
#define VBAT_VOL_LOW                        3600
#define VBAT_VOL_NONE                       3400

#define OTA_FLASH_START_ADD_FIRM_0 			0x00000
#define OTA_FLASH_START_ADD_FIRM_1 			0x40000
#define OTA_FLASH_SECTION_SIZE 				50
#define OTA_ERASE_LEN 						OTA_FLASH_SECTION_SIZE * 2
#define OTA_ERASE_SEATOR 					4096
#define OTA_ERASE_BYTE_SIZE 				OTA_ERASE_SEATOR / 16

#define NOTIFY_DG_BLE_OTA					0x01
#define NOTIFY_DG_DFU_OTA					0x02
#define NOTIFY_RCU_DFU_OTA					0x03
/*============================================================================*
 *                          Functions
 *============================================================================*/
void keyboard_rcu_init(void);
void rcu_main_loop(void);
void keyboard_rcu_loop(void);
void d24g_rf_loop(void);
u8 get_data_report_aaa(void);
void push_report_fifo(void);
void clear_pair_flag(void);
void set_pair_flag();
void d24_start_pair(void);
void idle_status_poll(void);
void adv_count_poll(void);
void pm_app_proc(void);
void get_data_report_release(void);
void get_data_report_gesture(u8 gesture_type, bool gesture_fast);
void get_data_report_heartbeat(void);
void get_data_report_device_fwversion(void);
void get_data_report_device_mac(void);
void get_mouse_report_move_press(u8 ts_press, s16 ts_x, s16 ts_y);
bool stuck_key_release_check(u8 chose_mode);
_attribute_ram_code_sec_ u8 rf_rx_process(rf_packet_t *p_rf_data);

#endif /* KEYBOARD_RCU_H_ */
