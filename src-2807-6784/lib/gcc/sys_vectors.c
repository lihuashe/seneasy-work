/**
 * @file sys_vectors.c
 * @brief 
 * @date Mon 10 Aug 2015 04:44:09 PM CST
 * @author liqiang
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
#include "features.h"
#include "co.h"
#include "peripheral.h"
#include "mbr.h"

/*********************************************************************
 * MACROS
 */
#define WEAK __attribute__((weak))
#define NVIC_IRQ_COUNT 36

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif

#define RAM_BASE    0x00200000

#ifndef RUN2SF_MAGIC_FLAG
#define RUN2SF_MAGIC_FLAG 0x46533252
#endif

/*********************************************************************
 * TYPEDEFS
 */
typedef void (*funcp_t) (void);

typedef void (*vector_table_entry_t)(void);

typedef struct {
    uint32_t *initial_sp_value; /**< Initial stack pointer value. */
    vector_table_entry_t reset;
    vector_table_entry_t nmi;
    vector_table_entry_t hard_fault;
    vector_table_entry_t memory_manage_fault; /* not in CM0 */
    vector_table_entry_t bus_fault;           /* not in CM0 */
    vector_table_entry_t usage_fault;         /* not in CM0 */
    vector_table_entry_t soft_fault;
    vector_table_entry_t reserved_x0020;
    vector_table_entry_t reserved_x0024;
    vector_table_entry_t reserved_x0028;
    vector_table_entry_t sv_call;
    vector_table_entry_t debug_monitor;       /* not in CM0 */
    vector_table_entry_t reserved_x0034;
    vector_table_entry_t pend_sv;
    vector_table_entry_t systick;
    vector_table_entry_t irq[NVIC_IRQ_COUNT];
} vector_table_t;

/*********************************************************************
 * EXTERN
 */

/* Symbols exported by the linker script(s): */
extern uint32_t _data_loadaddr, _data, _edata, _ebss, _estack;
extern uint32_t _vectors_loadaddr, _vectors, _evectors;
extern uint32_t _ramtext_loadaddr, _ramtext, _eramtext;


int main(void);
void system_init(void);
void system_init_submain(void);
void blocking_handler(void);
void null_handler(void);

void WEAK reset_handler(void);
void WEAK nmi_handler(void);
void WEAK hard_fault_handler(void);
void WEAK soft_fault_handler(int code);
void WEAK SVC_Handler(void);
void WEAK PendSV_Handler(void);
void WEAK SysTick_Handler(void);

/* Those defined only on ARMv7 and above */
#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
void WEAK mem_manage_handler(void);
void WEAK bus_fault_handler(void);
void WEAK usage_fault_handler(void);
void WEAK debug_monitor_handler(void);
#endif

/* External Interrupts */
void WEAK BT_IRQHandler(void);
void WEAK SLEEP_TIMER_IRQHandler(void);
void WEAK DMA_IRQHandler(void);
void WEAK GPIO_IRQHandler(void);
void WEAK TIM_IRQHandler(void);
void WEAK MAC98xx_RF_IRQHandler(void);
void WEAK MAC98xx_SPI_IRQHandler(void);
void WEAK PMU_TIMER_IRQHandler(void);
void WEAK IR_TX_IRQHandler(void);
void WEAK UART1_IRQHandler(void);
void WEAK EFUSE_IRQHandler(void);
void WEAK PIN_WAKEUP_IRQHandler(void);
void WEAK ADC_IRQHandler(void);
void WEAK I2C_IRQHandler(void);
void WEAK SF_IRQHandler(void);
void WEAK SOFT0_IRQHandler(void);
void WEAK SOFT1_IRQHandler(void);
void WEAK SOFT2_IRQHandler(void);
void WEAK SOFT3_IRQHandler(void);
void WEAK SOFT4_IRQHandler(void);
void WEAK SOFT5_IRQHandler(void);
void WEAK SOFT6_IRQHandler(void);
void WEAK SOFT7_IRQHandler(void);
void WEAK VTRACK_IRQHandler(void);
void WEAK CRY32M_RDY_IRQHandler(void);
void WEAK UART0_IRQHandler(void);
void WEAK GPIO0_IRQHandler(void);
void WEAK CC_INTR_IRQHandler(void);
void WEAK TIM0_IRQHandler(void);
void WEAK TIM1_IRQHandler(void);
void WEAK TIM2_IRQHandler(void);
void WEAK CHARGER_IRQHandler(void);
void WEAK AUDIO_IRQHandler(void);
void WEAK I2S_RX_IRQnIRQHandler(void);
void WEAK I2S_TX_IRQnIRQHandler(void);


#pragma weak nmi_handler                = null_handler
#pragma weak SVC_Handler                = null_handler
#pragma weak PendSV_Handler             = null_handler
#pragma weak SysTick_Handler            = null_handler

/* Those are defined only on CM3 or CM4 */
#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
#pragma weak mem_manage_handler         = blocking_handler
#pragma weak bus_fault_handler          = blocking_handler
#pragma weak usage_fault_handler        = blocking_handler
#pragma weak debug_monitor_handler      = null_handler
#endif

#pragma weak BT_IRQHandler              = blocking_handler
#pragma weak SLEEP_TIMER_IRQHandler     = blocking_handler
#pragma weak DMA_IRQHandler             = blocking_handler
#pragma weak GPIO_IRQHandler            = blocking_handler
#pragma weak TIM_IRQHandler             = blocking_handler
#pragma weak MAC98xx_RF_IRQHandler      = blocking_handler
#pragma weak MAC98xx_SPI_IRQHandler     = blocking_handler
#pragma weak PMU_TIMER_IRQHandler       = blocking_handler
#pragma weak IR_TX_IRQHandler           = blocking_handler
#pragma weak UART1_IRQHandler           = blocking_handler
#pragma weak EFUSE_IRQHandler           = blocking_handler
#pragma weak PIN_WAKEUP_IRQHandler      = blocking_handler
#pragma weak ADC_IRQHandler             = blocking_handler
#pragma weak I2C_IRQHandler             = blocking_handler
#pragma weak SF_IRQHandler              = blocking_handler
#pragma weak SOFT0_IRQHandler           = blocking_handler
#pragma weak SOFT1_IRQHandler           = blocking_handler
#pragma weak SOFT2_IRQHandler           = blocking_handler
#pragma weak SOFT3_IRQHandler           = blocking_handler
#pragma weak SOFT4_IRQHandler           = blocking_handler
#pragma weak SOFT5_IRQHandler           = blocking_handler
#pragma weak SOFT6_IRQHandler           = blocking_handler
#pragma weak SOFT7_IRQHandler           = blocking_handler
#pragma weak VTRACK_IRQHandler          = blocking_handler
#pragma weak CRY32M_RDY_IRQHandler      = blocking_handler
#pragma weak UART0_IRQHandler           = blocking_handler
#pragma weak GPIO0_IRQHandler           = blocking_handler
#pragma weak CC_INTR_IRQHandler         = blocking_handler
#pragma weak TIM0_IRQHandler            = blocking_handler
#pragma weak TIM1_IRQHandler            = blocking_handler
#pragma weak TIM2_IRQHandler            = blocking_handler
#pragma weak CHARGER_IRQHandler         = blocking_handler
#pragma weak AUDIO_IRQHandler           = blocking_handler
#pragma weak I2S_RX_IRQnIRQHandler      = blocking_handler
#pragma weak I2S_TX_IRQnIRQHandler      = blocking_handler


/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * LOCAL VARIABLES
 */
__attribute__ ((section(".vectors")))
vector_table_t vector_table = {
    .initial_sp_value = &_estack,
    .reset = reset_handler,
    .nmi = nmi_handler,
    .hard_fault = hard_fault_handler,
    .soft_fault = (vector_table_entry_t)soft_fault_handler,

    /* Those are defined only on CM3 or CM4 */
#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
    .memory_manage_fault = mem_manage_handler,
    .bus_fault = bus_fault_handler,
    .usage_fault = usage_fault_handler,
    .debug_monitor = debug_monitor_handler,
#endif

    .reserved_x0020 = (void *)0xFFFFFFFF,
    .reserved_x0024 = (void *)&_vectors_loadaddr, /* Base address in flash */
    .reserved_x0028 = (void *)RUN2SF_MAGIC_FLAG,  /* RUN2RF flag */
    .sv_call = SVC_Handler,
    .reserved_x0034 = (void *)(CONFIG_ROM_ID), /* VEC_APP_MAGIC */
    .pend_sv = PendSV_Handler,
    .systick = SysTick_Handler,
    .irq = {
        BT_IRQHandler             , //0
        SLEEP_TIMER_IRQHandler    , //1
        DMA_IRQHandler            , //2
        GPIO_IRQHandler           , //3
        TIM_IRQHandler            , //4
        MAC98xx_RF_IRQHandler     , //5
        MAC98xx_SPI_IRQHandler    , //6
        PMU_TIMER_IRQHandler      , //7
        IR_TX_IRQHandler          , //8
        UART1_IRQHandler          , //9
        EFUSE_IRQHandler          , //10
        PIN_WAKEUP_IRQHandler     , //11
        ADC_IRQHandler            , //12
        I2C_IRQHandler            , //13
        SF_IRQHandler             , //14
        SOFT0_IRQHandler          , //15
        SOFT1_IRQHandler          , //16
        SOFT2_IRQHandler          , //17
        SOFT3_IRQHandler          , //18
        SOFT4_IRQHandler          , //19
        SOFT5_IRQHandler          , //20
        SOFT6_IRQHandler          , //21
        SOFT7_IRQHandler          , //22
        VTRACK_IRQHandler         , //23
        CRY32M_RDY_IRQHandler     , //24
        UART0_IRQHandler          , //25
        GPIO0_IRQHandler          , //26
        CC_INTR_IRQHandler        , //27
        TIM0_IRQHandler           , //28
        TIM1_IRQHandler           , //29
        TIM2_IRQHandler           , //30
        CHARGER_IRQHandler        , //31
        AUDIO_IRQHandler          , //32
        I2S_RX_IRQnIRQHandler     , //33
        I2S_TX_IRQnIRQHandler     , //34
    }
};


/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */


/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief  reset handler
 **/
void WEAK __attribute__ ((naked,noinline)) reset_handler(void)
{
    // regis variable for guard stack
#ifndef CONFIG_XIP_RAM_ALL
    register uint32_t *src;
#endif

    register uint32_t *dest;

    // RAM version or gdb download needs override SP
    __set_MSP((uint32_t)&_estack);

    system_init();

#ifdef CONFIG_XIP_RAM_ALL
    dest = &_edata;
#else
    for (src = &_data_loadaddr, dest = &_data;
        dest < &_edata;
        src++, dest++) {
        *dest = *src;
    }
#endif

    while (dest < &_ebss) {
        *dest++ = 0;
    }

    // reset table
    for (src = &_vectors_loadaddr, dest = &_vectors;
        dest < &_evectors;
        src++, dest++) {
        *((uint32_t *)((uint32_t)dest | RAM_BASE)) = *src;
    }
    *((uint32_t *)((uint32_t)(&_vectors + 1) | RAM_BASE)) = (uint32_t)co_ram_reset_handler;

    // RAM .text
    for (src = &_ramtext_loadaddr, dest = &_ramtext;
        dest < &_eramtext;
        src++, dest++) {
        *((uint32_t *)((uint32_t)dest | RAM_BASE)) = *src;
    }

    // system init
    system_init_submain();

    /* Call the application's entry point. */
    main();
}

/**
 * @brief null_handler()
 *
 * @return 
 **/
void null_handler(void)
{
    while (1);
}

/**
 * @brief blocking_handler()
 *
 * @return 
 **/
void blocking_handler(void)
{
    while (1);
}

/**
 * @brief hard_fault_handler()
 *
 * @return 
 **/
void WEAK hard_fault_handler(void)
{
    while (1);
}

/**
 * @brief soft_fault_handler()
 *
 * @return 
 **/
void WEAK soft_fault_handler(int code)
{
    while (1);
}

/** @} */

