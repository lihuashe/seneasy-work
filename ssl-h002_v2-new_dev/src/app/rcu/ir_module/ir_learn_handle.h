/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file      ir_learn_handle.h
* @brief     ir learn handle header file.
* @details
* @author    elliot chen
* @date      2020-02-26
* @version   v1.1
*********************************************************************************************************
*/

#ifndef __IR_LEARN_HANDLE_H
#define __IR_LEARN_HANDLE_H

#ifdef __cplusplus
extern "C"  {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ir_learn_driver.h"
#include "key_handle.h"

#define  IR_LEARN_TIMEOUT               (20*1000)  /* 20s */

#define  INVALID_OFFSET                 0xff
#define  VALID_FLAG                     0x54

typedef enum
{
    IR_LEARN_KEY_RELEASE,
    IR_LEARN_KEY_PRESS,
} T_IR_LEARN_KEY_STATE;

typedef struct
{
    uint32_t            valid_flag;
    uint32_t            offset;
    uint32_t            key_index;
    float               frequency;
    float               duty_cycle;
    uint32_t            ir_learn_storage_buf[IR_LEARN_WAVE_MAX_LEN];
    uint32_t            buf_len;
} T_IR_LEARN_STORAGE_INFO;

typedef struct
{
    uint32_t            offset;
    T_KEY_INDEX_DEF     key_index;
} T_IR_LEARN_KEY_INFO;

bool ir_get_learned_wave_data(T_KEY_INDEX_DEF ir_send_key_index,
                              T_IR_SEND_PARA *ir_send_parameters);
void ir_learn_msg_proc(uint16_t msg_sub_type);
void ir_learn_init(void);
void ir_learn_deinit(void);
bool ir_learn_press_handle(T_KEY_INDEX_DEF ir_learn_key_index);
bool ir_learn_key_release_handle(void);
bool ir_learn_reset_ftl_learning_data(void);

#ifdef __cplusplus
}
#endif

#endif /* !defined (__IR_LEARN_CONFIG_H) */

/******************* (C) COPYRIGHT 2020 Realtek Semiconductor Corporation *****END OF FILE****/

