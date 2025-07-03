/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     bas_profile.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef BAS_PROFILE_H_
#define BAS_PROFILE_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include  "../app_config.h"
#include "../../sdk/tl_common.h"
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
extern const u16 my_batServiceUUID;
extern const u16 my_batCharUUID;
extern u8 batteryValueInCCC[2];
extern _attribute_data_retention_ u8 my_batVal[1];
extern const u16 include[3];
extern const u8 my_batCharVal[5];

#endif /* BAS_PROFILE_H_ */
