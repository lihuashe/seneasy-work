/**
 * @file     usb_timeout.h
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2021/02/07
 * @history
 * @note
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2020, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 */

#ifndef __HW_USB_TIMEOUT_H__
#define __HW_USB_TIMEOUT_H__

#ifdef __cplusplus
 extern "C" {
#endif /*__cplusplus*/


//=====================================================================================================================
// TYPEDEF STRUCT
//=====================================================================================================================
typedef struct
{
    uint32_t    TimeOutVal;      // time out value
    uint32_t    TickValCache;    // soft timer setting value
    uint8_t     IsTimeOut;       // time out flag
    uint8_t     reserved1;
    uint8_t     reserved2;
    uint8_t     reserved3;

} stUSB_TIMER_t;




/**
 * @brief  Set time out value of softtimer.
 * @param  stUSB_TIMER_t*: timer.
 */
extern void rom_usb_set_timeout(stUSB_TIMER_t* pTimer, uint32_t timeout);


/**
 * @brief  Check whether time out.
 * @param  stUSB_TIMER_t* timer.
 * @return uint8_t: timeout return 1,
 *                  not timeout return 0.
 */
extern uint8_t rom_usb_is_timeout(stUSB_TIMER_t* pTimer);


/**
 * @brief  Wait us.
 * @param  uint32_t:u32Us.
 */
extern void rom_usb_wait_us(uint32_t u32Us);


/**
 * @brief  Wait ms.
 * @param  uint32_t:u32Ms.
 */
extern void rom_usb_wait_ms(uint32_t u32Ms);


/**
 * @brief  Get indicate timer past time.
 * @param  stUSB_TIMER_t* timer.
 * @return uint32_t: Past time.
 */
extern uint32_t rom_usb_get_past_time(stUSB_TIMER_t* pTimer);



#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*__HW_USB_TIMEOUT_H__*/

