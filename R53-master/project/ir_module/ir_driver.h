/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     ir_driver.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef IR_DRIVER_H_
#define IR_DRIVER_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "../app_config.h"
#include "../../sdk/tl_common.h"
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
#define TYPE_IR_SEND               1
#define TYPE_IR_RELEASE            2

#define IR_POWER                              0x0C
#define IR_AMBILIGHT                          0x8F
#define IR_PRIME_VIDEO                        0xBA
#define IR_YOUTUBE                            0x79
#define IR_NETFLIX                            0x76
#define IR_RAKUTEN_TV                         0x22
#define IR_SOURCE                             0x38
#define IR_TV                                 0x9F
#define IR_MENU                               0x57
#define IR_UP                                 0x58
#define IR_DN                                 0x59
#define IR_LEFT                               0x5A
#define IR_RIGHT                              0x5B
#define IR_CENTER                             0x5C
#define IR_BACK                               0x0A
#define IR_VOICE                              0xB8
#define IR_HOME                               0x54
#define IR_REWIND                             0x2B
#define IR_PLAY                               0x2C
#define IR_FFWD                               0x28
#define IR_OPTIONS                            0x40
#define IR_PAUSE                              0x30
#define IR_STOP                               0x31
#define IR_RED                                0x6D
#define IR_GREEN                              0x6E
#define IR_YELLOW                             0x6F
#define IR_BLUE                               0x70
#define IR_VOL_UP                             0x10
#define IR_VOL_DN                             0x11
#define IR_GUIDE                              0xCC
#define IR_MUTE                               0x0D
#define IR_CHN_UP                             0x20
#define IR_CHN_DN                             0x21
#define IR_VK_1                               0x01
#define IR_VK_2                               0x02
#define IR_VK_3                               0x03
#define IR_VK_4                               0x04
#define IR_VK_5                               0x05
#define IR_VK_6                               0x06
#define IR_VK_7                               0x07
#define IR_VK_8                               0x08
#define IR_VK_9                               0x09
#define IR_VK_0                               0x00
#define IR_TEXT                               0x3C
#define IR_SET                                0xBF
#define IR_INFO                               0x0F
#define IR_SUBTITLE                           0x4B
#define IR_CC                                 0x46
#define IR_LIST                               0xD2
#define IR_POINT                              0xD9
#define IR_OTT_AMAZON                         0xBA  //186
#define IR_OTT_FREE                           0x29  //41
#define IR_OTT_DISNEY                         0x43  //67
#define IR_OTT_APPLE_TV                       0x44  //68
#define IR_OTT_CODE_01                        0xDE  //222
#define IR_OTT_CODE_02                        0xDF  //223

#define IR_EK_LOW_BAT                         0xC3
#define IR_EK_SOURCE_S                        0x38
#define IR_EK_SOURCE_L                        0xB7
#define IR_EK_OPTIONS_S                       0x40
#define IR_EK_OPTIONS_L                       0x0F
#define IR_EK_GUIDE_S                         0xCC
#define IR_EK_GUIDE_L                         0xD2
#define IR_EK_GOOGLE_DEBUG                    0x9D
#define IR_EK_GOOGLE_TALKBACK                 0x72
#define IR_EK_SUBTITLE_S                      0x4B
#define IR_EK_SUBTITLE_L                      0x3C
#define IR_EK_GREEN_S                         0x6E
#define IR_EK_GREEN_L                         0x6A

/*============================================================================*
 *                          Functions
 *============================================================================*/
void ir_driver_init_philips(void);
void ir_driver_send_philips(u8 repeat, u8 data);
void ir_dispatch_philips(u8 type, u8 repeat ,u8 ircode);
int ir_timer_stop_code(void);

#endif /* IR_DRIVER_H_ */
