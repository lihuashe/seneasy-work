/**
 * @attention
 * 
 *  Copyright 2024 Guangdong Seneasy Intelligent Technology Co., Ltd. All rights reserved.
 * @file     ir_driver.h
 * 
 * @author   ÕÅ¿­»ª(zhangkaihua@seneasy.com)
 * 
 * @brief    
 * 
 * @date     2024-11-19
 * 
*/

#ifndef __IR_DRIVER_H
#define __IR_DRIVER_H

#include "SYSCFG.h"
#include "FT61F0AX.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    IR_STATE_IDLE = 0,
    IR_STATE_COMMAND = 1,
    IR_STATE_REPEAT = 2,
    IR_STATE_DELAY = 3,
}ir_state_t;

typedef enum {
    IR_CHANGE_IDLE = 0,
    IR_CHANGE_COMMAND = 1,
    IR_CHANGE_SUCCESS = 2,
}ir_change_key_t;

typedef enum {
    IR_CODE_NEC = 0,
    IR_CODE_UNIVERSAL = 1,
}ir_code_state_t;

typedef enum {
    IR_NEC_SEND_FINISH = 1,
    IR_UNIVERSAL_SEND_FINISH = 2,
}ir_send_state_t;

typedef struct {
    uint16_t h;
    uint16_t l;
}ir_waveform_spec_t;

typedef struct 
{
    // ir_waveform_spec_t waveform_spec[8];
    uint16_t waveform_spec[16];

    uint8_t command[8];
    uint8_t command_idx;
    uint8_t command_len;

    uint8_t data[8];
    uint8_t data_idx;
    uint8_t data_len;

    uint8_t repeat[4];
    uint8_t repeat_idx;
    uint8_t repeat_len;

    uint16_t carry_frequcy_hz;

    bool pending_stop;
    uint8_t state; // 0-command; 1-data; 2-repeat
    uint8_t code;
}ir_data_t;

typedef struct 
{
    uint8_t flags;
    
    uint8_t dict_len;
    uint16_t dict[15];

    uint8_t command[256];
    uint8_t command_idx;
    uint8_t command_len;

    uint16_t pulse[28];
    uint8_t pulse_idx;

    uint8_t frames;
    uint16_t delay;
    uint8_t state; // 0-command; 1-data; 2-repeat
}ir_waveform_t;


extern ir_data_t ir_data;
extern ir_waveform_t ir_wave;
extern ir_code_state_t ir_code;
extern ir_change_key_t ir_change_key;
extern ir_send_state_t ir_send_state;

void tim1_isr();

#define IS_TIM1_INT() ((T1UIF) || (T1CC1IF) )

void ir_config_carrier(uint16_t frequence);

void ir_config_timer(uint16_t clk_khz);

void ir_start_send();

void ir_stop_send();

#endif
