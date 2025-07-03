#ifndef LLC_EXT_SCAN_STATE_MACHINE_H
#define LLC_EXT_SCAN_STATE_MACHINE_H

void rom_llc_ext_scan_state_machine_init(stLlcExtScan_t* pstScan);

void rom_llc_ext_scan_state_machine_update_scan_req_pdu_AdvA(stLlcExtScan_t* pstScan, uint8_t AdvA[6], bool RxAdd);

void rom_llc_ext_scan_state_machine_update_scan_req_pdu_ScanA(stLlcExtScan_t* pstScan, uint8_t ScanA[6], bool TxAdd);

void rom_llc_ext_scan_state_machine_enable(stLlcExtScan_t* pstScan, bool bEnable);

void rom_llc_ext_scan_state_machine_schedule_next_scanning(stLlcExtScan_t* pstScan);

void rom_llc_ext_scan_state_machine_backoff_scan_result(stLlcExtScan_t* pstScan, bool bSuccess);

bool rom_llc_ext_scan_state_machine_backoff_is_scan_canceled(stLlcExtScan_t* pstScan);

#endif
