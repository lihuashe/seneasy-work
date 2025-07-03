#ifndef PATCH_LLP_H
#define PATCH_LLP_H

void patch_llp(void);

EN_LLP_ERR_T patch_llp_start_adv_fragment(stLlpAdvFragmentParam_t *pstParam);

EN_LLP_ERR_T patch_llp_start_scan_fragment(stLlpScanFragmentParam_t *pstParam);

EN_LLP_ERR_T patch_llp_start_init_fragment(stLlpInitFragmentParam_t *pstParam);

EN_LLP_ERR_T patch_llp_start_slave_fragment(stLlpSlaveFragmentParam_t *pstParam);

EN_LLP_ERR_T patch_llp_start_master_fragment(stLlpMasterFragmentParam_t *pstParam);

EN_LLP_ERR_T patch_llp_start_ext_adv_fragment(stLlpExtAdvFragmentParam_t *pstParam);

EN_LLP_ERR_T patch_llp_start_ext_scan_fragment(stLlpExtScanFragmentParam_t *pstParam);

EN_LLP_ERR_T patch_llp_start_ext_init_fragment(stLlpExtInitFragmentParam_t *pstParam);

#endif
