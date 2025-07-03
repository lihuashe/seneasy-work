/*
 * Copyright (c) 2023 Shanghai Panchip Microelectronics Co., Ltd. All Rights Reserved.
 * @Date: 2023-05-22 17:15:46
 * @LastEditors: xuhaojie xuhaojie@panchip.com
 * @LastEditTime: 2023-05-24 17:09:48
 * @FilePath: \hc21l136_pan216\pan216a\log.h
 * @Description:
 * @History:
 */
#ifndef LOG_H
#define LOG_H

#include "stdio.h"

#define DBG_LEVEL     0

#define DBG_LOG       4
#define DBG_INFO      3
#define DBG_WARNING   2
#define DBG_ERROR     1

#define _DBG_LOG_HDR(lvl_name) \
    printf("[" lvl_name "]%s: ", __func__);

#define _DBG_LOG_END() \
    printf("\r\n");

#define dbg_log_line(lvl, fmt, ...) \
    do                              \
    {                               \
        _DBG_LOG_HDR(lvl);          \
        printf(fmt, ##__VA_ARGS__); \
        _DBG_LOG_END()              \
    } while (0)

#if (DBG_LEVEL >= DBG_LOG)
#define PanLog_D(fmt, ...) dbg_log_line("Debug", fmt, ##__VA_ARGS__)
#else
#define PanLog_D(...)
#endif

#if (DBG_LEVEL >= DBG_INFO)
#define PanLog_I(fmt, ...) dbg_log_line("Info", fmt, ##__VA_ARGS__)
#else
#define PanLog_I(...)
#endif

#if (DBG_LEVEL >= DBG_WARNING)
#define PanLog_W(fmt, ...) dbg_log_line("Warning", fmt, ##__VA_ARGS__)
#else
#define PanLog_W(...)
#endif

#if (DBG_LEVEL >= DBG_ERROR)
#define PanLog_E(fmt, ...) dbg_log_line("Error", fmt, ##__VA_ARGS__)
#else
#define PanLog_E(...)
#endif

#define dbg_log_hex(lvl, width, buf, size) \
    do                                     \
    {                                      \
        _DBG_LOG_HDR(lvl);                 \
        panlog_hexdump(width, buf, size);  \
        _DBG_LOG_END()                     \
    } while (0)

#if (DBG_LEVEL >= DBG_LOG)
#define PanLog_HEX(buf, size) dbg_log_hex("Hex", 8, buf, size)
#else
#define PanLog_HEX(...)
#endif

void panlog_hexdump(size_t width, const unsigned char *buf, size_t size);
void printHex(unsigned char *buf, int len);
    
#endif
