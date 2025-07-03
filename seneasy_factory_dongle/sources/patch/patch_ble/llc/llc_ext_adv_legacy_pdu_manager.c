#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>



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


#include "llc_ext_adv.h"

#include "llc_ext_adv_legacy_pdu_manager.h"

#include "rom_callback.h"

void rom_llc_ext_adv_legacy_pdu_manager_destroy_updating_advertising_pdu(stLlcExtAdv_t* pstAdv)
{
    if (NULL != pstAdv->pstUpdatingPrimaryAdvertisingPdu)
    {
        rom_llc_ext_adv_free_adv_channel_pdu(pstAdv->pstUpdatingPrimaryAdvertisingPdu);
        pstAdv->pstUpdatingPrimaryAdvertisingPdu = NULL;
    }
    
    pstAdv->u16UpdatingAdvertisingDataLength = 0;
}

void rom_llc_ext_adv_legacy_pdu_manager_destroy_updating_scan_response_pdu(stLlcExtAdv_t* pstAdv)
{
    if (NULL != pstAdv->pstUpdatingResponsePdu)
    {
        rom_llc_ext_adv_free_adv_channel_pdu(pstAdv->pstUpdatingResponsePdu);
        pstAdv->pstUpdatingResponsePdu = NULL;
    }
    
    pstAdv->u16UpdatingScanResponseDataLength = 0;
}


void rom_llc_ext_adv_legacy_pdu_manager_destroy_accepted_advertising_pdu(stLlcExtAdv_t* pstAdv)
{
    if (NULL != pstAdv->pstPrimaryAdvertisingPdu)
    {
        rom_llc_ext_adv_free_adv_channel_pdu(pstAdv->pstPrimaryAdvertisingPdu);
        pstAdv->pstPrimaryAdvertisingPdu = NULL;
    }
    
    pstAdv->u16AdvertisingDataLength = 0;
}

void rom_llc_ext_adv_legacy_pdu_manager_destroy_accepted_scan_response_pdu(stLlcExtAdv_t* pstAdv)
{
    if (NULL != pstAdv->pstResponsePdu)
    {
        rom_llc_ext_adv_free_adv_channel_pdu(pstAdv->pstResponsePdu);
        pstAdv->pstResponsePdu = NULL;
    }
    
    pstAdv->u16ScanResponseDataLength = 0;
}


void rom_llc_ext_adv_legacy_pdu_manager_accept_updating_advertising_pdu(stLlcExtAdv_t* pstAdv)
{
    rom_llc_ext_adv_legacy_pdu_manager_destroy_accepted_advertising_pdu(pstAdv);
    
    pstAdv->pstPrimaryAdvertisingPdu = pstAdv->pstUpdatingPrimaryAdvertisingPdu;
    
    pstAdv->u16AdvertisingDataLength = pstAdv->u16UpdatingAdvertisingDataLength;
    
    pstAdv->pstUpdatingPrimaryAdvertisingPdu = NULL;
    
    pstAdv->u16UpdatingAdvertisingDataLength = 0;
}

void rom_llc_ext_adv_legacy_pdu_manager_accept_updating_scan_response_pdu(stLlcExtAdv_t* pstAdv)
{
    rom_llc_ext_adv_legacy_pdu_manager_destroy_accepted_scan_response_pdu(pstAdv);
    
    pstAdv->pstResponsePdu = pstAdv->pstUpdatingResponsePdu;
    
    pstAdv->u16ScanResponseDataLength = pstAdv->u16UpdatingScanResponseDataLength;
    
    pstAdv->pstUpdatingResponsePdu = NULL;
    
    pstAdv->u16UpdatingScanResponseDataLength = 0;
}

uint8_t rom_llc_ext_adv_legacy_pdu_manager_set_advertising_data(stLlcExtAdv_t* pstAdv, uint8_t *pu8Data, uint8_t u8DataLength)
{
    if (NULL == pu8Data)
    {
        u8DataLength = 0;
    }
    
    switch(pstAdv->Advertising_Event_Properties)
    {
        case ADVERTISING_EVENT_PROPERTIES_ADV_DIRECT_IND_LOW_DUTY_CYCLE:
        case ADVERTISING_EVENT_PROPERTIES_ADV_DIRECT_IND_HIGH_DUTY_CYCLE:
        {
            if (0 != u8DataLength)
            {
                return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
            }
        }
            break;
        
            
        case ADVERTISING_EVENT_PROPERTIES_ADV_IND:
        case ADVERTISING_EVENT_PROPERTIES_ADV_SCAN_IND:
        case ADVERTISING_EVENT_PROPERTIES_ADV_NONCONN_IND:
            break;
        
        default:
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV_LEGACY_PDU_MANAGER, __LINE__, (pstAdv->Advertising_Event_Properties), (uint32_t)pstAdv);
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }
    
    rom_llc_ext_adv_legacy_pdu_manager_destroy_updating_advertising_pdu(pstAdv);
    
    //ADV_IND        : AdvA + AdvData
    //ADV_DIRECT_IND : AdvA + TargetA
    //ADV_NONCONN_IND: AdvA + AdvData
    //ADV_SCAN_IND   : AdvA + AdvData
    
    uint16_t u16ExpectedPayloadSize;
    if ((ADVERTISING_EVENT_PROPERTIES_ADV_DIRECT_IND_LOW_DUTY_CYCLE == pstAdv->Advertising_Event_Properties)
        || (ADVERTISING_EVENT_PROPERTIES_ADV_DIRECT_IND_HIGH_DUTY_CYCLE == pstAdv->Advertising_Event_Properties))
    {
        u16ExpectedPayloadSize = 12;
    }
    else
    {
        u16ExpectedPayloadSize = 6 + u8DataLength;
    }
    
    stLlcAdvChannelPdu_t* pstPdu = rom_llc_ext_adv_alloc_adv_channel_pdu(u16ExpectedPayloadSize, false);
    if (NULL == pstPdu)
    {
        return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
    }
    if (pstPdu->u16PayloadBufSize < u16ExpectedPayloadSize)
    {
        rom_llc_ext_adv_free_adv_channel_pdu(pstPdu);
        return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
    }
    
    memcpy(&pstPdu->pu8Payload[0], pstAdv->pu8OwnAddress, 6);
    
    stLlcSpec51AdvChannelPduHeader_t stHeader = {0};
    stHeader.bitField.Length = u16ExpectedPayloadSize;
    stHeader.bitField.TxAdd = pstAdv->bOwnAddressTxAdd;
    switch(pstAdv->Advertising_Event_Properties)
    {
        case ADVERTISING_EVENT_PROPERTIES_ADV_IND:
            memcpy(&pstPdu->pu8Payload[6], pu8Data, u8DataLength);
            stHeader.bitField.PDUType = LLC_SPEC_5_1_ADV_CH_PDU_TYPE_ADV_IND;
            stHeader.bitField.ChSel = rom_ble_impl_is_channel_selection_algorithm_no_2_supported();
            break;
        
        case ADVERTISING_EVENT_PROPERTIES_ADV_DIRECT_IND_LOW_DUTY_CYCLE:
        case ADVERTISING_EVENT_PROPERTIES_ADV_DIRECT_IND_HIGH_DUTY_CYCLE:
            memcpy(&pstPdu->pu8Payload[6], pstAdv->pu8PeerAddress, 6);
            stHeader.bitField.PDUType = LLC_SPEC_5_1_ADV_CH_PDU_TYPE_DIRECT_IND;
            stHeader.bitField.RxAdd = pstAdv->bPeerAddressRxAdd;
            stHeader.bitField.ChSel = rom_ble_impl_is_channel_selection_algorithm_no_2_supported();
            break;
        
        case ADVERTISING_EVENT_PROPERTIES_ADV_SCAN_IND:
            memcpy(&pstPdu->pu8Payload[6], pu8Data, u8DataLength);
            stHeader.bitField.PDUType = LLC_SPEC_5_1_ADV_CH_PDU_TYPE_ADV_SCAN_IND;
            break;
        
        case ADVERTISING_EVENT_PROPERTIES_ADV_NONCONN_IND:
            memcpy(&pstPdu->pu8Payload[6], pu8Data, u8DataLength);
            stHeader.bitField.PDUType = LLC_SPEC_5_1_ADV_CH_PDU_TYPE_NONCONN_IND;
            break;
        
        default:
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV_LEGACY_PDU_MANAGER, __LINE__, (pstAdv->Advertising_Event_Properties), (uint32_t)pstAdv);
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }
    pstPdu->u32Header = stHeader.u16Value;
    
    pstAdv->pstUpdatingPrimaryAdvertisingPdu = pstPdu;
    pstAdv->u16UpdatingAdvertisingDataLength = u16ExpectedPayloadSize;
    
    if (pstAdv->bEnabled)
    {
        pstAdv->bAcceptUpdatingPdus = true;
    }
    else
    {
        rom_llc_ext_adv_legacy_pdu_manager_accept_updating_advertising_pdu(pstAdv);
    }
    
    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}


uint8_t rom_llc_ext_adv_legacy_pdu_manager_set_scan_response_data(stLlcExtAdv_t* pstAdv, uint8_t *pu8Data, uint8_t u8DataLength)
{
    if (NULL == pu8Data)
    {
        u8DataLength = 0;
    }
    
    switch(pstAdv->Advertising_Event_Properties)
    {
        case ADVERTISING_EVENT_PROPERTIES_ADV_DIRECT_IND_LOW_DUTY_CYCLE:
        case ADVERTISING_EVENT_PROPERTIES_ADV_DIRECT_IND_HIGH_DUTY_CYCLE:
        case ADVERTISING_EVENT_PROPERTIES_ADV_NONCONN_IND:
        {
            if (0 == u8DataLength)
            {
                return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
            }
            else
            {
                return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
            }
        }
            break;
        
        case ADVERTISING_EVENT_PROPERTIES_ADV_IND:
        case ADVERTISING_EVENT_PROPERTIES_ADV_SCAN_IND:
            break;
        
        default:
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV_LEGACY_PDU_MANAGER, __LINE__, (pstAdv->Advertising_Event_Properties), (uint32_t)pstAdv);
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }
    
    rom_llc_ext_adv_legacy_pdu_manager_destroy_updating_scan_response_pdu(pstAdv);
    
    //AdvA + AdvData
    uint16_t u16ExpectedPayloadSize = 6 + u8DataLength;
    stLlcAdvChannelPdu_t* pstPdu = rom_llc_ext_adv_alloc_adv_channel_pdu(u16ExpectedPayloadSize, false);
    if (NULL == pstPdu)
    {
        return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
    }
    if (pstPdu->u16PayloadBufSize < u16ExpectedPayloadSize)
    {
        rom_llc_ext_adv_free_adv_channel_pdu(pstPdu);
        return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
    }
    
    memcpy(&pstPdu->pu8Payload[0], pstAdv->pu8OwnAddress, 6);
    memcpy(&pstPdu->pu8Payload[6], pu8Data, u8DataLength);
    stLlcSpec51AdvChannelPduHeader_t stHeader = {0};
    stHeader.bitField.Length = u16ExpectedPayloadSize;
    stHeader.bitField.TxAdd = pstAdv->bOwnAddressTxAdd;
    stHeader.bitField.PDUType = LLC_SPEC_5_1_ADV_CH_PDU_TYPE_SCAN_RSP;
    pstPdu->u32Header = stHeader.u16Value;
    
    pstAdv->pstUpdatingResponsePdu = pstPdu;
    pstAdv->u16UpdatingScanResponseDataLength = u16ExpectedPayloadSize;
    
    if (pstAdv->bEnabled)
    {
        pstAdv->bAcceptUpdatingPdus = true;
    }
    else
    {
        rom_llc_ext_adv_legacy_pdu_manager_accept_updating_scan_response_pdu(pstAdv);
    }
    
    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}

void rom_llc_ext_adv_legacy_pdu_manager_clear(stLlcExtAdv_t* pstAdv)
{
    rom_llc_ext_adv_legacy_pdu_manager_destroy_updating_advertising_pdu(pstAdv);
    rom_llc_ext_adv_legacy_pdu_manager_destroy_updating_scan_response_pdu(pstAdv);
    rom_llc_ext_adv_legacy_pdu_manager_destroy_accepted_advertising_pdu(pstAdv);
    rom_llc_ext_adv_legacy_pdu_manager_destroy_accepted_scan_response_pdu(pstAdv);
}


void rom_llc_ext_adv_legacy_pdu_manager_update_accepted_pdus_AdvA(stLlcExtAdv_t* pstAdv)
{
    stLlcAdvChannelPdu_t* pstPdu;
    stLlcSpec51AdvChannelPduHeader_t stHeader;
    
    pstPdu = pstAdv->pstPrimaryAdvertisingPdu;
    if (NULL != pstPdu)
    {
        stHeader.u16Value = pstPdu->u32Header & 0xFFFF;
        stHeader.bitField.TxAdd = pstAdv->bOwnAddressTxAdd;
        pstPdu->u32Header = stHeader.u16Value;
        memcpy(&pstPdu->pu8Payload[0], pstAdv->pu8OwnAddress, 6);
    }
    
    pstPdu = pstAdv->pstResponsePdu;
    if (NULL != pstPdu)
    {
        stHeader.u16Value = pstPdu->u32Header & 0xFFFF;
        stHeader.bitField.TxAdd = pstAdv->bOwnAddressTxAdd;
        pstPdu->u32Header = stHeader.u16Value;
        memcpy(&pstPdu->pu8Payload[0], pstAdv->pu8OwnAddress, 6);
    }
}

void rom_llc_ext_adv_legacy_pdu_manager_update_accepted_pdus_TargetA(stLlcExtAdv_t* pstAdv)
{
    if (!LLC_EXT_ADV_IS_DIRECTED(pstAdv->Advertising_Event_Properties))
    {
        return;
    }
    
    stLlcAdvChannelPdu_t* pstPdu;
    stLlcSpec51AdvChannelPduHeader_t stHeader;
    pstPdu = pstAdv->pstPrimaryAdvertisingPdu;
    if (NULL != pstPdu)
    {
        stHeader.u16Value = pstPdu->u32Header & 0xFFFF;
        stHeader.bitField.RxAdd = pstAdv->bPeerAddressRxAdd;
        pstPdu->u32Header = stHeader.u16Value;
        memcpy(&pstPdu->pu8Payload[6], pstAdv->pu8PeerAddress, 6);
    }
}