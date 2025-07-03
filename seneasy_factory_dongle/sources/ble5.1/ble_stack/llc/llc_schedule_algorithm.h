#ifndef LLC_SCHEDULE_ALGORITHM_H
#define LLC_SCHEDULE_ALGORITHM_H

stLlcSchedulerEvent_t* rom_llc_schedule_algorithm_get_next_event (pstLlcSchedulerEvent_t* pstEventListArray, pstLlcSchedulerEvent_t* pstTempListArray, 
    uint32_t u32EventNum);

#endif
