
#ifndef LLP_DTM_TX_H
#define LLP_DTM_TX_H

extern const stLlpDtmFragment_t g_stLlpDtmTxFragment;

EN_LLP_ERR_T rom_llp_dtm_tx_fragment_start(stLlpDtmTxFragmentParam_t* pstParam);

EN_LLP_DTM_FRAGMENT_RET rom_llp_dtm_tx_on_fragment_start(void);

void rom_llp_dtm_tx_on_fragment_stop(void);

EN_LLP_DTM_FRAGMENT_RET rom_llp_dtm_tx_on_interrupt(uint32_t u32IntState);

#endif
