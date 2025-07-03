/**
 * @file peripheral.h
 * @brief peripheral header
 * @date Thu 27 Nov 2014 04:49:22 PM CST
 * @author XIHAO
 *
 * @defgroup PERIPHERAL Peripheral
 * @ingroup XH98xx
 * @brief Peripheral drivers
 * @details
 *
 * The Peripheral Driver provides an interface of abstraction between the physical hardware and the application.
 * System-level software developers can use the driver to do the fast application software development,
 * instead of using the register level programming, which can reduce the total development time significantly.
 *
 * ### Features
 * - four channels DMA
 * - Two UART interface, one share with 7816 interface
 * - I2S interface
 * - Up to 40 bits general-purpose I/O GPIO
 * - Three I2C master or slave interface
 * - Two SPI master or slave interface
 * - Watchdog to prevent system dead lock
 * - RTC
 * - Three 32bit timers
 * - Keyboard controller
 * - Three way QDEC
 * - Eight single-end or differential-end 12bits GP-ADC
 * - AES HW encryption
 * - Audio ADC
 * - PPG Heart rate detection
 * - Touch sensor controller
 *
 * @{
 */

#ifndef __PERIPHERAL_H__
#define __PERIPHERAL_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "xh98xx.h"

/*********************************************************************
 * MACROS
 */

/// @cond

__STATIC_INLINE void XH_PSO_STATUS_CLR(void)
{
    do {
        XH_PSO->REG_UPD = CPM_REG_UPD_STATUS_CLR_MASK;
    }while(XH_PSO->REG_UPD);
}
__STATIC_INLINE void XH_PSO_UPD_RDY(void)
{
    XH_PSO_STATUS_CLR();
    XH_PSO->REG_UPD = CPM_REG_UPD_CPU_MASK;
    while((XH_PSO->REG_UPD & CPM_REG_UPD_CPU_STATUS_MASK) == 0);
}
__STATIC_INLINE void XH_PSO_32K_UPD_RDY(void)
{
    XH_PSO_STATUS_CLR();
    XH_PSO->REG_UPD = CPM_REG_UPD_CPU_MASK|CPM_REG_UPD_32K_MASK;
    while((XH_PSO->REG_UPD & (CPM_REG_UPD_CPU_STATUS_MASK|CPM_REG_UPD_32K_STATUS_MASK)) == 0);
}
__STATIC_INLINE void XH_PSO_XTAL32M_UPD_RDY(void)
{
    XH_PSO_STATUS_CLR();
    XH_PSO->REG_UPD = CPM_REG_UPD_CPU_MASK|CPM_REG_UPD_XTAL32M_MASK;
    while((XH_PSO->REG_UPD & (CPM_REG_UPD_CPU_STATUS_MASK|CPM_REG_UPD_XTAL32M_STATUS_MASK)) == 0);
}
__STATIC_INLINE void XH_PMU_UPD_RDY(void)
{
    register_set1(&XH_PMU->BASIC, PMU_BASIC_APB_BUS_UPD_REG_MASK);
    while(XH_PMU->BASIC & PMU_BASIC_APB_BUS_UPD_REG_MASK);
    while(XH_PMU->STATUS_READ & PMU_APB_BUS_UPD_DATA_MASK);
}

/// @endcond

/*********************************************************************
 * INCLUDES
 */

#include "co.h"
#include "gpio.h"
#include "uart.h"
#include "cpm.h"
#include "pmu.h"
#include "sf_base.h"
#include "sf.h"
#include "sf_sys.h"
#include "otp.h"
#include "efuse.h"
#include "calib.h"
#include "calib_repair.h"
#include "encoder_sim.h"
#include "wdt.h"
#include "dma_nds.h"
#include "adc.h"
#include "i2c_sim.h"
#include "spi.h"
#include "kpp_sim.h"
#include "timer.h"
#include "pinmux.h"
#include "lcd_serial.h"
#include "i2c.h"

/*********************************************************************
 * MACROS
 */

/// @cond

// Calibarate in controller_test.c with keil -O3 (24MHz), result is 3, set to 4 more safe
#define TIMEOUT_COEFFICIENT     (2*4) // x2 for 48MHz

#define WAIT_TIMEOUT_EX(event, us) \
    do{register uint32_t timeout; for(timeout=(us)*TIMEOUT_COEFFICIENT; timeout && !(event); --timeout){__NOP();}}while(0)

#if defined(CONFIG_SIMULATION) || defined(CONFIG_DEBUG)
#define WAIT_TIMEOUT(event, us) do{while(!(event)){}}while(0)
#else
// Wait read !!!ONE!!! register status.
#define WAIT_TIMEOUT(event, us) WAIT_TIMEOUT_EX(event, us)
#endif

/// bit mask
#define BITMASK(n)              (1u<<(n))
#define BIT_MASK(n)             (1u<<(n))

/// IS PPGA
#define IS_FPGA()               ((XH_SYS->REV_ID & SYS_REV_FPGA_FLAG_MASK) == SYS_REV_FPGA_FLAG)

/// @endcond

/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */

/// @cond

/**
 * @brief peripheral lowpower store
 *
 * @return None
 **/
void peripheral_lowpower_enter(co_power_status_t power_status);

/**
 * @brief peripheral lowpower restore
 *
 * @return None
 **/
void peripheral_lowpower_leave(co_power_status_t power_status);

/// @endcond

#ifdef __cplusplus
}
#endif

#endif

/** @} */

