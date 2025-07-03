/*
MLOG_ID_RANGE: 0x38A0~0x38FF
MLOG_ID_USED:  0x38A0~0x38B1
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

#include "llc_acl_data.h"
#include "llc_data_channel_pdu.h"
#include "llc_data.h"
#include "llc_ctrl_procedure.h"
#include "llc_connection_role.h"
#include "llc_master.h"



#include "hci.h"



#include "llc_common_extended_advertising_payload_format.h"
#include "llc_ext_initiator.h"
#include "llc_ext_initiator_state_machine.h"
#include "llc_ext_initiator_secondary_scanning.h"

#include "ble_impl.h"

#include "mlog.h"
#include "rom_callback.h"


bool rom_llc_ext_initiator_secondary_scanning_is_advertising_set_collision_with_another(stLlcExtInitiatorAdvertisingSet_t* pstSet1, stLlcExtInitiatorAdvertisingSet_t* pstSet2)
{
    return !((pstSet1->u64StartTimeUs > (pstSet2->u64StartTimeUs + pstSet2->u32WorkTimeUs))
        ||(pstSet2->u64StartTimeUs > (pstSet1->u64StartTimeUs + pstSet1->u32WorkTimeUs)));
}


bool rom_llc_ext_initiator_secondary_scanning_add_advertising_set_to_schedule_list(stLlcExtInitiator_t* pstInitiator, stLlcExtInitiatorAdvertisingSet_t* pstSet)
{
    if (NULL == pstSet)
    {
        return false;
    }

    stLlcExtInitiatorAdvertisingSet_t* pstPrevSet = NULL;
    stLlcExtInitiatorAdvertisingSet_t* pstNextSet = NULL;
    
    uint32_t u32Primask = __get_PRIMASK();
    __disable_irq();
    stLlcExtInitiatorAdvertisingSet_t* pstTmpSet = pstInitiator->pstSecondaryScanningSetScheduleListHead;
    while(NULL != pstTmpSet)
    {
        if (rom_llc_ext_initiator_secondary_scanning_is_advertising_set_collision_with_another(pstSet, pstTmpSet))
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
        pstInitiator->pstSecondaryScanningSetScheduleListHead = pstSet;
        pstInitiator->pstSecondaryScanningSetScheduleListTail = pstSet;
    }
    else if (NULL == pstPrevSet)//and NULL != pstNextSet
    {
        //add to list head
        pstSet->prev = NULL;
        pstSet->next = pstInitiator->pstSecondaryScanningSetScheduleListHead;
        pstInitiator->pstSecondaryScanningSetScheduleListHead->prev = pstSet;
        pstInitiator->pstSecondaryScanningSetScheduleListHead = pstSet;
    }
    else if (NULL == pstNextSet)// and NULL != pstPrevSet
    {
        //add to list tail
        pstSet->prev = pstInitiator->pstSecondaryScanningSetScheduleListTail;
        pstSet->next = NULL;
        pstInitiator->pstSecondaryScanningSetScheduleListTail->next = pstSet;
        pstInitiator->pstSecondaryScanningSetScheduleListTail = pstSet;
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


void rom_llc_ext_initiator_secondary_scanning_remove_advertising_set_from_schedule_list(stLlcExtInitiator_t* pstInitiator, stLlcExtInitiatorAdvertisingSet_t* pstSet)
{
    if (NULL == pstSet)
    {
        return;
    }
    
    uint32_t u32Primask = __get_PRIMASK();
    __disable_irq();
    
    stLlcExtInitiatorAdvertisingSet_t* pstPrevSet = pstSet->prev;
    stLlcExtInitiatorAdvertisingSet_t* pstNextSet = pstSet->next;
    
    if ((NULL == pstPrevSet)&&(NULL == pstNextSet))
    {
        if (pstSet == pstInitiator->pstSecondaryScanningSetScheduleListHead)
        {
            //is Head, is Tail
            pstInitiator->pstSecondaryScanningSetScheduleListHead = NULL;
            pstInitiator->pstSecondaryScanningSetScheduleListTail = NULL;
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
        pstInitiator->pstSecondaryScanningSetScheduleListHead = pstNextSet;
    }
    else if (NULL == pstNextSet)
    {
        //is not Head, is Tail
        pstPrevSet->next = NULL;
        pstInitiator->pstSecondaryScanningSetScheduleListTail = pstPrevSet;
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


stLlcExtInitiatorAdvertisingSet_t* rom_llc_ext_initiator_secondary_scanning_alloc_advertising_set(stLlcExtInitiator_t* pstInitiator)
{
    stLlcExtInitiatorAdvertisingSet_t* pstSet = rom_llc_ext_initiator_alloc_advertising_set();
    
    if (NULL != pstSet)
    {
        pstSet->pstAdvPdu = NULL;
        pstSet->pstRspPdu = NULL;
        pstSet->prev = NULL;
        pstSet->next = NULL;
    }
    
    return pstSet;
}

void rom_llc_ext_initiator_secondary_scanning_free_advertising_set(stLlcExtInitiator_t* pstInitiator, stLlcExtInitiatorAdvertisingSet_t* pstSet)
{
    stLlcAdvChannelPdu_t* pstFreePdu;
    pstFreePdu = pstSet->pstAdvPdu;
    pstSet->pstAdvPdu = NULL;
    if (NULL != pstFreePdu)
    {
        rom_llc_ext_initiator_free_pdu(pstInitiator, pstFreePdu);
    }
    
    pstFreePdu = pstSet->pstRspPdu;
    pstSet->pstRspPdu = NULL;
    if (NULL != pstFreePdu)
    {
        rom_llc_ext_initiator_free_pdu(pstInitiator, pstFreePdu);
    }
    
    pstSet->prev = NULL;
    pstSet->next = NULL;
    
    rom_llc_ext_initiator_free_advertising_set(pstSet);
}


void rom_llc_ext_initiator_secondary_scanning_on_enable(stLlcExtInitiator_t* pstInitiator)
{
    pstInitiator->pstSecondaryScanningSetScheduleListHead = NULL;
    pstInitiator->pstSecondaryScanningSetScheduleListTail = NULL;
    
}

void rom_llc_ext_initiator_secondary_scanning_on_disable(stLlcExtInitiator_t* pstInitiator)
{
    stLlcExtInitiatorAdvertisingSet_t* pstSet;
    
    pstSet = pstInitiator->pstSecondaryScanningSetScheduleListHead;
    while(NULL != pstSet)
    {
        stLlcExtInitiatorAdvertisingSet_t* pstRemoveSet = pstSet;
        pstSet = pstSet->next;
        
        rom_llc_ext_initiator_secondary_scanning_remove_advertising_set_from_schedule_list(pstInitiator, pstRemoveSet);
        rom_llc_ext_initiator_secondary_scanning_free_advertising_set(pstInitiator, pstRemoveSet);
    }
    pstInitiator->pstCurrentSecondaryScanningSet = NULL;
}

void rom_llc_ext_initiator_secondary_scanning_prepare_timing(stLlcExtInitiator_t* pstInitiator)
{
    //do not need to disable irq since it its running in interrupt when sets is not empty
    
    stLlcExtInitiatorAdvertisingSet_t* pstSet = pstInitiator->pstSecondaryScanningSetScheduleListHead;
    pstInitiator->pstCurrentSecondaryScanningSet = NULL;
    uint64_t u64TimeUs = rom_llc_timer_get_count(true);
    while(NULL != pstSet)
    {
        
        if (pstSet->u64StartTimeUs > u64TimeUs)
        {
            pstInitiator->pstCurrentSecondaryScanningSet = pstSet;
            return;
        }
        else
        {
            //remove timeout set
            stLlcExtInitiatorAdvertisingSet_t* pstRemoveSet = pstSet;
            pstSet = pstSet->next;
            
            rom_llc_ext_initiator_secondary_scanning_remove_advertising_set_from_schedule_list(pstInitiator, pstRemoveSet);
            rom_llc_ext_initiator_secondary_scanning_free_advertising_set(pstInitiator, pstRemoveSet);
        }
    }
}


bool rom_llc_ext_initiator_secondary_scanning_get_timing(stLlcExtInitiator_t* pstInitiator, stLlcExtInitiatorTiming_t* pstTiming)
{
    if (NULL == pstInitiator->pstCurrentSecondaryScanningSet)
    {
        return false;
    }
    
    pstTiming->u64StartTimeUs = pstInitiator->pstCurrentSecondaryScanningSet->u64StartTimeUs;
    pstTiming->u32WorkTimeUs = pstInitiator->pstCurrentSecondaryScanningSet->u32WorkTimeUs;
    pstTiming->u32MinWorkTimeUs = pstTiming->u32WorkTimeUs;
    return true;
}




void rom_llc_ext_initiator_secondary_scanning_on_received_extended_pdu_when_primary_scanning(stLlcExtInitiator_t* pstInitiator, stLlcAdvChannelPdu_t* pstPdu)
{
    stLlcExtInitiatorPduExtraInfo_t* pstExtraInfo = rom_llc_ext_initiator_get_pdu_extra_info(pstPdu);
    if (NULL == pstExtraInfo)
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_INITIATOR_SECONDARY_SCANNING, __LINE__, (uint32_t)pstInitiator, (uint32_t)pstPdu);
        return;
    }
    
    if (rom_llc_ext_initiator_is_pdu_empty(pstInitiator, pstPdu))
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
    
    if (! (stFormat.Connectable && (!stFormat.Scannable)))
    {
        return;
    }
    
    //connectable
    
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
    
    stLlcExtInitiatorAdvertisingSet_t *pstSet = rom_llc_ext_initiator_secondary_scanning_alloc_advertising_set(pstInitiator);
    if (NULL == pstSet)
    {
        return;
    }
    
    //set init
    uint64_t u64IdealStartTime = pstExtraInfo->u64PduStartTimeUs + (stFormat.AuxPtr_Offset30Us * 30);
    uint32_t u32OffsetTimeUs = (stFormat.AuxPtr_Offset30Us + stFormat.AuxPtr_OffsetUnits30Us) * 30;
    uint32_t u32WindowWideningUs = (rom_llc_get_llp_clock_accuracy_ppm() + stFormat.AuxPtr_ClockAccuracyPpm) * (u32OffsetTimeUs) / 1000000 + 32;
    pstSet->u64StartTimeUs = u64IdealStartTime - u32WindowWideningUs;
    pstSet->u32AnchorWindowUs = (stFormat.AuxPtr_OffsetUnits30Us * 30) + u32WindowWideningUs * 2;
    pstSet->u32WorkTimeUs = pstSet->u32AnchorWindowUs
            + rom_llc_tool_calculate_trx_time_us_by_pdu_size(2 + LLC_SPEC_5_1_ADV_CHANNEL_PDU_MAX_PAYLOAD_SIZE, stFormat.AuxPtr_Phy) + LLC_SPEC_5_1_T_IFS_US
            + rom_llc_tool_calculate_trx_time_us_by_pdu_size(2 + LLC_SPEC_5_1_AUX_CONNECT_REQ_PAYLOAD_SIZE, stFormat.AuxPtr_Phy) + LLC_SPEC_5_1_T_IFS_US
            + rom_llc_tool_calculate_trx_time_us_by_pdu_size(2 + LLC_SPEC_5_1_AUX_CONNECT_RSP_PAYLOAD_SIZE, stFormat.AuxPtr_Phy) + LLC_SPEC_5_1_T_IFS_US;
    pstSet->enumPrimaryPhy = pstExtraInfo->enumPhy;
    pstSet->enumSecondaryPhy = stFormat.AuxPtr_Phy;
    pstSet->u8ChannelIdx = stFormat.AuxPtr_ChannelIndex;
    pstSet->bDirected = stFormat.TargetA_Present;
    pstSet->enumAddressType = LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_ANONYMOUS;
    pstSet->u16DID = stFormat.ADI_DID;
    pstSet->u8SID = stFormat.ADI_SID;
    
    if (rom_llc_ext_initiator_secondary_scanning_add_advertising_set_to_schedule_list(pstInitiator, pstSet))
    {
        //MLOG1616I(0x38a0, "connectable or scannable advertising set START, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
    }
    else
    {
        rom_llc_ext_initiator_secondary_scanning_free_advertising_set(pstInitiator, pstSet);
    }
}

void rom_llc_ext_initiator_secondary_scanning_on_scheduled(stLlcExtInitiator_t* pstInitiator)
{
    stLlcExtInitiatorAdvertisingSet_t* pstCurrentset = pstInitiator->pstCurrentSecondaryScanningSet;
    EN_LLC_SPEC_5_1_HCI_PHY_T PHY;
    switch(pstCurrentset->enumSecondaryPhy)
    {
        case LL_PHY_1M:
            PHY = LLC_SPEC_5_1_HCI_PHY_LE_1M;
            break;
        
        case LL_PHY_2M:
            PHY = LLC_SPEC_5_1_HCI_PHY_LE_2M;
            break;
        
        case LL_PHY_CODED_S2:
        case LL_PHY_CODED_S8:
            PHY = LLC_SPEC_5_1_HCI_PHY_LE_CODED;
            break;
    }
    
    rom_llc_ext_initiator_state_machine_set_connection_req_pdu_by_phy(pstInitiator, PHY);
    
    
    pstInitiator->pstRecevingPdu = rom_llc_ext_initiator_alloc_pdu(pstInitiator);
    pstInitiator->stRxPduPtr.pu16PayloadBufSize = &(pstInitiator->pstRecevingPdu->u16PayloadBufSize);
    pstInitiator->stRxPduPtr.pu8Payload = pstInitiator->pstRecevingPdu->pu8Payload;
    pstInitiator->stRxPduPtr.pu32Header = &(pstInitiator->pstRecevingPdu->u32Header);
}

uint32_t rom_llc_ext_initiator_secondary_scanning_on_event_blocked (stLlcExtInitiator_t* pstInitiator)
{
    if (NULL != pstInitiator->pstRecevingPdu)
    {
        rom_llc_ext_initiator_free_pdu(pstInitiator, pstInitiator->pstRecevingPdu);
        pstInitiator->pstRecevingPdu = NULL;
    }
    
    uint32_t u32BlockedCount = 1;
    
    stLlcExtInitiatorAdvertisingSet_t* pstSet = pstInitiator->pstSecondaryScanningSetScheduleListHead;
    uint64_t u64TimeUs = rom_llc_timer_get_count(true);
    while(NULL != pstSet)
    {
        
        if (pstSet->u64StartTimeUs > u64TimeUs)
        {
            break;;
        }
        else
        {
            //remove timeout set
            
            stLlcExtInitiatorAdvertisingSet_t* pstRemoveSet = pstSet;
            pstSet = pstSet->next;
            
            rom_llc_ext_initiator_secondary_scanning_remove_advertising_set_from_schedule_list(pstInitiator, pstRemoveSet);
            rom_llc_ext_initiator_secondary_scanning_free_advertising_set(pstInitiator, pstRemoveSet);
            
            u32BlockedCount++;
        }
    }
    
    pstInitiator->pstCurrentSecondaryScanningSet = NULL;
    return u32BlockedCount;
}

EN_LLP_ERR_T rom_llc_ext_initiator_secondary_scanning_config_llp(stLlcExtInitiator_t* pstInitiator, uint32_t u32MaxWorkWindowTimeUs)
{
    stLlcExtInitiatorAdvertisingSet_t* pstCurrentset = pstInitiator->pstCurrentSecondaryScanningSet;
    if (NULL == pstCurrentset)
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_INITIATOR_SECONDARY_SCANNING, __LINE__, (uint32_t)pstInitiator, (uint32_t)0);
        return LLP_ERR_SYSTEM;
    }
    
    stLlpExtInitFragmentParam_t stParam;
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
    stParam.pstLlpTxPduPtr = &(pstInitiator->stTxPduPtr);
    stParam.pstLlpRxPduPtr = &(pstInitiator->stRxPduPtr);
    
    EN_LLP_ERR_T err = rom_llc_start_ext_init_fragment(&stParam);
    
    MLOG1688I(0x38a1, "EXT INIT SECONDARY SCANNING config ch:%u phy:%u priority:%u", stParam.u8ChannelIdx, stParam.enumPhy, pstInitiator->stSchedulerEvent.u8Priority);
    return err;
}

void rom_llc_ext_initiator_secondary_scanning_on_llp_fragment_start(stLlcExtInitiator_t* pstInitiator)
{
    pstInitiator->enumState = LLC_EXT_INITIATOR_STATE_RX_ADV;
    pstInitiator->bSecondaryFragmentTruncated = true;
}


void rom_llc_ext_initiator_secondary_scanning_on_llp_fragment_stop_at_aux_state(stLlcExtInitiator_t* pstInitiator, stLlcExtInitiatorAdvertisingSet_t* pstSet, bool bFragmentTruncated)
{
    //remove set from schedule list
    rom_llc_ext_initiator_secondary_scanning_remove_advertising_set_from_schedule_list(pstInitiator, pstSet);
    
    stLlcAdvChannelPdu_t* pstAdvPdu = pstSet->pstAdvPdu;
    stLlcAdvChannelPdu_t* pstRspPdu = pstSet->pstRspPdu;
    if ((NULL == pstAdvPdu)||(NULL == pstRspPdu))
    {
        MLOG1616I(0x38a2, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        return;
    }
    
    stLlcCommonExtendedAdvertisingPayloadFormat_t stAdvFormat;
    if (!rom_llc_common_extended_advertising_payload_parse(pstAdvPdu, &stAdvFormat))
    {
        MLOG1616I(0x38a3, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        return;
    }
    
    if (stAdvFormat.Header.bitField.Length > pstAdvPdu->u16PayloadBufSize)
    {
        MLOG1616I(0x38a4, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        return;
    }
    
    if (LLC_SPEC_5_1_ADV_CH_PDU_TYPE_AUX_ADV_IND != stAdvFormat.Header.bitField.PDUType)
    {
        MLOG1616I(0x38a5, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        return;
    }
    
    if ((stAdvFormat.Connectable != true)||(stAdvFormat.Scannable != false))
    {
        MLOG1616I(0x38a6, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        return;
    }
    
    if (stAdvFormat.CTEInfo_Present || (!stAdvFormat.ADI_Present) )
    {
        MLOG1616I(0x38a7, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        return;
    }
    
    if ((stAdvFormat.ADI_DID != pstSet->u16DID) || (stAdvFormat.ADI_SID != pstSet->u8SID))
    {
        MLOG1616I(0x38a8, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        return;
    }
    
    //connectable
    if ((!stAdvFormat.AdvA_Present) || stAdvFormat.AuxPtr_Present || stAdvFormat.SyncInfo_Present)
    {
        MLOG1616I(0x38a9, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        return;
    }
    
    pstSet->bDirected = stAdvFormat.TargetA_Present;
    if (rom_llc_ext_initiator_check_RPA_and_white_list_disallowed(pstInitiator,
            stAdvFormat.Header.bitField.TxAdd, stAdvFormat.AdvA_Present?stAdvFormat.AdvA:NULL,
            stAdvFormat.Header.bitField.RxAdd, stAdvFormat.TargetA_Present?stAdvFormat.TargetA:NULL,
            &pstSet->enumAddressType, pstSet->pu8Address,
            &pstSet->enumDirectAddressType, pstSet->pu8DirectAddress))
    {
        MLOG1616I(0x38aa, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        return;
    }
    
    stLlcCommonExtendedAdvertisingPayloadFormat_t stRspFormat;
    if (!rom_llc_common_extended_advertising_payload_parse(pstRspPdu, &stRspFormat))
    {
        MLOG1616I(0x38ab, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        return;
    }
    
    if (stRspFormat.Header.bitField.Length > pstAdvPdu->u16PayloadBufSize)
    {
        MLOG1616I(0x38ac, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        return;
    }
    
    if (LLC_SPEC_5_1_ADV_CH_PDU_TYPE_AUX_CONNECT_RSP != stRspFormat.Header.bitField.PDUType)
    {
        MLOG1616I(0x38ad, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        return;
    }
    
    if ((stRspFormat.Connectable != false)||(stRspFormat.Scannable != false))
    {
        MLOG1616I(0x38ae, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        return;
    }
    
    if ((!stRspFormat.AdvA_Present) || (!stRspFormat.TargetA_Present)
        || stRspFormat.CTEInfo_Present || stRspFormat.ADI_Present 
        || stRspFormat.AuxPtr_Present || stRspFormat.SyncInfo_Present
        || stRspFormat.TxPower_Present || stRspFormat.ACAD_Present || stRspFormat.AdvData_Present)
    {
        MLOG1616I(0x38af, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        return;
    }
    
    if ((stRspFormat.Header.bitField.RxAdd != pstInitiator->bOwnAddressTxAdd)
        ||(0 != memcmp(stRspFormat.TargetA, pstInitiator->pu8OwnAddress, 6)))
    {
        MLOG1616I(0x38b0, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        return;
    }
    
    
    if (rom_llc_ext_initiator_check_RPA_and_white_list_disallowed(pstInitiator,
                                                             stRspFormat.Header.bitField.TxAdd, stRspFormat.AdvA,
                                                             stRspFormat.Header.bitField.RxAdd, stRspFormat.TargetA,
                                                             &pstSet->enumAddressType, pstSet->pu8Address,
                                                             &pstSet->enumDirectAddressType, pstSet->pu8DirectAddress))
    {
        MLOG1616I(0x38b1, "advertising set ERROR, DID:%u SID:%u", pstSet->u16DID, pstSet->u8SID);
        return;
    }
    
    
    
    stLlcExtInitiatorPduExtraInfo_t* pstAdvPduExtraInfo = rom_llc_ext_initiator_get_pdu_extra_info(pstAdvPdu);
    if (NULL == pstAdvPduExtraInfo)
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_INITIATOR_SECONDARY_SCANNING, __LINE__, (uint32_t)pstInitiator, (uint32_t)pstAdvPdu);
        return;
    }
    
    
    stLlcExtInitiatorConnectionParameters_t* pstChosenConnectionParameters = pstInitiator->pstChosenConnectionParameters;
    
    stLlcSpec51AdvChannelPduHeader_t stConnectReqHeader;
    stConnectReqHeader.u16Value = pstChosenConnectionParameters->u32ConnectReqHeader & 0xFFFF;
    
    stLlcSpec51AuxConnectReq_t* pstConnectReq = (stLlcSpec51AuxConnectReq_t*)pstChosenConnectionParameters->pu8ConnectReqPduPayload;
    
    uint64_t u64PktEndTime = pstAdvPduExtraInfo->u64PduEndTimeUs;
    u64PktEndTime += LLC_SPEC_5_1_T_IFS_US + rom_llc_tool_calculate_trx_time_us_by_pdu_size(2 + LLC_SPEC_5_1_AUX_CONNECT_REQ_PAYLOAD_SIZE, pstSet->enumSecondaryPhy);
    switch(pstSet->enumSecondaryPhy)
    {
        case LL_PHY_1M:
        case LL_PHY_2M:
            u64PktEndTime += LLC_SPEC_5_1_UNCODED_AUX_CONNECT_REQ_TRANSMIT_WINDOW_DELAY_US;
            break;
        
        case LL_PHY_CODED_S2:
        case LL_PHY_CODED_S8:
            u64PktEndTime += LLC_SPEC_5_1_CODED_AUX_CONNECT_REQ_TRANSMIT_WINDOW_DELAY_US;
            break;
    }
    
    stLlcMasterInitParam_t* pstParam = &pstInitiator->stMasterInitParam;
    
    pstParam->u64StartTime = u64PktEndTime + (pstConnectReq->bitField.WinOffset*1250) + (pstConnectReq->bitField.WinSize/2)*1250;
    pstParam->u64TimeOfLastSync = pstAdvPduExtraInfo->u64PduStartTimeUs;
    pstParam->u32AccessAddress = pstConnectReq->bitField.AA;
    pstParam->u32CrcInit = pstConnectReq->bitField.CRCInit;
    pstParam->u16ConnItv1250us = pstConnectReq->bitField.Interval;
    pstParam->u16ConnSlaveLatency = pstConnectReq->bitField.Latency;
    pstParam->u16ConnSupervisionTimeout10ms = pstConnectReq->bitField.TimeOut;
    
    pstParam->bChSel = true;
    
    memcpy(pstParam->pu8ChM,pstConnectReq->bitField.ChM,5);
    pstParam->u8Hop = pstConnectReq->bitField.Hop;
    pstParam->u8Sca = pstConnectReq->bitField.SCA;
    pstParam->enumPhy = pstSet->enumSecondaryPhy;
    
    
    pstParam->u8SelfAddrType = pstInitiator->bOwnAddressTxAdd?1:0;
    memcpy(pstParam->pu8SelfAddr,pstInitiator->pu8OwnAddress,6);
    
    // pstParam->u8PeerAddrType = stConnectReqHeader.bitField.RxAdd?1:0;
    // memcpy(pstParam->pu8PeerAddr,pstConnectReq->bitField.AdvA,6);
    pstParam->u8PeerAddrType = stRspFormat.Header.bitField.TxAdd?1:0;
    memcpy(pstParam->pu8PeerAddr,stRspFormat.AdvA,6);
    
    switch(pstSet->enumAddressType)
    {
        case LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_PUBLIC_IDENTITY:
            pstParam->Peer_RPA_Resoluted = 1;
            pstParam->Peer_Identify_Address_Type = 0;
            memcpy(pstParam->Peer_Identify_Address, pstSet->pu8Address,6);
            break;
       
        case LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_RANDOM_IDENTITY:
            pstParam->Peer_RPA_Resoluted = 1;
            pstParam->Peer_Identify_Address_Type = 1;
            memcpy(pstParam->Peer_Identify_Address, pstSet->pu8Address,6);
            break;
        
        default:
            pstParam->Peer_RPA_Resoluted = 0;
            break;
    }
    
    pstParam->Own_Address_Be_RPA = pstInitiator->bOwnAddressResolvable?1:0;
    pstParam->u32RssiRelatedValue = pstRspPdu->u32RssiRelatedValue;
    
    pstInitiator->bCreateConnection = true;
}


bool rom_llc_ext_initiator_secondary_scanning_on_llp_fragment_stop(stLlcExtInitiator_t* pstInitiator)
{
    if (NULL != pstInitiator->pstRecevingPdu)
    {
        rom_llc_ext_initiator_free_pdu(pstInitiator, pstInitiator->pstRecevingPdu);
        pstInitiator->pstRecevingPdu = NULL;
    }
    
    stLlcExtInitiatorAdvertisingSet_t* pstSet = pstInitiator->pstCurrentSecondaryScanningSet;
    if (NULL != pstSet)
    {
        rom_llc_ext_initiator_secondary_scanning_on_llp_fragment_stop_at_aux_state(pstInitiator, pstSet, pstInitiator->bSecondaryFragmentTruncated);
        rom_llc_ext_initiator_secondary_scanning_free_advertising_set(pstInitiator, pstSet);
    }
    
    pstInitiator->pstCurrentSecondaryScanningSet = NULL;
    rom_llc_ext_initiator_state_machine_schedule_next_scanning(pstInitiator);
    return false;
}

EN_LLP_STATE_CMD_T rom_llc_ext_initiator_secondary_scanning_on_llp_tx_end(stLlcExtInitiator_t* pstInitiator, stLlpTxEndReport_t *pstReport)
{
    if (LLC_EXT_INITIATOR_STATE_TX_REQ != pstInitiator->enumState)
    {
        ////unexpected situation
            system_error(SYS_ERR_MODULE_LLC_EXT_INITIATOR_SECONDARY_SCANNING, __LINE__, (uint32_t)pstInitiator, (uint32_t)pstInitiator->enumState);
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    pstInitiator->enumState = LLC_EXT_INITIATOR_STATE_RX_RSP;
    return LLP_STATE_CMD_CONTINUE;
}


EN_LLP_STATE_CMD_T rom_llc_ext_initiator_secondary_scanning_on_llp_rx_adv_end(stLlcExtInitiator_t* pstInitiator, stLlcExtInitiatorAdvertisingSet_t* pstSet, stLlpRxEndReport_t *pstReport)
{
    if (rom_llc_ext_initiator_is_pdu_empty(pstInitiator, pstInitiator->pstRecevingPdu))
    {
        pstInitiator->enumState = LLC_EXT_INITIATOR_STATE_IDLE;
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    if (LLP_RX_RESULT_CRC_OK != pstReport->enumRxResult)
    {
        pstInitiator->enumState = LLC_EXT_INITIATOR_STATE_IDLE;
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    stLlcSpec51AdvChannelPduHeader_t stHeader;
    stHeader.u16Value = pstInitiator->pstRecevingPdu->u32Header & 0xFFFF;
    
    if (stHeader.bitField.Length > pstInitiator->pstRecevingPdu->u16PayloadBufSize)
    {
        pstInitiator->enumState = LLC_EXT_INITIATOR_STATE_IDLE;
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    if (LLC_SPEC_5_1_ADV_CH_PDU_TYPE_AUX_ADV_IND != stHeader.bitField.PDUType)
    {
        pstInitiator->enumState = LLC_EXT_INITIATOR_STATE_IDLE;
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    if (stHeader.bitField.Length < 2)
    {
        pstInitiator->enumState = LLC_EXT_INITIATOR_STATE_IDLE;
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    pstSet->pstAdvPdu = pstInitiator->pstRecevingPdu;
    
    //for better performance, "rom_llc_common_extended_advertising_payload_parse" is not used
    //pstSet->pstAdvPdu format will be re-check after fragment stop
    
    uint8_t Extended_Header_Flags = pstSet->pstAdvPdu->pu8Payload[1];
    uint8_t *AdvA = NULL;
    uint8_t *TargetA = NULL;
    if (Extended_Header_Flags & 1)//AdvA Present
    {
        AdvA = &(pstSet->pstAdvPdu->pu8Payload[2]);
        
        //update SCAN_REQ ASAP
        rom_llc_ext_initiator_state_machine_update_connection_req_pdu_AdvA(pstInitiator, AdvA, stHeader.bitField.TxAdd, false);
        rom_llc_update_tx_pdu(&pstInitiator->stTxPduPtr);
        
        pstInitiator->pstRecevingPdu = rom_llc_ext_initiator_alloc_pdu(pstInitiator);
        pstInitiator->stRxPduPtr.pu16PayloadBufSize = &(pstInitiator->pstRecevingPdu->u16PayloadBufSize);
        pstInitiator->stRxPduPtr.pu8Payload = pstInitiator->pstRecevingPdu->pu8Payload;
        pstInitiator->stRxPduPtr.pu32Header = &(pstInitiator->pstRecevingPdu->u32Header);
        rom_llc_update_rx_pdu(&pstInitiator->stRxPduPtr, LLC_SPEC_5_1_ADV_CHANNEL_PDU_MAX_PAYLOAD_SIZE);
    }
    else
    {
        pstInitiator->pstRecevingPdu = NULL;
        pstInitiator->enumState = LLC_EXT_INITIATOR_STATE_IDLE;
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    if (Extended_Header_Flags & 2)//TagetA Present
    {
        if (stHeader.bitField.Length < 14)
        {
            pstInitiator->enumState = LLC_EXT_INITIATOR_STATE_IDLE;
            return LLP_STATE_CMD_STOP_FRAGMENT;
        }
        TargetA = &(pstSet->pstAdvPdu->pu8Payload[8]);
    }
    
    if (rom_llc_ext_initiator_check_RPA_and_white_list_disallowed(pstInitiator,
        stHeader.bitField.TxAdd, AdvA,
        stHeader.bitField.RxAdd, TargetA,
        NULL, NULL,
        NULL, NULL))
    {
        pstInitiator->enumState = LLC_EXT_INITIATOR_STATE_IDLE;
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    
    pstInitiator->enumState = LLC_EXT_INITIATOR_STATE_TX_REQ;
    return LLP_STATE_CMD_CONTINUE;
}

EN_LLP_STATE_CMD_T rom_llc_ext_initiator_secondary_scanning_on_llp_rx_rsp_end(stLlcExtInitiator_t* pstInitiator, stLlcExtInitiatorAdvertisingSet_t* pstSet, stLlpRxEndReport_t *pstReport)
{
    pstInitiator->bSecondaryFragmentTruncated = false;
    pstInitiator->enumState = LLC_EXT_INITIATOR_STATE_IDLE;
    
    if (rom_llc_ext_initiator_is_pdu_empty(pstInitiator, pstInitiator->pstRecevingPdu))
    {
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    if (LLP_RX_RESULT_CRC_OK != pstReport->enumRxResult)
    {
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    stLlcSpec51AdvChannelPduHeader_t stHeader;
    stHeader.u16Value = pstInitiator->pstRecevingPdu->u32Header & 0xFFFF;
    
    if (stHeader.bitField.Length > pstInitiator->pstRecevingPdu->u16PayloadBufSize)
    {
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    if (LLC_SPEC_5_1_ADV_CH_PDU_TYPE_AUX_CONNECT_RSP != stHeader.bitField.PDUType)
    {
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    pstSet->pstRspPdu = pstInitiator->pstRecevingPdu;
    pstInitiator->pstRecevingPdu = NULL;
    
    return LLP_STATE_CMD_STOP_FRAGMENT;
}


EN_LLP_STATE_CMD_T rom_llc_ext_initiator_secondary_scanning_on_llp_rx_end(stLlcExtInitiator_t* pstInitiator, stLlpRxEndReport_t *pstReport)
{
    stLlcExtInitiatorAdvertisingSet_t* pstSet = pstInitiator->pstCurrentSecondaryScanningSet;
    stLlcAdvChannelPdu_t* pstPdu = pstInitiator->pstRecevingPdu;
    stLlcExtInitiatorPduExtraInfo_t* pstExtraInfo = rom_llc_ext_initiator_get_pdu_extra_info(pstPdu);
    if (LLP_RX_RESULT_CRC_OK == pstReport->enumRxResult)
    {
        pstPdu->u32RssiRelatedValue = pstReport->u32RssiRelatedValue;
        if (NULL != pstExtraInfo)
        {
            pstExtraInfo->u64PduStartTimeUs = pstReport->u64RxPacketStartTime;
            pstExtraInfo->u64PduEndTimeUs = pstReport->u64RxPacketEndTime;
        }
    }
    if (NULL != pstExtraInfo)
    {
        pstExtraInfo->enumPhy = pstSet->enumSecondaryPhy;
    }
    
    if (LLC_EXT_INITIATOR_STATE_RX_ADV == pstInitiator->enumState)
    {
        return rom_llc_ext_initiator_secondary_scanning_on_llp_rx_adv_end(pstInitiator, pstSet, pstReport);
    }
    else if (LLC_EXT_INITIATOR_STATE_RX_RSP == pstInitiator->enumState)
    {
        return rom_llc_ext_initiator_secondary_scanning_on_llp_rx_rsp_end(pstInitiator, pstSet, pstReport);
    }
    else
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_INITIATOR_SECONDARY_SCANNING, __LINE__, (uint32_t)pstSet, (uint32_t)pstInitiator->enumState);
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
}
