/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     bas_profile.c
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "bas_profile.h"
#include "stack/ble/ble.h"
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
const u16 my_batServiceUUID = SERVICE_UUID_BATTERY;
const u16 my_batCharUUID    = CHARACTERISTIC_UUID_BATTERY_LEVEL;
u8 batteryValueInCCC[2];
_attribute_data_retention_ u8 my_batVal[1]  = {99};
// Include attribute (Battery service)
const u16 include[3] = {BATT_PS_H, BATT_LEVEL_INPUT_CCB_H, SERVICE_UUID_BATTERY};

/*============================================================================*
 *                            Battery attribute values
 *============================================================================*/
const u8 my_batCharVal[5] = {
    CHAR_PROP_READ | CHAR_PROP_NOTIFY,
    U16_LO(BATT_LEVEL_INPUT_DP_H), U16_HI(BATT_LEVEL_INPUT_DP_H),
    U16_LO(CHARACTERISTIC_UUID_BATTERY_LEVEL), U16_HI(CHARACTERISTIC_UUID_BATTERY_LEVEL)
};

/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
