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
#include "bc.h"

#ifndef CONFIG_CALIB_REPAIR_TEMP_NOT_CHECK

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
 * @brief  adc temperature read
 *
 * @return default is 20C
 **/
__WEAK int adc_temperature_read(void)
{
    return 20;
}

/**
 * @brief  calib repair voltage temperature
 *
 * @param[in] t  temperature
 **/
static void calib_repair_voltage_temperature(int16_t t)
{
#ifndef CONFIG_DCDC_ABSENT
    uint32_t ldo_v1p2_vbat;
    uint32_t dcdc_vout;

    // High TX mode
    if (REGR(&XH_PMU->ANA_PD_1, MASK_POS(PMU_DCDC_VOUT)) == CALIB_REPAIR_DCDC_V_HIGH_TX_PWR)
        return;

    if (t > 70)
    {
        ldo_v1p2_vbat = CALIB_REPAIR_LDO_V_HIGH_T;
        dcdc_vout = CALIB_REPAIR_DCDC_V_HIGH_T;
    }
    else
    {
        ldo_v1p2_vbat = CALIB_REPAIR_LDO_V_NORMAL_T;
        dcdc_vout = CALIB_REPAIR_DCDC_V_NORMAL_T;
    }

    // LDO
    REGW(&XH_PMU->ANA_REG, MASK_1REG(PMU_ANA_LDO_V1P2_A_VBAT,
                calib_repair_value_select(ldo_v1p2_vbat+calib_repair_env.delta_vdd_1p2,0,CALIB_REPAIR_LDO_V_MAX)));
    // DCDC
    REGW(&XH_PMU->ANA_PD_1, MASK_1REG(PMU_DCDC_VOUT,
                calib_repair_value_select(dcdc_vout+calib_repair_env.delta_dcdc_1p27,0,CALIB_REPAIR_DCDC_V_MAX)));
#endif
}

/**
 * @brief rf temperature repair check
 **/
static void calib_repair_temperature(uint32_t cur_time)
{
//    if(!adc_is_running())
    {
        int16_t t = (int16_t)adc_temperature_read();
        int16_t delta_t_rc = calib_repair_env.rc_repair_temperature - t;
        int16_t pre_all_repair_t = calib_repair_env.rf_repair_temperature;
        bool topclk_recalibed = false;

        calib_repair_env.temperature = t;
        calib_repair_env.temp_repair_time = cur_time;

        // if delta-temperature>30C, re-calib rc
        if (delta_t_rc > 30 || delta_t_rc < -30)
        {
            calib_repair_env.rc_repair_temperature = t;

            pmu_topclk_recalib();
            topclk_recalibed = true;
        }

        // re-calib all
        if ((pre_all_repair_t<60 && t>70) || (pre_all_repair_t>70 && t<60))
        {
            calib_repair_env.rc_repair_temperature = t;
            calib_repair_env.rf_repair_temperature = t;

            if(!topclk_recalibed)
                pmu_topclk_recalib();

            calib_repair_voltage_temperature(t);

            calib_rf();
        }
    }
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief  calib repair temp init
 **/
void calib_repair_temp_init(void)
{
    int16_t t;

#ifdef CONFIG_USE_BASE_COMPONENT_SYMBOL
    bc_env.dep_calib_repair_temp_check = calib_repair_temp_check;
#endif

    if(0 == calib_repair_env.temp_repair_delay_s)
        return;

//    co_assert(!adc_is_running());

    t = (int16_t)adc_temperature_read();

    calib_repair_env.temperature = t;
    calib_repair_env.rf_repair_temperature = t;
    calib_repair_env.rc_repair_temperature = t;
    calib_repair_env.temp_repair_time = co_time();

    // voltage repair
    calib_repair_voltage_temperature(t);
}

/**
 * @brief  calib repair rc rf temperature check
 *
 * @param[in] allow_repair_delay_ms  allow repair delay ms
 *
 * @return repaired
 **/
void calib_repair_temp_check(void)
{
    if(0 == calib_repair_env.temp_repair_delay_s)
        return;

    uint32_t pre_time_tmp = calib_repair_env.temp_repair_time;
    if(co_time_delay_ms_past(calib_repair_env.temp_repair_delay_s*1000, &pre_time_tmp))
        calib_repair_temperature(pre_time_tmp);
}

/**
 * @brief  calib repiar rc rf temperature delay set
 *
 * @param[in] delay_ms  delay ms
 **/
void calib_repiar_temp_delay_set(uint32_t delay_ms)
{
    calib_repair_env.temp_repair_delay_s = delay_ms / 1000;
}

#endif

/** @} */

