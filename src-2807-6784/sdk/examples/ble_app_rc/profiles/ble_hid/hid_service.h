#ifndef __HID_SERVICE_H__
#define __HID_SERVICE_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "board_config.h"

enum {
    IDX_HID_SVC,
    IDX_HID_INFO_CHAR,
    IDX_HID_INFO_VAL,
	
    IDX_HID_CTRL_CHAR,
    IDX_HID_CTRL_VAL,
    
	IDX_HID_REP_MAP_CHAR,
    IDX_HID_REP_MAP_VAL,
    
	IDX_HID_PRO_MODE_CHAR,
    IDX_HID_PRO_MODE_VAL,
    
	IDX_HID_BKI_REP_CHAR,
    IDX_HID_BKI_REP_VAL,
    IDX_HID_BKI_REP_DESC,
    IDX_HID_BKO_REP_CHAR,
    IDX_HID_BKO_REP_VAL,
    
	IDX_HID_REPORT1_CHAR,
    IDX_HID_REPORT1_VAL,
    IDX_HID_REPORT1_REF,
    IDX_HID_REPORT1_DESC,
	
    IDX_HID_REPORT2_CHAR,
    IDX_HID_REPORT2_VAL,
    IDX_HID_REPORT2_REF,
	IDX_HID_REPORT2_DESC,
	
    IDX_HID_REPORT3_CHAR,
    IDX_HID_REPORT3_VAL,
    IDX_HID_REPORT3_REF,
    IDX_HID_REPORT3_DESC,
	
    IDX_HID_REPORT4_CHAR,
    IDX_HID_REPORT4_VAL,
    IDX_HID_REPORT4_REF,
    IDX_HID_REPORT4_DESC,
	
	IDX_HID_REPORT5_CHAR,
    IDX_HID_REPORT5_VAL,
    IDX_HID_REPORT5_REF,
    IDX_HID_REPORT5_DESC,
};

#define KEYBOARD_HANDLE				IDX_HID_REPORT2_VAL
#define CONSUMER_HANDLE				IDX_HID_REPORT1_VAL
#define MOUSE_HANDLE				IDX_HID_REPORT3_VAL	

void hid_service_init(void);
void hid_evt_cb(ble_evt_t *p_evt);
bool hid_gatt_report_notify(uint8_t rpt_info_id, uint8_t *p_data, uint16_t len);
bool hid_cccd_is_enable(void);
#endif /* __HID_XIAOMI_SERVICE_H__ */
