
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "RegMXD2670.h"

#include "llc_llp_interface.h"
#include "llc_spec_5_1.h"

#include "llp_hw.h"
#include "llp.h"
#include "llp_slave.h"
#include "patch_llp_slave.h"

#include "rom_callback.h"


typedef enum
{
    LLP_SLAVE_SUB_STATE_IDLE = 0,
    LLP_SLAVE_SUB_STATE_RX,
    LLP_SLAVE_SUB_STATE_TX,
}
EN_LLP_SLAVE_SUB_STATE_T;

extern volatile EN_LLP_SLAVE_SUB_STATE_T g_llpSlaveSubState;


EN_LLP_ERR_T patch_llp_slave_fragment_start(stLlpSlaveFragmentParam_t* pstParam)
{
    if(NULL == pstParam)
    {
        return LLP_ERR_PARAM;
    }
    
    EN_LLP_ERR_T err;
    
    err = rom_llp_hw_set_work_time_us(LLP_FRAGMENT_TYPE_SLAVE, pstParam->u32WorkTimeUs);
    if(LLP_ERR_NONE != err)
    {
        return err;
    }
    
    err = rom_llp_hw_set_ifs(pstParam->enumTxPhy, pstParam->enumRxPhy, pstParam->u16IfsUs, pstParam->u16IfsRxWindowUs);
    if(LLP_ERR_NONE != err)
    {
        return err;
    }
    
    err = rom_llp_hw_set_slave_rx_window(pstParam->u32RxWindowUs, pstParam->enumRxPhy);
    if(LLP_ERR_NONE != err)
    {
        return err;
    }
    
    //set channel,accesscode,crcinit
    err = rom_llp_hw_set_trx_param(false, pstParam->u8ChannelIdx, pstParam->u32AccessAddress, pstParam->u32CrcInit, 
                              true, pstParam->enumTxPhy, pstParam->enumRxPhy);
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
    err = rom_llp_hw_set_rtr_mode_start_time(pstParam->u64StartTimeUs, pstParam->enumRxPhy);
    if(LLP_ERR_NONE != err)
    {
        return err;
    }
    //set mode
    g_llpSlaveSubState = LLP_SLAVE_SUB_STATE_IDLE;
    rom_llp_hw_set_slave_mode();
    return LLP_ERR_NONE;
}
