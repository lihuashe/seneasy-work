#ifndef LLC_ACL_DATA_H
#define LLC_ACL_DATA_H



typedef struct stLlcAclDataNode
{
    uint16_t u16ConnHandle;
    uint8_t u8PacketBoundaryFlag;
    uint8_t u8BroacastFlag;
    uint16_t u16DataOffset;
    uint16_t u16DataLength;
    uint8_t* pu8Payload;
    uint16_t u16PayloadBufSize;
    struct stLlcAclDataNode* prev;
    struct stLlcAclDataNode* next;
    bool bMemUsed;
    uint32_t u32Reserved;
} __attribute__((aligned(4)))
stLlcAclData_t;


bool rom_llc_acl_data_mem_init(stLlcAclData_t* pstAclDataMemBuffer, uint16_t u16AclDataSize, uint8_t u8AclDataNum);

uint16_t rom_llc_acl_data_get_max_size(void);

uint8_t rom_llc_acl_data_get_max_num(void);

uint8_t rom_llc_acl_data_get_remaining_num(void);

stLlcAclData_t* rom_llc_acl_data_alloc(void);

void rom_llc_acl_data_free(stLlcAclData_t* pstAclData);

#endif
