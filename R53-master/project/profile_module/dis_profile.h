/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     dis_profile.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef DIS_PROFILE_H_
#define DIS_PROFILE_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include  "../app_config.h"
#include "../../sdk/tl_common.h"
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
extern _attribute_data_retention_ u8 my_deviceName[5];
extern _attribute_data_retention_ u8 my_FWRevtrs[21];
extern _attribute_data_retention_ u8 my_SWRevtrs[21];
extern _attribute_data_retention_ u8 my_HWRevtrs[4];
extern _attribute_data_retention_ u8 my_SerialNumtrs [4];
extern _attribute_data_retention_ u8 rcu_project_name [5];

extern const u16 my_devServiceUUID;
extern const u16 my_PnPUUID;
extern const u16 my_SysIDUUID;
extern const u16 my_ModelNUMUUID;
extern const u16 my_SerialNUMUUID;
extern const u16 my_FWRevUUID;
extern const u16 my_HWRevUUID;
extern const u16 my_SWRevUUID;
extern const u16 my_ManuNameUUID;

extern const u8 my_PnCharVal[5];
extern const u8 my_ManuNameCharVal[5];
extern const u8 my_SerialNUMCharVal[5];
extern const u8 my_FWRevCharVal[5];
extern const u8 my_HWRevCharVal[5];
extern const u8 my_SWRevCharVal[5];

void read_flash_rcu_fw (void);

#endif /* DIS_PROFILE_H_ */
