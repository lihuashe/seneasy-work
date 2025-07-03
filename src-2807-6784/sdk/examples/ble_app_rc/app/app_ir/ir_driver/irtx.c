/* ----------------------------------------------------------------------------
 * Copyright (c) 2020-2030 XIHAO Limited. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of XhMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * -------------------------------------------------------------------------- */

/**
 * @file     irtx.c
 * @brief    IR TX driver
 * @date     26 January 2022
 * @author   XIHAO AE Team
 *
 * @version:
 * Version 1.0
 *  - Initial release
 *
 * @{
 */


/*******************************************************************************
 * INCLUDES
 */
#include "irtx.h"
#include "peripheral.h"
#include "pinmux.h"
//#include "debug.h"


/*******************************************************************************
 * MACROS
 */


/*******************************************************************************
 * TYPEDEFS
 */




/*******************************************************************************
 * CONST & VARIABLES
 */
static irtx_env_t irtx_env;

/*******************************************************************************
 * LOCAL FUNCTIONS
 */

/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
/***************************************************************
  Function:     ir_tx_open
  Description:  open ir tx according the config parameters
  Input:        
                XH_IR_TX_Type *irtx
                    irtx hardware
                ir_tx_cfg_t cfg
                    irtx cfg parameters
  Output:       
                none
  Return:       void
  Author~{#:~}      ttl    2021-01-29   first version
***************************************************************/
void IR_TX_IRQHandler(void)
{    
    XH_IR_TX->PWM_INT_ST        = 0x7;//清除中断状态
    XH_IR_TX->FIFO_CNT_INT_ST   = 0x3;//清除中断状态
    
    if(NULL != irtx_env.callback[0] && (irtx_env.pwm_intr_mask&0x1) ) {
        irtx_env.callback[0]();
    }
    
    if(NULL != irtx_env.callback[1] && (irtx_env.pwm_intr_mask&0x2)) {
        irtx_env.callback[1]();
    }
    
    if(NULL != irtx_env.callback[2] && (irtx_env.pwm_intr_mask&0x4)) {
        irtx_env.callback[2]();
    }
    
    if(NULL != irtx_env.callback[3] && (irtx_env.fifo_cnt_intr_mask&0x1)) {
        if (((XH_IR_TX->FIFO_SR)&0x10) == 0x10)
        XH_IR_TX->FIFO_CNT_INT_MASK = 0;  //关闭fifo计数中断，否则中断会不停到来
        irtx_env.callback[3]();
    }
    
    if(NULL != irtx_env.callback[4] && (irtx_env.fifo_cnt_intr_mask&0x2)) {
        irtx_env.callback[4]();
    }
    
#if 0    
    //check the bit int of status register
    uint32_t stPwmIntSt = XH_IR_TX->PWM_INT_ST;
    uint32_t stFifoCntIntSt = XH_IR_TX->FIFO_CNT_INT_ST;

    stPwmIntSt &= ( IR_TX_PWM_INT_CFG_FRAME_INT_MASK | IR_TX_PWM_INT_CFG_PNUM_INT_MASK | IR_TX_PWM_INT_CFG_IR_DMA_FIFO_INT_MASK);
    if( stPwmIntSt )
    {
        if( stPwmIntSt &= ( IR_TX_PWM_INT_CFG_FRAME_INT_MASK))
        {
            //clear the int
            XH_IR_TX->PWM_INT_ST |= IR_TX_PWM_INT_CFG_FRAME_INT_MASK;

            //call the callback function
            irtx_env.cbFrame();
        }
        if( stPwmIntSt &= ( IR_TX_PWM_INT_CFG_PNUM_INT_MASK))
        {
            //clear the int
            XH_IR_TX->PWM_INT_ST |= IR_TX_PWM_INT_CFG_PNUM_INT_MASK;
        
            //call the callback function
            irtx_env.cbPnum();
        }
        if( stPwmIntSt &= ( IR_TX_PWM_INT_CFG_IR_DMA_FIFO_INT_MASK))
        {
            //clear the int
            XH_IR_TX->PWM_INT_ST |= IR_TX_PWM_INT_CFG_IR_DMA_FIFO_INT_MASK;
        
            //call the callback function
            irtx_env.cbDma();
        }
    }

    stFifoCntIntSt &= ( IR_TX_FIFO_CNT_INT_CFG_FIFO_INT_MASK | IR_TX_FIFO_CNT_INT_CFG_EMPTY_INT_MASK );
    if( stFifoCntIntSt )
    {
        if( stFifoCntIntSt &= ( IR_TX_FIFO_CNT_INT_CFG_FIFO_INT_MASK))
        {
            //clear the int
            XH_IR_TX->FIFO_CNT_INT_ST |= IR_TX_PWM_INT_CFG_FRAME_INT_MASK;

            //call the callback function
            irtx_env.cbFifoCnt();
        }
        if( stFifoCntIntSt &= ( IR_TX_FIFO_CNT_INT_CFG_EMPTY_INT_MASK))
        {
            //clear the int
            XH_IR_TX->FIFO_CNT_INT_ST |= IR_TX_FIFO_CNT_INT_CFG_EMPTY_INT_MASK;
        
            //call the callback function
            irtx_env.cbFifoEmpty();
        }
    }
#endif
}


void ir_tx_set_src(unsigned int *src,unsigned int len)
{
	irtx_env.data_num = len;
    irtx_env.src = src;
}


/***************************************************************
  Function:     ir_tx_config
  Description:  config ir tx according the config parameters
  Input:        
                ir_tx_config_t config
  Output:       
                none
  Return:       void
  Author~{#:~}      yangxs    2022-5-13
***************************************************************/
void ir_tx_config(ir_tx_config_t config)
{
    //入参检查
    co_assert(0 != config.carrier_freq0 && 0 != config.duty_cycle0);
    if (NORMAL != config.mode) {
        co_assert(0 != config.carrier_freq1 && 0 != config.duty_cycle1);
    }
    
    //设置时钟
    //Soft reset irtx
    register_set1(&XH_PSO->IRTX_CFG, CPM_IRTX_SOFT_RESET_MASK);
    //Open the irtx clk
    register_set0(&XH_PSO->IRTX_CFG, CPM_IRTX_GATE_EN_MASK);
    //Wait the irtx clk ok
    XH_PSO_UPD_RDY();
    
    //设置环境变量
    irtx_env.mode = config.mode;
    irtx_env.data_num = config.data_num;
    irtx_env.src = config.data_src;
    memcpy(irtx_env.callback,config.callback,sizeof(config.callback));
    irtx_env.pwm_intr_mask = config.pwm_intr_mask;
    irtx_env.fifo_cnt_intr_mask = config.fifo_cnt_intr_mask;
    
    //设置模式
    XH_IR_TX->PWM_MODE = config.mode;
    //XH_IR_TX->PWM_EN = 0x0;
    
    //设置极性
    XH_IR_TX->PWM_POLARITY = config.polarity;
    
    //设置PWM反转
    XH_IR_TX->PWM_INV = config.invert;
    
    //IR normal mode设置pnum与中断
    if (NORMAL == config.mode) {
        XH_IR_TX->PWM_PNUM = config.pnum;
    }
    
    //配置载波0频率以及占空比
    uint32_t clkIrtx = cpm_get_clock(CPM_IRTX_CLK);
    
    //config carrier0
    uint32_t psc = clkIrtx/config.carrier_freq0;
    XH_IR_TX->PWM_TMAX = psc;
    XH_IR_TX->PWM_TCMP = (uint32_t)psc * config.duty_cycle0;
    
    //配置载波1频率以及占空比
    //config carrier1
    if (NORMAL != config.mode) {
        psc = clkIrtx/config.carrier_freq1;
        XH_IR_TX->PWM_TMAX_SHADOW = psc;
        XH_IR_TX->PWM_TCMP_SHADOW = (uint32_t)psc * config.duty_cycle1;
    }
    
    //DMA FIFO mode设置源以及长度
    if (DMA_FIFO == config.mode) {
        XH_IR_TX->DMA_SADDR = (uint32_t)config.data_src;
        XH_IR_TX->DMA_TRANS_LENGTH = config.data_num * 4;
    }
    
    //配置中断
    if (config.bUse_intr) {
        NVIC_ClearPendingIRQ(IR_TX_IRQn);
        NVIC_SetPriority(IR_TX_IRQn, IRQ_PRIORITY_NORMAL);
        NVIC_EnableIRQ(IR_TX_IRQn);
        
        if (NORMAL == config.mode) {
            XH_IR_TX->PWM_INT_MASK = config.pwm_intr_mask & 0x5;
        } else {
            XH_IR_TX->FIFO_NUM_LVL = config.fifo_trigger_lvl;
            XH_IR_TX->PWM_INT_MASK = config.pwm_intr_mask;
            XH_IR_TX->FIFO_CNT_INT_MASK = config.fifo_cnt_intr_mask;
        }
    }
    
    //将ir tx power down写1，变成power on
    register_set1(&XH_IR_TX->ANA_IF, IR_TX_ANA_IF_POWER_DOWN_MASK);
    
#if 0    
    switch(config.mode) {
        case NORMAL:
            
            break;
        
        case FIFO:
            
            break;
        
        case DMA_FIFO:
            
            break;
        
        default:
            return;
    }
#endif
    
}

void ir_tx_start(void)
{
    if (NORMAL == irtx_env.mode) {
        XH_IR_TX->PWM_EN = 0x1;
        return;
    } else if (FIFO == irtx_env.mode) {
        // XH_IR_TX->PWM_EN = 0x1;
        for (uint32_t i = 0; i < irtx_env.data_num; i++) {
            XH_IR_TX->FIFO_DATA_ENTRY = irtx_env.src[i];
            while( (XH_IR_TX->FIFO_SR & 0x20) == 0x20 ){
                if (XH_IR_TX->PWM_EN != 0x01)
                XH_IR_TX->PWM_EN = 0x1;
                };
        }
		
		if (XH_IR_TX->PWM_EN != 0x01)
		{
			XH_IR_TX->PWM_EN = 0x01;
		}
		
        return;
    } else if (DMA_FIFO == irtx_env.mode) {
        XH_IR_TX->DMA_EN = 0x1;
        uint32_t read_data = XH_IR_TX->FIFO_SR;
          while (((read_data & 0x10) >> 4) != 0x1) {
            read_data = XH_IR_TX->FIFO_SR;
        }
        return;
    }
}

void ir_tx_stop(void)
{
    XH_IR_TX->PWM_EN = 0x0;
    XH_IR_TX->DMA_EN = 0x0;
}



/** @} */

