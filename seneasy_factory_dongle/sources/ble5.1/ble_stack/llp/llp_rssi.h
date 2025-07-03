
#ifndef LLP_RSSI_H
#define LLP_RSSI_H

void rom_llp_rssi_init(void);

void rom_llp_rssi_set_trim(uint8_t u8RssiAgc0, uint8_t u8RssiAgc1, uint8_t u8RssiAgc2);

uint32_t rom_llp_rssi_get_rssi_related_value(void);

int8_t rom_llp_rssi_get_rssi_by_related_value(uint32_t u32RssiRelatedValue);


#endif
