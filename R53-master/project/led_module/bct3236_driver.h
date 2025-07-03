/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     bct3236_driver.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef BCT3236_DRIVER_H_
#define BCT3236_DRIVER_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include  "../app_config.h"
#include "../../sdk/tl_common.h"
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
extern s16 flagbreathe;
extern s16 openallcnt;
extern u8 backlight_rcu_ledflag;
extern s16 backlight_rcu_ledcnt;
/*============================================================================*
 *                          Functions
 *============================================================================*/
extern void bct3236_init(void);
extern void bct3236_all_on(void);
extern void TimeCtrl();

void bct3236_breath_NormalKey_00(void);
void bct3236_breath_NormalKey_01(void);
void bct3236_breath_NormalKey_00_OFF(void);
void bct3236_breath_NormalKey_01_OFF(void);
void bct3236_breath_HiddenKey(void);
void bct3236_breath_HiddenKey_OFF(void);
void bct3236_breath_AllKey_00(void);
void bct3236_breath_AllKey_01(void);
void bct3236_breath_AllKey_00_OFF(void);
void bct3236_breath_AllKey_01_OFF(void);

#endif /* BCT3236_DRIVER_H_ */
