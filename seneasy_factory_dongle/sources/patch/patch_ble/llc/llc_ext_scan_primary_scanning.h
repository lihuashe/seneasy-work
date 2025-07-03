#ifndef LLC_EXT_SCAN_PRIMARY_SCANNING_H
#define LLC_EXT_SCAN_PRIMARY_SCANNING_H


#define LLC_EXT_SCAN_PRIMARY_SCANNING_MIN_WINDOW_US (2500)


#define LLC_EXT_SCAN_PRIMARY_SCANNING_WINDOW_ADJUSTABLE (1)


void rom_llc_ext_scan_primary_scanning_clear_phy(stLlcExtScan_t* pstScan);

uint8_t rom_llc_ext_scan_primary_scanning_get_phy_num(stLlcExtScan_t* pstScan);

bool rom_llc_ext_scan_primary_scanning_add_phy(stLlcExtScan_t* pstScan, EN_LL_PHY_T enumPhy, uint8_t Scan_Type, uint16_t Scan_Interval, uint16_t Scan_Window, uint8_t Channel_Map);

void rom_llc_ext_scan_primary_scanning_on_enable(stLlcExtScan_t* pstScan);

void rom_llc_ext_scan_primary_scanning_on_disable(stLlcExtScan_t* pstScan);

void rom_llc_ext_scan_primary_scanning_prepare_timing(stLlcExtScan_t* pstScan, bool bStart);

bool rom_llc_ext_scan_primary_scanning_get_timing(stLlcExtScan_t* pstScan, stLlcExtScanTiming_t* pstTiming);

void rom_llc_ext_scan_primary_scanning_on_scheduled(stLlcExtScan_t* pstScan);

uint32_t rom_llc_ext_scan_primary_scanning_on_event_blocked (stLlcExtScan_t* pstScan);

EN_LLP_ERR_T rom_llc_ext_scan_primary_scanning_config_llp(stLlcExtScan_t* pstScan, uint32_t u32MaxWorkWindowTimeUs);

void rom_llc_ext_scan_primary_scanning_on_llp_fragment_start(stLlcExtScan_t* pstScan);

bool rom_llc_ext_scan_primary_scanning_on_llp_fragment_stop(stLlcExtScan_t* pstScan);

EN_LLP_STATE_CMD_T rom_llc_ext_scan_primary_scanning_on_llp_tx_end(stLlcExtScan_t* pstScan, stLlpTxEndReport_t *pstReport);

EN_LLP_STATE_CMD_T rom_llc_ext_scan_primary_scanning_on_llp_rx_end(stLlcExtScan_t* pstScan, stLlpRxEndReport_t *pstReport);

#endif
