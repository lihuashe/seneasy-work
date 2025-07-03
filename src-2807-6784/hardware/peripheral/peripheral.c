/**
 * @file peripheral.c
 * @brief 
 * @date Fri 01 Dec 2017 09:06:10 AM CST
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


/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief peripheral lowpower store
 *
 * @return None
 **/
void peripheral_lowpower_enter(co_power_status_t power_status)
{
    cpm_store();
    gpio_store();

#ifndef CONFIG_OTP_ABSENT
    otp_lowpower_enter(power_status);
#endif

    // Gate not sleep module.
    // Avoid IE_CTRL generatting unexpect interrupt
    REGW1(&XH_PSO->TIM_CFG[0], CPM_GATE_EN_MASK);
    REGW1(&XH_PSO->TIM_CFG[1], CPM_GATE_EN_MASK);
    REGW1(&XH_PSO->TIM_CFG[2], CPM_GATE_EN_MASK);
    REGW1(&XH_PSO->UART1_CFG,  CPM_GATE_EN_MASK);
    REGW1(&XH_PSO->SPI0_CFG,   CPM_GATE_EN_MASK);
    XH_PSO_UPD_RDY();
}

/**
 * @brief peripheral lowpower restore
 *
 * @return None
 **/
void peripheral_lowpower_leave(co_power_status_t power_status)
{
#ifndef CONFIG_OTP_ABSENT
    otp_lowpower_leave(power_status);
#endif

    cpm_restore();
    pinmux_restore();
    gpio_restore();
}

/** @} */


