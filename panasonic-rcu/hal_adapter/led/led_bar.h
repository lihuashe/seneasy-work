

#ifndef APP_MAIN_APPLICATION_LED_MOUDLE_LED_H_
#define APP_MAIN_APPLICATION_LED_MOUDLE_LED_H_

#include "led_driver.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define LED_BAR_MAX_LEN 6
#define LED_BAR_LIGHT_FOREVER 0

    typedef struct
    {
        unsigned char led[LED_BAR_MAX_LEN];
        char last_state[LED_BAR_MAX_LEN];
        unsigned char num;
        bool isBlink;
    } LedBar;

    extern LedBar g_ledBar;
    extern void led_bar_status_change(LedBar *lb, const char *p, unsigned short keep_time_ms);
    extern void led_bar_all_on(LedBar *lb, unsigned short keep_time_ms);
    extern void led_bar_all_off(LedBar *lb);
    extern void led_bar_light_l2r(LedBar *lb, unsigned char cnt, unsigned short time_ms);
    extern void led_bar_light_single(LedBar *lb, unsigned char n, unsigned short time_ms);
    extern void led_bar_light_single_ignore(LedBar *lb, unsigned char n, unsigned short time_ms);
    extern void led_bar_light_last_status(LedBar *lb, unsigned short time_ms);
    extern bool led_bar_is_light(LedBar *lb);

    void led_bar_blink(LedBar *lb, const char *p, unsigned short t_ms);
    void led_bar_stop_blink(LedBar *lb);

#ifdef __cplusplus
}
#endif

#endif // APP_MAIN_APPLICATION_LED_MOUDLE_LED_H_
