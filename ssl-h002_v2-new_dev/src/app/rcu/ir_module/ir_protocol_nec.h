/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      ir_nec_protocol.h
* @brief
* @details
* @author    barry_bian
* @date      2020-02-26
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef _IR_NEC_PROTOCOL_H_
#define _IR_NEC_PROTOCOL_H_

#include "stdint.h"
#include "ir_send_driver.h"

#define IR_NEC_DECODE                   0

#define NEC_SEND_FREQUENCY              38000 /*38KHz*/
#define NEC_ADDRESS                     0x81
#define NEC_CODE_LEN                    4
#define NEC_MAX_HEADDER_LEN             16
#define NEC_MAX_LOG_WAVFORM_SIZE        2
#define NEC_MAX_CODE_SIZE               12
#define NEC_MAX_REPETA_CODE_SIZE        3
#define NEC_IR_COMMAND_PERIOD           108 /*ms*/
#define NEC_IR_REPEAT_CODE_PERIOD       108 /*ms*/

typedef struct
{
    uint16_t header_len;
    uint32_t header_context[NEC_MAX_HEADDER_LEN];
    uint32_t log0_context[NEC_MAX_LOG_WAVFORM_SIZE];
    uint32_t log1_context[NEC_MAX_LOG_WAVFORM_SIZE];
    uint32_t stop_context;
    uint32_t dif_base;
    uint32_t dif_divisor;
} T_IR_NEC_SPEC;

typedef struct
{
    uint32_t repeat_code_len;
    uint32_t repeat_code_buf[NEC_MAX_REPETA_CODE_SIZE];
} T_IR_NEC_REPEAT_CODE_SPEC;

typedef struct
{
    float carrier_frequency_hz;
    uint8_t code[NEC_MAX_CODE_SIZE];
    uint8_t code_len;
    uint16_t buf_len;
    uint32_t *p_buf;
} T_IR_NEC_BUF;

T_IRDA_RET ir_protocol_nec_command_encode(uint8_t ir_key_command,
                                          T_IR_SEND_PARA *p_ir_send_parameters);
T_IRDA_RET ir_protocol_nec_repeat_code_encode(T_IR_SEND_PARA *p_ir_send_parameters);

#endif
