/*********************************************************************
 * @file co_port_xh98xx.h
 * @brief 
 * @version 1.0
 * @date Mon 17 Nov 2014 01:45:02 PM CST
 * @author XIHAO
 *
 * @note 
 */

#ifndef __CO_PORT_XH98xxD_H__
#define __CO_PORT_XH98xxD_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "features.h"
#include "xh98xx.h"
//#include "bb_timer.h"

/*********************************************************************
 * MACROS
 */

#define CO_CPU_CLOCK_MHZ             32

/// IRQ priority
#define IRQ_PRIORITY_HIGHEST         1
#define IRQ_PRIORITY_HIGH            3
#define IRQ_PRIORITY_NORMAL          5
#define IRQ_PRIORITY_LOW             6
#define IRQ_PRIORITY_LOWEST          7

/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * INLINE FUNCTION
 */

// Native Bluetooth Clock is 32bit and unit 31.25us
#define CO_MAX_SYS_TIME                 0xFFFFFFFF
// Invalid system time
#define CO_INVALID_SYS_TIME             0xFFFFFFFF
// unit is 31.25us
#define CO_SYS_TIMER_RESOLUTION         2 //BB_TIMER_RESOLUTION
// unit transmite 1ms to 31.25us
#define CO_TIME_MS2SYS(time)            ((time) << 5)
#define CO_TIME_SYS2MS(time)            ((time) >> 5)

// irq latency and irq process time
#define CO_DISABLE_IRQ_BEGIN_CB()       co_disable_irq_begin_cb()
#define CO_DISABLE_IRQ_END_CB()         co_disable_irq_end_cb()
#define CO_IRQ_PROCESS_TIME_BEGIN()     do{uint32_t __irq_process_begin_cycle = DWT->CYCCNT
#define CO_IRQ_PROCESS_TIME_END()       co_set_irq_process_time(DWT->CYCCNT-__irq_process_begin_cycle);}while(0)

// Disable all irq
#define CO_DISABLE_IRQ()                do{uint32_t __irq_save = __get_PRIMASK(); __set_PRIMASK(1);
#define CO_RESTORE_IRQ()                __set_PRIMASK(__irq_save);} while(0)
#define CO_ENABLE_IRQ()                 __set_PRIMASK(0)
#define CO_DISABLE_IRQ_EX(irq_value)    do{irq_value = __get_PRIMASK(); __set_PRIMASK(1);}while(0)
#define CO_RESTORE_IRQ_EX(irq_value)    do{__set_PRIMASK(irq_value);}while(0)
// Disable irq except highest
#define CO_DISABLE_IRQ_EXCEPT_HIGHEST() do{__irq_save = __get_BASEPRI(); __set_BASEPRI_MAX(IRQ_PRIORITY_HIGH<<(8U-__NVIC_PRIO_BITS));
#define CO_RESTORE_IRQ_EXCEPT_HIGHEST() __set_BASEPRI(__irq_save);} while(0)
#define CO_DISABLE_IRQ_EX_EXCEPT_HIGHEST(irq_value) do{irq_value = __get_BASEPRI(); __set_BASEPRI_MAX(IRQ_PRIORITY_HIGH<<(8U-__NVIC_PRIO_BITS));}while(0)
#define CO_RESTORE_IRQ_EX_EXCEPT_HIGHEST(irq_value) do{__set_BASEPRI(irq_value);}while(0)

// Calculate BEGIN-END delay time
// CO_DELAY_CALC_END() return is us or just print __delay_us
#define CO_DELAY_CALC_INIT() \
    uint32_t __delay_cycle, __delay_us; \
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; \
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk
#define CO_DELAY_CALC_BEGIN() \
     __delay_cycle = DWT->CYCCNT;
#define CO_DELAY_CALC_END() \
    (__delay_cycle = DWT->CYCCNT - __delay_cycle, __delay_us = __delay_cycle / CO_CPU_CLOCK_MHZ)
#define CO_DELAY_CALC_CYCLE_END() \
    (__delay_cycle = DWT->CYCCNT - __delay_cycle, __delay_cycle)

// Is in irq ?
#define CO_IS_URGENCY() (__get_IPSR() != 0)

#if defined  (__GNUC__)
#define CO_ALIGN(x) __attribute__((aligned(x)))
#else
#define CO_ALIGN(x) __align(x)
#endif

// section for RAM text
#define CO_SECTION_RAMTEXT          __attribute__((section(".ramtext")))
#define CO_SECTION_RAMTEXT_ONCE     __attribute__((section(".ramtext.once")))

// USED
#define CO_USED_IN_ROM              __attribute__((used))

/*********************************************************************
 * Extra INCLUDES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */

/**
 * @brief co_tick()
 *
 * Note: Only for debug
 *
 * @return 
 **/
__STATIC_INLINE uint32_t co_tick(void)
{
    return DWT->CYCCNT / CO_CPU_CLOCK_MHZ;
}

/**
 * @brief  co cpu check
 **/
void co_cpu_check(void);

/**
 * @brief  co cpu suspend
 **/
void co_cpu_suspend(void);

/**
 * @brief  co ram reset handler
 **/
void co_ram_reset_handler (void);

#ifdef __cplusplus
}
#endif

#endif


