
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
#include "llc_common_extended_advertising_payload_format.h"

#include "llc_tool.h"
#include "ble_impl.h"


#include "llc_ext_adv.h"

#include "llc_ext_adv_state_machine.h"

#include "llc_ext_adv_pdu_manager.h"

#include "mlog.h"
#include "rom_callback.h"

//@ref BLUETOOTH CORE SPECIFICATION Version 5.1 | Vol 6, Part B



typedef struct
{
    uint8_t *pu8Buf1;
    uint8_t *pu8Buf2;
    uint16_t u16Buf1Length;
    uint16_t u16Buf2Length;
}
stLlcExtAdvPduManagerConcatBuffer_t;


void rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(stLlcExtAdv_t* pstAdv);


bool rom_llc_ext_adv_pdu_manager_generate_pdu_with_data(stLlcAdvChannelPdu_t* pstPdu, stLlcCommonExtendedAdvertisingPayloadFormat_t* pstFormat, 
    stLlcExtAdvPduManagerConcatBuffer_t* pstData, uint16_t u16DataOffset, uint16_t u16DataLength)
{
    
    pstFormat->AdvData_Present = true;
    if (u16DataOffset < pstData->u16Buf1Length)
    {
        //copy start from buf1
        if (u16DataOffset + u16DataLength <= pstData->u16Buf1Length)
        {
            //no need to copy buf2
            
            pstFormat->AdvData = &pstData->pu8Buf1[u16DataOffset];
            pstFormat->AdvData_Length = u16DataLength;
            
            return rom_llc_common_extended_advertising_payload_generate(pstPdu, pstFormat);
        }
        else
        {
            //need to copy buf2
            pstFormat->AdvData = &pstData->pu8Buf1[u16DataOffset];
            pstFormat->AdvData_Length = pstData->u16Buf1Length - u16DataOffset;
            if (!rom_llc_common_extended_advertising_payload_generate(pstPdu, pstFormat))
            {
                return false;
            }
            return rom_llc_common_extended_advertising_payload_append_AdvData(pstPdu, pstData->pu8Buf2, u16DataLength - (pstData->u16Buf1Length - u16DataOffset));
        }
    }
    else
    {
        //copy start from buf2
        
        pstFormat->AdvData = &pstData->pu8Buf2[u16DataOffset - pstData->u16Buf1Length];
        pstFormat->AdvData_Length = u16DataLength;
        return rom_llc_common_extended_advertising_payload_generate(pstPdu, pstFormat);
    }
}



uint8_t rom_llc_ext_adv_pdu_manager_generate_new_chain_pdus(bool bIsComplete,
    uint16_t u16DID, uint8_t u8SID, 
    uint16_t u16ClockAccuracyPpm, EN_LL_PHY_T enumSecondaryPhy,
    stLlcExtAdvPduManagerConcatBuffer_t* pstData,
    stLlcAdvChannelPdu_t** ppstChainPdusHead, stLlcAdvChannelPdu_t** ppstChainPdusTail)
{
    uint16_t u16ExpectedPayloadSize;
    stLlcAdvChannelPdu_t* pstPdu;
    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat;
    
    uint16_t u16DataLength = pstData->u16Buf1Length + pstData->u16Buf2Length;
    uint16_t u16DataOffset = 0;
    
    stLlcAdvChannelPdu_t* pstChainPdusHead = NULL;
    stLlcAdvChannelPdu_t* pstChainPdusTail = NULL;
    
    ////generate AUX_CHAIN_IND start
    //AdvMode:00b AdvA:X TargetA:X CTEInfo:C5 ADI:C3 AuxPtr:O SyncInfo:X TxPower:O ACAD:X AdvData:O
    while(u16DataOffset < u16DataLength)
    {
        uint16_t u16RemainLength = u16DataLength - u16DataOffset;

        //assume one packet could hold all remain data
        u16ExpectedPayloadSize = rom_llc_common_extended_advertising_payload_calculate_payload_length(false, false, false, true, !bIsComplete, false, false, 0, u16RemainLength);
        
        pstPdu = rom_llc_ext_adv_alloc_adv_channel_pdu(u16ExpectedPayloadSize, true);
        if (NULL == pstPdu)
        {
            //clear used pdus
            stLlcAdvChannelPdu_t* pstTmpPdu = pstChainPdusHead;
            while(NULL != pstTmpPdu)
            {
                stLlcAdvChannelPdu_t* pstFreePdu = pstTmpPdu;
                pstTmpPdu = pstTmpPdu->next;
                
                rom_llc_ext_adv_free_adv_channel_pdu(pstFreePdu);
            }
            return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
        }

        if ((pstPdu->u16PayloadBufSize >= u16ExpectedPayloadSize) && (u16ExpectedPayloadSize <= LLC_SPEC_5_1_ADV_CHANNEL_PDU_MAX_PAYLOAD_SIZE))
        {
            //one packet could hold all remain data
            memset(&stFormat, 0, sizeof(stFormat));
            stFormat.Header.bitField.PDUType = LLC_SPEC_5_1_ADV_CH_PDU_TYPE_AUX_CHAIN_IND;
            
            stFormat.Connectable = false;
            stFormat.Scannable = false;

            stFormat.ADI_Present = true;
            stFormat.ADI_DID = u16DID;
            stFormat.ADI_SID = u8SID;
            
            if (!bIsComplete)
            {
                stFormat.AuxPtr_Present = true;
                stFormat.AuxPtr_ClockAccuracyPpm = u16ClockAccuracyPpm;
                stFormat.AuxPtr_Phy = enumSecondaryPhy;
                stFormat.AuxPtr_ChannelIndex = 0;//update before adv event
                stFormat.AuxPtr_Offset30Us = 0;//update before adv event
            }

            if (!rom_llc_ext_adv_pdu_manager_generate_pdu_with_data(pstPdu, &stFormat, pstData, u16DataOffset, u16RemainLength))
            {
                system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, u16DataOffset|((uint32_t)(u16RemainLength<<16)));
                return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
            }

            u16DataOffset += u16RemainLength;
        }
        else
        {
            //one packet could not hold all remain data
            uint16_t u16PduCapacity = rom_llc_common_extended_advertising_payload_get_avaliable_AdvData_length(pstPdu->u16PayloadBufSize, false, false, false, true, true, false, false, 0);
            if (u16PduCapacity >= u16RemainLength)
            {
                system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, u16PduCapacity|((uint32_t)(u16RemainLength<<16)));
                return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
            }

            memset(&stFormat, 0, sizeof(stFormat));
            
            stFormat.Header.bitField.PDUType = LLC_SPEC_5_1_ADV_CH_PDU_TYPE_AUX_CHAIN_IND;
            
            stFormat.Connectable = false;
            stFormat.Scannable = false;

            stFormat.ADI_Present = true;
            stFormat.ADI_DID = u16DID;
            stFormat.ADI_SID = u8SID;

            stFormat.AuxPtr_Present = true;
            stFormat.AuxPtr_ClockAccuracyPpm = u16ClockAccuracyPpm;
            stFormat.AuxPtr_Phy = enumSecondaryPhy;
            stFormat.AuxPtr_ChannelIndex = 0;//update before adv event
            stFormat.AuxPtr_Offset30Us = 0;//update before adv event

            if (!rom_llc_ext_adv_pdu_manager_generate_pdu_with_data(pstPdu, &stFormat, pstData, u16DataOffset, u16PduCapacity))
            {
                system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, u16DataOffset|((uint32_t)(u16PduCapacity<<16)));
                return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
            }

            u16DataOffset += u16PduCapacity;

        }
        
        if (NULL == pstChainPdusHead)
        {
            pstPdu->prev = NULL;
            pstPdu->next = NULL;
            pstChainPdusHead = pstPdu;
            pstChainPdusTail = pstPdu;
        }
        else
        {
            pstPdu->prev = pstChainPdusTail;
            pstPdu->next = NULL;

            pstChainPdusTail->next = pstPdu;
            pstChainPdusTail = pstPdu;
        }
    }
    
    *ppstChainPdusHead = pstChainPdusHead;
    if (NULL != ppstChainPdusTail)
    {
        *ppstChainPdusTail = pstChainPdusTail;
    }
    
    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}

uint8_t rom_llc_ext_adv_pdu_manager_generate_ADV_EXT_IND_nonconnectable_nonscannable(stLlcExtAdv_t* pstAdv)
{
    uint16_t u16ExpectedPayloadSize;
    stLlcAdvChannelPdu_t* pstPdu;
    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat;
    
    
    //impl with auxiliary packet

    ////generate ADV_EXT_IND start
    //AdvMode:00b AdvA:C1 TargetA:X/C1 CTEInfo:X ADI:M AuxPtr:M SyncInfo:X TxPower:C1 ACAD:X AdvData:X
    u16ExpectedPayloadSize = rom_llc_common_extended_advertising_payload_calculate_payload_length(false, false, false, true, true, false, false, 0, 0);
    pstPdu = rom_llc_ext_adv_alloc_adv_channel_pdu(u16ExpectedPayloadSize, true);
    if (NULL == pstPdu)
    {
        return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
    }
    if (pstPdu->u16PayloadBufSize < u16ExpectedPayloadSize)
    {
        rom_llc_ext_adv_free_adv_channel_pdu(pstPdu);
        return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
    }

    memset(&stFormat, 0, sizeof(stFormat));
    
    stFormat.Header.bitField.PDUType = LLC_SPEC_5_1_ADV_CH_PDU_TYPE_ADV_EXT_IND;
    
    stFormat.Connectable = false;
    stFormat.Scannable = false;

    stFormat.ADI_Present = true;
    stFormat.ADI_SID = pstAdv->Advertising_SID;
    stFormat.ADI_DID = pstAdv->u16CurrentDID;

    stFormat.AuxPtr_Present = true;
    stFormat.AuxPtr_ClockAccuracyPpm = pstAdv->u16ClockAccuracyPpm;
    stFormat.AuxPtr_Phy = pstAdv->enumSecondaryPhy;
    stFormat.AuxPtr_ChannelIndex = 0;//update before adv event
    stFormat.AuxPtr_Offset30Us = 0;//update before adv event

    if (!rom_llc_common_extended_advertising_payload_generate(pstPdu, &stFormat))
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, (uint32_t)&stFormat);
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }

    pstAdv->pstUpdatingPrimaryAdvertisingPdu = pstPdu;
    ////generate ADV_EXT_IND end
    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}

uint8_t rom_llc_ext_adv_pdu_manager_generate_AUX_ADV_IND_and_AUX_CHAIN_INDs_nonconnectable_nonscannable(stLlcExtAdv_t* pstAdv, bool bIsComplete, bool bDirected, bool bAnonymous,
    stLlcExtAdvPduManagerConcatBuffer_t* pstData)
{
    uint16_t u16ExpectedPayloadSize;
    stLlcAdvChannelPdu_t* pstPdu;
    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat;
    
    uint16_t u16ConcatDataLength = pstData->u16Buf1Length + pstData->u16Buf2Length;
    
    ////generate AUX_ADV_IND start
    //AdvMode:00b AdvA:C4 TargetA:X/C2 CTEInfo:X ADI:M AuxPtr:O SyncInfo:O TxPower:O ACAD:O AdvData:O
    
    //assume one packet could hold all data
    u16ExpectedPayloadSize = rom_llc_common_extended_advertising_payload_calculate_payload_length(!bAnonymous, bDirected, false, true, !bIsComplete, false, false, 0, u16ConcatDataLength);
    pstPdu = rom_llc_ext_adv_alloc_adv_channel_pdu(u16ExpectedPayloadSize, true);
    if (NULL == pstPdu)
    {
        return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
    }
    if ((pstPdu->u16PayloadBufSize >= u16ExpectedPayloadSize) && (u16ExpectedPayloadSize <= LLC_SPEC_5_1_ADV_CHANNEL_PDU_MAX_PAYLOAD_SIZE))
    {
        ////one packet could hold all data
        memset(&stFormat, 0, sizeof(stFormat));
        
        stFormat.Header.bitField.PDUType = LLC_SPEC_5_1_ADV_CH_PDU_TYPE_AUX_ADV_IND;
        
        stFormat.Connectable = false;
        stFormat.Scannable = false;
        
        if (!bAnonymous)
        {
            stFormat.AdvA_Present = true;
            stFormat.AdvA = pstAdv->pu8OwnAddress;
            stFormat.Header.bitField.TxAdd = pstAdv->bOwnAddressTxAdd;
        }

        if (bDirected)
        {
            stFormat.TargetA_Present = true;
            stFormat.TargetA = pstAdv->pu8PeerAddress;
            stFormat.Header.bitField.RxAdd = pstAdv->bPeerAddressRxAdd;
        }
        
        stFormat.ADI_Present = true;
        stFormat.ADI_SID = pstAdv->Advertising_SID;
        stFormat.ADI_DID = pstAdv->u16CurrentDID;

        if (!bIsComplete)
        {
            stFormat.AuxPtr_Present = true;
            stFormat.AuxPtr_ClockAccuracyPpm = pstAdv->u16ClockAccuracyPpm;
            stFormat.AuxPtr_Phy = pstAdv->enumSecondaryPhy;
            stFormat.AuxPtr_ChannelIndex = 0;//update before adv event
            stFormat.AuxPtr_Offset30Us = 0;//update before adv event
        }
        
        stFormat.AdvData_Present = true;
        stFormat.AdvData_Length = pstData->u16Buf1Length;
        stFormat.AdvData = pstData->pu8Buf1;

        if (!rom_llc_common_extended_advertising_payload_generate(pstPdu, &stFormat))
        {
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, (uint32_t)&stFormat);
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }

        if (0 != pstData->u16Buf2Length)
        {
            if (!rom_llc_common_extended_advertising_payload_append_AdvData(pstPdu, pstData->pu8Buf2, pstData->u16Buf2Length))
            {
                system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, (uint32_t)&pstData);
                return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
            }
        }
        
        pstAdv->pstUpdatingSecondaryAdvertisingPdu = pstPdu;
        
        return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
    }
    ////one packet could not hold all data
    uint16_t u16PduCapacity = 0;

    u16PduCapacity = rom_llc_common_extended_advertising_payload_get_avaliable_AdvData_length(pstPdu->u16PayloadBufSize, !bAnonymous, bDirected, false, true, true, false, false, 0);
    if (u16PduCapacity >= u16ConcatDataLength)
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, u16PduCapacity|((uint32_t)(u16ConcatDataLength<<16)));
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }
    
    memset(&stFormat, 0, sizeof(stFormat));
    
    stFormat.Header.bitField.PDUType = LLC_SPEC_5_1_ADV_CH_PDU_TYPE_AUX_ADV_IND;
    
    stFormat.Connectable = false;
    stFormat.Scannable = false;

    if (!bAnonymous)
    {
        stFormat.AdvA_Present = true;
        stFormat.AdvA = pstAdv->pu8OwnAddress;
        stFormat.Header.bitField.TxAdd = pstAdv->bOwnAddressTxAdd;
    }
    
    if (bDirected)
    {
        stFormat.TargetA_Present = true;
        stFormat.TargetA = pstAdv->pu8PeerAddress;
        stFormat.Header.bitField.RxAdd = pstAdv->bPeerAddressRxAdd;
    }

    stFormat.ADI_Present = true;
    stFormat.ADI_SID = pstAdv->Advertising_SID;
    stFormat.ADI_DID = pstAdv->u16CurrentDID;

    stFormat.AuxPtr_Present = true;
    stFormat.AuxPtr_ClockAccuracyPpm = pstAdv->u16ClockAccuracyPpm;
    stFormat.AuxPtr_Phy = pstAdv->enumSecondaryPhy;
    stFormat.AuxPtr_ChannelIndex = 0;//update before adv event
    stFormat.AuxPtr_Offset30Us = 0;//update before adv event

    stFormat.AdvData_Present = true;
    if (u16PduCapacity <= pstData->u16Buf1Length)
    {
        stFormat.AdvData = pstData->pu8Buf1;
        stFormat.AdvData_Length = u16PduCapacity;
        if (!rom_llc_common_extended_advertising_payload_generate(pstPdu, &stFormat))
        {
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, (uint32_t)&stFormat);
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }
    }
    else
    {
        stFormat.AdvData = pstData->pu8Buf1;
        stFormat.AdvData_Length = pstData->u16Buf1Length;
        if (!rom_llc_common_extended_advertising_payload_generate(pstPdu, &stFormat))
        {
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, (uint32_t)&stFormat);
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }
        if (!rom_llc_common_extended_advertising_payload_append_AdvData(pstPdu, pstData->pu8Buf2, u16PduCapacity - pstData->u16Buf1Length))
        {
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, u16PduCapacity - pstData->u16Buf1Length);
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }
    }

    pstAdv->pstUpdatingSecondaryAdvertisingPdu = pstPdu;

    ////generate AUX_ADV_IND end
    
    
    ////generate AUX_CHAIN_IND start
    stLlcExtAdvPduManagerConcatBuffer_t stData;
    memset(&stData, 0, sizeof(stData));
    if (u16PduCapacity < pstData->u16Buf1Length)
    {
        stData.pu8Buf1 = &pstData->pu8Buf1[u16PduCapacity];
        stData.u16Buf1Length = pstData->u16Buf1Length - u16PduCapacity;
        stData.pu8Buf2 = pstData->pu8Buf2;
        stData.u16Buf2Length = pstData->u16Buf2Length;
    }
    else
    {
        stData.pu8Buf1 = &pstData->pu8Buf2[u16PduCapacity - pstData->u16Buf1Length];
        stData.u16Buf1Length = u16ConcatDataLength - u16PduCapacity;
    }
    
    return rom_llc_ext_adv_pdu_manager_generate_new_chain_pdus(bIsComplete,
        pstAdv->u16CurrentDID, pstAdv->Advertising_SID,
        pstAdv->u16ClockAccuracyPpm, pstAdv->enumSecondaryPhy,
        &stData,
        &pstAdv->pstUpdatingChainPdusHead, &pstAdv->pstUpdatingChainPdusTail);
    ////generate AUX_CHAIN_IND end
}

uint8_t rom_llc_ext_adv_pdu_manager_generate_new_pdus_nonconnectable_nonscannable(stLlcExtAdv_t* pstAdv, bool bIsComplete, bool bDirected, bool bAnonymous,
    uint8_t *pu8Data, uint8_t u8DataLength)
{
    uint8_t u8ErrCode;
    
    u8ErrCode = rom_llc_ext_adv_pdu_manager_generate_ADV_EXT_IND_nonconnectable_nonscannable(pstAdv);
    if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrCode)
    {
        return u8ErrCode;
    }
    
    stLlcExtAdvPduManagerConcatBuffer_t stData;
    memset(&stData, 0, sizeof(stData));
    stData.pu8Buf1 = pu8Data;
    stData.u16Buf1Length = u8DataLength;
    
    return rom_llc_ext_adv_pdu_manager_generate_AUX_ADV_IND_and_AUX_CHAIN_INDs_nonconnectable_nonscannable(pstAdv, bIsComplete, bDirected, bAnonymous, &stData);
}



uint8_t rom_llc_ext_adv_pdu_manager_generate_ADV_EXT_IND_connectable_nonscannable(stLlcExtAdv_t* pstAdv)
{
    uint16_t u16ExpectedPayloadSize;
    stLlcAdvChannelPdu_t* pstPdu;
    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat;
    

    ////generate ADV_EXT_IND start
    //AdvMode:01b AdvA:X TargetA:X CTEInfo:X ADI:M AuxPtr:M SyncInfo:X TxPower:C1 ACAD:X AdvData:X
    u16ExpectedPayloadSize = rom_llc_common_extended_advertising_payload_calculate_payload_length(false, false, false, true, true, false, false, 0, 0);
    pstPdu = rom_llc_ext_adv_alloc_adv_channel_pdu(u16ExpectedPayloadSize, true);
    if (NULL == pstPdu)
    {
        return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
    }
    if (pstPdu->u16PayloadBufSize < u16ExpectedPayloadSize)
    {
        rom_llc_ext_adv_free_adv_channel_pdu(pstPdu);
        return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
    }

    memset(&stFormat, 0, sizeof(stFormat));
    
    stFormat.Header.bitField.PDUType = LLC_SPEC_5_1_ADV_CH_PDU_TYPE_ADV_EXT_IND;
    
    stFormat.Connectable = true;
    stFormat.Scannable = false;

    stFormat.ADI_Present = true;
    stFormat.ADI_SID = pstAdv->Advertising_SID;
    stFormat.ADI_DID = pstAdv->u16CurrentDID;

    stFormat.AuxPtr_Present = true;
    stFormat.AuxPtr_ClockAccuracyPpm = pstAdv->u16ClockAccuracyPpm;
    stFormat.AuxPtr_Phy = pstAdv->enumSecondaryPhy;
    stFormat.AuxPtr_ChannelIndex = 0;//update before adv event
    stFormat.AuxPtr_Offset30Us = 0;//update before adv event

    if (!rom_llc_common_extended_advertising_payload_generate(pstPdu, &stFormat))
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, (uint32_t)&stFormat);
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }

    pstAdv->pstUpdatingPrimaryAdvertisingPdu = pstPdu;
    ////generate ADV_EXT_IND end
    
    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}

uint8_t rom_llc_ext_adv_pdu_manager_generate_AUX_ADV_IND_connectable_nonscannable(stLlcExtAdv_t* pstAdv, bool bDirected,
    uint8_t *pu8Data, uint8_t u8DataLength)
{
    uint16_t u16ExpectedPayloadSize;
    stLlcAdvChannelPdu_t* pstPdu;
    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat;
    
    ////generate AUX_ADV_IND start
    //AdvMode:01b AdvA:M TargetA:X/M CTEInfo:X ADI:M AuxPtr:X SyncInfo:X TxPower:O ACAD:O AdvData:O
    u16ExpectedPayloadSize = rom_llc_common_extended_advertising_payload_calculate_payload_length(true, bDirected, false, true, false, false, false, 0, u8DataLength);
    if (u16ExpectedPayloadSize > LLC_SPEC_5_1_ADV_CHANNEL_PDU_MAX_PAYLOAD_SIZE)
    {
        return LLC_SPEC_5_1_ERROR_CODE_PACKET_TOO_LONG;
    }
    pstPdu = rom_llc_ext_adv_alloc_adv_channel_pdu(u16ExpectedPayloadSize, true);
    if (NULL == pstPdu)
    {
        return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
    }
    if (pstPdu->u16PayloadBufSize < u16ExpectedPayloadSize)
    {
        rom_llc_ext_adv_free_adv_channel_pdu(pstPdu);
        return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
    }

    memset(&stFormat, 0, sizeof(stFormat));
    
    stFormat.Header.bitField.PDUType = LLC_SPEC_5_1_ADV_CH_PDU_TYPE_AUX_ADV_IND;
    
    stFormat.Connectable = true;
    stFormat.Scannable = false;

    stFormat.AdvA_Present = true;
    stFormat.AdvA = pstAdv->pu8OwnAddress;
    stFormat.Header.bitField.TxAdd = pstAdv->bOwnAddressTxAdd;
    
    if (bDirected)
    {
        stFormat.TargetA_Present = true;
        stFormat.TargetA = pstAdv->pu8PeerAddress;
        stFormat.Header.bitField.RxAdd = pstAdv->bPeerAddressRxAdd;
    }

    stFormat.ADI_Present = true;
    stFormat.ADI_SID = pstAdv->Advertising_SID;
    stFormat.ADI_DID = pstAdv->u16CurrentDID;
    
    stFormat.AdvData_Present = true;
    stFormat.AdvData_Length = u8DataLength;
    stFormat.AdvData = pu8Data;

    if (!rom_llc_common_extended_advertising_payload_generate(pstPdu, &stFormat))
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, (uint32_t)&stFormat);
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }

    pstAdv->pstUpdatingSecondaryAdvertisingPdu = pstPdu;
    ////generate AUX_ADV_IND end

    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}

uint8_t rom_llc_ext_adv_pdu_manager_generate_AUX_CONNECT_RSP_connectable_nonscannable(stLlcExtAdv_t* pstAdv)
{
    uint16_t u16ExpectedPayloadSize;
    stLlcAdvChannelPdu_t* pstPdu;
    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat;

    ////generate AUX_CONNECT_RSP start
    //AdvMode:00b AdvA:M TargetA:M CTEInfo:X ADI:X AuxPtr:X SyncInfo:X TxPower:X ACAD:X AdvData:X
    u16ExpectedPayloadSize = rom_llc_common_extended_advertising_payload_calculate_payload_length(true, true, false, false, false, false, false, 0, 0);
    pstPdu = rom_llc_ext_adv_alloc_adv_channel_pdu(u16ExpectedPayloadSize, true);
    if (NULL == pstPdu)
    {
        return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
    }
    if (pstPdu->u16PayloadBufSize < u16ExpectedPayloadSize)
    {
        rom_llc_ext_adv_free_adv_channel_pdu(pstPdu);
        return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
    }

    memset(&stFormat, 0, sizeof(stFormat));
    
    stFormat.Header.bitField.PDUType = LLC_SPEC_5_1_ADV_CH_PDU_TYPE_AUX_CONNECT_RSP;
    
    stFormat.Connectable = false;
    stFormat.Scannable = false;

    stFormat.AdvA_Present = true;
    stFormat.AdvA = pstAdv->pu8OwnAddress;
    stFormat.Header.bitField.TxAdd = pstAdv->bOwnAddressTxAdd;

    //update before sending
    uint8_t pu8TargetA[6] = {0};
    stFormat.TargetA_Present = true;
    stFormat.TargetA = pu8TargetA;
    stFormat.Header.bitField.RxAdd = false;

    if (!rom_llc_common_extended_advertising_payload_generate(pstPdu, &stFormat))
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, (uint32_t)&stFormat);
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }
    pstAdv->pstUpdatingResponsePdu = pstPdu;
    ////generate AUX_CONNECT_RSP end
    
    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}


uint8_t rom_llc_ext_adv_pdu_manager_generate_new_pdus_connectable_nonscannable(stLlcExtAdv_t* pstAdv, bool bIsComplete, bool bDirected,
    uint8_t *pu8Data, uint8_t u8DataLength)
{
    uint8_t u8ErrCode;
    
    u8ErrCode = rom_llc_ext_adv_pdu_manager_generate_ADV_EXT_IND_connectable_nonscannable(pstAdv);
    if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrCode)
    {
        return u8ErrCode;
    }
    
    u8ErrCode = rom_llc_ext_adv_pdu_manager_generate_AUX_ADV_IND_connectable_nonscannable(pstAdv, bDirected, pu8Data, u8DataLength);
    if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrCode)
    {
        return u8ErrCode;
    }
    
    return rom_llc_ext_adv_pdu_manager_generate_AUX_CONNECT_RSP_connectable_nonscannable(pstAdv);
}

uint8_t rom_llc_ext_adv_pdu_manager_generate_ADV_EXT_IND_nonconnectable_scannable(stLlcExtAdv_t* pstAdv)
{
    uint16_t u16ExpectedPayloadSize;
    stLlcAdvChannelPdu_t* pstPdu;
    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat;

    ////generate ADV_EXT_IND start
    //AdvMode:10b AdvA:X TargetA:X CTEInfo:X ADI:M AuxPtr:M SyncInfo:X TxPower:C1 ACAD:X AdvData:X
    u16ExpectedPayloadSize = rom_llc_common_extended_advertising_payload_calculate_payload_length(false, false, false, true, true, false, false, 0, 0);
    pstPdu = rom_llc_ext_adv_alloc_adv_channel_pdu(u16ExpectedPayloadSize, true);
    if (NULL == pstPdu)
    {
        return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
    }
    if (pstPdu->u16PayloadBufSize < u16ExpectedPayloadSize)
    {
        rom_llc_ext_adv_free_adv_channel_pdu(pstPdu);
        return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
    }

    memset(&stFormat, 0, sizeof(stFormat));
    
    stFormat.Header.bitField.PDUType = LLC_SPEC_5_1_ADV_CH_PDU_TYPE_ADV_EXT_IND;
    
    stFormat.Connectable = false;
    stFormat.Scannable = true;

    stFormat.ADI_Present = true;
    stFormat.ADI_SID = pstAdv->Advertising_SID;
    stFormat.ADI_DID = pstAdv->u16CurrentDID;

    stFormat.AuxPtr_Present = true;
    stFormat.AuxPtr_ClockAccuracyPpm = pstAdv->u16ClockAccuracyPpm;
    stFormat.AuxPtr_Phy = pstAdv->enumSecondaryPhy;
    stFormat.AuxPtr_ChannelIndex = 0;//update before adv event
    stFormat.AuxPtr_Offset30Us = 0;//update before adv event

    if (!rom_llc_common_extended_advertising_payload_generate(pstPdu, &stFormat))
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, (uint32_t)&stFormat);
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }

    pstAdv->pstUpdatingPrimaryAdvertisingPdu = pstPdu;
    ////generate ADV_EXT_IND end
    
    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}

uint8_t rom_llc_ext_adv_pdu_manager_generate_AUX_ADV_IND_nonconnectable_scannable(stLlcExtAdv_t* pstAdv, bool bDirected)
{
    uint16_t u16ExpectedPayloadSize;
    stLlcAdvChannelPdu_t* pstPdu;
    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat;
    
    ////generate AUX_ADV_IND start
    //AdvMode:10b AdvA:M TargetA:X/M CTEInfo:X ADI:M AuxPtr:X SyncInfo:X TxPower:O ACAD:O AdvData:X
    u16ExpectedPayloadSize = rom_llc_common_extended_advertising_payload_calculate_payload_length(true, bDirected, false, true, false, false, false, 0, 0);
    pstPdu = rom_llc_ext_adv_alloc_adv_channel_pdu(u16ExpectedPayloadSize, true);
    if (NULL == pstPdu)
    {
        return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
    }
    if (pstPdu->u16PayloadBufSize < u16ExpectedPayloadSize)
    {
        rom_llc_ext_adv_free_adv_channel_pdu(pstPdu);
        return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
    }

    memset(&stFormat, 0, sizeof(stFormat));
    
    stFormat.Header.bitField.PDUType = LLC_SPEC_5_1_ADV_CH_PDU_TYPE_AUX_ADV_IND;
    
    stFormat.Connectable = false;
    stFormat.Scannable = true;

    stFormat.AdvA_Present = true;
    stFormat.AdvA = pstAdv->pu8OwnAddress;
    stFormat.Header.bitField.TxAdd = pstAdv->bOwnAddressTxAdd;

    if (bDirected)
    {
        stFormat.TargetA_Present = true;
        stFormat.TargetA = pstAdv->pu8PeerAddress;
        stFormat.Header.bitField.RxAdd = pstAdv->bPeerAddressRxAdd;
    }
    
    stFormat.ADI_Present = true;
    stFormat.ADI_SID = pstAdv->Advertising_SID;
    stFormat.ADI_DID = pstAdv->u16CurrentDID;

    if (!rom_llc_common_extended_advertising_payload_generate(pstPdu, &stFormat))
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, (uint32_t)&stFormat);
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }

    pstAdv->pstUpdatingSecondaryAdvertisingPdu = pstPdu;
    ////generate AUX_ADV_IND end

    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}

uint8_t rom_llc_ext_adv_pdu_manager_generate_AUX_SCAN_RSP_and_AUX_CHAIN_INDs_nonconnectable_scannable(stLlcExtAdv_t* pstAdv, bool bIsComplete,
    stLlcExtAdvPduManagerConcatBuffer_t* pstData)
{
    uint16_t u16ExpectedPayloadSize;
    stLlcAdvChannelPdu_t* pstPdu;
    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat;

    uint16_t u16ConcatDataLength = pstData->u16Buf1Length + pstData->u16Buf2Length;
    
    ////generate AUX_SCAN_RSP start
    //AdvMode:00b AdvA:M TargetA:X CTEInfo:X ADI:O AuxPtr:O SyncInfo:X TxPower:O ACAD:O AdvData:M
    
    //assume one packet could hold all data
    u16ExpectedPayloadSize = rom_llc_common_extended_advertising_payload_calculate_payload_length(true, false, false, true, !bIsComplete, false, false, 0, u16ConcatDataLength);
    pstPdu = rom_llc_ext_adv_alloc_adv_channel_pdu(u16ExpectedPayloadSize, true);
    if (NULL == pstPdu)
    {
        return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
    }
    if ((pstPdu->u16PayloadBufSize >= u16ExpectedPayloadSize) && (u16ExpectedPayloadSize <= LLC_SPEC_5_1_ADV_CHANNEL_PDU_MAX_PAYLOAD_SIZE))
    {
        ////one packet could hold all data
        memset(&stFormat, 0, sizeof(stFormat));
        
        stFormat.Header.bitField.PDUType = LLC_SPEC_5_1_ADV_CH_PDU_TYPE_AUX_SCAN_RSP;
        
        stFormat.Connectable = false;
        stFormat.Scannable = false;

        stFormat.AdvA_Present = true;
        stFormat.AdvA = pstAdv->pu8OwnAddress;
        stFormat.Header.bitField.TxAdd = pstAdv->bOwnAddressTxAdd;

        stFormat.ADI_Present = true;
        stFormat.ADI_SID = pstAdv->Advertising_SID;
        stFormat.ADI_DID = pstAdv->u16CurrentDID;

        if (!bIsComplete)
        {
            stFormat.AuxPtr_Present = true;
            stFormat.AuxPtr_ClockAccuracyPpm = pstAdv->u16ClockAccuracyPpm;
            stFormat.AuxPtr_Phy = pstAdv->enumSecondaryPhy;
            stFormat.AuxPtr_ChannelIndex = 0;//update before adv event
            stFormat.AuxPtr_Offset30Us = 0;//update before adv event
        }
        
        stFormat.AdvData_Present = true;
        stFormat.AdvData = pstData->pu8Buf1;
        stFormat.AdvData_Length = pstData->u16Buf1Length;
        
        if (!rom_llc_common_extended_advertising_payload_generate(pstPdu, &stFormat))
        {
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, (uint32_t)&stFormat);
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }
        if (!rom_llc_common_extended_advertising_payload_append_AdvData(pstPdu, pstData->pu8Buf2, pstData->u16Buf2Length))
        {
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, (uint32_t)pstData);
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }
        
        pstAdv->pstUpdatingResponsePdu = pstPdu;
        return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
    }
    
    ////one packet could not hold all data
    uint16_t u16PduCapacity = 0;

    u16PduCapacity = rom_llc_common_extended_advertising_payload_get_avaliable_AdvData_length(pstPdu->u16PayloadBufSize, true, false, false, true, true, false, false, 0);
    if (u16PduCapacity >= u16ConcatDataLength)
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, u16PduCapacity|(((uint32_t)u16ConcatDataLength)<<16));
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }
    
    memset(&stFormat, 0, sizeof(stFormat));
    
    stFormat.Header.bitField.PDUType = LLC_SPEC_5_1_ADV_CH_PDU_TYPE_AUX_SCAN_RSP;
    
    stFormat.Connectable = false;
    stFormat.Scannable = false;

    stFormat.AdvA_Present = true;
    stFormat.AdvA = pstAdv->pu8OwnAddress;
    stFormat.Header.bitField.TxAdd = pstAdv->bOwnAddressTxAdd;

    stFormat.ADI_Present = true;
    stFormat.ADI_SID = pstAdv->Advertising_SID;
    stFormat.ADI_DID = pstAdv->u16CurrentDID;

    stFormat.AuxPtr_Present = true;
    stFormat.AuxPtr_ClockAccuracyPpm = pstAdv->u16ClockAccuracyPpm;
    stFormat.AuxPtr_Phy = pstAdv->enumSecondaryPhy;
    stFormat.AuxPtr_ChannelIndex = 0;//update before adv event
    stFormat.AuxPtr_Offset30Us = 0;//update before adv event

    stFormat.AdvData_Present = true;
    if (u16PduCapacity <= pstData->u16Buf1Length)
    {
        stFormat.AdvData = pstData->pu8Buf1;
        stFormat.AdvData_Length = u16PduCapacity;
        if (!rom_llc_common_extended_advertising_payload_generate(pstPdu, &stFormat))
        {
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, (uint32_t)&stFormat);
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }
    }
    else
    {
        stFormat.AdvData = pstData->pu8Buf1;
        stFormat.AdvData_Length = pstData->u16Buf1Length;
        if (!rom_llc_common_extended_advertising_payload_generate(pstPdu, &stFormat))
        {
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, (uint32_t)&stFormat);
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }
        if (!rom_llc_common_extended_advertising_payload_append_AdvData(pstPdu, pstData->pu8Buf2, u16PduCapacity - pstData->u16Buf1Length))
        {
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, u16PduCapacity - pstData->u16Buf1Length);
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }
    }
    pstAdv->pstUpdatingResponsePdu = pstPdu;

    ////generate AUX_SCAN_RSP end
    
    
    ////generate AUX_CHAIN_IND start
    stLlcExtAdvPduManagerConcatBuffer_t stData;
    memset(&stData, 0, sizeof(stData));
    if (u16PduCapacity < pstData->u16Buf1Length)
    {
        stData.pu8Buf1 = &pstData->pu8Buf1[u16PduCapacity];
        stData.u16Buf1Length = pstData->u16Buf1Length - u16PduCapacity;
        stData.pu8Buf2 = pstData->pu8Buf2;
        stData.u16Buf2Length = pstData->u16Buf2Length;
    }
    else
    {
        stData.pu8Buf1 = &pstData->pu8Buf2[u16PduCapacity - pstData->u16Buf1Length];
        stData.u16Buf1Length = u16ConcatDataLength - u16PduCapacity;
    }
    
    return rom_llc_ext_adv_pdu_manager_generate_new_chain_pdus(bIsComplete,
        pstAdv->u16CurrentDID, pstAdv->Advertising_SID,
        pstAdv->u16ClockAccuracyPpm, pstAdv->enumSecondaryPhy,
        &stData,
        &pstAdv->pstUpdatingChainPdusHead, &pstAdv->pstUpdatingChainPdusTail);
    ////generate AUX_CHAIN_IND end
}

uint8_t rom_llc_ext_adv_pdu_manager_generate_new_pdus_nonconnectable_scannable(stLlcExtAdv_t* pstAdv, bool bIsComplete, bool bDirected,
    uint8_t *pu8Data, uint8_t u8DataLength)
{
    uint8_t u8ErrCode;
    
    u8ErrCode = rom_llc_ext_adv_pdu_manager_generate_ADV_EXT_IND_nonconnectable_scannable(pstAdv);
    if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrCode)
    {
        return u8ErrCode;
    }
    
    u8ErrCode = rom_llc_ext_adv_pdu_manager_generate_AUX_ADV_IND_nonconnectable_scannable(pstAdv,bDirected);
    if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrCode)
    {
        return u8ErrCode;
    }
    
    stLlcExtAdvPduManagerConcatBuffer_t stData;
    memset(&stData, 0, sizeof(stData));
    stData.pu8Buf1 = pu8Data;
    stData.u16Buf1Length = u8DataLength;
    return rom_llc_ext_adv_pdu_manager_generate_AUX_SCAN_RSP_and_AUX_CHAIN_INDs_nonconnectable_scannable(pstAdv, bIsComplete, &stData);
}


uint8_t rom_llc_ext_adv_pdu_manager_append_data_to_chain_pdus(stLlcExtAdv_t* pstAdv, bool bIsComplete, bool bFragmentPreferred,
    uint8_t *pu8Data, uint8_t u8DataLength)
{
    if ((NULL == pstAdv->pstUpdatingChainPdusHead)||(NULL == pstAdv->pstUpdatingChainPdusTail))
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstAdv, 0);
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }
    
    
    uint16_t u16ExpectedPayloadSize;
    stLlcAdvChannelPdu_t* pstPdu;
    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat;
    
    uint8_t u8ErrCode;
    
    if (bFragmentPreferred)
    {
        //create new chain pdus
        stLlcAdvChannelPdu_t* pstNewPduHead = NULL;
        stLlcAdvChannelPdu_t* pstNewPduTail = NULL;
        stLlcExtAdvPduManagerConcatBuffer_t stData;
        memset(&stData, 0, sizeof(stData));
        stData.pu8Buf1 = pu8Data;
        stData.u16Buf1Length = u8DataLength;
        u8ErrCode = rom_llc_ext_adv_pdu_manager_generate_new_chain_pdus(bIsComplete,
            pstAdv->u16CurrentDID, pstAdv->Advertising_SID,
            pstAdv->u16ClockAccuracyPpm, pstAdv->enumSecondaryPhy,
            &stData,
            &pstNewPduHead, &pstNewPduTail);
        
        if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrCode)
        {
            return u8ErrCode;
        }
        
        pstNewPduHead->prev = pstAdv->pstUpdatingChainPdusTail;
        pstAdv->pstUpdatingChainPdusTail->next = pstNewPduHead;
        pstAdv->pstUpdatingChainPdusTail = pstNewPduTail;
        return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
    }

    
    //get tail pdu data
    pstPdu = pstAdv->pstUpdatingChainPdusTail;
    if (!rom_llc_common_extended_advertising_payload_parse(pstPdu, &stFormat))
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, (uint32_t)&stFormat);
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }
    
    //create new chain pdus
    stLlcExtAdvPduManagerConcatBuffer_t stConcatData;
    memset(&stConcatData, 0, sizeof(stConcatData));
    if (0 == stFormat.AdvData_Length)
    {
        stConcatData.pu8Buf1 = pu8Data;
        stConcatData.u16Buf1Length = u8DataLength;
    }
    else
    {
        stConcatData.pu8Buf1 = stFormat.AdvData;
        stConcatData.u16Buf1Length = stFormat.AdvData_Length;
        
        stConcatData.pu8Buf2 = pu8Data;
        stConcatData.u16Buf2Length = u8DataLength;
    }
    
    stLlcAdvChannelPdu_t* pstNewPduHead = NULL;
    stLlcAdvChannelPdu_t* pstNewPduTail = NULL;
    
    u8ErrCode = rom_llc_ext_adv_pdu_manager_generate_new_chain_pdus(bIsComplete,
        pstAdv->u16CurrentDID, pstAdv->Advertising_SID,
        pstAdv->u16ClockAccuracyPpm, pstAdv->enumSecondaryPhy,
        &stConcatData,
        &pstNewPduHead, &pstNewPduTail);
    
    if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrCode)
    {
        return u8ErrCode;
    }
    
    //replace pdu tail
    if (pstAdv->pstUpdatingChainPdusHead == pstAdv->pstUpdatingChainPdusTail)
    {
        rom_llc_ext_adv_free_adv_channel_pdu(pstAdv->pstUpdatingChainPdusTail);
        
        pstAdv->pstUpdatingChainPdusHead = pstNewPduHead;
        pstAdv->pstUpdatingChainPdusTail = pstNewPduTail;
    }
    else
    {
        pstAdv->pstUpdatingChainPdusTail->prev->next = pstNewPduHead;
        pstNewPduHead->prev = pstAdv->pstUpdatingChainPdusTail->prev;
        
        rom_llc_ext_adv_free_adv_channel_pdu(pstAdv->pstUpdatingChainPdusTail);
        
        pstAdv->pstUpdatingChainPdusTail = pstNewPduTail;
    }
    
    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}

uint8_t rom_llc_ext_adv_pdu_manager_append_data_to_existing_pdus_nonconnectable_nonscannable(stLlcExtAdv_t *pstAdv, bool bIsComplete, bool bFragmentPreferred,
    uint8_t *pu8Data, uint8_t u8DataLength)
{
    if (NULL != pstAdv->pstUpdatingChainPdusHead)
    {
        return rom_llc_ext_adv_pdu_manager_append_data_to_chain_pdus(pstAdv, bIsComplete, bFragmentPreferred, pu8Data, u8DataLength);
    }
    
    if (NULL == pstAdv->pstUpdatingSecondaryAdvertisingPdu)
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstAdv, 0);
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }
    
    if (bFragmentPreferred)
    {
        //just generate new chain pdus
        stLlcExtAdvPduManagerConcatBuffer_t stData;
        memset(&stData, 0, sizeof(stData));
        stData.pu8Buf1 = pu8Data;
        stData.u16Buf1Length = u8DataLength;
        
        return rom_llc_ext_adv_pdu_manager_generate_new_chain_pdus(bIsComplete,
            pstAdv->u16CurrentDID, pstAdv->Advertising_SID,
            pstAdv->u16ClockAccuracyPpm, pstAdv->enumSecondaryPhy,
            &stData,
            &pstAdv->pstUpdatingChainPdusHead, &pstAdv->pstUpdatingChainPdusTail);
    }
    
    
    stLlcAdvChannelPdu_t* pstPdu;
    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat;
    
    //get existing data
    pstPdu = pstAdv->pstUpdatingSecondaryAdvertisingPdu;
    if (!rom_llc_common_extended_advertising_payload_parse(pstPdu, &stFormat))
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, (uint32_t)&stFormat);
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }
    
    bool bAnonymous = !stFormat.AdvA_Present;
    bool bDirected = stFormat.TargetA_Present;
    
    //concat existing data with new data
    stLlcExtAdvPduManagerConcatBuffer_t stConcatData;
    memset(&stConcatData, 0, sizeof(stConcatData));
    if (0 == stFormat.AdvData_Length)
    {
        stConcatData.pu8Buf1 = pu8Data;
        stConcatData.u16Buf1Length = u8DataLength;
    }
    else
    {
        stConcatData.pu8Buf1 = stFormat.AdvData;
        stConcatData.u16Buf1Length = stFormat.AdvData_Length;
        stConcatData.pu8Buf2 = pu8Data;
        stConcatData.u16Buf2Length = u8DataLength;
    }
    
    //keep pdu to avoid existing-data-lost after destroy all pdus
    stLlcAdvChannelPdu_t* pstDataKeepPdu = pstPdu = pstAdv->pstUpdatingSecondaryAdvertisingPdu;
    pstPdu = pstAdv->pstUpdatingSecondaryAdvertisingPdu = NULL;
    
    
    //destroy all pdus
    rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(pstAdv);
    
    
    //re-generate pdus
    uint8_t u8ErrCode;
    u8ErrCode = rom_llc_ext_adv_pdu_manager_generate_ADV_EXT_IND_nonconnectable_nonscannable(pstAdv);
    if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrCode)
    {
        //generate fail, free keeping pdu
        rom_llc_ext_adv_free_adv_channel_pdu(pstDataKeepPdu);
        return u8ErrCode;
    }
    
    u8ErrCode = rom_llc_ext_adv_pdu_manager_generate_AUX_ADV_IND_and_AUX_CHAIN_INDs_nonconnectable_nonscannable(pstAdv,bIsComplete,bDirected,bAnonymous,&stConcatData);
    
    //generate done, free keeping pdu
    rom_llc_ext_adv_free_adv_channel_pdu(pstDataKeepPdu);
    
    return u8ErrCode;
}


uint8_t rom_llc_ext_adv_pdu_manager_append_data_to_existing_pdus_connectable_nonscannable(stLlcExtAdv_t *pstAdv, bool bIsComplete, bool bFragmentPreferred,
    uint8_t *pu8Data, uint8_t u8DataLength)
{
    if (NULL == pstAdv->pstUpdatingResponsePdu)
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstAdv, 0);
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }
    
    uint16_t u16ExpectedPayloadSize;
    stLlcAdvChannelPdu_t* pstPdu;
    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat;
    uint8_t u8ErrCode;
    
    //check AUX_SCAN_RSP
    //AdvMode:01b AdvA:M TargetA:X/M CTEInfo:X ADI:M AuxPtr:X SyncInfo:X TxPower:O ACAD:O AdvData:O
    pstPdu = pstAdv->pstUpdatingResponsePdu;
    if (!rom_llc_common_extended_advertising_payload_parse(pstPdu, &stFormat))
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, (uint32_t)&stFormat);
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }
    uint16_t u16ExistingDataLength = stFormat.AdvData_Length;
    uint16_t u16DataCapacity = rom_llc_common_extended_advertising_payload_get_avaliable_AdvData_length(pstPdu->u16PayloadBufSize,
        stFormat.AdvA_Present, stFormat.TargetA_Present, stFormat.CTEInfo_Present, stFormat.ADI_Present, 
            stFormat.AuxPtr_Present, stFormat.SyncInfo_Present, stFormat.TxPower_Present, stFormat.ACAD_Length);
    
    if (u16DataCapacity >= u16ExistingDataLength + u8DataLength)
    {
        //existing pdu could hold all data
        if (!rom_llc_common_extended_advertising_payload_append_AdvData(pstPdu, pu8Data, u8DataLength))
        {
            system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, u8DataLength);
            return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
        }
        
        return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
    }
    
    //existing pdu could not hold all additional data
    
    //alloc new PDU
    u16ExpectedPayloadSize = rom_llc_common_extended_advertising_payload_calculate_payload_length(stFormat.AdvA_Present, stFormat.TargetA_Present, stFormat.CTEInfo_Present, 
        stFormat.ADI_Present, stFormat.AuxPtr_Present, stFormat.SyncInfo_Present, stFormat.TxPower_Present, stFormat.ACAD_Length, 
            u16ExistingDataLength + u8DataLength);
    pstPdu = rom_llc_ext_adv_alloc_adv_channel_pdu(u16ExpectedPayloadSize, true);
    if (NULL == pstPdu)
    {
        return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
    }
    u16DataCapacity = rom_llc_common_extended_advertising_payload_get_avaliable_AdvData_length(pstPdu->u16PayloadBufSize,
        stFormat.AdvA_Present, stFormat.TargetA_Present, stFormat.CTEInfo_Present, stFormat.ADI_Present, 
            stFormat.AuxPtr_Present, stFormat.SyncInfo_Present, stFormat.TxPower_Present, stFormat.ACAD_Length);
    if (u16DataCapacity < u16ExistingDataLength + u8DataLength)
    {
        rom_llc_ext_adv_free_adv_channel_pdu(pstPdu);
        return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
    }
    
    if (!rom_llc_common_extended_advertising_payload_generate(pstPdu, &stFormat))
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, (uint32_t)&stFormat);
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }
    
    if (!rom_llc_common_extended_advertising_payload_append_AdvData(pstPdu, pu8Data, u8DataLength))
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, u8DataLength);
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }
    
    rom_llc_ext_adv_free_adv_channel_pdu(pstAdv->pstUpdatingResponsePdu);
    pstAdv->pstUpdatingResponsePdu = pstPdu;
    
    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}



uint8_t rom_llc_ext_adv_pdu_manager_append_data_to_existing_pdus_nonconnectable_scannable(stLlcExtAdv_t *pstAdv, bool bIsComplete, bool bFragmentPreferred,
    uint8_t *pu8Data, uint8_t u8DataLength)
{
    if (NULL != pstAdv->pstUpdatingChainPdusHead)
    {
        return rom_llc_ext_adv_pdu_manager_append_data_to_chain_pdus(pstAdv, bIsComplete, bFragmentPreferred, pu8Data, u8DataLength);
    }
    
    if ((NULL == pstAdv->pstUpdatingSecondaryAdvertisingPdu)||(NULL == pstAdv->pstUpdatingResponsePdu))
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstAdv, 0);
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }
    
    if (bFragmentPreferred)
    {
        //just generate new chain pdus
        stLlcExtAdvPduManagerConcatBuffer_t stData;
        memset(&stData, 0, sizeof(stData));
        stData.pu8Buf1 = pu8Data;
        stData.u16Buf1Length = u8DataLength;
        
        return rom_llc_ext_adv_pdu_manager_generate_new_chain_pdus(bIsComplete,
            pstAdv->u16CurrentDID, pstAdv->Advertising_SID,
            pstAdv->u16ClockAccuracyPpm, pstAdv->enumSecondaryPhy,
            &stData,
            &pstAdv->pstUpdatingChainPdusHead, &pstAdv->pstUpdatingChainPdusTail);
    }
    
    
    stLlcAdvChannelPdu_t* pstPdu;
    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat;
    
    //get IsDirected
    pstPdu = pstAdv->pstUpdatingSecondaryAdvertisingPdu;
    if (!rom_llc_common_extended_advertising_payload_parse(pstPdu, &stFormat))
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, (uint32_t)&stFormat);
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }
    bool bDirected = stFormat.TargetA_Present;
    
    //get existing data
    pstPdu = pstAdv->pstUpdatingResponsePdu;
    if (!rom_llc_common_extended_advertising_payload_parse(pstPdu, &stFormat))
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, (uint32_t)&stFormat);
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }
    
    //concat existing data with new data
    stLlcExtAdvPduManagerConcatBuffer_t stConcatData;
    memset(&stConcatData, 0, sizeof(stConcatData));
    if (0 == stFormat.AdvData_Length)
    {
        stConcatData.pu8Buf1 = pu8Data;
        stConcatData.u16Buf1Length = u8DataLength;
    }
    else
    {
        stConcatData.pu8Buf1 = stFormat.AdvData;
        stConcatData.u16Buf1Length = stFormat.AdvData_Length;
        stConcatData.pu8Buf2 = pu8Data;
        stConcatData.u16Buf2Length = u8DataLength;
    }
    
    //keep pdu to avoid existing-data-lost after destroy all pdus
    stLlcAdvChannelPdu_t* pstDataKeepPdu = pstPdu = pstAdv->pstUpdatingResponsePdu;
    pstPdu = pstAdv->pstUpdatingResponsePdu = NULL;
    
    //destroy all pdus
    rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(pstAdv);
    
    
    //re-generate pdus
    uint8_t u8ErrCode;
    u8ErrCode = rom_llc_ext_adv_pdu_manager_generate_ADV_EXT_IND_nonconnectable_scannable(pstAdv);
    if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrCode)
    {
        //generate fail, free keeping pdu
        rom_llc_ext_adv_free_adv_channel_pdu(pstDataKeepPdu);
        return u8ErrCode;
    }
    
    u8ErrCode = rom_llc_ext_adv_pdu_manager_generate_AUX_ADV_IND_nonconnectable_scannable(pstAdv, bDirected);
    if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrCode)
    {
        //generate fail, free keeping pdu
        rom_llc_ext_adv_free_adv_channel_pdu(pstDataKeepPdu);
        return u8ErrCode;
    }
    
    u8ErrCode = rom_llc_ext_adv_pdu_manager_generate_AUX_SCAN_RSP_and_AUX_CHAIN_INDs_nonconnectable_scannable(pstAdv, bIsComplete, &stConcatData);
    
    //generate done, free keeping pdu
    rom_llc_ext_adv_free_adv_channel_pdu(pstDataKeepPdu);
    
    return u8ErrCode;
}


void rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(stLlcExtAdv_t* pstAdv)
{
    if (NULL != pstAdv->pstUpdatingPrimaryAdvertisingPdu)
    {
        rom_llc_ext_adv_free_adv_channel_pdu(pstAdv->pstUpdatingPrimaryAdvertisingPdu);
        pstAdv->pstUpdatingPrimaryAdvertisingPdu = NULL;
    }

    if (NULL != pstAdv->pstUpdatingSecondaryAdvertisingPdu)
    {
        rom_llc_ext_adv_free_adv_channel_pdu(pstAdv->pstUpdatingSecondaryAdvertisingPdu);
        pstAdv->pstUpdatingSecondaryAdvertisingPdu = NULL;
    }

    if (NULL != pstAdv->pstUpdatingResponsePdu)
    {
        rom_llc_ext_adv_free_adv_channel_pdu(pstAdv->pstUpdatingResponsePdu);
        pstAdv->pstUpdatingResponsePdu = NULL;
    }

    stLlcAdvChannelPdu_t* pstPdu = pstAdv->pstUpdatingChainPdusHead;
    while(NULL != pstPdu)
    {
        stLlcAdvChannelPdu_t* pstFreePdu = pstPdu;
        pstPdu = pstPdu->next;

        rom_llc_ext_adv_free_adv_channel_pdu(pstFreePdu);
    }
    pstAdv->pstUpdatingChainPdusHead = NULL;
    pstAdv->pstUpdatingChainPdusTail = NULL;
    
    pstAdv->u16UpdatingAdvertisingDataLength = 0;
    pstAdv->u16UpdatingScanResponseDataLength = 0;
}

void rom_llc_ext_adv_pdu_manager_destroy_accepted_pdus(stLlcExtAdv_t* pstAdv)
{
    if (NULL != pstAdv->pstPrimaryAdvertisingPdu)
    {
        rom_llc_ext_adv_free_adv_channel_pdu(pstAdv->pstPrimaryAdvertisingPdu);
        pstAdv->pstPrimaryAdvertisingPdu = NULL;
    }

    if (NULL != pstAdv->pstSecondaryAdvertisingPdu)
    {
        rom_llc_ext_adv_free_adv_channel_pdu(pstAdv->pstSecondaryAdvertisingPdu);
        pstAdv->pstSecondaryAdvertisingPdu = NULL;
    }

    if (NULL != pstAdv->pstResponsePdu)
    {
        rom_llc_ext_adv_free_adv_channel_pdu(pstAdv->pstResponsePdu);
        pstAdv->pstResponsePdu = NULL;
    }
    
    stLlcAdvChannelPdu_t* pstPdu = pstAdv->pstChainPdusHead;
    while(NULL != pstPdu)
    {
        stLlcAdvChannelPdu_t* pstFreePdu = pstPdu;
        pstPdu = pstPdu->next;

        rom_llc_ext_adv_free_adv_channel_pdu(pstFreePdu);
    }
    pstAdv->pstChainPdusHead = NULL;
    pstAdv->pstChainPdusTail = NULL;
    
    pstAdv->u16AdvertisingDataLength = 0;
    pstAdv->u16ScanResponseDataLength = 0;
}

void rom_llc_ext_adv_pdu_manager_accept_updating_pdu(stLlcExtAdv_t* pstAdv)
{
    rom_llc_ext_adv_pdu_manager_destroy_accepted_pdus(pstAdv);
    
    pstAdv->pstPrimaryAdvertisingPdu = pstAdv->pstUpdatingPrimaryAdvertisingPdu;
    pstAdv->pstSecondaryAdvertisingPdu = pstAdv->pstUpdatingSecondaryAdvertisingPdu;
    pstAdv->pstResponsePdu = pstAdv->pstUpdatingResponsePdu;
    pstAdv->pstChainPdusHead = pstAdv->pstUpdatingChainPdusHead;
    pstAdv->pstChainPdusTail = pstAdv->pstUpdatingChainPdusTail;
    
    
    pstAdv->u32PrimaryAdvertisingPduOffsetToEachOther30us = pstAdv->u32UpdatingPrimaryAdvertisingPduOffsetToEachOther30us;
    pstAdv->u32PrimaryAdvEventOffsetToSecondaryAdvEvent30us = pstAdv->u32UpdatingPrimaryAdvEventOffsetToSecondaryAdvEvent30us;
    
    pstAdv->u32SecondaryAdvEventOffsetToChainTxUs = pstAdv->u32UpdatingSecondaryAdvEventOffsetToChainTxUs;
    pstAdv->u32PrimaryAdvertisingInterval30us = pstAdv->u32UpdatingPrimaryAdvertisingInterval30us;
    pstAdv->u16AdvertisingDataLength = pstAdv->u16UpdatingAdvertisingDataLength;
    pstAdv->u16ScanResponseDataLength = pstAdv->u16UpdatingScanResponseDataLength;
    
    
    pstAdv->pstUpdatingPrimaryAdvertisingPdu = NULL;
    pstAdv->pstUpdatingSecondaryAdvertisingPdu = NULL;
    pstAdv->pstUpdatingResponsePdu = NULL;
    pstAdv->pstUpdatingChainPdusHead = NULL;
    pstAdv->pstUpdatingChainPdusTail = NULL;
    
    pstAdv->u16UpdatingAdvertisingDataLength = 0;
    pstAdv->u16UpdatingScanResponseDataLength = 0;
}

uint8_t rom_llc_ext_adv_pdu_manager_generate_new_pdus(stLlcExtAdv_t* pstAdv, bool bIsComplete,
    uint8_t *pu8Data, uint8_t u8DataLength)
{
    
    bool bLegacy = LLC_EXT_ADV_IS_LEGACY(pstAdv->Advertising_Event_Properties);
    bool bConnectable = LLC_EXT_ADV_IS_CONNECTABLE(pstAdv->Advertising_Event_Properties);
    bool bScannable = LLC_EXT_ADV_IS_SCANNABLE(pstAdv->Advertising_Event_Properties);
    bool bDirected = LLC_EXT_ADV_IS_DIRECTED(pstAdv->Advertising_Event_Properties);
    bool bAnonymous = LLC_EXT_ADV_IS_ANONYMOUS(pstAdv->Advertising_Event_Properties);
    
    if (bLegacy)
    {
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }
    
    if (bConnectable && bScannable)
    {
        return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
    }
    
    if ((!bConnectable) && (!bScannable))
    {
        return rom_llc_ext_adv_pdu_manager_generate_new_pdus_nonconnectable_nonscannable(pstAdv, bIsComplete, bDirected, bAnonymous, pu8Data, u8DataLength);
    }
    
    if ((bConnectable) && (!bScannable))
    {
        return rom_llc_ext_adv_pdu_manager_generate_new_pdus_connectable_nonscannable(pstAdv, bIsComplete, bDirected, pu8Data, u8DataLength);
    }
    
    if ((!bConnectable) && (bScannable))
    {
        return rom_llc_ext_adv_pdu_manager_generate_new_pdus_nonconnectable_scannable(pstAdv, bIsComplete, bDirected, pu8Data, u8DataLength);
    }
    
    return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
}

uint8_t rom_llc_ext_adv_pdu_manager_append_data_to_existing_pdus(stLlcExtAdv_t* pstAdv, bool bIsComplete, bool bFragmentPreferred,
    uint8_t *pu8Data, uint8_t u8DataLength)
{
    
    bool bLegacy = LLC_EXT_ADV_IS_LEGACY(pstAdv->Advertising_Event_Properties);
    bool bConnectable = LLC_EXT_ADV_IS_CONNECTABLE(pstAdv->Advertising_Event_Properties);
    bool bScannable = LLC_EXT_ADV_IS_SCANNABLE(pstAdv->Advertising_Event_Properties);
    
    if (bLegacy)
    {
        return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
    }
    
    if (bConnectable && bScannable)
    {
        return false;
    }
    
    if ((!bConnectable) && (!bScannable))
    {
        return rom_llc_ext_adv_pdu_manager_append_data_to_existing_pdus_nonconnectable_nonscannable(pstAdv, bIsComplete, bFragmentPreferred, pu8Data, u8DataLength);
    }
    
    if ((bConnectable) && (!bScannable))
    {
        return rom_llc_ext_adv_pdu_manager_append_data_to_existing_pdus_connectable_nonscannable(pstAdv, bIsComplete, bFragmentPreferred, pu8Data, u8DataLength);
    }
    
    if ((!bConnectable) && (bScannable))
    {
        return rom_llc_ext_adv_pdu_manager_append_data_to_existing_pdus_nonconnectable_scannable(pstAdv, bIsComplete, bFragmentPreferred, pu8Data, u8DataLength);
    }
    
    return LLC_SPEC_5_1_ERROR_CODE_HARDWARE_FAILURE;
}


void rom_llc_ext_adv_pdu_manager_update_accepted_pdus_AdvA(stLlcExtAdv_t* pstAdv)
{
    stLlcAdvChannelPdu_t* pstPdu;
    
    pstPdu = pstAdv->pstPrimaryAdvertisingPdu;
    if (NULL != pstPdu)
    {
        rom_llc_common_extended_advertising_payload_update_AdvA(pstPdu, pstAdv->pu8OwnAddress, pstAdv->bOwnAddressTxAdd);
    }
    
    pstPdu = pstAdv->pstSecondaryAdvertisingPdu;
    if (NULL != pstPdu)
    {
        rom_llc_common_extended_advertising_payload_update_AdvA(pstPdu, pstAdv->pu8OwnAddress, pstAdv->bOwnAddressTxAdd);
    }
    
    pstPdu = pstAdv->pstResponsePdu;
    if (NULL != pstPdu)
    {
        rom_llc_common_extended_advertising_payload_update_AdvA(pstPdu, pstAdv->pu8OwnAddress, pstAdv->bOwnAddressTxAdd);
    }
    
}


void rom_llc_ext_adv_pdu_manager_update_accepted_pdus_TargetA(stLlcExtAdv_t* pstAdv)
{
    stLlcAdvChannelPdu_t* pstPdu;
    
    pstPdu = pstAdv->pstPrimaryAdvertisingPdu;
    if (NULL != pstPdu)
    {
        rom_llc_common_extended_advertising_payload_update_TargetA(pstPdu, pstAdv->pu8PeerAddress, pstAdv->bPeerAddressRxAdd);
    }
    
    pstPdu = pstAdv->pstSecondaryAdvertisingPdu;
    if (NULL != pstPdu)
    {
        rom_llc_common_extended_advertising_payload_update_TargetA(pstPdu, pstAdv->pu8PeerAddress, pstAdv->bPeerAddressRxAdd);
    }
    
    pstPdu = pstAdv->pstResponsePdu;
    if (NULL != pstPdu)
    {
        rom_llc_common_extended_advertising_payload_update_TargetA(pstPdu, pstAdv->pu8PeerAddress, pstAdv->bPeerAddressRxAdd);
    }
    
}


void rom_llc_ext_adv_pdu_manager_update_accepted_connect_response_pdu_TargetA(stLlcExtAdv_t* pstAdv, bool bRxAdd, uint8_t pu8TargetA[6])
{
#if 1
    //better perfomance
    
    stLlcAdvChannelPdu_t* pstPdu = pstAdv->pstResponsePdu;
    stLlcSpec51AdvChannelPduHeader_t stHeader;
    stHeader.u16Value = pstPdu->u32Header & 0xFFFF;
    stHeader.bitField.RxAdd = bRxAdd;
    pstPdu->u32Header = stHeader.u16Value;
    
    //Extended Header Length(6 bits) + AdvMode(2 bits) + Extended Header Flags(1 octets) + AdvA(6 octets)
    
    memcpy(&pstPdu->pu8Payload[8], pu8TargetA, 6);
    
#else
    rom_llc_common_extended_advertising_payload_update_TargetA(pstAdv->pstResponsePdu, pu8TargetA, bRxAdd);
#endif
}


void rom_llc_ext_adv_pdu_manager_update_accepted_pdus_ADI(stLlcExtAdv_t* pstAdv)
{
    stLlcAdvChannelPdu_t* pstPdu;
    
    pstPdu = pstAdv->pstPrimaryAdvertisingPdu;
    if (NULL != pstPdu)
    {
        rom_llc_common_extended_advertising_payload_update_ADI(pstPdu, pstAdv->u16CurrentDID, pstAdv->Advertising_SID);
    }
    
    pstPdu = pstAdv->pstSecondaryAdvertisingPdu;
    if (NULL != pstPdu)
    {
        rom_llc_common_extended_advertising_payload_update_ADI(pstPdu, pstAdv->u16CurrentDID, pstAdv->Advertising_SID);
    }
    
    pstPdu = pstAdv->pstResponsePdu;
    if (NULL != pstPdu)
    {
        rom_llc_common_extended_advertising_payload_update_ADI(pstPdu, pstAdv->u16CurrentDID, pstAdv->Advertising_SID);
    }
    
    pstPdu = pstAdv->pstChainPdusHead;
    while(NULL != pstPdu)
    {
        rom_llc_common_extended_advertising_payload_update_ADI(pstPdu, pstAdv->u16CurrentDID, pstAdv->Advertising_SID);
        
        pstPdu = pstPdu->next;
    }
}


void rom_llc_ext_adv_pdu_manager_update_accepted_primary_pdus_AuxPtr(stLlcExtAdv_t* pstAdv, uint32_t u32AuxPtrOffset30Us)
{
    if ((NULL == pstAdv->pstPrimaryAdvertisingPdu))
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstAdv, u32AuxPtrOffset30Us);
        return;
    }
    
    if (NULL == pstAdv->pstSecondaryAdvertisingPdu)
    {
        //no aux ptr present
        return;
    }
    
    stLlcAdvChannelPdu_t* pstPdu = NULL;
    
    //update ADV_EXT_IND
    pstPdu = pstAdv->pstPrimaryAdvertisingPdu;
    if (!rom_llc_common_extended_advertising_payload_update_AuxPtr(pstPdu, pstAdv->u8CurSecondaryChannelIdx, pstAdv->u16ClockAccuracyPpm, u32AuxPtrOffset30Us, pstAdv->enumSecondaryPhy))
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, (uint32_t)pstAdv);
        return;
    }
}


void rom_llc_ext_adv_pdu_manager_update_secondary_pdu_AuxPtr(stLlcExtAdv_t* pstAdv, stLlcAdvChannelPdu_t* pstPdu)
{
    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat = {0};
    stLlcExtAdvPduExtraInfo_t* pstExtraInfo = NULL;
    uint32_t u32AuxPtrOffset30Us = 0;
    
    if (NULL == pstPdu)
    {
        return;
    }
    
    if (!rom_llc_common_extended_advertising_payload_parse(pstPdu, &stFormat))
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, (uint32_t)pstAdv);
        return;
    }
    if (!stFormat.AuxPtr_Present)
    {
        return;
    }
    
    pstExtraInfo = rom_llc_ext_adv_get_pdu_extra_info(pstPdu);
    if (NULL == pstExtraInfo)
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, (uint32_t)pstAdv);
        return;
    }
    u32AuxPtrOffset30Us = pstExtraInfo->u32AuxPduOffset30us;
    if (!rom_llc_common_extended_advertising_payload_update_AuxPtr(pstPdu, pstAdv->u8CurSecondaryChannelIdx, pstAdv->u16ClockAccuracyPpm, u32AuxPtrOffset30Us, pstAdv->enumSecondaryPhy))
    {
        system_error(SYS_ERR_MODULE_LLC_EXT_ADV_PDU_MANAGER, __LINE__, (uint32_t)pstPdu, (uint32_t)pstAdv);
        return;
    }
}


void rom_llc_ext_adv_pdu_manager_update_accepted_secondary_pdus_AuxPtr(stLlcExtAdv_t* pstAdv)
{
    rom_llc_ext_adv_pdu_manager_update_secondary_pdu_AuxPtr(pstAdv, pstAdv->pstSecondaryAdvertisingPdu);
    
    rom_llc_ext_adv_pdu_manager_update_secondary_pdu_AuxPtr(pstAdv, pstAdv->pstResponsePdu);
    
    stLlcAdvChannelPdu_t* pstPdu = pstAdv->pstChainPdusHead;
    while(NULL != pstPdu)
    {
        rom_llc_ext_adv_pdu_manager_update_secondary_pdu_AuxPtr(pstAdv, pstPdu);
        
        pstPdu = pstPdu->next;
    }
}


uint8_t rom_llc_ext_adv_pdu_manager_set_advertising_data(stLlcExtAdv_t* pstAdv, EN_LLC_SPEC_5_1_HCI_ADV_DATA_OPERATION_T enumOperation, bool bFragmentPreferred, uint8_t *pu8Data, uint8_t u8DataLength)
{
    bool bConnectable = LLC_EXT_ADV_IS_CONNECTABLE(pstAdv->Advertising_Event_Properties);
    bool bScannable = LLC_EXT_ADV_IS_SCANNABLE(pstAdv->Advertising_Event_Properties);
    
    ////extended adv
    
    bool bIsAdvDataSupported = bConnectable||(!bConnectable && !bScannable);
    if (!bIsAdvDataSupported)
    {
        if ((LLC_SPEC_5_1_HCI_ADV_DATA_OPERATION_COMPLETE != enumOperation) || (0 != u8DataLength))
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
    }
    
    
    uint8_t u8ErrCode;
    switch(enumOperation)
    {
        case LLC_SPEC_5_1_HCI_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT:
        {
            if (0 == pstAdv->u16UpdatingAdvertisingDataLength)
            {
                return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
            }
            
            if (pstAdv->u16UpdatingAdvertisingDataLength + u8DataLength > LLC_SPEC_5_1_EXTENDED_ADV_MAX_ADVERTISING_DATA_LENGTH)
            {
                rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(pstAdv);
                return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
            }
            
            u8ErrCode = rom_llc_ext_adv_pdu_manager_append_data_to_existing_pdus(pstAdv, false, bFragmentPreferred, pu8Data, u8DataLength);
            if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS == u8ErrCode)
            {
                pstAdv->u16UpdatingAdvertisingDataLength += u8DataLength;
            }
            else
            {
                rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(pstAdv);
            }
            
            return u8ErrCode;
        }
        
        case LLC_SPEC_5_1_HCI_ADV_DATA_OPERATION_LAST_FRAGMENT:
        {
            if (0 == pstAdv->u16UpdatingAdvertisingDataLength)
            {
                return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
            }
            
            if (pstAdv->u16UpdatingAdvertisingDataLength + u8DataLength > LLC_SPEC_5_1_EXTENDED_ADV_MAX_ADVERTISING_DATA_LENGTH)
            {
                rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(pstAdv);
                return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
            }
            
            u8ErrCode = rom_llc_ext_adv_pdu_manager_append_data_to_existing_pdus(pstAdv, true, bFragmentPreferred, pu8Data, u8DataLength);
            if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrCode)
            {
                rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(pstAdv);
                return u8ErrCode;
            }
            pstAdv->u16UpdatingAdvertisingDataLength += u8DataLength;
            
            u8ErrCode = rom_llc_ext_adv_state_machine_set_updating_pdus_time(pstAdv);
            if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrCode)
            {
                rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(pstAdv);
                return u8ErrCode;
            }
                
            if (pstAdv->bEnabled)
            {
                pstAdv->bAcceptUpdatingPdus = true;
            }
            else
            {
                rom_llc_ext_adv_pdu_manager_accept_updating_pdu(pstAdv);
            }
            
            return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
        }
        
        case LLC_SPEC_5_1_HCI_ADV_DATA_OPERATION_FIRST_FRAGMENT:
        {
            rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(pstAdv);
            
            if ((uint16_t)u8DataLength > LLC_SPEC_5_1_EXTENDED_ADV_MAX_ADVERTISING_DATA_LENGTH)
            {
                return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
            }
            
            pstAdv->u16CurrentDID = (pstAdv->u16CurrentDID + 1)&0xFFF;
            
            u8ErrCode = rom_llc_ext_adv_pdu_manager_generate_new_pdus(pstAdv, false, pu8Data, u8DataLength);
            if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS == u8ErrCode)
            {
                pstAdv->u16UpdatingAdvertisingDataLength = u8DataLength;
            }
            else
            {
                rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(pstAdv);
            }
            
            return u8ErrCode;
        }
        
        case LLC_SPEC_5_1_HCI_ADV_DATA_OPERATION_COMPLETE:
        {
            rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(pstAdv);
            
            if ((uint16_t)u8DataLength > LLC_SPEC_5_1_EXTENDED_ADV_MAX_ADVERTISING_DATA_LENGTH)
            {
                return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
            }
            
            pstAdv->u16CurrentDID = (pstAdv->u16CurrentDID + 1)&0xFFF;
            u8ErrCode = rom_llc_ext_adv_pdu_manager_generate_new_pdus(pstAdv, true, pu8Data, u8DataLength);
            if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrCode)
            {
                rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(pstAdv);
                return u8ErrCode;
            }
            pstAdv->u16UpdatingAdvertisingDataLength = u8DataLength;
            
            u8ErrCode = rom_llc_ext_adv_state_machine_set_updating_pdus_time(pstAdv);
            if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrCode)
            {
                rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(pstAdv);
                return u8ErrCode;
            }
            
            if (pstAdv->bEnabled)
            {
                pstAdv->bAcceptUpdatingPdus = true;
            }
            else
            {
                rom_llc_ext_adv_pdu_manager_accept_updating_pdu(pstAdv);
            }
            
            return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
        }
        
        case LLC_SPEC_5_1_HCI_ADV_DATA_OPERATION_UNCHANGE:
        {
            if (0 == pstAdv->u16AdvertisingDataLength)
            {
                return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
            }
            
            pstAdv->u16CurrentDID = (pstAdv->u16CurrentDID + 1)&0xFFF;
            if (pstAdv->bEnabled)
            {
                pstAdv->bUpdateAcceptedPdusADI = true;
            }
            else
            {
                rom_llc_ext_adv_pdu_manager_update_accepted_pdus_ADI(pstAdv);
            }
            
            return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
        }
        
        default:
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
    }
}

uint8_t rom_llc_ext_adv_pdu_manager_set_scan_response_data(stLlcExtAdv_t* pstAdv, EN_LLC_SPEC_5_1_HCI_SCAN_RESPONSE_DATA_OPERATION_T enumOperation, bool bFragmentPreferred, uint8_t *pu8Data, uint8_t u8DataLength)
{
    bool bScannable = LLC_EXT_ADV_IS_SCANNABLE(pstAdv->Advertising_Event_Properties);
    ////extended adv
    
    bool bIsScanResponseDataSupported = bScannable;
    if (!bIsScanResponseDataSupported)
    {
        if ((LLC_SPEC_5_1_HCI_SCAN_RESPONSE_DATA_OPERATION_COMPLETE == enumOperation)&&(0 == u8DataLength))
        {
            //do noting
            return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
        }
        else
        {
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
        }
    }
    
    
    uint8_t u8ErrCode;
    switch(enumOperation)
    {
        case LLC_SPEC_5_1_HCI_SCAN_RESPONSE_DATA_OPERATION_INTERMEDIATE_FRAGMENT:
        {
            if (0 == pstAdv->u16UpdatingScanResponseDataLength)
            {
                return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
            }
            
            if (pstAdv->u16UpdatingScanResponseDataLength + u8DataLength > LLC_SPEC_5_1_EXTENDED_ADV_MAX_ADVERTISING_DATA_LENGTH)
            {
                rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(pstAdv);
                return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
            }
            
            u8ErrCode = rom_llc_ext_adv_pdu_manager_append_data_to_existing_pdus(pstAdv, false, bFragmentPreferred, pu8Data, u8DataLength);
            if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS == u8ErrCode)
            {
                pstAdv->u16UpdatingScanResponseDataLength += u8DataLength;
            }
            else
            {
                rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(pstAdv);
            }
            
            return u8ErrCode;
        }
        
        case LLC_SPEC_5_1_HCI_SCAN_RESPONSE_DATA_OPERATION_LAST_FRAGMENT:
        {
            if (0 == pstAdv->u16UpdatingScanResponseDataLength)
            {
                return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
            }
            
            if (pstAdv->u16UpdatingScanResponseDataLength + u8DataLength > LLC_SPEC_5_1_EXTENDED_ADV_MAX_ADVERTISING_DATA_LENGTH)
            {
                rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(pstAdv);
                return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
            }
            
            u8ErrCode = rom_llc_ext_adv_pdu_manager_append_data_to_existing_pdus(pstAdv, true, bFragmentPreferred, pu8Data, u8DataLength);
            if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrCode)
            {
                rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(pstAdv);
                return u8ErrCode;
            }
            pstAdv->u16UpdatingScanResponseDataLength += u8DataLength;
            
            u8ErrCode = rom_llc_ext_adv_state_machine_set_updating_pdus_time(pstAdv);
            if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrCode)
            {
                rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(pstAdv);
                return u8ErrCode;
            }
            
            if (pstAdv->bEnabled)
            {
                pstAdv->bAcceptUpdatingPdus = true;
            }
            else
            {
                rom_llc_ext_adv_pdu_manager_accept_updating_pdu(pstAdv);
            }
            
            return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
        }
        
        case LLC_SPEC_5_1_HCI_SCAN_RESPONSE_DATA_OPERATION_FIRST_FRAGMENT:
        {
            rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(pstAdv);
            
            if ((uint16_t)u8DataLength > LLC_SPEC_5_1_EXTENDED_ADV_MAX_ADVERTISING_DATA_LENGTH)
            {
                return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
            }
            
            pstAdv->u16CurrentDID = (pstAdv->u16CurrentDID + 1)&0xFFF;
            u8ErrCode = rom_llc_ext_adv_pdu_manager_generate_new_pdus(pstAdv, false, pu8Data, u8DataLength);
            if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS == u8ErrCode)
            {
                pstAdv->u16UpdatingScanResponseDataLength = u8DataLength;
            }
            else
            {
                rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(pstAdv);
            }
            
            return u8ErrCode;
        }
        
        case LLC_SPEC_5_1_HCI_SCAN_RESPONSE_DATA_OPERATION_COMPLETE:
        {
            rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(pstAdv);
            
            if ((uint16_t)u8DataLength > LLC_SPEC_5_1_EXTENDED_ADV_MAX_ADVERTISING_DATA_LENGTH)
            {
                return LLC_SPEC_5_1_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED;
            }
            
            pstAdv->u16CurrentDID = (pstAdv->u16CurrentDID + 1)&0xFFF;
            u8ErrCode = rom_llc_ext_adv_pdu_manager_generate_new_pdus(pstAdv, true, pu8Data, u8DataLength);
            if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrCode)
            {
                rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(pstAdv);
                return u8ErrCode;
            }
            pstAdv->u16UpdatingScanResponseDataLength = u8DataLength;
            
            u8ErrCode = rom_llc_ext_adv_state_machine_set_updating_pdus_time(pstAdv);
            if (LLC_SPEC_5_1_ERROR_CODE_SUCCESS != u8ErrCode)
            {
                rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(pstAdv);
                return u8ErrCode;
            }
            
            if (pstAdv->bEnabled)
            {
                pstAdv->bAcceptUpdatingPdus = true;
            }
            else
            {
                rom_llc_ext_adv_pdu_manager_accept_updating_pdu(pstAdv);
            }
            
            return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
        }
        
        
        default:
            return LLC_SPEC_5_1_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS;
    }
}


void rom_llc_ext_adv_pdu_manager_clear(stLlcExtAdv_t* pstAdv)
{
    rom_llc_ext_adv_pdu_manager_destroy_updating_pdus(pstAdv);
    rom_llc_ext_adv_pdu_manager_destroy_accepted_pdus(pstAdv);
}
