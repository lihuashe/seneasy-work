#ifndef LLP_DTM_RX_H
#define LLP_DTM_RX_H

extern const stLlpDtmFragment_t g_stLlpDtmRxFragment;

EN_LLP_ERR_T rom_llp_dtm_rx_fragment_start(stLlpDtmRxFragmentParam_t* pstParam);

EN_LLP_DTM_FRAGMENT_RET rom_llp_dtm_rx_on_fragment_start(void);

void rom_llp_dtm_rx_on_fragment_stop(void);

EN_LLP_DTM_FRAGMENT_RET rom_llp_dtm_rx_on_interrupt(uint32_t u32IntState);

#endif
