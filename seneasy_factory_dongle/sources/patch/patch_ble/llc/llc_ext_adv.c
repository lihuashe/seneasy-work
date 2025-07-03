/*
MLOG_ID_RANGE: 0x37C0~0x37CF
MLOG_ID_USED:  0x37C0~0x37C3
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "RegMXD2670.h"

#include "llc_llp_interface.h"



#include "llc.h"
#include "llc_spec_5_1.h"

#include "llc_scheduler.h"

#include "llc_address.h"
#include "llc_white_list.h"
#include "llc_privacy.h"

#include "llc_adv_channel_pdu.h"

#include "llc_tool.h"

#include "ble_impl.h"


#include "llc_common_extended_advertising_payload_format.h"
#include "llc_ext_adv.h"

#include "llc_ext_adv_pdu_manager.h"
#include "llc_ext_adv_state_machine.h"
#include "llc_ext_adv_legacy_pdu_manager.h"
#include "llc_ext_adv_legacy_state_machine.h"

#include "mlog.h"
#include "rom_callback.h"





stLlcExtAdv_t* g_pstLlcExtAdvMem = NULL;
uint8_t g_u8LlcExtAdvMaxNum = 0;

stLlcExtAdvPduExtraInfo_t* g_pstLlcExtAdvPduExtraInfoMem = NULL;
uint8_t g_u8LlcExtAdvPduExtraInfoMaxNum = 0;

bool rom_llc_ext_adv_mem_init(stLlcExtAdv_t* pstLlcExtAdvMem, uint8_t u8LlcExtAdvNum,
                              stLlcExtAdvPduExtraInfo_t* pstLlcExtAdvPduExtraInfoMem, uint8_t u8LlcExtAdvPduExtraInfoNum)
{
    uint32_t i;

    if (NULL == pstLlcExtAdvMem)
    {
        u8LlcExtAdvNum = 0;
    }
    g_pstLlcExtAdvMem = pstLlcExtAdvMem;
    g_u8LlcExtAdvMaxNum = u8LlcExtAdvNum;
    for(i = 0; i < g_u8LlcExtAdvMaxNum; i++)
    {
        g_pstLlcExtAdvMem[i].bCreated = false;
    }



    if (NULL == pstLlcExtAdvPduExtraInfoMem)
    {
        u8LlcExtAdvPduExtraInfoNum = 0;
    }
    g_pstLlcExtAdvPduExtraInfoMem = pstLlcExtAdvPduExtraInfoMem;
    g_u8LlcExtAdvPduExtraInfoMaxNum = u8LlcExtAdvPduExtraInfoNum;
    for(i = 0; i < g_u8LlcExtAdvPduExtraInfoMaxNum; i++)
    {
        g_pstLlcExtAdvPduExtraInfoMem[i].bMemUsed = false;
    }

    return true;
}


uint8_t rom_llc_ext_adv_get_num(void)
{
    return g_u8LlcExtAdvMaxNum;
}


stLlcExtAdv_t* rom_llc_ext_adv_get_instance_by_index(uint8_t u8AdvIdx)
{
    if ((NULL == g_pstLlcExtAdvMem) || (u8AdvIdx >= g_u8LlcExtAdvMaxNum))
    {
        return NULL;
    }

    return &(g_pstLlcExtAdvMem[u8AdvIdx]);
}


stLlcExtAdv_t* rom_llc_ext_adv_create_instance_by_handle(uint8_t Advertising_Handle)
{
    //check existing
    uint32_t i;
    for(i = 0; i < g_u8LlcExtAdvMaxNum; i++)
    {
        stLlcExtAdv_t *pstAdv = &g_pstLlcExtAdvMem[i];
        if (pstAdv->bCreated && (Advertising_Handle == pstAdv->Advertising_Handle))
        {
            return pstAdv;
        }
    }


    //create new
    for(i = 0; i < g_u8LlcExtAdvMaxNum; i++)
    {
        stLlcExtAdv_t *pstAdv = &g_pstLlcExtAdvMem[i];
        if (!pstAdv->bCreated)
        {
            pstAdv->bCreated = true;
            pstAdv->Advertising_Handle = Advertising_Handle;
            return pstAdv;
        }
    }

    //all instance used
    return NULL;
}


stLlcExtAdv_t* rom_llc_ext_adv_get_created_instance_by_handle(uint8_t Advertising_Handle)
{
    //check existing
    uint32_t i;
    for(i = 0; i < g_u8LlcExtAdvMaxNum; i++)
    {
        stLlcExtAdv_t *pstAdv = &g_pstLlcExtAdvMem[i];
        if (pstAdv->bCreated && (Advertising_Handle == pstAdv->Advertising_Handle))
        {
            return pstAdv;
        }
    }

    return NULL;
}


stLlcAdvChannelPdu_t* rom_llc_ext_adv_alloc_adv_channel_pdu(  uint16_t  u16RequestPayloadSize, bool bWithExtraInfo )
{
    //alloc pdu
    stLlcAdvChannelPdu_t* pstPdu = rom_llc_adv_channel_pdu_alloc(u16RequestPayloadSize);
    if (NULL == pstPdu)
    {
        return NULL;
    }

    if (bWithExtraInfo)
    {
        //alloc extra info
        stLlcExtAdvPduExtraInfo_t* pstExtraInfo = NULL;
        uint32_t u32Primask = __get_PRIMASK();
        __disable_irq();
        uint32_t i;
        for(i = 0; i < g_u8LlcExtAdvPduExtraInfoMaxNum; i++)
        {
            if (!g_pstLlcExtAdvPduExtraInfoMem[i].bMemUsed)
            {
                pstExtraInfo = &g_pstLlcExtAdvPduExtraInfoMem[i];
                pstExtraInfo->bMemUsed = true;
                break;
            }
        }
        __set_PRIMASK(u32Primask);
        if (NULL == pstExtraInfo)
        {
            rom_llc_adv_channel_pdu_free(pstPdu);
            return NULL;
        }
        pstPdu->u32Reserved = (uint32_t)pstExtraInfo;
    }
    else
    {
        pstPdu->u32Reserved = (uint32_t)NULL;
    }

    pstPdu->prev = NULL;
    pstPdu->next = NULL;

    return pstPdu;
}


void rom_llc_ext_adv_free_adv_channel_pdu(  stLlcAdvChannelPdu_t* pstPdu )
{
    if (NULL == pstPdu)
    {
        return;
    }

    if (NULL != pstPdu->u32Reserved)
    {
        //free extra info
        uint32_t u32Primask = __get_PRIMASK();
        __disable_irq();
        stLlcExtAdvPduExtraInfo_t* pstExtraInfo = (stLlcExtAdvPduExtraInfo_t*)pstPdu->u32Reserved;
        pstExtraInfo->bMemUsed = false;
        pstPdu->u32Reserved = NULL;
        __set_PRIMASK(u32Primask);
    }

    //free pdu
    rom_llc_adv_channel_pdu_free(pstPdu);
}


stLlcExtAdvPduExtraInfo_t* rom_llc_ext_adv_get_pdu_extra_info(stLlcAdvChannelPdu_t* pstPdu)
{
    if (NULL == pstPdu)
    {
        return NULL;
    }

    stLlcExtAdvPduExtraInfo_t* pstExtraInfo = (stLlcExtAdvPduExtraInfo_t*)pstPdu->u32Reserved;
    return pstExtraInfo;
}


void rom_llc_ext_adv_init(void)
{
    uint8_t u8AdvNum = rom_llc_ext_adv_get_num();
    uint8_t u8AdvIdx;
    for(u8AdvIdx = 0; u8AdvIdx < u8AdvNum; u8AdvIdx++)
    {
        stLlcExtAdv_t* pstAdv = rom_llc_ext_adv_get_instance_by_index(u8AdvIdx);
        rom_llc_ext_adv_default_paramters_init(pstAdv);
    }
}


uint8_t rom_llc_ext_adv_get_next_primary_adv_channel_idx(uint8_t u8CurChannelIdx, uint8_t u8ChannelFlags)
{
    if ((37 != u8CurChannelIdx) && (38 != u8CurChannelIdx) && (39 != u8CurChannelIdx))
    {
        if (u8ChannelFlags & LLC_SPEC_5_1_HCI_ADV_CHANNEL_MAP_37)
        {
            return 37;
        }

        if (u8ChannelFlags & LLC_SPEC_5_1_HCI_ADV_CHANNEL_MAP_38)
        {
            return 38;
        }

        if (u8ChannelFlags & LLC_SPEC_5_1_HCI_ADV_CHANNEL_MAP_39)
        {
            return 39;
        }

        return 37;
    }
    else
    {
        const uint8_t u8Map[8][3] =
        {
            { 38, 39, 37},//flags == 0, treat as 37|38|39
            { 37, 37, 37},//flags == 1, 37
            { 38, 38, 38},//flags == 2, 38
            { 38, 37, 37},//flags == 3, 37|38
            { 39, 39, 39},//flags == 4, 39
            { 39, 37, 37},//flags == 5, 37|39
            { 38, 39, 38},//flags == 6, 38|39
            { 38, 39, 37},//flags == 7, 37|38|39
        };

        //pstAdv->u8CurChannelIdx should be 37 or 38 or 39 now

        uint8_t u8Row = u8ChannelFlags;
        uint8_t u8Column = u8CurChannelIdx - 37;

        return u8Map[u8Row][u8Column];
    }
}

uint8_t rom_llc_ext_adv_get_next_secondary_adv_channel_idx(stLlcExtAdv_t* pstAdv, uint8_t u8CurChannelIdx)
{
    uint8_t unmappedChannel =  rom_ble_impl_get_rand()  % 37;
    
    uint8_t bytePos = unmappedChannel/8;
    uint8_t bitPos = unmappedChannel - bytePos*8;
    
    if(pstAdv->Secondary_Advertising_Channel_Map[bytePos] & (1U<<bitPos))
    {
        //the unmappedChannel is a used channel
        return unmappedChannel;
    }
    
    
    //the unmappedChannel is an unused channel
    
    uint8_t numUsedChannels = 0;
    uint8_t i;
    bytePos = 0;
    bitPos = 0;
    for(i = 0; i < 37; i++)
    {
        if(pstAdv->Secondary_Advertising_Channel_Map[bytePos] & (1U<<bitPos))
        {
            numUsedChannels++;
        }
        
        bitPos++;
        if(8 == bitPos)
        {
            bitPos = 0;
            bytePos++;
        }
    }
    
    if(0 == numUsedChannels)
    {
        //err ret
        //unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV, __LINE__, (uint32_t)pstAdv->Secondary_Advertising_Channel_Map, (uint32_t)u8CurChannelIdx);
        return 0;
    }
    
    uint8_t remappingIndex =  unmappedChannel % numUsedChannels;
    
    bytePos = 0;
    bitPos = 0;
    uint8_t usedChannelIndex = 0;
    for(i = 0; i < 37; i++)
    {
        if(pstAdv->Secondary_Advertising_Channel_Map[bytePos] & (1U<<bitPos))
        {
            if(usedChannelIndex == remappingIndex)
            {
                return i;
            }
            
            usedChannelIndex++;
        }
        
        bitPos++;
        if(8 == bitPos)
        {
            bitPos = 0;
            bytePos++;
        }
    }
    //err ret
    //unexpected situation
    system_error(SYS_ERR_MODULE_LLC_EXT_ADV, __LINE__, (uint32_t)pstAdv->Secondary_Advertising_Channel_Map, (uint32_t)u8CurChannelIdx);
    return 0;
}


bool rom_llc_ext_adv_check_RPA_and_white_list_disallowed_peer_scan(stLlcExtAdv_t* pstAdv, bool bIsPeerRandomOrPublic, uint8_t pu8PeerAddr[6],
                                                                   uint8_t* Scanner_Address_Type, uint8_t Scanner_Address[6])
{
    bool bIsWhiteListUsed = ( LLC_SPEC_5_1_HCI_ADV_FILTER_POLICY_CONN_ALL_SCAN_WHITELIST == pstAdv->Advertising_Filter_Policy )
                            || ( LLC_SPEC_5_1_HCI_ADV_FILTER_POLICY_CONN_WHITELIST_SCAN_WHITELIST == pstAdv->Advertising_Filter_Policy );
    bool bDirectAdv = LLC_EXT_ADV_IS_DIRECTED ( pstAdv->Advertising_Event_Properties );
    uint8_t u8PeerAddrType;
    if(rom_llc_privacy_is_resolving_enabled())
    {
        if((false == bIsPeerRandomOrPublic) || ( bIsPeerRandomOrPublic && rom_llc_privacy_is_addr_static_random_addr(pu8PeerAddr)) ) //identify address
        {
            uint8_t privacy_mode = 0;
            uint8_t irk_is_all_zero = 0;

            u8PeerAddrType = (bIsPeerRandomOrPublic) ? (LLC_SPEC_5_1_PEER_IDENTITY_ADDRESS_TYPE_RANDOM_IDENTITY) : (LLC_SPEC_5_1_PEER_IDENTITY_ADDRESS_TYPE_PUBLIC_IDENTITY);
            bool ret = rom_llc_privacy_is_device_in_resolving_list( u8PeerAddrType, pu8PeerAddr, &privacy_mode, &irk_is_all_zero);
            if(ret)
            {
                if ( !irk_is_all_zero && ( PRIVACY_NETWORK_MODE == privacy_mode ) )
                {
                    //network mode, peer irk not null, peer addr should not be identy addr
                    return true;
                }
            }

            u8PeerAddrType = ( bIsPeerRandomOrPublic ) ? ( LLC_SPEC_5_1_HCI_ADDRESS_TYPE_RANDOM ) : ( LLC_SPEC_5_1_HCI_ADDRESS_TYPE_PUBLIC );

            if ( bDirectAdv )
            {
                if ( ( LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RESOLVABLE_OR_RANDOM == pstAdv->Own_Address_Type ) ||
                     ( LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RESOLVABLE_OR_PUBLIC == pstAdv->Own_Address_Type ) )
                {

                    if ( ( 0 != memcmp ( pu8PeerAddr, pstAdv->pu8PeerIdentifyAddr, 6 ) ) ||
                         ( pstAdv->u8PeerIdentifyAddrType !=  u8PeerAddrType ) )
                    {

                        return true;
                    }

                }
                else
                {

                    if ( ( 0 != memcmp ( pstAdv->pu8PeerAddress, pu8PeerAddr, 6 ) ) ||
                         ( u8PeerAddrType !=  pstAdv->u8PeerAddrType ) )
                    {
                        return true;
                    }
                }
                *Scanner_Address_Type = ( bIsPeerRandomOrPublic ) ? ( LLC_SPEC_5_1_HCI_SCANNER_ADDRESS_TYPE_RANDOM ) : ( LLC_SPEC_5_1_HCI_SCANNER_ADDRESS_TYPE_PUBLIC );
                memcpy ( Scanner_Address, pu8PeerAddr, 6 );
                return false;

            }
            else if ( bIsWhiteListUsed && !rom_llc_white_list_is_device_in_list ( u8PeerAddrType, pu8PeerAddr ) )
            {
                return true;
            }
            else
            {
                *Scanner_Address_Type = (bIsPeerRandomOrPublic) ? (LLC_SPEC_5_1_HCI_SCANNER_ADDRESS_TYPE_RANDOM) : (LLC_SPEC_5_1_HCI_SCANNER_ADDRESS_TYPE_PUBLIC);
                memcpy(Scanner_Address, pu8PeerAddr, 6);
                return false;
            }
        }
        else if( bIsPeerRandomOrPublic && rom_llc_privacy_is_addr_RPA(pu8PeerAddr)) //RPA
        {
            uint8_t pu8PeerIdentifyAddr[6];
            uint8_t u8PeerIdentifyAddrType;
            if( rom_llc_privacy_resolve_peer_RPA(pu8PeerAddr, pu8PeerIdentifyAddr, &u8PeerIdentifyAddrType))
            {
                //resolve success

                if ( bDirectAdv )
                {
                    if ( ( LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RESOLVABLE_OR_RANDOM == pstAdv->Own_Address_Type ) ||
                         ( LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RESOLVABLE_OR_PUBLIC == pstAdv->Own_Address_Type ) )
                    {

                        if ( ( 0 != memcmp ( pu8PeerIdentifyAddr, pstAdv->pu8PeerIdentifyAddr, 6 ) ) ||
                             ( pstAdv->u8PeerIdentifyAddrType !=  u8PeerIdentifyAddrType ) )
                        {

                            return true;
                        }

                    }
                    else
                    {

                        if ( ( 0 != memcmp ( pstAdv->pu8PeerAddress, pu8PeerIdentifyAddr, 6 ) ) ||
                             ( u8PeerIdentifyAddrType !=  pstAdv->u8PeerAddrType ) )
                        {
                            return true;
                        }
                    }
                    if ( LLC_SPEC_5_1_PEER_IDENTITY_ADDRESS_TYPE_RANDOM_IDENTITY == u8PeerIdentifyAddrType )
                    {
                        *Scanner_Address_Type = LLC_SPEC_5_1_HCI_SCANNER_ADDRESS_TYPE_RANDOM_IDENTITY;
                    }
                    else
                    {
                        *Scanner_Address_Type = LLC_SPEC_5_1_HCI_SCANNER_ADDRESS_TYPE_PUBLIC_IDENTITY;
                    }

                    memcpy ( Scanner_Address, pu8PeerIdentifyAddr, 6 );

                    return false;

                }
                else if ( bIsWhiteListUsed )
                {
                    if (LLC_SPEC_5_1_PEER_IDENTITY_ADDRESS_TYPE_RANDOM_IDENTITY == u8PeerIdentifyAddrType)
                    {
                        u8PeerAddrType = LLC_SPEC_5_1_HCI_ADDRESS_TYPE_RANDOM;
                    }
                    else
                    {
                        u8PeerAddrType = LLC_SPEC_5_1_HCI_ADDRESS_TYPE_PUBLIC;
                    }

                    if (!rom_llc_white_list_is_device_in_list(u8PeerAddrType, pu8PeerIdentifyAddr))
                    {
                        return true;
                    }

                    if (LLC_SPEC_5_1_PEER_IDENTITY_ADDRESS_TYPE_RANDOM_IDENTITY == u8PeerIdentifyAddrType)
                    {
                        *Scanner_Address_Type = LLC_SPEC_5_1_HCI_SCANNER_ADDRESS_TYPE_RANDOM_IDENTITY;
                    }
                    else
                    {
                        *Scanner_Address_Type = LLC_SPEC_5_1_HCI_SCANNER_ADDRESS_TYPE_PUBLIC_IDENTITY;
                    }

                    memcpy(Scanner_Address, pu8PeerIdentifyAddr, 6);

                    return false;
                }
            }
        }
    }

    u8PeerAddrType = (bIsPeerRandomOrPublic) ? (LLC_SPEC_5_1_HCI_ADDRESS_TYPE_RANDOM) : (LLC_SPEC_5_1_HCI_ADDRESS_TYPE_PUBLIC);
    if (bIsWhiteListUsed && !rom_llc_white_list_is_device_in_list(u8PeerAddrType, pu8PeerAddr))
    {
        return true;
    }
    else
    {
        *Scanner_Address_Type = (bIsPeerRandomOrPublic) ? (LLC_SPEC_5_1_HCI_SCANNER_ADDRESS_TYPE_RANDOM) : (LLC_SPEC_5_1_HCI_SCANNER_ADDRESS_TYPE_PUBLIC);
        memcpy(Scanner_Address, pu8PeerAddr, 6);
        return false;
    }
}


bool rom_llc_ext_adv_check_RPA_and_white_list_disallowed_peer_connect(stLlcExtAdv_t* pstAdv, bool bIsPeerRandomOrPublic, uint8_t pu8PeerAddr[6])
{
    bool bDirectAdv = LLC_EXT_ADV_IS_DIRECTED(pstAdv->Advertising_Event_Properties);
    bool bIsWhiteListUsed = (LLC_SPEC_5_1_HCI_ADV_FILTER_POLICY_CONN_WHITELIST_SCAN_ALL == pstAdv->Advertising_Filter_Policy)
                            || (LLC_SPEC_5_1_HCI_ADV_FILTER_POLICY_CONN_WHITELIST_SCAN_WHITELIST == pstAdv->Advertising_Filter_Policy);


    uint8_t resolving_success = 0;
    uint8_t u8PeerAddrType = (bIsPeerRandomOrPublic) ? (LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RANDOM) : (LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_PUBLIC);
    pstAdv->bPeerRPA_Resolved = false;
    if(rom_llc_privacy_is_resolving_enabled())
    {
        if((false == bIsPeerRandomOrPublic) || ( bIsPeerRandomOrPublic && rom_llc_privacy_is_addr_static_random_addr(pu8PeerAddr)) )
        {
            //identify address
            uint8_t privacy_mode = 0;
            uint8_t irk_is_all_zero = 0;
            bool ret = rom_llc_privacy_is_device_in_resolving_list( u8PeerAddrType,   pu8PeerAddr, &privacy_mode, &irk_is_all_zero);
            if(ret)
            {
                if(!irk_is_all_zero && (PRIVACY_NETWORK_MODE == privacy_mode))
                {
                    //network mode, peer irk not null, peer addr should not be identy addr
                    return true;
                }
            }
            if((bIsWhiteListUsed) && (false == bDirectAdv))
            {
                return !rom_llc_white_list_is_device_in_list(u8PeerAddrType, pu8PeerAddr);
            }


        }
        else if( bIsPeerRandomOrPublic && rom_llc_privacy_is_addr_RPA(pu8PeerAddr)) //RPA
        {
            uint8_t u8RslAddrType;
            uint8_t pu8RslAddr[6];
            if( rom_llc_privacy_resolve_peer_RPA(pu8PeerAddr, pu8RslAddr, &u8RslAddrType))
            {
                //resolve success
                pstAdv->bPeerRPA_Resolved = true;
                if (bDirectAdv && ((LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RESOLVABLE_OR_RANDOM == pstAdv->Own_Address_Type) ||
                                   (LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RESOLVABLE_OR_PUBLIC == pstAdv->Own_Address_Type)))
                {

                    if((0 == memcmp(pu8RslAddr, pstAdv->pu8PeerIdentifyAddr, 6)) &&
                       (pstAdv->u8PeerIdentifyAddrType ==  u8RslAddrType))
                    {
                        return false;
                    }
                    else
                    {
                        return true;
                    }

                }
                else
                {

                    pstAdv->u8PeerIdentifyAddrType = u8RslAddrType;
                    memcpy(pstAdv->pu8PeerIdentifyAddr, pu8RslAddr, 6); //save
                    if (bDirectAdv)
                    {
                        if((0 == memcmp(pstAdv->pu8PeerAddress, pstAdv->pu8PeerIdentifyAddr, 6)) &&
                           (pstAdv->u8PeerIdentifyAddrType ==  pstAdv->u8PeerAddrType))
                        {
                            return false;
                        }
                        else
                        {
                            return true;
                        }

                    }

                }
                if((bIsWhiteListUsed) && (false == bDirectAdv))
                {
                    return !rom_llc_white_list_is_device_in_list(pstAdv->u8PeerIdentifyAddrType, pstAdv->pu8PeerIdentifyAddr);
                }
            }

        }

    }
    if(bDirectAdv)
    {
        if ((0 == memcmp(pstAdv->pu8PeerAddress, pu8PeerAddr, 6)) && (u8PeerAddrType ==  pstAdv->u8PeerAddrType))
        {
            return false;
        }
        return true;
    }

    if (bIsWhiteListUsed)
    {
        return !rom_llc_white_list_is_device_in_list(u8PeerAddrType, pu8PeerAddr);
    }
    else
    {
        return false;
    }

}


void rom_llc_ext_adv_default_paramters_init(stLlcExtAdv_t* pstAdv)
{
    if (NULL == pstAdv)
    {
        return;
    }

    pstAdv->bEnabled = false;
    pstAdv->bCreated = false;
    pstAdv->bAdvertisingRandomAddressSet = false;
    pstAdv->bCreateSlaveAfterFragmentStopped = false;
    pstAdv->bAcceptUpdatingPdus = false;
    pstAdv->bUpdateAcceptedPdusADI = false;
    pstAdv->bUpdateAcceptedPdusAddress = false;

    pstAdv->u16AdvertisingDataLength = 0;
    pstAdv->u16ScanResponseDataLength = 0;
    pstAdv->u16CurrentDID = 0;

    pstAdv->pstPrimaryAdvertisingPdu = NULL;
    pstAdv->pstSecondaryAdvertisingPdu = NULL;
    pstAdv->pstResponsePdu = NULL;
    pstAdv->pstChainPdusHead = NULL;
    pstAdv->pstChainPdusTail = NULL;

    pstAdv->pstCurrentChainPdu = NULL;

    pstAdv->pstUpdatingPrimaryAdvertisingPdu = NULL;
    pstAdv->pstUpdatingSecondaryAdvertisingPdu = NULL;
    pstAdv->pstUpdatingResponsePdu = NULL;
    pstAdv->pstUpdatingChainPdusHead = NULL;
    pstAdv->pstUpdatingChainPdusTail = NULL;


    pstAdv->u16RxPduPaylength = sizeof(pstAdv->pu8RxPduPayload);
    pstAdv->stRxPduPtr.pu32Header = &pstAdv->u32RxPduHeader;
    pstAdv->stRxPduPtr.pu16PayloadBufSize = &pstAdv->u16RxPduPaylength;
    pstAdv->stRxPduPtr.pu8Payload = pstAdv->pu8RxPduPayload;
}


uint8_t rom_llc_ext_adv_refresh_pdus(stLlcExtAdv_t* pstAdv)
{
    if (NULL == pstAdv)
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV, __LINE__, 0, 0);
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }
    
    if (NULL == pstAdv->pstPrimaryAdvertisingPdu)
    {
        return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
    }
    
    //take all pdus
    stLlcAdvChannelPdu_t* pstPrimaryAdvertisingPdu = pstAdv->pstPrimaryAdvertisingPdu;
    stLlcAdvChannelPdu_t* pstSecondaryAdvertisingPdu = pstAdv->pstSecondaryAdvertisingPdu;
    stLlcAdvChannelPdu_t* pstResponsePdu = pstAdv->pstResponsePdu;
    stLlcAdvChannelPdu_t* pstChainPdusHead = pstAdv->pstChainPdusHead;
    stLlcAdvChannelPdu_t* pstChainPdusTail = pstAdv->pstChainPdusTail;
    pstAdv->u16AdvertisingDataLength = 0;
    pstAdv->u16ScanResponseDataLength = 0;
    pstAdv->pstPrimaryAdvertisingPdu = NULL;
    pstAdv->pstSecondaryAdvertisingPdu = NULL;
    pstAdv->pstResponsePdu = NULL;
    pstAdv->pstChainPdusHead = NULL;
    pstAdv->pstChainPdusTail = NULL;
    
    
    stLlcSpec51AdvChannelPduHeader_t stHeader;
    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat;
    uint8_t u8ErrorCode;
    stLlcAdvChannelPdu_t* pstChainPdu;
    
    stHeader.u16Value = pstPrimaryAdvertisingPdu->u32Header & 0xFFFF;
    if (LLC_SPEC_5_1_ADV_CH_PDU_TYPE_ADV_EXT_IND == stHeader.bitField.PDUType)
    {
        //extended adv
        
        if(!rom_llc_common_extended_advertising_payload_parse(pstSecondaryAdvertisingPdu, &stFormat))
        {
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV, __LINE__, pstAdv->Advertising_Handle, (uint32_t)pstPrimaryAdvertisingPdu);
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }
        
        if (NULL == pstChainPdusHead)
        {
            //extended adv without chains pdu
            
            //refresh adv data (may be NULL)
            u8ErrorCode = rom_llc_ext_adv_set_advertising_data(pstAdv->Advertising_Handle, LLC_SPEC_5_1_HCI_ADV_DATA_OPERATION_COMPLETE, 
                LLC_SPEC_5_1_HCI_FRAGMENT_PREFERENCE_MAY_NOT_FRAGMENT, stFormat.AdvData_Length, stFormat.AdvData);
            if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrorCode)
            {
                //release pdus
                rom_llc_adv_channel_pdu_free(pstPrimaryAdvertisingPdu);
                rom_llc_adv_channel_pdu_free(pstSecondaryAdvertisingPdu);
                rom_llc_adv_channel_pdu_free(pstResponsePdu);
                pstChainPdu = pstChainPdusHead;
                while(NULL != pstChainPdu)
                {
                    stLlcAdvChannelPdu_t* pstFreePdu = pstChainPdu;
                    pstChainPdu = pstChainPdu->next;
                    
                    rom_llc_adv_channel_pdu_free(pstFreePdu);
                }
                
                return u8ErrorCode;
            }
            
            //refresh rsp data(if exists)
            if (NULL != pstResponsePdu)
            {
                if(!rom_llc_common_extended_advertising_payload_parse(pstResponsePdu, &stFormat))
                {
                    system_error(SYS_ERR_MODULE_LLC_EXT_ADV, __LINE__, pstAdv->Advertising_Handle, (uint32_t)pstResponsePdu);
                    return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
                }
                
                u8ErrorCode = rom_llc_ext_adv_set_scan_response_data(pstAdv->Advertising_Handle, LLC_SPEC_5_1_HCI_SCAN_RESPONSE_DATA_OPERATION_COMPLETE, 
                LLC_SPEC_5_1_HCI_FRAGMENT_PREFERENCE_MAY_NOT_FRAGMENT, stFormat.AdvData_Length, stFormat.AdvData);
                if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrorCode)
                {
                    //release pdus
                    rom_llc_adv_channel_pdu_free(pstPrimaryAdvertisingPdu);
                    rom_llc_adv_channel_pdu_free(pstSecondaryAdvertisingPdu);
                    rom_llc_adv_channel_pdu_free(pstResponsePdu);
                    pstChainPdu = pstChainPdusHead;
                    while(NULL != pstChainPdu)
                    {
                        stLlcAdvChannelPdu_t* pstFreePdu = pstChainPdu;
                        pstChainPdu = pstChainPdu->next;
                        
                        rom_llc_adv_channel_pdu_free(pstFreePdu);
                    }
                    
                    return u8ErrorCode;
                }
            }
            
            //release pdus
            rom_llc_adv_channel_pdu_free(pstPrimaryAdvertisingPdu);
            rom_llc_adv_channel_pdu_free(pstSecondaryAdvertisingPdu);
            rom_llc_adv_channel_pdu_free(pstResponsePdu);
            pstChainPdu = pstChainPdusHead;
            while(NULL != pstChainPdu)
            {
                stLlcAdvChannelPdu_t* pstFreePdu = pstChainPdu;
                pstChainPdu = pstChainPdu->next;
                
                rom_llc_adv_channel_pdu_free(pstFreePdu);
            }
            
            return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
            
        }
        else
        {
            //extended adv with chains pdu
            
            if (stFormat.Scannable)
            {
                //extended adv with chains pdu, scannable
                
                //refresh adv data (NULL since scannable)
                u8ErrorCode = rom_llc_ext_adv_set_advertising_data(pstAdv->Advertising_Handle, LLC_SPEC_5_1_HCI_ADV_DATA_OPERATION_COMPLETE, 
                    LLC_SPEC_5_1_HCI_FRAGMENT_PREFERENCE_MAY_NOT_FRAGMENT, 0, NULL);
                if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrorCode)
                {
                    //release pdus
                    rom_llc_adv_channel_pdu_free(pstPrimaryAdvertisingPdu);
                    rom_llc_adv_channel_pdu_free(pstSecondaryAdvertisingPdu);
                    rom_llc_adv_channel_pdu_free(pstResponsePdu);
                    pstChainPdu = pstChainPdusHead;
                    while(NULL != pstChainPdu)
                    {
                        stLlcAdvChannelPdu_t* pstFreePdu = pstChainPdu;
                        pstChainPdu = pstChainPdu->next;
                        
                        rom_llc_adv_channel_pdu_free(pstFreePdu);
                    }
                    
                    return u8ErrorCode;
                }
                
                //refresh rsp data(exists since chain pdus exists)
                if(!rom_llc_common_extended_advertising_payload_parse(pstResponsePdu, &stFormat))
                {
                    system_error(SYS_ERR_MODULE_LLC_EXT_ADV, __LINE__, pstAdv->Advertising_Handle, (uint32_t)pstResponsePdu);
                    return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
                }
                u8ErrorCode = rom_llc_ext_adv_set_scan_response_data(pstAdv->Advertising_Handle, LLC_SPEC_5_1_HCI_SCAN_RESPONSE_DATA_OPERATION_FIRST_FRAGMENT,
                LLC_SPEC_5_1_HCI_FRAGMENT_PREFERENCE_MAY_FRAGMENT, stFormat.AdvData_Length, stFormat.AdvData);
                if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrorCode)
                {
                    //release pdus
                    rom_llc_adv_channel_pdu_free(pstPrimaryAdvertisingPdu);
                    rom_llc_adv_channel_pdu_free(pstSecondaryAdvertisingPdu);
                    rom_llc_adv_channel_pdu_free(pstResponsePdu);
                    pstChainPdu = pstChainPdusHead;
                    while(NULL != pstChainPdu)
                    {
                        stLlcAdvChannelPdu_t* pstFreePdu = pstChainPdu;
                        pstChainPdu = pstChainPdu->next;
                        
                        rom_llc_adv_channel_pdu_free(pstFreePdu);
                    }
                    
                    return u8ErrorCode;
                }
                
                //refresh chain pdus
                pstChainPdu = pstChainPdusHead;
                while(NULL != pstChainPdu)
                {
                    if(!rom_llc_common_extended_advertising_payload_parse(pstChainPdu, &stFormat))
                    {
                        system_error(SYS_ERR_MODULE_LLC_EXT_ADV, __LINE__, pstAdv->Advertising_Handle, (uint32_t)pstChainPdu);
                        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
                    }
                    
                    if(NULL == pstChainPdu->next)
                    {
                        u8ErrorCode = rom_llc_ext_adv_set_scan_response_data(pstAdv->Advertising_Handle, LLC_SPEC_5_1_HCI_SCAN_RESPONSE_DATA_OPERATION_LAST_FRAGMENT,
                            LLC_SPEC_5_1_HCI_FRAGMENT_PREFERENCE_MAY_FRAGMENT, stFormat.AdvData_Length, stFormat.AdvData);
                    }
                    else
                    {
                        u8ErrorCode = rom_llc_ext_adv_set_scan_response_data(pstAdv->Advertising_Handle, LLC_SPEC_5_1_HCI_SCAN_RESPONSE_DATA_OPERATION_INTERMEDIATE_FRAGMENT,
                            LLC_SPEC_5_1_HCI_FRAGMENT_PREFERENCE_MAY_FRAGMENT, stFormat.AdvData_Length, stFormat.AdvData);
                    }
                    if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrorCode)
                    {
                        //release pdus
                        rom_llc_adv_channel_pdu_free(pstPrimaryAdvertisingPdu);
                        rom_llc_adv_channel_pdu_free(pstSecondaryAdvertisingPdu);
                        rom_llc_adv_channel_pdu_free(pstResponsePdu);
                        pstChainPdu = pstChainPdusHead;
                        while(NULL != pstChainPdu)
                        {
                            stLlcAdvChannelPdu_t* pstFreePdu = pstChainPdu;
                            pstChainPdu = pstChainPdu->next;
                            
                            rom_llc_adv_channel_pdu_free(pstFreePdu);
                        }
                        
                        return u8ErrorCode;
                    }
                    
                    pstChainPdu = pstChainPdu->next;
                }
                
                //release pdus
                rom_llc_adv_channel_pdu_free(pstPrimaryAdvertisingPdu);
                rom_llc_adv_channel_pdu_free(pstSecondaryAdvertisingPdu);
                rom_llc_adv_channel_pdu_free(pstResponsePdu);
                pstChainPdu = pstChainPdusHead;
                while(NULL != pstChainPdu)
                {
                    stLlcAdvChannelPdu_t* pstFreePdu = pstChainPdu;
                    pstChainPdu = pstChainPdu->next;
                    
                    rom_llc_adv_channel_pdu_free(pstFreePdu);
                }
                
                return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
            }
            else
            {
                //extended adv with chains pdu, non-scannable
                
                //refresh adv data (exits since chain pdus exists)
                u8ErrorCode = rom_llc_ext_adv_set_advertising_data(pstAdv->Advertising_Handle, LLC_SPEC_5_1_HCI_ADV_DATA_OPERATION_FIRST_FRAGMENT, 
                    LLC_SPEC_5_1_HCI_FRAGMENT_PREFERENCE_MAY_FRAGMENT, stFormat.AdvData_Length, stFormat.AdvData);
                if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrorCode)
                {
                    //release pdus
                    rom_llc_adv_channel_pdu_free(pstPrimaryAdvertisingPdu);
                    rom_llc_adv_channel_pdu_free(pstSecondaryAdvertisingPdu);
                    rom_llc_adv_channel_pdu_free(pstResponsePdu);
                    pstChainPdu = pstChainPdusHead;
                    while(NULL != pstChainPdu)
                    {
                        stLlcAdvChannelPdu_t* pstFreePdu = pstChainPdu;
                        pstChainPdu = pstChainPdu->next;
                        
                        rom_llc_adv_channel_pdu_free(pstFreePdu);
                    }
                    
                    return u8ErrorCode;
                }
                
                //refresh chain pdus
                pstChainPdu = pstChainPdusHead;
                while(NULL != pstChainPdu)
                {
                    if(!rom_llc_common_extended_advertising_payload_parse(pstChainPdu, &stFormat))
                    {
                        system_error(SYS_ERR_MODULE_LLC_EXT_ADV, __LINE__, pstAdv->Advertising_Handle, (uint32_t)pstChainPdu);
                        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
                    }
                    
                    if(NULL == pstChainPdu->next)
                    {
                        u8ErrorCode = rom_llc_ext_adv_set_advertising_data(pstAdv->Advertising_Handle, LLC_SPEC_5_1_HCI_ADV_DATA_OPERATION_LAST_FRAGMENT,
                            LLC_SPEC_5_1_HCI_FRAGMENT_PREFERENCE_MAY_FRAGMENT, stFormat.AdvData_Length, stFormat.AdvData);
                    }
                    else
                    {
                        u8ErrorCode = rom_llc_ext_adv_set_advertising_data(pstAdv->Advertising_Handle, LLC_SPEC_5_1_HCI_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT,
                            LLC_SPEC_5_1_HCI_FRAGMENT_PREFERENCE_MAY_FRAGMENT, stFormat.AdvData_Length, stFormat.AdvData);
                    }
                    if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrorCode)
                    {
                        //release pdus
                        rom_llc_adv_channel_pdu_free(pstPrimaryAdvertisingPdu);
                        rom_llc_adv_channel_pdu_free(pstSecondaryAdvertisingPdu);
                        rom_llc_adv_channel_pdu_free(pstResponsePdu);
                        pstChainPdu = pstChainPdusHead;
                        while(NULL != pstChainPdu)
                        {
                            stLlcAdvChannelPdu_t* pstFreePdu = pstChainPdu;
                            pstChainPdu = pstChainPdu->next;
                            
                            rom_llc_adv_channel_pdu_free(pstFreePdu);
                        }
                        
                        return u8ErrorCode;
                    }
                    
                    pstChainPdu = pstChainPdu->next;
                }
                
                //release pdus
                rom_llc_adv_channel_pdu_free(pstPrimaryAdvertisingPdu);
                rom_llc_adv_channel_pdu_free(pstSecondaryAdvertisingPdu);
                rom_llc_adv_channel_pdu_free(pstResponsePdu);
                pstChainPdu = pstChainPdusHead;
                while(NULL != pstChainPdu)
                {
                    stLlcAdvChannelPdu_t* pstFreePdu = pstChainPdu;
                    pstChainPdu = pstChainPdu->next;
                    
                    rom_llc_adv_channel_pdu_free(pstFreePdu);
                }
                
                return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
            }
        }
    }
    else
    {
        //legacy adv
        uint8_t u8AdvDataLength = 0;
        uint8_t* pu8AdvData = NULL;
        uint8_t u8ScanResponseDataLength = 0;
        uint8_t* pu8ScanResponseData = NULL;
        
        switch(stHeader.bitField.PDUType)
        {
            case LLC_SPEC_5_1_ADV_CH_PDU_TYPE_ADV_IND:
            case LLC_SPEC_5_1_ADV_CH_PDU_TYPE_ADV_SCAN_IND:
            {
                //6 : AdvA
                if (stHeader.bitField.Length < 6)
                {
                    system_error(SYS_ERR_MODULE_LLC_EXT_ADV, __LINE__, pstAdv->Advertising_Handle, stHeader.bitField.PDUType);
                    return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
                }
                
                u8AdvDataLength = stHeader.bitField.Length - 6;
                pu8AdvData = &(pstPrimaryAdvertisingPdu->pu8Payload[6]);
                
                if (NULL != pstResponsePdu)
                {
                    //PDUType check skipped
                    stHeader.u16Value = pstResponsePdu->u32Header & 0xFFFF;
                    if (stHeader.bitField.Length < 6)
                    {
                        system_error(SYS_ERR_MODULE_LLC_EXT_ADV, __LINE__, pstAdv->Advertising_Handle, stHeader.bitField.PDUType);
                        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
                    }
                    
                    u8ScanResponseDataLength = stHeader.bitField.Length - 6;
                    pu8ScanResponseData = &(pstResponsePdu->pu8Payload[6]);
                }
            }
                break;
            
            case LLC_SPEC_5_1_ADV_CH_PDU_TYPE_DIRECT_IND:
            {
                //no data
            }
                break;
            
            case LLC_SPEC_5_1_ADV_CH_PDU_TYPE_NONCONN_IND:
            {
                //6 : AdvA
                if (stHeader.bitField.Length < 6)
                {
                    system_error(SYS_ERR_MODULE_LLC_EXT_ADV, __LINE__, pstAdv->Advertising_Handle, stHeader.bitField.PDUType);
                    return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
                }
                
                u8AdvDataLength = stHeader.bitField.Length - 6;
                pu8AdvData = &(pstPrimaryAdvertisingPdu->pu8Payload[6]);
            }
                break;
            
            default:
            {
                system_error(SYS_ERR_MODULE_LLC_EXT_ADV, __LINE__, pstAdv->Advertising_Handle, stHeader.bitField.PDUType);
                return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
            }
        }
        
        u8ErrorCode = rom_llc_ext_adv_set_advertising_data(pstAdv->Advertising_Handle, LLC_SPEC_5_1_HCI_ADV_DATA_OPERATION_COMPLETE, 
                    LLC_SPEC_5_1_HCI_FRAGMENT_PREFERENCE_MAY_NOT_FRAGMENT, u8AdvDataLength, pu8AdvData);
        if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrorCode)
        {
            //release pdus
            rom_llc_adv_channel_pdu_free(pstPrimaryAdvertisingPdu);
            rom_llc_adv_channel_pdu_free(pstSecondaryAdvertisingPdu);
            rom_llc_adv_channel_pdu_free(pstResponsePdu);
            pstChainPdu = pstChainPdusHead;
            while(NULL != pstChainPdu)
            {
                stLlcAdvChannelPdu_t* pstFreePdu = pstChainPdu;
                pstChainPdu = pstChainPdu->next;
                
                rom_llc_adv_channel_pdu_free(pstFreePdu);
            }
            
            return u8ErrorCode;
        }
        
        u8ErrorCode = rom_llc_ext_adv_set_scan_response_data(pstAdv->Advertising_Handle, LLC_SPEC_5_1_HCI_SCAN_RESPONSE_DATA_OPERATION_COMPLETE,
        LLC_SPEC_5_1_HCI_FRAGMENT_PREFERENCE_MAY_NOT_FRAGMENT, u8ScanResponseDataLength, pu8ScanResponseData);
        
        //release pdus
        rom_llc_adv_channel_pdu_free(pstPrimaryAdvertisingPdu);
        rom_llc_adv_channel_pdu_free(pstSecondaryAdvertisingPdu);
        rom_llc_adv_channel_pdu_free(pstResponsePdu);
        pstChainPdu = pstChainPdusHead;
        while(NULL != pstChainPdu)
        {
            stLlcAdvChannelPdu_t* pstFreePdu = pstChainPdu;
            pstChainPdu = pstChainPdu->next;
            
            rom_llc_adv_channel_pdu_free(pstFreePdu);
        }
        
        return u8ErrorCode;
    }
}


uint8_t rom_llc_ext_adv_set_advertising_set_random_address(uint8_t Advertising_Handle, uint8_t Advertising_Random_Address[6])
{
    if (NULL == Advertising_Random_Address)
    {
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }

    stLlcExtAdv_t* pstAdv = rom_llc_ext_adv_get_created_instance_by_handle(Advertising_Handle);
    if (NULL == pstAdv)
    {
        return LLC_SPEC_5_1_ERROR_CODE_UNKNOWN_ADVERTISING_IDENTIFIER;
    }

//If the Host issues this command while an advertising set using connectable
//advertising is enabled, the Controller shall return the error code Command
//Disallowed (0x0C). The Host may issue this command at any other time
    if (pstAdv->bEnabled && (LLC_EXT_ADV_IS_CONNECTABLE(pstAdv->Advertising_Event_Properties)))
    {
        return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
    }


//If this command is used to change the address, the new random address shall
//take effect for advertising no later than the next successful
//HCI_LE_Extended_Set_Advertising_Enable command and for periodic
//advertising no later than the next successful
//HCI_LE_Periodic_Advertising_Enable command.
    memcpy(pstAdv->Advertising_Random_Address, Advertising_Random_Address, 6);
    pstAdv->bAdvertisingRandomAddressSet = true;
    
    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}


uint8_t rom_llc_ext_adv_set_parameters(uint8_t Advertising_Handle,
                                       uint16_t Advertising_Event_Properties,
                                       uint32_t Primary_Advertising_Interval_Min,
                                       uint32_t Primary_Advertising_Interval_Max,
                                       uint8_t Primary_Advertising_Channel_Map,
                                       uint8_t Secondary_Advertising_Channel_Map[5],
                                       uint8_t Own_Address_Type,
                                       uint8_t Peer_Address_Type,
                                       uint8_t Peer_Address[6],
                                       uint8_t Advertising_Filter_Policy,
                                       int8_t Advertising_Tx_Power,
                                       uint8_t Primary_Advertising_PHY,
                                       uint8_t Secondary_Advertising_Max_Skip,
                                       uint8_t Secondary_Advertising_PHY,
                                       uint16_t PHY_options,
                                       uint8_t Advertising_SID,
                                       uint8_t Scan_Request_Notification_Enable,
                                       int8_t *ps8SelectedTxPower)
{
/*
If the Advertising_Handle does not identify an existing advertising set and the
Controller is unable to support a new advertising set at present, the Controller
shall return the error code Memory Capacity Exceeded (0x07).
*/
    stLlcExtAdv_t* pstAdv = rom_llc_ext_adv_create_instance_by_handle(Advertising_Handle);
    if (NULL == pstAdv)
    {
        return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
    }
    
/*
If the Host issues this command when advertising is enabled for the specified
advertising set, the Controller shall return the error code Command Disallowed
(0x0C).
*/
    if (pstAdv->bEnabled)
    {
        return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
    }
    
    bool bIsLegacy = LLC_EXT_ADV_IS_LEGACY(Advertising_Event_Properties);
    bool bIsConnectable = LLC_EXT_ADV_IS_CONNECTABLE(Advertising_Event_Properties);
    bool bIsScannable = LLC_EXT_ADV_IS_SCANNABLE(Advertising_Event_Properties);
    bool bIsHighDutyCycle = LLC_EXT_ADV_IS_HIGH_DUTY_CYCLE(Advertising_Event_Properties);
    bool bIsDirected = LLC_EXT_ADV_IS_DIRECTED(Advertising_Event_Properties);
    bool bIsAnonymous = LLC_EXT_ADV_IS_ANONYMOUS(Advertising_Event_Properties);
    bool bIsIncludeTxPower = LLC_EXT_ADV_IS_INCLUDE_TX_POWER(Advertising_Event_Properties);
    
/*
The Advertising_Event_Properties parameter describes the type of advertising
event that is being configured and its basic properties. The type shall be one
supported by the Controller. In particular, the following restrictions apply to this
parameter:
    • If legacy advertising PDU types are being used, then the parameter value
    shall be one of those specified in Table 7.2. If the advertising set already
    contains data, the type shall be one that supports advertising data and the
    amount of data shall not exceed 31 octets.
    • If extended advertising PDU types are being used (bit 4 = 0) then:
    The advertisement shall not be both connectable and scannable.
    High duty cycle directed connectable advertising (≤ 3.75 ms
    advertising interval) shall not be used (bit 3 = 0).

If the Advertising_Event_Properties parameter does not describe an event type
supported by the Controller, contains an invalid bit combination, or specifies a
type that does not support advertising data when the advertising set already
contains some, the Controller shall return the error code Invalid HCI Command
Parameters (0x12).
*/
    if (bIsLegacy)
    {
        switch(Advertising_Event_Properties)
        {
            case ADVERTISING_EVENT_PROPERTIES_ADV_IND:
            case ADVERTISING_EVENT_PROPERTIES_ADV_SCAN_IND:
            {
                if ((pstAdv->u16AdvertisingDataLength > 31) || (pstAdv->u16ScanResponseDataLength > 31))
                {
                    return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
                }
            }
                break;
            
            case ADVERTISING_EVENT_PROPERTIES_ADV_DIRECT_IND_LOW_DUTY_CYCLE:
            case ADVERTISING_EVENT_PROPERTIES_ADV_DIRECT_IND_HIGH_DUTY_CYCLE:
            {
                if ((pstAdv->u16AdvertisingDataLength > 0) || (pstAdv->u16ScanResponseDataLength > 0))
                {
                    return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
                }
            }
                break;
            
            case ADVERTISING_EVENT_PROPERTIES_ADV_NONCONN_IND:
            {
                if ((pstAdv->u16AdvertisingDataLength > 31) || (pstAdv->u16ScanResponseDataLength > 0))
                {
                    return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
                }
            }
                break;

            default:
                return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
    }
    else
    {
        if (bIsConnectable && bIsScannable)
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }

        if (bIsHighDutyCycle)
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
    }
    

/*
The Own_Address_Type parameter shall be ignored for anonymous
advertising (bit 5 = 1).
*/
    if (!bIsAnonymous)
    {
        if (Own_Address_Type > 0x03)
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
    }
    

/*
If Directed advertising is selected, the Peer_Address_Type and Peer_Address
shall be valid and the Advertising_Filter_Policy parameter shall be ignored.
*/
    if (bIsDirected)
    {
        if (Peer_Address_Type > 0x01)
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
    }
    else
    {
        if (Advertising_Filter_Policy > 0x03)
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
    }
    
/*
The Primary_Advertising_Interval_Min parameter shall be less than or equal to
the Primary_Advertising_Interval_Max parameter. The
Primary_Advertising_Interval_Min and Primary_Advertising_Interval_Max
parameters should not be the same value so that the Controller can choose the
best advertising interval given other activities.

For high duty cycle connectable directed advertising event type
(ADV_DIRECT_IND), the Primary_Advertising_Interval_Min and
Primary_Advertising_Interval_Max parameters are not used and shall be
ignored.
    
If the primary advertising interval range provided by the Host
(Primary_Advertising_Interval_Min, Primary_Advertising_Interval_Max) is
outside the advertising interval range supported by the Controller, then the
Controller shall return the error code Unsupported Feature or Parameter Value
(0x11).
*/
    if (ADVERTISING_EVENT_PROPERTIES_ADV_DIRECT_IND_HIGH_DUTY_CYCLE != Advertising_Event_Properties)
    {
        if ((Primary_Advertising_Interval_Min < 0x000020) || (Primary_Advertising_Interval_Min > 0xFFFFFF))
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }

        if ((Primary_Advertising_Interval_Max < 0x000020) || (Primary_Advertising_Interval_Max > 0xFFFFFF))
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }

        if (Primary_Advertising_Interval_Min > Primary_Advertising_Interval_Max)
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
    }
    
    
/*
The Primary_Advertising_Channel_Map is a bit field that indicates the
advertising channels that shall be used when transmitting advertising packets.
At least one channel bit shall be set in the Primary_Advertising_Channel_Map
parameter.
*/
    if ((0 == Primary_Advertising_Channel_Map) || (Primary_Advertising_Channel_Map > 0x7))
    {
        return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
    }
    
/*
The Advertising_Tx_Power parameter indicates the maximum power level at
which the advertising packets are to be transmitted on the advertising physical
channels. The Controller shall choose a power level lower than or equal to the
one specified by the Host.
*/
//    if (Advertising_Tx_Power > 20)
//    {
//        return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
//    }
    
    
    
/*
The parameters beginning with “Secondary” are only valid when extended
advertising PDU types are being used (bit 4 = 0).
*/
/*
The Primary_Advertising_PHY parameter indicates the PHY on which the
advertising packets are transmitted on the primary advertising physical
channel. If legacy advertising PDUs are being used, the
Primary_Advertising_PHY shall indicate the LE 1M PHY. The
Secondary_Advertising_PHY parameter indicates the PHY on which the
advertising packets are be transmitted on the secondary advertising physical
channel. If the Host specifies a PHY that is not supported by the Controller, it
should return the error code Unsupported Feature or Parameter Value (0x11).
If Constant Tone Extensions are enabled for the advertising set and
Secondary_Advertising_PHY specifies a PHY that does not allow Constant
Tone Extensions, the Controller shall return the error code Command
Disallowed (0x0C).
*/
    switch(Primary_Advertising_PHY)
    {
        case LLC_SPEC_5_1_HCI_PHY_LE_1M:
        case LLC_SPEC_5_1_HCI_PHY_LE_CODED:
            break;

        case LLC_SPEC_5_1_HCI_PHY_LE_2M:
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        
        default:
            return LLC_SPEC_5_1_ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE;
    }
    if (bIsLegacy)
    {
        if (LLC_SPEC_5_1_HCI_PHY_LE_1M != Primary_Advertising_PHY)
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
    }
    else
    {
        switch(Secondary_Advertising_PHY)
        {
            case LLC_SPEC_5_1_HCI_PHY_LE_1M:
            case LLC_SPEC_5_1_HCI_PHY_LE_2M:
            case LLC_SPEC_5_1_HCI_PHY_LE_CODED:
                break;
            
            default:
                return LLC_SPEC_5_1_ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE;
        }
    }
/*
The Secondary_Advertising_Max_Skip parameter is the maximum number of
advertising events that can be skipped before the AUX_ADV_IND can be sent.
*/
    //vailid for 0x00~0xFF
    
/*
The Advertising_SID parameter specifies the value to be transmitted in the
Advertising SID subfield of the ADI field of the Extended Header of those
advertising physical channel PDUs that have an ADI field. If the advertising set
only uses PDUs that do not contain an ADI field, Advertising_SID shall be
ignored
*/
    if (Advertising_SID > 0x0F)
    {
        return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
    }
    
/*
The Scan_Request_Notification_Enable parameter indicates whether the
Controller shall send notifications upon the receipt of a scan request PDU that
is in response to an advertisement from the specified advertising set that
contains its device address and is from a scanner that is allowed by the
advertising filter policy.
*/
    if (Scan_Request_Notification_Enable > 0x01)
    {
        return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
    }

    
// phy options
    stLlcSpec51HciPhyOptions_t stPhyOptions;
    stPhyOptions.u16Value = PHY_options;
    switch(stPhyOptions.bitField.CodedTxPrefer)
    {
        case LLC_SPEC_5_1_PHY_OPTION_CODED_TX_PREFER_NO_PREFER:
        case LLC_SPEC_5_1_PHY_OPTION_CODED_TX_PREFER_S2:
        case LLC_SPEC_5_1_PHY_OPTION_CODED_TX_PREFER_S8:
            break;
        default:
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
    }
    
    
//paramters accept
    pstAdv->Advertising_Event_Properties = Advertising_Event_Properties;
    pstAdv->Primary_Advertising_Interval_Min = Primary_Advertising_Interval_Min;
    pstAdv->Primary_Advertising_Interval_Max = Primary_Advertising_Interval_Max;
    pstAdv->Primary_Advertising_Channel_Map = Primary_Advertising_Channel_Map;
    pstAdv->Own_Address_Type = Own_Address_Type;
    pstAdv->Peer_Address_Type = Peer_Address_Type;
    memcpy(pstAdv->Peer_Address, Peer_Address, 6);
    pstAdv->Advertising_Filter_Policy = Advertising_Filter_Policy;
    pstAdv->Advertising_Tx_Power = Advertising_Tx_Power;
    pstAdv->Primary_Advertising_PHY = Primary_Advertising_PHY;
    pstAdv->Secondary_Advertising_Max_Skip = Secondary_Advertising_Max_Skip;
    pstAdv->Secondary_Advertising_PHY = Secondary_Advertising_PHY;
    pstAdv->Advertising_SID = Advertising_SID;
    pstAdv->Scan_Request_Notification_Enable = Scan_Request_Notification_Enable;
    
    if (NULL == Secondary_Advertising_Channel_Map)
    {
        pstAdv->Secondary_Advertising_Channel_Map[0] = 0xFF;
        pstAdv->Secondary_Advertising_Channel_Map[1] = 0xFF;
        pstAdv->Secondary_Advertising_Channel_Map[2] = 0xFF;
        pstAdv->Secondary_Advertising_Channel_Map[3] = 0xFF;
        pstAdv->Secondary_Advertising_Channel_Map[4] = 0x1F;
    }
    else
    {
        Secondary_Advertising_Channel_Map[4] &= 0x1F;
        if (rom_llc_tool_is_array_all_zero(Secondary_Advertising_Channel_Map, 5))
        {
            pstAdv->Secondary_Advertising_Channel_Map[0] = 0xFF;
            pstAdv->Secondary_Advertising_Channel_Map[1] = 0xFF;
            pstAdv->Secondary_Advertising_Channel_Map[2] = 0xFF;
            pstAdv->Secondary_Advertising_Channel_Map[3] = 0xFF;
            pstAdv->Secondary_Advertising_Channel_Map[4] = 0x1F;
        }
        else
        {
            memcpy(pstAdv->Secondary_Advertising_Channel_Map, Secondary_Advertising_Channel_Map, 5);
        }
    }
    
    ////todo : selected tx power
    if (NULL != ps8SelectedTxPower)
    {
        *ps8SelectedTxPower = 0;
    }
    
    bool bCodedS2Preferred = LLC_SPEC_5_1_PHY_OPTION_CODED_TX_PREFER_S2 == stPhyOptions.bitField.CodedTxPrefer;
    
    switch(pstAdv->Primary_Advertising_PHY)
    {
        case LLC_SPEC_5_1_HCI_PHY_LE_1M:
            pstAdv->enumPrimaryPhy = LL_PHY_1M;
            break;        

        case LLC_SPEC_5_1_HCI_PHY_LE_CODED:
            pstAdv->enumPrimaryPhy = bCodedS2Preferred?LL_PHY_CODED_S2:LL_PHY_CODED_S8;
            break;

        default:
            ////unexpected situation, should be check when parameters set
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }

    if (!LLC_EXT_ADV_IS_LEGACY(pstAdv->Advertising_Event_Properties))
    {
        switch(pstAdv->Secondary_Advertising_PHY)
        {
            case LLC_SPEC_5_1_HCI_PHY_LE_1M:
                pstAdv->enumSecondaryPhy = LL_PHY_1M;
                break;

            case LLC_SPEC_5_1_HCI_PHY_LE_2M:
                pstAdv->enumSecondaryPhy = LL_PHY_2M;
            break;

            case LLC_SPEC_5_1_HCI_PHY_LE_CODED:
                pstAdv->enumSecondaryPhy = bCodedS2Preferred?LL_PHY_CODED_S2:LL_PHY_CODED_S8;
                break;

            default:
                ////unexpected situation, should be check when parameters set
                return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }
    }

    pstAdv->u8ApplySecondaryAdvertisingSkip = pstAdv->Secondary_Advertising_Max_Skip;
    pstAdv->u16ClockAccuracyPpm = rom_llc_get_llp_clock_accuracy_ppm();

    const uint8_t pu8ChannelMapToAdvExtIndNumTable[8] =
    {
        0,//0 : unexpected
        1,//1 : 37
        1,//2 : 38
        2,//3 : 37|38
        1,//4 : 39
        2,//5 : 37|39
        2,//6 : 38|39
        3,//7 : 37|38|39
    };
    pstAdv->u8PrimaryAdvertisingPduNum = pu8ChannelMapToAdvExtIndNumTable[pstAdv->Primary_Advertising_Channel_Map & 0x7];
    if (0 == pstAdv->u8PrimaryAdvertisingPduNum)
    {
        ////unexpected situation, should be check when parameters set
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }
    
    
/*
If the advertising set already contains advertising data or scan response data,
extended advertising is being used, and the length of the data is greater than
the maximum that the Controller can transmit within the longest possible
auxiliary advertising segment consistent with the parameters, the Controller
shall return the error code Packet Too Long (0x45). If advertising on the LE
Coded PHY, the S=8 coding shall be assumed.
*/
    //if any adv/rsp data exists, refresh it
    if ((pstAdv->u16AdvertisingDataLength > 0) || (pstAdv->u16ScanResponseDataLength > 0))
    {
        return rom_llc_ext_adv_refresh_pdus(pstAdv);
    }
    
    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}

uint8_t rom_llc_ext_adv_set_advertising_data(uint8_t Advertising_Handle,
                                             uint8_t Operation,
                                             uint8_t Fragment_Preference,
                                             uint8_t Advertising_Data_Length,
                                             uint8_t *Advertising_Data)
{
    if (NULL == Advertising_Data)
    {
        Advertising_Data_Length = 0;
    }

    stLlcExtAdv_t* pstAdv = rom_llc_ext_adv_get_created_instance_by_handle(Advertising_Handle);

    if (NULL == pstAdv)
    {
        return LLC_SPEC_5_1_ERROR_CODE_UNKNOWN_ADVERTISING_IDENTIFIER;
    }

    bool bIsLegacy = LLC_EXT_ADV_IS_LEGACY(pstAdv->Advertising_Event_Properties);
    bool bIsScannable = LLC_EXT_ADV_IS_SCANNABLE(pstAdv->Advertising_Event_Properties);


//    If the advertising set specifies a type that does not support advertising data, the
//    Controller shall return the error code Invalid HCI Command Parameters (0x12).
    if (bIsLegacy)
    {
        switch(pstAdv->Advertising_Event_Properties)
        {
            //directed legacy adv has no advertising data
            case ADVERTISING_EVENT_PROPERTIES_ADV_DIRECT_IND_LOW_DUTY_CYCLE:
            case ADVERTISING_EVENT_PROPERTIES_ADV_DIRECT_IND_HIGH_DUTY_CYCLE:
            {
                if (0 != Advertising_Data_Length)
                {
                    return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
                }
            }
                break;
            default:
                break;
        }
    }
    else
    {
        //scannable extended adv has no advertising data
        if (bIsScannable)
        {
            if (0 != Advertising_Data_Length)
            {
                return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
            }
        }
    }


//    If the advertising set uses legacy advertising PDUs that support advertising
//    data and either Operation is not 0x03 or the Advertising_Data_Length
//    parameter exceeds 31 octets, the Controller shall return the error code Invalid
//    HCI Command Parameters (0x12).
    if (bIsLegacy)
    {
        switch(pstAdv->Advertising_Event_Properties)
        {
            case ADVERTISING_EVENT_PROPERTIES_ADV_IND:
            case ADVERTISING_EVENT_PROPERTIES_ADV_SCAN_IND:
            case ADVERTISING_EVENT_PROPERTIES_ADV_NONCONN_IND:
            {
                if ((LLC_SPEC_5_1_HCI_ADV_DATA_OPERATION_COMPLETE != Operation) || (Advertising_Data_Length > 31))
                {
                    return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
                }
            }
                break;
            default:
                break;
        }
    }


//    If Operation is 0x04 and:
//    • advertising is currently disabled for the advertising set;
//    • the advertising set contains no data;
//    • the advertising set uses legacy PDUs; or
//    • Advertising_Data_Length is not zero;
//    then the Controller shall return the error code Invalid HCI Command
//    Parameters (0x12).
    if (LLC_SPEC_5_1_HCI_ADV_DATA_OPERATION_UNCHANGE == Operation)
    {
        if ((!pstAdv->bEnabled) || (0 == pstAdv->u16AdvertisingDataLength) || (bIsLegacy) || (0 != Advertising_Data_Length))
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
    }

//    If Operation is not 0x03 or 0x04 and Advertising_Data_Length is zero, the
//    Controller shall return the error code Invalid HCI Command Parameters (0x12).
    if ((LLC_SPEC_5_1_HCI_ADV_DATA_OPERATION_COMPLETE != Operation) && (LLC_SPEC_5_1_HCI_ADV_DATA_OPERATION_UNCHANGE != Operation))
    {
        if (0 == Advertising_Data_Length)
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
    }

//    If advertising is currently enabled for the specified advertising set and
//    Operation does not have the value 0x03 or 0x04, the Controller shall return the
//    error code Command Disallowed (0x0C).
    if (pstAdv->bEnabled)
    {
        if ((LLC_SPEC_5_1_HCI_ADV_DATA_OPERATION_COMPLETE != Operation) && (LLC_SPEC_5_1_HCI_ADV_DATA_OPERATION_UNCHANGE != Operation))
        {
            return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
        }
    }

    if (bIsLegacy)
    {
        return rom_llc_ext_adv_legacy_pdu_manager_set_advertising_data(pstAdv, Advertising_Data, Advertising_Data_Length);
    }
    else
    {
        return rom_llc_ext_adv_pdu_manager_set_advertising_data(pstAdv, Operation,
                                                                LLC_SPEC_5_1_HCI_FRAGMENT_PREFERENCE_MAY_FRAGMENT == Fragment_Preference, Advertising_Data, Advertising_Data_Length);
    }
}


uint8_t rom_llc_ext_adv_set_scan_response_data(uint8_t Advertising_Handle,
                                               uint8_t Operation,
                                               uint8_t Fragment_Preference,
                                               uint8_t Scan_Response_Data_Length,
                                               uint8_t *Scan_Response_Data)
{
    if (NULL == Scan_Response_Data)
    {
        Scan_Response_Data_Length = 0;
    }

    stLlcExtAdv_t* pstAdv = rom_llc_ext_adv_get_created_instance_by_handle(Advertising_Handle);

    if (NULL == pstAdv)
    {
        return LLC_SPEC_5_1_ERROR_CODE_UNKNOWN_ADVERTISING_IDENTIFIER;
    }

    bool bIsLegacy = LLC_EXT_ADV_IS_LEGACY(pstAdv->Advertising_Event_Properties);
    bool bIsScannable = LLC_EXT_ADV_IS_SCANNABLE(pstAdv->Advertising_Event_Properties);

//    If the advertising set is non-scannable and the Host uses this command other
//    than to discard existing data, the Controller shall return the error code Invalid
//    HCI Command Parameters (0x12). If the advertising set uses scannable
//    legacy advertising PDUs and either Operation is not 0x03 or the
//    Scan_Response_Data_Length parameter exceeds 31 octets, the Controller
//    shall return the error code Invalid HCI Command Parameters (0x12).
    if (!bIsScannable)
    {
        if ((LLC_SPEC_5_1_HCI_SCAN_RESPONSE_DATA_OPERATION_COMPLETE != Operation) || (0 != Scan_Response_Data_Length))
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
    }
    if (bIsLegacy)
    {
        if ((LLC_SPEC_5_1_HCI_SCAN_RESPONSE_DATA_OPERATION_COMPLETE != Operation) || (Scan_Response_Data_Length > 31))
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
    }

//    If Operation is not 0x03 and Scan_Response_Data_Length is zero, the
//    Controller shall return the error code Invalid HCl Command Parameters (0x12).
    if ((LLC_SPEC_5_1_HCI_SCAN_RESPONSE_DATA_OPERATION_COMPLETE != Operation) && (0 == Scan_Response_Data_Length))
    {
        return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
    }

//    If advertising is currently enabled for the specified advertising set and
//    Operation does not have the value 0x03, the Controller shall return the error
//    code Command Disallowed (0x0C).
    if (pstAdv->bEnabled && (LLC_SPEC_5_1_HCI_SCAN_RESPONSE_DATA_OPERATION_COMPLETE != Operation))
    {
        return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
    }

//    If the advertising set uses scannable extended advertising PDUs, advertising is
//    currently enabled for the specified advertising set, and
//    Scan_Response_Data_Length is zero, the Controller shall return the error
//    code Command Disallowed (0x0C).
    if (bIsScannable && !bIsLegacy && pstAdv->bEnabled)
    {
        if (0 == Scan_Response_Data_Length)
        {
            return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
        }
    }

    if (bIsLegacy)
    {
        return rom_llc_ext_adv_legacy_pdu_manager_set_scan_response_data(pstAdv, Scan_Response_Data, Scan_Response_Data_Length);
    }
    else
    {
        return rom_llc_ext_adv_pdu_manager_set_scan_response_data(pstAdv, Operation,
                                                                  LLC_SPEC_5_1_HCI_FRAGMENT_PREFERENCE_MAY_FRAGMENT == Fragment_Preference, Scan_Response_Data, Scan_Response_Data_Length);
    }
}


uint8_t rom_llc_ext_adv_set_enable_param(stLlcExtAdv_t *pstAdv, uint16_t Duration10Ms, uint8_t Max_Extended_Advertising_Events)
{
    bool bIsLegacy = LLC_EXT_ADV_IS_LEGACY(pstAdv->Advertising_Event_Properties);
    bool bIsScannable = LLC_EXT_ADV_IS_SCANNABLE(pstAdv->Advertising_Event_Properties);
    bool bIsConnectable = LLC_EXT_ADV_IS_CONNECTABLE(pstAdv->Advertising_Event_Properties);

    //check PDUS
    if (NULL == pstAdv->pstPrimaryAdvertisingPdu)
    {
        //non pdus set
        
        uint8_t u8ErrorCode = rom_llc_ext_adv_set_advertising_data(pstAdv->Advertising_Handle, LLC_SPEC_5_1_HCI_ADV_DATA_OPERATION_COMPLETE,
            LLC_SPEC_5_1_HCI_FRAGMENT_PREFERENCE_MAY_NOT_FRAGMENT, 0, NULL);
        
        if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrorCode)
        {
            return u8ErrorCode;
        }
    }
    if (bIsLegacy)
    {
        switch(pstAdv->Advertising_Event_Properties)
        {
            case ADVERTISING_EVENT_PROPERTIES_ADV_IND:
            case ADVERTISING_EVENT_PROPERTIES_ADV_SCAN_IND:
            {
                if (NULL == pstAdv->pstResponsePdu)
                {
                    uint8_t u8ErrorCode = rom_llc_ext_adv_set_scan_response_data(pstAdv->Advertising_Handle, LLC_SPEC_5_1_HCI_SCAN_RESPONSE_DATA_OPERATION_COMPLETE,
                        LLC_SPEC_5_1_HCI_FRAGMENT_PREFERENCE_MAY_NOT_FRAGMENT, 0, NULL);
                    
                    if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrorCode)
                    {
                        return u8ErrorCode;
                    }
                }
            }
                break;
            
            case ADVERTISING_EVENT_PROPERTIES_ADV_DIRECT_IND_LOW_DUTY_CYCLE:
            case ADVERTISING_EVENT_PROPERTIES_ADV_DIRECT_IND_HIGH_DUTY_CYCLE:
            case ADVERTISING_EVENT_PROPERTIES_ADV_NONCONN_IND:
                break;

            default:
                return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
    }
    else
    {
        //as impl with auxiliary packet, there should be AUX_ADV_IND
        if (NULL == pstAdv->pstSecondaryAdvertisingPdu)
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
        
        if (bIsScannable && (NULL == pstAdv->pstResponsePdu))
        {
            uint8_t u8ErrorCode = rom_llc_ext_adv_set_scan_response_data(pstAdv->Advertising_Handle, LLC_SPEC_5_1_HCI_SCAN_RESPONSE_DATA_OPERATION_COMPLETE,
                LLC_SPEC_5_1_HCI_FRAGMENT_PREFERENCE_MAY_NOT_FRAGMENT, 0, NULL);
            
            if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrorCode)
            {
                return u8ErrorCode;
            }
        }
    }
    
    
    //If the advertising is high duty cycle connectable directed advertising, then
    //Duration[i] shall be less than or equal to 1.28 seconds and shall not be equal to 0.
    if (ADVERTISING_EVENT_PROPERTIES_ADV_DIRECT_IND_HIGH_DUTY_CYCLE == pstAdv->Advertising_Event_Properties)
    {
        #if 1
        if ((Duration10Ms > 128) || (0 == Duration10Ms))
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
        #else
        if ((Duration10Ms > 128) || (0 == Duration10Ms))
        {
            Duration10Ms = 128;
        }
        #endif
    }
    
//    If the advertising data or scan response data in the advertising set is not
//    complete, the Controller shall return the error code Command Disallowed
//    (0x0C)
    if ((0 != pstAdv->u16UpdatingAdvertisingDataLength) || (0 != pstAdv->u16UpdatingScanResponseDataLength))
    {
        return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
    }

//    If the advertising set uses scannable extended advertising PDUs and no scan
//    response data is currently provided, the Controller shall return the error code
//    Command Disallowed (0x0C).
    if (bIsScannable && (!bIsLegacy))
    {
        if (0 == pstAdv->u16ScanResponseDataLength)
        {
            return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
        }
    }


//    If the advertising set uses connectable extended advertising PDUs and the
//    advertising data in the advertising set will not fit in the AUX_ADV_IND PDU,
//    the Controller shall return the error code Invalid HCI Command Parameters
//    (0x12)
    //already checked when set advertising data


//    If extended advertising is being used and the length of any advertising data or
//    of any scan response data is greater than the maximum that the Controller can
//    transmit within the longest possible auxiliary advertising segment consistent
//    with the chosen advertising interval, the Controller shall return the error code
//    Packet Too Long (0x45). If advertising on the LE Coded PHY, the S=8 coding
//    shall be assumed.
    //already checked when set advertising data


//    If the advertising set's Own_Address_Type parameter is set to 0x01 and the
//    random address for the advertising set has not been initialized, the Controller
//    shall return the error code Invalid HCI Command Parameters (0x12).
    if ((LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RANDOM == pstAdv->Own_Address_Type) && (!pstAdv->bAdvertisingRandomAddressSet))
    {
        return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
    }


//    If the advertising set's Own_Address_Type parameter is set to 0x03, the
//    controller's resolving list did not contain a matching entry, and the random
//    address for the advertising set has not been initialized, the Controller shall
//    return the error code Invalid HCI Command Parameters (0x12).

    memcpy(pstAdv->pu8PeerAddress, pstAdv->Peer_Address, 6);
    pstAdv->u8PeerAddrType = pstAdv->Peer_Address_Type;
    pstAdv->bPeerAddressRxAdd = (LLC_SPEC_5_1_HCI_ADV_PEER_ADDRESS_TYPE_RANDOM_OR_RANDOM_IDENTITY == pstAdv->Peer_Address_Type);
    
    switch(pstAdv->Own_Address_Type)
    {
        case LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_PUBLIC:
        {
            pstAdv->bOwnAddressTxAdd = false;
            pstAdv->bOwnAddressResolvable = false;
            rom_llc_address_get_own_public_address(pstAdv->pu8OwnAddress);
        }
        break;

        case LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RANDOM:
        {
            if (pstAdv->bAdvertisingRandomAddressSet)
            {
                pstAdv->bOwnAddressTxAdd = true;
                pstAdv->bOwnAddressResolvable = false;
                memcpy(pstAdv->pu8OwnAddress, pstAdv->Advertising_Random_Address, 6);
            }
            else
            {
                return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
            }

        }
        break;

        case LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RESOLVABLE_OR_PUBLIC:
        {
            if (rom_llc_address_get_own_resolvable_private_address_with_peer_addr(pstAdv->pu8OwnAddress, pstAdv->u8PeerAddrType, pstAdv->pu8PeerAddress))
            {
                MLOGXD(0x37c0, "ext adv use public RESOLVABLE address", pstAdv->pu8OwnAddress, 6);
                pstAdv->bOwnAddressTxAdd = true;
                pstAdv->bOwnAddressResolvable = true;
                pstAdv->u64OwnResolvableAddressUpdatedTime = rom_llc_timer_get_count(true);
            }
            else
            {
                pstAdv->bOwnAddressTxAdd = false;
                pstAdv->bOwnAddressResolvable = false;
                rom_llc_address_get_own_public_address(pstAdv->pu8OwnAddress);
                MLOG0D(0x37c1, "ext adv ADDRESS_TYPE_RESOLVABLE fall to PUBLIC ");
            }
        }
        break;

        case LLC_SPEC_5_1_HCI_OWN_ADDRESS_TYPE_RESOLVABLE_OR_RANDOM:
        {
            if (rom_llc_address_get_own_resolvable_private_address_with_peer_addr(pstAdv->pu8OwnAddress, pstAdv->u8PeerAddrType, pstAdv->pu8PeerAddress))
            {
                MLOGXD(0x37c2, "ext adv use random RESOLVABLE address", pstAdv->pu8OwnAddress, 6);
                pstAdv->bOwnAddressTxAdd = true;
                pstAdv->bOwnAddressResolvable = true;
                pstAdv->u64OwnResolvableAddressUpdatedTime = rom_llc_timer_get_count(true);
            }
            else
            {
                if (pstAdv->bAdvertisingRandomAddressSet)
                {
                    pstAdv->bOwnAddressTxAdd = true;
                    pstAdv->bOwnAddressResolvable = false;
                    memcpy(pstAdv->pu8OwnAddress, pstAdv->Advertising_Random_Address, 6);
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
            return LLC_SPEC_5_1_ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE;
        }
    }

    if(LLC_EXT_ADV_IS_DIRECTED(pstAdv->Advertising_Event_Properties))
    {
        uint8_t u8Rpa[6];
        pstAdv->u8PeerIdentifyAddrType = pstAdv->u8PeerAddrType;
        memcpy(pstAdv->pu8PeerIdentifyAddr, pstAdv->pu8PeerAddress, 6); //save direct addr
        if(rom_llc_privacy_gen_and_get_peer_RPA_with_peer_addr(u8Rpa, pstAdv->u8PeerAddrType, pstAdv->pu8PeerAddress))
        {
            //set TargetA to RPA generated with peer IRK
            memcpy(pstAdv->pu8PeerAddress, u8Rpa, 6);
            MLOGXD(0x37c3, "direct ext adv set TargetA to RPA", u8Rpa, 6);

            pstAdv->u8PeerAddrType = LLC_SPEC_5_1_HCI_ADV_PEER_ADDRESS_TYPE_RANDOM_OR_RANDOM_IDENTITY;
            pstAdv->bPeerAddressRxAdd = true;
        }
    }

    pstAdv->u32DurationUs = Duration10Ms * 10000;
    pstAdv->u8MaxExtendedAdvertisingEvents = Max_Extended_Advertising_Events;

    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}

uint8_t rom_llc_ext_adv_get_handle_count_in_parameters(uint8_t Handle, uint8_t Parameter_Total_Length, uint8_t* Parameters)
{
    /*
    Enable,                                 Size: 1 octet
    Number_of_Sets,                         Size: 1 octet
    Advertising_Handle[i],                  Size: 1 octet * Number_of_Sets
    Duration[i],                            Size: 2 octets * Number_of_Sets
    Max_Extended_Advertising_Events[i]      Size: 1 octet * Number_of_Sets
    */

    if (NULL == Parameters)
    {
        return 0;
    }

    if (Parameter_Total_Length < 2)
    {
        return 0;
    }

    uint8_t Number_of_Sets = Parameters[1];

    if (Number_of_Sets * 4 + 2 != Parameter_Total_Length)
    {
        return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
    }

    if (0 == Number_of_Sets)
    {
        return 0;
    }

    uint8_t u8HandleCount = 0;
    uint32_t i;
    uint8_t u8Offset = 2;
    for(i = 0; i < Number_of_Sets; i++)
    {
        uint8_t Advertising_Handle = Parameters[u8Offset + 0];

        if (Advertising_Handle == Handle)
        {
            u8HandleCount++;
        }

        u8Offset += 4;
    }

    return u8HandleCount;
}



uint8_t rom_llc_ext_adv_set_enable(uint8_t Parameter_Total_Length, uint8_t* Parameters)
{
    /*
    Enable,                                 Size: 1 octet
    Number_of_Sets,                         Size: 1 octet
    Advertising_Handle[i],                  Size: 1 octet * Number_of_Sets
    Duration[i],                            Size: 2 octets * Number_of_Sets
    Max_Extended_Advertising_Events[i]      Size: 1 octet * Number_of_Sets
    */

    if (NULL == Parameters)
    {
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }

    if (Parameter_Total_Length < 2)
    {
        return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
    }

    bool Enable = 0x00 != Parameters[0];
    uint8_t Number_of_Sets = Parameters[1];

    if (Number_of_Sets * 4 + 2 != Parameter_Total_Length)
    {
        return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
    }


    if ((!Enable) && (0 == Number_of_Sets))
    {
        //disable all adv set
        uint8_t u8AdvNum = rom_llc_ext_adv_get_num();
        uint8_t u8AdvIdx;
        for(u8AdvIdx = 0; u8AdvIdx < u8AdvNum; u8AdvIdx++)
        {
            stLlcExtAdv_t* pstAdv = rom_llc_ext_adv_get_instance_by_index(u8AdvIdx);
            if (NULL == pstAdv)
            {
                system_error(SYS_ERR_MODULE_LLC_EXT_ADV, __LINE__, u8AdvIdx, u8AdvNum);
                return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
            }
            if ((pstAdv->bCreated) && (pstAdv->bEnabled))
            {
                if (LLC_EXT_ADV_IS_LEGACY(pstAdv->Advertising_Event_Properties))
                {
                    rom_llc_ext_adv_legacy_state_machine_enable(pstAdv, false);
                }
                else
                {
                    rom_llc_ext_adv_state_machine_enable(pstAdv, false);
                }
            }
        }
        return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
    }

    if (Enable && (0 == Number_of_Sets))
    {
        return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
    }

    uint32_t i;

    //check duplicate handle
    uint8_t u8Offset = 2;
    for(i = 0; i < Number_of_Sets; i++)
    {
        uint8_t Advertising_Handle = Parameters[u8Offset + 0];

        if (1 != rom_llc_ext_adv_get_handle_count_in_parameters(Advertising_Handle, Parameter_Total_Length, Parameters))
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }

        u8Offset += 4;
    }

    //check exists
    u8Offset = 2;
    for(i = 0; i < Number_of_Sets; i++)
    {
        uint8_t Advertising_Handle = Parameters[u8Offset + 0];

        if (NULL == rom_llc_ext_adv_get_created_instance_by_handle(Advertising_Handle))
        {
            return LLC_SPEC_5_1_ERROR_CODE_UNKNOWN_ADVERTISING_IDENTIFIER;
        }

        u8Offset += 4;
    }


    //set param
    if (Enable)
    {
        u8Offset = 2;
        for(i = 0; i < Number_of_Sets; i++)
        {
            uint8_t Advertising_Handle = Parameters[u8Offset + 0];
            uint16_t Duration = rom_llc_tool_read_little_endian_uint16(&Parameters[u8Offset + 1]);
            uint8_t Max_Extended_Advertising_Events = Parameters[u8Offset + 3];

            stLlcExtAdv_t *pstAdv = rom_llc_ext_adv_get_created_instance_by_handle(Advertising_Handle);
            if (NULL == pstAdv)
            {
                system_error(SYS_ERR_MODULE_LLC_EXT_ADV, __LINE__, Advertising_Handle, 0);
                return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
            }

            uint8_t u8ErrorCode = rom_llc_ext_adv_set_enable_param(pstAdv, Duration, Max_Extended_Advertising_Events);
            if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrorCode)
            {
                return u8ErrorCode;
            }

            u8Offset += 4;
        }
    }

    //enable&disable
    u8Offset = 2;
    for(i = 0; i < Number_of_Sets; i++)
    {
        uint8_t Advertising_Handle = Parameters[u8Offset + 0];

        stLlcExtAdv_t *pstAdv = rom_llc_ext_adv_get_created_instance_by_handle(Advertising_Handle);
        if (NULL == pstAdv)
        {
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }

        if (LLC_EXT_ADV_IS_LEGACY(pstAdv->Advertising_Event_Properties))
        {
            rom_llc_ext_adv_legacy_state_machine_enable(pstAdv, Enable);
        }
        else
        {
            rom_llc_ext_adv_state_machine_enable(pstAdv, Enable);
        }

        u8Offset += 4;
    }

    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}

uint8_t rom_llc_ext_adv_read_maximum_advertising_data_length(uint16_t *pu16MaximumAdvertisingDataLength)
{
    if (NULL != pu16MaximumAdvertisingDataLength)
    {
        *pu16MaximumAdvertisingDataLength = LLC_SPEC_5_1_EXTENDED_ADV_MAX_ADVERTISING_DATA_LENGTH;
    }

    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}

uint8_t rom_llc_ext_adv_read_number_of_supported_advertising_sets(uint8_t *pu8NumSupportedAdvertisingSets)
{
    if (NULL != pu8NumSupportedAdvertisingSets)
    {
        *pu8NumSupportedAdvertisingSets = g_u8LlcExtAdvMaxNum;
    }

    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}

uint8_t rom_llc_ext_adv_remove_advertising_set(uint8_t Advertising_Handle)
{
    stLlcExtAdv_t* pstAdv = rom_llc_ext_adv_get_created_instance_by_handle(Advertising_Handle);

    if (NULL == pstAdv)
    {
        return LLC_SPEC_5_1_ERROR_CODE_UNKNOWN_ADVERTISING_IDENTIFIER;
    }

    if (pstAdv->bEnabled)
    {
        return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
    }

    //clear pdus
    if (LLC_EXT_ADV_IS_LEGACY(pstAdv->Advertising_Event_Properties))
    {
        rom_llc_ext_adv_legacy_pdu_manager_clear(pstAdv);
    }
    else
    {
        rom_llc_ext_adv_pdu_manager_clear(pstAdv);
    }

    //clear random addr
    pstAdv->bAdvertisingRandomAddressSet = false;

    //clear Advertising_Handle
    pstAdv->bCreated = false;

    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}

uint8_t rom_llc_ext_adv_clear_advertising_sets(void)
{
    uint8_t u8AdvNum = rom_llc_ext_adv_get_num();
    uint8_t u8AdvIdx;

    //check if advertising is enabled on any advertising set
    for(u8AdvIdx = 0; u8AdvIdx < u8AdvNum; u8AdvIdx++)
    {
        stLlcExtAdv_t* pstAdv = rom_llc_ext_adv_get_instance_by_index(u8AdvIdx);
        if (NULL == pstAdv)
        {
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV, __LINE__, u8AdvIdx, u8AdvNum);
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }
        if ((pstAdv->bCreated) && (pstAdv->bEnabled))
        {
            return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
        }
    }

    //clear sets
    for(u8AdvIdx = 0; u8AdvIdx < u8AdvNum; u8AdvIdx++)
    {
        stLlcExtAdv_t* pstAdv = rom_llc_ext_adv_get_instance_by_index(u8AdvIdx);
        if (NULL == pstAdv)
        {
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV, __LINE__, u8AdvIdx, u8AdvNum);
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }
        if ((pstAdv->bCreated))
        {
            rom_llc_ext_adv_remove_advertising_set(pstAdv->Advertising_Handle);
        }
    }

    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}

bool rom_llc_ext_adv_is_any_advertising_set_enabled(void)
{
    uint8_t u8AdvNum = rom_llc_ext_adv_get_num();
    uint8_t u8AdvIdx;

    for(u8AdvIdx = 0; u8AdvIdx < u8AdvNum; u8AdvIdx++)
    {
        stLlcExtAdv_t* pstAdv = rom_llc_ext_adv_get_instance_by_index(u8AdvIdx);
        if (NULL == pstAdv)
        {
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV, __LINE__, u8AdvIdx, u8AdvNum);
            return false;
        }
        if ((pstAdv->bCreated) && (pstAdv->bEnabled))
        {
            return true;
        }
    }
    
    return false;
}

bool rom_llc_ext_adv_is_white_list_in_use(void)
{
    uint8_t u8AdvNum = rom_llc_ext_adv_get_num();
    uint8_t u8AdvIdx;

    for(u8AdvIdx = 0; u8AdvIdx < u8AdvNum; u8AdvIdx++)
    {
        stLlcExtAdv_t* pstAdv = rom_llc_ext_adv_get_instance_by_index(u8AdvIdx);
        if (NULL == pstAdv)
        {
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV, __LINE__, u8AdvIdx, u8AdvNum);
            return false;
        }
        if ((pstAdv->bCreated)
            && (pstAdv->bEnabled)
            && (LLC_SPEC_5_1_HCI_ADV_FILTER_POLICY_CONN_ALL_SCAN_ALL == pstAdv->Advertising_Filter_Policy))
        {
            return true;
        }
    }
    
    return false;
}


void rom_llc_ext_adv_update_secondary_advertising_channel_map_for_all_extended_adv(uint8_t Secondary_Advertising_Channel_Map[5])
{
    if (NULL == Secondary_Advertising_Channel_Map)
    {
        return;
    }
    
    if (rom_llc_tool_is_array_all_zero(Secondary_Advertising_Channel_Map, 4) && (0 == (0x1F & Secondary_Advertising_Channel_Map[4])))
    {
        return;
    }
    
    uint8_t u8AdvNum = rom_llc_ext_adv_get_num();
    uint8_t u8AdvIdx;

    uint32_t u32Primask = __get_PRIMASK();
    
    for(u8AdvIdx = 0; u8AdvIdx < u8AdvNum; u8AdvIdx++)
    {
        stLlcExtAdv_t* pstAdv = rom_llc_ext_adv_get_instance_by_index(u8AdvIdx);
        if (NULL == pstAdv)
        {
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV, __LINE__, u8AdvIdx, u8AdvNum);
            return;
        }
        if ((pstAdv->bCreated) && (! LLC_EXT_ADV_IS_LEGACY(pstAdv->Advertising_Event_Properties)))
        {
            __disable_irq();
            
            pstAdv->Secondary_Advertising_Channel_Map[0] = Secondary_Advertising_Channel_Map[0];
            pstAdv->Secondary_Advertising_Channel_Map[1] = Secondary_Advertising_Channel_Map[1];
            pstAdv->Secondary_Advertising_Channel_Map[2] = Secondary_Advertising_Channel_Map[2];
            pstAdv->Secondary_Advertising_Channel_Map[3] = Secondary_Advertising_Channel_Map[3];
            pstAdv->Secondary_Advertising_Channel_Map[4] = Secondary_Advertising_Channel_Map[4];
            
            __set_PRIMASK(u32Primask);
        }
    }
}
