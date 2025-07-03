#ifndef __BOARD_CONFIG_H
#define __BOARD_CONFIG_H

#include "board_demo.h"

#define XH_SET_MAX_MTU_xh98xx(max_mtu) \
    do { extern uint16_t att_cu_data[]; att_cu_data[17] = (max_mtu); } while(0)

	
#define BLE_MAX_OCTETS  (251) // number of octets
#define BLE_MAX_TIME    (17040) // in us
	
#define LOCAL_DEFAULT_ADDR		{0x01,0x01,0x01,0x41,0xC1,0xA4}
	
/* Log level value */
#define APP_LOG_LEVEL_OFF       0
#define APP_LOG_LEVEL_ERROR     1
#define APP_LOG_LEVEL_WARNING   2
#define APP_LOG_LEVEL_INFO      3
#define APP_LOG_LEVEL_DEBUG     4
#define APP_LOG_LEVEL_VERBOSE   5

#define BLE_MTU_BUF				144


/******************************************系统休眠控制********************************/
#define MIC_SLEEP_TYPE				BIT(1)
#define IR_SEND_SLEEP_TYPE			BIT(2)
#define IR_LEARN_SLEEP_TYPE			BIT(3)
#define KEY_SLEEP_TYPE				BIT(4)
#define MOUSE_KEY_SLEEP_TYPE		BIT(5)
#define XIHAO_FACTORY_SLEEP_TYPE	BIT(6)
#define REMOTE_ENTER_MODE			BIT(7)

/******************************************系统latency控制********************************/
#define MIC_LATENCY_TYPE				BIT(1)
#define OTA_LATENCY_TYPE				BIT(2)
#define KEY_LATENCY_TYPE				BIT(3)
#define MOUSE_KEY_LATENCY_TYPE			BIT(4)
#define XIHAO_FACTORY_TYPE				BIT(5)


#define CONN_PARAM_MAX			8
#define CONN_PARAM_MIN			8
#define CONN_PARAM_LATENCY		99
#define CONN_PARAM_TIMEOUT		400
	
/**系统主模式*/
typedef enum {
	SYS_NORMAL_MODE=0,	/**正常模式*/
	SYS_SINGLE_MODE,	/**单载波模式*/
}SYSTEM_MODE_E;

/** rcu状态 */
typedef struct {

	unsigned char system_mode;			/**系统模式*/
	unsigned char remote_mode;			/**RCU当前特殊模式*/
	unsigned int remote_mode_tick;		/**RCU特殊模式通用计时*/
	unsigned int remote_mode_base_tick; /**模式计时器*/
	unsigned char remote_hold_mode;
	unsigned int remote_hold_timer;	
	unsigned int conn_start_tick;		/**rcu 配对重连次数 防止一直重连,到达最大次数放弃连接*/
	
	unsigned int remote_sleep_status;			/**遥控器休眠控制*/
	unsigned int remote_latency_status;			/**遥控器休眠控制*/
	
	unsigned int batt_det_cnt;					/**电量检测控制*/
	unsigned int batt_val_sum;					/**电量检测和,用于过滤*/
	unsigned char batt_cur_val;					/**当前电池电量*/
	unsigned short batt_mv;						/**当前电压*/
	unsigned char low_batt_status;				/**低压状态*/
	unsigned char ir_low_protect;				/**红外低压保护*/
	unsigned char ble_low_protect;				/**蓝牙低压保护*/

	unsigned char rcu_linklayer_status;			/**链路层状态*/
	unsigned char rcu_pair_hidmap_en;			/**配对hidmap读取完成*/
	unsigned char rcu_connecting_timeout; 		/**连接超时*/
	unsigned char rcu_disconn_reason;			/**rcu主动断链原因*/
	unsigned char trigger_pair_mode_flag;		//主要用来区分一键配对广播和组合键配对的广播时长
	unsigned char erase_learn_info_mode_state;	//组合键触发后松手不发码
		
	unsigned char rcu_reconnecting_count;		/**rcu 配对重连次数 防止一直重连,到达最大次数放弃连接*/

	unsigned char adv_type;						/**广播类型*/

	unsigned char ir_not_released;
	unsigned char ui_mic_enable;
	unsigned char ui_mic_enable_type;
	unsigned char app_audio_type; 			/**语音类型*/
	
	unsigned char ota_flag;
}BOARD_STATUS_T;

extern BOARD_STATUS_T g_board_status;

#endif //__BOARD_CONFIG_H
