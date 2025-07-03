#ifndef LLC_EXT_SCAN_DUPLICATE_FILTER_H
#define LLC_EXT_SCAN_DUPLICATE_FILTER_H

typedef struct
{
    uint64_t u64VersionNumer;
    
    EN_LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_T enumAddressType;
    uint8_t pu8Address[6];
    uint8_t u8SID;
    
    bool bMemUsed;
} __attribute__((aligned(8)))
stLlcExtScanDuplicateFilterReport_t;

void rom_llc_ext_scan_duplicate_filter_init(stLlcExtScanDuplicateFilterReport_t* pstLlcExtScanDuplicateFilterReportMem, uint8_t u8LlcExtScanDuplicateFilterReportNum);

void rom_llc_ext_scan_duplicate_filter_reset(void);

void rom_llc_ext_scan_duplicate_filter_add_report(EN_LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_T enumAddressType, uint8_t pu8Address[6], uint8_t u8SID);

bool rom_llc_ext_scan_duplicate_filter_is_report_duplicated(EN_LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_T enumAddressType, uint8_t pu8Address[6], uint8_t u8SID);

#endif
