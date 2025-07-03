#ifndef LLC_CHANNEL_SELECTION_ALGORITHM_H
#define LLC_CHANNEL_SELECTION_ALGORITHM_H


//return : channel idx
uint8_t rom_llc_channel_selection_algorithm_no_1(uint8_t* pu8ChM5Octecs, uint32_t u32HopIncrement, uint8_t* pu8LastUnmappedChannel);

uint8_t rom_llc_channel_selection_algorithm_no_2(uint8_t* pu8ChM5Octecs, uint16_t u16Counter, uint32_t u32AccessAddress);

#endif
