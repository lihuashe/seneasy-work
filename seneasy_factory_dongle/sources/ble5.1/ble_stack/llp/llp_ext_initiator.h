#ifndef LLP_EXT_INITIATOR_H
#define LLP_EXT_INITIATOR_H


extern const stLlpFragment_t g_stLlpExtInitiatorFragment;


EN_LLP_ERR_T rom_llp_ext_initiator_fragment_start(stLlpExtInitFragmentParam_t* pstParam);


EN_LLP_FRAGMENT_RET rom_llp_ext_initiator_on_fragment_start(void);

void rom_llp_ext_initiator_on_fragment_stop(void);


EN_LLP_FRAGMENT_RET rom_llp_ext_initiator_on_interrupt(uint32_t u32IntState);



#endif
