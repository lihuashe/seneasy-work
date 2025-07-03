/**
 * @file timer.c
 * @brief
 * @date Wed 31 May 2017 07:16:01 PM CST
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
#include "timer_dma.h"
/*********************************************************************
 * MACROS
 */




/*********************************************************************
 * TYPEDEFS
 */



/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * LOCAL VARIABLES
 */
XH_DMA_CH_Type *timer_rx_dma = NULL;
XH_DMA_CH_Type *timer_tx_dma = NULL;

uint32_t buf_32bits_aligned[2][TIM_TEST_BUF_SIZE/4];


/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */

/**
 * @brief tim_enable_irq()
 *
 * @param[in] index
 *
 * @return
 **/




/*********************************************************************
 * PUBLIC FUNCTIONS
 */

void print_buf(void *buf, int length, char *title)
{
    if (title)
        log_debug("%s:\r\n", title);

    // use length == 0 as string flag.
    if (length == 0)
        log_debug("%s\r\n", (char *)buf);
    else {
        for (int i = 0; i < length; i++)
            log_debug("%02x%s", ((uint8_t *)buf)[i], (i%16 != 15) ? " " : "\r\n");
    }
    log_debug("\r\n");
}

void cmd_dma_mem_to_tim_comm(XH_TIM_Type *timer)
{
    XH_TIM_Type *tim = timer;
    uint32_t tim_id = 0;

    tim_id = TIM_STRUCT2INDEX(tim);

    /// 0. Data init
    for (int i = 0; i < TIM_TEST_BUF_SIZE/4; i++) {
        //buf_32bits_aligned[0][i] = (tim_id<<8) + 0xA0u + i + 1;
        buf_32bits_aligned[0][i] = i;
        buf_32bits_aligned[1][i] = 0;
    }

    tim_config_t config = {
        .mode                   = TIM_TIMER_MODE,
        .config.timer.period_us = 10000, /* The counter is blocked when the auto-reload value tim->ARR is null. */
        .config.timer.callback  = NULL,
    };
    tim_dma_config_t txconfig = {
        .mem_to_tim   = true,
        .tim_addr     = (uint32_t *)&tim->CCR[0],//,&tim->ARR
        .mem_addr     = buf_32bits_aligned[0],
        .len_in_bytes = TIM_TEST_BUF_SIZE,
        .req          = TIM_DIER_UDE | TIM_DIER_TDE,
    };
    tim_dma_config_t rxconfig = {
        .mem_to_tim   = false,
        .tim_addr     = (uint32_t *)&tim->CCR[0],//&tim->ARR,
        .mem_addr     = buf_32bits_aligned[1],
        .len_in_bytes = TIM_TEST_BUF_SIZE,
        .req          = TIM_DIER_UDE | TIM_DIER_TDE,
    };

    /// 1. Init and configure TIMx
    tim_init();
    tim_config(tim, &config);

	/// 2. Configure DMA transfer and init.
    dma_init();
	timer_tx_dma = timer_tx_dma_config(tim, timer_tx_dma, &txconfig);
	timer_rx_dma = timer_rx_dma_config(tim, timer_rx_dma, &rxconfig);

    /// 3. DMA start.
	dma_start(timer_rx_dma,(uint32_t)&tim->CCR[0],(uint32_t)buf_32bits_aligned[1],TIM_TEST_BUF_SIZE/4);
	dma_start(timer_tx_dma,(uint32_t)buf_32bits_aligned[0],(uint32_t)&tim->CCR[0],TIM_TEST_BUF_SIZE/4);

	/// 4. Start TIMx
    tim_start(tim); /* tim->CR1 = 1; // Enable TIMx: CR1.CER = 1 */

	/// 5. DMA wait stop
	dma_wait_stop(timer_tx_dma);
	dma_wait_stop(timer_rx_dma);


    print_buf(buf_32bits_aligned[0], TIM_TEST_BUF_SIZE, "tim.txbuf");
    print_buf(buf_32bits_aligned[1], TIM_TEST_BUF_SIZE, "tim.rxbuf");
    if (memcmp(buf_32bits_aligned[0], buf_32bits_aligned[1], TIM_TEST_BUF_SIZE) == 0)
        log_debug("dma mem to/from tim tests pass.\r\n");
    else
        log_debug("dma mem to/from tim tests fail.\r\n");

	log_debug("\n");
}


void dma_timer(void)
{
    log_debug("timer0: dma mem to/from tim tests begin!.\r\n");
    cmd_dma_mem_to_tim_comm(XH_TIM0);
    log_debug("timer0: dma mem to/from tim tests end!.\r\n\n\n\n");

    log_debug("timer1: dma mem to/from tim tests begin!.\r\n");
    cmd_dma_mem_to_tim_comm(XH_TIM1);
    log_debug("timer1: dma mem to/from tim tests end!.\r\n\n\n\n");

    log_debug("timer2: dma mem to/from tim tests begin!.\r\n");
    cmd_dma_mem_to_tim_comm(XH_TIM2);
    log_debug("timer2: dma mem to/from tim tests end!.\r\n\n\n\n");
}


/** @} */


