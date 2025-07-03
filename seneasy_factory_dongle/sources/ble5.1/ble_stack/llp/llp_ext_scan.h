#ifndef LLP_EXT_SCAN_H
#define LLP_EXT_SCAN_H


extern const stLlpFragment_t g_stLlpExtScanFragment;


EN_LLP_ERR_T rom_llp_ext_scan_fragment_start(stLlpExtScanFragmentParam_t* pstParam);


EN_LLP_FRAGMENT_RET rom_llp_ext_scan_on_fragment_start(void);

void rom_llp_ext_scan_on_fragment_stop(void);


EN_LLP_FRAGMENT_RET rom_llp_ext_scan_on_interrupt(uint32_t u32IntState);



#endif
