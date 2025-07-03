#ifndef LLC_SLAVE_H
#define LLC_SLAVE_H


typedef struct
{
    uint64_t u64TimeOfLastSync;
    uint64_t u64TransmitWindowStartTime;
    uint16_t u16TransmitWindowSizeUs;
    
    uint32_t u32AccessAddress;
    uint32_t u32CrcInit;
    uint16_t u16ConnItv1250us;
    uint16_t u16ConnSlaveLatency;
    uint16_t u16ConnSupervisionTimeout10ms;
    bool bChSel;
    uint8_t pu8ChM[5];
    uint8_t u8Hop;
    uint8_t u8Sca;
    EN_LL_PHY_T enumPhy;
    uint8_t u8SelfAddrType;
    uint8_t pu8SelfAddr[6];
    uint8_t u8PeerAddrType;
    uint8_t pu8PeerAddr[6];
    uint32_t u32RssiRelatedValue;
    
    uint8_t Peer_RPA_Resoluted; //only  Peer_RPA_Resoluted,Peer_Identify_Address exists
    uint8_t Peer_Identify_Address_Type;
    uint8_t Peer_Identify_Address[6];
    uint8_t Own_Address_Be_RPA;
    
} __attribute__((aligned(8)))
stLlcSlaveInitParam_t;


bool rom_llc_slave_init(stLlcConnectionRole_t* pstRole, stLlcSlaveInitParam_t* pstParam);

uint32_t rom_llc_slave_on_event_blocked(void* pstRoleInstance);

void rom_llc_slave_on_scheduler_event_prepare(void* pstRoleInstance);

EN_LLP_ERR_T rom_llc_slave_config_llp (void* pstRoleInstance, uint32_t u32MaxWorkWindowTimeUs);

void rom_llc_slave_on_llp_fragment_start(void* pstRoleInstance);


bool rom_llc_slave_on_llp_fragment_stop(void* pstRoleInstance);

EN_LLP_STATE_CMD_T rom_llc_slave_on_llp_tx_end(void* pstRoleInstance, stLlpTxEndReport_t *pstReport);

EN_LLP_STATE_CMD_T rom_llc_slave_on_llp_rx_end(void* pstRoleInstance, stLlpRxEndReport_t *pstReport);

#endif
