/**
 * @file pmu_ex.c
 * @brief 
 * @date Fri 18 Sep 2020 02:43:26 PM CST
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
void pmu_topclk_x2_enable(bool enable);
void pmu_topclk_xtal32m_wait_ready(void);
void pmu_topclk_switch_to_rc32m(void);
void pmu_topclk_switch_to_xtal32m(void);
void pmu_topclk_switch_to_xtal32m_x2(void);
void pmu_topclk_rc32m_power_enable(bool enable);
void pmu_topclk_xtal32m_power_enable(bool enable);
bool pmu_topclk_xtal32m_is_enabled(void);
bool pmu_topclk_xtal32m_x2_is_enabled(void);
void pmu_wakeup_pin_wait_idle(void);
void pmu_32k_switch_to_rc(bool calib, bool pd_others);
pmu_32k_sel_t pmu_select_32k_get_reg(void);

/**
 * @brief pmu_random_seed_is_valid()
 *
 * It shall have no more than six consecutive zeros or ones.
 * It shall have no more than 24 transitions.
 *
 * @param[in] random_seed  
 *
 * @return 
 **/
static bool pmu_random_seed_is_valid(uint32_t random_seed)
{
    // No More than 6 consequtive zeros or ones.
    // No more than 24 transitions

    uint8_t num_consequtive_ones = 0;
    uint8_t num_consequtive_zeros = 0;
    uint8_t num_transitions = 0;
    uint8_t pre_bitval = 0;
    uint8_t j;
    uint8_t bitval;

    pre_bitval = random_seed & 1;
    for (j=1; j<32; j++)
    {
        bitval = (random_seed >> j) & 1;

        if (bitval == pre_bitval)
        {
            if(bitval)
                num_consequtive_ones++;
            else
                num_consequtive_zeros++;
        }
        else
        {
            num_consequtive_ones = 0;
            num_consequtive_zeros = 0;
            num_transitions++;
        }

        pre_bitval = bitval;

        if ((num_consequtive_ones > 5) || (num_consequtive_zeros > 5) || (num_transitions > 24))
            return false;
    }

    return true;
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief pmu_random_seed_fetch()
 *
 * @return XH_RANDOM->RANDOM
 **/
uint32_t pmu_random_seed_fetch(uint32_t pre_random)
{
    int i;
    pmu_32k_sel_t sel32k = pmu_select_32k_get_reg();

    // Random seed init, it use 32k rc, need:32*30us
    if (sel32k != PMU_32K_SEL_RC)
        pmu_32k_switch_to_rc(false /*calib*/, false /*pd others*/);

    // Open Random and try to use 32k rc to genete TRUE random seed
    REGW0(&XH_PSO->RNG_CFG, CPM_RNG_GATE_EN_MASK);
    XH_PSO_32K_UPD_RDY();

    // try (max 20ms fetch)
    for(i=0; i<200; ++i)
    {
        // 32bit random fetch time: 33us * 32bit = 1056ms
        co_delay_10us(10);
        if(pmu_random_seed_is_valid(XH_RANDOM->RANDOM) && (pre_random!=XH_RANDOM->RANDOM))
            break;
    }

    // Gate RANDOM to hold it as random seed
    REGW1(&XH_PSO->RNG_CFG, CPM_RNG_GATE_EN_MASK);
    XH_PSO_32K_UPD_RDY();

    // restore 32k
    if (sel32k != PMU_32K_SEL_RC)
        pmu_select_32k(sel32k);

    return XH_RANDOM->RANDOM;
}

/**
 * @brief xtal32k enable in deep sleep
 *
 * @note
 *  The xtal 32768Hz crystal startup is very very slow (0.5s~2s),
 *  Enable it in deepsleep, can make the wakeup faster
 *
 * @param[in] enable  enable or disable
 *
 * @return None
 **/
void pmu_32k_enable_in_deep_sleep(bool enable)
{
    pmu_env.enable_32k_with_deep_sleep = enable;
}

/**
 * @brief Change xtal 32m params
 *
 * @param[in] load_capacitance  load capacitance, range:0~31, default:15, step:0.75pf, max:23.25pF
 *
 * @note load_capacitance will effect xtal 32m precision and frequency offset.
 *
 * @return None
 **/
void pmu_xtal32m_change_param(int load_capacitance)
{
    if(load_capacitance >= 0)
    {
        int cur = REGR(&XH_PMU->CLK_CTRL_2, MASK_POS(PMU_REG_CTUNE_XTAL));

        if (cur > load_capacitance)
        {
            while(cur-- > load_capacitance)
            {
                REGW(&XH_PMU->CLK_CTRL_2, MASK_1REG(PMU_REG_CTUNE_XTAL, cur));
                co_delay_10us(1);
            }
        }
        else if (cur < load_capacitance)
        {
            while(cur++ < load_capacitance)
            {
                REGW(&XH_PMU->CLK_CTRL_2, MASK_1REG(PMU_REG_CTUNE_XTAL, cur));
                co_delay_10us(1);
            }
        }

        co_assert(REGR(&XH_PMU->CLK_CTRL_2, MASK_POS(PMU_REG_CTUNE_XTAL)) == load_capacitance);
    }
}

/**
 * @brief  pmu xtal32m get param
 *
 * @param[in] load_capacitance  load capacitance
 **/
void pmu_xtal32m_get_param(int *load_capacitance)
{
    *load_capacitance = REGR(&XH_PMU->CLK_CTRL_2, MASK_POS(PMU_REG_CTUNE_XTAL));
}

/**
 * @brief pmu gpio wakeup pin setup
 *
 * @note In XH98xx A1, the GPIO19 can't be used as WAKEUP io.
 *
 * @param[in] pin_mask  pin mask
 * @param[in] trigger_type  wakeup trigger type
 *
 * @return None
 **/
void pmu_wakeup_pin_set(uint32_t pin_mask, pmu_pin_wakeup_type_t trigger_type)
{
    CO_DISABLE_IRQ();

    switch(trigger_type)
    {
        case PMU_PIN_WAKEUP_DISABLE:
            register_set0(&XH_PMU->GPIO_WAKEUP, pin_mask);
            register_set0(&XH_PMU->GPIO_POL, pin_mask);
            break;

        case PMU_PIN_WAKEUP_LOW_LEVEL: // FALLING_EDGE
            register_set1(&XH_PMU->GPIO_WAKEUP, pin_mask);
            register_set1(&XH_PMU->GPIO_POL, pin_mask);
            break;

        case PMU_PIN_WAKEUP_HIGH_LEVEL: // RISING_EDGE
            register_set1(&XH_PMU->GPIO_WAKEUP, pin_mask);
            register_set0(&XH_PMU->GPIO_POL, pin_mask);
            break;
    }

    register_set(&XH_PMU->WAKE_DEB, PMU_PIN_WAKE_LEVEL_EDGE_SEL_MASK | PMU_PIN_DEBOUNCE_CYCLE_WAKE_MASK |
                                    PMU_PIN_DEBOUNCE_COEFF_WAKE_MASK | PMU_PIN_DEB_RST_MASK,
                                    PMU_PIN_WAKE_LEVEL_EDGE_SEL_1ST_MASK | PMU_PIN_DEB_RST_MASK);
    XH_PMU_UPD_RDY();

    // clear interrupt
    register_set1(&XH_PMU->MISC_CTRL, PMU_MISC_CLR_PMU_INT_MASK);
    pmu_wakeup_pin_wait_idle();

    CO_RESTORE_IRQ();

    if(trigger_type != PMU_PIN_WAKEUP_DISABLE)
    {
        // Enable IRQ
        NVIC_ClearPendingIRQ(PIN_WAKEUP_IRQn);
        NVIC_SetPriority(PIN_WAKEUP_IRQn, IRQ_PRIORITY_NORMAL);
        NVIC_EnableIRQ(PIN_WAKEUP_IRQn);
    }
}

/**
 * @brief pmu wakeup pin register callback
 *
 * @param[in] sleep_callback  callback
 * @param[in] deep_sleep_callback callback
 *
 * @note
 *  When deepsleep, xtal32k startup is very very slow (0.5s-2s),
 *  So the deepsleep pin wakeup irq may don't debounce
 *  @ref pmu_32k_enable_in_deep_sleep()
 *
 * @return None
 **/
void pmu_wakeup_pin_register_callback(gpio_callback_t sleep_callback, gpio_callback_t deep_sleep_callback)
{
    pmu_env.pin_wakeup_sleep_callback = sleep_callback;
//    pmu_env.pin_wakeup_deep_sleep_callback = deep_sleep_callback;
}

/**
 * @brief Force system to reboot
 *
 * @return None
 **/
void pmu_force_reboot(void)
{
    // Disable ALL IRQ, MUST use __set_PRIMASK(1)
     __set_PRIMASK(1);

    // Set flag
    XH_PMU->SW_STATUS &= ~PMU_SW_STATUS_REBOOT_SW_MASK;

    // Set flag
    XH_PMU->SW_STATUS |= PMU_SW_STATUS_REBOOT_FORCE_MASK;

    // Make sure the isp bit is cleared
    XH_PMU->BOOT_SEL &= ~1u; // @ref BOOT_SEL_ISP_BIT

    // Remap and Reset
    pmu_reset_to_rom();

    // Never come here
    while(1);
}

/**
 * @brief pmu validate the configuration
 *
 * @return None
 **/
void pmu_configuration_validate(void)
{
}

/**
 * @brief Get charge status
 *
 * @return status
 **/
pmu_charge_status_t pmu_charge_status(void)
{
    volatile uint32_t charge_status = XH_SYS->CHRGR_STAT;

    if(charge_status & SYS_CHRGR_INSERT_DETECT_MASK)
    {
        if(charge_status & SYS_CHRGR_FINISH_MASK)
            return PMU_CHARGE_COMPLETE;
        else
            return PMU_CHARGE_CHARGING;
    }
    else
    {
        return PMU_CHARGE_EXTRACT;
    }
}

/**
 * @brief pmu_reboot_reason()
 *
 * @return 
 **/
pmu_reboot_reason_t pmu_reboot_reason(void)
{
    if(XH_PMU->SW_STATUS & PMU_SW_STATUS_REBOOT_FROM_WDT_MASK)
        return PMU_REBOOT_FROM_WDT;

    if(XH_PMU->SW_STATUS & PMU_SW_STATUS_REBOOT_FROM_IFLASH_LOW_V_MASK)
        return PMU_REBOOT_FROM_IFLASH_LOW_V;

    if(XH_PMU->SW_STATUS & PMU_SW_STATUS_REBOOT_FROM_SLEEP_MASK)
        return PMU_REBOOT_FROM_ULTRA_DEEP_SLEEP;

    if(XH_PMU->SW_STATUS & PMU_SW_STATUS_REBOOT_FROM_OTA_ISP_MASK)
        return PMU_REBOOT_FROM_OTA_ISP;

    if(XH_PMU->SW_STATUS & PMU_SW_STATUS_REBOOT_FORCE_MASK)
        return PMU_REBOOT_FROM_USER;

    return PMU_REBOOT_FROM_POWER_ON;
}

/**
 * @brief get retention reg value
 *
 * @note This reg value will lost only after power down. default is 0x0000
 *
 * @return retention reg value
 **/
uint16_t pmu_retention_reg_get(void)
{
    return (uint16_t)register_get(&XH_PMU->SW_STATUS, MASK_POS(PMU_SW_STATUS_USER_RETENTION));
}

/**
 * @brief set retention reg
 *
 * @note This reg value will lost only after power down. default is 0x0000
 *
 * @param[in] data  reg value
 *
 * @return None
 **/
void pmu_retention_reg_set(uint16_t value)
{
    register_set(&XH_PMU->SW_STATUS, MASK_1REG(PMU_SW_STATUS_USER_RETENTION, value));
}

#ifdef PMU_DETECT_LOWPOWER_TIME
/**
 * @brief Detect lowpower time
 *
 * @param[in] pin  detect pin num, 0 to disable detect
 *
 * @return None
 **/
void pmu_detect_lowpower_time(uint8_t pin)
{
    pinmux_config(pin, PINMUX_GPIO_MODE_CFG);
    pmu_pin_mode_set(BIT_MASK(pin), PMU_PIN_MODE_PP);
    gpio_write(BIT_MASK(pin), (pin&0x80) ? GPIO_HIGH : GPIO_LOW);
    gpio_set_direction(BIT_MASK(pin), GPIO_OUTPUT);

    pmu_env.detect_lowpower_time_pin = pin;
}
#endif

/**
 * @brief  pmu recalib sysclk
 **/
void pmu_topclk_recalib(void)
{
    CPM_ANA_AHB_CLK_ENABLE();

    pmu_xtal32m_fast_startup(false);

    // power on rc32m and switch to it
    // Make sure CPU running on RC32M
    pmu_topclk_rc32m_power_enable(true);
    pmu_topclk_switch_to_rc32m();

    // calib RC
    calib_sys_rc();

    // to xtal32m
    pmu_topclk_switch_to_xtal32m();

    // calib RC32M
    calib_sys_rc32m();

    if (pmu_topclk_xtal32m_x2_is_enabled())
    {
        // to rc32m
        pmu_topclk_switch_to_rc32m();

        // delay 2us
        co_delay_us(2);

        // to xtal64m
        pmu_topclk_x2_enable(true);
        pmu_topclk_switch_to_xtal32m_x2();
    }

    // power off rc32m
    pmu_topclk_rc32m_power_enable(false);

    CPM_ANA_CLK_RESTORE();
}

/**
 * @brief  pmu xtal32m x2 close
 **/
void pmu_xtal32m_x2_close(void)
{
    CPM_ANA_AHB_CLK_ENABLE();

    // startup xtal32m x2
    if (pmu_topclk_xtal32m_is_enabled() && pmu_topclk_xtal32m_x2_is_enabled())
    {
        // to rc32m
        pmu_topclk_rc32m_power_enable(true);
        pmu_topclk_switch_to_rc32m();

        // delay 2us
        co_delay_us(2);

        // switch to xtal32m
        pmu_topclk_switch_to_xtal32m();

        // disable x2
        pmu_topclk_x2_enable(false);

        // power off rc32m
        pmu_topclk_rc32m_power_enable(false);
    }

    CPM_ANA_CLK_RESTORE();
}

/**
 * @brief pmu dump
 *
 * @param[in] printf_dump_func  like printf
 *
 * @note
 *
 * The dump infomation looks like this:
 *   [PMU] prevent_status=00000000
 *   [PMU] wakeup_pin=0001000004(cur_level=0001000004 sleep_level=0001000004)
 *   [PMU] pull_up=FFFD7F9CDF(cur_level=FFFD7F9CDC) pull_down=0000000000(cur_level=0000000000) all_cur_level=FFFFFFFFFC
 *   [PMU] clocking: CPU(128MHz) SRAM(000087FF,ULP:32MHz) SF0 SF1 UART0 GPIO ANA
 *
 * Explain:
 * 1st line:
 *   Something (peripheral, user...) prevent system sleep.
 *   bitmask reference @ref pmu_lowpower_peripheral_t
 * 2nd line:
 *   Bitmask of wakeup pin.
 *   If cur_level != sleep_level, system can't sleep.
 * 3rd line:
 *   Inside pull-up and pull-down status.
 *   if pull_up is not equal to it's cur_level, symtem has current leakage in sleep.
 *   if pull_down's cur_level is not equal to 0, system has current leakage in sleep.
 * 4th line:
 *   Working modules.
 *   SRAM: powered block, the more block are powered on, the greater the current consumption during sleep.
 *         reference: @ref pmu_ram_power_on and @ref pmu_ram_power_off_invalid_block
 **/
void pmu_dump(void *printf_dump_func)
{
    void (*__printf)(const char *format, ...) = (void (*)(const char *format, ...))printf_dump_func;
    uint32_t current_level_mask = XH_GPIO0->DATA;
    uint32_t wakeup_mask = XH_PMU->GPIO_WAKEUP;
    uint32_t pull_up_mask = XH_PMU->GPIO_PU_CTRL;
    uint32_t pull_down_mask = XH_PMU->GPIO_PD_CTRL;
    uint32_t pol_mask = XH_PMU->GPIO_POL;

    // Prevent status
    __printf("[PMU] prevent_status=%08X\n", pmu_lowpower_prevent_status());

    // Wakeup pin
    __printf("[PMU] wakeup_pin=%08X(cur_level=%08X sleep_level=%08X)\n",
            wakeup_mask, current_level_mask&wakeup_mask, pol_mask);

    // Pull status
    __printf("[PMU] pull_up=%08X(cur_level=%08X) pull_down=%08X(cur_level=%08X) all_cur_level=%08X\n",
            pull_up_mask, current_level_mask&pull_up_mask, pull_down_mask, current_level_mask&pull_down_mask, current_level_mask);

    // Clocking
    __printf("[PMU] clocking: CPU(%dMHz)", cpm_get_clock(CPM_CPU_CLK)/1000000);
    if(!(XH_PSO->SF0_CFG & CPM_GATE_EN_MASK))          __printf(" SF0");
    if(!(XH_PSO->SF1_CFG & CPM_GATE_EN_MASK))          __printf(" SF1");
    if(!(XH_PSO->TIM_CFG[0] & CPM_GATE_EN_MASK))       __printf(" TIMER0");
    if(!(XH_PSO->TIM_CFG[1] & CPM_GATE_EN_MASK))       __printf(" TIMER1");
    if(!(XH_PSO->TIM_CFG[2] & CPM_GATE_EN_MASK))       __printf(" TIMER2");
    if(!(XH_PSO->UART1_CFG & CPM_GATE_EN_MASK))        __printf(" UART1");
    if(!(XH_PSO->SPI0_CFG & CPM_GATE_EN_MASK))         __printf(" SPI0");
    if(!(XH_PSO->DMA_CFG & CPM_GATE_EN_MASK))          __printf(" DMA");
    if(!(XH_PSO->GPIO_CFG & CPM_GATE_EN_MASK))         __printf(" GPIO");
    if(!(XH_PSO->ANA_IF_CLK_CFG & CPM_GATE_EN_MASK))   __printf(" ANA");
    __printf("\n");
}

/**
 * @brief PIN_WAKEUP_IRQHandler()
 *
 * @return 
 **/
void PIN_WAKEUP_IRQHandler(void)
{
    uint32_t int_status = XH_PMU->GPIO_LATCH;

    // clear interrupt
    register_set1(&XH_PMU->MISC_CTRL, PMU_MISC_CLR_PMU_INT_MASK);

    // Sleep wakeup: may 2ms delay
    if(pmu_env.pin_wakeup_sleep_recently)
    {
        // clear gpio irq pending
        gpio_clear_interrupt_pending(int_status);

        // Callback
        if(pmu_env.pin_wakeup_sleep_callback && int_status)
            pmu_env.pin_wakeup_sleep_callback(int_status);

        // clear
        pmu_env.pin_wakeup_sleep_recently = false;
    }
}

/** @} */

