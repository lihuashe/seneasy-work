/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     led_driver.h
* @brief
* @details
* @author
* @date     2020-03-04
* @version  v1.0
* @note
*********************************************************************************************************
*/
#ifndef _LED_H_
#define _LED_H_

#include "stdint.h"
#include "board.h"

#if SUPPORT_LED_INDICATION_FEATURE
/**
*   @brief user guide for led driver
*   The driver support multi prio, multi event, and multi led.
*   If you want to control a led with a event as you designed, there is two way to
*   achieve this, modify the exist event and redefine a new event.
*
*   The led driver is drived by software timer, the timer peroid is 50ms by default.
*
*   #define  LED_PERIOD   50 //you an change the value according your requirement
*
*   For the whole led driver system, there is 5 place you might modify.
*
*   1. Define led num and led pin, which is in file board.h;
*      For example, define 2 leds as followed:
*      #define  LED_NUM_MAX   0x02
*      #define  LED_INDEX(n)   (n<<8)

*      //uint16_t, first byte led index, last byte led pin
*      #define  LED_1         (LED_INDEX(0) | P2_2)
*      #define  LED_2         (LED_INDEX(1) | P0_2)
*
*   2. Define led type index, which is in file led_driver.h, these types must be
*      defined in sequence.
*      For example,
*      #define  LED_TYPE_BLINK_OTA_UPDATE       (8)
*
*   3. Define led loop bits num, in file led_driver.h
*      //max value 32, min value 1, it indicate that there is how many bits to loop
*      //from LSB
*      #define LED_LOOP_BITS_OTA_UPDATE          (15)
*
*   4. Define led event bit map, in file led_driver.h
*      // it must be cooperated with led loop bits num
*      #define  LED_BIT_MAP_OTA_UPDATE       (0x4210)//on 50ms, off 200ms, period 50ms
*
*   5. Update led event table, led_event_arr[LED_TYPE_MAX], in file led_driver.c
*      Before you use the event you define ,you need to add led type, led loop bit num,
*      and led event bit map into event table.
*      const T_LED_EVENT_STG led_event_arr[LED_TYPE_MAX] =
*      {
*                ··· ···
*        {LED_TYPE_BLINK_OTA_UPDATE, LED_LOOP_BITS_OTA_UPDATE,  LED_BIT_MAP_OTA_UPDATE},
*      };
*
*   There are three interfaces for Led driver, as follow.
*
*   void led_module_init(void); // called when system boot;
*   T_LED_RET_CAUSE led_blink_start(uint16_t index, LED_TYPE type, uint8_t cnt);
*   T_LED_RET_CAUSE led_blink_exit(uint16_t index, LED_TYPE type);
*/

/*software timer period*/
#define  LED_PERIOD   50  /*50ms*/
#define  LED_CHANGE_PERIOD  60  /*60ms*/
#define  LED_DEBUG     0

/*led event types, the less value, the higher prio of the event*/
typedef   uint32_t      LED_TYPE;

typedef enum
{
    LED_TYPE_IDLE,
    LED_TYPE_RESTART,
    LED_TYPE_POWER_ON,
    LED_TYPE_BLINK_LOW_POWER,
    LED_TYPE_BLINK_IR_LEARN_SUCCESS,
    LED_TYPE_BLINK_IR_LEARN_WAITING,
    LED_TYPE_BLINK_IR_LEARN_MODE,
    LED_TYPE_BLINK_PAIR_ADV,
    LED_TYPE_BLINK_PAIR_SUCCESS,
    LED_TYPE_BLINK_FACTORY_RESET,
    LED_TYPE_BLINK_WARNING,
    LED_TYPE_BLINK_KEY_PRESS,
    LED_TYPE_ON,
    LED_TYPE_MAX
} LED_TYPE_ENUM;

/*led loop bit num for each event, max value 32*/
#define   LED_LOOP_BITS_IDLE                      32
#define   LED_LOOP_BITS_RESTART                   20
#define   LED_LOOP_BITS_POWER_ON                  10
#define   LED_LOOP_BITS_LOW_POWER                 3
#define   LED_LOOP_BITS_IR_LEARN_SUCCESS          4
#define   LED_LOOP_BITS_IR_LEARN_WAITING          32
#define   LED_LOOP_BITS_IR_LEARN_MODE             20
#define   LED_LOOP_BITS_PAIR_ADV                  8
#define   LED_LOOP_BITS_PAIR_SUCCESS              20
#define   LED_LOOP_BITS_FACTORY_RESET             4
#define   LED_LOOP_BITS_WARNING                   4
#define   LED_LOOP_BITS_KEY_PRESS                 3
#define   LED_LOOP_BITS_ON                        32

/*led bit map 32bits, High bits(low priority) ---  Low bits(high priority) */
#define LED_BIT_MAP_INVALID                0x00000000
#define LED_BIT_MAP_RESTART                0x000fffff
#define LED_BIT_MAP_POWER_ON               0x0000001f
#define LED_BIT_MAP_LOW_POWER              0x00000003
#define LED_BIT_MAP_IR_LEARN_SUCCESS       0x00000002
#define LED_BIT_MAP_IR_LEARN_WAITING       0xffffffff
#define LED_BIT_MAP_IR_LEARN_MODE          0x0000001f
#define LED_BIT_MAP_PAIR_ADV               0x000000F0
#define LED_BIT_MAP_PAIR_SUCCESS           0xFFC00
#define LED_BIT_MAP_FACTORY_RESET          0x00000003
#define LED_BIT_MAP_WARNING                0x00000003
#define LED_BIT_MAP_KEY_PRESS              0x00000003
#define LED_BIT_MAP_ON                     0xffffffff

typedef struct
{
    uint8_t   led_type_index;
    uint8_t   led_loop_cnt;
    uint32_t  led_bit_map;
}  T_LED_EVENT_STG;

/*struct support for led blink count*/
typedef struct
{
    uint8_t led_loop_cnt;
    uint8_t cur_tick_cnt;
} T_LED_CNT_STG;

/*support for each led data*/
typedef struct
{
    uint8_t led_index;
    uint8_t led_pin;
    T_LED_CNT_STG led_cnt_arr[LED_TYPE_MAX];
    uint32_t led_map;
} T_LED_DATA_STG;

/*led return code*/
typedef enum
{
    LED_SUCCESS                      = 0,
    LED_ERR_TYPE                     = 1,
    LED_ERR_INDEX                    = 2,
    LED_ERR_CODE_MAX
} T_LED_RET_CAUSE;

/*============================================================================*
 *                        <Led Module Interface>
 *============================================================================*/
void led_module_init(void);
T_LED_RET_CAUSE led_blink_start(uint16_t index, LED_TYPE type, uint8_t cnt);
T_LED_RET_CAUSE led_blink_exit(uint16_t index, LED_TYPE type);

#define LED_ON(index)                   led_blink_start(index, LED_TYPE_ON, 0)
#define LED_OFF(index)                  led_blink_exit(index, LED_TYPE_ON)
#define LED_BLINK(index, type, n)       led_blink_start(index, type, n)
#define LED_BLINK_EXIT(index, type)     led_blink_exit(index, type)

#endif

#endif

/******************* (C) COPYRIGHT 2020 Realtek Semiconductor Corporation *****END OF FILE****/
