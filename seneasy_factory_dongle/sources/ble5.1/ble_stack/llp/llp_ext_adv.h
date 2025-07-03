#ifndef LLP_EXT_ADV_H
#define LLP_EXT_ADV_H


extern const stLlpFragment_t g_stLlpExtAdvFragment;


EN_LLP_ERR_T rom_llp_ext_adv_fragment_start(stLlpExtAdvFragmentParam_t* pstParam);


EN_LLP_FRAGMENT_RET rom_llp_ext_adv_on_fragment_start(void);

void rom_llp_ext_adv_on_fragment_stop(void);


EN_LLP_FRAGMENT_RET rom_llp_ext_adv_on_interrupt(uint32_t u32IntState);



#endif
