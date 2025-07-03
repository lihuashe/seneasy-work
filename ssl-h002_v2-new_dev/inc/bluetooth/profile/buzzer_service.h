
#ifndef _BUZZER_SERVICE_H_
#define _BUZZER_SERVICE_H_

#include "profile_server.h"

#ifdef __cplusplus
extern "C"  {
#endif      /* __cplusplus */


/** @brief  Demo Profile service related UUIDs. */

#define GATT_UUID_CHAR_BUZZER_HANDSHAKE                       0xFFF2
#define GATT_UUID_CHAR_BUZZER_TEST_MODE                       0xFFF1

/** @brief  Index of each characteristic in Demo Profile service database. */
#define BLE_SERVICE_CHAR_BUZZER_HANDSHAKE_INDEX               0x02
#define GATT_SVC_BUZZER_HANDSHAKE_CHAR_CCCD_INDEX             0x03
#define BLE_SERVICE_CHAR_BUZZER_TEST_MODE_INDEX               0x05

#define BUZZER_WRITE_HANDSHAKE  1
#define BUZZER_WRITE_TEST_MODE  2

#define BUZZER_NOTIFY_ENABLE   1
#define BUZZER_NOTIFY_DISABLE  2

/** Message content */
typedef union
{
    struct
    {
        uint8_t len;
        uint8_t *report;
    } report_data;
} T_BUZZER_WRITE_PARAMETER;

/** @struct _TATVV_WRITE_MSG
  * @brief write message
  */
typedef struct
{
    uint8_t write_type; /**< ref: @ref Buzzer_Write_Info */
    T_BUZZER_WRITE_PARAMETER write_parameter;
} T_BUZZER_WRITE_MSG;

typedef union _TBUZZER_UPSTREAM_MSG_DATA
{
    uint8_t notification_indification_index;
    T_BUZZER_WRITE_MSG write_msg;
} BUZZER_UPSTREAM_MSG_DATA;

/** Buzzer service data to inform application */
typedef struct _T_BUZZER_CALLBACK_DATA
{
    T_SERVICE_CALLBACK_TYPE     msg_type;
    BUZZER_UPSTREAM_MSG_DATA    msg_data;
} T_BUZZER_CALLBACK_DATA;

extern uint8_t buzzer_svc_handshake_values[16];

uint8_t buzzer_svc_add_service(void *pFunc);

#ifdef __cplusplus
}
#endif

#endif
