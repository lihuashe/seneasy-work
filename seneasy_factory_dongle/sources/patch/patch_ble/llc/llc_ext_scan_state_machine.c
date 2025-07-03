
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
#include "llc_address.h"
#include "llc_privacy.h"
#include "llc_tool.h"

#include "llc_ext_scan.h"
#include "llc_ext_scan_primary_scanning.h"
#include "llc_ext_scan_secondary_scanning.h"



#include "extended_controller_interface.h"
#include "ble_impl.h"


#include "llc_priority_ext_scan.h"

#include "llc_ext_scan_state_machine.h"

#include "rom_callback.h"


void rom_llc_ext_scan_state_machine_on_scheduler_event_removed (void* pstRoleInstance);
uint32_t rom_llc_ext_scan_state_machine_on_event_blocked (void* pstRoleInstance);
EN_LLP_ERR_T rom_llc_ext_scan_state_machine_config_llp(void* pstRoleInstance, uint32_t u32MaxWorkWindowTimeUs);
void rom_llc_ext_scan_state_machine_on_llp_fragment_start(void* pstRoleInstance);
bool rom_llc_ext_scan_state_machine_on_llp_fragment_stop(void* pstRoleInstance);
EN_LLP_STATE_CMD_T rom_llc_ext_scan_state_machine_on_llp_tx_end(void* pstRoleInstance, stLlpTxEndReport_t *pstReport);
EN_LLP_STATE_CMD_T rom_llc_ext_scan_state_machine_on_llp_rx_end(void* pstRoleInstance, stLlpRxEndReport_t *pstReport);

void rom_llc_ext_scan_state_machine_schedule_next_scanning(stLlcExtScan_t* pstScan);
void rom_llc_ext_scan_state_machine_backoff_init(stLlcExtScan_t* pstScan);



void rom_llc_ext_scan_state_machine_init(stLlcExtScan_t* pstScan)
{
    pstScan->stSchedulerEvent.enumRole = LLC_SCHEDULER_ROLE_EXT_SCAN;
    pstScan->stSchedulerEvent.blockCallback = rom_llc_ext_scan_state_machine_on_event_blocked;
    pstScan->stSchedulerEvent.configCallback = rom_llc_ext_scan_state_machine_config_llp;
    pstScan->stSchedulerEvent.startCallback = rom_llc_ext_scan_state_machine_on_llp_fragment_start;
    pstScan->stSchedulerEvent.stopCallback = rom_llc_ext_scan_state_machine_on_llp_fragment_stop;
    pstScan->stSchedulerEvent.txEndCallback = rom_llc_ext_scan_state_machine_on_llp_tx_end;
    pstScan->stSchedulerEvent.rxEndCallback = rom_llc_ext_scan_state_machine_on_llp_rx_end;
    pstScan->stSchedulerEvent.eventAddedCallback = NULL;
    pstScan->stSchedulerEvent.eventRemovedCallback = rom_llc_ext_scan_state_machine_on_scheduler_event_removed;
    pstScan->stSchedulerEvent.eventPrepareCallback = NULL;
    pstScan->stSchedulerEvent.pstRoleInstance = pstScan;
    
    rom_llc_ext_scan_primary_scanning_clear_phy(pstScan);
    
    
    pstScan->u16ScanReqPduPayloadLen = sizeof(pstScan->pu8ScanReqPduPayload);
    pstScan->stTxPduPtr.pu8Payload = pstScan->pu8ScanReqPduPayload;
    pstScan->stTxPduPtr.pu16PayloadBufSize = &(pstScan->u16ScanReqPduPayloadLen);
    pstScan->stTxPduPtr.pu32Header = &(pstScan->u32ScanReqHeader);
}


void rom_llc_ext_scan_state_machine_update_scan_req_pdu_AdvA(stLlcExtScan_t* pstScan, uint8_t AdvA[6], bool RxAdd)
{
    stLlcSpec51AdvChannelPduHeader_t stHeader;
    stHeader.u16Value = pstScan->u32ScanReqHeader & 0xFFFF;
    stHeader.bitField.RxAdd = RxAdd;
    pstScan->u32ScanReqHeader = stHeader.u16Value;
    memcpy(&pstScan->pu8ScanReqPduPayload[6], AdvA, 6);
}

void rom_llc_ext_scan_state_machine_update_scan_req_pdu_ScanA(stLlcExtScan_t* pstScan, uint8_t ScanA[6], bool TxAdd)
{
    stLlcSpec51AdvChannelPduHeader_t stHeader;
    stHeader.u16Value = pstScan->u32ScanReqHeader & 0xFFFF;
    stHeader.bitField.TxAdd = TxAdd;
    pstScan->u32ScanReqHeader = stHeader.u16Value;
    memcpy(&pstScan->pu8ScanReqPduPayload[0], ScanA, 6);
}

uint8_t rom_llc_ext_scan_state_machine_update_enabled_parameters(stLlcExtScan_t* pstScan)
{
    //address
    switch(pstScan->Own_Address_Type)
    {
        case LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_PUBLIC:
        {
            pstScan->bOwnAddressTxAdd = false;
            pstScan->bOwnAddressResolvable = false;
            rom_llc_address_get_own_public_address(pstScan->pu8OwnAddress);
        }
        break;

        case LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RANDOM:
        {
            if (rom_llc_address_get_own_random_address(pstScan->pu8OwnAddress))
            {
                pstScan->bOwnAddressTxAdd = true;
                pstScan->bOwnAddressResolvable = false;
            }
            else
            {
                return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
            }

        }
        break;

        case LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RESOLVABLE_OR_PUBLIC:
        {
            if (rom_llc_address_get_own_resolvable_private_address(pstScan->pu8OwnAddress))
            {
                pstScan->bOwnAddressTxAdd = true;
                pstScan->bOwnAddressResolvable = true;
                pstScan->u64OwnResolvableAddressUpdatedTime = rom_llc_timer_get_count(true);
            }
            else
            {
                pstScan->bOwnAddressTxAdd = false;
                pstScan->bOwnAddressResolvable = false;
                rom_llc_address_get_own_public_address(pstScan->pu8OwnAddress);
            }
        }
        break;

        case LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RESOLVABLE_OR_RANDOM:
        {
            if (rom_llc_address_get_own_resolvable_private_address(pstScan->pu8OwnAddress))
            {
                pstScan->bOwnAddressTxAdd = true;
                pstScan->bOwnAddressResolvable = true;
                pstScan->u64OwnResolvableAddressUpdatedTime = rom_llc_timer_get_count(true);
            }
            else
            {
                if (rom_llc_address_get_own_random_address(pstScan->pu8OwnAddress))
                {
                    pstScan->bOwnAddressTxAdd = true;
                    pstScan->bOwnAddressResolvable = false;
                }
                else
                {
                    return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
                }
            }
        }
        break;
        default:
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
    }
    
    pstScan->u32DurationUs = pstScan->Duration * 10000;//unit 10ms
    pstScan->u64PeriodUs = ((uint64_t)pstScan->Period) * 1280000; //unit 1.28s
    
    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}

void rom_llc_ext_scan_state_machine_enable(stLlcExtScan_t* pstScan, bool bEnable)
{
    if (!bEnable)
    {
        uint32_t u32Primask = __get_PRIMASK();
        __disable_irq();
        if (!pstScan->bEnabled)
        {
            __set_PRIMASK(u32Primask);
            //already disabled
            rom_controller_return_parameters_HCI_LE_Set_Extended_Scan_Enable(LLC_SPEC_5_1_ERROR_CODE_SUCCESS);
            return;
        }
        pstScan->bDisabledByHost = true;
        __set_PRIMASK(u32Primask);
        
        rom_llc_scheduler_remove_event(&pstScan->stSchedulerEvent);
        return;
    }
    
    
    
    if (pstScan->bEnabled)
    {
        //already enabled
        
        //If the HCI_LE_Set_Extended_Scan_Enable command with Enable set to 0x01
        //is sent while scanning is already enabled, the timers used for duration and
        //period are reset to the new parameter values and a new scan period is started.
        //Any change to the Filter_Duplicates setting or the random address shall take
        //effect.
        
        pstScan->bUpdateEnabledParameters = true;
        return;
    }
    
    uint8_t u8ErrCode = rom_llc_ext_scan_state_machine_update_enabled_parameters(pstScan);
    if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrCode)
    {
        rom_controller_return_parameters_HCI_LE_Set_Extended_Scan_Enable(u8ErrCode);
        return;
    }
    
    //set SCAN_REQ and AUX_SCAN_REQ
    stLlcSpec51AdvChannelPduHeader_t stHeader;
    stHeader.u16Value = 0;
    stHeader.bitField.PDUType = LLC_SPEC_5_1_ADV_CH_PDU_TYPE_SCAN_REQ;//equals LLC_SPEC_5_1_ADV_CH_PDU_TYPE_AUX_SCAN_REQ
    stHeader.bitField.Length = LLC_SPEC_5_1_SCAN_REQ_PAYLOAD_SIZE;//equals LLC_SPEC_5_1_AUX_SCAN_REQ_PAYLOAD_SIZE
    pstScan->u32ScanReqHeader = stHeader.u16Value;
    rom_llc_ext_scan_state_machine_update_scan_req_pdu_ScanA(pstScan, pstScan->pu8OwnAddress, pstScan->bOwnAddressTxAdd);
    
    
    //backoff init
    rom_llc_ext_scan_state_machine_backoff_init(pstScan);
    
    
    rom_llc_ext_scan_primary_scanning_on_enable(pstScan);
    rom_llc_ext_scan_secondary_scanning_on_enable(pstScan);
    
    pstScan->stSchedulerEvent.u64NextEventTimePointUs = 0;
    
    rom_llc_ext_scan_state_machine_schedule_next_scanning(pstScan);
    
    pstScan->bUpdateEnabledParameters = false;
    pstScan->bDisabledByHost = false;
    pstScan->bEnabled = true;
    
    if (!rom_llc_scheduler_add_event(&pstScan->stSchedulerEvent))
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_STATE_MACHINE, __LINE__, (uint32_t)pstScan, 0);
    }
    rom_controller_return_parameters_HCI_LE_Set_Extended_Scan_Enable(LLC_SPEC_5_1_ERROR_CODE_SUCCESS);
}


void rom_llc_ext_scan_state_machine_schedule_next_scanning(stLlcExtScan_t* pstScan)
{
    stLlcExtScanTiming_t stPrimaryScanningTiming;
    stLlcExtScanTiming_t stSecondaryScanningTiming;
    
    bool bIsPrimaryScanning;
    stLlcExtScanTiming_t* pstTiming;
    uint32_t u32MaxWorkTimeUs;
    
    bool bEnabledParametersUpdated = false;
    if (pstScan->bUpdateEnabledParameters)
    {
        pstScan->bUpdateEnabledParameters = false;
        
        uint8_t u8ErrCode = rom_llc_ext_scan_state_machine_update_enabled_parameters(pstScan);
        rom_controller_return_parameters_HCI_LE_Set_Extended_Scan_Enable(u8ErrCode);
        if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS == u8ErrCode)
        {
            bEnabledParametersUpdated = true;
        }
    }
    
    if(pstScan->bOwnAddressResolvable)
    {
        if ( rom_llc_privacy_is_RPA_timeout(&pstScan->u64OwnResolvableAddressUpdatedTime))
        {
            rom_llc_address_get_own_resolvable_private_address(pstScan->pu8OwnAddress);
            
            rom_llc_ext_scan_state_machine_update_scan_req_pdu_ScanA(pstScan, pstScan->pu8OwnAddress, pstScan->bOwnAddressTxAdd);
        }
    }
    
    bool bStart = (0 == pstScan->stSchedulerEvent.u64NextEventTimePointUs);
    
    rom_llc_ext_scan_secondary_scanning_prepare_timing(pstScan);
    rom_llc_ext_scan_primary_scanning_prepare_timing(pstScan, bStart||bEnabledParametersUpdated);
    
    bool bPrimaryTimingExist = rom_llc_ext_scan_primary_scanning_get_timing(pstScan, &stPrimaryScanningTiming);
    bool bSecondaryTimingExist = rom_llc_ext_scan_secondary_scanning_get_timing(pstScan, &stSecondaryScanningTiming);
    
    if (bPrimaryTimingExist && bSecondaryTimingExist)
    {
        //arbitration
        uint32_t u32SecondaryScanningProtectTimeUs = rom_ble_impl_get_protect_time_us_beween_secondary_scanning_and_primary_scanning();
        if (stSecondaryScanningTiming.u64StartTimeUs >= 
            stPrimaryScanningTiming.u64StartTimeUs 
            + LLC_EXT_SCAN_PRIMARY_SCANNING_MIN_WINDOW_US 
            + u32SecondaryScanningProtectTimeUs)
        {
            bIsPrimaryScanning = true;
            pstTiming = &stPrimaryScanningTiming;
            u32MaxWorkTimeUs = stSecondaryScanningTiming.u64StartTimeUs - stPrimaryScanningTiming.u64StartTimeUs - u32SecondaryScanningProtectTimeUs;
            if (u32MaxWorkTimeUs > stPrimaryScanningTiming.u32WorkTimeUs)
            {
                u32MaxWorkTimeUs = stPrimaryScanningTiming.u32WorkTimeUs;
            }
        }
        else
        {
            bIsPrimaryScanning = false;
            pstTiming = &stSecondaryScanningTiming;
            u32MaxWorkTimeUs = stSecondaryScanningTiming.u32WorkTimeUs;
        }
    }
    else if (bPrimaryTimingExist)
    {
        //no secondary scanning
        bIsPrimaryScanning = true;
        pstTiming = &stPrimaryScanningTiming;
        u32MaxWorkTimeUs = stPrimaryScanningTiming.u32WorkTimeUs;
    }
    else if (bSecondaryTimingExist)
    {
        //no primary scanning
        bIsPrimaryScanning = false;
        pstTiming = &stSecondaryScanningTiming;
        u32MaxWorkTimeUs = stSecondaryScanningTiming.u32WorkTimeUs;
    }
    else
    {
        //scanning stopped
        rom_llc_scheduler_remove_event(&pstScan->stSchedulerEvent);
        return;
    }
    
    
    pstScan->bIsScanningPhyPrimary = bIsPrimaryScanning;
    pstScan->u32ScanningPhyMaxWorkTimeUs = u32MaxWorkTimeUs;
    
    pstScan->stSchedulerEvent.u64NextEventTimePointUs = pstTiming->u64StartTimeUs;
    pstScan->stSchedulerEvent.u32NextEventMinWorkTimeUs = pstTiming->u32MinWorkTimeUs;
    
    if (bIsPrimaryScanning)
    {
        rom_llc_ext_scan_primary_scanning_on_scheduled(pstScan);
    }
    else
    {
        rom_llc_ext_scan_secondary_scanning_on_scheduled(pstScan);
    }
    rom_llc_priority_ext_scan_on_scheduled(&pstScan->stSchedulerEvent);
}


void rom_llc_ext_scan_state_machine_on_event_removed_task_process(void* pParam)
{
    stLlcExtScan_t* pstScan = pParam;
    if (NULL == pstScan)
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_STATE_MACHINE, __LINE__, (uint32_t)pstScan, 0);
        return;
    }
    rom_llc_ext_scan_primary_scanning_on_disable(pstScan);
    rom_llc_ext_scan_secondary_scanning_on_disable(pstScan);
    
    if (NULL != pstScan->pstRecevingPdu)
    {
        rom_llc_ext_scan_free_pdu(pstScan, pstScan->pstRecevingPdu);
        pstScan->pstRecevingPdu = NULL;
    }
    
    if (pstScan->bDisabledByHost)
    {
        rom_controller_return_parameters_HCI_LE_Set_Extended_Scan_Enable(LLC_SPEC_5_1_ERROR_CODE_SUCCESS);
    }
    else
    {
        rom_controller_event_HCI_LE_Scan_Timeout();
    }
    
    pstScan->bEnabled = false;
}

void rom_llc_ext_scan_state_machine_on_scheduler_event_removed (void* pstRoleInstance)
{
    if (! rom_llc_send_msg_to_llc_task(rom_llc_ext_scan_state_machine_on_event_removed_task_process, pstRoleInstance))
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_STATE_MACHINE, __LINE__, (uint32_t)pstRoleInstance, 0);
    }
}

uint32_t rom_llc_ext_scan_state_machine_on_event_blocked (void* pstRoleInstance)
{
    stLlcExtScan_t* pstScan = pstRoleInstance;
    if (NULL == pstScan)
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_STATE_MACHINE, __LINE__, (uint32_t)pstScan, 0);
        return LLP_ERR_SYSTEM;
    }
    
    uint32_t u32BlockedCount;
    if (pstScan->bIsScanningPhyPrimary)
    {
        pstScan->bIsPrimaryScanningBlocked = true;
        u32BlockedCount = rom_llc_ext_scan_primary_scanning_on_event_blocked(pstScan);
    }
    else
    {
        pstScan->bIsPrimaryScanningBlocked = false;
        u32BlockedCount = rom_llc_ext_scan_secondary_scanning_on_event_blocked(pstScan);
    }
    
    rom_llc_ext_scan_state_machine_schedule_next_scanning(pstScan);
    
    return u32BlockedCount;
}

EN_LLP_ERR_T rom_llc_ext_scan_state_machine_config_llp(void* pstRoleInstance, uint32_t u32MaxWorkWindowTimeUs)
{
    stLlcExtScan_t* pstScan = pstRoleInstance;
    if (NULL == pstScan)
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_STATE_MACHINE, __LINE__, (uint32_t)pstScan, u32MaxWorkWindowTimeUs);
        return LLP_ERR_SYSTEM;
    }
    
    EN_LLP_ERR_T err;
    if (pstScan->bIsScanningPhyPrimary)
    {
        err = rom_llc_ext_scan_primary_scanning_config_llp(pstScan, LLC_TOOL_MIN(pstScan->u32ScanningPhyMaxWorkTimeUs, u32MaxWorkWindowTimeUs));
    }
    else
    {
        err = rom_llc_ext_scan_secondary_scanning_config_llp(pstScan, LLC_TOOL_MIN(pstScan->u32ScanningPhyMaxWorkTimeUs, u32MaxWorkWindowTimeUs));
    }
    
    switch(err)
    {
        case LLP_ERR_NONE:
            break;
        
        case LLP_ERR_TIME_POINT:
        {
            //proc a fail event
            rom_llc_ext_scan_state_machine_on_llp_fragment_start(pstScan);
            rom_llc_ext_scan_state_machine_on_llp_fragment_stop(pstScan);
        }
            break;
        
        default:
            ////unexpected situation
            system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_STATE_MACHINE, __LINE__, (uint32_t)pstScan, err);
            break;
    }
    
    return err;
}


void rom_llc_ext_scan_state_machine_on_llp_fragment_start(void* pstRoleInstance)
{
    stLlcExtScan_t* pstScan = pstRoleInstance;
    if (NULL == pstScan)
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_STATE_MACHINE, __LINE__, (uint32_t)pstScan, 0);
        return;
    }
    
    if (pstScan->bIsScanningPhyPrimary)
    {
        rom_llc_ext_scan_primary_scanning_on_llp_fragment_start(pstScan);
    }
    else
    {
        rom_llc_ext_scan_secondary_scanning_on_llp_fragment_start(pstScan);
    }
}

bool rom_llc_ext_scan_state_machine_on_llp_fragment_stop(void* pstRoleInstance)
{
    stLlcExtScan_t* pstScan = pstRoleInstance;
    if (NULL == pstScan)
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_STATE_MACHINE, __LINE__, (uint32_t)pstScan, 0);
        return false;
    }
    
    if (pstScan->bIsScanningPhyPrimary)
    {
        return rom_llc_ext_scan_primary_scanning_on_llp_fragment_stop(pstScan);
    }
    else
    {
        return rom_llc_ext_scan_secondary_scanning_on_llp_fragment_stop(pstScan);
    }
}

EN_LLP_STATE_CMD_T rom_llc_ext_scan_state_machine_on_llp_tx_end(void* pstRoleInstance, stLlpTxEndReport_t *pstReport)
{
    stLlcExtScan_t* pstScan = pstRoleInstance;
    if ((NULL == pstScan)||(NULL == pstReport))
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_STATE_MACHINE, __LINE__, (uint32_t)pstScan, (uint32_t)pstReport);
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    if (pstScan->bIsScanningPhyPrimary)
    {
        return rom_llc_ext_scan_primary_scanning_on_llp_tx_end(pstScan, pstReport);
    }
    else
    {
        return rom_llc_ext_scan_secondary_scanning_on_llp_tx_end(pstScan, pstReport);
    }
}

EN_LLP_STATE_CMD_T rom_llc_ext_scan_state_machine_on_llp_rx_end(void* pstRoleInstance, stLlpRxEndReport_t *pstReport)
{
    stLlcExtScan_t* pstScan = pstRoleInstance;
    if ((NULL == pstScan)||(NULL == pstReport))
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_STATE_MACHINE, __LINE__, (uint32_t)pstScan, (uint32_t)pstReport);
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    if (pstScan->bIsScanningPhyPrimary)
    {
        return rom_llc_ext_scan_primary_scanning_on_llp_rx_end(pstScan, pstReport);
    }
    else
    {
        return rom_llc_ext_scan_secondary_scanning_on_llp_rx_end(pstScan, pstReport);
    }
}

void rom_llc_ext_scan_state_machine_backoff_init(stLlcExtScan_t* pstScan)
{
    pstScan->u16BackoffCount = 1;
    pstScan->u16UpperLimit = 1;
    pstScan->u8BackOffConsecutiveSuccessCount = 0;
    pstScan->u8BackOffConsecutiveFailureCount = 0;
}

void rom_llc_ext_scan_state_machine_backoff_scan_result(stLlcExtScan_t* pstScan, bool bSuccess)
{
    if (bSuccess)
    {
        pstScan->u8BackOffConsecutiveSuccessCount++;
        pstScan->u8BackOffConsecutiveFailureCount = 0;
        if (2 == pstScan->u8BackOffConsecutiveSuccessCount)
        {
            pstScan->u8BackOffConsecutiveSuccessCount = 0;
            
            if (1 != pstScan->u16UpperLimit)
            {
                pstScan->u16UpperLimit >>= 1;
            }
        }
    }
    else
    {
        pstScan->u8BackOffConsecutiveSuccessCount = 0;
        pstScan->u8BackOffConsecutiveFailureCount++;
        if (2 == pstScan->u8BackOffConsecutiveFailureCount)
        {
            pstScan->u8BackOffConsecutiveFailureCount = 0;
            if (256 != pstScan->u16UpperLimit)
            {
                pstScan->u16UpperLimit <<= 1;
            }
        }
    }
    uint32_t u32Rand = rom_ble_impl_get_rand();
    pstScan->u16BackoffCount = (u32Rand % pstScan->u16UpperLimit) + 1;
}

bool rom_llc_ext_scan_state_machine_backoff_is_scan_canceled(stLlcExtScan_t* pstScan)
{
    if(0 != pstScan->u16BackoffCount)
    {
        pstScan->u16BackoffCount--;
    }
    
    return (0 != pstScan->u16BackoffCount);
}
