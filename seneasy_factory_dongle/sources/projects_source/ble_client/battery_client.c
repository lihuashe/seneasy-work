/**********************************************************************************************************************
 * @file     battery_client.c
 * @author   Maxscend AE Team
 * @version  V1.0
 * @date     2023/01/11
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
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "utility.h"
#include "app_cfg.h"


#include "gatt_uuid_def.h"
#include "gatt_client_api.h"
#include "ble_central.h"

#include "battery_client.h"


//=====================================================================================================================
// LOCAL VARIABLES
//=====================================================================================================================
static uint16_t u16BatteryLevelCharHandle = 0;
static uint16_t u16BatteryLevelCharDesCfgHandle = 0;
static uint16_t u16BatteryLevelCharDesReportRefHandle = 0;

battery_client_event_callback_t battery_client_event_callback;

/**********************************************************************************************************************
 * @brief  battery client get service handle
 * @param  pu16StartHandle: start handle
           pu16EndHandle: end handle
 * @output
 * @return None.
 *********************************************************************************************************************/
void battery_client_get_srvice_handle(uint16_t *pu16StartHandle, uint16_t *pu16EndHandle)
{
    ble_central_get_service_handle_by_uuid(0, BATT_SERV_UUID, NULL, pu16StartHandle, pu16EndHandle);
}

/**********************************************************************************************************************
 * @brief  battery client get characteristic handle
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
void battery_client_get_char_handle(void)
{
    if(!ble_central_get_char_value_handle_by_uuid(0, BATT_LEVEL_UUID, NULL, &u16BatteryLevelCharHandle, 0))
    {
        PRINTF("battery level char not found\n");
        return;
    }
    if(!ble_central_get_char_des_by_char_handle(u16BatteryLevelCharHandle, GATT_CLIENT_CHAR_CFG_UUID, &u16BatteryLevelCharDesCfgHandle))
    {
        PRINTF("battery level char cfg des not found\n");
        return;
    }
    if(!ble_central_get_char_des_by_char_handle(u16BatteryLevelCharHandle, GATT_REPORT_REF_UUID, &u16BatteryLevelCharDesReportRefHandle))
    {
        PRINTF("battery level char report ref des not found\n");
        return;
    }
    if(battery_client_event_callback)
    {
        battery_client_event_callback(EN_BATTERY_CLIENT_EVENT_CHAR_FIND_FINISH, NULL, 0);
    }
}

/**********************************************************************************************************************
 * @brief  battery client parse barttery level characteristic description config write result
 * @param  u8Result: 0 - write ok, 1 - write fail
 * @output
 * @return None.
 *********************************************************************************************************************/
void battery_client_parse_battery_level_char_des_cfg_write_result(uint8_t u8Result)
{
    if(0 == u8Result)
    {
        PRINTF("battery level char des cfg write ok\n");
        if(battery_client_event_callback)
        {
            battery_client_event_callback(EN_BATTERY_CLIENT_EVENT_CHAR_EN_NTF_FINISH, NULL, 0);
        }
    }
}

/**********************************************************************************************************************
 * @brief  battery client enable notify
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
void battery_client_enable_notify(void)
{
    uint8_t u8Data[2] = {1, 0};
    if(0 != u16BatteryLevelCharDesCfgHandle)
    {
        if(!ble_central_write_char_value(ble_central_get_conn_handle(), u16BatteryLevelCharDesCfgHandle, u8Data, sizeof(u8Data), battery_client_parse_battery_level_char_des_cfg_write_result))
        {
            PRINTF("battery level char write fail\n");
        }
    }
}

/**********************************************************************************************************************
 * @brief  battery client process notify data
 * @param  pu8Data: Notify data
           u16DataLen: Notify data length
 * @output
 * @return None.
 *********************************************************************************************************************/
void battery_client_process_notify_data(uint8_t *pu8Data, uint16_t u16DataLen)
{
    battery_client_event_callback(EN_BATTERY_CLIENT_EVENT_BATTERY_LEVEL_DATA, pu8Data, sizeof(pu8Data));
}

/**********************************************************************************************************************
 * @brief  battery client event callback regisest
 * @param  battery_client_event_callback_t: callback
 * @output
 * @return None.
 *********************************************************************************************************************/
void battery_client_event_callback_regiest(battery_client_event_callback_t cb)
{
    if(cb)
    {
        battery_client_event_callback = cb;
    }
}
