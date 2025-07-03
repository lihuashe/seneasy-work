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

#ifndef __IRRX_H
#define __IRRX_H

#ifdef  __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */
#include "xh98xx.h"
#include "peripheral.h"
 


typedef enum
{
    IRRX_TEST_CASE_420000 = 420000,
	IRRX_TEST_CASE_420001 = 420001,
    IRRX_TEST_CASE_420002 = 420002,
    IRRX_TEST_CASE_NOT_SUPPORT
}irrx_test_case_t;

typedef enum
{
    RX_CHANNEL_1 = 0,
    RX_CHANNEL_2,
    RX_CHANNEL_3,
	RX_CHANNEL_4,
	RX_CHANNEL_MAX,
} rx_channel_sel_t;

typedef enum
{
    RX_SENSE_4UA = 0,
    RX_SENSE_8UA,
    RX_SENSE_16UA,
	RX_SENSE_32UA,
	RX_SENSE_MAX,
} rx_current_sense_t;

typedef struct 
{
    rx_channel_sel_t    rx_channel;  
    rx_current_sense_t  rx_current_sense;
} ir_rx_config_t;

typedef struct 
{
		uint8_t         channel;
		cap_mode_t      cap_mode;
		uint32_t        psc;
		tim_callback_t  callback;
}ir_timer_config_t;

void ir_rx_config(ir_rx_config_t);

void ir_rx_start(void);

void ir_rx_stop(void);




#ifdef __cplusplus
}
#endif

#endif  /* __IRTX_H */
