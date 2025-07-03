#include <string.h>
#include "co_tools.h"
#include "batt_service.h"
#include "gatt_sig_uuid.h"

#define APP_LOG_DOMAIN      "serbatt"
#define APP_LOG_LEVEL       APP_BATT_SERVICE_LOG_LEVEL
#include "co_debug.h"

static uint16_t m_start_handle;
static uint8_t val_bat_lev = 100;

extern BOARD_STATUS_T g_board_status;

static const uint8_t serv_batt[]     = UUID16_ARR(BATT_SERV_UUID);
static const uint8_t char_bat_lev[]  = UUID16_ARR(BATT_LEVEL_UUID);
static uint16_t desc_bat_lev;

static const att_db_t att_db[] = {
	// Index            p_UUID           p_value                      v_len                           properties
	// Battary Service
	[IDX_BATT_SVC]    = {ATT_SERV_UUID,  (uint8_t *)serv_batt,        sizeof(serv_batt),              ATT_PROP_READ},
	[IDX_BATVAL_CHAR] = {ATT_CHAR_UUID,  NULL,                        0,                              ATT_PROP_READ},
	[IDX_BATVAL_VAL]  = {char_bat_lev,   (uint8_t *) &g_board_status.batt_cur_val,    sizeof(g_board_status.batt_cur_val),            (ATT_PROP_READ | ATT_PROP_NTF) },
	[IDX_BATVAL_DESC] = {ATT_CCCD_UUID,  (uint8_t *) &desc_bat_lev,   sizeof(uint16_t),               ATT_PROP_READ | ATT_PROP_WRITE, 0, 0, 0, sizeof(uint16_t)},
};
	
void batt_service_init(void)
{
    static const gatt_serv_db_t service_db = { att_db, sizeof(att_db) / sizeof(att_db[0]) };
    gatt_database_add(&service_db, &m_start_handle);
    co_assert(m_start_handle);
}


static void batt_level_report(void)
{
    uint16_t len = 1;
	APP_LOG_INF("g_board_status.batt_cur_val_report = %d\r\n");
	//&val_bat_lev,
    ble_gatts_hvx_params_t hvx = {
        m_start_handle + IDX_BATVAL_VAL, // handle
        BLE_GATT_HVX_NOTIFICATION, 		// opcode
        0, // offset
        &len, &g_board_status.batt_cur_val,
    };
    xh_ble_gatts_hvx(0, &hvx);
}

void batt_level_change(uint8_t val)
{
    val_bat_lev = val;
    uint16_t desc_value;
    ble_gatts_value_t gatt_value = { sizeof(desc_value), 0, (uint8_t *) &desc_value };
	
    xh_ble_gatts_value_get(0, m_start_handle + IDX_BATVAL_DESC, &gatt_value );
    
	if (desc_value) 
	{
        batt_level_report();
    }
}

void batt_service_evt_cb(ble_evt_t *p_evt)
{
    if (p_evt->header.evt_id == BLE_GATTS_EVT_WRITE) 
	{
        if (p_evt->evt.gatts_evt.params.write.handle == m_start_handle + IDX_BATVAL_DESC) 
		{
            if (*p_evt->evt.gatts_evt.params.write.data)
			{
                batt_level_report();
            }
        }
    }
}


