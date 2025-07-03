/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     key_handle.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef KEY_HANDLE_H_
#define KEY_HANDLE_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "../app_config.h"
#include "../../sdk/tl_common.h"
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
#if (PCB_VER == PCB_SRC_4526)
#define KEY_SOURCE      7
#define KEY_OPTIONS     39
#define KEY_GUIDE       40
#define KEY_VOICE       42
#define KEY_BACK        26
#define KEY_HOME        23
#define KEY_VOL_DN      38
#define KEY_MUTE        37
#define KEY_VK_0        5
#define KEY_UP          32
#define KEY_DOWN        34
#define KEY_LEFT        15
#define KEY_RIGHT       33
#define KEY_CENTER      41  
#elif (PCB_VER == PCB_SRC_3215)
#define KEY_SOURCE      5
#define KEY_OPTIONS     2
#define KEY_GUIDE       0
#define KEY_VOICE       12
#define KEY_BACK        3
#define KEY_HOME        17
#define KEY_VOL_DN      26
#define KEY_MUTE        25
#define KEY_VK_0        10
#define KEY_UP          7
#define KEY_DOWN        9
#define KEY_LEFT        1
#define KEY_RIGHT       15
#define KEY_CENTER      8
#endif 
#define KEY_POWER       24
#define KEY_AMBILIGHT   31
#define KEY_RED         6
#define KEY_BLUE        27 
#define KEY_123         11 
#define KEY_OTT_1       21
#define KEY_OTT_2       28
#define KEY_OTT_3       22
#define KEY_OTT_4       29
#define KEY_OTT_5       23
#define KEY_OTT_6       30
#define KEY_SUBTITLE    16  
#define KEY_NUM_1       0
#define KEY_NUM_2       7
#define KEY_NUM_3       14
#define KEY_NUM_4       1
#define KEY_NUM_5       8
#define KEY_NUM_6       15
#define KEY_NUM_7       2
#define KEY_NUM_8       9
#define KEY_NUM_9       16
#define KEY_NUM_0       10
#define KEY_REWIND      6
#define KEY_PAUSE       13
#define KEY_PLAY        20
#define KEY_FFWD        27
#define KEY_GREEN       13 
#define KEY_YELLOW      20
#define KEY_MENU        19
#define KEY_INFO        14
#define KEY_VOL_UP      4
#define KEY_CHN_UP      18
#define KEY_CHN_DN      33
#define KEY_LIST        32

extern u8 batt_poor_flag;
extern u8 key_clear_pair_flag;
/*============================================================================*
 *                          Functions
 *============================================================================*/
void key_handle_pressed_1_event(void);
void key_handle_pressed_2_event(void);
void key_handle_release(void);
void key_handle_LongPress(u8 key_mode);
void key_handle_deep_pressed(void);
void pressed_key_source(void);
void app_rcu_enter_pair(void);

#endif /* KEY_HANDLE_H_ */
