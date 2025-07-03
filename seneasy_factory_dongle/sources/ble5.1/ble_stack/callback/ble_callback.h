/**
 * @file     ble_callback.h
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2021/05/21
 * @history
 * @note
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2023, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 */

#ifndef __BLE_CALLBACK_H__
#define __BLE_CALLBACK_H__

// include header here to avoid confusion
#include <stdint.h>
#include <stdbool.h>

typedef struct stack_msg
{
    uint16_t conn_handle;
    uint8_t  module_type;
    uint8_t  len_of_ext_info;
    void    *p_ext_info;
    void    *dataPoolId;
} stack_msg_st;

typedef bool (*func_task_set_signals)(uint8_t module_type, uint16_t conn_handle, uint8_t len, uint8_t *pdata);

typedef void (*func_ble_event)(uint32_t u32Cmd, const void *p, uint32_t u32Len);
typedef bool (*func_ble_timer_start)(void *pTimerHandle, uint32_t u32TimeOutMs, void *pContext);
typedef bool (*func_ble_timer_stop)(void *pTimerHandle);
typedef bool (*func_ble_timer_create)(void *pTimerHandle, void *pTimeOutHandler);
typedef bool (*func_ble_timer_delete)(void *pTimerHandle);

void ble_callback_send_event(uint32_t u32Cmd, const void *p, uint32_t u32Len);
bool ble_callback_send_msg_to_stack_task(uint8_t module_type, uint16_t conn_handle, uint8_t len, uint8_t *pdata);
bool ble_callback_timer_create(void *pTimerHandle, void *pTimeOutHandler);
bool ble_callback_timer_start(void *pTimerHandle, uint32_t u32TimeOutMs, void *pContext);
bool ble_callback_timer_stop(void *pTimerHandle);
bool ble_callback_timer_delete(void *pTimerHandle);
void ble_callback_set_send_msg_to_stack_task_callback(func_task_set_signals callback);
void ble_callback_set_host_stack_send_event_callback(func_ble_event callback);
void ble_callback_set_timer_create_callback(func_ble_timer_create callback);
void ble_callback_set_timer_start_callback(func_ble_timer_start callback);
void ble_callback_set_send_timer_stop_callback(func_ble_timer_stop callback);
void ble_callback_set_send_timer_delete_callback(func_ble_timer_delete callback);

extern func_task_set_signals g_taskSetSignalCallback;
extern func_ble_event        g_bleEventCallback;

extern func_ble_timer_start g_bleTimerStartCallback;
extern func_ble_timer_stop  g_bleTimerStopCallback;

#endif /* __BLE_CALLBACK_H__ */
