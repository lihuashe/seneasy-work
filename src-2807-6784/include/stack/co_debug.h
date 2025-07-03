/*********************************************************************
 * @file co_debug.h
 * @brief 
 * @version 1.0
 * @date 14/11/3 19:28:20
 * @author XIHAO
 *
 * @note 
 */

#ifndef __CO_DEBUG_H__
#define __CO_DEBUG_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "features.h"
#include <stdint.h>
#include <stdio.h>

/*********************************************************************
 * MACROS
 */

#if defined(CONFIG_LOG_OUTPUT) || defined(ENABLE_DEBUG_LOG_SUPPORT)

#define dprintf(value)		printf(#value"= %d\r\n",value);

/* Log tag name */
#ifdef CONFIG_APP_LOG_SHOW_TAGS
#define APP_LOG_TAG_ERR         "[ERR]"
#define APP_LOG_TAG_WRN         "[WRN]"
#define APP_LOG_TAG_INF         "[INF]"
#define APP_LOG_TAG_DBG         "[DBG]"
#define APP_LOG_TAG_VBS         "[VBS]"
#else
#define APP_LOG_TAG_ERR         ""
#define APP_LOG_TAG_WRN         ""
#define APP_LOG_TAG_INF         ""
#define APP_LOG_TAG_DBG         ""
#define APP_LOG_TAG_VBS         ""
#endif /* CONFIG_APP_LOG_SHOW_TAGS */


/* Log level value */
#ifndef APP_LOG_LEVEL_OFF
#define APP_LOG_LEVEL_OFF       0
#endif

#ifndef APP_LOG_LEVEL_ERROR
#define APP_LOG_LEVEL_ERROR     1
#endif

#ifndef APP_LOG_LEVEL_WARNING
#define APP_LOG_LEVEL_WARNING   2
#endif

#ifndef APP_LOG_LEVEL_INFO
#define APP_LOG_LEVEL_INFO      3
#endif

#ifndef APP_LOG_LEVEL_DEBUG
#define APP_LOG_LEVEL_DEBUG     4
#endif

#ifndef APP_LOG_LEVEL_VERBOSE
#define APP_LOG_LEVEL_VERBOSE   5
#endif

#ifndef APP_LOG_LEVEL
#define APP_LOG_LEVEL                        APP_LOG_LEVEL_OFF
#endif


// debug output
#if APP_LOG_LEVEL >= APP_LOG_LEVEL_ERROR
#define APP_LOG_ERR(format, ...)                    do{printf("%s[%s] ", APP_LOG_TAG_ERR, APP_LOG_DOMAIN); printf(format,  ## __VA_ARGS__);}while(0)
#define APP_LOG_ERR_ARRAY(info,array, len)               do{printf("%s:",info);int __i; for(__i=0;__i<(len);++__i)printf("%02X ",((uint8_t *)(array))[__i]);printf("\r\n");}while(0)
#define APP_LOG_ERR_FUNC()                          do{APP_LOG_ERR("%s @ %d \r\n", __FUNCTION__, __LINE__);}while(0)
#define APP_LOG_ERR_NULL()                          do{printf("\n");}while(0)
#else
#define APP_LOG_ERR(format, ...)
#define APP_LOG_ERR_ARRAY(info,array, len)
#define APP_LOG_ERR_FUNC()
#define APP_LOG_ERR_NULL()
#endif

#if APP_LOG_LEVEL >= APP_LOG_LEVEL_WARNING
#define APP_LOG_WRN(format, ...)                    do{printf("%s[%s] ", APP_LOG_TAG_WRN, APP_LOG_DOMAIN); printf(format,  ## __VA_ARGS__);}while(0)
#define APP_LOG_WRN_ARRAY(info,array, len)               do{printf("%s:",info);int __i; for(__i=0;__i<(len);++__i)printf("%02X ",((uint8_t *)(array))[__i]);printf("\r\n");}while(0)
#define APP_LOG_WRN_FUNC()                          do{APP_LOG_WRN("%s @ %d \r\n", __FUNCTION__, __LINE__);}while(0)
#define APP_LOG_WRN_NULL()                          do{printf("\n");}while(0)
#else
#define APP_LOG_WRN(format, ...)
#define APP_LOG_WRN_ARRAY(info,array, len)
#define APP_LOG_WRN_FUNC()
#define APP_LOG_WRN_NULL()
#endif

#if APP_LOG_LEVEL >= APP_LOG_LEVEL_INFO
#define APP_LOG_INF(format, ...)                    do{printf("%s[%s] ", APP_LOG_TAG_INF, APP_LOG_DOMAIN); printf(format,  ## __VA_ARGS__);}while(0)
#define APP_LOG_INF_ARRAY(info,array, len)               do{printf("%s:",info);int __i; for(__i=0;__i<(len);++__i)printf("%02X ",((uint8_t *)(array))[__i]);printf("\r\n");}while(0)
#define APP_LOG_INF_FUNC()                          do{APP_LOG_INF("%s @ %d\r\n", __FUNCTION__, __LINE__);}while(0)
#define APP_LOG_INF_NULL()                          do{printf("\n");}while(0)
#else
#define APP_LOG_INF(format, ...)
#define APP_LOG_INF_ARRAY(info,array, len)
#define APP_LOG_INF_FUNC()
#define APP_LOG_INF_NULL()
#endif


#if APP_LOG_LEVEL >= APP_LOG_LEVEL_DEBUG
#define APP_LOG_DBG(format, ...)                    do{printf("%s[%s] ", APP_LOG_TAG_DBG, APP_LOG_DOMAIN); printf(format,  ## __VA_ARGS__);}while(0)
#define APP_LOG_DBG_ARRAY(info,array, len)               do{printf("%s:",info);int __i; for(__i=0;__i<(len);++__i)printf("%02X ",((uint8_t *)(array))[__i]); printf("\r\n");}while(0)
#define APP_LOG_DBG_FUNC()                          do{APP_LOG_DBG("%s @ %d \r\n", __FUNCTION__, __LINE__);}while(0)
#define APP_LOG_DBG_NULL()                          do{printf("\n");}while(0)
#else
#define APP_LOG_DBG(format, ...)
#define APP_LOG_DBG_ARRAY(info,array, len)
#define APP_LOG_DBG_FUNC()
#define APP_LOG_DBG_NULL()
#endif

#if APP_LOG_LEVEL >= APP_LOG_LEVEL_VERBOSE
#define APP_LOG_VBS(format, ...)                    do{printf("%s[%s] ", APP_LOG_TAG_DBG, APP_LOG_DOMAIN); printf(format,  ## __VA_ARGS__);}while(0)
#define APP_LOG_VBS_ARRAY(info,array, len)          do{printf("%s:",info);int __i; for(__i=0;__i<(len);++__i)printf("%02X ",((uint8_t *)(array))[__i]);printf("\r\n")}while(0)
#define APP_LOG_VBS_FUNC()                          do{APP_LOG_VBS("%s @ %d \r\n", __FUNCTION__, __LINE__);}while(0)
#define APP_LOG_VBS_NULL()                          do{printf("\n");}while(0)
#else
#define APP_LOG_VBS(format, ...)
#define APP_LOG_VBS_ARRAY(info,array, len)
#define APP_LOG_VBS_FUNC()
#define APP_LOG_VBS_NULL()
#endif

#else
#define APP_LOG_ERR(format, ...)
#define APP_LOG_ERR_ARRAY(info,array, len)
#define APP_LOG_ERR_FUNC()
#define APP_LOG_ERR_NULL()

#define APP_LOG_WRN(format, ...)
#define APP_LOG_WRN_ARRAY(info,array, len)
#define APP_LOG_WRN_FUNC()
#define APP_LOG_WRN_NULL()

#define APP_LOG_INF(format, ...)
#define APP_LOG_INF_ARRAY(info,array, len)
#define APP_LOG_INF_FUNC()
#define APP_LOG_INF_NULL()

#define APP_LOG_DBG(format, ...)
#define APP_LOG_DBG_ARRAY(info,array, len)
#define APP_LOG_DBG_FUNC()
#define APP_LOG_DBG_NULL()

#define APP_LOG_VBS(format, ...)
#define APP_LOG_VBS_ARRAY(info,array, len)
#define APP_LOG_VBS_FUNC()
#define APP_LOG_VBS_NULL()
#endif



// Compile Time Asserts
#define co_static_assert(exp)           extern char co_static_assert_failed[(exp)?1:-1]

#define CO_DEBUG_PRINT_BUFFER_SIZE      256
#define CO_DEBUG_PRINT_DOS_FORMAT       1 //The printf 'enter' use DOS(\r\n) or UNIX(\n)
#define CO_DEBUG_RECEIVE_BUFFER_SIZE    80
#define CO_DEBUG_RECEIVE_BOUNDARY_CODE  "\r\n"
#define CO_DEBUG_RECEIVE_BOUNDARY_LEN   2

#if defined(CONFIG_LOG_OUTPUT) && 0
#define TRACE_EVT(format, ...)          log_debug("[EVT] "format"\n",  ## __VA_ARGS__)
#define TRACE_MSG(format, ...)          log_debug("[MSG] "format"\n",  ## __VA_ARGS__)
#define TRACE_TIMER(format, ...)        log_debug("[TMR] "format"\n",  ## __VA_ARGS__)
#define TRACE_MEM(format, ...)          log_debug("[MEM] "format"\n",  ## __VA_ARGS__)
#define TRACE_LLT(format, ...)          log_debug("[LLT] "format"\n",  ## __VA_ARGS__)
#define TRACE_LLC_ACL(n,a,l,pb,hdl)     do{log_debug("[ACL] %s ", n); log_debug("%04X(%d) {", hdl,pb); log_debug_array(a,l); log_debug("[%d]}\n",l);}while(0)
#define TRACE_BB(format, ...)           log_debug("[BB ] "format"\n",  ## __VA_ARGS__)
#else
#define TRACE_EVT(format, ...)
#define TRACE_MSG(format, ...)
#define TRACE_TIMER(format, ...)
#define TRACE_MEM(format, ...)
#define TRACE_LLT(format, ...)
#define TRACE_LLC_ACL(n,a,l,pb,hdl)
#define TRACE_BB(format, ...)
#endif

/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * EXTERN VARIABLES
 */
#ifdef CONFIG_MONITOR_CPU_UTILIZATION
extern uint32_t cpu_idle_time;
extern uint32_t cpu_begin_time;
#endif


/*********************************************************************
 * EXTERN FUNCTIONS
 */

/**
 * @brief __co_assert()
 *
 * @param[in] exp  
 * @param[in] file  
 * @param[in] func  
 * @param[in] line  
 *
 * @return 
 **/
int __co_assert(const char *exp, const char *file, const char *func, int line);

#ifdef __cplusplus
}
#endif

#endif

