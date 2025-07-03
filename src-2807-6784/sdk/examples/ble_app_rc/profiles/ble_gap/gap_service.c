#include <string.h>
#include "co_tools.h"
#include "gap_service.h"

#include "gatt_sig_uuid.h"

#define APP_LOG_DOMAIN      "sergap"
#define APP_LOG_LEVEL       APP_GAP_SERVICE_LOG_LEVEL
#include "co_debug.h"

typedef struct {
	unsigned short intervalMin; /** Minimum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  
	unsigned short intervalMax; /** Maximum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  
	unsigned short latency; /** Number of LL latency connection events (0x0000 - 0x03e8) */

	unsigned short timeout; /** Connection Timeout (0x000A - 0x0C80 * 10 ms) */
} GAP_PERI_CONNECT_PARAMS_T;

static uint16_t m_start_handle;


static const uint8_t serv_gap[]      = UUID16_ARR(GAP_SERVICE_UUID);
static const uint8_t char_devname[]  = UUID16_ARR(GATT_DEVICE_NAME_UUID);
static const uint8_t char_apperar[]  = UUID16_ARR(GATT_APPEARANCE_UUID);
static const uint8_t char_conn_param[]     = UUID16_ARR(GATT_PERI_CONN_PARAM_UUID);

static const uint8_t gap_devices_name[] = {BLE_DEVICE_NAME};
static const uint8_t gap_appearance[] =   UUID16_ARR(GAP_APPEARE_GENERIC_RC);
static const GAP_PERI_CONNECT_PARAMS_T gap_conn_param =   {10,10,0,400};

static const att_db_t att_db[] = {
	// Index            p_UUID           p_value                      v_len                           properties
	// GAP Service
	[IDX_GAP_SVC]     = {ATT_SERV_UUID,  (uint8_t *)serv_gap,         sizeof(serv_gap),             ATT_PROP_READ},
	[IDX_DNAME_CHAR]  = {ATT_CHAR_UUID,  NULL,                        0,                            ATT_PROP_READ},
	[IDX_DNAME_VAL]   = {char_devname,   (uint8_t *)gap_devices_name, sizeof(gap_devices_name),     ATT_PROP_READ},
	[IDX_APPE_CHAR]   = {ATT_CHAR_UUID,  NULL,                        0,                            ATT_PROP_READ},
	[IDX_APPE_VAL]    = {char_apperar,   (uint8_t *)gap_appearance,   sizeof(gap_appearance),       ATT_PROP_READ},
	[IDX_CON_PARAM_CHAR]   = {ATT_CHAR_UUID,  NULL,                        0,                            ATT_PROP_READ},
	[IDX_CON_PARAM_VAL]    = {char_conn_param,(uint8_t *)&gap_conn_param,   sizeof(gap_conn_param),       ATT_PROP_READ},
};

	
void gap_service_init(void)
{
    static const gatt_serv_db_t service_db = { att_db, sizeof(att_db) / sizeof(att_db[0]) };
    gatt_database_add(&service_db, &m_start_handle);
    co_assert(m_start_handle);
}


