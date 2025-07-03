/**
 * @file     ipc_send_msg.c
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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "app_cfg.h"
#include "err_def.h"

#include "ipc_general_blocking.h"
#include "ipc_send_msg.h"

#ifdef __MAIN_PROCESSOR
    #include "ble_common.h"
#endif

/**
 * @brief  Send a message to cp by ipc.
 * @param  u32Cmd: Command.
 * @param  pu8Data: Pointer to the command data.
 * @param  u16DataLen: Data length.
 * @return uint32_t: status.
 */
uint32_t ipc_send_msg(uint32_t u32Cmd, uint8_t *pu8Data, uint16_t u16DataLen)
{
    uint32_t    u32Ret         = 0;
    uint16_t    u16Sum         = 0;
    uint16_t    u16SendDataLen = 0;
    stIpcData_t stIPCMsgData;

    if ((pu8Data != NULL) && (u16DataLen == 0))
    {
        return ERR_PARA_ERR;
    }

    if ((pu8Data == NULL) && (u16DataLen != 0))
    {
        return ERR_PARA_ERR;
    }

    if (u16DataLen > IPC_MSG_DATA_LENGTH_MAX)
    {
        return ERR_PARA_ERR;
    }

    stIPCMsgData.u32Cmd     = u32Cmd;
    stIPCMsgData.u16DataLen = u16DataLen;

    memcpy(stIPCMsgData.au8Data, pu8Data, u16DataLen);

    for (uint32_t i = 0; i < u16DataLen; i++)
    {
        u16Sum = u16Sum + stIPCMsgData.au8Data[i];
    }
    stIPCMsgData.u16CheckSum = u16Sum;

    u32Ret = ipc_general_send_msg_blocking((uint8_t *)&stIPCMsgData, u16DataLen + IPC_MSG_HEADER_LENGTH);

    return u32Ret;
}
