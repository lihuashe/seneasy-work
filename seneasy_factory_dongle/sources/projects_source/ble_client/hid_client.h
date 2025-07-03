#ifndef HID_CLIENT_H
#define HID_CLIENT_H

typedef enum
{
    EN_HID_CLIENT_EVENT_CHAR_FIND_FINISH = 0x00,
    EN_HID_CLIENT_EVENT_CHAR_EN_NTF_FINISH = 0x01,

    EN_HID_CLIENT_EVENT_KEY_CHAR_DATA = 0x02,
    EN_HID_CLIENT_EVENT_CONSUMER_CHAR_DATA = 0x03,

}EN_HID_CLIENT_EVENT_T;

typedef void (*hid_client_event_callback_t) (uint8_t u8Event, uint8_t *pu8Data, uint16_t u16DataLen);
void hid_client_event_callback_regiest(hid_client_event_callback_t cb);

void hid_client_get_char_handle(void);
void hid_client_enable_notify(void);
void hid_client_get_srvice_handle(uint16_t *pu16StartHandle, uint16_t *pu16EndHandle);
void hid_client_process_notify_data(uint8_t *pu8Data, uint16_t u16DataLen);

#endif