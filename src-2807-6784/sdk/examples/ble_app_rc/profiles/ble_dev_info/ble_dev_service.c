#include <string.h>
#include "co_tools.h"
#include "ble_dev_service.h"
#include "gatt_sig_uuid.h"

#define APP_LOG_DOMAIN      "serdev"
#define APP_LOG_LEVEL       APP_DEV_SERVICE_LOG_LEVEL
#include "co_debug.h"

static uint16_t m_start_handle;

static const uint8_t serv_dis[]      = UUID16_ARR(DEVINFO_SERV_UUID);
static const uint8_t char_namufact[] = UUID16_ARR(MANUFACTURER_NAME_UUID);
static const uint8_t char_model[]    = UUID16_ARR(MODEL_NUMBER_UUID);
static const uint8_t char_serial[]   = UUID16_ARR(SERIAL_NUMBER_UUID);
static const uint8_t char_hard_rev[] = UUID16_ARR(HARDWARE_REV_UUID);
static const uint8_t char_firm_rev[] = UUID16_ARR(FIRMWARE_REV_UUID);
static const uint8_t char_soft_rev[] = UUID16_ARR(SOFTWARE_REV_UUID);
static const uint8_t char_sys_id[]   = UUID16_ARR(SYSTEM_ID_UUID);
static const uint8_t char_2a2a[]     = UUID16_ARR(IEEE_11073_CERT_DATA_UUID);
static const uint8_t char_pnp_id[]   = UUID16_ARR(PNP_ID_UUID);
 

 static const att_db_t att_db[] = {
        // Index            p_UUID           p_value                      v_len                           properties
        // Device Infomation Service
        [IDX_DIS_SVC]     = {ATT_SERV_UUID,  (uint8_t *)serv_dis,         sizeof(serv_dis),               ATT_PROP_READ},
#if ((DIS_FEATURES & DIS_MANUFACTURER_NAME_CHAR_SUP) == DIS_MANUFACTURER_NAME_CHAR_SUP)
        [IDX_MANU_CHAR]   = {ATT_CHAR_UUID,  NULL,                        0,                              ATT_PROP_READ},
        [IDX_MANU_VAL]    = {char_namufact,  (uint8_t *)DIS_MANU_STR,     sizeof(DIS_MANU_STR) - 1,       ATT_PROP_READ},
#endif
		
#if ((DIS_FEATURES & DIS_MODEL_NB_STR_CHAR_SUP) == DIS_MODEL_NB_STR_CHAR_SUP)
        [IDX_MODEL_CHAR]  = {ATT_CHAR_UUID,  NULL,                        0,                              ATT_PROP_READ},
        [IDX_MODEL_VAL]   = {char_model,     (uint8_t *)DIS_MODEL_NUMBER, sizeof(DIS_MODEL_NUMBER) - 1,   ATT_PROP_READ},
#endif
		
#if ((DIS_FEATURES & DIS_SERIAL_NB_STR_CHAR_SUP) == DIS_SERIAL_NB_STR_CHAR_SUP)
		[IDX_SERIAL_CHAR] = {ATT_CHAR_UUID,  NULL,                        0,                              ATT_PROP_READ},
        [IDX_SERIAL_VAL]  = {char_serial,    (uint8_t *)DIS_SERIAL_NUMBER, sizeof(DIS_SERIAL_NUMBER) - 1, ATT_PROP_READ},
#endif
		
#if ((DIS_FEATURES & DIS_HARD_REV_STR_CHAR_SUP) == DIS_HARD_REV_STR_CHAR_SUP)
        [IDX_H_REV_CHAR]  = {ATT_CHAR_UUID,  NULL,                        0,                              ATT_PROP_READ},
        [IDX_H_REV_VAL]   = {char_hard_rev,  (uint8_t *)DIS_HARD_VERSION, sizeof(DIS_HARD_VERSION) - 1,   ATT_PROP_READ},
#endif
		
#if ((DIS_FEATURES & DIS_FIRM_REV_STR_CHAR_SUP) == DIS_FIRM_REV_STR_CHAR_SUP)
        [IDX_F_REV_CHAR]  = {ATT_CHAR_UUID,  NULL,                        0,                              ATT_PROP_READ},
        [IDX_F_REV_VAL]   = {char_firm_rev,  (uint8_t *)DIS_FIRM_VERSION, sizeof(DIS_FIRM_VERSION) - 1,   ATT_PROP_READ},
#endif	
	
#if ((DIS_FEATURES & DIS_SW_REV_STR_CHAR_SUP) == DIS_SW_REV_STR_CHAR_SUP)			
        [IDX_S_REV_CHAR]  = {ATT_CHAR_UUID,  NULL,                        0,                              ATT_PROP_READ},
        [IDX_S_REV_VAL]   = {char_soft_rev,  (uint8_t *)DIS_SOFT_VERSION, sizeof(DIS_SOFT_VERSION) - 1,   ATT_PROP_READ},
#endif

#if ((DIS_FEATURES & DIS_SYSTEM_ID_CHAR_SUP) == DIS_SYSTEM_ID_CHAR_SUP)		
        [IDX_SYSID_CHAR]  = {ATT_CHAR_UUID,  NULL,                        0,                              ATT_PROP_READ},
        [IDX_SYSID_VAL]   = {char_sys_id,    (uint8_t *)DIS_SYSTEM_ID,    sizeof(DIS_SYSTEM_ID) - 1,      ATT_PROP_READ},
#endif
	
#if ((DIS_FEATURES & DIS_IEEE_CHAR_SUP) == DIS_IEEE_CHAR_SUP)
        [IDX_2A2A_CHAR]   = {ATT_CHAR_UUID,  NULL,                        0,                              ATT_PROP_READ},
        [IDX_2A2A_VAL]    = {char_2a2a,      (uint8_t *)DIS_2A2A,         sizeof(DIS_2A2A) - 1,           ATT_PROP_READ},
#endif
				
#if ((DIS_FEATURES & DIS_PNP_ID_CHAR_SUP) == DIS_PNP_ID_CHAR_SUP)		
        [IDX_PNPID_CHAR]  = {ATT_CHAR_UUID,  NULL,                        0,                              ATT_PROP_READ},
        [IDX_PNPID_VAL]   = {char_pnp_id,    (uint8_t *)DIS_PNP_ID,       sizeof(DIS_PNP_ID) - 1,         ATT_PROP_READ},
#endif
};
 
void ble_dev_service_init(void)
{
    static const gatt_serv_db_t service_db = { att_db, sizeof(att_db) / sizeof(att_db[0]) };
    gatt_database_add(&service_db, &m_start_handle);
    co_assert(m_start_handle);
}


