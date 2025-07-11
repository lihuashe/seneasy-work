/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     ft3308_upgrade.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef FT3308_UPGRADE_H_
#define FT3308_UPGRADE_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "../app_config.h"
#include "../../tl_common.h"

#if(TOUCH_ENABLE)
/*============================================================================*
*                        Export Global Variables
*============================================================================*/

/*============================================================================*
 *                          Functions
 *============================================================================*/
int fts_read_reg(u8 addr, u8 *val);
int fts_fwupg_auto_upgrade(void);

#endif
#endif /* FT3308_UPGRADE_H_ */
