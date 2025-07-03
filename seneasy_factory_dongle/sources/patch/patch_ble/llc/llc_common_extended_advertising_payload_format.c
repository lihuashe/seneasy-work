#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "llc_llp_interface.h"

#include "llc_spec_5_1.h"


#include "llc_tool.h"


#include "llc_adv_channel_pdu.h"

#include "llc_common_extended_advertising_payload_format.h"

#include "mlog.h"


bool rom_llc_common_extended_advertising_payload_parse(stLlcAdvChannelPdu_t* pstPdu, stLlcCommonExtendedAdvertisingPayloadFormat_t* pstFormat)
{
    if ((NULL == pstPdu)||(NULL == pstFormat))
    {
        return false;
    }
    
    stLlcSpec51AdvChannelPduHeader_t stHeader;
    stHeader.u16Value = pstPdu->u32Header & 0xFFFF;
    
    uint8_t* pu8Payload = pstPdu->pu8Payload;
    uint16_t u16PayloadLength = stHeader.bitField.Length;
    
    if ((NULL == pstPdu->pu8Payload)||(0 == u16PayloadLength)||(u16PayloadLength > pstPdu->u16PayloadBufSize))
    {
        return false;
    }
    
    //BLUETOOTH CORE SPECIFICATION Version 5.1 | Vol 6, Part B page 2709
    
    uint8_t Extended_Header_Length = pu8Payload[0] & 0x3F;
    if ((Extended_Header_Length > 63)||(Extended_Header_Length + 1 > u16PayloadLength ))
    {
        return false;
    }
    
    pstFormat->Header.u16Value = stHeader.u16Value;
    
    uint8_t AdvMode = (pu8Payload[0] >> 6) & 0x3;
    pstFormat->Connectable = AdvMode & 1;
    pstFormat->Scannable = AdvMode & 2;
    
    uint8_t* pu8PayloadContentPtr = &pu8Payload[1];
    
    if (Extended_Header_Length > 0)
    {
        ////check
        
        uint8_t u8ExtendedHeaderFlagsPresentLength = 0;
        uint8_t Extended_Header_Flags = *pu8PayloadContentPtr;
        pu8PayloadContentPtr += 1;
        
        if (Extended_Header_Flags & (1<<0))
        {
            //AdvA present
            u8ExtendedHeaderFlagsPresentLength += 6;
        }
        
        if (Extended_Header_Flags & (1<<1))
        {
            //TargetA preset
            u8ExtendedHeaderFlagsPresentLength += 6;
        }
        
        if (Extended_Header_Flags & (1<<2))
        {
            //CTEInfo preset
            u8ExtendedHeaderFlagsPresentLength += 1;
        }
        
        if (Extended_Header_Flags & (1<<3))
        {
            //AdvDataInfo preset
            u8ExtendedHeaderFlagsPresentLength += 2;
        }
        
        if (Extended_Header_Flags & (1<<4))
        {
            //AuxPtr preset
            u8ExtendedHeaderFlagsPresentLength += 3;
        }
        
        if (Extended_Header_Flags & (1<<5))
        {
            //SyncInfo present
            u8ExtendedHeaderFlagsPresentLength += 18;
        }
        
        if (Extended_Header_Flags & (1<<6))
        {
            //TxPower present
            u8ExtendedHeaderFlagsPresentLength += 1;
        }
        
        if (u8ExtendedHeaderFlagsPresentLength + 1 > Extended_Header_Length)
        {
            return false;
        }
        
        ////set
        
        if (Extended_Header_Flags & (1<<0))
        {
            //AdvA present
            pstFormat->AdvA_Present = true;
            pstFormat->AdvA = pu8PayloadContentPtr;
            
            pu8PayloadContentPtr += 6;
        }
        else
        {
            pstFormat->AdvA_Present = false;
            pstFormat->AdvA = NULL;
        }
        
        if (Extended_Header_Flags & (1<<1))
        {
            //TargetA preset
            pstFormat->TargetA_Present = true;
            pstFormat->TargetA = pu8PayloadContentPtr;
            
            pu8PayloadContentPtr += 6;
        }
        else
        {
            pstFormat->TargetA_Present = false;
            pstFormat->TargetA = NULL;
        }
        
        if (Extended_Header_Flags & (1<<2))
        {
            //CTEInfo preset
            ////todo : CTEInfo
            pstFormat->CTEInfo_Present = true;
            
            pu8PayloadContentPtr += 1;
        }
        else
        {
            pstFormat->CTEInfo_Present = false;
        }
        
        if (Extended_Header_Flags & (1<<3))
        {
            //AdvDataInfo preset
            pstFormat->ADI_Present = true;
            pstFormat->ADI_DID = ((uint16_t)pu8PayloadContentPtr[0])|(((uint16_t)(pu8PayloadContentPtr[1]&0xF))<<8);
            pstFormat->ADI_SID = (pu8PayloadContentPtr[1]>>4)&0xF;
            
            pu8PayloadContentPtr += 2;
        }
        else
        {
            pstFormat->ADI_Present = false;
        }
        
        if (Extended_Header_Flags & (1<<4))
        {
            //AuxPtr preset
            pstFormat->AuxPtr_Present = true;
            pstFormat->AuxPtr_ChannelIndex = pu8PayloadContentPtr[0]&0x3F;
            
            if (pu8PayloadContentPtr[0] & (1<<6))
            {
                pstFormat->AuxPtr_ClockAccuracyPpm = 50;
            }
            else
            {
                pstFormat->AuxPtr_ClockAccuracyPpm = 500;
            }
            
            uint16_t u16AuxOffset = ((uint16_t)pu8PayloadContentPtr[1])|(((uint16_t)(pu8PayloadContentPtr[2]&0x1F))<<8);
            
            if (pu8PayloadContentPtr[0] & (1<<7))//Units 300us
            {
                pstFormat->AuxPtr_OffsetUnits30Us = 10;
            }
            else
            {
                pstFormat->AuxPtr_OffsetUnits30Us = 1;
            }
            pstFormat->AuxPtr_Offset30Us = u16AuxOffset * ((uint32_t)pstFormat->AuxPtr_OffsetUnits30Us);
            
            
            switch((pu8PayloadContentPtr[2]>>5)&0x7)
            {
                case 0:
                    pstFormat->AuxPtr_Phy = LL_PHY_1M;
                    break;
                
                case 1:
                    pstFormat->AuxPtr_Phy = LL_PHY_2M;
                    break;
                
                case 2:
                    pstFormat->AuxPtr_Phy = LL_PHY_CODED_S8;
                    break;
                
                default:
                    return false;
            }
            
            pu8PayloadContentPtr += 3;
        }
        else
        {
            pstFormat->AuxPtr_Present = false;
        }
        
        if (Extended_Header_Flags & (1<<5))
        {
            pstFormat->SyncInfo_Present = true;
            
            uint16_t u16SyncPacketOffset = ((uint16_t)pu8PayloadContentPtr[0])|(((uint16_t)(pu8PayloadContentPtr[1]&0x1F))<<8);
            if (pu8PayloadContentPtr[1] & (1<<5))
            {
                pstFormat->SyncInfo_OffsetUnits30Us = 10;
            }
            else
            {
                pstFormat->SyncInfo_OffsetUnits30Us = 1;
            }
            if (pu8PayloadContentPtr[1] & (1<<6))
            {
                pstFormat->SyncInfo_OffsetAdjust = true;
            }
            else
            {
                pstFormat->SyncInfo_OffsetAdjust = false;
            }
            pstFormat->SyncInfo_SyncPacketOffset30Us = u16SyncPacketOffset * ((uint32_t)pstFormat->SyncInfo_OffsetUnits30Us);
            if (pstFormat->SyncInfo_OffsetAdjust)
            {
                pstFormat->SyncInfo_SyncPacketOffset30Us += 2457600/30;
            }
            pstFormat->SyncInfo_Interval1250Us = ((uint16_t)pu8PayloadContentPtr[2])|(((uint16_t)pu8PayloadContentPtr[3])<<8);
            memcpy(&(pstFormat->SyncInfo_ChM[0]), &pu8PayloadContentPtr[4], 4);
            pstFormat->SyncInfo_ChM[4] = pu8PayloadContentPtr[8] & 0x1F;
            pstFormat->SyncInfo_ClockAccuracyPpm = rom_llc_spec_5_1_get_ppm_by_sca((pu8PayloadContentPtr[8]>>5)&0x7);
            pstFormat->SyncInfo_AA = rom_llc_tool_read_little_endian_uint32(&pu8PayloadContentPtr[9]);
            pstFormat->SyncInfo_CRCInit = ((uint32_t)pu8PayloadContentPtr[13])|(((uint32_t)pu8PayloadContentPtr[14])<<8)|(((uint32_t)pu8PayloadContentPtr[15])<<16);
            pstFormat->SyncInfo_EventCounter = rom_llc_tool_read_little_endian_uint16(&pu8PayloadContentPtr[16]);
            
            pu8PayloadContentPtr += 18;
        }
        else
        {
            pstFormat->SyncInfo_Present = false;
        }
        
        if (Extended_Header_Flags & (1<<6))
        {
            //TxPower present
            pstFormat->TxPower_Present = true;
            pstFormat->TxPower = (int8_t)pu8PayloadContentPtr[0];
            
            pu8PayloadContentPtr += 1;
        }
        else
        {
            pstFormat->TxPower_Present = false;
        }
        
        pstFormat->ACAD_Length = Extended_Header_Length - 1 - u8ExtendedHeaderFlagsPresentLength;
        if (pstFormat->ACAD_Length > 0)
        {
            pstFormat->ACAD_Present = true;
            pstFormat->ACAD = pu8PayloadContentPtr;
            
            pu8PayloadContentPtr += pstFormat->ACAD_Length;
        }
        else
        {
            pstFormat->ACAD_Present = false;
            pstFormat->ACAD = NULL;
        }
    }
    
    
    pstFormat->AdvData_Length = u16PayloadLength - 1 - Extended_Header_Length;
    if (pstFormat->AdvData_Length > 0)
    {
        pstFormat->AdvData_Present = true;
        pstFormat->AdvData = pu8PayloadContentPtr;
        //pu8PayloadContentPtr += pstFormat->AdvData_Length;
    }
    else
    {
        pstFormat->AdvData_Present = false;
        pstFormat->AdvData = NULL;
    }
    
    return true;
}


bool rom_llc_common_extended_advertising_payload_generate(stLlcAdvChannelPdu_t* pstPdu, stLlcCommonExtendedAdvertisingPayloadFormat_t* pstFormat)
{
    if ((NULL == pstPdu)||(NULL == pstFormat))
    {
        return false;
    }
    
    if ((NULL == pstPdu->pu8Payload)||(0 == pstPdu->u16PayloadBufSize))
    {
        return false;
    }
    
    
    bool AdvA_Present = pstFormat->AdvA_Present;
    bool TargetA_Present = pstFormat->TargetA_Present;
    bool CTEInfo_Present = pstFormat->CTEInfo_Present;
    bool ADI_Present = pstFormat->ADI_Present;
    bool AuxPtr_Present = pstFormat->AuxPtr_Present;
    bool SyncInfo_Present = pstFormat->SyncInfo_Present;
    bool TxPower_Present = pstFormat->TxPower_Present;
    bool ACAD_Present = pstFormat->ACAD_Present;
    bool AdvData_Present = pstFormat->AdvData_Present;
    
    if (AdvA_Present && (NULL == pstFormat->AdvA))
    {
        //param error
        return false;
    }
    if (TargetA_Present && (NULL == pstFormat->TargetA))
    {
        //param error
        return false;
    }
    if (CTEInfo_Present)
    {
        ////todo : unsupported
        return false;
    }
    if ((ACAD_Present) && ((NULL == pstFormat->ACAD)||(0 == pstFormat->ACAD_Length)))
    {
        //param error
        return false;
    }
    if ((AdvData_Present) && ((NULL == pstFormat->AdvData)||(0 == pstFormat->AdvData_Length)))
    {
        AdvData_Present = false;
    }
    
    uint8_t ACAD_Length = (ACAD_Present)?(pstFormat->ACAD_Length):(0);
    
    uint8_t AdvData_Length = (AdvData_Present)?(pstFormat->AdvData_Length):(0);
    
    uint16_t u16ExpectedPayloadLength = rom_llc_common_extended_advertising_payload_calculate_payload_length(AdvA_Present, TargetA_Present, CTEInfo_Present, ADI_Present,
        AuxPtr_Present, SyncInfo_Present, TxPower_Present,  ACAD_Length,   AdvData_Length);
    
    uint8_t *pu8PayloadBuf = pstPdu->pu8Payload;
    uint16_t u16PayloadBufSize = pstPdu->u16PayloadBufSize;
    if ((u16PayloadBufSize < u16ExpectedPayloadLength)||(u16ExpectedPayloadLength > LLC_SPEC_5_1_ADV_CHANNEL_PDU_MAX_PAYLOAD_SIZE))
    {
        return false;
    }
    
    pstFormat->Header.bitField.Length = u16ExpectedPayloadLength & 0xFF;
    pstPdu->u32Header = pstFormat->Header.u16Value;
    
    uint32_t u32Index = 0;
    
    ////Extended Header Length and AdvMode
    uint8_t Extended_Header_Length = u16ExpectedPayloadLength - pstFormat->AdvData_Length - 1;
    if (Extended_Header_Length > 0x3F)
    {
        ////todo : unexptected situation
        return false;
    }
    uint8_t AdvMode = 0;
    if (pstFormat->Connectable)
    {
        AdvMode |= 0x01;
    }
    if (pstFormat->Scannable)
    {
        AdvMode |= 0x02;
    }
    pu8PayloadBuf[u32Index] = Extended_Header_Length | (AdvMode << 6);
    u32Index += 1;
    
    
    ////Extended Header Flags
    if (AdvA_Present||TargetA_Present||CTEInfo_Present||ADI_Present||AuxPtr_Present||SyncInfo_Present||TxPower_Present||ACAD_Present)
    {
        uint8_t Extended_Header_Flags = 0;
        if (AdvA_Present)
        {
            Extended_Header_Flags |= 0x01;
        }
        if (TargetA_Present)
        {
            Extended_Header_Flags |= 0x02;
        }
        if (CTEInfo_Present)
        {
            Extended_Header_Flags |= 0x04;
        }
        if (ADI_Present)
        {
            Extended_Header_Flags |= 0x08;
        }
        if (AuxPtr_Present)
        {
            Extended_Header_Flags |= 0x10;
        }
        if (SyncInfo_Present)
        {
            Extended_Header_Flags |= 0x20;
        }
        if (TxPower_Present)
        {
            Extended_Header_Flags |= 0x40;
        }
        
        pu8PayloadBuf[u32Index] = Extended_Header_Flags;
        u32Index += 1;
    }
    
    ////AdvA
    if (AdvA_Present)
    {
        memcpy(&pu8PayloadBuf[u32Index], pstFormat->AdvA, 6);
        
        u32Index += 6;
    }
    
    ////TargetA
    if (TargetA_Present)
    {
        memcpy(&pu8PayloadBuf[u32Index], pstFormat->TargetA, 6);
        u32Index += 6;
    }
    
    ////todo : CTEInfo not support
    
    
    ////AdvDataInfo (ADI)
    if (ADI_Present)
    {
        pu8PayloadBuf[u32Index + 0] = pstFormat->ADI_DID & 0xFF;
        pu8PayloadBuf[u32Index + 1] = ((pstFormat->ADI_DID>>8)&0xF)|((pstFormat->ADI_SID & 0xF)<<4);
        
        u32Index += 2;
    }
    
    ////AuxPtr
    if (AuxPtr_Present)
    {
        uint8_t Channel_Index = pstFormat->AuxPtr_ChannelIndex;
        uint8_t CA = 0;
        if (pstFormat->AuxPtr_ClockAccuracyPpm <= 50)
        {
            CA = 1;
        }
        uint8_t Offset_Units = 0;
        if (pstFormat->AuxPtr_Offset30Us * 30 >= 245700)
        {
            Offset_Units = 1;
        }
        uint16_t AUX_Offset = pstFormat->AuxPtr_Offset30Us;
        if (0 != Offset_Units)
        {
            AUX_Offset /= 10;
        }
        uint8_t AUX_PHY = 0;
        switch(pstFormat->AuxPtr_Phy)
        {
            case LL_PHY_1M:
                AUX_PHY = 0;
                break;
            
            case LL_PHY_2M:
                AUX_PHY = 1;
                break;
            
            case LL_PHY_CODED_S2:
            case LL_PHY_CODED_S8:
                AUX_PHY = 2;
                break;
            
            default:
                ////todo: unexpected situation
                return false;
        }
        
        pu8PayloadBuf[u32Index + 0] = (Channel_Index&0x3F)|(CA<<6)|(Offset_Units<<7);
        pu8PayloadBuf[u32Index + 1] = AUX_Offset&0xFF;
        pu8PayloadBuf[u32Index + 2] = ((AUX_Offset>>8)&0x1F)|(AUX_PHY << 5);
        
        u32Index += 3;
    }
    
    
    if (SyncInfo_Present)
    {
        uint32_t SyncPacketOffset = pstFormat->SyncInfo_SyncPacketOffset30Us;
        uint8_t OffsetAdjust = (SyncPacketOffset >= 2457600/30)?(1):(0);
        if (OffsetAdjust)
        {
            SyncPacketOffset -= 2457600/30;
        }
        uint8_t Offset_Units = 0;
        if (SyncPacketOffset >= 245700/30)
        {
            Offset_Units = 1;
            SyncPacketOffset /= 10;
        }
        pu8PayloadBuf[u32Index + 0] = SyncInfo_Present & 0xFF;
        pu8PayloadBuf[u32Index + 1] = ((SyncInfo_Present>>8) & 0x1F)|(Offset_Units << 5)|(OffsetAdjust << 6);
        rom_llc_tool_write_little_endian_uint16(&pu8PayloadBuf[u32Index + 2], pstFormat->SyncInfo_Interval1250Us);
        memcpy(&pu8PayloadBuf[u32Index + 4], &(pstFormat->SyncInfo_ChM[0]), 4);
        pu8PayloadBuf[u32Index + 8] = (pstFormat->SyncInfo_ChM[4] & 0x1F)|(rom_llc_spec_5_1_get_sca_by_ppm(pstFormat->SyncInfo_ClockAccuracyPpm)<<5);
        rom_llc_tool_write_little_endian_uint32(&pu8PayloadBuf[u32Index + 9], pstFormat->SyncInfo_AA);
        pu8PayloadBuf[u32Index + 13] = (pstFormat->SyncInfo_CRCInit>>0) & 0xFF;
        pu8PayloadBuf[u32Index + 14] = (pstFormat->SyncInfo_CRCInit>>8) & 0xFF;
        pu8PayloadBuf[u32Index + 15] = (pstFormat->SyncInfo_CRCInit>>16) & 0xFF;
        rom_llc_tool_write_little_endian_uint16(&(pu8PayloadBuf[u32Index + 16]), pstFormat->SyncInfo_EventCounter);
        
        u32Index += 18;
    }
    
    ////TxPower
    if (TxPower_Present)
    {
        pu8PayloadBuf[u32Index] = (uint8_t)pstFormat->TxPower;
        
        u32Index += 1;
    }
    
    ////ACAD
    if (ACAD_Present)
    {
        memcpy(&pu8PayloadBuf[u32Index], pstFormat->ACAD, ACAD_Length);
        
        u32Index += ACAD_Length;
    }
    
    ////AdvData
    if (AdvData_Present)
    {
        memcpy(&pu8PayloadBuf[u32Index], pstFormat->AdvData, AdvData_Length);
        
        u32Index += AdvData_Length;
    }
    
    //check
    if (u32Index != u16ExpectedPayloadLength)
    {
        ////todo : unexpected situation
        return false;
    }
    
    return true;
}


uint16_t rom_llc_common_extended_advertising_payload_calculate_payload_length(bool AdvA_Present,
    bool TargetA_Present,
    bool CTEInfo_Present,
    bool ADI_Present,
    bool AuxPtr_Present,
    bool SyncInfo_Present,
    bool TxPower_Present,
    uint8_t ACAD_Length,
    uint16_t u16AdvDataLength)
{
    uint16_t u16PduPayloadLength = 1; //Extended Header Length + AdvMode
    
    if (AdvA_Present||TargetA_Present||CTEInfo_Present||ADI_Present||AuxPtr_Present||SyncInfo_Present||TxPower_Present||(ACAD_Length > 0))
    {
        u16PduPayloadLength += 1;//Extended Header Flags
    }
    
    if (AdvA_Present)
    {
        u16PduPayloadLength += 6;//AdvA
    }
    
    if (TargetA_Present)
    {
        u16PduPayloadLength += 6;//TargetA
    }
    
    if (CTEInfo_Present)
    {
        u16PduPayloadLength += 1;//CTEInfo
    }
    
    if (ADI_Present)
    {
        u16PduPayloadLength += 2;//AdvDataInfo (ADI)
    }
    
    if (AuxPtr_Present)
    {
        u16PduPayloadLength += 3;//AuxPtr
    }
    
    if (SyncInfo_Present)
    {
        u16PduPayloadLength += 18;//SyncInfo
    }
    
    if(TxPower_Present)
    {
        u16PduPayloadLength += 1;//TxPower
    }
    
    u16PduPayloadLength += ACAD_Length;//ACAD
    
    u16PduPayloadLength += u16AdvDataLength;//AdvData
    
    return u16PduPayloadLength;
}

uint16_t rom_llc_common_extended_advertising_payload_get_avaliable_AdvData_length(uint16_t u16PayloadBufferLength,
    bool AdvA_Present,
    bool TargetA_Present,
    bool CTEInfo_Present,
    bool ADI_Present,
    bool AuxPtr_Present,
    bool SyncInfo_Present,
    bool TxPower_Present,
    uint8_t ACAD_Length)
{
    if (u16PayloadBufferLength > LLC_SPEC_5_1_ADV_CHANNEL_PDU_MAX_PAYLOAD_SIZE)
    {
        u16PayloadBufferLength = LLC_SPEC_5_1_ADV_CHANNEL_PDU_MAX_PAYLOAD_SIZE;
    }
    
    uint16_t u16PduPayloadLength = 1; //Extended Header Length + AdvMode
    
    if (AdvA_Present||TargetA_Present||CTEInfo_Present||ADI_Present||AuxPtr_Present||SyncInfo_Present||TxPower_Present||(ACAD_Length > 0))
    {
        u16PduPayloadLength += 1;//Extended Header Flags
    }
    
    if (AdvA_Present)
    {
        u16PduPayloadLength += 6;//AdvA
    }
    
    if (TargetA_Present)
    {
        u16PduPayloadLength += 6;//TargetA
    }
    
    if (CTEInfo_Present)
    {
        u16PduPayloadLength += 1;//CTEInfo
    }
    
    if (ADI_Present)
    {
        u16PduPayloadLength += 2;//AdvDataInfo (ADI)
    }
    
    if (AuxPtr_Present)
    {
        u16PduPayloadLength += 3;//AuxPtr
    }
    
    if (SyncInfo_Present)
    {
        u16PduPayloadLength += 18;//SyncInfo
    }
    
    if(TxPower_Present)
    {
        u16PduPayloadLength += 1;//TxPower
    }
    
    u16PduPayloadLength += ACAD_Length;//ACAD
    
    if (u16PayloadBufferLength < u16PduPayloadLength)
    {
        return 0;
    }
    else
    {
        return u16PayloadBufferLength - u16PduPayloadLength;
    }
}

#if 0
bool rom_llc_common_extended_advertising_payload_update_AdvA(stLlcAdvChannelPdu_t* pstPdu, uint8_t AdvA[6], bool TxAdd)
{
    if ((NULL == pstPdu)||(NULL == AdvA))
    {
        return false;
    }
    
    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat = {0};
    if (!rom_llc_common_extended_advertising_payload_parse(pstPdu, &stFormat))
    {
        return false;
    }
    
    if (!stFormat.AdvA_Present)
    {
        return false;
    }
    
    stFormat.AdvA = AdvA;
    stFormat.Header.bitField.TxAdd = TxAdd;
    
    return rom_llc_common_extended_advertising_payload_generate(pstPdu, &stFormat);
}
#else
//better performance
bool rom_llc_common_extended_advertising_payload_update_AdvA(stLlcAdvChannelPdu_t* pstPdu, uint8_t AdvA[6], bool TxAdd)
{
    if ((NULL == pstPdu)||(NULL == AdvA))
    {
        return false;
    }
    
    stLlcSpec51AdvChannelPduHeader_t stHeader;
    stHeader.u16Value = pstPdu->u32Header & 0xFFFF;
    
    uint8_t* pu8Payload = pstPdu->pu8Payload;
    uint16_t u16PayloadLength = stHeader.bitField.Length;
    
    if ((NULL == pstPdu->pu8Payload)||(0 == u16PayloadLength)||(u16PayloadLength > pstPdu->u16PayloadBufSize))
    {
        return false;
    }
    
    uint8_t Extended_Header_Length = pu8Payload[0] & 0x3F;
    if ((Extended_Header_Length > 63)||(Extended_Header_Length + 1 > u16PayloadLength ))
    {
        return false;
    }
    
    uint8_t AdvMode = (pu8Payload[0] >> 6) & 0x3;
    
    uint8_t* pu8PayloadContentPtr = &pu8Payload[1];
    
    if (Extended_Header_Length > 0)
    {
        uint8_t Extended_Header_Flags = *pu8PayloadContentPtr;
        pu8PayloadContentPtr += 1;
        
        if (Extended_Header_Flags & (1<<0))
        {
            //AdvA present
            memcpy(pu8PayloadContentPtr, AdvA, 6);
            stHeader.bitField.TxAdd = TxAdd;
            pstPdu->u32Header = stHeader.u16Value;
            return true;
        }
        
    }
    
    return false;
}
#endif

#if 0
bool rom_llc_common_extended_advertising_payload_update_TargetA(stLlcAdvChannelPdu_t* pstPdu, uint8_t TargetA[6], bool RxAdd)
{
    if ((NULL == pstPdu)||(NULL == TargetA))
    {
        return false;
    }
    
    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat = {0};
    if (!rom_llc_common_extended_advertising_payload_parse(pstPdu, &stFormat))
    {
        return false;
    }
    
    if (!stFormat.TargetA_Present)
    {
        return false;
    }
    
    stFormat.TargetA = TargetA;
    stFormat.Header.bitField.RxAdd = RxAdd;
    
    return rom_llc_common_extended_advertising_payload_generate(pstPdu, &stFormat);
}
#else
//better performance
bool rom_llc_common_extended_advertising_payload_update_TargetA(stLlcAdvChannelPdu_t* pstPdu, uint8_t TargetA[6], bool RxAdd)
{
    if ((NULL == pstPdu)||(NULL == TargetA))
    {
        return false;
    }
    
    stLlcSpec51AdvChannelPduHeader_t stHeader;
    stHeader.u16Value = pstPdu->u32Header & 0xFFFF;
    
    uint8_t* pu8Payload = pstPdu->pu8Payload;
    uint16_t u16PayloadLength = stHeader.bitField.Length;
    
    if ((NULL == pstPdu->pu8Payload)||(0 == u16PayloadLength)||(u16PayloadLength > pstPdu->u16PayloadBufSize))
    {
        return false;
    }
    
    uint8_t Extended_Header_Length = pu8Payload[0] & 0x3F;
    if ((Extended_Header_Length > 63)||(Extended_Header_Length + 1 > u16PayloadLength ))
    {
        return false;
    }
    
    uint8_t AdvMode = (pu8Payload[0] >> 6) & 0x3;
    
    uint8_t* pu8PayloadContentPtr = &pu8Payload[1];
    
    if (Extended_Header_Length > 0)
    {
        uint8_t Extended_Header_Flags = *pu8PayloadContentPtr;
        pu8PayloadContentPtr += 1;
        
        if (Extended_Header_Flags & (1<<0))
        {
            //AdvA present
            
            pu8PayloadContentPtr += 6;
        }
        
        if (Extended_Header_Flags & (1<<1))
        {
            //TargetA preset
            memcpy(pu8PayloadContentPtr, TargetA, 6);
            stHeader.bitField.RxAdd = RxAdd;
            pstPdu->u32Header = stHeader.u16Value;
            
            return true;
        }
        
    }
    
    return false;
}
#endif

#if 0
bool rom_llc_common_extended_advertising_payload_update_ADI(stLlcAdvChannelPdu_t* pstPdu, uint16_t DID, uint8_t SID)
{
    if (NULL == pstPdu)
    {
        return false;
    }
    
    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat = {0};
    if (!rom_llc_common_extended_advertising_payload_parse(pstPdu, &stFormat))
    {
        return false;
    }
    
    if (!stFormat.ADI_Present)
    {
        return false;
    }
    
    stFormat.ADI_DID = DID;
    stFormat.ADI_SID = SID;
    
    return rom_llc_common_extended_advertising_payload_generate(pstPdu, &stFormat);
}
#else
//better performance
bool rom_llc_common_extended_advertising_payload_update_ADI(stLlcAdvChannelPdu_t* pstPdu, uint16_t DID, uint8_t SID)
{
    if (NULL == pstPdu)
    {
        return false;
    }
    
    stLlcSpec51AdvChannelPduHeader_t stHeader;
    stHeader.u16Value = pstPdu->u32Header & 0xFFFF;
    
    uint8_t* pu8Payload = pstPdu->pu8Payload;
    uint16_t u16PayloadLength = stHeader.bitField.Length;
    
    if ((NULL == pstPdu->pu8Payload)||(0 == u16PayloadLength)||(u16PayloadLength > pstPdu->u16PayloadBufSize))
    {
        return false;
    }
    
    uint8_t Extended_Header_Length = pu8Payload[0] & 0x3F;
    if ((Extended_Header_Length > 63)||(Extended_Header_Length + 1 > u16PayloadLength ))
    {
        return false;
    }
    
    uint8_t AdvMode = (pu8Payload[0] >> 6) & 0x3;
    
    uint8_t* pu8PayloadContentPtr = &pu8Payload[1];
    
    if (Extended_Header_Length > 0)
    {
        uint8_t Extended_Header_Flags = *pu8PayloadContentPtr;
        pu8PayloadContentPtr += 1;
        
        if (Extended_Header_Flags & (1<<0))
        {
            //AdvA present
            
            pu8PayloadContentPtr += 6;
        }
        
        if (Extended_Header_Flags & (1<<1))
        {
            //TargetA preset
            
            pu8PayloadContentPtr += 6;
        }
        
        if (Extended_Header_Flags & (1<<2))
        {
            //CTEInfo preset
            ////todo : CTEInfo
            
            pu8PayloadContentPtr += 1;
        }
        
        if (Extended_Header_Flags & (1<<3))
        {
            //AdvDataInfo preset
            pu8PayloadContentPtr[0] = DID & 0xFF;
            pu8PayloadContentPtr[1] = ((DID>>8)&0xF)|((SID & 0xF)<<4);
            
            return true;
        }
    }
    
    return false;
}
#endif


#if 0
bool rom_llc_common_extended_advertising_payload_update_AuxPtr(stLlcAdvChannelPdu_t* pstPdu, 
    uint8_t AuxPtr_ChannelIndex,
    uint16_t AuxPtr_ClockAccuracyPpm,
    uint32_t AuxPtr_Offset30Us,
    EN_LL_PHY_T AuxPtr_Phy)
{
    if (NULL == pstPdu)
    {
        return false;
    }
    
    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat = {0};
    if (!rom_llc_common_extended_advertising_payload_parse(pstPdu, &stFormat))
    {
        return false;
    }
    
    if (!stFormat.AuxPtr_Present)
    {
        return false;
    }
    
    stFormat.AuxPtr_ChannelIndex = AuxPtr_ChannelIndex;
    stFormat.AuxPtr_ClockAccuracyPpm = AuxPtr_ClockAccuracyPpm;
    stFormat.AuxPtr_Offset30Us = AuxPtr_Offset30Us;
    stFormat.AuxPtr_Phy = AuxPtr_Phy;
    
    return rom_llc_common_extended_advertising_payload_generate(pstPdu, &stFormat);
}
#else
//better performance
bool rom_llc_common_extended_advertising_payload_update_AuxPtr(stLlcAdvChannelPdu_t* pstPdu, 
    uint8_t AuxPtr_ChannelIndex,
    uint16_t AuxPtr_ClockAccuracyPpm,
    uint32_t AuxPtr_Offset30Us,
    EN_LL_PHY_T AuxPtr_Phy)
{
    if (NULL == pstPdu)
    {
        return false;
    }
    
    stLlcSpec51AdvChannelPduHeader_t stHeader;
    stHeader.u16Value = pstPdu->u32Header & 0xFFFF;
    
    uint8_t* pu8Payload = pstPdu->pu8Payload;
    uint16_t u16PayloadLength = stHeader.bitField.Length;
    
    if ((NULL == pstPdu->pu8Payload)||(0 == u16PayloadLength)||(u16PayloadLength > pstPdu->u16PayloadBufSize))
    {
        return false;
    }
    
    uint8_t Extended_Header_Length = pu8Payload[0] & 0x3F;
    if ((Extended_Header_Length > 63)||(Extended_Header_Length + 1 > u16PayloadLength ))
    {
        return false;
    }
    
    uint8_t AdvMode = (pu8Payload[0] >> 6) & 0x3;
    
    uint8_t* pu8PayloadContentPtr = &pu8Payload[1];
    
    if (Extended_Header_Length > 0)
    {
        uint8_t Extended_Header_Flags = *pu8PayloadContentPtr;
        pu8PayloadContentPtr += 1;
        
        if (Extended_Header_Flags & (1<<0))
        {
            //AdvA present
            
            pu8PayloadContentPtr += 6;
        }
        
        if (Extended_Header_Flags & (1<<1))
        {
            //TargetA preset
            
            pu8PayloadContentPtr += 6;
        }
        
        if (Extended_Header_Flags & (1<<2))
        {
            //CTEInfo preset
            ////todo : CTEInfo
            
            pu8PayloadContentPtr += 1;
        }
        
        if (Extended_Header_Flags & (1<<3))
        {
            //AdvDataInfo preset
            
            pu8PayloadContentPtr += 2;
        }
        
        if (Extended_Header_Flags & (1<<4))
        {
            //AuxPtr preset
            
            uint8_t Channel_Index = AuxPtr_ChannelIndex;
            uint8_t CA = 0;
            if (AuxPtr_ClockAccuracyPpm <= 50)
            {
                CA = 1;
            }
            uint8_t Offset_Units = 0;
            if (AuxPtr_Offset30Us * 30 >= 245700)
            {
                Offset_Units = 1;
            }
            uint16_t AUX_Offset = AuxPtr_Offset30Us;
            if (0 != Offset_Units)
            {
                AUX_Offset /= 10;
            }
            uint8_t AUX_PHY = 0;
            switch(AuxPtr_Phy)
            {
                case LL_PHY_1M:
                    AUX_PHY = 0;
                    break;
                
                case LL_PHY_2M:
                    AUX_PHY = 1;
                    break;
                
                case LL_PHY_CODED_S2:
                case LL_PHY_CODED_S8:
                    AUX_PHY = 2;
                    break;
                
                default:
                    ////todo: unexpected situation
                    return false;
            }
            
            pu8PayloadContentPtr[0] = (Channel_Index&0x3F)|(CA<<6)|(Offset_Units<<7);
            pu8PayloadContentPtr[1] = AUX_Offset&0xFF;
            pu8PayloadContentPtr[2] = ((AUX_Offset>>8)&0x1F)|(AUX_PHY << 5);
            
            return true;
        }
    }
    
    return false;
}
#endif

bool rom_llc_common_extended_advertising_payload_append_AdvData(stLlcAdvChannelPdu_t* pstPdu, uint8_t* pu8Data, uint8_t u8DataLength)
{
    if ((NULL == pstPdu)||(NULL == pstPdu->pu8Payload)||(0 == pstPdu->u16PayloadBufSize))
    {
        return false;
    }
    
    if ((NULL == pu8Data)||(0 == u8DataLength))
    {
        return true;
    }
    
    stLlcCommonExtendedAdvertisingPayloadFormat_t stFormat = {0};
    if (!rom_llc_common_extended_advertising_payload_parse(pstPdu, &stFormat))
    {
        return false;
    }
    
    uint16_t u16NewPayloadLength = stFormat.Header.bitField.Length + u8DataLength;
    
    if((u16NewPayloadLength > pstPdu->u16PayloadBufSize)||(u16NewPayloadLength > LLC_SPEC_5_1_ADV_CHANNEL_PDU_MAX_PAYLOAD_SIZE))
    {
        return false;
    }
    
    memcpy(&pstPdu->pu8Payload[stFormat.Header.bitField.Length], pu8Data, u8DataLength);
    
    stFormat.Header.bitField.Length += u8DataLength;
    
    pstPdu->u32Header = stFormat.Header.u16Value;
    
    return true;
}


