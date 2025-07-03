
#ifndef LLP_DTM_H
#define LLP_DTM_H


typedef enum
{
    LLP_DTM_FRAGMENT_CONTINUE = 0,
    LLP_DTM_FRAGMENT_STOP,
}
EN_LLP_DTM_FRAGMENT_RET;



typedef EN_LLP_DTM_FRAGMENT_RET (*callback_llp_dtm_fragment_on_start)(void);
typedef void (*callback_llp_dtm_fragment_on_stop)(void);
typedef EN_LLP_DTM_FRAGMENT_RET (*callback_llp_dtm_fragment_on_interrupt) (uint32_t u32IntState);

typedef struct
{
    
    EN_LLP_FRAGMENT_TYPE_T enumFragmentType;
    callback_llp_dtm_fragment_on_interrupt fragment_interrupt_callback;
}
stLlpDtmFragment_t;



void rom_llp_dtm_init(void);


void rom_llp_dtm_set_tx_end_report_callback(callback_on_llp_tx_end callback);

void rom_llp_dtm_set_rx_end_report_callback(callback_on_llp_rx_end callback);


EN_LLP_ERR_T rom_llp_dtm_start_tx_fragment(stLlpDtmTxFragmentParam_t *pstParam);


EN_LLP_ERR_T rom_llp_dtm_start_rx_fragment(stLlpDtmRxFragmentParam_t *pstParam);

EN_LLP_ERR_T rom_llp_dtm_stop_fragment(void);


EN_LLP_STATE_CMD_T rom_llp_dtm_report_tx_end(stLlpTxEndReport_t* pstReport);


EN_LLP_STATE_CMD_T rom_llp_dtm_report_rx_end(stLlpRxEndReport_t* pstReport);



#endif
