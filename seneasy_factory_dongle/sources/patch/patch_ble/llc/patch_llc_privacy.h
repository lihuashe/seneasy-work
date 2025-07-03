#ifndef PATCH_LLC_PRIVACY_H
#define PATCH_LLC_PRIVACY_H

void patch_llc_privacy(void);

uint8_t patch_llc_privacy_Add_Device_To_Resolving_List (uint8_t Peer_Identity_Address_Type,
    uint8_t Peer_Identity_Address[6],
    uint8_t Peer_IRK[16],
    uint8_t Local_IRK[16]);
    
    
uint8_t patch_llc_privacy_Remove_Device_From_Resolving_List (uint8_t Peer_Identity_Address_Type, uint8_t Peer_Identity_Address[6]);

#endif
