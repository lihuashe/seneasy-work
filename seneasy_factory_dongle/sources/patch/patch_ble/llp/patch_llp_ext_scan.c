
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "RegMXD2670.h"

#include "llc_llp_interface.h"

#include "llp_hw.h"
#include "llp.h"
#include "llp_ext_scan.h"
#include "patch_llp_ext_scan.h"

#include "mlog.h"
#include "rom_callback.h"

typedef enum
{
    LLP_EXT_SCAN_SUB_STATE_IDLE = 0,
    LLP_EXT_SCAN_SUB_STATE_CONTINUE_RX,
    LLP_EXT_SCAN_SUB_STATE_TX,
    LLP_EXT_SCAN_SUB_STATE_RX,
}
EN_LLP_EXT_SCAN_SUB_STATE_T;

extern volatile EN_LLP_EXT_SCAN_SUB_STATE_T g_llpExtScanSubState;

EN_LLP_ERR_T patch_llp_ext_scan_fragment_start(stLlpExtScanFragmentParam_t* pstParam)
{
    
    if(NULL == pstParam)
    {
        return LLP_ERR_PARAM;
    }
    
    EN_LLP_ERR_T err;
    
    err = rom_llp_hw_set_work_time_us(LLP_FRAGMENT_TYPE_EXT_SCAN, pstParam->u32WorkTimeUs);
    if(LLP_ERR_NONE != err)
    {
        return err;
    }
    
    if (pstParam->u32WorkTimeUs == pstParam->u32AnchorWindowUs)
    {
        rom_llp_hw_set_scan_init_anchor_window_us(0);
    }
    else
    {
        rom_llp_hw_set_scan_init_anchor_window_us(pstParam->u32AnchorWindowUs);
    }
    
    err = rom_llp_hw_set_ifs(pstParam->enumPhy, pstParam->enumPhy, pstParam->u16IfsUs, pstParam->u16IfsRxWindowUs);
    if(LLP_ERR_NONE != err)
    {
        return err;
    }
    
    //set channel,accesscode,crcinit
    err = rom_llp_hw_set_trx_param(pstParam->bIsSecondaryChannel, pstParam->u8ChannelIdx, pstParam->u32AccessAddress, pstParam->u32CrcInit, 
                              true, pstParam->enumPhy, pstParam->enumPhy);
    if(LLP_ERR_NONE != err)
    {
        return err;
    }
   
    //set trx pdu
    err = rom_llp_hw_set_tx_pdu(pstParam->pstLlpTxPduPtr);
    if(LLP_ERR_NONE != err)
    {
        return err;
    }
    err = rom_llp_hw_set_rx_pdu(pstParam->pstLlpRxPduPtr, pstParam->u16RxPayloadSizeLimit);
    if(LLP_ERR_NONE != err)
    {
        return err;
    }
    
    //set time
    err = rom_llp_hw_set_rtr_mode_start_time(pstParam->u64StartTimeUs, pstParam->enumPhy);
    if(LLP_ERR_NONE != err)
    {
        return err;
    }
    //set mode
    g_llpExtScanSubState = LLP_EXT_SCAN_SUB_STATE_IDLE;
    rom_llp_hw_set_scan_mode();
    
    return LLP_ERR_NONE;
}
