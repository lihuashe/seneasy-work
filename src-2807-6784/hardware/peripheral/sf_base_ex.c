/**
 * @file sf_base_ex.c
 * @brief 
 * @date Mon 16 Nov 2020 04:23:41 PM CST
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
#include "sf_base_ex.h"

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
 * @brief sfb_irq_process()
 *
 * @param[in] sf  sf object
 *
 * @return 
 **/
static void sfb_irq_process(XH_SF_Type *sf, uint32_t sf_index)
{
    // check
    if(sf->RAW_INTR_STATUS & SFB_DONE)
    {
        // clear
        sf->RAW_INTR_STATUS = SFB_DONE;

        // Default Disable IRQ
        sf->INTR_MASK = 0;

        // Allow sleep
        pmu_lowpower_allow(PMU_LP_SF0<<sf_index);

        // callback
        if(sfb_env.callback[sf_index])
            sfb_env.callback[sf_index](sf);
    }
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief set dma done event
 *
 * @param[in] sf  sf object
 * @param[in] callback  event callback
 **/
void sfb_dma_done_event_register(XH_SF_Type *sf, sfb_callback_t callback)
{
    IRQn_Type irqn = sfb_irqn(sf);
    int sf_index = sfb_index(sf);

    sfb_env.callback[sf_index] = callback;

    if (callback)
    {
        NVIC_ClearPendingIRQ(irqn);
        NVIC_SetPriority(irqn, IRQ_PRIORITY_NORMAL);
        NVIC_EnableIRQ(irqn);
    }
    else
    {
        NVIC_DisableIRQ(irqn);
    }
}

/**
 * @brief  sfb dma done event get
 *
 * @param[in] sf  sf
 *
 * @return callback
 **/
sfb_callback_t sfb_dma_done_event_get(XH_SF_Type *sf)
{
    return sfb_env.callback[sfb_index(sf)];
}

/**
 * @brief regs get
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ SFB_CS_NUM-1)
 *
 * @return reg
 **/
uint32_t sfb_regs_get(XH_SF_Type *sf, uint32_t cs)
{
    return sf->CONFIGURATION[cs].CTRL;
}

/**
 * @brief regs get
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ SFB_CS_NUM-1)
 * @param[in] regs  reg
 **/
void sfb_regs_set(XH_SF_Type *sf, uint32_t cs, uint32_t regs)
{
    int sf_index = sfb_index(sf);

    sf->CONFIGURATION[cs].CTRL = sfb_env.configuration_save[sf_index][cs].ctrl = regs;
}

/**
 * @brief  sfb lcd2lane enable
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] enable  enable
 **/
void sfb_lcd2lane_enable(XH_SF_Type *sf, uint32_t cs, bool enable)
{
    int sf_index = sfb_index(sf);
    sfb_env.lcd2lane[sf_index][cs] = enable;
}

/**
 * @brief SF IRQ handler
 **/
void SF_IRQHandler(void)
{
    sfb_irq_process(XH_SF,  0);
}

/**
 * @brief SF IRQ handler
 **/
void SF1_IRQHandler(void)
{
    sfb_irq_process(XH_SF1, 1);
}


/** @} */

