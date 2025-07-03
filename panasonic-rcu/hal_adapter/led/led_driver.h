
/**
 * @file led_driver.h
 * @author likeliang (likeliang@seneasy.com)
 * @brief
 * @version 0.1
 * @date 2025-03-05
 *
 *
 */
#ifndef APP_LED_MODULE_LED_DRIVER_H_
#define APP_LED_MODULE_LED_DRIVER_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include "types.h"
#include "py32f0xx_hal.h"

#define LDE_DRIVER_TIMER 0
#define LDE_DRIVER_LED_CTRL 1
#define LED_DRIVER_TYPE LDE_DRIVER_LED_CTRL
    // #define LED_DRIVER_TYPE         LDE_DRIVER_TIMER   // 注意：需要手动打开定时器中断回调

#define LED_NUM 20
#if LED_DRIVER_TYPE == LDE_DRIVER_TIMER
#define LED_ITEM(row, col) {0, false, row, col}
#define LED_MATRIX_ROW_CNT 4
#define LED_MATRIX_COL_CNT 5

    extern TIM_HandleTypeDef LED_TimHandle;

    typedef struct
    {
        unsigned char id;
        bool isOn;
        unsigned char row;
        unsigned char col;
    } Led;
    // led 开关具体实现
    void led_matrix_init();
    void led_matrix_ctrl(Led *this, bool _isOn);
#elif LED_DRIVER_TYPE == LDE_DRIVER_LED_CTRL

#define LED_ITEM(com, seg) {0, false, com, seg}
// 数码LED控制器的数据寄存器配置位置
#define LED_SEG_A 0 // 第1列矩阵led寄存器移位值
#define LED_SEG_B 1 // 第2列矩阵led寄存器移位值
#define LED_SEG_C 2 // 第3列矩阵led寄存器移位值
#define LED_SEG_D 3 // 第4列矩阵led寄存器移位值
#define LED_SEG_E 4 // 第4列矩阵led寄存器移位值

typedef struct
{
    unsigned char id;
    bool isOn;
    unsigned char com;
    unsigned char seg;
} Led;

typedef void (*led_ctrl)(Led *led, bool isOn);

#endif

    extern Led g_led[LED_NUM];

    // led 开关接口
    void led_init();
    void led_on(unsigned char id);
    void led_off(unsigned char id);
    void led_tim16_irq();
    void led_set_brightness(unsigned char id, int val);
    unsigned char led_get_brightness(unsigned char id);
    void led_toggle(unsigned char id);
    bool led_state(unsigned char id);
    void led_driver_sleep();
    void led_driver_wakeup();
#ifdef __cplusplus
}
#endif

#endif // APP_LED_MODULE_LED_DRIVER_H_
