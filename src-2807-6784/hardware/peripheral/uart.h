/**
 * @file uart.h
 * @brief uart driver
 * @date Thu 11 May 2017 10:22:38 AM CST
 * @author XIHAO
 *
 * @defgroup UART UART
 * @ingroup PERIPHERAL
 * @brief UART/ISO7816 driver
 * @details UART driver
 *
 * The UART is modeled after the industry-standard 16550. However, the register address space
 * has been relocated to 32-bit data boundaries for APB bus implementation.
 *
 * The UART is used for serial communication with a peripheral, modem (data carrier equipment, DCE)
 * or data set. Data is written from a master (CPU) over the APB bus to the UART and it is converted
 * to serial form and transmitted to the destination device. Serial data is also received by the UART
 * and stored for the master (CPU) to read back.
 *
 * The UART contains registers to control the character length, baud rate, parity generation/checking,
 * and interrupt generation. Although there is only one interrupt output signal from the UART, there
 * are several prioritized interrupt types that can be responsible for its assertion. Each of the
 * interrupt types can be separately enabled or disabled by the control registers.
 *
 * XH98xx has 2 UART; the UART0 is a common 2 wire (transmitter and receiver) controller, and the
 * UART1 support stream control (CTS/RTS). UART1 also supports ISO7816 protocols.
 *
 * @{
 *
 * @example example_uart.c
 * This is an example of how to use the uart
 *
 */

#ifndef __UART_H__
#define __UART_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "features.h"
#include "xh98xx.h"

/*********************************************************************
 * MACROS
 */


/*********************************************************************
 * TYPEDEFS
 */
/// Receive event callback
typedef void (*uart_rx_callback_t)(uint8_t data);

/// Transmit complete callback
typedef void (*uart_tx_cmp_callback_t)(void);

/// Flow control
typedef enum
{
    UART_FLOW_CTRL_DISABLED,
    UART_FLOW_CTRL_ENABLED,
}uart_flow_ctrl_t;

/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */

/**
 * @brief uart open
 *
 * @param[in] uart  uart object
 * @param[in] baud_rate  baud rate
 * @param[in] flow_ctrl  flow control (Only UART1 support)
 * @param[in] uart_rx_cb  receive callback
 *
 * @return None
 **/
void uart_open(XH_UART_Type *uart, uint32_t baud_rate, uart_flow_ctrl_t flow_ctrl, uart_rx_callback_t uart_rx_cb);

/**
 * @brief uart send with noblock
 *
 * @param[in] uart  uart object
 * @param[in] buf  transmit data buffer
 * @param[in] length  transmit data length
 * @param[in] txcb  transmit complete callback
 *
 * @return None
 **/
void uart_send_noblock(XH_UART_Type *uart, uint8_t **buf, unsigned *length, uart_tx_cmp_callback_t txcb);

/**
 * @brief uart wait send finish @ref uart_send_noblock
 *
 * @param[in] uart  uart object
 *
 * @return None
 **/
void uart_wait_send_finish(XH_UART_Type *uart);

/**
 * @brief uart send with block
 *
 * @param[in] uart  uart object
 * @param[in] buf  transmit data buffer
 * @param[in] length  transmit data length
 *
 * @return None
 **/
void uart_send_block(XH_UART_Type *uart, const uint8_t *buf, unsigned length);

/**
 * @brief uart empty fifo
 *
 * @param[in] uart  uart object
 *
 * @return None
 **/
void uart_empty_fifo(XH_UART_Type *uart);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

