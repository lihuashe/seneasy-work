#ifndef LLC_EXT_ADV_STATE_MACHINE_H
#define LLC_EXT_ADV_STATE_MACHINE_H


void rom_llc_ext_adv_state_machine_enable(stLlcExtAdv_t* pstAdv, bool bEnable);

uint8_t rom_llc_ext_adv_state_machine_set_updating_pdus_time(stLlcExtAdv_t* pstAdv);

#endif
