/**
 * @file gpio_ex.c
 * @brief 
 * @date Fri 18 Sep 2020 02:33:00 PM CST
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
/**
 * @brief gpio irq process function, for system calling
 *
 * @param[in] int_mask  gpio irq status
 * @param[in] level_mask
 *
 * @return None
 **/
static void gpio0_irq_process(uint32_t int_mask, uint32_t level_mask)
{
    // calculate unexpect single edge irq
    uint32_t cur_single_edge_mask = gpio_env.pin_single_edge_flag & int_mask;
    uint32_t cur_single_level_mask = level_mask & cur_single_edge_mask;
    uint32_t exp_single_level_mask = gpio_env.pin_single_edge_level & cur_single_edge_mask;
    uint32_t unexp_single_level_mask = cur_single_level_mask ^ exp_single_level_mask;

    // remove unexpect single edge irq
    int_mask &= ~unexp_single_level_mask;

    // notify
    if(int_mask && gpio_env.callback)
        gpio_env.callback(int_mask);
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief GPIO0_IRQHandler()
 *
 * @return 
 **/
void GPIO0_IRQHandler(void)
{
    uint32_t level_mask = XH_GPIO0->DATA;
    uint32_t int_status = XH_GPIO0->INTSTATUS;

    // clear
    XH_GPIO0->INTSTATUS = int_status;

    if(int_status)
        gpio0_irq_process(int_status, level_mask);
}

/**
 * @brief gpio initialize
 *
 * @return None
 **/
void gpio_open(void)
{
    gpio_env.callback = NULL;
    gpio_env.pin_single_edge_flag = 0;
    gpio_env.pin_single_edge_level = 0;

    // GPIO reset may be generate a invalid jitter when debuging-reset, so set all pin as input
    XH_GPIO0->OUTENCLR = 0xFFFFFFFF;

    // Reset and clock on
    register_set1(&XH_PSO->GPIO_CFG, CPM_GPIO_SOFT_RESET_MASK);
    register_set0(&XH_PSO->GPIO_CFG, CPM_GPIO_GATE_EN_MASK);
    XH_PSO_UPD_RDY();
}

/**
 * @brief  gpio open only clock
 **/
void gpio_open_clock(void)
{
    register_set0(&XH_PSO->GPIO_CFG, CPM_GPIO_GATE_EN_MASK);
    XH_PSO_UPD_RDY();
}

/**
 * @brief gpio set direction
 *
 * @param[in] pin_mask  pin mask
 * @param[in] dir  gpio direction
 *
 * @return None
 **/
void gpio_set_direction(uint32_t pin_mask, gpio_direction_t dir)
{
    if(dir == GPIO_INPUT)
        XH_GPIO0->OUTENCLR = pin_mask;
    else
        XH_GPIO0->OUTENSET = pin_mask;
}

/**
 * @brief gpio write value
 *
 * @param[in] pin_mask  pin mask
 * @param[in] level  gpio value with mask, @ref gpio_level_t
 *
 * @return None
 **/
void gpio_write(uint32_t pin_mask, uint32_t level)
{
    uint8_t *p = (uint8_t *)&pin_mask;

    if(p[0]) XH_GPIO0->MASK_0_7  [p[0]] = level;
    if(p[1]) XH_GPIO0->MASK_8_15 [p[1]] = level;
    if(p[2]) XH_GPIO0->MASK_16_23[p[2]] = level;
    if(p[3]) XH_GPIO0->MASK_24_31[p[3]] = level;
}

/**
 * @brief gpio set interrupt type
 *
 * @param[in] pin_mask  pin mask
 * @param[in] trigger_type  gpio trigger type
 *
 * @return None
 **/
void gpio_set_interrupt(uint32_t pin_mask, gpio_trigger_type_t trigger_type)
{
    /*
     * Single-Edge-Trigger is replaced as Both-Edge-Trigger.
     * Single-Edge-Trigger may cause the chip not go to sleep.
     * Software walkarround it.
     */

    CO_DISABLE_IRQ();

    switch(trigger_type)
    {
        case GPIO_FALLING_EDGE:
            XH_GPIO0->INTTYPESET = pin_mask;
            XH_GPIO0->INTBOTHSET = pin_mask;
            gpio_env.pin_single_edge_flag |= pin_mask;
            gpio_env.pin_single_edge_level &= ~pin_mask;
            break;

        case GPIO_RISING_EDGE:
            XH_GPIO0->INTTYPESET = pin_mask;
            XH_GPIO0->INTBOTHSET = pin_mask;
            gpio_env.pin_single_edge_flag |= pin_mask;
            gpio_env.pin_single_edge_level |= pin_mask;
            break;

        case GPIO_BOTH_EDGE:
            XH_GPIO0->INTTYPESET = pin_mask;
            XH_GPIO0->INTBOTHSET = pin_mask;
            gpio_env.pin_single_edge_flag &= ~pin_mask;
            break;

        case GPIO_LOW_LEVEL:
            XH_GPIO0->INTBOTHCLR = pin_mask;
            XH_GPIO0->INTTYPECLR = pin_mask;
            XH_GPIO0->INTPOLCLR  = pin_mask;
            gpio_env.pin_single_edge_flag &= ~pin_mask;
            break;

        case GPIO_HIGH_LEVEL:
            XH_GPIO0->INTBOTHCLR = pin_mask;
            XH_GPIO0->INTTYPECLR = pin_mask;
            XH_GPIO0->INTPOLSET  = pin_mask;
            gpio_env.pin_single_edge_flag &= ~pin_mask;
            break;

        default:
            break;
    }

    if(trigger_type == GPIO_TRIGGER_DISABLE)
    {
        XH_GPIO0->INTENCLR  = pin_mask;
        XH_GPIO0->INTSTATUS = pin_mask;
    }
    else
    {
        XH_GPIO0->INTSTATUS = pin_mask;
        XH_GPIO0->INTENSET  = pin_mask;
    }

    if(XH_GPIO0->INTENSET)
    {
        NVIC_ClearPendingIRQ(GPIO0_IRQn);
        NVIC_SetPriority(GPIO0_IRQn, IRQ_PRIORITY_NORMAL);
        NVIC_EnableIRQ(GPIO0_IRQn);
    }
    else
    {
        NVIC_DisableIRQ(GPIO0_IRQn);
    }

    CO_RESTORE_IRQ();
}

/**
 * @brief gpio set interrupt callback
 *
 * @param[in] callback  gpio event callback
 *
 * @return None
 **/
void gpio_set_interrupt_callback(gpio_callback_t callback)
{
    gpio_env.callback = callback;
}

/** @} */

