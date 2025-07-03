#ifndef LLP_SCAN_H
#define LLP_SCAN_H


extern const stLlpFragment_t g_stLlpScanFragment;


EN_LLP_ERR_T rom_llp_scan_fragment_start(stLlpScanFragmentParam_t* pstParam);


EN_LLP_FRAGMENT_RET rom_llp_scan_on_fragment_start(void);

void rom_llp_scan_on_fragment_stop(void);


EN_LLP_FRAGMENT_RET rom_llp_scan_on_interrupt(uint32_t u32IntState);



#endif
