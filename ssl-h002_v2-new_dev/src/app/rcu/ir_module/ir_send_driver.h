/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file         ir_send_driver.h
* @brief
* @details
* @author       barry_bian
* @date         2020-04-08
* @version      v1.0
* *********************************************************************************************************
*/

#ifndef __IR_SEND_DRIVER_H_
#define __IR_SEND_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"
#include "stdint.h"
#include "stdbool.h"

/*============================================================================*
 *                          IR Send config
 *============================================================================*/
#define IR_TX_GDMA_CHANNEL              GDMA_Channel1
#define IR_TX_GDMA_Channel_num          1
#define IR_TX_GDMA_Channel_IRQn         GDMA0_Channel1_IRQn
#define ir_tx_gdma_handler              GDMA0_Channel1_Handler

#define IR_SEND_WAVE_MAX_LEN            70
#define IR_REPEAT_CODE_MAX_LEN          3

#define IR_DUTY_CYCLE                   3

#define PULSE_HIGH                      ((uint32_t)0x80000000)
#define PULSE_LOW                       0x0
#define LOG_HIGH                        1
#define LOG_LOW                         0
#define IR_DATA_MSK                     0x7fffffffUL

#define IR_TX_FIFO_THR_LEVEL            2

typedef enum
{
    IR_SEND_KEY_PRESS,
    IR_SEND_KEY_RELEASE,
} T_IR_SEND_KEY_STATE;

typedef enum
{
    IR_SEND_IDLE,
    IR_SEND_CAMMAND,
    IR_SEND_CAMMAND_COMPLETE,
    IR_SEND_REPEAT_CODE,
    IR_SEND_REPEAT_CODE_COMPLETE,
} T_IR_SEND_STATE;

typedef enum
{
    IRDA_ERROR        = -1,
    IRDA_SUCCEED      = 0,
    IRDA_HEADER_ERROR = 1,
    IRDA_DATA_ERROR
} T_IRDA_RET;

typedef struct
{
    float    carrier_frequency_hz;
    float    duty_cycle;
    uint32_t ir_send_buf[IR_SEND_WAVE_MAX_LEN];
    uint32_t send_buf_len;
    uint32_t ir_repeat_code_buf[IR_REPEAT_CODE_MAX_LEN];
    uint32_t repeat_buf_len;
    uint32_t command_time_ms;
    uint32_t repeat_code_time_ms;
} T_IR_SEND_PARA;

typedef struct
{
    T_IR_SEND_STATE  ir_send_state;             /*ir send state*/
    T_IR_SEND_PARA   *p_ir_send_data;
} T_IR_SEND_STRUCT;



void ir_send_pad_init(void);
void ir_send_pinmux_init(void);
bool ir_send_check_dlps(void);
bool ir_send_is_working(void);
void ir_send_enter_dlps_config(void);
void ir_send_exit_dlps_config(void);
T_IR_SEND_STATE ir_send_get_current_state(void);
bool ir_send_module_init(T_IR_SEND_PARA *ir_send_para);
bool ir_send_command_start(void);
bool ir_send_repeat_code_start(void);
void ir_send_exit(void);
uint32_t ir_send_convert_to_carrier_cycle(uint32_t time, uint32_t freq);

#ifdef __cplusplus
}
#endif

#endif /*__IR_APP_CONFIG_H_*/


/******************* (C) COPYRIGHT 2017 Realtek Semiconductor Corporation *****END OF FILE****/

