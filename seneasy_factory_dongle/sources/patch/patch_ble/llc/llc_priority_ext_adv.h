#ifndef LLC_PRIORITY_EXT_ADV_H
#define LLC_PRIORITY_EXT_ADV_H


void rom_llc_priority_ext_adv_on_event_added(stLlcSchedulerEvent_t* pstEvent);

void rom_llc_priority_ext_adv_on_event_config_failed(stLlcSchedulerEvent_t* pstEvent);

void rom_llc_priority_ext_adv_on_event_blocked(stLlcSchedulerEvent_t* pstEvent, uint32_t u32BlockedCount);

void rom_llc_priority_ext_adv_on_event_stopped(stLlcSchedulerEvent_t* pstEvent);


void rom_llc_priority_ext_adv_on_adv_enabled(void* pstAdvInstance);
void rom_llc_priority_ext_adv_update_priority(void* pstAdvInstance, bool bPrimaryOrSecondary);
void rom_llc_priority_ext_adv_on_primary_scheduler_event_stopped(void* pstAdvInstance, bool bIsFailed);
void rom_llc_priority_ext_adv_on_secondary_advertising_event_stopped(void* pstAdvInstance, bool bIsFailed);

#endif
