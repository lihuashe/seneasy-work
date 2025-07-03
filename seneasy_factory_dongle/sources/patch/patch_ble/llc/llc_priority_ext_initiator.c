#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#include "llc_spec_5_1.h"

#include "llc_llp_interface.h"

#include "llc.h"
#include "llc_adv_channel_pdu.h"
#include "llc_scheduler.h"

#include "llc_priority.h"
#include "llc_priority_ext_initiator.h"


/**
优先级固定23
**/

void rom_llc_priority_ext_initiator_on_event_added(stLlcSchedulerEvent_t* pstEvent)
{
    pstEvent->u8Priority = 23;
}

void rom_llc_priority_ext_initiator_on_event_config_failed(stLlcSchedulerEvent_t* pstEvent)
{
    //do noting
}

void rom_llc_priority_ext_initiator_on_event_blocked(stLlcSchedulerEvent_t* pstEvent, uint32_t u32BlockedCount)
{
    //do noting
}

void rom_llc_priority_ext_initiator_on_event_stopped(stLlcSchedulerEvent_t* pstEvent)
{
    //do noting
}
