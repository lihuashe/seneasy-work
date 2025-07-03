#ifndef __APP_GAP_H
#define __APP_GAP_H
#include "ble.h"
#include "board_config.h"


#if (APP_SEC_NVDS_SUPPORT)
#include "nvds.h"
#endif

#define app_stop_adv()	xh_ble_gap_adv_stop()	//遥控器主动停止广播

struct app_nvds_tag_bond {
    uint8_t random[8];
    uint16_t ediv;
    uint8_t ltk_len;
    uint8_t irk[16];
    uint8_t ltk[16];
	ble_gap_addr_t id_addr;
};

struct app_sec_env_tag
{
    //Bond status
    bool bonded;

// #if (BLE_APP_SEC_CON)
//     //Secure Connections on current link
//     bool sec_con_enabled[BLE_CONNECTION_MAX];
// #endif
//     //connect bonded state
//     bool con_bonded[BLE_CONNECTION_MAX];
//     //connection information
//     struct app_sec_conn_info_tag info[BLE_CONNECTION_MAX]; 
};

/**断链原因*/
typedef enum {
	APP_NONE_DISCON = 0,			/**正常断链*/
	APP_PAIR_DISCON,				/**配对断链*/
	APP_LOWBATT_DISCON,				/**低压断链*/
	APP_OTA_DISCON,					/**ota断链*/
	APP_UNBIND_DISCON,				/**解除绑定*/
}APP_DISCON_E;


/**广播类型 ,这个也代表优先级*/
typedef enum {
	ADV_TYPE_NONE = 0,				/**空广播*/
	ADV_TYPE_RECONNECTION,			/**回连广播*/
	ADV_TYPE_DRI_RECONNECTION,			/**直连回连广播*/
	ADV_TYPE_GOOGLE_POWER,			/**mtk power广播*/
	ADV_TYPE_MTK_POWER,				/**mtk power广播*/
	ADV_TYPE_RTK_POWER,				/**rtk power广播*/
	ADV_TYPE_PAIR,					/**配对广播*/
	ADV_TYPE_FAC_PAIR,
}ADV_TYPE_E;


/**广播类型*/
typedef enum {
	RCU_LINKLAYER_IDLE = 0,				/**链路层状态等待*/
	RCU_LINKLAYER_ADV,					/**链路层状态为广播*/
	RCU_LINKLAYER_CONNECTING,			/**链路层状态为连接中*/
	RCU_LINKLAYER_CONNECTED,			/**回连广播*/
}RCU_LINKLAYER_E;


void start_ble_pair_adv_ind(void);
void start_ble_reconnection_adv_direct_ind(void);
void start_ble_reconnection_adv_ind(void);
void start_ble_mtk_power_adv_ind(void);
void start_ble_google_woble_adv_ind(void);
void start_ble_mtk_netflix_adv_ind(void);
void start_ble_rtk_power_adv_ind(void);
void start_ble_fac_test_ind(void);

void app_ble_gap_evt(ble_evt_t* p_ble_evt);
void app_gap_init(unsigned char sleep_status);
bool hid_is_device_bond(void);
void hid_remove_device_bond(void);
void app_disconnect_master(unsigned char dis_reason);

void remote_sleep_disable(unsigned char type_bit, bool status);
void remote_latency_disable(unsigned char type_bit, bool status);

#define ble_reconnection_adv()	start_ble_reconnection_adv_ind()		//发送回连广播
#define	gap_get_connect_status()	(g_board_status.rcu_linklayer_status >= RCU_LINKLAYER_CONNECTING)
#define	gap_get_connected_status()	(g_board_status.rcu_linklayer_status == RCU_LINKLAYER_CONNECTED)
#endif
