#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#include "llc_llp_interface.h"



#include "llc_spec_5_1.h"

#include "llc.h"
#include "llc_scheduler.h"


#include "llc_data_channel_pdu.h"
#include "llc_adv_channel_pdu.h"
#include "llc_acl_data.h"
#include "llc_data.h"
#include "llc_ctrl_procedure.h"
#include "llc_connection_role.h"
#include "llc_slave.h"
#include "llc_ext_adv.h"
#include "llc_priority.h"
#include "patch_llc_priority.h"
#include "llc_priority_ext_adv.h"
#include "controller_interface.h"

/**
DIRECT_IND_HIGH_DUTY_CYCLE 类型广播优先级固定为22
其它传统广播优先级范围100~25，调度事件运行后，优先级下降50，调度事件被block或配置失败，优先级上升1

对于扩展广播，primary channel event和secondary channel event单独计算优先级
primary channel event优先级范围100~51，调度事件运行后，优先级下降50，调度事件被block或配置失败，优先级上升1

secondary channel优先级范围50~25，secondary channel 广播事件正常结束后，优先级下降25，异常结束或被block，优先级上升1

已有的优先级框架接口函数无法满足需求，故全部留空，有效的接口函数由对应扩展广播代码直接调用
**/

////以下为留空的优先级框架接口
void rom_llc_priority_ext_adv_on_event_added(stLlcSchedulerEvent_t* pstEvent)
{
    //do noting
}

void rom_llc_priority_ext_adv_on_event_config_failed(stLlcSchedulerEvent_t* pstEvent)
{
    //do noting
}

void rom_llc_priority_ext_adv_on_event_blocked(stLlcSchedulerEvent_t* pstEvent, uint32_t u32BlockedCount)
{
    //do noting
}

void rom_llc_priority_ext_adv_on_event_stopped(stLlcSchedulerEvent_t* pstEvent)
{
    //do noting
}



////以下为扩展广播直接调用的接口
void rom_llc_priority_ext_adv_on_adv_enabled(void* pstAdvInstance)
{
    stLlcExtAdv_t* pstAdv = pstAdvInstance;
    
    if (ADVERTISING_EVENT_PROPERTIES_ADV_DIRECT_IND_HIGH_DUTY_CYCLE == pstAdv->Advertising_Event_Properties)
    {
        pstAdv->u8PrimaryChannelPriority = 22;
    }
    else if (LLC_EXT_ADV_IS_LEGACY(pstAdv->Advertising_Event_Properties))
    {
        pstAdv->u8PrimaryChannelPriority = 100;
    }
    else
    {
        pstAdv->u8PrimaryChannelPriority = 100;
        pstAdv->u8SecondaryChannelPriority = 50;
    }
}

void rom_llc_priority_ext_adv_update_priority(void* pstAdvInstance, bool bPrimaryOrSecondary)
{
    stLlcExtAdv_t* pstAdv = pstAdvInstance;
    if (bPrimaryOrSecondary)
    {
        pstAdv->stSchedulerEvent.u8Priority = pstAdv->u8PrimaryChannelPriority;
    }
    else
    {
        pstAdv->stSchedulerEvent.u8Priority = pstAdv->u8SecondaryChannelPriority;
    }
}

void rom_llc_priority_ext_adv_on_primary_scheduler_event_stopped(void* pstAdvInstance, bool bIsFailed)
{
    stLlcExtAdv_t* pstAdv = pstAdvInstance;
    
    if (ADVERTISING_EVENT_PROPERTIES_ADV_DIRECT_IND_HIGH_DUTY_CYCLE == pstAdv->Advertising_Event_Properties)
    {
        //pstAdv->u8PrimaryChannelPriority = 22;
    }
    else if (LLC_EXT_ADV_IS_LEGACY(pstAdv->Advertising_Event_Properties))
    {
        if (bIsFailed)
        {
            pstAdv->u8PrimaryChannelPriority = rom_llc_priority_get_sub_level(pstAdv->u8PrimaryChannelPriority, 1, 25);
        }
        else
        {
            pstAdv->u8PrimaryChannelPriority = rom_llc_priority_get_add_level(pstAdv->u8PrimaryChannelPriority, 50, 100);
        }
    }
    else
    {
        if (bIsFailed)
        {
            pstAdv->u8PrimaryChannelPriority = rom_llc_priority_get_sub_level(pstAdv->u8PrimaryChannelPriority, 1, 51);
        }
        else
        {
            pstAdv->u8PrimaryChannelPriority = rom_llc_priority_get_add_level(pstAdv->u8PrimaryChannelPriority, 50, 100);
        }
    }
}

void rom_llc_priority_ext_adv_on_secondary_advertising_event_stopped(void* pstAdvInstance, bool bIsFailed)
{
    stLlcExtAdv_t* pstAdv = pstAdvInstance;
    
    if (LLC_EXT_ADV_IS_LEGACY(pstAdv->Advertising_Event_Properties))
    {
    }
    else
    {
        if (bIsFailed)
        {
            pstAdv->u8SecondaryChannelPriority = rom_llc_priority_get_sub_level(pstAdv->u8SecondaryChannelPriority, 1, 25);
        }
        else
        {
            pstAdv->u8SecondaryChannelPriority = rom_llc_priority_get_add_level(pstAdv->u8SecondaryChannelPriority, 25, 50);
        }
    }
    
}


