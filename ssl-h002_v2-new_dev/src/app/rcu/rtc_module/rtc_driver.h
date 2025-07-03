/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtc_driver.h
* @details
* @author   chenjie_jin
* @date     2021-01-09
* @version  v1.0
*********************************************************************************************************
*/

#ifndef _RTC_DRIVER_H_
#define _RTC_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <board.h>

/*============================================================================*
 *                              Macro Definitions
 *============================================================================*/

/*============================================================================*
 *                              Types
 *============================================================================*/
typedef void (*rtc_driver_raw_cb)(void);

/*============================================================================*
 *                       Interface Functions
 *============================================================================*/
void rtc_driver_init(void);
void rtc_driver_deinit(void);
uint64_t rtc_driver_get_us(void);
uint64_t rtc_driver_get_ms(void);
uint32_t rtc_driver_get_seconds(void);
bool rtc_driver_set_comp_0_params(uint32_t time_ms, rtc_driver_raw_cb cb_func);
void rtc_driver_clear_comp_0(void);

#ifdef __cplusplus
}
#endif

#endif

/******************* (C) COPYRIGHT 2020 Realtek Semiconductor Corporation *****END OF FILE****/
