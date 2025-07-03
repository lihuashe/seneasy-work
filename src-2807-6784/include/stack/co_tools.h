/**
 * @file co.h
 * @brief library
 * @date Sat 17 Jan 2015 01:52:06 PM CST
 * @author XIHAO
 *
 * @defgroup COROUTINE Library
 * @ingroup XH98xx
 * @brief Basic Library
 * @details
 *
 * Provide some basic functions and structures, such as malloc, event, fifo, list, queue and so on
 *
 * @{
 */

#ifndef __CO_TOOLS_H__
#define __CO_TOOLS_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "features.h"


#ifdef CONFIG_ASSERT
// Run Time Asserts, It is important for DEBUG
#define co_assert(exp)                  ((void)( (exp) || (__co_assert(#exp, __FILE__, __FUNCTION__, __LINE__), 0)))
#define co_verify(exp)                  ((void)( (exp) || (__co_assert(#exp, __FILE__, __FUNCTION__, __LINE__), 0)))
#else
#define co_assert(exp)
#define co_verify(exp)                  (exp)
#endif

#define BIT(x) (1<<(x))
#define ARRAY_SIZE(a) 			(sizeof(a) / sizeof(*a))
	
#define U16_HI(a)    (((a) >> 8) & 0xFF)
#define U16_LO(a)    ((a) & 0xFF)

#ifdef __cplusplus
}
#endif

#endif	//__CO_TOOLS_H__

/** @} */

