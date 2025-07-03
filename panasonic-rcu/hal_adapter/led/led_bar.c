#include "led_bar.h"
#include "led_evt.h"
#include "time_utils.h"
#include <string.h>
// TODO BUG led点亮单个灯时长度控制有问题
/*============================================================================*
 *                              Global Variables
 *============================================================================*/

/*============================================================================*
 *                              Local Functions
 *============================================================================*/

/**
 * @brief 计算字符串p中点亮的LED灯数量
 *
 * @param lb LED Bar
 * @param p LED状态的字符串指针，“00100”为只有第三个灯亮
 * @return  字符串p中点亮的LED灯数量
 */
u8 led_bar_get_light_num(LedBar *lb, const char *p)
{
    u8 count = 0;
    for (int i = 0; i < lb->num && p; i++, p++)
        if (*p != '0')
            count++;
    return count;
}

/*============================================================================*
 *                              Global Functions
 *============================================================================*/

/**
 * @brief 根据字符串p中点亮或关闭对应的LED
 *
 * @param lb LED Bar
 * @param p LED状态的字符串的指针，“00100”为只有第三个灯亮
 * @param keep_time_ms 保持时间
 * @return none
 */
void led_bar_status_change(LedBar *lb, const char *p, unsigned short keep_time_ms)
{
    // 保存led新状态
    if (led_bar_get_light_num(lb, p))
        strncpy(lb->last_state, (void *)p, strlen(p) < lb->num ? strlen(p) : lb->num);

    for (int i = 0; i < lb->num && *p; i++, p++)
    {
        if (*p != '0')
        {
            if (keep_time_ms)
            {
                LedConfig lc;
                memset(&lc, 0, sizeof(lc));
                lc.onTime_ms = keep_time_ms;
                lc.repeatCount = 1;
                led_evt_setup(lb->led[i], lc);
            }
            else
            {
                led_evt_reset(lb->led[i]);
                led_on(lb->led[i]);
            }
        }
        else
        {
            led_evt_reset(lb->led[i]);
        }
    }
}

/**
 * @brief 立刻打开所有LED
 *
 * @param lb LED Bar
 * @param keep_time_ms 保持时间LED打开的时间，为了0时led不会自动熄灭
 * @return none
 */
void led_bar_all_on(LedBar *lb, unsigned short keep_time_ms)
{
    for (size_t i = 0; i < lb->num; i++)
    {
        LedConfig lc;
        memset(&lc, 0, sizeof(lc));
     
        if (keep_time_ms)
        {
            LedConfig lc;
            memset(&lc, 0, sizeof(lc));
            lc.onTime_ms = keep_time_ms;
            lc.repeatCount = 1;
            led_evt_setup(lb->led[i], lc);
        }
        else
        {
            led_evt_reset(lb->led[i]);
            led_on(lb->led[i]);
        }
    }
}

/**
 * @brief 立刻关掉所有LED
 *
 * @param lb LED Bar
 * @return
 */
void led_bar_all_off(LedBar *lb)
{
    led_bar_stop_blink(lb);
}

/**
 * @brief 从左往右点亮cnt个灯，第cnt后的灯会熄灭
 *
 * @param lb LED Bar
 * @param cnt
 * @param time_ms 点亮时间
 * @return {none}
 */
void led_bar_light_l2r(LedBar *lb, unsigned char cnt, unsigned short time_ms)
{
    // TODO BUG
    char led_state[] = "000000";
    led_state[lb->num] = '\0';
    for (int i = 0; i < cnt && i < lb->num; i++)
        led_state[i] = '1';
    led_bar_status_change(lb, led_state, time_ms);
}

/**
 * @brief 单独点亮第n个灯，其他灯会熄灭
 *
 * @param lb LED Bar
 * @param n 单独点亮第n个灯，其他灯会熄灭，大于LED数量无效
 * @param time_ms 点亮时间
 * @return {none}
 */
void led_bar_light_single(LedBar *lb, unsigned char n, unsigned short time_ms)
{
    // TODO BUG
    if (n > lb->num || n < 1)
        return;
    char led_state[] = "000000";
    led_state[n - 1] = '1';
    led_state[lb->num] = '\0';
    led_bar_status_change(lb, led_state, time_ms);
}

/**
 * @brief 点亮第n个灯，其他灯保持原状
 *
 * @param lb LED Bar
 * @param n 大于LED数量无效
 * @param time_ms 点亮时间，其他灯的时间也会刷新为该时间
 * @return
 */
void led_bar_light_single_ignore(LedBar *lb, unsigned char n, unsigned short time_ms)
{
    if (n > lb->num || n < 1)
        return;
    char led_state[6];
    strncpy(led_state, lb->last_state, lb->num);
    led_state[n - 1] = '1';
    led_bar_status_change(lb, led_state, time_ms);
}

void led_bar_light_last_status(LedBar *lb, unsigned short time_ms)
{
    led_bar_status_change(lb, lb->last_state, time_ms);
}

bool led_bar_is_light(LedBar *lb)
{
    if (lb->isBlink)
    {
        return true;
    }
    
    for (size_t i = 0; i < lb->num; i++)
    {
        if (led_state(lb->led[i]))
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief LED闪烁
 *
 * @param lb LED Bar
 * @param p LED点亮字符串
 * @param time_ms 闪烁间隔
 * @return
 */
void led_bar_blink(LedBar *lb, const char *p, unsigned short time_ms)
{
    if (!time_ms)
        return;
    lb->isBlink = true;
    for (int i = 0; i < lb->num && *p; i++, p++)
    {
        if (*p != '0')
        {
            LedConfig lc;
            memset(&lc, 0, sizeof(lc));
            lc.onTime_ms = time_ms;
            lc.offTime_ms = time_ms;
            lc.repeatCount = LED_EVT_FOREVER;
            led_evt_setup(lb->led[i], lc);
        }
        else
        {
            led_evt_reset(lb->led[i]);
            led_off(lb->led[i]);
        }
    }
}

/**
 * @brief
 *
 * @param lb LED Bar
 */
void led_bar_stop_blink(LedBar *lb)
{
    lb->isBlink = false;
    for (int i = 0; i < lb->num; i++)
    {
        led_evt_reset(lb->led[i]);
    }
}
