#ifndef LLC_DATA_H
#define LLC_DATA_H


////forward declaration
struct stLlcConnectionRoleNode;
struct stLlcCtrlProcedureNode;
////


typedef enum
{
    LLC_DATA_PDU_PROCESS_RESULT_REMOVE = 0,
    LLC_DATA_PDU_PROCESS_RESULT_KEEP,
    LLC_DATA_PDU_PROCESS_RESULT_DISCONNECTED,
}
EN_LLC_DATA_PDU_PROCESS_RESULT_T;

void rom_llc_data_connection_role_init(struct stLlcConnectionRoleNode* pstRole);
void rom_llc_data_process_main( struct stLlcConnectionRoleNode *pstRole );
void rom_llc_data_process_on_connection_event_closed( struct stLlcConnectionRoleNode *pstRole );
void rom_llc_data_disconnect_when_rx_data_pdu(struct stLlcConnectionRoleNode* pstRole, bool bDisconnectWhenRecvDataPdu);
void rom_llc_data_enable_tx_data_pdu_process_async( struct stLlcConnectionRoleNode *pstRole, bool bEnable);
bool rom_llc_data_is_tx_data_pdu_process_enabled( struct stLlcConnectionRoleNode *pstRole );

bool rom_llc_data_is_queued_tx_acl_data_list_empty( struct stLlcConnectionRoleNode *pstRole );
bool rom_llc_data_is_prepared_tx_pdu_list_empty( struct stLlcConnectionRoleNode *pstRole );
bool rom_llc_data_is_queued_tx_ctrl_pdu_list_empty( struct stLlcConnectionRoleNode *pstRole );
bool rom_llc_data_is_queued_tx_data_pdu_list_empty( struct stLlcConnectionRoleNode *pstRole );
bool rom_llc_data_is_transmiting_pdu_empty( struct stLlcConnectionRoleNode *pstRole );

#endif

