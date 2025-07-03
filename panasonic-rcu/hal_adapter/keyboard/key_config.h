
#ifndef APP_KEYBOARD_MODULE_KEY_CONFIG_H_
#define APP_KEYBOARD_MODULE_KEY_CONFIG_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include "py32f0xx_hal.h"
/*============================================================================*
 *                              key driver config
 *============================================================================*/

#define  KB_ROW_PINS    {GPIO_PIN_8, GPIO_PIN_11, GPIO_PIN_12}
#define  KB_COL_PINS    {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_7}
#define  KB_COL_PORT           GPIOB
#define  KB_ROW_PORT           GPIOA
#define  KB_ROW_NUM            3
#define  KB_COL_NUM            4


    // 定义按键对应值
    enum KB_CODE
    {
        KB_CODE_ANY = 0,
        KB_CODE_S1 = 1,
        KB_CODE_S2 = 2,
        KB_CODE_S3 = 3,
        KB_CODE_S4 = 4,
        KB_CODE_S5 = 5,
        KB_CODE_S6 = 6,
        KB_CODE_S7 = 7,
        KB_CODE_S8 = 8,
        KB_CODE_S9 = 9,
        KB_CODE_S10 = 10,
        KB_CODE_S11 = 11,
        KB_CODE_S12 = 12,
        // KB_CODE_S13 = 13,
        // KB_CODE_S14 = 14,
        // KB_CODE_S15 = 15,
        _NUM_KEYS,
        KB_NULL,
    };

#define NUM_KEYS (_NUM_KEYS - 1)

#define KEY_SCAN_INTERAL_MS                         5

#define STUCK_KEY_ENABLE                            0
#if STUCK_KEY_ENABLE
#define KEY_STUCK_TIME_MS                           30 * 1000
#endif

#define DEBOUNCE_EN                                 0 // 软件消抖
#if DEBOUNCE_EN
#define DEBOUNCE_THRESHOLD                          3 // 消抖阈值
#endif

#define MAX_DETECT_KEY_NUM                          3
#define AUTO_CLICK_AFTER_RELEASE_EN                 1 // 按键释放后自动触发还被按下的按键pressed事件

#define JOINED_KEY_EN                               1 // 连体按键功能使能

// 选择扫描方式
#define ROW_SCAN                                    0 
#define COL_SCAN                                    1 
#define ROW_COL_SCAN                                2 // 行列交替扫描
#define SCAN_METHOD                                 ROW_SCAN 
// #define SCAN_METHOD                                 COL_SCAN 

/*============================================================================*
 *                              key handle config
 *============================================================================*/

#define SAVE_LAST_KEY_EVENT_HANDLE_EN               0       // 保存上次触发按键的函数回调
#define KEY_EVENT_MASK_EN                           0       // 按键事件屏蔽

#define KEY_TIME_TOL_MS (KEY_SCAN_INTERAL_MS + 2)           // 按键长按允许误差ms

#define DROP_RELEASE_EVENT                          1       // 丢弃触发长按后的释放事件

#define ONLY_SINGLE_RELEASE                         1       // 释放事件改为只触发单键释放

#define LONG_RELEASE_EN                             1       // 开启长按释放事件，即按下足够的时间再释放的事件
#if LONG_RELEASE_EN
#define LONG_RELEASE_TIME_TOL_MS                    2000    // 释放事件判断上次长按的误差允许时间
#endif

#define REPEAT_EN                                   1       // 开启按键重复事件

#define LONG_CLICK_TIME_MS                          2 * 1000// 全局长按时间，大于该时间才会触发长按

#ifdef __cplusplus
}
#endif

#endif // APP_KEYBOARD_MODULE_KEY_CONFIG_H_
