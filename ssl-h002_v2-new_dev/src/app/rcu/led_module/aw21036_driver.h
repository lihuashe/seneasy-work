/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     aw21036_driver.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef AW21036_DRIVER_H_
#define AW21036_DRIVER_H_

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <swtimer.h>
#include <rtl876x.h>
#include "rtl876x_rcc.h"
#include "rtl876x_pinmux.h"
#include "i2c_init.h"
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
typedef uint8_t u8;

#define LED_AW21036_ENABLE                      1
	
                  
#define AW21036_CHIP_LEN_EN                     P1_0


#define AW210XX_CHIPID				0x18
#define AW21036_LED_NUM				36

#define AW_LED_ON                   1
#define AW_LED_OFF                  0

extern u8 LED_BR0_Data;
extern u8 LED_COL0_Data;
extern u8 Quick_WaitMs_Flag;
extern u8 BackLight_Connect_Init;
extern bool BR_TEST_CLOSE_FLAG;
extern bool BR_TEST_LED_DUTY_FLAG;
/*============================================================================*
 *                          Functions
 *============================================================================*/
extern void aw21036_init(void);
extern void aw21036_Disable(void);
extern void aw21036_all_on(u8 set_br0, u8 set_col0);
extern void aw21036_all_off(void);
extern void aw21036_all_breath(void);
extern void aw21036_breath_off(void);
extern void aw21036_breath_one(u8 led_cnt, u8 led_flag, u8 col_val, u8 br_val);
extern void aw21036_onoff_one(u8 led_cnt, u8 led_flag, u8 col_val, u8 br_val);

extern void aw21036_set_brightness(u8 index, u8 brightness);
extern void aw21036_set_color(u8 index, u8 color);
extern void aw21036_update_brightness_to_display(void);

extern void aw21036_breath_quick(u8 led_flag);
extern void aw21036_init_TEST(void);
/************************************************************
*               led_timer_T0 --- Ramp rise time
*               led_timer_T1 --- Hold on   time
*               led_timer_T2 --- Ramp fall time
*               led_timer_T3 --- Hold off  time

        T0/T2       Time      |     T0/T2       Time            
        0x00        0.00s     |     0x80        2.1s
        0x10        0.13s     |     0x90        2.6s
        0x20        0.26s     |     0xA0        3.1s
        0x30        0.38s     |     0xB0        4.2s
        0x40        0.51s     |     0xC0        5.2s
        0x50        0.77s     |     0xD0        6.2s
        0x60        1.04s     |     0xE0        7.3s
        0x70        1.60s     |     0xF0        8.3s     
 ***********************************************************/
#define led_timer_T0            0x10
#define led_timer_T1            0x01
#define led_timer_T2            0x60
#define led_timer_T3            0x06
#define led_on_wait_ms          1//1130
#define led_off_wait_ms         1//2100

#define led_timer_key_T2        0x10
#define led_timer_key_T3        0x01
#define led_off_key_wait_ms     1//130

/************************************************************
*  LED_Backlight_Level
*  level: 0--6
 ***********************************************************/
#define BR_LUX_LEVEL_0          71      //30      //<30 Lux
#define BR_LUX_LEVEL_1          234     //100     //30 -100 Lux
#define BR_LUX_LEVEL_2          468     //200     //100-200 Lux
#define BR_LUX_LEVEL_3          704     //300     //200-300 Lux
#define BR_LUX_LEVEL_4          1204    //500     //300-500 Lux
#define BR_LUX_LEVEL_5          1679    //700     //500-700 Lux



#endif /* AW21036_DRIVER_H_ */
