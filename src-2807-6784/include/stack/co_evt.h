/**
 * @file co_evt.h
 * @brief Event Module
 * @date Tue 16 May 2017 02:52:30 PM CST
 * @author XIHAO
 *
 * @defgroup CO_EVENT Event
 * @ingroup COROUTINE
 * @brief Event Module
 * @details
 *
 * @{
 */

#ifndef __CO_EVT_H__
#define __CO_EVT_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "co_tools.h"

/*********************************************************************
 * MACROS
 */
/// @cond
#define CO_EVT_LLT_SCHE_CANDIDATE  (1u<<0) //           | High priority
#define CO_EVT_MSG_BB2LL           (1u<<1) //           |
#define CO_EVT_LL                  (1u<<2) //           |
#define CO_EVT_LLT_ADAPT           (1u<<3) //           |
#define CO_EVT_TIMER               (1u<<4) //           |
#define CO_EVT_TIMER_EXEC          (1u<<5) //discard    |
#define CO_EVT_URGENCY_POOL        (1u<<6) //discard    |
#define CO_EVT_MSG_COMMON          (1u<<7) //           |
#define CO_EVT_HCI_RX              (1u<<8) //discard    |
#define CO_EVT_RF_ENABLE           (1u<<9) //           |
#define CO_EVT_BB_TIMER_OPERATE    (1u<<10) //          V Low priority
#define CO_EVT_SYS_CTZ_MAX         10 //with ctz
/// @endcond

/// User event ID, n MUST less then or equal to 21
#define CO_EVT_USER(n)             (1u<<(CO_EVT_SYS_CTZ_MAX + 1 + (n)))

/*********************************************************************
 * TYPEDEFS
 */
/// Event ID
typedef uint32_t co_evt_t;
/// Event callback
typedef void (*co_evt_callback_t)(void);
/// Set event callback
typedef void (*co_evt_set_callback_t)(void);

/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */

/// @cond
/**
 * @brief event init
 *
 * @return None
 **/
void co_evt_init(void);
/// @endcond

/**
 * @brief Set event
 *
 * @param[in] mask  Event BIT Mask, @ref CO_EVT_USER
 *
 * @return None
 **/
void co_evt_set(co_evt_t mask);

/**
 * @brief Clear Event
 *
 * @param[in] mask  Event BIT Mask, @ref CO_EVT_USER
 *
 * @return None
 **/
void co_evt_clear(co_evt_t mask);

/**
 * @brief Register the callback with one event bit mask
 *
 * @param[in] mask  Event BIT Mask, @ref CO_EVT_USER
 * @param[in] callback  Event BIT Mask callback
 *
 * @return Success or Fail
 **/
bool co_evt_register(co_evt_t mask, co_evt_callback_t callback);

/// @cond

/**
 * @brief co_evt_sche()
 *
 * @return None
 **/
void co_evt_sche(void);

/**
 * @brief co_evt_is_idle()
 *
 * @return idle ?
 **/
bool co_evt_is_idle(void);

/// @endcond

#ifdef __cplusplus
}
#endif

#endif

/** @} */

