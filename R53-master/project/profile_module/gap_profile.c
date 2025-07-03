/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     gap_profile.c
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
#include "gap_profile.h"
#include "stack/ble/ble.h"
/*============================================================================*
 *                            Global Variables
 *============================================================================*/
const u16 serviceChangeUUID     = GATT_UUID_SERVICE_CHANGE;
const u16 my_devNameUUID        = GATT_UUID_DEVICE_NAME;
const u16 my_gapServiceUUID     = SERVICE_UUID_GENERIC_ACCESS;
const u16 my_appearanceUIID     = GATT_UUID_APPEARANCE;
const u16 my_periConnParamUUID  = GATT_UUID_PERI_CONN_PARAM;
const u16 my_appearance         = GAP_APPEARE_UNKNOWN;
const u16 my_gattServiceUUID    = SERVICE_UUID_GENERIC_ATTRIBUTE;
const gap_periConnectParams_t my_periConnParameters = {8, 11, 0, 1000};
u16 serviceChangeVal[2] = {0};
u8 serviceChangeCCC[2]  = {0,0};

/*============================================================================*
 *                            GAP Attribute Values
 *============================================================================*/
const u8 my_devNameCharVal[5] = {
    CHAR_PROP_READ | CHAR_PROP_NOTIFY,
    U16_LO(GenericAccess_DeviceName_DP_H), U16_HI(GenericAccess_DeviceName_DP_H),
    U16_LO(GATT_UUID_DEVICE_NAME), U16_HI(GATT_UUID_DEVICE_NAME)
};
const u8 my_appearanceCharVal[5] = {
    CHAR_PROP_READ,
    U16_LO(GenericAccess_Appearance_DP_H), U16_HI(GenericAccess_Appearance_DP_H),
    U16_LO(GATT_UUID_APPEARANCE), U16_HI(GATT_UUID_APPEARANCE)
};
const u8 my_periConnParamCharVal[5] = {
    CHAR_PROP_READ,
    U16_LO(CONN_PARAM_DP_H), U16_HI(CONN_PARAM_DP_H),
    U16_LO(GATT_UUID_PERI_CONN_PARAM), U16_HI(GATT_UUID_PERI_CONN_PARAM)
};
const u8 my_serviceChangeCharVal[5] = {
    CHAR_PROP_INDICATE,
    U16_LO(GenericAttribute_ServiceChanged_DP_H), U16_HI(GenericAttribute_ServiceChanged_DP_H),
    U16_LO(GATT_UUID_SERVICE_CHANGE), U16_HI(GATT_UUID_SERVICE_CHANGE)
};

/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
