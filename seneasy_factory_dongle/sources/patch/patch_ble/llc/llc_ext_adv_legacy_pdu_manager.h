#ifndef LLC_EXT_ADV_LEGACY_PDU_MANAGER_H
#define LLC_EXT_ADV_LEGACY_PDU_MANAGER_H



uint8_t rom_llc_ext_adv_legacy_pdu_manager_set_advertising_data(stLlcExtAdv_t* pstAdv, uint8_t *pu8Data, uint8_t u8DataLength);


uint8_t rom_llc_ext_adv_legacy_pdu_manager_set_scan_response_data(stLlcExtAdv_t* pstAdv, uint8_t *pu8Data, uint8_t u8DataLength);


void rom_llc_ext_adv_legacy_pdu_manager_clear(stLlcExtAdv_t* pstAdv);

void rom_llc_ext_adv_legacy_pdu_manager_accept_updating_advertising_pdu(stLlcExtAdv_t* pstAdv);

void rom_llc_ext_adv_legacy_pdu_manager_accept_updating_scan_response_pdu(stLlcExtAdv_t* pstAdv);

void rom_llc_ext_adv_legacy_pdu_manager_update_accepted_pdus_AdvA(stLlcExtAdv_t* pstAdv);

void rom_llc_ext_adv_legacy_pdu_manager_update_accepted_pdus_TargetA(stLlcExtAdv_t* pstAdv);


#endif
