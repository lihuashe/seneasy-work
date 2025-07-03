/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     ota_profile.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef OTA_PROFILE_H_
#define OTA_PROFILE_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include  "../app_config.h"
#include "../../sdk/tl_common.h"
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
#define GOOGLE_OTA                      0   // 1: enable
#define TELINK_OTA                      1   // 1: enable

extern const u16 userdesc_UUID;
extern const u8 my_OtaUUID[16];
extern const u8 my_OtaServiceUUID[16];
extern const u8  my_OtaName[3];
extern const u8 my_OtaCharVal[19];
extern u8 my_OtaData;
extern _attribute_data_retention_ u16 my_Otaccc;

/*============================================================================*
 *                          Functions
 *============================================================================*/
int app_ota_handle(void * p);
void app_enter_ota_mode_1(void);
void app_debug_ota_result_1(int result);

#endif /* OTA_PROFILE_H_ */
