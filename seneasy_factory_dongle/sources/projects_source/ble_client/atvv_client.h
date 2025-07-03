#ifndef ATVV_CLIENT_H
#define ATVV_CLIENT_H

#define ATVV_CMD_GET_CAPS        (0x0A)
#define ATVV_CMD_MIC_OPEN        (0x0C)
#define ATVV_CMD_MIC_CLOSE       (0x0D)
#define ATVV_CMD_SEARCH_END      (0x0E)

typedef enum
{
    EN_ATVV_CLIENT_EVENT_CHAR_FIND_FINISH = 0x00,
    EN_ATVV_CLIENT_EVENT_CHAR_EN_NTF_FINISH = 0x01,
    EN_ATVV_CLIENT_EVENT_AUDIO_DATA = 0x02,
    EN_ATVV_CLIENT_EVENT_CAPS_RSP = 0x03,
    EN_ATVV_CLIENT_EVENT_AUDIO_START = 0x04,

}EN_ATVV_CLIENT_EVENT_T;

typedef void (*atvv_client_event_callback_t) (uint8_t u8Event, uint8_t *pu8Data, uint16_t u16DataLen);
void atvv_client_event_callback_regiest(atvv_client_event_callback_t cb);

void atvv_client_get_srvice_handle(uint16_t *pu16StartHandle, uint16_t *pu16EndHandle);
void atvv_client_get_char_handle(void);
void atvv_client_enable_notify(void);
void atvv_client_process_notify_data(uint8_t *pu8Data, uint16_t u16DataLen);
bool atvv_client_send_cmd(uint8_t u8Cmd);
#endif