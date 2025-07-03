/*********************************************************************
 * @file uart.c
 * @brief 
 * @version 1.0
 * @date Wed 19 Nov 2014 04:11:47 PM CST
 * @author XIHAO
 *
 * @note 
 */

/*********************************************************************
 * INCLUDES
 */
#include "features.h"
#include "xh98xx_config.h"
#include "uart.h"
#include "cpm.h"


/*********************************************************************
 * MACROS
 */
#define MODE_X_DIV          16
#define ISO7816_RX_MODE     1
#define ISO7816_TX_MODE     0

#define UART_TIMEOUT        (1024 * 1024 * 2)

/*********************************************************************
 * TYPEDEFS
 */
typedef struct
{
    uart_rx_callback_t      rx_cb;
    uart_tx_cmp_callback_t  tx_cmp_cb;
}uart_callback_t;

typedef struct
{
    uart_callback_t uart[1];
}uart_env_t;

/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * LOCAL VARIABLES
 */

static uart_env_t uart_env;

/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */

/**
 * @brief uart_check_irq()
 *
 * @param[in] uart  
 *
 * @return 
 **/
static void uart_check_irq(XH_UART_Type *uart, uart_callback_t *uart_cb)
{
    uint8_t status;
    volatile uint8_t ch;

    while (1)
    {
        status = uart->IIR & UART_IIR_ID;
        switch (status)
        {
            case UART_IIR_RDI:
                while(uart->LSR & UART_LSR_DR)
                {
                    ch = uart->RBR;
                    if (uart_cb->rx_cb)
                        uart_cb->rx_cb(ch);
                }
                break;

            case UART_IIR_THRI:
                uart->IER &= ~UART_IER_THRI;
                if (uart_cb->tx_cmp_cb)
                    uart_cb->tx_cmp_cb();
                break;

            case UART_IIR_NO_INT:
                return;

            case UART_IIR_BDI:
                status = uart->USR;
                break;

            case UART_IIR_RLSI:
                ch = uart->LSR;
                break;

            case UART_IIR_CTI:
                ch = uart->LSR;
                ch = uart->RBR;
                break;

            default:
                break;
        }
    }
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief uart initialize
 *
 * @param[in] uart  uart object
 * @param[in] baud_rate  baud rate
 * @param[in] flow_ctrl  flow control (Only UART1 support)
 * @param[in] uart_rx_cb  receive callback
 *
 * @return None
 **/
void uart_open(XH_UART_Type *uart, uint32_t baud_rate, uart_flow_ctrl_t flow_ctrl, uart_rx_callback_t uart_rx_cb)
{
    uint16_t baud_divisor;

    // Reset and Bypass UART1
    register_set1(&XH_PSO->UART1_CFG, CPM_UART_SOFT_RESET_MASK);
    register_set0(&XH_PSO->UART1_CFG, CPM_UART_DIV_SEL_MASK | CPM_UART_GATE_EN_MASK);
    XH_PSO_UPD_RDY();

    uart_env.uart[0].tx_cmp_cb = NULL;
    uart_env.uart[0].rx_cb = NULL;

    /* Compute divisor value. Normally, we should simply return:
     *   NS16550_CLK / MODE_X_DIV / baudrate
     * but we need to round that value by adding 0.5.
     * Rounding is especially important at high baud rates.
     */

    if (cpm_get_clock(CPM_TOP_CLK)>64000000 && baud_rate<19200)
        baud_divisor = 4; // make sure 9600bps is supported
    else
        baud_divisor = 1;

    cpm_set_clock(CPM_UART1_CLK, baud_divisor*baud_rate*MODE_X_DIV);

    // Disable LCR and irq
    uart->LCR = 0x00;
    uart->IER = 0;

    // Auto RTS – Becomes active when the following occurs:
    //   - Auto Flow Control is selected during configuration
    //   - FIFOs are implemented
    //   - RTS (MCR[1] bit and MCR[5]bit are both set)
    //   - FIFOs are enabled (FCR[0]) bit is set)
    //   - SIR mode is disabled (MCR[6] bit is not set)
    if(flow_ctrl == UART_FLOW_CTRL_ENABLED)
        uart->MCR = UART_MCR_RTS | UART_MCR_AFCE;
    else
        uart->MCR = 0x00;

    uart->FCR = UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT |
        UART_FCR_FIFO_EN | UART_FCR_TRIGGER_1;

    /* Baud rate setting.*/
    uart->LCR = UART_LCR_DLAB;
    uart->DLL = baud_divisor & 0xff;
    uart->DLH = (baud_divisor >> 8) & 0xff;

    /* 8 data, 1 stop, no parity */
    uart->LCR = UART_LCR_8N1;

    /* Set UARTs int mask */
    if(uart_rx_cb)
    {
        uart->IER = UART_IER_RLSI | UART_IER_RDI;

        NVIC_ClearPendingIRQ(UART1_IRQn);
        NVIC_SetPriority(UART1_IRQn, IRQ_PRIORITY_HIGH);
        NVIC_EnableIRQ(UART1_IRQn);
    }
    else
    {
        uart->IER = 0;

        NVIC_DisableIRQ(UART1_IRQn);
    }

    uart_env.uart[0].tx_cmp_cb = NULL;
    uart_env.uart[0].rx_cb = uart_rx_cb;
}

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
void uart_send_noblock(XH_UART_Type *uart, uint8_t **buf, unsigned *length, uart_tx_cmp_callback_t txcb)
{
    uint32_t count = *length;
    uint32_t fifo_len = 0;

    uart_env.uart[0].tx_cmp_cb = txcb;
    fifo_len = 16;

    if (count >= fifo_len)
        count = fifo_len;

    (*length) -= count;

    do
    {
        uart->THR = *((*buf)++);
        count--;
    }while(count);

    uart->IER |= UART_IER_THRI;
}

/**
 * @brief uart wait send finish @ref uart_send_noblock
 *
 * @param[in] uart  uart object
 *
 * @return None
 **/
void uart_wait_send_finish(XH_UART_Type *uart)
{
    while (!(uart->LSR & UART_LSR_TEMT));
}

/**
 * @brief uart send with block
 *
 * @param[in] uart  uart object
 * @param[in] buf  transmit data buffer
 * @param[in] length  transmit data length
 *
 * @return None
 **/
void uart_send_block(XH_UART_Type *uart, const uint8_t *buf, unsigned length)
{
    unsigned i;

    for(i=0; i<length; ++i)
    {
        while (!(uart->LSR & UART_LSR_THRE));
        uart->THR = buf[i];
    }

    while (!(uart->LSR & UART_LSR_TEMT));
}

/**
 * @brief uart empty fifo
 *
 * @param[in] uart  uart object
 *
 * @return None
 **/
void uart_empty_fifo(XH_UART_Type *uart)
{
    volatile uint32_t data;
    while (uart->LSR & UART_LSR_DR) /* Read all characters out of the fifo */
        data = uart->RBR;
    (void) data;
}

void UART1_IRQHandler(void)
{
    uart_check_irq(XH_UART1, &uart_env.uart[0]);
}
