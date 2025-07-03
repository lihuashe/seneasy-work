/*
 * @Author: lihuashe lihuashe@seneasy.com
 * @LastEditors: lihuashe
 * @LastEditTime: 2024-10-12 13:52:37
 * @file: .c
 * @Description: Copyright 2024 Guangdong Seneasy Intelligent Technology Co., Ltd. All rights reserved.
 */
#ifndef __KEY_HANDLE_CONFIG__
#define __KEY_HANDLE_CONFIG__

#include "driver.h"

// 按键配置结构体声明
typedef void (*EventHandler)(u8);

typedef struct
{
    u8 code1;
    u8 code2;
    EventHandler handler;
    const char *event_name;
} KeyCombinationEvent;

// 定义键码结构体
typedef struct
{
    u8 key_code;
    EventHandler handler;
    const char *event_name;
} KeyEvent;

extern u8 instant_press_event;
extern u8 release_press_event;
extern u8 short_press_event;
extern u8 press_three_s_event;
extern u8 press_aight_s_event;
extern u8 press_2_long_aight_s_event;

extern KeyEvent INSTANT_PRESS_EVENT[];
extern KeyEvent RELEASE_PRESS_EVENT[];
extern KeyEvent SHORT_PRESS_EVENT[];
extern KeyEvent PRESS_THREE_S_EVENT[];
extern KeyEvent PRESS_AIGHT_S_EVENT[];
extern KeyCombinationEvent PRESS_2_LONG_AIGHT_S_EVENT[];

extern void Variable_tansfer();

#endif