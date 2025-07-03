/*
MLOG_ID_RANGE: 0x3830~0x387F
MLOG_ID_USED:  0x3830~0x3857
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "RegMXD2670.h"



#include "llc_llp_interface.h"

#include "llc_spec_5_1.h"

#include "llc_adv_channel_pdu.h"
#include "llc_scheduler.h"

#include "llc.h"
#include "patch_llc.h"
#include "llc_address.h"
#include "llc_privacy.h"
#include "llc_tool.h"


#include "hci.h"

#include "extended_controller_interface.h"


#include "llc_common_extended_advertising_payload_format.h"
#include "llc_ext_scan.h"
#include "llc_ext_scan_state_machine.h"
#include "llc_ext_scan_secondary_scanning.h"
#include "llc_ext_scan_duplicate_filter.h"

#include "ble_impl.h"

#include "mlog.h"
#include "rom_callback.h"


void rom_llc_ext_scan_secondary_scanning_report_advertising_set(stLlcExtScan_t* pstScan, stLlcExtScanAdvertisingSet_t* pstSet);

bool rom_llc_ext_scan_secondary_scanning_is_advertising_set_collision_with_another(stLlcExtScanAdvertisingSet_t* pstSet1, stLlcExtScanAdvertisingSet_t* pstSet2)
{
    return !((pstSet1->u64StartTimeUs > (pstSet2->u64StartTimeUs + pstSet2->u32WorkTimeUs))
        ||(pstSet2->u64StartTimeUs > (pstSet1->u64StartTimeUs + pstSet1->u32WorkTimeUs)));
}


bool rom_llc_ext_scan_secondary_scanning_add_advertising_set_to_schedule_list(stLlcExtScan_t* pstScan, stLlcExtScanAdvertisingSet_t* pstSet)
{
    if (NULL == pstSet)
    {
        return false;
    }

    stLlcExtScanAdvertisingSet_t* pstPrevSet = NULL;
    stLlcExtScanAdvertisingSet_t* pstNextSet = NULL;
    
    uint32_t u32Primask = __get_PRIMASK();
    __disable_irq();
    stLlcExtScanAdvertisingSet_t* pstTmpSet = pstScan->pstSecondaryScanningSetScheduleListHead;
    while(NULL != pstTmpSet)
    {
        if (rom_llc_ext_scan_secondary_scanning_is_advertising_set_collision_with_another(pstSet, pstTmpSet))
        {
            __set_PRIMASK(u32Primask);
            return false;
        }
        
        if (pstSet->u64StartTimeUs < pstTmpSet->u64StartTimeUs)
        {
            pstNextSet = pstTmpSet;
            break;
        }
        else
        {
            pstPrevSet = pstTmpSet;
        }
        
        pstTmpSet = pstTmpSet->next;
    }
    if ((NULL == pstPrevSet) && (NULL == pstNextSet))
    {
        //list is empty
        pstSet->prev = NULL;
        pstSet->next = NULL;
        pstScan->pstSecondaryScanningSetScheduleListHead = pstSet;
        pstScan->pstSecondaryScanningSetScheduleListTail = pstSet;
    }
    else if (NULL == pstPrevSet)//and NULL != pstNextSet
    {
        //add to list head
        pstSet->prev = NULL;
        pstSet->next = pstScan->pstSecondaryScanningSetScheduleListHead;
        pstScan->pstSecondaryScanningSetScheduleListHead->prev = pstSet;
        pstScan->pstSecondaryScanningSetScheduleListHead = pstSet;
    }
    else if (NULL == pstNextSet)// and NULL != pstPrevSet
    {
        //add to list tail
        pstSet->prev = pstScan->pstSecondaryScanningSetScheduleListTail;
        pstSet->next = NULL;
        pstScan->pstSecondaryScanningSetScheduleListTail->next = pstSet;
        pstScan->pstSecondaryScanningSetScheduleListTail = pstSet;
    }
    else// NULL != pstPrevSet and NULL != pstNextSet
    {
        //add to list
        pstSet->prev = pstPrevSet;
        pstSet->next = pstNextSet;
        
        pstPrevSet->next = pstSet;
        pstNextSet->prev = pstSet;
    }
    __set_PRIMASK(u32Primask);
    return true;
}


void rom_llc_ext_scan_secondary_scanning_remove_advertising_set_from_schedule_list(stLlcExtScan_t* pstScan, stLlcExtScanAdvertisingSet_t* pstSet)
{
    if (NULL == pstSet)
    {
        return;
    }
    
    uint32_t u32Primask = __get_PRIMASK();
    __disable_irq();
    
    stLlcExtScanAdvertisingSet_t* pstPrevSet = pstSet->prev;
    stLlcExtScanAdvertisingSet_t* pstNextSet = pstSet->next;
    
    if ((NULL == pstPrevSet)&&(NULL == pstNextSet))
    {
        if (pstSet == pstScan->pstSecondaryScanningSetScheduleListHead)
        {
            //is Head, is Tail
            pstScan->pstSecondaryScanningSetScheduleListHead = NULL;
            pstScan->pstSecondaryScanningSetScheduleListTail = NULL;
        }
        else
        {
            //already removed
        }
    }
    else if (NULL == pstPrevSet)
    {
        //is Head, is not Tail
        pstNextSet->prev = NULL;
        pstScan->pstSecondaryScanningSetScheduleListHead = pstNextSet;
    }
    else if (NULL == pstNextSet)
    {
        //is not Head, is Tail
        pstPrevSet->next = NULL;
        pstScan->pstSecondaryScanningSetScheduleListTail = pstPrevSet;
    }
    else
    {
        //is not Head, is not Tail
        pstPrevSet->next = pstNextSet;
        pstNextSet->prev = pstPrevSet;
    }
    
    pstSet->prev = NULL;
    pstSet->next = NULL;
    
    __set_PRIMASK(u32Primask);
    
}


stLlcExtScanAdvertisingSet_t* rom_llc_ext_scan_secondary_scanning_alloc_advertising_set(stLlcExtScan_t* pstScan)
{
    stLlcExtScanAdvertisingSet_t* pstSet = rom_llc_ext_scan_alloc_advertising_set();
    
    if (NULL != pstSet)
    {
        pstSet->pstAdvPdu = NULL;
        pstSet->pstRspPdu = NULL;
        pstSet->pstChainPduListHead = NULL;
        pstSet->pstChainPduListTail = NULL;
        pstSet->prev = NULL;
        pstSet->next = NULL;
    }
    
    return pstSet;
}

void rom_llc_ext_scan_secondary_scanning_free_advertising_set(stLlcExtScan_t* pstScan, stLlcExtScanAdvertisingSet_t* pstSet)
{
    stLlcAdvChannelPdu_t* pstFreePdu;
    pstFreePdu = pstSet->pstAdvPdu;
    pstSet->pstAdvPdu = NULL;
    if (NULL != pstFreePdu)
    {
        rom_llc_ext_scan_free_pdu(pstScan, pstFreePdu);
    }
    
    pstFreePdu = pstSet->pstRspPdu;
    pstSet->pstRspPdu = NULL;
    if (NULL != pstFreePdu)
    {
        rom_llc_ext_scan_free_pdu(pstScan, pstFreePdu);
    }
    
    stLlcAdvChannelPdu_t* pstPdu = pstSet->pstChainPduListHead;
    while(NULL != pstPdu)
    {
        pstFreePdu = pstPdu;
        pstPdu = pstPdu->next;
        
        rom_llc_ext_scan_free_pdu(pstScan, pstFreePdu);
    }
    pstSet->pstChainPduListHead = NULL;
    pstSet->pstChainPduListTail = NULL;
    
    pstSet->prev = NULL;
    pstSet->next = NULL;
    
    rom_llc_ext_scan_free_advertising_set(pstSet);
}


void rom_llc_ext_scan_secondary_scanning_on_enable(stLlcExtScan_t* pstScan)
{
    pstScan->pstSecondaryScanningSetScheduleListHead = NULL;
    pstScan->pstSecondaryScanningSetScheduleListTail = NULL;
    
    pstScan->pstSecondaryScanningSetReportListHead = NULL;
    pstScan->pstSecondaryScanningSetReportListTail = NULL;
    pstScan->bSecondaryReporting = false;
}

void rom_llc_ext_scan_secondary_scanning_on_disable(stLlcExtScan_t* pstScan)
{
    stLlcExtScanAdvertisingSet_t* pstSet;
    
    pstSet = pstScan->pstSecondaryScanningSetScheduleListHead;
    while(NULL != pstSet)
    {
        stLlcExtScanAdvertisingSet_t* pstRemoveSet = pstSet;
        pstSet = pstSet->next;
        
        rom_llc_ext_scan_secondary_scanning_remove_advertising_set_from_schedule_list(pstScan, pstRemoveSet);
        rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstRemoveSet);
    }
    pstScan->pstCurrentSecondaryScanningSet = NULL;
    
    for(;;)
    {
        uint32_t u32Primask = __get_PRIMASK();
        __disable_irq();
        pstSet = pstScan->pstSecondaryScanningSetReportListHead;
        if (NULL == pstSet)
        {
            __set_PRIMASK(u32Primask);
            break;
        }
        pstScan->pstSecondaryScanningSetReportListHead = pstSet->next;
        if (NULL == pstScan->pstSecondaryScanningSetReportListHead)
        {
            pstScan->pstSecondaryScanningSetReportListTail = NULL;
        }
        else
        {
            pstScan->pstSecondaryScanningSetReportListHead->prev = NULL;
        }
        pstSet->prev = NULL;
        pstSet->next = NULL;
        __set_PRIMASK(u32Primask);
        rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
    }
}

void rom_llc_ext_scan_secondary_scanning_prepare_timing(stLlcExtScan_t* pstScan)
{
    //do not need to disable irq since it its running in interrupt when sets is not empty
    
    stLlcExtScanAdvertisingSet_t* pstSet = pstScan->pstSecondaryScanningSetScheduleListHead;
    pstScan->pstCurrentSecondaryScanningSet = NULL;
    uint64_t u64TimeUs = rom_llc_timer_get_count(true);
    while(NULL != pstSet)
    {
        if (pstSet->u64StartTimeUs > u64TimeUs)
        {
            pstScan->pstCurrentSecondaryScanningSet = pstSet;
            return;
        }
        else
        {
            //remove timeout set
            stLlcExtScanAdvertisingSet_t* pstRemoveSet = pstSet;
            pstSet = pstSet->next;
            
            rom_llc_ext_scan_secondary_scanning_remove_advertising_set_from_schedule_list(pstScan, pstRemoveSet);
            rom_llc_ext_scan_secondary_scanning_report_advertising_set(pstScan, pstRemoveSet);//report
        }
    }
}


bool rom_llc_ext_scan_secondary_scanning_get_timing(stLlcExtScan_t* pstScan, stLlcExtScanTiming_t* pstTiming)
{
    if (NULL == pstScan->pstCurrentSecondaryScanningSet)
    {
        return false;
    }
    
    pstTiming->u64StartTimeUs = pstScan->pstCurrentSecondaryScanningSet->u64StartTimeUs;
    pstTiming->u32WorkTimeUs = pstScan->pstCurrentSecondaryScanningSet->u32WorkTimeUs;
    pstTiming->u32MinWorkTimeUs = pstTiming->u32WorkTimeUs;
    return true;
}




void rom_llc_ext_scan_secondary_scanning_on_received_extended_pdu_when_primary_scanning(stLlcExtScan_t* pstScan, stLlcAdvChannelPdu_t* pstPdu)
{
    stLlcExtScanPduExtraInfo_t* pstExtraInfo = rom_llc_ext_scan_get_pdu_extra_info(pstPdu);
    if (NULL == pstExtraInfo)
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_SECONDARY_SCANNING, __LINE__, (uint32_t)pstScan, (uint32_t)pstPdu);
        return;
    }
    
    if (rom_llc_ext_scan_is_pdu_empty(pstScan, pstPdu))
    {
        return;
    }
    
    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat;
    if (!rom_llc_common_extended_advertising_payload_parse(pstPdu, &stFormat))
    {
        return;
    }
    
    if (LLC_SPEC_5_1_ADV_CH_PDU_TYPE_ADV_EXT_IND != stFormat.Header.bitField.PDUType)
    {
        return;
    }
    
    if (stFormat.Connectable && stFormat.Scannable)
    {
        //connectable and scannable
        
        //invalid
        return;
    }
    else if (stFormat.Connectable || stFormat.Scannable)
    {
        //connectable or scannable
        
        //check
        if (stFormat.AdvA_Present || stFormat.TargetA_Present || stFormat.CTEInfo_Present 
            || (!stFormat.ADI_Present) || (!stFormat.AuxPtr_Present) || stFormat.SyncInfo_Present
            || stFormat.ACAD_Present || stFormat.AdvData_Present)
        {
            return;
        }
        if ((stFormat.TxPower_Present) && (LL_PHY_1M != pstExtraInfo->enumPhy))
        {
            return;
        }
        
        if (0 == stFormat.AuxPtr_Offset30Us)
        {
            return;
        }
        
        stLlcExtScanAdvertisingSet_t *pstSet = rom_llc_ext_scan_secondary_scanning_alloc_advertising_set(pstScan);
        if (NULL == pstSet)
        {
            return;
        }
        
        //set init
        uint64_t u64IdealStartTime = pstExtraInfo->u64PduStartTimeUs + (stFormat.AuxPtr_Offset30Us * 30);
        uint32_t u32OffsetTimeUs = (stFormat.AuxPtr_Offset30Us + stFormat.AuxPtr_OffsetUnits30Us) * 30;
        uint32_t u32WindowWideningUs = (rom_llc_get_llp_clock_accuracy_ppm() + stFormat.AuxPtr_ClockAccuracyPpm) * (u32OffsetTimeUs) / 1000000 + 32;
        pstSet->enumSetState = LLC_EXT_SCAN_ADVERTISING_SET_STATE_AUX;
        pstSet->u64StartTimeUs = u64IdealStartTime - u32WindowWideningUs;
        pstSet->u32AnchorWindowUs = (stFormat.AuxPtr_OffsetUnits30Us * 30) + u32WindowWideningUs * 2;
        pstSet->u32WorkTimeUs = pstSet->u32AnchorWindowUs
            + rom_llc_tool_calculate_trx_time_us_by_pdu_size(2 + LLC_SPEC_5_1_ADV_CHANNEL_PDU_MAX_PAYLOAD_SIZE, stFormat.AuxPtr_Phy) + LLC_SPEC_5_1_T_IFS_US
            + rom_llc_tool_calculate_trx_time_us_by_pdu_size(2 + LLC_SPEC_5_1_AUX_SCAN_REQ_PAYLOAD_SIZE, stFormat.AuxPtr_Phy) + LLC_SPEC_5_1_T_IFS_US
            + rom_llc_tool_calculate_trx_time_us_by_pdu_size(2 + LLC_SPEC_5_1_ADV_CHANNEL_PDU_MAX_PAYLOAD_SIZE, stFormat.AuxPtr_Phy) + LLC_SPEC_5_1_T_IFS_US;
        pstSet->enumPrimaryPhy = pstExtraInfo->enumPhy;
        pstSet->enumSecondaryPhy = stFormat.AuxPtr_Phy;
        pstSet->u8ChannelIdx = stFormat.AuxPtr_ChannelIndex;
        pstSet->bScannable = stFormat.Scannable;
        pstSet->bConnectable = stFormat.Connectable;
        pstSet->bDirected = stFormat.TargetA_Present;
        pstSet->enumAddressType = LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_ANONYMOUS;
        pstSet->u16DID = stFormat.ADI_DID;
        pstSet->u8SID = stFormat.ADI_SID;
        if (stFormat.TxPower_Present)
        {
            pstSet->s8LastTxPower = stFormat.TxPower;
        }
        else
        {
            pstSet->s8LastTxPower = LLC_SPEC_5_1_HCI_TX_POWER_IS_NOT_AVAILABLE;
        }
        
        
        if (rom_llc_ext_scan_secondary_scanning_add_advertising_set_to_schedule_list(pstScan, pstSet))
        {
            //MLOG1616I(0x3830, "connectable or scannable advertising set START, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        }
        else
        {
            rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
        }
        return;
    }
    else
    {
        //non-connectable non-scannable
        
        //check
        if (stFormat.CTEInfo_Present || stFormat.SyncInfo_Present
            || stFormat.ACAD_Present || stFormat.AdvData_Present)
        {
            return;
        }
        if ((!stFormat.ADI_Present) && (!stFormat.AuxPtr_Present))
        {
            //Non-Connectable and Non-Scannable Undirected without auxiliary packet
            //or
            //Non-Connectable and Non-Scannable Directed without auxiliary packet
            
            if (!stFormat.AdvA_Present)
            {
                return;
            }
            
            stLlcExtScanAdvertisingSet_t *pstSet = rom_llc_ext_scan_secondary_scanning_alloc_advertising_set(pstScan);
            if (NULL == pstSet)
            {
                return;
            }
            
            //set init
            pstSet->enumSetState = LLC_EXT_SCAN_ADVERTISING_SET_STATE_IDLE;
            pstSet->u64StartTimeUs = 0;//0 indicating that this set is without auxiliary packet
            pstSet->u32AnchorWindowUs = 0;
            pstSet->u32WorkTimeUs = 0;
            pstSet->enumPrimaryPhy = pstExtraInfo->enumPhy;
            pstSet->enumSecondaryPhy = pstSet->enumPrimaryPhy;
            pstSet->u8ChannelIdx = 0;
            pstSet->bScannable = stFormat.Scannable;
            pstSet->bConnectable = stFormat.Connectable;
            pstSet->bDirected = stFormat.TargetA_Present;
            pstSet->enumAddressType = LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_ANONYMOUS;
            if (rom_llc_ext_scan_check_RPA_and_white_list_disallowed(pstScan,
                stFormat.Header.bitField.TxAdd, stFormat.AdvA_Present?stFormat.AdvA:NULL,
                stFormat.Header.bitField.RxAdd, stFormat.TargetA_Present?stFormat.TargetA:NULL,
                &pstSet->enumAddressType, pstSet->pu8Address,
                &pstSet->enumDirectAddressType, pstSet->pu8DirectAddress))
            {
                rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
                return;
            }
            pstSet->u16DID = 0;
            pstSet->u8SID = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_SID_NO_ADI_PROVIDED;
            if (stFormat.TxPower_Present)
            {
                pstSet->s8LastTxPower = stFormat.TxPower;
            }
            else
            {
                pstSet->s8LastTxPower = LLC_SPEC_5_1_HCI_TX_POWER_IS_NOT_AVAILABLE;
            }
            pstSet->u32RssiRelatedValueForSetWithoutAuxiliaryPacket = pstPdu->u32RssiRelatedValue;
            
            
            rom_llc_ext_scan_secondary_scanning_report_advertising_set(pstScan, pstSet);
            return;
        }
        else if (stFormat.ADI_Present && stFormat.AuxPtr_Present)
        {
            //Non-Connectable and Non-Scannable Undirected with auxiliary packet
            //or
            //Non-Connectable and Non-Scannable Directed with auxiliary packet
            
            if ((LL_PHY_1M != pstExtraInfo->enumPhy) && ((stFormat.AdvA_Present)||(stFormat.TargetA_Present)||(stFormat.TxPower_Present)))
            {
                return;
            }
            
            if (0 == stFormat.AuxPtr_Offset30Us)
            {
                return;
            }
            
            stLlcExtScanAdvertisingSet_t *pstSet = rom_llc_ext_scan_secondary_scanning_alloc_advertising_set(pstScan);
            if (NULL == pstSet)
            {
                return;
            }
            
            
            //set init
            uint64_t u64IdealStartTime = pstExtraInfo->u64PduStartTimeUs + (stFormat.AuxPtr_Offset30Us * 30);
            uint32_t u32OffsetTimeUs = (stFormat.AuxPtr_Offset30Us + stFormat.AuxPtr_OffsetUnits30Us) * 30;
            uint32_t u32WindowWideningUs = (rom_llc_get_llp_clock_accuracy_ppm() + stFormat.AuxPtr_ClockAccuracyPpm) * (u32OffsetTimeUs) / 1000000 + 32;
            pstSet->enumSetState = LLC_EXT_SCAN_ADVERTISING_SET_STATE_AUX;
            pstSet->u64StartTimeUs = u64IdealStartTime - u32WindowWideningUs;
            pstSet->u32AnchorWindowUs = (stFormat.AuxPtr_OffsetUnits30Us * 30) + u32WindowWideningUs * 2;
            pstSet->u32WorkTimeUs = pstSet->u32AnchorWindowUs + rom_llc_tool_calculate_trx_time_us_by_pdu_size(2 + LLC_SPEC_5_1_ADV_CHANNEL_PDU_MAX_PAYLOAD_SIZE, stFormat.AuxPtr_Phy);
            pstSet->enumPrimaryPhy = pstExtraInfo->enumPhy;
            pstSet->enumSecondaryPhy = stFormat.AuxPtr_Phy;
            pstSet->u8ChannelIdx = stFormat.AuxPtr_ChannelIndex;
            pstSet->bScannable = stFormat.Scannable;
            pstSet->bConnectable = stFormat.Connectable;
            pstSet->bDirected = stFormat.TargetA_Present;
            pstSet->enumAddressType = LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_ANONYMOUS;
            if (rom_llc_ext_scan_check_RPA_and_white_list_disallowed(pstScan,
                stFormat.Header.bitField.TxAdd, stFormat.AdvA_Present?stFormat.AdvA:NULL,
                stFormat.Header.bitField.RxAdd, stFormat.TargetA_Present?stFormat.TargetA:NULL,
                &pstSet->enumAddressType, pstSet->pu8Address,
                &pstSet->enumDirectAddressType, pstSet->pu8DirectAddress))
            {
                rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
                return;
            }
            pstSet->u16DID = stFormat.ADI_DID;
            pstSet->u8SID = stFormat.ADI_SID;
            if (stFormat.TxPower_Present)
            {
                pstSet->s8LastTxPower = stFormat.TxPower;
            }
            else
            {
                pstSet->s8LastTxPower = LLC_SPEC_5_1_HCI_TX_POWER_IS_NOT_AVAILABLE;
            }
            
            if (rom_llc_ext_scan_secondary_scanning_add_advertising_set_to_schedule_list(pstScan, pstSet))
            {
                //MLOG1616I(0x3831, "nonconnectable nonscannable advertising set START, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
            }
            else
            {
                rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
            }
            return;
            
        }
        else
        {
            return;
        }
    }
}

void rom_llc_ext_scan_secondary_scanning_on_scheduled(stLlcExtScan_t* pstScan)
{
    pstScan->pstRecevingPdu = rom_llc_ext_scan_alloc_pdu(pstScan);
    
    pstScan->stRxPduPtr.pu16PayloadBufSize = &(pstScan->pstRecevingPdu->u16PayloadBufSize);
    pstScan->stRxPduPtr.pu8Payload = pstScan->pstRecevingPdu->pu8Payload;
    pstScan->stRxPduPtr.pu32Header = &(pstScan->pstRecevingPdu->u32Header);
}

uint32_t rom_llc_ext_scan_secondary_scanning_on_event_blocked (stLlcExtScan_t* pstScan)
{
    if (NULL != pstScan->pstRecevingPdu)
    {
        rom_llc_ext_scan_free_pdu(pstScan, pstScan->pstRecevingPdu);
        pstScan->pstRecevingPdu = NULL;
    }
    
    uint32_t u32BlockedCount = 1;
    
    stLlcExtScanAdvertisingSet_t* pstSet = pstScan->pstSecondaryScanningSetScheduleListHead;
    uint64_t u64TimeUs = rom_llc_timer_get_count(true);
    while(NULL != pstSet)
    {
        if (pstSet->u64StartTimeUs > u64TimeUs)
        {
            break;
        }
        else
        {
            //remove timeout set
            
            stLlcExtScanAdvertisingSet_t* pstRemoveSet = pstSet;
            pstSet = pstSet->next;
            
            rom_llc_ext_scan_secondary_scanning_remove_advertising_set_from_schedule_list(pstScan, pstRemoveSet);
            rom_llc_ext_scan_secondary_scanning_report_advertising_set(pstScan, pstRemoveSet);//report
            
            u32BlockedCount++;
        }
    }
    
    pstScan->pstCurrentSecondaryScanningSet = NULL;
    return u32BlockedCount;
}

EN_LLP_ERR_T rom_llc_ext_scan_secondary_scanning_config_llp(stLlcExtScan_t* pstScan, uint32_t u32MaxWorkWindowTimeUs)
{
    stLlcExtScanAdvertisingSet_t* pstCurrentset = pstScan->pstCurrentSecondaryScanningSet;
    if (NULL == pstCurrentset)
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_SECONDARY_SCANNING, __LINE__, (uint32_t)pstScan, (uint32_t)0);
        return LLP_ERR_SYSTEM;
    }
    
    stLlpExtScanFragmentParam_t stParam;
    stParam.u64StartTimeUs = pstCurrentset->u64StartTimeUs;
    stParam.u32WorkTimeUs = LLC_TOOL_MIN(pstCurrentset->u32WorkTimeUs, u32MaxWorkWindowTimeUs);
    stParam.u32AnchorWindowUs = pstCurrentset->u32AnchorWindowUs;
    stParam.u16IfsUs = LLC_SPEC_5_1_T_IFS_US;
    stParam.u16IfsRxWindowUs = rom_ble_impl_get_ifs_rx_window_us(pstCurrentset->enumSecondaryPhy);
    stParam.u8ChannelIdx = pstCurrentset->u8ChannelIdx;
    stParam.bIsSecondaryChannel = true;
    stParam.enumPhy = pstCurrentset->enumSecondaryPhy;
    stParam.u32AccessAddress = LLC_SPEC_5_1_ADV_CHANNEL_ACCESS_ADDRESS;
    stParam.u32CrcInit = LLC_SPEC_5_1_ADV_CHANNEL_CRC_INIT;
    stParam.u16RxPayloadSizeLimit = LLC_SPEC_5_1_ADV_CHANNEL_PDU_MAX_PAYLOAD_SIZE;
    stParam.pstLlpTxPduPtr = &(pstScan->stTxPduPtr);
    stParam.pstLlpRxPduPtr = &(pstScan->stRxPduPtr);
    
    EN_LLP_ERR_T err = rom_llc_start_ext_scan_fragment(&stParam);
    
    MLOG1688I(0x3832, "EXT SCAN SECONDARY SCANNING config ch:%u phy:%u priority:%u", stParam.u8ChannelIdx, stParam.enumPhy, pstScan->stSchedulerEvent.u8Priority);
    return err;
}

void rom_llc_ext_scan_secondary_scanning_on_llp_fragment_start(stLlcExtScan_t* pstScan)
{
    pstScan->enumState = LLC_EXT_SCAN_STATE_RX_ADV;
    pstScan->bSecondaryFragmentTruncated = true;
}


void rom_llc_ext_scan_secondary_scanning_report_process_connectable(stLlcExtScan_t* pstScan, stLlcExtScanAdvertisingSet_t* pstSet)
{
    if (NULL == pstSet->pstAdvPdu)
    {
        return;
    }
    
    //process set with Num_Reports==1
    /*        
    Subevent_Code,                          Size: 1 octet
    Num_Reports,                            Size: 1 octet
    
    Event_Type,                             Size: 2 octets
    Address_Type,                           Size: 1 octet
    Address,                                Size: 6 octets
    Primary_PHY,                            Size: 1 octet
    Secondary_PHY,                          Size: 1 octet
    Advertising_SID,                        Size: 1 octet
    Tx_Power,                               Size: 1 octet
    RSSI,                                   Size: 1 octet
    Periodic_Advertising_Interval,          Size: 2 octets
    Direct_Address_Type,                    Size: 1 octet
    Direct_Address,                         Size: 6 octets
    Data_Length,                            Size: 1 octet
    Data                                    Size: (Data_Length) octets
    */
    
    #define REPORT_PARAMETERS_SIZE_WITHOUT_DATA (1+1+2+1+6+1+1+1+1+1+2+1+6+1)

    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat = {0};
    
    uint8_t pu8Parameters[256];
    uint8_t u8ReportsNum = 0;
    uint16_t u16ParametersMaxLength = LLC_TOOL_MIN(sizeof(pu8Parameters), rom_hci_get_event_parameter_max_length());
    if (u16ParametersMaxLength < (REPORT_PARAMETERS_SIZE_WITHOUT_DATA))
    {
        //unable to send report
        return;
    }
    uint8_t u8DataMaxLength = LLC_TOOL_MIN(u16ParametersMaxLength - (REPORT_PARAMETERS_SIZE_WITHOUT_DATA), 229);//229 : core spec
    
    if (!rom_llc_common_extended_advertising_payload_parse(pstSet->pstAdvPdu, &stFormat))
    {
        return;
    }
    if (stFormat.AdvData_Present && (0 == u8DataMaxLength))
    {
        //unable to send report with data
        return;
    }
    
    if (LLC_SPEC_5_1_HCI_FILTER_DUPLICATES_DISABLED != pstScan->Filter_Duplicates)
    {
        if (rom_llc_ext_scan_duplicate_filter_is_report_duplicated(pstSet->enumAddressType,
            pstSet->pu8Address,
            pstSet->u8SID))
        {
            return;
        }
        else
        {
            rom_llc_ext_scan_duplicate_filter_add_report(pstSet->enumAddressType,
            pstSet->pu8Address,
            pstSet->u8SID);
        }
    }
    
    //Subevent_Code
    pu8Parameters[0] = LLC_SPEC_5_1_HCI_SUBEVENT_CODE_HCI_LE_Extended_Advertising_Report;
    
    //Num_Reports
    pu8Parameters[1] = 1;
    
    //Event_Type
    stLlcSpec51HciExtendedAdvertisingReportEventType_t Event_Type = {0};
    Event_Type.bitField.Connectable = pstSet->bConnectable;
    Event_Type.bitField.Scannable = pstSet->bScannable;
    Event_Type.bitField.Directed = pstSet->bDirected;
    Event_Type.bitField.ScanResponse = false;
    Event_Type.bitField.LegacyPDUsUsed = false;
    //Data_Status Set Later
    
    //Address_Type
    pu8Parameters[4] = pstSet->enumAddressType;
    
    //Address
    memcpy(&(pu8Parameters[5]), pstSet->pu8Address, 6);
    
    //Primary_PHY
    switch(pstSet->enumPrimaryPhy)
    {
        case LL_PHY_1M:
            pu8Parameters[11] = LLC_SPEC_5_1_HCI_PHY_LE_1M;
            break;
        
        case LL_PHY_2M:
            pu8Parameters[11] = LLC_SPEC_5_1_HCI_PHY_LE_2M;
            break;
        
        case LL_PHY_CODED_S2:
        case LL_PHY_CODED_S8:
            pu8Parameters[11] = LLC_SPEC_5_1_HCI_PHY_LE_CODED;
            break;
    }
    
    //Secondary_PHY
    switch(pstSet->enumSecondaryPhy)
    {
        case LL_PHY_1M:
            pu8Parameters[12] = LLC_SPEC_5_1_HCI_PHY_LE_1M;
            break;
        
        case LL_PHY_2M:
            pu8Parameters[12] = LLC_SPEC_5_1_HCI_PHY_LE_2M;
            break;
        
        case LL_PHY_CODED_S2:
        case LL_PHY_CODED_S8:
            pu8Parameters[12] = LLC_SPEC_5_1_HCI_PHY_LE_CODED;
            break;
    }
    
    //Advertising_SID
    pu8Parameters[13] = pstSet->u8SID;
    
    //Tx_Power
    pu8Parameters[14] = (uint8_t)pstSet->s8LastTxPower;
    
    //RSSI
    pu8Parameters[15] = (uint8_t)rom_llc_get_rssi(pstSet->pstAdvPdu->u32RssiRelatedValue);
    
    //Periodic_Advertising_Interval
    rom_llc_tool_write_little_endian_uint16(&(pu8Parameters[16]), 0);
    
    //Direct_Address_Type
    pu8Parameters[18] = pstSet->enumDirectAddressType;
    
    //Direct_Address
    memcpy(&(pu8Parameters[19]), pstSet->pu8DirectAddress, 6);
    
    uint16_t u16DataOffset = 0;
    do
    {
        //Data_Length
        uint8_t Data_Length = stFormat.AdvData_Length - u16DataOffset;
        if (Data_Length > u8DataMaxLength)
        {
            Data_Length = u8DataMaxLength;
        }
        pu8Parameters[25] = Data_Length;
        
        //Data
        if (Data_Length > 0)
        {
            memcpy(&(pu8Parameters[26]), &(stFormat.AdvData[u16DataOffset]), Data_Length);
        }
        
        u16DataOffset += Data_Length;
        
        //Event_Type Data_Status
        if (u16DataOffset < stFormat.AdvData_Length)
        {
            Event_Type.bitField.DataStatus = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_EVENT_TYPE_DATA_STATUS_INCOMPLETE_MORE_DATA;
        }
        else
        {
            Event_Type.bitField.DataStatus = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_EVENT_TYPE_DATA_STATUS_COMPLETE;
        }
        rom_llc_tool_write_little_endian_uint16(&(pu8Parameters[2]), Event_Type.u16Value);
        rom_controller_event_HCI_LE_Extended_Advertising_Report(REPORT_PARAMETERS_SIZE_WITHOUT_DATA + Data_Length, pu8Parameters);
    }
    while(u16DataOffset < stFormat.AdvData_Length);
}

void rom_llc_ext_scan_secondary_scanning_report_process_scannable(stLlcExtScan_t* pstScan, stLlcExtScanAdvertisingSet_t* pstSet)
{
    if (NULL == pstSet->pstAdvPdu)
    {
        return;
    }
    
    //process set with Num_Reports==1
    /*        
    Subevent_Code,                          Size: 1 octet
    Num_Reports,                            Size: 1 octet
    
    Event_Type,                             Size: 2 octets
    Address_Type,                           Size: 1 octet
    Address,                                Size: 6 octets
    Primary_PHY,                            Size: 1 octet
    Secondary_PHY,                          Size: 1 octet
    Advertising_SID,                        Size: 1 octet
    Tx_Power,                               Size: 1 octet
    RSSI,                                   Size: 1 octet
    Periodic_Advertising_Interval,          Size: 2 octets
    Direct_Address_Type,                    Size: 1 octet
    Direct_Address,                         Size: 6 octets
    Data_Length,                            Size: 1 octet
    Data                                    Size: (Data_Length) octets
    */
    
    #define REPORT_PARAMETERS_SIZE_WITHOUT_DATA (1+1+2+1+6+1+1+1+1+1+2+1+6+1)

    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat = {0};
    


    uint8_t pu8Parameters[256];
    uint8_t u8ReportsNum = 0;
    uint16_t u16ParametersMaxLength = LLC_TOOL_MIN(sizeof(pu8Parameters), rom_hci_get_event_parameter_max_length());
    if (u16ParametersMaxLength < (REPORT_PARAMETERS_SIZE_WITHOUT_DATA))
    {
        //unable to send report
        return;
    }
    uint8_t u8DataMaxLength = LLC_TOOL_MIN(u16ParametersMaxLength - (REPORT_PARAMETERS_SIZE_WITHOUT_DATA), 229);//229 : core spec
    
    if (NULL == pstSet->pstRspPdu)
    {
        if (LLC_SPEC_5_1_HCI_FILTER_DUPLICATES_DISABLED != pstScan->Filter_Duplicates)
        {
            if (rom_llc_ext_scan_duplicate_filter_is_report_duplicated(pstSet->enumAddressType,
                pstSet->pu8Address,
                pstSet->u8SID))
            {
                return;
            }
            else
            {
                rom_llc_ext_scan_duplicate_filter_add_report(pstSet->enumAddressType,
                pstSet->pu8Address,
                pstSet->u8SID);
            }
        }
        
        //Subevent_Code
        pu8Parameters[0] = LLC_SPEC_5_1_HCI_SUBEVENT_CODE_HCI_LE_Extended_Advertising_Report;
        
        //Num_Reports
        pu8Parameters[1] = 1;
        
        //Event_Type
        stLlcSpec51HciExtendedAdvertisingReportEventType_t Event_Type = {0};
        Event_Type.bitField.Connectable = pstSet->bConnectable;
        Event_Type.bitField.Scannable = pstSet->bScannable;
        Event_Type.bitField.Directed = pstSet->bDirected;
        Event_Type.bitField.ScanResponse = false;
        Event_Type.bitField.LegacyPDUsUsed = false;
        Event_Type.bitField.DataStatus = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_EVENT_TYPE_DATA_STATUS_INCOMPLETE_NO_MORE_DATA;
        rom_llc_tool_write_little_endian_uint16(&(pu8Parameters[2]), Event_Type.u16Value);
        
        //Address_Type
        pu8Parameters[4] = pstSet->enumAddressType;
        
        //Address
        memcpy(&(pu8Parameters[5]), pstSet->pu8Address, 6);
        
        //Primary_PHY
        switch(pstSet->enumPrimaryPhy)
        {
            case LL_PHY_1M:
                pu8Parameters[11] = LLC_SPEC_5_1_HCI_PHY_LE_1M;
                break;
            
            case LL_PHY_2M:
                pu8Parameters[11] = LLC_SPEC_5_1_HCI_PHY_LE_2M;
                break;
            
            case LL_PHY_CODED_S2:
            case LL_PHY_CODED_S8:
                pu8Parameters[11] = LLC_SPEC_5_1_HCI_PHY_LE_CODED;
                break;
        }
        
        //Secondary_PHY
        switch(pstSet->enumSecondaryPhy)
        {
            case LL_PHY_1M:
                pu8Parameters[12] = LLC_SPEC_5_1_HCI_PHY_LE_1M;
                break;
            
            case LL_PHY_2M:
                pu8Parameters[12] = LLC_SPEC_5_1_HCI_PHY_LE_2M;
                break;
            
            case LL_PHY_CODED_S2:
            case LL_PHY_CODED_S8:
                pu8Parameters[12] = LLC_SPEC_5_1_HCI_PHY_LE_CODED;
                break;
        }
        
        //Advertising_SID
        pu8Parameters[13] = pstSet->u8SID;
        
        //Tx_Power
        pu8Parameters[14] = (uint8_t)pstSet->s8LastTxPower;
        
        //RSSI
        pu8Parameters[15] = (uint8_t)rom_llc_get_rssi(pstSet->pstAdvPdu->u32RssiRelatedValue);
        
        //Periodic_Advertising_Interval
        rom_llc_tool_write_little_endian_uint16(&(pu8Parameters[16]), 0);
        
        //Direct_Address_Type
        pu8Parameters[18] = pstSet->enumDirectAddressType;
        
        //Direct_Address
        memcpy(&(pu8Parameters[19]), pstSet->pu8DirectAddress, 6);
        
        //Data_Length
        pu8Parameters[25] = 0;
        
        //Data
        //NULL
        rom_controller_event_HCI_LE_Extended_Advertising_Report(REPORT_PARAMETERS_SIZE_WITHOUT_DATA + 0, pu8Parameters);
        return;
    }
    
    
    if (!rom_llc_common_extended_advertising_payload_parse(pstSet->pstRspPdu, &stFormat))
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_SECONDARY_SCANNING, __LINE__, (uint32_t)pstSet, (uint32_t)pstSet->pstRspPdu);
        return;
    }
    if (stFormat.AdvData_Present && (0 == u8DataMaxLength))
    {
        //unable to send report with data
        return;
    }
    
    if (LLC_SPEC_5_1_HCI_FILTER_DUPLICATES_DISABLED != pstScan->Filter_Duplicates)
    {
        if (rom_llc_ext_scan_duplicate_filter_is_report_duplicated(pstSet->enumAddressType,
            pstSet->pu8Address,
            pstSet->u8SID))
        {
            return;
        }
        else
        {
            rom_llc_ext_scan_duplicate_filter_add_report(pstSet->enumAddressType,
            pstSet->pu8Address,
            pstSet->u8SID);
        }
    }
    
    //Subevent_Code
    pu8Parameters[0] = LLC_SPEC_5_1_HCI_SUBEVENT_CODE_HCI_LE_Extended_Advertising_Report;
    
    //Num_Reports
    pu8Parameters[1] = 1;
    
    //Event_Type
    stLlcSpec51HciExtendedAdvertisingReportEventType_t Event_Type = {0};
    Event_Type.bitField.Connectable = pstSet->bConnectable;
    Event_Type.bitField.Scannable = pstSet->bScannable;
    Event_Type.bitField.Directed = pstSet->bDirected;
    Event_Type.bitField.ScanResponse = true;
    Event_Type.bitField.LegacyPDUsUsed = false;
    //Data_Status Set Later
    
    //Address_Type
    pu8Parameters[4] = pstSet->enumAddressType;
    
    //Address
    memcpy(&(pu8Parameters[5]), pstSet->pu8Address, 6);
    
    //Primary_PHY
    switch(pstSet->enumPrimaryPhy)
    {
        case LL_PHY_1M:
            pu8Parameters[11] = LLC_SPEC_5_1_HCI_PHY_LE_1M;
            break;
        
        case LL_PHY_2M:
            pu8Parameters[11] = LLC_SPEC_5_1_HCI_PHY_LE_2M;
            break;
        
        case LL_PHY_CODED_S2:
        case LL_PHY_CODED_S8:
            pu8Parameters[11] = LLC_SPEC_5_1_HCI_PHY_LE_CODED;
            break;
    }
    
    //Secondary_PHY
    switch(pstSet->enumSecondaryPhy)
    {
        case LL_PHY_1M:
            pu8Parameters[12] = LLC_SPEC_5_1_HCI_PHY_LE_1M;
            break;
        
        case LL_PHY_2M:
            pu8Parameters[12] = LLC_SPEC_5_1_HCI_PHY_LE_2M;
            break;
        
        case LL_PHY_CODED_S2:
        case LL_PHY_CODED_S8:
            pu8Parameters[12] = LLC_SPEC_5_1_HCI_PHY_LE_CODED;
            break;
    }
    
    //Advertising_SID
    pu8Parameters[13] = pstSet->u8SID;
    
    //Tx_Power
    pu8Parameters[14] = (uint8_t)pstSet->s8LastTxPower;
    
    //RSSI
    pu8Parameters[15] = (uint8_t)rom_llc_get_rssi(pstSet->pstAdvPdu->u32RssiRelatedValue);
    
    //Periodic_Advertising_Interval
    rom_llc_tool_write_little_endian_uint16(&(pu8Parameters[16]), 0);
    
    //Direct_Address_Type
    pu8Parameters[18] = pstSet->enumDirectAddressType;
    
    //Direct_Address
    memcpy(&(pu8Parameters[19]), pstSet->pu8DirectAddress, 6);
    
    uint16_t u16DataOffset = 0;
    do
    {
        //Data_Length
        uint8_t Data_Length = stFormat.AdvData_Length - u16DataOffset;
        if (Data_Length > u8DataMaxLength)
        {
            Data_Length = u8DataMaxLength;
        }
        pu8Parameters[25] = Data_Length;
        
        //Data
        if (Data_Length > 0)
        {
            memcpy(&(pu8Parameters[26]), &(stFormat.AdvData[u16DataOffset]), Data_Length);
        }
        
        u16DataOffset += Data_Length;
        
        //Event_Type Data_Status
        if (u16DataOffset < stFormat.AdvData_Length)
        {
            Event_Type.bitField.DataStatus = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_EVENT_TYPE_DATA_STATUS_INCOMPLETE_MORE_DATA;
        }
        else
        {
            if (stFormat.AdvA_Present)
            {
                if (NULL == pstSet->pstChainPduListHead)
                {
                    Event_Type.bitField.DataStatus = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_EVENT_TYPE_DATA_STATUS_INCOMPLETE_NO_MORE_DATA;
                }
                else
                {
                    Event_Type.bitField.DataStatus = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_EVENT_TYPE_DATA_STATUS_INCOMPLETE_MORE_DATA;
                }
            }
            else
            {
                Event_Type.bitField.DataStatus = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_EVENT_TYPE_DATA_STATUS_COMPLETE;
            }
        }
        rom_llc_tool_write_little_endian_uint16(&(pu8Parameters[2]), Event_Type.u16Value);
        rom_controller_event_HCI_LE_Extended_Advertising_Report(REPORT_PARAMETERS_SIZE_WITHOUT_DATA + Data_Length, pu8Parameters);
    }
    while(u16DataOffset < stFormat.AdvData_Length);
    
    stLlcAdvChannelPdu_t* pstChainPdu = pstSet->pstChainPduListHead;
    while(NULL != pstChainPdu)
    {
        if (!rom_llc_common_extended_advertising_payload_parse(pstChainPdu, &stFormat))
        {
            ////unexpected situation
            system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_SECONDARY_SCANNING, __LINE__, (uint32_t)pstSet, (uint32_t)pstChainPdu);
            return;
        }
        
        //RSSI
        pu8Parameters[15] = (uint8_t)rom_llc_get_rssi(pstChainPdu->u32RssiRelatedValue);
        
        if (stFormat.AdvData_Present && (0 == u8DataMaxLength))
        {
            //unable to report
            
            //Data_Length
            pu8Parameters[25] = 0;
            
            Event_Type.bitField.DataStatus = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_EVENT_TYPE_DATA_STATUS_INCOMPLETE_NO_MORE_DATA;
            rom_llc_tool_write_little_endian_uint16(&(pu8Parameters[2]), Event_Type.u16Value);
            rom_controller_event_HCI_LE_Extended_Advertising_Report(REPORT_PARAMETERS_SIZE_WITHOUT_DATA + 0, pu8Parameters);
            return;
        }
        
        uint16_t u16DataOffset = 0;
        do
        {
            //Data_Length
            uint8_t Data_Length = stFormat.AdvData_Length - u16DataOffset;
            if (Data_Length > u8DataMaxLength)
            {
                Data_Length = u8DataMaxLength;
            }
            pu8Parameters[25] = Data_Length;
            
            //Data
            if (Data_Length > 0)
            {
                memcpy(&(pu8Parameters[26]), &(stFormat.AdvData[u16DataOffset]), Data_Length);
            }
            
            u16DataOffset += Data_Length;
            
            //Event_Type Data_Status
            if (u16DataOffset < stFormat.AdvData_Length)
            {
                Event_Type.bitField.DataStatus = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_EVENT_TYPE_DATA_STATUS_INCOMPLETE_MORE_DATA;
            }
            else
            {
                if (stFormat.AdvA_Present)
                {
                    if (NULL == pstChainPdu->next)
                    {
                        Event_Type.bitField.DataStatus = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_EVENT_TYPE_DATA_STATUS_INCOMPLETE_NO_MORE_DATA;
                    }
                    else
                    {
                        Event_Type.bitField.DataStatus = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_EVENT_TYPE_DATA_STATUS_INCOMPLETE_MORE_DATA;
                    }
                }
                else
                {
                    Event_Type.bitField.DataStatus = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_EVENT_TYPE_DATA_STATUS_COMPLETE;
                }
            }
            rom_llc_tool_write_little_endian_uint16(&(pu8Parameters[2]), Event_Type.u16Value);
            rom_controller_event_HCI_LE_Extended_Advertising_Report(REPORT_PARAMETERS_SIZE_WITHOUT_DATA + Data_Length, pu8Parameters);
        }
        while(u16DataOffset < stFormat.AdvData_Length);
        
        pstChainPdu = pstChainPdu->next;
    }
}

void rom_llc_ext_scan_secondary_scanning_report_process_nonconnectable_nonscannable(stLlcExtScan_t* pstScan, stLlcExtScanAdvertisingSet_t* pstSet)
{
    //process set with Num_Reports==1
    /*        
    Subevent_Code,                          Size: 1 octet
    Num_Reports,                            Size: 1 octet
    
    Event_Type,                             Size: 2 octets
    Address_Type,                           Size: 1 octet
    Address,                                Size: 6 octets
    Primary_PHY,                            Size: 1 octet
    Secondary_PHY,                          Size: 1 octet
    Advertising_SID,                        Size: 1 octet
    Tx_Power,                               Size: 1 octet
    RSSI,                                   Size: 1 octet
    Periodic_Advertising_Interval,          Size: 2 octets
    Direct_Address_Type,                    Size: 1 octet
    Direct_Address,                         Size: 6 octets
    Data_Length,                            Size: 1 octet
    Data                                    Size: (Data_Length) octets
    */
    
    #define REPORT_PARAMETERS_SIZE_WITHOUT_DATA (1+1+2+1+6+1+1+1+1+1+2+1+6+1)

    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat = {0};
    


    uint8_t pu8Parameters[256];
    uint8_t u8ReportsNum = 0;
    uint16_t u16ParametersMaxLength = LLC_TOOL_MIN(sizeof(pu8Parameters), rom_hci_get_event_parameter_max_length());
    if (u16ParametersMaxLength < (REPORT_PARAMETERS_SIZE_WITHOUT_DATA))
    {
        //unable to send report
        return;
    }
    uint8_t u8DataMaxLength = LLC_TOOL_MIN(u16ParametersMaxLength - (REPORT_PARAMETERS_SIZE_WITHOUT_DATA), 229);//229 : core spec
    
    if (NULL == pstSet->pstAdvPdu)
    {
        //no auxiliary packet received
        
        if (0 != pstSet->u64StartTimeUs)
        {
            //with auxiliary packet, receive packet failed
            return;
        }
        
        //without auxiliary packet
        if (LLC_SPEC_5_1_HCI_FILTER_DUPLICATES_DISABLED != pstScan->Filter_Duplicates)
        {
            if (rom_llc_ext_scan_duplicate_filter_is_report_duplicated(pstSet->enumAddressType,
                pstSet->pu8Address,
                pstSet->u8SID))
            {
                return;
            }
            else
            {
                rom_llc_ext_scan_duplicate_filter_add_report(pstSet->enumAddressType,
                pstSet->pu8Address,
                pstSet->u8SID);
            }
        }
        
        //Subevent_Code
        pu8Parameters[0] = LLC_SPEC_5_1_HCI_SUBEVENT_CODE_HCI_LE_Extended_Advertising_Report;
        
        //Num_Reports
        pu8Parameters[1] = 1;
        
        //Event_Type
        stLlcSpec51HciExtendedAdvertisingReportEventType_t Event_Type = {0};
        Event_Type.bitField.Connectable = pstSet->bConnectable;
        Event_Type.bitField.Scannable = pstSet->bScannable;
        Event_Type.bitField.Directed = pstSet->bDirected;
        Event_Type.bitField.ScanResponse = false;
        Event_Type.bitField.LegacyPDUsUsed = false;
        Event_Type.bitField.DataStatus = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_EVENT_TYPE_DATA_STATUS_COMPLETE;
        rom_llc_tool_write_little_endian_uint16(&(pu8Parameters[2]), Event_Type.u16Value);
        
        //Address_Type
        pu8Parameters[4] = pstSet->enumAddressType;
        
        //Address
        memcpy(&(pu8Parameters[5]), pstSet->pu8Address, 6);
        
        //Primary_PHY
        switch(pstSet->enumPrimaryPhy)
        {
            case LL_PHY_1M:
                pu8Parameters[11] = LLC_SPEC_5_1_HCI_PHY_LE_1M;
                break;
            
            case LL_PHY_2M:
                pu8Parameters[11] = LLC_SPEC_5_1_HCI_PHY_LE_2M;
                break;
            
            case LL_PHY_CODED_S2:
            case LL_PHY_CODED_S8:
                pu8Parameters[11] = LLC_SPEC_5_1_HCI_PHY_LE_CODED;
                break;
        }
        
        //Secondary_PHY
        pu8Parameters[12] = LLC_SPEC_5_1_HCI_PHY_NONE;
        
        //Advertising_SID
        pu8Parameters[13] = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_SID_NO_ADI_PROVIDED;
        
        //Tx_Power
        pu8Parameters[14] = (uint8_t)pstSet->s8LastTxPower;
        
        //RSSI
        pu8Parameters[15] = (uint8_t)rom_llc_get_rssi(pstSet->u32RssiRelatedValueForSetWithoutAuxiliaryPacket);
        
        //Periodic_Advertising_Interval
        rom_llc_tool_write_little_endian_uint16(&(pu8Parameters[16]), 0);
        
        
        //Direct_Address_Type
        pu8Parameters[18] = pstSet->enumDirectAddressType;
        
        //Direct_Address
        memcpy(&(pu8Parameters[19]), pstSet->pu8DirectAddress, 6);
        
        //Data_Length
        pu8Parameters[25] = 0;
        
        //Data
        //null
        rom_controller_event_HCI_LE_Extended_Advertising_Report(REPORT_PARAMETERS_SIZE_WITHOUT_DATA + 0, pu8Parameters);
        return;
    }
    
    //with auxiliary packet
    
    if (!rom_llc_common_extended_advertising_payload_parse(pstSet->pstAdvPdu, &stFormat))
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_SECONDARY_SCANNING, __LINE__, (uint32_t)pstSet, (uint32_t)pstSet->pstAdvPdu);
        return;
    }
    if (stFormat.AdvData_Present && (0 == u8DataMaxLength))
    {
        //unable to send report with data
        return;
    }
    
    if (LLC_SPEC_5_1_HCI_FILTER_DUPLICATES_DISABLED != pstScan->Filter_Duplicates)
    {
        if (rom_llc_ext_scan_duplicate_filter_is_report_duplicated(pstSet->enumAddressType,
            pstSet->pu8Address,
            pstSet->u8SID))
        {
            return;
        }
        else
        {
            rom_llc_ext_scan_duplicate_filter_add_report(pstSet->enumAddressType,
            pstSet->pu8Address,
            pstSet->u8SID);
        }
    }
    
    //Subevent_Code
    pu8Parameters[0] = LLC_SPEC_5_1_HCI_SUBEVENT_CODE_HCI_LE_Extended_Advertising_Report;
    
    //Num_Reports
    pu8Parameters[1] = 1;
    
    //Event_Type
    stLlcSpec51HciExtendedAdvertisingReportEventType_t Event_Type = {0};
    Event_Type.bitField.Connectable = pstSet->bConnectable;
    Event_Type.bitField.Scannable = pstSet->bScannable;
    Event_Type.bitField.Directed = pstSet->bDirected;
    Event_Type.bitField.ScanResponse = false;
    Event_Type.bitField.LegacyPDUsUsed = false;
    //Data_Status Set Later
    
    //Address_Type
    pu8Parameters[4] = pstSet->enumAddressType;
    
    //Address
    memcpy(&(pu8Parameters[5]), pstSet->pu8Address, 6);
    
    //Primary_PHY
    switch(pstSet->enumPrimaryPhy)
    {
        case LL_PHY_1M:
            pu8Parameters[11] = LLC_SPEC_5_1_HCI_PHY_LE_1M;
            break;
        
        case LL_PHY_2M:
            pu8Parameters[11] = LLC_SPEC_5_1_HCI_PHY_LE_2M;
            break;
        
        case LL_PHY_CODED_S2:
        case LL_PHY_CODED_S8:
            pu8Parameters[11] = LLC_SPEC_5_1_HCI_PHY_LE_CODED;
            break;
    }
    
    //Secondary_PHY
    switch(pstSet->enumSecondaryPhy)
    {
        case LL_PHY_1M:
            pu8Parameters[12] = LLC_SPEC_5_1_HCI_PHY_LE_1M;
            break;
        
        case LL_PHY_2M:
            pu8Parameters[12] = LLC_SPEC_5_1_HCI_PHY_LE_2M;
            break;
        
        case LL_PHY_CODED_S2:
        case LL_PHY_CODED_S8:
            pu8Parameters[12] = LLC_SPEC_5_1_HCI_PHY_LE_CODED;
            break;
    }
    
    //Advertising_SID
    pu8Parameters[13] = pstSet->u8SID;
    
    //Tx_Power
    pu8Parameters[14] = (uint8_t)pstSet->s8LastTxPower;
    
    //RSSI
    pu8Parameters[15] = (uint8_t)rom_llc_get_rssi(pstSet->pstAdvPdu->u32RssiRelatedValue);
    
    //Periodic_Advertising_Interval
    if (stFormat.SyncInfo_Present)
    {
        rom_llc_tool_write_little_endian_uint16(&(pu8Parameters[16]), stFormat.SyncInfo_Interval1250Us);
    }
    else
    {
        rom_llc_tool_write_little_endian_uint16(&(pu8Parameters[16]), 0);
    }
    
    //Direct_Address_Type
    pu8Parameters[18] = pstSet->enumDirectAddressType;
    
    //Direct_Address
    memcpy(&(pu8Parameters[19]), pstSet->pu8DirectAddress, 6);
    
    uint16_t u16DataOffset = 0;
    do
    {
        //Data_Length
        uint8_t Data_Length = stFormat.AdvData_Length - u16DataOffset;
        if (Data_Length > u8DataMaxLength)
        {
            Data_Length = u8DataMaxLength;
        }
        pu8Parameters[25] = Data_Length;
        
        //Data
        if (Data_Length > 0)
        {
            memcpy(&(pu8Parameters[26]), &(stFormat.AdvData[u16DataOffset]), Data_Length);
        }
        
        u16DataOffset += Data_Length;
        
        //Event_Type Data_Status
        if (u16DataOffset < stFormat.AdvData_Length)
        {
            Event_Type.bitField.DataStatus = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_EVENT_TYPE_DATA_STATUS_INCOMPLETE_MORE_DATA;
        }
        else
        {
            if (stFormat.AdvA_Present)
            {
                if (NULL == pstSet->pstChainPduListHead)
                {
                    Event_Type.bitField.DataStatus = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_EVENT_TYPE_DATA_STATUS_INCOMPLETE_NO_MORE_DATA;
                }
                else
                {
                    Event_Type.bitField.DataStatus = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_EVENT_TYPE_DATA_STATUS_INCOMPLETE_MORE_DATA;
                }
            }
            else
            {
                Event_Type.bitField.DataStatus = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_EVENT_TYPE_DATA_STATUS_COMPLETE;
            }
        }
        rom_llc_tool_write_little_endian_uint16(&(pu8Parameters[2]), Event_Type.u16Value);
        rom_controller_event_HCI_LE_Extended_Advertising_Report(REPORT_PARAMETERS_SIZE_WITHOUT_DATA + Data_Length, pu8Parameters);
    }
    while(u16DataOffset < stFormat.AdvData_Length);
    
    stLlcAdvChannelPdu_t* pstChainPdu = pstSet->pstChainPduListHead;
    while(NULL != pstChainPdu)
    {
        if (!rom_llc_common_extended_advertising_payload_parse(pstChainPdu, &stFormat))
        {
            ////unexpected situation
            system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_SECONDARY_SCANNING, __LINE__, (uint32_t)pstSet, (uint32_t)pstChainPdu);
            return;
        }
        
        //RSSI
        pu8Parameters[15] = (uint8_t)rom_llc_get_rssi(pstChainPdu->u32RssiRelatedValue);
        
        if (stFormat.AdvData_Present && (0 == u8DataMaxLength))
        {
            //unable to report
            
            //Data_Length
            pu8Parameters[25] = 0;
            
            Event_Type.bitField.DataStatus = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_EVENT_TYPE_DATA_STATUS_INCOMPLETE_NO_MORE_DATA;
            rom_llc_tool_write_little_endian_uint16(&(pu8Parameters[2]), Event_Type.u16Value);
            rom_controller_event_HCI_LE_Extended_Advertising_Report(REPORT_PARAMETERS_SIZE_WITHOUT_DATA + 0, pu8Parameters);
            return;
        }
        
        uint16_t u16DataOffset = 0;
        do
        {
            //Data_Length
            uint8_t Data_Length = stFormat.AdvData_Length - u16DataOffset;
            if (Data_Length > u8DataMaxLength)
            {
                Data_Length = u8DataMaxLength;
            }
            pu8Parameters[25] = Data_Length;
            
            //Data
            if (Data_Length > 0)
            {
                memcpy(&(pu8Parameters[26]), &(stFormat.AdvData[u16DataOffset]), Data_Length);
            }
            
            u16DataOffset += Data_Length;
            
            //Event_Type Data_Status
            if (u16DataOffset < stFormat.AdvData_Length)
            {
                Event_Type.bitField.DataStatus = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_EVENT_TYPE_DATA_STATUS_INCOMPLETE_MORE_DATA;
            }
            else
            {
                if (stFormat.AdvA_Present)
                {
                    if (NULL == pstChainPdu->next)
                    {
                        Event_Type.bitField.DataStatus = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_EVENT_TYPE_DATA_STATUS_INCOMPLETE_NO_MORE_DATA;
                    }
                    else
                    {
                        Event_Type.bitField.DataStatus = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_EVENT_TYPE_DATA_STATUS_INCOMPLETE_MORE_DATA;
                    }
                }
                else
                {
                    Event_Type.bitField.DataStatus = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_EVENT_TYPE_DATA_STATUS_COMPLETE;
                }
            }
            rom_llc_tool_write_little_endian_uint16(&(pu8Parameters[2]), Event_Type.u16Value);
            rom_controller_event_HCI_LE_Extended_Advertising_Report(REPORT_PARAMETERS_SIZE_WITHOUT_DATA + Data_Length, pu8Parameters);
        }
        while(u16DataOffset < stFormat.AdvData_Length);
        
        pstChainPdu = pstChainPdu->next;
    }
}

void rom_llc_ext_scan_secondary_scanning_report_task_process(void* pParam)
{
    stLlcExtScan_t* pstScan = (stLlcExtScan_t*)pParam;
    stLlcExtScanAdvertisingSet_t* pstSet;
    for(;;)
    {
        //take set from list
        uint32_t u32Primask = __get_PRIMASK();
        __disable_irq();
        pstSet = pstScan->pstSecondaryScanningSetReportListHead;
        if (NULL == pstSet)
        {
            pstScan->bSecondaryReporting = false;
            __set_PRIMASK(u32Primask);
            break;
        }
        pstScan->pstSecondaryScanningSetReportListHead = pstSet->next;
        if (NULL == pstScan->pstSecondaryScanningSetReportListHead)
        {
            pstScan->pstSecondaryScanningSetReportListTail = NULL;
        }
        else
        {
            pstScan->pstSecondaryScanningSetReportListHead->prev = NULL;
        }
        pstSet->prev = NULL;
        pstSet->next = NULL;
        __set_PRIMASK(u32Primask);
        
        //process
        if (pstSet->bConnectable && pstSet->bScannable)
        {
            ////unexpected situation
            system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_SECONDARY_SCANNING, __LINE__, (uint32_t)pstSet, (uint32_t)0);
        }
        else if (pstSet->bConnectable)
        {
            //connectable
            rom_llc_ext_scan_secondary_scanning_report_process_connectable(pstScan, pstSet);
        }
        else if (pstSet->bScannable)
        {
            //scannable
            rom_llc_ext_scan_secondary_scanning_report_process_scannable(pstScan, pstSet);
        }
        else
        {
            //nonconnectable nonscannable
            rom_llc_ext_scan_secondary_scanning_report_process_nonconnectable_nonscannable(pstScan, pstSet);
        }
        
        //free set
        rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
    }
}

void rom_llc_ext_scan_secondary_scanning_report_advertising_set(stLlcExtScan_t* pstScan, stLlcExtScanAdvertisingSet_t* pstSet)
{
    uint32_t u32Primask = __get_PRIMASK();
    __disable_irq();
    
    //add to report list
    if (NULL == pstScan->pstSecondaryScanningSetReportListHead)
    {
        pstSet->prev = NULL;
        pstSet->next = NULL;
        
        pstScan->pstSecondaryScanningSetReportListHead = pstSet;
        pstScan->pstSecondaryScanningSetReportListTail = pstSet;
    }
    else
    {
        pstSet->prev = pstScan->pstSecondaryScanningSetReportListTail;
        pstSet->next = NULL;
        
        pstScan->pstSecondaryScanningSetReportListTail->next = pstSet;
        pstScan->pstSecondaryScanningSetReportListTail = pstSet;
    }
    
    //task process
    //bSecondaryReporting flag to reduce duplicated msg sent
    if (!pstScan->bSecondaryReporting)
    {
        pstScan->bSecondaryReporting = rom_llc_send_msg_to_llc_task(rom_llc_ext_scan_secondary_scanning_report_task_process, pstScan);
    }
    __set_PRIMASK(u32Primask);
}


void rom_llc_ext_scan_secondary_scanning_on_llp_fragment_stop_at_aux_state(stLlcExtScan_t* pstScan, stLlcExtScanAdvertisingSet_t* pstSet, bool bFragmentTruncated)
{
    //remove set from schedule list
    rom_llc_ext_scan_secondary_scanning_remove_advertising_set_from_schedule_list(pstScan, pstSet);
    
    stLlcAdvChannelPdu_t* pstAdvPdu = pstSet->pstAdvPdu;
    if (NULL == pstAdvPdu)
    {
        MLOG1616I(0x3833, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
        return;
    }
    
    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat;
    if (!rom_llc_common_extended_advertising_payload_parse(pstAdvPdu, &stFormat))
    {
        MLOG1616I(0x3834, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
        return;
    }
    
    if (stFormat.Header.bitField.Length > pstAdvPdu->u16PayloadBufSize)
    {
        MLOG1616I(0x3835, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
        return;
    }
    
    if (LLC_SPEC_5_1_ADV_CH_PDU_TYPE_AUX_ADV_IND != stFormat.Header.bitField.PDUType)
    {
        MLOG1616I(0x3836, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
        return;
    }
    
    if ((stFormat.Connectable != pstSet->bConnectable)||(stFormat.Scannable != pstSet->bScannable))
    {
        MLOG1616I(0x3837, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
        return;
    }
    
    if (stFormat.CTEInfo_Present || (!stFormat.ADI_Present) )
    {
        MLOG1616I(0x3838, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
        return;
    }
    
    if ((stFormat.ADI_DID != pstSet->u16DID) || (stFormat.ADI_SID != pstSet->u8SID))
    {
        MLOG1616I(0x3839, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
        return;
    }
    
    if (pstSet->bConnectable)
    {
        //connectable
        
        if ((!stFormat.AdvA_Present) || stFormat.AuxPtr_Present || stFormat.SyncInfo_Present)
        {
            MLOG1616I(0x383a, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
            rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
            return;
        }
        
        pstSet->bDirected = stFormat.TargetA_Present;
        if (rom_llc_ext_scan_check_RPA_and_white_list_disallowed(pstScan,
                stFormat.Header.bitField.TxAdd, stFormat.AdvA_Present?stFormat.AdvA:NULL,
                stFormat.Header.bitField.RxAdd, stFormat.TargetA_Present?stFormat.TargetA:NULL,
                &pstSet->enumAddressType, pstSet->pu8Address,
                &pstSet->enumDirectAddressType, pstSet->pu8DirectAddress))
        {
            MLOG1616I(0x383b, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
            rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
            return;
        }
        
        if (stFormat.TxPower_Present)
        {
            pstSet->s8LastTxPower = stFormat.TxPower;
        }
        rom_llc_ext_scan_secondary_scanning_report_advertising_set(pstScan, pstSet);
    }
    else if (pstSet->bScannable)
    {
        //scannable
        
        if ((!stFormat.AdvA_Present) || stFormat.AuxPtr_Present || stFormat.SyncInfo_Present || stFormat.AdvData_Present)
        {
            MLOG1616I(0x383c, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
            rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
            return;
        }
        
        pstSet->bDirected = stFormat.TargetA_Present;
        if (rom_llc_ext_scan_check_RPA_and_white_list_disallowed(pstScan,
                stFormat.Header.bitField.TxAdd, stFormat.AdvA_Present?stFormat.AdvA:NULL,
                stFormat.Header.bitField.RxAdd, stFormat.TargetA_Present?stFormat.TargetA:NULL,
                &pstSet->enumAddressType, pstSet->pu8Address,
                &pstSet->enumDirectAddressType, pstSet->pu8DirectAddress))
        {
            MLOG1616I(0x383d, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
            rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
            return;
        }
        
        if (stFormat.TxPower_Present)
        {
            pstSet->s8LastTxPower = stFormat.TxPower;
        }
        
        //parse rsp pdu
        stLlcAdvChannelPdu_t* pstRspPdu = pstSet->pstRspPdu;
        if (NULL == pstRspPdu)
        {
            rom_llc_ext_scan_secondary_scanning_report_advertising_set(pstScan, pstSet);//report since scan type may be PASSIVE
            return;
        }
        
        if (!rom_llc_common_extended_advertising_payload_parse(pstRspPdu, &stFormat))
        {
            MLOG1616I(0x383e, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
            rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
            return;
        }
        
        if (stFormat.Header.bitField.Length > pstAdvPdu->u16PayloadBufSize)
        {
            MLOG1616I(0x383f, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
            rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
            return;
        }
        
        if (LLC_SPEC_5_1_ADV_CH_PDU_TYPE_AUX_SCAN_RSP != stFormat.Header.bitField.PDUType)
        {
            MLOG1616I(0x3840, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
            rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
            return;
        }
        
        if (stFormat.Connectable || stFormat.Scannable)
        {
            MLOG1616I(0x3841, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
            rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
            return;
        }
        
        if ((!stFormat.AdvA_Present) || stFormat.TargetA_Present || stFormat.CTEInfo_Present || stFormat.SyncInfo_Present || (!stFormat.AdvData_Present))
        {
            MLOG1616I(0x3842, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
            rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
            return;
        }
        
        if ((stFormat.ADI_Present) && ((stFormat.ADI_DID != pstSet->u16DID) || (stFormat.ADI_SID != pstSet->u8SID)))
        {
            MLOG1616I(0x3843, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
            rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
            return;
        }
        
        if (stFormat.TxPower_Present)
        {
            pstSet->s8LastTxPower = stFormat.TxPower;
        }
        
        
        if (stFormat.AuxPtr_Present)
        {
            if (stFormat.AuxPtr_Phy != pstSet->enumSecondaryPhy)
            {
                MLOG1616I(0x3844, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
                rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
                return;
            }
            
            if (0 == stFormat.AuxPtr_Offset30Us)
            {
                rom_llc_ext_scan_secondary_scanning_report_advertising_set(pstScan, pstSet);
                return;
            }
            
            //update set
            stLlcExtScanPduExtraInfo_t* pstExtraInfo = rom_llc_ext_scan_get_pdu_extra_info(pstRspPdu);
            uint64_t u64IdealStartTime = pstExtraInfo->u64PduStartTimeUs + (stFormat.AuxPtr_Offset30Us * 30);
            uint32_t u32OffsetTimeUs = (stFormat.AuxPtr_Offset30Us + stFormat.AuxPtr_OffsetUnits30Us) * 30;
            uint32_t u32WindowWideningUs = (rom_llc_get_llp_clock_accuracy_ppm() + stFormat.AuxPtr_ClockAccuracyPpm) * (u32OffsetTimeUs) / 1000000 + 32;
            pstSet->enumSetState = LLC_EXT_SCAN_ADVERTISING_SET_STATE_CHAIN;
            pstSet->u64StartTimeUs = u64IdealStartTime - u32WindowWideningUs;
            pstSet->u32AnchorWindowUs = (stFormat.AuxPtr_OffsetUnits30Us * 30) + u32WindowWideningUs * 2;
            pstSet->u32WorkTimeUs = pstSet->u32AnchorWindowUs + rom_llc_tool_calculate_trx_time_us_by_pdu_size(2 + LLC_SPEC_5_1_ADV_CHANNEL_PDU_MAX_PAYLOAD_SIZE, stFormat.AuxPtr_Phy);
            pstSet->u8ChannelIdx = stFormat.AuxPtr_ChannelIndex;
            
            //add to schedule list
            if (rom_llc_ext_scan_secondary_scanning_add_advertising_set_to_schedule_list(pstScan, pstSet))
            {
                //MLOG1616I(0x3845, "advertising set CONTINUE CHAIN, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
            }
            else
            {
                rom_llc_ext_scan_secondary_scanning_report_advertising_set(pstScan, pstSet);
            }
        }
        else
        {
            MLOG1616I(0x3846, "advertising set FINISH, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
            rom_llc_ext_scan_secondary_scanning_report_advertising_set(pstScan, pstSet);
            return;
        }
        
    }
    else
    {
        //nonn-connectable non-scannable
        
        uint8_t *AdvA;
        bool bHasAddressSet;
        uint8_t *TargetA;
        bool bHasDirectAddressSet;
        
        if (LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_ANONYMOUS != pstSet->enumAddressType)
        {
            //AdvA present in ADV_EXT_IND
            if (stFormat.AdvA_Present)
            {
                MLOG1616I(0x3847, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
                rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
                return;
            }
            
            bHasAddressSet = true;
            AdvA = NULL;
        }
        else
        {
            //AdvA present in AUX_ADV_IND or Anonymous
            bHasAddressSet = false;
            if (stFormat.AdvA_Present)
            {
                AdvA = stFormat.AdvA;
            }
            else
            {
                //Anonymous
                AdvA = NULL;
            }
        }
        
        if (pstSet->bDirected)
        {
            //TargetA present in ADV_EXT_IND
            if (stFormat.TargetA_Present)
            {
                MLOG1616I(0x3848, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
                rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
                return;
            }
            
            bHasDirectAddressSet = true;
            TargetA = NULL;
        }
        else
        {
            //TargetA present in AUX_ADV_IND or undirected
            bHasDirectAddressSet = false;
            if (stFormat.TargetA_Present)
            {
                pstSet->bDirected = true;
                TargetA = stFormat.TargetA;
            }
            else
            {
                //undirected
                TargetA = NULL;
            }
        }
        if (!(bHasAddressSet && bHasDirectAddressSet))
        {
            if (rom_llc_ext_scan_check_RPA_and_white_list_disallowed(pstScan,
                stFormat.Header.bitField.TxAdd, AdvA,
                stFormat.Header.bitField.RxAdd, TargetA,
                (bHasAddressSet)?(NULL):(&pstSet->enumAddressType), (bHasAddressSet)?(NULL):(pstSet->pu8Address),
                (bHasDirectAddressSet)?(NULL):(&pstSet->enumDirectAddressType), (bHasDirectAddressSet)?(NULL):(pstSet->pu8DirectAddress)))
            {
                MLOG1616I(0x3849, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
                rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
                return;
            }
        }
        
        
        if (stFormat.TxPower_Present)
        {
            pstSet->s8LastTxPower = stFormat.TxPower;
        }
        
        
        if (stFormat.AuxPtr_Present)
        {
            if (stFormat.AuxPtr_Phy != pstSet->enumSecondaryPhy)
            {
                MLOG1616I(0x384a, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
                rom_llc_ext_scan_secondary_scanning_free_advertising_set(pstScan, pstSet);
                return;
            }
            
            if (0 == stFormat.AuxPtr_Offset30Us)
            {
                rom_llc_ext_scan_secondary_scanning_report_advertising_set(pstScan, pstSet);
                return;
            }
            
            //update set
            stLlcExtScanPduExtraInfo_t* pstExtraInfo = rom_llc_ext_scan_get_pdu_extra_info(pstAdvPdu);
            uint64_t u64IdealStartTime = pstExtraInfo->u64PduStartTimeUs + (stFormat.AuxPtr_Offset30Us * 30);
            uint32_t u32OffsetTimeUs = (stFormat.AuxPtr_Offset30Us + stFormat.AuxPtr_OffsetUnits30Us) * 30;
            uint32_t u32WindowWideningUs = (rom_llc_get_llp_clock_accuracy_ppm() + stFormat.AuxPtr_ClockAccuracyPpm) * (u32OffsetTimeUs) / 1000000 + 32;
            pstSet->enumSetState = LLC_EXT_SCAN_ADVERTISING_SET_STATE_CHAIN;
            pstSet->u64StartTimeUs = u64IdealStartTime - u32WindowWideningUs;
            pstSet->u32AnchorWindowUs = (stFormat.AuxPtr_OffsetUnits30Us * 30) + u32WindowWideningUs * 2;
            pstSet->u32WorkTimeUs = pstSet->u32AnchorWindowUs + rom_llc_tool_calculate_trx_time_us_by_pdu_size(2 + LLC_SPEC_5_1_ADV_CHANNEL_PDU_MAX_PAYLOAD_SIZE, stFormat.AuxPtr_Phy);
            pstSet->u8ChannelIdx = stFormat.AuxPtr_ChannelIndex;
            
            //add to schedule list
            if (rom_llc_ext_scan_secondary_scanning_add_advertising_set_to_schedule_list(pstScan, pstSet))
            {
                MLOG1616I(0x384b, "advertising set CONTINUE CHAIN, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
            }
            else
            {
                MLOG1616I(0x384c, "advertising set CHAIN Collision, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
                rom_llc_ext_scan_secondary_scanning_report_advertising_set(pstScan, pstSet);
            }
        }
        else
        {
            MLOG1616I(0x384d, "advertising set FINISH, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
            rom_llc_ext_scan_secondary_scanning_report_advertising_set(pstScan, pstSet);
            return;
        }
    }
    
}

void rom_llc_ext_scan_secondary_scanning_remove_advertising_set_last_chain_pdu(stLlcExtScan_t* pstScan, stLlcExtScanAdvertisingSet_t* pstSet)
{
    uint32_t u32Primask = __get_PRIMASK();
    __disable_irq();
    stLlcAdvChannelPdu_t* pstLastChainPdu = pstSet->pstChainPduListTail;
    if (NULL == pstLastChainPdu)
    {
        __set_PRIMASK(u32Primask);
        return;
    }
    stLlcAdvChannelPdu_t* pstPrevPdu = pstLastChainPdu->prev;
    if (NULL == pstPrevPdu)
    {
        //is Head
        pstSet->pstChainPduListHead = NULL;
        pstSet->pstChainPduListTail = NULL;
    }
    else
    {
        //is not Head
        pstPrevPdu->next = NULL;
        pstSet->pstChainPduListTail = pstPrevPdu;
    }
    pstLastChainPdu->prev = NULL;
    pstLastChainPdu->next = NULL;
    __set_PRIMASK(u32Primask);
    rom_llc_ext_scan_free_pdu(pstScan, pstLastChainPdu);
}

void rom_llc_ext_scan_secondary_scanning_on_llp_fragment_stop_at_chain_state(stLlcExtScan_t* pstScan, stLlcExtScanAdvertisingSet_t* pstSet, bool bFragmentTruncated)
{
    //remove set from schedule list
    rom_llc_ext_scan_secondary_scanning_remove_advertising_set_from_schedule_list(pstScan, pstSet);
    
    if (bFragmentTruncated)
    {
        MLOG1616I(0x384e, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        rom_llc_ext_scan_secondary_scanning_report_advertising_set(pstScan, pstSet);
        return;
    }
    
    stLlcAdvChannelPdu_t* pstLastChainPdu = pstSet->pstChainPduListTail;
    if (NULL == pstLastChainPdu)
    {
        rom_llc_ext_scan_secondary_scanning_report_advertising_set(pstScan, pstSet);
        return;
    }
    
    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat;
    if (!rom_llc_common_extended_advertising_payload_parse(pstLastChainPdu, &stFormat))
    {
        MLOG1616I(0x384f, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        rom_llc_ext_scan_secondary_scanning_remove_advertising_set_last_chain_pdu(pstScan, pstSet);
        rom_llc_ext_scan_secondary_scanning_report_advertising_set(pstScan, pstSet);
        return;
    }
    
    if (stFormat.Header.bitField.Length > pstLastChainPdu->u16PayloadBufSize)
    {
        MLOG1616I(0x3850, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        rom_llc_ext_scan_secondary_scanning_remove_advertising_set_last_chain_pdu(pstScan, pstSet);
        rom_llc_ext_scan_secondary_scanning_report_advertising_set(pstScan, pstSet);
        return;
    }
    
    if (LLC_SPEC_5_1_ADV_CH_PDU_TYPE_AUX_CHAIN_IND != stFormat.Header.bitField.PDUType)
    {
        MLOG1616I(0x3851, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        rom_llc_ext_scan_secondary_scanning_remove_advertising_set_last_chain_pdu(pstScan, pstSet);
        rom_llc_ext_scan_secondary_scanning_report_advertising_set(pstScan, pstSet);
        return;
    }
    
    if (stFormat.Connectable || stFormat.Scannable || stFormat.AdvA_Present || stFormat.TargetA_Present || stFormat.CTEInfo_Present 
        || (!stFormat.ADI_Present) || stFormat.SyncInfo_Present || stFormat.ACAD_Present)
    {
        MLOG1616I(0x3852, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        rom_llc_ext_scan_secondary_scanning_remove_advertising_set_last_chain_pdu(pstScan, pstSet);
        rom_llc_ext_scan_secondary_scanning_report_advertising_set(pstScan, pstSet);
        return;
    }
    
    if ((stFormat.ADI_DID != pstSet->u16DID) || (stFormat.ADI_SID != pstSet->u8SID))
    {
        MLOG1616I(0x3853, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        rom_llc_ext_scan_secondary_scanning_remove_advertising_set_last_chain_pdu(pstScan, pstSet);
        rom_llc_ext_scan_secondary_scanning_report_advertising_set(pstScan, pstSet);
        return;
    }
    
    
    if (stFormat.TxPower_Present)
    {
        pstSet->s8LastTxPower = stFormat.TxPower;
    }
    
    
    if (stFormat.AuxPtr_Present)
    {
        if (stFormat.AuxPtr_Phy != pstSet->enumSecondaryPhy)
        {
            MLOG1616I(0x3854, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
            rom_llc_ext_scan_secondary_scanning_remove_advertising_set_last_chain_pdu(pstScan, pstSet);
            rom_llc_ext_scan_secondary_scanning_report_advertising_set(pstScan, pstSet);
            return;
        }
        
        if (0 == stFormat.AuxPtr_Offset30Us)
        {
            rom_llc_ext_scan_secondary_scanning_report_advertising_set(pstScan, pstSet);
            return;
        }
        
        //update set
        stLlcExtScanPduExtraInfo_t* pstExtraInfo = rom_llc_ext_scan_get_pdu_extra_info(pstLastChainPdu);
        uint64_t u64IdealStartTime = pstExtraInfo->u64PduStartTimeUs + (stFormat.AuxPtr_Offset30Us * 30);
        uint32_t u32OffsetTimeUs = (stFormat.AuxPtr_Offset30Us + stFormat.AuxPtr_OffsetUnits30Us) * 30;
        uint32_t u32WindowWideningUs = (rom_llc_get_llp_clock_accuracy_ppm() + stFormat.AuxPtr_ClockAccuracyPpm) * (u32OffsetTimeUs) / 1000000 + 32;
        pstSet->enumSetState = LLC_EXT_SCAN_ADVERTISING_SET_STATE_CHAIN;
        pstSet->u64StartTimeUs = u64IdealStartTime - u32WindowWideningUs;
        pstSet->u32AnchorWindowUs = (stFormat.AuxPtr_OffsetUnits30Us * 30) + u32WindowWideningUs * 2;
        pstSet->u32WorkTimeUs = pstSet->u32AnchorWindowUs + rom_llc_tool_calculate_trx_time_us_by_pdu_size(2 + LLC_SPEC_5_1_ADV_CHANNEL_PDU_MAX_PAYLOAD_SIZE, stFormat.AuxPtr_Phy);
        pstSet->u8ChannelIdx = stFormat.AuxPtr_ChannelIndex;
        
        //add set
        if (rom_llc_ext_scan_secondary_scanning_add_advertising_set_to_schedule_list(pstScan, pstSet))
        {
            MLOG1616I(0x3855, "advertising set CONTINUE CHAIN, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        }
        else
        {
            MLOG1616I(0x3856, "advertising set CHAIN Collision, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
            rom_llc_ext_scan_secondary_scanning_report_advertising_set(pstScan, pstSet);
        }
    }
    else
    {
        MLOG1616I(0x3857, "advertising set FINISH, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        rom_llc_ext_scan_secondary_scanning_report_advertising_set(pstScan, pstSet);
    }
}

bool rom_llc_ext_scan_secondary_scanning_on_llp_fragment_stop(stLlcExtScan_t* pstScan)
{
    if (NULL != pstScan->pstRecevingPdu)
    {
        rom_llc_ext_scan_free_pdu(pstScan, pstScan->pstRecevingPdu);
        pstScan->pstRecevingPdu = NULL;
    }
    
    stLlcExtScanAdvertisingSet_t* pstSet = pstScan->pstCurrentSecondaryScanningSet;
    if (NULL != pstSet)
    {
        switch(pstSet->enumSetState)
        {
            case LLC_EXT_SCAN_ADVERTISING_SET_STATE_AUX:
            {
                rom_llc_ext_scan_secondary_scanning_on_llp_fragment_stop_at_aux_state(pstScan, pstSet, pstScan->bSecondaryFragmentTruncated);
            }
                break;
            
            case LLC_EXT_SCAN_ADVERTISING_SET_STATE_CHAIN:
            {
                rom_llc_ext_scan_secondary_scanning_on_llp_fragment_stop_at_chain_state(pstScan, pstSet, pstScan->bSecondaryFragmentTruncated);
            }
                break;
            
            
            case LLC_EXT_SCAN_ADVERTISING_SET_STATE_IDLE:
            default:
                ////unexpected situation
                system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_SECONDARY_SCANNING, __LINE__, (uint32_t)pstSet, (uint32_t)pstSet->enumSetState);
                break;
        }
    }
    
    pstScan->pstCurrentSecondaryScanningSet = NULL;
    rom_llc_ext_scan_state_machine_schedule_next_scanning(pstScan);
    return false;
}

EN_LLP_STATE_CMD_T rom_llc_ext_scan_secondary_scanning_on_llp_tx_end(stLlcExtScan_t* pstScan, stLlpTxEndReport_t *pstReport)
{
    if (LLC_EXT_SCAN_STATE_TX_REQ != pstScan->enumState)
    {
        ////unexpected situation
            system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_SECONDARY_SCANNING, __LINE__, (uint32_t)pstScan, (uint32_t)pstScan->enumState);
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    pstScan->enumState = LLC_EXT_SCAN_STATE_RX_RSP;
    return LLP_STATE_CMD_CONTINUE;
}


EN_LLP_STATE_CMD_T rom_llc_ext_scan_secondary_scanning_on_llp_rx_adv_end(stLlcExtScan_t* pstScan, stLlcExtScanAdvertisingSet_t* pstSet, stLlpRxEndReport_t *pstReport)
{
    if (rom_llc_ext_scan_is_pdu_empty(pstScan, pstScan->pstRecevingPdu))
    {
        pstScan->enumState = LLC_EXT_SCAN_STATE_IDLE;
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    if (LLP_RX_RESULT_CRC_OK != pstReport->enumRxResult)
    {
        pstScan->enumState = LLC_EXT_SCAN_STATE_IDLE;
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    stLlcSpec51AdvChannelPduHeader_t stHeader;
    stHeader.u16Value = pstScan->pstRecevingPdu->u32Header & 0xFFFF;
    
    if (stHeader.bitField.Length > pstScan->pstRecevingPdu->u16PayloadBufSize)
    {
        pstScan->enumState = LLC_EXT_SCAN_STATE_IDLE;
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    if (LLC_SPEC_5_1_ADV_CH_PDU_TYPE_AUX_ADV_IND != stHeader.bitField.PDUType)
    {
        pstScan->enumState = LLC_EXT_SCAN_STATE_IDLE;
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    if (!pstSet->bScannable)
    {
        pstSet->pstAdvPdu = pstScan->pstRecevingPdu;
        pstScan->pstRecevingPdu = NULL;
        
        pstScan->bSecondaryFragmentTruncated = false;
        pstScan->enumState = LLC_EXT_SCAN_STATE_IDLE;
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    
    if (stHeader.bitField.Length < 2)
    {
        pstScan->enumState = LLC_EXT_SCAN_STATE_IDLE;
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    pstSet->pstAdvPdu = pstScan->pstRecevingPdu;
    
    
    //for better performance, "rom_llc_common_extended_advertising_payload_parse" is not used
    //pstSet->pstAdvPdu format will be re-check after fragment stop
    
    uint8_t Extended_Header_Flags = pstSet->pstAdvPdu->pu8Payload[1];
    uint8_t *AdvA = NULL;
    uint8_t *TargetA = NULL;
    if (Extended_Header_Flags & 1)//AdvA Present
    {
        AdvA = &(pstSet->pstAdvPdu->pu8Payload[2]);
        
        //update SCAN_REQ ASAP
        rom_llc_ext_scan_state_machine_update_scan_req_pdu_AdvA(pstScan, AdvA, stHeader.bitField.TxAdd);
        rom_llc_update_tx_pdu(&pstScan->stTxPduPtr);
    }
    
    pstScan->pstRecevingPdu = rom_llc_ext_scan_alloc_pdu(pstScan);
    pstScan->stRxPduPtr.pu16PayloadBufSize = &(pstScan->pstRecevingPdu->u16PayloadBufSize);
    pstScan->stRxPduPtr.pu8Payload = pstScan->pstRecevingPdu->pu8Payload;
    pstScan->stRxPduPtr.pu32Header = &(pstScan->pstRecevingPdu->u32Header);
    rom_llc_update_rx_pdu(&pstScan->stRxPduPtr, LLC_SPEC_5_1_ADV_CHANNEL_PDU_MAX_PAYLOAD_SIZE);
    
    if (Extended_Header_Flags & 2)//TagetA Present
    {
        if (stHeader.bitField.Length < 14)
        {
            pstScan->enumState = LLC_EXT_SCAN_STATE_IDLE;
            return LLP_STATE_CMD_STOP_FRAGMENT;
        }
        TargetA = &(pstSet->pstAdvPdu->pu8Payload[8]);
    }
    
    bool bScanRequestCancelled = false;
    if (LLC_SPEC_5_1_HCI_SCAN_TYPE_PASSIVE == pstScan->pstCurrentPrimaryScanningPhy->Scan_Type)
    {
        bScanRequestCancelled = true;
    }
    
    if (rom_llc_ext_scan_check_RPA_and_white_list_disallowed(pstScan,
        stHeader.bitField.TxAdd, AdvA,
        stHeader.bitField.RxAdd, TargetA,
        NULL, NULL,
        NULL, NULL))
    {
        bScanRequestCancelled = true;
    }
    
    if (bScanRequestCancelled)
    {
        pstScan->bSecondaryFragmentTruncated = false;
        pstScan->enumState = LLC_EXT_SCAN_STATE_IDLE;
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    else
    {
        if (rom_llc_ext_scan_state_machine_backoff_is_scan_canceled(pstScan))
        {
            pstScan->bSecondaryFragmentTruncated = false;
            pstScan->enumState = LLC_EXT_SCAN_STATE_IDLE;
            return LLP_STATE_CMD_STOP_FRAGMENT;
        }
        
        pstScan->enumState = LLC_EXT_SCAN_STATE_TX_REQ;
        return LLP_STATE_CMD_CONTINUE;
    }
}

EN_LLP_STATE_CMD_T rom_llc_ext_scan_secondary_scanning_on_llp_rx_rsp_end(stLlcExtScan_t* pstScan, stLlcExtScanAdvertisingSet_t* pstSet, stLlpRxEndReport_t *pstReport)
{
    pstScan->bSecondaryFragmentTruncated = false;
    pstScan->enumState = LLC_EXT_SCAN_STATE_IDLE;
    
    if (rom_llc_ext_scan_is_pdu_empty(pstScan, pstScan->pstRecevingPdu))
    {
        rom_llc_ext_scan_state_machine_backoff_scan_result(pstScan, false);
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    if (LLP_RX_RESULT_CRC_OK != pstReport->enumRxResult)
    {
        rom_llc_ext_scan_state_machine_backoff_scan_result(pstScan, false);
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    stLlcSpec51AdvChannelPduHeader_t stHeader;
    stHeader.u16Value = pstScan->pstRecevingPdu->u32Header & 0xFFFF;
    
    if (stHeader.bitField.Length > pstScan->pstRecevingPdu->u16PayloadBufSize)
    {
        rom_llc_ext_scan_state_machine_backoff_scan_result(pstScan, false);
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    if (LLC_SPEC_5_1_ADV_CH_PDU_TYPE_AUX_SCAN_RSP != stHeader.bitField.PDUType)
    {
        rom_llc_ext_scan_state_machine_backoff_scan_result(pstScan, false);
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    rom_llc_ext_scan_state_machine_backoff_scan_result(pstScan, true);
    
    pstSet->pstRspPdu = pstScan->pstRecevingPdu;
    pstScan->pstRecevingPdu = NULL;
    
    return LLP_STATE_CMD_STOP_FRAGMENT;
}

EN_LLP_STATE_CMD_T rom_llc_ext_scan_secondary_scanning_on_llp_rx_chain_end(stLlcExtScan_t* pstScan, stLlcExtScanAdvertisingSet_t* pstSet, stLlpRxEndReport_t *pstReport)
{
    if (rom_llc_ext_scan_is_pdu_empty(pstScan, pstScan->pstRecevingPdu))
    {
        pstScan->enumState = LLC_EXT_SCAN_STATE_IDLE;
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    if (LLP_RX_RESULT_CRC_OK != pstReport->enumRxResult)
    {
        pstScan->enumState = LLC_EXT_SCAN_STATE_IDLE;
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    uint32_t u32Primask = __get_PRIMASK();
    __disable_irq();
    if (NULL == pstSet->pstChainPduListHead)
    {
        pstScan->pstRecevingPdu->prev = NULL;
        pstScan->pstRecevingPdu->next = NULL;
        
        pstSet->pstChainPduListHead = pstScan->pstRecevingPdu;
        pstSet->pstChainPduListTail = pstScan->pstRecevingPdu;
    }
    else
    {
        pstScan->pstRecevingPdu->prev = pstSet->pstChainPduListTail;
        pstScan->pstRecevingPdu->next = NULL;
        
        pstSet->pstChainPduListTail->next = pstScan->pstRecevingPdu;
        pstSet->pstChainPduListTail = pstScan->pstRecevingPdu;
    }
    __set_PRIMASK(u32Primask);
    
    pstScan->pstRecevingPdu = NULL;
    pstScan->bSecondaryFragmentTruncated = false;
    pstScan->enumState = LLC_EXT_SCAN_STATE_IDLE;
    return LLP_STATE_CMD_STOP_FRAGMENT;
}

EN_LLP_STATE_CMD_T rom_llc_ext_scan_secondary_scanning_on_llp_rx_end(stLlcExtScan_t* pstScan, stLlpRxEndReport_t *pstReport)
{
    stLlcExtScanAdvertisingSet_t* pstSet = pstScan->pstCurrentSecondaryScanningSet;
    stLlcAdvChannelPdu_t* pstPdu = pstScan->pstRecevingPdu;
    stLlcExtScanPduExtraInfo_t* pstExtraInfo = rom_llc_ext_scan_get_pdu_extra_info(pstPdu);
    if (LLP_RX_RESULT_CRC_OK == pstReport->enumRxResult)
    {
        pstPdu->u32RssiRelatedValue = pstReport->u32RssiRelatedValue;
        if (NULL != pstExtraInfo)
        {
            pstExtraInfo->u64PduStartTimeUs = pstReport->u64RxPacketStartTime;
        }
    }
    if (NULL != pstExtraInfo)
    {
        pstExtraInfo->enumPhy = pstSet->enumSecondaryPhy;
    }
    
    if (LLC_EXT_SCAN_ADVERTISING_SET_STATE_AUX == pstSet->enumSetState)
    {
        if (LLC_EXT_SCAN_STATE_RX_ADV == pstScan->enumState)
        {
            return rom_llc_ext_scan_secondary_scanning_on_llp_rx_adv_end(pstScan, pstSet, pstReport);
        }
        else if (LLC_EXT_SCAN_STATE_RX_RSP == pstScan->enumState)
        {
            return rom_llc_ext_scan_secondary_scanning_on_llp_rx_rsp_end(pstScan, pstSet, pstReport);
        }
        else
        {
            ////unexpected situation
            system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_SECONDARY_SCANNING, __LINE__, (uint32_t)pstSet, (uint32_t)pstScan->enumState);
            return LLP_STATE_CMD_STOP_FRAGMENT;
        }
    }
    else if (LLC_EXT_SCAN_ADVERTISING_SET_STATE_CHAIN == pstSet->enumSetState)
    {
        if (LLC_EXT_SCAN_STATE_RX_ADV == pstScan->enumState)
        {
            return rom_llc_ext_scan_secondary_scanning_on_llp_rx_chain_end(pstScan, pstSet, pstReport);
        }
        else
        {
            ////unexpected situation
            system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_SECONDARY_SCANNING, __LINE__, (uint32_t)pstSet, (uint32_t)pstScan->enumState);
            return LLP_STATE_CMD_STOP_FRAGMENT;
        }
    }
    else
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_SECONDARY_SCANNING, __LINE__, (uint32_t)pstSet, (uint32_t)pstSet->enumSetState);
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
}
