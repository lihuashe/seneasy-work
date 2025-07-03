/******************************************************************************
 * @file     u_printf.h
 *
 * @brief    for TLSR chips
 *
 * @author   public@telink-semi.com;
 * @date     Sep. 30, 2010
 *
 * @attention
 *
 *  Copyright (C) 2019-2020 Telink Semiconductor (Shanghai) Co., Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *****************************************************************************/
#pragma once

#if (UART_PRINT_DEBUG_ENABLE || USB_PRINT_DEBUG_ENABLE || UART_PRINT_ENABLE)//print info by a gpio or usb printer
    int  u_printf(const char *fmt, ...);
    int  u_sprintf(char* s, const char *fmt, ...);
    void u_array_printf(unsigned char*data, unsigned int len);

    #define printf          u_printf
    #define sprintf         u_sprintf
    #define array_printf    u_array_printf
#else
    int dummy_print(const char *format, ...);  // to remove warnings
    void dummy_print2(unsigned char*data, unsigned int len);

    #define printf          dummy_print
    #define sprintf
    #define array_printf    dummy_print2
#endif

