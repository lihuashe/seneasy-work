#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "err_def.h"
#include "utility.h"
#include "msg_queue.h"
#include "ipc_general_queue.h"
#include "ipc.h"

#define IPC_SEND_RETRY_TIMES      (10)

ipc_event_callback_t ipc_event_callback;

static uint8_t ipcQueueBufferMem[IpcMessageBufferMemSize(IPC_MSG_COUNT, IPC_MSG_DATA_SIZE)];

static uint8_t gu8SendData[IPC_MSG_DATA_SIZE];

static uint8_t ipc_example_receive_msg_callback(uint8_t *pu8Data, uint32_t u32DataLen)
{
    if(ipc_event_callback != NULL)
    {
        ipc_event_callback(pu8Data, u32DataLen);
    }
    return 0;
}

bool ipc_module_init(ipc_event_callback_t ipc_event_cb)
{
    if(NULL == ipc_event_cb)
    {
        return false;
    }
    ipc_event_callback = ipc_event_cb;
    
    stIpcInit_t stIpcInit;
    
    stIpcInit.pu8Buf = ipcQueueBufferMem;
    stIpcInit.u32BufSize = sizeof(ipcQueueBufferMem);
    stIpcInit.u32MsgCount = IPC_MSG_COUNT;
    stIpcInit.u32MsgSize = IPC_MSG_DATA_SIZE;
    stIpcInit.enMode = QUEUE_MODE_NO_OVERFLOW;
    stIpcInit.pfnCallback = ipc_example_receive_msg_callback;
    
    ipc_general_init(&stIpcInit);
    return true;
}

bool ipc_send(uint8_t u8Module, uint8_t u8Cmd, uint8_t* pu8Data, uint32_t u32DataLen)
{
    EN_ERR_STA_T enRet;
    uint8_t u8RetryTimes = 0;
    if(((NULL == pu8Data) && (0 != u32DataLen)) || (u32DataLen > IPC_MSG_DATA_SIZE - 2))
    {
        return false;
    }
    gu8SendData[0] = u8Module;
    gu8SendData[1] = u8Cmd;
    memcpy(gu8SendData+2, pu8Data, u32DataLen);
    enRet = ipc_general_send_msg_nonblocking(gu8SendData, u32DataLen+2);
    while(ERR_STA_OK != enRet)
    {
        rom_delay_ms(1);
        enRet = ipc_general_send_msg_nonblocking(gu8SendData, u32DataLen+2);
        u8RetryTimes++;
        if(u8RetryTimes == IPC_SEND_RETRY_TIMES)
        {
            return false;
        }
    }
    return true;
}
