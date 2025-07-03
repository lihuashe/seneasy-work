#ifndef __APP_KEYBOARD_H
#define __APP_KEYBOARD_H

#include "board_config.h"
#include "key_scan.h"

#define HID_TYPE_KEYBOARD              0x07
#define HID_TYPE_COMSUMER              0x0C

#define IDLE_KEY                0
#define CONSUMER_KEY            BIT_MASK(0)
#define KEYBOARD_KEY            BIT_MASK(1)
#define IR_KEY                  BIT_MASK(2)
#define MOUSE_KEY				BIT_MASK(3)




/**
 * @brief  按键键码表
 */
typedef struct {
	unsigned char key_index;				/**按键索引*/
    unsigned char key_types;				/**按键类型 红外键/蓝牙键/红外&蓝牙键*/
    unsigned char ir_code;                  /**红外码值*/
    unsigned char hid_type;                 /**蓝牙类型 多媒体/红外*/
    unsigned short ble_key;                 /**蓝牙码值*/
}KEY_TABLE_T;

typedef struct {
    unsigned char key_type;
    unsigned char key_not_released;
	unsigned char key_pressed_count;
	unsigned char power_adv_key_cnt;
	unsigned char netflix_adv_key_cnt;
	unsigned char google_adv_key_cnt;
	unsigned char stuck_flag;
    KB_DATA_T 	last_kb_event;
}KEYBOARD_CTL_T;


extern KB_DATA_T	kb_event;
extern KEYBOARD_CTL_T g_keyboard_ctl;

void key_task_func(KEY_SCAN_EVENT_T event, const KB_DATA_T data);
bool check_pair_comb_key(void);
void remote_pair_key_task(unsigned char remode_mode);
void remote_pair_handle(unsigned char remode_mode);
void disconnect_rematch_pair(void);
void pairing_disconnect_rematch(void);

void kyemissing_disconnect_delay_start(void);
void keymissing_timer_close(void);
void power_on_auido_type_check(void);


#if (AUTO_PAIR_ENABLE)
bool check_auto_pair_key(void);
#endif

#endif	//__APP_KEYBOARD_H
