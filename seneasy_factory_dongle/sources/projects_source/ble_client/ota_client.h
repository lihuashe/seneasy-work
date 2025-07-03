#ifndef OTA_CLIENT_H
#define OTA_CLIENT_H

#define OTA_DATA_SIZE 256
//MXD OTA 3.0
#define MXD_OTA_SERVICE_UUID    0xFD10  //
#define MXD_OTA_WRITE_UUID      0xFD11  //
#define MXD_OTA_NOTIFY_UUID     0xFD12  //
typedef enum
{
    EN_OTA_CLIENT_EVENT_CHAR_FIND_FINISH = 0x00,
    EN_OTA_CLIENT_EVENT_CHAR_EN_NTF_FINISH = 0x01,
    EN_OTA_CLIENT_EVENT_DATA = 0x02,
}EN_OTA_CLIENT_EVENT_T;

typedef enum
{
    GET_CHIP_FIRMWARE_VERSION = 0x00,
}EN_OTA_CLIENT_CMD_T;

typedef void (*ota_client_event_callback_t) (uint8_t u8Event, uint8_t *pu8Data, uint16_t u16DataLen);

void ota_client_get_srvice_handle(uint16_t *pu16StartHandle, uint16_t *pu16EndHandle);
void ota_client_get_char_handle(void);
void ota_client_parse_battery_level_char_des_cfg_write_result(uint8_t u8Result);
void ota_client_enable_notify(void);
void ota_client_event_callback_regiest(ota_client_event_callback_t cb);
void ota_client_process_notify_data(uint8_t *pu8Data, uint16_t u16DataLen);
bool ota_client_send_data(uint8_t *pu8OtaBuff, uint16_t u16Len);

#endif