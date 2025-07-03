
/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2018 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef _APP_TASK_H_
#define _APP_TASK_H_

#include "app_msg.h"
#include "stdbool.h"
#include "uart_driver.h"
#include "platform_utils.h"
#include "rtl876x_nvic.h"
#include "os_sched.h"
#include "aw21036_driver.h"
#include "rtl876x_gpio.h"
#include "os_sync.h"
#include "battery_driver.h"


extern void *app_task_handle;
extern void *evt_queue_handle;
extern void *io_queue_handle;
extern void *product_test_io_queue_handle;
extern void *i2c_mutex_handle;

void app_task_init(void);
bool app_send_msg_to_apptask(T_IO_MSG *p_msg);

#endif

