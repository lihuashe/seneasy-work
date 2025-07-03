
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>



#include "llc_llp_interface.h"

#include "llc_spec_5_1.h"

#include "llc_adv_channel_pdu.h"
#include "llc_scheduler.h"

#include "llc.h"
#include "llc_address.h"
#include "llc_privacy.h"
#include "llc_tool.h"

#include "llc_acl_data.h"
#include "llc_data_channel_pdu.h"
#include "llc_data.h"
#include "llc_ctrl_procedure.h"
#include "llc_connection_role.h"
#include "llc_master.h"


#include "llc_ext_initiator.h"
#include "llc_ext_initiator_primary_scanning.h"
#include "llc_ext_initiator_secondary_scanning.h"


#include "controller_interface.h"

#include "ble_impl.h"


#include "llc_priority_ext_initiator.h"

#include "llc_ext_initiator_state_machine.h"

#include "rom_callback.h"


void rom_llc_ext_initiator_state_machine_on_scheduler_event_removed (void* pstRoleInstance);
uint32_t rom_llc_ext_initiator_state_machine_on_event_blocked (void* pstRoleInstance);
EN_LLP_ERR_T rom_llc_ext_initiator_state_machine_config_llp(void* pstRoleInstance, uint32_t u32MaxWorkWindowTimeUs);
void rom_llc_ext_initiator_state_machine_on_llp_fragment_start(void* pstRoleInstance);
bool rom_llc_ext_initiator_state_machine_on_llp_fragment_stop(void* pstRoleInstance);
EN_LLP_STATE_CMD_T rom_llc_ext_initiator_state_machine_on_llp_tx_end(void* pstRoleInstance, stLlpTxEndReport_t *pstReport);
EN_LLP_STATE_CMD_T rom_llc_ext_initiator_state_machine_on_llp_rx_end(void* pstRoleInstance, stLlpRxEndReport_t *pstReport);

void rom_llc_ext_initiator_state_machine_schedule_next_scanning(stLlcExtInitiator_t* pstInitiator);



void rom_llc_ext_initiator_state_machine_init(stLlcExtInitiator_t* pstInitiator)
{
    pstInitiator->stSchedulerEvent.enumRole = LLC_SCHEDULER_ROLE_EXT_INIT;
    pstInitiator->stSchedulerEvent.blockCallback = rom_llc_ext_initiator_state_machine_on_event_blocked;
    pstInitiator->stSchedulerEvent.configCallback = rom_llc_ext_initiator_state_machine_config_llp;
    pstInitiator->stSchedulerEvent.startCallback = rom_llc_ext_initiator_state_machine_on_llp_fragment_start;
    pstInitiator->stSchedulerEvent.stopCallback = rom_llc_ext_initiator_state_machine_on_llp_fragment_stop;
    pstInitiator->stSchedulerEvent.txEndCallback = rom_llc_ext_initiator_state_machine_on_llp_tx_end;
    pstInitiator->stSchedulerEvent.rxEndCallback = rom_llc_ext_initiator_state_machine_on_llp_rx_end;
    pstInitiator->stSchedulerEvent.eventAddedCallback = NULL;
    pstInitiator->stSchedulerEvent.eventRemovedCallback = rom_llc_ext_initiator_state_machine_on_scheduler_event_removed;
    pstInitiator->stSchedulerEvent.eventPrepareCallback = NULL;
    pstInitiator->stSchedulerEvent.pstRoleInstance = pstInitiator;
    
    rom_llc_ext_initiator_state_machine_clear_connection_parameters(pstInitiator);
    rom_llc_ext_initiator_primary_scanning_clear_phy(pstInitiator);
}


void rom_llc_ext_initiator_state_machine_clear_connection_parameters(stLlcExtInitiator_t* pstInitiator)
{
    stLlcExtInitiatorConnectionParameters_t* pstConnectionParameters;

    uint32_t i;
    for(i = 0; i < LLC_EXT_INITIATOR_MAX_CONNECTION_PARAMETERS; i++)
    {
        pstConnectionParameters = &(pstInitiator->stConnectionParameters[i]);
        pstConnectionParameters->bConnectionParametersProvided = false;
    }
}

bool rom_llc_ext_initiator_state_machine_add_connection_parameters(stLlcExtInitiator_t* pstInitiator,
    EN_LLC_SPEC_5_1_HCI_PHY_T PHY,
    uint16_t Conn_Interval_Min,
    uint16_t Conn_Interval_Max,
    uint16_t Conn_Latency,
    uint16_t Supervision_Timeout)
{
    stLlcExtInitiatorConnectionParameters_t* pstConnectionParameters;

    uint32_t i;
    for(i = 0; i < LLC_EXT_INITIATOR_MAX_CONNECTION_PARAMETERS; i++)
    {
        pstConnectionParameters = &(pstInitiator->stConnectionParameters[i]);

        if (!pstConnectionParameters->bConnectionParametersProvided)
        {
            pstConnectionParameters->bConnectionParametersProvided = true;
            
            pstConnectionParameters->PHY = PHY;
            pstConnectionParameters->Conn_Interval_Min = Conn_Interval_Min;
            pstConnectionParameters->Conn_Interval_Max = Conn_Interval_Max;
            pstConnectionParameters->Conn_Latency = Conn_Latency;
            pstConnectionParameters->Supervision_Timeout = Supervision_Timeout;
            
            //set CONNECT_IND and AUX_CONNECT_REQ
            
            //payload
            uint32_t u32Rand;
            stLlcSpec51ConnectInd_t* pstConnectInd = (stLlcSpec51ConnectInd_t*)pstConnectionParameters->pu8ConnectReqPduPayload;
            memcpy(pstConnectInd->bitField.InitA, pstInitiator->pu8OwnAddress, 6);
            pstConnectInd->bitField.AA = rom_ble_impl_generate_access_address();
            u32Rand = rom_ble_impl_get_rand();
            pstConnectInd->bitField.CRCInit = u32Rand & 0xFFFFFF;
            uint8_t u8WindowSize;
            uint16_t u16WindowOffset;
            rom_ble_impl_get_connection_window_size_and_window_offset(&u8WindowSize, &u16WindowOffset);
            pstConnectInd->bitField.WinSize = u8WindowSize;
            pstConnectInd->bitField.WinOffset = u16WindowOffset;
            pstConnectInd->bitField.Interval = rom_ble_impl_determine_create_connection_interval(Conn_Interval_Min, Conn_Interval_Max);
            pstConnectInd->bitField.Latency = Conn_Latency;
            pstConnectInd->bitField.TimeOut = Supervision_Timeout;
            rom_ble_impl_get_default_channel_map(pstConnectInd->bitField.ChM);
            u32Rand = rom_ble_impl_get_rand();
            pstConnectInd->bitField.Hop = (u32Rand % 12) + 5; // 5 to 16
            pstConnectInd->bitField.SCA = rom_llc_spec_5_1_get_sca_by_ppm(rom_llc_get_llp_clock_accuracy_ppm());
            
            //header
            stLlcSpec51AdvChannelPduHeader_t stHeader;
            stHeader.u16Value = 0;
            stHeader.bitField.PDUType = LLC_SPEC_5_1_ADV_CH_PDU_TYPE_CONNECT_IND;//equals LLC_SPEC_5_1_ADV_CH_PDU_TYPE_AUX_CONNECT_REQ
            stHeader.bitField.Length = LLC_SPEC_5_1_CONNECT_IND_PAYLOAD_SIZE;//equals LLC_SPEC_5_1_AUX_CONNECT_REQ_PAYLOAD_SIZE
            stHeader.bitField.TxAdd = pstInitiator->bOwnAddressTxAdd;
            pstConnectionParameters->u32ConnectReqHeader = stHeader.u16Value;
            
            //payload size
            pstConnectionParameters->u16ConnectReqPduPayloadLen = sizeof(pstConnectionParameters->pu8ConnectReqPduPayload);
            return true;
        }
    }

    return false;
}

void rom_llc_ext_initiator_state_machine_set_connection_req_pdu_by_phy(stLlcExtInitiator_t* pstInitiator, EN_LLC_SPEC_5_1_HCI_PHY_T PHY)
{
//Where the connection is made on a PHY whose bit is not set in the
//Initiating_PHYs parameter, the Controller shall use the Conn_Interval_Min[i],
//Conn_Interval_Max[i], Conn_Latency[i], Supervision_Timeout[i],
//Minimum_CE_Length[i], and Maximum_CE_Length[i] parameters for an
//implementation-chosen PHY whose bit is set in the Initiating_PHYs parameter
    
    stLlcExtInitiatorConnectionParameters_t* pstChosenConnectionParameters = NULL;
    
    uint32_t i;
    for(i = 0; i < LLC_EXT_INITIATOR_MAX_CONNECTION_PARAMETERS; i++)
    {
        stLlcExtInitiatorConnectionParameters_t* pstConnectionParameters = &(pstInitiator->stConnectionParameters[i]);

        if (pstConnectionParameters->bConnectionParametersProvided)
        {
            if (PHY == pstConnectionParameters->PHY)
            {
                pstChosenConnectionParameters = pstConnectionParameters;
                break;
            }
            else if (NULL == pstChosenConnectionParameters)
            {
                pstChosenConnectionParameters = pstConnectionParameters;
            }
        }
    }
    
    if (NULL == pstChosenConnectionParameters)
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_INITIATOR_STATE_MACHINE, __LINE__, (uint32_t)pstInitiator, PHY);
        return;
    }
    
    pstInitiator->pstChosenConnectionParameters = pstChosenConnectionParameters;
    
    pstInitiator->stTxPduPtr.pu32Header = &pstChosenConnectionParameters->u32ConnectReqHeader;
    pstInitiator->stTxPduPtr.pu8Payload = pstChosenConnectionParameters->pu8ConnectReqPduPayload;
    pstInitiator->stTxPduPtr.pu16PayloadBufSize = &pstChosenConnectionParameters->u16ConnectReqPduPayloadLen;
}

void rom_llc_ext_initiator_state_machine_update_connection_req_pdu_AdvA(stLlcExtInitiator_t* pstInitiator, uint8_t AdvA[6], bool RxAdd, bool bChSel)
{
    stLlcSpec51AdvChannelPduHeader_t stHeader;
    stHeader.u16Value = pstInitiator->pstChosenConnectionParameters->u32ConnectReqHeader & 0xFFFF;
    stHeader.bitField.RxAdd = RxAdd;
    stHeader.bitField.ChSel = rom_ble_impl_is_channel_selection_algorithm_no_2_supported() && bChSel;
    pstInitiator->pstChosenConnectionParameters->u32ConnectReqHeader = stHeader.u16Value;
    
    memcpy(&pstInitiator->pstChosenConnectionParameters->pu8ConnectReqPduPayload[6], AdvA, 6);
}

void rom_llc_ext_initiator_state_machine_update_connection_req_pdu_InitA(stLlcExtInitiator_t* pstInitiator, uint8_t InitA[6], bool TxAdd)
{
    stLlcExtInitiatorConnectionParameters_t* pstConnectionParameters;

    uint32_t i;
    for(i = 0; i < LLC_EXT_INITIATOR_MAX_CONNECTION_PARAMETERS; i++)
    {
        pstConnectionParameters = &(pstInitiator->stConnectionParameters[i]);

        if (pstConnectionParameters->bConnectionParametersProvided)
        {
            //payload
            stLlcSpec51ConnectInd_t* pstConnectInd = (stLlcSpec51ConnectInd_t*)pstConnectionParameters->pu8ConnectReqPduPayload;
            memcpy(pstConnectInd->bitField.InitA, pstInitiator->pu8OwnAddress, 6);
            
            //header
            stLlcSpec51AdvChannelPduHeader_t stHeader;
            stHeader.u16Value = pstConnectionParameters->u32ConnectReqHeader;
            stHeader.bitField.TxAdd = pstInitiator->bOwnAddressTxAdd;
            pstConnectionParameters->u32ConnectReqHeader = stHeader.u16Value;
        }
    }
}

uint8_t rom_llc_ext_initiator_state_machine_enable(stLlcExtInitiator_t* pstInitiator, bool bEnable)
{
    if (!bEnable)
    {
        if (!pstInitiator->bEnabled)
        {
            //already disabled
            return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
        }
        
        rom_llc_scheduler_remove_event(&pstInitiator->stSchedulerEvent);
        return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
    }
    
    if (pstInitiator->bEnabled)
    {
        //already enabled
        return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
    }
    
    stLlcConnectionRole_t* pstConnectionRole = rom_llc_connection_role_alloc_instance();
    if (NULL == pstConnectionRole)
    {
        return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
    }
    pstInitiator->pstConnectionRole = pstConnectionRole;
    
    pstInitiator->pstChosenConnectionParameters = NULL;
    
    rom_llc_ext_initiator_primary_scanning_on_enable(pstInitiator);
    rom_llc_ext_initiator_secondary_scanning_on_enable(pstInitiator);
    
    pstInitiator->stSchedulerEvent.u64NextEventTimePointUs = 0;
    
    rom_llc_ext_initiator_state_machine_schedule_next_scanning(pstInitiator);
    
    pstInitiator->bCreateConnection = false;
    pstInitiator->bEnabled = true;
    
    if (!rom_llc_scheduler_add_event(&pstInitiator->stSchedulerEvent))
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_INITIATOR_STATE_MACHINE, __LINE__, (uint32_t)pstInitiator, 0);
    }
    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}


void rom_llc_ext_initiator_state_machine_schedule_next_scanning(stLlcExtInitiator_t* pstInitiator)
{
    stLlcExtInitiatorTiming_t stPrimaryScanningTiming;
    stLlcExtInitiatorTiming_t stSecondaryScanningTiming;
    
    bool bIsPrimaryScanning;
    stLlcExtInitiatorTiming_t* pstTiming;
    uint32_t u32MaxWorkTimeUs;
    
    if (pstInitiator->bCreateConnection)
    {
        pstInitiator->bCreateConnection = false;
        
        if(!rom_llc_master_init(pstInitiator->pstConnectionRole,&pstInitiator->stMasterInitParam))
        {
            //unexpected situation
            system_error(SYS_ERR_MODULE_LLC_EXT_INITIATOR_STATE_MACHINE, __LINE__, (uint32_t)&pstInitiator->stMasterInitParam, (uint32_t)pstInitiator);
            return;
        }
        
        if (!rom_llc_connection_role_schedule_start(pstInitiator->pstConnectionRole))
        {
            //unexpected situation
            system_error(SYS_ERR_MODULE_LLC_EXT_INITIATOR_STATE_MACHINE, __LINE__, (uint32_t)pstInitiator->pstConnectionRole, (uint32_t)pstInitiator);
            return;
        }
        
        //connection create success, set to NULL to avoid it being free after initiator stopped
        pstInitiator->pstConnectionRole = NULL;
        
        //stop initiator
        rom_llc_scheduler_remove_event(&pstInitiator->stSchedulerEvent);
        return;
    }
    
    if (pstInitiator->bOwnAddressResolvable)
    {
        if ( rom_llc_privacy_is_RPA_timeout(&pstInitiator->u64OwnResolvableAddressUpdatedTime))
        {
            rom_llc_address_get_own_resolvable_private_address(pstInitiator->pu8OwnAddress);
            
            rom_llc_ext_initiator_state_machine_update_connection_req_pdu_InitA(pstInitiator, pstInitiator->pu8OwnAddress, pstInitiator->bOwnAddressTxAdd);
        }
    }
    
    bool bStart = (0 == pstInitiator->stSchedulerEvent.u64NextEventTimePointUs);
    
    rom_llc_ext_initiator_secondary_scanning_prepare_timing(pstInitiator);
    rom_llc_ext_initiator_primary_scanning_prepare_timing(pstInitiator, bStart);
    
    bool bPrimaryTimingExist = rom_llc_ext_initiator_primary_scanning_get_timing(pstInitiator, &stPrimaryScanningTiming);
    bool bSecondaryTimingExist = rom_llc_ext_initiator_secondary_scanning_get_timing(pstInitiator, &stSecondaryScanningTiming);
    
    if (bPrimaryTimingExist && bSecondaryTimingExist)
    {
        //arbitration
        uint32_t u32SecondaryScanningProtectTimeUs = rom_ble_impl_get_protect_time_us_beween_secondary_scanning_and_primary_scanning();
        if (stSecondaryScanningTiming.u64StartTimeUs >= 
            stPrimaryScanningTiming.u64StartTimeUs 
            + LLC_EXT_INITIATOR_PRIMARY_SCANNING_MIN_WINDOW_US 
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
        rom_llc_scheduler_remove_event(&pstInitiator->stSchedulerEvent);
        return;
    }
    
    
    pstInitiator->bIsScanningPhyPrimary = bIsPrimaryScanning;
    pstInitiator->u32ScanningPhyMaxWorkTimeUs = u32MaxWorkTimeUs;
    
    pstInitiator->stSchedulerEvent.u64NextEventTimePointUs = pstTiming->u64StartTimeUs;
    pstInitiator->stSchedulerEvent.u32NextEventMinWorkTimeUs = pstTiming->u32MinWorkTimeUs;
    
    if (bIsPrimaryScanning)
    {
        rom_llc_ext_initiator_primary_scanning_on_scheduled(pstInitiator);
    }
    else
    {
        rom_llc_ext_initiator_secondary_scanning_on_scheduled(pstInitiator);
    }
}


void rom_llc_ext_initiator_state_machine_on_event_removed_task_process(void* pParam)
{
    stLlcExtInitiator_t* pstInitiator = pParam;
    if (NULL == pstInitiator)
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_INITIATOR_STATE_MACHINE, __LINE__, (uint32_t)pstInitiator, 0);
        return;
    }
    rom_llc_ext_initiator_primary_scanning_on_disable(pstInitiator);
    rom_llc_ext_initiator_secondary_scanning_on_disable(pstInitiator);
    
    if (NULL != pstInitiator->pstRecevingPdu)
    {
        rom_llc_ext_initiator_free_pdu(pstInitiator, pstInitiator->pstRecevingPdu);
        pstInitiator->pstRecevingPdu = NULL;
    }
    
    if (NULL != pstInitiator->pstConnectionRole)
    {
        //if connection not created(connection cancelled), free connection role and send 
        //LE Connection Complete with error
        
        rom_llc_connection_role_free_instance(pstInitiator->pstConnectionRole);
        pstInitiator->pstConnectionRole = NULL;
        
        uint8_t pu8ZeroAddress[6];
        memset(pu8ZeroAddress, 0,6);
        
        rom_controller_event_VENDOR_Connection_Complete(LLC_SPEC_5_1_ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, //Status
            0, //Handle
            LLC_SPEC_5_1_HCI_ROLE_MASTER, //Role
            pstInitiator->Peer_Address_Type,//Peer_Address_Type
            pstInitiator->Peer_Address,//Peer_Address
            0,//Conn_Interval
            0,//Conn_Latency
            0,//Supervision_Timeout
            0,//Master_Clock_Accuracy
            pstInitiator->Own_Address_Type,//Own_Address_Type
            pstInitiator->pu8OwnAddress,//Own_Address
            pstInitiator->bOwnAddressResolvable,//Own_Address_Be_RPA
            0,//Peer_RPA_Resoluted
            0,//Peer_Identify_Address_Type
            pu8ZeroAddress);//Peer_Identify_Address
                            
        rom_controller_event_HCI_LE_Enhanced_Connection_Complete(LLC_SPEC_5_1_ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER,//Status
            0,//Connection_Handle
            LLC_SPEC_5_1_HCI_ROLE_MASTER, //Role
            pstInitiator->Peer_Address_Type,//Peer_Address_Type
            pstInitiator->Peer_Address,//Peer_Address
            pstInitiator->bOwnAddressResolvable?pstInitiator->pu8OwnAddress:pu8ZeroAddress,//Local_Resolvable_Private_Address
            pu8ZeroAddress,//Peer_Resolvable_Private_Address
            0,//Conn_Interval
            0,//Conn_Latency
            0,//Supervision_Timeout
            0);//Master_Clock_Accuracy
    }
    
    pstInitiator->bEnabled = false;
}

void rom_llc_ext_initiator_state_machine_on_scheduler_event_removed (void* pstRoleInstance)
{
    if (! rom_llc_send_msg_to_llc_task(rom_llc_ext_initiator_state_machine_on_event_removed_task_process, pstRoleInstance))
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_INITIATOR_STATE_MACHINE, __LINE__, (uint32_t)pstRoleInstance, 0);
    }
}

uint32_t rom_llc_ext_initiator_state_machine_on_event_blocked (void* pstRoleInstance)
{
    stLlcExtInitiator_t* pstInitiator = pstRoleInstance;
    if (NULL == pstInitiator)
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_INITIATOR_STATE_MACHINE, __LINE__, (uint32_t)pstInitiator, 0);
        return LLP_ERR_SYSTEM;
    }
    
    uint32_t u32BlockedCount;
    if (pstInitiator->bIsScanningPhyPrimary)
    {
        u32BlockedCount = rom_llc_ext_initiator_primary_scanning_on_event_blocked(pstInitiator);
    }
    else
    {
        u32BlockedCount = rom_llc_ext_initiator_secondary_scanning_on_event_blocked(pstInitiator);
    }
    
    rom_llc_ext_initiator_state_machine_schedule_next_scanning(pstInitiator);
    return u32BlockedCount;
}

EN_LLP_ERR_T rom_llc_ext_initiator_state_machine_config_llp(void* pstRoleInstance, uint32_t u32MaxWorkWindowTimeUs)
{
    stLlcExtInitiator_t* pstInitiator = pstRoleInstance;
    if (NULL == pstInitiator)
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_INITIATOR_STATE_MACHINE, __LINE__, (uint32_t)pstInitiator, u32MaxWorkWindowTimeUs);
        return LLP_ERR_SYSTEM;
    }
    
    EN_LLP_ERR_T err;
    if (pstInitiator->bIsScanningPhyPrimary)
    {
        err = rom_llc_ext_initiator_primary_scanning_config_llp(pstInitiator, LLC_TOOL_MIN(pstInitiator->u32ScanningPhyMaxWorkTimeUs, u32MaxWorkWindowTimeUs));
    }
    else
    {
        err = rom_llc_ext_initiator_secondary_scanning_config_llp(pstInitiator, LLC_TOOL_MIN(pstInitiator->u32ScanningPhyMaxWorkTimeUs, u32MaxWorkWindowTimeUs));
    }
    
    switch(err)
    {
        case LLP_ERR_NONE:
            break;
        
        case LLP_ERR_TIME_POINT:
        {
            //proc a fail event
            rom_llc_ext_initiator_state_machine_on_llp_fragment_start(pstInitiator);
            rom_llc_ext_initiator_state_machine_on_llp_fragment_stop(pstInitiator);
        }
            break;
        
        default:
            ////unexpected situation
            system_error(SYS_ERR_MODULE_LLC_EXT_INITIATOR_STATE_MACHINE, __LINE__, (uint32_t)pstInitiator, err);
            break;
    }
    
    return err;
}


void rom_llc_ext_initiator_state_machine_on_llp_fragment_start(void* pstRoleInstance)
{
    stLlcExtInitiator_t* pstInitiator = pstRoleInstance;
    if (NULL == pstInitiator)
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_INITIATOR_STATE_MACHINE, __LINE__, (uint32_t)pstInitiator, 0);
        return;
    }
    
    if (pstInitiator->bIsScanningPhyPrimary)
    {
        rom_llc_ext_initiator_primary_scanning_on_llp_fragment_start(pstInitiator);
    }
    else
    {
        rom_llc_ext_initiator_secondary_scanning_on_llp_fragment_start(pstInitiator);
    }
}

bool rom_llc_ext_initiator_state_machine_on_llp_fragment_stop(void* pstRoleInstance)
{
    stLlcExtInitiator_t* pstInitiator = pstRoleInstance;
    if (NULL == pstInitiator)
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_INITIATOR_STATE_MACHINE, __LINE__, (uint32_t)pstInitiator, 0);
        return false;
    }
    
    if (pstInitiator->bIsScanningPhyPrimary)
    {
        return rom_llc_ext_initiator_primary_scanning_on_llp_fragment_stop(pstInitiator);
    }
    else
    {
        return rom_llc_ext_initiator_secondary_scanning_on_llp_fragment_stop(pstInitiator);
    }
}

EN_LLP_STATE_CMD_T rom_llc_ext_initiator_state_machine_on_llp_tx_end(void* pstRoleInstance, stLlpTxEndReport_t *pstReport)
{
    stLlcExtInitiator_t* pstInitiator = pstRoleInstance;
    if ((NULL == pstInitiator)||(NULL == pstReport))
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_INITIATOR_STATE_MACHINE, __LINE__, (uint32_t)pstInitiator, (uint32_t)pstReport);
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    if (pstInitiator->bIsScanningPhyPrimary)
    {
        return rom_llc_ext_initiator_primary_scanning_on_llp_tx_end(pstInitiator, pstReport);
    }
    else
    {
        return rom_llc_ext_initiator_secondary_scanning_on_llp_tx_end(pstInitiator, pstReport);
    }
}

EN_LLP_STATE_CMD_T rom_llc_ext_initiator_state_machine_on_llp_rx_end(void* pstRoleInstance, stLlpRxEndReport_t *pstReport)
{
    stLlcExtInitiator_t* pstInitiator = pstRoleInstance;
    if ((NULL == pstInitiator)||(NULL == pstReport))
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_INITIATOR_STATE_MACHINE, __LINE__, (uint32_t)pstInitiator, (uint32_t)pstReport);
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    if (pstInitiator->bIsScanningPhyPrimary)
    {
        return rom_llc_ext_initiator_primary_scanning_on_llp_rx_end(pstInitiator, pstReport);
    }
    else
    {
        return rom_llc_ext_initiator_secondary_scanning_on_llp_rx_end(pstInitiator, pstReport);
    }
}
