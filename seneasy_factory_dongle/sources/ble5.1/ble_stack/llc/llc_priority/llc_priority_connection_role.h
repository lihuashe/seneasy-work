#ifndef LLC_PRIORITY_H
#define LLC_PRIORITY_H


void rom_llc_priority_connection_role_on_event_added(stLlcSchedulerEvent_t* pstEvent);

void rom_llc_priority_connection_role_on_event_config_failed(stLlcSchedulerEvent_t* pstEvent);

void rom_llc_priority_connection_role_on_event_blocked(stLlcSchedulerEvent_t* pstEvent, uint32_t u32BlockedCount);

void rom_llc_priority_connection_role_on_event_stopped(stLlcSchedulerEvent_t* pstEvent);


#endif
