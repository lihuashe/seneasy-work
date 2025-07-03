/*
MLOG_ID_RANGE: 0x3810~0x382F
MLOG_ID_USED:  0x3810~0x3810
*/

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
#include "patch_llc.h"
#include "llc_address.h"
#include "llc_privacy.h"
#include "llc_tool.h"



#include "extended_controller_interface.h"
#include "hci.h"
#include "ble_impl.h"


#include "llc_ext_scan.h"
#include "llc_ext_scan_primary_scanning.h"
#include "llc_ext_scan_secondary_scanning.h"
#include "llc_ext_scan_state_machine.h"
#include "llc_ext_scan_duplicate_filter.h"

#include "mlog.h"
#include "rom_callback.h"


void rom_llc_ext_scan_primary_scanning_report_task_process(void* pParam);

void rom_llc_ext_scan_primary_scanning_clear_phy(stLlcExtScan_t* pstScan)
{
    stLlcExtScanPrimaryScanningPhy_t* pstScanningPhy;

    uint32_t i;
    for(i = 0; i < LLC_EXT_SCAN_MAX_PRIMARY_SCANNING_PHYS; i++)
    {
        pstScanningPhy = &(pstScan->stPrimaryScanningPhys[i]);
        pstScanningPhy->bScanningPhyEnabled = false;
    }
}

uint8_t rom_llc_ext_scan_primary_scanning_get_phy_num(stLlcExtScan_t* pstScan)
{
    stLlcExtScanPrimaryScanningPhy_t* pstScanningPhy;
    uint8_t u8PhyNum = 0;
    uint32_t i;
    for(i = 0; i < LLC_EXT_SCAN_MAX_PRIMARY_SCANNING_PHYS; i++)
    {
        pstScanningPhy = &(pstScan->stPrimaryScanningPhys[i]);
        if (pstScanningPhy->bScanningPhyEnabled)
        {
            u8PhyNum++;
        }
    }
    return u8PhyNum;
}

bool rom_llc_ext_scan_primary_scanning_add_phy(stLlcExtScan_t* pstScan, EN_LL_PHY_T enumPhy, uint8_t Scan_Type, uint16_t Scan_Interval, uint16_t Scan_Window, uint8_t Channel_Map)
{
    stLlcExtScanPrimaryScanningPhy_t* pstScanningPhy;

    uint32_t i;
    for(i = 0; i < LLC_EXT_SCAN_MAX_PRIMARY_SCANNING_PHYS; i++)
    {
        pstScanningPhy = &(pstScan->stPrimaryScanningPhys[i]);

        if (!pstScanningPhy->bScanningPhyEnabled)
        {
            pstScanningPhy->bScanningPhyEnabled = true;
            pstScanningPhy->enumPhy = enumPhy;
            pstScanningPhy->Scan_Type = Scan_Type;
            pstScanningPhy->Scan_Interval = Scan_Interval;
            pstScanningPhy->Scan_Window = Scan_Window;
            pstScanningPhy->Channel_Map = Channel_Map;
            return true;
        }
    }

    return false;
}

//1M -> 2M -> Coded -> NULL -> 1M...
stLlcExtScanPrimaryScanningPhy_t* rom_llc_ext_scan_primary_scanning_get_next_phy(stLlcExtScan_t* pstScan, stLlcExtScanPrimaryScanningPhy_t* pstCurScanningPhy)
{
    stLlcExtScanPrimaryScanningPhy_t* pstScanningPhy;

    if (NULL == pstCurScanningPhy)
    {
        pstScanningPhy = &(pstScan->stPrimaryScanningPhys[0]);
        if (pstScanningPhy->bScanningPhyEnabled)
        {
            return pstScanningPhy;
        }
        else
        {
            return NULL;
        }
    }

    uint32_t i;
    bool bCurPhyFound = false;
    for(i = 0; i < LLC_EXT_SCAN_MAX_PRIMARY_SCANNING_PHYS; i++)
    {
        pstScanningPhy = &(pstScan->stPrimaryScanningPhys[i]);
        if (bCurPhyFound)
        {
            if (pstScanningPhy->bScanningPhyEnabled)
            {
                return pstScanningPhy;
            }
            else
            {
                return NULL;
            }
        }

        if ((pstScanningPhy == pstCurScanningPhy) && (pstScanningPhy->bScanningPhyEnabled))
        {
            bCurPhyFound = true;
        }
    }

    return NULL;
}


//37 -> 38 -> 39 -> 37...
void rom_llc_ext_scan_primary_scanning_channel_update(stLlcExtScanPrimaryScanningPhy_t* pstScanningPhy)
{
    uint8_t u8CurChannelIdx = pstScanningPhy->u8CurrentChannelIdx;
    uint8_t u8ChannelFlags = pstScanningPhy->Channel_Map & 0x7;
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

    pstScanningPhy->u8CurrentChannelIdx = u8NextChannelIdx;
}

void rom_llc_ext_scan_primary_scanning_reset_duplicate_filter_task_process(void* pParam)
{
    rom_llc_ext_scan_duplicate_filter_reset();
}

void rom_llc_ext_scan_primary_scanning_reset_duplicate_filter(void)
{
    if (!rom_llc_send_msg_to_llc_task(rom_llc_ext_scan_primary_scanning_reset_duplicate_filter_task_process, NULL))
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_PRIMARY_SCANNING, __LINE__, 0, 0);
    }
}


void rom_llc_ext_scan_primary_scanning_add_pdu_to_report_list_tail(stLlcExtScan_t* pstScan, stLlcAdvChannelPdu_t* pstPdu)
{
    uint32_t u32Primask = __get_PRIMASK();
    __disable_irq();

    if (NULL == pstScan->pstPrimaryScanningReportPduListHead)
    {
        pstPdu->prev = NULL;
        pstPdu->next = NULL;
        pstScan->pstPrimaryScanningReportPduListHead = pstPdu;
        pstScan->pstPrimaryScanningReportPduListTail = pstPdu;
    }
    else
    {
        pstPdu->prev = pstScan->pstPrimaryScanningReportPduListTail;
        pstPdu->next = NULL;
        pstScan->pstPrimaryScanningReportPduListTail->next = pstPdu;
        pstScan->pstPrimaryScanningReportPduListTail = pstPdu;
    }
    __set_PRIMASK(u32Primask);
}

void rom_llc_ext_scan_primary_scanning_remove_and_free_pdu_from_report_list_head(stLlcExtScan_t* pstScan)
{
    uint32_t u32Primask = __get_PRIMASK();
    __disable_irq();
    stLlcAdvChannelPdu_t* pstPdu = pstScan->pstPrimaryScanningReportPduListHead;
    if (NULL == pstPdu)
    {
        __set_PRIMASK(u32Primask);
        return;
    }

    pstScan->pstPrimaryScanningReportPduListHead = pstPdu->next;
    if (NULL == pstScan->pstPrimaryScanningReportPduListHead)
    {
        pstScan->pstPrimaryScanningReportPduListTail = NULL;
    }
    else
    {
        pstScan->pstPrimaryScanningReportPduListHead->prev = NULL;
    }
    __set_PRIMASK(u32Primask);
    rom_llc_ext_scan_free_pdu(pstScan, pstPdu);
}


void rom_llc_ext_scan_primary_scanning_on_enable(stLlcExtScan_t* pstScan)
{
    pstScan->pstPrimaryScanningReportPduListHead = NULL;
    pstScan->pstPrimaryScanningReportPduListTail = NULL;
    pstScan->bPrimaryReporting = false;
}

void rom_llc_ext_scan_primary_scanning_on_disable(stLlcExtScan_t* pstScan)
{
    uint32_t u32Primask = __get_PRIMASK();
    for(;;)
    {
        __disable_irq();
        stLlcAdvChannelPdu_t *pstPdu = pstScan->pstPrimaryScanningReportPduListHead;
        __set_PRIMASK(u32Primask);
        if (NULL == pstPdu)
        {
            break;
        }
        
        rom_llc_ext_scan_primary_scanning_remove_and_free_pdu_from_report_list_head(pstScan);
    }
}


void rom_llc_ext_scan_primary_scanning_prepare_timing(stLlcExtScan_t* pstScan, bool bStart)
{
    if (bStart)
    {
        uint32_t u32TotalScanWindowUs = 0;
        uint32_t u32MinScanIntervalUs = 0xFFFFFFFF;
        stLlcExtScanPrimaryScanningPhy_t* pstScanningPhy = rom_llc_ext_scan_primary_scanning_get_next_phy(pstScan, NULL);
        while(NULL != pstScanningPhy)
        {
            uint32_t u32ScanIntervalUs = pstScanningPhy->Scan_Interval * 625;
            uint32_t u32ScanWindowUs = pstScanningPhy->Scan_Window * 625;
            if (u32ScanWindowUs < LLC_EXT_SCAN_PRIMARY_SCANNING_MIN_WINDOW_US)
            {
                u32ScanWindowUs = LLC_EXT_SCAN_PRIMARY_SCANNING_MIN_WINDOW_US;
            }
            
            //phy init
            pstScanningPhy->u32ChannelScanWindowUs = u32ScanWindowUs;
            pstScanningPhy->u64OriginalStartTimeUs = 0;
            pstScanningPhy->u8CurrentChannelIdx = 0;
            rom_llc_ext_scan_primary_scanning_channel_update(pstScanningPhy);


            if (u32ScanIntervalUs < u32MinScanIntervalUs)
            {
                u32MinScanIntervalUs = u32ScanIntervalUs;
            }

            u32TotalScanWindowUs += pstScanningPhy->u32ChannelScanWindowUs + rom_ble_impl_get_time_us_between_ext_scan_channel();

            //loop
            pstScanningPhy = rom_llc_ext_scan_primary_scanning_get_next_phy(pstScan, pstScanningPhy);
        }

        if (u32MinScanIntervalUs < u32TotalScanWindowUs)
        {
            u32MinScanIntervalUs = u32TotalScanWindowUs;
        }

        pstScan->u32PrimaryScanIntervalUs = u32MinScanIntervalUs;
        
        
        rom_llc_ext_scan_primary_scanning_reset_duplicate_filter();
        pstScan->pstCurrentPrimaryScanningPhy = rom_llc_ext_scan_primary_scanning_get_next_phy(pstScan, NULL);
        pstScan->pstCurrentPrimaryScanningPhy->u32EventStartTimeOffsetUs = 0;
        uint64_t u64TimeUs = rom_ble_impl_get_ext_scan_schedule_start_time();
        pstScan->pstCurrentPrimaryScanningPhy->u64OriginalStartTimeUs = u64TimeUs;
        
        pstScan->u64DurationStartTimeUs = u64TimeUs;
        pstScan->u64IntervalStartTimeUs = u64TimeUs;
        return;
    }
    
    for(;;)
    {
        stLlcExtScanPrimaryScanningPhy_t* pstCurrentPhy;
        stLlcExtScanPrimaryScanningPhy_t* pstNextPhy;

        pstCurrentPhy = pstScan->pstCurrentPrimaryScanningPhy;
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
        if (u64TimeUs + LLC_EXT_SCAN_PRIMARY_SCANNING_MIN_WINDOW_US <= u64CurrentPhyEndTimeUs)
        {
            //continue current phy, update start time
            pstCurrentPhy->u32EventStartTimeOffsetUs = u64TimeUs - pstCurrentPhy->u64OriginalStartTimeUs;
            return;
        }

        //current phy passed
        rom_llc_ext_scan_primary_scanning_channel_update(pstCurrentPhy);

        pstNextPhy = rom_llc_ext_scan_primary_scanning_get_next_phy(pstScan, pstCurrentPhy);
        if (NULL == pstNextPhy)
        {
            //current interval passed
            pstNextPhy = rom_llc_ext_scan_primary_scanning_get_next_phy(pstScan, NULL);

            uint64_t u64NextPhyStartTimeUs = pstScan->u64IntervalStartTimeUs + pstScan->u32PrimaryScanIntervalUs;
            if ((0 != pstScan->u32DurationUs) && (u64NextPhyStartTimeUs > pstScan->u64DurationStartTimeUs + pstScan->u32DurationUs))
            {
                //duration passed
                if (0 == pstScan->u64PeriodUs)
                {
                    //primary scan stop
                    pstScan->pstCurrentPrimaryScanningPhy = NULL;
                }
                else
                {
                    //next period
                    u64NextPhyStartTimeUs = pstScan->u64DurationStartTimeUs + pstScan->u64PeriodUs;

                    pstNextPhy->u64OriginalStartTimeUs = u64NextPhyStartTimeUs;
                    pstNextPhy->u32EventStartTimeOffsetUs = 0;

                    pstScan->u64DurationStartTimeUs = u64NextPhyStartTimeUs;
                    pstScan->u64IntervalStartTimeUs = u64NextPhyStartTimeUs;

                    pstScan->pstCurrentPrimaryScanningPhy = pstNextPhy;

                    if (LLC_SPEC_5_1_HCI_FILTER_DUPLICATES_ENABLED_AND_RESET_FOR_EACH_SCAN_PERIOD == pstScan->Filter_Duplicates)
                    {
                        rom_llc_ext_scan_primary_scanning_reset_duplicate_filter();
                    }
                }
            }
            else
            {
                //next interval
                pstNextPhy->u64OriginalStartTimeUs = u64NextPhyStartTimeUs;
                pstNextPhy->u32EventStartTimeOffsetUs = 0;

                pstScan->u64IntervalStartTimeUs = u64NextPhyStartTimeUs;

                pstScan->pstCurrentPrimaryScanningPhy = pstNextPhy;
            }

            continue;
        }

        //NULL != pstNextPhy
        uint64_t u64NextPhyStartTimeUs = u64CurrentPhyEndTimeUs + rom_ble_impl_get_time_us_between_ext_scan_channel();
        if ((0 != pstScan->u32DurationUs) && (u64NextPhyStartTimeUs > pstScan->u64DurationStartTimeUs + pstScan->u32DurationUs))
        {
            //duration passed
            if (0 == pstScan->u64PeriodUs)
            {
                //primary scan stop
                pstScan->pstCurrentPrimaryScanningPhy = NULL;
            }
            else
            {
                //next period
                u64NextPhyStartTimeUs = pstScan->u64DurationStartTimeUs + pstScan->u64PeriodUs;

                pstNextPhy->u64OriginalStartTimeUs = u64NextPhyStartTimeUs;
                pstNextPhy->u32EventStartTimeOffsetUs = 0;

                pstScan->u64DurationStartTimeUs = u64NextPhyStartTimeUs;
                pstScan->u64IntervalStartTimeUs = u64NextPhyStartTimeUs;

                pstScan->pstCurrentPrimaryScanningPhy = pstNextPhy;
            }
        }
        else
        {
            //next phy
            pstNextPhy->u64OriginalStartTimeUs = u64NextPhyStartTimeUs;
            pstNextPhy->u32EventStartTimeOffsetUs = 0;

            pstScan->pstCurrentPrimaryScanningPhy = pstNextPhy;
        }
    }
}


bool rom_llc_ext_scan_primary_scanning_get_timing(stLlcExtScan_t* pstScan, stLlcExtScanTiming_t* pstTiming)
{
    if (NULL == pstScan->pstCurrentPrimaryScanningPhy)
    {
        return false;
    }

    stLlcExtScanPrimaryScanningPhy_t* pstCurrentPhy = pstScan->pstCurrentPrimaryScanningPhy;


    pstTiming->u64StartTimeUs = pstCurrentPhy->u64OriginalStartTimeUs + pstCurrentPhy->u32EventStartTimeOffsetUs;
    pstTiming->u32WorkTimeUs = pstCurrentPhy->u32ChannelScanWindowUs - pstCurrentPhy->u32EventStartTimeOffsetUs;
    pstTiming->u32MinWorkTimeUs = LLC_EXT_SCAN_PRIMARY_SCANNING_MIN_WINDOW_US;

    return true;
}


void rom_llc_ext_scan_primary_scanning_update_receving_pdu(stLlcExtScan_t* pstScan, stLlcAdvChannelPdu_t* pstPdu)
{
    pstScan->pstRecevingPdu = pstPdu;

    pstScan->stRxPduPtr.pu16PayloadBufSize = &(pstScan->pstRecevingPdu->u16PayloadBufSize);
    pstScan->stRxPduPtr.pu8Payload = pstScan->pstRecevingPdu->pu8Payload;
    pstScan->stRxPduPtr.pu32Header = &(pstScan->pstRecevingPdu->u32Header);
    
    rom_llc_update_rx_pdu(&pstScan->stRxPduPtr, LLC_SPEC_5_1_ADV_CHANNEL_PDU_MAX_PAYLOAD_SIZE);
}


void rom_llc_ext_scan_primary_scanning_on_scheduled(stLlcExtScan_t* pstScan)
{
    pstScan->pstRecevingPdu = rom_llc_ext_scan_alloc_pdu(pstScan);
    pstScan->stRxPduPtr.pu16PayloadBufSize = &(pstScan->pstRecevingPdu->u16PayloadBufSize);
    pstScan->stRxPduPtr.pu8Payload = pstScan->pstRecevingPdu->pu8Payload;
    pstScan->stRxPduPtr.pu32Header = &(pstScan->pstRecevingPdu->u32Header);
}


uint32_t rom_llc_ext_scan_primary_scanning_on_event_blocked (stLlcExtScan_t* pstScan)
{
    if (NULL != pstScan->pstRecevingPdu)
    {
        rom_llc_ext_scan_free_pdu(pstScan, pstScan->pstRecevingPdu);
        pstScan->pstRecevingPdu = NULL;
    }
    
    uint32_t u32BlockedCount = 1;
    for(;;)
    {
        stLlcExtScanPrimaryScanningPhy_t* pstCurrentPhy;
        stLlcExtScanPrimaryScanningPhy_t* pstNextPhy;

        pstCurrentPhy = pstScan->pstCurrentPrimaryScanningPhy;
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
        if (u64TimeUs + LLC_EXT_SCAN_PRIMARY_SCANNING_MIN_WINDOW_US <= u64CurrentPhyEndTimeUs)
        {
            //continue current phy, update start time
            pstCurrentPhy->u32EventStartTimeOffsetUs = u64TimeUs - pstCurrentPhy->u64OriginalStartTimeUs;
            return u32BlockedCount;
        }

        //current phy passed
        rom_llc_ext_scan_primary_scanning_channel_update(pstCurrentPhy);
        
        u32BlockedCount++;

        pstNextPhy = rom_llc_ext_scan_primary_scanning_get_next_phy(pstScan, pstCurrentPhy);
        if (NULL == pstNextPhy)
        {
            //current interval passed
            pstNextPhy = rom_llc_ext_scan_primary_scanning_get_next_phy(pstScan, NULL);

            uint64_t u64NextPhyStartTimeUs = pstScan->u64IntervalStartTimeUs + pstScan->u32PrimaryScanIntervalUs;
            if ((0 != pstScan->u32DurationUs) && (u64NextPhyStartTimeUs > pstScan->u64DurationStartTimeUs + pstScan->u32DurationUs))
            {
                //duration passed
                if (0 == pstScan->u64PeriodUs)
                {
                    //primary scan stop
                    pstScan->pstCurrentPrimaryScanningPhy = NULL;
                }
                else
                {
                    //next period
                    u64NextPhyStartTimeUs = pstScan->u64DurationStartTimeUs + pstScan->u64PeriodUs;

                    pstNextPhy->u64OriginalStartTimeUs = u64NextPhyStartTimeUs;
                    pstNextPhy->u32EventStartTimeOffsetUs = 0;

                    pstScan->u64DurationStartTimeUs = u64NextPhyStartTimeUs;
                    pstScan->u64IntervalStartTimeUs = u64NextPhyStartTimeUs;

                    pstScan->pstCurrentPrimaryScanningPhy = pstNextPhy;

                    if (LLC_SPEC_5_1_HCI_FILTER_DUPLICATES_ENABLED_AND_RESET_FOR_EACH_SCAN_PERIOD == pstScan->Filter_Duplicates)
                    {
                        rom_llc_ext_scan_primary_scanning_reset_duplicate_filter();
                    }
                }
            }
            else
            {
                //next interval
                pstNextPhy->u64OriginalStartTimeUs = u64NextPhyStartTimeUs;
                pstNextPhy->u32EventStartTimeOffsetUs = 0;

                pstScan->u64IntervalStartTimeUs = u64NextPhyStartTimeUs;

                pstScan->pstCurrentPrimaryScanningPhy = pstNextPhy;
            }

            continue;
        }

        //NULL != pstNextPhy
        uint64_t u64NextPhyStartTimeUs = u64CurrentPhyEndTimeUs + rom_ble_impl_get_time_us_between_ext_scan_channel();
        if ((0 != pstScan->u32DurationUs) && (u64NextPhyStartTimeUs > pstScan->u64DurationStartTimeUs + pstScan->u32DurationUs))
        {
            //duration passed
            if (0 == pstScan->u64PeriodUs)
            {
                //primary scan stop
                pstScan->pstCurrentPrimaryScanningPhy = NULL;
            }
            else
            {
                //next period
                u64NextPhyStartTimeUs = pstScan->u64DurationStartTimeUs + pstScan->u64PeriodUs;

                pstNextPhy->u64OriginalStartTimeUs = u64NextPhyStartTimeUs;
                pstNextPhy->u32EventStartTimeOffsetUs = 0;

                pstScan->u64DurationStartTimeUs = u64NextPhyStartTimeUs;
                pstScan->u64IntervalStartTimeUs = u64NextPhyStartTimeUs;

                pstScan->pstCurrentPrimaryScanningPhy = pstNextPhy;
            }
        }
        else
        {
            //next phy
            pstNextPhy->u64OriginalStartTimeUs = u64NextPhyStartTimeUs;
            pstNextPhy->u32EventStartTimeOffsetUs = 0;

            pstScan->pstCurrentPrimaryScanningPhy = pstNextPhy;
        }
    }
}


EN_LLP_ERR_T rom_llc_ext_scan_primary_scanning_config_llp(stLlcExtScan_t* pstScan, uint32_t u32MaxWorkWindowTimeUs)
{
    stLlcExtScanPrimaryScanningPhy_t* pstCurrentPhy = pstScan->pstCurrentPrimaryScanningPhy;

    stLlpExtScanFragmentParam_t stParam;
    stParam.u32WorkTimeUs = LLC_TOOL_MIN((pstCurrentPhy->u32ChannelScanWindowUs - pstCurrentPhy->u32EventStartTimeOffsetUs), u32MaxWorkWindowTimeUs);

#if LLC_EXT_SCAN_PRIMARY_SCANNING_WINDOW_ADJUSTABLE
    //adjustable start time
    uint64_t u64ExpectedStartTimeUs = pstCurrentPhy->u64OriginalStartTimeUs + pstCurrentPhy->u32EventStartTimeOffsetUs;
    uint64_t u64TimeNow = rom_llc_timer_get_count(true);
    if (u64TimeNow < u64ExpectedStartTimeUs)
    {
        stParam.u64StartTimeUs = u64ExpectedStartTimeUs;
    }
    else
    {
        if (u64TimeNow + LLC_EXT_SCAN_PRIMARY_SCANNING_MIN_WINDOW_US <= u64ExpectedStartTimeUs + stParam.u32WorkTimeUs)
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
    stParam.pstLlpTxPduPtr = &(pstScan->stTxPduPtr);
    stParam.pstLlpRxPduPtr = &(pstScan->stRxPduPtr);
    
    EN_LLP_ERR_T err = rom_llc_start_ext_scan_fragment(&stParam);
    MLOG1688I(0x3810, "EXT SCAN PRIMARY SCANNING config ch:%u phy:%u priority:%u", stParam.u8ChannelIdx, stParam.enumPhy, pstScan->stSchedulerEvent.u8Priority);
    return err;
}


void rom_llc_ext_scan_primary_scanning_on_llp_fragment_start(stLlcExtScan_t* pstScan)
{
    pstScan->pstReceivedExtendedPduWhenPrimaryScanning = NULL;
    pstScan->enumState = LLC_EXT_SCAN_STATE_RX_ADV;
}



bool rom_llc_ext_scan_primary_scanning_on_llp_fragment_stop(stLlcExtScan_t* pstScan)
{
    pstScan->enumState = LLC_EXT_SCAN_STATE_IDLE;

    if (NULL != pstScan->pstReceivedExtendedPduWhenPrimaryScanning)
    {
        rom_llc_ext_scan_secondary_scanning_on_received_extended_pdu_when_primary_scanning(pstScan, pstScan->pstReceivedExtendedPduWhenPrimaryScanning);
        rom_llc_ext_scan_free_pdu(pstScan, pstScan->pstReceivedExtendedPduWhenPrimaryScanning);
        pstScan->pstReceivedExtendedPduWhenPrimaryScanning = NULL;
    }
    
    if (NULL != pstScan->pstRecevingPdu)
    {
        rom_llc_ext_scan_free_pdu(pstScan, pstScan->pstRecevingPdu);
        pstScan->pstRecevingPdu = NULL;
    }

    rom_llc_ext_scan_state_machine_schedule_next_scanning(pstScan);
    return false;
}


EN_LLP_STATE_CMD_T rom_llc_ext_scan_primary_scanning_on_llp_tx_end(stLlcExtScan_t* pstScan, stLlpTxEndReport_t *pstReport)
{
    if (LLC_EXT_SCAN_STATE_TX_REQ != pstScan->enumState)
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_PRIMARY_SCANNING, __LINE__, (uint32_t)pstScan, pstScan->enumState);
        return LLP_STATE_CMD_STOP_FRAGMENT;
    }

    pstScan->enumState = LLC_EXT_SCAN_STATE_RX_RSP;

    return LLP_STATE_CMD_CONTINUE;
}


EN_LLP_STATE_CMD_T rom_llc_ext_scan_primary_scanning_on_llp_rx_adv_end(stLlcExtScan_t* pstScan, stLlpRxEndReport_t *pstReport, stLlcAdvChannelPdu_t* pstPdu, stLlcExtScanPduExtraInfo_t* pstExtraInfo)
{
    if (rom_llc_ext_scan_is_pdu_empty(pstScan, pstPdu))
    {
        //re-alloc pdu
        rom_llc_ext_scan_free_pdu(pstScan, pstPdu);
        rom_llc_ext_scan_primary_scanning_update_receving_pdu(pstScan, rom_llc_ext_scan_alloc_pdu(pstScan));

        return LLP_STATE_CMD_CANCEL_TX;
    }

    if (LLP_RX_RESULT_CRC_OK != pstReport->enumRxResult)
    {
        pstScan->enumState = LLC_EXT_SCAN_STATE_RX_ADV;
        return LLP_STATE_CMD_CANCEL_TX;
    }

    stLlcSpec51AdvChannelPduHeader_t stHeader;
    stHeader.u16Value = pstPdu->u32Header & 0xFFFF;

    if (stHeader.bitField.Length > pstPdu->u16PayloadBufSize)
    {
        //re-alloc pdu
        rom_llc_ext_scan_free_pdu(pstScan, pstPdu);
        rom_llc_ext_scan_primary_scanning_update_receving_pdu(pstScan, rom_llc_ext_scan_alloc_pdu(pstScan));

        return LLP_STATE_CMD_CANCEL_TX;
    }


    bool bScanReportCancelled = false;
    bool bScanRequestCancelled = false;
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

        case LLC_SPEC_5_1_ADV_CH_PDU_TYPE_ADV_SCAN_IND:
        {
            if (stHeader.bitField.Length < 6)
            {
                return LLP_STATE_CMD_CANCEL_TX;
            }
            pstExtraInfo->enumEventTypeForPrimaryScanning = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_LEGACY_EVENT_TYPE_ADV_SCAN_IND;
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
            bScanRequestCancelled = true;
            AdvA = &(pstPdu->pu8Payload[0]);
            TargetA = &(pstPdu->pu8Payload[6]);
        }
            break;

        case LLC_SPEC_5_1_ADV_CH_PDU_TYPE_NONCONN_IND:
        {
            if (stHeader.bitField.Length < 6)
            {
                return LLP_STATE_CMD_CANCEL_TX;
            }
            pstExtraInfo->enumEventTypeForPrimaryScanning = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_LEGACY_EVENT_TYPE_ADV_NONCONN_IND;
            bScanRequestCancelled = true;
            AdvA = &(pstPdu->pu8Payload[0]);
        }
            break;

        case LLC_SPEC_5_1_ADV_CH_PDU_TYPE_ADV_EXT_IND:
        {
            pstScan->pstReceivedExtendedPduWhenPrimaryScanning = pstPdu;
            rom_llc_ext_scan_primary_scanning_update_receving_pdu(pstScan, rom_llc_ext_scan_alloc_pdu(pstScan));
        }
            return LLP_STATE_CMD_STOP_FRAGMENT;

        default:
            return LLP_STATE_CMD_CANCEL_TX;
    }

    //update SCAN_REQ ASAP
    rom_llc_ext_scan_state_machine_update_scan_req_pdu_AdvA(pstScan, AdvA, stHeader.bitField.TxAdd);
    rom_llc_update_tx_pdu(&pstScan->stTxPduPtr);


    //Scan Type filter
    if (LLC_SPEC_5_1_HCI_SCAN_TYPE_PASSIVE == pstScan->pstCurrentPrimaryScanningPhy->Scan_Type)
    {
        bScanRequestCancelled = true;
    }

    //PRA and WhiteList filter
    if (rom_llc_ext_scan_check_RPA_and_white_list_disallowed(pstScan,
                                                             stHeader.bitField.TxAdd, AdvA,
                                                             stHeader.bitField.RxAdd, TargetA,
                                                             &(pstExtraInfo->enumAddressTypeForPrimaryScanning), pstExtraInfo->pu8AddressForPrimaryScanning,
                                                             &(pstExtraInfo->enumDirectAddressTypeForPrimaryScanning), pstExtraInfo->pu8DirectAddressForPrimaryScanning))
    {
        bScanRequestCancelled = true;
        bScanReportCancelled = true;
    }


    stLlcAdvChannelPdu_t* pstNewPdu = rom_llc_ext_scan_alloc_pdu(pstScan);
    if (bScanReportCancelled)
    {
        //reuse pdu, since pstNewPdu may be empty
        rom_llc_ext_scan_free_pdu(pstScan, pstNewPdu);
        pstNewPdu = pstPdu;
        //rom_llc_ext_scan_primary_scanning_update_receving_pdu(pstScan, pstNewPdu);
    }
    else
    {
        rom_llc_ext_scan_primary_scanning_add_pdu_to_report_list_tail(pstScan, pstPdu);
        rom_llc_ext_scan_primary_scanning_update_receving_pdu(pstScan, pstNewPdu);
        uint32_t u32Primask = __get_PRIMASK();
        __disable_irq();
        if (!pstScan->bPrimaryReporting)
        {
            pstScan->bPrimaryReporting = rom_llc_send_msg_to_llc_task(rom_llc_ext_scan_primary_scanning_report_task_process, pstScan);
        }
        __set_PRIMASK(u32Primask);
    }

    if (bScanRequestCancelled)
    {
        return LLP_STATE_CMD_CANCEL_TX;
    }
    else
    {
        if (rom_llc_ext_scan_is_pdu_empty(pstScan, pstNewPdu))
        {
            return LLP_STATE_CMD_CANCEL_TX;
        }

        if (rom_llc_ext_scan_state_machine_backoff_is_scan_canceled(pstScan))
        {
            return LLP_STATE_CMD_CANCEL_TX;
        }

        stLlcExtScanPduExtraInfo_t* pstRspPduExtraInfo = rom_llc_ext_scan_get_pdu_extra_info(pstNewPdu);
        switch(pstExtraInfo->enumEventTypeForPrimaryScanning)
        {
            case LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_LEGACY_EVENT_TYPE_ADV_IND:
                pstRspPduExtraInfo->enumEventTypeForPrimaryScanning = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_LEGACY_EVENT_TYPE_SCAN_RSP_TO_ADV_IND;
                break;

            case LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_LEGACY_EVENT_TYPE_ADV_SCAN_IND:
                pstRspPduExtraInfo->enumEventTypeForPrimaryScanning = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_LEGACY_EVENT_TYPE_SCAN_RSP_TO_ADV_SCAN_IND;
                break;

            default:
                ////unexpected situation
                system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_PRIMARY_SCANNING, __LINE__, (uint32_t)pstScan, pstExtraInfo->enumEventTypeForPrimaryScanning);
                break;
        }
        pstRspPduExtraInfo->enumAddressTypeForPrimaryScanning = pstExtraInfo->enumAddressTypeForPrimaryScanning;
        memcpy(pstRspPduExtraInfo->pu8AddressForPrimaryScanning, pstExtraInfo->pu8AddressForPrimaryScanning, 6);
        pstRspPduExtraInfo->enumDirectAddressTypeForPrimaryScanning = pstExtraInfo->enumDirectAddressTypeForPrimaryScanning;
        memcpy(pstRspPduExtraInfo->pu8DirectAddressForPrimaryScanning, pstExtraInfo->pu8DirectAddressForPrimaryScanning, 6);

        pstScan->enumState = LLC_EXT_SCAN_STATE_TX_REQ;
        return LLP_STATE_CMD_CONTINUE;
    }
}


EN_LLP_STATE_CMD_T rom_llc_ext_scan_primary_scanning_on_llp_rx_rsp_end(stLlcExtScan_t* pstScan, stLlpRxEndReport_t *pstReport, stLlcAdvChannelPdu_t* pstPdu, stLlcExtScanPduExtraInfo_t* pstExtraInfo)
{
    pstScan->enumState = LLC_EXT_SCAN_STATE_RX_ADV;

    if (rom_llc_ext_scan_is_pdu_empty(pstScan, pstPdu))
    {
        rom_llc_ext_scan_state_machine_backoff_scan_result(pstScan, false);

        //re-alloc pdu
        rom_llc_ext_scan_free_pdu(pstScan, pstPdu);
        rom_llc_ext_scan_primary_scanning_update_receving_pdu(pstScan, rom_llc_ext_scan_alloc_pdu(pstScan));

        return LLP_STATE_CMD_CONTINUE;
    }

    if (LLP_RX_RESULT_CRC_OK != pstReport->enumRxResult)
    {
        rom_llc_ext_scan_state_machine_backoff_scan_result(pstScan, false);
        return LLP_STATE_CMD_CONTINUE;
    }

    stLlcSpec51AdvChannelPduHeader_t stHeader;
    stHeader.u16Value = pstPdu->u32Header & 0xFFFF;
    
    if (stHeader.bitField.Length > pstPdu->u16PayloadBufSize)
    {
        rom_llc_ext_scan_state_machine_backoff_scan_result(pstScan, false);
        //re-alloc pdu
        rom_llc_ext_scan_free_pdu(pstScan, pstPdu);
        rom_llc_ext_scan_primary_scanning_update_receving_pdu(pstScan, rom_llc_ext_scan_alloc_pdu(pstScan));

        return LLP_STATE_CMD_CONTINUE;
    }

    if (LLC_SPEC_5_1_ADV_CH_PDU_TYPE_SCAN_RSP != stHeader.bitField.PDUType)
    {
        rom_llc_ext_scan_state_machine_backoff_scan_result(pstScan, false);
        return LLP_STATE_CMD_CONTINUE;
    }

    if (stHeader.bitField.Length < 6)
    {
        rom_llc_ext_scan_state_machine_backoff_scan_result(pstScan, false);
        return LLP_STATE_CMD_CONTINUE;
    }

    uint8_t* AdvA = &(pstPdu->pu8Payload[0]);
    EN_LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_T enumAddressType;
    uint8_t pu8Address[6];
    if (rom_llc_ext_scan_check_RPA_and_white_list_disallowed(pstScan,
                                                             stHeader.bitField.TxAdd, AdvA,
                                                             false, NULL,
                                                             &enumAddressType, pu8Address,
                                                             NULL, NULL))
    {
        rom_llc_ext_scan_state_machine_backoff_scan_result(pstScan, false);
        return LLP_STATE_CMD_CONTINUE;
    }

    if ((enumAddressType != pstExtraInfo->enumAddressTypeForPrimaryScanning)
        || (0 != memcmp(pu8Address, pstExtraInfo->pu8AddressForPrimaryScanning, 6)))
    {
        rom_llc_ext_scan_state_machine_backoff_scan_result(pstScan, false);
        return LLP_STATE_CMD_CONTINUE;
    }

    rom_llc_ext_scan_state_machine_backoff_scan_result(pstScan, true);

    rom_llc_ext_scan_primary_scanning_add_pdu_to_report_list_tail(pstScan, pstPdu);
    rom_llc_ext_scan_primary_scanning_update_receving_pdu(pstScan, rom_llc_ext_scan_alloc_pdu(pstScan));
    uint32_t u32Primask = __get_PRIMASK();
    __disable_irq();
    if (!pstScan->bPrimaryReporting)
    {
        pstScan->bPrimaryReporting = rom_llc_send_msg_to_llc_task(rom_llc_ext_scan_primary_scanning_report_task_process, pstScan);
    }
    __set_PRIMASK(u32Primask);
    
    return LLP_STATE_CMD_CONTINUE;
}


EN_LLP_STATE_CMD_T rom_llc_ext_scan_primary_scanning_on_llp_rx_end(stLlcExtScan_t* pstScan, stLlpRxEndReport_t *pstReport)
{
    stLlcExtScanPrimaryScanningPhy_t* pstPhy = pstScan->pstCurrentPrimaryScanningPhy;
    stLlcAdvChannelPdu_t* pstPdu = pstScan->pstRecevingPdu;
    stLlcExtScanPduExtraInfo_t* pstExtraInfo = rom_llc_ext_scan_get_pdu_extra_info(pstPdu);
    if (LLP_RX_RESULT_CRC_OK == pstReport->enumRxResult)
    {
        pstPdu->u32RssiRelatedValue = pstReport->u32RssiRelatedValue;
        if (NULL != pstExtraInfo)
        {
            pstExtraInfo->u64PduStartTimeUs = pstReport->u64RxPacketStartTime;
        }
    }
    if (NULL != pstExtraInfo)
    {
        pstExtraInfo->enumPhy = pstPhy->enumPhy;
    }

    EN_LLP_STATE_CMD_T enumStateCmd;
    if (LLC_EXT_SCAN_STATE_RX_ADV == pstScan->enumState)
    {
        enumStateCmd = rom_llc_ext_scan_primary_scanning_on_llp_rx_adv_end(pstScan, pstReport, pstPdu, pstExtraInfo);
    }
    else if (LLC_EXT_SCAN_STATE_RX_RSP == pstScan->enumState)
    {
        enumStateCmd = rom_llc_ext_scan_primary_scanning_on_llp_rx_rsp_end(pstScan, pstReport, pstPdu, pstExtraInfo);
    }
    else
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_PRIMARY_SCANNING, __LINE__, (uint32_t)pstScan, pstScan->enumState);
        enumStateCmd = LLP_STATE_CMD_STOP_FRAGMENT;
    }
    
    return enumStateCmd;
}


void rom_llc_ext_scan_primary_scanning_report_task_process(void* pParam)
{
    stLlcExtScan_t* pstScan = (stLlcExtScan_t*)pParam;

    uint32_t u32Primask = __get_PRIMASK();
    __disable_irq();
    pstScan->bPrimaryReporting = false;//flag to reduce duplicated msg sent
    __set_PRIMASK(u32Primask);

    /*
    Subevent_Code,                              Size: 1 octet
    Num_Reports,                                Size: 1 octet

    Event_Type[i],                              Size: 2 octets
    Address_Type[i],                            Size: 1 octet
    Address[i],                                 Size: 6 octets
    Primary_PHY[i],                             Size: 1 octet
    Secondary_PHY[i],                           Size: 1 octet
    Advertising_SID[i],                         Size: 1 octet
    Tx_Power[i],                                Size: 1 octet
    RSSI[i],                                    Size: 1 octet
    Periodic_Advertising_Interval[i],           Size: 2 octets
    Direct_Address_Type[i],                     Size: 1 octet
    Direct_Address[i],                          Size: 6 octets
    Data_Length[i],                             Size: 1 octet
    Data[i]                                     Size: (Data_Length) octets
    */

#define SINGLE_REPORT_PARAMETERS_SIZE_WITHOUT_DATA (2+1+6+1+1+1+1+1+2+1+6+1)

    uint8_t pu8Parameters[256];
    uint8_t u8ReportsNum = 0;

    uint16_t u16ParametersMaxLength = LLC_TOOL_MIN(sizeof(pu8Parameters), rom_hci_get_event_parameter_max_length());
    uint8_t u8ParametersCurrentLength = 2;
    uint8_t u8NextReportOffset = 2;

    if (u16ParametersMaxLength < u8ParametersCurrentLength)
    {
        ////unexpected situation
        system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_PRIMARY_SCANNING, __LINE__, rom_hci_get_event_parameter_max_length(), 0);
        return;
    }
    
    for(;;)
    {
        //peek pdu
        __disable_irq();
        stLlcAdvChannelPdu_t *pstPdu = pstScan->pstPrimaryScanningReportPduListHead;
        __set_PRIMASK(u32Primask);

        if(NULL == pstPdu)
        {
            break;
        }
        stLlcExtScanPduExtraInfo_t* pstExtraInfo = rom_llc_ext_scan_get_pdu_extra_info(pstPdu);
        if (NULL == pstExtraInfo)
        {
            //invalid PDU
            ////unexpected situation
            system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_PRIMARY_SCANNING, __LINE__, (uint32_t)pstScan, (uint32_t)pstPdu);
            continue;
        }

        //check
        stLlcSpec51AdvChannelPduHeader_t stHeader;
        stHeader.u16Value = pstPdu->u32Header & 0xFFFF;
        if (stHeader.bitField.Length < 6)
        {
            //invalid PDU
            ////unexpected situation
            system_error(SYS_ERR_MODULE_LLC_EXT_SCAN_PRIMARY_SCANNING, __LINE__, (uint32_t)pstPdu, (uint32_t)stHeader.u16Value);
            continue;
        }

        uint8_t u8DataLength;
        if (LLC_SPEC_5_1_ADV_CH_PDU_TYPE_DIRECT_IND == stHeader.bitField.PDUType)
        {
            u8DataLength = 0;
        }
        else
        {
            u8DataLength = stHeader.bitField.Length - 6;
        }

        uint16_t u16NeedSize = SINGLE_REPORT_PARAMETERS_SIZE_WITHOUT_DATA + u8DataLength;
        if (u8ParametersCurrentLength + u16NeedSize > u16ParametersMaxLength)
        {
            //cannot hold more device
            break;
        }

        if (LLC_SPEC_5_1_HCI_FILTER_DUPLICATES_DISABLED != pstScan->Filter_Duplicates)
        {
            if (rom_llc_ext_scan_duplicate_filter_is_report_duplicated(pstExtraInfo->enumAddressTypeForPrimaryScanning,
                                                                       pstExtraInfo->pu8AddressForPrimaryScanning,
                                                                       LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_SID_NO_ADI_PROVIDED))
            {
                rom_llc_ext_scan_primary_scanning_remove_and_free_pdu_from_report_list_head(pstScan);
                continue;
            }
            else
            {
                rom_llc_ext_scan_duplicate_filter_add_report(pstExtraInfo->enumAddressTypeForPrimaryScanning,
                                                             pstExtraInfo->pu8AddressForPrimaryScanning,
                                                             LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_SID_NO_ADI_PROVIDED);
            }
        }

        //Event_Type
        rom_llc_tool_write_little_endian_uint16(&pu8Parameters[u8NextReportOffset], pstExtraInfo->enumEventTypeForPrimaryScanning);

        //Address_Type
        pu8Parameters[u8NextReportOffset + 2] = pstExtraInfo->enumAddressTypeForPrimaryScanning;

        //Address
        memcpy(&pu8Parameters[u8NextReportOffset + 3], pstExtraInfo->pu8AddressForPrimaryScanning, 6);

        //Primary_PHY
        if (LL_PHY_1M == pstExtraInfo->enumPhy)
        {
            pu8Parameters[u8NextReportOffset + 9] = LLC_SPEC_5_1_HCI_PHY_LE_1M;
        }
        else if (LL_PHY_2M == pstExtraInfo->enumPhy)
        {
            pu8Parameters[u8NextReportOffset + 9] = LLC_SPEC_5_1_HCI_PHY_LE_2M;
        }
        else
        {
            pu8Parameters[u8NextReportOffset + 9] = LLC_SPEC_5_1_HCI_PHY_LE_CODED;
        }

        //Secondary_PHY
        pu8Parameters[u8NextReportOffset + 10] = LLC_SPEC_5_1_HCI_PHY_NONE;

        //Advertising_SID
        pu8Parameters[u8NextReportOffset + 11] = LLC_SPEC_5_1_HCI_EXTENDED_ADVERTISING_REPORT_SID_NO_ADI_PROVIDED;

        //Tx_Power
        pu8Parameters[u8NextReportOffset + 12] = LLC_SPEC_5_1_HCI_TX_POWER_IS_NOT_AVAILABLE;

        //RSSI
        pu8Parameters[u8NextReportOffset + 13] = (uint8_t)rom_llc_get_rssi(pstPdu->u32RssiRelatedValue);

        //Periodic_Advertising_Interval
        rom_llc_tool_write_little_endian_uint16(&pu8Parameters[u8NextReportOffset + 14], 0);

        //Direct_Address_Type
        pu8Parameters[u8NextReportOffset + 16] = pstExtraInfo->enumDirectAddressTypeForPrimaryScanning;

        //Direct_Address
        memcpy(&pu8Parameters[u8NextReportOffset + 17], pstExtraInfo->pu8DirectAddressForPrimaryScanning, 6);

        //Data_Length
        pu8Parameters[u8NextReportOffset + 23] = u8DataLength;

        //Data
        memcpy(&pu8Parameters[u8NextReportOffset + 24], &pstPdu->pu8Payload[6], u8DataLength);



        u8ReportsNum++;
        u8NextReportOffset += u16NeedSize;
        u8ParametersCurrentLength += u16NeedSize;

        rom_llc_ext_scan_primary_scanning_remove_and_free_pdu_from_report_list_head(pstScan);
    }

    if (0 == u8ReportsNum)
    {
        return;
    }

    //Subevent_Code
    pu8Parameters[0] = LLC_SPEC_5_1_HCI_SUBEVENT_CODE_HCI_LE_Extended_Advertising_Report;
    //Num_Reports
    pu8Parameters[1] = u8ReportsNum;

    rom_controller_event_HCI_LE_Extended_Advertising_Report(u8ParametersCurrentLength, pu8Parameters);
    
}

