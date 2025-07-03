
#ifndef APP_MY_LOG_H_
#define APP_MY_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define __DEBUG_CLOSE__ 0
#define __DEBUG_ERR__   1
#define __DEBUG_WARN__  2
#define __DEBUG_INFO__  3
#define __DEBUG_DEV__   4


#ifndef __DEBUG__
#define __DEBUG__ __DEBUG_CLOSE__
#endif

#include "SEGGER_RTT.h"

#if __DEBUG__ == __DEBUG_CLOSE__
#define my_log(fmt, ...) 
#else
#define my_log(fmt, ...) SEGGER_RTT_printf(0, fmt, ##__VA_ARGS__)
#endif

#define printf my_log

#ifndef my_log
#define my_log printf
#endif

#ifdef __cplusplus
}
#endif

#endif  // APP_MY_LOG_H_
