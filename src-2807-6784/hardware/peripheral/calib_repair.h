/**
 * @file calib_repair.h
 * @brief 
 * @date Fri, Mar 16, 2018  6:27:24 PM
 * @author XIHAO
 *
 * @defgroup 
 * @ingroup 
 * @brief 
 * @details 
 *
 * @{
 */

#ifndef __CALIB_REPAIR_H__
#define __CALIB_REPAIR_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "features.h"
#include "peripheral.h"
#include "co.h"


/*********************************************************************
 * MACROS
 */

#define CALIB_REPAIR_DCDC_V_NORMAL_T        12
#define CALIB_REPAIR_DCDC_V_HIGH_T          14
#define CALIB_REPAIR_DCDC_V_HIGH_TX_PWR     16
#define CALIB_REPAIR_DCDC_V_MAX             31
#define CALIB_REPAIR_LDO_V_NORMAL_T         2
#define CALIB_REPAIR_LDO_V_HIGH_T           5
#define CALIB_REPAIR_LDO_V_HIGH_TX_PWR      6
#define CALIB_REPAIR_LDO_V_MAX              7


/*********************************************************************
 * TYPEDEFS
 */
typedef struct
{
    // sys voltage
    int8_t delta_pa_1p0;
    int8_t delta_pa_vco;
#ifndef CONFIG_DCDC_ABSENT
    int8_t delta_vdd_1p2;
    int8_t delta_dcdc_1p27;
#endif

#ifndef CONFIG_CALIB_REPAIR_TEMP_NOT_CHECK
    // Fix RF bug: temperature
    int16_t temperature;

    // rf and rc temperature repiar
    uint32_t temp_repair_delay_s;
    uint32_t temp_repair_time;
    int16_t  rc_repair_temperature;
    int16_t  rf_repair_temperature;
#endif

#ifndef CONFIG_CALIB_REPAIR_RC32K_NOT_CHECK
    // rc32k temperature repair
    uint32_t rc32k_repair_delay_ms;
    uint32_t rc32k_repair_time;
#endif
}calib_repair_t;


/*********************************************************************
 * EXTERN VARIABLES
 */
extern calib_repair_t calib_repair_env;

/*********************************************************************
 * EXTERN FUNCTIONS
 */

/**
 * @brief  calib repair temp init
 **/
void calib_repair_temp_init(void);

/**
 * @brief  calib repair rc32k init
 **/
void calib_repair_rc32k_init(void);

/**
 * @brief rf temperature repair check
 **/
void calib_repair_temp_check(void);

/**
 * @brief rc32k temperature repair check
 **/
void calib_repair_rc32k_check(void);

/**
 * @brief  calib repiar rc rf temperature delay set
 *
 * @param[in] delay_ms  delay ms
 **/
void calib_repiar_temp_delay_set(uint32_t delay_ms);

/**
 * @brief set repair_rc32k_temperature_delay
 *
 * @param[in] delay_ms  
 *
 * @return None
 **/
void calib_repair_rc32k_delay_set(uint32_t delay_ms);

/**
 * @brief  calib repair value select
 *
 * @param[in] calc  calc
 * @param[in] min  min
 * @param[in] max  max
 *
 * @return select
 **/
__STATIC_INLINE int calib_repair_value_select(int calc, int min, int max)
{
    if (calc < min)
        calc = min;
    else if (calc > max)
        calc = max;
    return calc;
}

#ifdef __cplusplus
}
#endif

#endif

/** @} */

