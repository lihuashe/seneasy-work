/*********************************************************************
 * @file gpio.c
 * @brief 
 * @version 1.0
 * @date Sun 14 Dec 2014 02:18:30 PM CST
 * @author XIHAO
 *
 * @note 
 */

/*********************************************************************
 * INCLUDES
 */
#include "peripheral.h"
#include "co.h"

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
gpio_env_t gpio_env;

/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief gpio_store()
 *
 * @return 
 **/
void gpio_store(void)
{
    if(XH_PSO->GPIO_CFG & CPM_GPIO_GATE_EN_MASK)
    {
        // Force open GPIO clock
        register_set0(&XH_PSO->GPIO_CFG, CPM_GPIO_GATE_EN_MASK);
        XH_PSO_UPD_RDY();
    }

    // store gpio status
    gpio_env.lowpower.store_reg.gpio0.DATAOUT    = XH_GPIO0->DATAOUT;
    gpio_env.lowpower.store_reg.gpio0.OUTENSET   = XH_GPIO0->OUTENSET;
    gpio_env.lowpower.store_reg.gpio0.INTENSET   = XH_GPIO0->INTENSET;
    gpio_env.lowpower.store_reg.gpio0.INTPOLSET  = XH_GPIO0->INTPOLSET;
    gpio_env.lowpower.store_reg.gpio0.INTTYPESET = XH_GPIO0->INTTYPESET;
    gpio_env.lowpower.store_reg.gpio0.INTBOTHSET = XH_GPIO0->INTBOTHSET;

    // Disable not-wakeup-IO's IRQ
    XH_GPIO0->INTENCLR = ~pmu_wakeup_pin_get();
}

/**
 * @brief gpio_restore()
 *
 * @return 
 **/
void gpio_restore(void)
{
    if(XH_PSO->GPIO_CFG & CPM_GPIO_GATE_EN_MASK)
    {
        register_set1(&XH_PSO->GPIO_CFG, CPM_GPIO_SOFT_RESET_MASK);
        register_set0(&XH_PSO->GPIO_CFG, CPM_GPIO_GATE_EN_MASK);
        XH_PSO_UPD_RDY();

        // Output level
        XH_GPIO0->DATAOUT    = gpio_env.lowpower.store_reg.gpio0.DATAOUT;
        // Direction
        XH_GPIO0->OUTENSET   = gpio_env.lowpower.store_reg.gpio0.OUTENSET;
        // Interrupt
        XH_GPIO0->INTPOLSET  = gpio_env.lowpower.store_reg.gpio0.INTPOLSET;
        XH_GPIO0->INTTYPESET = gpio_env.lowpower.store_reg.gpio0.INTTYPESET;
        XH_GPIO0->INTBOTHSET = gpio_env.lowpower.store_reg.gpio0.INTBOTHSET;
        // Enable IRQ
        XH_GPIO0->INTENSET   = gpio_env.lowpower.store_reg.gpio0.INTENSET;

        // Enable CPU GPIO NVIC
        if(XH_GPIO0->INTENSET)
        {
            NVIC_SetPriority(GPIO0_IRQn, IRQ_PRIORITY_NORMAL);
            NVIC_EnableIRQ(GPIO0_IRQn);
        }
    }
    else
    {
        // Enable IRQ
        XH_GPIO0->INTENSET = gpio_env.lowpower.store_reg.gpio0.INTENSET;
    }
}

