
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "llc_llp_interface.h"

#include "llc_spec_5_1.h"

#include "llc_adv_channel_pdu.h"
#include "llc_scheduler.h"

#include "llc.h"

#include "llc_ext_scan.h"

#include "llc_ext_scan_duplicate_filter.h"


////this module is NOT Thread-Safed


stLlcExtScanDuplicateFilterReport_t* g_pstLlcExtScanDuplicateFilterReportMem = NULL;
uint8_t g_u8LlcExtScanDuplicateFilterReportNum = 0;
volatile uint64_t g_u64LlcExtScanDuplicateFilterVersionNumber = 0;

void rom_llc_ext_scan_duplicate_filter_init(stLlcExtScanDuplicateFilterReport_t* pstLlcExtScanDuplicateFilterReportMem, uint8_t u8LlcExtScanDuplicateFilterReportNum)
{
    if (NULL == pstLlcExtScanDuplicateFilterReportMem)
    {
        u8LlcExtScanDuplicateFilterReportNum = 0;
    }
    
    g_pstLlcExtScanDuplicateFilterReportMem = pstLlcExtScanDuplicateFilterReportMem;
    g_u8LlcExtScanDuplicateFilterReportNum = u8LlcExtScanDuplicateFilterReportNum;
    
    uint32_t i;
    for(i = 0; i < g_u8LlcExtScanDuplicateFilterReportNum; i++)
    {
        g_pstLlcExtScanDuplicateFilterReportMem[i].bMemUsed = false;
    }
    g_u64LlcExtScanDuplicateFilterVersionNumber = 0;
}


void rom_llc_ext_scan_duplicate_filter_reset(void)
{
    uint32_t i;
    for(i = 0; i < g_u8LlcExtScanDuplicateFilterReportNum; i++)
    {
        g_pstLlcExtScanDuplicateFilterReportMem[i].bMemUsed = false;
    }
    g_u64LlcExtScanDuplicateFilterVersionNumber = 0;
}


void rom_llc_ext_scan_duplicate_filter_add_report(EN_LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_T enumAddressType, uint8_t pu8Address[6], uint8_t u8SID)
{
    if (LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_ANONYMOUS == enumAddressType)
    {
        return;
    }
    
    stLlcExtScanDuplicateFilterReport_t* pstEarliestReport = NULL;
    stLlcExtScanDuplicateFilterReport_t* pstUnusedReport = NULL;
    uint64_t u64EarLiestReportVersionNumber = 0xFFFFFFFFFFFFFFFF;
    uint32_t i;
    for(i = 0; i < g_u8LlcExtScanDuplicateFilterReportNum; i++)
    {
        stLlcExtScanDuplicateFilterReport_t* pstReport = &g_pstLlcExtScanDuplicateFilterReportMem[i];
        if (pstReport->bMemUsed)
        {
            if ((enumAddressType == pstReport->enumAddressType)
                && (u8SID == pstReport->u8SID)
                && (0 == memcmp(pu8Address, pstReport->pu8Address, 6)))
            {
                //already added
                return;
            }
            else
            {
                if (pstReport->u64VersionNumer < u64EarLiestReportVersionNumber)
                {
                    pstEarliestReport = pstReport;
                    u64EarLiestReportVersionNumber = pstReport->u64VersionNumer;
                }
            }
        }
        else
        {
            if (NULL == pstUnusedReport)
            {
                pstUnusedReport = pstReport;
            }
        }
    }
    
    if (NULL != pstUnusedReport)
    {
        pstUnusedReport->bMemUsed = true;
        pstUnusedReport->enumAddressType = enumAddressType;
        memcpy(pstUnusedReport->pu8Address, pu8Address, 6);
        pstUnusedReport->u8SID = u8SID;
        
        pstUnusedReport->u64VersionNumer = g_u64LlcExtScanDuplicateFilterVersionNumber;
        g_u64LlcExtScanDuplicateFilterVersionNumber++;
        return;
    }
    
    if (NULL != pstEarliestReport)
    {
        pstEarliestReport->enumAddressType = enumAddressType;
        memcpy(pstEarliestReport->pu8Address, pu8Address, 6);
        pstEarliestReport->u8SID = u8SID;
        
        pstEarliestReport->u64VersionNumer = g_u64LlcExtScanDuplicateFilterVersionNumber;
        g_u64LlcExtScanDuplicateFilterVersionNumber++;
    }
}


bool rom_llc_ext_scan_duplicate_filter_is_report_duplicated(EN_LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_T enumAddressType, uint8_t pu8Address[6], uint8_t u8SID)
{
    if (LLC_SPEC_5_1_HCI_ADVERTISING_REPORT_ADDRESS_TYPE_ANONYMOUS == enumAddressType)
    {
        return false;
    }
    
    uint32_t i;
    for(i = 0; i < g_u8LlcExtScanDuplicateFilterReportNum; i++)
    {
        stLlcExtScanDuplicateFilterReport_t* pstReport = &g_pstLlcExtScanDuplicateFilterReportMem[i];
        if ((pstReport->bMemUsed)
            && (enumAddressType == pstReport->enumAddressType)
            && (u8SID == pstReport->u8SID)
            && (0 == memcmp(pu8Address, pstReport->pu8Address, 6)))
        {
            return true;
        }
    }
    
    return false;
}
