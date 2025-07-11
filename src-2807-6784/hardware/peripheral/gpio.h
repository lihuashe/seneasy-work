/**
 * @file gpio.h
 * @brief gpio driver
 * @date Wed 31 May 2017 07:15:14 PM CST
 * @author XIHAO
 *
 * @defgroup GPIO GPIO
 * @ingroup PERIPHERAL
 * @brief GPIO driver
 * @details GPIO dirver
 *
 * The XH98xx has up to 19 software-configurable I/O pins.
 *
 * <pre>
 * Features:
 *  • Fully programmable pin assignment
 *  • Selectable pull-up, pull-down resistors per pin
 *  • Pins retain their last state when system enters the sleep mode
 *  • Ability to wakeup chip by any GPIOs in sleep mode
 * </pre>
 *
 * @{
 *
 * @example example_gpio.c
 * This is an example of how to use the gpio
 *
 */

#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "xh98xx.h"
#include <stdint.h>
#include <stdbool.h>

/*********************************************************************
 * MACROS
 */

/// GPIO fast read 0-31, reference @ref gpio_read
#define GPIO_READ_0_31(pin_mask)            (XH_GPIO0->DATA & (pin_mask))

/// GPIO fast write 0-7, reference @ref gpio_write
#define GPIO_WRITE_0_7(pin_mask, level)     do{ XH_GPIO0->MASK_0_7  [((pin_mask)>>0)&0xff]  = (level); }while(0)
/// GPIO fast write 8-15, reference @ref gpio_write
#define GPIO_WRITE_8_15(pin_mask, level)    do{ XH_GPIO0->MASK_8_15 [((pin_mask)>>8)&0xff]  = (level); }while(0)
/// GPIO fast write 16-23, reference @ref gpio_write
#define GPIO_WRITE_16_23(pin_mask, level)   do{ XH_GPIO0->MASK_16_23[((pin_mask)>>16)&0xff] = (level); }while(0)
/// GPIO fast write 24-31, reference @ref gpio_write
//#define GPIO_WRITE_24_31(pin_mask, level)   do{ XH_GPIO0->MASK_24_31[((pin_mask)>>24)&0xff] = (level); }while(0)

/// GPIO fast input setting 0-31, reference @ref gpio_set_direction
#define GPIO_SET_INPUT_0_31(pin_mask)       do{ XH_GPIO0->OUTENCLR = pin_mask; }while(0)
/// GPIO fast output setting 0-31, reference @ref gpio_set_direction
#define GPIO_SET_OUTPUT_0_31(pin_mask)      do{ XH_GPIO0->OUTENSET = pin_mask; }while(0)

/*********************************************************************
 * TYPEDEFS
 */

/// Level
typedef enum
{
    GPIO_LOW = 0,
    GPIO_HIGH = ~0,
}gpio_level_t;

/// Direction
typedef enum
{
    GPIO_INPUT = 0,
    GPIO_OUTPUT = ~0,
}gpio_direction_t;

/// Interrupt trigger type
typedef enum
{
    GPIO_FALLING_EDGE,
    GPIO_RISING_EDGE,
    GPIO_BOTH_EDGE,
    GPIO_LOW_LEVEL,
    GPIO_HIGH_LEVEL,
    GPIO_TRIGGER_DISABLE,
}gpio_trigger_type_t;

/**
 * @brief gpio event callback
 *
 * @param[in] pin_mask  current pin mask
 *
 * @return None
 **/
typedef void (*gpio_callback_t)(uint32_t pin_mask);

/// @cond
typedef struct
{
    gpio_callback_t callback;
    uint32_t pin_single_edge_flag;
    uint32_t pin_single_edge_level;

    struct
    {
        struct
        {
            struct
            {
                uint32_t DATAOUT;
                uint32_t OUTENSET;
                uint32_t INTENSET;
                uint32_t INTPOLSET;
                uint32_t INTTYPESET;
                uint32_t INTBOTHSET;
            }gpio0;
        }store_reg;
    }lowpower;
}gpio_env_t;
/// @endcond

/*********************************************************************
 * EXTERN VARIABLES
 */
/// @cond
extern gpio_env_t gpio_env;
/// @endcond

/*********************************************************************
 * EXTERN FUNCTIONS
 */

/**
 * @brief gpio initialize
 *
 * @return None
 **/
void gpio_open(void);

/**
 * @brief  gpio open only clock
 **/
void gpio_open_clock(void);

/**
 * @brief gpio set interrupt type
 *
 * @param[in] pin_mask  pin mask
 * @param[in] trigger_type  gpio trigger type
 *
 * @return None
 **/
void gpio_set_interrupt(uint32_t pin_mask, gpio_trigger_type_t trigger_type);

/**
 * @brief gpio set interrupt callback
 *
 * @param[in] callback  gpio event callback
 *
 * @return None
 **/
void gpio_set_interrupt_callback(gpio_callback_t callback);

/**
 * @brief gpio set direction
 *
 * @param[in] pin_mask  pin mask
 * @param[in] dir  gpio direction
 *
 * @return None
 **/
void gpio_set_direction(uint32_t pin_mask, gpio_direction_t dir);

/**
 * @brief gpio write value
 *
 * @param[in] pin_mask  pin mask
 * @param[in] level  gpio value with mask, @ref gpio_level_t
 *
 * @return None
 **/
void gpio_write(uint32_t pin_mask, uint32_t level);

/**
 * @brief gpio store, inside function
 *
 * Just for system call before sleep
 *
 * @return None
 **/
void gpio_store(void);

/**
 * @brief gpio restore, inside function
 *
 * Just for system call after sleep
 *
 * @return None
 **/
void gpio_restore(void);

/**
 * @brief gpio read input value
 *
 * @param[in] pin_mask  pin mask
 *
 * @return gpio value with mask
 **/
__STATIC_INLINE uint32_t gpio_read(uint32_t pin_mask)
{
    return XH_GPIO0->DATA & pin_mask;
}

/**
 * @brief gpio read current output status
 *
 * @param[in] pin_mask  pin mask
 *
 * @return gpio value with mask
 **/
__STATIC_INLINE uint32_t gpio_read_output_status(uint32_t pin_mask)
{
    return XH_GPIO0->DATAOUT & pin_mask;
}

/**
 * @brief gpio_clear_interrupt_pending()
 *
 * @param[in] pin_mask  
 *
 * @return 
 **/
__STATIC_INLINE void gpio_clear_interrupt_pending(uint32_t pin_mask)
{
    XH_GPIO0->INTSTATUS = pin_mask;
}

/**
 * @brief gpio toggle
 *
 * @param[in] pin_mask  pin mask
 *
 * @return None
 **/
__STATIC_INLINE void gpio_toggle(uint32_t pin_mask)
{
    gpio_write(pin_mask, gpio_read_output_status(pin_mask) ^ pin_mask);
}

#ifdef __cplusplus
}
#endif

#endif

/** @} */

