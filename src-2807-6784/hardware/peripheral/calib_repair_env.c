/**
 * @file calib_repair_env.c
 * @brief 
 * @date Thu 23 Sep 2021 10:33:39 AM CST
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

/*********************************************************************
 * MACROS
 */
#define RF_INVALID_TEMPERATURE  1000

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


/*********************************************************************
 * PUBLIC FUNCTIONS
 */
calib_repair_t calib_repair_env =
{
    /* delta_pa_1p0             */ 0,
    /* delta_pa_vco             */ 0,
#ifndef CONFIG_DCDC_ABSENT
    /* delta_vdd_1p2            */ 0,
    /* delta_dcdc_1p27          */ 0,
#endif

#ifndef CONFIG_CALIB_REPAIR_TEMP_NOT_CHECK
    /* temperature              */ 20,

    /* temp_repair_delay_s      */ 60,
    /* temp_repair_time         */ CO_INVALID_SYS_TIME,
    /* rc_repair_temperature    */ RF_INVALID_TEMPERATURE,
    /* rf_repair_temperature    */ RF_INVALID_TEMPERATURE,
#endif

#ifndef CONFIG_CALIB_REPAIR_RC32K_NOT_CHECK
    /* rc32k_repair_delay_ms    */ 1000*60*1,
    /* rc32k_repair_time        */ CO_INVALID_SYS_TIME,
#endif
};

/** @} */

