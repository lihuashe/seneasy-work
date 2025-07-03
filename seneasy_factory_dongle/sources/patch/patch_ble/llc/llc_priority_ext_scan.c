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
#include "patch_llc_priority.h"
#include "llc_priority_ext_scan.h"

#include "llc_ext_scan.h"


/**
对于secondary scanning， 优先级固定25
对于primary scanning，优先级范围100~26，调度事件运行后，优先级下降50，调度事件被block或配置失败，优先级上升1
已有的优先级框架接口函数无法满足需求，故添加一个接口函数由对应扩展扫描代码直接调用
**/

void rom_llc_priority_ext_scan_on_event_added(stLlcSchedulerEvent_t* pstEvent)
{
    stLlcExtScan_t* pstScan = pstEvent->pstRoleInstance;
    pstScan->u8PriorityForPrimaryScannning = 100;
    pstScan->u8PriorityForSecondaryScannning = 25;
    pstEvent->u8Priority = pstScan->u8PriorityForPrimaryScannning;
}

void rom_llc_priority_ext_scan_on_event_config_failed(stLlcSchedulerEvent_t* pstEvent)
{
    stLlcExtScan_t* pstScan = pstEvent->pstRoleInstance;
    if (pstScan->bIsScanningPhyPrimary)
    {
        pstScan->u8PriorityForPrimaryScannning = rom_llc_priority_get_sub_level(pstScan->u8PriorityForPrimaryScannning,1,26);
        pstEvent->u8Priority = pstScan->u8PriorityForPrimaryScannning;
    }
}

void rom_llc_priority_ext_scan_on_event_blocked(stLlcSchedulerEvent_t* pstEvent, uint32_t u32BlockedCount)
{
    stLlcExtScan_t* pstScan = pstEvent->pstRoleInstance;
    if (pstScan->bIsPrimaryScanningBlocked)
    {
        if (u32BlockedCount > 255)
        {
            pstScan->u8PriorityForPrimaryScannning = rom_llc_priority_get_sub_level(pstScan->u8PriorityForPrimaryScannning,255,26);
        }
        else
        {
            pstScan->u8PriorityForPrimaryScannning = rom_llc_priority_get_sub_level(pstScan->u8PriorityForPrimaryScannning,u32BlockedCount&0xFF,26);
        }
    }
    if (pstScan->bIsScanningPhyPrimary)
    {
        pstEvent->u8Priority = pstScan->u8PriorityForPrimaryScannning;
    }
    else
    {
        pstEvent->u8Priority = pstScan->u8PriorityForSecondaryScannning;
    }
}

void rom_llc_priority_ext_scan_on_event_stopped(stLlcSchedulerEvent_t* pstEvent)
{
    stLlcExtScan_t* pstScan = pstEvent->pstRoleInstance;
    if (pstScan->bIsScanningPhyPrimary)
    {
        pstScan->u8PriorityForPrimaryScannning = rom_llc_priority_get_add_level(pstScan->u8PriorityForPrimaryScannning,50,100);
        pstEvent->u8Priority = pstScan->u8PriorityForPrimaryScannning;
    }
}

void rom_llc_priority_ext_scan_on_scheduled(stLlcSchedulerEvent_t* pstEvent)
{
    stLlcExtScan_t* pstScan = pstEvent->pstRoleInstance;
    if (pstScan->bIsScanningPhyPrimary)
    {
        pstEvent->u8Priority = pstScan->u8PriorityForPrimaryScannning;
    }
    else
    {
        pstEvent->u8Priority = pstScan->u8PriorityForSecondaryScannning;
    }
}
