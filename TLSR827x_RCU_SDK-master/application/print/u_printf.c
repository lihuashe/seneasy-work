/******************************************************************************
 * @file     u_printf.c
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

/*
 Copyright 2001, 2002 Georges Menie (www.menie.org)
 stdarg version contributed by Christian Ettinger

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 putchar is the only external dependency for this file,
 if you have a working putchar, leave it commented out.
 If not, uncomment the define below and
 replace outbyte(c) by your own function call.

 #define putchar(c) outbyte(c)
 */
#if 1
#include <stdarg.h>

#include "drivers.h"
extern int putchar(int c);
static void printchar(char **str, int c) {
    if (str) {
        **str = c;
        ++(*str);
    } else
        (void) putchar(c);
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

static int prints(char **out, const char *string, int width, int pad) {
    register int pc = 0, padchar = ' ';

    if (width > 0) {
        register int len = 0;
        register const char *ptr;
        for (ptr = string; *ptr; ++ptr)
            ++len;
        if (len >= width)
            width = 0;
        else
            width -= len;
        if (pad & PAD_ZERO)
            padchar = '0';
    }
    if (!(pad & PAD_RIGHT)) {
        for (; width > 0; --width) {
            printchar(out, padchar);
            ++pc;
        }
    }
    for (; *string; ++string) {
        printchar(out, *string);
        ++pc;
    }
    for (; width > 0; --width) {
        printchar(out, padchar);
        ++pc;
    }

    return pc;
}

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

static int printi(char **out, int i, int b, int sg, int width, int pad,
        int letbase) {
    char print_buf[PRINT_BUF_LEN];
    register char *s;
    register int t, neg = 0, pc = 0;
    register unsigned int u = i;

    if (i == 0) {
        print_buf[0] = '0';
        print_buf[1] = '\0';
        return prints(out, print_buf, width, pad);
    }

    if (sg && b == 10 && i < 0) {
        neg = 1;
        u = -i;
    }

    s = print_buf + PRINT_BUF_LEN - 1;
    *s = '\0';

    while (u) {
        t = u % b;
        if (t >= 10)
            t += letbase - '0' - 10;
        *--s = t + '0';
        u /= b;
    }

    if (neg) {
        if (width && (pad & PAD_ZERO)) {
            printchar(out, '-');
            ++pc;
            --width;
        } else {
            *--s = '-';
        }
    }

    return pc + prints(out, s, width, pad);
}

static int print(char **out, const char *format, va_list args) {
    register int width, pad;
    register int pc = 0;
    char scr[2];

    for (; *format != 0; ++format) {
        if (*format == '%') {
            ++format;
            width = pad = 0;
            if (*format == '\0')
                break;
            if (*format == '%')
                goto out;
            if (*format == '-') {
                ++format;
                pad = PAD_RIGHT;
            }
            while (*format == '0') {
                ++format;
                pad |= PAD_ZERO;
            }
            for (; *format >= '0' && *format <= '9'; ++format) {
                width *= 10;
                width += *format - '0';
            }
            if (*format == 's') {
                register char *s = (char *) va_arg( args, int );
                pc += prints(out, s ? s : "(null)", width, pad);
                continue;
            }
            if (*format == 'd') {
                pc += printi(out, va_arg( args, int ), 10, 1, width, pad, 'a');
                continue;
            }
            if (*format == 'x') {
                pc += printi(out, va_arg( args, int ), 16, 0, width, pad, 'a');
                continue;
            }
            if (*format == 'X') {
                pc += printi(out, va_arg( args, int ), 16, 0, width, pad, 'A');
                continue;
            }
            if (*format == 'u') {
                pc += printi(out, va_arg( args, int ), 10, 0, width, pad, 'a');
                continue;
            }
            if (*format == 'c') {
                /* char are converted to int then pushed on the stack */
                scr[0] = (char) va_arg( args, int );
                scr[1] = '\0';
                pc += prints(out, scr, width, pad);
                continue;
            }
        } else {
            out: printchar(out, *format);
            ++pc;
        }
    }
    if (out)
        **out = '\0';
    va_end( args );
    return pc;
}

#if (UART_PRINT_ENABLE)
extern void app_set_printf_uart_flag(unsigned char data);
#endif

char uart_log_buff[256];
int u_printf(const char *format, ...) {
    int len;

    #if (UART_PRINT_ENABLE)
    app_set_printf_uart_flag(1);
    #endif
    va_list args;
    va_start( args, format );
    // status = print(0, format, args);
    char * p_str = uart_log_buff;
    len = print(&p_str, format, args);
    while((reg_uart_buf_cnt & FLD_UART_TX_BUF_CNT) != 0);   // 等待上一次缓冲区内容发送完成
    uart_ndma_clear_tx_index();     // 重置发送计数
    reg_uart_status0 = 0xC0;
    for(int i = 0; i < len; i++) {
        uart_ndma_send_byte(uart_log_buff[i]);
    }
    #if (UART_PRINT_ENABLE)
    app_set_printf_uart_flag(0);
    #endif
    return len;
}

int dummy_print(const char *format, ...)  // to remove warnings
{
    return 0;
}


int u_sprintf(char *out, const char *format, ...) {
    va_list args;
    va_start( args, format );
    return print(&out, format, args);
}

void dummy_print2(unsigned char*data, unsigned int len) {
}

void u_array_printf(unsigned char*data, unsigned int len) {
    u_printf("{");
    for(int i = 0; i < len; ++i){
        u_printf("%X%s", data[i], i<(len)-1? ":":" ");
    }
    u_printf("}\n");
}

#endif

