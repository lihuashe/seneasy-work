#ifndef LLP_MASTER_H
#define LLP_MASTER_H

extern const stLlpFragment_t g_stLlpMasterFragment;


EN_LLP_ERR_T rom_llp_master_fragment_start(stLlpMasterFragmentParam_t* pstParam);


EN_LLP_FRAGMENT_RET rom_llp_master_on_fragment_start(void);


void rom_llp_master_on_fragment_stop(void);

EN_LLP_FRAGMENT_RET rom_llp_master_on_interrupt(uint32_t u32IntState);

#endif
