#include <string.h>
#include "co_tools.h"
#include "gatt_service.h"
#include "gatt_sig_uuid.h"

#define APP_LOG_DOMAIN      "sergatt"
#define APP_LOG_LEVEL       APP_GATT_SERVICE_LOG_LEVEL
#include "co_debug.h"

struct gatt_svc_changed {
    uint16_t start_hdl; /**Service start handle which changed*/
    uint16_t end_hdl; 	/**Service end handle which changed*/
};

static uint16_t m_start_handle;

static const uint8_t serv_gap[]      = UUID16_ARR(GATT_SERVICE_UUID);
static const uint8_t char_changed[]  = UUID16_ARR(GATT_SERVICE_CHANGED_UUID);

static const struct gatt_svc_changed svc_chg = {0x0001, 0xFFFF};
static uint16_t char_chanded_cccd = 0;
	
static const att_db_t att_db[] = {
        // Index            p_UUID           p_value                      v_len                           properties
        // GAP Service
        [IDX_GATTM_SERVICE]     		= {ATT_SERV_UUID,  (uint8_t *)serv_gap,         sizeof(serv_gap),	ATT_PROP_READ},
        [IDX_GATTM_CHAR_SVC_CHANGED]  	= {ATT_CHAR_UUID,  NULL,                        0,			ATT_PROP_READ},
        [IDX_GATTM_SVC_CHANGED]   		= {char_changed,   (uint8_t *)&svc_chg,  		sizeof(svc_chg),   	ATT_PROP_IND},
        [IDX_GATTM_SVC_CHANGED_CFG]   	= {ATT_CCCD_UUID, (uint8_t *)&char_chanded_cccd,	sizeof(char_chanded_cccd),	ATT_PROP_READ | ATT_PROP_WRITE, 0, 0, 0, sizeof(uint16_t)},
};

void gatt_service_init(void)
{
    static const gatt_serv_db_t service_db = { att_db, sizeof(att_db) / sizeof(att_db[0]) };
    gatt_database_add(&service_db, &m_start_handle);
    co_assert(m_start_handle);
}


