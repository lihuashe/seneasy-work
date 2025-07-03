#ifndef LLC_ENCRYPTION_PROCEDURE_H
#define LLC_ENCRYPTION_PROCEDURE_H

void rom_llc_encryption_procedure_init(stLlcConnectionRole_t *pstRole);

bool rom_llc_encryption_procedure_peer_initiated_init(stLlcConnectionRole_t *pstRole, stLlcCtrlProcedure_t* pstProcedure, stLlcDataChannelPdu_t* pstFirstPdu);

uint8_t rom_llc_encryption_procedure_self_initiated_init(stLlcConnectionRole_t *pstRole, stLlcCtrlProcedure_t* pstProcedure, bool bPending,
    uint8_t *pu8Rand64bit, uint8_t* pu8EDIV16bit, uint8_t *pu8LTK128bit);

EN_LLC_CTRL_PROCEDURE_ERR_T rom_llc_encryption_procedure_on_recv_ctrl_pdu(stLlcConnectionRole_t *pstRole, stLlcCtrlProcedure_t* pstProcedure, stLlcDataChannelPdu_t* pstCtrlPdu);

uint8_t rom_llc_encryption_procedure_ltk_req_reply ( stLlcConnectionRole_t *pstRole, stLlcCtrlProcedure_t* pstProcedure,  uint8_t *pu8LTK );

#endif
