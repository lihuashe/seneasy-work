#ifndef LLP_INITIATOR_H
#define LLP_INITIATOR_H


extern const stLlpFragment_t g_stLlpInitiatorFragment;


EN_LLP_ERR_T rom_llp_initiator_fragment_start(stLlpInitFragmentParam_t* pstParam);


EN_LLP_FRAGMENT_RET rom_llp_initiator_on_fragment_start(void);

void rom_llp_initiator_on_fragment_stop(void);


EN_LLP_FRAGMENT_RET rom_llp_initiator_on_interrupt(uint32_t u32IntState);



#endif
