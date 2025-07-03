/* ----------------------------------------------------------------------------
 * Copyright (c) 2020-2030 XiHao Limited. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of XiHao nor the names of its contributors
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
 * @file     irrx.c
 * @brief    IR RX driver
 * @date     26 January 2022
 * @author   XiHao AE Team
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
#include "irrx.h"
#include "peripheral.h"
#include "pinmux.h"


/*******************************************************************************
 * MACROS
 */


/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * CONST & VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */

/*******************************************************************************
 * PUBLIC FUNCTIONS
 */

void ir_rx_config(ir_rx_config_t config)
{	
    REGW(&XH_PSO->IRTX_CFG, MASK_1REG(CPM_IRTX_SOFT_RESET,1));
    REGW(&XH_PSO->IRTX_CFG, MASK_1REG(CPM_IRTX_GATE_EN,0));
    XH_PSO_UPD_RDY();
    
    CPM_ANA_CLK_ENABLE();
    
    /* RX input channel select signal */
    REGW(&XH_DAIF->IR_RX_CFG,MASK_1REG(DAIF_IRRX_CH_SEL, 0));

    /* RX current sensitivity control */
    REGW(&XH_DAIF->IR_RX_CFG,MASK_1REG(DAIF_IR_SENSE, 0));
    
    CPM_ANA_CLK_RESTORE();	
}

void ir_rx_start(void)
{
    CPM_ANA_CLK_ENABLE();
    
    /* RX power down control signal */
    REGW(&XH_DAIF->IR_RX_CFG,MASK_1REG(DAIF_IR_PDB_RX, 1));
    
    /* RX input enable signal */
    REGW(&XH_DAIF->IR_RX_CFG,MASK_1REG(DAIF_IRRX_EN, 1));
    
    CPM_ANA_CLK_RESTORE();	
}

void ir_rx_stop(void)
{
    CPM_ANA_CLK_ENABLE();
    
    /* RX power down control signal */
    REGW(&XH_DAIF->IR_RX_CFG,MASK_1REG(DAIF_IR_PDB_RX, 0));
    
    /* RX input enable signal */
    REGW(&XH_DAIF->IR_RX_CFG,MASK_1REG(DAIF_IRRX_EN, 0));
    
    CPM_ANA_CLK_RESTORE();	
}



/** @} */

