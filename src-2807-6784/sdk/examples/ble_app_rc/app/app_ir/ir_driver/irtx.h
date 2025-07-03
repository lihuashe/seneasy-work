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
 * @file     irtx.h
 * @brief    IR TX driver
 * @date     13 May 2022
 * @author   AE Group
 *
 * @defgroup DOC DOC
 * @ingroup  DOCUMENT
 * @brief    templete
 * @details  irtx driver

 * @version:
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __IRTX_H
#define __IRTX_H

#ifdef  __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */
#include "xh98xx.h"
#include "peripheral.h"


/*******************************************************************************
 * MACROS
 */
#define PWM_PNUM_INTR_MASK          0x1
#define PWM_DMA_FIFO_INTR_MASK      0x2
#define PWM_FRAME_INTR_MASK         0x4

#define FIFO_CNT_INTR_MASK          0x1
#define FIFO_EMPTY_INTR_MASK        0x2

/*******************************************************************************
 * TYPEDEFS
 */
typedef void (*intr_callback_t)(void);
void irtx_test_case(uint8_t* pCmd);
typedef enum
{
    IRTX_TEST_CASE_220000 = 220000,
    IRTX_TEST_CASE_220001 = 220001,
    IRTX_TEST_CASE_220002 = 220002,
    IRTX_TEST_CASE_220100 = 220100,
    IRTX_TEST_CASE_220101 = 220101,
    IRTX_TEST_CASE_220102 = 220102,
    IRTX_TEST_CASE_220200 = 220200,
    IRTX_TEST_CASE_220201 = 220201,
    IRTX_TEST_CASE_NOT_SUPPORT
}irtx_test_case_t;

typedef enum
{
    NONE,
    FIFO_EMPTY_INTR,
    FIFO_CNT_INTR,
    PWM_PNUM_INTR,
    PWM_DMA_FIFO_INTR,
    PWM_FRAME_INTR
    
} intr_oprate_t;


typedef enum
{
    NORMAL   = 0,
    FIFO     = 1,
    DMA_FIFO = 3,
} ir_tx_mode_t;

typedef enum
{
    POL_HIGH   = 0x0,
    POL_LOW    = 0x1,
} ir_tx_polarity_t;

typedef enum
{
    INV_DISABLE   = 0x0,
    INV_ENABLE    = 0x1,
} ir_tx_invert_t;

typedef struct {
    ir_tx_mode_t        mode;
    
    uint32_t            carrier_freq0;
    double              duty_cycle0;
    uint32_t            carrier_freq1;
    double              duty_cycle1;
    
    ir_tx_polarity_t    polarity;
    ir_tx_invert_t      invert;
    
    uint32_t*           data_src;
    uint32_t            data_num;
    
    uint32_t            pnum;
    
    uint32_t            fifo_trigger_lvl;
    
    bool                bUse_intr;
    uint32_t            pwm_intr_mask;
    uint32_t            fifo_cnt_intr_mask;
    intr_callback_t     callback[5];
} ir_tx_config_t;




typedef struct
{
    ir_tx_mode_t        mode;
    uint32_t            data_num;
    uint32_t*           src;
    uint32_t            pwm_intr_mask;
    uint32_t            fifo_cnt_intr_mask;
    intr_callback_t     callback[5];
}irtx_env_t;


void ir_tx_config(ir_tx_config_t);

void ir_tx_start(void);

void ir_tx_stop(void);

void ir_tx_set_src(unsigned int *src,unsigned int len);


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * 
 * @brief EFUSE initialization
 *
 *******************************************************************************
 */

/** @} */

#ifdef __cplusplus
}
#endif

#endif  /* __IRTX_H */
