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
#include "../../sdk/tl_common.h"
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
#define BR_CENTER                             0x0041
#define BR_VOICE                              0x0221

//////////////////// key type ///////////////////////
#define IDLE_KEY                   0
#define CONSUMER_KEY               1
#define KEYBOARD_KEY               2
#define IR_KEY                     3
#define VOICE_KEY                  4

extern const unsigned int Kb_Drive_Pins_RCU[KB_DRIVE_NUM];
#if (PCB_VER == PCB_SRC_4526)
extern const unsigned char Kb_Map_Normal_RCU[KB_SCAN_NUM][KB_DRIVE_NUM];
#elif (PCB_VER == PCB_SRC_3215)
extern const unsigned char Kb_Map_Normal_RCU[KB_SCAN_NUM][KB_DRIVE_NUM];
#endif
extern const unsigned char Kb_Wakeup_Key_Map_Correspend_Index_RCU[12];
extern const unsigned char Kb_Map_devicelayout_Correspend_Index_RCU[9];
extern const unsigned char navi_key_index_RCU[4];
extern const unsigned char Kb_Map_Testmode_Seq_Correspend_Index_RCU[5];
extern const unsigned char Kb_Map_Ble_RCU[KB_SCAN_NUM * KB_DRIVE_NUM];
#if (PCB_VER == PCB_SRC_4526)
extern const unsigned char Kb_Map_Ir_RCU[KB_SCAN_NUM * KB_DRIVE_NUM];
extern const unsigned char Kb_Map_Ir_HIDDEN_CODE[KB_SCAN_NUM * KB_DRIVE_NUM];
#elif (PCB_VER == PCB_SRC_3215)
extern const unsigned char Kb_Map_Ir_RCU_EU01[KB_SCAN_NUM * KB_DRIVE_NUM];
extern const unsigned char Kb_Map_Ir_HIDDEN_CODE_EU01[KB_SCAN_NUM * KB_DRIVE_NUM];
extern const unsigned char Kb_Map_Ir_RCU_LM01[KB_SCAN_NUM * KB_DRIVE_NUM];
extern const unsigned char Kb_Map_Ir_HIDDEN_CODE_LM01[KB_SCAN_NUM * KB_DRIVE_NUM];
#endif

typedef enum{
    APP_ACCESSIBILITY_SHORTCUT_NONE=0,
    APP_ACCESSIBILITY_SHORTCUT_PAIRING,
    APP_ACCESSIBILITY_SHORTCUT_FACTORYRESET,
    APP_ACCESSIBILITY_SHORTCUT_BUGREPORT,
    APP_ACCESSIBILITY_SHORTCUT_DOWNBACK,
    APP_ACCESSIBILITY_SHORTCUT_SINGLEKEY,
    APP_ACCESSIBILITY_SHORTCUT_DEEPBACK,
    APP_ACCESSIBILITY_SHORTCUT_DEEPVOLDN,
    APP_ACCESSIBILITY_SHORTCUT_KEYPRESS
}APP_Accessibility_Shortcut_Mode;

extern _attribute_data_retention_ u32 keyScanLongPressTick;
extern _attribute_data_retention_ u32 LongPressTick;
extern _attribute_data_retention_ APP_Accessibility_Shortcut_Mode  Accessibility_Shortcut_Mode;
extern _attribute_data_retention_ u8 Accessibility_Shortcut_Flag;
extern _attribute_data_retention_ u8  is_pairing_mode;
extern _attribute_data_retention_ u8  is_reconn_mode;
extern _attribute_data_retention_ u8  repairing_flag;
#if (TL_SZ119_TVSSU) 
extern u8 rcu_ssu_callback;
extern u8 key_delay_callback;
#endif
/*============================================================================*
 *                          Functions
 *============================================================================*/
extern void kb_driver_map_init(void);
extern u8 app_custom_get_drive_len(void);

#endif /* KEY_DRIVER_H_ */
