/*
MLOG_ID_RANGE: 0x37E0~0x37EF
MLOG_ID_USED:  0x37E0~0x37E7
*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "RegMXD2670.h"



#include "llc_llp_interface.h"



#include "llc.h"
#include "patch_llc.h"
#include "llc_spec_5_1.h"

#include "llc_scheduler.h"

#include "llc_address.h"
#include "llc_white_list.h"
#include "llc_privacy.h"

#include "llc_adv_channel_pdu.h"
#include "llc_acl_data.h"
#include "llc_data_channel_pdu.h"
#include "llc_data.h"
#include "llc_ctrl_procedure.h"
#include "llc_connection_role.h"
#include "llc_slave.h"

#include "llc_common_extended_advertising_payload_format.h"

#include "llc_tool.h"
#include "ble_impl.h"



#include "llc_ext_adv.h"

#include "llc_ext_adv_pdu_manager.h"
#include "llc_priority_ext_adv.h"

#include "controller_interface.h"

#include "extended_controller_interface.h"

#include "llc_ext_adv_state_machine.h"

#include "mlog.h"
#include "rom_callback.h"


void rom_llc_ext_adv_state_machine_on_scheduler_event_removed (void* pstRoleInstance);
void rom_llc_ext_adv_state_machine_start_first_primary_advertising_event(stLlcExtAdv_t* pstAdv, uint64_t u64StartTime);

uint32_t rom_llc_ext_adv_state_machine_on_event_blocked (void* pstRoleInstance);
EN_LLP_ERR_T rom_llc_ext_adv_state_machine_config_llp(void* pstRoleInstance, uint32_t u32MaxWorkWindowTimeUs);
void rom_llc_ext_adv_state_machine_on_llp_fragment_start(void* pstRoleInstance);
bool rom_llc_ext_adv_state_machine_on_llp_fragment_stop_with_info(stLlcExtAdv_t* pstAdv, bool bIsFailed, bool bLoopProc);
bool rom_llc_ext_adv_state_machine_on_llp_fragment_stop(void* pstRoleInstance);
EN_LLP_STATE_CMD_T rom_llc_ext_adv_state_machine_on_llp_tx_end(void* pstRoleInstance, stLlpTxEndReport_t *pstReport);
EN_LLP_STATE_CMD_T rom_llc_ext_adv_state_machine_on_llp_rx_end(void* pstRoleInstance, stLlpRxEndReport_t *pstReport);


void rom_llc_ext_adv_state_machine_enable(stLlcExtAdv_t* pstAdv, bool bEnable)
{
    if (!bEnable)
    {
        if (!pstAdv->bEnabled)
        {
            //already disabled
            return;
        }
        
        rom_llc_scheduler_remove_event(&pstAdv->stSchedulerEvent);
        
        return;
    }
    
//    If the HCI_LE_Set_Extended_Advertising_Enable command is sent again for
//    an advertising set while that set is enabled, the timer used for the duration and
//    the number of events counter are reset and any change to the random address
//    shall take effect.
    
    
    if (pstAdv->bEnabled)
    {
        //already enabled
        uint32_t u32Primask = __get_PRIMASK();
        __disable_irq();
        pstAdv->bUpdateAcceptedPdusAddress = true;
        pstAdv->u8CurrentExtendedAdvertisingEvents = 0;
        pstAdv->u64AdvStartTimeUs = 0;
        __set_PRIMASK(u32Primask);
        return;
    }
    rom_llc_ext_adv_pdu_manager_update_accepted_pdus_AdvA(pstAdv);
    rom_llc_ext_adv_pdu_manager_update_accepted_pdus_TargetA(pstAdv);
    pstAdv->u8CurrentExtendedAdvertisingEvents = 0;
    pstAdv->u64AdvStartTimeUs = 0;
    
    pstAdv->stSchedulerEvent.enumRole = LLC_SCHEDULER_ROLE_EXT_ADV;
    pstAdv->stSchedulerEvent.blockCallback = rom_llc_ext_adv_state_machine_on_event_blocked;
    pstAdv->stSchedulerEvent.configCallback = rom_llc_ext_adv_state_machine_config_llp;
    pstAdv->stSchedulerEvent.startCallback = rom_llc_ext_adv_state_machine_on_llp_fragment_start;
    pstAdv->stSchedulerEvent.stopCallback = rom_llc_ext_adv_state_machine_on_llp_fragment_stop;
    pstAdv->stSchedulerEvent.txEndCallback = rom_llc_ext_adv_state_machine_on_llp_tx_end;
    pstAdv->stSchedulerEvent.rxEndCallback = rom_llc_ext_adv_state_machine_on_llp_rx_end;
    pstAdv->stSchedulerEvent.eventAddedCallback = NULL;
    pstAdv->stSchedulerEvent.eventRemovedCallback = rom_llc_ext_adv_state_machine_on_scheduler_event_removed;
    pstAdv->stSchedulerEvent.eventPrepareCallback = NULL;
    pstAdv->stSchedulerEvent.pstRoleInstance = pstAdv;
    
    pstAdv->bCreateSlaveAfterFragmentStopped = false;
    pstAdv->bSendScanRequestNotificationAfterFragmentStopped = false;
    pstAdv->bAcceptUpdatingPdus = false;
    pstAdv->bUpdateAcceptedPdusADI = false;
    pstAdv->bUpdateAcceptedPdusAddress = false;
    
    pstAdv->enumState = LLC_EXT_ADV_STATE_IDLE;
    pstAdv->u8CurPrimaryChannelIdx = rom_llc_ext_adv_get_next_primary_adv_channel_idx(0, pstAdv->Primary_Advertising_Channel_Map);
    pstAdv->u8CurSecondaryChannelIdx = rom_llc_ext_adv_get_next_secondary_adv_channel_idx(pstAdv, 0);
    
    rom_llc_priority_ext_adv_on_adv_enabled(pstAdv);
    
    rom_llc_ext_adv_state_machine_start_first_primary_advertising_event(pstAdv, rom_ble_impl_get_ext_adv_schedule_start_time(pstAdv->Advertising_Handle));
    rom_llc_priority_ext_adv_update_priority(pstAdv, true);
    
    if (!rom_llc_scheduler_add_event(&pstAdv->stSchedulerEvent))
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
    }
    
    pstAdv->bEnabled = true;
}


uint8_t rom_llc_ext_adv_state_machine_set_updating_pdus_time(stLlcExtAdv_t* pstAdv)
{
    bool bConnectable = LLC_EXT_ADV_IS_CONNECTABLE(pstAdv->Advertising_Event_Properties);
    bool bScannable = LLC_EXT_ADV_IS_SCANNABLE(pstAdv->Advertising_Event_Properties);
    
    if (bConnectable && bScannable)
    {
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }
    
    
    if ((NULL == pstAdv->pstUpdatingPrimaryAdvertisingPdu)
        || (NULL == pstAdv->pstUpdatingSecondaryAdvertisingPdu))
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }
    
    stLlcSpec51AdvChannelPduHeader_t stHeader = {0};
    stLlcExtAdvPduExtraInfo_t* pstExtraInfo = NULL;
    uint32_t u32NeedTimeUs = 0;

    //ADV_EXT_IND
    stHeader.u16Value = pstAdv->pstUpdatingPrimaryAdvertisingPdu->u32Header & 0xFFFF;
    uint32_t u32AdvExtIndTimeUs = rom_llc_tool_calculate_trx_time_us_by_pdu_size(stHeader.bitField.Length + 2, pstAdv->enumPrimaryPhy);
    
    pstAdv->u32UpdatingPrimaryAdvertisingPduOffsetToEachOther30us = (u32AdvExtIndTimeUs + rom_ble_impl_get_time_us_between_primary_advertising_pdus() + 29) / 30;
    
    uint32_t u32UpdatingAdvExtIndOffsetToAuxAdvInd30us = (u32AdvExtIndTimeUs + rom_ble_impl_get_time_us_between_ADV_EXT_IND_and_AUX_ADV_IND() + 29) / 30;
    
    pstAdv->u32UpdatingPrimaryAdvEventOffsetToSecondaryAdvEvent30us = ((pstAdv->u32UpdatingPrimaryAdvertisingPduOffsetToEachOther30us) * (pstAdv->u8PrimaryAdvertisingPduNum - 1)) + u32UpdatingAdvExtIndOffsetToAuxAdvInd30us;
    
    u32NeedTimeUs += pstAdv->u32UpdatingPrimaryAdvEventOffsetToSecondaryAdvEvent30us*30;
    

    if (bConnectable)
    {
        ////connectable nonscannable
        
        //AUX_ADV_IND
        stHeader.u16Value = pstAdv->pstUpdatingSecondaryAdvertisingPdu->u32Header & 0xFFFF;
        uint32_t u32AuxAdvIndTimeUs = rom_llc_tool_calculate_trx_time_us_by_pdu_size(stHeader.bitField.Length + 2, pstAdv->enumSecondaryPhy);

        //AUX_CONNECT_REQ
        uint32_t u32AuxConnectReqTimeUs = rom_llc_tool_calculate_trx_time_us_by_pdu_size(LLC_SPEC_5_1_AUX_CONNECT_REQ_PAYLOAD_SIZE + 2, pstAdv->enumSecondaryPhy);
        
        u32NeedTimeUs += u32AuxAdvIndTimeUs + LLC_SPEC_5_1_T_IFS_US + u32AuxConnectReqTimeUs + LLC_SPEC_5_1_T_IFS_US;
        
        //AUX_CONNECT_RSP
        stHeader.u16Value = pstAdv->pstUpdatingResponsePdu->u32Header & 0xFFFF;
        uint32_t u32AuxConnectRspTimeUs = rom_llc_tool_calculate_trx_time_us_by_pdu_size(stHeader.bitField.Length + 2, pstAdv->enumSecondaryPhy);
        
        u32NeedTimeUs += u32AuxConnectRspTimeUs;
    }
    else if (bScannable)
    {
        ////nonconnectable scannable
        //AUX_ADV_IND
        stHeader.u16Value = pstAdv->pstUpdatingSecondaryAdvertisingPdu->u32Header & 0xFFFF;
        uint32_t u32AuxAdvIndTimeUs = rom_llc_tool_calculate_trx_time_us_by_pdu_size(stHeader.bitField.Length + 2, pstAdv->enumSecondaryPhy);
        
        //AUX_SCAN_REQ
        uint32_t u32AuxScanReqTimeUs = rom_llc_tool_calculate_trx_time_us_by_pdu_size(LLC_SPEC_5_1_AUX_SCAN_REQ_PAYLOAD_SIZE + 2, pstAdv->enumSecondaryPhy);
        
        u32NeedTimeUs += u32AuxAdvIndTimeUs + LLC_SPEC_5_1_T_IFS_US + u32AuxScanReqTimeUs + LLC_SPEC_5_1_T_IFS_US;
        
        //AUX_SCAN_RSP
        stHeader.u16Value = pstAdv->pstUpdatingResponsePdu->u32Header & 0xFFFF;
        uint32_t u32AuxScanRspTimeUs = rom_llc_tool_calculate_trx_time_us_by_pdu_size(stHeader.bitField.Length + 2, pstAdv->enumSecondaryPhy);
        
        pstExtraInfo = rom_llc_ext_adv_get_pdu_extra_info(pstAdv->pstUpdatingResponsePdu);
        if (NULL == pstExtraInfo)
        {
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }
        if (NULL == pstAdv->pstUpdatingChainPdusHead)
        {
            pstExtraInfo->u32AuxPduOffset30us = 0;
            
            u32NeedTimeUs += u32AuxScanRspTimeUs;
        }
        else
        {
            pstExtraInfo->u32AuxPduOffset30us = (u32AuxScanRspTimeUs + rom_ble_impl_get_time_us_between_AUX_SCAN_RSP_and_AUX_CHAIN_IND() + 29) / 30;
            u32NeedTimeUs += pstExtraInfo->u32AuxPduOffset30us * 30;
            
            pstAdv->u32UpdatingSecondaryAdvEventOffsetToChainTxUs = (u32AuxAdvIndTimeUs + LLC_SPEC_5_1_T_IFS_US + u32AuxScanReqTimeUs + LLC_SPEC_5_1_T_IFS_US + pstExtraInfo->u32AuxPduOffset30us * 30);
        }
    }
    else
    {
        ////nonconnectable nonscannable
        //AUX_ADV_IND
        stHeader.u16Value = pstAdv->pstUpdatingSecondaryAdvertisingPdu->u32Header & 0xFFFF;
        uint32_t u32AuxAdvIndTimeUs = rom_llc_tool_calculate_trx_time_us_by_pdu_size(stHeader.bitField.Length + 2, pstAdv->enumSecondaryPhy);
        
        pstExtraInfo = rom_llc_ext_adv_get_pdu_extra_info(pstAdv->pstUpdatingSecondaryAdvertisingPdu);
        if (NULL == pstExtraInfo)
        {
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }
        if (NULL == pstAdv->pstUpdatingChainPdusHead)
        {
            pstExtraInfo->u32AuxPduOffset30us = 0;
            
            u32NeedTimeUs += u32AuxAdvIndTimeUs;
        }
        else
        {
            pstExtraInfo->u32AuxPduOffset30us = (u32AuxAdvIndTimeUs + rom_ble_impl_get_time_us_between_AUX_ADV_IND_and_AUX_CHAIN_IND() + 29) / 30;
            u32NeedTimeUs += pstExtraInfo->u32AuxPduOffset30us * 30;
            
            pstAdv->u32UpdatingSecondaryAdvEventOffsetToChainTxUs = (pstExtraInfo->u32AuxPduOffset30us * 30);
        }
    }
    
    
    //AUX_CHAIN_IND(if exist)
    stLlcAdvChannelPdu_t* pstPdu = pstAdv->pstUpdatingChainPdusHead;
    while(NULL != pstPdu)
    {
        pstExtraInfo = rom_llc_ext_adv_get_pdu_extra_info(pstPdu);
        if (NULL == pstExtraInfo)
        {
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }
        
        stHeader.u16Value = pstPdu->u32Header & 0xFFFF;
        uint32_t u32AuxChainIndTimeUs = rom_llc_tool_calculate_trx_time_us_by_pdu_size(stHeader.bitField.Length + 2, pstAdv->enumSecondaryPhy);
        
        if (NULL == pstPdu->next)
        {
            pstExtraInfo->u32AuxPduOffset30us = 0;
            u32NeedTimeUs += u32AuxChainIndTimeUs;
        }
        else
        {
            pstExtraInfo->u32AuxPduOffset30us = (u32AuxChainIndTimeUs + rom_ble_impl_get_time_us_between_AUX_CHAIN_INDs() + 29) / 30;
            u32NeedTimeUs += pstExtraInfo->u32AuxPduOffset30us * 30;
        }
        
        
        
        pstPdu = pstPdu->next;
    }

    u32NeedTimeUs += LLC_SPEC_5_1_T_MAFS_US;
    
    if (u32NeedTimeUs > pstAdv->Primary_Advertising_Interval_Max * 625)
    {
        return LLC_SPEC_5_1_ERROR_CODE_PACKET_TOO_LONG;
    }
    
    pstAdv->u32UpdatingPrimaryAdvertisingInterval30us = rom_ble_impl_determine_ext_adv_interval_30us(pstAdv->Advertising_Handle, u32NeedTimeUs, pstAdv->Primary_Advertising_Interval_Min*625, pstAdv->Primary_Advertising_Interval_Max*625);
    
    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}


void rom_llc_ext_adv_state_machine_start_first_primary_advertising_event(stLlcExtAdv_t* pstAdv, uint64_t u64AdvertisingSequenceStartTime)
{
    if (pstAdv->bAcceptUpdatingPdus)
    {
        pstAdv->bAcceptUpdatingPdus = false;
        rom_llc_ext_adv_pdu_manager_accept_updating_pdu(pstAdv);
    }
    
    if (pstAdv->bUpdateAcceptedPdusAddress)
    {
        pstAdv->bUpdateAcceptedPdusAddress = false;
        rom_llc_ext_adv_pdu_manager_update_accepted_pdus_AdvA(pstAdv);
        rom_llc_ext_adv_pdu_manager_update_accepted_pdus_TargetA(pstAdv);
    }
    
    if (pstAdv->bUpdateAcceptedPdusADI)
    {
        pstAdv->bUpdateAcceptedPdusADI = false;
        rom_llc_ext_adv_pdu_manager_update_accepted_pdus_ADI(pstAdv);
    }
    
    if( pstAdv->bOwnAddressResolvable)
    {
        if ( rom_llc_privacy_is_RPA_timeout(&pstAdv->u64OwnResolvableAddressUpdatedTime))
        {
            rom_llc_address_get_own_resolvable_private_address(pstAdv->pu8OwnAddress); //gen new RPA after RPA timeout
            rom_llc_ext_adv_pdu_manager_update_accepted_pdus_AdvA(pstAdv);
        }
    }
    
    
    //reset timeline
    
    pstAdv->enumStateMachine = LLC_EXT_ADV_STATE_MACHINE_PRIMARY_ADV;
    
    
    pstAdv->u8PrimaryAdvertisingPduIdx = 0;
    pstAdv->u8CurPrimaryChannelIdx = rom_llc_ext_adv_get_next_primary_adv_channel_idx(0, pstAdv->Primary_Advertising_Channel_Map);
    
    pstAdv->u64PrimaryAdvertisingEventStartTime = u64AdvertisingSequenceStartTime;
    if (NULL != pstAdv->pstSecondaryAdvertisingPdu)
    {
        uint64_t u32OffsetMin30us = pstAdv->u8ApplySecondaryAdvertisingSkip * pstAdv->u32PrimaryAdvertisingInterval30us;
        uint64_t u32OffsetMax30us = pstAdv->u8ApplySecondaryAdvertisingSkip * (pstAdv->u32PrimaryAdvertisingInterval30us + 333);//advDelay (0~10ms)
        uint64_t u32Offset30us = u32OffsetMin30us + (rom_ble_impl_get_rand() % (u32OffsetMax30us - u32OffsetMin30us + 1));
        
        pstAdv->u64SecondaryAdvertisingEventStartTime = pstAdv->u64PrimaryAdvertisingEventStartTime + u32Offset30us*30 + pstAdv->u32PrimaryAdvEventOffsetToSecondaryAdvEvent30us*30;
        
        pstAdv->u8CurrentSecondaryAdvertisingSkip = 0;
        pstAdv->u8CurSecondaryChannelIdx = rom_llc_ext_adv_get_next_secondary_adv_channel_idx(pstAdv, pstAdv->u8CurSecondaryChannelIdx);
        
        rom_llc_ext_adv_pdu_manager_update_accepted_secondary_pdus_AuxPtr(pstAdv);
        rom_llc_ext_adv_pdu_manager_update_accepted_primary_pdus_AuxPtr(pstAdv, u32Offset30us + pstAdv->u32PrimaryAdvEventOffsetToSecondaryAdvEvent30us);
    }
    
    uint64_t u64NextSchedulerEventStartTime = pstAdv->u64PrimaryAdvertisingEventStartTime;
    EN_LL_PHY_T enumNextPhy = pstAdv->enumPrimaryPhy;
    stLlcAdvChannelPdu_t* pstNextPdu = pstAdv->pstPrimaryAdvertisingPdu;
    
    stLlcSpec51AdvChannelPduHeader_t stHeader = {0};
    stHeader.u16Value = pstNextPdu->u32Header&0xFFFF;
    pstAdv->stSchedulerEvent.u32NextEventMinWorkTimeUs = rom_llc_tool_calculate_trx_time_us_by_pdu_size(stHeader.bitField.Length + 2, enumNextPhy);
    pstAdv->stSchedulerEvent.u64NextEventTimePointUs = u64NextSchedulerEventStartTime;
    
    pstAdv->stTxPduPtr.pu16PayloadBufSize = &(pstNextPdu->u16PayloadBufSize);
    pstAdv->stTxPduPtr.pu32Header = &(pstNextPdu->u32Header);
    pstAdv->stTxPduPtr.pu8Payload = pstNextPdu->pu8Payload;
}


void rom_llc_ext_adv_state_machine_start_secondary_advertising_event(stLlcExtAdv_t* pstAdv)
{
    pstAdv->enumStateMachine = LLC_EXT_ADV_STATE_MACHINE_SECONDARY_ADV;
    
    uint64_t u64NextSchedulerEventStartTime = pstAdv->u64SecondaryAdvertisingEventStartTime;
    EN_LL_PHY_T enumNextPhy = pstAdv->enumSecondaryPhy;
    stLlcAdvChannelPdu_t* pstNextPdu = pstAdv->pstSecondaryAdvertisingPdu;
    
    stLlcSpec51AdvChannelPduHeader_t stHeader = {0};
    stHeader.u16Value = pstNextPdu->u32Header&0xFFFF;
    pstAdv->stSchedulerEvent.u32NextEventMinWorkTimeUs = rom_llc_tool_calculate_trx_time_us_by_pdu_size(stHeader.bitField.Length + 2, enumNextPhy);
    pstAdv->stSchedulerEvent.u64NextEventTimePointUs = u64NextSchedulerEventStartTime;
    
    pstAdv->stTxPduPtr.pu16PayloadBufSize = &(pstNextPdu->u16PayloadBufSize);
    pstAdv->stTxPduPtr.pu32Header = &(pstNextPdu->u32Header);
    pstAdv->stTxPduPtr.pu8Payload = pstNextPdu->pu8Payload;
}


void rom_llc_ext_adv_state_machine_start_secondary_chain_tx(stLlcExtAdv_t* pstAdv)
{
    pstAdv->enumStateMachine = LLC_EXT_ADV_STATE_MACHINE_SECONDARY_CHAIN_TX;
    
    pstAdv->pstCurrentChainPdu = pstAdv->pstChainPdusHead;
    
    uint64_t u64NextSchedulerEventStartTime = pstAdv->u64SecondaryAdvertisingEventStartTime + pstAdv->u32SecondaryAdvEventOffsetToChainTxUs;
    EN_LL_PHY_T enumNextPhy = pstAdv->enumSecondaryPhy;
    stLlcAdvChannelPdu_t* pstNextPdu = pstAdv->pstChainPdusHead;
    
    stLlcSpec51AdvChannelPduHeader_t stHeader = {0};
    stHeader.u16Value = pstNextPdu->u32Header&0xFFFF;
    pstAdv->stSchedulerEvent.u32NextEventMinWorkTimeUs = rom_llc_tool_calculate_trx_time_us_by_pdu_size(stHeader.bitField.Length + 2, enumNextPhy);
    pstAdv->stSchedulerEvent.u64NextEventTimePointUs = u64NextSchedulerEventStartTime;
    
    pstAdv->stTxPduPtr.pu16PayloadBufSize = &(pstNextPdu->u16PayloadBufSize);
    pstAdv->stTxPduPtr.pu32Header = &(pstNextPdu->u32Header);
    pstAdv->stTxPduPtr.pu8Payload = pstNextPdu->pu8Payload;
}


void rom_llc_ext_adv_state_machine_PRIMARY_ADV_on_llp_fragment_stop(stLlcExtAdv_t* pstAdv, bool bIsFailed)
{
    rom_llc_priority_ext_adv_on_primary_scheduler_event_stopped(pstAdv, bIsFailed);
    
    if (pstAdv->u8PrimaryAdvertisingPduIdx + 1 < pstAdv->u8PrimaryAdvertisingPduNum)
    {
        ////Primary Adv Event continue
        
        pstAdv->enumStateMachine = LLC_EXT_ADV_STATE_MACHINE_PRIMARY_ADV;
        
        pstAdv->u8PrimaryAdvertisingPduIdx++;
        pstAdv->u8CurPrimaryChannelIdx = rom_llc_ext_adv_get_next_primary_adv_channel_idx(pstAdv->u8CurPrimaryChannelIdx, pstAdv->Primary_Advertising_Channel_Map);
        
        uint64_t u64NextSchedulerEventStartTime = pstAdv->u64PrimaryAdvertisingEventStartTime + (pstAdv->u32PrimaryAdvertisingPduOffsetToEachOther30us * 30 * pstAdv->u8PrimaryAdvertisingPduIdx);
        
        if (NULL != pstAdv->pstSecondaryAdvertisingPdu)
        {
            rom_llc_ext_adv_pdu_manager_update_accepted_primary_pdus_AuxPtr(pstAdv, (pstAdv->u64SecondaryAdvertisingEventStartTime - u64NextSchedulerEventStartTime)/30);
        }
        
        pstAdv->stSchedulerEvent.u64NextEventTimePointUs = u64NextSchedulerEventStartTime;
        return;
    }
    
    //Primary Adv Event finished
    
    if (pstAdv->u8CurrentSecondaryAdvertisingSkip == pstAdv->u8ApplySecondaryAdvertisingSkip)
    {
        //max skip reached
        if (NULL == pstAdv->pstSecondaryAdvertisingPdu)
        {
            //no secondary pdu
            
            //advDelay 0~10ms
            rom_llc_ext_adv_state_machine_start_first_primary_advertising_event(pstAdv, pstAdv->u64PrimaryAdvertisingEventStartTime + (pstAdv->u32PrimaryAdvertisingInterval30us + (rom_ble_impl_get_rand() % 334)) * 30);
            return;
        }
        
        //secondary pdu exists
        rom_llc_ext_adv_state_machine_start_secondary_advertising_event(pstAdv);
        return;
    }
    
    if (pstAdv->u8CurrentSecondaryAdvertisingSkip + 1 == pstAdv->u8ApplySecondaryAdvertisingSkip)
    {
        //max skip is about to reach
        
        pstAdv->enumStateMachine = LLC_EXT_ADV_STATE_MACHINE_PRIMARY_ADV;
        
        pstAdv->u8CurrentSecondaryAdvertisingSkip++;
        pstAdv->u8PrimaryAdvertisingPduIdx = 0;
        pstAdv->u8CurPrimaryChannelIdx = rom_llc_ext_adv_get_next_primary_adv_channel_idx(0, pstAdv->Primary_Advertising_Channel_Map);
        
        pstAdv->u64PrimaryAdvertisingEventStartTime = pstAdv->u64SecondaryAdvertisingEventStartTime - pstAdv->u32PrimaryAdvEventOffsetToSecondaryAdvEvent30us*30;
        
        uint64_t u64NextSchedulerEventStartTime = pstAdv->u64PrimaryAdvertisingEventStartTime;
        
        if (NULL != pstAdv->pstSecondaryAdvertisingPdu)
        {
            rom_llc_ext_adv_pdu_manager_update_accepted_primary_pdus_AuxPtr(pstAdv, (pstAdv->u64SecondaryAdvertisingEventStartTime - u64NextSchedulerEventStartTime)/30);
        }
        
        pstAdv->stSchedulerEvent.u64NextEventTimePointUs = u64NextSchedulerEventStartTime;
        return;
    }
    
    //determine primary adverting event start time
    
    uint64_t u64LeftTime30Us = (pstAdv->u64SecondaryAdvertisingEventStartTime - pstAdv->u32PrimaryAdvEventOffsetToSecondaryAdvEvent30us*30 - pstAdv->u64PrimaryAdvertisingEventStartTime)/30;
    uint8_t u8LeftEventCount = pstAdv->u8ApplySecondaryAdvertisingSkip - pstAdv->u8CurrentSecondaryAdvertisingSkip;
    
    if ((pstAdv->u32PrimaryAdvertisingInterval30us)*u8LeftEventCount > u64LeftTime30Us)
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
    }
    
    if ((pstAdv->u32PrimaryAdvertisingInterval30us + 333)*u8LeftEventCount < u64LeftTime30Us)
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
    }
    
    uint32_t u32AdvEventOffsetLowerLimit30us = pstAdv->u32PrimaryAdvertisingInterval30us;
    if (u32AdvEventOffsetLowerLimit30us + (u8LeftEventCount - 1)*(pstAdv->u32PrimaryAdvertisingInterval30us + 333) < u64LeftTime30Us)
    {
        u32AdvEventOffsetLowerLimit30us = u64LeftTime30Us - (u8LeftEventCount - 1)*(pstAdv->u32PrimaryAdvertisingInterval30us + 333);
        if (u32AdvEventOffsetLowerLimit30us > pstAdv->u32PrimaryAdvertisingInterval30us + 333)
        {
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
        }
    }
    
    uint32_t u32AdvEventOffsetUpperLimit30us = pstAdv->u32PrimaryAdvertisingInterval30us + 333;
    if (u32AdvEventOffsetUpperLimit30us + (u8LeftEventCount - 1)*(pstAdv->u32PrimaryAdvertisingInterval30us) > u64LeftTime30Us)
    {
        u32AdvEventOffsetUpperLimit30us = u64LeftTime30Us - (u8LeftEventCount - 1)*(pstAdv->u32PrimaryAdvertisingInterval30us);
        if (u32AdvEventOffsetUpperLimit30us < pstAdv->u32PrimaryAdvertisingInterval30us)
        {
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
        }
    }
    
    if (u32AdvEventOffsetLowerLimit30us > u32AdvEventOffsetUpperLimit30us)
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
    }
    
    uint32_t u32RandomAdvEventOffset30us = u32AdvEventOffsetLowerLimit30us + (rom_ble_impl_get_rand()%(u32AdvEventOffsetUpperLimit30us - u32AdvEventOffsetLowerLimit30us + 1));
    
    pstAdv->enumStateMachine = LLC_EXT_ADV_STATE_MACHINE_PRIMARY_ADV;
    pstAdv->u8CurrentSecondaryAdvertisingSkip++;
    pstAdv->u8PrimaryAdvertisingPduIdx = 0;
    pstAdv->u8CurPrimaryChannelIdx = rom_llc_ext_adv_get_next_primary_adv_channel_idx(0, pstAdv->Primary_Advertising_Channel_Map);
    
    pstAdv->u64PrimaryAdvertisingEventStartTime += u32RandomAdvEventOffset30us*30;
    
    uint64_t u64NextSchedulerEventStartTime = pstAdv->u64PrimaryAdvertisingEventStartTime;
    
    if (NULL != pstAdv->pstSecondaryAdvertisingPdu)
    {
        rom_llc_ext_adv_pdu_manager_update_accepted_primary_pdus_AuxPtr(pstAdv, (pstAdv->u64SecondaryAdvertisingEventStartTime - u64NextSchedulerEventStartTime)/30);
    }
    
    pstAdv->stSchedulerEvent.u64NextEventTimePointUs = u64NextSchedulerEventStartTime;
}


void rom_llc_ext_adv_state_machine_SECONDARY_ADV_on_llp_fragment_stop(stLlcExtAdv_t* pstAdv, bool bIsFailed)
{
    if (bIsFailed)
    {
        rom_llc_priority_ext_adv_on_secondary_advertising_event_stopped(pstAdv, true);
        rom_llc_ext_adv_state_machine_start_first_primary_advertising_event(pstAdv, pstAdv->u64PrimaryAdvertisingEventStartTime + (pstAdv->u32PrimaryAdvertisingInterval30us + (rom_ble_impl_get_rand() % 334)) * 30);
        return;
    }
    
    
    bool bConnectable = LLC_EXT_ADV_IS_CONNECTABLE(pstAdv->Advertising_Event_Properties);
    bool bScannable = LLC_EXT_ADV_IS_SCANNABLE(pstAdv->Advertising_Event_Properties);
    
    bool bGoToChainTx = false;
    
    if (bConnectable)
    {
        bGoToChainTx = false;
    }
    else if (bScannable)
    {
        bGoToChainTx = pstAdv->bNeedChainTx;
    }
    else
    {
        bGoToChainTx = NULL != pstAdv->pstChainPdusHead;
    }
    
    if (bGoToChainTx)
    {
        rom_llc_ext_adv_state_machine_start_secondary_chain_tx(pstAdv);
    }
    else
    {
        rom_llc_priority_ext_adv_on_secondary_advertising_event_stopped(pstAdv, false);
        
        //advDelay 0~10ms
        rom_llc_ext_adv_state_machine_start_first_primary_advertising_event(pstAdv, pstAdv->u64PrimaryAdvertisingEventStartTime + (pstAdv->u32PrimaryAdvertisingInterval30us + (rom_ble_impl_get_rand() % 334)) * 30);
    }
}

void rom_llc_ext_adv_state_machine_SECONDARY_CHAIN_TX_on_llp_fragment_stop(stLlcExtAdv_t* pstAdv, bool bIsFailed)
{
    if (bIsFailed)
    {
        rom_llc_priority_ext_adv_on_secondary_advertising_event_stopped(pstAdv, true);
        rom_llc_ext_adv_state_machine_start_first_primary_advertising_event(pstAdv, pstAdv->u64PrimaryAdvertisingEventStartTime + (pstAdv->u32PrimaryAdvertisingInterval30us + (rom_ble_impl_get_rand() % 334)) * 30);
        return;
    }
    
    if (pstAdv->pstCurrentChainPdu == pstAdv->pstChainPdusTail)
    {
        //CHAIN_TX finished
        rom_llc_priority_ext_adv_on_secondary_advertising_event_stopped(pstAdv, false);
        rom_llc_ext_adv_state_machine_start_first_primary_advertising_event(pstAdv, pstAdv->u64PrimaryAdvertisingEventStartTime + (pstAdv->u32PrimaryAdvertisingInterval30us + (rom_ble_impl_get_rand() % 334)) * 30);
        return;
    }
    
    //CHAIN_TX continue
    
    pstAdv->enumStateMachine = LLC_EXT_ADV_STATE_MACHINE_SECONDARY_CHAIN_TX;
    
    stLlcExtAdvPduExtraInfo_t* pstExtraInfo = rom_llc_ext_adv_get_pdu_extra_info(pstAdv->pstCurrentChainPdu);
    if (NULL == pstExtraInfo)
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
    }
    
    pstAdv->pstCurrentChainPdu = pstAdv->pstCurrentChainPdu->next;
    if (NULL == pstAdv->pstCurrentChainPdu)
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
    }
    
    
    uint64_t u64NextSchedulerEventStartTime = pstAdv->stSchedulerEvent.u64NextEventTimePointUs + pstExtraInfo->u32AuxPduOffset30us*30;
    EN_LL_PHY_T enumNextPhy = pstAdv->enumSecondaryPhy;
    stLlcAdvChannelPdu_t* pstNextPdu = pstAdv->pstCurrentChainPdu;
    
    stLlcSpec51AdvChannelPduHeader_t stHeader = {0};
    stHeader.u16Value = pstNextPdu->u32Header&0xFFFF;
    pstAdv->stSchedulerEvent.u32NextEventMinWorkTimeUs = rom_llc_tool_calculate_trx_time_us_by_pdu_size(stHeader.bitField.Length + 2, enumNextPhy);
    pstAdv->stSchedulerEvent.u64NextEventTimePointUs = u64NextSchedulerEventStartTime;
    
    pstAdv->stTxPduPtr.pu16PayloadBufSize = &(pstNextPdu->u16PayloadBufSize);
    pstAdv->stTxPduPtr.pu32Header = &(pstNextPdu->u32Header);
    pstAdv->stTxPduPtr.pu8Payload = pstNextPdu->pu8Payload;
}


uint32_t rom_llc_ext_adv_state_machine_on_event_blocked(void* pstRoleInstance)
{
    stLlcExtAdv_t* pstAdv = (stLlcExtAdv_t*)pstRoleInstance;
    if (NULL == pstAdv)
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
        return 0;
    }
    
    
    
    uint32_t u32BlockedCount = 0;
    uint64_t u64CurTime;
    do
    {
        u32BlockedCount++;
        
        rom_llc_ext_adv_state_machine_on_llp_fragment_start(pstAdv);
        //stop failed
        rom_llc_ext_adv_state_machine_on_llp_fragment_stop_with_info(pstAdv, true, true);
        if (!rom_llc_scheduler_is_event_added(&pstAdv->stSchedulerEvent))
        {
            break;
        }
        
        u64CurTime = rom_llc_timer_get_count(true);
        
    }
    while(u64CurTime >= pstAdv->stSchedulerEvent.u64NextEventTimePointUs);
    
    MLOG32D(0x37e0, "rom_llc_ext_adv_state_machine_on_event_blocked - u32BlockedCount:%u", u32BlockedCount);
    
    return u32BlockedCount;
}


EN_LLP_ERR_T rom_llc_ext_adv_state_machine_config_llp(void* pstRoleInstance, uint32_t u32MaxWorkWindowTimeUs)
{
    stLlcExtAdv_t* pstAdv = (stLlcExtAdv_t*)pstRoleInstance;
    if (NULL == pstAdv)
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
        return LLP_ERR_SYSTEM;
    }

    stLlpExtAdvFragmentParam_t stParam = {0};
    
    switch(pstAdv->enumStateMachine)
    {
        case LLC_EXT_ADV_STATE_MACHINE_PRIMARY_ADV:
            stParam.bIsSecondaryChannel = false;
            stParam.u8ChannelIdx = pstAdv->u8CurPrimaryChannelIdx;
            stParam.enumPhy = pstAdv->enumPrimaryPhy;
            stParam.u16IfsUs = LLC_SPEC_5_1_T_IFS_US;
            stParam.u16IfsRxWindowUs = rom_ble_impl_get_ifs_rx_window_us(pstAdv->enumPrimaryPhy);
            stParam.u32AccessAddress = LLC_SPEC_5_1_ADV_CHANNEL_ACCESS_ADDRESS;
            stParam.u64StartTimeUs = pstAdv->stSchedulerEvent.u64NextEventTimePointUs;
            stParam.u32WorkTimeUs = u32MaxWorkWindowTimeUs;
            stParam.u32CrcInit = LLC_SPEC_5_1_ADV_CHANNEL_CRC_INIT;
            stParam.u16RxPayloadSizeLimit = 0;
            stParam.pstLlpTxPduPtr = &(pstAdv->stTxPduPtr);
            stParam.pstLlpRxPduPtr = &(pstAdv->stRxPduPtr);
            break;
        
        case LLC_EXT_ADV_STATE_MACHINE_SECONDARY_ADV:
            stParam.bIsSecondaryChannel = true;
            stParam.u8ChannelIdx = pstAdv->u8CurSecondaryChannelIdx;
            stParam.enumPhy = pstAdv->enumSecondaryPhy;
            stParam.u16IfsUs = LLC_SPEC_5_1_T_IFS_US;
            stParam.u16IfsRxWindowUs = rom_ble_impl_get_ifs_rx_window_us(pstAdv->enumSecondaryPhy);
            stParam.u32AccessAddress = LLC_SPEC_5_1_ADV_CHANNEL_ACCESS_ADDRESS;
            stParam.u64StartTimeUs = pstAdv->stSchedulerEvent.u64NextEventTimePointUs;
            stParam.u32WorkTimeUs = u32MaxWorkWindowTimeUs;
            stParam.u32CrcInit = LLC_SPEC_5_1_ADV_CHANNEL_CRC_INIT;
            if (LLC_EXT_ADV_IS_CONNECTABLE(pstAdv->Advertising_Event_Properties))
            {
                stParam.u16RxPayloadSizeLimit = LLC_SPEC_5_1_AUX_CONNECT_REQ_PAYLOAD_SIZE;
            }
            else if (LLC_EXT_ADV_IS_SCANNABLE(pstAdv->Advertising_Event_Properties))
            {
                stParam.u16RxPayloadSizeLimit = LLC_SPEC_5_1_AUX_SCAN_REQ_PAYLOAD_SIZE;
            }
            else
            {
                stParam.u16RxPayloadSizeLimit = 0;
            }
            
            stParam.pstLlpTxPduPtr = &(pstAdv->stTxPduPtr);
            stParam.pstLlpRxPduPtr = &(pstAdv->stRxPduPtr);
            break;
        
        case LLC_EXT_ADV_STATE_MACHINE_SECONDARY_CHAIN_TX:
            stParam.bIsSecondaryChannel = true;
            stParam.u8ChannelIdx = pstAdv->u8CurSecondaryChannelIdx;
            stParam.enumPhy = pstAdv->enumSecondaryPhy;
            stParam.u16IfsUs = LLC_SPEC_5_1_T_IFS_US;
            stParam.u16IfsRxWindowUs = rom_ble_impl_get_ifs_rx_window_us(pstAdv->enumSecondaryPhy);
            stParam.u32AccessAddress = LLC_SPEC_5_1_ADV_CHANNEL_ACCESS_ADDRESS;
            stParam.u64StartTimeUs = pstAdv->stSchedulerEvent.u64NextEventTimePointUs;
            stParam.u32WorkTimeUs = u32MaxWorkWindowTimeUs;
            stParam.u32CrcInit = LLC_SPEC_5_1_ADV_CHANNEL_CRC_INIT;
            stParam.u16RxPayloadSizeLimit = 0;
            stParam.pstLlpTxPduPtr = &(pstAdv->stTxPduPtr);
            stParam.pstLlpRxPduPtr = &(pstAdv->stRxPduPtr);
            break;
        
        case LLC_EXT_ADV_STATE_MACHINE_IDLE:
        default:
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
            return LLP_ERR_SYSTEM;
    }
    
    EN_LLP_ERR_T err = rom_llc_start_ext_adv_fragment(&stParam);
    MLOG1688I(0x37e1, "EXT ADV config ch:%u phy:%u priority:%u", stParam.u8ChannelIdx, stParam.enumPhy, pstAdv->stSchedulerEvent.u8Priority);
    if (LLP_ERR_NONE != err)
    {
        //proc fail event
        uint64_t u64CurTime;
        do
        {
            rom_llc_ext_adv_state_machine_on_llp_fragment_start(pstAdv);
            rom_llc_ext_adv_state_machine_on_llp_fragment_stop_with_info(pstAdv, true, true);
            if (!rom_llc_scheduler_is_event_added(&pstAdv->stSchedulerEvent))
            {
                break;
            }
            
            u64CurTime = rom_llc_timer_get_count(true);
            
        }
        while(u64CurTime >= pstAdv->stSchedulerEvent.u64NextEventTimePointUs);
    }
    
    return err;
}


void rom_llc_ext_adv_state_machine_on_llp_fragment_start(void* pstRoleInstance)
{
    stLlcExtAdv_t* pstAdv = (stLlcExtAdv_t*)pstRoleInstance;
    if (NULL == pstAdv)
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
        return;
    }
    
    if (LLC_EXT_ADV_STATE_IDLE != pstAdv->enumState)
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
        return;
    }
    
    switch(pstAdv->enumStateMachine)
    {
        case LLC_EXT_ADV_STATE_MACHINE_PRIMARY_ADV:
            pstAdv->enumState = LLC_EXT_ADV_STATE_PRIMARY_TX_ADV;
            break;
        
        case LLC_EXT_ADV_STATE_MACHINE_SECONDARY_ADV:
            pstAdv->enumState = LLC_EXT_ADV_STATE_SECONDARY_TX_ADV;
            pstAdv->bNeedChainTx = false;
            break;
        
        case LLC_EXT_ADV_STATE_MACHINE_SECONDARY_CHAIN_TX:
            pstAdv->enumState = LLC_EXT_ADV_STATE_SECONDARY_TX_CHAIN;
            break;
        
        case LLC_EXT_ADV_STATE_MACHINE_IDLE:
        default:
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
            return;
    }
    
    
//The Duration[i] parameter indicates the duration for which that advertising set
//is enabled. The duration begins at the start of the first advertising event of this
//advertising set.
    if ((0 == pstAdv->u64AdvStartTimeUs) && (LLC_EXT_ADV_STATE_MACHINE_PRIMARY_ADV == pstAdv->enumStateMachine))
    {
        pstAdv->u64AdvStartTimeUs = pstAdv->stSchedulerEvent.u64NextEventTimePointUs;
    }
    
}

bool rom_llc_ext_adv_state_machine_on_llp_fragment_stop_with_info(stLlcExtAdv_t* pstAdv, bool bIsFailed, bool bLoopProc)
{
    pstAdv->enumState = LLC_EXT_ADV_STATE_IDLE;
    
    if (!bLoopProc)
    {
        MLOGI(0x37e2, "rom_llc_ext_adv_state_machine_on_llp_fragment_stop_with_info - fragment end:%u", pstAdv->enumStateMachine);
    }
    
    if (pstAdv->bSendScanRequestNotificationAfterFragmentStopped)
    {
        pstAdv->bSendScanRequestNotificationAfterFragmentStopped = false;
        
        rom_controller_event_HCI_LE_Scan_Request_Received(pstAdv->Advertising_Handle, pstAdv->Scanner_Address_Type, pstAdv->Scanner_Address);
    }
    
    if (pstAdv->bCreateSlaveAfterFragmentStopped)
    {
        pstAdv->bCreateSlaveAfterFragmentStopped = false;
        
        
        //create slave
        stLlcConnectionRole_t* pstSlave = rom_llc_connection_role_alloc_instance();
        if (NULL == pstSlave)
        {
            //no mem
            MLOG0W(0x37e3, "rom_llc_ext_adv_state_machine_on_llp_fragment_stop_with_info - create slave failed, no mem");
            return false;
        }
        
        stLlcSpec51AdvChannelPduHeader_t stSpec51RxHeader = {0};
        stSpec51RxHeader.u16Value = pstAdv->u32RxPduHeader & 0xFFFF;
        stLlcSpec51AuxConnectReq_t* pstConnectReq = (stLlcSpec51AuxConnectReq_t*)pstAdv->pu8RxPduPayload;
        
        if (0 == pstAdv->u64PacketEndTime)
        {
            pstAdv->u64PacketEndTime = pstAdv->u64TimeOfLastSync + rom_llc_tool_calculate_trx_time_us_by_pdu_size(stSpec51RxHeader.bitField.Length + 2, pstAdv->enumSecondaryPhy);
        }
        
        stLlcSlaveInitParam_t stParam = {0};
        stParam.u64TimeOfLastSync = pstAdv->u64TimeOfLastSync;
        
        uint16_t transmitWindowDelay;
        switch(pstAdv->enumSecondaryPhy)
        {
            case LL_PHY_1M:
            case LL_PHY_2M:
                transmitWindowDelay = LLC_SPEC_5_1_UNCODED_AUX_CONNECT_REQ_TRANSMIT_WINDOW_DELAY_US;
                break;
            
            case LL_PHY_CODED_S2:
            case LL_PHY_CODED_S8:
                transmitWindowDelay = LLC_SPEC_5_1_CODED_AUX_CONNECT_REQ_TRANSMIT_WINDOW_DELAY_US;
                break;
            
            default:
                system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
                break;
        }
        
        stParam.u64TransmitWindowStartTime = pstAdv->u64PacketEndTime + transmitWindowDelay + pstConnectReq->bitField.WinOffset*1250;
        stParam.u16TransmitWindowSizeUs = pstConnectReq->bitField.WinSize*1250;
        
        stParam.u32AccessAddress = pstConnectReq->bitField.AA;
        stParam.u32CrcInit = pstConnectReq->bitField.CRCInit;
        stParam.u16ConnItv1250us = pstConnectReq->bitField.Interval;
        stParam.u16ConnSlaveLatency = pstConnectReq->bitField.Latency;
        stParam.u16ConnSupervisionTimeout10ms = pstConnectReq->bitField.TimeOut;
        stParam.bChSel = true;//Channel Selection Algorithm #2 shall be used
        memcpy(stParam.pu8ChM,pstConnectReq->bitField.ChM,5);
        stParam.u8Hop = pstConnectReq->bitField.Hop;
        stParam.u8Sca = pstConnectReq->bitField.SCA;
        stParam.enumPhy = pstAdv->enumSecondaryPhy;
        stParam.u8SelfAddrType = stSpec51RxHeader.bitField.RxAdd;
        memcpy(stParam.pu8SelfAddr,pstConnectReq->bitField.AdvA,6);
        stParam.u8PeerAddrType = stSpec51RxHeader.bitField.TxAdd;
        memcpy(stParam.pu8PeerAddr,pstConnectReq->bitField.InitA,6);
        stParam.u32RssiRelatedValue = pstAdv->u32RssiRelatedValue;

        stParam.Own_Address_Be_RPA = pstAdv->bOwnAddressResolvable;
        stParam.Peer_RPA_Resoluted = pstAdv->bPeerRPA_Resolved;
        if(stParam.Peer_RPA_Resoluted)
        {
            memcpy(stParam.Peer_Identify_Address,pstAdv->pu8PeerIdentifyAddr,6);
            stParam.Peer_Identify_Address_Type = pstAdv->u8PeerIdentifyAddrType;
        }
        
        if (!rom_llc_slave_init(pstSlave,&stParam))
        {
            MLOG0W(0x37e4, "rom_llc_ext_adv_state_machine_on_llp_fragment_stop_with_info - create slave failed, slave init failed");
            rom_llc_connection_role_free_instance(pstSlave);
            return false;
        }
        
        if (!rom_llc_connection_role_schedule_start(pstSlave))
        {
            MLOG0W(0x37e5, "rom_llc_ext_adv_state_machine_on_llp_fragment_stop_with_info - create slave failed, slave start failed");
            rom_llc_connection_role_free_instance(pstSlave);
        }
        rom_llc_scheduler_remove_event(&pstAdv->stSchedulerEvent);
        
        rom_controller_event_HCI_LE_Advertising_Set_Terminated(LLC_SPEC_5_1_ERROR_CODE_SUCCESS, pstAdv->Advertising_Handle,
            pstSlave->u16ConnHandle, (0 == pstAdv->u8MaxExtendedAdvertisingEvents)?(0):(pstAdv->u8CurrentExtendedAdvertisingEvents));
        
        return false;
    }
    
    
    EN_LLC_EXT_ADV_STATE_MACHINE_T enumPrevStateMachine = pstAdv->enumStateMachine;
    switch(enumPrevStateMachine)
    {
        case LLC_EXT_ADV_STATE_MACHINE_PRIMARY_ADV:
            rom_llc_ext_adv_state_machine_PRIMARY_ADV_on_llp_fragment_stop(pstAdv, bIsFailed);
            break;
        
        case LLC_EXT_ADV_STATE_MACHINE_SECONDARY_ADV:
            rom_llc_ext_adv_state_machine_SECONDARY_ADV_on_llp_fragment_stop(pstAdv, bIsFailed);
            break;
        
        case LLC_EXT_ADV_STATE_MACHINE_SECONDARY_CHAIN_TX:
            rom_llc_ext_adv_state_machine_SECONDARY_CHAIN_TX_on_llp_fragment_stop(pstAdv, bIsFailed);
            break;
        
        case LLC_EXT_ADV_STATE_MACHINE_IDLE:
        default:
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
            return false;
    }

    EN_LLC_EXT_ADV_STATE_MACHINE_T enumNextStateMachine = pstAdv->enumStateMachine;
    
    if ((LLC_EXT_ADV_STATE_MACHINE_PRIMARY_ADV == enumNextStateMachine)
        && ((LLC_EXT_ADV_STATE_MACHINE_SECONDARY_ADV == enumPrevStateMachine)||(LLC_EXT_ADV_STATE_MACHINE_SECONDARY_CHAIN_TX == enumPrevStateMachine)))
    {
        pstAdv->u8CurrentExtendedAdvertisingEvents++;
        if (0 != pstAdv->u8MaxExtendedAdvertisingEvents)
        {
            if (pstAdv->u8CurrentExtendedAdvertisingEvents >= pstAdv->u8MaxExtendedAdvertisingEvents)
            {
                rom_llc_scheduler_remove_event(&pstAdv->stSchedulerEvent);
                
                rom_controller_event_HCI_LE_Advertising_Set_Terminated(LLC_SPEC_5_1_ERROR_CODE_LIMIT_REACHED, pstAdv->Advertising_Handle,
                    0, pstAdv->u8CurrentExtendedAdvertisingEvents);
                
                return false;
            }
        }
    }
    
    
//The Controller should not start an extended advertising event
//that it cannot complete within the duration.
    if ((0 != pstAdv->u32DurationUs) && (0 != pstAdv->u64AdvStartTimeUs))
    {
        if (pstAdv->u64AdvStartTimeUs + pstAdv->u32DurationUs <= pstAdv->stSchedulerEvent.u64NextEventTimePointUs + pstAdv->stSchedulerEvent.u32NextEventMinWorkTimeUs)
        {
            rom_llc_scheduler_remove_event(&pstAdv->stSchedulerEvent);
            
            rom_controller_event_HCI_LE_Advertising_Set_Terminated(LLC_SPEC_5_1_ERROR_CODE_ADVERTISING_TIMEOUT, pstAdv->Advertising_Handle,
                0, (0 == pstAdv->u8MaxExtendedAdvertisingEvents)?(0):(pstAdv->u8CurrentExtendedAdvertisingEvents));
            
            return false;
        }
    }
    
    switch(enumNextStateMachine)
    {
        case LLC_EXT_ADV_STATE_MACHINE_PRIMARY_ADV:
            rom_llc_priority_ext_adv_update_priority(pstAdv, true);
            break;
        
        case LLC_EXT_ADV_STATE_MACHINE_SECONDARY_ADV:
            rom_llc_priority_ext_adv_update_priority(pstAdv, false);
            break;
        
        case LLC_EXT_ADV_STATE_MACHINE_SECONDARY_CHAIN_TX:
            rom_llc_priority_ext_adv_update_priority(pstAdv, false);
            break;
        
        case LLC_EXT_ADV_STATE_MACHINE_IDLE:
        default:
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, enumNextStateMachine);
            return false;
    }
    
    if (!bLoopProc)
    {
        MLOGI(0x37e6, "rom_llc_ext_adv_state_machine_on_llp_fragment_stop_with_info - enumNextStateMachine:%u", enumNextStateMachine);
    }
    return false;
}


bool rom_llc_ext_adv_state_machine_on_llp_fragment_stop(void* pstRoleInstance)
{
    stLlcExtAdv_t* pstAdv = (stLlcExtAdv_t*)pstRoleInstance;
    if (NULL == pstAdv)
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
        return false;
    }
    
    //stop succeed
    return rom_llc_ext_adv_state_machine_on_llp_fragment_stop_with_info(pstAdv, false, false);
}


EN_LLP_STATE_CMD_T rom_llc_ext_adv_state_machine_on_llp_tx_end(void* pstRoleInstance, stLlpTxEndReport_t *pstReport)
{
    stLlcExtAdv_t* pstAdv = (stLlcExtAdv_t*)pstRoleInstance;
    if (NULL == pstAdv)
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    switch(pstAdv->enumStateMachine)
    {
        case LLC_EXT_ADV_STATE_MACHINE_PRIMARY_ADV:
        {
            if (LLC_EXT_ADV_STATE_PRIMARY_TX_ADV != pstAdv->enumState)
            {
                system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
            }
            pstAdv->enumState = LLC_EXT_ADV_STATE_IDLE;
            return LLP_STATE_CMD_STOP_FRAGMENT;
        }
            
        
        case LLC_EXT_ADV_STATE_MACHINE_SECONDARY_ADV:
        {
            if (LLC_EXT_ADV_STATE_SECONDARY_TX_ADV == pstAdv->enumState)
            {
                if ((LLC_EXT_ADV_IS_CONNECTABLE(pstAdv->Advertising_Event_Properties)) || (LLC_EXT_ADV_IS_SCANNABLE(pstAdv->Advertising_Event_Properties)))
                {
                    pstAdv->enumState = LLC_EXT_ADV_STATE_SECONDARY_RX_REQ;
                    return LLP_STATE_CMD_CONTINUE;
                }
                else
                {
                    pstAdv->enumState = LLC_EXT_ADV_STATE_IDLE;
                    return LLP_STATE_CMD_STOP_FRAGMENT;
                }
            }
            else if (LLC_EXT_ADV_STATE_SECONDARY_TX_RSP == pstAdv->enumState)
            {
                pstAdv->enumState = LLC_EXT_ADV_STATE_IDLE;
                return LLP_STATE_CMD_STOP_FRAGMENT;
            }
            else
            {
                system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
                return LLP_STATE_CMD_STOP_FRAGMENT;
            }
        }
        
        case LLC_EXT_ADV_STATE_MACHINE_SECONDARY_CHAIN_TX:
        {
            if (LLC_EXT_ADV_STATE_SECONDARY_TX_CHAIN != pstAdv->enumState)
            {
                system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
            }
            pstAdv->enumState = LLC_EXT_ADV_STATE_IDLE;
            return LLP_STATE_CMD_STOP_FRAGMENT;
        }
        
        case LLC_EXT_ADV_STATE_MACHINE_IDLE:
        default:
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
            return LLP_STATE_CMD_STOP_FRAGMENT;
    }
}


EN_LLP_STATE_CMD_T rom_llc_ext_adv_state_machine_on_llp_rx_end(void* pstRoleInstance, stLlpRxEndReport_t *pstReport)
{
    stLlcExtAdv_t* pstAdv = (stLlcExtAdv_t*)pstRoleInstance;
    if (NULL == pstAdv)
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    if (LLC_EXT_ADV_STATE_MACHINE_SECONDARY_ADV != pstAdv->enumStateMachine)
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    if  (LLC_EXT_ADV_STATE_SECONDARY_RX_REQ != pstAdv->enumState)
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    if  (NULL == pstReport)
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    
    if (LLP_RX_RESULT_CRC_OK != pstReport->enumRxResult)
    {
        pstAdv->enumState = LLC_EXT_ADV_STATE_IDLE;
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    stLlcSpec51AdvChannelPduHeader_t stRxHeader = {0};
    stRxHeader.u16Value = pstAdv->u32RxPduHeader & 0xFFFF;
    
    
    if ((LLC_SPEC_5_1_ADV_CH_PDU_TYPE_AUX_SCAN_REQ == stRxHeader.bitField.PDUType))
    {
        if (LLC_SPEC_5_1_AUX_SCAN_REQ_PAYLOAD_SIZE != stRxHeader.bitField.Length)
        {
            pstAdv->enumState = LLC_EXT_ADV_STATE_IDLE;
            return LLP_STATE_CMD_STOP_FRAGMENT;
        }
        
        if (!LLC_EXT_ADV_IS_SCANNABLE(pstAdv->Advertising_Event_Properties))
        {
            pstAdv->enumState = LLC_EXT_ADV_STATE_IDLE;
            return LLP_STATE_CMD_STOP_FRAGMENT;
        }
        
        stLlcSpec51AuxScanReq_t* pstScanReq = (stLlcSpec51AuxScanReq_t*)pstAdv->pu8RxPduPayload;
    
        pstAdv->stTxPduPtr.pu16PayloadBufSize = &pstAdv->pstResponsePdu->u16PayloadBufSize;
        pstAdv->stTxPduPtr.pu8Payload = pstAdv->pstResponsePdu->pu8Payload;
        pstAdv->stTxPduPtr.pu32Header = &pstAdv->pstResponsePdu->u32Header;
        rom_llc_update_tx_pdu(&pstAdv->stTxPduPtr);
        
        //check addr type
        if (stRxHeader.bitField.RxAdd != pstAdv->bOwnAddressTxAdd)
        {
            pstAdv->enumState = LLC_EXT_ADV_STATE_IDLE;
            return LLP_STATE_CMD_STOP_FRAGMENT;
        }

        //check addr
        if (0 != memcmp(pstAdv->pu8OwnAddress, pstScanReq->bitField.AdvA, 6))
        {
            pstAdv->enumState = LLC_EXT_ADV_STATE_IDLE;
            return LLP_STATE_CMD_STOP_FRAGMENT;
        }
        
        //white list
        if (rom_llc_ext_adv_check_RPA_and_white_list_disallowed_peer_scan(pstAdv, stRxHeader.bitField.TxAdd, pstScanReq->bitField.ScanA,
            &pstAdv->Scanner_Address_Type, pstAdv->Scanner_Address))
        {
            pstAdv->enumState = LLC_EXT_ADV_STATE_IDLE;
            return LLP_STATE_CMD_STOP_FRAGMENT;
        }
        
        if (LLC_SPEC_5_1_HCI_SCAN_REQUEST_NOTIFICATION_ENABLE_ENABLED == pstAdv->Scan_Request_Notification_Enable)
        {
            pstAdv->bSendScanRequestNotificationAfterFragmentStopped = true;
        }
        
        pstAdv->enumState = LLC_EXT_ADV_STATE_SECONDARY_TX_RSP;
        
        if (NULL != pstAdv->pstChainPdusHead)
        {
            pstAdv->bNeedChainTx = true;
        }
        
        return LLP_STATE_CMD_CONTINUE;
    }
    else if ((LLC_SPEC_5_1_ADV_CH_PDU_TYPE_AUX_CONNECT_REQ == stRxHeader.bitField.PDUType)||(LLC_SPEC_5_1_AUX_CONNECT_REQ_PAYLOAD_SIZE != stRxHeader.bitField.Length))
    {
        if (LLC_SPEC_5_1_AUX_CONNECT_REQ_PAYLOAD_SIZE != stRxHeader.bitField.Length)
        {
            pstAdv->enumState = LLC_EXT_ADV_STATE_IDLE;
            return LLP_STATE_CMD_STOP_FRAGMENT;
        }
        
        if (!LLC_EXT_ADV_IS_CONNECTABLE(pstAdv->Advertising_Event_Properties))
        {
            pstAdv->enumState = LLC_EXT_ADV_STATE_IDLE;
            return LLP_STATE_CMD_STOP_FRAGMENT;
        }
        
        stLlcSpec51AuxConnectReq_t* pstConnectReq = (stLlcSpec51AuxConnectReq_t*)pstAdv->pu8RxPduPayload;
    
        //update response
        rom_llc_ext_adv_pdu_manager_update_accepted_connect_response_pdu_TargetA(pstAdv, stRxHeader.bitField.TxAdd, pstConnectReq->bitField.InitA);
        
        pstAdv->stTxPduPtr.pu16PayloadBufSize = &pstAdv->pstResponsePdu->u16PayloadBufSize;
        pstAdv->stTxPduPtr.pu8Payload = pstAdv->pstResponsePdu->pu8Payload;
        pstAdv->stTxPduPtr.pu32Header = &pstAdv->pstResponsePdu->u32Header;
        rom_llc_update_tx_pdu(&pstAdv->stTxPduPtr);
        
        //check addr type
        if (stRxHeader.bitField.RxAdd != pstAdv->bOwnAddressTxAdd)
        {
            pstAdv->enumState = LLC_EXT_ADV_STATE_IDLE;
            return LLP_STATE_CMD_STOP_FRAGMENT;
        }

        //check addr
        if (0 != memcmp(pstAdv->pu8OwnAddress, pstConnectReq->bitField.AdvA, 6))
        {
            pstAdv->enumState = LLC_EXT_ADV_STATE_IDLE;
            return LLP_STATE_CMD_STOP_FRAGMENT;
        }
        
        if (rom_llc_ext_adv_check_RPA_and_white_list_disallowed_peer_connect(pstAdv, stRxHeader.bitField.TxAdd, pstConnectReq->bitField.InitA))
        {
            pstAdv->enumState = LLC_EXT_ADV_STATE_IDLE;
            return LLP_STATE_CMD_STOP_FRAGMENT;
        }
        
        pstAdv->u64PacketEndTime = pstReport->u64RxPacketEndTime;
        pstAdv->u64TimeOfLastSync = pstReport->u64RxPacketStartTime;
        pstAdv->u32RssiRelatedValue = pstReport->u32RssiRelatedValue;
        pstAdv->bCreateSlaveAfterFragmentStopped = true;
        
        MLOG0I(0x37e7, "rom_llc_ext_adv_state_machine_on_llp_rx_end - GO TO SLAVE");
        
        pstAdv->enumState = LLC_EXT_ADV_STATE_SECONDARY_TX_RSP;
        return LLP_STATE_CMD_CONTINUE;
    }
    else
    {
        pstAdv->enumState = LLC_EXT_ADV_STATE_IDLE;
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
}


void rom_llc_ext_adv_state_machine_on_scheduler_event_removed_task_process(void* pParam)
{
    stLlcExtAdv_t* pstAdv = (stLlcExtAdv_t*)pParam;
    if (NULL == pstAdv)
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstAdv, 0);
        return;
    }
    
    pstAdv->bEnabled = false;
}

void rom_llc_ext_adv_state_machine_on_scheduler_event_removed (void* pstRoleInstance)
{
    bool bSendSucceed = rom_llc_send_msg_to_llc_task(rom_llc_ext_adv_state_machine_on_scheduler_event_removed_task_process, pstRoleInstance);
    if (!bSendSucceed)
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_STATE_MACHINE, __LINE__, (uint32_t)pstRoleInstance, 0);
    }
    
}
