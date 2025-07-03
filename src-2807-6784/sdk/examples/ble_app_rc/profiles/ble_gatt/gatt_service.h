#ifndef __GATT_SERVICE_H__
#define __GATT_SERVICE_H__

#include "ble.h"

enum {
    IDX_GATTM_SERVICE,
    IDX_GATTM_CHAR_SVC_CHANGED,       // GATT_IDX_CHAR_SVC_CHANGED - Service Changed declaration
    IDX_GATTM_SVC_CHANGED,            // GATT_IDX_SVC_CHANGED - Service Changed definition
    IDX_GATTM_SVC_CHANGED_CFG,        // GATT_IDX_SVC_CHANGED_CFG - Service Changed Client Characteristic Configuration Descriptor
};


void gatt_service_init(void);

#endif /* __COMMON_SERVICE_XIAOMI_H__ */
