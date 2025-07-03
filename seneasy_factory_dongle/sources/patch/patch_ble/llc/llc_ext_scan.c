
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



#include "extended_controller_interface.h"


#include "llc_ext_scan.h"
#include "llc_ext_scan_primary_scanning.h"
#include "llc_ext_scan_state_machine.h"


stLlcExtScan_t* g_pstLlcExtScanMem = NULL;

stLlcExtScanPduExtraInfo_t* g_pstLlcExtScanPduExtraInfoMem = NULL;
uint8_t g_u8LlcExtScanPduExtraInfoNum = 0;

stLlcExtScanAdvertisingSet_t* g_pstLlcExtScanAdvertisingSetMem = NULL;
uint8_t g_u8LlcExtScanAdvertisingSetNum = 0;

bool rom_llc_ext_scan_mem_init(stLlcExtScan_t* pstLlcExtScanMemBuffer,
    stLlcExtScanPduExtraInfo_t* pstLlcExtScanPduExtraInfoMem, uint8_t u8LlcExtScanPduExtraInfoNum,
    stLlcExtScanAdvertisingSet_t* pstLlcExtScanAdvertisingSetMemBuffer, uint8_t u8LlcExtScanAdvertisingSetNum)
{
    uint32_t i;
    
    g_pstLlcExtScanMem = pstLlcExtScanMemBuffer;
    
    if (NULL != g_pstLlcExtScanMem)
    {
        g_pstLlcExtScanMem->bEnabled = false;
    }
    
    if (NULL == pstLlcExtScanPduExtraInfoMem)
    {
        u8LlcExtScanPduExtraInfoNum = 0;
    }
    g_pstLlcExtScanPduExtraInfoMem = pstLlcExtScanPduExtraInfoMem;
    g_u8LlcExtScanPduExtraInfoNum = u8LlcExtScanPduExtraInfoNum;
    for(i = 0; i < g_u8LlcExtScanPduExtraInfoNum; i++)
    {
        g_pstLlcExtScanPduExtraInfoMem[i].bMemUsed = false;
    }
    
    if (NULL == pstLlcExtScanAdvertisingSetMemBuffer)
    {
        u8LlcExtScanAdvertisingSetNum = 0;
    }
    g_pstLlcExtScanAdvertisingSetMem = pstLlcExtScanAdvertisingSetMemBuffer;
    g_u8LlcExtScanAdvertisingSetNum = u8LlcExtScanAdvertisingSetNum;
    for(i = 0; i < g_u8LlcExtScanAdvertisingSetNum; i++)
    {
        g_pstLlcExtScanAdvertisingSetMem[i].bMemUsed = false;
    }
    
    
    return true;
}


stLlcAdvChannelPdu_t* rom_llc_ext_scan_alloc_pdu(stLlcExtScan_t* pstScan)
{
    stLlcAdvChannelPdu_t* pstPdu = rom_llc_adv_channel_pdu_alloc(LLC_SPEC_5_1_ADV_CHANNEL_PDU_MAX_PAYLOAD_SIZE);
    if (NULL == pstPdu)
    {
        pstPdu = &(pstScan->stRxEmptyPdu);
    }
    
    //alloc extra info
    stLlcExtScanPduExtraInfo_t* pstExtraInfo = NULL;
    uint32_t u32Primask = __get_PRIMASK();
    __disable_irq();
    uint32_t i;
    for(i = 0; i < g_u8LlcExtScanPduExtraInfoNum; i++)
    {
        if (!g_pstLlcExtScanPduExtraInfoMem[i].bMemUsed)
        {
            pstExtraInfo = &g_pstLlcExtScanPduExtraInfoMem[i];
            pstExtraInfo->bMemUsed = true;
            break;
        }
    }
    __set_PRIMASK(u32Primask);
    pstPdu->u32Reserved = (uint32_t)pstExtraInfo;
    
    return pstPdu;
}

bool rom_llc_ext_scan_is_pdu_empty(stLlcExtScan_t* pstScan, stLlcAdvChannelPdu_t* pstPdu)
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

void rom_llc_ext_scan_free_pdu(stLlcExtScan_t* pstScan, stLlcAdvChannelPdu_t* pstPdu)
{
    if (NULL == pstPdu)
    {
        return;
    }
    
    //free extra info
    stLlcExtScanPduExtraInfo_t* pstExtraInfo = (stLlcExtScanPduExtraInfo_t*)pstPdu->u32Reserved;
    if (NULL != pstExtraInfo)
    {
        uint32_t u32Primask = __get_PRIMASK();
        __disable_irq();
        pstExtraInfo->bMemUsed = false;
        pstPdu->u32Reserved = NULL;
        __set_PRIMASK(u32Primask);
    }
    
    //free pdu
    if ((&(pstScan->stRxEmptyPdu)) != pstPdu)
    {
        rom_llc_adv_channel_pdu_free(pstPdu);
    }
}

stLlcExtScanPduExtraInfo_t* rom_llc_ext_scan_get_pdu_extra_info(stLlcAdvChannelPdu_t* pstPdu)
{
    if (NULL == pstPdu)
    {
        return NULL;
    }

    stLlcExtScanPduExtraInfo_t* pstExtraInfo = (stLlcExtScanPduExtraInfo_t*)pstPdu->u32Reserved;
    return pstExtraInfo;
}


stLlcExtScanAdvertisingSet_t* rom_llc_ext_scan_alloc_advertising_set(void)
{
    stLlcExtScanAdvertisingSet_t* pstAdvertisingSet = NULL;
    
    uint32_t u32Primask = __get_PRIMASK();
    __disable_irq();
    uint32_t i;
    for(i = 0; i < g_u8LlcExtScanAdvertisingSetNum; i++)
    {
        if (!g_pstLlcExtScanAdvertisingSetMem[i].bMemUsed)
        {
            pstAdvertisingSet = &g_pstLlcExtScanAdvertisingSetMem[i];
            pstAdvertisingSet->bMemUsed = true;
            pstAdvertisingSet->prev = NULL;
            pstAdvertisingSet->next = NULL;
            break;
        }
    }
    __set_PRIMASK(u32Primask);
    
    return pstAdvertisingSet;
}

void rom_llc_ext_scan_free_advertising_set(stLlcExtScanAdvertisingSet_t* pstAdvertisingSet)
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



stLlcExtScan_t* rom_llc_ext_scan_get_instance(void)
{
    return g_pstLlcExtScanMem;
}


void rom_llc_ext_scan_init(void)
{
    
    stLlcExtScan_t* pstScan = rom_llc_ext_scan_get_instance();
    if (NULL == pstScan)
    {
        return;
    }
    
    pstScan->stRxEmptyPdu.pu8Payload = NULL;
    pstScan->stRxEmptyPdu.u16PayloadBufSize = 0;
    
    rom_llc_ext_scan_state_machine_init(pstScan);
}

bool rom_llc_ext_scan_is_enabled(void)
{
    stLlcExtScan_t* pstScan = rom_llc_ext_scan_get_instance();
    if (NULL == pstScan)
    {
        return false;
    }
    return pstScan->bEnabled;
}

bool rom_llc_ext_scan_is_white_list_in_use(void)
{
    stLlcExtScan_t* pstScan = rom_llc_ext_scan_get_instance();
    if (NULL == pstScan)
    {
        return false;
    }
    
    if (!pstScan->bEnabled)
    {
        return false;
    }
    
    switch(pstScan->Scanning_Filter_Policy)
    {
        case LLC_SPEC_5_1_HCI_SCAN_FILTER_POLICY_ACCEPT_ALL:
        case LLC_SPEC_5_1_HCI_SCAN_FILTER_POLICY_ACCEPT_ALL_NOT_RESOLVED_DIRECTED:
            return false;
        
        case LLC_SPEC_5_1_HCI_SCAN_FILTER_POLICY_ACCEPT_WHITELIST:
        case LLC_SPEC_5_1_HCI_SCAN_FILTER_POLICY_ACCEPT_WHITELIST_NOT_RESOLVED_DIRECTED:
            return true;
        
        default:
            return false;
    }
}

uint8_t rom_llc_ext_scan_set_parameters(stLlcExtScanSetParameters_t* pstParameters)
{
    if (NULL == pstParameters)
    {
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
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
    
    uint8_t Scanning_Filter_Policy = pstParameters->Scanning_Filter_Policy;
    switch(Scanning_Filter_Policy)
    {
        case LLC_SPEC_5_1_HCI_SCAN_FILTER_POLICY_ACCEPT_ALL:
        case LLC_SPEC_5_1_HCI_SCAN_FILTER_POLICY_ACCEPT_WHITELIST:
        case LLC_SPEC_5_1_HCI_SCAN_FILTER_POLICY_ACCEPT_ALL_NOT_RESOLVED_DIRECTED:
        case LLC_SPEC_5_1_HCI_SCAN_FILTER_POLICY_ACCEPT_WHITELIST_NOT_RESOLVED_DIRECTED:
            break;
        
        default:
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
    }
    
    if (!(pstParameters->Scan_PHY_Enabled_1M || pstParameters->Scan_PHY_Enabled_Coded))
    {
        return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
    }
    if (pstParameters->Scan_PHY_Enabled_1M)
    {
        uint8_t Scan_Type = pstParameters->Scan_Type_1M;
        uint16_t Scan_Interval = pstParameters->Scan_Interval_1M;
        uint16_t Scan_Window = pstParameters->Scan_Window_1M;
        uint8_t Scan_Map = pstParameters->Scan_Map_1M;
        
        if ((LLC_SPEC_5_1_HCI_SCAN_TYPE_PASSIVE != Scan_Type) && (LLC_SPEC_5_1_HCI_SCAN_TYPE_ACTIVE != Scan_Type))
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
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
    if (pstParameters->Scan_PHY_Enabled_Coded)
    {
        uint8_t Scan_Type = pstParameters->Scan_Type_Coded;
        uint16_t Scan_Interval = pstParameters->Scan_Interval_Coded;
        uint16_t Scan_Window = pstParameters->Scan_Window_Coded;
        uint8_t Scan_Map = pstParameters->Scan_Map_Coded;
        
        if ((LLC_SPEC_5_1_HCI_SCAN_TYPE_PASSIVE != Scan_Type) && (LLC_SPEC_5_1_HCI_SCAN_TYPE_ACTIVE != Scan_Type))
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
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
    
    stLlcExtScan_t* pstScan = rom_llc_ext_scan_get_instance();
    if (NULL == pstScan)
    {
        return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
    }
    
    if (pstScan->bEnabled)
    {
        return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
    }
    
    rom_llc_ext_scan_primary_scanning_clear_phy(pstScan);
    if (pstParameters->Scan_PHY_Enabled_1M)
    {
        uint8_t Scan_Type = pstParameters->Scan_Type_1M;
        uint16_t Scan_Interval = pstParameters->Scan_Interval_1M;
        uint16_t Scan_Window = pstParameters->Scan_Window_1M;
        uint8_t Scan_Map = pstParameters->Scan_Map_1M;
        
        if (!rom_llc_ext_scan_primary_scanning_add_phy(pstScan, LL_PHY_1M, Scan_Type, Scan_Interval, Scan_Window, Scan_Map))
        {
            rom_llc_ext_scan_primary_scanning_clear_phy(pstScan);
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }
    }
    
    if (pstParameters->Scan_PHY_Enabled_Coded)
    {
        uint8_t Scan_Type = pstParameters->Scan_Type_Coded;
        uint16_t Scan_Interval = pstParameters->Scan_Interval_Coded;
        uint16_t Scan_Window = pstParameters->Scan_Window_Coded;
        uint8_t Scan_Map = pstParameters->Scan_Map_Coded;
        
        if (!rom_llc_ext_scan_primary_scanning_add_phy(pstScan, LL_PHY_CODED_S8, Scan_Type, Scan_Interval, Scan_Window, Scan_Map))
        {
            rom_llc_ext_scan_primary_scanning_clear_phy(pstScan);
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }
    }
    
    //accept parameters
    pstScan->Own_Address_Type = Own_Address_Type;
    pstScan->Scanning_Filter_Policy = Scanning_Filter_Policy;
    
    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
    
}

void rom_llc_ext_scan_set_enable(uint8_t Enable, uint8_t Filter_Duplicates, uint16_t Duration, uint16_t Period)
{
    if ((LLC_SPEC_5_1_HCI_SCAN_ENABLE_DISABLED != Enable) && (LLC_SPEC_5_1_HCI_SCAN_ENABLE_ENABLED != Enable))
    {
        rom_controller_return_parameters_HCI_LE_Set_Extended_Scan_Enable(LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS);
        return;
    }
    
    stLlcExtScan_t* pstScan = rom_llc_ext_scan_get_instance();
    if (NULL == pstScan)
    {
        rom_controller_return_parameters_HCI_LE_Set_Extended_Scan_Enable(LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED);
        return;
    }
    
    
    switch(Filter_Duplicates)
    {
        case LLC_SPEC_5_1_HCI_FILTER_DUPLICATES_DISABLED:
        case LLC_SPEC_5_1_HCI_FILTER_DUPLICATES_ENABLED:
        case LLC_SPEC_5_1_HCI_FILTER_DUPLICATES_ENABLED_AND_RESET_FOR_EACH_SCAN_PERIOD:
            break;
        
        default:
            rom_controller_return_parameters_HCI_LE_Set_Extended_Scan_Enable(LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS);
            return;
    }
    
    if ((0 != Duration) && (0 != Period) && ((Duration * 10) >= (Period * 1280)))//unit 10ms, //unit 1.28s
    {
        rom_controller_return_parameters_HCI_LE_Set_Extended_Scan_Enable(LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS);
        return;
    }
    
    if (0 == rom_llc_ext_scan_primary_scanning_get_phy_num(pstScan))
    {
        rom_controller_return_parameters_HCI_LE_Set_Extended_Scan_Enable(LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED);
        return;
    }

    
    bool bEnable = LLC_SPEC_5_1_HCI_SCAN_ENABLE_ENABLED == Enable;
    pstScan->Filter_Duplicates = Filter_Duplicates;
    pstScan->Duration = Duration;
    pstScan->Period = Period;
    
    rom_llc_ext_scan_state_machine_enable(pstScan, bEnable);
}



bool rom_llc_ext_scan_check_RPA_and_white_list_disallowed(stLlcExtScan_t* pstScan, 
    bool bPeerTxAdd, uint8_t pu8PeerAdvA[6], 
    bool bPeerRxAdd, uint8_t pu8PeerTargetA[6],
    EN_LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_T* Peer_Address_Type, uint8_t Peer_Address[6],
    EN_LLC_SPEC_5_1_HCI_DIRECT_ADDRESS_TYPE_T* Direct_Address_Type, uint8_t Direct_Address[6])
{
    //only be invoked in interrupt

    //input pstScan
    //input bPeerTxAdd
    //input pu8PeerAdvA : nullable, if so, peer is anonymous
    //input bPeerRxAdd
    //input pu8PeerTagetA : nullable, if so, it is not a directed adv

    //output Peer_Address_Type : nuallble
    //output Peer_Address : nuallble

    //output Direct_Address_Type : nullable
    //output Direct_Address : nullable

    //weike impl
    
    uint8_t pu8PeerIdentifyAddr[6];
    uint8_t u8PeerIdentifyAddrType;
    bool bResolving_peer = false;
    bool bResolving_target = false;
    
    if ( NULL == pu8PeerAdvA )
    {
        if ( NULL != Peer_Address_Type )
        {
            *Peer_Address_Type = LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_ANONYMOUS;
        }
        
        if ( LLC_SPEC_5_1_HCI_SCAN_FILTER_POLICY_ACCEPT_WHITELIST == pstScan->Scanning_Filter_Policy ||
                LLC_SPEC_5_1_HCI_SCAN_FILTER_POLICY_ACCEPT_WHITELIST_NOT_RESOLVED_DIRECTED  == pstScan->Scanning_Filter_Policy )
        {
            uint8_t pu8AnonymousAddr[6] = {0};
            if (!rom_llc_white_list_is_device_in_list(LLC_SPEC_5_1_HCI_ADDRESS_TYPE_ANONYMOUS,pu8AnonymousAddr))
            {
                return true;
            }
        }
    }
    else
    {
        if ( NULL != Peer_Address_Type )
        {
            *Peer_Address_Type = bPeerTxAdd?LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_RANDOM:LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_PUBLIC;
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
                        *Peer_Address_Type = u8PeerIdentifyAddrType?LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_RANDOM_IDENTITY:LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_PUBLIC_IDENTITY;
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
        if ( LLC_SPEC_5_1_HCI_SCAN_FILTER_POLICY_ACCEPT_WHITELIST == pstScan->Scanning_Filter_Policy ||
                LLC_SPEC_5_1_HCI_SCAN_FILTER_POLICY_ACCEPT_WHITELIST_NOT_RESOLVED_DIRECTED  == pstScan->Scanning_Filter_Policy )
        {
            if ( bResolving_peer )
            {
                if ( !rom_llc_white_list_is_device_in_list ( u8PeerIdentifyAddrType?LLC_SPEC_5_1_HCI_ADDRESS_TYPE_RANDOM:LLC_SPEC_5_1_HCI_ADDRESS_TYPE_PUBLIC, pu8PeerIdentifyAddr ) )
                {
                    //cancel req and report
                    return true;
                }
            }
            else
            {
                if ( !rom_llc_white_list_is_device_in_list ( bPeerTxAdd?LLC_SPEC_5_1_HCI_ADDRESS_TYPE_RANDOM:LLC_SPEC_5_1_HCI_ADDRESS_TYPE_PUBLIC, pu8PeerAdvA ) )
                {
                    //cancel req and report
                    return true;
                }
            }
        }
    }

    if ( NULL == pu8PeerTargetA )
    {
        //do nothing
    }
    else
    {
        if ( NULL != Direct_Address_Type )
        {
            *Direct_Address_Type = bPeerRxAdd?LLC_SPEC_5_1_HCI_DIRECT_ADDRESS_TYPE_RANDOM:LLC_SPEC_5_1_HCI_DIRECT_ADDRESS_TYPE_PUBLIC;
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

            if ( ( ( LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RESOLVABLE_OR_RANDOM == pstScan->Own_Address_Type ) ||
                    ( LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RESOLVABLE_OR_PUBLIC == pstScan->Own_Address_Type ) ) )
            {
                //local is RPA, targetA is identify ,should not send req or report adv
                return true;
            }
            else if ( ( bPeerRxAdd == pstScan->bOwnAddressTxAdd )
                      && ( 0 == memcmp ( pu8PeerTargetA, pstScan->pu8OwnAddress, 6 ) ) )
            {
                return false;
            }
            else
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
                    if ( ( ( LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RESOLVABLE_OR_RANDOM == pstScan->Own_Address_Type ) ||
                            ( LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RANDOM == pstScan->Own_Address_Type ) ) )
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

                return false;
            }
            else
            {

                if ( LLC_SPEC_5_1_HCI_SCAN_FILTER_POLICY_ACCEPT_ALL_NOT_RESOLVED_DIRECTED == pstScan->Scanning_Filter_Policy ||
                        LLC_SPEC_5_1_HCI_SCAN_FILTER_POLICY_ACCEPT_WHITELIST_NOT_RESOLVED_DIRECTED  == pstScan->Scanning_Filter_Policy )
                {
                    // Directed advertising PDUs where the initiator's address is a resolvable private address that cannot be resolved are also accepted.
                    return false;

                }
                return true;
            }
        }
    }

    return false;
}
