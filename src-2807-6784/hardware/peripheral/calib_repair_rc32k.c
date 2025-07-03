/**
 * @file calib_repair.c
 * @brief 
 * @date Fri, Mar 16, 2018  6:09:52 PM
 * @author XIHAO
 *
 * @addtogroup 
 * @ingroup 
 * @details 
 *
 * @{
 */

/*********************************************************************
 * INCLUDES
 */
#include "peripheral.h"
#include "co.h"
#include "bb.h"

#ifndef CONFIG_CALIB_REPAIR_RC32K_NOT_CHECK

/*********************************************************************
 * MACROS
 */


/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * LOCAL VARIABLES
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */

/**
 * @brief rc32k temperature repair check
 **/
static void calib_repair_rc32k(uint32_t cur_time)
{
#ifdef CONFIG_SLEEP_FAST_MODE
    // Switch baseband clock to HXTAL
    if(co_power_fast_sleep_mode_is_enabled())
        bb_fast_startup();
#endif

    // check and re-select (will trigger calib_rc32k)
    pmu_select_32k(PMU_32K_SEL_RC);

    // save
    calib_repair_env.rc32k_repair_time = cur_time;
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief  calib repair rc32k init
 **/
void calib_repair_rc32k_init(void)
{
    calib_repair_env.rc32k_repair_time = co_time();
}

/**
 * @brief  calib repair rc32k temperature check
 *
 * @param[in] allow_repair_delay_ms  allow repair delay ms
 *
 * @return repaired
 **/
void calib_repair_rc32k_check(void)
{
    if (0 == calib_repair_env.rc32k_repair_delay_ms)
        return;

    uint32_t pre_time_tmp = calib_repair_env.rc32k_repair_time;
    if(co_time_delay_ms_past(calib_repair_env.rc32k_repair_delay_ms, &pre_time_tmp)) // delay is pasted
        calib_repair_rc32k(pre_time_tmp);
}

/**
 * @brief set repair_rc32k_temperature_delay
 *
 * @param[in] delay_ms  
 *
 * @return None
 **/
void calib_repair_rc32k_delay_set(uint32_t delay_ms)
{
    calib_repair_env.rc32k_repair_delay_ms = delay_ms;
}

#endif

/** @} */

