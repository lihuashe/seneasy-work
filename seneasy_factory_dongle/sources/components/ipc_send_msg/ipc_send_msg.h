/**
 * @file     ipc_send_msg.h
 * @version  V1.0
 * @date     2022/11/7
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

#ifndef __IPC_SEND_MSG_H__
#define __IPC_SEND_MSG_H__

/**
 * @brief  DEFINE
 */
// Whether the ipc message contains check sum
#define IPC_MSG_CHECKSUM_ENABLED 0

#define IPC_MSG_HEADER_LENGTH    8
#define IPC_MSG_DATA_LENGTH_MAX  288

/**
 * @brief  TYPEDEF
 */
typedef struct __attribute__((packed))
{
    uint32_t u32Cmd;
    uint16_t u16DataLen;
    uint16_t u16CheckSum;
    uint8_t  au8Data[IPC_MSG_DATA_LENGTH_MAX];

} stIpcData_t, *pstIpcData_t;

/**
 * @brief  Send a message to cp by ipc.
 * @param  u32Cmd: Command.
 * @param  pu8Data: Pointer to the command data.
 * @param  u16DataLen: Data length.
 * @return uint32_t: status.
 */
extern uint32_t ipc_send_msg(uint32_t u32Cmd, uint8_t *pu8Data, uint16_t u16DataLen);

#endif /* __IPC_SEND_MSG_H__ */
