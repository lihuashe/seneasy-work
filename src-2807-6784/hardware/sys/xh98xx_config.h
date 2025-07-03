/**
 * @file xh98xx_config.h
 * @brief 
 * @date 2015/07/16 14:52:07
 * @author XIHAO
 *
 * @addtogroup 
 * @ingroup 
 * @details 
 *
 * @{
 */

#ifndef __XH98xx_H__
#define __XH98xx_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * Configuration of the Cortex-M4 Processor and Core Peripherals
 */
#define __CHECK_DEVICE_DEFINES    1 /*!< CHECK */
#define __CM4_REV                 0x0001 /*!< CM4 r0p1 */
#define __FPU_PRESENT             0 /*!< FPU */
#define __MPU_PRESENT             0 /*!< MPU */
#define __NVIC_PRIO_BITS          4 /*!< 4 Bits for the Priority Levels    */
#define __Vendor_SysTickConfig    0 /*!< Set to 1 if different SysTick Config is used */

/*********************************************************************
 * Interrupt Number Definition
 */
typedef enum IRQn
{
    /******  Cortex-M4 Processor Exceptions Numbers ***************************************************/
    NonMaskableInt_IRQn         = -14,    /*!< 2 Non Maskable Interrupt                             */
    MemoryManagement_IRQn       = -12,    /*!< 4 Cortex-M4 Memory Management Interrupt              */
    BusFault_IRQn               = -11,    /*!< 5 Cortex-M4 Bus Fault Interrupt                      */
    UsageFault_IRQn             = -10,    /*!< 6 Cortex-M4 Usage Fault Interrupt                    */
    SVCall_IRQn                 = -5,     /*!< 11 Cortex-M4 SV Call Interrupt                       */
    DebugMonitor_IRQn           = -4,     /*!< 12 Cortex-M4 Debug Monitor Interrupt                 */
    PendSV_IRQn                 = -2,     /*!< 14 Cortex-M4 Pend SV Interrupt                       */
    SysTick_IRQn                = -1,     /*!< 15 Cortex-M4 System Tick Interrupt                   */

    /******  XH98xx specific Interrupt Numbers *********************************************************/
    BT_IRQn                     = 0,
    SLEEP_TIMER_IRQn            = 1,
    DMA_IRQn                    = 2,
    GPIO_IRQn                   = 3,
    TIM_IRQn                    = 4,
    MAC98xx_RF_IRQn             = 5,
    MAC98xx_SPI_IRQn            = 6,
    PMU_TIMER_IRQn              = 7,
#ifdef CONFIG_XH98xx
    IR_TX_IRQn                  = 8, // XH98xx
#else
    SF1_IRQn                    = 8, // XH98xxD/XH98xxF
#endif
    UART1_IRQn                  = 9,
#ifdef CONFIG_XH98xx
    EFUSE_IRQn                  = 10,
#else
    POWER_DOWN_IRQn             = 10,
#endif
    PIN_WAKEUP_IRQn             = 11,
    ADC_IRQn                    = 12,
#ifdef CONFIG_XH98xx
    I2C_IRQn                    = 13,
#else
    SPI0_IRQn                   = 13,
#endif
    SF_IRQn                     = 14,
    SOFT0_IRQn                  = 15,
    SOFT1_IRQn                  = 16,
    SOFT2_IRQn                  = 17,
    SOFT3_IRQn                  = 18,
    SOFT4_IRQn                  = 19,
    RTC_AF_IRQn                 = 20,
    RTC_1HZ_IRQn                = 21,
    RTC_BLE_IRQn                = 22,
    VTRACK_IRQn                 = 23,
    CRY32M_RDY_IRQn             = 24,
    UART0_IRQn                  = 25, // XH98xxF/XH98xx
    GPIO0_IRQn                  = 26,
    CC_INTR_IRQn                = 27,
    TIM0_IRQn                   = 28,
    TIM1_IRQn                   = 29,
    TIM2_IRQn                   = 30,
#ifdef CONFIG_XH98xx
    SF1_IRQn                    = 31, // XH98xx
    AUDIO_IRQn                  = 32, // XH98xx
    I2S_RX_IRQn                 = 33, // XH98xx
    I2S_TX_IRQn                 = 34, // XH98xx
#else
    CHARGER_IRQn                = 31, // XH98xxD/XH98xxF
    UART2_IRQn                  = 32, // XH98xxF
#endif
} IRQn_Type;

/*********************************************************************
 * INCLUDES
 */

#include "core_cm4.h"
#include <stdint.h>
#include "xh98xx_driver.h"

#ifdef __CC_ARM
#pragma anon_unions
#endif

/*********************************************************************
 * MACROS
 */

/// Base address
#define XH_SYS_BASE                 0x40000000
#define XH_CPM_PSO_BASE             0x40001000
#define XH_EFUSE_BASE               0x40002000
#define XH_RF_2_4G_BASE             0x40004000
#define XH_RANDOM_BASE              0x40004000
#define XH_CPM_ROM_P_BASE           0x40008000
#define XH_CPM_ROM_M_BASE           0x40009000
#define XH_MAC98xx_BASE             0x4000b000
#define XH_PHY_BASE                 0x40020000
#define XH_UART1_BASE               0x40040000
#define XH_AUDIO_BASE               0x40050000 // XH98xx (conflict with XH_SPI0_BASE)
#define XH_I2C_BASE                 0x40070000 // XH98xx
#define XH_SPI0_BASE                0x40050000
#define XH_SPI1_BASE                0x40060000
#define XH_UART0_BASE               0x40080000 // XH98xxF
#define XH_DAIF_BASE                0x400A0000
#define XH_GPADC_BASE               0x400A2000
#define XH_TIM0_BASE                0x400C0000
#define XH_TIM1_BASE                0x400C0100
#define XH_TIM2_BASE                0x400C0200
#define XH_PMU_BASE                 0x400e0000
#define XH_UART2_BASE               0x400f0000 // XH98xxF
#define XH_IR_TX_BASE               0x400f0000  //xh98xx
#define XH_DMAC_BASE                0x41100000
#define XH_DMA_CH0_BASE             (XH_DMAC_BASE + 0x44)
#define XH_DMA_CH1_BASE             (XH_DMA_CH0_BASE + 0x14)
#define XH_GPIO0_BASE               0x41200000
#define XH_AES_BASE                 0x41300000
#define XH_LCBB_BASE                0x41306000
#define XH_HABBB_BASE               0x41308000
#define XH_LEBB_BASE                0x4130A000
#define XH_LEBB_TX_BASE             0x41306200
#define XH_LEBB_RX_BASE             0x41306400
#define XH_I2S_TX_BASE              0x41400000
#define XH_I2S_RX_BASE              0x41500000
#define XH_SF0_BASE                 0x51000000
#define XH_SF1_BASE                 0x53000000
#ifdef CONFIG_XH98xxD
#define XH_OTP_BASE                 0x60004000
#else
#define XH_OTP_BASE                 0x60100000
#endif
#define XH_HCACHE_BASE              0xE0042000

// base address
#define XH_BUS_ROM_BASE              0x00100000
#define XH_BUS_SRAM_BASE             0x20000000
#define XH_BUS_SRAM_CODE_BASE        0x00200000
#define XH_BUS_SF_CACHABLE_BASE      0x00400000
#define XH_BUS_SF_CACHABLE_BASE_1    0x00800000
#define XH_BUS_OTP_CACHABLE_BASE     0x00C00000
#define XH_BUS_SF_NONCACHABLE_BASE   0x50000000
#define XH_BUS_SF_NONCACHABLE_BASE_1 0x52000000
#define XH_BUS_OTP_NONCACHABLE_BASE  0x60000000
#define XH_BUS_EFUSE_NONCACHABLE_BASE 0x40002000 // @ref XH_EFUSE->READ_DATA
#define XH_BUS_EFUSE_FAKE_BASE       0x70000000 // only used for ISP
#define XH_BUS_SF_BASE               XH_BUS_SF_CACHABLE_BASE
#define XH_BUS_SF_BASE_1             XH_BUS_SF_CACHABLE_BASE_1
#define XH_BUS_OTP_BASE              XH_BUS_OTP_CACHABLE_BASE
#define XH_BUS_BASE_MASK             0xFFC00000
#define XH_BUS_OFFSET_MASK           0x003FFFFF
#define XH_BUS_BASE(addr)            ((addr) & XH_BUS_BASE_MASK)
#define XH_BUS_OFFSET(addr)          ((addr) & XH_BUS_OFFSET_MASK)
#if defined(CONFIG_XH98xxD)
#define XH_BUS_ROM_SIZE             (24*1024)
#define XH_BUS_SRAM_SIZE            (24*1024)
#define XH_BUS_OTP_SIZE             (16*1024)
#elif defined(CONFIG_XH98xxF)
#define XH_BUS_ROM_SIZE             (64*1024)
#define XH_BUS_SRAM_SIZE            (16*1024)
#define XH_BUS_OTP_SIZE             (24*1024)
#elif defined(CONFIG_XH98xx)
#define XH_BUS_ROM_SIZE             (80*1024)
#define XH_BUS_SRAM_SIZE            (40*1024)
#define XH_BUS_OTP_SIZE             (0*1024)
#endif

/// Function
#define XH_SYS                      ((XH_SYS_Type *) XH_SYS_BASE)
#define XH_AES                      ((XH_AES_Type *) XH_AES_BASE)
#define XH_PSO                      ((XH_PSO_CPM_Type *) XH_CPM_PSO_BASE)
#define XH_EFUSE                    ((XH_EFUSE_Type *) XH_EFUSE_BASE)
#define XH_PMU                      ((XH_PMU_Type *) XH_PMU_BASE)
#define XH_LCBB                     ((XH_LCBB_Type *) XH_LCBB_BASE)
#define XH_LEBB                     ((XH_LEBB_Type *) XH_LEBB_BASE)
#define XH_PHY                      ((XH_PHY_Type *) XH_PHY_BASE)
#define XH_GPIO0                    ((XH_GPIO_Type *) XH_GPIO0_BASE)
#define XH_RTC                      ((XH_RTC_Type *) XH_RTC_BASE)
#define XH_UART0                    ((XH_UART_EX_Type *) XH_UART0_BASE) // XH98xxF
#define XH_UART1                    ((XH_UART_Type *) XH_UART1_BASE)
#define XH_UART2                    ((XH_UART_EX_Type *) XH_UART2_BASE) // XH98xxF
#define XH_AUDIO                    ((XH_AUDIO_Type *) XH_AUDIO_BASE) // XH98xx
#define XH_SPI0                     ((XH_SPI_Type *) XH_SPI0_BASE)
#define XH_SPI1                     ((XH_SPI_Type *) XH_SPI1_BASE)
#define XH_I2C                      ((XH_I2C_Type *) XH_I2C_BASE)
#define XH_TIM0                     ((XH_TIM_Type *) XH_TIM0_BASE)
#ifdef CONFIG_XH98xx
#define XH_TIM1                     ((XH_TIM_1_Type *) XH_TIM1_BASE)
#else
#define XH_TIM1                     ((XH_TIM_Type *) XH_TIM1_BASE)
#endif
#define XH_TIM2                     ((XH_TIM_Type *) XH_TIM2_BASE)
#define XH_SF                       ((XH_SF_Type *) XH_SF0_BASE)
#define XH_SF1                      ((XH_SF_Type *) XH_SF1_BASE)
#define XH_OTP                      ((XH_OTP_Type *) XH_OTP_BASE)
#define XH_RANDOM                   ((XH_RANDOM_Type *)  XH_RANDOM_BASE)
#define XH_MAC98xx                  ((XH_MAC98xx_Type *) XH_MAC98xx_BASE)
#define XH_PATCH                    ((XH_PATCH_Type *) XH_CPM_ROM_P_BASE)
#define XH_DAIF                     ((XH_DAIF_Type *) XH_DAIF_BASE)
#define XH_GPADC                    ((XH_GPADC_Type *) XH_GPADC_BASE)
#define XH_DMA_CH0                  ((XH_DMA_CH_Type *) XH_DMA_CH0_BASE)
#define XH_DMA_CH1                  ((XH_DMA_CH_Type *) XH_DMA_CH1_BASE)
#define XH_DMA_CH2                  ((XH_DMA_CH_Type *) XH_DMA_CH2_BASE)
#define XH_DMA_CH3                  ((XH_DMA_CH_Type *) XH_DMA_CH3_BASE)
#define XH_DMAC                     ((XH_DMAC_Type *) XH_DMAC_BASE)
#define XH_HCACHE                   ((XH_HCACHE_Type *) XH_HCACHE_BASE)
#define XH_IR_TX                    ((XH_IR_TX_Type *) XH_IR_TX_BASE)
#define XH_I2S_TX                   ((XH_I2S_Type *) XH_I2S_TX_BASE)
#define XH_I2S_RX                   ((XH_I2S_Type *) XH_I2S_RX_BASE)
#define XH_I2S                      XH_I2S_TX
#define XH_AUDIO                    ((XH_AUDIO_Type *) XH_AUDIO_BASE)
#define XH_PRIVACY                  (&XH_LEBB->PRIVACY)
#define XH_FILTER                   (&XH_LEBB->FILTER)
#define XH_CCM                      (&XH_LEBB->CCM)

/**
 * @addtogroup Peripheral_Registers_Bits_Definition
 * @{
 */

/******************************************************************************/
/*                         Peripheral Registers_Bits_Definition               */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*                                  BLE                                       */
/*                                                                            */
/******************************************************************************/

/// Rlist and Wlist number
#define XH_HW_RLIST_NUM             10  //Only 2 used
#define XH_HW_WLIST_NUM             10

/// ECC mode
#define XH_ECC_GEN_PUBLIC_KEY_MODE  1
#define XH_ECC_GEN_DHKEY_MODE       0

/// Privacy mode
#define PRIVACY_MODE_ACTIVE_SCANNING    1
#define PRIVACY_MODE_PASSIVE_SCANNING   2
#define PRIVACY_MODE_ADVERTISING        3
#define PRIVACY_MODE_INITIATING         4

/******************************************************************************/
/*                                                                            */
/*                General Purpose and Alternate Function I/O                  */
/*                                                                            */
/******************************************************************************/

/*******************  Bit definition for GPIO_INTTYPE register  *******************/
#define GPIO_INTTYPE_LOW_HIGH                 0
#define GPIO_INTTYPE_FALL_RISE                1

/*******************  Bit definition for GPIO_INTPOL register  *******************/
#define GPIO_INTPOL_LOW                       0
#define GPIO_INTPOL_HIGH                      1
#define GPIO_INTPOL_FALL                      0
#define GPIO_INTPOL_RISE                      1


/******************************************************************************/
/*                                                                            */
/*                               SystemTick                                   */
/*                                                                            */
/******************************************************************************/

/*****************  Bit definition for SysTick_CTRL register  *****************/
#define  SysTick_CTRL_ENABLE                 ((uint32_t)0x00000001)        /*!< Counter enable */
#define  SysTick_CTRL_TICKINT                ((uint32_t)0x00000002)        /*!< Counting down to 0 pends the SysTick handler */
#define  SysTick_CTRL_CLKSOURCE              ((uint32_t)0x00000004)        /*!< Clock source */
#define  SysTick_CTRL_COUNTFLAG              ((uint32_t)0x00010000)        /*!< Count Flag */

/*****************  Bit definition for SysTick_LOAD register  *****************/
#define  SysTick_LOAD_RELOAD                 ((uint32_t)0x00FFFFFF)        /*!< Value to load into the SysTick Current Value Register when the counter reaches 0 */

/*****************  Bit definition for SysTick_VAL register  ******************/
#define  SysTick_VAL_CURRENT                 ((uint32_t)0x00FFFFFF)        /*!< Current value at the time the register is accessed */

/*****************  Bit definition for SysTick_CALIB register  ****************/
#define  SysTick_CALIB_TENMS                 ((uint32_t)0x00FFFFFF)        /*!< Reload value to use for 10ms timing */
#define  SysTick_CALIB_SKEW                  ((uint32_t)0x40000000)        /*!< Calibration value is not exactly 10 ms */
#define  SysTick_CALIB_NOREF                 ((uint32_t)0x80000000)        /*!< The reference clock is not provided */

/******************************************************************************/
/*                                                                            */
/*                  Nested Vectored Interrupt Controller                      */
/*                                                                            */
/******************************************************************************/

/******************  Bit definition for NVIC_ISER register  *******************/
#define  NVIC_ISER_SETENA                    ((uint32_t)0xFFFFFFFF)        /*!< Interrupt set enable bits */
#define  NVIC_ISER_SETENA_0                  ((uint32_t)0x00000001)        /*!< bit 0 */
#define  NVIC_ISER_SETENA_1                  ((uint32_t)0x00000002)        /*!< bit 1 */
#define  NVIC_ISER_SETENA_2                  ((uint32_t)0x00000004)        /*!< bit 2 */
#define  NVIC_ISER_SETENA_3                  ((uint32_t)0x00000008)        /*!< bit 3 */
#define  NVIC_ISER_SETENA_4                  ((uint32_t)0x00000010)        /*!< bit 4 */
#define  NVIC_ISER_SETENA_5                  ((uint32_t)0x00000020)        /*!< bit 5 */
#define  NVIC_ISER_SETENA_6                  ((uint32_t)0x00000040)        /*!< bit 6 */
#define  NVIC_ISER_SETENA_7                  ((uint32_t)0x00000080)        /*!< bit 7 */
#define  NVIC_ISER_SETENA_8                  ((uint32_t)0x00000100)        /*!< bit 8 */
#define  NVIC_ISER_SETENA_9                  ((uint32_t)0x00000200)        /*!< bit 9 */
#define  NVIC_ISER_SETENA_10                 ((uint32_t)0x00000400)        /*!< bit 10 */
#define  NVIC_ISER_SETENA_11                 ((uint32_t)0x00000800)        /*!< bit 11 */
#define  NVIC_ISER_SETENA_12                 ((uint32_t)0x00001000)        /*!< bit 12 */
#define  NVIC_ISER_SETENA_13                 ((uint32_t)0x00002000)        /*!< bit 13 */
#define  NVIC_ISER_SETENA_14                 ((uint32_t)0x00004000)        /*!< bit 14 */
#define  NVIC_ISER_SETENA_15                 ((uint32_t)0x00008000)        /*!< bit 15 */
#define  NVIC_ISER_SETENA_16                 ((uint32_t)0x00010000)        /*!< bit 16 */
#define  NVIC_ISER_SETENA_17                 ((uint32_t)0x00020000)        /*!< bit 17 */
#define  NVIC_ISER_SETENA_18                 ((uint32_t)0x00040000)        /*!< bit 18 */
#define  NVIC_ISER_SETENA_19                 ((uint32_t)0x00080000)        /*!< bit 19 */
#define  NVIC_ISER_SETENA_20                 ((uint32_t)0x00100000)        /*!< bit 20 */
#define  NVIC_ISER_SETENA_21                 ((uint32_t)0x00200000)        /*!< bit 21 */
#define  NVIC_ISER_SETENA_22                 ((uint32_t)0x00400000)        /*!< bit 22 */
#define  NVIC_ISER_SETENA_23                 ((uint32_t)0x00800000)        /*!< bit 23 */
#define  NVIC_ISER_SETENA_24                 ((uint32_t)0x01000000)        /*!< bit 24 */
#define  NVIC_ISER_SETENA_25                 ((uint32_t)0x02000000)        /*!< bit 25 */
#define  NVIC_ISER_SETENA_26                 ((uint32_t)0x04000000)        /*!< bit 26 */
#define  NVIC_ISER_SETENA_27                 ((uint32_t)0x08000000)        /*!< bit 27 */
#define  NVIC_ISER_SETENA_28                 ((uint32_t)0x10000000)        /*!< bit 28 */
#define  NVIC_ISER_SETENA_29                 ((uint32_t)0x20000000)        /*!< bit 29 */
#define  NVIC_ISER_SETENA_30                 ((uint32_t)0x40000000)        /*!< bit 30 */
#define  NVIC_ISER_SETENA_31                 ((uint32_t)0x80000000)        /*!< bit 31 */

/******************  Bit definition for NVIC_ICER register  *******************/
#define  NVIC_ICER_CLRENA                   ((uint32_t)0xFFFFFFFF)        /*!< Interrupt clear-enable bits */
#define  NVIC_ICER_CLRENA_0                  ((uint32_t)0x00000001)        /*!< bit 0 */
#define  NVIC_ICER_CLRENA_1                  ((uint32_t)0x00000002)        /*!< bit 1 */
#define  NVIC_ICER_CLRENA_2                  ((uint32_t)0x00000004)        /*!< bit 2 */
#define  NVIC_ICER_CLRENA_3                  ((uint32_t)0x00000008)        /*!< bit 3 */
#define  NVIC_ICER_CLRENA_4                  ((uint32_t)0x00000010)        /*!< bit 4 */
#define  NVIC_ICER_CLRENA_5                  ((uint32_t)0x00000020)        /*!< bit 5 */
#define  NVIC_ICER_CLRENA_6                  ((uint32_t)0x00000040)        /*!< bit 6 */
#define  NVIC_ICER_CLRENA_7                  ((uint32_t)0x00000080)        /*!< bit 7 */
#define  NVIC_ICER_CLRENA_8                  ((uint32_t)0x00000100)        /*!< bit 8 */
#define  NVIC_ICER_CLRENA_9                  ((uint32_t)0x00000200)        /*!< bit 9 */
#define  NVIC_ICER_CLRENA_10                 ((uint32_t)0x00000400)        /*!< bit 10 */
#define  NVIC_ICER_CLRENA_11                 ((uint32_t)0x00000800)        /*!< bit 11 */
#define  NVIC_ICER_CLRENA_12                 ((uint32_t)0x00001000)        /*!< bit 12 */
#define  NVIC_ICER_CLRENA_13                 ((uint32_t)0x00002000)        /*!< bit 13 */
#define  NVIC_ICER_CLRENA_14                 ((uint32_t)0x00004000)        /*!< bit 14 */
#define  NVIC_ICER_CLRENA_15                 ((uint32_t)0x00008000)        /*!< bit 15 */
#define  NVIC_ICER_CLRENA_16                 ((uint32_t)0x00010000)        /*!< bit 16 */
#define  NVIC_ICER_CLRENA_17                 ((uint32_t)0x00020000)        /*!< bit 17 */
#define  NVIC_ICER_CLRENA_18                 ((uint32_t)0x00040000)        /*!< bit 18 */
#define  NVIC_ICER_CLRENA_19                 ((uint32_t)0x00080000)        /*!< bit 19 */
#define  NVIC_ICER_CLRENA_20                 ((uint32_t)0x00100000)        /*!< bit 20 */
#define  NVIC_ICER_CLRENA_21                 ((uint32_t)0x00200000)        /*!< bit 21 */
#define  NVIC_ICER_CLRENA_22                 ((uint32_t)0x00400000)        /*!< bit 22 */
#define  NVIC_ICER_CLRENA_23                 ((uint32_t)0x00800000)        /*!< bit 23 */
#define  NVIC_ICER_CLRENA_24                 ((uint32_t)0x01000000)        /*!< bit 24 */
#define  NVIC_ICER_CLRENA_25                 ((uint32_t)0x02000000)        /*!< bit 25 */
#define  NVIC_ICER_CLRENA_26                 ((uint32_t)0x04000000)        /*!< bit 26 */
#define  NVIC_ICER_CLRENA_27                 ((uint32_t)0x08000000)        /*!< bit 27 */
#define  NVIC_ICER_CLRENA_28                 ((uint32_t)0x10000000)        /*!< bit 28 */
#define  NVIC_ICER_CLRENA_29                 ((uint32_t)0x20000000)        /*!< bit 29 */
#define  NVIC_ICER_CLRENA_30                 ((uint32_t)0x40000000)        /*!< bit 30 */
#define  NVIC_ICER_CLRENA_31                 ((uint32_t)0x80000000)        /*!< bit 31 */

/******************  Bit definition for NVIC_ISPR register  *******************/
#define  NVIC_ISPR_SETPEND                   ((uint32_t)0xFFFFFFFF)        /*!< Interrupt set-pending bits */
#define  NVIC_ISPR_SETPEND_0                 ((uint32_t)0x00000001)        /*!< bit 0 */
#define  NVIC_ISPR_SETPEND_1                 ((uint32_t)0x00000002)        /*!< bit 1 */
#define  NVIC_ISPR_SETPEND_2                 ((uint32_t)0x00000004)        /*!< bit 2 */
#define  NVIC_ISPR_SETPEND_3                 ((uint32_t)0x00000008)        /*!< bit 3 */
#define  NVIC_ISPR_SETPEND_4                 ((uint32_t)0x00000010)        /*!< bit 4 */
#define  NVIC_ISPR_SETPEND_5                 ((uint32_t)0x00000020)        /*!< bit 5 */
#define  NVIC_ISPR_SETPEND_6                 ((uint32_t)0x00000040)        /*!< bit 6 */
#define  NVIC_ISPR_SETPEND_7                 ((uint32_t)0x00000080)        /*!< bit 7 */
#define  NVIC_ISPR_SETPEND_8                 ((uint32_t)0x00000100)        /*!< bit 8 */
#define  NVIC_ISPR_SETPEND_9                 ((uint32_t)0x00000200)        /*!< bit 9 */
#define  NVIC_ISPR_SETPEND_10                ((uint32_t)0x00000400)        /*!< bit 10 */
#define  NVIC_ISPR_SETPEND_11                ((uint32_t)0x00000800)        /*!< bit 11 */
#define  NVIC_ISPR_SETPEND_12                ((uint32_t)0x00001000)        /*!< bit 12 */
#define  NVIC_ISPR_SETPEND_13                ((uint32_t)0x00002000)        /*!< bit 13 */
#define  NVIC_ISPR_SETPEND_14                ((uint32_t)0x00004000)        /*!< bit 14 */
#define  NVIC_ISPR_SETPEND_15                ((uint32_t)0x00008000)        /*!< bit 15 */
#define  NVIC_ISPR_SETPEND_16                ((uint32_t)0x00010000)        /*!< bit 16 */
#define  NVIC_ISPR_SETPEND_17                ((uint32_t)0x00020000)        /*!< bit 17 */
#define  NVIC_ISPR_SETPEND_18                ((uint32_t)0x00040000)        /*!< bit 18 */
#define  NVIC_ISPR_SETPEND_19                ((uint32_t)0x00080000)        /*!< bit 19 */
#define  NVIC_ISPR_SETPEND_20                ((uint32_t)0x00100000)        /*!< bit 20 */
#define  NVIC_ISPR_SETPEND_21                ((uint32_t)0x00200000)        /*!< bit 21 */
#define  NVIC_ISPR_SETPEND_22                ((uint32_t)0x00400000)        /*!< bit 22 */
#define  NVIC_ISPR_SETPEND_23                ((uint32_t)0x00800000)        /*!< bit 23 */
#define  NVIC_ISPR_SETPEND_24                ((uint32_t)0x01000000)        /*!< bit 24 */
#define  NVIC_ISPR_SETPEND_25                ((uint32_t)0x02000000)        /*!< bit 25 */
#define  NVIC_ISPR_SETPEND_26                ((uint32_t)0x04000000)        /*!< bit 26 */
#define  NVIC_ISPR_SETPEND_27                ((uint32_t)0x08000000)        /*!< bit 27 */
#define  NVIC_ISPR_SETPEND_28                ((uint32_t)0x10000000)        /*!< bit 28 */
#define  NVIC_ISPR_SETPEND_29                ((uint32_t)0x20000000)        /*!< bit 29 */
#define  NVIC_ISPR_SETPEND_30                ((uint32_t)0x40000000)        /*!< bit 30 */
#define  NVIC_ISPR_SETPEND_31                ((uint32_t)0x80000000)        /*!< bit 31 */

/******************  Bit definition for NVIC_ICPR register  *******************/
#define  NVIC_ICPR_CLRPEND                   ((uint32_t)0xFFFFFFFF)        /*!< Interrupt clear-pending bits */
#define  NVIC_ICPR_CLRPEND_0                 ((uint32_t)0x00000001)        /*!< bit 0 */
#define  NVIC_ICPR_CLRPEND_1                 ((uint32_t)0x00000002)        /*!< bit 1 */
#define  NVIC_ICPR_CLRPEND_2                 ((uint32_t)0x00000004)        /*!< bit 2 */
#define  NVIC_ICPR_CLRPEND_3                 ((uint32_t)0x00000008)        /*!< bit 3 */
#define  NVIC_ICPR_CLRPEND_4                 ((uint32_t)0x00000010)        /*!< bit 4 */
#define  NVIC_ICPR_CLRPEND_5                 ((uint32_t)0x00000020)        /*!< bit 5 */
#define  NVIC_ICPR_CLRPEND_6                 ((uint32_t)0x00000040)        /*!< bit 6 */
#define  NVIC_ICPR_CLRPEND_7                 ((uint32_t)0x00000080)        /*!< bit 7 */
#define  NVIC_ICPR_CLRPEND_8                 ((uint32_t)0x00000100)        /*!< bit 8 */
#define  NVIC_ICPR_CLRPEND_9                 ((uint32_t)0x00000200)        /*!< bit 9 */
#define  NVIC_ICPR_CLRPEND_10                ((uint32_t)0x00000400)        /*!< bit 10 */
#define  NVIC_ICPR_CLRPEND_11                ((uint32_t)0x00000800)        /*!< bit 11 */
#define  NVIC_ICPR_CLRPEND_12                ((uint32_t)0x00001000)        /*!< bit 12 */
#define  NVIC_ICPR_CLRPEND_13                ((uint32_t)0x00002000)        /*!< bit 13 */
#define  NVIC_ICPR_CLRPEND_14                ((uint32_t)0x00004000)        /*!< bit 14 */
#define  NVIC_ICPR_CLRPEND_15                ((uint32_t)0x00008000)        /*!< bit 15 */
#define  NVIC_ICPR_CLRPEND_16                ((uint32_t)0x00010000)        /*!< bit 16 */
#define  NVIC_ICPR_CLRPEND_17                ((uint32_t)0x00020000)        /*!< bit 17 */
#define  NVIC_ICPR_CLRPEND_18                ((uint32_t)0x00040000)        /*!< bit 18 */
#define  NVIC_ICPR_CLRPEND_19                ((uint32_t)0x00080000)        /*!< bit 19 */
#define  NVIC_ICPR_CLRPEND_20                ((uint32_t)0x00100000)        /*!< bit 20 */
#define  NVIC_ICPR_CLRPEND_21                ((uint32_t)0x00200000)        /*!< bit 21 */
#define  NVIC_ICPR_CLRPEND_22                ((uint32_t)0x00400000)        /*!< bit 22 */
#define  NVIC_ICPR_CLRPEND_23                ((uint32_t)0x00800000)        /*!< bit 23 */
#define  NVIC_ICPR_CLRPEND_24                ((uint32_t)0x01000000)        /*!< bit 24 */
#define  NVIC_ICPR_CLRPEND_25                ((uint32_t)0x02000000)        /*!< bit 25 */
#define  NVIC_ICPR_CLRPEND_26                ((uint32_t)0x04000000)        /*!< bit 26 */
#define  NVIC_ICPR_CLRPEND_27                ((uint32_t)0x08000000)        /*!< bit 27 */
#define  NVIC_ICPR_CLRPEND_28                ((uint32_t)0x10000000)        /*!< bit 28 */
#define  NVIC_ICPR_CLRPEND_29                ((uint32_t)0x20000000)        /*!< bit 29 */
#define  NVIC_ICPR_CLRPEND_30                ((uint32_t)0x40000000)        /*!< bit 30 */
#define  NVIC_ICPR_CLRPEND_31                ((uint32_t)0x80000000)        /*!< bit 31 */

/******************  Bit definition for NVIC_IABR register  *******************/
#define  NVIC_IABR_ACTIVE                    ((uint32_t)0xFFFFFFFF)        /*!< Interrupt active flags */
#define  NVIC_IABR_ACTIVE_0                  ((uint32_t)0x00000001)        /*!< bit 0 */
#define  NVIC_IABR_ACTIVE_1                  ((uint32_t)0x00000002)        /*!< bit 1 */
#define  NVIC_IABR_ACTIVE_2                  ((uint32_t)0x00000004)        /*!< bit 2 */
#define  NVIC_IABR_ACTIVE_3                  ((uint32_t)0x00000008)        /*!< bit 3 */
#define  NVIC_IABR_ACTIVE_4                  ((uint32_t)0x00000010)        /*!< bit 4 */
#define  NVIC_IABR_ACTIVE_5                  ((uint32_t)0x00000020)        /*!< bit 5 */
#define  NVIC_IABR_ACTIVE_6                  ((uint32_t)0x00000040)        /*!< bit 6 */
#define  NVIC_IABR_ACTIVE_7                  ((uint32_t)0x00000080)        /*!< bit 7 */
#define  NVIC_IABR_ACTIVE_8                  ((uint32_t)0x00000100)        /*!< bit 8 */
#define  NVIC_IABR_ACTIVE_9                  ((uint32_t)0x00000200)        /*!< bit 9 */
#define  NVIC_IABR_ACTIVE_10                 ((uint32_t)0x00000400)        /*!< bit 10 */
#define  NVIC_IABR_ACTIVE_11                 ((uint32_t)0x00000800)        /*!< bit 11 */
#define  NVIC_IABR_ACTIVE_12                 ((uint32_t)0x00001000)        /*!< bit 12 */
#define  NVIC_IABR_ACTIVE_13                 ((uint32_t)0x00002000)        /*!< bit 13 */
#define  NVIC_IABR_ACTIVE_14                 ((uint32_t)0x00004000)        /*!< bit 14 */
#define  NVIC_IABR_ACTIVE_15                 ((uint32_t)0x00008000)        /*!< bit 15 */
#define  NVIC_IABR_ACTIVE_16                 ((uint32_t)0x00010000)        /*!< bit 16 */
#define  NVIC_IABR_ACTIVE_17                 ((uint32_t)0x00020000)        /*!< bit 17 */
#define  NVIC_IABR_ACTIVE_18                 ((uint32_t)0x00040000)        /*!< bit 18 */
#define  NVIC_IABR_ACTIVE_19                 ((uint32_t)0x00080000)        /*!< bit 19 */
#define  NVIC_IABR_ACTIVE_20                 ((uint32_t)0x00100000)        /*!< bit 20 */
#define  NVIC_IABR_ACTIVE_21                 ((uint32_t)0x00200000)        /*!< bit 21 */
#define  NVIC_IABR_ACTIVE_22                 ((uint32_t)0x00400000)        /*!< bit 22 */
#define  NVIC_IABR_ACTIVE_23                 ((uint32_t)0x00800000)        /*!< bit 23 */
#define  NVIC_IABR_ACTIVE_24                 ((uint32_t)0x01000000)        /*!< bit 24 */
#define  NVIC_IABR_ACTIVE_25                 ((uint32_t)0x02000000)        /*!< bit 25 */
#define  NVIC_IABR_ACTIVE_26                 ((uint32_t)0x04000000)        /*!< bit 26 */
#define  NVIC_IABR_ACTIVE_27                 ((uint32_t)0x08000000)        /*!< bit 27 */
#define  NVIC_IABR_ACTIVE_28                 ((uint32_t)0x10000000)        /*!< bit 28 */
#define  NVIC_IABR_ACTIVE_29                 ((uint32_t)0x20000000)        /*!< bit 29 */
#define  NVIC_IABR_ACTIVE_30                 ((uint32_t)0x40000000)        /*!< bit 30 */
#define  NVIC_IABR_ACTIVE_31                 ((uint32_t)0x80000000)        /*!< bit 31 */

/******************  Bit definition for NVIC_PRI0 register  *******************/
#define  NVIC_IPR0_PRI_0                     ((uint32_t)0x000000FF)        /*!< Priority of interrupt 0 */
#define  NVIC_IPR0_PRI_1                     ((uint32_t)0x0000FF00)        /*!< Priority of interrupt 1 */
#define  NVIC_IPR0_PRI_2                     ((uint32_t)0x00FF0000)        /*!< Priority of interrupt 2 */
#define  NVIC_IPR0_PRI_3                     ((uint32_t)0xFF000000)        /*!< Priority of interrupt 3 */

/******************  Bit definition for NVIC_PRI1 register  *******************/
#define  NVIC_IPR1_PRI_4                     ((uint32_t)0x000000FF)        /*!< Priority of interrupt 4 */
#define  NVIC_IPR1_PRI_5                     ((uint32_t)0x0000FF00)        /*!< Priority of interrupt 5 */
#define  NVIC_IPR1_PRI_6                     ((uint32_t)0x00FF0000)        /*!< Priority of interrupt 6 */
#define  NVIC_IPR1_PRI_7                     ((uint32_t)0xFF000000)        /*!< Priority of interrupt 7 */

/******************  Bit definition for NVIC_PRI2 register  *******************/
#define  NVIC_IPR2_PRI_8                     ((uint32_t)0x000000FF)        /*!< Priority of interrupt 8 */
#define  NVIC_IPR2_PRI_9                     ((uint32_t)0x0000FF00)        /*!< Priority of interrupt 9 */
#define  NVIC_IPR2_PRI_10                    ((uint32_t)0x00FF0000)        /*!< Priority of interrupt 10 */
#define  NVIC_IPR2_PRI_11                    ((uint32_t)0xFF000000)        /*!< Priority of interrupt 11 */

/******************  Bit definition for NVIC_PRI3 register  *******************/
#define  NVIC_IPR3_PRI_12                    ((uint32_t)0x000000FF)        /*!< Priority of interrupt 12 */
#define  NVIC_IPR3_PRI_13                    ((uint32_t)0x0000FF00)        /*!< Priority of interrupt 13 */
#define  NVIC_IPR3_PRI_14                    ((uint32_t)0x00FF0000)        /*!< Priority of interrupt 14 */
#define  NVIC_IPR3_PRI_15                    ((uint32_t)0xFF000000)        /*!< Priority of interrupt 15 */

/******************  Bit definition for NVIC_PRI4 register  *******************/
#define  NVIC_IPR4_PRI_16                    ((uint32_t)0x000000FF)        /*!< Priority of interrupt 16 */
#define  NVIC_IPR4_PRI_17                    ((uint32_t)0x0000FF00)        /*!< Priority of interrupt 17 */
#define  NVIC_IPR4_PRI_18                    ((uint32_t)0x00FF0000)        /*!< Priority of interrupt 18 */
#define  NVIC_IPR4_PRI_19                    ((uint32_t)0xFF000000)        /*!< Priority of interrupt 19 */

/******************  Bit definition for NVIC_PRI5 register  *******************/
#define  NVIC_IPR5_PRI_20                    ((uint32_t)0x000000FF)        /*!< Priority of interrupt 20 */
#define  NVIC_IPR5_PRI_21                    ((uint32_t)0x0000FF00)        /*!< Priority of interrupt 21 */
#define  NVIC_IPR5_PRI_22                    ((uint32_t)0x00FF0000)        /*!< Priority of interrupt 22 */
#define  NVIC_IPR5_PRI_23                    ((uint32_t)0xFF000000)        /*!< Priority of interrupt 23 */

/******************  Bit definition for NVIC_PRI6 register  *******************/
#define  NVIC_IPR6_PRI_24                    ((uint32_t)0x000000FF)        /*!< Priority of interrupt 24 */
#define  NVIC_IPR6_PRI_25                    ((uint32_t)0x0000FF00)        /*!< Priority of interrupt 25 */
#define  NVIC_IPR6_PRI_26                    ((uint32_t)0x00FF0000)        /*!< Priority of interrupt 26 */
#define  NVIC_IPR6_PRI_27                    ((uint32_t)0xFF000000)        /*!< Priority of interrupt 27 */

/******************  Bit definition for NVIC_PRI7 register  *******************/
#define  NVIC_IPR7_PRI_28                    ((uint32_t)0x000000FF)        /*!< Priority of interrupt 28 */
#define  NVIC_IPR7_PRI_29                    ((uint32_t)0x0000FF00)        /*!< Priority of interrupt 29 */
#define  NVIC_IPR7_PRI_30                    ((uint32_t)0x00FF0000)        /*!< Priority of interrupt 30 */
#define  NVIC_IPR7_PRI_31                    ((uint32_t)0xFF000000)        /*!< Priority of interrupt 31 */

/******************  Bit definition for SCB_CPUID register  *******************/
#define  SCB_CPUID_REVISION                  ((uint32_t)0x0000000F)        /*!< Implementation defined revision number */
#define  SCB_CPUID_PARTNO                    ((uint32_t)0x0000FFF0)        /*!< Number of processor within family */
#define  SCB_CPUID_Constant                  ((uint32_t)0x000F0000)        /*!< Reads as 0x0F */
#define  SCB_CPUID_VARIANT                   ((uint32_t)0x00F00000)        /*!< Implementation defined variant number */
#define  SCB_CPUID_IMPLEMENTER               ((uint32_t)0xFF000000)        /*!< Implementer code. ARM is 0x41 */

/*******************  Bit definition for SCB_ICSR register  *******************/
#define  SCB_ICSR_VECTACTIVE                 ((uint32_t)0x000001FF)        /*!< Active ISR number field */
#define  SCB_ICSR_RETTOBASE                  ((uint32_t)0x00000800)        /*!< All active exceptions minus the IPSR_current_exception yields the empty set */
#define  SCB_ICSR_VECTPENDING                ((uint32_t)0x003FF000)        /*!< Pending ISR number field */
#define  SCB_ICSR_ISRPENDING                 ((uint32_t)0x00400000)        /*!< Interrupt pending flag */
#define  SCB_ICSR_ISRPREEMPT                 ((uint32_t)0x00800000)        /*!< It indicates that a pending interrupt becomes active in the next running cycle */
#define  SCB_ICSR_PENDSTCLR                  ((uint32_t)0x02000000)        /*!< Clear pending SysTick bit */
#define  SCB_ICSR_PENDSTSET                  ((uint32_t)0x04000000)        /*!< Set pending SysTick bit */
#define  SCB_ICSR_PENDSVCLR                  ((uint32_t)0x08000000)        /*!< Clear pending pendSV bit */
#define  SCB_ICSR_PENDSVSET                  ((uint32_t)0x10000000)        /*!< Set pending pendSV bit */
#define  SCB_ICSR_NMIPENDSET                 ((uint32_t)0x80000000)        /*!< Set pending NMI bit */

/*******************  Bit definition for SCB_VTOR register  *******************/
#define  SCB_VTOR_TBLOFF                     ((uint32_t)0x1FFFFF80)        /*!< Vector table base offset field */
#define  SCB_VTOR_TBLBASE                    ((uint32_t)0x20000000)        /*!< Table base in code(0) or RAM(1) */

/*!<*****************  Bit definition for SCB_AIRCR register  *******************/
#define  SCB_AIRCR_VECTRESET                 ((uint32_t)0x00000001)        /*!< System Reset bit */
#define  SCB_AIRCR_VECTCLRACTIVE             ((uint32_t)0x00000002)        /*!< Clear active vector bit */
#define  SCB_AIRCR_SYSRESETREQ               ((uint32_t)0x00000004)        /*!< Requests chip control logic to generate a reset */

#define  SCB_AIRCR_PRIGROUP                  ((uint32_t)0x00000700)        /*!< PRIGROUP[2:0] bits (Priority group) */
#define  SCB_AIRCR_PRIGROUP_0                ((uint32_t)0x00000100)        /*!< Bit 0 */
#define  SCB_AIRCR_PRIGROUP_1                ((uint32_t)0x00000200)        /*!< Bit 1 */
#define  SCB_AIRCR_PRIGROUP_2                ((uint32_t)0x00000400)        /*!< Bit 2  */

/* prority group configuration */
#define  SCB_AIRCR_PRIGROUP0                 ((uint32_t)0x00000000)        /*!< Priority group=0 (7 bits of pre-emption priority, 1 bit of subpriority) */
#define  SCB_AIRCR_PRIGROUP1                 ((uint32_t)0x00000100)        /*!< Priority group=1 (6 bits of pre-emption priority, 2 bits of subpriority) */
#define  SCB_AIRCR_PRIGROUP2                 ((uint32_t)0x00000200)        /*!< Priority group=2 (5 bits of pre-emption priority, 3 bits of subpriority) */
#define  SCB_AIRCR_PRIGROUP3                 ((uint32_t)0x00000300)        /*!< Priority group=3 (4 bits of pre-emption priority, 4 bits of subpriority) */
#define  SCB_AIRCR_PRIGROUP4                 ((uint32_t)0x00000400)        /*!< Priority group=4 (3 bits of pre-emption priority, 5 bits of subpriority) */
#define  SCB_AIRCR_PRIGROUP5                 ((uint32_t)0x00000500)        /*!< Priority group=5 (2 bits of pre-emption priority, 6 bits of subpriority) */
#define  SCB_AIRCR_PRIGROUP6                 ((uint32_t)0x00000600)        /*!< Priority group=6 (1 bit of pre-emption priority, 7 bits of subpriority) */
#define  SCB_AIRCR_PRIGROUP7                 ((uint32_t)0x00000700)        /*!< Priority group=7 (no pre-emption priority, 8 bits of subpriority) */

#define  SCB_AIRCR_ENDIANESS                 ((uint32_t)0x00008000)        /*!< Data endianness bit */
#define  SCB_AIRCR_VECTKEY                   ((uint32_t)0xFFFF0000)        /*!< Register key (VECTKEY) - Reads as 0xFA05 (VECTKEYSTAT) */

/*******************  Bit definition for SCB_SCR register  ********************/
#define  SCB_SCR_SLEEPONEXIT                 ((uint8_t)0x02)               /*!< Sleep on exit bit */
#define  SCB_SCR_SLEEPDEEP                   ((uint8_t)0x04)               /*!< Sleep deep bit */
#define  SCB_SCR_SEVONPEND                   ((uint8_t)0x10)               /*!< Wake up from WFE */

/********************  Bit definition for SCB_CCR register  *******************/
#define  SCB_CCR_NONBASETHRDENA              ((uint16_t)0x0001)            /*!< Thread mode can be entered from any level in Handler mode by controlled return value */
#define  SCB_CCR_USERSETMPEND                ((uint16_t)0x0002)            /*!< Enables user code to write the Software Trigger Interrupt register to trigger (pend) a Main exception */
#define  SCB_CCR_UNALIGN_TRP                 ((uint16_t)0x0008)            /*!< Trap for unaligned access */
#define  SCB_CCR_DIV_0_TRP                   ((uint16_t)0x0010)            /*!< Trap on Divide by 0 */
#define  SCB_CCR_BFHFNMIGN                   ((uint16_t)0x0100)            /*!< Handlers running at priority -1 and -2 */
#define  SCB_CCR_STKALIGN                    ((uint16_t)0x0200)            /*!< On exception entry, the SP used prior to the exception is adjusted to be 8-byte aligned */

/*******************  Bit definition for SCB_SHPR register ********************/
#define  SCB_SHPR_PRI_N                      ((uint32_t)0x000000FF)        /*!< Priority of system handler 4,8, and 12. Mem Manage, reserved and Debug Monitor */
#define  SCB_SHPR_PRI_N1                     ((uint32_t)0x0000FF00)        /*!< Priority of system handler 5,9, and 13. Bus Fault, reserved and reserved */
#define  SCB_SHPR_PRI_N2                     ((uint32_t)0x00FF0000)        /*!< Priority of system handler 6,10, and 14. Usage Fault, reserved and PendSV */
#define  SCB_SHPR_PRI_N3                     ((uint32_t)0xFF000000)        /*!< Priority of system handler 7,11, and 15. Reserved, SVCall and SysTick */

/******************  Bit definition for SCB_SHCSR register  *******************/
#define  SCB_SHCSR_MEMFAULTACT               ((uint32_t)0x00000001)        /*!< MemManage is active */
#define  SCB_SHCSR_BUSFAULTACT               ((uint32_t)0x00000002)        /*!< BusFault is active */
#define  SCB_SHCSR_USGFAULTACT               ((uint32_t)0x00000008)        /*!< UsageFault is active */
#define  SCB_SHCSR_SVCALLACT                 ((uint32_t)0x00000080)        /*!< SVCall is active */
#define  SCB_SHCSR_MONITORACT                ((uint32_t)0x00000100)        /*!< Monitor is active */
#define  SCB_SHCSR_PENDSVACT                 ((uint32_t)0x00000400)        /*!< PendSV is active */
#define  SCB_SHCSR_SYSTICKACT                ((uint32_t)0x00000800)        /*!< SysTick is active */
#define  SCB_SHCSR_USGFAULTPENDED            ((uint32_t)0x00001000)        /*!< Usage Fault is pended */
#define  SCB_SHCSR_MEMFAULTPENDED            ((uint32_t)0x00002000)        /*!< MemManage is pended */
#define  SCB_SHCSR_BUSFAULTPENDED            ((uint32_t)0x00004000)        /*!< Bus Fault is pended */
#define  SCB_SHCSR_SVCALLPENDED              ((uint32_t)0x00008000)        /*!< SVCall is pended */
#define  SCB_SHCSR_MEMFAULTENA               ((uint32_t)0x00010000)        /*!< MemManage enable */
#define  SCB_SHCSR_BUSFAULTENA               ((uint32_t)0x00020000)        /*!< Bus Fault enable */
#define  SCB_SHCSR_USGFAULTENA               ((uint32_t)0x00040000)        /*!< UsageFault enable */

/*******************  Bit definition for SCB_CFSR register  *******************/
/*!< MFSR */
#define  SCB_CFSR_IACCVIOL                   ((uint32_t)0x00000001)        /*!< Instruction access violation */
#define  SCB_CFSR_DACCVIOL                   ((uint32_t)0x00000002)        /*!< Data access violation */
#define  SCB_CFSR_MUNSTKERR                  ((uint32_t)0x00000008)        /*!< Unstacking error */
#define  SCB_CFSR_MSTKERR                    ((uint32_t)0x00000010)        /*!< Stacking error */
#define  SCB_CFSR_MMARVALID                  ((uint32_t)0x00000080)        /*!< Memory Manage Address Register address valid flag */
/*!< BFSR */
#define  SCB_CFSR_IBUSERR                    ((uint32_t)0x00000100)        /*!< Instruction bus error flag */
#define  SCB_CFSR_PRECISERR                  ((uint32_t)0x00000200)        /*!< Precise data bus error */
#define  SCB_CFSR_IMPRECISERR                ((uint32_t)0x00000400)        /*!< Imprecise data bus error */
#define  SCB_CFSR_UNSTKERR                   ((uint32_t)0x00000800)        /*!< Unstacking error */
#define  SCB_CFSR_STKERR                     ((uint32_t)0x00001000)        /*!< Stacking error */
#define  SCB_CFSR_BFARVALID                  ((uint32_t)0x00008000)        /*!< Bus Fault Address Register address valid flag */
/*!< UFSR */
#define  SCB_CFSR_UNDEFINSTR                 ((uint32_t)0x00010000)        /*!< The processor attempt to execute an undefined instruction */
#define  SCB_CFSR_INVSTATE                   ((uint32_t)0x00020000)        /*!< Invalid combination of EPSR and instruction */
#define  SCB_CFSR_INVPC                      ((uint32_t)0x00040000)        /*!< Attempt to load EXC_RETURN into pc illegally */
#define  SCB_CFSR_NOCP                       ((uint32_t)0x00080000)        /*!< Attempt to use a coprocessor instruction */
#define  SCB_CFSR_UNALIGNED                  ((uint32_t)0x01000000)        /*!< Fault occurs when there is an attempt to make an unaligned memory access */
#define  SCB_CFSR_DIVBYZERO                  ((uint32_t)0x02000000)        /*!< Fault occurs when SDIV or DIV instruction is used with a divisor of 0 */

/*******************  Bit definition for SCB_HFSR register  *******************/
#define  SCB_HFSR_VECTTBL                    ((uint32_t)0x00000002)        /*!< Fault occurs because of vector table read on exception processing */
#define  SCB_HFSR_FORCED                     ((uint32_t)0x40000000)        /*!< Hard Fault activated when a configurable Fault was received and cannot activate */
#define  SCB_HFSR_DEBUGEVT                   ((uint32_t)0x80000000)        /*!< Fault related to debug */

/*******************  Bit definition for SCB_DFSR register  *******************/
#define  SCB_DFSR_HALTED                     ((uint8_t)0x01)               /*!< Halt request flag */
#define  SCB_DFSR_BKPT                       ((uint8_t)0x02)               /*!< BKPT flag */
#define  SCB_DFSR_DWTTRAP                    ((uint8_t)0x04)               /*!< Data Watchpoint and Trace (DWT) flag */
#define  SCB_DFSR_VCATCH                     ((uint8_t)0x08)               /*!< Vector catch flag */
#define  SCB_DFSR_EXTERNAL                   ((uint8_t)0x10)               /*!< External debug request flag */

/*******************  Bit definition for SCB_MMFAR register  ******************/
#define  SCB_MMFAR_ADDRESS                   ((uint32_t)0xFFFFFFFF)        /*!< Mem Manage fault address field */

/*******************  Bit definition for SCB_BFAR register  *******************/
#define  SCB_BFAR_ADDRESS                    ((uint32_t)0xFFFFFFFF)        /*!< Bus fault address field */

/*******************  Bit definition for SCB_afsr register  *******************/
#define  SCB_AFSR_IMPDEF                     ((uint32_t)0xFFFFFFFF)        /*!< Implementation defined */

/******************************************************************************/
/*                                                                            */
/*                             DMA Controller                                 */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/*                     DMA Controller                                                   */
/*                                                                            */
/******************************************************************************/

/* Bitfields in ctrl @XH_DMA_CH_CTRL_REG_Type */
#define DWC_CH_CTRL_EN_POS                   0
#define DWC_CH_CTRL_INT_TC_POS               1
#define DWC_CH_CTRL_INT_ERR_POS              2
#define DWC_CH_CTRL_INT_ABT_POS              3
#define DWC_CH_CTRL_DST_REQ_SEL_POS          4
#define DWC_CH_CTRL_SRC_REQ_SEL_POS          8
#define DWC_CH_CTRL_DST_ADDR_CTRL_POS        12
#define DWC_CH_CTRL_SRC_ADDR_CTRL_POS        14
#define DWC_CH_CTRL_DST_MODE_POS             16
#define DWC_CH_CTRL_SRC_MODE_POS             17
#define DWC_CH_CTRL_DST_WIDTH_POS            18
#define DWC_CH_CTRL_SRC_WIDTH_POS            20
#define DWC_CH_CTRL_SRC_BURST_SIZE_POS       22
#define DWC_CH_CTRL_PRIORITY_POS             29
#define DWC_CH_CTRL_EN_MASK                  0x00000001
#define DWC_CH_CTRL_INT_TC_MASK              0x00000002
#define DWC_CH_CTRL_INT_ERR_MASK             0x00000004
#define DWC_CH_CTRL_INT_ABT_MASK             0x00000008
#define DWC_CH_CTRL_DST_REQ_SEL_MASK         0x000000F0
#define DWC_CH_CTRL_SRC_REQ_SEL_MASK         0x00000F00
#define DWC_CH_CTRL_DST_ADDR_CTRL_MASK       0x00003000
#define DWC_CH_CTRL_SRC_ADDR_CTRL_MASK       0x0000C000
#define DWC_CH_CTRL_DST_MODE_MASK            0x00010000
#define DWC_CH_CTRL_SRC_MODE_MASK            0x00020000
#define DWC_CH_CTRL_DST_WIDTH_MASK           0x000C0000
#define DWC_CH_CTRL_SRC_WIDTH_MASK           0x00300000
#define DWC_CH_CTRL_SRC_BURST_SIZE_MASK      0x01C00000
#define DWC_CH_CTRL_PRIORITY_MASK            0x20000000

#define DWC_CTLL_DST_INC        0
#define DWC_CTLL_DST_DEC        1
#define DWC_CTLL_DST_FIX        2
#define DWC_CTLL_SRC_INC        0
#define DWC_CTLL_SRC_DEC        1
#define DWC_CTLL_SRC_FIX        2

/******************************************************************************/
/*                                                                            */
/*                           SPI                                              */
/*                                                                            */
/******************************************************************************/
// CTRL
#define SPI_CLK_DIVIDER_POS             0
#define SPI_MASTER_ENABLE_POS           16
#define SPI_MODE1_POS                   17
#define SPI_MASTER_CE_AT_END_POS        18
#define SPI_SOFT_RESET_POS              19
#define SPI_MSB_FIRST_POS               20
#define SPI_INVERT_CLOCK_POS            21
#define SPI_USE_RDY_OUT_POS             22
#define SPI_BIDIRECT_DATA_POS           23
#define SPI_ACTIVE_DO_ENL_POS           24
#define SPI_INACTIVE_DO_ENL_POS         25
#define SPI_RX_TRIGGER_LEVEL_POS        26
#define SPI_RX_CLR_FIFO_POS             28
#define SPI_TX_CLR_FIFO_POS             29
#define SPI_RX_FIFO_ENABLE_POS          30
#define SPI_TX_FIFO_ENABLE_POS          31
#define SPI_CLK_DIVIDER_MASK            0x0000FFFF
#define SPI_MASTER_ENABLE_MASK          0x00010000
#define SPI_MODE1_MASK                  0x00020000
#define SPI_MASTER_CE_AT_END_MASK       0x00040000
#define SPI_SOFT_RESET_MASK             0x00080000
#define SPI_MSB_FIRST_MASK              0x00100000
#define SPI_INVERT_CLOCK_MASK           0x00200000
#define SPI_USE_RDY_OUT_MASK            0x00400000
#define SPI_BIDIRECT_DATA_MASK          0x00800000
#define SPI_ACTIVE_DO_ENL_MASK          0x01000000
#define SPI_INACTIVE_DO_ENL_MASK        0x02000000
#define SPI_RX_TRIGGER_LEVEL_MASK       0x0C000000
#define SPI_RX_CLR_FIFO_MASK            0x10000000
#define SPI_TX_CLR_FIFO_MASK            0x20000000
#define SPI_RX_FIFO_ENABLE_MASK         0x40000000
#define SPI_TX_FIFO_ENABLE_MASK         0x80000000
// STAT
#define SPI_STAT_RX_NOTEMPTY_POS        0
#define SPI_STAT_RX_FIFO_OVERRUN_POS    1
#define SPI_STAT_TX_FIFO_OVERRUN_POS    2
#define SPI_STAT_RESERVED_POS           3
#define SPI_STAT_TX_EMPTY_INT_EN_POS    4
#define SPI_STAT_TX_EMPTY_POS           5
#define SPI_STAT_RX_TRIG_INT_EN_POS     6
#define SPI_STAT_RX_FIFO_TRIG_POS       7
#define SPI_STAT_RX_BYTE_CNT_POS        8
#define SPI_STAT_TX_BYTE_CNT_POS        16
#define SPI_STAT_SPI_ACTIVE_POS         24
#define SPI_STAT_SPI_RDY_IN_POS         25
#define SPI_STAT_SPI_RDY_OUT_POS        26
#define SPI_STAT_WAITING_FOR_RDY_POS    27
#define SPI_STAT_BIT_COUNT_POS          28
#define SPI_STAT_SPI_INT_POS            31
#define SPI_STAT_RX_NOTEMPTY_MASK       0x00000001
#define SPI_STAT_RX_FIFO_OVERRUN_MASK   0x00000002
#define SPI_STAT_TX_FIFO_OVERRUN_MASK   0x00000004
#define SPI_STAT_RESERVED_MASK          0x00000008
#define SPI_STAT_TX_EMPTY_INT_EN_MASK   0x00000010
#define SPI_STAT_TX_EMPTY_MASK          0x00000020
#define SPI_STAT_RX_TRIG_INT_EN_MASK    0x00000040
#define SPI_STAT_RX_FIFO_TRIG_MASK      0x00000080
#define SPI_STAT_RX_BYTE_CNT_MASK       0x0000FF00
#define SPI_STAT_TX_BYTE_CNT_MASK       0x00FF0000
#define SPI_STAT_SPI_ACTIVE_MASK        0x01000000
#define SPI_STAT_SPI_RDY_IN_MASK        0x02000000
#define SPI_STAT_SPI_RDY_OUT_MASK       0x04000000
#define SPI_STAT_WAITING_FOR_RDY_MASK   0x08000000
#define SPI_STAT_BIT_COUNT_MASK         0x70000000
#define SPI_STAT_SPI_INT_MASK           0x80000000
// CSNCTRL
#define SPI_CS_MANUAL_POS               0
#define SPI_CS_LEVEL_POS                1
#define SPI_CS_MANUAL_MASK              0x00000001
#define SPI_CS_LEVEL_MASK               0x00000002

/******************************************************************************/
/*                                                                            */
/*                             IR TX                                          */
/*                                                                            */
/******************************************************************************/
#define IR_TX_ANA_IF_POWER_DOWN_POS    1
#define IR_TX_ANA_IF_POWER_DOWN_MASK   0x00000002



/******************************************************************************/
/*                                                                            */
/*                             RTC                                            */
/*                                                                            */
/******************************************************************************/
// RTC CR
#define RTC_READY_POS               7
#define RTC_1HZ_POS                 6
#define RTC_1HZIE_POS               5
#define RTC_AF_POS                  4
#define RTC_AIE_POS                 3
#define RTC_AE_POS                  2
#define RTC_CE_POS                  0
#define RTC_1HZ_FLAG_MASK           0x00000200
#define RTC_AF_FLAG_MASK            0x00000100
#define RTC_READY_MASK              0x00000080
#define RTC_1HZ_MASK                0x00000040
#define RTC_1HZIE_MASK              0x00000020
#define RTC_AF_MASK                 0x00000010
#define RTC_AIE_MASK                0x00000008
#define RTC_AE_MASK                 0x00000004
#define RTC_CE_MASK                 0x00000001
// RTC GR
#define RTC_LOCK_POS                31
#define RTC_ADJC_POS                16
#define RTC_NC1HZ_POS               0
#define RTC_LOCK_MASK               0x80000000
#define RTC_ADJC_MASK               0x03FF0000
#define RTC_NC1HZ_MASK              0x0000FFFF
/******************************************************************************/
/*                                                                            */
/*                            PSO CPM                                         */
/*                                                                            */
/******************************************************************************/
// REV
#define CPM_CPM_REV_POS                     0
#define CPM_CPM_REV_MASK                    0xFFFFFFFF
// CPU_CFG
#define CPM_AHB_CLK_PERIPH_EN_POS           23
#define CPM_AHB_CLK_RAM_EN_POS              22
#define CPM_CPU_DIV_COEFF_POS               8
#define CPM_CPU_DIV_SEL_POS                 2
#define CPM_CPU_DIV_EN_POS                  1
#define CPM_AHB_CLK_PERIPH_EN_MASK          0x00800000
#define CPM_AHB_CLK_RAM_EN_MASK             0x00400000
#define CPM_CPU_DIV_COEFF_MASK              0x0000FF00
#define CPM_CPU_DIV_SEL_MASK                0x00000004
#define CPM_CPU_DIV_EN_MASK                 0x00000002

// APB_CFG
#define CPM_ROM_APB_SOFT_RESET_POS          19
#define CPM_PSO_APB_SOFT_RESET_POS          18
#define CPM_PMU_APB_SOFT_RESET_POS          16
#define CPM_ROM_APB_GATE_EN_POS             3
#define CPM_PSO_APB_GATE_EN_POS             2
#define CPM_RTC_APB_GATE_EN_POS             1
#define CPM_PMU_APB_GATE_EN_POS             0
#define CPM_ROM_APB_SOFT_RESET_MASK         0x00080000
#define CPM_PSO_APB_SOFT_RESET_MASK         0x00040000
#define CPM_PMU_APB_SOFT_RESET_MASK         0x00010000
#define CPM_ROM_APB_GATE_EN_MASK            0x00000008
#define CPM_PSO_APB_GATE_EN_MASK            0x00000004
#define CPM_RTC_APB_GATE_EN_MASK            0x00000002
#define CPM_PMU_APB_GATE_EN_MASK            0x00000001
// REG_UPD
#define CPM_REG_UPD_RW_POS                  8
#define CPM_REG_UPD_RW_STATUS_POS           7
#define CPM_REG_UPD_32K_STATUS_POS          6
#define CPM_REG_UPD_XTAL32M_STATUS_POS      5
#define CPM_REG_UPD_CPU_STATUS_POS          4
#define CPM_REG_UPD_STATUS_CLR_POS          3
#define CPM_REG_UPD_32K_POS                 2
#define CPM_REG_UPD_XTAL32M_POS             1
#define CPM_REG_UPD_CPU_POS                 0
#define CPM_REG_UPD_RW_MASK                 0x00000100
#define CPM_REG_UPD_RW_STATUS_MASK          0x00000080
#define CPM_REG_UPD_32K_STATUS_MASK         0x00000040
#define CPM_REG_UPD_XTAL32M_STATUS_MASK     0x00000020
#define CPM_REG_UPD_CPU_STATUS_MASK         0x00000010
#define CPM_REG_UPD_STATUS_CLR_MASK         0x00000008
#define CPM_REG_UPD_32K_MASK                0x00000004
#define CPM_REG_UPD_XTAL32M_MASK            0x00000002
#define CPM_REG_UPD_CPU_MASK                0x00000001
// SF_CFG
#define CPM_SF_DIV_COEFF_POS                8
#define CPM_SF_SOFT_RESET_POS               4
#define CPM_SF_DIV_SEL_POS                  2
#define CPM_SF_DIV_EN_POS                   1
#define CPM_SF_GATE_EN_POS                  0
#define CPM_SF_DIV_COEFF_MASK               0x0000FF00
#define CPM_SF_SOFT_RESET_MASK              0x00000010
#define CPM_SF_DIV_SEL_MASK                 0x00000004
#define CPM_SF_DIV_EN_MASK                  0x00000002
#define CPM_SF_GATE_EN_MASK                 0x00000001
// TIM0_CFG TIM1_CFG TIM2_CFG
#define CPM_TIMER_DIV_COEFF_POS             8
#define CPM_TIMER_SOFT_RESET_POS            4
#define CPM_TIMER_DIV_SEL_POS               2
#define CPM_TIMER_DIV_EN_POS                1
#define CPM_TIMER_GATE_EN_POS               0
#define CPM_TIMER_DIV_COEFF_MASK            0x0000FF00
#define CPM_TIMER_SOFT_RESET_MASK           0x00000010
#define CPM_TIMER_DIV_SEL_MASK              0x00000004
#define CPM_TIMER_DIV_EN_MASK               0x00000002
#define CPM_TIMER_GATE_EN_MASK              0x00000001
// UART0_CFG UART1_CFG
#define CPM_UART_DIV_COEFF_FRC_POS          24
#define CPM_UART_DIV_COEFF_INT_POS          8
#define CPM_UART_SOFT_RESET_POS             4
#define CPM_UART_DIV_SEL_POS                2
#define CPM_UART_DIV_EN_POS                 1
#define CPM_UART_GATE_EN_POS                0
#define CPM_UART_DIV_COEFF_FRC_MASK         0xFF000000
#define CPM_UART_DIV_COEFF_INT_MASK         0x0001FF00
#define CPM_UART_SOFT_RESET_MASK            0x00000010
#define CPM_UART_DIV_SEL_MASK               0x00000004
#define CPM_UART_DIV_EN_MASK                0x00000002
#define CPM_UART_GATE_EN_MASK               0x00000001
// I2C_CFG
#define CPM_I2C_DIV_COEFF_POS               8
#define CPM_I2C_SOFT_RESET_POS              4
#define CPM_I2C_DIV_SEL_POS                 2
#define CPM_I2C_DIV_EN_POS                  1
#define CPM_I2C_GATE_EN_POS                 0
#define CPM_I2C_DIV_COEFF_MASK              0x0000FF00
#define CPM_I2C_SOFT_RESET_MASK             0x00000010
#define CPM_I2C_DIV_SEL_MASK                0x00000004
#define CPM_I2C_DIV_EN_MASK                 0x00000002
#define CPM_I2C_GATE_EN_MASK                0x00000001
// IRTX_CFG
#define CPM_IRTX_DIV_COEFF_POS              8
#define CPM_IRTX_SOFT_RESET_POS             4
#define CPM_IRTX_DIV_SEL_POS                2
#define CPM_IRTX_DIV_EN_POS                 1
#define CPM_IRTX_GATE_EN_POS                0
#define CPM_IRTX_DIV_COEFF_INT_MASK         0x0000FF00
#define CPM_IRTX_SOFT_RESET_MASK            0x00000010
#define CPM_IRTX_DIV_SEL_MASK               0x00000004
#define CPM_IRTX_DIV_EN_MASK                0x00000002
#define CPM_IRTX_GATE_EN_MASK               0x00000001
// SPI0_CFG
#define CPM_SPI0_SOFT_RESET_POS             4
#define CPM_SPI0_GATE_EN_POS                0
#define CPM_SPI0_SOFT_RESET_MASK            0x00000010
#define CPM_SPI0_GATE_EN_MASK               0x00000001
// BASEBAND_CFG
#define CPM_BASEBAND_GATE_EN_POS            0
#define CPM_BASEBAND_GATE_EN_MASK           0x00000003
// CPU_TCLK_CFG
#define CPM_CPU_TCLK_DIV_COEFF_POS          8
#define CPM_CPU_TCLK_DIV_SEL_POS            2
#define CPM_CPU_TCLK_DIV_EN_POS             1
#define CPM_CPU_TCLK_GATE_EN_POS            0
#define CPM_CPU_TCLK_DIV_COEFF_MASK         0x0000FF00
#define CPM_CPU_TCLK_DIV_SEL_MASK           0x00000004
#define CPM_CPU_TCLK_DIV_EN_MASK            0x00000002
#define CPM_CPU_TCLK_GATE_EN_MASK           0x00000001
// AHB_CFG
#define CPM_RAM_AUTO_GATE_EN_POS            16
#define CPM_MCU_WAKEUP_CYCLE_POS            8
#define CPM_AHB_SOFT_RESET_POS              4
#define CPM_RAM_AUTO_GATE_EN_MASK           0x00010000
#define CPM_MCU_WAKEUP_CYCLE_MASK           0x00003F00
#define CPM_AHB_SOFT_RESET_MASK             0x00000010
// DMA_CFG
#define CPM_DMA_SOFT_RESET_POS              4
#define CPM_DMA_GATE_EN_POS                 0
#define CPM_DMA_SOFT_RESET_MASK             0x00000010
#define CPM_DMA_GATE_EN_MASK                0x00000001
// RAM_CFG
#define CPM_RAM_GATE_EN_POS                 0
#define CPM_RAM_GATE_EN_MASK                0xFFFFFFFF
// AUDIO_CFG
#define CPM_AUDIO_SOFT_RESET_POS            4
#define CPM_AUDIO_12M_GATE_EN_POS           1
#define CPM_AUDIO_GATE_EN_POS               0
#define CPM_AUDIO_SOFT_RESET_MASK           0x00000010
#define CPM_AUDIO_12M_GATE_EN_MASK          0x00000002
#define CPM_AUDIO_GATE_EN_MASK              0x00000001
// GPIO_CFG
#define CPM_GPIO_SOFT_RESET_POS             4
#define CPM_GPIO_GATE_EN_POS                0
#define CPM_GPIO_SOFT_RESET_MASK            0x00000010
#define CPM_GPIO_GATE_EN_MASK               0x00000001
// QDEC_CFG
#define CPM_QDEC_DIV_COEFF_POS              8
#define CPM_QDEC_SOFT_RESET_POS             4
#define CPM_QDEC_DIV_SEL_POS                2
#define CPM_QDEC_DIV_EN_POS                 1
#define CPM_QDEC_GATE_EN_POS                0
#define CPM_QDEC_DIV_COEFF_MASK             0x0000FF00
#define CPM_QDEC_SOFT_RESET_MASK            0x00000010
#define CPM_QDEC_DIV_SEL_MASK               0x00000004
#define CPM_QDEC_DIV_EN_MASK                0x00000002
#define CPM_QDEC_GATE_EN_MASK               0x00000001
// SPI1_CFG
#define CPM_SPI1_SOFT_RESET_POS             4
#define CPM_SPI1_GATE_EN_POS                0
#define CPM_SPI1_SOFT_RESET_MASK            0x00000010
#define CPM_SPI1_GATE_EN_MASK               0x00000001
// PHY_CFG
#define CPM_PHY_SOFT_RESET_POS              4
#define CPM_PHY_CLK_GATE_EN_POS             1
#define CPM_PHY_AHB_GATE_EN_POS             0
#define CPM_PHY_GATE_EN_POS                 0
#define CPM_PHY_SOFT_RESET_MASK             0x00000010
#define CPM_PHY_CLK_GATE_EN_MASK            0x00000002
#define CPM_PHY_AHB_GATE_EN_MASK            0x00000001
#define CPM_PHY_GATE_EN_MASK                0x00000003
// RNG_CFG
#define CPM_RNG_SOFT_RESET_POS              4
#define CPM_RNG_GATE_EN_POS                 0
#define CPM_RNG_SOFT_RESET_MASK             0x00000010
#define CPM_RNG_GATE_EN_MASK                0x00000001
// I2S_CFG
#define CPM_I2S_SLV_SEL_POS                 7
#define CPM_I2S_EXT_INV_POS                 6
//#define CPM_I2S_TX_SOFT_SEL_POS             5 //~{T-@45D~}
//#define CPM_I2S_RX_SOFT_SEL_POS             4 //~{T-@45D~}
#define CPM_I2S_TX_SOFT_RESET_POS             5 //~{:s@4P^8D5D~}
#define CPM_I2S_RX_SOFT_RESET_POS             4 //~{:s@4P^8D5D~}
#define CPM_I2S_TX_AHB_EN_POS               1
#define CPM_I2S_RX_AHB_EN_POS               0
#define CPM_I2S_SLV_SEL_MASK                0x00000080
#define CPM_I2S_EXT_INV_MASK                0x00000040
#define CPM_I2S_TX_SOFT_RESET_MASK          0x00000020
#define CPM_I2S_RX_SOFT_RESET_MASK          0x00000010
#define CPM_I2S_TX_AHB_EN_MASK              0x00000002
#define CPM_I2S_RX_AHB_EN_MASK              0x00000001
// STATUS_READ
#define CPM_ROM_CLK_SYNC_DONE_POS           13
#define CPM_CPU_TCLK_SYNC_DONE_POS          12
#define CPM_QDEC_CLK_SYNC_DONE_POS          11
#define CPM_KPP_CLK_SYNC_DONE_POS           10
#define CPM_I2C3_CLK_SYNC_DONE_POS          9
#define CPM_I2C2_CLK_SYNC_DONE_POS          8
#define CPM_I2C_CLK_SYNC_DONE_POS           7
#define CPM_UART1_CLK_SYNC_DONE_POS         6
#define CPM_IRTX_CLK_SYNC_DONE_POS          5 //xh98xx
#define CPM_TIMER3_CLK_SYNC_DONE_POS        4
#define CPM_TIMER2_CLK_SYNC_DONE_POS        3
#define CPM_TIMER1_CLK_SYNC_DONE_POS        2
#define CPM_SF_CLK_SYNC_DONE_POS            1
#define CPM_MAIN_CLK_SYNC_DONE_POS          0
#define CPM_ROM_CLK_SYNC_DONE_MASK          0x00002000
#define CPM_CPU_TCLK_SYNC_DONE_MASK         0x00001000
#define CPM_QDEC_CLK_SYNC_DONE_MASK         0x00000800
#define CPM_KPP_CLK_SYNC_DONE_MASK          0x00000400
#define CPM_I2C3_CLK_SYNC_DONE_MASK         0x00000200
#define CPM_I2C2_CLK_SYNC_DONE_MASK         0x00000100
#define CPM_I2C_CLK_SYNC_DONE_MASK          0x00000080
#define CPM_UART1_CLK_SYNC_DONE_MASK        0x00000040
#define CPM_IRTX_CLK_SYNC_DONE_MASK         0x00000020 //xh98xx
#define CPM_TIMER3_CLK_SYNC_DONE_MASK       0x00000010
#define CPM_TIMER2_CLK_SYNC_DONE_MASK       0x00000008
#define CPM_TIMER1_CLK_SYNC_DONE_MASK       0x00000004
#define CPM_SF_CLK_SYNC_DONE_MASK           0x00000002
#define CPM_MAIN_CLK_SYNC_DONE_MASK         0x00000001
// ANA_IF_AHB_CFG
#define CPM_ANA_IF_AHB_SOFT_RESET_POS       4
#define CPM_ANA_IF_AHB_GATE_EN_POS          0
#define CPM_ANA_IF_AHB_SOFT_RESET_MASK      0x00000010
#define CPM_ANA_IF_AHB_GATE_EN_MASK         0x00000001
// XH98xx_CFG
#define CPM_XH98xx_SOFT_RESET_POS           4
#define CPM_XH98xx_GATE_EN_POS              0
#define CPM_XH98xx_SOFT_RESET_MASK          0x00000010
#define CPM_XH98xx_GATE_EN_MASK             0x00000001
// ANA_IF_CLK_CFG
#define CPM_ANA_IF_CLK_GATE_EN_POS          0
#define CPM_ANA_IF_CLK_GATE_EN_MASK         0x00000001
#define CPM_EFUSE_SOFT_RESET_POS            4
#define CPM_EFUSE_GATE_EN_POS               0
#define CPM_EFUSE_SOFT_RESET_MASK           0x00000010
#define CPM_EFUSE_GATE_EN_MASK              0x00000001
// SF, TIM0, TIM1, TIM2, I2C, KPP, QDEC div
#define CPM_DIV_COEFF_POS                   8
#define CPM_DIV_SEL_POS                     2
#define CPM_DIV_EN_POS                      1
#define CPM_GATE_EN_POS                     0
#define CPM_DIV_COEFF_MASK                  0x0000FF00
#define CPM_DIV_SEL_MASK                    0x00000004
#define CPM_DIV_EN_MASK                     0x00000002
#define CPM_GATE_EN_MASK                    0x00000001
// EFUSE_CFG
#define CPM_EFUSE_SOFT_RESET_POS            4
#define CPM_EFUSE_GATE_EN_POS               0
#define CPM_EFUSE_SOFT_RESET_MASK           0x00000010
#define CPM_EFUSE_GATE_EN_MASK              0x00000001

/******************************************************************************/
/*                                                                            */
/*                                   EFUSE                                      */
/*                                                                            */
/******************************************************************************/
// EFUSE program enable PROGRAM_ENABLE (offset 0xF000)
#define EFUSE_PROGRAM_DISABLE                            0U
#define EFUSE_PROGRAM_ENABLE                            (1U << 0)
// EFUSE program start PROGRAM_START (offset 0xF004)
#define EFUSE_PROGRAM_START_POS                          0U
#define EFUSE_PROGRAM_START_MASK                        (1U << 0)
// EFUSE avdd selected AVDD_ENABLE_SELECT (offset 0xF008)
#define EFUSE_AVDD_ENABLE_SELECT_HDW                     0x0
#define EFUSE_AVDD_ENABLE_SELECT_SOFT                   (1U << 0)
// EFUSE program address PROGRAM_ADDRESS (offset 0xF00C)
#define EFUSE_PROGRAM_ADDRESS_POS                        9U
#define EFUSE_PROGRAM_ADDRESS_MASK                       0x1FF
// EFUSE program config0 PROGRAM_CFG0 (offset 0xF018)
#define EFUSE_PROGRAM_CFG0_L16_NS                       1000U
#define EFUSE_PROGRAM_CFG0_H16_NS                       100U
// EFUSE program config1 PROGRAM_CFG1 (offset 0xF01C)
#define EFUSE_PROGRAM_CFG1_T_PGM_POS                    16
#define EFUSE_PROGRAM_CFG1_T_AEN_POS                    0
#define EFUSE_PROGRAM_CFG1_T_PGM_MASK                   0xFFFF0000
#define EFUSE_PROGRAM_CFG1_T_AEN_MASK                   0x0000FFFF
// EFUSE program config2 PROGRAM_CFG2 (offset 0xF020)
#define EFUSE_PROGRAM_CFG2_L16_NS                       50U
// EFUSE read config READ_CFG (offset 0xF024)
#define EFUSE_READ_CFG_L8_NS                            40U
#define EFUSE_READ_CFG_L16_NS                           75U
#define EFUSE_READ_CFG_H24_NS                           10U
#define EFUSE_READ_CFG_H32_NS                           100U
// EFUSE program interrupt PROGRAM_INTR (offset 0xF028)
#define EFUSE_PROGRAM_INTR_DISABLE                       0U
#define EFUSE_PROGRAM_INTR_ENABLE                       (1U << 0)
#define EFUSE_PROGRAM_INTR_CLR_POS                       1U
#define EFUSE_PROGRAM_INTR_CLR_MASK                     (1U << 1)
#define EFUSE_PROGRAM_INTR_POS                           2U
#define EFUSE_PROGRAM_INTR_MASK                         (1U << 2)
// EFUSE status STATUS (offset 0xF02C)
#define EFUSE_STATUS_CTRL_STATE_POS                      0U
#define EFUSE_STATUS_CTRL_STATE_MASK                    (1U << 0)
#define EFUSE_STATUS_READ_PROTECT_POS                    1U
#define EFUSE_STATUS_READ_PROTECT_MASK                  (1U << 1)
#define EFUSE_PROGRAM_START_POS                          0U
#define EFUSE_PROGRAM_START_MASK                        (1U << 0)

/******************************************************************************/
/*                                                                            */
/*                                   PMU                                      */
/*                                                                            */
/******************************************************************************/
// BASIC
#define PMU_BASIC_DCDC_ON_DELAY_POS                     0
#define PMU_BASIC_PIN_WAKEUP_MODE_POS                   4
#define PMU_BASIC_PIN_WAKEUP_MODE_1_POS                 5
#define PMU_BASIC_PSO_ON_EN_POS                         6
#define PMU_BASIC_STATE_RESET_POS                       7
//#define PMU_BASIC_RTC_CLK_GATE_POS                      8
#define PMU_BASIC_CPU_32K_CLK_GATE_POS                  9
#define PMU_BASIC_RW_32K_CLK_GATE_POS                   10
//#define PMU_BASIC_POWER_DOWN_ACK_POS                    11
//#define PMU_BASIC_POWER_DOWN_DISABLE_POS                12
//#define PMU_BASIC_CM3_OFF_EN_POS                        13
#define PMU_BASIC_SLEEP_WO_32K_POS                      14
#define PMU_BASIC_CLK48M_OFF_ONLY_POS                   15
#define PMU_BASIC_WAKEUPB_DIS_POS                       16 //xh98xx
//#define PMU_BASIC_QDEC_INT_PMU_EN_POS                   17
#define PMU_BASIC_TIMER_CLK_EN_POS                      18
#define PMU_BASIC_TIMER_INT_WAKE_EN_POS                 19
#define PMU_BASIC_TIMER_INT_MASK_POS                    20
#define PMU_BASIC_APB_BUS_UPD_REG_POS                   24
#define PMU_BASIC_PWRON_EN_POS                          25
#define PMU_BASIC_ANA_CRY32K_READY_POS                  26
#define PMU_BASIC_STATE_POS                             27
#define PMU_BASIC_DCDC_ON_DELAY_MASK                    0x0000000F
#define PMU_BASIC_PIN_WAKEUP_MODE_MASK                  0x00000010
#define PMU_BASIC_PIN_WAKEUP_MODE_1_MASK                0x00000020
#define PMU_BASIC_PSO_ON_EN_MASK                        0x00000040
#define PMU_BASIC_STATE_RESET_MASK                      0x00000080
//#define PMU_BASIC_RTC_CLK_GATE_MASK                     0x00000100
#define PMU_BASIC_CPU_32K_CLK_GATE_MASK                 0x00000200
#define PMU_BASIC_RW_32K_CLK_GATE_MASK                  0x00000400
//#define PMU_BASIC_POWER_DOWN_ACK_MASK                   0x00000800
//#define PMU_BASIC_POWER_DOWN_DISABLE_MASK               0x00001000
//#define PMU_BASIC_CM3_OFF_EN_MASK                       0x00002000
#define PMU_BASIC_SLEEP_WO_32K_MASK                     0x00004000
#define PMU_BASIC_CLK48M_OFF_ONLY_MASK                  0x00008000
#define PMU_BASIC_WAKEUPB_DIS_MASK                      0x00010000
//#define PMU_BASIC_QDEC_INT_PMU_EN_MASK                  0x00020000
#define PMU_BASIC_TIMER_CLK_EN_MASK                     0x00040000
#define PMU_BASIC_TIMER_INT_WAKE_EN_MASK                0x00080000
#define PMU_BASIC_TIMER_INT_MASK_MASK                   0x00100000
#define PMU_BASIC_APB_BUS_UPD_REG_MASK                  0x01000000
#define PMU_BASIC_PWRON_EN_MASK                         0x02000000
#define PMU_BASIC_ANA_CRY32K_READY_MASK                 0x04000000
#define PMU_BASIC_STATE_MASK                            0xF8000000
// PSO_PM
#define PMU_PM_PSO_POWER_STATUS_POS                     0
#define PMU_PM_PSO_POWER_STATUS_MASK                    0x00000001
// XTAL32M_CNS0
#define PMU_XTAL32M_DISTURB_M123_NUM_CFG_POS            24
#define PMU_XTAL32M_EN_XTAL32M_PKD_AUTO_DROP_POS        20
#define PMU_XTAL32M_EN_OSC32M_ME_POS                    19
#define PMU_XTAL32M_EN_OSC32M_MO_POS                    18
#define PMU_XTAL32M_SEL_CPUCLK_POS                      16
#define PMU_XTAL32M_EN_OSC32M_CHIRPRAMP_ME_POS          15
#define PMU_XTAL32M_EN_OSC32M_CHIRPRAMP_MO_POS          14
#define PMU_XTAL32M_EN_XTAL32M_PKD_ME_POS               13
#define PMU_XTAL32M_EN_XTAL32M_PKD_MO_POS               12
#define PMU_XTAL32M_EN_XTAL32M_NRB_ME_POS               11
#define PMU_XTAL32M_EN_XTAL32M_NRB_MO_POS               8
#define PMU_EN_64M_DGLITCH_POS                          5
#define PMU_EN_32M_DGLITCH_POS                          4
#define PMU_XTAL32M_PD_XTAL32M_DIGBUF_MO_POS            3
#define PMU_XTAL32M_PD_XTAL32M_DIGBUF_ME_POS            2
#define PMU_XTAL32M_PD_XTAL32M_PLLBUF_MO_POS            1
#define PMU_XTAL32M_PD_XTAL32M_PLLBUF_ME_POS            0
#define PMU_XTAL32M_DISTURB_M123_NUM_CFG_MASK           0xFF000000
#define PMU_XTAL32M_EN_XTAL32M_PKD_AUTO_DROP_MASK       0x00100000
#define PMU_XTAL32M_EN_OSC32M_ME_MASK                   0x00080000
#define PMU_XTAL32M_EN_OSC32M_MO_MASK                   0x00040000
#define PMU_XTAL32M_SEL_CPUCLK_MASK                     0x00010000
#define PMU_XTAL32M_EN_OSC32M_CHIRPRAMP_ME_MASK         0x00008000
#define PMU_XTAL32M_EN_OSC32M_CHIRPRAMP_MO_MASK         0x00004000
#define PMU_XTAL32M_EN_XTAL32M_PKD_ME_MASK              0x00002000
#define PMU_XTAL32M_EN_XTAL32M_PKD_MO_MASK              0x00001000
#define PMU_XTAL32M_EN_XTAL32M_NRB_ME_MASK              0x00000800
#define PMU_XTAL32M_EN_XTAL32M_NRB_MO_MASK              0x00000300
#define PMU_EN_64M_DGLITCH_MASK                         0x00000020
#define PMU_EN_32M_DGLITCH_MASK                         0x00000010
#define PMU_XTAL32M_PD_XTAL32M_DIGBUF_MO_MASK           0x00000008
#define PMU_XTAL32M_PD_XTAL32M_DIGBUF_ME_MASK           0x00000004
#define PMU_XTAL32M_PD_XTAL32M_PLLBUF_MO_MASK           0x00000002
#define PMU_XTAL32M_PD_XTAL32M_PLLBUF_ME_MASK           0x00000001
// RAM_PM_1
#define PMU_PM_RAM_POWER_ON_POS                         0
#define PMU_PM_ICACHE_POWER_ON_POS                      2
#define PMU_PM_RAM_POWER_ON_MASK                        0x00000003
#define PMU_PM_ICACHE_POWER_ON_MASK                     0x00000004
// RAM_PM_2
#define PMU_PM_RAM_POWER_STATUS_POS                     0
#define PMU_PM_ICACHE_POWER_STATUS_POS                  2
#define PMU_PM_RAM_POWER_STATUS_MASK                    0x0000000F
#define PMU_PM_ICACHE_POWER_STATUS_MASK                 0x00000004
// ANA_PD
#if defined(CONFIG_XH98xxD) || defined(CONFIG_XH98xxF)
#define PMU_ANA_OSC_CLK1M_HW_DIS_POS                    0
#define PMU_ANA_REG_PD_DIGITAL_LDO_POS                  1
#define PMU_ANA_REG_PD_BIAS_POS                         3
#define PMU_ANA_PD_FLASH_LDO_POS                        4
#define PMU_ANA_PD_LDO_3P3_POS                          5
#define PMU_ANA_PD_BOR_POS                              6
#define PMU_ANA_EN_IOLDO_ILIMIT_POS                     7
#define PMU_ANA_CHG_TRIM_POS                            8
#define PMU_ANA_PD_CHG_POS                              14
#define PMU_ANA_CHG_ISEL_POS                            15
#define PMU_ANA_BUCK_DIS_POS                            16
#define PMU_ANA_LDO_1P5_DIS_POS                         17
#define PMU_ANA_BUCK_OSC_PD_POS                         18
#define PMU_ANA_REG_PD_BUF_DIG_POS                      21
#define PMU_ANA_PD_LDO_ANADIG_POS                       22
#define PMU_ANA_EN_64M_POS                              23
#define PMU_ANA_TRIM_VREF_POS                           28
#define PMU_ANA_OSC_CLK1M_HW_DIS_MASK                   0x00000001
#define PMU_ANA_REG_PD_DIGITAL_LDO_MASK                 0x00000002
#define PMU_ANA_REG_PD_BIAS_MASK                        0x00000008
#define PMU_ANA_PD_FLASH_LDO_MASK                       0x00000010
#define PMU_ANA_PD_LDO_3P3_MASK                         0x00000020
#define PMU_ANA_PD_BOR_MASK                             0x00000040
#define PMU_ANA_EN_IOLDO_ILIMIT_MASK                    0x00000080
#define PMU_ANA_CHG_TRIM_MASK                           0x00003F00
#define PMU_ANA_PD_CHG_MASK                             0x00004000
#define PMU_ANA_CHG_ISEL_MASK                           0x00008000
#define PMU_ANA_BUCK_DIS_MASK                           0x00010000
#define PMU_ANA_LDO_1P5_DIS_MASK                        0x00020000
#define PMU_ANA_BUCK_OSC_PD_MASK                        0x00040000
#define PMU_ANA_REG_PD_BUF_DIG_MASK                     0x00200000
#define PMU_ANA_PD_LDO_ANADIG_MASK                      0x00400000
#define PMU_ANA_EN_64M_MASK                             0x00800000
#define PMU_ANA_TRIM_VREF_MASK                          0xF0000000
#else
#define PMU_ANA_OSC_CLK1M_HW_DIS_POS                    0
#define PMU_ANA_REG_PD_DIGITAL_LDO_POS                  1
#define PMU_REG_PD_DCDC_POS                             2
#define PMU_ANA_REG_PD_BIAS_POS                         3
#define PMU_ANA_PD_FLASH_LDO_POS                        4
#define PMU_ANA_PD_LDO_3P3_POS                          5
#define PMU_ANA_PD_BOR_POS                              6
#define PMU_DCDC_SHORT_POS                              6
#define PMU_REG_PD_DCDC_OSC_POS                         7
#define PMU_DCDC_LIMIT_POS                              8
#define PMU_DCDC_ILIMIT_POS                             11
#define PMU_DCDC_SLOPE_POS                              12
#define PMU_DCDC_DIS_POS                                16
#define PMU_ANA_BUCK_DIS_POS                            16 // xh98xx new name
#define PMU_ANA_1P2LDO_DIS_POS                          17
#define PMU_ANA_LDO_1P5_DIS_POS                         17 // xh98xx new name
#define PMU_ANA_BUCK_OSC_PD_POS                         18
#define PMU_VSUB_SEL_POS                                19
#define PMU_ANA_PD_LDO_ANADIG_SEL_POS                   21
#define PMU_ANA_PD_LDO_ANADIG_POS                       22
#define PMU_ANA_EN_64M_POS                              23
#define PMU_DCDC_CTUNE_POS                              24
#define PMU_DCDC_OSC_TEST_POS                           27
#define PMU_ANA_TRIM_VREF_POS                           28
#define PMU_ANA_OSC_CLK1M_HW_DIS_MASK                   0x00000001
#define PMU_ANA_REG_PD_DIGITAL_LDO_MASK                 0x00000002
#define PMU_REG_PD_DCDC_MASK                            0x00000004
#define PMU_ANA_REG_PD_BIAS_MASK                        0x00000008
#define PMU_ANA_PD_FLASH_LDO_MASK                       0x00000010
#define PMU_ANA_PD_LDO_3P3_MASK                         0x00000020
#define PMU_ANA_PD_BOR_MASK                             0x00000040
#define PMU_DCDC_SHORT_MASK                             0x00000040
#define PMU_REG_PD_DCDC_OSC_MASK                        0x00000080
#define PMU_DCDC_LIMIT_MASK                             0x00000700
#define PMU_DCDC_ILIMIT_MASK                            0x00000800
#define PMU_DCDC_SLOPE_MASK                             0x0000F000
#define PMU_ANA_BUCK_DIS_MASK                           0x00010000
#define PMU_DCDC_DIS_MASK                               0x00010000
#define PMU_ANA_LDO_1P5_DIS_MASK                        0x00020000
#define PMU_ANA_1P2LDO_DIS_MASK                         0x00020000
#define PMU_ANA_BUCK_OSC_PD_MASK                        0x00040000
#define PMU_VSUB_SEL_MASK                               0x00180000
#define PMU_ANA_PD_LDO_ANADIG_SEL_MASK                  0x00200000
#define PMU_ANA_PD_LDO_ANADIG_MASK                      0x00400000
#define PMU_ANA_EN_64M_MASK                             0x00800000
#define PMU_DCDC_CTUNE_MASK                             0x07000000
#define PMU_DCDC_OSC_TEST_MASK                          0x08000000
#define PMU_ANA_TRIM_VREF_MASK                          0xF0000000
#endif
// GPIO_POL
#define PMU_REG_GPIO_POL_31_0_POS                       0
#define PMU_REG_GPIO_POL_31_0_MASK                      0xFFFFFFFF
// GPIO_POL_1
#define PMU_GPIO_POL_POWERDOWN_POS                      0
#define PMU_GPIO_POL_POWERDOWN_MASK                     0x00000001
// MISC_CTRL
#define PMU_MISC_CLK_32K_SEL_POS                        1
#define PMU_MISC_CLK_GATE_POS                           3
//#define PMU_MISC_CRY_32K_GATE_POS                       4
#define PMU_MISC_RC_32K_GATE_POS                        5
#define PMU_MISC_CLOCK_SOFT_RESET_POS                   6
//#define PMU_MISC_RTC_CLOCK_SOFT_RESET_POS               7
#define PMU_MISC_BASEBAND_1_RESET_POS                   8
#define PMU_MISC_BASEBAND_1_APB_RESET_POS               9
#define PMU_MISC_BASEBAND_2_RESET_POS                   10
#define PMU_MISC_RW_32K_SOFT_RSTN_POS                   11
#define PMU_MISC_GPIO_AUTO_LATCH_CTRL_POS               12
#define PMU_MISC_FIRST_RUN_REG_POS                      13
//#define PMU_MISC_REG_PD_RC32K_POS                       15
#define PMU_MISC_GPIO_ANA_EN_POS                        16
#define PMU_MISC_BASEBAND_2_APB_RESET_POS               21
#define PMU_MISC_BASEBAND_3_RESET_POS                   22
#define PMU_MISC_BASEBAND_3_APB_RESET_POS               23
#define PMU_MISC_CLK_64M_SEL_POS                        24
#define PMU_MISC_CLR_PMU_INT_POS                        25
#define PMU_MISC_CLR_PIN_WAKEUP_NOCLK_POS               26
//#define PMU_MISC_RTC_WDT_DIS_EN_POS                     27
//#define PMU_MISC_RTC_WDT_FLAG_EN_POS                    28
#define PMU_MISC_JLINK_ENABLE_POS                       29
//#define PMU_MISC_RTC_APB_SOFT_RESET_POS                 30
//#define PMU_MISC_GP_COMP_INT_LATCH_EN_POS               31
#define PMU_MISC_CLK_32K_SEL_MASK                       0x00000002
#define PMU_MISC_CLK_GATE_MASK                          0x00000008
//#define PMU_MISC_CRY_32K_GATE_MASK                      0x00000010
#define PMU_MISC_RC_32K_GATE_MASK                       0x00000020
#define PMU_MISC_CLOCK_SOFT_RESET_MASK                  0x00000040
//#define PMU_MISC_RTC_CLOCK_SOFT_RESET_MASK              0x00000080
#define PMU_MISC_RW_32K_SOFT_RSTN_MASK                  0x00000800
#define PMU_MISC_GPIO_AUTO_LATCH_CTRL_MASK              0x00001000
#define PMU_MISC_FIRST_RUN_REG_MASK                     0x00002000
//#define PMU_MISC_REG_PD_RC32K_MASK                      0x00008000
#define PMU_MISC_GPIO_ANA_EN_MASK                       0x00FF0000
#define PMU_MISC_BASEBAND_1_RESET_MASK                  0x00000300
#define PMU_MISC_BASEBAND_2_RESET_MASK                  0x00200400
#define PMU_MISC_BASEBAND_3_RESET_MASK                  0x00C00000
#define PMU_MISC_BASEBAND_RESET_MASK                    (PMU_MISC_BASEBAND_1_RESET_MASK|PMU_MISC_BASEBAND_2_RESET_MASK|PMU_MISC_BASEBAND_3_RESET_MASK)
#define PMU_MISC_CLK_64M_SEL_MASK                       0x01000000
#define PMU_MISC_CLR_PMU_INT_MASK                       0x02000000
#define PMU_MISC_CLR_PIN_WAKEUP_NOCLK_MASK              0x04000000
//#define PMU_MISC_RTC_WDT_DIS_EN_MASK                    0x08000000
//#define PMU_MISC_RTC_WDT_FLAG_EN_MASK                   0x10000000
#define PMU_MISC_JLINK_ENABLE_MASK                      0x20000000
//#define PMU_MISC_RTC_APB_SOFT_RESET_MASK                0x40000000
//#define PMU_MISC_GP_COMP_INT_LATCH_EN_MASK              0x80000000
// WAKE_DEB
#define PMU_PIN_DEBOUNCE_CYCLE_WAKE_POS                 0
//#define PMU_PIN_DEBOUNCE_CYCLE_PD_POS                   4
#define PMU_PIN_DEBOUNCE_COEFF_WAKE_POS                 8
//#define PMU_PIN_DEBOUNCE_COEFF_PD_POS                   10
#define PMU_PIN_WAKE_LEVEL_EDGE_SEL_POS                 13
#define PMU_GPIO_INT_DISABLE_POS                        15
#define PMU_PIN_DEB_RST_POS                             16
#define PMU_PIN_DEBOUNCE_CYCLE_WAKE_MASK                0x0000000F
//#define PMU_PIN_DEBOUNCE_CYCLE_PD_MASK                  0x000000F0
#define PMU_PIN_DEBOUNCE_COEFF_WAKE_MASK                0x00000300
//#define PMU_PIN_DEBOUNCE_COEFF_PD_MASK                  0x00001C00
#define PMU_PIN_WAKE_LEVEL_EDGE_SEL_MASK                0x00006000
#define PMU_PIN_WAKE_LEVEL_EDGE_SEL_1ST_MASK            0x00002000
#define PMU_PIN_WAKE_LEVEL_EDGE_SEL_2ND_MASK            0x00006000
#define PMU_GPIO_INT_DISABLE_MASK                       0x00008000
#define PMU_PIN_DEB_RST_MASK                            0x00010000
// GPIO_OE_CTRL
#define PMU_GPIO_OEB_REG_31_0_POS                       0
#define PMU_GPIO_OEB_REG_31_0_MASK                      0xFFFFFFFF
// GPIO_OE_CTRL_1
#define PMU_GPIO_OEB_SEL_POS                            0
#define PMU_GPIO_OEB_SEL_MASK                           0x00000001
// GPIO_PU_CTRL
#define PMU_GPIO_PU_REG_31_0_POS                        0
#define PMU_GPIO_PU_REG_31_0_MASK                       0xFFFFFFFF
// XTAL32M_CNS1
#define PMU_XTAL32M_T5_CFG_POS                          25
#define PMU_XTAL32M_DISTURB_M23_T1_CFG_POS              22
#define PMU_XTAL32M_RESTART_POS                         12
#define PMU_XTAL32M_PKDVREF_POR_POS                     4
#define PMU_XTAL32M_NRB_POR_POS                         0
#define PMU_XTAL32M_T5_CFG_MASK                         0x0E000000
#define PMU_XTAL32M_DISTURB_M23_T1_CFG_MASK             0x01C00000
#define PMU_XTAL32M_RESTART_MASK                        0x00001000
#define PMU_XTAL32M_PKDVREF_POR_MASK                    0x000001F0
#define PMU_XTAL32M_NRB_POR_MASK                        0x00000003
// AHB_REMAP
#define PMU_CHIP_AHB_BUS_REMAP_POS                      0
#define PMU_CPU_SOFT_RESET_POS                          4
#define PMU_CHIP_AHB_BUS_REMAP_EN_POS                   5
#define PMU_WAIT_DEEP_SLEEP_EN_POS                      6
//#define PMU_WDT_RESET_FLAG_POS                          7
//#define PMU_WDT_RESET_POS                               8
#define PMU_MCU_RESET_POS                               9
#define PMU_PSO_RESET_POS                               10
#define PMU_PIN_WAKEUP_POS                              11
#define PMU_PIN_WAKEUP_LATCH_POS                        12
#define PMU_SLEEP_TIMER_INT_POS                         13
#define PMU_BB_INT_POS                                  14
#define PMU_NATIVE_INT_POS                              15
#define PMU_QDEC_INT_POS                                16
#define PMU_CHIP_AHB_BUS_REMAP_MASK                     0x0000000F
#define PMU_CPU_SOFT_RESET_MASK                         0x00000010
#define PMU_CHIP_AHB_BUS_REMAP_EN_MASK                  0x00000020
#define PMU_WAIT_DEEP_SLEEP_EN_MASK                     0x00000040
//#define PMU_WDT_RESET_FLAG_MASK                         0x00000080
//#define PMU_WDT_RESET_MASK                              0x00000100
#define PMU_MCU_RESET_MASK                              0x00000200
#define PMU_PSO_RESET_MASK                              0x00000400
#define PMU_PIN_WAKEUP_MASK                             0x00000800
#define PMU_PIN_WAKEUP_LATCH_MASK                       0x00001000
#define PMU_SLEEP_TIMER_INT_MASK                        0x00002000
#define PMU_BB_INT_MASK                                 0x00004000
#define PMU_NATIVE_INT_MASK                             0x00008000
#define PMU_QDEC_INT_MASK                               0x00010000
// GPIO_ODA_CTRL
#define PMU_GPIO_ODA_CTRL_31_0_POS                      0
#define PMU_GPIO_ODA_CTRL_31_0_MASK                     0xFFFFFFFF
// XTAL32M_CNS2
#define PMU_XTAL32M_POR_WAIT_CTRL_POS                   24
#define PMU_XTAL32M_PKDVREF_THRSHLD_POS                 11
#define PMU_XTAL32M_LAST_WAIT_CFG_POS                   8
#define PMU_XTAL32M_DN_M1T7_M2T1_CFG_POS                5
#define PMU_XTAL32M_DN_MODE_POS                         4
#define PMU_XTAL32M_POR_WAIT_CTRL_MASK                  0xFF000000
#define PMU_XTAL32M_PKDVREF_THRSHLD_MASK                0x0000F800
#define PMU_XTAL32M_LAST_WAIT_CFG_MASK                  0x00000700
#define PMU_XTAL32M_DN_M1T7_M2T1_CFG_MASK               0x000000E0
#define PMU_XTAL32M_DN_MODE_MASK                        0x00000010
// ANA_REG
#ifdef CONFIG_XH98xxD
#define PMU_ANA_LDO_V1P0_ANADIG_1P2_POS                 0       // 00=0.95;01=1.00;10=1.05;11=1.10
#define PMU_ANA_LDO_V1P0_GPADC_1P2_POS                  2       // 00=0.95;01=1.00;10=1.05;11=1.10
#define PMU_ANA_LDO_V1P0_IF_1P2_POS                     4       // 00=0.95;01=1.00;10=1.05;11=1.10
#define PMU_ANA_LDO_V1P0_OSC_1P2_POS                    6       // 00=0.95;01=1.00;10=1.05;11=1.10
#define PMU_ANA_LDO_V1P0_RF_1P2_POS                     8       // 00=0.95;01=1.00;10=1.05;11=1.10
#define PMU_ANA_LDO_V1P0_RXADC_1P2_POS                  10      // 00=0.95;01=1.00;10=1.05;11=1.10
#define PMU_ANA_LDO_V1P0_PKDADC_1P2_POS                 12      // 00=0.95;01=1.00;10=1.05;11=1.10
#define PMU_ANA_LDO_V0P9_DIG_VBAT_POS                   14      // 0000=0.65;0111=1.00;1111=1.40;step=50mv
#define PMU_ANA_LDO_V0P9_RET_VBAT_POS                   18      // 000=0.65;100=0.85;111=1.0 step=50mv
#define PMU_ANA_LDO_V1P0_PA_VBAT_POS                    21      // if(mode_ldo_pa=0) {00=0.95;01=1.00;10=1.05;11=1.10} if(mode_ldo_pa=1) {00=1.45v;01=1.60v,10=1.75v,11=1.90v}
#define PMU_ANA_CTRL_LDO_DIG_UPDATE_POS                 23
#define PMU_ANA_LDO_V1P0_ANADIG_1P2_MASK                0x00000003
#define PMU_ANA_LDO_V1P0_GPADC_1P2_MASK                 0x0000000C
#define PMU_ANA_LDO_V1P0_IF_1P2_MASK                    0x00000030
#define PMU_ANA_LDO_V1P0_OSC_1P2_MASK                   0x000000C0
#define PMU_ANA_LDO_V1P0_RF_1P2_MASK                    0x00000300
#define PMU_ANA_LDO_V1P0_RXADC_1P2_MASK                 0x00000C00
#define PMU_ANA_LDO_V1P0_PKDADC_1P2_MASK                0x00003000
#define PMU_ANA_LDO_V0P9_DIG_VBAT_MASK                  0x0003C000
#define PMU_ANA_LDO_V0P9_RET_VBAT_MASK                  0x001C0000
#define PMU_ANA_LDO_V1P0_PA_VBAT_MASK                   0x00600000
#define PMU_ANA_CTRL_LDO_DIG_UPDATE_MASK                0x00800000
#elif CONFIG_XH98xxF
#define PMU_ANA_LDO_V1P0_ANADIG_1P2_POS                 0       // 00=0.90;01=0.95;10=1.00;11=1.05
#define PMU_ANA_LDO_V1P0_GPADC_1P2_POS                  2       // 00=0.95;01=1.00;10=1.05;11=1.10
#define PMU_ANA_LDO_V1P0_IF_1P2_POS                     4       // 00=0.95;01=1.00;10=1.05;11=1.10
#define PMU_ANA_LDO_V1P0_OSC_1P2_POS                    6       // 00=0.90;01=0.95;10=1.00;11=1.05
#define PMU_ANA_LDO_V1P0_RF_1P2_POS                     8       // 00=0.95;01=1.00;10=1.05;11=1.10
#define PMU_ANA_LDO_V1P0_RXADC_1P2_POS                  10      // 00=0.90;01=0.95;10=1.00;11=1.05
#define PMU_ANA_LDO_V1P0_PKDADC_1P2_POS                 12      // 00=0.95;01=1.00;10=1.05;11=1.10
#define PMU_ANA_LDO_V0P9_DIG_VBAT_POS                   14      // 0000=0.65;0111=1.00;1001=1.1v;1010=1.15v;1111=1.40;step=50mv
#define PMU_ANA_LDO_V0P9_RET_VBAT_POS                   18      // 000=0.65;100=0.85;110=0.95;111=1.0 step=50mv
#define PMU_ANA_LDO_V1P0_PA_VBAT_POS                    21      // 在mode_ldo_pa=0时，0000=0.8;1000=1.0;1111=1.175;在mode_ldo_pa=1时，0000=1.35v;1000=1.55v,1111=1.725；step=25mv"
#define PMU_ANA_CTRL_LDO_DIG_UPDATE_POS                 27
#define PMU_ANA_LDO_V1P0_ANADIG_1P2_MASK                0x00000003
#define PMU_ANA_LDO_V1P0_GPADC_1P2_MASK                 0x0000000C
#define PMU_ANA_LDO_V1P0_IF_1P2_MASK                    0x00000030
#define PMU_ANA_LDO_V1P0_OSC_1P2_MASK                   0x000000C0
#define PMU_ANA_LDO_V1P0_RF_1P2_MASK                    0x00000300
#define PMU_ANA_LDO_V1P0_RXADC_1P2_MASK                 0x00000C00
#define PMU_ANA_LDO_V1P0_PKDADC_1P2_MASK                0x00003000
#define PMU_ANA_LDO_V0P9_DIG_VBAT_MASK                  0x0003C000
#define PMU_ANA_LDO_V0P9_RET_VBAT_MASK                  0x001C0000
#define PMU_ANA_LDO_V1P0_PA_VBAT_MASK                   0x01E00000
#define PMU_ANA_CTRL_LDO_DIG_UPDATE_MASK                0x08000000
#else //if CONFIG_XH98xx
//#define PMU_ANA_LDO_ANADIG_POS                          0       // 00:0.9v, 01:0.95v, 10:1.0v(default), 11:1.05v
//#define PMU_ANA_RETLDO_1P2_POS                          2       // 00:1125mv(default), 01:1175mv, 10:1225mv, 11:1275mv
//#define PMU_ANA_LDO_IF_POS                              4       // 00:0.9v, 01:0.95v, 10:1.0v(default), 11:1.05v
//#define PMU_ANA_LDO_OSC_POS                             6       // 00:0.9v, 01:0.95v, 10:1.0v(default), 11:1.05v
//#define PMU_ANA_LDO_RF_POS                              8       // 00:0.9v, 01:0.95v, 10:1.0v(default), 11:1.05v
//#define PMU_ANA_LDO_RXADC_POS                           10      // 00:0.9v, 01:0.95v, 10:1.0v(default), 11:1.05v
//#define PMU_ANA_LDO_DIG_CURRENT_POS                     12
//#define PMU_ANA_LDO_DIG_POS                             14      // 0000:0.65v, 0111:1v, 1001:1.1v(default), 1111:1.4v, 50mv/step
//#define PMU_ANA_LDO_RET_POS                             18      // 000:725mv, 001: 775mv, 010:825mv, 011:875mv, 100: 925mv(default), 101:975mv, 110:1025mv, 111:1075mv, 50mv/step
//#define PMU_ANA_LDO_PA_POS                              21      // 0000:0.8v, 1000:1v(default), 1111:1.175v, 25mv/step
//#define PMU_ANA_LDO_ANA_POS                             25      // 000:1.1v, 100:1.3v(default), 111: 1.45v, 50mv/step
#define PMU_ANA_RETBG_TEST_POS                          28
#define PMU_ANA_BG_TEST_POS                             29
#define PMU_ANA_CTRL_LDO_DIG_UPDATE_POS                 31
//#define PMU_ANA_LDO_ANADIG_MASK                         0x00000003
//#define PMU_ANA_RETLDO_1P2_MASK                         0x0000000C
//#define PMU_ANA_LDO_IF_MASK                             0x00000030
//#define PMU_ANA_LDO_OSC_MASK                            0x000000C0
//#define PMU_ANA_LDO_RF_MASK                             0x00000300
//#define PMU_ANA_LDO_RXADC_MASK                          0x00000C00
//#define PMU_ANA_LDO_DIG_CURRENT_MASK                    0x00003000
//#define PMU_ANA_LDO_DIG_MASK                            0x0003C000
//#define PMU_ANA_LDO_RET_MASK                            0x001C0000
//#define PMU_ANA_LDO_PA_MASK                             0x01E00000
//#define PMU_ANA_LDO_ANA_MASK                            0x0E000000
#define PMU_ANA_RETBG_TEST_MASK                         0x10000000
#define PMU_ANA_BG_TEST_MASK                            0x20000000
#define PMU_ANA_CTRL_LDO_DIG_UPDATE_MASK                0x80000000
#define PMU_ANA_LDO_V1P0_ANADIG_1P2_POS                 0       // 00=0.90;01=0.95;10=1.00;11=1.05
#define PMU_ANA_LDO_V1P0_GPADC_1P2_POS                  2       // 00=0.95;01=1.00;10=1.05;11=1.10
#define PMU_ANA_LDO_V1P0_IF_1P2_POS                     4       // 00=0.95;01=1.00;10=1.05;11=1.10
#define PMU_ANA_LDO_V1P0_OSC_1P2_POS                    6       // 00=0.90;01=0.95;10=1.00;11=1.05
#define PMU_ANA_LDO_V1P0_RF_1P2_POS                     8       // 00=0.95;01=1.00;10=1.05;11=1.10
#define PMU_ANA_LDO_V1P0_RXADC_1P2_POS                  10      // 00=0.90;01=0.95;10=1.00;11=1.05
#define PMU_ANA_LDO_V1P0_PKDADC_1P2_POS                 12      // 00=0.95;01=1.00;10=1.05;11=1.10
#define PMU_ANA_LDO_V0P9_DIG_VBAT_POS                   14      // 0000=0.65;0111=1.00;1001=1.1v;1010=1.15v;1111=1.40;step=50mv
#define PMU_ANA_LDO_V0P9_RET_VBAT_POS                   18      // 000=0.65;100=0.85;110=0.95;111=1.0 step=50mv
#define PMU_ANA_LDO_V1P0_PA_VBAT_POS                    21      // 在mode_ldo_pa=0时，0000=0.8;1000=1.0;1111=1.175;在mode_ldo_pa=1时，0000=1.35v;1000=1.55v,1111=1.725；step=25mv"
#define PMU_ANA_LDO_V1P2_A_VBAT_POS                     25      // 000:1.1v, 100:1.3v(default), 111: 1.45v, 50mv/step
#define PMU_ANA_LDO_V1P0_ANADIG_1P2_MASK                0x00000003
#define PMU_ANA_LDO_V1P0_GPADC_1P2_MASK                 0x0000000C
#define PMU_ANA_LDO_V1P0_IF_1P2_MASK                    0x00000030
#define PMU_ANA_LDO_V1P0_OSC_1P2_MASK                   0x000000C0
#define PMU_ANA_LDO_V1P0_RF_1P2_MASK                    0x00000300
#define PMU_ANA_LDO_V1P0_RXADC_1P2_MASK                 0x00000C00
#define PMU_ANA_LDO_V1P0_PKDADC_1P2_MASK                0x00003000
#define PMU_ANA_LDO_V0P9_DIG_VBAT_MASK                  0x0003C000
#define PMU_ANA_LDO_V0P9_RET_VBAT_MASK                  0x001C0000
#define PMU_ANA_LDO_V1P0_PA_VBAT_MASK                   0x01E00000
#define PMU_ANA_LDO_V1P2_A_VBAT_MASK                    0x0E000000
#endif
// CLK_CTRL_1
#define PMU_REG_CTUNE_FDOUB_POS                         0
#define PMU_SEL_XTAL32M_NRB_POS                         4
#define PMU_PD_RTC32K_LDOSTARTUP_POS                    7
#define PMU_SEL_XTAL32M_GM_POS                          8
#define PMU_RC_32K_RCTUNE_SEL_POS                       12
#define PMU_CLK_CALI_DONE_POS                           13 // for otp
#define PMU_EN_IREF_BG_POS                              13 // xh98xx new
#define PMU_CTRL_XTAL32M_LDO_POS                        14 // xh98xx new
#define PMU_PD_CKOPMU32K_POS                            16 // xh98xx new
#define PMU_RC32K_SEL_LDO16_POS                         17 // xh98xx new
#define PMU_SEL_OSC32M_FREQRANGE_POS                    18
#define PMU_RCTUNE_RC32K_UPDATE_REG_POS                 19
#define PMU_REG_CTUNE_OSC_POS                           20
#define PMU_CT_OSC32M_RAMP_POS                          28
#define PMU_CTUNE_OSC_SEL_POS                           31
#define PMU_REG_CTUNE_FDOUB_MASK                        0x0000000F
#define PMU_SEL_XTAL32M_NRB_MASK                        0x00000070
#define PMU_PD_RTC32K_LDOSTARTUP_MASK                   0x00000080
#define PMU_SEL_XTAL32M_GM_MASK                         0x00000F00
#define PMU_RC_32K_RCTUNE_SEL_MASK                      0x00001000
#define PMU_CLK_CALI_DONE_MASK                          0x00002000
#define PMU_EN_IREF_BG_MASK                             0x00002000
#define PMU_CTRL_XTAL32M_LDO_MASK                       0x0000C000
#define PMU_PD_CKOPMU32K_MASK                           0x00010000
#define PMU_RC32K_SEL_LDO16_MASK                        0x00020000
#define PMU_SEL_OSC32M_FREQRANGE_MASK                   0x00040000
#define PMU_RCTUNE_RC32K_UPDATE_REG_MASK                0x00080000
#define PMU_REG_CTUNE_OSC_MASK                          0x0FF00000
#define PMU_CT_OSC32M_RAMP_MASK                         0x70000000
#define PMU_CTUNE_OSC_SEL_MASK                          0x80000000
// CLK_CTRL_2
#define PMU_SEL_RTC_IGLOB_POS                           30
#define PMU_REG_CTUNE_XTAL_POS                          20
#define PMU_REG_SEL_DIG_CLK_POS                         15
#define PMU_CTUNE_RC32K_REG_POS                         8
#define PMU_RTUNE_RC32K_REG_POS                         0
#define PMU_SEL_RTC_IGLOB_MASK                          0xC0000000
#define PMU_REG_CTUNE_XTAL_MASK                         0x01F00000
#define PMU_REG_SEL_DIG_CLK_MASK                        0x00008000
#define PMU_CTUNE_RC32K_REG_MASK                        0x00007F00
#define PMU_RTUNE_RC32K_REG_MASK                        0x000000FF
// GPIO_IE_CTRL
#ifdef CONFIG_XH98xx
#define PMU_GPIO_IE_CTRL_POS                            0
#define PMU_GPIO_SF_CTRL_POS                            23
#define PMU_GPIO_IE_CTRL_MASK                           0x007FFFFF
#define PMU_GPIO_SF_CTRL_MASK                           0x1F880000
#else
#define PMU_GPIO_IE_CTRL_POS                            0
#define PMU_GPIO_SF_CTRL_POS                            19
#define PMU_GPIO_IE_CTRL_MASK                           0x0007FFFF
#define PMU_GPIO_SF_CTRL_MASK                           0x01F80000
#endif
// COMP_CTRL
#define PMU_REF_GP_COMP1_POS                            0
#define PMU_SIG_GP_COMP1_POS                            3
#define PMU_PD_GP_COMP_POS                              6
#define PMU_VCM_GP_COMP2_POS                            8
#define PMU_SIG_GP_COMP2_POS                            11
#define PMU_GP_COMP_POL_SEL_POS                         14
#define PMU_GP_COMP_OUT_POS                             16
#define PMU_GP_COMP_INT_LATCH_POS                       18
#define PMU_REF_GP_COMP1_MASK                           0x00000007
#define PMU_SIG_GP_COMP1_MASK                           0x00000038
#define PMU_PD_GP_COMP_MASK                             0x000000C0
#define PMU_VCM_GP_COMP2_MASK                           0x00000700
#define PMU_SIG_GP_COMP2_MASK                           0x00003800
#define PMU_GP_COMP_POL_SEL_MASK                        0x0000C000
#define PMU_GP_COMP_OUT_MASK                            0x00030000
#define PMU_GP_COMP_INT_LATCH_MASK                      0x00040000
// ANA_PD_1
#if defined(CONFIG_XH98xxD) || defined(CONFIG_XH98xxF)
//#define PMU_DCDC_SLOPE_POS                              0
//#define PMU_DCDC_VOUT_POS                               4
//#define PMU_DCDC_MODE_POS                               7
//#define PMU_DCDC_OSC_TEST_POS                           8
//#define PMU_DCDC_SHORT_POS                              9
#define PMU_EN_RC32K_TST_POS                            10
//#define PMU_RW_PWR_HW_CTRL_POS                          11
#define PMU_PD_XTAL32M_DIGBUF_POS                       12
#define PMU_PD_XTAL32M_DIGBUF_SEL_POS                   13
#define PMU_PD_XTAL32M_PLLBUF_POS                       14
#define PMU_PD_XTAL32M_PLLBUF_SEL_POS                   15
//#define PMU_DCDC_SLOPE_MASK                             0x0000000F
//#define PMU_DCDC_VOUT_MASK                              0x00000070
//#define PMU_DCDC_MODE_MASK                              0x00000080
//#define PMU_DCDC_OSC_TEST_MASK                          0x00000100
//#define PMU_DCDC_SHORT_MASK                             0x00000200
#define PMU_EN_RC32K_TST_MASK                           0x00000400
//#define PMU_RW_PWR_HW_CTRL_MASK                         0x00000800
#define PMU_PD_XTAL32M_DIGBUF_MASK                      0x00001000
#define PMU_PD_XTAL32M_DIGBUF_SEL_MASK                  0x00002000
#define PMU_PD_XTAL32M_PLLBUF_MASK                      0x00004000
#define PMU_PD_XTAL32M_PLLBUF_SEL_MASK                  0x00008000
#else
#define PMU_RTUNE1_OSC32K_POS                           16
#define PMU_PD_XTAL32M_PLLBUF_SEL_POS                   15
#define PMU_PD_XTAL32M_PLLBUF_POS                       14
#define PMU_PD_XTAL32M_DIGBUF_SEL_POS                   13
#define PMU_PD_XTAL32M_DIGBUF_POS                       12
#define PMU_SEL_RTC32K_ATB_POS                          10
#define PMU_EN_RC32K_TST_POS                            9
#define PMU_PD_LDO_RET1P2_POS                           8
#define PMU_PD_PSM_DCDC_POS                             7
#define PMU_DCDC_PSM_TRIM_POS                           6
#define PMU_DCDC_ZCS_TRIM_POS                           5
#define PMU_DCDC_VOUT_POS                               0
#define PMU_RTUNE1_OSC32K_MASK                          0x00FF0000
#define PMU_PD_XTAL32M_PLLBUF_SEL_MASK                  0x00008000
#define PMU_PD_XTAL32M_PLLBUF_MASK                      0x00004000
#define PMU_PD_XTAL32M_DIGBUF_SEL_MASK                  0x00002000
#define PMU_PD_XTAL32M_DIGBUF_MASK                      0x00001000
#define PMU_SEL_RTC32K_ATB_MASK                         0x00000C00
#define PMU_EN_RC32K_TST_MASK                           0x00000200
#define PMU_PD_LDO_RET1P2_MASK                          0x00000100
#define PMU_PD_PSM_DCDC_MASK                            0x00000080
#define PMU_DCDC_PSM_TRIM_MASK                          0x00000040
#define PMU_DCDC_ZCS_TRIM_MASK                          0x00000020
#define PMU_DCDC_VOUT_MASK                              0x0000001F
#endif
// MISC_CTRL_1
#define PMU_MISC_DEEP_SLEEP_CM3_INT_DIS_POS             0
#define PMU_MISC_WDT_RESET_REMAP_DIS_POS                1
#define PMU_MISC_WDT_RESET_PSO_DIS_POS                  2
#define PMU_MISC_WDT_RESET_ALL_DIS_POS                  3
#define PMU_MISC_CRY32M_KEEP_ON_POS                     4
#define PMU_MISC_GPIO_LATCH_CLK_EN_POS                  5
//#define PMU_MISC_CRY32K_RDY_INT_EN_POS                  6
#define PMU_MISC_XTAL32M_BUF_CTRL_SEL_POS               7
//#define PMU_MISC_CRY_32K_READY_GEN_CTRL_POS             8
#define PMU_MISC_FLASH_UV_SEL_POS                       8
#define PMU_MISC_REG_PD_CRY32M_POS                      12
#define PMU_MISC_REG_PD_RC32M_POS                       13
//#define PMU_MISC_QDEC_CLK_EN_POS                        14
//#define PMU_MISC_QDEC_CLK_SEL_POS                       15
//#define PMU_MISC_QDEC_SOFT_RESET_POS                    16
//#define PMU_MISC_QDEC_APB_SOFT_RESET_POS                17
#define PMU_MISC_CRY32M_EN_POS                          18
#define PMU_MISC_OSC_CLK1M_GATE_POS                     19
#define PMU_MISC_RC32K_PSO_GATE_POS                     20
#define PMU_MISC_CPU_JTAG_TEST_EN_POS                   22
#define PMU_MISC_CPU_JTAG_ICG_EN_POS                    23
#define PMU_MISC_MODE_DIGLDO_POS                        24
#define PMU_MISC_TIMER_INT_CPU_EN_POS                   25
#define PMU_MISC_TIMER_EN_POS                           26
#define PMU_MISC_TIMER_CNT_LOAD_POS                     27
#define PMU_MISC_TIMER_INT_CLR_POS                      28
#define PMU_MISC_DEEP_SLEEP_CM3_INT_DIS_MASK            0x00000001
#define PMU_MISC_WDT_RESET_REMAP_DIS_MASK               0x00000002
#define PMU_MISC_WDT_RESET_PSO_DIS_MASK                 0x00000004
#define PMU_MISC_WDT_RESET_ALL_DIS_MASK                 0x00000008
#define PMU_MISC_CRY32M_KEEP_ON_MASK                    0x00000010
#define PMU_MISC_GPIO_LATCH_CLK_EN_MASK                 0x00000020
//#define PMU_MISC_CRY32K_RDY_INT_EN_MASK                 0x00000040
#define PMU_MISC_XTAL32M_BUF_CTRL_SEL_MASK              0x00000080
//#define PMU_MISC_CRY_32K_READY_GEN_CTRL_MASK            0x00000F00
#define PMU_MISC_FLASH_UV_SEL_MASK                      0x00000F00
#define PMU_MISC_REG_PD_CRY32M_MASK                     0x00001000
#define PMU_MISC_REG_PD_RC32M_MASK                      0x00002000
//#define PMU_MISC_QDEC_CLK_EN_MASK                       0x00004000
//#define PMU_MISC_QDEC_CLK_SEL_MASK                      0x00008000
//#define PMU_MISC_QDEC_SOFT_RESET_MASK                   0x00010000
//#define PMU_MISC_QDEC_APB_SOFT_RESET_MASK               0x00020000
#define PMU_MISC_CRY32M_EN_MASK                         0x00040000
#define PMU_MISC_OSC_CLK1M_GATE_MASK                    0x00080000
#define PMU_MISC_RC32K_PSO_GATE_MASK                    0x00100000
#define PMU_MISC_CPU_JTAG_TEST_EN_MASK                  0x00400000
#define PMU_MISC_CPU_JTAG_ICG_EN_MASK                   0x00800000
#define PMU_MISC_MODE_DIGLDO_MASK                       0x01000000
#define PMU_MISC_TIMER_INT_CPU_EN_MASK                  0x02000000
#define PMU_MISC_TIMER_EN_MASK                          0x04000000
#define PMU_MISC_TIMER_CNT_LOAD_MASK                    0x08000000
#define PMU_MISC_TIMER_INT_CLR_MASK                     0x10000000
// RAM_CTRL_1
// RAM_CTRL_2
#define PMU_RAM_DS_HW_CTRL_EN_POS                       14
#define PMU_RAM_DS_HW_CTRL_EN_MASK                      0x00004000
// RAM_CTRL_3
#define PMU_RAM_RW_LS_POS                               0
#define PMU_RAM_RW_DS_POS                               1
#define PMU_RAM_RW_SD_POS                               2
#define PMU_RAM_RW_LS_MASK                              0x00000001
#define PMU_RAM_RW_DS_MASK                              0x00000002
#define PMU_RAM_RW_SD_MASK                              0x00000004
// RAM_CTRL_4
// RAM_CTRL_5
// RW_EXT_WAKEUP_CON
#define PMU_RW_EXT_WAKEUP_POS                           0
#define PMU_RW_EXT_WAKEUP_MASK                          0x00000001
// OSC_INT_CTRL
#define PMU_RW_WAKEUP_INT_CLR_POS                       0
#define PMU_RW_WAKEUP_INT_MASK_PMU_POS                  4
#define PMU_RW_WAKEUP_INT_MASK_CPU_POS                  5
#define PMU_RW_WAKEUP_INT_CLR_MASK                      0x00000001
#define PMU_RW_WAKEUP_INT_MASK_PMU_MASK                 0x00000010
#define PMU_RW_WAKEUP_INT_MASK_CPU_MASK                 0x00000020
// GPIO_STATUS_READ
#define PMU_GPIO_OEB_AUTO_LATCH_31_0_POS                0
#define PMU_GPIO_OEB_AUTO_LATCH_31_0_MASK               0xFFFFFFFF
// GPIO_STATUS_READ_2
#define PMU_GPIO_OUT_AUTO_LATCH_31_0_POS                0
#define PMU_GPIO_OUT_AUTO_LATCH_31_0_MASK               0xFFFFFFFF
// WDT_STATUS
#define PMU_WDT_LD_KR_POS                               0
#define PMU_WDT_FLAG_POS                                4
#define PMU_WDT_TIMER_POS                               16
#define PMU_WDT_LD_KR_MASK                              0x00000001
#define PMU_WDT_FLAG_MASK                               0x00000010
#define PMU_WDT_TIMER_MASK                              0x07ff0000
// GPIO_WAKEUP_1
#define PMU_GPIO_WAKEUP_POWERDOWN_POS                   0
#define PMU_GPIO_WAKEUP_POWERDOWN_MASK                  0x00000001
// GPIO_ODE_CTRL
#define PMU_GPIO_ODE_REG_31_0_POS                       0
#define PMU_GPIO_ODE_REG_31_0_MASK                      0xFFFFFFFF
// GPIO_ODE_CTRL_1
#define PMU_GPIO_PMU_DBG_POS                            8
#define PMU_GPIO_ODE_REG_39_32_POS                      0
#define PMU_GPIO_PMU_DBG_MASK                           0x00000100
#define PMU_GPIO_ODE_REG_39_32_MASK                     0x000000FF
// GPIO_PD_CTRL
#define PMU_GPIO_PD_REG_31_0_POS                        0
#define PMU_GPIO_PD_REG_31_0_MASK                       0xFFFFFFFF
// GPIO_LATCH
#define PMU_GPIO_INT_LATCH_31_0_POS                     0
#define PMU_GPIO_INT_LATCH_31_0_MASK                    0xFFFFFFFF
// GPIO_LATCH_1
#define PMU_GPIO_INT_LATCH_42_32_POS                    0
#define PMU_GPIO_INT_LATCH_42_32_MASK                   0x000007FF
// GPIO_NOCLK_LATCH
#define PMU_GPIO_INT_NOCLK_LATCH_31_0_POS               0
#define PMU_GPIO_INT_NOCLK_LATCH_31_0_MASK              0xFFFFFFFF
// STATUS_READ
#define PMU_TIMER_CNT_LOAD_STATUS_POS                   26
#define PMU_SEL_RC32K_TUNE_POS                          22
#define PMU_RCTUNE_RC32K_UPDATE_STATUS_POS              21
#define PMU_CON_SWITCH_FLASH_FSM_POS                    18
#define PMU_RC32M_READY_POS                             16
#define PMU_RC32K_READY_POS                             15
#define PMU_PIN_WAKEUP_ORG_SYNC_POS                     14
#define PMU_PIN_WAKEUP_DIGITAL_POS                      13
#define PMU_TIMER_INT_CLR_STATE_POS                     12
#define PMU_NATIVE_INT_32K_POS                          11
#define PMU_FIRST_RUN_POS                               10
#define PMU_TIMER_INT_POS                               9
#define PMU_CLK_32K_DIV_DONE_POS                        8
#define PMU_CTRL_LDO_DIG_POS                            4
#define PMU_CLK_32K_RC_DIN_STATUS_POS                   2
#define PMU_CLR_PMU_INT_SYNC_APB_POS                    1
#define PMU_APB_BUS_UPD_DATA_POS                        0
#define PMU_TIMER_CNT_LOAD_STATUS_MASK                  0x04000000
#define PMU_SEL_RC32K_TUNE_MASK                         0x00400000
#define PMU_RCTUNE_RC32K_UPDATE_STATUS_MASK             0x00200000
#define PMU_CON_SWITCH_FLASH_FSM_MASK                   0x001C0000
#define PMU_RC32M_READY_MASK                            0x00010000
#define PMU_RC32K_READY_MASK                            0x00008000
#define PMU_PIN_WAKEUP_ORG_SYNC_MASK                    0x00004000
#define PMU_PIN_WAKEUP_DIGITAL_MASK                     0x00002000
#define PMU_TIMER_INT_CLR_STATE_MASK                    0x00001000
#define PMU_NATIVE_INT_32K_MASK                         0x00000800
#define PMU_FIRST_RUN_MASK                              0x00000400
#define PMU_TIMER_INT_MASK                              0x00000200
#define PMU_CLK_32K_DIV_DONE_MASK                       0x00000100
#define PMU_CTRL_LDO_DIG_MASK                           0x000000F0
#define PMU_CLK_32K_RC_DIN_STATUS_MASK                  0x00000004
#define PMU_CLR_PMU_INT_SYNC_APB_MASK                   0x00000002
#define PMU_APB_BUS_UPD_DATA_MASK                       0x00000001
// FLASH_LOW_VOL_CTRL_0
#define PMU_FLASH_1_POWER_READY_DIS_POS                 11
#define PMU_FLASH_0_POWER_READY_DIS_POS                 10
#define PMU_FLASH_LOW_VOL_FLAG_CLR_POS                  9
#define PMU_OTP_LOW_VOL_FLAG_CLR_POS                    8
#define PMU_FLASH_LOW_VOL_FLAG_POS                      7
#define PMU_OTP_LOW_VOL_FLAG_POS                        6
#define PMU_FLASH_POWER_ON_FLAG_POS                     5
#define PMU_OTP_POWER_ON_FLAG_POS                       4
#define PMU_FLASH_POWER_READY_SYNC_POS                  3
#define PMU_OTP_POWER_READY_SYNC_POS                    2
#define PMU_FLASH_LOW_VOL_RSTB_DIS_POS                  1
#define PMU_OTP_LOW_VOL_RSTB_DIS_POS                    0
#define PMU_FLASH_1_POWER_READY_DIS_MASK                0x00000800
#define PMU_FLASH_0_POWER_READY_DIS_MASK                0x00000400
#define PMU_FLASH_LOW_VOL_FLAG_CLR_MASK                 0x00000200
#define PMU_OTP_LOW_VOL_FLAG_CLR_MASK                   0x00000100
#define PMU_FLASH_LOW_VOL_FLAG_MASK                     0x00000080
#define PMU_OTP_LOW_VOL_FLAG_MASK                       0x00000040
#define PMU_FLASH_POWER_ON_FLAG_MASK                    0x00000020
#define PMU_OTP_POWER_ON_FLAG_MASK                      0x00000010
#define PMU_FLASH_POWER_READY_SYNC_MASK                 0x00000008
#define PMU_OTP_POWER_READY_SYNC_MASK                   0x00000004
#define PMU_FLASH_LOW_VOL_RSTB_DIS_MASK                 0x00000002
#define PMU_OTP_LOW_VOL_RSTB_DIS_MASK                   0x00000001
// SW_STATUS
#define PMU_SW_STATUS_SYS_CALIBED_MASK              0x00000001
#define PMU_SW_STATUS_RF_CALIBED_MASK               0x00000002
#define PMU_SW_STATUS_APP_RUNNED_MASK               0x00000004
#define PMU_SW_STATUS_REBOOT_FORCE_MASK             0x00000008
#define PMU_SW_STATUS_REBOOT_FROM_OTA_ISP_MASK      0x00000010
#define PMU_SW_STATUS_REBOOT_FROM_SLEEP_MASK        0x00000020
#define PMU_SW_STATUS_REBOOT_SW_MASK                (PMU_SW_STATUS_REBOOT_FORCE_MASK | PMU_SW_STATUS_REBOOT_FROM_OTA_ISP_MASK | PMU_SW_STATUS_REBOOT_FROM_SLEEP_MASK)
#define PMU_SW_STATUS_REBOOT_FROM_WDT_MASK          0x00000040
#define PMU_SW_STATUS_REBOOT_FROM_SOFT_RESET_MASK   0x00000080
#define PMU_SW_STATUS_REBOOT_FROM_IFLASH_LOW_V_MASK 0x00000100
#define PMU_SW_STATUS_REBOOT_HW_MASK                (PMU_SW_STATUS_REBOOT_FROM_WDT_MASK | PMU_SW_STATUS_REBOOT_FROM_SOFT_RESET_MASK | PMU_SW_STATUS_REBOOT_FROM_IFLASH_LOW_V_MASK)
#define PMU_SW_STATUS_REBOOT_MASK                   (PMU_SW_STATUS_REBOOT_SW_MASK | PMU_SW_STATUS_REBOOT_HW_MASK)
//#define PMU_SW_STATUS_XTAL32M_OPENED_MASK           0x00000100
#define PMU_SW_STATUS_AGGRESSIVE_CPUGATE_MASK       0x00000200
#define PMU_SW_STATUS_XTAL32M_ABSENT_MASK           0x00000400
#define PMU_SW_STATUS_DCDC_ENABLED_MASK             0x00000800
#define PMU_SW_STATUS_FLASH_OPEND_MASK              0x00001000
#define PMU_SW_STATUS_OTP_OPEND_MASK                0x00002000
#define PMU_SW_STATUS_SCBOOT_IN_FLASH_MASK          0x00004000
#define PMU_SW_STATUS_USER_RETENTION_POS            16
#define PMU_SW_STATUS_USER_RETENTION_MASK           0xFFFF0000


/******************************************************************************/
/*                                                                            */
/*                                   SYS                                      */
/*                                                                            */
/******************************************************************************/
// REV_ID
#define SYS_REV_ANA_POS                             0
#define SYS_REV_SOFT_POS                            4
#define SYS_REV_CHIP_POS                            8
#define SYS_REV_FPGA_FLAG_POS                       0
#define SYS_REV_OTP_CHECK_POS                       15
#define SYS_REV_ANA_MASK                            0x0000000F
#define SYS_REV_SOFT_MASK                           0x000000F0
#define SYS_REV_CHIP_MASK                           0x00000F00
#define SYS_REV_FPGA_FLAG_MASK                      0x00000FFF
#define SYS_REV_BONDING_OTP_MASK                    0x00008000
#define SYS_REV_SOFT_MAIN_VERSION                   0
#define SYS_REV_SOFT_RAM_VERSION                    1
#define SYS_REV_SOFT_SFLASH_VERSION                 2
#define SYS_REV_FPGA_FLAG                           0x555
// GPIO_POWER_UP_STATUS
#define SYS_GPIO_STAUS_POWERDOWN_MASK               0x80000000
// SOFT_INT
#define SYS_SOFT_INI_0_MASK                         0x00000001
#define SYS_SOFT_INI_1_MASK                         0x00000002
#define SYS_SOFT_INI_2_MASK                         0x00000004
#define SYS_SOFT_INI_3_MASK                         0x00000008
#define SYS_SOFT_INI_4_MASK                         0x00000010
#define SYS_SOFT_INI_5_MASK                         0x00000020
#define SYS_SOFT_INI_6_MASK                         0x00000040
#define SYS_SOFT_INI_7_MASK                         0x00000080
// SYS_TICK
#define SYS_CPU_SYS_TICK_CON_POS                    0
#define SYS_CPU_SYS_TICK_UPD_POS                    26
#define SYS_CPU_SYS_TICK_CON_MASK                   0x03FFFFFF
#define SYS_CPU_SYS_TICK_UPD_MASK                   0x04000000
// RST_32K_OSC_CTRL
#define SYS_CRY32M_DIV_EN_POS                       12
#define SYS_CLK_32K_DIV_RST_TRIG_POS                11
#define SYS_RST_32K_RC_TRIG_POS                     10
#define SYS_RST_32K_RC_TIME_POS                     0
#define SYS_CRY32M_DIV_EN_MASK                      0x00001000
#define SYS_CLK_32K_DIV_RST_TRIG_MASK               0x00000800
#define SYS_RST_32K_RC_TRIG_MASK                    0x00000400
#define SYS_RST_32K_RC_TIME_MASK                    0x000003FF
// MON
#define SYS_MON_CPM(n)                              (0x0000 + n)
#define SYS_MON_TIMER0(n)                           (0x0100 + n)
#define SYS_MON_TIMER1(n)                           (0x0200 + n)
#define SYS_MON_TIMER2(n)                           (0x0300 + n)
#define SYS_MON_SFLASH(n)                           (0x0400 + n)
#define SYS_MON_KPP(n)                              (0x0500 + n)
#define SYS_MON_SPI0(n)                             (0x0600 + n)
#define SYS_MON_SPI1(n)                             (0x0700 + n)
#define SYS_MON_WATCHDOG(n)                         (0x0800 + n)
#define SYS_MON_APB(n)                              (0x0900 + n)
#define SYS_MON_AHB(n)                              (0x0A00 + n)
#define SYS_MON_UART0(n)                            (0x0B00 + n)
#define SYS_MON_UART1(n)                            (0x0C00 + n)
#define SYS_MON_BASEBAND(n)                         (0x0D00 + n)
#define SYS_MON_PHY(n)                              (0x0E00 + n)
//#define SYS_MON_PHY(n)                              (0x0F00 + n)
#define SYS_MON_DAIF(n)                             (0x1000 + n)
#define SYS_MON_PMU(n)                              (0x1100 + n)
#define SYS_MON_QDEC(n)                             (0x1200 + n)
#define SYS_MON_RNG(n)                              (0x1300 + n)
// Simulation
#define SYS_SIMULATION_FLAG                         (*(__IO uint32_t *)0x40000c0c)
// CHRGR_STAT
#define SYS_CHRGR_FINISH_MASK                       0x00000001
#define SYS_CHRGR_CHARGING_LARGE0_TRICKLE1_MASK     0x00000002
#define SYS_CHRGR_INSERT_DETECT_MASK                0x00000004
// PINMUX
#define SYS_PINMUX_MASK(f,p)                        (0x7F<<(p)),((f)<<(p))
// PINMUX0
#define SYS_PINMUX_GPIO_DIN_HOLD_SEL_MASK           0x00000080
#define SYS_PINMUX_SYSPLL_GT_CPUCLK_HW_CTRL_MASK    0x00800000

// LPO_TRIM
#define LCBB_LPO_TRIM_POS                           16
#define LCBB_LPO_TRIM_MASK                          0x00010000

// PD_CFG0
#define DAIF_PD_CONSTGM_RX_MO_POS               1
#define DAIF_PD_LDO_TRS_MO_POS                  2
#define DAIF_PD_LNA_MO_POS                      3
#define DAIF_PD_LDO_RF_MO_POS                   4
#define DAIF_PD_AGCAMP_MO_POS                   5
#define DAIF_PD_TXCALI_MO_POS                   6
#define DAIF_PD_PDTIF_MO_POS                    7
#define DAIF_PKDADC_PD_MO_POS                   8
#define DAIF_PD_MIX_MO_POS                      9
#define DAIF_PD_TIA_MO_POS                      10
#define DAIF_PD_FILTER_MO_POS                   11
#define DAIF_RXADC_PD_MO_POS                    12
#define DAIF_PD_LDO_RXADC_MO_POS                13
#define DAIF_PD_PA_MO_POS                       14
#define DAIF_PD_LDO_PA_MO_POS                   15
#define DAIF_PD_CONSTGM_RX_ME_POS               17
#define DAIF_PD_LDO_TRS_ME_POS                  18
#define DAIF_PD_LNA_ME_POS                      19
#define DAIF_PD_LDO_RF_ME_POS                   20
#define DAIF_PD_AGCAMP_ME_POS                   21
#define DAIF_PD_TXCALI_ME_POS                   22
#define DAIF_PD_PDTIF_ME_POS                    23
#define DAIF_PKDADC_PD_ME_POS                   24
#define DAIF_PD_MIX_ME_POS                      25
#define DAIF_PD_TIA_ME_POS                      26
#define DAIF_PD_FILTER_ME_POS                   27
#define DAIF_RXADC_PD_ME_POS                    28
#define DAIF_PD_LDO_RXADC_ME_POS                29
#define DAIF_PD_PA_ME_POS                       30
#define DAIF_PD_LDO_PA_ME_POS                   31
#define DAIF_PD_CONSTGM_RX_MO_MASK              0x00000002
#define DAIF_PD_LDO_TRS_MO_MASK                 0x00000004
#define DAIF_PD_LNA_MO_MASK                     0x00000008
#define DAIF_PD_LDO_RF_MO_MASK                  0x00000010
#define DAIF_PD_AGCAMP_MO_MASK                  0x00000020
#define DAIF_PD_TXCALI_MO_MASK                  0x00000040
#define DAIF_PD_PDTIF_MO_MASK                   0x00000080
#define DAIF_PKDADC_PD_MO_MASK                  0x00000100
#define DAIF_PD_MIX_MO_MASK                     0x00000200
#define DAIF_PD_TIA_MO_MASK                     0x00000400
#define DAIF_PD_FILTER_MO_MASK                  0x00000800
#define DAIF_RXADC_PD_MO_MASK                   0x00001000
#define DAIF_PD_LDO_RXADC_MO_MASK               0x00002000
#define DAIF_PD_PA_MO_MASK                      0x00004000
#define DAIF_PD_LDO_PA_MO_MASK                  0x00008000
#define DAIF_PD_CONSTGM_RX_ME_MASK              0x00020000
#define DAIF_PD_LDO_TRS_ME_MASK                 0x00040000
#define DAIF_PD_LNA_ME_MASK                     0x00080000
#define DAIF_PD_LDO_RF_ME_MASK                  0x00100000
#define DAIF_PD_AGCAMP_ME_MASK                  0x00200000
#define DAIF_PD_TXCALI_ME_MASK                  0x00400000
#define DAIF_PD_PDTIF_ME_MASK                   0x00800000
#define DAIF_PKDADC_PD_ME_MASK                  0x01000000
#define DAIF_PD_MIX_ME_MASK                     0x02000000
#define DAIF_PD_TIA_ME_MASK                     0x04000000
#define DAIF_PD_FILTER_ME_MASK                  0x08000000
#define DAIF_RXADC_PD_ME_MASK                   0x10000000
#define DAIF_PD_LDO_RXADC_ME_MASK               0x20000000
#define DAIF_PD_PA_ME_MASK                      0x40000000
#define DAIF_PD_LDO_PA_ME_MASK                  0x80000000
// PD_CFG1
#define DAIF_PD_PLL_ALL_MO_POS                  0
#define DAIF_PD_LDO_IF_MO_POS                   1
#define DAIF_PD_LDO_ANADIG_MO_POS               2
#define DAIF_PD_LOTX_MO_POS                     3
#define DAIF_PD_VCDET_MO_POS                    4
#define DAIF_PD_LDO_PKDADC_MO_POS               5 // xh98xx remove
#define DAIF_PKDADC_RST_MO_POS                  6
#define DAIF_RST_PDTIF_MO_POS                   7
#define DAIF_PD_TOPLDO_VCO_MO_POS               8 // xh98xx new
#define DAIF_PD_PLL_ALL_ME_POS                  16
#define DAIF_PD_LDO_IF_ME_POS                   17
#define DAIF_PD_LDO_ANADIG_ME_POS               18
#define DAIF_PD_LOTX_ME_POS                     19
#define DAIF_PD_VCDET_ME_POS                    20
#define DAIF_PD_LDO_PKDADC_ME_POS               21 // xh98xx remove
#define DAIF_PKDADC_RST_ME_POS                  22
#define DAIF_RST_PDTIF_ME_POS                   23
#define DAIF_PD_TOPLDO_VCO_ME_POS               24 // xh98xx new
#define DAIF_PD_PLL_ALL_MO_MASK                 0x00000001
#define DAIF_PD_LDO_IF_MO_MASK                  0x00000002
#define DAIF_PD_LDO_ANADIG_MO_MASK              0x00000004
#define DAIF_PD_LOTX_MO_MASK                    0x00000008
#define DAIF_PD_VCDET_MO_MASK                   0x00000010
#define DAIF_PD_LDO_PKDADC_MO_MASK              0x00000020
#define DAIF_PKDADC_RST_MO_MASK                 0x00000040
#define DAIF_RST_PDTIF_MO_MASK                  0x00000080
#define DAIF_PD_TOPLDO_VCO_MO_MASK              0x00000100
#define DAIF_PD_PLL_ALL_ME_MASK                 0x00010000
#define DAIF_PD_LDO_IF_ME_MASK                  0x00020000
#define DAIF_PD_LDO_ANADIG_ME_MASK              0x00040000
#define DAIF_PD_LOTX_ME_MASK                    0x00080000
#define DAIF_PD_VCDET_ME_MASK                   0x00100000
#define DAIF_PD_LDO_PKDADC_ME_MASK              0x00200000
#define DAIF_PKDADC_RST_ME_MASK                 0x00400000
#define DAIF_RST_PDTIF_ME_MASK                  0x00800000
#define DAIF_PD_TOPLDO_VCO_ME_MASK              0x01000000
// PD_CFG2
#define DAIF_PD_PLL_CP_MO_POS                   0
#define DAIF_PD_TXDAC_PLL_ME_POS                0 // xh98xx new
#define DAIF_PD_PLL_PFD_MO_POS                  1
#define DAIF_PD_PLL_MMD_MO_POS                  2
#define DAIF_PD_TXDAC_PLL_MO_POS                2 // xh98xx new
#define DAIF_PD_PLL_BUF_MO_POS                  3
#define DAIF_PD_DACVREF_POS                     4 // xh98xx new
#define DAIF_PD_PLL_VCO_MO_POS                  5
#define DAIF_PD_PKDET_VCO_MO_POS                7
#define DAIF_PD_PKDET_VCO_ME_POS                8
#define DAIF_PD_PPG_TX_POS                      12
#define DAIF_PD_PPG_RX_POS                      13
#define DAIF_PD_PPG_BIAS_POS                    14
#define DAIF_PPG_TX_RST_POS                     15
#define DAIF_PD_PLL_BUF_ME_POS                  19
#define DAIF_PD_PLL_CP_MO_MASK                  0x00000001
#define DAIF_PD_TXDAC_PLL_ME_MASK               0x00000001
#define DAIF_PD_PLL_PFD_MO_MASK                 0x00000002
#define DAIF_PD_PLL_MMD_MO_MASK                 0x00000004
#define DAIF_PD_TXDAC_PLL_MO_MASK               0x00000004
#define DAIF_PD_PLL_BUF_MO_MASK                 0x00000008
#define DAIF_PD_DACVREF_MASK                    0x00000010
#define DAIF_PD_PLL_VCO_MO_MASK                 0x00000020
#define DAIF_PD_PKDET_VCO_MO_MASK               0x00000080
#define DAIF_PD_PKDET_VCO_ME_MASK               0x00000100
#define DAIF_PD_PPG_TX_MASK                     0x00001000
#define DAIF_PD_PPG_RX_MASK                     0x00002000
#define DAIF_PD_PPG_BIAS_MASK                   0x00004000
#define DAIF_PPG_TX_RST_MASK                    0x00008000
#define DAIF_PD_PLL_BUF_ME_MASK                 0x00080000
// PA_CNS
#define DAIF_EN_BYPASS_PALDO_POS                19 // xh98xx new
#define DAIF_PA_1V_CTRL_POS                     18
#define DAIF_PADR1S_CT_MO_POS                   15
#define DAIF_PADR2S_PCT_MO_POS                  11
#define DAIF_PADR2S_NCT_MO_POS                  7
#define DAIF_PACORE_CT_MO_POS                   3
#define DAIF_TX_ATTEN_MO_POS                    0
#define DAIF_PA_DBG_POS                         20
#define DAIF_PA_GAIN_IDX_SEL_POS                21
#define DAIF_PA_GAIN_IDX_REG_POS                24
#define DAIF_MODE_LDO_PA_POS                    30
#define DAIF_TXCALI_OUT_SYNC_POS                31
#define DAIF_EN_BYPASS_PALDO_MASK               0x00080000
#define DAIF_PA_1V_CTRL_MASK                    0x00040000
#define DAIF_PADR1S_CT_MO_MASK                  0x00038000
#define DAIF_PADR2S_PCT_MO_MASK                 0x00007800
#define DAIF_PADR2S_NCT_MO_MASK                 0x00000780
#define DAIF_PACORE_CT_MO_MASK                  0x00000078
#define DAIF_TX_ATTEN_MO_MASK                   0x00000003
#define DAIF_PA_DBG_MASK                        0x00100000
#define DAIF_PA_GAIN_IDX_SEL_MASK               0x00200000
#define DAIF_PA_GAIN_IDX_REG_MASK               0x3F000000
#define DAIF_MODE_LDO_PA_MASK                   0x40000000
#define DAIF_TXCALI_OUT_SYNC_MASK               0x80000000
// RC32K_TUN
#define DAIF_RC_32K_TUNE_START_POS              0
#define DAIF_RC_32K_TUNE_REF_CLK_SEL_POS        1
#define DAIF_RST_RCOSC_32K_POS                  2 //xh98xx new
#define DAIF_RC_32K_TUNE_SWRSTN_POS             3
#define DAIF_RC_32K_TUNE_WIN_CNT_THRESHOLD_POS  4
#define DAIF_RC_32K_TUNE_TARGET_REG_POS         12
#define DAIF_RC_32K_TUNE_START_MASK             0x00000001
#define DAIF_RC_32K_TUNE_REF_CLK_SEL_MASK       0x00000002
#define DAIF_RST_RCOSC_32K_MASK                 0x00000004
#define DAIF_RC_32K_TUNE_SWRSTN_MASK            0x00000008
#define DAIF_RC_32K_TUNE_WIN_CNT_THRESHOLD_MASK 0x00000FF0
#define DAIF_RC_32K_TUNE_TARGET_REG_MASK        0x1FFFF000
// PLL_AFC_CTRL
#define DAIF_PLL_AFC_START_FREQ_POS             0
#define DAIF_PLL_AFC_STOP_FREQ_POS              16
#define DAIF_PLL_AFC_START_FREQ_MASK            0x00000FFF
#define DAIF_PLL_AFC_STOP_FREQ_MASK             0x0FFF0000
// RC32K_TUN_OUT
#define DAIF_RTUNE_POS                          0
#define DAIF_CTUNE_POS                          8
#define DAIF_RTUNE_MASK                         0x000000FF
#define DAIF_CTUNE_MASK                         0x00007F00
// CLK_CFG
#define DAIF_EN_CKO16M_XTAL_DIG_POS             0
#define DAIF_EN_CKO16M_XTAL_ANA_POS             1
#define DAIF_EN_CKO16M_XTAL_GPADC_POS           2 // xh98xx remove
#define DAIF_EN_CKO16M_XTAL_PLL_POS             3
#define DAIF_SEL_AUDIO_CLK_POS                  4 // xh98xx new
#define DAIF_EN_CKO16M_XTAL_AUDIO_POS           5 // xh98xx remove
#define DAIF_EN_CKO16M_XTAL_DIG_MASK            0x00000001
#define DAIF_EN_CKO16M_XTAL_ANA_MASK            0x00000002
#define DAIF_EN_CKO16M_XTAL_GPADC_MASK          0x00000004
#define DAIF_EN_CKO16M_XTAL_PLL_MASK            0x00000008
#define DAIF_SEL_AUDIO_CLK_MASK                 0x00000010
#define DAIF_EN_CKO16M_XTAL_AUDIO_MASK          0x00000020
// XTAL32M_INTRS
#define DAIF_XTAL32M_CLK_RDY_INTRS_POS          0
#define DAIF_XTAL32M_CLK_FATAL_INTRS_POS        1
#define DAIF_XTAL32M_CLK_RDY_INTRS_MASK_POS     2
#define DAIF_XTAL32M_CLK_FATAL_INTRS_MASK_POS   3
#define DAIF_XTAL32M_CLK_RDY_POS                16
#define DAIF_XTAL32M_FATAL_POS                  17
#define DAIF_XTAL32M_CLK_RDY_INTRS_MASK         0x00000001
#define DAIF_XTAL32M_CLK_FATAL_INTRS_MASK       0x00000002
#define DAIF_XTAL32M_CLK_RDY_INTRS_MASK_MASK    0x00000004
#define DAIF_XTAL32M_CLK_FATAL_INTRS_MASK_MASK  0x00000008
#define DAIF_XTAL32M_CLK_RDY_MASK               0x00010000
#define DAIF_XTAL32M_FATAL_MASK                 0x00020000
// FREQ_CFG0
#define DAIF_FREQ_REG_MO_POS                    0
#define DAIF_FREQ_REG_ME_POS                    12
#define DAIF_INTER_FREQ_2M_15_0_POS             16
#define DAIF_FREQ_REG_MO_MASK                   0x00000FFF
#define DAIF_FREQ_REG_ME_MASK                   0x00001000
#define DAIF_INTER_FREQ_2M_15_0_MASK            0xFFFF0000
// FREQ_CFG1
#define DAIF_INTER_FREQ_1M_POS                  0
#define DAIF_INTER_FREQ_1M_MASK                 0x01FFFFFF
// FREQ_CFG2
#define DAIF_FREQ_COR_POS                       0
#define DAIF_INTER_FREQ_2M_24_16_POS            23
#define DAIF_FREQ_COR_MASK                      0x007FFFFF
#define DAIF_INTER_FREQ_2M_24_16_MASK           0xFF800000
// VCO_CTRL0
#define DAIF_AFC_START_POS                      0
#define DAIF_KDCO_START_POS                     1
#define DAIF_VTRACK_EN_POS                      2
#define DAIF_VTRACK_MODE_POS                    3
#define DAIF_VTRACK_MO_POS                      4
#define DAIF_TRX_DBG_POS                        7
#define DAIF_TX_TRACK_OUF_POS                   8
#define DAIF_RX_TRACK_OUF_POS                   9
#define DAIF_AFC_EN_MO_POS                      10
#define DAIF_AFC_EN_ME_POS                      11
#define DAIF_AFC_TUNE_MO_POS                    12
#define DAIF_RX_EN_MO_POS                       21
#define DAIF_TX_EN_MO_POS                       22
#define DAIF_AFC_TUNE_ME_POS                    23
#define DAIF_KDCO_MO_POS                        24
#define DAIF_KDCO_ME_POS                        31
#define DAIF_AFC_START_MASK                     0x00000001
#define DAIF_KDCO_START_MASK                    0x00000002
#define DAIF_VTRACK_EN_MASK                     0x00000004
#define DAIF_VTRACK_MODE_MASK                   0x00000008
#define DAIF_VTRACK_MO_MASK                     0x00000070
#define DAIF_TRX_DBG_MASK                       0x00000080
#define DAIF_TX_TRACK_OUF_MASK                  0x00000100
#define DAIF_RX_TRACK_OUF_MASK                  0x00000200
#define DAIF_AFC_EN_MO_MASK                     0x00000400
#define DAIF_AFC_EN_ME_MASK                     0x00000800
#define DAIF_AFC_TUNE_MO_MASK                   0x001FF000
#define DAIF_RX_EN_MO_MASK                      0x00200000
#define DAIF_TX_EN_MO_MASK                      0x00400000
#define DAIF_AFC_TUNE_ME_MASK                   0x00800000
#define DAIF_KDCO_MO_MASK                       0x3F000000
#define DAIF_KDCO_ME_MASK                       0x80000000
// VCO_CTRL1
#define DAIF_PEAKDET_START_POS                  0
#define DAIF_AFC_RAM_AUTO_GATE_POS              1
#define DAIF_BLE_1M_2M_SEL_MO_POS               4
#define DAIF_BLE_1M_2M_SEL_ME_POS               5
#define DAIF_AFC_TARGET_BASE_POS                16
#define DAIF_PEAKDET_START_MASK                 0x00000001
#define DAIF_AFC_RAM_AUTO_GATE_MASK             0x00000002
#define DAIF_BLE_1M_2M_SEL_MO_MASK              0x00000010
#define DAIF_BLE_1M_2M_SEL_ME_MASK              0x00000020
#define DAIF_AFC_TARGET_BASE_MASK               0xFFFF0000
// PLL_CTRL0
#if defined(CONFIG_XH98xxD) || defined(CONFIG_XH98xxF)
#define DAIF_LDO_PFD_POS                        0
#define DAIF_LDO_CP_POS                         2
#define DAIF_LDO_BUF_POS                        6
#define DAIF_LDO_VCO_POS                        8
#define DAIF_PLL_POWER_POS                      10  // xh98xx new (must be 1)
#define DAIF_LDO_BUF2_POS                       11
#define DAIF_ICP_CTRL_POS                       14
#define DAIF_REG_PLL_31_0_POS                   0
#define DAIF_LDO_PFD_MASK                       0x00000003
#define DAIF_LDO_CP_MASK                        0x0000000C
#define DAIF_LDO_BUF_MASK                       0x000000C0
#define DAIF_LDO_VCO_MASK                       0x00000300
#define DAIF_PLL_POWER_MASK                     0x00000400
#define DAIF_LDO_BUF2_MASK                      0x00001800
#define DAIF_ICP_CTRL_MASK                      0x0001C000
#define DAIF_REG_PLL_31_0_MASK                  0xFFFFFFFF
#else
#define DAIF_LDO_PFD_POS                        0       // 00=0.95;01=1;10=1.05;11=1.1;
#define DAIF_LDO_VCO_POS                        8       // 000=0.97V;001=1.00V;010=1.04V;011=1.07V;100=1.10V;101=1.14V;110=1.19V;111=1.29V;
#define DAIF_LDO_BUF2_POS                       11      // 00=0.65;01=0.7;10=0.75;11=0.8;
#define DAIF_BP_DIOX_POS                        13      // 1表示CP线性化处理，0表示CP没有通过的线性化处理
#define DAIF_ICP_CTRL_POS                       14      // 000=40uA;001=50uA;010=60uA;011=70uA;100=80uA;101=90uA;110=100uA;111=110uA
#define DAIF_VREF_VCDET_POS                     17      // 00代表的是Vctrl电压范围为0.05V到0.85V; 01代表的是Vctrl电压范围为0.15V到0.75V 10代表的是Vctrl电压范围为0.25V到0.65V 11代表的是Vctrl电压范围为0.35V到0.55V
#define DAIF_VREF_SEL_PEAKDET_POS               19      // VCO单端Vp的幅度电压 000=0.115V;001=0.175V;010=0.235V;011=0.285V;100=0.335V;101=0.385V;110=0.435V;111=0.485
#define DAIF_SEL_DIGTEST_POS                    22      // 0代表的是测试clkfb时钟;1代表的是测试clkref时钟;
#define DAIF_EN_DIGTEST_POS                     23      // 0代表的是关闭数字电路测试功能；1代表的是开启测试数字电路功能
#define DAIF_REG_CTUN_POS                       24      // 0代表的是有一个ctunning电容不接入VCO； 1代表的是有一个多余的电容加入VCO，使其频率下降
#define DAIF_REG_FTUN_POS                       25      // 00代表无ftuning电容加入VCO；10/01代表是的有一个ftuning电路加入VCO，11代表的是有2个ftuning电容加入VCO
#define DAIF_EN_CP_TX_POS                       27      // 在tx下，CP是否加电流的使能信号，1=CP增加电流，0表示CP不增加电流
#define DAIF_SEL_DCTEST_POS                     29      // 000=高阻; 001= dac中电流偏置模块的参考电压;010=vco控制电压;011= vco的地;100= pfd的地;101=pfd的电源电压; 110= buffer2的电源电压; 111= vco的电源电压;
#define DAIF_LDO_PFD_MASK                       0x00000003
#define DAIF_LDO_VCO_MASK                       0x00000700
#define DAIF_LDO_BUF2_MASK                      0x00001800
#define DAIF_BP_DIOX_MASK                       0x00002000
#define DAIF_ICP_CTRL_MASK                      0x0001C000
#define DAIF_VREF_VCDET_MASK                    0x00060000
#define DAIF_VREF_SEL_PEAKDET_MASK              0x00380000
#define DAIF_SEL_DIGTEST_MASK                   0x00400000
#define DAIF_EN_DIGTEST_MASK                    0x00800000
#define DAIF_REG_CTUN_MASK                      0x01000000
#define DAIF_REG_FTUN_MASK                      0x06000000
#define DAIF_EN_CP_TX_MASK                      0x08000000
#define DAIF_SEL_DCTEST_MASK                    0xE0000000
#endif

// PLL_CTRL1
#define DAIF_REG_PLL_33_32_POS                  0
#define DAIF_DIGI_DIN_REG_POS                   4
#define DAIF_DIGI_DIN_BYPASS_POS                16
#define DAIF_VBIAS_DIV2_POS                     17 // xh98xx new
#define DAIF_CON_BIAS_IDAC_PLL_POS              20 // xh98xx new
#define DAIF_REG_PLL_33_32_MASK                 0x00000003
#define DAIF_DIGI_DIN_REG_MASK                  0x00003FF0
#define DAIF_DIGI_DIN_BYPASS_MASK               0x00010000
#define DAIF_VBIAS_DIV2_MASK                    0x000E0000
#define DAIF_CON_BIAS_IDAC_PLL_MASK             0x03F00000
// PLL_CTRL2
#define DAIF_DITHER_EN_POS                      0
#define DAIF_DITHER_IN_POS                      1
#define DAIF_ENB_INTEGER_SDM_POS                2
#define DAIF_EN_BLE_POS                         4 // xh98xx remove
#define DAIF_SEL_SDM_ORDER_POS                  5
#define DAIF_DATA_SYNC_BYPASS_POS               6
#define DAIF_MOD_DIV_MO_POS                     8
#define DAIF_MOD_DIV_ME_POS                     15
#define DAIF_ICON_VCO_MO_POS                    16
#define DAIF_ICON_VCO_ME_POS                    19
#define DAIF_DIN_SDM_TX_MO_POS                  20 // xh98xx remove
#define DAIF_DIN_SDM_TX_ME_POS                  24 // xh98xx remove
#define DAIF_EN_KVCO2_POS                       20 // xh98xx new
#define DAIF_ENB_RX_DIV_MO_POS                  23 // xh98xx new
#define DAIF_ENB_RX_DIV_ME_POS                  24 // xh98xx new
#define DAIF_MOD_DIV_EDGE_SEL_POS               25
#define DAIF_TX_SDM_EDGE_SEL_POS                26
#define DAIF_PLL_CLK_FB_INV_POS                 27
#define DAIF_PLL_SEL_INJ_POS                    28 // xh98xx remove
#define DAIF_SEL_DACCLK_PLL_POS                 28 // xh98xx new
#define DAIF_PLL_SEL_ILN_POS                    29
#define DAIF_ENQUAD_DIV_MO_POS                  30
#define DAIF_ENQUAD_DIV_ME_POS                  31
#define DAIF_DITHER_EN_MASK                     0x00000001
#define DAIF_DITHER_IN_MASK                     0x00000002
#define DAIF_ENB_INTEGER_SDM_MASK               0x00000004
#define DAIF_EN_BLE_MASK                        0x00000010
#define DAIF_SEL_SDM_ORDER_MASK                 0x00000020
#define DAIF_DATA_SYNC_BYPASS_MASK              0x00000040
#define DAIF_MOD_DIV_MO_MASK                    0x00007F00
#define DAIF_MOD_DIV_ME_MASK                    0x00008000
#define DAIF_ICON_VCO_MO_MASK                   0x00070000
#define DAIF_ICON_VCO_ME_MASK                   0x00080000
#define DAIF_DIN_SDM_TX_MO_MASK                 0x00F00000
#define DAIF_DIN_SDM_TX_ME_MASK                 0x01000000
#define DAIF_EN_KVCO2_MASK                      0x00700000
#define DAIF_ENB_RX_DIV_MO_MASK                 0x00800000
#define DAIF_ENB_RX_DIV_ME_MASK                 0x01000000
#define DAIF_MOD_DIV_EDGE_SEL_MASK              0x02000000
#define DAIF_TX_SDM_EDGE_SEL_MASK               0x04000000
#define DAIF_PLL_CLK_FB_INV_MASK                0x08000000
#define DAIF_PLL_SEL_INJ_MASK                   0x10000000
#define DAIF_SEL_DACCLK_PLL_MASK                0x10000000
#define DAIF_PLL_SEL_ILN_MASK                   0x20000000
#define DAIF_ENQUAD_DIV_MO_MASK                 0x40000000
#define DAIF_ENQUAD_DIV_ME_MASK                 0x80000000
// LNA_MIX_CFG
#define DAIF_EN_ATTEN_MO_POS                    0
#define DAIF_EN_ATTEN_ME_POS                    1
#define DAIF_GAIN_ATTEN_MO_POS                  2
#define DAIF_GAIN_ATTEN_ME_POS                  5
#define DAIF_GAIN_LNA_MO_POS                    6
#define DAIF_GAIN_LNA_ME_POS                    9
#define DAIF_LNA_VBCT_POS                       10
#define DAIF_MIX_VBCT_POS                       13
#define DAIF_CT_CONSTGM_POS                     16
#define DAIF_EN_ATTEN_MO_MASK                   0x00000001
#define DAIF_EN_ATTEN_ME_MASK                   0x00000002
#define DAIF_GAIN_ATTEN_MO_MASK                 0x0000000C
#define DAIF_GAIN_ATTEN_ME_MASK                 0x00000020
#define DAIF_GAIN_LNA_MO_MASK                   0x000001C0
#define DAIF_GAIN_LNA_ME_MASK                   0x00000200
#define DAIF_LNA_VBCT_MASK                      0x00000C00
#define DAIF_MIX_VBCT_MASK                      0x0000E000
#define DAIF_CT_CONSTGM_MASK                    0x00030000
// TIA_LPF_CFG
#define DAIF_FILTER_LP_CP_SEL_POS               0
#define DAIF_FILTER_MOD_SEL_MO_POS              1
#define DAIF_FILTER_MOD_SEL_ME_POS              2
#define DAIF_FILTER_SWAP_POS                    4
#define DAIF_FILTER_IBCT_POS                    5 // xh98xx new
#define DAIF_GAIN_TIA_MO_POS                    8
#define DAIF_GAIN_TIA_ME_POS                    10
#define DAIF_GAIN_LPF_MO_POS                    11
#define DAIF_GAIN_LPF_ME_POS                    14
#define DAIF_FILTER_CTUNE_POS                   16
#define DAIF_TIA_CTUNE_POS                      20
#define DAIF_TIA_LP_CP_SEL_POS                  24
#define DAIF_TIA_SWAP_POS                       25
#define DAIF_TIA_MOD_SEL_MO_POS                 26
#define DAIF_TIA_MOD_SEL_ME_POS                 27
#define DAIF_FILTER_LP_CP_SEL_MASK              0x00000001
#define DAIF_FILTER_MOD_SEL_MO_MASK             0x00000002
#define DAIF_FILTER_MOD_SEL_ME_MASK             0x00000004
#define DAIF_FILTER_SWAP_MASK                   0x00000010
#define DAIF_FILTER_IBCT_MASK                   0x00000060
#define DAIF_GAIN_TIA_MO_MASK                   0x00000300
#define DAIF_GAIN_TIA_ME_MASK                   0x00000400
#define DAIF_GAIN_LPF_MO_MASK                   0x00003800
#define DAIF_GAIN_LPF_ME_MASK                   0x00004000
#define DAIF_FILTER_CTUNE_MASK                  0x000F0000
#define DAIF_TIA_CTUNE_MASK                     0x00F00000
#define DAIF_TIA_LP_CP_SEL_MASK                 0x01000000
#define DAIF_TIA_SWAP_MASK                      0x02000000
#define DAIF_TIA_MOD_SEL_MO_MASK                0x04000000
#define DAIF_TIA_MOD_SEL_ME_MASK                0x08000000
// RC32M_TUN
#define DAIF_RC_32M_TUNE_START_POS              0
#define DAIF_RC_32M_TUNE_POS                    8
#define DAIF_RC_32M_TUNE_START_MASK             0x00000001
#define DAIF_RC_32M_TUNE_MASK                   0x0000FF00
// RXADC_CTRL
#define DAIF_RST_MO_POS                         0
#define DAIF_RST_ME_POS                         1
#define DAIF_CNT_EN_POS                         8 // xh98xx remove
#define DAIF_B2T_EN_POS                         9 // xh98xx remove
#define DAIF_DELAY_POS                          12
#define DAIF_RST_MO_MASK                        0x00000001
#define DAIF_RST_ME_MASK                        0x00000002
#define DAIF_CNT_EN_MASK                        0x00000100
#define DAIF_B2T_EN_MASK                        0x00000200
#define DAIF_DELAY_MASK                         0x00007000
// RX_RCCAL_CTRL
#define DAIF_START_POS                          0
#define DAIF_DLY1_POS                           16
#define DAIF_DLY2_POS                           24
#define DAIF_START_MASK                         0x00000001
#define DAIF_DLY1_MASK                          0x00FF0000
#define DAIF_DLY2_MASK                          0xFF000000
// RX_RCCAL_STAT
#define DAIF_FSM_POS                            24
#define DAIF_CNT_T2_POS                         12
#define DAIF_CNT_T1_POS                         0
#define DAIF_FSM_MASK                           0x07000000
#define DAIF_CNT_T2_MASK                        0x003FF000
#define DAIF_CNT_T1_MASK                        0x000003FF
// MAIN_ST_CFG0
#define DAIF_TXLDO_WAIT_POS                     0
#define DAIF_TX_PLL_WAIT_POS                    16
#define DAIF_TXLDO_WAIT_MASK                    0x0000FFFF
#define DAIF_TX_PLL_WAIT_MASK                   0xFFFF0000
// MAIN_ST_CFG1
#define DAIF_PA_WAIT_POS                        0
#define DAIF_RAMP_1US_POS                       8
#define DAIF_TX_WAIT_POS                        9
#define DAIF_AGC_SYNC_TIME_POS                  12
#define DAIF_RXLDO_WAIT_POS                     16
#define DAIF_PA_WAIT_MASK                       0x000000FF
#define DAIF_RAMP_1US_MASK                      0x00000100
#define DAIF_TX_WAIT_MASK                       0x00000E00
#define DAIF_AGC_SYNC_TIME_MASK                 0x0000F000
#define DAIF_RXLDO_WAIT_MASK                    0xFFFF0000
// MAIN_ST_CFG2
#define DAIF_RX_PLL_WAIT_POS                    0
#define DAIF_TX_END_WAIT_POS                    16
#define DAIF_RX_END_WAIT_POS                    24
#define DAIF_RX_PLL_WAIT_MASK                   0x0000FFFF
#define DAIF_TX_END_WAIT_MASK                   0x00FF0000
#define DAIF_RX_END_WAIT_MASK                   0xFF000000
// AGC_CFG0
#define DAIF_AGCSEL_IQ_MO_POS                   0
#define DAIF_PKDADC_CNT_EN_POS                  1 // xh98xx remove
#define DAIF_PKDADC_B2T_EN_POS                  2 // xh98xx remove
#define DAIF_EN_PKDADC_VREF_TEST_POS            2 // xh98xx new
#define DAIF_AGC_OP3_POS                        3
#define DAIF_PKDADC_DELAY_POS                   4
#define DAIF_AGC_OP2_SET_POS                    7
#define DAIF_PKDADC_CLKSEL_POS                  8
#define DAIF_AGC_A_POS                          10
#define DAIF_AGC_B_POS                          14
#define DAIF_OS_CALI_EN_POS                     18
#define DAIF_OS_OFFSET_EN_POS                   19
#define DAIF_CALI_PD_SETTLE_POS                 20
#define DAIF_AGC_GAIN1_POS                      24 // xh98xx remove
#define DAIF_PIF_LAG_POS                        24 // xh98xx new
#define DAIF_EN_PIF_LAG_POS                     29 // xh98xx new
#define DAIF_PDTIF_CALI_MO_POS                  30
#define DAIF_PDTIF_CALI_ME_POS                  31
#define DAIF_AGCSEL_IQ_MO_MASK                  0x00000001
#define DAIF_PKDADC_CNT_EN_MASK                 0x00000002
#define DAIF_PKDADC_B2T_EN_MASK                 0x00000004
#define DAIF_EN_PKDADC_VREF_TEST_MASK           0x00000004
#define DAIF_AGC_OP3_MASK                       0x00000008
#define DAIF_PKDADC_DELAY_MASK                  0x00000070
#define DAIF_AGC_OP2_SET_MASK                   0x00000080
#define DAIF_PKDADC_CLKSEL_MASK                 0x00000300
#define DAIF_AGC_A_MASK                         0x00003C00
#define DAIF_AGC_B_MASK                         0x0003C000
#define DAIF_OS_CALI_EN_MASK                    0x00040000
#define DAIF_OS_OFFSET_EN_MASK                  0x00080000
#define DAIF_CALI_PD_SETTLE_MASK                0x00F00000
#define DAIF_AGC_GAIN1_MASK                     0x1F000000
#define DAIF_PIF_LAG_MASK                       0x1F000000
#define DAIF_EN_PIF_LAG_MASK                    0x20000000
#define DAIF_PDTIF_CALI_MO_MASK                 0x40000000
#define DAIF_PDTIF_CALI_ME_MASK                 0x80000000
// AGC_CFG1
#define DAIF_AGC_PFS_POS                        0
#define DAIF_AGC_PTH_POS                        9
#define DAIF_AGC_SETTLE_TIME1_POS               18
#define DAIF_AGC_SETTLE_TIME2_POS               20
#define DAIF_WAIT_SYNC_TIME_POS                 22
#define DAIF_LOAD_CYCLE_POS                     25
#define DAIF_GAIN_INI_POS                       28
#define DAIF_AGC_PFS_MASK                       0x000001FF
#define DAIF_AGC_PTH_MASK                       0x0003FE00
#define DAIF_AGC_SETTLE_TIME1_MASK              0x000C0000
#define DAIF_AGC_SETTLE_TIME2_MASK              0x00300000
#define DAIF_WAIT_SYNC_TIME_MASK                0x01C00000
#define DAIF_LOAD_CYCLE_MASK                    0x0E000000
#define DAIF_GAIN_INI_MASK                      0xF0000000
// AGC_CFG2
#define DAIF_PRXADC_SEL_POS                     0
#define DAIF_AGC_STATE_SEL_POS                  4
#define DAIF_PIF_OS_FLAG_POS                    7
#define DAIF_AGC_OP4_POS                        8
#define DAIF_PKDADC_TRIM_VBIAS_POS              9  // xh98xx new
#define DAIF_FILT_GAIN_INI_POS                  12 // xh98xx new
#define DAIF_PIF_OS_REG_POS                     17 // xh98xx [23:17]
#define DAIF_INI_WAIT_TIME_POS                  26
#define DAIF_GET_PIF_TIME_POS                   28
#define DAIF_PRXADC_SEL_MASK                    0x00000003
#define DAIF_AGC_STATE_SEL_MASK                 0x00000010
#define DAIF_PIF_OS_FLAG_MASK                   0x00000080
#define DAIF_AGC_OP4_MASK                       0x00000100
#define DAIF_PKDADC_TRIM_VBIAS_MASK             0x00000E00
#define DAIF_FILT_GAIN_INI_MASK                 0x00007000
#define DAIF_PIF_OS_REG_MASK                    0x03FE0000
#define DAIF_INI_WAIT_TIME_MASK                 0x0C000000
#define DAIF_GET_PIF_TIME_MASK                  0x30000000
// AGC_CFG3
#define DAIF_PIF_OS_POS                         0  // xh98xx [6:0]
#define DAIF_PIF_OS_REG_2M_POS                  16 // xh98xx [22:16]
#define DAIF_PIF_OS_MASK                        0x000001FF
#define DAIF_PIF_OS_REG_2M_MASK                 0x01FF0000
// IR_RX_CFG
#define DAIF_IR_PDB_RX_POS                      0
#define DAIF_IRRX_EN_POS                        1
#define DAIF_IRRX_CH_SEL_POS                    2
#define DAIF_IR_SENSE_POS                       4
#define DAIF_IR_TEST_EN_POS                     6
#define DAIF_IR_TX_IN_ME_POS                    7
#define DAIF_IR_TX_IN_MO_POS                    8
#define DAIF_IR_PDB_RX_MASK                     0x00000001
#define DAIF_IRRX_EN_MASK                       0x00000002
#define DAIF_IRRX_CH_SEL_MASK                   0x0000000C
#define DAIF_IR_SENSE_MASK                      0x00000030
#define DAIF_IR_TEST_EN_MASK                    0x00000040
#define DAIF_IR_TX_IN_ME_MASK                   0x00000080
#define DAIF_IR_TX_IN_MO_MASK                   0x00000100
// TIA_DCOC_CFG
#define DAIF_TIA_ST_POS                         0
#define DAIF_TIA_IBCT_POS                       1
#define DAIF_TIA_EN_DCOC_POS                    2
#define DAIF_TIA_DIN_DCOC_I_MO_POS              3
#define DAIF_TIA_DIN_DCOC_Q_MO_POS              8
#define DAIF_TIA_SEL_DCOC_FS_POS                13
#define DAIF_TIA_DIN_DCOC_I_ME_POS              15
#define DAIF_TIA_DIN_DCOC_Q_ME_POS              16
#define DAIF_TIA_DCOC_CALI_POS                  17
#define DAIF_TIA_ST_MASK                        0x00000001
#define DAIF_TIA_IBCT_MASK                      0x00000002
#define DAIF_TIA_EN_DCOC_MASK                   0x00000004
#define DAIF_TIA_DIN_DCOC_I_MO_MASK             0x000000F8
#define DAIF_TIA_DIN_DCOC_Q_MO_MASK             0x00001F00
#define DAIF_TIA_SEL_DCOC_FS_MASK               0x00006000
#define DAIF_TIA_DIN_DCOC_I_ME_MASK             0x00008000
#define DAIF_TIA_DIN_DCOC_Q_ME_MASK             0x00010000
#define DAIF_TIA_DCOC_CALI_MASK                 0x00020000
#if defined(CONFIG_XH98xxD) || defined(CONFIG_XH98xxF)
// IF_AGC_LUT_REG0
#define DAIF_IF_AGC_LUT_0_POS                   0
#define DAIF_IF_AGC_LUT_1_POS                   9
#define DAIF_IF_AGC_LUT_2_POS                   18
#define DAIF_IF_AGC_LUT_0_MASK                  0x000001FF
#define DAIF_IF_AGC_LUT_1_MASK                  0x0003FE00
#define DAIF_IF_AGC_LUT_2_MASK                  0x07FC0000
// IF_AGC_LUT_REG1
#define DAIF_IF_AGC_LUT_3_POS                   0
#define DAIF_IF_AGC_LUT_4_POS                   9
#define DAIF_IF_AGC_LUT_5_POS                   18
#define DAIF_IF_AGC_LUT_3_MASK                  0x000001FF
#define DAIF_IF_AGC_LUT_4_MASK                  0x0003FE00
#define DAIF_IF_AGC_LUT_5_MASK                  0x07FC0000
// IF_AGC_LUT_REG2
#define DAIF_IF_AGC_LUT_6_POS                   0
#define DAIF_IF_AGC_LUT_7_POS                   9
#define DAIF_IF_AGC_LUT_8_POS                   18
#define DAIF_IF_AGC_LUT_6_MASK                  0x000001FF
#define DAIF_IF_AGC_LUT_7_MASK                  0x0003FE00
#define DAIF_IF_AGC_LUT_8_MASK                  0x07FC0000
// IF_AGC_LUT_REG3
#define DAIF_IF_AGC_LUT_9_POS                   0
#define DAIF_IF_AGC_LUT_9_MASK                  0x000001FF
#else
// IF_AGC_LUT_REG0
#define DAIF_IF_AGC_LUT_0_POS                   0
#define DAIF_IF_AGC_LUT_1_POS                   7
#define DAIF_IF_AGC_LUT_2_POS                   14
#define DAIF_IF_AGC_LUT_3_POS                   21
#define DAIF_IF_AGC_LUT_0_MASK                  0x0000007F
#define DAIF_IF_AGC_LUT_1_MASK                  0x00003F80
#define DAIF_IF_AGC_LUT_2_MASK                  0x001FC000
#define DAIF_IF_AGC_LUT_3_MASK                  0x0FE00000
// IF_AGC_LUT_REG1
#define DAIF_IF_AGC_LUT_4_POS                   0
#define DAIF_IF_AGC_LUT_5_POS                   7
#define DAIF_IF_AGC_LUT_6_POS                   14
#define DAIF_IF_AGC_LUT_7_POS                   21
#define DAIF_IF_AGC_LUT_4_MASK                  0x0000007F
#define DAIF_IF_AGC_LUT_5_MASK                  0x00003F80
#define DAIF_IF_AGC_LUT_6_MASK                  0x001FC000
#define DAIF_IF_AGC_LUT_7_MASK                  0x0FE00000
// IF_AGC_LUT_REG2
#define DAIF_IF_AGC_LUT_8_POS                   0
#define DAIF_IF_AGC_LUT_9_POS                   7
#define DAIF_IF_AGC_LUT_8_MASK                  0x0000007F
#define DAIF_IF_AGC_LUT_9_MASK                  0x00003F80
#endif
// FILT_AGC_LUT_REG0
#define DAIF_FILT_AGC_LUT_0_POS                 0
#define DAIF_FILT_AGC_LUT_1_POS                 9
#define DAIF_FILT_AGC_LUT_2_POS                 18
#define DAIF_FILT_AGC_LUT_0_MASK                0x000001FF
#define DAIF_FILT_AGC_LUT_1_MASK                0x0003FE00
#define DAIF_FILT_AGC_LUT_2_MASK                0x07FC0000
// FILT_AGC_LUT_REG1
#define DAIF_FILT_AGC_LUT_3_POS                 0
#define DAIF_FILT_AGC_LUT_4_POS                 9
#define DAIF_FILT_AGC_LUT_5_POS                 18
#define DAIF_FILT_AGC_LUT_3_MASK                0x000001FF
#define DAIF_FILT_AGC_LUT_4_MASK                0x0003FE00
#define DAIF_FILT_AGC_LUT_5_MASK                0x07FC0000
// FILT_AGC_LUT_REG2
#define DAIF_FILT_AGC_LUT_6_POS                 0
#define DAIF_FILT_AGC_LUT_7_POS                 9
#define DAIF_FILT_AGC_LUT_8_POS                 18
#define DAIF_FILT_AGC_LUT_6_MASK                0x000001FF
#define DAIF_FILT_AGC_LUT_7_MASK                0x0003FE00
#define DAIF_FILT_AGC_LUT_8_MASK                0x07FC0000
// RX_GAINC_LUT_REG0
#define DAIF_LUT0_POS                           0
#define DAIF_LUT1_POS                           8
#define DAIF_LUT2_POS                           16
#define DAIF_LUT3_POS                           24
#define DAIF_LUT0_MASK                          0x000000FF
#define DAIF_LUT1_MASK                          0x0000FF00
#define DAIF_LUT2_MASK                          0x00FF0000
#define DAIF_LUT3_MASK                          0xFF000000
// RX_GAINC_LUT_REG1
#define DAIF_LUT4_POS                           0
#define DAIF_LUT5_POS                           8
#define DAIF_LUT6_POS                           16
#define DAIF_LUT7_POS                           24
#define DAIF_LUT4_MASK                          0x000000FF
#define DAIF_LUT5_MASK                          0x0000FF00
#define DAIF_LUT6_MASK                          0x00FF0000
#define DAIF_LUT7_MASK                          0xFF000000
// RX_GAINC_LUT_REG2
#define DAIF_LUT8_POS                           0
#define DAIF_LUT8_MASK                          0x000000FF
// DCOC_CFG
#define DAIF_EN_DCOC_POS                        0
#define DAIF_SEL_DCOC_FS_POS                    2
#define DAIF_LUT_AUTO_GATE_POS                  3
#define DAIF_DIN_DCOC_I_MO_POS                  8
#define DAIF_DIN_DCOC_I_ME_POS                  15
#define DAIF_DIN_DCOC_Q_MO_POS                  16
#define DAIF_DIN_DCOC_Q_ME_POS                  21
#define DAIF_DCOC_RX_GIDX_POS                   24
#define DAIF_DCOC_FILT_GIDX_POS                 28
#define DAIF_DCOC_CALI_POS                      31
#define DAIF_EN_DCOC_MASK                       0x00000001
#define DAIF_SEL_DCOC_FS_MASK                   0x00000004
#define DAIF_LUT_AUTO_GATE_MASK                 0x00000008
#define DAIF_DIN_DCOC_I_MO_MASK                 0x00001F00
#define DAIF_DIN_DCOC_I_ME_MASK                 0x00008000
#define DAIF_DIN_DCOC_Q_MO_MASK                 0x001F0000
#define DAIF_DIN_DCOC_Q_ME_MASK                 0x00200000
#define DAIF_DCOC_RX_GIDX_MASK                  0x0F000000
#define DAIF_DCOC_FILT_GIDX_MASK                0x70000000
#define DAIF_DCOC_CALI_MASK                     0x80000000
// TRX_SW_CFG
#define DAIF_TRX_EN_MO_POS                      0
#define DAIF_TRX_EN_ME_POS                      1
#define DAIF_TRX_EN_MO_MASK                     0x00000001
#define DAIF_TRX_EN_ME_MASK                     0x00000002
// GIO_AUTO_CTRL
#define DAIF_GIO_AUTO_EN_POS                    8
#define DAIF_GIO_TX_DLY_POS                     0
#define DAIF_GIO_AUTO_EN_MASK                   0x00000100
#define DAIF_GIO_TX_DLY_MASK                    0x000000FF
// CLK_ENS
#define DAIF_ADC_CLK_EN_POS                     0
#define DAIF_PLL_AFC_CLK_EN_POS                 1
#define DAIF_PLL_AMP_CLK_EN_POS                 2
#define DAIF_PLL_VTRACK_CLK_EN_POS              3
#define DAIF_PLL_LUT_CLK_EN_POS                 4
#define DAIF_PLL_CLK_AFC_EN_POS                 5
#define DAIF_RC_32K_TUNE_CLK_EN_POS             6
#define DAIF_RC_32M_TUNE_CLK_EN_POS             7
#define DAIF_RC_CALIB_CLK_EN_POS                8
#define DAIF_MAIN_FSM_CLK_EN_POS                9
#define DAIF_RX_AGC_CLK_EN_POS                  10
#define DAIF_DCOC_LUT_CLK_EN_POS                11
#define DAIF_SDM_CLK_EN_POS                     12
#define DAIF_CPS_CLK32M_EN_POS                  13
#define DAIF_CPS_CLK32K_EN_POS                  14
#define DAIF_MON_DMA_RF_CLK_EN_POS              15
#define DAIF_XTAL32M_CTRL_CLK_EN_POS            16
#define DAIF_PLL_CLK_REF_EN_POS                 17
#define DAIF_CC_CLK_EN_POS                      18
#define DAIF_ADC_CLK_EN_MASK                    0x00000001
#define DAIF_PLL_AFC_CLK_EN_MASK                0x00000002
#define DAIF_PLL_AMP_CLK_EN_MASK                0x00000004
#define DAIF_PLL_VTRACK_CLK_EN_MASK             0x00000008
#define DAIF_PLL_LUT_CLK_EN_MASK                0x00000010
#define DAIF_PLL_CLK_AFC_EN_MASK                0x00000020
#define DAIF_RC_32K_TUNE_CLK_EN_MASK            0x00000040
#define DAIF_RC_32M_TUNE_CLK_EN_MASK            0x00000080
#define DAIF_RC_CALIB_CLK_EN_MASK               0x00000100
#define DAIF_MAIN_FSM_CLK_EN_MASK               0x00000200
#define DAIF_RX_AGC_CLK_EN_MASK                 0x00000400
#define DAIF_DCOC_LUT_CLK_EN_MASK               0x00000800
#define DAIF_SDM_CLK_EN_MASK                    0x00001000
#define DAIF_CPS_CLK32M_EN_MASK                 0x00002000
#define DAIF_CPS_CLK32K_EN_MASK                 0x00004000
#define DAIF_MON_DMA_RF_CLK_EN_MASK             0x00008000
#define DAIF_XTAL32M_CTRL_CLK_EN_MASK           0x00010000
#define DAIF_PLL_CLK_REF_EN_MASK                0x00020000
#define DAIF_CC_CLK_EN_MASK                     0x00040000
// PLL_LUT_DBG
#define DAIF_PLL_LUT_IDX_POS                    16
#define DAIF_PLL_LUT_DATA_POS                   0
#define DAIF_PLL_LUT_DATA_TRACK_POS             0
#define DAIF_PLL_LUT_DATA_TUNE_POS              6
#define DAIF_PLL_LUT_WR_POS                     24
#define DAIF_PLL_LUT_LU_POS                     28
#define DAIF_PLL_LUT_IDX_MASK                   0x007F0000
#define DAIF_PLL_LUT_DATA_MASK                  0x00001FFF
#define DAIF_PLL_LUT_DATA_TRACK_MASK            0x0000003F
#define DAIF_PLL_LUT_DATA_TUNE_MASK             0x00001FC0
#define DAIF_PLL_LUT_WR_MASK                    0x01000000
#define DAIF_PLL_LUT_LU_MASK                    0x10000000
// RX_ADC_OUT
#define DAIF_RXADC_QOUT_POS                     0
#define DAIF_RXADC_IOUT_POS                     16
#define DAIF_RXADC_QOUT_MASK                    0x000001FF
#define DAIF_RXADC_IOUT_MASK                    0x01FF0000
// ANA_TST_CTRL
#define DAIF_ANA_TST_OUT_POS                    0
#define DAIF_ANA_TST_DC_POS                     0
#define DAIF_ANA_TST_EN_POS                     12
#define DAIF_ANA_TST_OUT_MASK                   0x000003FF
#define DAIF_ANA_TST_DC_MASK                    0x0000000F
#define DAIF_ANA_TST_EN_MASK                    0x00001000
// KDCO_LUT_1M_REG0
#define DAIF_LUT_1M_0_POS                       0
#define DAIF_LUT_1M_1_POS                       8
#define DAIF_LUT_1M_2_POS                       16
#define DAIF_LUT_1M_3_POS                       24
#define DAIF_LUT_1M_0_MASK                      0x0000003F
#define DAIF_LUT_1M_1_MASK                      0x00003F00
#define DAIF_LUT_1M_2_MASK                      0x003F0000
#define DAIF_LUT_1M_3_MASK                      0x3F000000
// KDCO_LUT_1M_REG1
#define DAIF_LUT_1M_4_POS                       0
#define DAIF_LUT_1M_5_POS                       8
#define DAIF_LUT_1M_4_MASK                      0x0000003F
#define DAIF_LUT_1M_5_MASK                      0x00003F00
// DCOC_CFG1
#define DAIF_OFFSET_I_MO_POS                    16
#define DAIF_OFFSET_I_ME_POS                    24
#define DAIF_OFFSET_Q_MO_POS                    0
#define DAIF_OFFSET_Q_ME_POS                    8
#define DAIF_OFFSET_I_MO_MASK                   0x00FF0000
#define DAIF_OFFSET_I_ME_MASK                   0x01000000
#define DAIF_OFFSET_Q_MO_MASK                   0x000000FF
#define DAIF_OFFSET_Q_ME_MASK                   0x00000100
// KDCO_LUT_2M_REG0
#define DAIF_LUT_2M_0_POS                       0
#define DAIF_LUT_2M_1_POS                       8
#define DAIF_LUT_2M_2_POS                       16
#define DAIF_LUT_2M_3_POS                       24
#define DAIF_LUT_2M_0_MASK                      0x0000003F
#define DAIF_LUT_2M_1_MASK                      0x00003F00
#define DAIF_LUT_2M_2_MASK                      0x003F0000
#define DAIF_LUT_2M_3_MASK                      0x3F000000
// KDCO_LUT_2M_REG1
#define DAIF_LUT_2M_4_POS                       0
#define DAIF_LUT_2M_5_POS                       8
#define DAIF_LUT_2M_4_MASK                      0x0000003F
#define DAIF_LUT_2M_5_MASK                      0x00003F00
// MON_DMA_CFG0
#define DAIF_RF_MON_ENABLE_POS                  0
#define DAIF_RF_MON_DONE_INTR_POS               1
#define DAIF_RF_MON_CFG_POS                     4
#define DAIF_RF_MON_THRSHLD_POS                 8
#define DAIF_PC_MON_ENABLE_POS                  12
#define DAIF_PC_MON_DONE_INTR_POS               13
#define DAIF_PC_MON_THRSHLD_POS                 16
#define DAIF_RF_MON_ENABLE_MASK                 0x00000001
#define DAIF_RF_MON_DONE_INTR_MASK              0x00000002
#define DAIF_RF_MON_CFG_MASK                    0x00000030
#define DAIF_RF_MON_THRSHLD_MASK                0x00000F00
#define DAIF_PC_MON_ENABLE_MASK                 0x00001000
#define DAIF_PC_MON_DONE_INTR_MASK              0x00002000
#define DAIF_PC_MON_THRSHLD_MASK                0x000F0000
// MON_DMA_CFG1
#define DAIF_RF_MON_MAX_POS                     0
#define DAIF_PC_MON_MAX_POS                     16
#define DAIF_RF_MON_MAX_MASK                    0x0000FFFF
#define DAIF_PC_MON_MAX_MASK                    0xFFFF0000
// MON_DMA_CFG2
#define DAIF_MON_DMA_BASE_ADDR_POS              2
#define DAIF_MON_DMA_BASE_ADDR_MASK             0xFFFFFFFC
// DBG_REG
#define DAIF_EN_POS                             4
#define DAIF_IDX_POS                            0
#define DAIF_DATA_POS                           16
#define DAIF_EN_MASK                            0x00000010
#define DAIF_IDX_MASK                           0x0000000F
#define DAIF_DATA_MASK                          0xFFFF0000
// XH98xxCB_CFG
#define DAIF_FSM_98xxCB_FIX_EN_POS              0
#define DAIF_FSM_98xxCB_FIX_EN_MASK             0x00000001
// KDCO_LUT_1M_REG0
#define DAIF_LUT_1M_0_POS                       0
#define DAIF_LUT_1M_1_POS                       8
#define DAIF_LUT_1M_2_POS                       16
#define DAIF_LUT_1M_3_POS                       24
#define DAIF_LUT_1M_0_MASK                      0x0000003F
#define DAIF_LUT_1M_1_MASK                      0x00003F00
#define DAIF_LUT_1M_2_MASK                      0x003F0000
#define DAIF_LUT_1M_3_MASK                      0x3F000000
// KDCO_LUT_1M_REG1
#define DAIF_LUT_1M_4_POS                       0
#define DAIF_LUT_1M_5_POS                       8
#define DAIF_LUT_1M_4_MASK                      0x0000003F
#define DAIF_LUT_1M_5_MASK                      0x00003F00
// KDCO_LUT_2M_REG0
#define DAIF_LUT_2M_0_POS                       0
#define DAIF_LUT_2M_1_POS                       8
#define DAIF_LUT_2M_2_POS                       16
#define DAIF_LUT_2M_3_POS                       24
#define DAIF_LUT_2M_0_MASK                      0x0000003F
#define DAIF_LUT_2M_1_MASK                      0x00003F00
#define DAIF_LUT_2M_2_MASK                      0x003F0000
#define DAIF_LUT_2M_3_MASK                      0x3F000000
// KDCO_LUT_2M_REG1
#define DAIF_LUT_2M_4_POS                       0
#define DAIF_LUT_2M_5_POS                       8
#define DAIF_LUT_2M_4_MASK                      0x0000003F
#define DAIF_LUT_2M_5_MASK                      0x00003F00
// PA_GAIN_REG_18 ~ PA_GAIN_REG_01
#define DAIF_PA_GAIN_PADR1S_CT_MO_POS           14
#define DAIF_PA_GAIN_PADR2S_PCT_MO_POS          10
#define DAIF_PA_GAIN_PADR2S_NCT_MO_POS          6
#define DAIF_PA_GAIN_PACORE_CT_MO_POS           2
#define DAIF_PA_GAIN_TX_ATTEN_MO_POS            0
#define DAIF_PA_GAIN_PADR1S_CT_MO_MASK          0x0001C000
#define DAIF_PA_GAIN_PADR2S_PCT_MO_MASK         0x00003C00
#define DAIF_PA_GAIN_PADR2S_NCT_MO_MASK         0x000003C0
#define DAIF_PA_GAIN_PACORE_CT_MO_MASK          0x0000003C
#define DAIF_PA_GAIN_TX_ATTEN_MO_MASK           0x00000003
// PPG_CTRL
#define DAIF_PPG_RF_TUNE_POS                    0
#define DAIF_PPG_CF_TUNE_POS                    3
#define DAIF_PPG_DCOC_POS                       8
#define DAIF_PPG_CTRL_TXLDO_POS                 14
#define DAIF_PPG_TX_IDAC_POS                    16
#define DAIF_PPG_CTRL_RXLDO_POS                 22
#define DAIF_PPG_TX_SWITCH_POS                  24
#define DAIF_PPG_RF_TUNE_MASK                   0x00000007
#define DAIF_PPG_CF_TUNE_MASK                   0x00000038
#define DAIF_PPG_DCOC_MASK                      0x00003F00
#define DAIF_PPG_CTRL_TXLDO_MASK                0x0000C000
#define DAIF_PPG_TX_IDAC_MASK                   0x003F0000
#define DAIF_PPG_CTRL_RXLDO_MASK                0x00C00000
#define DAIF_PPG_TX_SWITCH_MASK                 0x0F000000
// TRX_EXT_PD_CFG
#define DAIF_TX_EXT_PD_EN_POS                   0
#define DAIF_TX_EXT_PD_POL_POS                  1
#define DAIF_TX_EXT_PD_TCFG_POS                 2
#define DAIF_RX_EXT_PD_EN_POS                   16
#define DAIF_RX_EXT_PD_POL_POS                  17
#define DAIF_RX_EXT_PD_TCFG_POS                 18
#define DAIF_TX_EXT_PD_EN_MASK                  0x00000001
#define DAIF_TX_EXT_PD_POL_MASK                 0x00000002
#define DAIF_TX_EXT_PD_TCFG_MASK                0x0000FFFC
#define DAIF_RX_EXT_PD_EN_MASK                  0x00010000
#define DAIF_RX_EXT_PD_POL_MASK                 0x00020000
#define DAIF_RX_EXT_PD_TCFG_MASK                0xFFFC0000
// CLK_CHK_CNS
#define DAIF_START_POS                          0
#define DAIF_INTR_EN_POS                        1
#define DAIF_INTR_ST_POS                        2
#define DAIF_REF_SEL_POS                        4
#define DAIF_WIN_CNT_THRSH_POS                  8
#define DAIF_START_MASK                         0x00000001
#define DAIF_INTR_EN_MASK                       0x00000002
#define DAIF_INTR_ST_MASK                       0x00000004
#define DAIF_REF_SEL_MASK                       0x00000010
#define DAIF_WIN_CNT_THRSH_MASK                 0xFFFFFF00
// CLK_CHK_STATUS
#define DAIF_CLK_CNT_POS                        0
#define DAIF_CLK_CNT_MASK                       0xFFFFFFFF
// DCOC_LUT*64
#define DAIF_RXQ_OFFSET_POS                     0
#define DAIF_RXI_OFFSET_POS                     8
#define DAIF_ANA_DCOC_Q_POS                     16
#define DAIF_ANA_DCOC_I_POS                     24
#define DAIF_RXQ_OFFSET_MASK                    0x000000FF
#define DAIF_RXI_OFFSET_MASK                    0x0000FF00
#define DAIF_ANA_DCOC_Q_MASK                    0x001F0000
#define DAIF_ANA_DCOC_I_MASK                    0x1F000000
// TIA_DCOC_LUT
#define DAIF_ANA_TIA_DCOC_I_POS                 5
#define DAIF_ANA_TIA_DCOC_Q_POS                 0
#define DAIF_ANA_TIA_DCOC_I_MASK                0x000003E0
#define DAIF_ANA_TIA_DCOC_Q_MASK                0x0000001F
// INTR / INTR_MSK
#define GPADC_EOC_0_POS                         0
#define GPADC_EOC_1_POS                         1
#define GPADC_EOC_2_POS                         2
#define GPADC_EOC_3_POS                         3
#define GPADC_EOC_4_POS                         4
#define GPADC_EOC_5_POS                         5
#define GPADC_EOC_6_POS                         6
#define GPADC_EOC_7_POS                         7
#define GPADC_EOC_0_MASK                        0x00000001
#define GPADC_EOC_1_MASK                        0x00000002
#define GPADC_EOC_2_MASK                        0x00000004
#define GPADC_EOC_3_MASK                        0x00000008
#define GPADC_EOC_4_MASK                        0x00000010
#define GPADC_EOC_5_MASK                        0x00000020
#define GPADC_EOC_6_MASK                        0x00000040
#define GPADC_EOC_7_MASK                        0x00000080
#ifdef CONFIG_XH98xxD
#define GPADC_EOS_POS                           8
#define GPADC_EOA_POS                           9
#define GPADC_OVR_POS                           10
#define GPADC_DMA_POS                           11
#define GPADC_EOS_MASK                          0x00000100
#define GPADC_EOA_MASK                          0x00000200
#define GPADC_OVR_MASK                          0x00000400
#define GPADC_DMA_MASK                          0x00000800
#define GPADC_INTR_ALL_MASK                     0x00000FFF
#endif
#ifdef CONFIG_XH98xxF
#define GPADC_EOC_8_POS                         8
#define GPADC_EOC_9_POS                         9
#define GPADC_EOC_A_POS                         10
#define GPADC_EOS_POS                           11
#define GPADC_EOA_POS                           12
#define GPADC_OVR_POS                           13
#define GPADC_DMA_POS                           14
#define GPADC_EOC_8_MASK                        0x00000100  // xh98xxF add ch EOC MASK and modify EOS EOA OVR DMA MASK
#define GPADC_EOC_9_MASK                        0x00000200
#define GPADC_EOC_A_MASK                        0x00000400
#define GPADC_EOS_MASK                          0x00000800
#define GPADC_EOA_MASK                          0x00001000
#define GPADC_OVR_MASK                          0x00002000
#define GPADC_DMA_MASK                          0x00004000
#define GPADC_INTR_ALL_MASK                     0x00007FFF
#endif
#ifdef CONFIG_XH98xx
#define GPADC_EOC_8_POS                         8
#define GPADC_EOC_9_POS                         9
#define GPADC_EOS_POS                           10
#define GPADC_EOA_POS                           11
#define GPADC_OVR_POS                           12
#define GPADC_EOC_8_MASK                        0x00000100  // xh98xxF add ch EOC MASK and modify EOS EOA OVR DMA MASK
#define GPADC_EOC_9_MASK                        0x00000200
#define GPADC_EOS_MASK                          0x00000400
#define GPADC_EOA_MASK                          0x00000800
#define GPADC_OVR_MASK                          0x00001000
#define GPADC_INTR_ALL_MASK                     0x00001FFF
#endif

// DLY_CFG
#define GPADC_CFG_CHG_DLY_POS                   0
#define GPADC_PD_DLY_MAX_POS                    8
#define GPADC_ORB_DLY_POS                       16
#define GPADC_CFG_CHG_DLY_MASK                  0x000000FF
#define GPADC_PD_DLY_MAX_MASK                   0x0000FF00
#define GPADC_ORB_DLY_MASK                      0x01FF0000
// ADC_CFG0
#define GPADC_ADC_START_POS                     0
#define GPADC_ADC_STOP_POS                      4
#define GPADC_ADC_SUSPEND_POS                   12
#define GPADC_ADC_START_MASK                    0x00000001
#define GPADC_ADC_STOP_MASK                     0x00000010
#define GPADC_ADC_SUSPEND_MASK                  0x00001000
// ADC_CFG1
#define GPADC_AUTO_PD1_POS                      0
#define GPADC_AUTO_DELAY_POS                    1
#define GPADC_AUTO_PD2_POS                      2
#define GPADC_TRIG_MODE_POS                     4
#define GPADC_TRIG_RES_POS                      6
#define GPADC_RSV0_POS                          7
#define GPADC_TRIG_HW_SEL_POS                   8
#define GPADC_DMA_EN_POS                        12
#define GPADC_GP_BIT_SEL_POS                    13  // xh98xx new
#define GPADC_RSV2_POS                          17
#define GPADC_AVG_CFG_POS                       20
#define GPADC_DIGI_CLK_FREQ_POS                 24
#define GPADC_AUTO_PD1_MASK                     0x00000001
#define GPADC_AUTO_DELAY_MASK                   0x00000002
#define GPADC_AUTO_PD2_MASK                     0x00000004
#define GPADC_TRIG_MODE_MASK                    0x00000030
#define GPADC_TRIG_RES_MASK                     0x00000040
#define GPADC_RSV0_MASK                         0x00000080
#define GPADC_TRIG_HW_SEL_MASK                  0x00000F00
#define GPADC_DMA_EN_MASK                       0x00001000
#define GPADC_GP_BIT_SEL_MASK                   0x00006000  // xh98xx new
#define GPADC_RSV2_MASK                         0x00020000
#ifdef CONFIG_XH98xxD
#define GPADC_AVG_CFG_MASK                      0x00300000
#else
#define GPADC_AVG_CFG_MASK                      0x00700000
#endif
#define GPADC_DIGI_CLK_FREQ_MASK                0xFF000000
// ADC_CFG2
#define GPADC_SCANDIR_POS                       0
#ifdef CONFIG_XH98xxD
#define GPADC_SEQ_VECT_POS                      4
#else
#define GPADC_SEQ_VECT_POS                      1
#endif
#define GPADC_SEQ_LIFE_POS                      16
#define GPADC_SCANDIR_MASK                      0x00000001
#ifdef CONFIG_XH98xxD
#define GPADC_SEQ_VECT_MASK                     0x00000FF0
#else
#define GPADC_SEQ_VECT_MASK                     0x00000FFE
#endif
#define GPADC_SEQ_LIFE_MASK                     0xFFFF0000
// ADC_SW_TRIGGER
#define GPADC_SW_TRIGGER_POS                    0
#define GPADC_SW_TRIGGER_TRUE_POS               31
#define GPADC_SW_TRIGGER_MASK                   0x00000001
#define GPADC_SW_TRIGGER_TRUE_MASK              0x80000000
// CH_0_CFG ~ CH_7_CFG
#ifdef CONFIG_XH98xxD
#define GPADC_MODE_VERF_POS                     0
#define GPADC_BP_POS                            1
#define GPADC_MODE_VERF_MASK                    0x00000001
#define GPADC_BP_MASK                           0x80000000
#endif
#ifdef CONFIG_XH98xxF
#define GPADC_MODE_VERF_POS                     0
#define GPADC_BP_POS                            1
#define GPADC_MODE_VERF_MASK                    0x00000001
#define GPADC_BP_MASK                           0x00000002
#endif
#ifdef CONFIG_XH98xx
#define GPADC_MODE_VERF_POS                     0
#define GPADC_MODE_VERF_MASK                    0x00000003
#endif

#if defined(CONFIG_XH98xxD) || defined(CONFIG_XH98xxF)
// GAIN_ERR
#define GPADC_GAIN_ERR_15_POS                   0
#define GPADC_GAIN_ERR_25_POS                   16
#define GPADC_GAIN_ERR_15_MASK                  0x000003FF
#define GPADC_GAIN_ERR_25_MASK                  0x03FF0000
#elif defined(CONFIG_XH98xx)
// GAIN_ERR_REG0
#define GPADC_GAIN_ERR_125_POS                   0
#define GPADC_GAIN_ERR_175_POS                   16
#define GPADC_GAIN_ERR_125_MASK                  0x00003FFF
#define GPADC_GAIN_ERR_175_MASK                  0x3FFF0000
// GAIN_ERR_REG1
#define GPADC_GAIN_ERR_200_POS                   0
#define GPADC_GAIN_ERR_200_MASK                  0x00003FFF
#endif

#if defined(CONFIG_XH98xxD) || defined(CONFIG_XH98xxF)
// GAIN_VOS
#define GPADC_VOS_15_POS                   0
#define GPADC_VOS_25_POS                   16
#define GPADC_VOS_15_MASK                  0x000003FF
#define GPADC_VOS_25_MASK                  0x03FF0000
#elif defined(CONFIG_XH98xx)
// GAIN_VOS_REG0
#define GPADC_VOS_125_POS                   0
#define GPADC_VOS_175_POS                   16
#define GPADC_VOS_125_MASK                  0x00003FFF
#define GPADC_VOS_175_MASK                  0x3FFF0000
// GAIN_VOS_REG0
#define GPADC_VOS_200_POS                   0
#define GPADC_VOS_200_MASK                  0x00003FFF
#endif

// GAIN_VOS_TEMP
#if defined(CONFIG_XH98xxD) || defined(CONFIG_XH98xxF)
#define GPADC_VOS_TEMP_15_POS              0
#define GPADC_VOS_TEMP_25_POS              16
#define GPADC_VOS_TEMP_15_MASK             0x0000007F
#define GPADC_VOS_TEMP_25_MASK             0x007F0000
#elif defined(CONFIG_XH98xx)
#define GPADC_VOS_TEMP_125_POS              0
#define GPADC_VOS_TEMP_175_POS              8
#define GPADC_VOS_TEMP_200_POS              16
#define GPADC_TEMP_SEL_POS                  24
#define GPADC_PD_GP_TEMP_GEN_POS            27
#define GPADC_VOS_TEMP_125_MASK             0x000000FF
#define GPADC_VOS_TEMP_175_MASK             0x0000FF00
#define GPADC_VOS_TEMP_200_MASK             0x00FF0000
#define GPADC_TEMP_SEL_MASK                 0x07000000
#define GPADC_PD_GP_TEMP_GEN_MASK           0x08000000
#endif

// GAIN_SMP_OUT_CTL
#define GPADC_AUTO_COMPEN_POS                   0
#define GPADC_AUTO_COMPEN_MASK                  0x00000001

// CH_0_DATA ~ CH_7_DATA CH_DMA_DATA
#define GPADC_DATA_LOW_POS                      0
#define GPADC_DATA_LOW_MASK                     0x0000FFFF

// CH_DMA_DATA
#define GPADC_CH_DMA_DATA_POS 0
#define GPADC_CH_DMA_DATA_MASK 0xFFFFFFFF

// DMA_CNS
#define GPADC_DFSM_POS                          0
#define GPADC_CH_ID_DMA_POS                     4
#ifdef CONFIG_XH98xxD
#define GPADC_DFSM_MASK                         0x00000001
#else
#define GPADC_DFSM_MASK                         0x00000003
#endif
#define GPADC_CH_ID_DMA_MASK                    0x000000F0

// ADC_DBG0
#define GPADC_SFSM_POS                          0
#define GPADC_CH_ID_SMP_POS                     4
#define GPADC_DATA_RDY_VECT_POS                 8
#ifdef CONFIG_XH98xxD
#define GPADC_SEQ_LIFE_CNT_POS                  16
#else
#define GPADC_SEQ_LIFE_CNT_POS                  18
#endif
#define GPADC_SFSM_MASK                         0x0000000F
#define GPADC_CH_ID_SMP_MASK                    0x000000F0
#ifdef CONFIG_XH98xxD
#define GPADC_DATA_RDY_VECT_MASK                0x001FFF00
#define GPADC_SEQ_LIFE_CNT_MASK                 0xFFFF0000
#else
#define GPADC_DATA_RDY_VECT_MASK                0x0003FF00
#define GPADC_SEQ_LIFE_CNT_MASK                 0xFFFC0000
#endif

// ADC_DBG3
#define GPADC_PD_GP_POWER_MO_POS                0
#define GPADC_PD_GP_POWER_ME_POS                16

#define GPADC_PD_GP_MO_POS                      4
#define GPADC_PD_GP_ME_POS                      20

#define GPADC_RST_MO_POS                        5
#define GPADC_RST_ME_POS                        21

#define GPADC_PD_GP_POWER_MO_MASK               0x00000001
#define GPADC_PD_GP_POWER_ME_MASK               0x00010000

#define GPADC_PD_GP_MO_MASK                     0x00000008
#define GPADC_PD_GP_ME_MASK                     0x00080000

#define GPADC_RST_MO_MASK                       0x00000020
#define GPADC_RST_ME_MASK                       0x00200000

// CMPN_S_VOS_G0_LUT_REG0 ~ ...
#define GPADC_LUT_0_POS                         0
#define GPADC_LUT_1_POS                         16
#define GPADC_LUT_0_MASK                        0x00000FFF
#define GPADC_LUT_1_MASK                        0x0FFF0000
// CMPN_S_VOS_G0_LUT_REG1 ~ ...
#define GPADC_LUT_2_POS                         0
#define GPADC_LUT_3_POS                         16
#define GPADC_LUT_2_MASK                        0x00000FFF
#define GPADC_LUT_3_MASK                        0x0FFF0000
// CMPN_CTRL
#define GPADC_COMPEN_EN_POS                     0
#define GPADC_SEL_COMPEN_REG_POS                4
#define GPADC_SEL_COMPEN_ME_POS                 5
#define GPADC_FOUR_FIFTH_FIX_POS                6
#define GPADC_VCM_MN_POS                        15
#define GPADC_VCM_REG_POS                       16
#define GPADC_COMPEN_EN_MASK                    0x00000001
#define GPADC_SEL_COMPEN_REG_MASK               0x00000010
#define GPADC_SEL_COMPEN_ME_MASK                0x00000020
#define GPADC_FOUR_FIFTH_FIX_MASK               0x00000040
#define GPADC_VCM_MN_MASK                       0x00008000
#define GPADC_VCM_REG_MASK                      0xFFFF0000

// FILTER_CTRL
#define GPADC_GP_CIC_ORD_POS                    0
#define GPADC_GP_CIC_OSR_POS                    4
#define GPADC_GP_CIC_SCALE_POS                  8
#define GPADC_GP_CIC_SHIFT_POS                  12
#define GPADC_GP_IIR_1ST_BPS_POS                16
#define GPADC_GP_IIR_2ND_BPS_POS                17
#define BPADC_GP_IIR_3RD_BPS_POS                18
#define GPADC_OUT_VAILD_CNT_POS                 20

#define GPADC_GP_CIC_ORD_MASK                   0x00000003
#define GPADC_GP_CIC_OSR_MASK                   0x00000070
#define GPADC_GP_CIC_SCALE_MASK                 0x00000700
#define GPADC_GP_CIC_SHIFT_MASK                 0x00003000
#define GPADC_GP_IIR_1ST_BPS_MASK               0x00010000
#define GPADC_GP_IIR_2ND_BPS_MASK               0x00020000
#define BPADC_GP_IIR_3RD_BPS_MASK               0x00040000
#define GPADC_OUT_VAILD_CNT_MASK                0x0FF00000

/******************************************************************************/
/*                                                                            */
/*                                    SF                                      */
/*                                                                            */
/******************************************************************************/
// CONFIGURATION.CTRL
#define SF_CTRL_DMA_SEL_POS          27
#define SF_CTRL_BYTE_RW_EN_POS       26
#define SF_CTRL_ALGIN_3BYTE_EN_POS   25
#define SF_CTRL_DMA_DIRECTY_POS      24
#define SF_CTRL_LCD_RD_EN_POS        23
#define SF_CTRL_RGB_MODE_POS         21
#define SF_CTRL_LCD_SPI_CTRL_POS     18
#define SF_CTRL_WIDTH_POS            16
#define SF_CTRL_FE_DLY_SAMPLE_POS    14
#define SF_CTRL_DLY_SAMPLE_POS       12
#define SF_CTRL_DLYX_SAMPLE_POS      12
#define SF_CTRL_BP_CLOCK_DIV_POS     10
#define SF_CTRL_CPOL_POS             9
#define SF_CTRL_CPHA_POS             8
#define SF_CTRL_MODE_POS             8
#define SF_CTRL_CLOCK_DIV_POS        0
#define SF_CTRL_DMA_SEL_MASK         0x08000000
#define SF_CTRL_BYTE_RW_EN_MASK      0x04000000
#define SF_CTRL_ALGIN_3BYTE_EN_MASK  0x02000000
#define SF_CTRL_DMA_DIRECTY_MASK     0x01000000
#define SF_CTRL_LCD_RD_EN_MASK       0x00800000
#define SF_CTRL_RGB_MODE_MASK        0x00600000
#define SF_CTRL_LCD_SPI_CTRL_MASK    0x001C0000
#define SF_CTRL_WIDTH_MASK           0x00030000
#define SF_CTRL_DLYX_SAMPLE_MASK     0x00007000
#define SF_CTRL_FE_DLY_SAMPLE_MASK   0x00004000
#define SF_CTRL_DLY_SAMPLE_MASK      0x00003000
#define SF_CTRL_BP_CLOCK_DIV_MASK    0x00000400
#define SF_CTRL_CPOL_MASK            0x00000200
#define SF_CTRL_CPHA_MASK            0x00000100
#define SF_CTRL_MODE_MASK            0x00000300
#define SF_CTRL_CLOCK_DIV_MASK       0x000000FF
// CONFIGURATION.CS
#define SF_CS_RECOVER_POS            24
#define SF_CS_HOLD_POS               16
#define SF_CS_SETUP_POS              8
#define SF_CS_POL_POS                0
#define SF_CS_RECOVER_MASK           0xFF000000
#define SF_CS_HOLD_MASK              0x00FF0000
#define SF_CS_SETUP_MASK             0x0000FF00
#define SF_CS_POL_MASK               0x00000001
// SW_SPI_CFG0_REG
#define SF_SW_CFG0_DUMMY_CYCLE_CNT_POS     24
#define SF_SW_CFG0_CMD_P1_BIT_CNT_POS      16
#define SF_SW_CFG0_CMD_P1_BUS_WIDTH_POS    12
#define SF_SW_CFG0_CMD_P0_BUS_WIDTH_POS    8
#define SF_SW_CFG0_CMD_P0_BIT_CNT_POS      0
#define SF_SW_CFG0_DUMMY_CYCLE_CNT_MASK    0xFF000000
#define SF_SW_CFG0_CMD_P1_BIT_CNT_MASK     0x003F0000
#define SF_SW_CFG0_CMD_P1_BUS_WIDTH_MASK   0x00003000
#define SF_SW_CFG0_CMD_P0_BUS_WIDTH_MASK   0x00000300
#define SF_SW_CFG0_CMD_P0_BIT_CNT_MASK     0x0000007F
// SW_SPI_CFG1_REG
#define SF_SW_CFG1_SW_CFG_EN_POS           31
#define SF_SW_CFG1_BUF_WIDTH_BYTES_POS     24
#define SF_SW_CFG1_SDATA_BUS_WIDTH_POS     20
#define SF_SW_CFG1_SDATA_BYTE_CNT_POS      0
#define SF_SW_CFG1_SW_CFG_EN_MASK          0x80000000
#define SF_SW_CFG1_BUF_WIDTH_BYTES_MASK    0x07000000
#define SF_SW_CFG1_SDATA_BUS_WIDTH_MASK    0x00300000
#define SF_SW_CFG1_SDATA_BYTE_CNT_MASK     0x000FFFFF

/******************************************************************************/
/*                                                                            */
/*                                    Encoder                                 */
/*                                                                            */
/******************************************************************************/
// ENCODER_EN
#define ENCODER_EN_IEN_POS          1
#define ENCODER_EN_CEN_POS          0
#define ENCODER_EN_IEN_MASK         0x00000002
#define ENCODER_EN_CEN_MASK         0x00000001
// ENCODER_MODE
#define ENCODER_MODE_CC2P_POS       3
#define ENCODER_MODE_CC1P_POS       2
#define ENCODER_MODE_SMS_POS        0
#define ENCODER_MODE_CC2P_MASK      0x00000008
#define ENCODER_MODE_CC1P_MASK      0x00000004
#define ENCODER_MODE_SMS_MASK       0x00000003
// ENCODER_OF
#define ENCODER_OF_DIR_POS          2
#define ENCODER_OF_OVERFLOW_POS     1
#define ENCODER_OF_UNDERFLOW_POS    0
#define ENCODER_OF_DIR_MASK         0x00000004
#define ENCODER_OF_OVERFLOW_MASK    0x00000002
#define ENCODER_OF_UNDERFLOW_MASK   0x00000001

/******************************************************************************/
/*                                                                            */
/*                                    Timer                                   */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for TIM_CR1 register  ********************/
#define  TIM_CR1_CEN                         ((uint16_t)0x0001)            /*!<Counter enable */
#define  TIM_CR1_UDIS                        ((uint16_t)0x0002)            /*!<Update disable */
#define  TIM_CR1_URS                         ((uint16_t)0x0004)            /*!<Update request source */
#define  TIM_CR1_OPM                         ((uint16_t)0x0008)            /*!<One pulse mode */
#define  TIM_CR1_DIR                         ((uint16_t)0x0010)            /*!<Direction */
#define  TIM_CR1_DIR_MASK                    ((uint16_t)0x0010)            /*!<Direction */

#define  TIM_CR1_CMS                         ((uint16_t)0x0060)            /*!<CMS[1:0] bits (Center-aligned mode selection) */
#define  TIM_CR1_CMS_0                       ((uint16_t)0x0020)            /*!<Bit 0 */
#define  TIM_CR1_CMS_1                       ((uint16_t)0x0040)            /*!<Bit 1 */

#define  TIM_CR1_ARPE                        ((uint16_t)0x0080)            /*!<Auto-reload preload enable */

#define  TIM_CR1_CKD                         ((uint16_t)0x0300)            /*!<CKD[1:0] bits (clock division) */
#define  TIM_CR1_CKD_0                       ((uint16_t)0x0100)            /*!<Bit 0 */
#define  TIM_CR1_CKD_1                       ((uint16_t)0x0200)            /*!<Bit 1 */

/*******************  Bit definition for TIM_CR2 register  ********************/
#define  TIM_CR2_CCPC                        ((uint16_t)0x0001)            /*!<Capture/Compare Preloaded Control */
#define  TIM_CR2_CCUS                        ((uint16_t)0x0004)            /*!<Capture/Compare Control Update Selection */
#define  TIM_CR2_CCDS                        ((uint16_t)0x0008)            /*!<Capture/Compare DMA Selection */

#define  TIM_CR2_MMS                         ((uint16_t)0x0070)            /*!<MMS[2:0] bits (Master Mode Selection) */
#define  TIM_CR2_MMS_0                       ((uint16_t)0x0010)            /*!<Bit 0 */
#define  TIM_CR2_MMS_1                       ((uint16_t)0x0020)            /*!<Bit 1 */
#define  TIM_CR2_MMS_2                       ((uint16_t)0x0040)            /*!<Bit 2 */

#define  TIM_CR2_TI1S                        ((uint16_t)0x0080)            /*!<TI1 Selection */
#define  TIM_CR2_OIS1                        ((uint16_t)0x0100)            /*!<Output Idle state 1 (OC1 output) */
#define  TIM_CR2_OIS1N                       ((uint16_t)0x0200)            /*!<Output Idle state 1 (OC1N output) */
#define  TIM_CR2_OIS2                        ((uint16_t)0x0400)            /*!<Output Idle state 2 (OC2 output) */
#define  TIM_CR2_OIS2N                       ((uint16_t)0x0800)            /*!<Output Idle state 2 (OC2N output) */
#define  TIM_CR2_OIS3                        ((uint16_t)0x1000)            /*!<Output Idle state 3 (OC3 output) */
#define  TIM_CR2_OIS3N                       ((uint16_t)0x2000)            /*!<Output Idle state 3 (OC3N output) */
#define  TIM_CR2_OIS4                        ((uint16_t)0x4000)            /*!<Output Idle state 4 (OC4 output) */

/*******************  Bit definition for TIM_SMCR register  *******************/
#define  TIM_SMCR_SMS                        ((uint16_t)0x0007)            /*!<SMS[2:0] bits (Slave mode selection) */
#define  TIM_SMCR_SMS_0                      ((uint16_t)0x0001)            /*!<Bit 0 */
#define  TIM_SMCR_SMS_1                      ((uint16_t)0x0002)            /*!<Bit 1 */
#define  TIM_SMCR_SMS_2                      ((uint16_t)0x0004)            /*!<Bit 2 */
#define _TIM_SMCR_SMS(n) ((((n) & 7) << 0) | (((n) >> 3) << 16))

#define  TIM_SMCR_TS                         ((uint16_t)0x0070)            /*!<TS[2:0] bits (Trigger selection) */
#define  TIM_SMCR_TS_0                       ((uint16_t)0x0010)            /*!<Bit 0 */
#define  TIM_SMCR_TS_1                       ((uint16_t)0x0020)            /*!<Bit 1 */
#define  TIM_SMCR_TS_2                       ((uint16_t)0x0040)            /*!<Bit 2 */
#define _TIM_SMCR_TS(n)                ((n) << 4)

#define  TIM_SMCR_MSM                        ((uint16_t)0x0080)            /*!<Master/slave mode */

#define  TIM_SMCR_ETF                        ((uint16_t)0x0F00)            /*!<ETF[3:0] bits (External trigger filter) */
#define  TIM_SMCR_ETF_0                      ((uint16_t)0x0100)            /*!<Bit 0 */
#define  TIM_SMCR_ETF_1                      ((uint16_t)0x0200)            /*!<Bit 1 */
#define  TIM_SMCR_ETF_2                      ((uint16_t)0x0400)            /*!<Bit 2 */
#define  TIM_SMCR_ETF_3                      ((uint16_t)0x0800)            /*!<Bit 3 */

#define  TIM_SMCR_ETPS                       ((uint16_t)0x3000)            /*!<ETPS[1:0] bits (External trigger prescaler) */
#define  TIM_SMCR_ETPS_0                     ((uint16_t)0x1000)            /*!<Bit 0 */
#define  TIM_SMCR_ETPS_1                     ((uint16_t)0x2000)            /*!<Bit 1 */

#define  TIM_SMCR_ECE                        ((uint16_t)0x4000)            /*!<External clock enable */
#define  TIM_SMCR_ETP                        ((uint16_t)0x8000)            /*!<External trigger polarity */

/*******************  Bit definition for TIM_DIER register  *******************/
#define  TIM_DIER_UIE                        ((uint16_t)0x0001)            /*!<Update interrupt enable */
#define  TIM_DIER_CC1IE                      ((uint16_t)0x0002)            /*!<Capture/Compare 1 interrupt enable */
#define  TIM_DIER_CC2IE                      ((uint16_t)0x0004)            /*!<Capture/Compare 2 interrupt enable */
#define  TIM_DIER_CC3IE                      ((uint16_t)0x0008)            /*!<Capture/Compare 3 interrupt enable */
#define  TIM_DIER_CC4IE                      ((uint16_t)0x0010)            /*!<Capture/Compare 4 interrupt enable */
#define  TIM_DIER_COMIE                      ((uint16_t)0x0020)            /*!<COM interrupt enable */
#define  TIM_DIER_TIE                        ((uint16_t)0x0040)            /*!<Trigger interrupt enable */
#define  TIM_DIER_BIE                        ((uint16_t)0x0080)            /*!<Break interrupt enable */
#define  TIM_DIER_UDE                        ((uint16_t)0x0100)            /*!<Update DMA request enable */
#define  TIM_DIER_CC1DE                      ((uint16_t)0x0200)            /*!<Capture/Compare 1 DMA request enable */
#define  TIM_DIER_CC2DE                      ((uint16_t)0x0400)            /*!<Capture/Compare 2 DMA request enable */
#define  TIM_DIER_CC3DE                      ((uint16_t)0x0800)            /*!<Capture/Compare 3 DMA request enable */
#define  TIM_DIER_CC4DE                      ((uint16_t)0x1000)            /*!<Capture/Compare 4 DMA request enable */
#define  TIM_DIER_COMDE                      ((uint16_t)0x2000)            /*!<COM DMA request enable */
#define  TIM_DIER_TDE                        ((uint16_t)0x4000)            /*!<Trigger DMA request enable */

#define _TIM_DIER_IRQ_MASK                  (TIM_DIER_UIE   |         \
        TIM_DIER_CC1IE |         \
        TIM_DIER_CC2IE |         \
        TIM_DIER_CC3IE |         \
        TIM_DIER_CC4IE |         \
        TIM_DIER_COMIE |         \
        TIM_DIER_TIE   |         \
        TIM_DIER_BIE)

/********************  Bit definition for TIM_SR register  ********************/
#define  TIM_SR_UIF                          ((uint16_t)0x0001)            /*!<Update interrupt Flag */
#define  TIM_SR_CC1IF                        ((uint16_t)0x0002)            /*!<Capture/Compare 1 interrupt Flag */
#define  TIM_SR_CC2IF                        ((uint16_t)0x0004)            /*!<Capture/Compare 2 interrupt Flag */
#define  TIM_SR_CC3IF                        ((uint16_t)0x0008)            /*!<Capture/Compare 3 interrupt Flag */
#define  TIM_SR_CC4IF                        ((uint16_t)0x0010)            /*!<Capture/Compare 4 interrupt Flag */
#define  TIM_SR_COMIF                        ((uint16_t)0x0020)            /*!<COM interrupt Flag */
#define  TIM_SR_TIF                          ((uint16_t)0x0040)            /*!<Trigger interrupt Flag */
#define  TIM_SR_BIF                          ((uint16_t)0x0080)            /*!<Break interrupt Flag */
#define  TIM_SR_CC1OF                        ((uint16_t)0x0200)            /*!<Capture/Compare 1 Overcapture Flag */
#define  TIM_SR_CC2OF                        ((uint16_t)0x0400)            /*!<Capture/Compare 2 Overcapture Flag */
#define  TIM_SR_CC3OF                        ((uint16_t)0x0800)            /*!<Capture/Compare 3 Overcapture Flag */
#define  TIM_SR_CC4OF                        ((uint16_t)0x1000)            /*!<Capture/Compare 4 Overcapture Flag */

/*******************  Bit definition for TIM_EGR register  ********************/
#define  TIM_EGR_UG                          ((uint8_t)0x01)               /*!<Update Generation */
#define  TIM_EGR_CC1G                        ((uint8_t)0x02)               /*!<Capture/Compare 1 Generation */
#define  TIM_EGR_CC2G                        ((uint8_t)0x04)               /*!<Capture/Compare 2 Generation */
#define  TIM_EGR_CC3G                        ((uint8_t)0x08)               /*!<Capture/Compare 3 Generation */
#define  TIM_EGR_CC4G                        ((uint8_t)0x10)               /*!<Capture/Compare 4 Generation */
#define  TIM_EGR_COMG                        ((uint8_t)0x20)               /*!<Capture/Compare Control Update Generation */
#define  TIM_EGR_TG                          ((uint8_t)0x40)               /*!<Trigger Generation */
#define  TIM_EGR_BG                          ((uint8_t)0x80)               /*!<Break Generation */

/******************  Bit definition for TIM_CCMR1 register  *******************/
#define  TIM_CCMR1_CC1S                      ((uint16_t)0x0003)            /*!<CC1S[1:0] bits (Capture/Compare 1 Selection) */
#define  TIM_CCMR1_CC1S_0                    ((uint16_t)0x0001)            /*!<Bit 0 */
#define  TIM_CCMR1_CC1S_1                    ((uint16_t)0x0002)            /*!<Bit 1 */
#define _TIM_CCMR1_CC1S(n) ((n) << 0)

#define  TIM_CCMR1_OC1FE                     ((uint16_t)0x0004)            /*!<Output Compare 1 Fast enable */
#define  TIM_CCMR1_OC1PE                     ((uint16_t)0x0008)            /*!<Output Compare 1 Preload enable */

#define  TIM_CCMR1_OC1M                      ((uint16_t)0x0070)            /*!<OC1M[2:0] bits (Output Compare 1 Mode) */
#define  TIM_CCMR1_OC1M_0                    ((uint16_t)0x0010)            /*!<Bit 0 */
#define  TIM_CCMR1_OC1M_1                    ((uint16_t)0x0020)            /*!<Bit 1 */
#define  TIM_CCMR1_OC1M_2                    ((uint16_t)0x0040)            /*!<Bit 2 */
#define _TIM_CCMR1_OC1M(n) (((n) & 7) << 4) //| (((n) >> 2) << 16))

#define  TIM_CCMR1_OC1CE                     ((uint16_t)0x0080)            /*!<Output Compare 1Clear Enable */

#define  TIM_CCMR1_CC2S                      ((uint16_t)0x0300)            /*!<CC2S[1:0] bits (Capture/Compare 2 Selection) */
#define  TIM_CCMR1_CC2S_0                    ((uint16_t)0x0100)            /*!<Bit 0 */
#define  TIM_CCMR1_CC2S_1                    ((uint16_t)0x0200)            /*!<Bit 1 */
#define _TIM_CCMR1_CC2S(n) ((n) << 8)

#define  TIM_CCMR1_OC2FE                     ((uint16_t)0x0400)            /*!<Output Compare 2 Fast enable */
#define  TIM_CCMR1_OC2PE                     ((uint16_t)0x0800)            /*!<Output Compare 2 Preload enable */

#define  TIM_CCMR1_OC2M                      ((uint16_t)0x7000)            /*!<OC2M[2:0] bits (Output Compare 2 Mode) */
#define  TIM_CCMR1_OC2M_0                    ((uint16_t)0x1000)            /*!<Bit 0 */
#define  TIM_CCMR1_OC2M_1                    ((uint16_t)0x2000)            /*!<Bit 1 */
#define  TIM_CCMR1_OC2M_2                    ((uint16_t)0x4000)            /*!<Bit 2 */
#define _TIM_CCMR1_OC2M(n) (((n) & 7) << 12) //| (((n) >> 2) << 24))

#define  TIM_CCMR1_OC2CE                     ((uint16_t)0x8000)            /*!<Output Compare 2 Clear Enable */

/*----------------------------------------------------------------------------*/

#define  TIM_CCMR1_IC1PSC                    ((uint16_t)0x000C)            /*!<IC1PSC[1:0] bits (Input Capture 1 Prescaler) */
#define  TIM_CCMR1_IC1PSC_0                  ((uint16_t)0x0004)            /*!<Bit 0 */
#define  TIM_CCMR1_IC1PSC_1                  ((uint16_t)0x0008)            /*!<Bit 1 */
#define  TIM_CCMR1_IC1PSC_2                  ((uint16_t)0x000C)            /*!<Bit 0, Bit 1 */

#define  TIM_CCMR1_IC1F                      ((uint16_t)0x00F0)            /*!<IC1F[3:0] bits (Input Capture 1 Filter) */
#define  TIM_CCMR1_IC1F_0                    ((uint16_t)0x0010)            /*!<Bit 0 */
#define  TIM_CCMR1_IC1F_1                    ((uint16_t)0x0020)            /*!<Bit 1 */
#define  TIM_CCMR1_IC1F_2                    ((uint16_t)0x0040)            /*!<Bit 2 */
#define  TIM_CCMR1_IC1F_3                    ((uint16_t)0x0080)            /*!<Bit 3 */
#define _TIM_CCMR1_IC1F(n)                   ((uint16_t)((n)<<4)) 

#define  TIM_CCMR1_IC2PSC                    ((uint16_t)0x0C00)            /*!<IC2PSC[1:0] bits (Input Capture 2 Prescaler) */
#define  TIM_CCMR1_IC2PSC_0                  ((uint16_t)0x0400)            /*!<Bit 0 */
#define  TIM_CCMR1_IC2PSC_1                  ((uint16_t)0x0800)            /*!<Bit 1 */
#define  TIM_CCMR1_IC2PSC_2                  ((uint16_t)0x0C00)            /*!<Bit 1, Bit 0*/

#define  TIM_CCMR1_IC2F                      ((uint16_t)0xF000)            /*!<IC2F[3:0] bits (Input Capture 2 Filter) */
#define  TIM_CCMR1_IC2F_0                    ((uint16_t)0x1000)            /*!<Bit 0 */
#define  TIM_CCMR1_IC2F_1                    ((uint16_t)0x2000)            /*!<Bit 1 */
#define  TIM_CCMR1_IC2F_2                    ((uint16_t)0x4000)            /*!<Bit 2 */
#define  TIM_CCMR1_IC2F_3                    ((uint16_t)0x8000)            /*!<Bit 3 */
#define _TIM_CCMR1_IC2F(n)                   ((uint16_t)((n)<<12))

/******************  Bit definition for TIM_CCMR2 register  *******************/
#define  TIM_CCMR2_CC3S                      ((uint16_t)0x0003)            /*!<CC3S[1:0] bits (Capture/Compare 3 Selection) */
#define  TIM_CCMR2_CC3S_0                    ((uint16_t)0x0001)            /*!<Bit 0 */
#define  TIM_CCMR2_CC3S_1                    ((uint16_t)0x0002)            /*!<Bit 1 */
#define _TIM_CCMR2_CC3S(n)             ((n) << 0)

#define  TIM_CCMR2_OC3FE                     ((uint16_t)0x0004)            /*!<Output Compare 3 Fast enable */
#define  TIM_CCMR2_OC3PE                     ((uint16_t)0x0008)            /*!<Output Compare 3 Preload enable */

#define  TIM_CCMR2_OC3M                      ((uint16_t)0x0070)            /*!<OC3M[2:0] bits (Output Compare 3 Mode) */
#define  TIM_CCMR2_OC3M_0                    ((uint16_t)0x0010)            /*!<Bit 0 */
#define  TIM_CCMR2_OC3M_1                    ((uint16_t)0x0020)            /*!<Bit 1 */
#define  TIM_CCMR2_OC3M_2                    ((uint16_t)0x0040)            /*!<Bit 2 */
#define _TIM_CCMR2_OC3M(n) (((n) & 7) << 4) //| (((n) >> 2) << 16))

#define  TIM_CCMR2_OC3CE                     ((uint16_t)0x0080)            /*!<Output Compare 3 Clear Enable */

#define  TIM_CCMR2_CC4S                      ((uint16_t)0x0300)            /*!<CC4S[1:0] bits (Capture/Compare 4 Selection) */
#define  TIM_CCMR2_CC4S_0                    ((uint16_t)0x0100)            /*!<Bit 0 */
#define  TIM_CCMR2_CC4S_1                    ((uint16_t)0x0200)            /*!<Bit 1 */
#define _TIM_CCMR2_CC4S(n)             ((n) << 8)

#define  TIM_CCMR2_OC4FE                     ((uint16_t)0x0400)            /*!<Output Compare 4 Fast enable */
#define  TIM_CCMR2_OC4PE                     ((uint16_t)0x0800)            /*!<Output Compare 4 Preload enable */

#define  TIM_CCMR2_OC4M                      ((uint16_t)0x7000)            /*!<OC4M[2:0] bits (Output Compare 4 Mode) */
#define  TIM_CCMR2_OC4M_0                    ((uint16_t)0x1000)            /*!<Bit 0 */
#define  TIM_CCMR2_OC4M_1                    ((uint16_t)0x2000)            /*!<Bit 1 */
#define  TIM_CCMR2_OC4M_2                    ((uint16_t)0x4000)            /*!<Bit 2 */
#define _TIM_CCMR2_OC4M(n) (((n) & 7) << 12) //| (((n) >> 2) << 24))

#define  TIM_CCMR2_OC4CE                     ((uint16_t)0x8000)            /*!<Output Compare 4 Clear Enable */

/*----------------------------------------------------------------------------*/

#define  TIM_CCMR2_IC3PSC                    ((uint16_t)0x000C)            /*!<IC3PSC[1:0] bits (Input Capture 3 Prescaler) */
#define  TIM_CCMR2_IC3PSC_0                  ((uint16_t)0x0004)            /*!<Bit 0 */
#define  TIM_CCMR2_IC3PSC_1                  ((uint16_t)0x0008)            /*!<Bit 1 */
#define  TIM_CCMR2_IC3PSC_2                  ((uint16_t)0x000C)            /*!<Bit 1, Bit 0 */

#define  TIM_CCMR2_IC3F                      ((uint16_t)0x00F0)            /*!<IC3F[3:0] bits (Input Capture 3 Filter) */
#define  TIM_CCMR2_IC3F_0                    ((uint16_t)0x0010)            /*!<Bit 0 */
#define  TIM_CCMR2_IC3F_1                    ((uint16_t)0x0020)            /*!<Bit 1 */
#define  TIM_CCMR2_IC3F_2                    ((uint16_t)0x0040)            /*!<Bit 2 */
#define  TIM_CCMR2_IC3F_3                    ((uint16_t)0x0080)            /*!<Bit 3 */
#define _TIM_CCMR2_IC3F(n)                   ((uint16_t)((n)<<4))

#define  TIM_CCMR2_IC4PSC                    ((uint16_t)0x0C00)            /*!<IC4PSC[1:0] bits (Input Capture 4 Prescaler) */
#define  TIM_CCMR2_IC4PSC_0                  ((uint16_t)0x0400)            /*!<Bit 0 */
#define  TIM_CCMR2_IC4PSC_1                  ((uint16_t)0x0800)            /*!<Bit 1 */

#define  TIM_CCMR2_IC4F                      ((uint16_t)0xF000)            /*!<IC4F[3:0] bits (Input Capture 4 Filter) */
#define  TIM_CCMR2_IC4F_0                    ((uint16_t)0x1000)            /*!<Bit 0 */
#define  TIM_CCMR2_IC4F_1                    ((uint16_t)0x2000)            /*!<Bit 1 */
#define  TIM_CCMR2_IC4F_2                    ((uint16_t)0x4000)            /*!<Bit 2 */
#define  TIM_CCMR2_IC4F_3                    ((uint16_t)0x8000)            /*!<Bit 3 */

/*******************  Bit definition for TIM_CCER register  *******************/
#define  TIM_CCER_CC1E                       ((uint16_t)0x0001)            /*!<Capture/Compare 1 output enable */
#define  TIM_CCER_CC1P                       ((uint16_t)0x0002)            /*!<Capture/Compare 1 output Polarity */
#define  TIM_CCER_CC1NE                      ((uint16_t)0x0004)            /*!<Capture/Compare 1 Complementary output enable */
#define  TIM_CCER_CC1NP                      ((uint16_t)0x0008)            /*!<Capture/Compare 1 Complementary output Polarity */
#define  TIM_CCER_CC2E                       ((uint16_t)0x0010)            /*!<Capture/Compare 2 output enable */
#define  TIM_CCER_CC2P                       ((uint16_t)0x0020)            /*!<Capture/Compare 2 output Polarity */
#define  TIM_CCER_CC2NE                      ((uint16_t)0x0040)            /*!<Capture/Compare 2 Complementary output enable */
#define  TIM_CCER_CC2NP                      ((uint16_t)0x0080)            /*!<Capture/Compare 2 Complementary output Polarity */
#define  TIM_CCER_CC3E                       ((uint16_t)0x0100)            /*!<Capture/Compare 3 output enable */
#define  TIM_CCER_CC3P                       ((uint16_t)0x0200)            /*!<Capture/Compare 3 output Polarity */
#define  TIM_CCER_CC3NE                      ((uint16_t)0x0400)            /*!<Capture/Compare 3 Complementary output enable */
#define  TIM_CCER_CC3NP                      ((uint16_t)0x0800)            /*!<Capture/Compare 3 Complementary output Polarity */
#define  TIM_CCER_CC4E                       ((uint16_t)0x1000)            /*!<Capture/Compare 4 output enable */
#define  TIM_CCER_CC4P                       ((uint16_t)0x2000)            /*!<Capture/Compare 4 output Polarity */
#define  TIM_CCER_CC4NP                      ((uint16_t)0x8000)            /*!<Capture/Compare 4 Complementary output Polarity */

/*******************  Bit definition for TIM_CNT register  ********************/
#define  TIM_CNT_CNT                         ((uint16_t)0xFFFF)            /*!<Counter Value */

/*******************  Bit definition for TIM_PSC register  ********************/
#define  TIM_PSC_PSC                         ((uint16_t)0xFFFF)            /*!<Prescaler Value */

/*******************  Bit definition for TIM_ARR register  ********************/
#define  TIM_ARR_ARR                         ((uint16_t)0xFFFF)            /*!<actual auto-reload Value */

/*******************  Bit definition for TIM_RCR register  ********************/
#define  TIM_RCR_REP                         ((uint8_t)0xFF)               /*!<Repetition Counter Value */

/*******************  Bit definition for TIM_CCR1 register  *******************/
#define  TIM_CCR1_CCR1                       ((uint16_t)0xFFFF)            /*!<Capture/Compare 1 Value */
/*----------------------------------------------------------------------------*/

#define  TIM_CCMR1_IC1PSC                    ((uint16_t)0x000C)            /*!<IC1PSC[1:0] bits (Input Capture 1 Prescaler) */
#define  TIM_CCMR1_IC1PSC_0                  ((uint16_t)0x0004)            /*!<Bit 0 */
#define  TIM_CCMR1_IC1PSC_1                  ((uint16_t)0x0008)            /*!<Bit 1 */

#define  TIM_CCMR1_IC1F                      ((uint16_t)0x00F0)            /*!<IC1F[3:0] bits (Input Capture 1 Filter) */
#define  TIM_CCMR1_IC1F_0                    ((uint16_t)0x0010)            /*!<Bit 0 */
#define  TIM_CCMR1_IC1F_1                    ((uint16_t)0x0020)            /*!<Bit 1 */
#define  TIM_CCMR1_IC1F_2                    ((uint16_t)0x0040)            /*!<Bit 2 */
#define  TIM_CCMR1_IC1F_3                    ((uint16_t)0x0080)            /*!<Bit 3 */

#define  TIM_CCMR1_IC2PSC                    ((uint16_t)0x0C00)            /*!<IC2PSC[1:0] bits (Input Capture 2 Prescaler) */
#define  TIM_CCMR1_IC2PSC_0                  ((uint16_t)0x0400)            /*!<Bit 0 */
#define  TIM_CCMR1_IC2PSC_1                  ((uint16_t)0x0800)            /*!<Bit 1 */

#define  TIM_CCMR1_IC2F                      ((uint16_t)0xF000)            /*!<IC2F[3:0] bits (Input Capture 2 Filter) */
#define  TIM_CCMR1_IC2F_0                    ((uint16_t)0x1000)            /*!<Bit 0 */
#define  TIM_CCMR1_IC2F_1                    ((uint16_t)0x2000)            /*!<Bit 1 */
#define  TIM_CCMR1_IC2F_2                    ((uint16_t)0x4000)            /*!<Bit 2 */
#define  TIM_CCMR1_IC2F_3                    ((uint16_t)0x8000)            /*!<Bit 3 */

/******************  Bit definition for TIM_CCMR2 register  *******************/
#define  TIM_CCMR2_CC3S                      ((uint16_t)0x0003)            /*!<CC3S[1:0] bits (Capture/Compare 3 Selection) */
#define  TIM_CCMR2_CC3S_0                    ((uint16_t)0x0001)            /*!<Bit 0 */
#define  TIM_CCMR2_CC3S_1                    ((uint16_t)0x0002)            /*!<Bit 1 */

#define  TIM_CCMR2_OC3FE                     ((uint16_t)0x0004)            /*!<Output Compare 3 Fast enable */
#define  TIM_CCMR2_OC3PE                     ((uint16_t)0x0008)            /*!<Output Compare 3 Preload enable */

#define  TIM_CCMR2_OC3M                      ((uint16_t)0x0070)            /*!<OC3M[2:0] bits (Output Compare 3 Mode) */
#define  TIM_CCMR2_OC3M_0                    ((uint16_t)0x0010)            /*!<Bit 0 */
#define  TIM_CCMR2_OC3M_1                    ((uint16_t)0x0020)            /*!<Bit 1 */
#define  TIM_CCMR2_OC3M_2                    ((uint16_t)0x0040)            /*!<Bit 2 */

#define  TIM_CCMR2_OC3CE                     ((uint16_t)0x0080)            /*!<Output Compare 3 Clear Enable */

#define  TIM_CCMR2_CC4S                      ((uint16_t)0x0300)            /*!<CC4S[1:0] bits (Capture/Compare 4 Selection) */
#define  TIM_CCMR2_CC4S_0                    ((uint16_t)0x0100)            /*!<Bit 0 */
#define  TIM_CCMR2_CC4S_1                    ((uint16_t)0x0200)            /*!<Bit 1 */

#define  TIM_CCMR2_OC4FE                     ((uint16_t)0x0400)            /*!<Output Compare 4 Fast enable */
#define  TIM_CCMR2_OC4PE                     ((uint16_t)0x0800)            /*!<Output Compare 4 Preload enable */

#define  TIM_CCMR2_OC4M                      ((uint16_t)0x7000)            /*!<OC4M[2:0] bits (Output Compare 4 Mode) */
#define  TIM_CCMR2_OC4M_0                    ((uint16_t)0x1000)            /*!<Bit 0 */
#define  TIM_CCMR2_OC4M_1                    ((uint16_t)0x2000)            /*!<Bit 1 */
#define  TIM_CCMR2_OC4M_2                    ((uint16_t)0x4000)            /*!<Bit 2 */

#define  TIM_CCMR2_OC4CE                     ((uint16_t)0x8000)            /*!<Output Compare 4 Clear Enable */

/*----------------------------------------------------------------------------*/

#define  TIM_CCMR2_IC3PSC                    ((uint16_t)0x000C)            /*!<IC3PSC[1:0] bits (Input Capture 3 Prescaler) */
#define  TIM_CCMR2_IC3PSC_0                  ((uint16_t)0x0004)            /*!<Bit 0 */
#define  TIM_CCMR2_IC3PSC_1                  ((uint16_t)0x0008)            /*!<Bit 1 */

#define  TIM_CCMR2_IC3F                      ((uint16_t)0x00F0)            /*!<IC3F[3:0] bits (Input Capture 3 Filter) */
#define  TIM_CCMR2_IC3F_0                    ((uint16_t)0x0010)            /*!<Bit 0 */
#define  TIM_CCMR2_IC3F_1                    ((uint16_t)0x0020)            /*!<Bit 1 */
#define  TIM_CCMR2_IC3F_2                    ((uint16_t)0x0040)            /*!<Bit 2 */
#define  TIM_CCMR2_IC3F_3                    ((uint16_t)0x0080)            /*!<Bit 3 */

#define  TIM_CCMR2_IC4PSC                    ((uint16_t)0x0C00)            /*!<IC4PSC[1:0] bits (Input Capture 4 Prescaler) */
#define  TIM_CCMR2_IC4PSC_0                  ((uint16_t)0x0400)            /*!<Bit 0 */
#define  TIM_CCMR2_IC4PSC_1                  ((uint16_t)0x0800)            /*!<Bit 1 */
#define  TIM_CCMR2_IC4PSC_2                  ((uint16_t)0x0C00)            /*!<Bit 0, Bit 1 */

#define  TIM_CCMR2_IC4F                      ((uint16_t)0xF000)            /*!<IC4F[3:0] bits (Input Capture 4 Filter) */
#define  TIM_CCMR2_IC4F_0                    ((uint16_t)0x1000)            /*!<Bit 0 */
#define  TIM_CCMR2_IC4F_1                    ((uint16_t)0x2000)            /*!<Bit 1 */
#define  TIM_CCMR2_IC4F_2                    ((uint16_t)0x4000)            /*!<Bit 2 */
#define  TIM_CCMR2_IC4F_3                    ((uint16_t)0x8000)            /*!<Bit 3 */
#define _TIM_CCMR2_IC4F(n)                   ((uint16_t)((n)<<12)) 

/*******************  Bit definition for TIM_CCER register  *******************/
#define  TIM_CCER_CC1E                       ((uint16_t)0x0001)            /*!<Capture/Compare 1 output enable */
#define  TIM_CCER_CC1P                       ((uint16_t)0x0002)            /*!<Capture/Compare 1 output Polarity */
#define  TIM_CCER_CC1NE                      ((uint16_t)0x0004)            /*!<Capture/Compare 1 Complementary output enable */
#define  TIM_CCER_CC1NP                      ((uint16_t)0x0008)            /*!<Capture/Compare 1 Complementary output Polarity */
#define  TIM_CCER_CC2E                       ((uint16_t)0x0010)            /*!<Capture/Compare 2 output enable */
#define  TIM_CCER_CC2P                       ((uint16_t)0x0020)            /*!<Capture/Compare 2 output Polarity */
#define  TIM_CCER_CC2NE                      ((uint16_t)0x0040)            /*!<Capture/Compare 2 Complementary output enable */
#define  TIM_CCER_CC2NP                      ((uint16_t)0x0080)            /*!<Capture/Compare 2 Complementary output Polarity */
#define  TIM_CCER_CC3E                       ((uint16_t)0x0100)            /*!<Capture/Compare 3 output enable */
#define  TIM_CCER_CC3P                       ((uint16_t)0x0200)            /*!<Capture/Compare 3 output Polarity */
#define  TIM_CCER_CC3NE                      ((uint16_t)0x0400)            /*!<Capture/Compare 3 Complementary output enable */
#define  TIM_CCER_CC3NP                      ((uint16_t)0x0800)            /*!<Capture/Compare 3 Complementary output Polarity */
#define  TIM_CCER_CC4E                       ((uint16_t)0x1000)            /*!<Capture/Compare 4 output enable */
#define  TIM_CCER_CC4P                       ((uint16_t)0x2000)            /*!<Capture/Compare 4 output Polarity */
#define  TIM_CCER_CC4NP                      ((uint16_t)0x8000)            /*!<Capture/Compare 4 Complementary output Polarity */
#define  TIM_CCER_CCE(n)                     ((uint16_t)(0x0001<<((n)*4)))
#define  TIM_CCER_CCP(n)                     ((uint16_t)(0x0002<<((n)*4)))
#define  TIM_CCER_CCNE(n)                    ((uint16_t)(0x0004<<((n)*4)))
#define  TIM_CCER_CCNP(n)                    ((uint16_t)(0x0008<<((n)*4)))

/*******************  Bit definition for TIM_CNT register  ********************/
#define  TIM_CNT_CNT                         ((uint16_t)0xFFFF)            /*!<Counter Value */

/*******************  Bit definition for TIM_PSC register  ********************/
#define  TIM_PSC_PSC                         ((uint16_t)0xFFFF)            /*!<Prescaler Value */

/*******************  Bit definition for TIM_ARR register  ********************/
#define  TIM_ARR_ARR                         ((uint16_t)0xFFFF)            /*!<actual auto-reload Value */

/*******************  Bit definition for TIM_RCR register  ********************/
#define  TIM_RCR_REP                         ((uint8_t)0xFF)               /*!<Repetition Counter Value */

/*******************  Bit definition for TIM_CCR1 register  *******************/
#define  TIM_CCR1_CCR1                       ((uint16_t)0xFFFF)            /*!<Capture/Compare 1 Value */

/*******************  Bit definition for TIM_CCR2 register  *******************/
#define  TIM_CCR2_CCR2                       ((uint16_t)0xFFFF)            /*!<Capture/Compare 2 Value */

/*******************  Bit definition for TIM_CCR3 register  *******************/
#define  TIM_CCR3_CCR3                       ((uint16_t)0xFFFF)            /*!<Capture/Compare 3 Value */

/*******************  Bit definition for TIM_CCR4 register  *******************/
#define  TIM_CCR4_CCR4                       ((uint16_t)0xFFFF)            /*!<Capture/Compare 4 Value */

/*******************  Bit definition for TIM_BDTR register  *******************/
#define  TIM_BDTR_DTG                        ((uint16_t)0x00FF)            /*!<DTG[0:7] bits (Dead-Time Generator set-up) */
#define  TIM_BDTR_DTG_0                      ((uint16_t)0x0001)            /*!<Bit 0 */
#define  TIM_BDTR_DTG_1                      ((uint16_t)0x0002)            /*!<Bit 1 */
#define  TIM_BDTR_DTG_2                      ((uint16_t)0x0004)            /*!<Bit 2 */
#define  TIM_BDTR_DTG_3                      ((uint16_t)0x0008)            /*!<Bit 3 */
#define  TIM_BDTR_DTG_4                      ((uint16_t)0x0010)            /*!<Bit 4 */
#define  TIM_BDTR_DTG_5                      ((uint16_t)0x0020)            /*!<Bit 5 */
#define  TIM_BDTR_DTG_6                      ((uint16_t)0x0040)            /*!<Bit 6 */
#define  TIM_BDTR_DTG_7                      ((uint16_t)0x0080)            /*!<Bit 7 */

#define  TIM_BDTR_LOCK                       ((uint16_t)0x0300)            /*!<LOCK[1:0] bits (Lock Configuration) */
#define  TIM_BDTR_LOCK_0                     ((uint16_t)0x0100)            /*!<Bit 0 */
#define  TIM_BDTR_LOCK_1                     ((uint16_t)0x0200)            /*!<Bit 1 */

#define  TIM_BDTR_OSSI                       ((uint16_t)0x0400)            /*!<Off-State Selection for Idle mode */
#define  TIM_BDTR_OSSR                       ((uint16_t)0x0800)            /*!<Off-State Selection for Run mode */
#define  TIM_BDTR_BKE                        ((uint16_t)0x1000)            /*!<Break enable */
#define  TIM_BDTR_BKP                        ((uint16_t)0x2000)            /*!<Break Polarity */
#define  TIM_BDTR_AOE                        ((uint16_t)0x4000)            /*!<Automatic Output enable */
#define  TIM_BDTR_MOE                        ((uint16_t)0x8000)            /*!<Main Output enable */

/*******************  Bit definition for TIM_DCR register  ********************/
#define  TIM_DCR_DBA                         ((uint16_t)0x001F)            /*!<DBA[4:0] bits (DMA Base Address) */
#define  TIM_DCR_DBA_0                       ((uint16_t)0x0001)            /*!<Bit 0 */
#define  TIM_DCR_DBA_1                       ((uint16_t)0x0002)            /*!<Bit 1 */
#define  TIM_DCR_DBA_2                       ((uint16_t)0x0004)            /*!<Bit 2 */
#define  TIM_DCR_DBA_3                       ((uint16_t)0x0008)            /*!<Bit 3 */
#define  TIM_DCR_DBA_4                       ((uint16_t)0x0010)            /*!<Bit 4 */

#define  TIM_DCR_DBL                         ((uint16_t)0x1F00)            /*!<DBL[4:0] bits (DMA Burst Length) */
#define  TIM_DCR_DBL_0                       ((uint16_t)0x0100)            /*!<Bit 0 */
#define  TIM_DCR_DBL_1                       ((uint16_t)0x0200)            /*!<Bit 1 */
#define  TIM_DCR_DBL_2                       ((uint16_t)0x0400)            /*!<Bit 2 */
#define  TIM_DCR_DBL_3                       ((uint16_t)0x0800)            /*!<Bit 3 */
#define  TIM_DCR_DBL_4                       ((uint16_t)0x1000)            /*!<Bit 4 */

/*******************  Bit definition for TIM_DMAR register  *******************/
#define  TIM_DMAR_DMAB                       ((uint16_t)0xFFFF)            /*!<DMA register for burst accesses */

/*******************  Bit definition for TIM_OR register  *********************/
#define TIM_OR_TI4_RMP                       ((uint16_t)0x00C0)            /*!<TI4_RMP[1:0] bits (TIM5 Input 4 remap) */
#define TIM_OR_TI4_RMP_0                     ((uint16_t)0x0040)            /*!<Bit 0 */
#define TIM_OR_TI4_RMP_1                     ((uint16_t)0x0080)            /*!<Bit 1 */
#define TIM_OR_ITR1_RMP                      ((uint16_t)0x0C00)            /*!<ITR1_RMP[1:0] bits (TIM2 Internal trigger 1 remap) */
#define TIM_OR_ITR1_RMP_0                    ((uint16_t)0x0400)            /*!<Bit 0 */
#define TIM_OR_ITR1_RMP_1                    ((uint16_t)0x0800)            /*!<Bit 1 */

/******************************************************************************/
/*                                                                            */
/*                        Serial Peripheral Interface                         */
/*                                                                            */
/******************************************************************************/

/*******************  Bit definition for SPI_CTRL0 register  ******************/
#define  SPI_CTRL0_DFS_MSK                   ((uint16_t)0x000F)            /*!< data frame size = n+1 */
#define  SPI_CTRL0_DFS_SFT                   0
#define  SPI_CTRL0_DFS(x)                    (((x) - 1) << SPI_CTRL0_DFS_SFT)
#define  SPI_CTRL0_FRF_MOTO                  (0x0 << 4)                    /*!< frame format: motorola SPI */
#define  SPI_CTRL0_FRF_TI                    (0x1 << 4)                    /*!<               TI SSP */
#define  SPI_CTRL0_FRF_NS                    (0x2 << 4)                    /*!<               NS Microwire */
#define  SPI_CTRL0_SCPH                      (1UL << 6)                    /*!< serial clock phase */
#define  SPI_CTRL0_SCPOL                     (1UL << 7)                    /*!< serial clock polarity */
#define  SPI_CTRL0_TMOD_TXRX                 (0x0 << 8)                    /*!< transer mode: transmit & receive */
#define  SPI_CTRL0_TMOD_TX                   (0x1 << 8)                    /*!<               transmit */
#define  SPI_CTRL0_TMOD_RX                   (0x2 << 8)                    /*!<               receive */
#define  SPI_CTRL0_TMOD_EEPROM               (0x3 << 8)                    /*!<               eeprom */
#define  SPI_CTRL0_SLV_OE                    (1UL << 10)                   /*!< 0: enabele, 1: disable slave txd */
#define  SPI_CTRL0_SRL                       (1UL << 11)                   /*!< 0: normal,  1: loopback */
#define  SPI_CTRL0_CFS_MSK                   ((uint16_t)0xF000)            /*!< control frame size = n+1 */
#define  SPI_CTRL0_CFS_SFT                   12
#define  SPI_CTRL0_CFS(x)                    (((x) - 1) << SPI_CTRL0_CFS_SFT)

/*******************  Bit definition for SPI_CTRL1 register  ******************/
#define  SPI_CTRL1_NDF(x)                    ((x) - 1)                     /*!< number of data frames = n + 1 */

/*******************  Bit definition for SPI_SSIENR register  ****************/
#define  SPI_SSIENR_EN                       (1UL << 0)                    /*!< 0: disable, halt transfer, flush FIFOs, program CTRLx; 1: enabel */

/*******************  Bit definition for SPI_MWCR register  ******************/

/*******************  Bit definition for SPI_SER register  *******************/
#define  SPI_SER(n)                          (1UL << (n))                  /*!< slave select enable */

/*******************  Bit definition for SPI_BAUDR register  *****************/
#define  SPI_BAUDR_SCKDV_MSK                 ((uint16)0xFFFF)              /*!< clock divider: Fspi_clk = Fsclk / SCKDV */

/*******************  Bit definition for SPI_TXFTLR register ****************/
#define  SPI_TXFTLR_TFT(n)                   (n)                           /*!< transmit FIFO threshold: txe_intr if <= n */

/*******************  Bit definition for SPI_RXFTLR register ****************/
#define  SPI_RXFTLR_RFT(n)                   (n)                           /*!< receive FIFO threshold: rxf_intr if > n */

/*******************  Bit definition for SPI_SR register ********************/
#define  SPI_SR_BUSY                         (1UL << 0)                    /*!< 0: idle or disabled; 1: transfering data */
#define  SPI_SR_TFNF                         (1UL << 1)                    /*!< 0: tFIFO is full;      1: not full */
#define  SPI_SR_TFE                          (1UL << 2)                    /*!< 0: tFIFO is not empty; 1: empty */
#define  SPI_SR_RFNE                         (1UL << 3)                    /*!< 0: rFIFO is empty;     1: not empty */
#define  SPI_SR_RFF                          (1UL << 4)                    /*!< 0: rFIFO is not full;  1: full */
#define  SPI_SR_TXE                          (1UL << 5)                    /*!< 0: no error;         1: transmission error */
#define  SPI_SR_DCOL                         (1UL << 6)                    /*!< 0: no error;         1: transmit data collision error */

/*******************  Bit definition for SPI_IMR register *******************/
#define  SPI_IMR_TXEIM                       (1UL << 0)                    /*!< tFIFO empty interrupt mask */
#define  SPI_IMR_TXOIM                       (1UL << 1)                    /*!< tFIFO overflow interrupt mask */
#define  SPI_IMR_RXUIM                       (1UL << 2)                    /*!< rFIFO underflow interrupt mask */
#define  SPI_IMR_RXOIM                       (1UL << 3)                    /*!< rFIFO overflow interrupt mask */
#define  SPI_IMR_RXFIM                       (1UL << 4)                    /*!< rFIFO full interrupt mask */
#define  SPI_IMR_MSTIM                       (1UL << 5)                    /*!< multi-maskter contention interrupt mask */

/*******************  Bit definition for SPI_ISR register *******************/
/*******************  Bit definition for SPI_RISR register ******************/

/******************************************************************************/
/*                                                                            */
/*                      Inter-integrated Circuit Interface                    */
/*                                                                            */
/******************************************************************************/

/* Control register defines */
#define I2C_CON_MASTER                       (1UL << 0)
#define I2C_CON_SPEED_STD                    (1UL << 1)
#define I2C_CON_SPEED_FAST                   (2UL << 1)
#define I2C_CON_SPEED_HIGH                   (3UL << 1)
#define I2C_CON_SPEED_MASK                   (3UL << 1)
#define I2C_CON_10BITADDR_SLAVE              (1UL << 3)
#define I2C_CON_10BITADDR_MASTER             (1UL << 4)
#define I2C_CON_RESTART_EN                   (1UL << 5)
#define I2C_CON_SLAVE_DISABLE                (1UL << 6)

/* Interrupt status & mask registers defines */
#define I2C_INTR_RX_UNDER                    (1UL << 0)
#define I2C_INTR_RX_OVER                     (1UL << 1)
#define I2C_INTR_RX_FULL                     (1UL << 2)
#define I2C_INTR_TX_OVER                     (1UL << 3)
#define I2C_INTR_TX_EMPTY                    (1UL << 4)
#define I2C_INTR_RD_REQ                      (1UL << 5)
#define I2C_INTR_TX_ABRT                     (1UL << 6)
#define I2C_INTR_RX_DONE                     (1UL << 7)
#define I2C_INTR_ACTIVITY                    (1UL << 8)
#define I2C_INTR_STOP_DET                    (1UL << 9)
#define I2C_INTR_START_DET                   (1UL << 10)
#define I2C_INTR_GEN_CALL                    (1UL << 11)

/* Status register defines */
#define I2C_STATUS_ACTIVITY                  (1UL << 0)

/* Abort source register defines */
#define I2C_TX_ABRT_7B_ADDR_NOACK            (1UL << 0)
#define I2C_TX_ABRT_10ADDR1_NOACK            (1UL << 1)
#define I2C_TX_ABRT_10ADDR2_NOACK            (1UL << 2)
#define I2C_TX_ABRT_TXDATA_NOACK             (1UL << 3)
#define I2C_TX_ABRT_GCALL_NOACK              (1UL << 4)
#define I2C_TX_ABRT_GCALL_READ               (1UL << 5)
#define I2C_TX_ABRT_SBYTE_ACKDET             (1UL << 7)
#define I2C_TX_ABRT_SBYTE_NORSTRT            (1UL << 9)
#define I2C_TX_ABRT_10B_RD_NORSTRT           (1UL << 10)
#define I2C_TX_ABRT_MASTER_DIS               (1UL << 11)
#define I2C_TX_ARB_LOST                      (1UL << 12)

#define I2C_DMA_CR_TDMAE                     (1UL << 1)
#define I2C_DMA_CR_RDMAE                     (1UL << 0)
#define I2C_CON1_TX_ENABLE                   (0UL << 12)
#define I2C_CON1_RX_ENABLE                   (1UL << 12)
#define I2C_CON1_READBYTES_UPDATE            (1UL << 16)
#define I2C_CON1_CLEAR_I2C_ENABLE            (1UL << 31)

/******************************************************************************/
/*                                                                            */
/*                     Universal Asynchronous Receiver Transmitter            */
/*                                                                            */
/******************************************************************************/
/*
 * These are the definitions for the FIFO Control Register
 */
#define UART_FCR_FIFO_EN    0x01 /* Fifo enable */
#define UART_FCR_CLEAR_RCVR 0x02 /* Clear the RCVR FIFO */
#define UART_FCR_CLEAR_XMIT 0x04 /* Clear the XMIT FIFO */
#define UART_FCR_DMA_SELECT 0x08 /* For DMA applications */
#define UART_FCR_TRIGGER_MASK   0xC0 /* Mask for the FIFO trigger range */
#define UART_FCR_TRIGGER_1  0x00 /* Mask for trigger set at 1 */
#define UART_FCR_TRIGGER_4  0x40 /* Mask for trigger set at 4 */
#define UART_FCR_TRIGGER_8  0x80 /* Mask for trigger set at 8 */
#define UART_FCR_TRIGGER_14 0xC0 /* Mask for trigger set at 14 */

#define UART_FCR_RXSR       0x02 /* Receiver soft reset */
#define UART_FCR_TXSR       0x04 /* Transmitter soft reset */

/*
 * These are the definitions for the Modem Control Register
 */
#define UART_MCR_DTR    0x01        /* DTR   */
#define UART_MCR_RTS    0x02        /* RTS   */
#define UART_MCR_OUT1   0x04        /* Out 1 */
#define UART_MCR_OUT2   0x08        /* Out 2 */
#define UART_MCR_AFCE   0x20        /* Auto Flow Control Enable */
#define UART_MCR_LOOP   0x10        /* Enable loopback test mode */

#define UART_MCR_DMA_EN 0x04
#define UART_MCR_TX_DFR 0x08

/*
 * These are the definitions for the Line Control Register
 *
 * Note: if the word length is 5 bits (UART_LCR_WLEN5), then setting
 * UART_LCR_STOP will select 1.5 stop bits, not 2 stop bits.
 */
#define UART_LCR_WLS_MSK 0x03       /* character length select mask */
#define UART_LCR_WLS_5  0x00        /* 5 bit character length */
#define UART_LCR_WLS_6  0x01        /* 6 bit character length */
#define UART_LCR_WLS_7  0x02        /* 7 bit character length */
#define UART_LCR_WLS_8  0x03        /* 8 bit character length */
#define UART_LCR_STB    0x04        /* Number of stop Bits, off = 1, on = 1.5 or 2) */
#define UART_LCR_PEN    0x08        /* Parity eneble */
#define UART_LCR_EPS    0x10        /* Even Parity Select */
#define UART_LCR_STKP   0x20        /* Stick Parity */
#define UART_LCR_SBRK   0x40        /* Set Break */
#define UART_LCR_BKSE   0x80        /* Bank select enable */
#define UART_LCR_DLAB   0x80        /* Divisor latch access bit */

/*
 * These are the definitions for the Line Status Register
 */
#define UART_LSR_DR 0x01        /* Data ready */
#define UART_LSR_OE 0x02        /* Overrun */
#define UART_LSR_PE 0x04        /* Parity error */
#define UART_LSR_FE 0x08        /* Framing error */
#define UART_LSR_BI 0x10        /* Break */
#define UART_LSR_THRE   0x20        /* Xmit holding register empty */
#define UART_LSR_TEMT   0x40        /* Xmitter empty */
#define UART_LSR_ERR    0x80        /* Error */

#define UART_MSR_DCD    0x80        /* Data Carrier Detect */
#define UART_MSR_RI 0x40        /* Ring Indicator */
#define UART_MSR_DSR    0x20        /* Data Set Ready */
#define UART_MSR_CTS    0x10        /* Clear to Send */
#define UART_MSR_DDCD   0x08        /* Delta DCD */
#define UART_MSR_TERI   0x04        /* Trailing edge ring indicator */
#define UART_MSR_DDSR   0x02        /* Delta DSR */
#define UART_MSR_DCTS   0x01        /* Delta CTS */

/*
 * These are the definitions for the Interrupt Identification Register
 */
#define UART_IIR_NO_INT 0x01    /* No interrupts pending */
#define UART_IIR_ID 0x0F    /* Mask for the interrupt ID */

#define UART_IIR_MSI    0x00    /* Modem status interrupt */
#define UART_IIR_THRI   0x02    /* Transmitter holding register empty */
#define UART_IIR_RDI    0x04    /* Receiver data interrupt */
#define UART_IIR_RLSI   0x06    /* Receiver line status interrupt */
#define UART_IIR_BDI    0x07    /* busy detect */
#define UART_IIR_CTI    0x0C    /* character timeout */

/*
 * These are the definitions for the Interrupt Enable Register
 */
#define UART_IER_MSI    0x08    /* Enable Modem status interrupt */
#define UART_IER_RLSI   0x04    /* Enable receiver line status interrupt */
#define UART_IER_THRI   0x02    /* Enable Transmitter holding register int. */
#define UART_IER_RDI    0x01    /* Enable receiver data interrupt */

/* useful defaults for LCR */
#define UART_LCR_8N1    0x03

// UART0_CFG UART2_CFG
#define UART_CON_TX_IRQ_EN_POS              9
#define UART_CON_RX_IRQ_EN_POS              8
#define UART_CON_SM_MD_POS                  6
#define UART_CON_SM_EN_POS                  5
#define UART_CON_RX_EN_POS                  4
#define UART_CON_TX_BT_POS                  3
#define UART_CON_RX_BT_POS                  2
#define UART_CON_TX_INT_FLG_POS             1
#define UART_CON_RX_INT_FLG_POS             0
#define UART_CON_TX_IRQ_EN_MASK             0x00000200
#define UART_CON_RX_IRQ_EN_MASK             0x00000100
#define UART_CON_SM_MD_MASK                 0x000000C0
#define UART_CON_SM_EN_MASK                 0x00000020
#define UART_CON_RX_EN_MASK                 0x00000010
#define UART_CON_TX_BT_MASK                 0x00000008
#define UART_CON_RX_BT_MASK                 0x00000004
#define UART_CON_TX_INT_FLG_MASK            0x00000002
#define UART_CON_RX_INT_FLG_MASK            0x00000001

#define UART_PCON_SMOD_WRITE_POS            7
#define UART_PCON_SMOD_READ_POS             0
#define UART_PCON_SMOD_WRITE_MASK           0x00000080
#define UART_PCON_SMOD_READ_MASK            0x00000001

#define UART_ACON_BAUD_BIT_POS            7
#define UART_ACON_BAUD_BIT_MASK           0x00000080

#define UART_RELL_POS            0
#define UART_RELL_MASK           0x000000FF

#define UART_RELH_POS            0
#define UART_RELH_MASK           0x00000003


// HCache
#define HCACHE_CONFIG_GCLKDIS_MASK  0x00000001
#define HCACHE_CTRL_CEN_MASK        0x00000001
#define HCACHE_STATUS_CSTS_MASK     0x00000001
#define HCACHE_MAINT0_INVALL_MASK   0x00000001


/******************************************************************************/
/*                                                                            */
/*                                  Audio                                     */
/*                                                                            */
/******************************************************************************/
// ADC_CTRL
#define AU_CIC_ONLY_POS                     23
#define AU_ADC_CIC_3ORD_POS                 22
#define AU_ADC_CIC_2ORD_POS                 21
#define AU_ADC_CIC_1ORD_POS                 20
#define AU_ADC_OUT_SEL_POS                  19
#define AU_ADC_CIC_4ORD_POS                 18
#define AU_ADC_ANTI_CLIP_POS                17
#define AU_ADC_INPUT_INV_POS                16
#define AU_ADC_SR_32K_POS                   14
#define AU_ADC_SR_44K_POS                   13
#define AU_ADC_SR_48K_POS                   12
#define AU_ADC_SR_4X_POS                    10
#define AU_ADC_SR_2X_POS                    9
#define AU_ADC_SR_1X_POS                    8
#define AU_ADC_24B_EN_POS                   7
#define AU_ADC_CIC_SCALE_POS                4
#define AU_ADC_DC_EN_POS                    3
#define AU_DMIC_EN_POS                      2
#define AU_ADC_SW_RESET_X_POS               1
#define AU_ADC_EN_POS                       0
#define AU_CIC_ONLY_MASK                    0x00800000
#define AU_ADC_CIC_3ORD_MASK                0x00400000
#define AU_ADC_CIC_2ORD_MASK                0x00200000
#define AU_ADC_CIC_1ORD_MASK                0x00100000
#define AU_ADC_OUT_SEL_MASK                 0x00080000
#define AU_ADC_CIC_4ORD_MASK                0x00040000
#define AU_ADC_ANTI_CLIP_MASK               0x00020000
#define AU_ADC_INPUT_INV_MASK               0x00010000
#define AU_ADC_SR_32K_MASK                  0x00004000
#define AU_ADC_SR_44K_MASK                  0x00002000
#define AU_ADC_SR_48K_MASK                  0x00001000
#define AU_ADC_SR_4X_MASK                   0x00000400
#define AU_ADC_SR_2X_MASK                   0x00000200
#define AU_ADC_SR_1X_MASK                   0x00000100
#define AU_ADC_24B_EN_MASK                  0x00000080
#define AU_ADC_CIC_SCALE_MASK               0x00000070
#define AU_ADC_DC_EN_MASK                   0x00000008
#define AU_DMIC_EN_MASK                     0x00000004
#define AU_ADC_SW_RESET_X_MASK              0x00000002
#define AU_ADC_EN_MASK                      0x00000001
// ADC_DECI_FILT_11
#define AU_ADC_DECI_B1_A2_POS               16
#define AU_ADC_DECI_B1_A1_POS               0
#define AU_ADC_DECI_B1_A2_MASK              0xFFFF0000
#define AU_ADC_DECI_B1_A1_MASK              0x0000FFFF
// ADC_DECI_FILT_12
#define AU_ADC_DECI_B1_B1_POS               16
#define AU_ADC_DECI_B1_B0_POS               0
#define AU_ADC_DECI_B1_B1_MASK              0xFFFF0000
#define AU_ADC_DECI_B1_B0_MASK              0x0000FFFF
// ADC_DECI_FILT_13
#define AU_ADC_DECI_B1_B2_POS               0
#define AU_ADC_DECI_B1_B2_MASK              0x0000FFFF
// ADC_DECI_FILT_21
#define AU_ADC_DECI_B2_A2_POS               16
#define AU_ADC_DECI_B2_A1_POS               0
#define AU_ADC_DECI_B2_A2_MASK              0xFFFF0000
#define AU_ADC_DECI_B2_A1_MASK              0x0000FFFF
// ADC_DECI_FILT_22
#define AU_ADC_DECI_B2_B1_POS               16
#define AU_ADC_DECI_B2_B0_POS               0
#define AU_ADC_DECI_B2_B1_MASK              0xFFFF0000
#define AU_ADC_DECI_B2_B0_MASK              0x0000FFFF
// ADC_DECI_FILT_23
#define AU_ADC_DECI_B2_B2_POS               0
#define AU_ADC_DECI_B2_B2_MASK              0x0000FFFF
// ADC_DECI_FILT_31
#define AU_ADC_DECI_B3_A2_POS               16
#define AU_ADC_DECI_B3_A1_POS               0
#define AU_ADC_DECI_B3_A2_MASK              0xFFFF0000
#define AU_ADC_DECI_B3_A1_MASK              0x0000FFFF
// ADC_DECI_FILT_32
#define AU_ADC_DECI_B3_B1_POS               16
#define AU_ADC_DECI_B3_B0_POS               0
#define AU_ADC_DECI_B3_B1_MASK              0xFFFF0000
#define AU_ADC_DECI_B3_B0_MASK              0x0000FFFF
// ADC_DECI_FILT_33
#define AU_ADC_DECI_B3_B2_POS               0
#define AU_ADC_DECI_B3_B2_MASK              0x0000FFFF
// ADC_DECI_FILT_41
#define AU_ADC_DECI_B4_A2_POS               16
#define AU_ADC_DECI_B4_A1_POS               0
#define AU_ADC_DECI_B4_A2_MASK              0xFFFF0000
#define AU_ADC_DECI_B4_A1_MASK              0x0000FFFF
// ADC_DECI_FILT_42
#define AU_ADC_DECI_B4_B1_POS               16
#define AU_ADC_DECI_B4_B0_POS               0
#define AU_ADC_DECI_B4_B1_MASK              0xFFFF0000
#define AU_ADC_DECI_B4_B0_MASK              0x0000FFFF
// ADC_DECI_FILT_43
#define AU_ADC_DECI_B4_B2_POS               0
#define AU_ADC_DECI_B4_B2_MASK              0x0000FFFF
// ADC_DECI_FILT_51
#define AU_ADC_DECI_B5_A2_POS               16
#define AU_ADC_DECI_B5_A1_POS               0
#define AU_ADC_DECI_B5_A2_MASK              0xFFFF0000
#define AU_ADC_DECI_B5_A1_MASK              0x0000FFFF
// ADC_DECI_FILT_52
#define AU_ADC_DECI_B5_B1_POS               16
#define AU_ADC_DECI_B5_B0_POS               0
#define AU_ADC_DECI_B5_B1_MASK              0xFFFF0000
#define AU_ADC_DECI_B5_B0_MASK              0x0000FFFF
// ADC_DECI_FILT_53
#define AU_ADC_DECI_B5_B2_POS               0
#define AU_ADC_DECI_B5_B2_MASK              0x0000FFFF
// ADC_IIR_FILT_11
#define AU_ADC_IIR_B1_A2_POS                16
#define AU_ADC_IIR_B1_A1_POS                0
#define AU_ADC_IIR_B1_A2_MASK               0xFFFF0000
#define AU_ADC_IIR_B1_A1_MASK               0x0000FFFF
// ADC_IIR_FILT_12
#define AU_ADC_IIR_B1_B1_POS                16
#define AU_ADC_IIR_B1_B0_POS                0
#define AU_ADC_IIR_B1_B1_MASK               0xFFFF0000
#define AU_ADC_IIR_B1_B0_MASK               0x0000FFFF
// ADC_IIR_FILT_13
#define AU_ADC_IIR_BPS_POS                  16
#define AU_ADC_IIR_B1_B2_POS                0
#define AU_ADC_IIR_BPS_MASK                 0x00010000
#define AU_ADC_IIR_B1_B2_MASK               0x0000FFFF
// ADC_IIR_FILT_21
#define AU_ADC_IIR_B2_A2_POS                16
#define AU_ADC_IIR_B2_A1_POS                0
#define AU_ADC_IIR_B2_A2_MASK               0xFFFF0000
#define AU_ADC_IIR_B2_A1_MASK               0x0000FFFF
// ADC_IIR_FILT_22
#define AU_ADC_IIR_B2_B1_POS                16
#define AU_ADC_IIR_B2_B0_POS                0
#define AU_ADC_IIR_B2_B1_MASK               0xFFFF0000
#define AU_ADC_IIR_B2_B0_MASK               0x0000FFFF
// ADC_IIR_FILT_23
#define AU_ADC_IIR_B2_B2_POS                0
#define AU_ADC_IIR_B2_B2_MASK               0x0000FFFF
// ADC_IIR_FILT_31
#define AU_ADC_IIR_B3_A2_POS                16
#define AU_ADC_IIR_B3_A1_POS                0
#define AU_ADC_IIR_B3_A2_MASK               0xFFFF0000
#define AU_ADC_IIR_B3_A1_MASK               0x0000FFFF
// ADC_IIR_FILT_32
#define AU_ADC_IIR_B3_B1_POS                16
#define AU_ADC_IIR_B3_B0_POS                0
#define AU_ADC_IIR_B3_B1_MASK               0xFFFF0000
#define AU_ADC_IIR_B3_B0_MASK               0x0000FFFF
// ADC_IIR_FILT_33
#define AU_ADC_IIR_B3_B2_POS                0
#define AU_ADC_IIR_B3_B2_MASK               0x0000FFFF
// ADC_DRC_CTRL_1
#define AU_ADC_DRC_AT1_POS                  16
#define AU_ADC_DRC_AT0_POS                  0
#define AU_ADC_DRC_AT1_MASK                 0xFFFF0000
#define AU_ADC_DRC_AT0_MASK                 0x0000FFFF
// ADC_DRC_CTRL_2
#define AU_ADC_DRC_RT1_POS                  16
#define AU_ADC_DRC_RT0_POS                  0
#define AU_ADC_DRC_RT1_MASK                 0xFFFF0000
#define AU_ADC_DRC_RT0_MASK                 0x0000FFFF
// ADC_DRC_CTRL_3
#define AU_ADC_DRC_GAIN_JITTER_LIMIT_POS    28
#define AU_ADC_DRC_NT_POS                   24
#define AU_ADC_DRC_ET_POS                   16
#define AU_ADC_DRC_CT_POS                   8
#define AU_ADC_DRC_LT_POS                   2
#define AU_ADC_DRC_MODE_SEL_POS             0
#define AU_ADC_DRC_GAIN_JITTER_LIMIT_MASK   0xF0000000
#define AU_ADC_DRC_NT_MASK                  0x07000000
#define AU_ADC_DRC_ET_MASK                  0x007F0000
#define AU_ADC_DRC_CT_MASK                  0x00007F00
#define AU_ADC_DRC_LT_MASK                  0x0000007C
#define AU_ADC_DRC_MODE_SEL_MASK            0x00000003
// ADC_DRC_CTRL_4
#define AU_ADC_DRC_GAIN_NEG_LIMIT_POS       24
#define AU_ADC_DRC_GAIN_POS_LIMIT_POS       16
#define AU_ADC_DRC_GAIN_SHFT_POS            10
#define AU_ADC_DRC_ES_INV_POS               9
#define AU_ADC_DRC_NS_MODE_POS              8
#define AU_ADC_DRC_DELAY_POS                6
#define AU_ADC_DRC_ES_POS                   3
#define AU_ADC_DRC_CS_POS                   0
#define AU_ADC_DRC_GAIN_NEG_LIMIT_MASK      0xFF000000
#define AU_ADC_DRC_GAIN_POS_LIMIT_MASK      0x00FF0000
#define AU_ADC_DRC_GAIN_SHFT_MASK           0x00000C00
#define AU_ADC_DRC_ES_INV_MASK              0x00000200
#define AU_ADC_DRC_NS_MODE_MASK             0x00000100
#define AU_ADC_DRC_DELAY_MASK               0x000000C0
#define AU_ADC_DRC_ES_MASK                  0x00000038
#define AU_ADC_DRC_CS_MASK                  0x00000007
// ADC_DRC_CTRL_5
#define AU_ADC_DRC_RMS_EN_POS               16
#define AU_ADC_DRC_TAV_POS                  0
#define AU_ADC_DRC_RMS_EN_MASK              0x00010000
#define AU_ADC_DRC_TAV_MASK                 0x0000FFFF
// ADC_VOL_CTRL
#define AU_ADC_UNMUTE_RATE_POS              28
#define AU_ADC_VOL_UPDATE_POS               24
#define AU_LADCVOL_POS                      8
#define AU_ADC_MUTE_RATE_POS                4
#define AU_ADC_MUTE_BYPASS_POS              2
#define AU_ADCUNMU_POS                      1
#define AU_ADCMU_POS                        0
#define AU_ADC_UNMUTE_RATE_MASK             0xF0000000
#define AU_ADC_VOL_UPDATE_MASK              0x01000000
#define AU_LADCVOL_MASK                     0x0000FF00
#define AU_ADC_MUTE_RATE_MASK               0x000000F0
#define AU_ADC_MUTE_BYPASS_MASK             0x00000004
#define AU_ADCUNMU_MASK                     0x00000002
#define AU_ADCMU_MASK                       0x00000001
// CODEC_GAIN_READ
#define AU_ADC_GAINL_MON_POS                0
#define AU_ADC_GAINL_MON_MASK               0x000000FF
// CODEC_CLK_CTRL_1
#define AU_DMIC_CLK_CTRL_POS                6
#define AU_DMIC_CLK_SEL_POS                 5
#define AU_CLK_32K_EN_POS                   4
#define AU_I2S_RXCLK_RSTN_REG_POS           3
#define AU_I2S_TXCLK_RSTN_REG_POS           2
#define AU_CODEC_CLK_EN_REG_POS             1
#define AU_CODEC_RSTN_REG_POS               0
#define AU_DMIC_CLK_CTRL_MASK               0x00000040
#define AU_DMIC_CLK_SEL_MASK                0x00000020
#define AU_CLK_32K_EN_MASK                  0x00000010
#define AU_I2S_RXCLK_RSTN_REG_MASK          0x00000008
#define AU_I2S_TXCLK_RSTN_REG_MASK          0x00000004
#define AU_CODEC_CLK_EN_REG_MASK            0x00000002
#define AU_CODEC_RSTN_REG_MASK              0x00000001
// CODEC_CLK_CTRL_2
#define AU_I2S_TX_CLK_DONE_POS              24
#define AU_I2S_TX_HIGH_NUM_POS              12
#define AU_I2S_TX_ODD_POS                   8
#define AU_I2S_TX_DIV_EN_POS                7
#define AU_I2S_TX_DIV_SEL_POS               6
#define AU_I2S_TX_DIV_COEFF_POS             0
#define AU_I2S_TX_CLK_DONE_MASK             0x01000000
#define AU_I2S_TX_HIGH_NUM_MASK             0x001FF000
#define AU_I2S_TX_ODD_MASK                  0x00000100
#define AU_I2S_TX_DIV_EN_MASK               0x00000080
#define AU_I2S_TX_DIV_SEL_MASK              0x00000040
#define AU_I2S_TX_DIV_COEFF_MASK            0x0000003F
// CODEC_CLK_CTRL_3
#define AU_I2S_RX_CLK_DONE_POS              24
#define AU_I2S_RX_HIGH_NUM_POS              12
#define AU_I2S_RX_ODD_POS                   8
#define AU_I2S_RX_DIV_EN_POS                7
#define AU_I2S_RX_DIV_SEL_POS               6
#define AU_I2S_RX_DIV_COEFF_POS             0
#define AU_I2S_RX_CLK_DONE_MASK             0x01000000
#define AU_I2S_RX_HIGH_NUM_MASK             0x001FF000
#define AU_I2S_RX_ODD_MASK                  0x00000100
#define AU_I2S_RX_DIV_EN_MASK               0x00000080
#define AU_I2S_RX_DIV_SEL_MASK              0x00000040
#define AU_I2S_RX_DIV_COEFF_MASK            0x0000003F
// ADC_DITHER_CTRL_1
#define AU_ADC_DITHER_OUT_EN_POS            16
#define AU_PNOISE_EN_POS                    15
#define AU_ADC_PEAK_MODE_POS                14
#define AU_ADC_DITHER_EN_POS                13
#define AU_ADC_DITHER_PD_POS                12
#define AU_ADC_DITHER_OUT_EN_MASK           0x00010000
#define AU_PNOISE_EN_MASK                   0x00008000
#define AU_ADC_PEAK_MODE_MASK               0x00004000
#define AU_ADC_DITHER_EN_MASK               0x00002000
#define AU_ADC_DITHER_PD_MASK               0x00001000
// ADC_DITHER_CTRL_2
#define AU_ADC_DITHER_RT_POS                16
#define AU_ADC_DITHER_AT_POS                0
#define AU_ADC_DITHER_RT_MASK               0xFFFF0000
#define AU_ADC_DITHER_AT_MASK               0x0000FFFF
// CIC_ONLY_CTRL
#define AU_CIC_ONLY_TIMER_POS               0
#define AU_CIC_ONLY_TIMER_MASK              0x000FFFFF
// CODEC_INT_STATUS
#define AU_CIC_ONLY_INT_STATUS_POS          7
#define AU_AUDIO_OFF_INT_POS                6
#define AU_AUDIO_ON_INT_POS                 5
#define AU_CODEC_INT_POS                    4
#define AU_ADC_UNMUTE_INT_POS               3
#define AU_ADC_MUTE_INT_POS                 2
#define AU_ADC_OUTL_CLIP_INT_POS            1
#define AU_ADC_SIGNAL_LARGE_INT_POS         0
#define AU_CIC_ONLY_INT_STATUS_MASK         0x00000080
#define AU_AUDIO_OFF_INT_MASK               0x00000040
#define AU_AUDIO_ON_INT_MASK                0x00000020
#define AU_CODEC_INT_MASK                   0x00000010
#define AU_ADC_UNMUTE_INT_MASK              0x00000008
#define AU_ADC_MUTE_INT_MASK                0x00000004
#define AU_ADC_OUTL_CLIP_INT_MASK           0x00000002
#define AU_ADC_SIGNAL_LARGE_INT_MASK        0x00000001
// CODEC_INT_CTRL
#define AU_CIC_ONLY_INT_MASK_POS            7
#define AU_AUDIO_OFF_INT_MASK_POS           6
#define AU_AUDIO_ON_INT_MASK_POS            5
#define AU_ADC_UNMUTE_INT_MASK_POS          3
#define AU_ADC_MUTE_INT_MASK_POS            2
#define AU_ADC_CLIP_INT_MASK_POS            1
#define AU_ADC_SIGNAL_LARGE_INT_IM_POS      0
#define AU_CIC_ONLY_INT_MASK_MASK           0x00000080
#define AU_AUDIO_OFF_INT_MASK_MASK          0x00000040
#define AU_AUDIO_ON_INT_MASK_MASK           0x00000020
#define AU_ADC_UNMUTE_INT_MASK_MASK         0x00000008
#define AU_ADC_MUTE_INT_MASK_MASK           0x00000004
#define AU_ADC_CLIP_INT_MASK_MASK           0x00000002
#define AU_ADC_SIGNAL_LARGE_INT_IM_MASK     0x00000001
// CIC_ONLY_RDATA
#define AU_CIC_ONLY_OUT_31_0_POS            0
#define AU_CIC_ONLY_OUT_31_0_MASK           0xFFFFFFFF
// CIC_ONLY_RDATA_1
#define AU_CIC_ONLY_OUT_36_32_POS           0
#define AU_CIC_ONLY_OUT_36_32_MASK          0x0000001F
// RSVD_1
#define AU_RSVD_REG_1_POS                   0
#define AU_RSVD_REG_1_MASK                  0xFFFFFFFF
// ADC_PEAK_READ
#define AU_ADC_PEAK_R_POS                   0
#define AU_ADC_PEAK_L_POS                   0
#define AU_ADC_PEAK_R_MASK                  0xFFFFFFFF
#define AU_ADC_PEAK_L_MASK                  0x0000FFFF
// AEC_DELAY_CFG
#define AU_AEC_DELAY_CNT_POS                16
#define AU_SIGNAL_AMP_THD_POS               0
#define AU_AEC_DELAY_CNT_MASK               0xFFFF0000
#define AU_SIGNAL_AMP_THD_MASK              0x0000FFFF
// IF_CTRL
#define AU_I2S_TXCLK_MST_INV_POS            7
#define AU_I2S_RXCLK_MST_INV_POS            6
#define AU_I2S_TX_MS_SEL_POS                5
#define AU_I2S_RX_MS_SEL_POS                4
#define AU_I2S_TX_CNT_EN_POS                3
#define AU_I2S_CON_CTRL_POS                 2
#define AU_TRANSMIT_EN_POS                  1
#define AU_RECEIVE_EN_POS                   0
#define AU_I2S_TXCLK_MST_INV_MASK           0x00000080
#define AU_I2S_RXCLK_MST_INV_MASK           0x00000040
#define AU_I2S_TX_MS_SEL_MASK               0x00000020
#define AU_I2S_RX_MS_SEL_MASK               0x00000010
#define AU_I2S_TX_CNT_EN_MASK               0x00000008
#define AU_I2S_CON_CTRL_MASK                0x00000004
#define AU_TRANSMIT_EN_MASK                 0x00000002
#define AU_RECEIVE_EN_MASK                  0x00000001
// CODEC_ANA_CTRL_1
#define AU_PD_AU_CLK_REG_POS                           0
#define AU_PD_AU_PGA_REG_POS                           1
#define AU_PD_GP_PGABUF_POS                            2
#define AU_EN_PGAIN_OPEN_POS                           3
#define AU_AU_PGA_GAIN_POS                             4
#define AU_AU_SEL_PGAIN_POS                            8
#define AU_PD_AUADC_BIAS_REG_POS                       11
#define AU_PD_AUADC_CORE_REG_POS                       12
#define AU_AU_ENADC_DEM_POS                            13
#define AU_AUADC_GAIN_POS                              14
#define AU_AU_RCTUNE_POS                               16
#define AU_AUADC_TSTSEL_POS                            20
#define AU_AU_RST_ADC_REG_POS                          23
#define AU_AU_SHRT_ADC_POS                             24
#define AU_AU_EN_TST_AUADC_BIAS_POS                    25
#define AU_AU_SEL_ADC_INPUT_POS                        26
#define AU_PD_AUREF_REG_POS                            27
#define AU_AU_EN_REF_BYP_REG_POS                       28
#define AU_PD_AU_BIAS_GEN_REG_POS                      29
#define AU_SEL_INNERIREF_VBAT_AU_POS                   30
#define AU_PD_AUDIO_IREF_VBAT_AU_REG_POS               31
#define AU_PD_AU_CLK_REG_MASK                          0x00000001
#define AU_PD_AU_PGA_REG_MASK                          0x00000002
#define AU_PD_GP_PGABUF_MASK                           0x00000004
#define AU_EN_PGAIN_OPEN_MASK                          0x00000008
#define AU_AU_PGA_GAIN_MASK                            0x000000F0
#define AU_AU_SEL_PGAIN_MASK                           0x00000700
#define AU_PD_AUADC_BIAS_REG_MASK                      0x00000800
#define AU_PD_AUADC_CORE_REG_MASK                      0x00001000
#define AU_AU_ENADC_DEM_MASK                           0x00002000
#define AU_AUADC_GAIN_MASK                             0x0000C000
#define AU_AU_RCTUNE_MASK                              0x000F0000
#define AU_AUADC_TSTSEL_MASK                           0x00700000
#define AU_AU_RST_ADC_REG_MASK                         0x00800000
#define AU_AU_SHRT_ADC_MASK                            0x01000000
#define AU_AU_EN_TST_AUADC_BIAS_MASK                   0x02000000
#define AU_AU_SEL_ADC_INPUT_MASK                       0x04000000
#define AU_PD_AUREF_REG_MASK                           0x08000000
#define AU_AU_EN_REF_BYP_REG_MASK                      0x10000000
#define AU_PD_AU_BIAS_GEN_REG_MASK                     0x20000000
#define AU_SEL_INNERIREF_VBAT_AU_MASK                  0x40000000
#define AU_PD_AUDIO_IREF_VBAT_AU_REG_MASK              0x80000000
// CODEC_ANA_CTRL_2
#define AU_AUDIO_TSTSEL_POS                            0
#define AU_EN_AUDIO_BIAS_TST_POS                       3
#define AU_EN_AU_TST_POS                               4
#define AU_PD_AU_AULDO28_REG_POS                       5
#define AU_PD_AU_AULDO12_REG_POS                       6
#define AU_SEL_INNERIREF_AUVREF_VBAT_AU_POS            7
#define AU_CTRL_AU_AULDO12_POS                         8
#define AU_EN_AUREF_SRC_DEGEN_POS                      10
#define AU_CTRL_AU_AULDO28_POS                         12
#define AU_AU_LDO28_MODE_POS                           14
#define AU_EN_LDO28_OFFCHIPCAP_POS                     15
#define AU_SEL_BIAS_VBAT_AU_POS                        16
#define AU_SEL_GPADC_MUX_POS                           21
#define AU_EN_PGA_CHOPPER_POS                          22
#define AU_PD_CHOPPER_CLKGEN_POS                       23
#define AU_ADC_MODE_POS                                24
#define AU_RTH_EN_POS                                  25
#define AU_VDRV_EN_POS                                 26
#define AU_EN_GPBUF_CHOPPER_POS                        27
#define AU_EN_REF_CAPBYP_POS                           28
#define AU_SEL_GPBUF_INNER_IREF_POS                    29
#define AU_AUDIO_TSTSEL_MASK                           0x00000007
#define AU_EN_AUDIO_BIAS_TST_MASK                      0x00000008
#define AU_EN_AU_TST_MASK                              0x00000010
#define AU_PD_AU_AULDO28_REG_MASK                      0x00000020
#define AU_PD_AU_AULDO12_REG_MASK                      0x00000040
#define AU_SEL_INNERIREF_AUVREF_VBAT_AU_MASK           0x00000080
#define AU_CTRL_AU_AULDO12_MASK                        0x00000300
#define AU_EN_AUREF_SRC_DEGEN_MASK                     0x00000C00
#define AU_CTRL_AU_AULDO28_MASK                        0x00003000
#define AU_AU_LDO28_MODE_MASK                          0x00004000
#define AU_EN_LDO28_OFFCHIPCAP_MASK                    0x00008000
#define AU_SEL_BIAS_VBAT_AU_MASK                       0x001F0000
#define AU_SEL_GPADC_MUX_MASK                          0x00200000
#define AU_EN_PGA_CHOPPER_MASK                         0x00400000
#define AU_PD_CHOPPER_CLKGEN_MASK                      0x00800000
#define AU_ADC_MODE_MASK                               0x01000000
#define AU_RTH_EN_MASK                                 0x02000000
#define AU_VDRV_EN_MASK                                0x04000000
#define AU_EN_GPBUF_CHOPPER_MASK                       0x08000000
#define AU_EN_REF_CAPBYP_MASK                          0x10000000
#define AU_SEL_GPBUF_INNER_IREF_MASK                   0x20000000
// CODEC_ANA_PWR_1
#define AU_AUDIO_ADC_CTRL_POS                          10
#define AU_AUDIO_LDO_CTRL_POS                          9
#define AU_AUDIO_ON_EN_POS                             8
#define AU_PD_AUREF_AU_TIME_POS                        0
#define AU_AUDIO_ADC_CTRL_MASK                         0x00000400
#define AU_AUDIO_LDO_CTRL_MASK                         0x00000200
#define AU_AUDIO_ON_EN_MASK                            0x00000100
#define AU_PD_AUREF_AU_TIME_MASK                       0x000000FF

/**
 * Peripheral_Registers_Bits_Definition
 * @}
 */

/*********************************************************************
 * TYPEDEFS
 */

/// XH_SYS_Type
typedef struct
{
    __I  uint32_t REV_ID;               // offset:0x00
    __I  uint32_t CHIP_ID;              // offset:0x04
         uint32_t RESERVE1[3];
    __IO uint32_t SYS_TICK;             // offset:0x14
    __IO uint32_t GPIO_POWER_UP_STATUS; // offset:0x18
    __IO uint32_t GPIO_POWER_UP_STATUS_1;//offset:0x1C
    __IO uint32_t RST_32K_OSC_CTRL;     // offset:0x20
         uint32_t RESERVE3[5];
    __IO uint32_t MON;                  // offset:0x38
         uint32_t RESERVE4[1];
    __I  uint32_t CHRGR_STAT;           // offset:0x40
    __IO uint32_t SOFT_INT_SET;         // offset:0x44
    __IO uint32_t SOFT_INT_CLR;         // offset:0x48
         uint32_t RESERVE5[13];
    __IO uint32_t PINMUX[8];            // offset:0x80
}XH_SYS_Type;

/// XH_AES_Type
typedef struct
{
    __IO uint32_t DATA[4];
    __IO uint32_t KEY[4];
    __IO uint32_t START  : 1;
    __IO uint32_t RSV1   : 3;
    __IO uint32_t STATUS : 1;
    __IO uint32_t RSV2   : 27;
}XH_AES_Type;

/// XH_PMU_Type
typedef struct
{
    __IO uint32_t BASIC;              // offset:0x00
    __IO uint32_t PSO_PM;             // offset:0x04
    __IO uint32_t XTAL32M_CNS0;       // offset:0x08
    __IO uint32_t REMOVED01;          // offset:0x0C
    __IO uint32_t RAM_PM_1;           // offset:0x10
    __IO uint32_t RAM_PM_2;           // offset:0x14
    __IO uint32_t ANA_PD;             // offset:0x18
    __IO uint32_t GPIO_POL;           // offset:0x1C
    __IO uint32_t GPIO_POL_1;         // offset:0x20
    __IO uint32_t MISC_CTRL;          // offset:0x24
    __IO uint32_t WAKE_DEB;           // offset:0x28
    __IO uint32_t GPIO_OE_CTRL;       // offset:0x2C
    __IO uint32_t GPIO_OE_CTRL_1;     // offset:0x30
    __IO uint32_t GPIO_PU_CTRL;       // offset:0x34
    __IO uint32_t XTAL32M_CNS1;       // offset:0x38
    __IO uint32_t AHB_REMAP;          // offset:0x3C
    __IO uint32_t GPIO_ODA_CTRL;      // offset:0x40
    __IO uint32_t XTAL32M_CNS2;       // offset:0x44
    __IO uint32_t ANA_REG;            // offset:0x48
    __IO uint32_t CLK_CTRL_1;         // offset:0x4C
    __IO uint32_t CLK_CTRL_2;         // offset:0x50
    __IO uint32_t GPIO_IE_CTRL;       // offset:0x54
    __IO uint32_t GPIO_IE_CTRL_1;     // offset:0x58
    __IO uint32_t COMP_CTRL;          // offset:0x5C
    __IO uint32_t ANA_PD_1;           // offset:0x60
    __IO uint32_t MISC_CTRL_1;        // offset:0x64
    __IO uint32_t RAM_CTRL_1;         // offset:0x68
    __IO uint32_t RAM_CTRL_2;         // offset:0x6C
    __IO uint32_t RAM_CTRL_3;         // offset:0x70
    __IO uint32_t RAM_CTRL_4;         // offset:0x74
    __IO uint32_t RAM_CTRL_5;         // offset:0x78
    __IO uint32_t TIMER_VAL;          // offset:0x7C
    __IO uint32_t RW_EXT_WAKEUP;      // offset:0x80
    __IO uint32_t OSC_INT_CTRL;       // offset:0x84
    __IO uint32_t GPIO_STATUS_READ;   // offset:0x88
    __IO uint32_t REMOVED0;           // offset:0x8C
    __IO uint32_t GPIO_STATUS_READ_2; // offset:0x90
    __IO uint32_t WDT_STATUS;         // offset:0x94
    __IO uint32_t GPIO_WAKEUP;        // offset:0x98
    __IO uint32_t GPIO_WAKEUP_1;      // offset:0x9C
    __IO uint32_t GPIO_ODE_CTRL;      // offset:0xA0
    __IO uint32_t GPIO_ODE_CTRL_1;    // offset:0xA4
    __IO uint32_t GPIO_PD_CTRL;       // offset:0xA8
    __IO uint32_t REMOVED2;           // offset:0xAC
    __IO uint32_t GPIO_LATCH;         // offset:0xB0
    __IO uint32_t GPIO_LATCH_1;       // offset:0xB4
    __IO uint32_t GPIO_NOCLK_LATCH;   // offset:0xB8
    __IO uint32_t WDT_RLR_CFG;        // offset:0xBC
    __IO uint32_t STATUS_READ;        // offset:0xC0
    __IO uint32_t GPIO_DRV_CTRL_0;    // offset:0xC4
    __IO uint32_t GPIO_DRV_CTRL_1;    // offset:0xC8
    __IO uint32_t GPIO_DRV_CTRL_2;    // offset:0xCC
    __IO uint32_t GPIO_DRV_CTRL_3;    // offset:0xD0
    __IO uint32_t TIMER_CNT;          // offset:0xD4
    __IO uint32_t XH98xx_PID;         // offset:0xD8  // XH98xxF
    __IO uint32_t BB_DEEPSLTIME;      // offset:0xDC
    __IO uint32_t BB_FINECNT;         // offset:0xE0
    __IO uint32_t BB_CLKN;            // offset:0xE4
    __IO uint32_t WDT_KR_CFG;         // offset:0xE8
    __IO uint32_t SW_STATUS;          // offset:0xEC
    __IO uint32_t CPU_STATUS;         // offset:0xF0
    __IO uint32_t LDR_HOOK;           // offset:0xF4
    __IO uint32_t BOOT_SEL;           // offset:0xF8
    __IO uint32_t ANA_CON;            // offset:0xFC
    __IO uint32_t FLASH_LOW_VOL_CTRL_0; // offset:0x0100
    __IO uint32_t FLASH_LOW_VOL_CTRL_1; // offset:0x0104
    __IO uint32_t RC_32M_CTRL;        // offset:0x0108
    __IO uint32_t TIMER_SET;          // offset:0x010C
         uint32_t RESERVE2[0x0140/4 - 0x010C/4 - 1];
    __IO uint32_t PA_GAIN_REG[18];    // offset:0x0140
    __IO uint32_t GPIO_ADD_CTRL;      // offset:0x0188
}XH_PMU_Type;

#if defined(CONFIG_XH98xxD) || defined(CONFIG_XH98xxF)
/// XH_PSO_CPM_Type
typedef struct
{
    __IO uint32_t RESERVE0[1];      // offset:0x00
    __IO uint32_t CPU_CFG;          // offset:0x04  Don't update, WFI auto update
    __IO uint32_t APB_CFG;          // offset:0x08
    __IO uint32_t REG_UPD;          // offset:0x0C
    __IO uint32_t SF0_CFG;          // offset:0x10
    __IO uint32_t TIM_CFG[3];       // offset:0x14
    __IO uint32_t UART0_CFG;        // offset:0x20  XH98xxF, only gate_en/soft_reset
    __IO uint32_t UART1_CFG;        // offset:0x24
    __IO uint32_t RESERVE2[2];      // offset:0x28
    __IO uint32_t SPI0_CFG;         // offset:0x30
    __IO uint32_t RESERVE3[3];      // offset:0x34
    __IO uint32_t BASEBAND_CFG;     // offset:0x40
    __IO uint32_t CPU_TCLK_CFG;     // offset:0x44
    __IO uint32_t AHB_CFG;          // offset:0x48
    __IO uint32_t DMA_CFG;          // offset:0x4C
    __IO uint32_t RAM_CFG;          // offset:0x50
    __IO uint32_t RESERVE4[1];      // offset:0x54
    __IO uint32_t GPIO_CFG;         // offset:0x58
    __IO uint32_t RESERVE5[2];      // offset:0x5C
    __IO uint32_t BTPHY_CFG;        // offset:0x64
    __IO uint32_t RNG_CFG;          // offset:0x68
    __IO uint32_t RESERVE6[1];      // offset:0x6C
    __IO uint32_t STATUS_READ;      // offset:0x70
    __IO uint32_t ANA_IF_AHB_CFG;   // offset:0x74
    __IO uint32_t XH98xx_CFG;       // offset:0x78
    __IO uint32_t ANA_IF_CLK_CFG;   // offset:0x7C
    __IO uint32_t SF1_CFG;          // offset:0x80
    __IO uint32_t RESERVE7[1];      // offset:0x84
    __IO uint32_t UART2_CFG;        // offset:0x88  XH98xxF, only gate_en/soft_reset
}XH_PSO_CPM_Type;
#else
/// XH_PSO_CPM_Type
typedef struct
{
    __IO uint32_t REV;              // offset:0x00
    __IO uint32_t CPU_CFG;          // offset:0x04  Don't update, WFI auto update
    __IO uint32_t APB_CFG;          // offset:0x08
    __IO uint32_t REG_UPD;          // offset:0x0C
    __IO uint32_t SF0_CFG;          // offset:0x10
    __IO uint32_t TIM_CFG[3];       // offset:0x14
    __IO uint32_t UART0_CFG;        // offset:0x20
    __IO uint32_t UART1_CFG;        // offset:0x24
    __IO uint32_t I2C_CFG;          // offset:0x28
    __IO uint32_t IRTX_CFG;         // offset:0x2C
    __IO uint32_t BASEBAND_CFG;     // offset:0x30
    __IO uint32_t CPU_TCLK_CFG;     // offset:0x34
    __IO uint32_t AHB_CFG;          // offset:0x38
    __IO uint32_t DMA_CFG;          // offset:0x3C
    __IO uint32_t RAM_CFG;          // offset:0x40
    __IO uint32_t AUDIO_CFG;        // offset:0x44
    __IO uint32_t GPIO_CFG;         // offset:0x48
    __IO uint32_t BTPHY_CFG;        // offset:0x4C
    __IO uint32_t RNG_CFG;          // offset:0x50
    __IO uint32_t I2S_CFG;          // offset:0x54
    __IO uint32_t STATUS_READ;      // offset:0x58
    __IO uint32_t ANA_IF_AHB_CFG;   // offset:0x5C
    __IO uint32_t XH98xx_CFG;       // offset:0x60
    __IO uint32_t ANA_IF_CLK_CFG;   // offset:0x64
    __IO uint32_t EFUSE_CFG;        // offset:0x68
    __IO uint32_t SF1_CFG;          // offset:0x6C
    __IO uint32_t SPI0_CFG;         // removed
}XH_PSO_CPM_Type;
#endif

/// XH_EFUSE_Type
typedef struct {
    __I  uint8_t   READ_DATA[32];       // offset:0x00
         uint8_t   RSVD[0xF00-32];
    __IO uint32_t  PROGRAM_ENABLE;      // offset:0xF00
    __IO uint32_t  PROGRAM_START;       // offset:0xF04
    __IO uint32_t  AVDD_ENABLE_SELECT;  // offset:0xF08
    __IO uint32_t  PROGRAM_ADDRESS;     // offset:0xF0C
    __IO uint32_t  PROGRAM_DATA;        // offset:0xF10
    __IO uint32_t  READ_PROT_CODE;      // offset:0xF14
    __IO uint32_t  PROGRAM_CFG0;        // offset:0xF18
    __IO uint32_t  PROGRAM_CFG1;        // offset:0xF1C
    __IO uint32_t  PROGRAM_CFG2;        // offset:0xF20
    __IO uint32_t  READ_CFG;            // offset:0xF24
    __IO uint32_t  PROGRAM_INTR;        // offset:0xF28
    __I  uint32_t  STATUS;              // offset:0xF2C
} XH_EFUSE_Type;

/// XH_PHY_Type
typedef struct
{
    __IO uint32_t EN_SDET;                    // offset:0x00*4
    __IO uint32_t OFFSET_EN_DET_CD_PHY;       // offset:0x01*4
    __IO uint32_t OFFSET_EN_DET_UCD_PHY;      // offset:0x02*4
    __IO uint32_t OFFSET_EN_SDET;             // offset:0x03*4
    __IO uint32_t EN_FAGC;                    // offset:0x04*4
    __IO uint32_t EN_ROT2;                    // offset:0x05*4
    __IO uint32_t EN_DC_REMOVAL;              // offset:0x06*4
    __IO uint32_t EN_DAGC;                    // offset:0x07*4
    __IO uint32_t RX_RFGAIN_MAX;              // offset:0x08*4
    __IO uint32_t RX_DAGC_HYS_DB;             // offset:0x09*4
    __IO uint32_t RX_DAGC_REF;                // offset:0x0A*4
    __IO uint32_t RX_DAGC_LWIN;               // offset:0x0B*4
    __IO uint32_t DCNOTCH_K;                  // offset:0x0C*4
    __IO uint32_t DCNOTCH_K2;                 // offset:0x0D*4
    __IO uint32_t RSSI_K;                     // offset:0x0E*4
    __IO uint32_t RSSI_EST_REAL_TIME;         // offset:0x0F*4
    __IO uint32_t RSSI_EST_SEL;               // offset:0x10*4
    __IO uint32_t RSSI_CAP_MODE;              // offset:0x11*4
    __IO uint32_t RSSI_TIMEOUT_CNST;          // offset:0x12*4
    __IO uint32_t RSSI_TIMEOUT_OFFSET_CNST;   // offset:0x13*4
    __IO uint32_t FAGC_REF_DB;                // offset:0x14*4
    __IO uint32_t FAGC_LWIN;                  // offset:0x15*4
    __IO uint32_t FAGC_UPD_MODE;              // offset:0x16*4
    __IO uint32_t BUF_SETL_LEN_CODED_PHY;     // offset:0x17*4
    __IO uint32_t BUF_SET1_LEN_UNCODED_PHY;   // offset:0x18*4
    __IO uint32_t RSSI_COND_EN;               // offset:0x19*4
    __IO uint32_t RSSI_COND_TH;               // offset:0x1A*4
    __IO uint32_t PAR_TH_UNCODED_PHY;         // offset:0x1B*4
    __IO uint32_t PAR_TH_CODED_PHY;           // offset:0x1C*4
    __IO uint32_t SPWR_MIN_TH_UNCODED_PHY;    // offset:0x1D*4
    __IO uint32_t SPWR_MIN_TH_CODED_PHY;      // offset:0x1E*4
    __IO uint32_t DEM_SAT_CODED_PHY;          // offset:0x1F*4
    __IO uint32_t BW_K;                       // offset:0x20*4
    __IO uint32_t STR_K;                      // offset:0x21*4
    __IO uint32_t FD_CFO_CMP;                 // offset:0x22*4
    __IO uint32_t EN_STR;                     // offset:0x23*4
    __IO uint32_t STR_OFFSET_CD_PHY;          // offset:0x24*4
    __IO uint32_t STR_OFFSET_UCD_PHY;         // offset:0x25*4
    __IO uint32_t DEM_SAT_TH;                 // offset:0x26*4
    __IO uint32_t SDET_A;                     // offset:0x27*4
    __IO uint32_t SDET_B;                     // offset:0x28*4
    __IO uint32_t EN_CFO_TRAC_SDET;           // offset:0x29*4
    __IO uint32_t TED_CNT_500K_TH;            // offset:0x2A*4
    __IO uint32_t TED_CNT_125K_TH;            // offset:0x2B*4
    __IO uint32_t TED_LOC_500K_STEP;          // offset:0x2C*4
    __IO uint32_t TED_LOC_125K_STEP;          // offset:0x2D*4
    __IO uint32_t TED_ADJ_500K_TH;            // offset:0x2E*4
    __IO uint32_t TED_ADJ_125K_TH;            // offset:0x2F*4
    __IO uint32_t CODED_PHY_STR_SEL;          // offset:0x30*4
    __IO uint32_t EN_DEWHITENING;             // offset:0x31*4
    __IO uint32_t EN_AA_DETECTED;             // offset:0x32*4
    __IO uint32_t SIG_DBM_EST_O;              // offset:0x33*4
    __IO uint32_t RX_EN_REG;                  // offset:0x34*4
    __IO uint32_t IQ_IN_SWAP;                      // offset:0x35*4
         uint32_t RESERVED0[0x40-0x35-1];
    __IO uint32_t RX_ADDR_ERR_TH;             // offset:0x40*4
    __IO uint32_t RX_ADDR_ERR;                // offset:0x41*4
    __IO uint32_t RX_ADDR_ERR_CLR;            // offset:0x42*4
    __IO uint32_t RX_OUT;                     // offset:0x43*4
    __IO uint32_t RX_CLK_GATE;                // offset:0x44*4
    __IO uint32_t CTE_CTRL;                   // offset:0x45*4
    __IO uint32_t ANT_Q_REG[5];               // offset:0x46*4
}XH_PHY_Type;

/// XH_PRIVACY_ADDR_Type
typedef struct
{
    __IO uint32_t ADDRESS[1];    // [0 :31]     bit[31:0] of address 
    __IO uint32_t ADDRESS_H :16; // [0 :15]     bit[47:32] of address 
         uint32_t RESERVE   :15; // [16:30]
    __IO uint32_t TYPE      :1;  // [31:31]     address type 1:random address 0:public address
}XH_PRIVACY_ADDR_Type;

// XH_FILTER_ADDR_Type
typedef struct
{
    __IO uint32_t ADDRESS[1];    // [0 :31]
    __IO uint32_t ADDRESS_H :16; // [0 :15]
         uint32_t RESERVE   :13; // [16:30]
    __IO uint32_t MATCHED   :1;  // [29:29]
    __IO uint32_t TYPE      :1;  // [30:30]
    __IO uint32_t LOADED    :1;  // [31:31]
}XH_FILTER_ADDR_Type;

/// XH_LCBB_Type
typedef struct
{
    uint32_t rsv1[40];
    __IO uint32_t LPO_TRIM; //offset 0xA0
}XH_LCBB_Type;

/// XH_LEBB_Type
typedef struct
{
    __IO uint32_t CTRL0;

    struct {
        __IO uint32_t PUBLIC[2];
        __IO uint32_t RANDOM[2];
    }BDADDR;

    __IO uint32_t AA;
    __IO uint32_t CTRL1;
    __IO uint32_t CRC_INIT;

    struct {
        __IO uint32_t KEY[4];
        __IO uint32_t IV[2];
        __IO uint32_t COUNT;
        __IO uint32_t COUNT_H :8;
        __IO uint32_t LENGTH  :5;
        __IO uint32_t LLID    :2;
             uint32_t RESERVE :17;
    }CCM;

    struct {
        __IO uint32_t TX;
        __IO uint32_t RX;
    }HEADER;

         uint32_t RESERVE18;
         uint32_t RESERVE19;
         uint32_t RESERVE20;

    __IO uint32_t STATUS;
    __IO uint32_t CTRL3;

         uint32_t RESERVE23;

    struct {
        XH_FILTER_ADDR_Type LIST[XH_HW_WLIST_NUM];
        XH_FILTER_ADDR_Type DIRECT_ADDR;
        XH_FILTER_ADDR_Type LOCAL_ADDR; //LOADED not used
    }FILTER;

         uint32_t RESERVE48;

    /// Privacy
    struct {
        /// STATUS (uint32_t)
        __IO uint32_t RESOLVE_IDX    :2;
        __IO uint32_t PEER_IDA_IS_ZERO :1;
        __IO uint32_t IRKS_ARE_ZERO  :1;
        __IO uint32_t LC_RESOLVE_OK  :1;
             uint32_t RESERVE2       :3;
        __IO uint32_t RX_TAR_IS_RES  :1;
             uint32_t RESERVE3       :3;
        __IO uint32_t RX_SRC_IS_RES  :1;
             uint32_t RESERVE4       :3;
        __IO uint32_t RESOLVE_OK     :1;
             uint32_t RESERVE5       :7;
        __IO uint32_t RESOLVE_DONE   :1;
             uint32_t RESERVE6       :7;

        /// CTRL (uint32_t)
        __IO uint32_t MODE           :3;
             uint32_t RESERVE7       :5;
        __IO uint32_t LOCAL_IRK_ZERO :10;
             uint32_t RESERVE8       :2;
        __IO uint32_t LOCATED        :10;
             uint32_t RESERVE9       :1;
        __IO uint32_t ENABLE         :1;

        XH_PRIVACY_ADDR_Type LOCAL_ADDR;
        XH_PRIVACY_ADDR_Type ADVA; // Only for adv

        uint32_t RESERVE55;
        uint32_t RESERVE56;
        uint32_t RESERVE57;
        uint32_t RESERVE58;
        uint32_t RESERVE59;

        struct {
            __IO uint32_t PEER_IRK[4];
            XH_PRIVACY_ADDR_Type PEER_IDA; //Peer ID address
        }LIST[XH_HW_RLIST_NUM];

        __IO uint32_t LOCAL_IRK[4];

        XH_PRIVACY_ADDR_Type PEER_ADDR;
    }PRIVACY;
}XH_LEBB_Type;

/// XH_GPIO_Type
typedef struct
{
    __I  uint32_t DATA;         // offset:0x00
    __IO uint32_t DATAOUT;      // offset:0x04
         uint32_t RESERVED0[2];
    __IO uint32_t OUTENSET;     // offset:0x10
    __IO uint32_t OUTENCLR;     // offset:0x14
    __IO uint32_t ALTFUNCSET;   // offset:0x18
    __IO uint32_t ALTFUNCCLR;   // offset:0x1C
    __IO uint32_t INTENSET;     // offset:0x20
    __IO uint32_t INTENCLR;     // offset:0x24
    __IO uint32_t INTTYPESET;   // offset:0x28
    __IO uint32_t INTTYPECLR;   // offset:0x2C
    __IO uint32_t INTPOLSET;    // offset:0x30
    __IO uint32_t INTPOLCLR;    // offset:0x34
    __IO uint32_t INTSTATUS;    // offset:0x38
         uint32_t RESERVED1;
    __IO uint32_t INTBOTHSET;   // offset:0x40
    __IO uint32_t INTBOTHCLR;   // offset:0x44
         uint8_t  RESERVED3[0x1000-0x0048];

    __IO uint32_t MASK_0_7[0x100];   // offset:0x1000
    __IO uint32_t MASK_8_15[0x100];  // offset:0x1400
    __IO uint32_t MASK_16_23[0x100]; // offset:0x1800
    __IO uint32_t MASK_24_31[0x100]; // offset:0x1C00
}XH_GPIO_Type;

/// XH_RTC_Type
typedef struct
{
    __IO uint32_t CR;
    __IO uint32_t SR;
    __IO uint32_t SAR;
    __IO uint32_t GR;
    __IO uint32_t _SR;
}XH_RTC_Type;

/**
 * @brief I2C
 */
typedef struct
{
    __IO uint32_t CON;
    __IO uint32_t TAR;
    __IO uint32_t SAR;
    __IO uint32_t XH_MADDR;
    __IO uint32_t DATA_CMD;
    __IO uint32_t SS_SCL_HCNT;
    __IO uint32_t SS_SCL_LCNT;
    __IO uint32_t FS_SCL_HCNT;
    __IO uint32_t FS_SCL_LCNT;
    __IO uint32_t XH_SCL_HCNT;
    __IO uint32_t XH_SCL_LCNT;
    __I  uint32_t INTR_STAT;
    __IO uint32_t INTR_MASK;
    __I  uint32_t RAW_INTR_STAT;
    __IO uint32_t RX_TL;
    __IO uint32_t TX_TL;
    __I  uint32_t CLR_INTR;
    __I  uint32_t CLR_RX_UNDER;
    __I  uint32_t CLR_RX_OVER;
    __I  uint32_t CLR_TX_OVER;
    __I  uint32_t CLR_RD_REQ;
    __I  uint32_t CLR_TX_ABRT;
    __I  uint32_t CLR_RX_DONE;
    __I  uint32_t CLR_ACTIVITY;
    __I  uint32_t CLR_STOP_DET;
    __I  uint32_t CLR_START_DET;
    __I  uint32_t CLR_GEN_CALL;
    __IO uint32_t ENABLE;
    __I  uint32_t STATUS;
    __I  uint32_t TXFLR;
    __I  uint32_t RXFLR;
    __IO uint32_t SDA_HOLD;
    __IO uint32_t TX_ABRT_SOURCE;
    __IO uint32_t SLV_DATA_NACK_ONLY;
    __IO uint32_t DMA_CR;
    __IO uint32_t DMA_TDLR;
    __IO uint32_t DMA_RDLR;
    __IO uint32_t SDA_SETUP;
    __IO uint32_t ACK_GENERAL_CALL;
    __I  uint32_t ENABLE_STATUS;
    __IO uint32_t CON1;
    uint8_t Reserved0a4_0f3[0x0f4-0x0a4];
    __I  uint32_t COMP_PARAM_1;
    __I  uint32_t COMP_VERSION;
    __I  uint32_t COMP_TYPE;
}XH_I2C_Type;
/**
 * @brief Timer
 */

typedef struct
{
    __IO uint16_t CR1;         /*!< TIM control register 1,              Address offset: 0x00 */
    uint16_t      RESERVED0;   /*!< Reserved, 0x02                                            */
    __IO uint16_t CR2;         /*!< TIM control register 2,              Address offset: 0x04 */
    uint16_t      RESERVED1;   /*!< Reserved, 0x06                                            */
    __IO uint16_t SMCR;        /*!< TIM slave mode control register,     Address offset: 0x08 */
    uint16_t      RESERVED2;   /*!< Reserved, 0x0A                                            */
    __IO uint16_t DIER;        /*!< TIM DMA/interrupt enable register,   Address offset: 0x0C */
    uint16_t      RESERVED3;   /*!< Reserved, 0x0E                                            */
    __IO uint16_t SR;          /*!< TIM status register,                 Address offset: 0x10 */
    uint16_t      RESERVED4;   /*!< Reserved, 0x12                                            */
    __IO uint16_t EGR;         /*!< TIM event generation register,       Address offset: 0x14 */
    uint16_t      RESERVED5;   /*!< Reserved, 0x16                                            */
    __IO uint16_t CCMR1;       /*!< TIM capture/compare mode register 1, Address offset: 0x18 */
    uint16_t      RESERVED6;   /*!< Reserved, 0x1A                                            */
    __IO uint16_t CCMR2;       /*!< TIM capture/compare mode register 2, Address offset: 0x1C */
    uint16_t      RESERVED7;   /*!< Reserved, 0x1E                                            */
    __IO uint16_t CCER;        /*!< TIM capture/compare enable register, Address offset: 0x20 */
    uint16_t      RESERVED8;   /*!< Reserved, 0x22                                            */
    __IO uint32_t CNT;         /*!< TIM counter register,                Address offset: 0x24 */
    __IO uint16_t PSC;         /*!< TIM prescaler,                       Address offset: 0x28 */
    uint16_t      RESERVED9;   /*!< Reserved, 0x2A                                            */
    __IO uint32_t ARR;         /*!< TIM auto-reload register,            Address offset: 0x2C */
    __IO uint16_t RCR;         /*!< TIM repetition counter register,     Address offset: 0x30 */
    uint16_t      RESERVED10;  /*!< Reserved, 0x32                                            */
    __IO uint32_t CCR[4];      /*!< TIM capture/compare register 1,2,3,4 Address offset: 0x34,0x38,0x3C,0x40 */
    __IO uint16_t BDTR;        /*!< TIM break and dead-time register,    Address offset: 0x44 */
    uint16_t      RESERVED11;  /*!< Reserved, 0x46                                            */
    __IO uint16_t DCR;         /*!< TIM DMA control register,            Address offset: 0x48 */
    uint16_t      RESERVED12;  /*!< Reserved, 0x4A                                            */
    __IO uint16_t DMAR;        /*!< TIM DMA address for full transfer,   Address offset: 0x4C */
    uint16_t      RESERVED13;  /*!< Reserved, 0x4E                                            */
    /* __IO uint16_t OR;          /\*!< TIM option register,                 Address offset: 0x50 *\/ */
    /* uint16_t      RESERVED14;  /\*!< Reserved, 0x52                                            *\/ */
} XH_TIM_Type;

typedef struct
{
    __IO uint32_t CR1;          /*!< TIM control register 1,                Address offset: 0x00 */
    __IO uint32_t CR2;          /*!< TIM control register 2,                Address offset: 0x04 */
    __IO uint32_t SMCR;         /*!< TIM slave mode control register,       Address offset: 0x08 */
    __IO uint32_t DIER;         /*!< TIM DMA/interrupt enable register,     Address offset: 0x0C */
    __IO uint32_t SR;           /*!< TIM status register,                   Address offset: 0x10 */
    __IO uint32_t EGR;          /*!< TIM event generation register,         Address offset: 0x14 */
    __IO uint32_t CCMR1;        /*!< TIM capture/compare mode register 1,   Address offset: 0x18 */
    __IO uint32_t CCER;         /*!< TIM capture/compare enable register,   Address offset: 0x1C */
    __IO uint32_t CNT;          /*!< TIM counter register,                  Address offset: 0x20 */
    __IO uint32_t PSC;          /*!< TIM prescaler,                         Address offset: 0x24 */
    __IO uint32_t ARR;          /*!< TIM auto-reload register,              Address offset: 0x28 */
    __IO uint32_t RCR;          /*!< TIM repetition counter register,       Address offset: 0x2C */
    __IO uint32_t CCR[1];       /*!< TIM capture/compare register,          Address offset: 0x30 */
    __IO uint32_t BDTR;         /*!< TIM break and dead-time register,      Address offset: 0x34 */
    __IO uint32_t DCR;          /*!< TIM DMA control register,              Address offset: 0x38 */
    __IO uint32_t DMAR;         /*!< TIM DMA address for full transfer,     Address offset: 0x3C */
} XH_TIM_1_Type;

/**
 * @brief Serial Peripheral Interface
 */

typedef struct
{
    __IO uint32_t CTRL;
    __IO uint32_t WDATA;
    __I  uint32_t RDATA;
    __IO uint32_t STAT;
    __IO uint32_t DMACR;
    __IO uint32_t DMATDLR;
    __IO uint32_t DMARDLR;
    __IO uint32_t CSNCTRL;
} XH_SPI_Type;

/**
 * @brief Universal Asynchronous Receiver Transmitter
 */

typedef struct
{
    union
    {
        __I  uint32_t RBR;
        __O  uint32_t THR;
        __IO uint32_t DLL;
    };                           // offset: 0x00
    union
    {
        __IO uint32_t DLH;
        __IO uint32_t IER;
    };                           // offset: 0x04
    union
    {
        __I  uint32_t IIR;
        __O  uint32_t FCR;
    };                           // offset: 0x08
    __IO uint32_t LCR;           // offset: 0x0C
    __IO uint32_t MCR;           // offset: 0x10
    __I  uint32_t LSR;           // offset: 0x14
    __I  uint32_t MSR;           // offset: 0x18
    __IO uint32_t SCR;           // offset: 0x1C
    __IO uint32_t LPDLL;         // offset: 0x20
    __IO uint32_t LPDLH;         // offset: 0x24
    struct
    {
        // ISO7816_CTRL0 offset: 0x28
        __IO uint32_t ENABLE    :1;
        __IO uint32_t NOACK_EN  :1; // In RX, don't return ack
        __IO uint32_t TRX_EN    :1; // 0:TX 1:RX
        __IO uint32_t RETRANS_EN:1; // In TX, enable re-tx on check error
        __IO uint32_t SAMPLE_DLY:8;
        __I  uint32_t TX_DONE   :1;
             uint32_t RESERVE1  :19;

        // ISO7816_CTRL1 offset: 0x2C
        __I  uint32_t ERROR_CNT; //0-7bit:RX_ERROR 8-15bit:TX_ERROR
    }ISO7816;
    union
    {
        __I  uint32_t SRBR[16];
        __O  uint32_t STHR[16];
    };                           // offset: 0x30
    __IO uint32_t FAR;           // offset: 0x70
    __I  uint32_t TFR;
    __O  uint32_t RFW;
    __I  uint32_t USR;
    __I  uint32_t TFL;
    __I  uint32_t RFL;
    __O  uint32_t SRR;
    __IO uint32_t SRTS;
    __IO uint32_t SBCR;
    __IO uint32_t SDMAM;
    __IO uint32_t SFE;
    __IO uint32_t SRT;
    __IO uint32_t STET;
    __IO uint32_t HTX;
    __O  uint32_t DMASA;
    uint8_t Reservedac_f0[0xf4-0xac];
    __I uint32_t CPR;
    __I uint32_t UCV;
    __I uint32_t CTR;
} XH_UART_Type;

/**
 * @brief Universal Asynchronous Receiver Transmitter
 */
typedef struct
{
    union {
        uint32_t CON;
        struct
        {
            __IO uint32_t RX_INT_FLG    :1;
            __IO uint32_t TX_INT_FLG    :1;
            __IO uint32_t RX_BT         :1;
            __IO uint32_t TX_BT         :1;
            __IO uint32_t RX_EN         :1;
            __IO uint32_t SM_EN         :1;
            __IO uint32_t SM_MD         :2;
            __IO uint32_t RX_IRQ_EN     :1;
            __IO uint32_t TX_IRQ_EN     :1;
                 uint32_t RESERVE1      :22;
        }CTL;
    };

    union
    {
        __I  uint32_t RBR;
        __O  uint32_t THR;
    };
    __IO  uint32_t RELL;
    __IO  uint32_t RELH;
    __IO  uint32_t PCON;
    __IO  uint32_t ADON;
} XH_UART_EX_Type;

/**
 * @brief spi flash interface
 */
typedef struct
{
    __I  uint32_t INTR_STATUS;
    __IO uint32_t RAW_INTR_STATUS;
    __IO uint32_t INTR_MASK;
    __IO uint32_t COMMAND;
    __IO uint32_t COMMAND_DATA0_REG;
    __IO uint32_t COMMAND_DATA1_REG;
    __IO uint32_t READ0_REG;
    __IO uint32_t READ1_REG;
    __IO uint32_t ADDRESS_REG;
    __IO uint32_t READ_OPCODE_REG;
    struct {
        __IO uint32_t CTRL;
        __IO uint32_t CS;
    }CONFIGURATION[2];

    __IO uint32_t TRANS_REMAP_REG;
    __IO uint32_t WP_HOLD_REG;
    __IO uint32_t SW_SPI_CFG0_REG;
    __IO uint32_t SW_SPI_CFG1_REG;
} XH_SF_Type;

/**
 * @brief OTP
 */
typedef struct
{
    __IO uint32_t CONFIG;       // offset:0x00
    __IO uint32_t COMMAND;      // offset:0x04
    __IO uint32_t W_PROTECT;    // offset:0x08
    __IO uint32_t TROW_EN;      // offset:0x0C
    __IO uint32_t EXECUTE;      // offset:0x10
    __IO uint32_t ADDR;         // offset:0x14
    __IO uint32_t WDATA;        // offset:0x18
    __IO uint32_t RAM_BADDR_L;  // offset:0x1C
    __IO uint32_t STATUS;       // offset:0x20
    __IO uint32_t INT;          // offset:0x24
    __IO uint32_t PROTECT_CODE; // offset:0x28
    __IO uint32_t PCLK_TIMING_CFG; //offset:0x2c
    __IO uint32_t PCLK_TIMING_CFG_EN; //offset:0x30
} XH_OTP_Type;

/** 
 * @brief PATCH
 */
typedef struct
{
    __IO uint32_t ENABLE[2];
    struct
    {
        __IO uint32_t ADDRESS;
        __IO uint32_t DATA;
    }CFG[64];
} XH_PATCH_Type;

/** 
 * @brief MAC98xx
 */
typedef struct
{
    __IO uint32_t SPIRCON;
    __IO uint32_t SPIRSTAT;
    __IO uint32_t SPIRDAT;
    __IO uint32_t RFCON;
    __IO uint32_t ICR;
} XH_MAC98xx_Type;

/** 
 * @brief RANDOM
 */
typedef struct
{
    __IO uint32_t RANDOM;
} XH_RANDOM_Type;

/**
 * @brief DA IF
 */
typedef struct
{
    __IO uint32_t PD_CFG0;                      // offset: 4*0x000 = 0x00
    __IO uint32_t PD_CFG1;                      // offset: 4*0x001 = 0x04
    __IO uint32_t PD_CFG2;                      // offset: 4*0x002 = 0x08
    __IO uint32_t PA_CNS;                       // offset: 4*0x003 = 0x0C
    __IO uint32_t RC32K_TUN;                    // offset: 4*0x004 = 0x10
    __IO uint32_t CLK_CFG;                      // offset: 4*0x005 = 0x14
    __IO uint32_t REMOVED0;                     // offset: 4*0x006 = 0x18
    __IO uint32_t REMOVED1;                     // offset: 4*0x007 = 0x1C
    __IO uint32_t XTAL32M_INTRS;                // offset: 4*0x008 = 0x20
    __IO uint32_t FREQ_CFG0;                    // offset: 4*0x009 = 0x24
    __IO uint32_t FREQ_CFG1;                    // offset: 4*0x00A = 0x28
    __IO uint32_t FREQ_CFG2;                    // offset: 4*0x00B = 0x2C
    __IO uint32_t VCO_CTRL0;                    // offset: 4*0x00C = 0x30
    __IO uint32_t VCO_CTRL1;                    // offset: 4*0x00D = 0x34
    __IO uint32_t PLL_CTRL0;                    // offset: 4*0x00E = 0x38
    __IO uint32_t PLL_CTRL1;                    // offset: 4*0x00F = 0x3C
    __IO uint32_t PLL_CTRL2;                    // offset: 4*0x010 = 0x40
    __IO uint32_t LNA_MIX_CFG;                  // offset: 4*0x011 = 0x44
    __IO uint32_t TIA_LPF_CFG;                  // offset: 4*0x012 = 0x48
    __IO uint32_t RC32M_TUN;                    // offset: 4*0x013 = 0x4C
    __IO uint32_t RXADC_CTRL;                   // offset: 4*0x014 = 0x50
    __IO uint32_t RX_RCCAL_CTRL;                // offset: 4*0x015 = 0x54
    __IO uint32_t RX_RCCAL_STAT;                // offset: 4*0x016 = 0x58
    __IO uint32_t MAIN_ST_CFG0;                 // offset: 4*0x017 = 0x5C
    __IO uint32_t MAIN_ST_CFG1;                 // offset: 4*0x018 = 0x60
    __IO uint32_t MAIN_ST_CFG2;                 // offset: 4*0x019 = 0x64
    __IO uint32_t AGC_CFG0;                     // offset: 4*0x01A = 0x68
    __IO uint32_t AGC_CFG1;                     // offset: 4*0x01B = 0x6C
    __IO uint32_t AGC_CFG2;                     // offset: 4*0x01C = 0x70
    __IO uint32_t AGC_CFG3;                     // offset: 4*0x01D = 0x74
    __IO uint32_t IR_RX_CFG;                    // offset: 4*0x01E = 0x78 // xh98xx new
    __IO uint32_t TIA_DCOC_CFG;                 // offset: 4*0x01F = 0x7C // xh98xx new
    __IO uint32_t IF_AGC_LUT_REG0;              // offset: 4*0x020 = 0x80
    __IO uint32_t IF_AGC_LUT_REG1;              // offset: 4*0x021 = 0x84
    __IO uint32_t IF_AGC_LUT_REG2;              // offset: 4*0x022 = 0x88
    __IO uint32_t IF_AGC_LUT_REG3;              // offset: 4*0x023 = 0x8C
    __IO uint32_t FILT_AGC_LUT_REG0;            // offset: 4*0x024 = 0x90
    __IO uint32_t FILT_AGC_LUT_REG1;            // offset: 4*0x025 = 0x94
    __IO uint32_t FILT_AGC_LUT_REG2;            // offset: 4*0x026 = 0x98
    __IO uint32_t RX_GAINC_LUT_REG0;            // offset: 4*0x027 = 0x9C
    __IO uint32_t RX_GAINC_LUT_REG1;            // offset: 4*0x028 = 0xA0
    __IO uint32_t RX_GAINC_LUT_REG2;            // offset: 4*0x029 = 0xA4
         uint32_t RESERVE1[7];
    __IO uint32_t DCOC_CFG;                     // offset: 4*0x031 = 0xC4
    __IO uint32_t TRX_SW_CFG;                   // offset: 4*0x032 = 0xC8
    __IO uint32_t GIO_AUTO_CTRL;                // offset: 4*0x033 = 0xCC
    __IO uint32_t CLK_ENS;                      // offset: 4*0x034 = 0xD0
    __IO uint32_t PLL_LUT_DBG;                  // offset: 4*0x035 = 0xD4
    __IO uint32_t RX_ADC_OUT;                   // offset: 4*0x036 = 0xD8
    __IO uint32_t ANA_TST_CTRL;                 // offset: 4*0x037 = 0xDC
#if defined(CONFIG_XH98xxD) || defined(CONFIG_XH98xxF)
    __IO uint32_t KDCO_LUT_1M_REG0;             // offset: 4*0x038 = 0xE0
    __IO uint32_t KDCO_LUT_1M_REG1;             // offset: 4*0x039 = 0xE4
    __IO uint32_t DCOC_CFG1;                    // offset: 4*0x03A = 0xE8
    __IO uint32_t KDCO_LUT_2M_REG0;             // offset: 4*0x03B = 0xEC
    __IO uint32_t KDCO_LUT_2M_REG1;             // offset: 4*0x03C = 0xF0
#else
         uint32_t RESERVE2[2];
    __IO uint32_t DCOC_CFG1;                    // offset: 4*0x03A = 0xE8
         uint32_t RESERVE3[2];
#endif
    __IO uint32_t MON_DMA_CFG0;                 // offset: 4*0x03D = 0xF4
    __IO uint32_t MON_DMA_CFG1;                 // offset: 4*0x03E = 0xF8
    __IO uint32_t MON_DMA_CFG2;                 // offset: 4*0x03F = 0xFC
    __IO uint32_t DBG_REG;                      // offset: 4*0x040 = 0x100
    __IO uint32_t XH98xxCB_CFG;                 // offset: 4*0x041 = 0x104 // xh98xx new
#if defined(CONFIG_XH98xxD) || defined(CONFIG_XH98xxF)
         uint32_t RESERVE4[4];
#else
    __IO uint32_t KDCO_LUT_1M_REG0;             // offset: 4*0x042 = 0x108 // xh98xx new
    __IO uint32_t KDCO_LUT_1M_REG1;             // offset: 4*0x043 = 0x10C // xh98xx new
    __IO uint32_t KDCO_LUT_2M_REG0;             // offset: 4*0x044 = 0x110 // xh98xx new
    __IO uint32_t KDCO_LUT_2M_REG1;             // offset: 4*0x045 = 0x114 // xh98xx new
#endif
         uint32_t RESERVE5[10];
    __IO uint32_t PA_GAIN_REG[18];              // offset: 4*0x050 ~ 4*0x061 = 0x140 ~ 0x184
    __IO uint32_t REMOVED2;                     // offset: 4*0x062 = 0x188
    __IO uint32_t REMOVED3;                     // offset: 4*0x063 = 0x18C
    // __IO uint32_t PPG_CTRL;                  // offset: 4*0x064 = 0x190
    __IO uint32_t PLL_AFC_CTRL;                 // offset: 4*0x064 = 0x190
    __IO uint32_t RC32K_TUN_OUT;                // offset: 4*0x065 = 0x194
    __IO uint32_t TRX_EXT_PD_CFG;               // offset: 4*0x066 = 0x198
    __IO uint32_t CLK_CHK_CNS;                  // offset: 4*0x067 = 0x19C
    __IO uint32_t CLK_CHK_STATUS;               // offset: 4*0x068 = 0x1A0
         uint32_t RESERVE6[0x400-0x068-1];
    __IO uint32_t DCOC_LUT[64];                 // offset: 4*0x400 = 0x1000
         uint32_t RESERVE7[0x1000-0x400-64];
    __IO uint32_t TIA_DCOC_LUT[9];              // offset: 4*0x1000 = 0x4000 // xh98xx new
} XH_DAIF_Type;

typedef struct
{
    __IO uint32_t INTR;                         // offset: 4*0x000 = 0x00
    __IO uint32_t INTR_MSK;                     // offset: 4*0x001 = 0x04
    __IO uint32_t DLY_CFG;                      // offset: 4*0x002 = 0x08
         uint32_t RESERVED1;                    // offset: 4*0x003
    __IO uint32_t ADC_CFG0;                     // offset: 4*0x004 = 0x10
    __IO uint32_t ADC_CFG1;                     // offset: 4*0x005 = 0x14
    __IO uint32_t ADC_CFG2;                     // offset: 4*0x006 = 0x18
         uint32_t RESERVED2;                    // offset: 4*0x007
    __IO uint32_t ADC_SW_TRIGGER;               // offset: 4*0x008 = 0x20
    __IO uint32_t VOS_REG0;                     // offset: 4*0x009 = 0x24
    __IO uint32_t VOS_REG1;                     // offset: 4*0x00A = 0x28
         uint32_t RESERVED3;                    // offset: 4*0x00B
    __IO uint32_t CH_0_CFG;                     // offset: 4*0x00C = 0x30
    __IO uint32_t CH_1_CFG;                     // offset: 4*0x00D = 0x34
    __IO uint32_t CH_2_CFG;                     // offset: 4*0x00E = 0x38
    __IO uint32_t CH_3_CFG;                     // offset: 4*0x00F = 0x3c
    __IO uint32_t CH_4_CFG;                     // offset: 4*0x010 = 0x40
    __IO uint32_t CH_5_CFG;                     // offset: 4*0x011 = 0x44
    __IO uint32_t CH_6_CFG;                     // offset: 4*0x012 = 0x48
    __IO uint32_t CH_7_CFG;                     // offset: 4*0x013 = 0x4c
#ifdef CONFIG_XH98xxD
    __IO uint32_t GAIN_ERR;                     // offset: 4*0x014 = 0x50
    __IO uint32_t VOS;                          // offset: 4*0x015 = 0x54
    __IO uint32_t VOS_TEMP;                     // offset: 4*0x016 = 0x58
    __IO uint32_t SMP_OUT_CTL;                  // offset: 4*0x017 = 0x5c
         uint32_t RESERVED4[4];                 // offset: 4*(0x018~0x001B
    __IO uint32_t CH_0_DATA;                    // offset: 4*0x01C = 0x70
    __IO uint32_t CH_1_DATA;                    // offset: 4*0x01D = 0x74
    __IO uint32_t CH_2_DATA;                    // offset: 4*0x01E = 0x78
    __IO uint32_t CH_3_DATA;                    // offset: 4*0x01F = 0x7C
    __IO uint32_t CH_4_DATA;                    // offset: 4*0x020 = 0x80
    __IO uint32_t CH_5_DATA;                    // offset: 4*0x021 = 0x84
    __IO uint32_t CH_6_DATA;                    // offset: 4*0x022 = 0x88
    __IO uint32_t CH_7_DATA;                    // offset: 4*0x023 = 0x8C
         uint32_t RESERVED5[8];                 // offset: 4*(0x024~0x2B)
#endif
#ifdef CONFIG_XH98xxF
    __IO uint32_t CH_8_CFG;                     // offset: 4*0x014 = 0x50 // xh98xxF add new ch offset
    __IO uint32_t CH_9_CFG;                     // offset: 4*0x015 = 0x54
    __IO uint32_t CH_10_CFG;                    // offset: 4*0x016 = 0x58
         uint32_t RESERVED4;                    // offset: 4*0x017 = 0x5c
    __IO uint32_t GAIN_ERR;                     // offset: 4*0x018 = 0x60 // xh98xxF modify offset
    __IO uint32_t VOS;                          // offset: 4*0x019 = 0x64
    __IO uint32_t VOS_TEMP;                     // offset: 4*0x01A = 0x68
    __IO uint32_t SMP_OUT_CTL;                  // offset: 4*0x01B = 0x6c
    __IO uint32_t CH_0_DATA;                    // offset: 4*0x01C = 0x70
    __IO uint32_t CH_1_DATA;                    // offset: 4*0x01D = 0x74
    __IO uint32_t CH_2_DATA;                    // offset: 4*0x01E = 0x78
    __IO uint32_t CH_3_DATA;                    // offset: 4*0x01F = 0x7C
    __IO uint32_t CH_4_DATA;                    // offset: 4*0x020 = 0x80
    __IO uint32_t CH_5_DATA;                    // offset: 4*0x021 = 0x84
    __IO uint32_t CH_6_DATA;                    // offset: 4*0x022 = 0x88
    __IO uint32_t CH_7_DATA;                    // offset: 4*0x023 = 0x8C
    __IO uint32_t CH_8_DATA;                    // offset: 4*0x024 = 0x90 // xh98xxF add new ch offset
    __IO uint32_t CH_9_DATA;                    // offset: 4*0x025 = 0x94
    __IO uint32_t CH_10_DATA;                   // offset: 4*0x026 = 0x98
         uint32_t RESERVED5[5];                 // offset: 4*(0x027~0x2B)
#endif
#ifdef CONFIG_XH98xx
    __IO uint32_t CH_8_CFG;                     // offset: 4*0x014 = 0x50 // xh98xx add new ch offset
    __IO uint32_t CH_9_CFG;                     // offset: 4*0x015 = 0x54
    __IO uint32_t GAIN_ERR_REG0;                // offset: 4*0x016 = 0x58
    __IO uint32_t GAIN_ERR_REG1;                // offset: 4*0x017 = 0x5c
    __IO uint32_t VOS_TEMP;                     // offset: 4*0x018 = 0x60
    __IO uint32_t SMP_OUT_CTL;                  // offset: 4*0x019 = 0x64
    __IO uint32_t CH_0_DATA;                    // offset: 4*0x01A = 0x68
    __IO uint32_t CH_1_DATA;                    // offset: 4*0x01B = 0x6c
    __IO uint32_t CH_2_DATA;                    // offset: 4*0x01C = 0x70
    __IO uint32_t CH_3_DATA;                    // offset: 4*0x01D = 0x74
    __IO uint32_t CH_4_DATA;                    // offset: 4*0x01E = 0x78
    __IO uint32_t CH_5_DATA;                    // offset: 4*0x01F = 0x7C
    __IO uint32_t CH_6_DATA;                    // offset: 4*0x020 = 0x80
    __IO uint32_t CH_7_DATA;                    // offset: 4*0x021 = 0x84
    __IO uint32_t CH_8_DATA;                    // offset: 4*0x022 = 0x88
    __IO uint32_t CH_9_DATA;                    // offset: 4*0x023 = 0x8C
         uint32_t RESERVED5[8];                 // offset: 4*(0x024~0x2B)
#endif
    __IO uint32_t CH_DMA_DATA;                  // offset: 4*0x02C = 0xB0
    __IO uint32_t DMA_CNS;                      // offset: 4*0x02D = 0xB4
         uint32_t RESERVED6[10];                // offset: 4*(0x02E~0x037)
    __IO uint32_t ADC_DBG0;                     // offset: 4*0x038 = 0xE0
    __IO uint32_t ADC_DBG1;                     // offset: 4*0x039 = 0xE4
         uint32_t RESERVED8;                    // offset: 4*0x03A = 0xE8
    __IO uint32_t ADC_DBG3;                     // offset: 4*0x03B = 0xEC
         uint32_t RESERVED9[4];                 // offset: 4*(0x03C~0x03F)
    __IO uint32_t CMPN_S_VOS_G0_LUT_REG0;       // offset: 4*0x040 = 0x100
    __IO uint32_t CMPN_S_VOS_G0_LUT_REG1;       // offset: 4*0x041 = 0x104
    __IO uint32_t CMPN_S_VOS_G0_LUT_REG2;       // offset: 4*0x042
         uint32_t RESERVED10[1];                // offset: 4*0x043
    __IO uint32_t CMPN_S_VOS_G1_LUT_REG0;       // offset: 4*0x044
    __IO uint32_t CMPN_S_VOS_G1_LUT_REG1;       // offset: 4*0x045
    __IO uint32_t CMPN_S_VOS_G1_LUT_REG2;       // offset: 4*0x046
         uint32_t RESERVED11[1];                // offset: 4*0x047
    __IO uint32_t CMPN_S_VOS_G2_LUT_REG0;       // offset: 4*0x048
    __IO uint32_t CMPN_S_VOS_G2_LUT_REG1;       // offset: 4*0x049
    __IO uint32_t CMPN_S_VOS_G2_LUT_REG2;       // offset: 4*0x04A
         uint32_t RESERVED12[1];                // offset: 4*0x04B
    __IO uint32_t CMPN_S_GAIN_G0_LUT_REG0;      // offset: 4*0x04C
    __IO uint32_t CMPN_S_GAIN_G0_LUT_REG1;      // offset: 4*0x04D
    __IO uint32_t CMPN_S_GAIN_G0_LUT_REG2;      // offset: 4*0x04E
         uint32_t RESERVED13[1];                // offset: 4*0x04F
    __IO uint32_t CMPN_S_GAIN_G1_LUT_REG0;      // offset: 4*0x050
    __IO uint32_t CMPN_S_GAIN_G1_LUT_REG1;      // offset: 4*0x051
    __IO uint32_t CMPN_S_GAIN_G1_LUT_REG2;      // offset: 4*0x052
         uint32_t RESERVED14[1];                // offset: 4*0x053
    __IO uint32_t CMPN_S_GAIN_G2_LUT_REG0;      // offset: 4*0x054
    __IO uint32_t CMPN_S_GAIN_G2_LUT_REG1;      // offset: 4*0x055
    __IO uint32_t CMPN_S_GAIN_G2_LUT_REG2;      // offset: 4*0x056
         uint32_t RESERVED15[1];                // offset: 4*0x057
    __IO uint32_t CMPN_S_VCM_G0_LUT_REG0;       // offset: 4*0x058
    __IO uint32_t CMPN_S_VCM_G0_LUT_REG1;       // offset: 4*0x059
    __IO uint32_t CMPN_S_VCM_G0_LUT_REG2;       // offset: 4*0x05A
         uint32_t RESERVED16[1];                // offset: 4*0x05B
    __IO uint32_t CMPN_S_VCM_G1_LUT_REG0;       // offset: 4*0x05C
    __IO uint32_t CMPN_S_VCM_G1_LUT_REG1;       // offset: 4*0x05D
    __IO uint32_t CMPN_S_VCM_G1_LUT_REG2;       // offset: 4*0x05E
         uint32_t RESERVED17[1];                // offset: 4*0x05F
    __IO uint32_t CMPN_S_VCM_G2_LUT_REG0;       // offset: 4*0x060
    __IO uint32_t CMPN_S_VCM_G2_LUT_REG1;       // offset: 4*0x061
    __IO uint32_t CMPN_S_VCM_G2_LUT_REG2;       // offset: 4*0x062
         uint32_t RESERVED18[1];                // offset: 4*0x063
    __IO uint32_t CMPN_D_VOS_LUT_REG0;          // offset: 4*0x064
    __IO uint32_t CMPN_D_VOS_LUT_REG1;          // offset: 4*0x065
    __IO uint32_t CMPN_D_VOS_LUT_REG2;          // offset: 4*0x066
         uint32_t RESERVED19[1];                // offset: 4*0x067
    __IO uint32_t CMPN_D_GAIN_LUT_REG0;         // offset: 4*0x068
    __IO uint32_t CMPN_D_GAIN_LUT_REG1;         // offset: 4*0x069
    __IO uint32_t CMPN_D_GAIN_LUT_REG2;         // offset: 4*0x06A
         uint32_t RESERVED20[1];                // offset: 4*0x06B
    __IO uint32_t CMPN_CTRL;                    // offset: 4*0x06C
         uint32_t RESERVED21[19];               // offset: 4*(0x06D~0x7F)
    __IO uint32_t IIR_1ST_COEF_12;              // offset: 4*0x080
    __IO uint32_t IIR_1ST_COEF_34;              // offset: 4*0x081
    __IO uint32_t IIR_1ST_COEF_5;               // offset: 4*0x082
         uint32_t RESERVED22[1];                // offset: 4*0x083
    __IO uint32_t IIR_2ND_COEF_12;              // offset: 4*0x084
    __IO uint32_t IIR_2ND_COEF_34;              // offset: 4*0x085
    __IO uint32_t IIR_2ND_COEF_5;               // offset: 4*0x086
         uint32_t RESERVED23[1];                // offset: 4*0x087
    __IO uint32_t IIR_3RD_COEF_12;              // offset: 4*0x088
    __IO uint32_t IIR_3RD_COEF_34;              // offset: 4*0x089
    __IO uint32_t IIR_3RD_COEF_5;               // offset: 4*0x08A
         uint32_t RESERVED24[1];                // offset: 4*0x08B
    __IO uint32_t IIR_4TH_COEF_12;              // offset: 4*0x08C
    __IO uint32_t IIR_4TH_COEF_34;              // offset: 4*0x08D
    __IO uint32_t IIR_4TH_COEF_5;               // offset: 4*0x08E
         uint32_t RESERVED25[1];                // offset: 4*0x08F
    __IO uint32_t IIR_GAIN;                     // offset: 4*0x090
    __IO uint32_t FILTER_CNT_PERIOD;            // offset: 4*0x094
    __IO uint32_t FILTER_CTRL;                  // offset: 4*0x098
} XH_GPADC_Type;

/**
 * @brief IR_TX
 */
typedef struct
{
      __IO uint32_t PWM_EN;             //0x00
      __IO uint32_t PWM_MODE;           //0x04
      __IO uint32_t PWM_INV;            //0x08
      __IO uint32_t PWM_POLARITY;       //0x0C
      __IO uint32_t PWM_TCMP;           //0x10
      __IO uint32_t PWM_TMAX;           //0x14
      __IO uint32_t PWM_TCMP_SHADOW;    //0x18
      __IO uint32_t PWM_TMAX_SHADOW;    //0x1C
      __IO uint32_t PWM_PNUM;           //0x20
      __I  uint32_t PWM_CNT;            //0x24
      __I  uint32_t PWM_PULSE_CNT;      //0x28
      __IO uint32_t PWM_INT_MASK;       //0x2C
      __IO uint32_t PWM_INT_ST;         //0x30
      __IO uint32_t FIFO_CNT_INT_MASK;  //0x34
      __IO uint32_t FIFO_CNT_INT_ST;    //0x38
      __IO uint32_t FIFO_DATA_ENTRY;    //0x3C
      __IO uint32_t FIFO_NUM_LVL;       //0x40
      __I  uint32_t FIFO_SR;            //0x44
      __O  uint32_t FIFO_CLR;           //0x48
      __IO uint32_t DMA_SADDR;          //0x4C
      __IO uint32_t DMA_TRANS_LENGTH;   //0x50
      __IO uint32_t DMA_EN;             //0x54
      __IO uint32_t ANA_IF;             //0x58
}XH_IR_TX_Type;

/**
 * @brief DMA controller
 */
//ChCtrl
#define DMA_ENABLE_POS                              0
#define DMA_INTTCMASK_POS                           1              
#define DMA_INTERRMASK_POS                          2
#define DMA_INTABTMASK_POS                          3
#define DMA_DSTREQSEL_POS                           4
#define DMA_SRCREQSEL_POS                           8
#define DMA_DSTADDRCTRL_POS                         12
#define DMA_SRCADDRCTRL_POS                         14
#define DMA_DSTMODE_POS                             16
#define DMA_SRCMODE_POS                             17
#define DMA_DSTWIDTH_POS                            18
#define DMA_SRCWIDTH_POS                            20
#define DMA_SRCBURSTSIZE_POS                        22
#define DMA_PRIORITY_POS                            29      
#define DMA_ENABLE_MASK                             0x00000001          
#define DMA_INTTCMASK_MASK                          0x00000002     
#define DMA_INTERRMASK_MASK                         0x00000004 
#define DMA_INTABTMASK_MASK                         0x00000008 
#define DMA_DSTREQSEL_MASK                          0x000000F0 
#define DMA_SRCREQSEL_MASK                          0x00000F00 
#define DMA_DSTADDRCTRL_MASK                        0x00003000 
#define DMA_SRCADDRCTRL_MASK                        0x0000C000 
#define DMA_DSTMODE_MASK                            0x00010000 
#define DMA_SRCMODE_MASK                            0x00020000 
#define DMA_DSTWIDTH_MASK                           0x000C0000 
#define DMA_SRCWIDTH_MASK                           0x00300000 
#define DMA_SRCBURSTSIZE_MASK                       0x01C00000 
#define DMA_PRIORITY_MASK                           0x20000000 

typedef struct XH_DMA_CH_Type
{
    __IO uint32_t Ctrl;
    __IO uint32_t SrcAddr;
    __IO uint32_t DstAddr;
    __IO uint32_t TranSize;
    __IO struct XH_DMA_CH_Type* LLPointer;
} XH_DMA_CH_Type;

//IdRev
#define DMAC_REVMINJOR_POS                           0
#define DMAC_REVMAJOR_POS                            4   
#define DMAC_ID_POS                                  12   
#define DMAC_REVMINJOR_MASK                          0x0000000F
#define DMAC_REVMAJOR_MASK                           0x00000FF0   
#define DMAC_ID_MASK                                 0xFFFFF000   
//DMACfg
#define DMAC_CHANNELNUM_POS                          0
#define DMAC_FIFIODEPTH_POS                          4   
#define DMAC_REQNUM_POS                              10 
#define DMAC_REQSYNC_POS                             30  
#define DMAC_CHAINXFR_POS                            31    
#define DMAC_CHANNELNUM_MASK                         0x0000000F
#define DMAC_FIFIODEPTH_MASK                         0x000003F0   
#define DMAC_REQNUM_MASK                             0x00007C00
#define DMAC_REQSYNC_MASK                            0x40000000
#define DMAC_CHAINXFR_MASK                           0x80000000 
//DMACtrl
#define DMAC_RESET_POS                               0    
#define DMAC_RESET_MASK                              0x00000001
//Status
#define DMAC_ERROR_POS                               0
#define DMAC_ABORT_POS                               8   
#define DMAC_TC_POS                                  16 
#define DMAC_ERROR_MASK                              0x000000FF
#define DMAC_ABORT_MASK                              0x0000FF00   
#define DMAC_TC_MASK                                 0x00FF0000

typedef struct
{
    /* ID and revision register */
    __I uint32_t IdRev;
    /* __I  uint32_t IdRev; */
    uint32_t Reserved0[(0x10-0x04)/4];
    /* Configuration register */
    __I uint32_t DMACfg;
    uint32_t Reserved1[(0x20-0x14)/4];
    /* Global control register  */
    __O uint32_t DMACtrl;
    uint32_t Reserved2[(0x30-0x24)/4];
      /* Channel status registers  */
    __IO union
    {
        uint32_t all;
        uint32_t status;
    } IntStatus;

    __I  uint32_t ChEN; /* Alias of the Enable field of all ChnCtrl registers */
    uint32_t Reserved3[(0x40-0x38)/4];

    /* Channel control registers */
    __O  uint32_t ChAbort;
    __IO XH_DMA_CH_Type Ch[8];
} XH_DMAC_Type;

typedef struct
{
    __I  uint32_t TYPE;
    __IO uint32_t CONFIG;
    __IO uint32_t CTRL;
    __IO uint32_t STATUS;
         uint32_t RESERVE0[4];
    __IO uint32_t MAINT0;
    __IO uint32_t MAINT1;
    __IO uint32_t MON_CONFIG;
    __IO uint32_t MON_EN;
    __IO uint32_t MON_CTRL;
    __IO uint32_t MON_STATUS;
} XH_HCACHE_Type;

typedef struct
{
    __IO uint32_t ADC_CTRL;             // offset: 0x0
    __IO uint32_t ADC_DECI_FILT_11;     // offset: 0x4
    __IO uint32_t ADC_DECI_FILT_12;     // offset: 0x8
    __IO uint32_t ADC_DECI_FILT_13;     // offset: 0xc
    __IO uint32_t ADC_DECI_FILT_21;     // offset: 0x10
    __IO uint32_t ADC_DECI_FILT_22;     // offset: 0x14
    __IO uint32_t ADC_DECI_FILT_23;     // offset: 0x18
    __IO uint32_t ADC_DECI_FILT_31;     // offset: 0x1c
    __IO uint32_t ADC_DECI_FILT_32;     // offset: 0x20
    __IO uint32_t ADC_DECI_FILT_33;     // offset: 0x24
    __IO uint32_t ADC_DECI_FILT_41;     // offset: 0x28
    __IO uint32_t ADC_DECI_FILT_42;     // offset: 0x2c
    __IO uint32_t ADC_DECI_FILT_43;     // offset: 0x30
    __IO uint32_t ADC_DECI_FILT_51;     // offset: 0x34
    __IO uint32_t ADC_DECI_FILT_52;     // offset: 0x38
    __IO uint32_t ADC_DECI_FILT_53;     // offset: 0x3c
    __IO uint32_t ADC_IIR_FILT_11;      // offset: 0x40
    __IO uint32_t ADC_IIR_FILT_12;      // offset: 0x44
    __IO uint32_t ADC_IIR_FILT_13;      // offset: 0x48
    __IO uint32_t ADC_IIR_FILT_21;      // offset: 0x4c
    __IO uint32_t ADC_IIR_FILT_22;      // offset: 0x50
    __IO uint32_t ADC_IIR_FILT_23;      // offset: 0x54
    __IO uint32_t ADC_IIR_FILT_31;      // offset: 0x58
    __IO uint32_t ADC_IIR_FILT_32;      // offset: 0x5c
    __IO uint32_t ADC_IIR_FILT_33;      // offset: 0x60
    __IO uint32_t ADC_DRC_CTRL_1;       // offset: 0x64
    __IO uint32_t ADC_DRC_CTRL_2;       // offset: 0x68
    __IO uint32_t ADC_DRC_CTRL_3;       // offset: 0x6c
    __IO uint32_t ADC_DRC_CTRL_4;       // offset: 0x70
    __IO uint32_t ADC_DRC_CTRL_5;       // offset: 0x74
    __IO uint32_t ADC_VOL_CTRL;         // offset: 0x78
         uint32_t RESERVED1[(0xc0-0x78-1)/4];
    __IO uint32_t CODEC_GAIN_READ;      // offset: 0xc0
    __IO uint32_t CODEC_CLK_CTRL_1;     // offset: 0xc4
    __IO uint32_t CODEC_CLK_CTRL_2;     // offset: 0xc8
    __IO uint32_t CODEC_CLK_CTRL_3;     // offset: 0xcc
    __IO uint32_t ADC_DITHER_CTRL_1;    // offset: 0xd0
    __IO uint32_t ADC_DITHER_CTRL_2;    // offset: 0xd4
    __IO uint32_t CIC_ONLY_CTRL;        // offset: 0xd8
    __IO uint32_t CODEC_INT_STATUS;     // offset: 0xdc
    __IO uint32_t CODEC_INT_CTRL;       // offset: 0xe0
    __IO uint32_t CIC_ONLY_RDATA;       // offset: 0xe4
    __IO uint32_t CIC_ONLY_RDATA_1;     // offset: 0xe8
    __IO uint32_t RSVD_1;               // offset: 0xec
    __IO uint32_t ADC_PEAK_READ;        // offset: 0xf0
         uint32_t RESERVED2;            // offset: 0xf4
    __IO uint32_t AEC_DELAY_CFG;        // offset: 0xf8
    __IO uint32_t IF_CTRL;              // offset: 0xfc
    __IO uint32_t CODEC_ANA_CTRL_1;     // offset: 0x100
    __IO uint32_t CODEC_ANA_CTRL_2;     // offset: 0x104
         uint32_t RESERVED3;            // offset: 0x108
    __IO uint32_t CODEC_ANA_PWR_1;      // offset: 0x10C
} XH_AUDIO_Type;

/**
 * @brief I2S
 */
typedef struct
{
    union
    {
      __I uint32_t LRBR;
      __O uint32_t LTHR;
    };

    union
    {
      __I uint32_t RRBR;
      __O uint32_t RTHR;
    };

    __IO uint32_t RER;
    __IO uint32_t TER;
    __IO uint32_t RCR;
    __IO uint32_t TCR;
    __I  uint32_t ISR;
    __IO uint32_t IMR;
    __I  uint32_t ROR;
    __I  uint32_t TOR;
    __IO uint32_t RFCR;
    __IO uint32_t TFCR;
    __O  uint32_t RFF;
    __O  uint32_t TFF;

    __IO uint32_t RESERVE[2];

}XH_I2SCHN_Type;

typedef struct
{
    __IO uint32_t IER;
    __IO uint32_t IRER;
    __IO uint32_t ITER;
    __IO uint32_t CER;
    __IO uint32_t CCR;
    __IO uint32_t RXFFR;
    __IO uint32_t TXFFR;

    __IO uint32_t RESERVE0;

    XH_I2SCHN_Type CHN_R0;
    XH_I2SCHN_Type CHN_R1;
    XH_I2SCHN_Type CHN_R2;
    XH_I2SCHN_Type CHN_R3;

    __IO uint32_t RESERVE1[40];

    __I  uint32_t RXDMA;
    __O  uint32_t RRXDMA;
    __O  uint32_t TXDMA;
    __O  uint32_t RTXDMA;

    __IO uint32_t RESERVE2[8];

    __I union {
        uint32_t all;
        struct {
            /**
             * I2S wordsize:
             * 0x0 = 12 bit resolution
             * 0x1 = 16 bit resolution
             * 0x2 = 20 bit resolution
             * 0x3 = 24 bit resolution
             * 0x4 = 32 bit resolution
             * 0x5 - 0x7 = reserved
             */
            uint32_t I2S_RX_WORDSIZE_0 : 3;
            uint32_t I2S_RX_WORDSIZE_1 : 3;
            uint32_t                   : 1;
            uint32_t I2S_RX_WORDSIZE_2 : 3;
            uint32_t I2S_RX_WORDSIZE_3 : 3;
            uint32_t                   : 19;
        };
    } I2S_COMP_PARAM_2;
    __I union {
        uint32_t all;
        struct {
            uint32_t APB_DATA_WIDTH        : 2;
            uint32_t I2S_FIFO_DEPTH_GLOBAL : 2;
            uint32_t I2S_MODE_EN           : 1;
            uint32_t I2S_TRANSMITTER_BLOCK : 1;
            uint32_t I2S_RECEIVER_BLOCK    : 1;
            uint32_t I2S_RX_CHANNELS       : 2;
            uint32_t I2S_TX_CHANNELS       : 2;
            uint32_t                       : 5;
            /**
             * I2S wordsize:
             * 0x0 = 12 bit resolution
             * 0x1 = 16 bit resolution
             * 0x2 = 20 bit resolution
             * 0x3 = 24 bit resolution
             * 0x4 = 32 bit resolution
             * 0x5 - 0x7 = reserved
             */
            uint32_t I2S_TX_WORDSIZE_0     : 3;
            uint32_t I2S_TX_WORDSIZE_1     : 3;
            uint32_t I2S_TX_WORDSIZE_2     : 3;
            uint32_t I2S_TX_WORDSIZE_3     : 3;
            uint32_t                       : 4;
        };
    } I2S_COMP_PARAM_1;
    __I  uint32_t I2S_COMP_VERSION;
    __I  uint32_t I2S_COMP_TYPE;
} XH_I2S_Type;
/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */


#ifdef __cplusplus
}
#endif

#endif

/** @} */
