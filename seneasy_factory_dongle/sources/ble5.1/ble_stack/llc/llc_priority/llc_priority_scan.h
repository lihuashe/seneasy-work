#ifndef LLC_PRIORITY_SCAN_H
#define LLC_PRIORITY_SCAN_H


void rom_llc_priority_scan_on_event_added(stLlcSchedulerEvent_t* pstEvent);

void rom_llc_priority_scan_on_event_config_failed(stLlcSchedulerEvent_t* pstEvent);

void rom_llc_priority_scan_on_event_blocked(stLlcSchedulerEvent_t* pstEvent, uint32_t u32BlockedCount);

void rom_llc_priority_scan_on_event_stopped(stLlcSchedulerEvent_t* pstEvent);


#endif
