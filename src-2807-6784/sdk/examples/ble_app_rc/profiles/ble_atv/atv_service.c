#include <string.h>
#include "co_tools.h"
#include "atv_service.h"
#include "gatt_sig_uuid.h"
#include "co_evt.h"
#include "app_atv.h"

#define APP_LOG_DOMAIN      "atv"
#define APP_LOG_LEVEL       APP_ATV_LOG_LEVEL
#include "co_debug.h"

#if (BLE_APP_AUDIO_TYPE & GOOGLE_AUDIO_TYPE)

static uint16_t m_start_handle;


static uint16_t desc_audio = 0x0000;
static uint16_t desc_control = 0x0000;

static const uint8_t serv_atvv[]        = {0x64, 0xB6, 0x17, 0xF6, 0x01, 0xAF, 0x7D, 0xBC, 0x05, 0x4F, 0x21, 0x5A, 0x01, 0x00, 0x5E, 0xAB};
static const uint8_t command_uuid[]     = {0x64, 0xB6, 0x17, 0xF6, 0x01, 0xAF, 0x7D, 0xBC, 0x05, 0x4F, 0x21, 0x5A, 0x02, 0x00, 0x5E, 0xAB};
static const uint8_t audio_uuid[]       = {0x64, 0xB6, 0x17, 0xF6, 0x01, 0xAF, 0x7D, 0xBC, 0x05, 0x4F, 0x21, 0x5A, 0x03, 0x00, 0x5E, 0xAB};
static const uint8_t control_uuid[]     = {0x64, 0xB6, 0x17, 0xF6, 0x01, 0xAF, 0x7D, 0xBC, 0x05, 0x4F, 0x21, 0x5A, 0x04, 0x00, 0x5E, 0xAB};



static const att_db_t att_db[] = {
	// Index                    p_UUID           p_value                    v_len                       properties    128bit PR  PW
	[ATVVS_IDX_SVC]          = {ATT_SERV_UUID,   (uint8_t *)&serv_atvv,      sizeof(serv_atvv),          ATT_PROP_READ,  0,   0,  0},
	
	[ATVVS_IDX_COMMAND_CHAR] = {ATT_CHAR_UUID,   NULL,                       0,                          ATT_PROP_READ,  0,   0,  0},
	[ATVVS_IDX_COMMAND_VAL]  = {command_uuid,    NULL,                       0,                          ATT_PROP_WRITE_CMD, 1,   0,  0},
   
	[ATVVS_IDX_AUDIO_CHAR]   = {ATT_CHAR_UUID,   NULL,                       0,                          ATT_PROP_READ,  0,   0,  0},
	[ATVVS_IDX_AUDIO_VAL]    = {audio_uuid,      NULL,                       0,                          ATT_PROP_READ|ATT_PROP_NTF,   1,   1,  1},
	[ATVVS_IDX_AUDIO_CCC]   = {ATT_CCCD_UUID,   (uint8_t *)&desc_audio,     sizeof(uint16_t),			 ATT_PROP_WRITE | ATT_PROP_READ,  0,   1,  1, sizeof(uint16_t)},
   
	[ATVVS_IDX_CONTROL_CHAR] = {ATT_CHAR_UUID,   NULL,                       0,                          ATT_PROP_READ,  0,   0,  0},
	[ATVVS_IDX_CONTROL_VAL]  = {control_uuid,    NULL,                       0,                          ATT_PROP_READ|ATT_PROP_NTF, 1,   1,  1},
	[ATVVS_IDX_CONTROL_CCC] = {ATT_CCCD_UUID,   (uint8_t *)&desc_control,   sizeof(uint16_t),            ATT_PROP_WRITE | ATT_PROP_READ,  0,   1,  1, sizeof(uint16_t)},

};
	
void atv_service_init(void)
{
    static const gatt_serv_db_t service_db = { att_db, sizeof(att_db) / sizeof(att_db[0]) };
    gatt_database_add(&service_db, &m_start_handle);
    co_assert(m_start_handle);
}



void atv_service_evt_cb(ble_evt_t *p_evt)
{
    if (p_evt->header.evt_id == BLE_GATTS_EVT_WRITE) 
	{
        uint8_t* data = p_evt->evt.gatts_evt.params.write.data;
        uint16_t len = p_evt->evt.gatts_evt.params.write.len;
		
//		APP_LOG_INF("data[0] = %d,len = %d\r\n",p_evt->evt.gatts_evt.params.write.data[0],p_evt->evt.gatts_evt.params.write.len);
        switch (p_evt->evt.gatts_evt.params.write.handle - m_start_handle) 
        {
            case ATVVS_IDX_COMMAND_VAL:
				APP_LOG_INF("ATVVS_IDX_COMMAND_VAL\r\n");
				atvv_char_tx_handle(data,len);
                break;
            case ATVVS_IDX_AUDIO_CCC:
				APP_LOG_INF("cccd atv audio:%d\r\n",data[0]);
                break;
            case ATVVS_IDX_CONTROL_CCC: 
                APP_LOG_INF("cccd atv ctl:%d\r\n",data[0]);
                break;
        }
    }
	else if (p_evt->header.evt_id == BLE_EVT_TX_COMPLETE) 
	{
		//send data success , result cb
		if (atvv_transmit_flag == ATVV_VOICE_START)
		{
           
        } 
    }
}


/*发送数据*/
void atv_ntf_data(uint8_t att_idx,uint8_t *data,uint16_t len)
{	
	uint32_t err = 0;
	
	uint8_t pkg_num;
	uint16_t att_handle = m_start_handle + att_idx;

    xh_ble_tx_buffer_count_get(&pkg_num);

	 if (pkg_num == 0 || len==0)		
	{
        APP_LOG_INF("rf tx full or len err\r\n");
		return;
    }
	
	ble_gatts_hvx_params_t hvx = {
		att_handle, BLE_GATT_HVX_NOTIFICATION, 0, &len, (uint8_t *)data
	};
//	APP_LOG_INF("atvv_send_data\r\n");
	err = xh_ble_gatts_hvx(0, &hvx);
//	APP_LOG_INF("err = %d\r\n",err);    
}
#endif

