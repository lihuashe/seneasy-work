/**
 * @file mmi.c
 * @brief 
 * @date 2017/07/19 16:44:06
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
#define MMI_REVERSAL_SHOW

/*********************************************************************
 * TYPEDEFS
 */
typedef enum
{
    MMI_BEGIN,
    MMI_END,
}mmi_state_t;

typedef void (* mmi_display_event_t)(int event, mmi_state_t state);

/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * LOCAL VARIABLES
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */
static const signed char __mmi_led_table[] =
{
    /* MMI_EVENT_TEST */             8,
    /* MMI_EVENT_ADV */              8,
    /* MMI_EVENT_SCAN */             8,
    /* MMI_EVENT_INIT */             8,

    /* MMI_EVENT_MASTER */           8,
    /* MMI_EVENT_SLAVE */            8,
    /* MMI_EVENT_LINK_1 */          10,
    /* MMI_EVENT_LINK_2 */          -1,
    /* MMI_EVENT_LINK_3 */          -1,
    /* MMI_EVENT_LINK_4 */          -1,
    /* MMI_EVENT_LINK_5 */          -1,
    /* MMI_EVENT_LINK_6 */          -1,
    /* MMI_EVENT_LINK_7 */          -1,
    /* MMI_EVENT_LINK_8 */          -1,

    /* MMI_EVENT_TRANSCEIVER */     -1,

    /* MMI_EVENT_CO_SCHE */         -1,

    /* MMI_EVENT_CPU_GATE */        11,
    /* MMI_EVENT_SLEEP */            9,
    /* MMI_EVENT_DEEP_SLEEP */       9,

    /* MMI_EVENT_SOFTFAULT */       -1,
    /* MMI_EVENT_HARDFAULT */       -1,

    /* MMI_EVENT_BLE_IRQ */         -1,
    /* MMI_EVENT_BLE_IRQ_PKA */     -1,
    /* MMI_EVENT_BLE_IRQ_PKD */     -1,
    /* MMI_EVENT_BLE_IRQ_SYNC */    -1,
    /* MMI_EVENT_BLE_IRQ_TIM0 */    -1,
    /* MMI_EVENT_BLE_IRQ_TIM1 */    -1,
    /* MMI_EVENT_BLE_IRQ_TIM2 */    -1,
    /* MMI_EVENT_BLE_IRQ_TIM3 */    -1,
    /* MMI_EVENT_BLE_IRQ_NOPKD */   -1,
};

extern mmi_display_event_t mmi_display_event_callback;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/**
 * @brief mmi_display_event_handler()
 *
 * @param[in] event  
 * @param[in] state  
 *
 * @return 
 **/
static void mmi_display_event_handler(int event, mmi_state_t state)
{
    if(__mmi_led_table[event] >= 0)
    {
        CO_DISABLE_IRQ();
        gpio_write(1u<<__mmi_led_table[event],
#ifdef MMI_REVERSAL_SHOW
                state==MMI_BEGIN ? GPIO_LOW : GPIO_HIGH
#else
                state==MMI_BEGIN ? GPIO_HIGH : GPIO_LOW
#endif
                );
        CO_RESTORE_IRQ();
    }
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
void mmi_init(void)
{
    mmi_display_event_callback = mmi_display_event_handler;
}

/** @} */

