/**
 * @file pmu.h
 * @brief PMU driver
 * @date Thu 26 Nov 2015 04:42:25 PM CST
 * @author XIHAO
 *
 * @defgroup PMU PMU
 * @ingroup PERIPHERAL
 * @brief Power Manage Unit driver
 * @details 
 *
 * @{
 */

#ifndef __PMU_H__
#define __PMU_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "features.h"
#include "gpio.h"
#include "co.h"

/*********************************************************************
 * MACROS
 */

/// @ref pmu_lowpower_peripheral_t
#define PMU_LP_USER_N(n)                  ((pmu_lowpower_peripheral_t)((int)PMU_LP_USER<<(n)))

//#define PMU_DETECT_LOWPOWER_TIME

/*********************************************************************
 * TYPEDEFS
 */
/// PMU sleep state
typedef enum
{
    PMU_SLEEP,
    PMU_DEEP_SLEEP,
    PMU_SHUTDOWN,
    PMU_SLEEP_HXTAL_OFF,
}pmu_lowpower_state_t;

/// @cond
/// PMU analog power type
typedef enum
{
    PMU_ANA_RF          = 1<<0,
    PMU_ANA_ADC         = 1<<1,
    PMU_ANA_CALIB_RC32K = 1<<2,
    PMU_ANA_CALIB_RF    = 1<<3,
}pmu_ana_type_t;
/// @endcond

/// PMU Low power peripheral type
typedef enum
{
    PMU_LP_TIM0     = 1<<0,  // Prevent/Allow sleep, use function tim_start/tim_stop
    PMU_LP_TIM1     = 1<<1,  // Prevent/Allow sleep, use function tim_start/tim_stop
    PMU_LP_TIM2     = 1<<2,  // Prevent/Allow sleep, use function tim_start/tim_stop
    PMU_LP_UART0    = 1<<3,  // User control
    PMU_LP_UART1    = 1<<4,  // User control
    PMU_LP_I2C0     = 1<<5,  // User control
    PMU_LP_I2C1     = 1<<6,  // User control
    PMU_LP_SPI0     = 1<<7,  // User control
    PMU_LP_SPI1     = 1<<8,  // User control
    PMU_LP_WDT      = 1<<9,  // Prevent/Allow deep sleep, use function wdt_enable
    PMU_LP_ENCODER0 = 1<<10, // Prevent/Allow sleep, use function encoder_start/encoder_stop
    PMU_LP_ENCODER1 = 1<<11, // Prevent/Allow sleep, use function encoder_start/encoder_stop
    PMU_LP_ENCODER2 = 1<<12, // Prevent/Allow sleep, use function encoder_start/encoder_stop
    PMU_LP_KPP      = 1<<13, // Prevent/Allow sleep automatically
    PMU_LP_I2S      = 1<<14, // Prevent/Allow sleep, use function i2s_start/i2s_stop
    PMU_LP_DMA_CH0  = 1<<15, // Prevent/Allow sleep, use function dma_start/dma_stop
    PMU_LP_DMA_CH1  = 1<<16, // Prevent/Allow sleep, use function dma_start/dma_stop
    PMU_LP_DMA_CH2  = 1<<17, // Prevent/Allow sleep, use function dma_start/dma_stop
    PMU_LP_DMA_CH3  = 1<<18, // Prevent/Allow sleep, use function dma_start/dma_stop
    PMU_LP_ADC      = 1<<19, // Prevent/Allow sleep, use function encoder_start/encoder_stop
    PMU_LP_RTC      = 1<<20, // Prevent/Allow 'deep sleep', use function rtc_start/rtc_stop
    PMU_LP_SF0      = 1<<21, // System auto control
    PMU_LP_SF1      = 1<<22, // System auto control
    PMU_LP_SF2      = 1<<23, // system auto control
    PMU_LP_DMA_CH4  = 1<<24, // Prevent/Allow sleep, use function dma_start/dma_stop
    PMU_LP_DMA_CH5  = 1<<25, // Prevent/Allow sleep, use function dma_start/dma_stop
    PMU_LP_DMA_CH6  = 1<<26, // Prevent/Allow sleep, use function dma_start/dma_stop
    PMU_LP_DMA_CH7  = 1<<27, // Prevent/Allow sleep, use function dma_start/dma_stop
    PMU_LP_USER     = 1<<28, // Prevent/Allow sleep for user used
    PMU_LP_ALL      = ~0,
}pmu_lowpower_peripheral_t;

/// 32k select, work with RTC, baseband in sleep
typedef enum
{
    PMU_32K_SEL_RC           = 0,
    PMU_32K_SEL_32768HZ_XTAL = 1, // XH98xx no xtal32k
    PMU_32K_SEL_DIV          = 2,
}pmu_32k_sel_t;

/// PMU remap
typedef enum
{
    /// ROM
    PMU_REMAP_FROM_ROM = 1,
    /// RAM
    PMU_REMAP_FROM_RAM = 2,
    /// SFLASH inside
    PMU_REMAP_FROM_SFLASH = 4,
    /// OTP
    PMU_REMAP_FROM_OTP = 8,
}pmu_remap_t;

/// Mode
typedef enum
{
    /// Float (Input)
    PMU_PIN_MODE_FLOAT,
    /// Push pull (Output)
    PMU_PIN_MODE_PP,
    /// Pull up (221kOHM~394kOHM)
    PMU_PIN_MODE_PU,
    /// Pull down (141kOHM~303kOHM)
    PMU_PIN_MODE_PD,
    /// Open drain
    PMU_PIN_MODE_OD,
    /// Open drain, pull up
    PMU_PIN_MODE_OD_PU,
}pmu_pin_mode_t;

/// Interrupt trigger type
typedef enum
{
    PMU_PIN_WAKEUP_LOW_LEVEL,
    PMU_PIN_WAKEUP_HIGH_LEVEL,
    PMU_PIN_WAKEUP_DISABLE,
}pmu_pin_wakeup_type_t;

/// Charge status
typedef enum
{
    /// Charger extract
    PMU_CHARGE_EXTRACT,
    /// Charger insert and charging
    PMU_CHARGE_CHARGING,
    /// Charger insert and charge complete
    PMU_CHARGE_COMPLETE,
}pmu_charge_status_t;

/// Reboot reason
typedef enum
{
    /// Reboot from power on
    PMU_REBOOT_FROM_POWER_ON,
    /// Reboot from ultra deep sleep
    PMU_REBOOT_FROM_ULTRA_DEEP_SLEEP,
    /// Reboot from OTA ISP (DISCARD)
    PMU_REBOOT_FROM_OTA_ISP,
    /// Users take the initiative to reboot
    PMU_REBOOT_FROM_USER,
    /// Reboot from watch dog
    PMU_REBOOT_FROM_WDT,
    /// Reboot from inside flash low voltage reset
    PMU_REBOOT_FROM_IFLASH_LOW_V,
}pmu_reboot_reason_t;

/// Driven current
typedef enum
{
    PMU_PIN_DRIVEN_CURRENT_2MA  = 0, ///< obsolete
    PMU_PIN_DRIVEN_CURRENT_4MA  = 1, ///< obsolete
    PMU_PIN_DRIVEN_CURRENT_6MA  = 2, ///< obsolete
    PMU_PIN_DRIVEN_CURRENT_12MA = 3, ///< obsolete
    PMU_PIN_DRIVEN_CURRENT_5MA  = 1, ///< obsolete
    PMU_PIN_DRIVEN_CURRENT_10MA = 2, ///< obsolete
    PMU_PIN_DRIVEN_CURRENT_20MA = 3, ///< obsolete
    PMU_PIN_DRIVEN_CURRENT_MIN  = 0, ///< Please reference datasheet
    PMU_PIN_DRIVEN_CURRENT_MAX  = 3, ///< Please reference datasheet
}pmu_pin_driven_current_t;

/// Pull up resistance
typedef enum
{
    PMU_PIN_PU_RESISTANCE_4K    = 0,
    PMU_PIN_PU_RESISTANCE_10K   = 1,
    PMU_PIN_PU_RESISTANCE_300K  = 2,
    PMU_PIN_PU_RESISTANCE_2M    = 3,
}pmu_pin_pu_resistance_t;

/**
 * @brief timer overflow callback
 **/
typedef void (* pmu_timer_overflow_callback_t)(void);

/// @cond
typedef struct
{
    volatile uint32_t ana_mask;
    volatile uint32_t lp_state;
    gpio_callback_t pin_wakeup_sleep_callback;
    volatile bool pin_wakeup_sleep_recently;
    bool enable_32k_with_deep_sleep;

#ifdef PMU_DETECT_LOWPOWER_TIME
    uint8_t detect_lowpower_time_pin;
#endif

    // just for debug
    bool enable_swd_in_sleep;
    // force ie
    uint32_t pin_sleep_force_enable_ie_mask;
}pmu_env_t;

/*********************************************************************
 * EXTERN VARIABLES
 */
extern pmu_env_t pmu_env;

/*********************************************************************
 * EXTERN FUNCTIONS
 */

/**
 * @brief pmu initialize
 *
 * @return None
 **/
void pmu_init(void);

/**
 * @brief pmu select xtal32m as top clock, call by system
 *
 * @return None
 **/
void pmu_xtal32m_startup(void);
/// @endcond

/**
 * @brief  pmu xtal32m x2 startup that lead system top clock is 64MHz
 **/
void pmu_xtal32m_x2_startup(void);

/**
 * @brief  pmu xtal32m fast startup
 *
 * @param[in] force  force
 **/
void pmu_xtal32m_fast_startup(bool force);

/**
 * @brief  pmu xtal32m x2 close
 **/
void pmu_xtal32m_x2_close(void);

/**
 * @brief  pmu random seed fetch
 *
 * @param[in] pre_random  previous
 *
 * @return random seed
 **/
uint32_t pmu_random_seed_fetch(uint32_t pre_random);

/**
 * @brief pmu select 32k
 *
 * @param[in] clk32k  32k clock select
 *
 * @return None
 *
 * @note Must be called before stack init
 **/
void pmu_select_32k(pmu_32k_sel_t clk32k);

/**
 * @brief pmu get 32k select
 *
 * @return 32k select
 **/
//pmu_32k_sel_t pmu_select_32k_get(void);

/**
 * @brief xtal32k enable in deep sleep
 *
 * @note
 *  The xtal 32768Hz crystal startup is very very slow (0.5s~2s),
 *  Its current consumption is about 1uA,
 *  Enable it in deepsleep, can make the wakeup faster
 *
 * @param[in] enable  enable or disable
 *
 * @return None
 **/
void pmu_32k_enable_in_deep_sleep(bool enable);

/**
 * @brief Change xtal 32k params
 *
 * @param[in] load_capacitance  load capacitance, range:0~15 default:8 step:0.84pF, max:12.6pF
 * @param[in] drive_current  drive current, range:0-3, default:1
 *
 * @note load_capacitance may effect xtal 32k startup time and precision,
 *       appropriate value can speed up startup time.
 *
 * @return None
 **/
void pmu_xtal32k_change_param(int load_capacitance, int drive_current);

/**
 * @brief Change xtal 24m params
 *
 * @param[in] load_capacitance  load capacitance, range:0~31, default:15, step:0.75pf, max:23.25pF
 *
 * @note load_capacitance will effect xtal 24m precision and frequency offset.
 *
 * @return None
 **/
void pmu_xtal32m_change_param(int load_capacitance);

/**
 * @brief  pmu xtal32m get param
 *
 * @param[in] load_capacitance  load capacitance
 **/
void pmu_xtal32m_get_param(int *load_capacitance);

/**
 * @brief pmu enable dcdc
 *
 * @param[in] enable  enable/disable
 *
 * @return None
 **/
void pmu_dcdc_enable(bool enable);

/**
 * @brief pmu gpio wakeup pin setup
 *
 * @note In XH98xx A1, the PA22 can't be used as WAKEUP io.
 *
 * @param[in] pin_mask  pin mask
 * @param[in] trigger_type  wakeup trigger type
 *
 * @return None
 **/
void pmu_wakeup_pin_set(uint32_t pin_mask, pmu_pin_wakeup_type_t trigger_type);

/**
 * @brief wakeup pin get
 *
 * @return wakeup pin mask
 **/
uint32_t pmu_wakeup_pin_get(void);

/**
 * @brief pmu wakeup pin register callback
 *
 * @param[in] sleep_callback  callback
 * @param[in] deep_sleep_callback callback
 *
 * @note
 *  When deepsleep, xtal32k startup is very very slow (0.5s-2s),
 *  So the deepsleep pin wakeup irq may don't debounce
 *
 * @return None
 **/
void pmu_wakeup_pin_register_callback(gpio_callback_t sleep_callback, gpio_callback_t deep_sleep_callback);

/**
 * @brief pmu prevent lowpower entry
 *
 * @param[in] lpp  peripheral
 *
 * @return None
 **/
void pmu_lowpower_prevent(pmu_lowpower_peripheral_t lpp);

/**
 * @brief pmu allow lowpower entry
 *
 * @param[in] lpp  peripheral
 *
 * @return None
 **/
void pmu_lowpower_allow(pmu_lowpower_peripheral_t lpp);

/**
 * @brief lowpower is prevented
 *
 * @return prevented ?
 **/
pmu_lowpower_peripheral_t pmu_lowpower_prevent_status(void);

/**
 * @brief what power status should be entried
 *
 * @return power status
 **/
co_power_status_t pmu_power_status(void);

/**
 * @brief pmu enter lowpower, call by system
 *
 * @param[in] state  lowpower state
 *
 * @return None
 **/
void pmu_lowpower_enter(pmu_lowpower_state_t state);

/**
 * @brief pmu leave lowpower status, call by system
 *
 * @param[in] state  lowpower state
 *
 * @return None
 **/
void pmu_lowpower_leave(pmu_lowpower_state_t state);

/**
 * @brief pmu pin state store, call by system
 *
 * @return None
 **/
void pmu_pin_state_store(void);

/**
 * @brief pmu pin state restore, call by system
 *
 * @return None
 **/
void pmu_pin_state_restore(void);

/**
 * @brief Set pin mode
 *
 * @param[in] pin_mask  pin mask
 * @param[in] mode  pin mode
 *
 * @return None
 **/
void pmu_pin_mode_set(uint32_t pin_mask, pmu_pin_mode_t mode);

/**
 * @brief Set gpio driven current
 *
 * @param[in] pin_mask  pin mask
 * @param[in] driven  current driven (Large driven current should be push-pull output)
 *
 * @return None
 **/
void pmu_pin_driven_current_set(uint32_t pin_mask, pmu_pin_driven_current_t driven);

/// @cond
/**
 * @brief pmu reversion check, call by system
 *
 * @return None
 **/
void pmu_reversion_check(void);
/// @endcond

/**
 * @brief pmu enable shutdown pin
 *
 * @return None
 **/
void pmu_shutdown_pin_enable(void);

/// @cond
/**
 * @brief pmu analog power enable, call by system
 *
 * @param[in] enable  enable/disable
 * @param[in] ana  analog type
 *
 * @return None
 **/
void pmu_ana_enable(bool enable, pmu_ana_type_t ana);

/**
 * @brief analog is enabled
 *
 * @param[in] ana  analog module
 *
 * @return enabled?
 **/
bool pmu_ana_is_enabled(pmu_ana_type_t ana);
/// @endcond

/**
 * @brief Force system to reboot
 *
 * @return None
 **/
void pmu_force_reboot(void);

/**
 * @brief Force into reboot sleep mode
 *
 * Power consumption is lower than the deep sleep mode. All SRAM will be powered down.
 * But chip will be reboot from ROM when wakeup.
 *
 * @return None
 **/
void pmu_force_into_reboot_sleep_mode(uint32_t ram_retention);

/// @cond
/**
 * @brief pmu validate the configuration
 *
 * @return None
 **/
void pmu_configuration_validate(void);
/// @endcond

/**
 * @brief Get charge status
 *
 * @return status
 **/
pmu_charge_status_t pmu_charge_status(void);

/**
 * @brief pmu reboot reason
 *
 * @return reason
 **/
pmu_reboot_reason_t pmu_reboot_reason(void);

/**
 * @brief get retention reg value
 *
 * @note This reg value will lost only after power down.
 *
 * @return retention reg value
 **/
uint16_t pmu_retention_reg_get(void);

/**
 * @brief set retention reg
 *
 * @note This reg value will lost only after power down.
 *
 * @param[in] value  reg value
 *
 * @return None
 **/
void pmu_retention_reg_set(uint16_t value);

/// @cond
/**
 * @brief pmu_wait_system_ready()
 *
 * @return 
 **/
void pmu_wait_system_ready(void);

/**
 * @brief Detect lowpower time
 *
 * @param[in] pin  detect pin num, 0 to disable detect
 *
 * @return None
 **/
void pmu_detect_lowpower_time(uint8_t pin);

/**
 * @brief  pmu recalib sysclk
 **/
void pmu_topclk_recalib(void);
/// @endcond

/**
 * @brief pmu enable INSIDE dcdc
 *
 * @param[in] enable  enable/disable
 *
 * @return None
 **/
void pmu_dcdc_enable(bool enable);

/**
 * @brief  pmu dcdc is enabled
 *
 * @return  is enabled
 **/
bool pmu_dcdc_is_enabled(void);

/**
 * @brief pmu dump
 *
 * @param[in] printf_dump_func  like printf
 *
 * @note
 *
 * @verbatim
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
 * @endverbatim
 **/
void pmu_dump(void *printf_dump_func);

/**
 * @brief reset cpu to ROM
 *
 * @return None
 **/
void pmu_reset_to_rom(void);

/**
 * @brief reset cpu to RAM
 *
 * @return None
 **/
void pmu_reset_to_ram(void);

/**
 * @brief reset cpu to OTP
 *
 * @return None
 **/
void pmu_reset_to_otp(void);

/// @cond
/**
 * @brief pmu reset cpu
 *
 * Note: This function must be INLINE function, avoid stack used
 *
 * @return None
 **/
__STATIC_FORCEINLINE void pmu_cpu_reset(void)
{
    // flag
    register_set1(&XH_PMU->SW_STATUS, PMU_SW_STATUS_REBOOT_FROM_SOFT_RESET_MASK);

    // Use DMB to wait until all outstanding
    // memory accesses are completed
    __DMB();

#if 0
    register_set0(&XH_PMU->AHB_REMAP, PMU_CPU_SOFT_RESET_MASK);
#else
    // Read back PRIGROUP and merge with SYSRESETREQ
    SCB->AIRCR = 0x05FA0001 | (SCB->AIRCR & 0x700);
#endif
}

/**
 * @brief pmu memery address remap
 *
 * @param[in] remap  remap mode
 *
 * Note: This function must be INLINE function, avoid stack used
 *
 * @return None
 **/
__STATIC_FORCEINLINE void pmu_memery_remap(pmu_remap_t remap)
{
    // MUST write EN firstly, then write REMAP value
    register_set(&XH_PMU->AHB_REMAP, MASK_1REG(PMU_CHIP_AHB_BUS_REMAP_EN, 1));
    register_set(&XH_PMU->AHB_REMAP, MASK_1REG(PMU_CHIP_AHB_BUS_REMAP, remap));
}
/// @endcond

/**
 * @brief get ultra deep sleep wakeup pin value
 *
 * @note Only valid when pmu_reboot_reason() return PMU_REBOOT_FROM_ULTRA_DEEP_SLEEP.
 * @note Must be called before any pmu_wakeup_pin_set() calling.
 *
 * @return ultra deep sleep wakeup pin value
 **/
__STATIC_INLINE uint32_t pmu_ultra_deep_sleep_wakeup_pin(void)
{
    return XH_PMU->GPIO_LATCH;
}

/**
 *******************************************************************************
 * @brief  pmu pull up resistance set
 *
 * @param[in] resistance  resistance
 *******************************************************************************
 */
__STATIC_INLINE void pmu_pull_up_resistance_set(uint32_t pin_mask, pmu_pin_pu_resistance_t resistance)
{
    if(resistance & 0x01)
        XH_PMU->GPIO_DRV_CTRL_2 |= pin_mask;
    else
        XH_PMU->GPIO_DRV_CTRL_2 &= ~pin_mask;

    if(resistance & 0x02)
        XH_PMU->GPIO_POL_1 |= pin_mask;
    else
        XH_PMU->GPIO_POL_1 &= ~pin_mask;
}

/**
 *******************************************************************************
 * @brief  pmu pin io10 reset mode enable
 *
 * @note  IO10 is RESET mode by default.
 *
 * @param[in] enable  enable
 *******************************************************************************
 */
__STATIC_INLINE void pmu_pin_pa6_reset_mode_enable(bool enable)
{
    //   0:  IO10 is RESET
    //   1:  IO10 is GPIO
    REGW(&XH_PMU->MISC_CTRL_1, MASK_1REG(PMU_MISC_MODE_DIGLDO, enable ? 0 : 1));
}

#ifdef __cplusplus
}
#endif

#endif

/** @} */

