/**********************************************************************************************************************
 * @file     ble_central.h
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2022/02/13
 * @history
 * @note
 **********************************************************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2021, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 *********************************************************************************************************************/

#ifndef __BLE_CENTRAL_H__
#define __BLE_CENTRAL_H__


typedef void (*ble_central_event_callback_t) (uint8_t u8Event, uint8_t *pu8Data, uint16_t u16DataLen);
typedef void (*ble_central_char_read_callback_t) (uint8_t *pu8Data, uint16_t u16DataLen);
typedef void (*ble_central_char_write_callback_t) (uint8_t u8Result);

typedef enum
{
    EN_BLE_CENTRAL_EVENT_FIND_SERVICE_FINISH = 0x00,
    EN_BLE_CENTRAL_EVENT_CHAR_READ_RESULT = 0x01,
    EN_BLE_CENTRAL_EVENT_NOTIFY = 0x02,

}EN_BLE_CENTRAL_EVENT_T;

/**
 * @brief  ble central init.
 */
bool ble_central_init(ble_central_event_callback_t ble_central_event_cb);

bool ble_central_get_service_handle_by_uuid(uint8_t u8IsUuid128, uint16_t u16Uuid16, uint8_t *pu8Uuid128, uint16_t *pu16StartHandle, uint16_t *pu16EndHandle);
bool ble_central_get_char_value_handle_by_uuid(uint8_t u8IsUuid128, uint16_t u16Uuid16, uint8_t *pu8Uuid128, uint16_t *pu16Handle, uint8_t u8CharIdx);
bool ble_central_get_char_des_by_char_handle(uint16_t u16CharHandle, uint16_t u16CharDesUuid, uint16_t *pu16Handle);

bool ble_central_read_char_value(uint16_t u16ConnHandle, uint16_t u16ValueHandle, ble_central_char_read_callback_t read_cb);
bool ble_central_write_char_value(uint16_t u16ConnHandle, uint16_t u16ValueHandle, uint8_t *pu8Data, uint16_t u16DataLen, ble_central_char_write_callback_t write_cb);

uint16_t ble_central_get_conn_handle(void);

void ble_central_data_buf_free(void *p);
uint8_t ble_central_discover_primary_services(uint16_t u16ConnHandl);

void ble_central_send_security_req(uint16_t u16ConnHandl);
void ble_central_free_client_data_buffer(void);

bool ble_central_start_scan(void);


#endif /* __BLE_CENTRAL_H__ */

