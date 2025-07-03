/**
 * @file wdt.c
 * @brief
 * @date Thu 04 May 2017 05:37:02 PM CST
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
#define WDT_MAX_TOP  15

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * LOCAL VARIABLES
 */
//static wdt_env_t wdt_env = {0};

/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */

/**
 * @brief watch dog keepalive
 *
 * @return None
 **/
void wdt_keepalive(void)
{
    XH_PMU->WDT_KR_CFG = 0xAAAA;
}

/**
 * @brief enable watch dog
 *
 * @param[in] timeout  timeout with second, 0 to disable It
 **/
void wdt_enable(uint32_t timeout)
{
    if (timeout)
    {
        while(XH_PMU->WDT_STATUS & PMU_WDT_LD_KR_MASK);
        XH_PMU->WDT_RLR_CFG = timeout * 128;
        XH_PMU->WDT_KR_CFG = 0x5555;
        while(XH_PMU->WDT_STATUS & PMU_WDT_LD_KR_MASK);
        XH_PMU->WDT_KR_CFG = 0xAAAA;
    }
    else
    {
        while(XH_PMU->WDT_STATUS & PMU_WDT_LD_KR_MASK);
        XH_PMU->WDT_KR_CFG = 0x6666;
    }
}

/** @} */


