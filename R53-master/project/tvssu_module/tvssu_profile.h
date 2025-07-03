/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     tvssu_profile.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef TVSSU_PROFILE_H_
#define TVSSU_PROFILE_H_
/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include  "../app_config.h"
#include "../../sdk/tl_common.h"
#if (TL_SZ119_TVSSU)
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
extern int SSP_Profile_WriteAttrCB(void *p);
extern const u8 my_SimpleSetupServiceUUID[16];
extern const u8 my_SimpleSetupUUID[16];
extern u8 simplesetupValueInCCC[2];
extern u8 my_SimpleSetupData[20];
extern const u8 my_SimpleSetupName[13];
extern const u8 ss_CharVal[19];
/*============================================================================*
 *                          Functions
 *============================================================================*/
extern int seneasy_tvssu_attr_write_cb(void *p) ;
#endif
#endif /* SENEASY_PROFILE_H_ */
