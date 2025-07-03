#ifndef BATTERY_CLIENT_H
#define BATTERY_CLIENT_H

typedef enum
{
    EN_BATTERY_CLIENT_EVENT_CHAR_FIND_FINISH = 0x00,
    EN_BATTERY_CLIENT_EVENT_CHAR_EN_NTF_FINISH = 0x01,
    EN_BATTERY_CLIENT_EVENT_BATTERY_LEVEL_DATA = 0x02,
}EN_BATTERY_CLIENT_EVENT_T;

typedef void (*battery_client_event_callback_t) (uint8_t u8Event, uint8_t *pu8Data, uint16_t u16DataLen);
void battery_client_event_callback_regiest(battery_client_event_callback_t cb);

void battery_client_get_srvice_handle(uint16_t *pu16StartHandle, uint16_t *pu16EndHandle);
void battery_client_get_char_handle(void);
void battery_client_enable_notify(void);
void battery_client_process_notify_data(uint8_t *pu8Data, uint16_t u16DataLen);

#endif