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
#include "../../tl_common.h"

#if (UI_KEYBOARD_ENABLE)
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
//18 KEY
#define KEY_POWER               100
#define KEY_OK                  57
#define KEY_UP                  64
#define KEY_DOWN                59
#define KEY_LEFT                69
#define KEY_RIGHT               70
#define KEY_BACK                60
#define KEY_DB_WINDOWS          66
#define KEY_SEL_BUTTON          67
#define KEY_FLAME               68
#define KEY_FREEZE_SCREEN       63
#define KEY_DISABLE_TOUCH       61
#define KEY_VOLUME_UP           62
#define KEY_VOLUME_DOWN         58
#define KEY_INPUT_SELECT        71
#define KEY_SETTING             65
#define KEY_MOUSE_L             56
#define KEY_MOUSE_R             0

//55 KEY
#if (PCB_S1626_CHANGE)
#define KEY_ESCAPE              5
#define KEY_TAB                 4
#define KEY_CAPS_LOCK           3
#define KEY_SHIFT_LEFT          2
#define KEY_CONTROL_LEFT        1
#define KEY_FN                  11
#define KEY_GUI_LEFT            20
#define KEY_ALT_LEFT            24
#define KEY_SPACE               19
#define KEY_ARROW_LEFT          44
#define KEY_ARROW_RIGHT         54
#define KEY_ARROW_UP            51
#define KEY_ARROW_DOWN          49
#define KEY_SHIFT_RIGHT         55
#define KEY_ENTER               53
#define KEY_DELETE              50
#define KEY_EQUAL               52
#define KEY_PERIOD              40
#define KEY_SLASH               45
#define KEY_1                   9
#define KEY_2                   10
#define KEY_3                   29
#define KEY_4                   34
#define KEY_5                   39
#define KEY_6                   28
#define KEY_7                   33
#define KEY_8                   15
#define KEY_9                   42
#define KEY_0                   43
#define KEY_Q                   8
#define KEY_W                   14
#define KEY_E                   18
#define KEY_R                   23
#define KEY_T                   27
#define KEY_Y                   32
#define KEY_U                   37
#define KEY_I                   41
#define KEY_O                   47
#define KEY_P                   48
#define KEY_A                   7
#define KEY_S                   13
#define KEY_D                   17
#define KEY_F                   22
#define KEY_G                   26
#define KEY_H                   31
#define KEY_J                   36
#define KEY_K                   38
#define KEY_L                   46
#define KEY_Z                   6
#define KEY_X                   12
#define KEY_C                   16
#define KEY_V                   21
#define KEY_B                   25
#define KEY_N                   30
#define KEY_M                   35
#else
#define KEY_ESCAPE              54
#define KEY_TAB                 55
#define KEY_CAPS_LOCK           53
#define KEY_SHIFT_LEFT          50
#define KEY_CONTROL_LEFT        52
#define KEY_FN                  42
#define KEY_GUI_LEFT            33
#define KEY_ALT_LEFT            28
#define KEY_SPACE               19
#define KEY_ARROW_LEFT          10
#define KEY_ARROW_RIGHT         5
#define KEY_ARROW_UP            8
#define KEY_ARROW_DOWN          9
#define KEY_SHIFT_RIGHT         4
#define KEY_ENTER               3
#define KEY_DELETE              2
#define KEY_EQUAL               1
#define KEY_PERIOD              18
#define KEY_SLASH               14
#define KEY_1                   49
#define KEY_2                   44
#define KEY_3                   39
#define KEY_4                   34
#define KEY_5                   29
#define KEY_6                   24
#define KEY_7                   20
#define KEY_8                   15
#define KEY_9                   11
#define KEY_0                   6
#define KEY_Q                   51
#define KEY_W                   45
#define KEY_E                   40
#define KEY_R                   35
#define KEY_T                   30
#define KEY_Y                   25
#define KEY_U                   21
#define KEY_I                   16
#define KEY_O                   12
#define KEY_P                   7
#define KEY_A                   48
#define KEY_S                   46
#define KEY_D                   38
#define KEY_F                   36
#define KEY_G                   31
#define KEY_H                   26
#define KEY_J                   22
#define KEY_K                   17
#define KEY_L                   13
#define KEY_Z                   43
#define KEY_X                   47
#define KEY_C                   41
#define KEY_V                   37
#define KEY_B                   32
#define KEY_N                   27
#define KEY_M                   23
#endif
/*============================================================================*
 *                          Functions
 *============================================================================*/
void key_handle_pressed_1_event(void);
void key_handle_pressed_2_event(void);
void key_handle_pressed_3_event(void);
void key_handle_release(void);
void key_handle_LongPress(u8 key_mode);
void key_handle_deep_pressed(void);
void key_handle_pressed_power(void);
void key_handle_release_power(void);
void send_ir_key_handle(u8 ir_key);
u8 ir_direction_key_linkage_handle(u8 ir_key0, u8 ir_key1);

#endif
#endif /* KEY_HANDLE_H_ */
