/*
MLOG_ID_RANGE: 0x3880~0x389F
MLOG_ID_USED:  0x3880~0x3880
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>



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
#include "ble_impl.h"


#include "llc_ext_initiator.h"
#include "llc_ext_initiator_primary_scanning.h"
#include "llc_ext_initiator_secondary_scanning.h"
#include "llc_ext_initiator_state_machine.h"

#include "mlog.h"
#include "rom_callback.h"



void rom_llc_ext_initiator_primary_scanning_clear_phy(stLlcExtInitiator_t* pstInitiator)
{
    stLlcExtInitiatorPrimaryScanningPhy_t* pstInitiatorningPhy;

    uint32_t i;
    for(i = 0; i < LLC_EXT_INITIATOR_MAX_PRIMARY_SCANNING_PHYS; i++)
    {
        pstInitiatorningPhy = &(pstInitiator->stPrimaryScanningPhys[i]);
        pstInitiatorningPhy->bScanningPhyEnabled = false;
    }
}

uint8_t rom_llc_ext_initiator_primary_scanning_get_phy_num(stLlcExtInitiator_t* pstInitiator)
{
    stLlcExtInitiatorPrimaryScanningPhy_t* pstInitiatorningPhy;
    uint8_t u8PhyNum = 0;
    uint32_t i;
    for(i = 0; i < LLC_EXT_INITIATOR_MAX_PRIMARY_SCANNING_PHYS; i++)
    {
        pstInitiatorningPhy = &(pstInitiator->stPrimaryScanningPhys[i]);
        if (pstInitiatorningPhy->bScanningPhyEnabled)
        {
            u8PhyNum++;
        }
    }
    return u8PhyNum;
}

bool rom_llc_ext_initiator_primary_scanning_add_phy(stLlcExtInitiator_t* pstInitiator, EN_LL_PHY_T enumPhy, uint16_t Scan_Interval, uint16_t Scan_Window, uint8_t Channel_Map)
{
    stLlcExtInitiatorPrimaryScanningPhy_t* pstInitiatorningPhy;

    uint32_t i;
    for(i = 0; i < LLC_EXT_INITIATOR_MAX_PRIMARY_SCANNING_PHYS; i++)
    {
        pstInitiatorningPhy = &(pstInitiator->stPrimaryScanningPhys[i]);

        if (!pstInitiatorningPhy->bScanningPhyEnabled)
        {
            pstInitiatorningPhy->bScanningPhyEnabled = true;
            pstInitiatorningPhy->enumPhy = enumPhy;
            pstInitiatorningPhy->Scan_Interval = Scan_Interval;
            pstInitiatorningPhy->Scan_Window = Scan_Window;
            pstInitiatorningPhy->Channel_Map = Channel_Map;
            return true;
        }
    }

    return false;
}

//1M -> 2M -> Coded -> NULL -> 1M...
stLlcExtInitiatorPrimaryScanningPhy_t* rom_llc_ext_initiator_primary_scanning_get_next_phy(stLlcExtInitiator_t* pstInitiator, stLlcExtInitiatorPrimaryScanningPhy_t* pstCurScanningPhy)
{
    stLlcExtInitiatorPrimaryScanningPhy_t* pstInitiatorningPhy;

    if (NULL == pstCurScanningPhy)
    {
        pstInitiatorningPhy = &(pstInitiator->stPrimaryScanningPhys[0]);
        if (pstInitiatorningPhy->bScanningPhyEnabled)
        {
            return pstInitiatorningPhy;
        }
        else
        {
            return NULL;
        }
    }

    uint32_t i;
    bool bCurPhyFound = false;
    for(i = 0; i < LLC_EXT_INITIATOR_MAX_PRIMARY_SCANNING_PHYS; i++)
    {
        pstInitiatorningPhy = &(pstInitiator->stPrimaryScanningPhys[i]);
        if (bCurPhyFound)
        {
            if (pstInitiatorningPhy->bScanningPhyEnabled)
            {
                return pstInitiatorningPhy;
            }
            else
            {
                return NULL;
            }
        }

        if ((pstInitiatorningPhy == pstCurScanningPhy) && (pstInitiatorningPhy->bScanningPhyEnabled))
        {
            bCurPhyFound = true;
        }
    }

    return NULL;
}


//37 -> 38 -> 39 -> 37...
void rom_llc_ext_initiator_primary_scanning_channel_update(stLlcExtInitiatorPrimaryScanningPhy_t* pstInitiatorningPhy)
{
    uint8_t u8CurChannelIdx = pstInitiatorningPhy->u8CurrentChannelIdx;
    uint8_t u8ChannelFlags = pstInitiatorningPhy->Channel_Map & 0x7;
    uint8_t u8NextChannelIdx;
    if ((37 != u8CurChannelIdx) && (38 != u8CurChannelIdx) && (39 != u8CurChannelIdx))
    {
        if (u8ChannelFlags & LLC_SPEC_5_1_HCI_ADV_CHANNEL_MAP_37)
        {
            u8NextChannelIdx = 37;
        }
        else if (u8ChannelFlags & LLC_SPEC_5_1_HCI_ADV_CHANNEL_MAP_38)
        {
            u8NextChannelIdx = 38;
        }
        else if (u8ChannelFlags & LLC_SPEC_5_1_HCI_ADV_CHANNEL_MAP_39)
        {
            u8NextChannelIdx = 39;
        }
        else
        {
            //flags == 0, treat as 37|38|39
            u8NextChannelIdx = 37;
        }
    }
    else
    {
        const uint8_t pu8Map[8][3] =
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

        //u8CurChannelIdx should be 37 or 38 or 39 now

        uint8_t u8Row = u8ChannelFlags;
        uint8_t u8Column = u8CurChannelIdx - 37;

        u8NextChannelIdx = pu8Map[u8Row][u8Column];
    }

    pstInitiatorningPhy->u8CurrentChannelIdx = u8NextChannelIdx;
}




void rom_llc_ext_initiator_primary_scanning_on_enable(stLlcExtInitiator_t* pstInitiator)
{
    //nonting to do
}

void rom_llc_ext_initiator_primary_scanning_on_disable(stLlcExtInitiator_t* pstInitiator)
{
    //nonting to do
}


void rom_llc_ext_initiator_primary_scanning_prepare_timing(stLlcExtInitiator_t* pstInitiator, bool bStart)
{
    if (bStart)
    {
        uint32_t u32TotalScanWindowUs = 0;
        uint32_t u32MinScanIntervalUs = 0xFFFFFFFF;
        stLlcExtInitiatorPrimaryScanningPhy_t* pstInitiatorningPhy = rom_llc_ext_initiator_primary_scanning_get_next_phy(pstInitiator, NULL);
        while(NULL != pstInitiatorningPhy)
        {
            uint32_t u32ScanIntervalUs = pstInitiatorningPhy->Scan_Interval * 625;
            uint32_t u32ScanWindowUs = pstInitiatorningPhy->Scan_Window * 625;
            if (u32ScanWindowUs < LLC_EXT_INITIATOR_PRIMARY_SCANNING_MIN_WINDOW_US)
            {
                u32ScanWindowUs = LLC_EXT_INITIATOR_PRIMARY_SCANNING_MIN_WINDOW_US;
            }
            
            //phy init
            pstInitiatorningPhy->u32ChannelScanWindowUs = u32ScanWindowUs;
            pstInitiatorningPhy->u64OriginalStartTimeUs = 0;
            pstInitiatorningPhy->u8CurrentChannelIdx = 0;
            rom_llc_ext_initiator_primary_scanning_channel_update(pstInitiatorningPhy);


            if (u32ScanIntervalUs < u32MinScanIntervalUs)
            {
                u32MinScanIntervalUs = u32ScanIntervalUs;
            }

            u32TotalScanWindowUs += pstInitiatorningPhy->u32ChannelScanWindowUs + rom_ble_impl_get_time_us_between_ext_scan_channel();

            //loop
            pstInitiatorningPhy = rom_llc_ext_initiator_primary_scanning_get_next_phy(pstInitiator, pstInitiatorningPhy);
        }

        if (u32MinScanIntervalUs < u32TotalScanWindowUs)
        {
            u32MinScanIntervalUs = u32TotalScanWindowUs;
        }

        pstInitiator->u32PrimaryScanIntervalUs = u32MinScanIntervalUs;
        
        
        pstInitiator->pstCurrentPrimaryScanningPhy = rom_llc_ext_initiator_primary_scanning_get_next_phy(pstInitiator, NULL);
        pstInitiator->pstCurrentPrimaryScanningPhy->u32EventStartTimeOffsetUs = 0;
        uint64_t u64TimeUs = rom_ble_impl_get_ext_initiator_schedule_start_time();
        pstInitiator->pstCurrentPrimaryScanningPhy->u64OriginalStartTimeUs = u64TimeUs;
        
        pstInitiator->u64IntervalStartTimeUs = u64TimeUs;
        return;
    }
    
    for(;;)
    {
        stLlcExtInitiatorPrimaryScanningPhy_t* pstCurrentPhy;
        stLlcExtInitiatorPrimaryScanningPhy_t* pstNextPhy;

        pstCurrentPhy = pstInitiator->pstCurrentPrimaryScanningPhy;
        if (NULL == pstCurrentPhy)
        {
            //primary scan stopped
            return;
        }
        
        uint64_t u64TimeUs = rom_llc_timer_get_count(true) + rom_ble_impl_get_delay_time_us_for_primary_scanning_window_start();//set a delay time to avoid missing start time

        uint64_t u64CurrentPhyStartTimeUs = pstCurrentPhy->u64OriginalStartTimeUs + pstCurrentPhy->u32EventStartTimeOffsetUs;
        if (u64TimeUs <= u64CurrentPhyStartTimeUs)
        {
            //start current phy
            return;
        }

        uint64_t u64CurrentPhyEndTimeUs = pstCurrentPhy->u64OriginalStartTimeUs + pstCurrentPhy->u32ChannelScanWindowUs;
        if (u64TimeUs + LLC_EXT_INITIATOR_PRIMARY_SCANNING_MIN_WINDOW_US <= u64CurrentPhyEndTimeUs)
        {
            //continue current phy, update start time
            pstCurrentPhy->u32EventStartTimeOffsetUs = u64TimeUs - pstCurrentPhy->u64OriginalStartTimeUs;
            return;
        }

        //current phy passed
        rom_llc_ext_initiator_primary_scanning_channel_update(pstCurrentPhy);

        pstNextPhy = rom_llc_ext_initiator_primary_scanning_get_next_phy(pstInitiator, pstCurrentPhy);
        if (NULL == pstNextPhy)
        {
            //current interval passed
            pstNextPhy = rom_llc_ext_initiator_primary_scanning_get_next_phy(pstInitiator, NULL);

            uint64_t u64NextPhyStartTimeUs = pstInitiator->u64IntervalStartTimeUs + pstInitiator->u32PrimaryScanIntervalUs;
            
            //next interval
            pstNextPhy->u64OriginalStartTimeUs = u64NextPhyStartTimeUs;
            pstNextPhy->u32EventStartTimeOffsetUs = 0;

            pstInitiator->u64IntervalStartTimeUs = u64NextPhyStartTimeUs;

            pstInitiator->pstCurrentPrimaryScanningPhy = pstNextPhy;

            continue;
        }

        //NULL != pstNextPhy
        uint64_t u64NextPhyStartTimeUs = u64CurrentPhyEndTimeUs + rom_ble_impl_get_time_us_between_ext_scan_channel();
        
        //next phy
        pstNextPhy->u64OriginalStartTimeUs = u64NextPhyStartTimeUs;
        pstNextPhy->u32EventStartTimeOffsetUs = 0;

        pstInitiator->pstCurrentPrimaryScanningPhy = pstNextPhy;
    }
}


bool rom_llc_ext_initiator_primary_scanning_get_timing(stLlcExtInitiator_t* pstInitiator, stLlcExtInitiatorTiming_t* pstTiming)
{
    if (NULL == pstInitiator->pstCurrentPrimaryScanningPhy)
    {
        return false;
    }

    stLlcExtInitiatorPrimaryScanningPhy_t* pstCurrentPhy = pstInitiator->pstCurrentPrimaryScanningPhy;


    pstTiming->u64StartTimeUs = pstCurrentPhy->u64OriginalStartTimeUs + pstCurrentPhy->u32EventStartTimeOffsetUs;
    pstTiming->u32WorkTimeUs = pstCurrentPhy->u32ChannelScanWindowUs - pstCurrentPhy->u32EventStartTimeOffsetUs;
    pstTiming->u32MinWorkTimeUs = LLC_EXT_INITIATOR_PRIMARY_SCANNING_MIN_WINDOW_US;

    return true;
}


void rom_llc_ext_initiator_primary_scanning_update_receving_pdu(stLlcExtInitiator_t* pstInitiator, stLlcAdvChannelPdu_t* pstPdu)
{
    pstInitiator->pstRecevingPdu = pstPdu;

    pstInitiator->stRxPduPtr.pu16PayloadBufSize = &(pstInitiator->pstRecevingPdu->u16PayloadBufSize);
    pstInitiator->stRxPduPtr.pu8Payload = pstInitiator->pstRecevingPdu->pu8Payload;
    pstInitiator->stRxPduPtr.pu32Header = &(pstInitiator->pstRecevingPdu->u32Header);
    rom_llc_update_rx_pdu(&pstInitiator->stRxPduPtr, LLC_SPEC_5_1_ADV_CHANNEL_PDU_MAX_PAYLOAD_SIZE);
}


void rom_llc_ext_initiator_primary_scanning_on_scheduled(stLlcExtInitiator_t* pstInitiator)
{
    stLlcExtInitiatorPrimaryScanningPhy_t* pstCurrentPhy = pstInitiator->pstCurrentPrimaryScanningPhy;
    
    EN_LLC_SPEC_5_1_HCI_PHY_T PHY;
    switch(pstCurrentPhy->enumPhy)
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


uint32_t rom_llc_ext_initiator_primary_scanning_on_event_blocked (stLlcExtInitiator_t* pstInitiator)
{
    if (NULL != pstInitiator->pstRecevingPdu)
    {
        rom_llc_ext_initiator_free_pdu(pstInitiator, pstInitiator->pstRecevingPdu);
        pstInitiator->pstRecevingPdu = NULL;
    }
    
    uint32_t u32BlockedCount = 1;
    for(;;)
    {
        stLlcExtInitiatorPrimaryScanningPhy_t* pstCurrentPhy;
        stLlcExtInitiatorPrimaryScanningPhy_t* pstNextPhy;

        pstCurrentPhy = pstInitiator->pstCurrentPrimaryScanningPhy;
        if (NULL == pstCurrentPhy)
        {
            //primary scan stopped
            return u32BlockedCount;
        }

        uint64_t u64TimeUs = rom_llc_timer_get_count(true) + rom_ble_impl_get_delay_time_us_for_primary_scanning_window_start();//set a delay time to avoid missing start time

        uint64_t u64CurrentPhyStartTimeUs = pstCurrentPhy->u64OriginalStartTimeUs + pstCurrentPhy->u32EventStartTimeOffsetUs;
        if (u64TimeUs <= u64CurrentPhyStartTimeUs)
        {
            //start current phy
            return u32BlockedCount;
        }

        uint64_t u64CurrentPhyEndTimeUs = pstCurrentPhy->u64OriginalStartTimeUs + pstCurrentPhy->u32ChannelScanWindowUs;
        if (u64TimeUs + LLC_EXT_INITIATOR_PRIMARY_SCANNING_MIN_WINDOW_US <= u64CurrentPhyEndTimeUs)
        {
            //continue current phy, update start time
            pstCurrentPhy->u32EventStartTimeOffsetUs = u64TimeUs - pstCurrentPhy->u64OriginalStartTimeUs;
            return u32BlockedCount;
        }

        //current phy passed
        rom_llc_ext_initiator_primary_scanning_channel_update(pstCurrentPhy);
        
        u32BlockedCount++;

        pstNextPhy = rom_llc_ext_initiator_primary_scanning_get_next_phy(pstInitiator, pstCurrentPhy);
        if (NULL == pstNextPhy)
        {
            //current interval passed
            pstNextPhy = rom_llc_ext_initiator_primary_scanning_get_next_phy(pstInitiator, NULL);

            uint64_t u64NextPhyStartTimeUs = pstInitiator->u64IntervalStartTimeUs + pstInitiator->u32PrimaryScanIntervalUs;
            
            //next interval
            pstNextPhy->u64OriginalStartTimeUs = u64NextPhyStartTimeUs;
            pstNextPhy->u32EventStartTimeOffsetUs = 0;

            pstInitiator->u64IntervalStartTimeUs = u64NextPhyStartTimeUs;

            pstInitiator->pstCurrentPrimaryScanningPhy = pstNextPhy;

            continue;
        }

        //NULL != pstNextPhy
        uint64_t u64NextPhyStartTimeUs = u64CurrentPhyEndTimeUs + rom_ble_impl_get_time_us_between_ext_scan_channel();
        
        //next phy
        pstNextPhy->u64OriginalStartTimeUs = u64NextPhyStartTimeUs;
        pstNextPhy->u32EventStartTimeOffsetUs = 0;

        pstInitiator->pstCurrentPrimaryScanningPhy = pstNextPhy;
        
    }
}


EN_LLP_ERR_T rom_llc_ext_initiator_primary_scanning_config_llp(stLlcExtInitiator_t* pstInitiator, uint32_t u32MaxWorkWindowTimeUs)
{
    stLlcExtInitiatorPrimaryScanningPhy_t* pstCurrentPhy = pstInitiator->pstCurrentPrimaryScanningPhy;

    stLlpExtInitFragmentParam_t stParam;
    stParam.u32WorkTimeUs = LLC_TOOL_MIN((pstCurrentPhy->u32ChannelScanWindowUs - pstCurrentPhy->u32EventStartTimeOffsetUs), u32MaxWorkWindowTimeUs);

#if LLC_EXT_INITIATOR_PRIMARY_SCANNING_WINDOW_ADJUSTABLE
    //adjustable start time
    uint64_t u64ExpectedStartTimeUs = pstCurrentPhy->u64OriginalStartTimeUs + pstCurrentPhy->u32EventStartTimeOffsetUs;
    uint64_t u64TimeNow = rom_llc_timer_get_count(true);
    if (u64TimeNow < u64ExpectedStartTimeUs)
    {
        stParam.u64StartTimeUs = u64ExpectedStartTimeUs;
    }
    else
    {
        if (u64TimeNow + LLC_EXT_INITIATOR_PRIMARY_SCANNING_MIN_WINDOW_US <= u64ExpectedStartTimeUs + stParam.u32WorkTimeUs)
        {
            stParam.u64StartTimeUs = u64TimeNow;
        }
        else
        {
            //start time missed
            return LLP_ERR_TIME_POINT;
        }
    }
#else
    //fixed start time
    stParam.u64StartTimeUs = pstCurrentPhy->u64OriginalStartTimeUs + pstCurrentPhy->u32EventStartTimeOffsetUs;
#endif


    stParam.u32AnchorWindowUs = 0;
    stParam.u16IfsUs = LLC_SPEC_5_1_T_IFS_US;
    stParam.u16IfsRxWindowUs = rom_ble_impl_get_ifs_rx_window_us(pstCurrentPhy->enumPhy);
    stParam.u8ChannelIdx = pstCurrentPhy->u8CurrentChannelIdx;
    stParam.bIsSecondaryChannel = false;
    stParam.enumPhy = pstCurrentPhy->enumPhy;
    stParam.u32AccessAddress = LLC_SPEC_5_1_ADV_CHANNEL_ACCESS_ADDRESS;
    stParam.u32CrcInit = LLC_SPEC_5_1_ADV_CHANNEL_CRC_INIT;
    stParam.u16RxPayloadSizeLimit = LLC_SPEC_5_1_ADV_CHANNEL_PDU_MAX_PAYLOAD_SIZE;
    stParam.pstLlpTxPduPtr = &(pstInitiator->stTxPduPtr);
    stParam.pstLlpRxPduPtr = &(pstInitiator->stRxPduPtr);
    
    EN_LLP_ERR_T err = rom_llc_start_ext_init_fragment(&stParam);
    MLOG1688I(0x3880, "EXT INIT PRIMARY SCANNING config ch:%u phy:%u priority:%u", stParam.u8ChannelIdx, stParam.enumPhy, pstInitiator->stSchedulerEvent.u8Priority);
    return err;
}


void rom_llc_ext_initiator_primary_scanning_on_llp_fragment_start(stLlcExtInitiator_t* pstInitiator)
{
    pstInitiator->pstReceivedExtendedPduWhenPrimaryScanning = NULL;
    pstInitiator->bConnectIndSentWhenPrimaryScanning = false;
    pstInitiator->enumState = LLC_EXT_INITIATOR_STATE_RX_ADV;
}



bool rom_llc_ext_initiator_primary_scanning_on_llp_fragment_stop(stLlcExtInitiator_t* pstInitiator)
{
    pstInitiator->enumState = LLC_EXT_INITIATOR_STATE_IDLE;
    
    if (pstInitiator->bConnectIndSentWhenPrimaryScanning)
    {
        pstInitiator->bConnectIndSentWhenPrimaryScanning = false;
        
        if (NULL == pstInitiator->pstRecevingPdu)
        {
            ////unexpected situation
            system_error(SYS_ERR_MODULE_LLC_EXT_INITIATOR_PRIMARY_SCANNING, __LINE__, (uint32_t)pstInitiator, (uint32_t)0);
            return false;
        }
        
        stLlcExtInitiatorConnectionParameters_t* pstChosenConnectionParameters = pstInitiator->pstChosenConnectionParameters;
    
        stLlcSpec51AdvChannelPduHeader_t stConnectReqHeader;
        stConnectReqHeader.u16Value = pstChosenConnectionParameters->u32ConnectReqHeader & 0xFFFF;
        
        stLlcSpec51ConnectInd_t* pstConnectInd = (stLlcSpec51ConnectInd_t*)pstChosenConnectionParameters->pu8ConnectReqPduPayload;
        
        stLlcAdvChannelPdu_t* pstAdvPdu = pstInitiator->pstRecevingPdu;
        stLlcExtInitiatorPduExtraInfo_t* pstAdvPduExtraInfo = rom_llc_ext_initiator_get_pdu_extra_info(pstAdvPdu);
        if (NULL == pstAdvPduExtraInfo)
        {
            ////unexpected situation
            system_error(SYS_ERR_MODULE_LLC_EXT_INITIATOR_PRIMARY_SCANNING, __LINE__, (uint32_t)pstInitiator, (uint32_t)pstAdvPdu);
            return false;
        }
        stLlcExtInitiatorPrimaryScanningPhy_t* pstPhy = pstInitiator->pstCurrentPrimaryScanningPhy;
        
        
        uint64_t u64PktEndTime = pstAdvPduExtraInfo->u64PduEndTimeUs;
        u64PktEndTime += LLC_SPEC_5_1_T_IFS_US + rom_llc_tool_calculate_trx_time_us_by_pdu_size(2 + LLC_SPEC_5_1_CONNECT_IND_PAYLOAD_SIZE, pstPhy->enumPhy);
        u64PktEndTime += LLC_SPEC_5_1_CONNECT_IND_TRANSMIT_WINDOW_DELAY_US;
        
        stLlcMasterInitParam_t* pstParam = &pstInitiator->stMasterInitParam;
        
        pstParam->u64StartTime = u64PktEndTime + (pstConnectInd->bitField.WinOffset*1250) + (pstConnectInd->bitField.WinSize/2)*1250;
        pstParam->u64TimeOfLastSync = pstAdvPduExtraInfo->u64PduStartTimeUs;
        pstParam->u32AccessAddress = pstConnectInd->bitField.AA;
        pstParam->u32CrcInit = pstConnectInd->bitField.CRCInit;
        pstParam->u16ConnItv1250us = pstConnectInd->bitField.Interval;
        pstParam->u16ConnSlaveLatency = pstConnectInd->bitField.Latency;
        pstParam->u16ConnSupervisionTimeout10ms = pstConnectInd->bitField.TimeOut;
        
        pstParam->bChSel = stConnectReqHeader.bitField.ChSel;
        
        memcpy(pstParam->pu8ChM,pstConnectInd->bitField.ChM,5);
        pstParam->u8Hop = pstConnectInd->bitField.Hop;
        pstParam->u8Sca = pstConnectInd->bitField.SCA;
        pstParam->enumPhy = pstPhy->enumPhy;
        
        
        pstParam->u8SelfAddrType = pstInitiator->bOwnAddressTxAdd?1:0;
        memcpy(pstParam->pu8SelfAddr,pstInitiator->pu8OwnAddress,6);
        
        pstParam->u8PeerAddrType = stConnectReqHeader.bitField.RxAdd?1:0;
        memcpy(pstParam->pu8PeerAddr,pstConnectInd->bitField.AdvA,6);
        
        switch(pstAdvPduExtraInfo->enumAddressTypeForPrimaryScanning)
        {
            case LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_PUBLIC_IDENTITY:
                pstParam->Peer_RPA_Resoluted = 1;
                pstParam->Peer_Identify_Address_Type = 0;
                memcpy(pstParam->Peer_Identify_Address, pstAdvPduExtraInfo->pu8AddressForPrimaryScanning,6);
                break;
           
            case LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_RANDOM_IDENTITY:
                pstParam->Peer_RPA_Resoluted = 1;
                pstParam->Peer_Identify_Address_Type = 1;
                memcpy(pstParam->Peer_Identify_Address, pstAdvPduExtraInfo->pu8AddressForPrimaryScanning,6);
                break;
            
            default:
                pstParam->Peer_RPA_Resoluted = 0;
                break;
        }
        
        pstParam->Own_Address_Be_RPA = pstInitiator->bOwnAddressResolvable?1:0;
        pstParam->u32RssiRelatedValue = pstAdvPdu->u32RssiRelatedValue;
        
        pstInitiator->bCreateConnection = true;
        
        rom_llc_ext_initiator_free_pdu(pstInitiator, pstInitiator->pstRecevingPdu);
        pstInitiator->pstRecevingPdu = NULL;
    }
    else
    {
        if (NULL != pstInitiator->pstReceivedExtendedPduWhenPrimaryScanning)
        {
            rom_llc_ext_initiator_secondary_scanning_on_received_extended_pdu_when_primary_scanning(pstInitiator, pstInitiator->pstReceivedExtendedPduWhenPrimaryScanning);
            
            rom_llc_ext_initiator_free_pdu(pstInitiator, pstInitiator->pstReceivedExtendedPduWhenPrimaryScanning);
            pstInitiator->pstReceivedExtendedPduWhenPrimaryScanning = NULL;
        }
        
        if (NULL != pstInitiator->pstRecevingPdu)
        {
            rom_llc_ext_initiator_free_pdu(pstInitiator, pstInitiator->pstRecevingPdu);
            pstInitiator->pstRecevingPdu = NULL;
        }
    }

    rom_llc_ext_initiator_state_machine_schedule_next_scanning(pstInitiator);
    return false;
}


EN_LLP_STATE_CMD_T rom_llc_ext_initiator_primary_scanning_on_llp_tx_end(stLlcExtInitiator_t* pstInitiator, stLlpTxEndReport_t *pstReport)
{
    if (LLC_EXT_INITIATOR_STATE_TX_REQ != pstInitiator->enumState)
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_INITIATOR_PRIMARY_SCANNING, __LINE__, (uint32_t)pstInitiator, pstInitiator->enumState);
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }

    pstInitiator->bConnectIndSentWhenPrimaryScanning = true;
    pstInitiator->enumState = LLC_EXT_INITIATOR_STATE_IDLE;
    return LLP_STATE_CMD_STOP_FRAGMENT;
}


EN_LLP_STATE_CMD_T rom_llc_ext_initiator_primary_scanning_on_llp_rx_adv_end(stLlcExtInitiator_t* pstInitiator, stLlpRxEndReport_t *pstReport, stLlcAdvChannelPdu_t* pstPdu, stLlcExtInitiatorPduExtraInfo_t* pstExtraInfo)
{
    if (rom_llc_ext_initiator_is_pdu_empty(pstInitiator, pstPdu))
    {
        //re-alloc pdu
        rom_llc_ext_initiator_free_pdu(pstInitiator, pstPdu);
        rom_llc_ext_initiator_primary_scanning_update_receving_pdu(pstInitiator, rom_llc_ext_initiator_alloc_pdu(pstInitiator));

        return LLP_STATE_CMD_CANCEL_TX;
    }

    if (LLP_RX_RESULT_CRC_OK != pstReport->enumRxResult)
    {
        pstInitiator->enumState = LLC_EXT_INITIATOR_STATE_RX_ADV;
        return LLP_STATE_CMD_CANCEL_TX;
    }

    stLlcSpec51AdvChannelPduHeader_t stHeader;
    stHeader.u16Value = pstPdu->u32Header & 0xFFFF;

    if (stHeader.bitField.Length > pstPdu->u16PayloadBufSize)
    {
        //re-alloc pdu
        rom_llc_ext_initiator_free_pdu(pstInitiator, pstPdu);
        rom_llc_ext_initiator_primary_scanning_update_receving_pdu(pstInitiator, rom_llc_ext_initiator_alloc_pdu(pstInitiator));

        return LLP_STATE_CMD_CANCEL_TX;
    }

    uint8_t *AdvA = NULL;
    uint8_t *TargetA = NULL;

    switch(stHeader.bitField.PDUType)
    {
        case LLC_SPEC_5_1_ADV_CH_PDU_TYPE_ADV_IND:
        {
            if (stHeader.bitField.Length < 6)
            {
                return LLP_STATE_CMD_CANCEL_TX;
            }
            pstExtraInfo->enumEventTypeForPrimaryScanning = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_LEGACY_EVENT_TYPE_ADV_IND;
            AdvA = &(pstPdu->pu8Payload[0]);
        }
            break;

        case LLC_SPEC_5_1_ADV_CH_PDU_TYPE_DIRECT_IND:
        {
            if (12 != stHeader.bitField.Length)
            {
                return LLP_STATE_CMD_CANCEL_TX;
            }
            pstExtraInfo->enumEventTypeForPrimaryScanning = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_LEGACY_EVENT_TYPE_ADV_DIRECT_IND;
            AdvA = &(pstPdu->pu8Payload[0]);
            TargetA = &(pstPdu->pu8Payload[6]);
        }
            break;


        case LLC_SPEC_5_1_ADV_CH_PDU_TYPE_ADV_EXT_IND:
        {
            pstInitiator->pstReceivedExtendedPduWhenPrimaryScanning = pstPdu;
            rom_llc_ext_initiator_primary_scanning_update_receving_pdu(pstInitiator, rom_llc_ext_initiator_alloc_pdu(pstInitiator));
        }
            return LLP_STATE_CMD_STOP_FRAGMENT;

        
        case LLC_SPEC_5_1_ADV_CH_PDU_TYPE_ADV_SCAN_IND:
        case LLC_SPEC_5_1_ADV_CH_PDU_TYPE_NONCONN_IND:
        default:
            return LLP_STATE_CMD_CANCEL_TX;
    }

    //update SCAN_REQ ASAP
    rom_llc_ext_initiator_state_machine_update_connection_req_pdu_AdvA(pstInitiator, AdvA, stHeader.bitField.TxAdd, stHeader.bitField.ChSel);
    rom_llc_update_tx_pdu(&pstInitiator->stTxPduPtr);

    //PRA and WhiteList filter
    if (rom_llc_ext_initiator_check_RPA_and_white_list_disallowed(pstInitiator,
                                                             stHeader.bitField.TxAdd, AdvA,
                                                             stHeader.bitField.RxAdd, TargetA,
                                                             &(pstExtraInfo->enumAddressTypeForPrimaryScanning), pstExtraInfo->pu8AddressForPrimaryScanning,
                                                             &(pstExtraInfo->enumDirectAddressTypeForPrimaryScanning), pstExtraInfo->pu8DirectAddressForPrimaryScanning))
    {
        return LLP_STATE_CMD_CANCEL_TX;
    }

    pstInitiator->enumState = LLC_EXT_INITIATOR_STATE_TX_REQ;
    return LLP_STATE_CMD_CONTINUE;
    
}


EN_LLP_STATE_CMD_T rom_llc_ext_initiator_primary_scanning_on_llp_rx_end(stLlcExtInitiator_t* pstInitiator, stLlpRxEndReport_t *pstReport)
{
    stLlcExtInitiatorPrimaryScanningPhy_t* pstPhy = pstInitiator->pstCurrentPrimaryScanningPhy;
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
        pstExtraInfo->enumPhy = pstPhy->enumPhy;
    }

    EN_LLP_STATE_CMD_T enumStateCmd;
    if (LLC_EXT_INITIATOR_STATE_RX_ADV == pstInitiator->enumState)
    {
        enumStateCmd = rom_llc_ext_initiator_primary_scanning_on_llp_rx_adv_end(pstInitiator, pstReport, pstPdu, pstExtraInfo);
    }
    else
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_INITIATOR_PRIMARY_SCANNING, __LINE__, (uint32_t)pstInitiator, pstInitiator->enumState);
        enumStateCmd = LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    return enumStateCmd;
}
