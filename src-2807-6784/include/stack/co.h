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

#ifndef __CO_H__
#define __CO_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "features.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "co_msg_id.h"
#include "co_msg_param.h"
#include "co_tools.h"
#include "co_util.h"
#include "co_list.h"
#include "co_q.h"
#include "co_fifo.h"
#include "co_evt.h"
#include "co_power.h"
#include "co_allocator.h"
#include "co_msg.h"
#include "co_port.h"
#include "co_sche.h"
#include "co_timer.h"
#include "co_stack.h"
#include "co_fault.h"
#include "co_errno.h"
#include "co_delay.h"
#include "co_port_xh98xx.h"
#include "co_time.h"

/*********************************************************************
 * MACROS
 */


/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */

/// @cond
/**
 * @brief co_init()
 *
 * @return 
 **/
void co_init(void);
/// @endcond

#ifdef __cplusplus
}
#endif

#endif

/** @} */

