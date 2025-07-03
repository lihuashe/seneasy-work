/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file         ir_send_handle.h
* @brief
* @details
* @author       barry_bian
* @date         2020-04-08
* @version      v1.0
* *********************************************************************************************************
*/

#ifndef __IR_SEND_HANDLE_H_
#define __IR_SEND_HANDLE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"
#include "stdint.h"
#include "stdbool.h"
#include "ir_send_driver.h"
#include "key_handle.h"

typedef enum
{
    NEC_PROTOCOL     = 0,
} T_IR_PROTOCOL;

void ir_send_msg_proc(uint16_t msg_sub_type);
void ir_send_key_press_handle(T_KEY_INDEX_DEF ir_key_index);
void ir_send_key_release_handle(void);
void ir_power_key_send_timer_cb(TimerHandle_t p_timer);

#ifdef __cplusplus
}
#endif

#endif /*__IR_APP_CONFIG_H_*/


/******************* (C) COPYRIGHT 2017 Realtek Semiconductor Corporation *****END OF FILE****/

