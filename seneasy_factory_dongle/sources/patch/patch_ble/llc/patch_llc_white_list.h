#ifndef PATCH_LLC_WHITE_LIST_H
#define PATCH_LLC_WHITE_LIST_H

void patch_llc_white_list(void);

uint8_t patch_llc_white_list_remove_device(uint8_t u8AddressType, uint8_t pu8Address[6]);

#endif
