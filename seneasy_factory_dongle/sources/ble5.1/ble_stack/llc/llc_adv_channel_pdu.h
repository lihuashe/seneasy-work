#ifndef LLC_ADV_CHANNEL_PDU_H
#define LLC_ADV_CHANNEL_PDU_H


typedef struct stLlcAdvChannelPduNode
{
    uint32_t u32Header;
    uint8_t* pu8Payload;
    uint16_t u16PayloadBufSize;
    bool bMemUsed;
    uint8_t u8ReportAddrType;
    uint32_t u32RssiRelatedValue;    
    struct stLlcAdvChannelPduNode* prev;
    struct stLlcAdvChannelPduNode* next;
    uint32_t u32Reserved;
} __attribute__((aligned(4)))
stLlcAdvChannelPdu_t;

typedef struct
{
    stLlcAdvChannelPdu_t* pstLongAdvChannelPduMemBuffer;
    uint16_t u16LongAdvChannelPduPayloadSize;
    uint8_t u8LongAdvChannelPduNum;
    
    stLlcAdvChannelPdu_t* pstShortAdvChannelPduMemBuffer;
    uint16_t u16ShortAdvChannelPduPayloadSize;
    uint8_t u8ShortAdvChannelPduNum;
}
stLlcAdvChannelPduMemCfg_t;

bool rom_llc_adv_channel_pdu_mem_init(stLlcAdvChannelPduMemCfg_t* pstCfg);

uint16_t rom_llc_adv_channel_pdu_get_max_size( void );

stLlcAdvChannelPdu_t* rom_llc_adv_channel_pdu_alloc(  uint16_t  u16RequestPayloadSize);

void rom_llc_adv_channel_pdu_free( stLlcAdvChannelPdu_t* pstPdu);



#endif
