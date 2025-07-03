/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     key_driver.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef KEY_DRIVER_H_
#define KEY_DRIVER_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "../app_config.h"
#include "../../tl_common.h"

#if (UI_KEYBOARD_ENABLE)
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
typedef enum{
    APP_ACCESSIBILITY_SHORTCUT_NONE=0,
    APP_ACCESSIBILITY_SHORTCUT_PAIRING,
    APP_ACCESSIBILITY_SHORTCUT_SHIPMODE,
    APP_ACCESSIBILITY_SHORTCUT_CHANGEMODE,
    APP_ACCESSIBILITY_SHORTCUT_OTADONGLE,
    APP_ACCESSIBILITY_SHORTCUT_SINGLEKEY,
    APP_ACCESSIBILITY_SHORTCUT_DEEPVOLDN,
    APP_ACCESSIBILITY_SHORTCUT_KEYPRESS,
    APP_ACCESSIBILITY_SHORTCUT_FACTORYMODE
}APP_Accessibility_Shortcut_Mode;

extern int 	key_not_released;
extern u8 key_power_flag;
extern _attribute_data_retention_ u32 keyScanLongPressTick;
extern _attribute_data_retention_ u32 LongPressTick;
extern _attribute_data_retention_ APP_Accessibility_Shortcut_Mode  Accessibility_Shortcut_Mode;
extern _attribute_data_retention_ u8 Accessibility_Shortcut_Flag;
/*============================================================================*
 *                          Functions
 *============================================================================*/
void key_wakeup_init(void);
void proc_keyboard (u8 e, u8 *p, int n);
void keyscan_gpio_init(void);
void keyscan_scan_func(void);
void key_change_proc(void);

#endif
#endif /* KEY_DRIVER_H_ */
