
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
#include "llc_white_list.h"
#include "llc_privacy.h"
#include "llc_tool.h"

#include "llc_acl_data.h"
#include "llc_data_channel_pdu.h"
#include "llc_data.h"
#include "llc_ctrl_procedure.h"
#include "llc_connection_role.h"

#include "llc_master.h"



#include "ble_impl.h"


#include "llc_ext_initiator.h"
#include "llc_ext_initiator_primary_scanning.h"
#include "llc_ext_initiator_state_machine.h"

stLlcExtInitiator_t* g_pstLlcExtInitiatorMem = NULL;

stLlcExtInitiatorPduExtraInfo_t* g_pstLlcExtInitiatorPduExtraInfoMem = NULL;
uint8_t g_u8LlcExtInitiatorPduExtraInfoNum = 0;

stLlcExtInitiatorAdvertisingSet_t* g_pstLlcExtInitiatorAdvertisingSetMem = NULL;
uint8_t g_u8LlcExtInitiatorAdvertisingSetNum = 0;

bool rom_llc_ext_initiator_mem_init(stLlcExtInitiator_t* pstLlcExtInitiatorMemBuffer,
    stLlcExtInitiatorPduExtraInfo_t* pstLlcExtInitiatorPduExtraInfoMem, uint8_t u8LlcExtInitiatorPduExtraInfoNum,
    stLlcExtInitiatorAdvertisingSet_t* pstLlcExtInitiatorAdvertisingSetMemBuffer, uint8_t u8LlcExtInitiatorAdvertisingSetNum)
{
    uint32_t i;
    
    g_pstLlcExtInitiatorMem = pstLlcExtInitiatorMemBuffer;
    
    if (NULL != g_pstLlcExtInitiatorMem)
    {
        g_pstLlcExtInitiatorMem->bEnabled = false;
    }
    
    if (NULL == pstLlcExtInitiatorPduExtraInfoMem)
    {
        u8LlcExtInitiatorPduExtraInfoNum = 0;
    }
    g_pstLlcExtInitiatorPduExtraInfoMem = pstLlcExtInitiatorPduExtraInfoMem;
    g_u8LlcExtInitiatorPduExtraInfoNum = u8LlcExtInitiatorPduExtraInfoNum;
    for(i = 0; i < g_u8LlcExtInitiatorPduExtraInfoNum; i++)
    {
        g_pstLlcExtInitiatorPduExtraInfoMem[i].bMemUsed = false;
    }
    
    if (NULL == pstLlcExtInitiatorAdvertisingSetMemBuffer)
    {
        u8LlcExtInitiatorAdvertisingSetNum = 0;
    }
    g_pstLlcExtInitiatorAdvertisingSetMem = pstLlcExtInitiatorAdvertisingSetMemBuffer;
    g_u8LlcExtInitiatorAdvertisingSetNum = u8LlcExtInitiatorAdvertisingSetNum;
    for(i = 0; i < g_u8LlcExtInitiatorAdvertisingSetNum; i++)
    {
        g_pstLlcExtInitiatorAdvertisingSetMem[i].bMemUsed = false;
    }
    
    
    return true;
}


stLlcAdvChannelPdu_t* rom_llc_ext_initiator_alloc_pdu(stLlcExtInitiator_t* pstInitiator)
{
    stLlcAdvChannelPdu_t* pstPdu = rom_llc_adv_channel_pdu_alloc(LLC_SPEC_5_1_ADV_CHANNEL_PDU_MAX_PAYLOAD_SIZE);
    if (NULL == pstPdu)
    {
        pstPdu = &(pstInitiator->stRxEmptyPdu);
    }
    
    //alloc extra info
    stLlcExtInitiatorPduExtraInfo_t* pstExtraInfo = NULL;
    uint32_t u32Primask = __get_PRIMASK();
    __disable_irq();
    uint32_t i;
    for(i = 0; i < g_u8LlcExtInitiatorPduExtraInfoNum; i++)
    {
        if (!g_pstLlcExtInitiatorPduExtraInfoMem[i].bMemUsed)
        {
            pstExtraInfo = &g_pstLlcExtInitiatorPduExtraInfoMem[i];
            pstExtraInfo->bMemUsed = true;
            break;
        }
    }
    __set_PRIMASK(u32Primask);
    pstPdu->u32Reserved = (uint32_t)pstExtraInfo;
    
    return pstPdu;
}

bool rom_llc_ext_initiator_is_pdu_empty(stLlcExtInitiator_t* pstInitiator, stLlcAdvChannelPdu_t* pstPdu)
{
    if (NULL == pstPdu)
    {
        return true;
    }
    if (0 == pstPdu->u16PayloadBufSize)
    {
        return true;
    }
    if (NULL == pstPdu->u32Reserved)
    {
        return true;
    }
    
    return false;
}

void rom_llc_ext_initiator_free_pdu(stLlcExtInitiator_t* pstInitiator, stLlcAdvChannelPdu_t* pstPdu)
{
    if (NULL == pstPdu)
    {
        return;
    }
    
    //free extra info
    stLlcExtInitiatorPduExtraInfo_t* pstExtraInfo = (stLlcExtInitiatorPduExtraInfo_t*)pstPdu->u32Reserved;
    if (NULL != pstExtraInfo)
    {
        uint32_t u32Primask = __get_PRIMASK();
        __disable_irq();
        pstExtraInfo->bMemUsed = false;
        pstPdu->u32Reserved = NULL;
        __set_PRIMASK(u32Primask);
    }
    
    //free pdu
    if ((&(pstInitiator->stRxEmptyPdu)) != pstPdu)
    {
        rom_llc_adv_channel_pdu_free(pstPdu);
    }
}

stLlcExtInitiatorPduExtraInfo_t* rom_llc_ext_initiator_get_pdu_extra_info(stLlcAdvChannelPdu_t* pstPdu)
{
    if (NULL == pstPdu)
    {
        return NULL;
    }

    stLlcExtInitiatorPduExtraInfo_t* pstExtraInfo = (stLlcExtInitiatorPduExtraInfo_t*)pstPdu->u32Reserved;
    return pstExtraInfo;
}


stLlcExtInitiatorAdvertisingSet_t* rom_llc_ext_initiator_alloc_advertising_set(void)
{
    stLlcExtInitiatorAdvertisingSet_t* pstAdvertisingSet = NULL;
    
    uint32_t u32Primask = __get_PRIMASK();
    __disable_irq();
    uint32_t i;
    for(i = 0; i < g_u8LlcExtInitiatorAdvertisingSetNum; i++)
    {
        if (!g_pstLlcExtInitiatorAdvertisingSetMem[i].bMemUsed)
        {
            pstAdvertisingSet = &g_pstLlcExtInitiatorAdvertisingSetMem[i];
            pstAdvertisingSet->bMemUsed = true;
            pstAdvertisingSet->prev = NULL;
            pstAdvertisingSet->next = NULL;
            break;
        }
    }
    __set_PRIMASK(u32Primask);
    
    return pstAdvertisingSet;
}

void rom_llc_ext_initiator_free_advertising_set(stLlcExtInitiatorAdvertisingSet_t* pstAdvertisingSet)
{
    if (NULL == pstAdvertisingSet)
    {
        return;
    }
    
    uint32_t u32Primask = __get_PRIMASK();
    __disable_irq();
    pstAdvertisingSet->bMemUsed = false;
    __set_PRIMASK(u32Primask);
}



stLlcExtInitiator_t* rom_llc_ext_initiator_get_instance(void)
{
    return g_pstLlcExtInitiatorMem;
}


void rom_llc_ext_initiator_init(void)
{
    stLlcExtInitiator_t* pstInitiator = rom_llc_ext_initiator_get_instance();
    if (NULL == pstInitiator)
    {
        return;
    }
    
    pstInitiator->stRxEmptyPdu.pu8Payload = NULL;
    pstInitiator->stRxEmptyPdu.u16PayloadBufSize = 0;
    
    rom_llc_ext_initiator_state_machine_init(pstInitiator);
}


bool rom_llc_ext_initiator_is_enabled(void)
{
    stLlcExtInitiator_t* pstInitiator = rom_llc_ext_initiator_get_instance();
    if (NULL == pstInitiator)
    {
        return false;
    }
    
    return pstInitiator->bEnabled;
}


bool rom_llc_ext_initiator_is_white_list_in_use(void)
{
    stLlcExtInitiator_t* pstInitiator = rom_llc_ext_initiator_get_instance();
    if (NULL == pstInitiator)
    {
        return false;
    }
    
    if (!pstInitiator->bEnabled)
    {
        return false;
    }
    
    
    return LLC_SPEC_5_1_HCI_INITIATOR_FILTER_POLICY_WHITELIST_IS_USED == pstInitiator->Initiator_Filter_Policy;
}


bool rom_llc_ext_initiator_is_valid_connection_parameters(uint16_t Conn_Interval_Min,
    uint16_t Conn_Interval_Max,
    uint16_t Conn_Latency,
    uint16_t Supervision_Timeout)
{
    if ((Conn_Interval_Min < 0x0006) || (Conn_Interval_Min > 0x0C80))
    {
        return false;
    }
    
    if ((Conn_Interval_Max < 0x0006) || (Conn_Interval_Max > 0x0C80))
    {
        return false;
    }
    
    if (Conn_Interval_Max < Conn_Interval_Min)
    {
        return false;
    }
    
    if (Conn_Latency > 0x01F3)
    {
        return false;
    }
    
    if ((Supervision_Timeout < 0x000A) || (Supervision_Timeout > 0x0C80))
    {
        return false;
    }
    
    if ((Supervision_Timeout*10000) <= ((1 + Conn_Latency) * (Conn_Interval_Max*625) * 2))
    {
        return false;
    }
    
    return true;
}


uint8_t rom_llc_ext_initiator_create_connection(stLlcExtInitiatorSetParameters_t* pstParameters)
{
    if (NULL == pstParameters)
    {
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }
    
    uint8_t Initiator_Filter_Policy = pstParameters->Initiator_Filter_Policy;
    switch(Initiator_Filter_Policy)
    {
        case LLC_SPEC_5_1_HCI_INITIATOR_FILTER_POLICY_WHITELIST_IS_NOT_USED:
        case LLC_SPEC_5_1_HCI_INITIATOR_FILTER_POLICY_WHITELIST_IS_USED:
            break;
        
        default:
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
    }
    
    uint8_t Own_Address_Type = pstParameters->Own_Address_Type;
    switch(Own_Address_Type)
    {
        case LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_PUBLIC:
        case LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RANDOM:
        case LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RESOLVABLE_OR_PUBLIC:
        case LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RESOLVABLE_OR_RANDOM:
            break;
        
        default:
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
    }
    
    uint8_t Peer_Address_Type = pstParameters->Peer_Address_Type;
    if (LLC_SPEC_5_1_HCI_INITIATOR_FILTER_POLICY_WHITELIST_IS_NOT_USED == Initiator_Filter_Policy)
    {
        switch(Peer_Address_Type)
        {
            case LLC_SPEC_5_1_HCI_EXT_CONN_PEER_ADDRESS_TYPE_PUBLIC_OR_PUBLIC_IDENTITY:
            case LLC_SPEC_5_1_HCI_EXT_CONN_PEER_ADDRESS_TYPE_RANDOM_OR_RANDOM_IDENTITY:
                break;
            
            default:
                return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
    }
    
    
    if (!(pstParameters->Scan_PHY_Enabled_1M || pstParameters->Scan_PHY_Enabled_Coded))
    {
        return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
    }
    if (pstParameters->Scan_PHY_Enabled_1M)
    {
        uint16_t Scan_Interval = pstParameters->Scan_Interval_1M;
        uint16_t Scan_Window = pstParameters->Scan_Window_1M;
        uint8_t Scan_Map = pstParameters->Scan_Map_1M;
        if (Scan_Interval < 0x0004)
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
        if (Scan_Window < 0x0004)
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
        if (Scan_Window > Scan_Interval)
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
    }
    if (pstParameters->Scan_PHY_Enabled_Coded)
    {
        uint16_t Scan_Interval = pstParameters->Scan_Interval_Coded;
        uint16_t Scan_Window = pstParameters->Scan_Window_Coded;
        uint8_t Scan_Map = pstParameters->Scan_Map_Coded;
        
        if (rom_ble_impl_is_command_filter_enabled())
        {
            uint64_t u64FeatureSet = rom_ble_impl_get_feature_set();
            if (0 == (u64FeatureSet & LLC_SPEC_5_1_FEATURE_SET_LE_CODED_PHY))
            {
                return LLC_SPEC_5_1_ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE;
            }
        }
        
        
        if (Scan_Interval < 0x0004)
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
        if (Scan_Window < 0x0004)
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
        if (Scan_Window > Scan_Interval)
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
    }
    
    if (! (pstParameters->Connection_Parameters_Provided_1M || pstParameters->Connection_Parameters_Provided_2M || pstParameters->Connection_Parameters_Provided_Coded))
    {
        return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
    }
    if (pstParameters->Connection_Parameters_Provided_1M)
    {
        uint16_t Conn_Interval_Min = pstParameters->Conn_Interval_Min_1M;
        uint16_t Conn_Interval_Max = pstParameters->Conn_Interval_Max_1M;
        uint16_t Conn_Latency = pstParameters->Conn_Latency_1M;
        uint16_t Supervision_Timeout = pstParameters->Supervision_Timeout_1M;
        
        if (!rom_llc_ext_initiator_is_valid_connection_parameters(Conn_Interval_Min, Conn_Interval_Max, Conn_Latency, Supervision_Timeout))
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
    }
    if (pstParameters->Connection_Parameters_Provided_2M)
    {
        uint16_t Conn_Interval_Min = pstParameters->Conn_Interval_Min_2M;
        uint16_t Conn_Interval_Max = pstParameters->Conn_Interval_Max_2M;
        uint16_t Conn_Latency = pstParameters->Conn_Latency_2M;
        uint16_t Supervision_Timeout = pstParameters->Supervision_Timeout_2M;
        
        if (rom_ble_impl_is_command_filter_enabled())
        {
            uint64_t u64FeatureSet = rom_ble_impl_get_feature_set();
            if (0 == (u64FeatureSet & LLC_SPEC_5_1_FEATURE_SET_LE_2M_PHY))
            {
                return LLC_SPEC_5_1_ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE;
            }
        }
        
        if (!rom_llc_ext_initiator_is_valid_connection_parameters(Conn_Interval_Min, Conn_Interval_Max, Conn_Latency, Supervision_Timeout))
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
    }
    if (pstParameters->Connection_Parameters_Provided_Coded)
    {
        uint16_t Conn_Interval_Min = pstParameters->Conn_Interval_Min_Coded;
        uint16_t Conn_Interval_Max = pstParameters->Conn_Interval_Max_Coded;
        uint16_t Conn_Latency = pstParameters->Conn_Latency_Coded;
        uint16_t Supervision_Timeout = pstParameters->Supervision_Timeout_Coded;
        
        if (rom_ble_impl_is_command_filter_enabled())
        {
            uint64_t u64FeatureSet = rom_ble_impl_get_feature_set();
            if (0 == (u64FeatureSet & LLC_SPEC_5_1_FEATURE_SET_LE_CODED_PHY))
            {
                return LLC_SPEC_5_1_ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE;
            }
        }
        
        if (!rom_llc_ext_initiator_is_valid_connection_parameters(Conn_Interval_Min, Conn_Interval_Max, Conn_Latency, Supervision_Timeout))
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
    }
    
    
    stLlcExtInitiator_t* pstInitiator = rom_llc_ext_initiator_get_instance();
    if (NULL == pstInitiator)
    {
        return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
    }
    
    if (pstInitiator->bEnabled)
    {
        return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
    }
    
    
    bool bPeerPublic = LLC_SPEC_5_1_HCI_EXT_CONN_PEER_ADDRESS_TYPE_PUBLIC_OR_PUBLIC_IDENTITY == Peer_Address_Type;
    if (rom_llc_connection_role_is_device_connnected(bPeerPublic, pstParameters->Peer_Address))
    {
        return LLC_SPEC_5_1_ERROR_CODE_CONNECTION_ALREADY_EXISTS;
    }
    
    
    //accept parameters
    
    pstInitiator->Initiator_Filter_Policy = Initiator_Filter_Policy;
    pstInitiator->Own_Address_Type = Own_Address_Type;
    
    
    switch(pstInitiator->Own_Address_Type)
    {
        case LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_PUBLIC:
        {
            pstInitiator->bOwnAddressTxAdd = false;
            pstInitiator->bOwnAddressResolvable = false;
            rom_llc_address_get_own_public_address(pstInitiator->pu8OwnAddress);
        }
        break;

        case LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RANDOM:
        {
            if (rom_llc_address_get_own_random_address(pstInitiator->pu8OwnAddress))
            {
                pstInitiator->bOwnAddressTxAdd = true;
                pstInitiator->bOwnAddressResolvable = false;
            }
            else
            {
                return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
            }

        }
        break;

        case LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RESOLVABLE_OR_PUBLIC:
        {
            if (rom_llc_address_get_own_resolvable_private_address(pstInitiator->pu8OwnAddress))
            {
                pstInitiator->bOwnAddressTxAdd = true;
                pstInitiator->bOwnAddressResolvable = true;
                pstInitiator->u64OwnResolvableAddressUpdatedTime = rom_llc_timer_get_count(true);
            }
            else
            {
                pstInitiator->bOwnAddressTxAdd = false;
                pstInitiator->bOwnAddressResolvable = false;
                rom_llc_address_get_own_public_address(pstInitiator->pu8OwnAddress);
            }
        }
        break;

        case LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RESOLVABLE_OR_RANDOM:
        {
            if (rom_llc_address_get_own_resolvable_private_address(pstInitiator->pu8OwnAddress))
            {
                pstInitiator->bOwnAddressTxAdd = true;
                pstInitiator->bOwnAddressResolvable = true;
                pstInitiator->u64OwnResolvableAddressUpdatedTime = rom_llc_timer_get_count(true);
            }
            else
            {
                if (rom_llc_address_get_own_random_address(pstInitiator->pu8OwnAddress))
                {
                    pstInitiator->bOwnAddressTxAdd = true;
                    pstInitiator->bOwnAddressResolvable = false;
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
    
    pstInitiator->Peer_Address_Type = Peer_Address_Type;
    memcpy(pstInitiator->Peer_Address, pstParameters->Peer_Address, 6);
    
    
    rom_llc_ext_initiator_primary_scanning_clear_phy(pstInitiator);
    if (pstParameters->Scan_PHY_Enabled_1M)
    {
        uint16_t Scan_Interval = pstParameters->Scan_Interval_1M;
        uint16_t Scan_Window = pstParameters->Scan_Window_1M;
        uint8_t Scan_Map = pstParameters->Scan_Map_1M;
        
        if (!rom_llc_ext_initiator_primary_scanning_add_phy(pstInitiator, LL_PHY_1M, Scan_Interval, Scan_Window, Scan_Map))
        {
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }
    }
    if (pstParameters->Scan_PHY_Enabled_Coded)
    {
        uint16_t Scan_Interval = pstParameters->Scan_Interval_Coded;
        uint16_t Scan_Window = pstParameters->Scan_Window_Coded;
        uint8_t Scan_Map = pstParameters->Scan_Map_Coded;
        
        if (!rom_llc_ext_initiator_primary_scanning_add_phy(pstInitiator, LL_PHY_CODED_S8, Scan_Interval, Scan_Window, Scan_Map))
        {
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }
    }
    
    rom_llc_ext_initiator_state_machine_clear_connection_parameters(pstInitiator);
    if (pstParameters->Connection_Parameters_Provided_1M)
    {
        uint16_t Conn_Interval_Min = pstParameters->Conn_Interval_Min_1M;
        uint16_t Conn_Interval_Max = pstParameters->Conn_Interval_Max_1M;
        uint16_t Conn_Latency = pstParameters->Conn_Latency_1M;
        uint16_t Supervision_Timeout = pstParameters->Supervision_Timeout_1M;
        if (!rom_llc_ext_initiator_state_machine_add_connection_parameters(pstInitiator, LLC_SPEC_5_1_HCI_PHY_LE_1M,
            Conn_Interval_Min,
            Conn_Interval_Max,
            Conn_Latency,
            Supervision_Timeout))
        {
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }
    }
    if (pstParameters->Connection_Parameters_Provided_2M)
    {
        uint16_t Conn_Interval_Min = pstParameters->Conn_Interval_Min_2M;
        uint16_t Conn_Interval_Max = pstParameters->Conn_Interval_Max_2M;
        uint16_t Conn_Latency = pstParameters->Conn_Latency_2M;
        uint16_t Supervision_Timeout = pstParameters->Supervision_Timeout_2M;
        if (!rom_llc_ext_initiator_state_machine_add_connection_parameters(pstInitiator, LLC_SPEC_5_1_HCI_PHY_LE_2M,
            Conn_Interval_Min,
            Conn_Interval_Max,
            Conn_Latency,
            Supervision_Timeout))
        {
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }
    }
    if (pstParameters->Connection_Parameters_Provided_Coded)
    {
        uint16_t Conn_Interval_Min = pstParameters->Conn_Interval_Min_Coded;
        uint16_t Conn_Interval_Max = pstParameters->Conn_Interval_Max_Coded;
        uint16_t Conn_Latency = pstParameters->Conn_Latency_Coded;
        uint16_t Supervision_Timeout = pstParameters->Supervision_Timeout_Coded;
        if (!rom_llc_ext_initiator_state_machine_add_connection_parameters(pstInitiator, LLC_SPEC_5_1_HCI_PHY_LE_CODED,
            Conn_Interval_Min,
            Conn_Interval_Max,
            Conn_Latency,
            Supervision_Timeout))
        {
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }
    }
    
    return rom_llc_ext_initiator_state_machine_enable(pstInitiator, true);
}

uint8_t rom_llc_ext_initiator_create_connection_cancel(void)
{
    stLlcExtInitiator_t* pstInitiator = rom_llc_ext_initiator_get_instance();
    if (NULL == pstInitiator)
    {
        return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
    }
    
    if (!pstInitiator->bEnabled)
    {
        return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
    }
    
    return rom_llc_ext_initiator_state_machine_enable(pstInitiator, false);
}


bool rom_llc_ext_initiator_check_RPA_and_white_list_disallowed ( stLlcExtInitiator_t *pstInitiator,
        bool bPeerTxAdd, uint8_t pu8PeerAdvA[6],
        bool bPeerRxAdd, uint8_t pu8PeerTargetA[6],
        EN_LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_T *Peer_Address_Type, uint8_t Peer_Address[6],
        EN_LLC_SPEC_5_1_HCI_DIRECT_ADDRESS_TYPE_T *Direct_Address_Type, uint8_t Direct_Address[6] )
{
    //only be invoked in interrupt

    //input pstInitiator
    //input bPeerTxAdd
    //input pu8PeerAdvA : nullable, if so, peer is anonymous
    //input bPeerRxAdd
    //input pu8PeerTagetA : nullable, if so, it is not a directed adv

    //output Peer_Address_Type : nullable
    //output Peer_Address : nullable

    //output Direct_Address_Type : nullable
    //output Direct_Address : nullable

    //weike impl

    if ( NULL == pu8PeerAdvA ) //ANONYMOUS
    {
        if ( NULL != Peer_Address_Type )
        {
            *Peer_Address_Type = LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_ANONYMOUS;
        }
        return true; // should not connect to ANONYMOUS device
    }

    uint8_t pu8PeerIdentifyAddr[6];
    uint8_t u8PeerIdentifyAddrType;
    bool bResolving_peer = false;
    bool bResolving_target = false;

    bool bAdv_HostPeer_match = false;


    if ( NULL != Peer_Address_Type )
    {
        *Peer_Address_Type = bPeerTxAdd ? LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_RANDOM : LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_PUBLIC;
    }
    if ( NULL != Peer_Address )
    {
        memcpy ( Peer_Address, pu8PeerAdvA, 6 );
    }

    bool bPeerIdentify = ( ( false == bPeerTxAdd ) || ( bPeerTxAdd && rom_llc_privacy_is_addr_static_random_addr ( pu8PeerAdvA ) ) ) ; //identify address
    bool bPeerRPA = ( bPeerTxAdd && rom_llc_privacy_is_addr_RPA ( pu8PeerAdvA ) ); //RPA
    if ( rom_llc_privacy_is_resolving_enabled() )
    {
        if ( bPeerIdentify ) //identify address
        {
            uint8_t privacy_mode = 0;
            uint8_t irk_is_all_zero = 0;
            bool ret = rom_llc_privacy_is_device_in_resolving_list ( bPeerTxAdd,   pu8PeerAdvA, &privacy_mode, &irk_is_all_zero );
            if ( ret )
            {
                if ( !irk_is_all_zero && ( PRIVACY_NETWORK_MODE == privacy_mode ) )
                {
                    //network mode, peer irk not null, peer addr should not be identy addr
                    return true;
                }
            }
        }
        else if ( bPeerRPA ) //RPA
        {
            if ( rom_llc_privacy_resolve_peer_RPA ( pu8PeerAdvA,   pu8PeerIdentifyAddr, &u8PeerIdentifyAddrType ) )
            {
                if ( NULL != Peer_Address_Type )
                {
                    *Peer_Address_Type = u8PeerIdentifyAddrType ? LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_RANDOM_IDENTITY : LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_PUBLIC_IDENTITY;
                }
                if ( NULL != Peer_Address )
                {
                    memcpy ( Peer_Address, pu8PeerIdentifyAddr, 6 );
                }
                //resolve success
                bResolving_peer = true;

            }

        }

    }
    if ( LLC_SPEC_5_1_HCI_INITIATOR_FILTER_POLICY_WHITELIST_IS_NOT_USED == pstInitiator->Initiator_Filter_Policy || ( NULL != pu8PeerTargetA ) )
    {
        if ( bPeerRPA && bResolving_peer) //RPA
        {
            // if ( bResolving_peer )
            {

                if ( ( u8PeerIdentifyAddrType == pstInitiator->Peer_Address_Type )
                        && ( 0 == memcmp ( pu8PeerIdentifyAddr, pstInitiator->Peer_Address, 6 ) ) )
                {
                    //
                    bAdv_HostPeer_match = true;
                }
            }

        }
        else
        {
            uint8_t Adv_Address_Type = bPeerTxAdd ? LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_RANDOM : LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_PUBLIC;
            if ( ( Adv_Address_Type == pstInitiator->Peer_Address_Type )
                    && ( 0 == memcmp ( pu8PeerAdvA, pstInitiator->Peer_Address, 6 ) ) )
            {
                //
                bAdv_HostPeer_match = true;
            }
        }
    }



    if ( NULL != pu8PeerTargetA ) //direct adv
    {
        if ( NULL != Direct_Address_Type )
        {
            *Direct_Address_Type = bPeerRxAdd ? LLC_SPEC_5_1_HCI_DIRECT_ADDRESS_TYPE_RANDOM : LLC_SPEC_5_1_HCI_DIRECT_ADDRESS_TYPE_PUBLIC;
        }
        if ( NULL != Direct_Address )
        {
            memcpy ( Direct_Address, pu8PeerTargetA, 6 );
        }
        bool bTargetdentify = ( ( false == bPeerRxAdd ) || ( bPeerRxAdd && rom_llc_privacy_is_addr_static_random_addr ( pu8PeerTargetA ) ) ) ; //identify address
        bool bTargetRPA = ( bPeerRxAdd && rom_llc_privacy_is_addr_RPA ( pu8PeerTargetA ) ); //RPA

        if ( bTargetdentify )
        {
            //identify address

            if(pstInitiator->bOwnAddressResolvable)
            {
                //local is RPA, targetA is identify ,should not send req or report adv
                return true;
            }
            else if ( ( bPeerRxAdd != pstInitiator->bOwnAddressTxAdd )
                      || ( 0 != memcmp ( pu8PeerTargetA, pstInitiator->pu8OwnAddress, 6 ) ) )
            {
                
                return true;
            }


        }
        else if ( bTargetRPA )  //RPA
        {
            if ( rom_llc_privacy_is_resolving_enabled() && rom_llc_privacy_resolve_local_RPA ( pu8PeerTargetA ) )
            {
                //resolve local RPA success
                if ( NULL != Direct_Address_Type )
                {
                   
                   if ( ( ( LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RESOLVABLE_OR_RANDOM == pstInitiator->Own_Address_Type ) ||
                    ( LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RANDOM == pstInitiator->Own_Address_Type ) ) )
                    {
                        *Direct_Address_Type = LLC_SPEC_5_1_HCI_DIRECT_ADDRESS_TYPE_RANDOM_IDENTITY;
                        if ( NULL != Direct_Address )
                        {
                            rom_llc_address_get_own_random_address ( Direct_Address );
                        }

                    }
                    else
                    {
                        *Direct_Address_Type = LLC_SPEC_5_1_HCI_DIRECT_ADDRESS_TYPE_PUBLIC_IDENTITY;
                        if ( NULL != Direct_Address )
                        {
                            rom_llc_address_get_own_public_address ( Direct_Address );
                        }
                    }

                }
              

            }
            else
            {
                return true;
            }
        }
        else //nonRPA
        {
            if ( ( bPeerRxAdd != pstInitiator->bOwnAddressTxAdd )
                      || ( 0 != memcmp ( pu8PeerTargetA, pstInitiator->pu8OwnAddress, 6 ) ) )
            {
                
                return true;
            }
        }

        if ( bAdv_HostPeer_match )  //for direct adv, devices whose adv in wl(WHITELIST_IS_USED) or match to initiator  adv  should be connected
        {
            //for direct adv , if target addr == initiator addr , should be connected Whether whitelisting is enabled or not
            return false;
        }
       
    }
    if ( LLC_SPEC_5_1_HCI_INITIATOR_FILTER_POLICY_WHITELIST_IS_USED == pstInitiator->Initiator_Filter_Policy )
    {
        if ( bResolving_peer )
        {
            if ( rom_llc_white_list_is_device_in_list ( u8PeerIdentifyAddrType ? LLC_SPEC_5_1_HCI_ADDRESS_TYPE_RANDOM : LLC_SPEC_5_1_HCI_ADDRESS_TYPE_PUBLIC, pu8PeerIdentifyAddr ) )
            {

                return false;
            }
        }
        else
        {
            if ( rom_llc_white_list_is_device_in_list ( bPeerTxAdd ? LLC_SPEC_5_1_HCI_ADDRESS_TYPE_RANDOM : LLC_SPEC_5_1_HCI_ADDRESS_TYPE_PUBLIC, pu8PeerAdvA ) )
            {

                return false;
            }
        }

    }
    else if ( LLC_SPEC_5_1_HCI_INITIATOR_FILTER_POLICY_WHITELIST_IS_NOT_USED == pstInitiator->Initiator_Filter_Policy )
    {
        if ( bAdv_HostPeer_match )
        {
            return false;
        }
    }
    return true;
}
