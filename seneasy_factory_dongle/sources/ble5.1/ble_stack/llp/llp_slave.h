#ifndef LLP_SLAVE_H
#define LLP_SLAVE_H

extern const stLlpFragment_t g_stLlpSlaveFragment;


EN_LLP_ERR_T rom_llp_slave_fragment_start(stLlpSlaveFragmentParam_t* pstParam);


EN_LLP_FRAGMENT_RET rom_llp_slave_on_fragment_start(void);


void rom_llp_slave_on_fragment_stop(void);

EN_LLP_FRAGMENT_RET rom_llp_slave_on_interrupt(uint32_t u32IntState);

#endif
