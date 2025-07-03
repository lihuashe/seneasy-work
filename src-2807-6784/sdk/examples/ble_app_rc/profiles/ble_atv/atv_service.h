#ifndef __ATV_SERVICE_H
#define __ATV_SERVICE_H

#include "ble.h"
#include "board_config.h"

enum {
    ATVVS_IDX_SVC,

    ATVVS_IDX_COMMAND_CHAR,
    ATVVS_IDX_COMMAND_VAL,

    ATVVS_IDX_AUDIO_CHAR,
    ATVVS_IDX_AUDIO_VAL,
    ATVVS_IDX_AUDIO_CCC,

    ATVVS_IDX_CONTROL_CHAR,
    ATVVS_IDX_CONTROL_VAL,
    ATVVS_IDX_CONTROL_CCC,
};

	
void atv_service_init(void);
void atv_service_evt_cb(ble_evt_t *p_evt);
void atv_ntf_data(uint8_t att_idx,uint8_t *data,uint16_t len);

#endif //__ATV_SERVICE_H
