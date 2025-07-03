#ifndef LLC_PRIORITY_H
#define LLC_PRIORITY_H


////for scheduler

void rom_llc_priority_on_event_added(stLlcSchedulerEvent_t* pstEvent);

void rom_llc_priority_on_event_started(stLlcSchedulerEvent_t* pstEvent);

void rom_llc_priority_on_event_config_failed(stLlcSchedulerEvent_t* pstEvent);

void rom_llc_priority_on_event_blocked(stLlcSchedulerEvent_t* pstEvent, uint32_t u32BlockedCount);

void rom_llc_priority_on_event_stopped(stLlcSchedulerEvent_t* pstEvent);

bool llc_priority_is_a_higher_than_b(uint8_t u8PriorityA, uint8_t u8PriorityB);

////



////for priority

void rom_llc_priority_add(stLlcSchedulerEvent_t* pstEvent, uint32_t u32AddLevel, uint8_t u8LimitLevel);

void rom_llc_priority_sub(stLlcSchedulerEvent_t* pstEvent, uint32_t u32SubLevel, uint8_t u8LimitLevel);

////

#endif
