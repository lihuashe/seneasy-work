#ifndef IPC_H
#define IPC_H

typedef enum
{
    EN_IPC_EVENT_EXE_FUNC = 0x00,
    EN_IPC_EVENT_FUNC_RET = 0x01,
    EN_IPC_EVENT_DATA_REPORT = 0x02,

}EN_IPC_EVENT_T;

typedef struct
{
    uint8_t u8Module;
    uint8_t u8Cmd;
    uint8_t *pu8Data;
    uint16_t u16DataLen;
}st_ipc_heade_t;

typedef void (* ipc_event_callback_t) (uint8_t* pu8Data, uint16_t u16DataLen);

bool ipc_module_init(ipc_event_callback_t ipc_event_cb);

bool ipc_send(uint8_t u8Module, uint8_t u8Cmd, uint8_t* pu8Data, uint32_t u32DataLen);

#endif

