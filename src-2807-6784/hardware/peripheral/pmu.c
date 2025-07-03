/**
 * @file pmu.c
 * @brief 
 * @date Thu 26 Nov 2015 04:42:20 PM CST
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
//#include "cfg.h"

/*********************************************************************
 * MACROS
 */

// unit: us
#define TIMEOUT_XTAL_32MHZ          100000

// DEBUG
#define PMU_PIN_STATE_SLEEP_ENABLE //only for debug

// USE RC64M when wakeup
//#define PMU_USE_RC64M_AFTER_WAKEUP

#define PMU_SWD_IO_MASK             0x3

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * LOCAL VARIABLES
 */
pmu_env_t pmu_env = {0};

/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */


#ifdef PMU_DETECT_LOWPOWER_TIME
/**
 * @brief  pmu detect lowpower time pin toggle
 **/
static void pmu_detect_lowpower_time_pin_toggle(void)
{
    if(pmu_env.detect_lowpower_time_pin)
        XH_PMU->GPIO_ODA_CTRL ^= 1u<<pmu_env.detect_lowpower_time_pin;
}
#endif

/**
 * @brief pmu_nvic_restore()
 *
 * @return 
 **/
static void pmu_nvic_restore(void)
{
    // Enable PIN IRQ
    NVIC_SetPriority(PIN_WAKEUP_IRQn, IRQ_PRIORITY_NORMAL);
    NVIC_EnableIRQ(PIN_WAKEUP_IRQn);
#if defined(CONFIG_XH98xxD) && defined(CONFIG_XH98xxF)
    NVIC_SetPriority(POWER_DOWN_IRQn, IRQ_PRIORITY_HIGH);
    NVIC_EnableIRQ(POWER_DOWN_IRQn);
#endif
}

/**
 * @brief pmu_gpio_lowpower_is_ready()
 *
 * @return 
 **/
static bool pmu_gpio_lowpower_is_ready(void)
{
    uint32_t pin_wakeup_0_31  = XH_PMU->GPIO_WAKEUP;
    bool pin_0_31_ready = pin_wakeup_0_31 ? (gpio_read(pin_wakeup_0_31)==XH_PMU->GPIO_POL) : true;

    return pin_0_31_ready;
}

/**
 * @brief  pmu pin input enable
 *
 * @param[in] enable_mask  enable mask
 **/
static void pmu_pin_input_enable(uint32_t enable_mask)
{
    REGW(&XH_PMU->GPIO_IE_CTRL, MASK_1REG(PMU_GPIO_IE_CTRL, enable_mask));
}

/**
 * @brief  pmu select 32k get from reg
 *
 * @return 32k
 **/
pmu_32k_sel_t pmu_select_32k_get_reg(void)
{
    if (XH_PMU->MISC_CTRL & PMU_MISC_CLK_32K_SEL_MASK)
        return PMU_32K_SEL_DIV;

    return PMU_32K_SEL_RC;
}

/**
 * @brief  pmu 32k switch to rc
 *
 * @param[in] calib  calib
 * @param[in] pd_others  pd others
 **/
void pmu_32k_switch_to_rc(bool calib, bool pd_others)
{
    // Power on rc32k
//    REGW0(&XH_PMU->MISC_CTRL, PMU_MISC_REG_PD_RC32K_MASK);
//    while(!(XH_PMU->STATUS_READ & PMU_RC32K_READY_MASK));

    // calib it
    if(calib)
        calib_rc32k();

    // Switch
    REGW0(&XH_PMU->MISC_CTRL, PMU_MISC_CLK_32K_SEL_MASK); //1:div32k 0:other
    while(!(XH_PMU->STATUS_READ & PMU_CLK_32K_DIV_DONE_MASK));

    if (pd_others)
    {
        // power down others
        REGW0(&XH_SYS->RST_32K_OSC_CTRL, SYS_CRY32M_DIV_EN_MASK); //div32k
    }
}

/**
 * @brief pmu_wakeup_pin_wait_idle()
 *
 * @return 
 **/
void pmu_wakeup_pin_wait_idle(void)
{
    while(XH_PMU->MISC_CTRL & PMU_MISC_CLR_PMU_INT_MASK);
    while(XH_PMU->STATUS_READ & PMU_CLR_PMU_INT_SYNC_APB_MASK);
}

/**
 * @brief  pmu xtal32m is keep on
 *
 * @return on ?
 **/
static bool pmu_topclk_xtal32m_is_keep_on(void)
{
    return (XH_PMU->MISC_CTRL_1 & PMU_MISC_CRY32M_KEEP_ON_MASK) ? true : false;
}

/**
 * @brief  pmu topclk double preset
 **/
/*static*/ void pmu_topclk_x2_enable(bool enable)
{
    if (enable)
    {
#ifdef CONFIG_XH98xx
        // digital ldo to 1.05v (0000=0.65;0111=1.00;1111=1.40;step=50mv)
        REGW(&XH_PMU->ANA_REG, MASK_2REG(PMU_ANA_LDO_V0P9_DIG_VBAT, REG_SSAT(PMU_ANA_LDO_V0P9_DIG_VBAT,8+calib_repair_env.delta_vdd_1p2), PMU_ANA_CTRL_LDO_DIG_UPDATE,1));
#endif
        // enable
        REGW1(&XH_PMU->ANA_PD, PMU_ANA_EN_64M_MASK);
    }
    else
    {
        if(XH_PMU->ANA_PD & PMU_ANA_EN_64M_MASK)
        {
            // disable
            REGW0(&XH_PMU->ANA_PD, PMU_ANA_EN_64M_MASK);
#ifdef CONFIG_XH98xx
            // digital ldo to 0.95v
            REGW(&XH_PMU->ANA_REG, MASK_2REG(PMU_ANA_LDO_V0P9_DIG_VBAT, REG_SSAT(PMU_ANA_LDO_V0P9_DIG_VBAT,6+calib_repair_env.delta_vdd_1p2), PMU_ANA_CTRL_LDO_DIG_UPDATE,1));
#endif
        }
    }
}

/**
 * @brief  pmu topclk xtal32m wait ready
 **/
/*static*/ void pmu_topclk_xtal32m_wait_ready(void)
{
    // WAIT Ready
    while(!(XH_DAIF->XTAL32M_INTRS & DAIF_XTAL32M_CLK_RDY_MASK));

//    // Check
//    if(!(XH_DAIF->XTAL32M_INTRS & DAIF_XTAL32M_CLK_RDY_MASK))
//        // System error, LOOP forever
//        co_fault_set(CO_FAULT_HCLK_CRASH);
}

/**
 * @brief  pmu topclk switch to rc32m
 **/
/*static*/ void pmu_topclk_switch_to_rc32m(void)
{
    // To RC32M
    register_set0(&XH_PMU->MISC_CTRL, PMU_MISC_CLK_64M_SEL_MASK); // 0:RC32MHz 1:64MHz
    while(!(XH_PSO->STATUS_READ & CPM_MAIN_CLK_SYNC_DONE_MASK));
}

#ifdef PMU_USE_RC64M_AFTER_WAKEUP
/**
 * @brief  pmu topclk switch to rc64m
 **/
/*static*/ void pmu_topclk_switch_to_rc32m_x2(void)
{
    // from RC32M
    REGW1(&XH_PMU->CLK_CTRL_2, PMU_REG_SEL_DIG_CLK_MASK); // 0:XTAL 1:RC
    REGW1(&XH_PMU->XTAL32M_CNS0, PMU_XTAL32M_SEL_CPUCLK_MASK); // 0:XTAL32M 1:64M
    co_delay_us(1);
    // To 64M
    REGW1(&XH_PMU->MISC_CTRL, PMU_MISC_CLK_64M_SEL_MASK); // 0:RC32MHz 1:64MHz
    while(!(XH_PSO->STATUS_READ & CPM_MAIN_CLK_SYNC_DONE_MASK));
}
#endif

/**
 * @brief  pmu xtal32m switch to 32m
 **/
/*static*/ void pmu_topclk_switch_to_xtal32m(void)
{
    // from XTAL32M
    REGW0(&XH_PMU->XTAL32M_CNS0, PMU_XTAL32M_SEL_CPUCLK_MASK); // 0:XTAL32M 1:64M
    co_delay_us(1);
    // To XTAL32M
    REGW1(&XH_PMU->MISC_CTRL, PMU_MISC_CLK_64M_SEL_MASK); // 0:RC32MHz 1:CPUCLK
    while(!(XH_PSO->STATUS_READ & CPM_MAIN_CLK_SYNC_DONE_MASK));
}

/**
 * @brief  pmu xtal32m switch to 64m
 **/
/*static*/ void pmu_topclk_switch_to_xtal32m_x2(void)
{
    // from XTAL32M
    REGW0(&XH_PMU->CLK_CTRL_2, PMU_REG_SEL_DIG_CLK_MASK); // 0:XTAL 1:RC
    REGW1(&XH_PMU->XTAL32M_CNS0, PMU_XTAL32M_SEL_CPUCLK_MASK); // 0:XTAL32M 1:64M
    co_delay_us(1);
    // To 64M
    REGW1(&XH_PMU->MISC_CTRL, PMU_MISC_CLK_64M_SEL_MASK); // 0:RC32MHz 1:64MHz
    while(!(XH_PSO->STATUS_READ & CPM_MAIN_CLK_SYNC_DONE_MASK));
}

/**
 * @brief  pmu rc32m power enable
 **/
/*static*/ void pmu_topclk_rc32m_power_enable(bool enable)
{
    if(enable)
    {
        register_set0(&XH_PMU->MISC_CTRL_1, PMU_MISC_REG_PD_RC32M_MASK);
        // must delay more than 15us
        co_delay_10us(3);
    }
    else
    {
        register_set1(&XH_PMU->MISC_CTRL_1, PMU_MISC_REG_PD_RC32M_MASK);
    }
}

/**
 * @brief  pmu topclk xtal32m power enable
 *
 * @param[in] enable  
 *
 * XTAL32M
 *
 * if CRY32M_EN==1, xtal24m will be fast-startuped automatically after wakeup (ignore PD_CRY32M)
 *
 * CRY32M_EN does not control xtal24m directly, fucntion:
 *   - xtal24m fast-startup FLAG after wakeup (ignore PD_CRY32M)
 *   - 0 to reset xtal24m-startup-ready signal
 *
 * PD_CRY32M edge can control xtal24m directly, function:
 *   - rising edge: power down
 *   - falling edge: power on
 *
 **/
/*static*/ void pmu_topclk_xtal32m_power_enable(bool enable)
{
    if(enable)
    {
        // open xtal32m ctrl clock
        REGW1(&XH_DAIF->CLK_ENS, DAIF_XTAL32M_CTRL_CLK_EN_MASK);

        // Power on
        REGW1(&XH_PMU->MISC_CTRL_1, PMU_MISC_CRY32M_EN_MASK);
        REGW0(&XH_PMU->MISC_CTRL_1, PMU_MISC_REG_PD_CRY32M_MASK);

        // wait
        pmu_topclk_xtal32m_wait_ready();

        // close xtal32m ctrl clock
        REGW1(&XH_DAIF->CLK_ENS, DAIF_XTAL32M_CTRL_CLK_EN_MASK);
    }
    else
    {
        REGW0(&XH_PMU->MISC_CTRL_1, PMU_MISC_CRY32M_EN_MASK);
        REGW1(&XH_PMU->MISC_CTRL_1, PMU_MISC_REG_PD_CRY32M_MASK); // rising edge
    }
}

/**
 * @brief  pmu topclk xtal32m is enabled
 **/
/*static*/ bool pmu_topclk_xtal32m_is_enabled(void)
{
    return (XH_PMU->MISC_CTRL_1 & PMU_MISC_REG_PD_CRY32M_MASK) ? false : true;
}

/**
 * @brief  pmu topclk xtal32m is enabled
 **/
/*static*/ bool pmu_topclk_xtal32m_x2_is_enabled(void)
{
    return (XH_PMU->ANA_PD & PMU_ANA_EN_64M_MASK) ? true : false;
}

/**
 * @brief  pmu xtal32m startup param setup
 **/
static void pmu_xtal32m_startup_param_setup(void)
{
    REGW(&XH_PMU->CLK_CTRL_2, MASK_1REG(PMU_REG_CTUNE_XTAL, 8));
    REGW(&XH_PMU->XTAL32M_CNS1, MASK_1REG(PMU_XTAL32M_NRB_POR, 0));
}

/**
 * @brief  pmu xtal32m fast startup param setup
 **/
static void pmu_xtal32m_fast_startup_param_setup(void)
{
    REGW(&XH_PMU->XTAL32M_CNS1, MASK_1REG(PMU_XTAL32M_NRB_POR,3));
}

/**
 * @brief  pmu pin input state lowpower enter
 *
 * @param[in] enter  enter
 **/
void pmu_pin_input_state_lowpower_enter(bool enter)
{
#ifdef PMU_PIN_STATE_SLEEP_ENABLE
    if (enter)
    {
        // Only wakeup pin as INPUT
        // NOTE: the not IE_CTRL controling IO will force to LOW level
        uint32_t pin_input_mask = XH_PMU->GPIO_WAKEUP | pmu_env.pin_sleep_force_enable_ie_mask;
        if (pmu_env.enable_swd_in_sleep) pin_input_mask |= PMU_SWD_IO_MASK;
        pmu_pin_input_enable(pin_input_mask);
    }
    else
    {
        // Enable all pin can INPUT
        pmu_pin_input_enable(0xFFFFFFFF);
    }
#endif
}

/**
 * @brief  pmu clock lowpower enter
 *
 * @param[in] enter  enter
 * @param[in] state  state
 **/
static void pmu_clock_lowpower_enter(bool enter, pmu_lowpower_state_t state)
{
    CPM_ANA_AHB_CLK_ENABLE();

    if(enter)
    {
        // power on and switch RC32M
        pmu_topclk_rc32m_power_enable(true);
        pmu_topclk_switch_to_rc32m();

        // check xtal32m and power off
        if(pmu_topclk_xtal32m_is_enabled() && !pmu_topclk_xtal32m_is_keep_on())
        {
            pmu_topclk_xtal32m_power_enable(false);
            pmu_topclk_x2_enable(false);
        }
    }
    else
    {
        // restore calibed value
        calib_sys_restore();

        if(state != PMU_SLEEP_HXTAL_OFF)
        {
            pmu_xtal32m_fast_startup(true/*force*/);
        }
#ifdef PMU_USE_RC64M_AFTER_WAKEUP
        else
        {
            // RC64M
            pmu_topclk_x2_enable(true);
            pmu_topclk_switch_to_rc32m_x2();
        }
#endif

        // disable all daif clock
        XH_DAIF->CLK_ENS = 0;
    }

    CPM_ANA_CLK_RESTORE();
}

/**
 * @brief  pmu dcdc lowpower enter
 *
 * @param[in] enter  enter
 *
 * @note
 *  Flow:
 *    sleep:  {open-LDO -> delay 50us  -> close-DCDC} -> [close-LDO]
 *    wakeup: [open-LDO -> delay 200us] -> {open-DCDC  -> delay 100us -> close-LDO}
 *    {...}: software flow
 *    [...]: digital flow
 **/
static void pmu_dcdc_lowpower_enter(bool enter)
{
#ifndef CONFIG_DCDC_ABSENT
    if (pmu_dcdc_is_enabled())
    {
        if(enter)
        {
            REGW(&XH_PMU->ANA_PD, MASK_1REG(PMU_ANA_LDO_1P5_DIS, 0));
            co_delay_10us(5);
            REGW(&XH_PMU->ANA_PD, MASK_1REG(PMU_ANA_BUCK_DIS, 1));
        }
        else
        {
            REGW(&XH_PMU->ANA_PD, MASK_1REG(PMU_ANA_BUCK_DIS, 0));
            co_delay_10us(10);
            REGW(&XH_PMU->ANA_PD, MASK_1REG(PMU_ANA_LDO_1P5_DIS, 1));
        }
    }
#endif
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief pmu initialize
 *
 * @return None
 **/
void pmu_init(void)
{
    // Pinmux control the pin
    register_set0(&XH_PMU->MISC_CTRL, PMU_MISC_GPIO_AUTO_LATCH_CTRL_MASK);

    // GPIO wakeup mask disable (by default all is open)
    XH_PMU->GPIO_WAKEUP = 0;
    XH_PMU->GPIO_POL = 0;
    // Enable IE control
    pmu_pin_input_enable(0xFFFFFFFF);

    // Disable test_en
    REGW0(&XH_PMU->MISC_CTRL_1, PMU_MISC_CPU_JTAG_TEST_EN_MASK|PMU_MISC_CPU_JTAG_ICG_EN_MASK);

#if defined(CONFIG_XH98xxD) && (CONFIG_HARDWARE_VERSION==1)
    // xtal32m fast startup time to 128us
    REGW(&XH_PMU->XTAL32M_CNS1, MASK_1REG(PMU_XTAL32M_T5_CFG, 0));
    // better RX sensitivity
    REGW(&XH_PMU->ANA_REG, MASK_3REG(PMU_ANA_LDO_V1P0_RXADC_1P2,0, PMU_ANA_LDO_V1P0_OSC_1P2,0, PMU_ANA_LDO_V1P0_ANADIG_1P2,0));
#endif

    // Store wdt-reset and clear it
    if(XH_PMU->WDT_STATUS & PMU_WDT_FLAG_MASK)
    {
        // Clear
        register_set0(&XH_PMU->WDT_STATUS, PMU_WDT_FLAG_MASK);

        // Soft flag
        register_set1(&XH_PMU->SW_STATUS, PMU_SW_STATUS_REBOOT_FROM_WDT_MASK);
        register_set0(&XH_PMU->SW_STATUS, PMU_SW_STATUS_REBOOT_FROM_SOFT_RESET_MASK);
    }
    else
    {
        register_set0(&XH_PMU->SW_STATUS, PMU_SW_STATUS_REBOOT_FROM_WDT_MASK);
    }

    if (XH_PMU->FLASH_LOW_VOL_CTRL_0 & PMU_FLASH_LOW_VOL_FLAG_MASK)
    {
        // clear
        XH_PMU->FLASH_LOW_VOL_CTRL_1 = 0;

        // Soft flag
        register_set1(&XH_PMU->SW_STATUS, PMU_SW_STATUS_REBOOT_FROM_IFLASH_LOW_V_MASK);
        register_set0(&XH_PMU->SW_STATUS, PMU_SW_STATUS_REBOOT_FROM_SOFT_RESET_MASK);
    }
    else
    {
        register_set0(&XH_PMU->SW_STATUS, PMU_SW_STATUS_REBOOT_FROM_IFLASH_LOW_V_MASK);
    }
}

/**
 * @brief pmu select xtal24m as top clock, call by system
 **/
void pmu_xtal32m_startup(void)
{
    CPM_ANA_AHB_CLK_ENABLE();

    // check: whether is xtal24m opened
    if(!pmu_topclk_xtal32m_is_enabled())
    {
        // Next XTAL32M startup use normal-startup mode.
        REGW1(&XH_PMU->MISC_CTRL, PMU_MISC_FIRST_RUN_REG_MASK);

        // power on rc32m and switch to it
        // Make sure CPU running on RC32M
        pmu_topclk_rc32m_power_enable(true);
        pmu_topclk_switch_to_rc32m();

        // Try open xtal32m
        pmu_xtal32m_startup_param_setup();
        pmu_topclk_xtal32m_power_enable(true);

        // calib RC
        calib_sys_rc();

        // to xtal32m
        pmu_topclk_switch_to_xtal32m();

        // calib RC32M
        calib_sys_rc32m();

        // power off rc32m
        pmu_topclk_rc32m_power_enable(false);
    }

    // disable all daif clock
    XH_DAIF->CLK_ENS = 0;

    CPM_ANA_CLK_DISABLE();
}

/**
 * @brief  pmu xtal32m fast startup
 *
 * @param[in] force  force
 **/
void pmu_xtal32m_fast_startup(bool force)
{
    // check
    if(force || !pmu_topclk_xtal32m_is_enabled())
    {
        CPM_ANA_AHB_CLK_ENABLE();

        // Next XTAL32M startup use fast-startup mode.
        REGW0(&XH_PMU->MISC_CTRL, PMU_MISC_FIRST_RUN_REG_MASK);

        // Make sure CPU running on RC32M
        pmu_topclk_rc32m_power_enable(true);
        pmu_topclk_switch_to_rc32m();

        // Try open xtal24m
        pmu_xtal32m_fast_startup_param_setup();
        pmu_topclk_xtal32m_power_enable(true);

        // to xtal32m
        pmu_topclk_switch_to_xtal32m();

        // power off rc32m
        pmu_topclk_rc32m_power_enable(false);

        /*
         * Aggressive CPU gate feature:
         *   when enter WFI, all CPU clock will be gated and SWD can't visit.
         * Enable dependent:
         *   1. XTAL32M must be startuped.
         *   2. DMA must be stoped.
         */
        if(XH_PMU->SW_STATUS & PMU_SW_STATUS_AGGRESSIVE_CPUGATE_MASK)
        {
            if (XH_PSO->DMA_CFG & CPM_DMA_GATE_EN_MASK)
                REGW1(XH_SYS->PINMUX, SYS_PINMUX_SYSPLL_GT_CPUCLK_HW_CTRL_MASK);
        }

        CPM_ANA_CLK_RESTORE();
    }
}

/**
 * @brief  pmu xtal32m x2 startup
 **/
void pmu_xtal32m_x2_startup(void)
{
    CPM_ANA_AHB_CLK_ENABLE();

    // startup xtal32m
    pmu_xtal32m_fast_startup(false/*force*/);

    // startup xtal32m x2
//    if (!pmu_topclk_xtal32m_x2_is_enabled())
    {
        // to rc32m
        pmu_topclk_rc32m_power_enable(true);
        pmu_topclk_switch_to_rc32m();

        // delay 2us
        co_delay_us(2);

        // to xtal64m
        pmu_topclk_x2_enable(true);
        pmu_topclk_switch_to_xtal32m_x2();

        // power off rc32m
        pmu_topclk_rc32m_power_enable(false);
    }

    CPM_ANA_CLK_RESTORE();
}

/**
 * @brief  pmu select 32k
 *
 * @param[in] clk32k  clk32k
 **/
void pmu_select_32k(pmu_32k_sel_t clk32k)
{
    // Default: rc32k powered on, xtal32k powered down
    switch(clk32k)
    {
        case PMU_32K_SEL_RC:
            pmu_select_32k(PMU_32K_SEL_DIV);
            pmu_32k_switch_to_rc(true /*calib*/, true/*pd_others*/);
            break;

        case PMU_32K_SEL_32768HZ_XTAL:
            break;

        case PMU_32K_SEL_DIV:
            // Open clock
            REGW1(&XH_SYS->RST_32K_OSC_CTRL, SYS_CRY32M_DIV_EN_MASK);

            // Switch
            REGW1(&XH_PMU->MISC_CTRL, PMU_MISC_CLK_32K_SEL_MASK); //1:div32k 0:other
            while(!(XH_PMU->STATUS_READ & PMU_CLK_32K_DIV_DONE_MASK));
            break;
    }

//    pmu_env.clock_32k = clk32k;
}

///**
// * @brief pmu get 32k select
// *
// * @return 32k select
// **/
//pmu_32k_sel_t pmu_select_32k_get(void)
//{
//    return pmu_env.clock_32k;
//}

/**
 * @brief pmu_wakeup_pin_get()
 *
 * @return
 **/
uint32_t pmu_wakeup_pin_get(void)
{
    return XH_PMU->GPIO_WAKEUP;
}

/**
 * @brief what power status should be entried
 *
 * @return power status
 **/
co_power_status_t pmu_power_status(void)
{
    co_power_status_t max_power_status = POWER_DEEP_SLEEP;

    if(pmu_env.lp_state)
    {
        // General peripheral lowpower control
        if(pmu_env.lp_state & ~(PMU_LP_RTC|PMU_LP_WDT))
        {
            return POWER_IDLE;
        }

        // WDT need 32K, can't into deep sleep (Must check WDT firster than RTC)
        else if(pmu_env.lp_state & PMU_LP_WDT)
        {
            max_power_status = POWER_SLEEP;
            // fall through
        }
    }

    return pmu_gpio_lowpower_is_ready() ? max_power_status : POWER_IDLE;
}

/**
 * @brief pmu enter lowpower, call by system
 *
 * @param[in] state  lowpower state
 *
 * @return None
 **/
void pmu_lowpower_enter(pmu_lowpower_state_t state)
{
    switch(state)
    {
        case PMU_DEEP_SLEEP:
            if(!pmu_env.enable_32k_with_deep_sleep)
            {
                // Set a flag to power down 32K
                register_set1(&XH_PMU->BASIC, PMU_BASIC_SLEEP_WO_32K_MASK);
                XH_PMU_UPD_RDY();
                break;
            }
            // fall through

        case PMU_SLEEP:
        case PMU_SLEEP_HXTAL_OFF:
            // Set a flag to keep on 32K
            register_set0(&XH_PMU->BASIC, PMU_BASIC_SLEEP_WO_32K_MASK);
            XH_PMU_UPD_RDY();
            break;

        case PMU_SHUTDOWN:
            while(1);
    }

    // pin input state
    pmu_pin_input_state_lowpower_enter(true);

    // Wait wake IO clear ok
    pmu_wakeup_pin_wait_idle();

    // clock lowpoer enter
    pmu_clock_lowpower_enter(true, state);

    // dcdc lowpower enter (must do after cpu switch to RC32M)
    pmu_dcdc_lowpower_enter(true);
}

/**
 * @brief pmu leave lowpower status, call by system
 *
 * @note The time between wake trigger (sleep timer or gpio) and CPU running is 450us
 *       pmu_basic[3:0] ctrl ldo startup time
 *
 * @param[in] state  lowpower state
 *
 * @return None
 **/
void pmu_lowpower_leave(pmu_lowpower_state_t state)
{
#ifdef PMU_DETECT_LOWPOWER_TIME
    pmu_detect_lowpower_time_pin_toggle();
#endif

    // dcdc lowpower leave
    pmu_dcdc_lowpower_enter(false);

#ifdef PMU_DETECT_LOWPOWER_TIME
    pmu_detect_lowpower_time_pin_toggle();
#endif

    // clock lowpoer leave
    pmu_clock_lowpower_enter(false, state);

#ifdef PMU_DETECT_LOWPOWER_TIME
    pmu_detect_lowpower_time_pin_toggle();
#endif

    // pin input state
    pmu_pin_input_state_lowpower_enter(false);

    switch(state)
    {
        case PMU_SLEEP:
        case PMU_SLEEP_HXTAL_OFF:
            pmu_env.pin_wakeup_sleep_recently = true;
            break;

        case PMU_DEEP_SLEEP:
            break;

        default:
            break;
    }

    pmu_env.ana_mask = 0;

    pmu_nvic_restore();
}

/**
 * @brief pmu pin state store, call by system
 *
 * @return None
 **/
void pmu_pin_state_store(void)
{
#ifdef PMU_PIN_STATE_SLEEP_ENABLE
    // Digital does not latch SWD IO in XH98xx/XH98xx
    if (pmu_env.enable_swd_in_sleep)
        return;

#ifdef PMU_DETECT_LOWPOWER_TIME
    if(pmu_env.detect_lowpower_time_pin)
    {
        // make sure all gpio is stable. FIXME: better idea
        co_delay_10us(1);

        uint32_t saved_output;
        uint32_t pin_output_mask = ~XH_PMU->GPIO_WAKEUP;
        // Get ouput data level
        saved_output = gpio_read(pin_output_mask);
        // direction: OE#
        XH_PMU->GPIO_OE_CTRL = ~pin_output_mask;
        // output level
        XH_PMU->GPIO_ODA_CTRL = saved_output;

        // Pin control through PMU
        REGW1(&XH_PMU->GPIO_OE_CTRL_1, PMU_GPIO_OEB_SEL_MASK);
    }
    else
#endif

    // Pin control through PMU
    REGW1(&XH_PMU->MISC_CTRL, PMU_MISC_GPIO_AUTO_LATCH_CTRL_MASK);
#endif
}

/**
 * @brief pmu pin state restore, call by system
 *
 * @return None
 **/
void pmu_pin_state_restore(void)
{
#ifdef PMU_PIN_STATE_SLEEP_ENABLE
#ifdef PMU_DETECT_LOWPOWER_TIME
    if(pmu_env.detect_lowpower_time_pin)
    {
        pmu_detect_lowpower_time_pin_toggle();
        // Pin control through PINMUX
        REGW0(&XH_PMU->GPIO_OE_CTRL_1, PMU_GPIO_OEB_SEL_MASK);
    }
    else
#endif

    // Pin control through PINMUX
    REGW0(&XH_PMU->MISC_CTRL, PMU_MISC_GPIO_AUTO_LATCH_CTRL_MASK);
#endif
}

/**
 * @brief Set pin mode
 *
 * @param[in] pin_mask  pin mask
 * @param[in] mode  pin mode
 *
 * @return None
 **/
void pmu_pin_mode_set(uint32_t pin_mask, pmu_pin_mode_t mode)
{
    switch(mode)
    {
        case PMU_PIN_MODE_FLOAT:
        case PMU_PIN_MODE_PP:
            XH_PMU->GPIO_ODE_CTRL &= ~pin_mask;
            XH_PMU->GPIO_PU_CTRL  &= ~pin_mask;
            XH_PMU->GPIO_PD_CTRL  &= ~pin_mask;
            break;

        case PMU_PIN_MODE_PD:
            XH_PMU->GPIO_ODE_CTRL &= ~pin_mask;
            XH_PMU->GPIO_PU_CTRL  &= ~pin_mask;
            XH_PMU->GPIO_PD_CTRL  |=  pin_mask;
            break;

        case PMU_PIN_MODE_PU:
            XH_PMU->GPIO_ODE_CTRL &= ~pin_mask;
            XH_PMU->GPIO_PU_CTRL  |=  pin_mask;
            XH_PMU->GPIO_PD_CTRL  &= ~pin_mask;
            break;

        case PMU_PIN_MODE_OD:
            XH_PMU->GPIO_ODE_CTRL |=  pin_mask;
            XH_PMU->GPIO_PU_CTRL  &= ~pin_mask;
            XH_PMU->GPIO_PD_CTRL  &= ~pin_mask;
            break;

        case PMU_PIN_MODE_OD_PU:
            XH_PMU->GPIO_ODE_CTRL |=  pin_mask;
            XH_PMU->GPIO_PU_CTRL  |=  pin_mask;
            XH_PMU->GPIO_PD_CTRL  &= ~pin_mask;
            break;

        default:
            break;
    }
}

/**
 * @brief Set gpio driven current
 *
 * @param[in] pin_mask  pin mask
 * @param[in] driven  current driven (Large driven current should be push-pull output)
 *
 * @return None
 **/
void pmu_pin_driven_current_set(uint32_t pin_mask, pmu_pin_driven_current_t driven)
{
    if(driven & 0x01)
        XH_PMU->GPIO_DRV_CTRL_0 |= pin_mask;
    else
        XH_PMU->GPIO_DRV_CTRL_0 &= ~pin_mask;

#ifndef CONFIG_XH98xx
    if(driven & 0x02)
        XH_PMU->GPIO_DRV_CTRL_2 |= pin_mask;
    else
        XH_PMU->GPIO_DRV_CTRL_2 &= ~pin_mask;
#endif
}

/**
 * @brief pmu analog power enable, call by system
 *
 * @param[in] enable  enable/disable
 * @param[in] ana  analog type
 *
 * @return None
 **/
void pmu_ana_enable(bool enable, pmu_ana_type_t ana)
{
    CO_DISABLE_IRQ();

    if(enable)
    {
        if ((pmu_env.ana_mask & ana) == 0)
        {
            // Digital BUG: RF 2M can't close ANA_AHB clock
            if(pmu_env.ana_mask == 0)
                cpm_ana_clock_enable();

            switch(ana)
            {
                case PMU_ANA_RF:
                    CPM_ANA_CLK_ENABLE();
                    REGW1(&XH_DAIF->CLK_ENS, DAIF_PLL_VTRACK_CLK_EN_MASK | DAIF_PLL_LUT_CLK_EN_MASK |
                            DAIF_MAIN_FSM_CLK_EN_MASK | DAIF_RX_AGC_CLK_EN_MASK | DAIF_DCOC_LUT_CLK_EN_MASK |
                            DAIF_SDM_CLK_EN_MASK | DAIF_PLL_CLK_REF_EN_MASK);
                    REGW1(&XH_DAIF->VCO_CTRL0, DAIF_VTRACK_EN_MASK);
                    REGW1(&XH_DAIF->CLK_CFG, DAIF_EN_CKO16M_XTAL_DIG_MASK|DAIF_EN_CKO16M_XTAL_ANA_MASK|DAIF_EN_CKO16M_XTAL_PLL_MASK);
                    CPM_ANA_CLK_RESTORE();
                    break;

                case PMU_ANA_ADC:
                    REGW1(&XH_DAIF->CLK_ENS, DAIF_ADC_CLK_EN_MASK);
                    REGW1(&XH_DAIF->CLK_CFG, DAIF_EN_CKO16M_XTAL_GPADC_MASK);
                    break;

                case PMU_ANA_CALIB_RC32K:
                    REGW1(&XH_DAIF->CLK_ENS, DAIF_RC_32K_TUNE_CLK_EN_MASK);
                    break;

                default:
                    break;
            }

            pmu_env.ana_mask |= ana;
        }
    }
    else
    {
        if (pmu_env.ana_mask & ana)
        {
            pmu_env.ana_mask &= ~ana;

            switch(ana)
            {
                case PMU_ANA_RF:
                    CPM_ANA_CLK_ENABLE();
                    REGW0(&XH_DAIF->CLK_ENS, DAIF_PLL_VTRACK_CLK_EN_MASK | DAIF_PLL_LUT_CLK_EN_MASK |
                            DAIF_MAIN_FSM_CLK_EN_MASK | DAIF_RX_AGC_CLK_EN_MASK | DAIF_DCOC_LUT_CLK_EN_MASK |
                            DAIF_SDM_CLK_EN_MASK | DAIF_PLL_CLK_REF_EN_MASK);
                    REGW0(&XH_DAIF->VCO_CTRL0, DAIF_VTRACK_EN_MASK);
                    REGW0(&XH_DAIF->CLK_CFG, /*DAIF_EN_CKO16M_XTAL_DIG_MASK|*/DAIF_EN_CKO16M_XTAL_ANA_MASK|DAIF_EN_CKO16M_XTAL_PLL_MASK);
                    CPM_ANA_CLK_RESTORE();
                    break;

                case PMU_ANA_ADC:
                    REGW0(&XH_DAIF->CLK_ENS, DAIF_ADC_CLK_EN_MASK);
                    REGW0(&XH_DAIF->CLK_CFG, DAIF_EN_CKO16M_XTAL_GPADC_MASK);
                    break;

                case PMU_ANA_CALIB_RC32K:
                    REGW0(&XH_DAIF->CLK_ENS, DAIF_RC_32K_TUNE_CLK_EN_MASK);
                    break;

                default:
                    break;
            }

            if(pmu_env.ana_mask == 0)
                cpm_ana_clock_disable();
        }
    }

    CO_RESTORE_IRQ();
}

/**
 * @brief analog is enabled
 *
 * @param[in] ana  analog module
 *
 * @return enabled?
 **/
bool pmu_ana_is_enabled(pmu_ana_type_t ana)
{
    return (pmu_env.ana_mask & ana) ? true : false;
}

/**
 * @brief reset cpu to ROM
 *
 * @return None
 **/
void pmu_reset_to_rom(void)
{
    __disable_irq();
    pmu_memery_remap(PMU_REMAP_FROM_ROM);
    pmu_cpu_reset();
    while(1);
}

/**
 * @brief reset cpu to RAM
 *
 * @return None
 **/
void pmu_reset_to_ram(void)
{
    __disable_irq();
    pmu_memery_remap(PMU_REMAP_FROM_RAM);
    pmu_cpu_reset();
    while(1);
}

#ifndef CONFIG_OTP_ABSENT
/**
 * @brief reset cpu to RAM
 *
 * @return None
 **/
void pmu_reset_to_otp(void)
{
    __disable_irq();
    pmu_memery_remap(PMU_REMAP_FROM_OTP);
    pmu_cpu_reset();
    while(1);
}
#endif

/**
 * @brief Force into reboot sleep mode
 *
 * Power consumption is lower than the deep sleep mode. All SRAM will be powered down.
 * But chip will be reboot from ROM when wakeup.
 *
 * @return None
 **/
void pmu_force_into_reboot_sleep_mode(uint32_t ram_retention)
{
    // Disable ALL IRQ, MUST use __set_PRIMASK(1)
     __set_PRIMASK(1);

    // Set flag
    XH_PMU->SW_STATUS &= ~PMU_SW_STATUS_REBOOT_SW_MASK;

    // Set flag
    XH_PMU->SW_STATUS |= PMU_SW_STATUS_REBOOT_FROM_SLEEP_MASK | PMU_SW_STATUS_REBOOT_FROM_SOFT_RESET_MASK;

    // Make sure the isp bit is cleared
    XH_PMU->BOOT_SEL &= ~1u; // @ref BOOT_SEL_ISP_BIT

    // DEEPSLEEP flag
    SCB->SCR |= SCB_SCR_SLEEPDEEP;

    /* !!! Please Make Sure Belowe has no stack used !!! */

    // Remap to ROM
    pmu_memery_remap(PMU_REMAP_FROM_ROM);

    // Power off specified RAM
    XH_PMU->RAM_PM_1 = ram_retention;

    // Good Night!
    __WFI();

    // refix bug
    pmu_wait_system_ready();

    // Must be some IRQ pending, Force reboot
    pmu_cpu_reset();

    // Never come here
    while(1);
}

/**
 * @brief pmu_wait_system_ready()
 *
 * @return 
 **/
void pmu_wait_system_ready(void)
{
    // Check PMU_STATE (PMU_BASIC[31:27]) equal to 7
    while(register_get(&XH_PMU->BASIC, MASK_POS(PMU_BASIC_STATE)) != 7);
}

/**
 * @brief pmu prevent lowpower entry
 *
 * @param[in] lpp  peripheral
 *
 * @return None
 **/
void pmu_lowpower_prevent(pmu_lowpower_peripheral_t lpp)
{
    CO_DISABLE_IRQ();
    pmu_env.lp_state |= (uint32_t)lpp;
    CO_RESTORE_IRQ();
}

/**
 * @brief pmu allow lowpower entry
 *
 * @param[in] lpp  peripheral
 *
 * @return None
 **/
void pmu_lowpower_allow(pmu_lowpower_peripheral_t lpp)
{
    CO_DISABLE_IRQ();
    pmu_env.lp_state &= ~((uint32_t)lpp);
    CO_RESTORE_IRQ();
}

/**
 * @brief lowpower status
 *
 * @return lowpower status
 **/
pmu_lowpower_peripheral_t pmu_lowpower_prevent_status(void)
{
    return (pmu_lowpower_peripheral_t)pmu_env.lp_state;
}

#ifndef CONFIG_DCDC_ABSENT
/**
 * @brief  pmu dcdc is enabled
 *
 * @return  is enabled
 **/
bool pmu_dcdc_is_enabled(void)
{
    return (XH_PMU->SW_STATUS & PMU_SW_STATUS_DCDC_ENABLED_MASK) ? true : false;
}

/**
 * @brief pmu enable INSIDE dcdc
 *
 * @param[in] enable  enable/disable
 *
 * @return None
 **/
void pmu_dcdc_enable(bool enable)
{
    if(enable)
    {
        REGW(&XH_PMU->ANA_PD, MASK_1REG(PMU_ANA_BUCK_DIS, 0));
        co_delay_10us(20);
        REGW(&XH_PMU->ANA_PD, MASK_1REG(PMU_ANA_LDO_1P5_DIS, 1));
        REGW1(&XH_PMU->SW_STATUS, PMU_SW_STATUS_DCDC_ENABLED_MASK);
    }
    else
    {
        REGW(&XH_PMU->ANA_PD, MASK_1REG(PMU_ANA_LDO_1P5_DIS, 0));
        co_delay_10us(20);
        REGW(&XH_PMU->ANA_PD, MASK_1REG(PMU_ANA_BUCK_DIS, 1));
        REGW0(&XH_PMU->SW_STATUS, PMU_SW_STATUS_DCDC_ENABLED_MASK);
    }
}
#endif

/** @} */

