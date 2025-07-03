/**
*********************************************************************************************************
*               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     pm_handle.h
* @brief    Related functions of battery management
* @details
* @author   lizefeng
* @date     2024-02-01
* @version  v1.0
*********************************************************************************************************
*/
#ifndef PM_HANDLE_H_
#define PM_HANDLE_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/

#include "driver.h"
#include "key_driver.h"
#include "pm.h"
extern volatile u32  dev_wake_time_tick;
extern void  dev_wake_function();
extern void kb_wake_config(void);
extern void stuck_dev_now_wake_function(void);
extern void dev_now_wake_function();
extern void gpio_high_z_config(void);
extern void key_pressed_check_event(void);
#endif
