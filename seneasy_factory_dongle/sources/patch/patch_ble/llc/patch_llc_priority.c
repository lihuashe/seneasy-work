#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "llc_llp_interface.h"

#include "llc.h"

#include "llc_hook.h"

#include "llc_scheduler.h"

#include "llc_tool.h"

#include "llc_schedule_algorithm.h"

#include "llc_priority_ext_adv.h"
#include "llc_priority_ext_scan.h"
#include "llc_priority_ext_initiator.h"

#include "rom_callback.h"
#include "llc_priority.h"
#include "patch_llc_priority.h"

extern uint8_t  g_u8CompensationCoefficientUs;
extern uint32_t g_u32CompensationTimeUsDefault;
extern uint32_t g_u32CompensationTimeUsMin;
extern uint32_t g_u32CompensationTimeUsMax;

void patch_llc_hook_prefix_priority_on_event_added(bool *pbIsDone, void *pEvent)
{
    stLlcSchedulerEvent_t *pstEvent = (stLlcSchedulerEvent_t *)pEvent;

    if ((NULL == pstEvent) || (NULL == pstEvent->pstRoleInstance))
    {
        // unexpected situation
        system_error(SYS_ERR_MODULE_LLC_PRIORITY, __LINE__ + 1000, (uint32_t)pstEvent, 0);
        return;
    }

    switch (pstEvent->enumRole)
    {
        case LLC_SCHEDULER_ROLE_EXT_ADV:
        {
            pstEvent->u32CompensationTimeUs = g_u32CompensationTimeUsDefault;
            rom_llc_priority_ext_adv_on_event_added(pstEvent);
            *pbIsDone = true;
        }
        break;

        case LLC_SCHEDULER_ROLE_EXT_SCAN:
        {
            pstEvent->u32CompensationTimeUs = g_u32CompensationTimeUsDefault;
            rom_llc_priority_ext_scan_on_event_added(pstEvent);
            *pbIsDone = true;
        }
        break;

        case LLC_SCHEDULER_ROLE_EXT_INIT:
        {
            pstEvent->u32CompensationTimeUs = g_u32CompensationTimeUsDefault;
            rom_llc_priority_ext_initiator_on_event_added(pstEvent);
            *pbIsDone = true;
        }
        break;

        default:
            *pbIsDone = false;
            break;
    }
}

void patch_llc_hook_prefix_priority_on_event_config_failed(bool *pbIsDone, void *pEvent)
{
    stLlcSchedulerEvent_t *pstEvent = (stLlcSchedulerEvent_t *)pEvent;

    if ((NULL == pstEvent) || (NULL == pstEvent->pstRoleInstance))
    {
        // unexpected situation
        system_error(SYS_ERR_MODULE_LLC_PRIORITY, __LINE__ + 1000, (uint32_t)pstEvent, 0);
        return;
    }

    switch (pstEvent->enumRole)
    {
        case LLC_SCHEDULER_ROLE_EXT_ADV:
        {
            // Increase Time Point Err Failed Event Compensation
            pstEvent->u32CompensationTimeUs *= g_u8CompensationCoefficientUs;
            if (pstEvent->u32CompensationTimeUs > g_u32CompensationTimeUsMax)
            {
                pstEvent->u32CompensationTimeUs = g_u32CompensationTimeUsMax;
            }
            rom_llc_priority_ext_adv_on_event_config_failed(pstEvent);
            *pbIsDone = true;
        }
        break;

        case LLC_SCHEDULER_ROLE_EXT_SCAN:
        {
            // Increase Time Point Err Failed Event Compensation
            pstEvent->u32CompensationTimeUs *= g_u8CompensationCoefficientUs;
            if (pstEvent->u32CompensationTimeUs > g_u32CompensationTimeUsMax)
            {
                pstEvent->u32CompensationTimeUs = g_u32CompensationTimeUsMax;
            }
            rom_llc_priority_ext_scan_on_event_config_failed(pstEvent);
            *pbIsDone = true;
        }
        break;

        case LLC_SCHEDULER_ROLE_EXT_INIT:
        {
            // Increase Time Point Err Failed Event Compensation
            pstEvent->u32CompensationTimeUs *= g_u8CompensationCoefficientUs;
            if (pstEvent->u32CompensationTimeUs > g_u32CompensationTimeUsMax)
            {
                pstEvent->u32CompensationTimeUs = g_u32CompensationTimeUsMax;
            }
            rom_llc_priority_ext_initiator_on_event_config_failed(pstEvent);
            *pbIsDone = true;
        }
        break;

        default:
            *pbIsDone = false;
            break;
    }
}

void patch_llc_hook_prefix_priority_on_event_blocked(bool *pbIsDone, void *pEvent, uint32_t u32BlockedCount)
{
    stLlcSchedulerEvent_t *pstEvent = (stLlcSchedulerEvent_t *)pEvent;

    if ((NULL == pstEvent) || (NULL == pstEvent->pstRoleInstance))
    {
        // unexpected situation
        system_error(SYS_ERR_MODULE_LLC_PRIORITY, __LINE__ + 1000, (uint32_t)pstEvent, 0);
        return;
    }

    switch (pstEvent->enumRole)
    {
        case LLC_SCHEDULER_ROLE_EXT_ADV:
        {
            rom_llc_priority_ext_adv_on_event_blocked(pstEvent, u32BlockedCount);
            *pbIsDone = true;
        }
        break;

        case LLC_SCHEDULER_ROLE_EXT_SCAN:
        {
            rom_llc_priority_ext_scan_on_event_blocked(pstEvent, u32BlockedCount);
            *pbIsDone = true;
        }
        break;

        case LLC_SCHEDULER_ROLE_EXT_INIT:
        {
            rom_llc_priority_ext_initiator_on_event_blocked(pstEvent, u32BlockedCount);
            *pbIsDone = true;
        }
        break;

        default:
            *pbIsDone = false;
            break;
    }
}

void patch_llc_hook_prefix_priority_on_event_stopped(bool *pbIsDone, void *pEvent)
{
    stLlcSchedulerEvent_t *pstEvent = (stLlcSchedulerEvent_t *)pEvent;

    if ((NULL == pstEvent) || (NULL == pstEvent->pstRoleInstance))
    {
        // unexpected situation
        system_error(SYS_ERR_MODULE_LLC_PRIORITY, __LINE__ + 1000, (uint32_t)pstEvent, 0);
        return;
    }

    switch (pstEvent->enumRole)
    {
        case LLC_SCHEDULER_ROLE_EXT_ADV:
        {
            // Decrease Successed Event Compensation
            pstEvent->u32CompensationTimeUs /= g_u8CompensationCoefficientUs;
            if (pstEvent->u32CompensationTimeUs < g_u32CompensationTimeUsMin)
            {
                pstEvent->u32CompensationTimeUs = g_u32CompensationTimeUsMin;
            }

            rom_llc_priority_ext_adv_on_event_stopped(pstEvent);
            *pbIsDone = true;
        }
        break;

        case LLC_SCHEDULER_ROLE_EXT_SCAN:
        {
            // Decrease Successed Event Compensation
            pstEvent->u32CompensationTimeUs /= g_u8CompensationCoefficientUs;
            if (pstEvent->u32CompensationTimeUs < g_u32CompensationTimeUsMin)
            {
                pstEvent->u32CompensationTimeUs = g_u32CompensationTimeUsMin;
            }

            rom_llc_priority_ext_scan_on_event_stopped(pstEvent);
            *pbIsDone = true;
        }
        break;

        case LLC_SCHEDULER_ROLE_EXT_INIT:
        {
            // Decrease Successed Event Compensation
            pstEvent->u32CompensationTimeUs /= g_u8CompensationCoefficientUs;
            if (pstEvent->u32CompensationTimeUs < g_u32CompensationTimeUsMin)
            {
                pstEvent->u32CompensationTimeUs = g_u32CompensationTimeUsMin;
            }

            rom_llc_priority_ext_initiator_on_event_stopped(pstEvent);
            *pbIsDone = true;
        }
        break;

        default:
            *pbIsDone = false;
            break;
    }
}

uint8_t rom_llc_priority_get_add_level(uint8_t u8CurPriority, uint32_t u32AddPriority, uint8_t u8LimitPriority)
{
    if (u8CurPriority + u32AddPriority > u8LimitPriority)
    {
        return u8LimitPriority;
    }
    else
    {
        return u8CurPriority + u32AddPriority;
    }
}

uint8_t rom_llc_priority_get_sub_level(uint8_t u8CurPriority, uint32_t u32SubPriority, uint8_t u8LimitPriority)
{
    if (u8CurPriority > u32SubPriority + u8LimitPriority)
    {
        return u8CurPriority - u32SubPriority;
    }
    else
    {
        return u8LimitPriority;
    }
}

void patch_llc_priority(void)
{
    g_llcHookPrefixTable[LLC_HOOK_PREFIX_priority_on_event_added] = patch_llc_hook_prefix_priority_on_event_added;
    g_llcHookPrefixTable[LLC_HOOK_PREFIX_priority_on_event_config_failed] =
        patch_llc_hook_prefix_priority_on_event_config_failed;
    g_llcHookPrefixTable[LLC_HOOK_PREFIX_priority_on_event_blocked] = patch_llc_hook_prefix_priority_on_event_blocked;
    g_llcHookPrefixTable[LLC_HOOK_PREFIX_priority_on_event_stopped] = patch_llc_hook_prefix_priority_on_event_stopped;
}
