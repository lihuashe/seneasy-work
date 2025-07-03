#ifndef LLP_ADV_H
#define LLP_ADV_H


extern const stLlpFragment_t g_stLlpAdvFragment;


EN_LLP_ERR_T rom_llp_adv_fragment_start(stLlpAdvFragmentParam_t* pstParam);


EN_LLP_FRAGMENT_RET rom_llp_adv_on_fragment_start(void);

void rom_llp_adv_on_fragment_stop(void);


EN_LLP_FRAGMENT_RET rom_llp_adv_on_interrupt(uint32_t u32IntState);



#endif
