/*********************************************************************
 * @file uart_ex.c
 * @brief 
 * @version 1.0
 * @date Thur 02 Sep 2021 05:11:47 PM CST
 * @author tagntl
 *
 * @note 
 */

/*********************************************************************
 * INCLUDES
 */
#include "peripheral.h"
#include "uart_ex.h"
#include "cpm.h"
#include "xh98xx_config.h"
#include <assert.h>

uint8_t multi_flg_ex;
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */
typedef struct
{
    uart_rx_callback_t      rx_cb;
    uart_tx_cmp_callback_t  tx_cmp_cb;
}uart_ex_callback_t;

typedef struct
{
    uart_ex_callback_t uart[2];
}uart_ex_env_t;

static uart_ex_env_t uart_ex_env;


/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * LOCAL VARIABLES
 */



/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */


/**
 * @brief uart_ex_check_irq()
 *
 * @param[in] uart  
 *
 * @return 
 **/
static void uart_ex_check_irq(XH_UART_EX_Type *uart, uart_ex_callback_t *uart_cb)
{
    uint8_t status;
    volatile uint8_t ch;
    
    status = uart->CON & 0x3;
    
    if( 1 == status )
        //uart->CTL.RX_INT_FLG = 0;
        REGW(&uart->CON,MASK_1REG(UART_CON_RX_INT_FLG, 0));
    else
        REGW(&uart->CON,MASK_1REG(UART_CON_TX_INT_FLG, 0));
        //uart->CTL.TX_INT_FLG = 0;
    
    switch (status)
    {
        case 1:
        {
            ch = uart->RBR;
            if (uart_cb->rx_cb)
                uart_cb->rx_cb(ch);
        }
        break;
        
        case 2:
        {
            if (uart_cb->tx_cmp_cb)
            uart_cb->tx_cmp_cb();
        }
        break;

        default:
        break;
    }
}

/**
 * @brief uart_ex_set_clk()
 *
 * @param[in] uart  
 *
 * @return 
 **/
static void uart_ex_set_clk(XH_UART_EX_Type *uart, const uart_ex_cfg_t *uart_ex_cfg)
{
    uint32_t top_clk = 0;
    uint32_t baud_divisor=0;
    uint32_t uart_mode = 0;
    uint8_t smod = 0;
    uart_mode = uart_ex_cfg->mode;

    top_clk = cpm_get_clock(CPM_TOP_CLK);

    REGW(&uart->CON,MASK_1REG(UART_CON_SM_MD, uart_mode));

    /* Set smod bit of PCON */
    REGW(&uart->PCON,MASK_1REG(UART_PCON_SMOD_WRITE, uart_ex_cfg->smod_val));
    
    /* the bd bit must be 1 */
    REGW(&uart->ADON,MASK_1REG(UART_ACON_BAUD_BIT, 1));

    smod = uart_ex_cfg->smod_val;
    
    if( UART_EX_MD_1 == uart_mode || UART_EX_MD_3 == uart_mode )
    {
        if( smod )
            baud_divisor = 1024 - ((2*top_clk)>>6) / uart_ex_cfg->baud_rate;
        else
            baud_divisor = 1024 - (top_clk>>6) / uart_ex_cfg->baud_rate;
        
        uart->RELL = baud_divisor & 0xFF;
        uart->RELH = (baud_divisor & 0x300)>>8;
    }

}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief uart ex initialize
 *
 * @param[in] uart  uart object
 * @param[in] cfg
 *
 * @return bool:  
 *                 false   uart ex open fail
 *                 true    uart ex open success
 **/
void uart_ex_open(XH_UART_EX_Type *uart, const uart_ex_cfg_t *cfg)
{
    uint32_t top_clk = 0;
    uint32_t baud_judge = 0;
    top_clk = cpm_get_clock(CPM_TOP_CLK);
    baud_judge = top_clk>>6;
    
    co_assert((XH_UART0 == uart));

    if( UART_EX_MD_2 == cfg->mode )
    {
        if( cfg->smod_val )
            co_assert(!(baud_judge%cfg->baud_rate));
        else
            co_assert(!(baud_judge%(2*cfg->baud_rate)));
    }

    co_assert(cfg->baud_rate);

    if( XH_UART0 == uart )
    {
        // Reset and Ungate Clk of UART0
        register_set1(&XH_PSO->UART0_CFG, CPM_UART_SOFT_RESET_MASK);
        register_set0(&XH_PSO->UART0_CFG, CPM_UART_SOFT_RESET_MASK | CPM_UART_GATE_EN_MASK);
        XH_PSO_UPD_RDY();

        uart_ex_env.uart[0].tx_cmp_cb = NULL;
        uart_ex_env.uart[0].rx_cb = NULL;
    }

    /* Set uart ctl register */ 
    
    if(( UART_EX_MD_2 == cfg->mode ) || ( UART_EX_MD_3 == cfg->mode ))
    {
        REGW(&uart->CON,MASK_3REG(UART_CON_RX_BT, cfg->rb80_val,UART_CON_TX_BT, cfg->tb80_val,UART_CON_SM_EN,1));
    }

    uart_ex_set_clk(uart, cfg);

    /* Set UARTs int mask */
    if(cfg->rx_cb)
    {
        REGW(&uart->CON,MASK_2REG(UART_CON_RX_EN, 1,UART_CON_RX_IRQ_EN, 1));

        if ( uart == XH_UART0 ) 
        {
            NVIC_ClearPendingIRQ(UART0_IRQn);
            NVIC_SetPriority(UART0_IRQn, IRQ_PRIORITY_HIGH);
            NVIC_EnableIRQ(UART0_IRQn);
        }
    }
    else
    {
        REGW(&uart->CON,MASK_2REG(UART_CON_RX_EN, 0,UART_CON_RX_IRQ_EN, 0));
        if ( uart == XH_UART0 )
            NVIC_DisableIRQ(UART0_IRQn);
    }

    if(uart == XH_UART0)
    {
        uart_ex_env.uart[0].tx_cmp_cb = NULL;
        uart_ex_env.uart[0].rx_cb = cfg->rx_cb;
    }
}

/**
 * @brief uart_ex_close()
 *
 * @param[in] uart  
 *
 * @return 
 **/
void uart_ex_close(XH_UART_EX_Type *uart)
{
    if(uart == XH_UART0)
        register_set1(&XH_PSO->UART0_CFG, CPM_UART_GATE_EN_MASK);
    
    XH_PSO_UPD_RDY();
}


/**
 * @brief uart ex send with block
 *
 * @param[in] uart  uart object
 * @param[in] buf  transmit data buffer
 * @param[in] length  transmit data length
 *
 * @return None
 **/
void uart_ex_send_block(XH_UART_EX_Type *uart, const uint8_t *buf, uint32_t length)
{
    uint32_t i;
    REGW(&uart->CON,MASK_1REG(UART_CON_SM_EN,1));

    for(i=0; i<length; i++)
    {
        uart->THR = buf[i];

        while(!(REGR(&uart->CON,MASK_POS(UART_CON_TX_INT_FLG))))
        {
            if(1 == multi_flg_ex)
            {
                uart->THR = buf[i];
                multi_flg_ex = 0;
            }
        }; 

        REGW(&uart->CON,MASK_1REG(UART_CON_TX_INT_FLG, 0));
    }
}

void UART0_IRQHandler(void)
{
    uart_ex_check_irq(XH_UART0, &uart_ex_env.uart[0]);
}


