#ifndef __BATT_SERVICE_H
#define __BATT_SERVICE_H

#include "ble.h"
#include "board_config.h"

enum {
    IDX_BATT_SVC,
    IDX_BATVAL_CHAR,
    IDX_BATVAL_VAL,
    IDX_BATVAL_DESC,
};

	
void batt_service_init(void);
void batt_service_evt_cb(ble_evt_t *p_evt);
void batt_level_change(uint8_t val);

#endif //__BATT_SERVICE_H
