#ifndef LLP_SLEEP_H
#define LLP_SLEEP_H

void rom_llp_sleep_init(void);

void rom_llp_sleep_set_time(uint16_t u16SleepThresholdTimeUs, uint16_t u16WakeupTimeUs);

void rom_llp_sleep_wakeup(void);

void rom_llp_sleep_on_fragment_config_succeed(uint64_t u64FragmentStartTime);

void rom_llp_sleep_on_wakeup_interrupt(void);

void rom_llp_sleep_on_system_wakeup_by_llc(void);

bool rom_llp_sleep_is_system_sleep_allowed(void);


#endif
