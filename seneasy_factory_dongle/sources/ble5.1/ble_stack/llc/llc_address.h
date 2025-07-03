#ifndef LLC_ADDRESS_H
#define LLC_ADDRESS_H


void rom_llc_address_init(void);

void rom_llc_address_get_own_public_address(uint8_t pu8Addr[6]);

bool rom_llc_address_get_own_random_address(uint8_t pu8Addr[6]);

bool rom_llc_address_get_own_resolvable_private_address(uint8_t pu8Addr[6]);
bool rom_llc_address_get_own_resolvable_private_address_with_peer_addr(uint8_t pu8LocalRPA[6],uint8_t u8PeerIdentifyAddrType, uint8_t pu8PeerIdentifyAddr[6]);

uint8_t rom_llc_address_set_random_address(uint8_t pu8RandomAddr[6]);

uint8_t rom_llc_address_set_public_address(uint8_t pu8PublicAddr[6]);


#endif
