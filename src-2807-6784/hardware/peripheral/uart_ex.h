/**
 * @file uart_ex.h
 * @version 1.0
 * @date Thur 02 Sep 2021 05:11:47 PM CST
 * @author tangtanglin
 *
 *
 * @{
 *
 * @example example_uart_ex.c
 * This is an example of how to use the uart
 *
 */

#ifndef __UART_EX_H__
#define __UART_EX_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "peripheral.h"

/*********************************************************************
 * MACROS
 */


/*********************************************************************
 * TYPEDEFS
 */
/// Receive event callback
//typedef void (*uart_rx_callback_t)(uint8_t data);

/// Transmit complete callback
//typedef void (*uart_tx_cmp_callback_t)(void);

typedef uint8_t uart_ex_mode_enum;
enum uart_ex_mode_values
{
    UART_EX_MD_1 = 1,
    UART_EX_MD_2,
    UART_EX_MD_3,
    UART_EX_MD_MAX,
};

typedef uint8_t uart_ex_tb80_enum;
enum uart_ex_tb80_values
{
    TB80_LOW = 0,
    TB80_HIGH,
    TB80_MAX,
};


typedef uint8_t uart_ex_rb80_enum;
enum uart_ex_rb80_values
{
    RB80_LOW = 0,
    RB80_HIGH,
    RB80_MAX,
};

typedef uint32_t uart_ex_smod_enum;
enum uart_ex_smod_values
{
    SMOD_0 = 0,
    SMOD_1,
    SMOD_MAX,
};

typedef struct
{
    uint32_t                baud_rate; 
    uart_ex_mode_enum       mode; 
    uart_ex_tb80_enum       tb80_val;
    uart_ex_rb80_enum       rb80_val;
    uart_ex_smod_enum       smod_val;
    uart_rx_callback_t      rx_cb;
    uart_tx_cmp_callback_t  tx_cb;
}uart_ex_cfg_t;



/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */

/**
 * @brief uart ex open
 *
 * @param[in] uart  uart object
 * @param[in] baud_rate  baud rate
 * @param[in] flow_ctrl  flow control (Only UART1 support)
 * @param[in] uart_rx_cb  receive callback
 *
 * @return None
 **/
void uart_ex_open(XH_UART_EX_Type *uart, const uart_ex_cfg_t *uart_ex_cfg);

/**
 * @brief uart send with block
 *
 * @param[in] uart  uart object
 * @param[in] buf  transmit data buffer
 * @param[in] length  transmit data length
 *
 * @return None
 **/
void uart_ex_send_block(XH_UART_EX_Type *uart, const uint8_t *buf, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

