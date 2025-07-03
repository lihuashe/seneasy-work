/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     keyboard_rcu.c
* @brief
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "keyboard_rcu.h"
#include "app.h"
#include "key_module/key_driver.h"
#include "key_module/key_handle.h"
#include "sensor_module/da217_driver.h"
#include "sensor_module/da217.h"
#include "pa_module/gsr2501_driver.h"
#include "ir_module/ir_driver.h"
#include "led_module/led_driver.h"
#include "led_module/pwm_handle.h"
#include "charge_module/lp4081b_driver.h"
#include "touch_module/ft3308_driver.h"
#include "rf_ble_module/app_att.h"
#include "ota_module/ota_handle.h"
#include "app_ui.h"
#include "factory_module/factory_profile.h"
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
#define KEYBOARD_FUN_ENABLE_AAA 1
#define CONSUMER_FUN_ENABLE_AAA 1
#define SYSTEM_KEY_FUN_ENABLE_AAA 1

#define xstr(s) str(s)
#define str(s) #s
#define FW_VERSION_STR xstr(VERSION)

MYFIFO_INIT(fifo_km, 12, 16); // The size must be a multiple of 4 bytes
#define BUF_SIZE_KEYBOARD_AAA 8
_attribute_data_retention_ u8 keyboard_buf_aaa[BUF_SIZE_KEYBOARD_AAA];
_attribute_data_retention_ u8 keyboard_buf_last_aaa[BUF_SIZE_KEYBOARD_AAA];
kb_data_t_aaa key_buf_aaa;
_attribute_data_retention_ u8 has_new_report_aaa;
_attribute_data_retention_ u8 connect_ok = 0;
_attribute_data_retention_ u8 pair_success_flag;
_attribute_data_retention_ u8 pair_report_info;
_attribute_data_retention_ u8 keyboard_send_need_f = 0;
_attribute_data_retention_ volatile int device_ack_received = 0;
_attribute_data_retention_ u32 rf_24G_deep_tick;
_attribute_data_retention_ u32 rf_24G_deep_time_ms = 3000;
_attribute_data_retention_ u8 app_func_mode = RF_24G_MODE;
_attribute_data_retention_ u32 app_rcu_main_init_timer = 0;
_attribute_data_retention_ u8 app_rcu_main_init_flag = 1;
_attribute_data_retention_ u32 app_rcu_kb_pressed_timer = 0;
_attribute_data_retention_ u8 app_rcu_kb_pressed_flag = 0;
_attribute_data_retention_ u32 app_led_blink_timer = 0;
_attribute_data_retention_ u16 app_led_blink_time_ms = 250;
_attribute_data_retention_ u32 app_led_on_timer = 0;
_attribute_data_retention_ u16 app_led_on_time_ms = 3000;
_attribute_data_retention_ u8 app_notify_dongleOTA;
_attribute_data_retention_ u32 app_rcu_24g_pair_timer = 0;
_attribute_data_retention_ u32 app_goto_sleep_timer = 0;
_attribute_data_retention_ u32 lowBattDet_tick = 0;
_attribute_data_retention_ u32 app_led_2_blink_timer = 0;
_attribute_data_retention_ u16 app_led_2_blink_time_ms = 250;
_attribute_data_retention_ u32 app_ota_dfu_wait_timer = 0;
_attribute_data_retention_ u32 app_ota_dfu_download_timer = 0;
_attribute_data_retention_ u32 app_heartbeat_delay_timer = 0;
_attribute_data_retention_ u32 app_report_info_timer = 0;
_attribute_data_retention_ u32 app_factory_mode_timer = 0;
_attribute_data_retention_ u32 app_rcu_cache_data_timer = 0;
_attribute_data_retention_ u32 app_rcu_cache_heart_timer = 0;
_attribute_data_retention_ u8 rcu_wakeup_cnt = 0;
_attribute_data_retention_ u32 app_tx_send_data_timer = 0;
_attribute_data_retention_ u16 app_tx_send_data_timer_ms = 2000;
_attribute_data_retention_ u8 kb_data_seq = 0;
u8 rcu_wakeup_touch = 0;
u8 cache_data_key0 = 0;
u8 cache_data_key1 = 0;
u8 cache_data_keycnt = 0;
u16 factory_report_cnt = 0;
u8 app_ship_mode_led = 0;
u8 led_blink_cnt = 0;
u16 app_rcu_batt_mv = 0;
u8 report_release_flag = 0;
u8 report_gesture_flag = 0;
u8 report_mouse_L_flag = 0;
u8 report_mouse_R_flag = 0;
u8 notify_dongleOTA_flag = 0;
u8 rcu_ota_dfu_flag = 0;
u8 rcu_heartbeat_flag = 0;
bool stuck_earse_sleep_flag = 0;
u32 stuck_earse_sleep_time = 0;
u32 stuck_ear_sleep_ck_time = 0;
static u8 wait_count = 0;
static u16 download_count = 0;
static u8 download_timeout = 0;
#define RCU_DFU_OTA_TIME_MS 120 // 10s
volatile u32 start_rf_tick = 0;
#if CONSUMER_REPORT_BYTE_AAA
#define BUF_SIZE_CONSUMER_AAA 1
#else
#define BUF_SIZE_CONSUMER_AAA 4
#endif

#if CONSUMER_FUN_ENABLE_AAA
#if CONSUMER_REPORT_BYTE_AAA
u8 consumer_buf_aaa;
u8 consumer_buf_last_aaa;
const u16 consumer_list[] = {
	0x221, // 0xa3 C_WWW_SEARCH
	0x223, // 0xa4 C_WWW_HOME
	0x224, // 0xa5 C_WWW_BACKWARD
	0x225, // 0xa6 C_WWW_FORWARD
	0x226, // 0xa7 C_WWW_STOP
	0x227, // 0xa8 C_WWW_REFRESH
	0x22A, // 0xa9 C_MY_FAVORITE
	0x183, // 0xaa C_MEDIA_SELECT

	0x18A, // 0xab C_EMAIL
	0x192, // 0xac C_CALCULATOR
	0x194, // 0xad C_MY_COMPUTER
	0xB5,  // 0xae C_NEXT_TRACK
	0xB6,  // 0xaf C_PRE_TRACK
	0xB7,  // 0xb0 C_STOP
	0xCD,  // 0xb1 C_PLAY_PAUSE
	0xE2,  // 0xb2 C_MUTE

	0xE9,  // 0xb3 C_VOL_INC
	0xEA,  // 0xb4 C_VOL_DEC
	0x00,  // 0xb5 telink�Զ����?
	0x22D, // 0xb6 USAGE ZOOM IN
	0x22E, // 0xb7	 USAGE ZOOM OUT
	0x236, // 0xb8	 USAGE PAN LEFT
	0x237, // 0xb9	 USAGE PAN RIGHT
	0x30B, // 0xba	C_BRIGHT_INC

	0x30A, // 0xbb	C_BRIGHT_DEC
	0xB8,  // 0Xbc	 c_rject
	0x30,  // 0Xbd	C_POWER
	0x19E, // 0Xbe	C_TERMINAL_LOCK
};
#else
u32 consumer_buf_aaa;
u32 consumer_buf_last_aaa;
#endif
#if SYSTEM_KEY_FUN_ENABLE_AAA
u8 system_buf_aaa;
u8 system_buf_last_aaa;
#endif
#endif
#if (PCB_S1626_CHANGE)
const unsigned char KB_Map_RCU_HID_KB[KB_SCAN_NUM * KB_DRIVE_NUM] = {
	HID_NONE, HID_KB_CONTROL_LEFT, HID_KB_SHIFT_LEFT, HID_KB_CAPS_LOCK, HID_KB_TAB, HID_KB_ESCAPE, HID_KB_Z, HID_KB_A,
	HID_KB_Q, HID_KB_1, HID_KB_2, HID_KB_FN, HID_KB_X, HID_KB_S, HID_KB_W, HID_KB_8,
	HID_KB_C, HID_KB_D, HID_KB_E, HID_KB_SPACE, HID_KB_GUI_LEFT, HID_KB_V, HID_KB_F, HID_KB_R,
	HID_KB_ALT_LEFT, HID_KB_B, HID_KB_G, HID_KB_T, HID_KB_6, HID_KB_3, HID_KB_N, HID_KB_H,
	HID_KB_Y, HID_KB_7, HID_KB_4, HID_KB_M, HID_KB_J, HID_KB_U, HID_KB_K, HID_KB_5,
	HID_KB_PERIOD, HID_KB_I, HID_KB_9, HID_KB_0, HID_KB_ARROW_LEFT, HID_KB_SLASH, HID_KB_L, HID_KB_O,
	HID_KB_P, HID_KB_ARROW_DOWN, HID_KB_BACKSPACE, HID_KB_ARROW_UP, HID_KB_EQUAL, HID_KB_ENTER, HID_KB_ARROW_RIGHT, HID_KB_SHIFT_RIGHT,
	HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE,
	HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE};

const unsigned char KB_Map_RCU_HID_FN[KB_SCAN_NUM * KB_DRIVE_NUM] = {
	HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE,
	HID_KB_GRAVE, HID_KB_1, HID_KB_2, HID_NONE, HID_NONE, HID_NONE, HID_KB_MINUS, HID_KB_8,
	HID_NONE, HID_NONE, HID_KB_BRACKET_LEFT, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_KB_BRACKET_RIGHT,
	HID_NONE, HID_NONE, HID_NONE, HID_KB_BRACKET_LEFT, HID_KB_6, HID_KB_3, HID_NONE, HID_KB_APOSTROPHE,
	HID_KB_BRACKET_RIGHT, HID_KB_7, HID_KB_4, HID_NONE, HID_KB_APOSTROPHE, HID_KB_COMMA, HID_KB_SEMICOLON, HID_KB_5,
	HID_KB_COMMA, HID_KB_PERIOD, HID_KB_9, HID_KB_0, HID_KB_HOME, HID_KB_SLASH, HID_KB_SEMICOLON, HID_KB_BACKSLASH,
	HID_KB_MINUS, HID_KB_PAGE_DOWN, HID_KB_DELETE, HID_KB_PAGE_UP, HID_KB_EQUAL, HID_KB_ENTER, HID_KB_END, HID_NONE,
	HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE,
	HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE};
#else
const unsigned char KB_Map_RCU_HID_KB[KB_SCAN_NUM * KB_DRIVE_NUM] = {
	HID_NONE, HID_KB_EQUAL, HID_KB_BACKSPACE, HID_KB_ENTER, HID_KB_SHIFT_RIGHT, HID_KB_ARROW_RIGHT, HID_KB_0, HID_KB_P,
	HID_KB_ARROW_UP, HID_KB_ARROW_DOWN, HID_KB_ARROW_LEFT, HID_KB_9, HID_KB_O, HID_KB_L, HID_KB_SLASH, HID_KB_8,
	HID_KB_I, HID_KB_K, HID_KB_PERIOD, HID_KB_SPACE, HID_KB_7, HID_KB_U, HID_KB_J, HID_KB_M,
	HID_KB_6, HID_KB_Y, HID_KB_H, HID_KB_N, HID_KB_ALT_LEFT, HID_KB_5, HID_KB_T, HID_KB_G,
	HID_KB_B, HID_KB_GUI_LEFT, HID_KB_4, HID_KB_R, HID_KB_F, HID_KB_V, HID_KB_D, HID_KB_3,
	HID_KB_E, HID_KB_C, HID_KB_FN, HID_KB_Z, HID_KB_2, HID_KB_W, HID_KB_S, HID_KB_X,
	HID_KB_A, HID_KB_1, HID_KB_SHIFT_LEFT, HID_KB_Q, HID_KB_CONTROL_LEFT, HID_KB_CAPS_LOCK, HID_KB_ESCAPE, HID_KB_TAB,
	HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE,
	HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE};

const unsigned char KB_Map_RCU_HID_FN[KB_SCAN_NUM * KB_DRIVE_NUM] = {
	HID_NONE, HID_KB_EQUAL, HID_KB_DELETE, HID_KB_ENTER, HID_NONE, HID_KB_END, HID_KB_0, HID_KB_MINUS,
	HID_KB_PAGE_UP, HID_KB_PAGE_DOWN, HID_KB_HOME, HID_KB_9, HID_KB_BACKSLASH, HID_KB_SEMICOLON, HID_KB_SLASH, HID_KB_8,
	HID_KB_PERIOD, HID_KB_SEMICOLON, HID_KB_COMMA, HID_NONE, HID_KB_7, HID_KB_COMMA, HID_KB_APOSTROPHE, HID_NONE,
	HID_KB_6, HID_KB_BRACKET_RIGHT, HID_KB_APOSTROPHE, HID_NONE, HID_NONE, HID_KB_5, HID_KB_BRACKET_LEFT, HID_NONE,
	HID_NONE, HID_NONE, HID_KB_4, HID_KB_BRACKET_RIGHT, HID_NONE, HID_NONE, HID_NONE, HID_KB_3,
	HID_KB_BRACKET_LEFT, HID_NONE, HID_NONE, HID_NONE, HID_KB_2, HID_KB_MINUS, HID_NONE, HID_NONE,
	HID_NONE, HID_KB_1, HID_NONE, HID_KB_GRAVE, HID_NONE, HID_NONE, HID_NONE, HID_NONE,
	HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE,
	HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE, HID_NONE};
#endif
/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief  keyboard_rcu_init
 * @param  none
 * @return none
 * @retval void
 */
void keyboard_rcu_init(void)
{
	// printf("--->>> [work_debug] keyboard_rcu_init\r\n");
	rf_24G_deep_tick = clock_time() | 1;
#if (UI_KEYBOARD_ENABLE)
	key_wakeup_init();
	keyscan_gpio_init();
#endif
#if (UI_LED_ENABLE)
	led_1_driver_init();
	led_2_driver_init();
#endif
#if (GSENSOR_ENABLE)
	Gsensor_iic_init();
	Gsensor_read_data(&Acc_data.acc_x, &Acc_data.acc_y, &Acc_data.acc_z);
#endif
#if (GSENSOR_DA217_ENABLE)
	if (APP_WAKEUP_SRC_DEEP_PAD != app_ui_get_wakeup_src())
		sensor_init_normal();
	else
		sensor_init_deepRetn();
	sensor_init_FaceUp();
#endif
#if (PA_MODULE_ENABLE)
	pa_module_init();
	pa_set_mode(MODE_BYPASS);
#endif
#if (IR_MODULE_ENABLE)
	ir_driver_init();
#endif
#if (CHARGE_ENABLE)
	lp4081_init();
	irqb_gpio_init();
	app_rcu_batt_mv = get_battery_voltage();
	// printf("--->>> [work_debug] app_rcu_batt_mv: %d mV\r\n",app_rcu_batt_mv);
#else
	app_rcu_batt_mv = 4000;
#endif
#if (TOUCH_ENABLE)
	FT3308_iic_init();
#endif
	// app_rcu_main_init_timer = clock_time() | 1;
	init_device_info();

	int wakeup_src = app_ui_get_wakeup_src();
	if (wakeup_src == APP_WAKEUP_SRC_DEEP_TIMER)
	{
		rcu_wakeup_cnt++;
		printf("--->>> APP_WAKEUP_SRC_DEEP_TIMER --- rcu_wakeup_cnt: %d<<<---\r\n", rcu_wakeup_cnt);
		if (rcu_wakeup_cnt >= 5)
		{
			rcu_wakeup_cnt = 0;
			rcu_wakeup_touch = 0x00;
		}
		else
			rcu_wakeup_touch = 0x01;
		FT3308_chip_rsin();
		rf_24G_deep_time_ms = 1000;
		rf_24G_deep_tick = clock_time();
	}
	else
	{
		rcu_wakeup_cnt = 0;
		rcu_wakeup_touch = 0x01;
		rf_24G_deep_time_ms = 3000;
	}
}

/******************************************************************
 * @brief  rcu_main_loop
 * @param  none
 * @return none
 * @retval void
 */
void rcu_main_loop(void)
{
#if (GSENSOR_ENABLE)
	Gsensor_loop();
#endif
#if (GSENSOR_DA217_ENABLE)
	sensor_event_handler();
#endif
#if (UI_LED_ENABLE)
	// gpio_write(GPIO_LED_2,LED_ON_LEVAL);
#endif
#if (UI_KEYBOARD_ENABLE)
	if (app_rcu_batt_mv > VBAT_VOL_NONE)
	{
		proc_keyboard(0, 0, 0);
		keyscan_scan_func();
	}
#endif
#if (PM_APP_ENABLE)
	pm_app_proc();
#endif
#if (IR_MODULE_ENABLE)
	if (!ir_hw_initialed)
	{
		ir_hw_initialed = 1;
		ir_driver_init();
	}
	if (app_led_2_blink_timer == 0)
	{
		if (ir_send_ctrl.is_sending)
		{
			if (factory_mode_flag != 1)
			{
				led_2_driver_init();
				gpio_write(GPIO_LED_2, LED_ON_LEVAL);
			}
		}
	}
#endif
#if (CHARGE_ENABLE)
	irqb_scan_func();
	if (user_led_blink_flag == 0x01)
		led_breathing_light();
	if (user_led_on_flag == 0x01)
		gpio_write(GPIO_LED_1, LED_ON_LEVAL);
#endif
#if (TOUCH_ENABLE)
	if (app_rcu_batt_mv > VBAT_VOL_NONE)
	{
		if (app_rcu_main_init_flag)
			FT3308_loop();
	}
#endif
#if (FACTORY_ENABLE)
	if (factory_mode_flag == 0x02)
		gpio_write(GPIO_LED_2, LED_ON_LEVAL);
#endif

#if (STUCK_KEY_PROCESS_ENABLE)
	const unsigned int Kb_Drive_Pins_RCU[KB_DRIVE_NUM] = KB_DRIVE_PINS;
	extern u32 stuckkey_keypresstimer;
	extern u32 drive_pins[];
	if ((key_not_released && stuckkey_keypresstimer && clock_time_exceed(stuckkey_keypresstimer, STUCK_KEY_ENTERDEEP_TIME * 1000000)) ||
		(stuck_key_release_check(0x02) && stuck_earse_sleep_flag && stuck_earse_sleep_time && clock_time_exceed(stuck_earse_sleep_time, (STUCK_KEY_ENTERDEEP_TIME - 3) * 1000000)))
	{
		printf("stuck key\r\n");
		stuckkey_keypresstimer = 0;
		stuck_earse_sleep_time = 0;
		u8 len = sizeof(Kb_Drive_Pins_RCU) / sizeof(Kb_Drive_Pins_RCU[0]);
		for (int i = 0; i < len; i++)
		{
			extern u8 stuckKeyPress[];
			if (stuckKeyPress[i])
			{
				printf("stuck key i = %d\r\n", i);
				cpu_set_gpio_wakeup(drive_pins[i], KB_LINE_HIGH_VALID, 1);
			}
			else
			{
				cpu_set_gpio_wakeup(drive_pins[i], KB_LINE_HIGH_VALID, 0);
			}
		}
		if (key_power_flag)
		{
			key_power_flag = 0;
			cpu_set_gpio_wakeup(GPIO_PA5, Level_High, 1);
			cpu_set_gpio_wakeup(GPIO_PA7, Level_High, 1);
		}
		get_data_report_release();
		// printf("Enter Sleep!!!\r\n");
		// cpu_sleep_wakeup(DEEPSLEEP_MODE,PM_WAKEUP_PAD,0);
		printf("into deep fail\r\n");
		app_goto_sleep_timer = clock_time();
	}
	if (stuck_earse_sleep_flag && stuckkey_keypresstimer)
	{
		stuck_earse_sleep_flag = 0;
		stuck_earse_sleep_time = 0;
		stuck_ear_sleep_ck_time = 0;
	}
#endif

#if (CHARGE_ENABLE)
	if (app_rcu_charge_check_timer && clock_time_exceed(app_rcu_charge_check_timer, 500 * 1000))
	{ // 500ms
		app_rcu_charge_check_timer = 0;
		app_check_charge_batt();
	}
#endif

	if (app_rcu_main_init_timer && clock_time_exceed(app_rcu_main_init_timer, 500 * 1000))
	{ // 500ms
#if (TOUCH_ENABLE)
		FT3308_iic_init();
#endif
		app_rcu_main_init_timer = 0;
		app_rcu_main_init_flag = 1;
	}
	if (app_rcu_kb_pressed_timer && clock_time_exceed(app_rcu_kb_pressed_timer, 500 * 1000))
	{ // 500ms
		app_rcu_kb_pressed_timer = clock_time() | 1;
		app_rcu_kb_pressed_flag = 1;
	}
	if (pair_flag && app_led_blink_timer && clock_time_exceed(app_led_blink_timer, app_led_blink_time_ms * 1000))
	{
		app_led_blink_timer = 0;
		app_led_blink_timer = clock_time() | 1;
		static bool led_blink_flag = false;
		led_blink_flag = !led_blink_flag;
		if (factory_mode_flag)
		{
			if (led_blink_flag)
				gpio_write(GPIO_LED_2, LED_ON_LEVAL);
			else
				gpio_write(GPIO_LED_2, !LED_ON_LEVAL);
		}
		else
		{
			if (led_blink_flag)
				gpio_write(GPIO_LED_1, LED_ON_LEVAL);
			else
				gpio_write(GPIO_LED_1, !LED_ON_LEVAL);
		}
	}
	if (app_notify_dongleOTA && app_led_blink_timer && clock_time_exceed(app_led_blink_timer, app_led_blink_time_ms * 1000))
	{
		led_blink_cnt++;
		if (led_blink_cnt <= 5)
			app_led_blink_timer = clock_time() | 1;
		else
		{
			app_led_blink_timer = 0;
			led_blink_cnt = 0;
			app_notify_dongleOTA = 0;
		}
		static bool led_blink_flag = false;
		led_blink_flag = !led_blink_flag;
		if (led_blink_flag)
			gpio_write(GPIO_LED_1, LED_ON_LEVAL);
		else
			gpio_write(GPIO_LED_1, !LED_ON_LEVAL);

		if (led_blink_cnt == 0)
		{
			if (notify_dongleOTA_flag == NOTIFY_RCU_DFU_OTA)
			{
				printf("********** DFU OTA Waiting !!! **********\r\n");
				notify_dongleOTA_flag = 0x00;
				gpio_write(GPIO_LED_1, LED_ON_LEVAL);
				rcu_ota_dfu_flag = 0x01;
				app_ota_dfu_wait_timer = clock_time() | 1;
			}
		}
	}
	if (app_led_on_timer && clock_time_exceed(app_led_on_timer, app_led_on_time_ms * 1000))
	{
		app_led_on_timer = 0;
		gpio_write(GPIO_LED_1, !LED_ON_LEVAL);
	}
	if (app_rcu_24g_pair_timer && clock_time_exceed(app_rcu_24g_pair_timer, 30 * 1000 * 1000))
	{
		app_rcu_24g_pair_timer = 0;
		rf_24G_deep_tick = clock_time();
		app_led_blink_timer = 0;
#if (FACTORY_ENABLE)
		if (factory_mode_flag)
			gpio_write(GPIO_LED_2, !LED_ON_LEVAL);
		else
			gpio_write(GPIO_LED_1, !LED_ON_LEVAL);
		factory_mode_flag = 0;
#else
		gpio_write(GPIO_LED_1, !LED_ON_LEVAL);
#endif
		pair_flag = 0;
		analog_write(DEEP_ANA_REG1, 0);
	}
	if (app_ship_mode_led && app_led_blink_timer && clock_time_exceed(app_led_blink_timer, app_led_blink_time_ms * 1000))
	{
#if (CHARGE_ENABLE)
		led_blink_cnt++;
		if (led_blink_cnt <= 5)
			app_led_blink_timer = clock_time() | 1;
		else
		{
			app_led_blink_timer = 0;
			led_blink_cnt = 0;
			app_ship_mode_led = 2;
		}
		static bool led_blink_flag = false;
		led_blink_flag = !led_blink_flag;
		if (led_blink_flag)
			gpio_write(GPIO_LED_2, LED_ON_LEVAL);
		else
			gpio_write(GPIO_LED_2, !LED_ON_LEVAL);

		if (app_ship_mode_led == 2)
			enter_ship_mode();
#endif
	}
	if (app_led_2_blink_timer && clock_time_exceed(app_led_2_blink_timer, app_led_2_blink_time_ms * 1000))
	{
		led_blink_cnt++;
		if (led_blink_cnt <= 5)
			app_led_2_blink_timer = clock_time() | 1;
		else
		{
			app_led_2_blink_timer = 0;
			led_blink_cnt = 0;
		}
		static bool led2_blink_flag = false;
		led2_blink_flag = !led2_blink_flag;
		if (led2_blink_flag)
			gpio_write(GPIO_LED_2, LED_ON_LEVAL);
		else
			gpio_write(GPIO_LED_2, !LED_ON_LEVAL);
	}
	if (app_goto_sleep_timer && clock_time_exceed(app_goto_sleep_timer, 100 * 1000))
	{
		app_goto_sleep_timer = 0;
		printf("Enter Sleep!!!\r\n");
		cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);
	}
	if (app_ota_dfu_wait_timer && clock_time_exceed(app_ota_dfu_wait_timer, 1 * 1000 * 1000)) // RCU_DFU_OTA_TIME_MS
	{
		wait_count++;
		if (wait_count >= 120) // 2min
		{
			wait_count = 0;
			app_ota_dfu_wait_timer = 0;
			printf("********** DFU OTA Waiting TimeOut !!! **********\r\n");
			led_1_driver_init();
			gpio_write(GPIO_LED_1, !LED_ON_LEVAL);
			rcu_ota_dfu_flag = 0x00;
			rf_24G_deep_tick = clock_time();
		}
		else
		{
			app_ota_dfu_wait_timer = clock_time() | 1;
			printf("********** DFU OTA Waiting Count: %d **********\r\n", wait_count);
			report_ota_status(0x00);
		}
	}
	if (app_ota_dfu_download_timer && clock_time_exceed(app_ota_dfu_download_timer, 250 * 1000))
	{
		download_count++;
		download_timeout++;
		if (download_count >= 720) // 3min
		{
			download_count = 0;
			app_ota_dfu_download_timer = 0;
			printf("********** DFU OTA DownLoad TimeOut !!! **********\r\n");
			led_1_driver_init();
			gpio_write(GPIO_LED_1, !LED_ON_LEVAL);
			rcu_ota_dfu_flag = 0x00;
			rf_24G_deep_tick = clock_time();
		}
		else
			app_ota_dfu_download_timer = clock_time() | 1;

		if (download_timeout >= 2)
		{
			download_timeout = 0;
			report_ota_status(0x01);
		}

		static bool led_blink_flag1 = false;
		led_blink_flag1 = !led_blink_flag1;
		if (led_blink_flag1)
			gpio_write(GPIO_LED_1, LED_ON_LEVAL);
		else
			gpio_write(GPIO_LED_1, !LED_ON_LEVAL);
	}
	if (app_heartbeat_delay_timer && clock_time_exceed(app_heartbeat_delay_timer, 100 * 1000))
	{
		app_heartbeat_delay_timer = clock_time() | 1;
		get_data_report_heartbeat();
	}
	if (app_report_info_timer && clock_time_exceed(app_report_info_timer, 1000 * 1000))
	{
		pair_report_info++;
		if (pair_report_info == 1)
		{
			app_report_info_timer = clock_time() | 1;
			get_data_report_device_fwversion();
		}
		else if (pair_report_info >= 2)
		{
			pair_report_info = 0;
			app_report_info_timer = 0;
			get_data_report_device_mac();
		}
	}
	if (app_factory_mode_timer && clock_time_exceed(app_factory_mode_timer, 100 * 1000)) // 2min
	{
		factory_report_cnt++;
		if (factory_report_cnt >= 1200)
		{
			factory_report_cnt = 0;
			printf("Exit Factory Mode!!!\r\n");
			app_factory_mode_timer = 0;
			gpio_write(GPIO_LED_2, !LED_ON_LEVAL);
			factory_mode_flag = 0;
			rf_24G_deep_tick = clock_time();
		}
		else
		{
			app_factory_mode_timer = clock_time() | 1;
			if (factory_report_ram_flag == 0)
				report_factory_status(0x00, raw_buff);
			else
			{
				// factory_report_ram_flag = 0;
				report_factory_status(0x01, raw_buff);
			}
		}
	}
	if (app_rcu_cache_data_timer && clock_time_exceed(app_rcu_cache_data_timer, 40 * 1000))
	{
		app_rcu_cache_data_timer = 0;
		kb_event.keycode[0] = cache_data_key0;
		kb_event.keycode[1] = cache_data_key1;
		kb_event.cnt = cache_data_keycnt;
		app_rcu_kb_pressed_flag = 1;
	}
	if (app_rcu_cache_heart_timer && clock_time_exceed(app_rcu_cache_heart_timer, 1000 * 1000))
	{
		app_rcu_cache_heart_timer = 0;
		if (rcu_heartbeat_flag == 2)
			app_heartbeat_delay_timer = clock_time();
	}
	if (app_tx_send_data_timer && clock_time_exceed(app_tx_send_data_timer, app_tx_send_data_timer_ms * 1000))
	{
		app_tx_send_data_timer = 0;
	}
}

/******************************************************************
 * @brief  keyboard_rcu_loop
 * @param  none
 * @return none
 * @retval void
 */
void keyboard_rcu_loop(void)
{
#if (BATT_CHECK_ENABLE)
	user_batt_check_proc();
#endif
	get_data_report_aaa();
	idle_status_poll();

	if ((connect_ok == 0))
	{
		adv_count_poll();
	}
}

/******************************************************************
 * @brief  d24g_rf_loop
 * @param  none
 * @return none
 * @retval void
 */
void d24g_rf_loop(void)
{
	u8 *ptr = 0;
	check_rf_fast_setting_time();
	if (rf_state == RF_IDLE_STATUS)
	{
		if (device_status <= STATE_PAIRING)
		{
			// printf("device_status <= STATE_PAIRING\r\n");
			if (user_cfg.rf_vid != U16_MAX)
				set_pair_access_code(0x39517695); // set_pair_access_code(rf_access_code_16to32(user_cfg.rf_vid));
			else
				set_pair_access_code(0x39517695);

			pair_success_flag = 0;
			if (device_status == STATE_PAIRING)
				rf_set_power_level_index(user_cfg.paring_tx_power);

			connect_ok = 0;
			ptr = (u8 *)&rf_pair_buf;
			keyboard_send_need_f = 1; // pairing
			start_rf_tick = 0;
		}
		else if (device_status == STATE_NORMAL)
		{
			// printf("device_status ==STATE_NORMAL\r\n");
#if (AES_METHOD == 1)
			ptr = (u8 *)&rf_km_buf_enc;
#else
			ptr = (u8 *)&rf_km_buf;
#endif
			set_data_access_code(flash_dev_info.dongle_id);
			rf_set_power_level_index(user_cfg.tx_power);

			if (keyboard_send_need_f == 0)
			{
				u8 *p = my_fifo_get(&fifo_km);
				if (p)
				{
					if ((p_km_data->cmd == KB_CMD) || (p_km_data->cmd == MOUSE_CMD && (report_mouse_L_flag || report_mouse_R_flag)))
					{
						if (kb_data_seq <= 0xFF)
							kb_data_seq++;
						else
							kb_data_seq = 0;
						p[3] = kb_data_seq;
					}
					// printf("------>>>>>> [KeyBoard Data]: %1x,%1x,%1x,%1x,%1x,%1x <<<<<<------\n",p[2],p[3],p[4],p[5],p[6],p[7]);
					app_rcu_cache_heart_timer = clock_time();
					start_rf_tick = clock_time() | 1;
					keyboard_send_need_f = 2;
					u8 *tmp = (u8 *)&p[0];
					km_3_c_1_data_t *km_dat1;

					u8 *src = (u8 *)&p_km_data->cmd;
					km_dat1 = (km_3_c_1_data_t *)&src[0];
					memcpy(&km_dat1->km_dat[0], &tmp[2], 6);

					p_km_data->pn_no = 1;
					p_km_data->seq_no++;

#if (AES_METHOD == 1)
					memcpy((u8 *)&p_km_data_enc->cmd, (u8 *)&p_km_data->cmd, sizeof(km_3_c_1_data_t));
					aes_encrypt(private_key, &p_km_data->pn_no, &p_km_data_enc->pn_no);
#endif

#if DATA_3_CHOOSE_1_ENABLE
					u8 *km_dat2, *km_dat3;
					km_dat2 = src + sizeof(km_3_c_1_data_t);
					km_dat3 = km_dat2 + sizeof(km_3_c_1_data_t);
					km_dat1->crc16 = crc16_user(&src[0], sizeof(km_3_c_1_data_t) - 2);
					memcpy(&km_dat2[0], &src[0], sizeof(km_3_c_1_data_t));
					memcpy(&km_dat3[0], &src[0], sizeof(km_3_c_1_data_t));
#endif
				}
			}
		}
#if D24G_OTA_ENABLE_AAA
		else if (device_status == STATE_OTA)
		{
			ptr = (u8 *)&rf_ota_buf;

			set_data_access_code(flash_dev_info.dongle_id);
			rf_set_power_level_index(user_cfg.tx_power);

			if (keyboard_send_need_f == 0)
			{
				u8 *p = my_fifo_get(&fifo_km);

				if (p)
				{
					keyboard_send_need_f = 3;

					u8 *tmp = (u8 *)&p[0];

					p_ota_data->seq_no++;

					memcpy((u8 *)&p_ota_data->pno_no, &tmp[0], 29);
				}
			}
		}
#endif

		if (keyboard_send_need_f)
		{
			rf_state = RF_TX_START_STATUS;
			rf_set_tx_rx_off(); // must add
			device_ack_received = 0;
			rf_stx_to_rx(ptr, rf_rx_timeout_us);
			reg_rf_irq_status = 0xffff;
		}
	}
	else if (rf_state == RF_RX_END_STATUS)
	{
		irq_device_rx();
		check_rf_complet_status();
	}
	else if (rf_state == RF_RX_TIMEOUT_STATUS)
	{
		check_rf_complet_status();
	}
}

void d24g_fyll_loop()
{
	u8 *ptr = 0;
	
	// printf("device_status ==STATE_NORMAL\r\n");
#if (AES_METHOD == 1)
	ptr = (u8 *)&rf_km_buf_enc;
#else
	ptr = (u8 *)&rf_km_buf;
#endif

	u8 *p = my_fifo_get(&fifo_km);
	if (p)
	{
		if ((p_km_data->cmd == KB_CMD) || (p_km_data->cmd == MOUSE_CMD && (report_mouse_L_flag || report_mouse_R_flag)))
		{
			p[3] = kb_data_seq++;
		}
		// printf("------>>>>>> [KeyBoard Data]: %1x,%1x,%1x,%1x,%1x,%1x <<<<<<------\n",p[2],p[3],p[4],p[5],p[6],p[7]);
		app_rcu_cache_heart_timer = clock_time();
		start_rf_tick = clock_time() | 1;
		keyboard_send_need_f = 2;
		u8 *tmp = (u8 *)&p[0];
		km_3_c_1_data_t *km_dat1;

		u8 *src = (u8 *)&p_km_data->cmd;
		km_dat1 = (km_3_c_1_data_t *)&src[0];
		memcpy(&km_dat1->km_dat[0], &tmp[2], 6);

		p_km_data->pn_no = 1;
		p_km_data->seq_no++;

#if (AES_METHOD == 1)
		memcpy((u8 *)&p_km_data_enc->cmd, (u8 *)&p_km_data->cmd, sizeof(km_3_c_1_data_t));
		aes_encrypt(private_key, &p_km_data->pn_no, &p_km_data_enc->pn_no);
#endif

#if DATA_3_CHOOSE_1_ENABLE
		u8 *km_dat2, *km_dat3;
		km_dat2 = src + sizeof(km_3_c_1_data_t);
		km_dat3 = km_dat2 + sizeof(km_3_c_1_data_t);
		km_dat1->crc16 = crc16_user(&src[0], sizeof(km_3_c_1_data_t) - 2);
		memcpy(&km_dat2[0], &src[0], sizeof(km_3_c_1_data_t));
		memcpy(&km_dat3[0], &src[0], sizeof(km_3_c_1_data_t));
#endif
#if __DEBUG__ || 0
		printf(">>>[line:%d] fyll \r\n", __LINE__);
#endif
		if (fyll_sample_send_one_packet(&rf_km_buf.dat, 32))
		{
#if __DEBUG__ || 0
			printf(">>>[line:%d] fyll send success\r\n", __LINE__);
#endif
			// my_fifo_reset(&fifo_km);
			my_fifo_pop(&fifo_km);
		}
	}

#if D24G_OTA_ENABLE_AAA
	else if (device_status == STATE_OTA)
	{
		ptr = (u8 *)&rf_ota_buf;

		set_data_access_code(flash_dev_info.dongle_id);
		rf_set_power_level_index(user_cfg.tx_power);

		if (keyboard_send_need_f == 0)
		{
			u8 *p = my_fifo_get(&fifo_km);

			if (p)
			{
				keyboard_send_need_f = 3;

				u8 *tmp = (u8 *)&p[0];

				p_ota_data->seq_no++;

				memcpy((u8 *)&p_ota_data->pno_no, &tmp[0], 29);
			}
		}
	}
#endif
}

/******************************************************************
 * @brief  rf_rx_process
 * @param  none
 * @return none
 * @retval void
 */
_attribute_ram_code_sec_ u8 rf_rx_process(rf_packet_t *p_rf_data)
{
	if (device_status == STATE_PAIRING)
	{
		pair_ack_data_t *pair_ack_dat_ptr = (pair_ack_data_t *)&p_rf_data->dat[0];
#if (AES_METHOD == 0)
		if ((pair_ack_dat_ptr->cmd == PAIR_ACK_CMD) && (pair_ack_dat_ptr->did == p_pair_dat->did))
#elif (AES_METHOD == 1)
		if ((pair_ack_dat_ptr->cmd == PAIR_ACK_CMD) && (memcmp((u8 *)&pair_ack_dat_ptr->did, (u8 *)&p_pair_dat->did, 16) == 0))
#endif
		{
			dongle_id = pair_ack_dat_ptr->gid;
			pair_success_flag = 1;
			printf("pairing success--------- 0x%04x\n", pair_ack_dat_ptr->gid);
			pair_report_info = 0;
			app_report_info_timer = clock_time() | 1;
			return 1;
		}
	}
	else if (device_status == STATE_NORMAL)
	{
		km_ack_data_t *km_ack_dat_ptr = (km_ack_data_t *)&p_rf_data->dat[0];
		ota_24G_data_t *ota_24g_dat_ptr = (ota_24G_data_t *)&p_rf_data->dat[0];
		factory_24G_data_t *factory_24g_dat_ptr = (factory_24G_data_t *)&p_rf_data->dat[0];
		if ((keyboard_send_need_f == 2)) // ack
		{
			if (km_ack_dat_ptr->cmd == KB_ACK_CMD)
			{
				printf("KB_ACK_CMD cmd come back\r\n");
				if (p_rf_data->dat[1] != kb_data_seq)
					return 1;
				my_fifo_reset(&fifo_km);
				if (pair_flag == 0)
					device_status = STATE_NONE;
				keyboard_send_need_f = 0;

				if (report_release_flag == 0x01)
				{
					report_release_flag = 0x00;
					get_data_report_release();
				}
				// if(report_gesture_flag == 0x01)
				// {
				// 	report_gesture_flag = 0x00;
				// 	report_release_flag = 0x01;
				// 	get_data_report_release();
				// }
				if (rcu_heartbeat_flag)
					rcu_heartbeat_flag = 2; // app_heartbeat_delay_timer = clock_time();
				else
					app_heartbeat_delay_timer = 0;
				app_rcu_cache_data_timer = 0;
				return 1;
			}
			if (km_ack_dat_ptr->cmd == MOUSE_ACK_CMD)
			{
				// printf("MOUSE_ACK_CMD cmd come back\r\n");
				if (report_mouse_L_flag == 0x02 || report_mouse_R_flag == 0x02)
				{
					if (p_rf_data->dat[1] != kb_data_seq)
					{
						keyboard_send_need_f = 0;
						get_mouse_report_move_press(0x00, 0x00, 0x00);
						return 1;
					}
					report_mouse_R_flag = 0x00;
					report_mouse_L_flag = 0x00;
				}
				my_fifo_reset(&fifo_km);
				if (pair_flag == 0)
					device_status = STATE_NONE;
				keyboard_send_need_f = 0;
				if (rcu_heartbeat_flag)
					app_heartbeat_delay_timer = clock_time();
				else
					app_heartbeat_delay_timer = 0;
				return 1;
			}
			if (km_ack_dat_ptr->cmd == DG_OTA_ACK_CMD)
			{
				printf("Dongle OTA cmd come back\r\n");
				my_fifo_reset(&fifo_km);
				if (pair_flag == 0)
					device_status = STATE_NONE;
				keyboard_send_need_f = 0;
				if (ota_24g_dat_ptr->buf[0] == 0x00)
				{
					app_notify_dongleOTA = 2;
					led_blink_cnt = 0;
					app_led_blink_time_ms = 100;
					app_led_blink_timer = clock_time() | 1;
				}
				if (ota_24g_dat_ptr->buf[0] == 0x0A)
				{
					app_notify_dongleOTA = 2;
					app_rcu_kb_pressed_flag = 1;
				}
				return 1;
			}
			if (km_ack_dat_ptr->cmd == D24G_OTA_ACK_CMD)
			{
				// printf("RCU OTA cmd come back\r\n");
				my_fifo_reset(&fifo_km);
				if (pair_flag == 0)
					device_status = STATE_NONE;
				keyboard_send_need_f = 0;

				u8 ota_24g_buff[40];
				for (int i = 0; i < 40; i++)
					ota_24g_buff[i] = 0xff;
				memcpy(ota_24g_buff, &ota_24g_dat_ptr->buf[4], 16 + 16);

				if (rcu_ota_dfu_flag == 0x01)
				{
					rcu_ota_dfu_flag = 0x02;
					app_ota_dfu_wait_timer = 0;
					app_ota_dfu_download_timer = clock_time() | 1;
					ota_dfu_write(0x00, ota_24g_buff);
				}
				else if (rcu_ota_dfu_flag == 0x02)
					ota_dfu_write(0x01, ota_24g_buff);

				if (rcu_ota_dfu_flag == 0x02)
				{
					ota_24g_address = ota_24g_dat_ptr->buf[0] | (ota_24g_dat_ptr->buf[1] << 8) | (ota_24g_dat_ptr->buf[2] << 16) | (ota_24g_dat_ptr->buf[3] << 24);
					if (ota_24g_address == flash_write_addr)
						flash_write_addr += 32;
					download_timeout = 0;
					report_ota_status(0x01);
				}
				if (rcu_ota_dfu_flag == 0x03)
				{
					rcu_ota_dfu_flag = 0x04;
					report_ota_status(0x02);
				}
				else if (rcu_ota_dfu_flag == 0x04)
					ota_dfu_write(0x02, ota_24g_buff);

				return 1;
			}
			if (km_ack_dat_ptr->cmd == FACTORY_TEST_ACK_CMD)
			{
				printf("FACTORY_TEST_ACK_CMD cmd come back\r\n");
				my_fifo_reset(&fifo_km);
				if (pair_flag == 0)
					device_status = STATE_NORMAL;
				u8 recv_buff[20];
				memcpy(recv_buff, &factory_24g_dat_ptr->buf[0], 20);
				printf("factory_24g_dat_ptr->buf: %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x \r\n", recv_buff[0], recv_buff[1], recv_buff[2], recv_buff[3], recv_buff[4], recv_buff[5], recv_buff[6], recv_buff[7], recv_buff[8], recv_buff[9]);
				recv_factory_data(recv_buff);

				return 1;
			}
		}
		return 1;
	}
	return 0;
}

/******************************************************************
 * @brief  get_data_report_aaa
 * @param  none
 * @return none
 * @retval void
 */
u8 get_data_report_aaa(void)
{
	if (rcu_ota_dfu_flag >= 0x02)
		return 0;
	u8 has_new_key_event = 0;
	u8 key_num = kb_event.cnt;
	u8 key0 = kb_event.keycode[0];
	u8 key1 = kb_event.keycode[1];
	if (app_rcu_kb_pressed_flag == 0x01)
	{
		app_rcu_kb_pressed_flag = 0x00;
		has_new_key_event = key_not_released; // matrix_scan_aaa();

		if (has_new_key_event)
		{
			my_fifo_reset(&fifo_km);
			if (pair_flag == 0)
				device_status = STATE_NORMAL;

			p_km_data->cmd = KB_CMD;

			//{HID_USAGE, MODIFIER, KEY_CODE4, KEY_CODE3, KEY_CODE2, KEY_CODE1}
			u8 temp_buff[6] = {0x07, 0x00, 0x00, 0x00, 0x00, 0x00};

			if (key_num == 1)
			{
				temp_buff[0] = HID_USAGE_KEYBOARD;
				temp_buff[1] = 0x00;
				temp_buff[5] = KB_Map_RCU_HID_KB[key0];
			}
			else if (key_num == 2)
			{
				temp_buff[0] = HID_USAGE_KEYBOARD;
				// Disable Key to Face_up_flag
				if (key0 == KEY_OK || key0 == KEY_UP || key0 == KEY_DOWN || key0 == KEY_LEFT || key0 == KEY_RIGHT ||
					key0 == KEY_BACK || key0 == KEY_DB_WINDOWS || key0 == KEY_SEL_BUTTON || key0 == KEY_FLAME || key0 == KEY_FREEZE_SCREEN ||
					key0 == KEY_DISABLE_TOUCH || key0 == KEY_VOLUME_UP || key0 == KEY_VOLUME_DOWN || key0 == KEY_INPUT_SELECT || key0 == KEY_SETTING)
				{
					temp_buff[5] = KB_Map_RCU_HID_KB[key1];
				}
				else if (key1 == KEY_OK || key1 == KEY_UP || key1 == KEY_DOWN || key1 == KEY_LEFT || key1 == KEY_RIGHT ||
						 key1 == KEY_BACK || key1 == KEY_DB_WINDOWS || key1 == KEY_SEL_BUTTON || key1 == KEY_FLAME || key1 == KEY_FREEZE_SCREEN ||
						 key1 == KEY_DISABLE_TOUCH || key1 == KEY_VOLUME_UP || key1 == KEY_VOLUME_DOWN || key1 == KEY_INPUT_SELECT || key1 == KEY_SETTING)
				{
					temp_buff[5] = KB_Map_RCU_HID_KB[key0];
				}
				else
				{
					// FN
					if ((key0 == KEY_FN) || (key1 == KEY_FN))
					{
						if (key0 == KEY_FN)
						{
							switch (key1)
							{
							case KEY_1:
							case KEY_2:
							case KEY_3:
							case KEY_4:
							case KEY_5:
							case KEY_6:
							case KEY_7:
							case KEY_8:
							case KEY_9:
							case KEY_0:
							case KEY_EQUAL:
							case KEY_Q:
							case KEY_T:
							case KEY_Y:
							case KEY_U:
							case KEY_I:
							case KEY_W:
							case KEY_J:
							case KEY_L:
							case KEY_SLASH:
								temp_buff[4] = HID_KB_SHIFT_LEFT;
								temp_buff[5] = KB_Map_RCU_HID_FN[key1];
								break;
							case KEY_P:
							case KEY_E:
							case KEY_R:
							case KEY_O:
							case KEY_H:
							case KEY_K:
							case KEY_PERIOD:
							case KEY_ARROW_UP:
							case KEY_ARROW_DOWN:
							case KEY_ARROW_LEFT:
							case KEY_ARROW_RIGHT:
							case KEY_DELETE:
								temp_buff[5] = KB_Map_RCU_HID_FN[key1];
								break;
							case KEY_ENTER:
								temp_buff[3] = HID_KB_CONTROL_LEFT;
								temp_buff[4] = HID_KB_ALT_LEFT;
								temp_buff[5] = HID_KB_DELETE;
							default:
								break;
							}
						}
						if (key1 == KEY_FN)
						{
							switch (key0)
							{
							case KEY_1:
							case KEY_2:
							case KEY_3:
							case KEY_4:
							case KEY_5:
							case KEY_6:
							case KEY_7:
							case KEY_8:
							case KEY_9:
							case KEY_0:
							case KEY_EQUAL:
							case KEY_Q:
							case KEY_T:
							case KEY_Y:
							case KEY_U:
							case KEY_I:
							case KEY_P:
							case KEY_J:
							case KEY_L:
							case KEY_SLASH:
								temp_buff[4] = HID_KB_SHIFT_LEFT;
								temp_buff[5] = KB_Map_RCU_HID_FN[key0];
								break;
							case KEY_W:
							case KEY_E:
							case KEY_R:
							case KEY_O:
							case KEY_H:
							case KEY_K:
							case KEY_PERIOD:
							case KEY_ARROW_UP:
							case KEY_ARROW_DOWN:
							case KEY_ARROW_LEFT:
							case KEY_ARROW_RIGHT:
							case KEY_DELETE:
								temp_buff[5] = KB_Map_RCU_HID_FN[key0];
								break;
							case KEY_ENTER:
								temp_buff[3] = HID_KB_CONTROL_LEFT;
								temp_buff[4] = HID_KB_ALT_LEFT;
								temp_buff[5] = HID_KB_DELETE;
							default:
								break;
							}
						}
					}
					else
					{
						temp_buff[4] = KB_Map_RCU_HID_KB[key0];
						temp_buff[5] = KB_Map_RCU_HID_KB[key1];
					}
				}
			}

			if (app_notify_dongleOTA == 1)
			{
				// app_notify_dongleOTA = 2;
				p_km_data->cmd = DG_OTA_CMD;
				memset(temp_buff, 0, sizeof(temp_buff));
				temp_buff[0] = notify_dongleOTA_flag;
				temp_buff[1] = FW_VERSION_STR[14] - 0x30;
				temp_buff[2] = FW_VERSION_STR[16] - 0x30;
				temp_buff[3] = FW_VERSION_STR[18] - 0x30;
				temp_buff[4] = 0x0A;
			}
			if (app_notify_dongleOTA == 2)
			{
				app_notify_dongleOTA = 3;
				p_km_data->cmd = DG_OTA_CMD;
				memset(temp_buff, 0, sizeof(temp_buff));
				temp_buff[0] = notify_dongleOTA_flag;
				temp_buff[1] = FW_VERSION_STR[14] - 0x30;
				temp_buff[2] = FW_VERSION_STR[16] - 0x30;
				temp_buff[3] = FW_VERSION_STR[18] - 0x30;
				temp_buff[4] = 0x0B;

				if (app_notify_dongleOTA == 3)
				{
					led_blink_cnt = 0;
					app_led_blink_time_ms = 100;
					app_led_blink_timer = clock_time() | 1;
				}
			}
			memset(keyboard_buf_aaa, 0, sizeof(keyboard_buf_aaa));
			memcpy(keyboard_buf_aaa, &temp_buff, sizeof(keyboard_buf_aaa) - 2);

			rcu_heartbeat_flag = 1;
		}

#if (IS_SINGLE_GPIO_CHANGE_MODE)
		sing_gpio_mode_poll();
#endif
		push_report_fifo();

		if ((device_status == STATE_NORMAL))
		{
			if (has_new_key_event)
			{
				has_new_key_event = 0;
				reset_idle_status();
				my_fifo_push(&fifo_km, keyboard_buf_aaa, 6);
			}
			else if ((idle_count < 3) || key_buf_aaa.press_cnt)
			{
				u8 *p = my_fifo_get(&fifo_km);
				if (p == 0)
					my_fifo_push(&fifo_km, keyboard_buf_aaa, 6);
			}
			cache_data_key0 = key0;
			cache_data_key1 = key1;
			cache_data_keycnt = kb_event.cnt;
			app_rcu_cache_data_timer = clock_time();
		}
	}
	return has_new_key_event;
}
void get_data_report_release(void)
{
	if (rcu_ota_dfu_flag >= 0x02)
		return;
	my_fifo_reset(&fifo_km);
	if (pair_flag == 0)
		device_status = STATE_NORMAL;
	p_km_data->cmd = KB_CMD;
	u8 temp_buff[6] = {0x07, 0x00, 0x00, 0x00, 0x00, 0x00};
	memset(keyboard_buf_aaa, 0, sizeof(keyboard_buf_aaa));
	memcpy(keyboard_buf_aaa, &temp_buff, sizeof(keyboard_buf_aaa) - 2);

#if (IS_SINGLE_GPIO_CHANGE_MODE)
	sing_gpio_mode_poll();
#endif
	push_report_fifo();

	if ((device_status == STATE_NORMAL))
	{
		reset_idle_status();
		my_fifo_push(&fifo_km, keyboard_buf_aaa, 6);
	}
	app_heartbeat_delay_timer = 0;
	rcu_heartbeat_flag = 0;
}
void get_data_report_gesture(u8 gesture_type, bool gesture_fast)
{
#if (TOUCH_ENABLE)
	if (rcu_ota_dfu_flag >= 0x02)
		return;
	if (Face_up_flag)
	{
		my_fifo_reset(&fifo_km);
		if (pair_flag == 0)
			device_status = STATE_NORMAL;
		report_gesture_flag = 0x01;
		p_km_data->cmd = KB_CMD;
		u8 temp_buff[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		temp_buff[0] = 0x0E;
		temp_buff[3] = 0xFF;
		temp_buff[4] = gesture_type;
		temp_buff[5] = gesture_fast;

		memset(keyboard_buf_aaa, 0, sizeof(keyboard_buf_aaa));
		memcpy(keyboard_buf_aaa, &temp_buff, sizeof(keyboard_buf_aaa) - 2);

#if (IS_SINGLE_GPIO_CHANGE_MODE)
		sing_gpio_mode_poll();
#endif
		push_report_fifo();

		if ((device_status == STATE_NORMAL))
		{
			reset_idle_status();
			my_fifo_push(&fifo_km, keyboard_buf_aaa, 6);
		}
		// rcu_heartbeat_flag = 1;
	}
#endif
}
void get_data_report_heartbeat(void)
{
	if (rcu_ota_dfu_flag >= 0x02)
		return;
	my_fifo_reset(&fifo_km);
	if (pair_flag == 0)
		device_status = STATE_NORMAL;
	p_km_data->cmd = HEART_BEAT_CMD;
	u8 temp_buff[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	memset(keyboard_buf_aaa, 0, sizeof(keyboard_buf_aaa));
	memcpy(keyboard_buf_aaa, &temp_buff, sizeof(keyboard_buf_aaa) - 2);

#if (IS_SINGLE_GPIO_CHANGE_MODE)
	sing_gpio_mode_poll();
#endif
	push_report_fifo();

	if ((device_status == STATE_NORMAL))
	{
		reset_idle_status();
		my_fifo_push(&fifo_km, keyboard_buf_aaa, 6);
	}
}
void get_data_report_device_fwversion(void)
{
	if (rcu_ota_dfu_flag >= 0x02)
		return;
	my_fifo_reset(&fifo_km);
	if (pair_flag == 0)
		device_status = STATE_NORMAL;
	p_km_data->cmd = RCU_FW_CMD;
	u8 temp_buff[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	temp_buff[0] = FW_VERSION_STR[14] - 0x30;
	temp_buff[1] = FW_VERSION_STR[16] - 0x30;
	temp_buff[2] = FW_VERSION_STR[18] - 0x30;
	memset(keyboard_buf_aaa, 0, sizeof(keyboard_buf_aaa));
	memcpy(keyboard_buf_aaa, &temp_buff, sizeof(keyboard_buf_aaa) - 2);

#if (IS_SINGLE_GPIO_CHANGE_MODE)
	sing_gpio_mode_poll();
#endif
	push_report_fifo();

	if ((device_status == STATE_NORMAL))
	{
		reset_idle_status();
		my_fifo_push(&fifo_km, keyboard_buf_aaa, 6);
	}
}
void get_data_report_device_mac(void)
{
	if (rcu_ota_dfu_flag >= 0x02)
		return;
	my_fifo_reset(&fifo_km);
	if (pair_flag == 0)
		device_status = STATE_NORMAL;
	p_km_data->cmd = RCU_MAC_CMD;
	u8 temp_buff[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	u8 mac_public[6];
	// u8 mac_random_static[6];
	// blc_initMacAddress(CFG_ADR_MAC, mac_public, mac_random_static);
	flash_read_page(CFG_ADR_MAC, 6, mac_public);
	for (int i = 0; i < 6; i++)
		temp_buff[i] = mac_public[5 - i];
	memset(keyboard_buf_aaa, 0, sizeof(keyboard_buf_aaa));
	memcpy(keyboard_buf_aaa, &temp_buff, sizeof(keyboard_buf_aaa) - 2);

#if (IS_SINGLE_GPIO_CHANGE_MODE)
	sing_gpio_mode_poll();
#endif
	push_report_fifo();

	if ((device_status == STATE_NORMAL))
	{
		reset_idle_status();
		my_fifo_push(&fifo_km, keyboard_buf_aaa, 6);
	}
	if (factory_mode_flag)
	{
		factory_mode_flag = 2;
		app_factory_mode_timer = clock_time() | 1;
		printf("Enter Factory Mode #####\r\n");
	}
}

/******************************************************************
 * @brief  get_mouse_report_move_press
 * @param  none
 * @return none
 * @retval void
 */
void get_mouse_report_move_press(u8 ts_press, s16 ts_x, s16 ts_y)
{
	if (rcu_ota_dfu_flag >= 0x02)
		return;

#if (FACTORY_ENABLE)
	if (factory_mode_flag)
	{
		if (factory_Gsensor_Flag == 0x00)
		{
			my_fifo_reset(&fifo_km);
			if (pair_flag == 0)
				device_status = STATE_NORMAL;
			p_km_data->cmd = MOUSE_CMD;
			u8 temp_buff[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
			temp_buff[0] = HID_USAGE_MOUSE;
			temp_buff[3] = 0x00;
			temp_buff[4] = ts_x;
			temp_buff[5] = ts_y;
			memset(keyboard_buf_aaa, 0, sizeof(keyboard_buf_aaa));
			memcpy(keyboard_buf_aaa, &temp_buff, sizeof(keyboard_buf_aaa) - 2);

#if (IS_SINGLE_GPIO_CHANGE_MODE)
			sing_gpio_mode_poll();
#endif
			push_report_fifo();

			if ((device_status == STATE_NORMAL))
			{
				reset_idle_status();
				my_fifo_push(&fifo_km, keyboard_buf_aaa, 6);
			}
		}
		else if (factory_Gsensor_Flag == 0x01)
		{
			if (Face_up_flag)
			{
				my_fifo_reset(&fifo_km);
				if (pair_flag == 0)
					device_status = STATE_NORMAL;
				p_km_data->cmd = MOUSE_CMD;
				u8 temp_buff[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
				temp_buff[0] = HID_USAGE_MOUSE;
				temp_buff[3] = 0x00;
				temp_buff[4] = ts_x;
				temp_buff[5] = ts_y;
				memset(keyboard_buf_aaa, 0, sizeof(keyboard_buf_aaa));
				memcpy(keyboard_buf_aaa, &temp_buff, sizeof(keyboard_buf_aaa) - 2);

#if (IS_SINGLE_GPIO_CHANGE_MODE)
				sing_gpio_mode_poll();
#endif
				push_report_fifo();

				if ((device_status == STATE_NORMAL))
				{
					reset_idle_status();
					my_fifo_push(&fifo_km, keyboard_buf_aaa, 6);
				}
			}
		}
		return;
	}
#endif

	if (Face_up_flag)
	{
		if (report_mouse_L_flag == 0x01 || report_mouse_R_flag == 0x01)
		{
			if (report_mouse_L_flag)
				ts_press = 0x01;
			if (report_mouse_R_flag)
				ts_press = 0x02;
			rcu_heartbeat_flag = 1;
		}
		else
		{
			app_heartbeat_delay_timer = 0;
			rcu_heartbeat_flag = 0;
		}

		my_fifo_reset(&fifo_km);
		if (pair_flag == 0)
			device_status = STATE_NORMAL;
		p_km_data->cmd = MOUSE_CMD;

		//{button, x, y, wheel}
		u8 temp_buff[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		temp_buff[0] = HID_USAGE_MOUSE;
		temp_buff[3] = ts_press;
		temp_buff[4] = ts_x;
		temp_buff[5] = ts_y;
		memset(keyboard_buf_aaa, 0, sizeof(keyboard_buf_aaa));
		memcpy(keyboard_buf_aaa, &temp_buff, sizeof(keyboard_buf_aaa) - 2);

#if (IS_SINGLE_GPIO_CHANGE_MODE)
		sing_gpio_mode_poll();
#endif
		push_report_fifo();

		if ((device_status == STATE_NORMAL))
		{
			reset_idle_status();
			my_fifo_push(&fifo_km, keyboard_buf_aaa, 6);
		}
	}
}

/******************************************************************
 * @brief  push_report_fifo
 * @param  none
 * @return none
 * @retval void
 */
void push_report_fifo(void)
{
	TX_PACKET_AAA kb_dat;

#if CONSUMER_FUN_ENABLE_AAA
	if (has_new_report_aaa & HAS_CONSUMER_REPORT)
	{
		kb_dat.type = HAS_CONSUMER_REPORT;
		kb_dat.len = BUF_SIZE_CONSUMER_AAA;
		kb_dat.buf[0] = consumer_buf_aaa;

		if (!my_fifo_push(&fifo_km, (u8 *)(&kb_dat), sizeof(TX_PACKET_AAA)))
		{
			has_new_report_aaa &= ~HAS_CONSUMER_REPORT;
		}
	}
#endif

#if KEYBOARD_FUN_ENABLE_AAA
	if (has_new_report_aaa & HAS_KEYBOARD_REPORT)
	{
		kb_dat.type = HAS_KEYBOARD_REPORT;
		kb_dat.len = BUF_SIZE_KEYBOARD_AAA;
		memcpy(&kb_dat.buf[0], keyboard_buf_aaa, BUF_SIZE_KEYBOARD_AAA);

		if (!my_fifo_push(&fifo_km, (u8 *)(&kb_dat), sizeof(TX_PACKET_AAA)))
		{
			has_new_report_aaa &= ~HAS_KEYBOARD_REPORT;
		}
	}
#endif

#if SYSTEM_KEY_FUN_ENABLE_AAA
	if (has_new_report_aaa & HAS_SYSYTEM_REPORT)
	{
		kb_dat.type = HAS_SYSYTEM_REPORT;
		kb_dat.len = 1;
		kb_dat.buf[0] = system_buf_aaa;

		if (!my_fifo_push(&fifo_km, (u8 *)(&kb_dat), sizeof(TX_PACKET_AAA)))
		{
			has_new_report_aaa &= ~HAS_SYSYTEM_REPORT;
		}
	}
#endif
}

bool stuck_key_release_check(u8 chose_mode)
{
	extern u8 stuckKeyPress[];
	u32 pin[] = KB_DRIVE_PINS;
	u8 gpio_read_cnt = 0;
	for (int i = 0; i < (sizeof(pin) / sizeof(*pin)); i++)
	{
		if (gpio_read(pin[i]))
		{
			if (chose_mode == 0x01)
			{
				gpio_read_cnt++;
				stuck_ear_sleep_ck_time = 0;
				stuck_earse_sleep_flag = 1;
				stuck_earse_sleep_time = clock_time();
				// printf("--->>> stuck_earse_sleep_flag mode one\r\n");
			}
			else if (chose_mode == 0x02)
			{
				gpio_read_cnt++;
				// printf("--->>> stuck_earse_sleep check mode two\r\n");
			}
			stuckKeyPress[i] = 1;
		}
		else
		{
			stuckKeyPress[i] = 0;
		}
	}

	return gpio_read_cnt;
}

/**
 * @brief      power management code for application
 * @param[in]  none
 * @return     none
 */
void pm_app_proc(void)
{

	if (stuck_earse_sleep_flag)
	{
		if (!stuck_key_release_check(0x02) && stuck_ear_sleep_ck_time == 0)
			stuck_ear_sleep_ck_time = clock_time();
	}

#if (PM_APP_ENABLE)
	need_suspend_flag = 1;
	if ((rf_24G_deep_tick && clock_time_exceed(rf_24G_deep_tick, rf_24G_deep_time_ms * 1000)) ||
		(stuck_ear_sleep_ck_time && clock_time_exceed(stuck_ear_sleep_ck_time, rf_24G_deep_time_ms * 1000)))
	{
		rf_24G_deep_tick = 0;
		if ((need_suspend_flag == 0x01) && (key_not_released == 0) && (pair_flag == 0) && (rcu_ota_dfu_flag == 0) && (factory_mode_flag == 0))
		{
#if (CHARGE_ENABLE)
			if (app_rcu_charge_flag == true)
			{
				rf_24G_deep_tick = clock_time();
				return;
			}
#endif
#if (TOUCH_ENABLE)
			if (ft_touch_sleep_flag == true)
			{
				rf_24G_deep_tick = clock_time();
				return;
			}
#endif
#if (TOUCH_ENABLE)
			FT3308_sleep_config();
			if (rcu_wakeup_touch)
			{
				if (stuck_key_release_check(0x01))
					return;

#if (GSENSOR_DA217_ENABLE)
				cpu_set_gpio_wakeup(DA217_INT1, Level_Low, 0);
#endif
				gpio_setup_up_down_resistor(FT3308_CHIP_DRIVER_PIN, PM_PIN_PULLDOWN_100K); // touch ic monitor
				printf("DEEPSLEEP_MODE_RET_SRAM_LOW16K --- PM_WAKEUP_PAD / PM_WAKEUP_TIMER\r\n");
				cpu_sleep_wakeup(DEEPSLEEP_MODE_RET_SRAM_LOW16K, PM_WAKEUP_PAD | PM_WAKEUP_TIMER, clock_time() + 60 * CLOCK_16M_SYS_TIMER_CLK_1S); // 5min
			}
			else
			{
#if (GSENSOR_DA217_ENABLE)
				cpu_set_gpio_wakeup(DA217_INT1, Level_Low, 1);
#endif
				gpio_setup_up_down_resistor(FT3308_CHIP_DRIVER_PIN, PM_PIN_UP_DOWN_FLOAT); // touch ic sleep mode
				printf("DEEPSLEEP_MODE_RET_SRAM_LOW16K --- PM_WAKEUP_PAD\r\n");
				cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0); // deepSleep
			}
#else
			printf("cpu_sleep_wakeup(DEEPSLEEP_MODE)\r\n");
			cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0); // deepSleep
#endif
		}
	}
#endif
}

/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
