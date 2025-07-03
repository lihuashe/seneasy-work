/**
 * @file wdt.h
 * @brief watch dog driver
 * @date Thu 04 May 2017 05:37:07 PM CST
 * @author XIHAO
 *
 * @defgroup WDT WDT
 * @ingroup PERIPHERAL
 * @brief Watch dog driver
 * @details WDT driver
 *
 * @{
 */

#ifndef __WDT_H__
#define __WDT_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "peripheral.h"

/*********************************************************************
 * MACROS
 */
#define WDT_TIMEOUT_MAX             (31)  // Max is 31.99s

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
 * @brief watch dog keepalive
 *
 * @return None
 **/
void wdt_keepalive(void);

/**
 * @brief enable watch dog
 *
 * @param[in] timeout  timeout with second, 0 to disable It
 **/
void wdt_enable(uint32_t timeout);

/// @cond
/**
 * @brief wdt_restore()
 *
 * @return 
 **/
void wdt_restore(void);
/// @endcond

#ifdef __cplusplus
}
#endif

#endif

/** @} */

