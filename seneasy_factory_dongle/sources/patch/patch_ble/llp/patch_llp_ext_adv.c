
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "RegMXD2670.h"

#include "llc_llp_interface.h"
#include "llc_spec_5_1.h"

#include "llp_hw.h"
#include "llp.h"
#include "llp_ext_adv.h"
#include "patch_llp_ext_adv.h"

#include "rom_callback.h"

typedef enum
{
    LLP_ADV_SUB_STATE_IDLE = 0,
    LLP_ADV_SUB_STATE_TX1,
    LLP_ADV_SUB_STATE_RX,
    LLP_ADV_SUB_STATE_TX2,
}
EN_LLP_ADV_SUB_STATE_T;

extern volatile EN_LLP_ADV_SUB_STATE_T g_llpExtAdvSubState;


EN_LLP_ERR_T patch_llp_ext_adv_fragment_start(stLlpExtAdvFragmentParam_t* pstParam)
{
    
    if(NULL == pstParam)
    {
        return LLP_ERR_PARAM;
    }
    
    EN_LLP_ERR_T err;
    
    
    err = rom_llp_hw_set_work_time_us(LLP_FRAGMENT_TYPE_EXT_ADV, pstParam->u32WorkTimeUs);
    if(LLP_ERR_NONE != err)
    {
        return err;
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
    err = rom_llp_hw_set_trt_mode_start_time(pstParam->u64StartTimeUs, pstParam->enumPhy);
    if(LLP_ERR_NONE != err)
    {
        return err;
    }
    //set mode
    g_llpExtAdvSubState = LLP_ADV_SUB_STATE_IDLE;
    rom_llp_hw_set_adv_mode();
    return LLP_ERR_NONE;
}
