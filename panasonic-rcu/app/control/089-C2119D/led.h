
#ifndef APP_VIEW_LED_H_
#define APP_VIEW_LED_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "led_bar.h"
#include "led_evt.h"

// led点亮时间，ms
#define LED_LIGHT_TIME LED_BAR_LIGHT_FOREVER

extern const LedConfig LED_EVT_LIGHT_FOREVER;
extern const LedConfig LED_EVT_BLINK_1Hz_FOREVER;
extern const LedConfig LED_EVT_LIGHT_3S;
extern LedBar g_ledBarTime;
extern LedBar g_ledBarMode;
extern LedBar g_ledBarFanSpeed;
void led_all_off();
#ifdef __cplusplus
}
#endif

#endif  // APP_VIEW_LED_H_
