#ifndef LLC_PHY_UPDATE_PROCEDURE_H
#define LLC_PHY_UPDATE_PROCEDURE_H

void rom_llc_phy_update_procedure_init(stLlcConnectionRole_t *pstRole);

bool rom_llc_phy_update_procedure_peer_initiated_init(stLlcConnectionRole_t *pstRole, stLlcCtrlProcedure_t* pstProcedure, stLlcDataChannelPdu_t* pstFirstPdu);

uint8_t rom_llc_phy_update_procedure_self_initiated_init(stLlcConnectionRole_t *pstRole, stLlcCtrlProcedure_t* pstProcedure, bool bPending,
    uint8_t u8AllPhys, uint8_t u8TxPhys, uint8_t u8RxPhys, uint16_t u16PhyOptions, bool bIsInitialtedByHost);

EN_LLC_CTRL_PROCEDURE_ERR_T rom_llc_phy_update_procedure_on_recv_ctrl_pdu(stLlcConnectionRole_t *pstRole, stLlcCtrlProcedure_t* pstProcedure, stLlcDataChannelPdu_t* pstCtrlPdu);


EN_LLC_CTRL_PROCEDURE_ON_EVENT_CLOSED_RESULT_T rom_llc_phy_update_procedure_on_conn_event_closed(stLlcConnectionRole_t *pstRole, stLlcCtrlProcedure_t* pstProcedure);

void rom_llc_phy_update_procedure_on_procedure_rejected(stLlcConnectionRole_t *pstRole, stLlcCtrlProcedure_t* pstProcedure, uint8_t u8ErrorCode);

uint8_t rom_llc_phy_update_procedure_set_default_phy(uint8_t u8AllPhys, uint8_t u8TxPhys, uint8_t u8RxPhys);

#endif
