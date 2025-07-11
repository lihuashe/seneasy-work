/*
 * @Author: lihuashe lihuashe@seneasy.com
 * @Date: 2024-08-06 13:42:20
 * @LastEditors: lihuashe lihuashe@seneasy.com
 * @LastEditTime: 2024-08-30 09:11:53
 * @FilePath: \13.SRC-1116-6254-jomoo\chip\B80\drivers\driver.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/********************************************************************************************************
 * @file	driver.h
 *
 * @brief	This is the header file for B80
 *
 * @author	Driver Group
 * @date	2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#pragma once

#include "driver_func_cfg.h"

#include "app_emi.h"
#include "emi.h"
#include "lib/include/pm.h"
#include "lib/include/random.h"
#include "lib/include/rf_drv.h"
#include "lib/include/swire.h"
#include "lib/include/otp/otp.h"

#include "bsp.h"
#include "aes.h"
#include "analog.h"
#include "compiler.h"
#include "register.h"
#include "gpio_default.h"
#include "gpio.h"
#include "pwm.h"
#include "irq.h"
#include "clock.h"
#include "flash.h"
#include "adc.h"
#include "i2c.h"
#include "spi.h"
#include "uart.h"
#include "register.h"
#include "watchdog.h"
#include "register.h"
#include "dfifo.h"
#include "dma.h"
#include "timer.h"
#include "s7816.h"
#include "printf.h"
#include "usbhw.h"
#include "ir_learn.h"
#include "qdec.h"
#include "keyscan.h"
#include "flash/flash_type.h"
#include "stimer.h"
#include "sdk_version.h"
#include "types.h"
#include "bath_rcu.h"
#include "utility.h"
#include "rf_base.h"
#include "pwm_handle.h"
#include "pm_handle.h"
#include "adc_handle.h"
#include "factory_test.h"
#include "rf_drv.h"
#include "key_driver.h"
#include "utility.h"
#include "system.h"
#include "my_key.h"
#include "my_led.h"
