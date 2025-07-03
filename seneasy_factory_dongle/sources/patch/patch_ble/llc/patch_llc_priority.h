#ifndef PATCH_LLC_PRIORITY_H
#define PATCH_LLC_PRIORITY_H

uint8_t rom_llc_priority_get_add_level(uint8_t u8CurPriority, uint32_t u32AddPriority, uint8_t u8LimitPriority);

uint8_t rom_llc_priority_get_sub_level(uint8_t u8CurPriority, uint32_t u32SubPriority, uint8_t u8LimitPriority);

void patch_llc_priority(void);


#endif
