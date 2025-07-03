#include "peripheral.h"
#include "cc.h"
#include "radio.h"
#include "cmsis_compiler.h"
#include "app_gap.h"

#include "gap_service.h"
#include "gatt_service.h"
#include "ble_dev_service.h"
#include "batt_service.h"
#include "hid_service.h"
#include "app_audio.h"
#include "app_keyboard.h"

#if (BLE_APP_AUDIO_TYPE & GOOGLE_AUDIO_TYPE)
#include "app_atv.h"
#include "atv_service.h"
#endif

#include "app_batt.h"
#include "app_led.h"

#define APP_LOG_DOMAIN      "gap"
#define APP_LOG_LEVEL       APP_GAP_LOG_LEVEL
#include "co_debug.h"

static ble_gap_id_key_t id_key;

static bool app_sec_get_ltk(ble_gap_master_id_t *master_id, ble_gap_enc_key_t *enc_key, ble_gap_id_key_t *id_key, uint8_t read_flag);	

/*配对广播包定义*/
unsigned char tbl_advData[] = {
	0x02, 0x01, 0x05,
	0x03, 0x03, 0x12, 0x18, 
	BLE_DEVICE_NAME_LEN, 0x09, BLE_DEVICE_NAME,
	0x05, 0xFF, 0x5D, 0x00, 0x55, 0x66,
};

/*配对扫描广播包定义*/
unsigned char tbl_scanRsp [] = {
	0x03, 0x03, 0x12, 0x18,
	0x03, 0x19, 0x80, 0x01,
	BLE_DEVICE_NAME_LEN, 0x09, BLE_DEVICE_NAME,
	0x05, 0xFF, 0x5D, 0x00, 0x55, 0x66,
};
 
/*回连广播包定义*/
unsigned char tbl_reconnection_advData[] = {
	  0x02,0x01,0x04,
      0x03, 0x19, 0x80, 0x01,
};

/*回连扫描广播包定义*/
unsigned char tbl_reconnection_scanAdvData[] = {
	 0x03, 0x19, 0x80, 0x01,
};

static ble_gap_enc_key_t enc_key = {
#if !APP_SEC_NVDS_SUPPORT
    {
        { 0xea, 0x39, 0xa0, 0xdf, 0xb6, 0xe5, 0x2c, 0x4b, 0x02, 0xf6, 0x11, 0x59, 0x38, 0xf4, 0x37, 0x87 }, // Fixed LTK
    },
    {
        0xDB93, // Fixed EDIV
        { 0xbe, 0x61, 0x3c, 0x0f, 0x9a, 0x9d, 0xd8, 0x0b } // Fixed Random
    }
#endif
};

static ble_gap_id_key_t m_peer_id_info;
struct app_sec_env_tag app_sec_env;
static ble_gap_addr_t peer_addr;


const static ble_gap_sec_params_t sec_params = {
    1,                    // bond
    0,                    // MITM not support
    BLE_GAP_IO_CAPS_NONE, // IO capability, Fix to NO Input NO Output
    0,                    // oob not support
    0,                    // secure connection not support
    0,                    // keypress not support
    0x10,                 // min key size 16
    0x10,                 // max key size 16
    {1, 0},               // Slave  Key distribution: LTK, IRK
    {0, 1},               // Master Key distribution: LTK, IRK
};

unsigned char wist_addr[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	
	
static co_timer_t conn_update_timer;



/**
 * @brief		使能/关闭 休眠
 * @param[in]	none
 * @return      none
 * @note		none
 */
void remote_sleep_disable(unsigned char type_bit, bool status)
{
	if (status)
	{
		if (!g_board_status.remote_sleep_status)
		{
			pmu_lowpower_prevent(PMU_LP_USER);
		}
		g_board_status.remote_sleep_status |= type_bit;
	}
	else
	{
		g_board_status.remote_sleep_status &= (~type_bit);
		
		if (!g_board_status.remote_sleep_status)
		{
			pmu_lowpower_allow(PMU_LP_USER);
		}
	}

}

/**
 * @brief		使能/关闭 latency
 * @param[in]	type_bit--操作功能类型
 * @param[in]	status--使能/关闭
 */
void remote_latency_disable(unsigned char type_bit, bool status)
{
	extern void llt_slave_latency_ignore(uint8_t link_handle, bool enable);
	
	if (status)
	{
		/**非连接状态,不能关闭latency*/
		if (!gap_get_connected_status())
		{
			return;
		}
	
		if (!g_board_status.remote_latency_status)
		{
			llt_slave_latency_ignore(NULL, true); 
			APP_LOG_INF("latenct status en\r\n");
		}
		g_board_status.remote_latency_status |= type_bit;
	}
	else
	{
		g_board_status.remote_latency_status &= (~type_bit);
		
		if (!g_board_status.remote_latency_status)
		{
			llt_slave_latency_ignore(NULL, false); 
			APP_LOG_INF("latenct status disen\r\n");
		}
	}
}



static void param_timer_func(co_timer_t *timer, void *param)
{
	APP_LOG_INF("conn_update_timer_cb");
	ble_gap_conn_params_t updatr_param = {CONN_PARAM_MIN, CONN_PARAM_MAX, CONN_PARAM_LATENCY, CONN_PARAM_TIMEOUT};

	if (g_board_status.ota_flag)
		return;
    
	xh_ble_gap_conn_param_update(0, &updatr_param);

	APP_LOG_INF("conn_update_timer_cb");
}

/**
 * @brief        断链处理
 * @param[in]    dis_reason-设备断链原因
 */
static void app_discon_event_handle(uint8_t dis_reason)
{
	APP_LOG_INF("dis_reason = %d\n",dis_reason);
	if (dis_reason == APP_PAIR_DISCON)
	{
		start_ble_pair_adv_ind();
	}
	g_board_status.rcu_disconn_reason = APP_NONE_DISCON;
}

/**
 * @brief        改变连接状态
 * @param[in]    linklayer_status-连接状态
 */
static void app_change_linklayer_status(uint8_t linklayer_status)
{
	APP_LOG_INF("linklayer_status:%d\r\n",linklayer_status);
	
	//从空闲状态退出需要关闭flash断电处理
	#if (SLEEP_FLASH_OFF_EN)
	if (g_board_status.rcu_linklayer_status == RCU_LINKLAYER_IDLE)
	{
		sfs_auto_disen_set(false);
	}
	#endif
	g_board_status.rcu_linklayer_status = linklayer_status;
}

unsigned char app_get_linklayer_status(void)
{
	return g_board_status.rcu_linklayer_status;
}
	
	
/**
 * @brief        遥控器主动断链
 * @param[in]    dis_reason-断链原因
 */
void app_disconnect_master(unsigned char dis_reason)
{
	g_board_status.rcu_disconn_reason = dis_reason;
	xh_ble_gap_disconnect(0,0x13);
}
	
void app_disconnect_keymissing(unsigned char dis_reason)
{
	g_board_status.rcu_disconn_reason = dis_reason;
	xh_ble_gap_disconnect(0,0x06);
}

/**
 * @brief      发送配对广播
 */
void start_ble_pair_adv_ind(void)
{
	if(gap_get_connect_status())
	{
		app_disconnect_master(APP_PAIR_DISCON);
		return;
	}
	else if (g_board_status.adv_type != ADV_TYPE_NONE)
	{
		app_stop_adv();
	}
	
	{
		//bba_easy_filter_policy_scan_wlst_con_wlst_enable(false,0,wist_addr);
		xh_ble_gap_adv_data_set(tbl_advData, sizeof(tbl_advData), tbl_scanRsp, sizeof(tbl_scanRsp));
		ble_gap_adv_params_t adv_params;
		adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
		adv_params.p_peer_addr = NULL;
		adv_params.fp          = BLE_GAP_ADV_FP_ANY;
		adv_params.interval    = 0x28;//0x80;
		if (g_board_status.trigger_pair_mode_flag)
		{
			adv_params.timeout     = PAIR_ADV_TIMEOUT;
		}
		else
		{
			adv_params.timeout     = 3*PAIR_ADV_TIMEOUT;
		}

		*(uint8_t*)&adv_params.channel_mask = 0x0;
		xh_ble_gap_adv_start(&adv_params);
		
		g_board_status.adv_type = ADV_TYPE_PAIR;
		app_change_linklayer_status(RCU_LINKLAYER_ADV);

		APP_LOG_INF("start pair advertising...\r\n");
	}
}

/**
 * @brief      发送定向回连广播 (注意,定向广播只能发送1s)
 */
void start_ble_reconnection_adv_direct_ind(void)
{
	
    if(gap_get_connect_status() || !hid_is_device_bond() || g_board_status.rcu_linklayer_status >= RCU_LINKLAYER_CONNECTING)// !bond_status
	{
		return;
	}
	
	if (g_board_status.adv_type >= ADV_TYPE_PAIR)
	{
		return;
	}

	ble_gap_adv_params_t adv_params;
	adv_params.type        = BLE_GAP_ADV_TYPE_ADV_LOW_DIRECT_IND;//BLE_GAP_ADV_TYPE_ADV_IND;
	adv_params.p_peer_addr = &peer_addr;//NULL;
	adv_params.fp          = BLE_GAP_ADV_FP_ANY;
	adv_params.interval    = 0x28;//0x80;
	adv_params.timeout     = RECONNECTION_ADV_DIR_TIMEOUT;	//定向广播时间最大1.28s
	*(uint8_t*)&adv_params.channel_mask = 0x0;
	xh_ble_gap_adv_start(&adv_params);
	
	g_board_status.adv_type = ADV_TYPE_DRI_RECONNECTION;
	app_change_linklayer_status(RCU_LINKLAYER_ADV);
	
  APP_LOG_INF("start reconnection direct adv...\r\n");
}



/**
 * @brief      发送普通回连广播
 */
void start_ble_reconnection_adv_ind(void)
{
	if(gap_get_connect_status() || !hid_is_device_bond())
	{
		APP_LOG_INF("not device bond, not reconnection:%d,%d\r\n",gap_get_connect_status(),hid_is_device_bond());
		return;
	}
	
	if (g_board_status.adv_type >= ADV_TYPE_MTK_POWER)
	{
		return;
	}
	
    //bba_easy_filter_policy_scan_wlst_con_wlst_enable(true,0,wist_addr);
	
	xh_ble_gap_adv_data_set(tbl_reconnection_advData, sizeof(tbl_reconnection_advData), tbl_reconnection_scanAdvData, 0);//sizeof(tbl_reconnection_scanAdvData)

	ble_gap_adv_params_t adv_params;
	adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
	adv_params.p_peer_addr = NULL;
	adv_params.fp          = BLE_GAP_ADV_FP_ANY;
	adv_params.interval    = 0x28;//0x80;
	adv_params.timeout     = RECONNECTION_ADV_TIMEOUT;
	*(uint8_t*)&adv_params.channel_mask = 0x0;
	xh_ble_gap_adv_start(&adv_params);
	
	g_board_status.adv_type = ADV_TYPE_RECONNECTION;
	app_change_linklayer_status(RCU_LINKLAYER_ADV);
	
    APP_LOG_INF("start reconnection advertising...\r\n");
}

static void app_sec_set_ltk_from_master_id(ble_gap_master_id_t* master_id, ble_gap_enc_key_t* enc_key)
{
	#if APP_SEC_NVDS_SUPPORT
    // Generate LTK from ediv & random
    if(enc_key->master_id.ediv == master_id->ediv && app_sec_env.bonded)
	{
        // LTK already saved in memory
        APP_LOG_INF("[SEC]: Get LTK from RAM success\n");
    } 
	else 
	{ // enc_key not inited
        nvds_tag_len_t length = sizeof(struct app_nvds_tag_bond);
        struct app_nvds_tag_bond app_nvds;
        memset(enc_key, 0, sizeof(ble_gap_enc_key_t));
        uint8_t res = nvds_get(NVDS_TAG_SMP_BOND_INFO, &length, (void*)&app_nvds);
        if(res == NVDS_OK){
            if(memcmp(master_id->rand, app_nvds.random, BLE_GAP_SEC_RAND_LEN)){
                APP_LOG_INF("[SEC]: Random in NVDS not match.\n");
                return;
            }
            APP_LOG_INF("[SEC]: Get LTK from NVDS success\n");
            enc_key->master_id.ediv = app_nvds.ediv;
            memcpy(enc_key->master_id.rand, app_nvds.random, BLE_GAP_SEC_RAND_LEN);
            enc_key->enc_info.ltk_len = app_nvds.ltk_len;
            memcpy(enc_key->enc_info.ltk, app_nvds.ltk, enc_key->enc_info.ltk_len);
        } else {
            APP_LOG_INF("[SEC]: Get LTK from NVDS failed\n");
        }
    }

    APP_LOG_INF("[SEC]: EDIV: 0x%04X, Random", enc_key->master_id.ediv);
//    APP_LOG_INF_ARRAY_EX("", enc_key->master_id.rand, BLE_GAP_SEC_RAND_LEN);
//    APP_LOG_INF_ARRAY_EX("[SEC]: LTK", enc_key->enc_info.ltk, enc_key->enc_info.ltk_len);
	#endif
}

void app_ble_gap_evt(ble_evt_t* p_ble_evt)
{
    if (p_ble_evt->header.evt_id == BLE_GAP_EVT_DISCONNECTED) 
	{
        APP_LOG_INF("[GAP]: Disonnected. reason:0x%02X\n", p_ble_evt->evt.gap_evt.params.disconnected.reason);
		co_timer_del(&conn_update_timer);
		app_change_linklayer_status(RCU_LINKLAYER_IDLE);
		g_board_status.adv_type = ADV_TYPE_NONE;
		
		if (p_ble_evt->evt.gap_evt.params.disconnected.reason == 0x08)
		{
			//是否需要回连接
			ble_reconnection_adv();
		}
		else
		{
			if (g_board_status.rcu_disconn_reason)
			{
				app_discon_event_handle(g_board_status.rcu_disconn_reason);
			}
		}
		
		pairing_disconnect_rematch();
		
		#if (BLE_AUDIO_ENABLE)
		disconnect_mic_close();
		#endif

		g_board_status.remote_latency_status = 0;	//断连重置latency状态
    }
	else if (p_ble_evt->header.evt_id == BLE_GAP_EVT_CONNECTED) 
	{
        ble_gap_addr_t *addr = &p_ble_evt->evt.gap_evt.params.connected.peer_addr;
        ble_gap_conn_params_t *param = &p_ble_evt->evt.gap_evt.params.connected.conn_params;

        peer_addr.addr_type = addr->addr_type;
        memcpy(peer_addr.addr, addr->addr ,6);
		
        APP_LOG_INF("[GAP]: Connected. %02x %02X:%02X:%02X:%02X:%02X:%02X Intv:%d Lat:%d Tout:%d\n",
            addr->addr_type, addr->addr[5], addr->addr[4], addr->addr[3], addr->addr[2], addr->addr[1], addr->addr[0],
            param->max_conn_interval, param->slave_latency, param->conn_sup_timeout
        );

		app_change_linklayer_status(RCU_LINKLAYER_CONNECTING);

		g_board_status.adv_type = ADV_TYPE_NONE;
		
		#if (MTU_REQUEST_ENABLE)
		gattc_mtu_exchange(BLE_MTU_BUF);
		#endif
    }
	else if (p_ble_evt->header.evt_id == BLE_GAP_EVT_CONN_PARAM_UPDATE) 
	{
        ble_gap_conn_params_t *p = &p_ble_evt->evt.gap_evt.params.conn_param_update.conn_params;
        APP_LOG_INF("[GAP]: Conn Param Updated: Intv:%d Lat:%d Tout:%d\n",
            p->max_conn_interval, p->slave_latency, p->conn_sup_timeout);
		
		if (p->conn_sup_timeout != CONN_PARAM_TIMEOUT || p->slave_latency != CONN_PARAM_LATENCY)
		{
			co_timer_set(&conn_update_timer, UPDATE_CONNECT_PARAM_TIMER, TIMER_ONE_SHOT, param_timer_func, NULL);
		}
    }
	else if (p_ble_evt->header.evt_id == BLE_GATTC_EVT_MTU_CHANGED) 
	{
        APP_LOG_INF("[GATT]: MTU changed:%d\n", p_ble_evt->evt.gattc_evt.params.mtu_changed.mtu);
    }
	else if (p_ble_evt->header.evt_id == BLE_GATTS_EVT_WRITE) 
	{
        //APP_LOG_INF("[GATT]: Write request: 0x%04X  ", p_ble_evt->evt.gatts_evt.params.write.handle);
        //APP_LOG_INF("", p_ble_evt->evt.gatts_evt.params.write.data, p_ble_evt->evt.gatts_evt.params.write.len);
    }
	else if (p_ble_evt->header.evt_id == BLE_GAP_EVT_TIMEOUT)
	{	
		if (p_ble_evt->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_ADVERTISING)
		{
			g_board_status.adv_type = ADV_TYPE_NONE;
		}
	}
	else if (p_ble_evt->header.evt_id == BLE_GAP_EVT_SEC_PARAMS_REQUEST)
	{
        APP_LOG_INF("---->BLE_GAP_EVT_SEC_PARAMS_REQUEST\r\n");
		#if APP_SEC_NVDS_SUPPORT
		memset(&enc_key, 0, sizeof(ble_gap_enc_key_t));
		#endif
        ble_gap_sec_keyset_t sec_keyset = {
            { (ble_gap_enc_key_t*) &enc_key, NULL, NULL }
        };
		
		sec_keyset.keys_central.p_id_key = &m_peer_id_info;
        ble_gap_sec_params_t *req_sec_params = &p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params;
        ble_gap_sec_params_t rsp_sec_params = sec_params;
        *(uint8_t*)&rsp_sec_params.kdist_periph &= *(uint8_t*)&req_sec_params->kdist_periph;
        *(uint8_t*)&rsp_sec_params.kdist_central &= *(uint8_t*)&req_sec_params->kdist_central;
        xh_ble_gap_sec_params_reply(0, 0, &rsp_sec_params , &sec_keyset);
    }
	else if (p_ble_evt->header.evt_id == BLE_GAP_EVT_SEC_INFO_REQUEST) 
	{
		APP_LOG_INF("---->BLE_GAP_EVT_SEC_INFO_REQUEST\r\n");
		if(app_sec_get_ltk(&p_ble_evt->evt.gap_evt.params.sec_info_request.master_id,&enc_key,&id_key,0))
		{
//			app_sec_set_ltk_from_master_id(&p_ble_evt->evt.gap_evt.params.sec_info_request.master_id, (ble_gap_enc_key_t*)&enc_key);			
			APP_LOG_INF("[SEC]: Send LTK response\n");
			xh_ble_gap_sec_info_reply(0, &enc_key.enc_info, NULL, NULL);	
		}
		else
		{
			APP_LOG_INF("Send TLK Missing response\n");
			kyemissing_disconnect_delay_start();
			xh_ble_gap_sec_info_reply(0,NULL,NULL,NULL);
		}
    } 
	else if (p_ble_evt->header.evt_id == BLE_GAP_EVT_CONN_SEC_UPDATE) 
	{
        // Encrypt state changed.
        APP_LOG_INF("---->BLE_GAP_EVT_CONN_SEC_UPDATE\r\n");
		
		co_timer_set(&conn_update_timer, UPDATE_CONNECT_PARAM_TIMER, TIMER_ONE_SHOT, param_timer_func, NULL);
		app_change_linklayer_status(RCU_LINKLAYER_CONNECTED);
    } 
	else if (p_ble_evt->header.evt_id == BLE_GAP_EVT_AUTH_STATUS) 
	{
		APP_LOG_INF("---->BLE_GAP_EVT_AUTH_STATUS\r\n");
        // Bond finished.
        ble_gap_evt_auth_status_t *p = &p_ble_evt->evt.gap_evt.params.auth_status;
        APP_LOG_INF("[SEC]: Bond status: %d\n", p->auth_status);
        APP_LOG_INF("[SEC]: EDIV: 0x%04X, Random", enc_key.master_id.ediv);

#if (IDENTITY_ADDRS_RESOLV_EN)
		uint8_t ide_res = nvds_put(NVDS_TAG_IDENTITY_ADDRS, NVDS_TAG_IDENTITY_ADDRS_LEN, (void*)&m_peer_id_info.id_addr_info.addr);
        if(ide_res == NVDS_OK)
		{
           	APP_LOG_INF_ARRAY("\n[SEC]: Peer addr", &m_peer_id_info.id_addr_info.addr, NVDS_TAG_IDENTITY_ADDRS_LEN);
			APP_LOG_INF("\nPeer addr type = %d",m_peer_id_info.id_addr_info.addr_type);
        } 
		else 
		{
			memset(m_peer_id_info.id_addr_info.addr,0,NVDS_TAG_IDENTITY_ADDRS_LEN);
            APP_LOG_INF("[SEC]: Save  Peer addr into NVDS failed\n");
        }
#endif

#if APP_SEC_NVDS_SUPPORT
        // save LTK to nvds
        nvds_tag_len_t length = sizeof(struct app_nvds_tag_bond);
        struct app_nvds_tag_bond app_nvds;
        app_nvds.ediv = enc_key.master_id.ediv;
        app_nvds.ltk_len = enc_key.enc_info.ltk_len;
		app_nvds.id_addr.addr_type = peer_addr.addr_type;
		
        memcpy(app_nvds.random, enc_key.master_id.rand, BLE_GAP_SEC_RAND_LEN);
        memcpy(app_nvds.ltk, enc_key.enc_info.ltk, enc_key.enc_info.ltk_len);
        memcpy(app_nvds.irk, &id_key.id_info.irk[0], 16);
		memcpy(app_nvds.id_addr.addr, peer_addr.addr ,6);
		
        uint8_t res = nvds_put(NVDS_TAG_SMP_BOND_INFO, length, (void*)&app_nvds);
        if(res == NVDS_OK)
		{
            APP_LOG_INF("[SEC]: Save LTK into NVDS success\n");
			app_sec_env.bonded = true;
        } 
		else 
		{
            APP_LOG_INF("[SEC]: Save LTK into NVDS failed\n");
			app_sec_env.bonded = false;
        }
#endif
    } 
	else if (p_ble_evt->header.evt_id == BLE_GAP_EVT_PASSKEY_DISPLAY) 
	{
        // Request to display a passkey to the user. @ref ble_gap_evt_passkey_display_t
		APP_LOG_INF("---->BLE_GAP_EVT_PASSKEY_DISPLAY\r\n");
    } 
	else if (p_ble_evt->header.evt_id == BLE_GAP_EVT_AUTH_KEY_REQUEST) 
	{
        // Request to provide an authentication key. @ref ble_gap_evt_auth_key_request_t
        // Reply the request with xh_ble_gap_auth_key_reply
		APP_LOG_INF("---->BLE_GAP_EVT_AUTH_KEY_REQUEST\r\n");
    }
	
	batt_service_evt_cb(p_ble_evt);

	hid_evt_cb(p_ble_evt);
	
	#if (BLE_APP_AUDIO_TYPE & GOOGLE_AUDIO_TYPE)
	atv_service_evt_cb(p_ble_evt);
	#endif
}


bool hid_is_device_bond(void)
{
	return app_sec_env.bonded;
}

void hid_remove_device_bond(void)
{
	int err;
#if (APP_SEC_NVDS_SUPPORT)
	 // Check if we are well bonded
    if (app_sec_env.bonded == true)
    {
        // Update the environment variable
		APP_LOG_INF("clear bond info\n");
        app_sec_env.bonded = false;
		
		#if APP_SEC_NVDS_SUPPORT
		memset(&enc_key, 0, sizeof(ble_gap_enc_key_t));
		#endif
		
        err = nvds_del(NVDS_TAG_SMP_BOND_INFO);
		if(err == NVDS_OK)
		{
			APP_LOG_INF("NVDS_TAG_SMP_BOND_INFO DEL SUCCESS\r\n");			
		}
		else
		{
			APP_LOG_INF("NVDS_TAG_SMP_BOND_INFO ERROR\n");
		}

    }
#endif

}


void app_sec_init(void)
{
    app_sec_env.bonded = false;

    nvds_tag_len_t length = sizeof(struct app_nvds_tag_bond);
    struct app_nvds_tag_bond app_nvds;
    
    uint8_t res = nvds_get(NVDS_TAG_SMP_BOND_INFO, &length, (void*)&app_nvds);
    if(res == NVDS_OK){
        app_sec_env.bonded = true;
				memcpy(&peer_addr, &app_nvds.id_addr ,sizeof(peer_addr));
    }else{
        
    }
}


#if (IDENTITY_ADDRS_RESOLV_EN)
void identity_adds_init(void)
{
	nvds_tag_len_t length = NVDS_TAG_IDENTITY_ADDRS_LEN;

    uint8_t res = nvds_get(NVDS_TAG_IDENTITY_ADDRS, &length, (void*)&m_peer_id_info.id_addr_info.addr);
    if(res == NVDS_OK){
        APP_LOG_INF_ARRAY("\n[SEC]: Peer addr", &m_peer_id_info.id_addr_info.addr, NVDS_TAG_IDENTITY_ADDRS_LEN);
    }else{
        APP_LOG_INF("\n[SEC]: Peer addr fail\n");
    }
}
#endif

void app_gap_init(unsigned char sleep_status)
{

	app_sec_init();
	
	#if (IDENTITY_ADDRS_RESOLV_EN)
	identity_adds_init();
	#endif
	
	gap_service_init();
	gatt_service_init();
	ble_dev_service_init();
	batt_service_init();
	hid_service_init();
	
	#if (BLE_APP_AUDIO_TYPE & GOOGLE_AUDIO_TYPE)
	atv_service_init();
	#endif
	
	#if (BLE_AUDIO_ENABLE)
	app_audio_init();
	#endif

	batt_detect_handle();

	//上电发送回连广播
	if (sleep_status == PMU_REBOOT_FROM_POWER_ON || sleep_status == PMU_REBOOT_FROM_USER) //这个是用于ota成功后回连
	{
		if (hid_is_device_bond())
		{
			ble_reconnection_adv();
		}
	}
}

static uint8_t is_all_zeros(uint8_t *data, int len)
{
    while(len--) {
        if (*data++) {
            return false;
        }
    }
    return true;
}

static bool app_sec_get_ltk(ble_gap_master_id_t *master_id, ble_gap_enc_key_t *enc_key, ble_gap_id_key_t *id_key, uint8_t read_flag)
{
#if APP_SEC_NVDS_SUPPORT
    APP_LOG_INF("[SEC]: EDIV: 0x%04X, Random", master_id->ediv);
//    log_debug_array_ex("", master_id->rand, BLE_GAP_SEC_RAND_LEN);
    if (!is_all_zeros(master_id->rand, 8) && !memcmp(master_id->rand, enc_key->master_id.rand, 8)) {
        // LTK already saved in memory
        APP_LOG_INF("[SEC]: Get LTK from RAM success by Random app_sec_get_ltk\r\n");
    } else if (is_all_zeros(master_id->rand, 8) && !is_all_zeros(id_key->id_info.irk, 16)
                && gap_rpa_check(peer_addr.addr, id_key->id_info.irk)) {
        APP_LOG_INF("[SEC]: Get LTK(sc) from RAM success by IRK\n");
    } else if (is_all_zeros(master_id->rand, 8) && !memcmp(&peer_addr, &id_key->id_addr_info, 6)) {
        APP_LOG_INF("[SEC]: Get LTK(sc) from RAM success by Address\n");
    } else { // enc_key not inited
        if (read_flag) {
            return false;
        }
        memset(enc_key, 0, sizeof(ble_gap_enc_key_t));
        memset(id_key, 0, sizeof(ble_gap_id_key_t));
        nvds_tag_len_t length = sizeof(struct app_nvds_tag_bond);
        struct app_nvds_tag_bond app_nvds;
        memset(&app_nvds, 0, sizeof(app_nvds));
        uint8_t res = nvds_get(NVDS_TAG_SMP_BOND_INFO, &length, (void*)&app_nvds);
        if(res == NVDS_OK){
            APP_LOG_INF("[SEC]: Load LTK from NVDS to RAM\n");
            enc_key->master_id.ediv = app_nvds.ediv;
            memcpy(enc_key->master_id.rand, app_nvds.random, BLE_GAP_SEC_RAND_LEN);
            enc_key->enc_info.ltk_len = app_nvds.ltk_len;
            memcpy(enc_key->enc_info.ltk, app_nvds.ltk, enc_key->enc_info.ltk_len);
            memcpy(id_key->id_info.irk, app_nvds.irk, 16);
            id_key->id_addr_info.addr_type = app_nvds.id_addr.addr_type;
            memcpy(id_key->id_addr_info.addr, app_nvds.id_addr.addr, BLE_GAP_ADDR_LEN);
            return app_sec_get_ltk(master_id, enc_key, id_key, true);
        } else {
            APP_LOG_INF("[SEC]: Get LTK from NVDS failed\n");
            return false;
        }
    }
#endif
//    log_debug_array_ex("", enc_key->master_id.rand, BLE_GAP_SEC_RAND_LEN);
//    log_debug_array_ex("[SEC]: LTK", enc_key->enc_info.ltk, enc_key->enc_info.ltk_len);
    return true;
}

