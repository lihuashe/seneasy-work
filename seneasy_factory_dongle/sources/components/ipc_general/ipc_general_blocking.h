/**
 * @file     ipc_general_blocking.h
 * @version  V1.0
 * @date     2022/12/27
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

#ifndef __IPC_GENERAL_BLOCKING_H__
#define __IPC_GENERAL_BLOCKING_H__

/**
 * @brief  TYPEDEF
 */
typedef int32_t (*ipc_rx_msg_cb_func)(uint8_t *pu8DataBuffer, uint16_t u32DataLen);

/**
 * @brief  General ipc Initialization.
 * @param  pu8DataBuf: the buffer to save data.
 * @param  u32BufMaxLen: the buffersize to save data.
 * @param  pfnCallback: Callback functions.
 * @return status: @ref EN_ERR_STA_T.
 */
extern uint8_t ipc_general_init(uint8_t *pu8DataBuf, uint32_t u32BufMaxLen, ipc_rx_msg_cb_func pfnCallback);

/**
 * @brief  Send ipc message by blocking. This API can be called from Interrupt Service Routines (ISR).
 * @param  pu8Data: Pointer to the data buffer which will be sent.
 * @param  u32DataLen: Data length.
 * @return status: @ref EN_ERR_STA_T.
 */
extern int32_t ipc_general_send_msg_blocking(uint8_t *pu8Data, uint32_t u32DataLen);

#endif /* __IPC_GENERAL_BLOCKING_H__ */
