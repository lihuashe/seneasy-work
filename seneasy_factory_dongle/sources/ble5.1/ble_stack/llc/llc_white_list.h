#ifndef LLC_WHITE_LIST_H
#define LLC_WHITE_LIST_H

typedef struct
{
    uint8_t u8AddrType;
    bool bMemUsed;
    uint8_t pu8Addr[6];
} __attribute__((aligned(8)))
stLlcWhiteListDevice_t;


bool rom_llc_white_list_init(stLlcWhiteListDevice_t* pstWhiteListBuf, uint8_t u8WhiteListMaxNum);

bool rom_llc_white_list_is_device_in_list(uint8_t u8AddrType, uint8_t pu8Addr[6]);

uint8_t rom_llc_white_list_read_size(uint8_t* pu8Size);

uint8_t rom_llc_white_list_clear(void);

uint8_t rom_llc_white_list_add_device(uint8_t u8AddressType, uint8_t pu8Address[6]);

uint8_t rom_llc_white_list_remove_device(uint8_t u8AddressType, uint8_t pu8Address[6]);

#endif
