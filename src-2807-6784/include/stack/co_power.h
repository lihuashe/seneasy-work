/**
 * @file co_power.h
 * @brief power manage module
 * @date Thu 16 Jul 2015 04:45:15 PM CST
 * @author XIHAO
 *
 * @defgroup CO_POWER Power Manage
 * @ingroup XH98xx
 * @brief Power Manage
 * @details 
 *
 * XH98xx has 5 power modes: active mode, idle mode, sleep mode, deep sleep mode and shutdown mode.
 *
 * - In the active mode and idle mode, the clock of digital modules (Timer, UART, SPI, PWM …) can be
 * enabled or disabled independently. The power of analog modules which have independent power domain
 * can also be enabled or disabled by application.
 *
 * - In the idle mode, the clock of processor is gated and all of the interrupts can wakeup system.
 *
 * - In the sleep mode, the interrupts of GPIO, BLE sleep timer can wake up the system.
 *
 * - In the deep sleep mode, only the interrupts of GPIO can wakeup system.
 * 32k clock is power off, so BLE stack does not work in the deep sleep mode.
 *
 * Power mode setting depends on user setting, peripherals’ status and BLE’s status.
 *
 * The user power mode setting is managed by user who can use API co_power_register_user_status()
 * to set power mode user want the system to be.
 *
 * The peripherals’ status is managed by drivers. Actually the driver knows when the peripherals
 * cannot enter into sleep mode. For example when the ADC is sampling, the ADC driver knows the
 * system cannot enter into sleep mode at this time. User does not take care of the peripherals’
 * status.
 *
 * The status of BLE is managed by BLE stack. User will be called by API co_power_register_sleep_event()
 * to obtain the status of BLE.
 *
 * <b>After leave sleep (obtain the event @ref POWER_SLEEP_LEAVE_TOP_HALF), user should
 * reinitialize (or restore) all used peripheral</b>
 *
 * @{
 */

#ifndef __CO_POWER_H__
#define __CO_POWER_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */
/// Power status
typedef enum
{
    /// All modules are alive, and CPU run with full speed
    POWER_ACTIVE,
    /// All modules are alive, but CPU clock is gating
    POWER_IDLE,
    /// Power down most of module(CPU, Peripheral, etc),
    /// but 32K is alive, only gpio and sleep-timer can wake up chip
    POWER_SLEEP,
    /// Power down most of module(CPU, Peripheral, etc),
    /// 32K is not alive, only gpio can wake up chip
    POWER_DEEP_SLEEP,
    /// Power down all module,
    /// only the power pin can wake up chip
    POWER_SHUTDOWN,
}co_power_status_t;

/// Sleep state
typedef enum
{
    /// Event with sleep entering
    POWER_SLEEP_ENTRY,
    /// Event with sleep leaving (IO can not be controlled immediately)
    /// User should reinitialize (or restore) all used peripheral in this event.
    POWER_SLEEP_LEAVE_TOP_HALF,
    /// Event with sleep leaving (IO can be controlled immediately)
    POWER_SLEEP_LEAVE_BOTTOM_HALF,
}co_power_sleep_state_t;

/**
 * @brief sleep state event callback
 *
 * @param[in] sleep_state  current sleep state @ref co_power_sleep_state_t
 * @param[in] power_status  power status, only POWER_SLEEP and POWER_DEEP_SLEEP are valid @ref co_power_status_t
 *
 * @return None
 **/
typedef void (*co_power_sleep_callback_t)(co_power_sleep_state_t sleep_state, co_power_status_t power_status);

/**
 * @brief power status event callback
 *
 * @return Wanted power status @ref co_power_status_t
 **/
typedef co_power_status_t (*co_power_status_callback_t)(void);

typedef struct
{
    bool sleep_enable;
    bool fast_sleep_enable;

    co_power_status_callback_t usr_status_cb;

    co_power_sleep_callback_t sleep_state_cb;
}co_power_env_t;

/*********************************************************************
 * EXTERN VARIABLES
 */
extern volatile co_power_env_t co_power_env;

/*********************************************************************
 * EXTERN FUNCTIONS
 */

/**
 * @brief co_power_sleep_enable()
 *
 * @param[in] enable  
 *
 * @return 
 **/
__STATIC_INLINE void co_power_sleep_enable(bool enable)
{
    co_power_env.sleep_enable = enable;
}

/**
 * @brief co_power_sleep_is_enabled()
 *
 * @return 
 **/
__STATIC_INLINE bool co_power_sleep_is_enabled(void)
{
    return co_power_env.sleep_enable;
}

/**
 * @brief fast sleep mode enable
 *
 * @param[in] enable  
 *
 * @return None
 **/
__STATIC_INLINE void co_power_fast_sleep_mode_enable(bool enable)
{
    co_power_env.fast_sleep_enable = enable;
}

/**
 * @brief fast sleep mode is enabled
 *
 * @return whether enabled
 **/
__STATIC_INLINE bool co_power_fast_sleep_mode_is_enabled(void)
{
    return co_power_env.fast_sleep_enable;
}

/**
 * @brief co_power_register_user_status()
 *
 * @param[in] status_cb  
 *
 * @return 
 **/
__STATIC_INLINE void co_power_register_user_status(co_power_status_callback_t status_cb)
{
    co_power_env.usr_status_cb = status_cb;
}

/**
 * @brief co_power_register_sleep_event()
 *
 * @param[in] event_cb  
 *
 * @return 
 **/
__STATIC_INLINE void co_power_register_sleep_event(co_power_sleep_callback_t event_cb)
{
    co_power_env.sleep_state_cb = event_cb;
}

/**
 * @brief co_power_sleep_handler()
 *
 * @param[in] sleep_state  
 * @param[in] power_status  
 *
 * @return None
 **/
__STATIC_INLINE void co_power_sleep_handler(co_power_sleep_state_t sleep_state, co_power_status_t power_status)
{
    if(co_power_env.sleep_state_cb)
        co_power_env.sleep_state_cb(sleep_state, power_status);
}

/**
 * @brief co_power_manage()
 *
 * @return None
 **/
void co_power_manage(void);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

