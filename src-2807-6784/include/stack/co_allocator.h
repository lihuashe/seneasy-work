/**
 * @file co_allocator.h
 * @brief allocator
 * @date Sat 17 Jan 2015 01:52:06 PM CST
 * @author XIHAO
 *
 * @defgroup CO_ALLOCATOR Allocator
 * @ingroup COROUTINE
 * @brief Allocator Module
 * @details Provide malloc, calloc, realloc, free function
 *
 * @{
 */

#ifndef __CO_ALLOCATOR_H__
#define __CO_ALLOCATOR_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "stdint.h"
#include "co.h"
#include "co_mem.h"

/*********************************************************************
 * MACROS
 */

/// @cond

#define co_malloc   MEM_MALLOC
#define co_calloc   MEM_CALLOC
#define co_realloc  MEM_REALLOC
#define co_free     MEM_FREE

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * EXTERN VARIABLES
 */

/*********************************************************************
 * EXTERN FUNCTIONS
 */

/**
 * @brief co_allocator_init()
 *
 * @return 
 **/
void co_allocator_init(void);

/// @endcond

/*********************************************************************
 * MACROS FUNCTION
 */

#ifdef __cplusplus
}
#endif

#endif

/** @} */

