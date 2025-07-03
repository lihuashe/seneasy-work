#include "app_keyboard.h"
#include "peripheral.h"
#include "key_scan.h"
#include "app_gap.h"
#include "app_mode.h"
#include "hid_service.h"
#include "app_led.h"
#include "app_batt.h"
#include "hw_radio.h"

#if (BLE_APP_AUDIO_TYPE & GOOGLE_AUDIO_TYPE)
#include "app_atv.h"
#endif

#if (REMOTE_IR_ENABLE)
#include "app_ir.h"
#endif

#define APP_LOG_DOMAIN      "kpro"
#define APP_LOG_LEVEL       APP_KEY_LOG_LEVEL
#include "co_debug.h"


KB_DATA_T 	kb_event;
KEYBOARD_CTL_T 	g_keyboard_ctl;

#if (BLE_APP_AUDIO_TYPE)
static co_timer_t key_release_sleep_timer;	
#endif		
static co_timer_t keymissing_disconnect_timer;	
static co_timer_t remote_stuck_key_timer;

/*按键码值表*/
static const KEY_TABLE_T key_tb[] = KB_MAP_CODE;

static co_timer_t act_code_interval_send_timer;
static void act_code_interval_send_func(co_timer_t *timer, void *param);

#if (BLE_APP_AUDIO_TYPE)
static void key_release_sleep_handle(co_timer_t *timer, void *param);
#endif

void key_auido_release_handle(void)
{
	#if (BLE_APP_AUDIO_TYPE & GOOGLE_AUDIO_TYPE)
	if (BLE_APP_AUDIO_TYPE & GOOGLE_AUDIO_TYPE)
	{
		google_key_release();
	}
	#endif

	if (g_keyboard_ctl.last_kb_event.cnt == 1
		&& (g_keyboard_ctl.last_kb_event.keycode[0] == VOICE_AUDIO_KEY_INDEX 
	|| g_keyboard_ctl.last_kb_event.keycode[0] == OK_KEY_INDEX))
	{
		co_timer_set(&key_release_sleep_timer, 500, TIMER_ONE_SHOT,
		key_release_sleep_handle, NULL);
	}
}

bool key_auido_press_handle(void)
{
	#if (BLE_APP_AUDIO_TYPE & GOOGLE_AUDIO_TYPE)
	if ((BLE_APP_AUDIO_TYPE & GOOGLE_AUDIO_TYPE) && (g_board_status.app_audio_type == GOOGLE_AUDIO_TYPE))
	{
		google_voice_start();
		return true;
	}
	#endif
	
	return false;
}


/**
 * @brief		单键的处理
 * @param[in]	none
 * @return      none
 */
void one_key_handle(unsigned char key_id)
{
	APP_LOG_INF_FUNC();
	
	KEY_TABLE_T *key = (KEY_TABLE_T *)&key_tb[key_id];

	// send ble key
	if(gap_get_connected_status() && key->key_types & KEY_TYPE_BLE && 
		get_remote_mode() != REMOTE_PAIR_WAIT 
	)
	{

#if (REMOTE_BATT_ENABLE)
		if (g_board_status.ble_low_protect)
		{
			APP_LOG_INF("not send ble, low protect\r\n");
			return ;
		}
#endif		
		
	#if (BLE_APP_AUDIO_TYPE & GOOGLE_AUDIO_TYPE)
	if ((key_id == VOICE_AUDIO_KEY_INDEX || key_id == OK_KEY_INDEX) && !g_board_status.ota_flag)
	{
		remote_latency_disable(KEY_LATENCY_TYPE,true);
	}
	#endif
    if (key_id == VOICE_AUDIO_KEY_INDEX && !g_board_status.ota_flag)
    {
			//语音按键处理
		if (key_auido_press_handle())
		{
			return;
		}
    }
		
		//如果有code 码为0,可以注释
		if(0 == key->ble_key && key_id != VOICE_AUDIO_KEY_INDEX)
		{
			return;
		}
		if (key->ble_key)
		{
#if (BLT_APP_LED_ENABLE & LED1_LED_FLAG)
			device_led_setup(led_cfg[LED_BLE_PRESS]);
#endif
			#if 0
			if(HID_TYPE_KEYBOARD == key->hid_type)
			{
				g_keyboard_ctl.key_type |= KEYBOARD_KEY;
				unsigned char key_buf[8] = {0,0,0,0,0,0,0,0};
				key_buf[2] = key->ble_key;
				
				hid_gatt_report_notify(KEYBOARD_HANDLE, key_buf, 8);
				APP_LOG_INF("send ble:0x07 %x\r\n",key->ble_key);
			}
			else
			{
				g_keyboard_ctl.key_type |= CONSUMER_KEY;
				hid_gatt_report_notify(CONSUMER_HANDLE,  (unsigned char *)&key->ble_key, 2);
				APP_LOG_INF("send ble:0x0C %x\r\n",key->ble_key);
			}
			#endif	
		}
	}
    #if (REMOTE_IR_ENABLE)
	if (key->key_types & KEY_TYPE_IR)
	{
		#if (LOW_BATT_PRODECT_ENABLE)
		if (g_board_status.ir_low_protect)
		{
			APP_LOG_INF("ir_low_protect:%d\r\n",g_board_status.ir_low_protect);
		}	
		else
		#endif
		{			
		
	#if (BLT_APP_LED_ENABLE & LED1_LED_FLAG)
			device_led_setup(led_cfg[LED_IR_PRESS]);
	#endif
		
			g_keyboard_ctl.key_type |= IR_KEY;

			{
				if(!g_board_status.ir_not_released)
				{
                    APP_LOG_INF("send ir:%d\r\n",key->ir_code);
                    ir_default_code_send(key_id,key->ir_code);
                    g_board_status.ir_not_released = 1;
				}
			}
			
		}
	}
	else
	{
		APP_LOG_INF("key type null\r\n");
	}
    #endif
	        
    if (!gap_get_connect_status())
	{       
		ble_reconnection_adv();
	}
}



/**
 * @brief		按键释放处理
 * @param[in]	none
 * @return      none
 */
void key_release_handle(void)
{
    APP_LOG_INF_FUNC();

	key_auido_release_handle();

	
    if (kb_event.cnt == 0)
    {		
		g_board_status.erase_learn_info_mode_state = 0;
        g_keyboard_ctl.key_not_released = 0;
    }
	
	if ((g_keyboard_ctl.key_type & KEYBOARD_KEY || g_keyboard_ctl.key_type & CONSUMER_KEY || g_keyboard_ctl.key_type & MOUSE_KEY))
	{
		#if (BLT_APP_LED_ENABLE & LED1_LED_FLAG)
		device_led_setup(led_cfg[LED_BLE_PRESS_RELEASE]);
		#endif
	}
	if (g_keyboard_ctl.key_type & IR_KEY)
	{
		#if (BLT_APP_LED_ENABLE & LED1_LED_FLAG)
		device_led_setup(led_cfg[LED_PRESS_RELEASE]);
		#endif
	}

    if(gap_get_connect_status())
    {				
        if(g_keyboard_ctl.key_type & CONSUMER_KEY)
        {
            g_keyboard_ctl.key_type &= ~CONSUMER_KEY;
            unsigned short consumer_key = 0;
            hid_gatt_report_notify(CONSUMER_HANDLE,  (unsigned char *)&consumer_key, 2);
        }

        if(g_keyboard_ctl.key_type & KEYBOARD_KEY)
        {
            g_keyboard_ctl.key_type &= ~KEYBOARD_KEY;
            unsigned char key_buf[8] = {0,0,0,0,0,0,0,0};
            hid_gatt_report_notify(KEYBOARD_HANDLE, key_buf, 8);
        }
    }
	
    #if (REMOTE_IR_ENABLE)
    if(g_keyboard_ctl.key_type &  IR_KEY)
    {
        g_keyboard_ctl.key_type &= ~IR_KEY;

        if(g_board_status.ir_not_released)
        {
            g_board_status.ir_not_released = 0;
            ir_release_send();  //release
        }
    }
    #endif
	
	g_keyboard_ctl.key_type = IDLE_KEY;
}

/**
 * @brief		按键发码处理
 * @param[in]	none
 * @return      none
 */
void key_change_proc(void)
{	
    unsigned char key0 = kb_event.keycode[0];

    g_keyboard_ctl.key_not_released = 1;

	if(kb_event.cnt)
	{
		#if (REMOTE_BATT_ENABLE)
		batt_detect_handle();
		#endif
		#if (REMOTE_BATT_ENABLE)
		if (g_board_status.low_batt_status )
		{
			if (g_board_status.ble_low_protect)
			{
				if(gap_get_connect_status())
				{
					app_disconnect_master(APP_LOWBATT_DISCON);
				}
				return;
			}
			#if (BLT_APP_LED_ENABLE & LED1_LED_FLAG)
			device_led_setup(led_cfg[LED_LOW_PRESSURE]);
			#endif
		}
		
		#endif
	}
	
    if(kb_event.cnt == 1)
    {
    	one_key_handle(key0);
    }
    else /*kb_event.cnt == 0, key release*/
    {   
        key_release_handle();
    }

    memcpy(&g_keyboard_ctl.last_kb_event,&kb_event,sizeof(KB_DATA_T));  
}


#if (BLE_APP_AUDIO_TYPE)
/**
 * @brief		按键释放开启休眠
 */
static void key_release_sleep_handle(co_timer_t *timer, void *param)
{
	remote_latency_disable(KEY_LATENCY_TYPE,false);
	co_timer_del(&key_release_sleep_timer);
}
#endif


#if (STUCK_KEY_PROCESS_ENABLE)
/**
 * @brief		卡键处理
 */
static void stuck_key_handle(co_timer_t *timer, void *param)
{
	APP_LOG_INF_FUNC();
#if (STUCK_KEY_PROCESS_ENABLE & IR_STUCK_KEY)	
	if(g_keyboard_ctl.key_type &  IR_KEY)
    {
        g_keyboard_ctl.key_type &= ~IR_KEY;

        if(g_board_status.ir_not_released)
        {
            g_board_status.ir_not_released = 0;
			#if (REMOTE_IR_ENABLE)
            ir_release_send();  //release
			#endif
        }
    }
#endif
	
#if (STUCK_KEY_PROCESS_ENABLE & BLE_STUCK_KEY)
	if(gap_get_connect_status())
	{
		if(g_keyboard_ctl.key_type & CONSUMER_KEY)
		{
			g_keyboard_ctl.key_type &= ~CONSUMER_KEY;
			unsigned short consumer_key = 0;
            hid_gatt_report_notify(CONSUMER_HANDLE,  (unsigned char *)&consumer_key, 2);
		}
		else if(g_keyboard_ctl.key_type & KEYBOARD_KEY)
		{
			g_keyboard_ctl.key_type &= ~KEYBOARD_KEY;
			unsigned char key_buf[8] = {0,0,0,0,0,0,0,0};
			hid_gatt_report_notify(KEYBOARD_HANDLE, key_buf, 8);

		}
		
//		app_disconnect_master(APP_NONE_DISCON);
		remote_latency_disable(KEY_LATENCY_TYPE,false);
	}
	#if (BLT_APP_LED_ENABLE)
	led_module_init();
	#endif
#endif

	set_stuck_key_scan_timer();
	
	pmu_lowpower_allow(PMU_LP_ALL);		
	
	g_keyboard_ctl.stuck_flag = 1;
	
}

/**
 * @brief		卡键检测
 */
static bool stuck_key_detect(void)
{
	if(kb_event.cnt && !g_keyboard_ctl.stuck_flag)
	{ //key press
		APP_LOG_INF("stuck_key_detect\n");		
		co_timer_set(&remote_stuck_key_timer,STUCK_KEY_TIMEOUT,TIMER_ONE_SHOT,stuck_key_handle,NULL);
	}
	else
	{
		#if (STUCK_KEY_PROCESS_ENABLE)
		g_keyboard_ctl.stuck_flag = 0;
		co_timer_del(&remote_stuck_key_timer);
		#endif
	}
	
	if (g_keyboard_ctl.stuck_flag)
	{
		return true;
	}
	else
	{
		return false;
	}
}
#endif


/**
 ***************************************************************************************************
 * @brief Keyscan event handler
 *
 * @param[in] event: keyscan event type, see @ref key_scan_event_t
 * @param[in] data:  keyscan data, see @ref key_scan_data_t
  * 
 * @return  void
 ***************************************************************************************************
 */
void key_task_func(KEY_SCAN_EVENT_T event, const KB_DATA_T kb_data)
{
	memcpy(&kb_event,&kb_data,sizeof(KB_DATA_T));       
	
  APP_LOG_INF("kcnt->%d kcod0->%d keycod1->%d \r\n",
            kb_event.cnt, kb_event.keycode[0], kb_event.keycode[1]);
	
	if (g_board_status.system_mode == SYS_SINGLE_MODE)
	{
        
	}
	else
	{
		#if (STUCK_KEY_PROCESS_ENABLE)
		if (stuck_key_detect())
		{
			return;
		}
		#endif
		
		key_change_proc();
		remote_mode_hold_trigger();
		remote_mode_key_task();
		

	}
}

/*******************************配对处理***************************/
/**
 * @brief		检测配对组合键
 * @param[in]	none
 * @return      none
 */
bool check_pair_comb_key(void)
{	
	if (get_remote_mode() != REMOTE_IDLE)
	{
		return false;
	}
	/*有双键按下 键值为配对键 则配对键按下*/
	if (kb_event.cnt==2 && ((kb_event.keycode[0]==PAIR_KEY1_INDEX&&kb_event.keycode[1]==PAIR_KEY0_INDEX)
		|| (kb_event.keycode[0]== PAIR_KEY0_INDEX &&kb_event.keycode[1]== PAIR_KEY1_INDEX)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * @brief		keymissing断链处理
 */
void keymissing_disconnect_rematch(co_timer_t *timer, void *param)
{
	//配对处理
	if (get_remote_mode() >= REMOTE_PAIR_START && get_remote_mode() < REMOTE_PAIR_OK &&
		g_board_status.rcu_reconnecting_count < RECONNECTING_MAX_COUNT)
	{
		APP_LOG_INF("rcu_reconnecting_count:%d\r\n",g_board_status.rcu_reconnecting_count);
		
		g_board_status.rcu_reconnecting_count++;
		set_remote_mode(REMOTE_PAIR_START);
	}
	else //正常断链
	{
		APP_LOG_INF("APP_Keymissing_DISCON\n");
		app_disconnect_master(APP_NONE_DISCON);
	}
}
/**
 * @brief		keymissing断链延时定时器启动
 */
void kyemissing_disconnect_delay_start(void)
{
	co_timer_set(&keymissing_disconnect_timer, 500, TIMER_ONE_SHOT, keymissing_disconnect_rematch, NULL);
}

void keymissing_timer_close(void)
{
    co_timer_del(&keymissing_disconnect_timer);
}

/**
 * @brief	配对过程中断链处理
 */
void pairing_disconnect_rematch(void)
{
	if (g_board_status.adv_type != ADV_TYPE_PAIR && get_remote_mode() >= REMOTE_PAIR_START && get_remote_mode() < REMOTE_PAIR_OK &&
		g_board_status.rcu_reconnecting_count < RECONNECTING_MAX_COUNT)
	{
		APP_LOG_INF("rcu_reconnecting_count:%d\r\n",g_board_status.rcu_reconnecting_count);
		
		g_board_status.rcu_reconnecting_count++;
		set_remote_mode(REMOTE_PAIR_START);
	}
}

void remote_pair_key_task(unsigned char remode_mode)	
{
	switch (remode_mode)
	{
		default:
		{
			break;
		}
	}
}	

/**
 * @brief		配对loop处理
 * @param[in]	none
 * @return      none
 */
void remote_pair_handle(unsigned char remode_mode)
{
	switch (remode_mode)
	{
		case REMOTE_PAIR_START:
		{				
			if(gap_get_connect_status())
			{
				app_disconnect_master(APP_UNBIND_DISCON);
				set_remote_mode(REMOTE_CON_DIS_WAIT);
			}
			else
			{

				hid_remove_device_bond();							
				start_ble_pair_adv_ind();
				set_remote_mode(REMOTE_PAIR_WAIT); 	
			
				#if (BLT_APP_LED_ENABLE & LED1_LED_FLAG)
				device_led_setup(led_cfg[LED_WAIT_PAIR]);	
				#endif 				
			}
			
			g_board_status.rcu_pair_hidmap_en = 0;
		}
		break;
	case REMOTE_CON_DIS_WAIT:
	{
		if (remote_clock_exceed(get_remote_mode_tick(),PAIR_ADV_TIMEOUT*1000))
		{
			#if (BLT_APP_LED_ENABLE & LED1_LED_FLAG)
				device_led_setup(led_cfg[LED_PAIR_FAIL]);
			#endif
				set_remote_mode(REMOTE_PAIR_FAIL);
		}
		else if (!gap_get_connect_status())
		{
			hid_remove_device_bond();							
			start_ble_pair_adv_ind();

			#if (BLT_APP_LED_ENABLE & LED1_LED_FLAG)
			device_led_setup(led_cfg[LED_WAIT_PAIR]);	
			#endif 	
			set_remote_mode(REMOTE_PAIR_WAIT);
		}
	}
	break;
	case REMOTE_PAIR_WAIT:
		if (gap_get_connected_status() 
			)  
		{
#if (BLT_APP_LED_ENABLE & LED1_LED_FLAG)
			device_led_setup(led_cfg[LED_PAIR_SUCCESS]);
#endif
			set_remote_mode(REMOTE_PAIR_OK);
		}
        else if (remote_clock_exceed(get_remote_mode_tick(),PAIR_ADV_TIMEOUT*1000))
		{
			#if (BLT_APP_LED_ENABLE & LED1_LED_FLAG)
				device_led_setup(led_cfg[LED_PAIR_FAIL]);
			#endif
				set_remote_mode(REMOTE_PAIR_FAIL);
		}
		break;
	case REMOTE_PAIR_OK:
		if(remote_clock_exceed(get_remote_mode_tick(), 2*1000))
		{
			set_remote_mode(REMOTE_PAIR_END);
		}
		break;
	case REMOTE_PAIR_FAIL:
		if(gap_get_connect_status())
		{
			hid_remove_device_bond();
			app_disconnect_master(APP_UNBIND_DISCON);
		}
		
		set_remote_mode(REMOTE_PAIR_END);
		break;
	case REMOTE_PAIR_END:
		set_remote_mode(REMOTE_TASK_FINISH);
		break;
	default:
		break;
	}
}
