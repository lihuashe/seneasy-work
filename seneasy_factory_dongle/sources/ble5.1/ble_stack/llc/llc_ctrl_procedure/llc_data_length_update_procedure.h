#ifndef LLC_DATA_LENGTH_UPDATE_PROCEDURE_H
#define LLC_DATA_LENGTH_UPDATE_PROCEDURE_H

void rom_llc_data_length_update_procedure_init(stLlcConnectionRole_t *pstRole);

bool rom_llc_data_length_update_procedure_peer_initiated_init(stLlcConnectionRole_t *pstRole, stLlcCtrlProcedure_t* pstProcedure, stLlcDataChannelPdu_t* pstFirstPdu);

uint8_t rom_llc_data_length_update_procedure_self_initiated_init(stLlcConnectionRole_t *pstRole, stLlcCtrlProcedure_t* pstProcedure, bool bPending,
    uint16_t u16TxOctets, uint16_t u16TxTime);

EN_LLC_CTRL_PROCEDURE_ERR_T rom_llc_data_length_update_procedure_on_recv_ctrl_pdu(stLlcConnectionRole_t *pstRole, stLlcCtrlProcedure_t* pstProcedure, stLlcDataChannelPdu_t* pstCtrlPdu);

#endif
