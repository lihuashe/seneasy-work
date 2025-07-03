/**
 * @attention
 *
 *  Copyright 2024 Guangdong Seneasy Intelligent Technology Co., Ltd. All rights reserved.
 * @file     fyll_host.c
 *
 * @author   张凯华(zhangkaihua@seneasy.com)
 *
 * @brief
 *
 * @date     2024-07-05
 *
 */

#include "tl_common.h"
#include "driver.h"
#include "app_config.h"
#include "fyll.h"
#include "tl_tpll.h"

#define DEBUG_PRINT_REG(n) printf("[" #n "]=%02x\r\n", read_reg8(0x##n))

//--------------------------------------------------------------------+
// Variables: transmission statistics variables
//--------------------------------------------------------------------+

volatile unsigned int trans_statistics_cnt_tx = 0;
volatile unsigned int trans_statistics_cnt_rx = 0;
volatile unsigned int trans_statistics_cnt_rx_dr = 0;
volatile unsigned int trans_statistics_cnt_invalid_pid = 0;
volatile unsigned int trans_statistics_cnt_tx_ds = 0;
volatile unsigned int trans_statistics_cnt_max_retry = 0;

uint8_t fyll_channel_tab[FYLL_MAX_CHANNEL_TAB_SIZE] = FYLL_DEFAULT_CHANNEL_TAB;
uint8_t fyll_channel_tab_size;  // Channel subset size
uint8_t fyll_channel_tab_index; // Channel subset index
uint8_t fyll_rx_dr;             // received data ready
bool fyll_sync_on_f;
uint16_t fyll_sync_period; // Counter for counting duration since previous successfull TX
volatile fyll_state_t fyll_state_var;
bool fyll_pending_goto_idle_f; // 是否等待进入空闲状态
int fyll_rx_rssi;
uint16_t fyll_timeout_cnt;              // 发送或接收超时计数器
uint16_t fyll_channel_switch_cnt;       // 前一次传输频道切换次数
uint16_t fyll_try_cnt;                  // 前一次传输尝试次数
bool fyll_tx_success_f;                 // 前一次传输是否成功
uint8_t fyll_current_tx_payload_lenght; // 当前发送的有效数据长度
bool fyll_pending_tx_start_f;
uint8_t fyll_timer_period_modified_f; // 是否需要修改定时器周期
int16_t fyll_tries_pr_channel_cnt;    // 在单个频道上尝试发送的计数

//--------------------------------------------------------------------+
// Implementation: Radio channel manager (CHM)
//--------------------------------------------------------------------+
static uint8_t gzll_chm_rx_channel_index;

void fyll_chm_execute(void)
{
    gzll_chm_rx_channel_index = (gzll_chm_rx_channel_index + 1) % fyll_channel_tab_size;
}

void fyll_chm_reset_rx_channel_index()
{
    gzll_chm_rx_channel_index = fyll_channel_tab_index;
}

uint8_t fyll_chm_get_next_rx_channel()
{
    return (gzll_chm_rx_channel_index + 1) % fyll_channel_tab_size;
}

uint8_t fyll_chm_get_current_rx_channel()
{
    return gzll_chm_rx_channel_index;
}

//--------------------------------------------------------------------+
// Implementation: Misc. internal functions
//--------------------------------------------------------------------+
void inline fyll_delay_50us(uint8_t n)
{
    while (n--)
    {
        WaitUs(50);
    }
}

void fyll_set_system_idle(void)
{
    if (fyll_state_var == FYLL_HOST_ACTIVE)
    {
        // 等待可能在途的ack响应完成
        fyll_delay_50us(10);
    }

    // 停止rf
    rf_set_tx_rx_off();

    fyll_pending_goto_idle_f = false;
    fyll_state_var = FYLL_IDLE;
}

void fyll_reload_tries_per_channel_counter()
{
    if (fyll_sync_on_f)
    {
        fyll_tries_pr_channel_cnt = 2;
    }
    else
    {
        fyll_tries_pr_channel_cnt = FYLL_DEFAULT_CHANNEL_TAB_SIZE * 7;
    }
}

void fyll_set_radio_auto_tries()
{
    int16_t ard = FYLL_DEFAULT_PARAM_RX_PERIOD / 2;
    ard -= TRF_TX_WAIT_US + TRF_TX_SETTLE_US;
    ard -= (FYLL_CONST_BYTES_PR_PACKET + fyll_current_tx_payload_lenght) * FYLL_US_PR_BYTE;
    ard -= TRF_RX_WAIT_US + TRF_RX_WAIT_ACK_TIMEOUT_US;

    if (fyll_tries_pr_channel_cnt > 15)
    {
        trf_tpll_set_auto_retry(15, ard);
    }
    else
    {
        trf_tpll_set_auto_retry((uint8_t)(fyll_tries_pr_channel_cnt - 1), ard);
    }
}

void fyll_start_new_tx(fyll_new_tx_ch_t channel_select)
{
    uint8_t temp;

    /*
     设置在每个频道上的最大重发次数
     */
    fyll_reload_tries_per_channel_counter();
    fyll_set_radio_auto_tries();

    /*
     根据不同的模式,选择即将要发射的频道
        FYLL_CHANNEL_PREVIOUS_SUCCESS, 使用上一次传输成功的频道
        FYLL_CHANNEL_RANDOM, 使用频道表中随机频道
        FYLL_CHANNEL_ESTIMATED, 使用预计接收端下一个周期要使用的频道
        FYLL_CHANNEL_NEXT_INDEX, 使用频道表中的下一个频道
     */
    switch (channel_select)
    {
    case FYLL_CHANNEL_PREVIOUS_SUCCESS:
    default:
        temp = fyll_channel_tab_index;
        break;
    case FYLL_CHANNEL_RANDOM:
        temp = rand() % fyll_channel_tab_size;
        break;
    case FYLL_CHANNEL_ESTIMATED:
        temp = fyll_chm_get_next_rx_channel();
        break;
    case FYLL_CHANNEL_NEXT_INDEX:
        temp = (fyll_channel_tab_index + 1) % fyll_channel_tab_size;
    }

    /*
     如果频道发送了改变,需要重新配置频道
     */
    if (temp != fyll_channel_tab_index)
    {
        fyll_channel_tab_index = temp;
        //trf_tpll_set_rf_channel(fyll_channel_tab[fyll_channel_tab_index]);
        fyll_channel_switch_cnt++;
    }

    /*
     如果当前处于sync状态:
        等到下一次同步发送
     否则:
        立刻启动发送
     */
    if (fyll_sync_on_f)
    {
        fyll_pending_tx_start_f = true;
    }
    else
    {
        trf_tpll_start_tx();
    }
}

//--------------------------------------------------------------------+
// Implementation: Application interface (user functions)
//--------------------------------------------------------------------+
void trf_tpll_event_handler(trf_tpll_event_id_t evt_id);

void fyll_init(void)
{
    uint8_t err_code;

    printf("fyll_init\r\n");
    /** Init status variables */
    fyll_channel_tab_size = FYLL_DEFAULT_CHANNEL_TAB_SIZE;
    fyll_channel_tab_index = 0;

    fyll_pending_goto_idle_f = false;
    fyll_timer_period_modified_f = false;

    fyll_pending_tx_start_f = false;
    fyll_tx_success_f = true;

    fyll_sync_on_f = false;
    fyll_sync_period = 0;

    fyll_rx_dr = 0;
    fyll_rx_rssi = 0;

    /** TPLL模式及配置 */
    trf_tpll_config_t trf_tpll_config = TRF_TPLL_DEFALT_CONFIG;
#if FYLL_ROLE == FYLL_DEVICE
    trf_tpll_config.mode = TRF_TPLL_MODE_PTX;
#else
    trf_tpll_config.mode = TRF_TPLL_MODE_PRX;
#endif
    trf_tpll_config.bitrate = TRF_TPLL_BITRATE_2MBPS;
    trf_tpll_config.crc = TRF_TPLL_CRC_16BIT;
    trf_tpll_config.tx_power = FYLL_DEFAULT_PARAM_OUTPUT_POWER;
    trf_tpll_config.event_handler = trf_tpll_event_handler;
    trf_tpll_config.preamble_len = 2;
    trf_tpll_config.payload_len = 32;

    err_code = trf_tpll_init(&trf_tpll_config);
    assert((err_code == TRF_SUCCESS));

    /** 传输管道地址配置
     *
     * B80只支持管道0,其他管道配置无效
     */
    trf_tpll_set_address_width(TRF_TPLL_ADDRESS_WIDTH_5BYTES);

    uint8_t base_addr0[4] = {FYLL_BASE_ADDRESS_0};
    err_code = trf_tpll_set_base_address_0(base_addr0);
    assert((err_code == TRF_SUCCESS));

    uint8_t base_addr1[4] = {FYLL_BASE_ADDRESS_1};
    err_code = trf_tpll_set_base_address_1(base_addr1);
    assert((err_code == TRF_SUCCESS));

    uint8_t addr_prefix[6] = {FYLL_BASE_ADDRESS_PREFIX};
    err_code = trf_tpll_set_prefixes(addr_prefix, 6);
    assert((err_code == TRF_SUCCESS));

    trf_tpll_set_txpipe(TRF_TPLL_PIPE0);

    trf_tpll_enable_crcfilter(0);
    trf_tpll_set_rf_channel(23);

/** fyll 射频基带参数配置
 *
 * PRX Timing Sequence:
 * |------------|------------|------------|------------|------------|------------|
 * | rx settle  |  actual rx | tx wait    |  tx settle | actual tx  |  rx wait   |
 * |--------no ack-----------|------------ ack ----------------------------------|
 *
 * PTX Timing Sequence
 * |------------|------------|------------|------------|------------|------------|
 * |  tx settle | actual tx  |  rx wait   |  rx settle |  actual rx |  ARD       |
 * |--------no ack-----------|------------ ack ----------------------------------|
 *
 * ARD: auto retransmit delay
 */
#if FYLL_ROLE == FYLL_DEVICE
    // fyll device tx settle time = TRF_TX_SETTLE_US;
    trf_tpll_set_tx_settle(TRF_TX_SETTLE_US);
    // fyll device actual tx time = (CONST_BYTE_PR_PACKET + tx_payload_lenght) * FYLL_US_PR_BYTE

    // fyll device rx wait time = TRF_RX_WAIT_US
    trf_tpll_set_rx_wait(TRF_RX_WAIT_US);
    trf_tpll_set_tx_wait(TRF_TX_WAIT_US);

    // fyll device rx time = TRF_RX_WAIT_ACK_TIMEOUT_US
    trf_tpll_set_rx_settle(TRF_RX_SETTLE_US);
    trf_tpll_set_rx_timeout(TRF_RX_WAIT_ACK_TIMEOUT_US);
#else
    // fyll host rx time = {rx settle, actual rx} = INFINITE
    trf_tpll_set_rx_settle(TRF_RX_SETTLE_US);

    // fyll host tx wait = TRF_TX_WAIT_US
    trf_tpll_set_tx_wait(TRF_TX_WAIT_US);

    // fyll host tx settle time = TRF_TX_SETTLE_US
    trf_tpll_set_tx_settle(TRF_TX_SETTLE_US);

    // fyll host actual ack tx time = (CONST_BYTE_PR_PACKET + ack_payload_lenght) * FYLL_US_PR_BYTE

    //  e.g. ack payload = 64byte, total tx time = 114 + (1 + 2 + 5 + 1 + 64) * 4 = 292us

    // fyll host rx wait = TRF_RX_WAIT_US
    trf_tpll_set_rx_wait(TRF_RX_WAIT_US);
#endif

    /** 清空缓冲区 */
    trf_tpll_flush_rx();
    trf_tpll_flush_tx(TRF_TPLL_PIPE0);

    /** 设置全局定时器 */
    timer0_set_mode(TIMER_MODE_SYSCLK, 0, FYLL_DEFAULT_PARAM_RX_PERIOD * CLOCK_SYS_CLOCK_1US);
    timer_start(TIMER0);

    /** 设置射频基带和协议栈为空闲状态 */
    fyll_set_system_idle();
    trf_tpll_set_rf_channel(fyll_channel_tab[fyll_channel_tab_index]);

#if __DEBUG__
#if FYLL_ROLE == FYLL_DEVICE
    printf("firefly link layer start as DEVICE\r\n");
#else
    printf("firefly link layser start as HOST\r\n");
#endif
#endif

    /** RF PA GPIO config*/
#if RF_FEM_ENABLE
    // gpio_set_func(GPIO_RF_FEM_LNA, RX_CYC2LNA);
    // gpio_set_func(GPIO_RF_FEM_PA, TX_CYC2PA);
#endif
}

#if FYLL_ROLE == FYLL_HOST
static trf_tpll_payload_t ack_payload = TRF_TPLL_CREATE_PAYLOAD(TRF_TPLL_PIPE0, 0);

void fyll_rx_start()
{
    trf_tpll_start_rx();

    fyll_timeout_cnt = 0;
    fyll_state_var = FYLL_HOST_ACTIVE;
}

bool fyll_ack_payload_write(uint8_t *src, uint8_t lenght)
{
    if (lenght == 0 || lenght > FYLL_MAX_PAYLOAD_LENGTH || trf_tpll_txfifo_full(TRF_TPLL_PIPE0))
    {
        return false;
    }
    ack_payload.length = lenght;
    memcpy(ack_payload.data, src, lenght);

    if (trf_tpll_write_payload(&ack_payload) == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

#else
bool fyll_tx_data(uint8_t *src, uint8_t length)
{
    assert(src != NULL);

    if (length == 0 || length > FYLL_MAX_PAYLOAD_LENGTH)
    {
        return false;
    }

    // 构建rf传输数据包,写入发送缓冲
    trf_tpll_payload_t tx_payload = TRF_TPLL_CREATE_PAYLOAD(TRF_TPLL_PIPE0, 0);
    tx_payload.noack = 0;
    tx_payload.length = length;
    memcpy(tx_payload.data, src, length);

    fyll_current_tx_payload_lenght = length;
    /*
    从空闲状态启动发送
    */
    if (fyll_state_var == FYLL_IDLE)
    {
        // 准备开始新的传输
        fyll_timeout_cnt = 0;
        fyll_channel_switch_cnt = 0;
        fyll_try_cnt = 0;

        /** 清除缓冲区中遗留的为发送数据 */
        trf_tpll_flush_tx(TRF_TPLL_PIPE0);
        /** 新发送数据写入缓冲区 */
        trf_tpll_write_payload(&tx_payload);

        fyll_tx_success_f = false;

        fyll_device_modes_t mode = FYLL_DEFAULT_DEVICE_MODE;

        if (fyll_sync_on_f)
        {
            switch (mode)
            {
            case FYLL_DEVICE_MODE_2:
            default:
                fyll_start_new_tx(FYLL_CHANNEL_PREVIOUS_SUCCESS);
                break;
            case FYLL_DEVICE_MODE_3:
                fyll_start_new_tx(FYLL_CHANNEL_RANDOM);
                break;
            case FYLL_DEVICE_MODE_4:
                fyll_start_new_tx(FYLL_CHANNEL_ESTIMATED);
                break;
            }
        }
        else
        {
            switch (mode)
            {
            case FYLL_DEVICE_MODE_0:
            case FYLL_DEVICE_MODE_2:
                fyll_start_new_tx(FYLL_CHANNEL_PREVIOUS_SUCCESS);
                break;
            default:
                fyll_start_new_tx(FYLL_CHANNEL_RANDOM);
                break;
            }
        }

        fyll_state_var = FYLL_DEVICE_ACTIVE;

        return true;
    }
    /*
     当前正处于发送状态,
        如果缓冲区未满: 待发送数据写入缓冲区,在定时器任务里面自动启动发送
        如果缓冲区已满: 返回错误,无法启动这次传输
     */
    else
    {
        if (!trf_tpll_txfifo_full(TRF_TPLL_PIPE0))
        {
            trf_tpll_write_payload(&tx_payload);
            return true;
        }
        else
        {
            return false;
        }
    }
}

bool fyll_device_mode_2_rx_channel_match(void)
{
    if (fyll_sync_on_f)
    {
        if (fyll_channel_tab_index == fyll_chm_get_next_rx_channel())
        {
            return true;
        }

        return false;
    }
    else
    {
        return true;
    }
}

bool fyll_tx_success(void)
{
    assert(fyll_state_var != FYLL_DEVICE_ACTIVE);

    return fyll_tx_success_f;
}

#endif

fyll_state_t fyll_get_state(void)
{
    return fyll_state_var;
}

void inline fyll_tx_fifo_flush(void)
{
    trf_tpll_flush_tx(TRF_TPLL_PIPE0);
}

bool fyll_rx_data_ready()
{
    if (fyll_rx_dr != 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool fyll_rx_fifo_read(uint8_t *dst, uint8_t *length)
{
    assert(dst != NULL);
    assert(length != NULL);

    trf_tpll_payload_t rx_payload;

    if (fyll_rx_dr)
    {
        trf_tpll_read_rx_payload(&rx_payload);
        fyll_rx_rssi = rx_payload.rssi;
        *length = rx_payload.length;
        memcpy(dst, rx_payload.data, rx_payload.length);
        fyll_rx_dr--;
        printf("flly payload:");
        for (int i = 0; i < *length; i++)
        {
            printf("%d ", dst[i]);
        }
        printf("\r\n");
        return true;
    }

    return false;
}

void fyll_rx_fifo_flush(void)
{
    trf_tpll_flush_rx();
    fyll_rx_dr = 0;
}

int fyll_rx_power_rssi(void)
{
    return fyll_rx_rssi;
}

void fyll_go_to_idle()
{
    if (fyll_state_var == FYLL_DEVICE_ACTIVE)
    {
        fyll_pending_goto_idle_f = true;

        while (fyll_state_var != FYLL_IDLE)
        {
            ;
        }
    }
    else
    {
        if (fyll_state_var == FYLL_HOST_ACTIVE)
        {
            // fyll_set_system_idle();

            /* host 启动后不进入空闲模式 */
        }
    }
}

//--------------------------------------------------------------------+
// Implementation: HAL
//--------------------------------------------------------------------+

void fyll_set_timer_period(uint32_t peroid)
{
    timer0_set_mode(TIMER_MODE_SYSCLK, 0, peroid * CLOCK_SYS_CLOCK_1US);
    timer_start(TIMER0);
}

/*-----------------------------------------------------------------------------
  Implementation: Interrupt Service Routines (ISR)
-----------------------------------------------------------------------------*/

static trf_tpll_event_handler_t m_event_handler;

#define FLD_RF_IRQ_TPLL_PTX (FLD_RF_IRQ_TX | FLD_RF_IRQ_TX_DS | FLD_RF_IRQ_RETRY_HIT | FLD_RF_IRQ_RX | FLD_RF_IRQ_INVALID_PID)
#define FLD_RF_IRQ_TPLL_PRX (FLD_RF_IRQ_TX | FLD_RF_IRQ_TX_DS | FLD_RF_IRQ_RX | FLD_RF_IRQ_INVALID_PID)
/**
 * @brief Telink RF中断
 *
 */
__attribute__((section(".ram_code"))) __attribute__((optimize("-Os"))) void fyll_radio_isr(void)
{
    unsigned short src_rf = rf_irq_src_get();
    m_event_handler = trf_tpll_get_event_handler();

    static bool has_malformed_packet = false;
    uint8_t tries = 0;

    /** 发送结束,记录此次发送重试的次数 */
    if (src_rf & FLD_RF_IRQ_TX_DS || src_rf & FLD_RF_IRQ_RETRY_HIT)
    {
        tries = trf_tpll_get_transmit_attempts() + 1;
        fyll_tries_pr_channel_cnt -= tries;
        fyll_try_cnt += tries;
    }

    if (src_rf & FLD_RF_IRQ_RX)
    {
        rf_irq_clr_src(FLD_RF_IRQ_RX);
        trf_tpll_rxirq_handler(m_event_handler);
        trans_statistics_cnt_rx++;

#if FYLL_ROLE == FYLL_DEVICE
        // 接受到一个被破坏的包
        if (src_rf == 0x0001)
        {
            has_malformed_packet = true;
        }
#endif
    }

    if (src_rf & FLD_RF_IRQ_TX)
    {
        rf_irq_clr_src(FLD_RF_IRQ_TX);
        m_event_handler(TRF_TPLL_EVENT_TX_FINISH);
        trans_statistics_cnt_tx++;
    }
    /** FLD_RF_IRQ_TX_DS
     *
     * PTX: Ack Receive
     *
     * PRX: Receive 2nd Packet
     */
    if (src_rf & FLD_RF_IRQ_TX_DS)
    {
        rf_irq_clr_src(FLD_RF_IRQ_TX_DS);

#if FYLL_ROLE == FYLL_DEVICE
        if (fyll_state_var == FYLL_DEVICE_ACTIVE)
        {
            /*
             定时器时间设置为: rx周期 - (TX时间 + ACK时间 + GAP)
             */
            fyll_timer_period_modified_f = 1;
            uint16_t new_period = FYLL_DEFAULT_PARAM_RX_PERIOD;
            // e.g. Tx(32B) + Ack(32B),总时间: 5 + 114 + (11 + 32) * 4 + 5 + 114 + (11 + 32) * 4
            new_period -= FYLL_DEFAULT_PARAM_RX_PERIOD_MODIFIER;
            new_period -= (FYLL_CONST_BYTES_PR_PACKET * 2 + fyll_current_tx_payload_lenght) * FYLL_US_PR_BYTE;
            if (src_rf & FLD_RF_IRQ_RX_DR)
            {
                new_period -= 32 * FYLL_US_PR_BYTE;
                new_period -= 20;
            }
            else
            {
                new_period -= 115;
            }

            // 固定的间隔时间
            // new_period -= FYLL_DEFAULT_PARAM_RX_PERIOD_MODIFIER;
            // ACK总时间
            // new_period -= 20 * FYLL_US_PR_BYTE;     // to-do: 无法直接获取当前接收到的payload长度

            fyll_set_timer_period(new_period);

            fyll_chm_reset_rx_channel_index();

            fyll_sync_period = FYLL_DEFAULT_PARAM_MAX_SYNC_PERIOD;
            fyll_device_modes_t mode = FYLL_DEFAULT_DEVICE_MODE;
            if (mode == FYLL_DEVICE_MODE_2 || mode == FYLL_DEVICE_MODE_3 || mode == FYLL_DEVICE_MODE_4)
            {
                fyll_sync_on_f = true;
            }

            /*
             如果发送缓冲区为空,进入空闲状态
             否则,定时器任务会启动下一次传输
            */
            if (trf_tpll_txfifo_empty(TRF_TPLL_PIPE0))
            {
                fyll_tx_success_f = true;
                fyll_set_system_idle();
            }
            else
            {
                fyll_reload_tries_per_channel_counter();
                fyll_timeout_cnt = 0;
                fyll_try_cnt = 0;
            }
        }
#endif

        trans_statistics_cnt_tx_ds++;
    }

#if FYLL_ROLE == FYLL_DEVICE
    /**
     * PTX: 达到自动最大尝试次数
     */
    if (src_rf & FLD_RF_IRQ_RETRY_HIT)
    {
        rf_irq_clr_src(FLD_RF_IRQ_RETRY_HIT);
        m_event_handler(TRF_TPLL_EVENT_TX_FALIED);
        trans_statistics_cnt_max_retry++;

        /** 重用发送缓冲 */
        trf_tpll_update_txfifo_rptr(trf_tpll_get_txpipe()); // adjust rptr
        trf_tpll_reuse_tx(TRF_TPLL_PIPE0);

        fyll_timeout_cnt += tries;

        /**
         * 如果重试次数超过限制,或者用户要求停止:
         * - 强制进入空闲状态
         *
         * 否则:
         * - 根据当前状态,选择新的频道或者在当前频道继续尝试发送
         */
        if ((fyll_timeout_cnt > FYLL_DEFAULT_PARAM_TX_TIMEOUT) || fyll_pending_goto_idle_f)
        {
            fyll_set_system_idle();
        }
        else
        {
            /**
             * 在当前频道重试的次数用完:
             * - 需要切换频道
             * 否则:
             * - 在当前频道继续重试发送
             */
            if (fyll_tries_pr_channel_cnt == 0)
            {
                /*
                 如果切换频道的总次数达到限制:
                    可能是同步异常,清除同步后,用随机频道重试
                 否则:
                    使用预计频道或者频道表轮巡频道重试.
                */
                if (fyll_channel_switch_cnt > FYLL_DEFAULT_PARAM_COLLISION_CHANNEL_SWITCH_LIMIT)
                {
                    fyll_sync_period = 0;
                    fyll_sync_on_f = false;
                    fyll_start_new_tx(FYLL_CHANNEL_RANDOM);
                }
                else
                {
                    if (fyll_sync_on_f)
                    {
                        /** RX错误会导致同步异常, 立即启动下一次传输以调整周期 */
                        if (has_malformed_packet)
                        {
                            has_malformed_packet = false;
                            fyll_timer_period_modified_f = true;
                            fyll_set_timer_period(20);
                        }
                        fyll_start_new_tx(FYLL_CHANNEL_ESTIMATED);
                    }
                    else
                    {
                        fyll_start_new_tx(FYLL_CHANNEL_NEXT_INDEX);
                    }
                }
            }
            else
            {
                fyll_set_radio_auto_tries();
                trf_tpll_start_tx();
            }
        }
    }
#endif

    if (src_rf & FLD_RF_IRQ_INVALID_PID)
    {
        rf_irq_clr_src(FLD_RF_IRQ_INVALID_PID);
        trans_statistics_cnt_invalid_pid++;
    }

    /** 清除非预期的中断 */
#if FYLL_ROLE == FYLL_DEVICE
    if (src_rf & ~FLD_RF_IRQ_TPLL_PTX)
    {
        rf_irq_clr_src(FLD_RF_IRQ_ALL);
    }
#else
    if (src_rf & ~FLD_RF_IRQ_TPLL_PRX)
    {
        rf_irq_clr_src(FLD_RF_IRQ_ALL);
    }
#endif
}

/**
 * @brief TRF TPLL事件处理
 *
 * @param evt_id
 * @arg TRF_TPLL_EVENT_TX_FINISH
 * @arg TRF_TPLL_EVENT_TX_FALIED
 * @arg TRF_TPLL_EVENT_RX_RECEIVED
 */
__attribute__((section(".ram_code"))) __attribute__((optimize("-Os"))) void trf_tpll_event_handler(trf_tpll_event_id_t evt_id)
{
    switch (evt_id)
    {
    /** 数据发送已完成 */
    case TRF_TPLL_EVENT_TX_FINISH:
        break;
    case TRF_TPLL_EVENT_TX_FALIED:
        break;
    case TRF_TPLL_EVENT_RX_RECEIVED:
        trans_statistics_cnt_rx_dr++;
        fyll_rx_dr++;
        break;
    }
}

void fyll_trans_statistics(void);

/**
 * @brief    Telink Timer 中断
 *
 * @return
 */
_attribute_ram_code_sec_noinline_ void fyll_timer_isr()
{
    if (timer_get_interrupt_status(FLD_TMR_STA_TMR0))
    {
        timer_clear_interrupt_status(FLD_TMR_STA_TMR0); // clear irq status

        fyll_chm_execute();

/********************************
 * 设备端定时器任务处理
 *******************************/
#if FYLL_ROLE == FYLL_DEVICE
        /** 重置定时器周期 */
        if (fyll_timer_period_modified_f)
        {
            fyll_timer_period_modified_f = false;
            fyll_set_timer_period(FYLL_DEFAULT_PARAM_RX_PERIOD);
        }
        /** 当前发送状态, 检查是否有等待启动的发送 */
        if (fyll_state_var == FYLL_DEVICE_ACTIVE)
        {
            if (fyll_pending_tx_start_f)
            {
                if (!fyll_sync_on_f || (fyll_channel_tab_index == fyll_chm_get_current_rx_channel()))
                {
                    trf_tpll_start_tx();
                    fyll_pending_tx_start_f = 0;
                }
            }
        }

        /** 一定时间后,清除同步状态 */
        if (fyll_sync_period > 0)
        {
            fyll_sync_period--;
        }
        else
        {
            fyll_sync_on_f = false;
        }

/********************************
 * 主机端定时器任务处理
 *******************************/
#else
        if (fyll_state_var == FYLL_HOST_ACTIVE)
        {
            uint8_t temp = fyll_chm_get_current_rx_channel();

            // 在新频道上接收
            if (temp != fyll_channel_tab_index)
            {
                // trf_tpll_set_new_rf_channel(fyll_channel_tab[temp]);
                // rf_set_tx_rx_off();
                trf_tpll_set_rf_channel(fyll_channel_tab[temp]);
                fyll_channel_tab_index = temp;
            }
            trf_tpll_start_rx();
        }
#endif
    }
}

void fyll_trans_statistics(void)
{
    // static unsigned long tick = 0;
    const char *state_str[3] = {
        "FYLL_IDLE",
        "FYLL_DEVICE_ACTIVE",
        "FYLL_HOST_ACTIVE"};

    // if (clock_time_exceed(tick, 1000*1000))
    // {
    //     tick = clock_time();
    printf("[%8d]:"
           "state = %s,"
           "tx = %d,"
           "rx = %d,"
           "rx_dr = %d,"
           "invalid_pid = %d,"
           "tx_ds = %d,"
           "max_retry = %d,"
           "sync = %d,"
           "fyll_tries_pr_channel_cnt = %d"
           "\r\n",
           clock_time() / SYSTEM_TIMER_TICK_1US,
           state_str[fyll_get_state()],
           trans_statistics_cnt_tx,
           trans_statistics_cnt_rx,
           trans_statistics_cnt_rx_dr,
           trans_statistics_cnt_invalid_pid,
           trans_statistics_cnt_tx_ds,
           trans_statistics_cnt_max_retry,
           fyll_sync_on_f,
           fyll_tries_pr_channel_cnt);
    // }
}

#if FYLL_ROLE == FYLL_DEVICE

uint8_t fyll_test_data[64] = {0, 1, 2, 3, 4, 5, 6, 7};
static int fyll_test_tx_cnt = 0;

#define TEST_MAX_TX_CNT 100

void fyll_test_task()
{
    static unsigned long tick_for_tx = 0;

    // if (clock_time_exceed(tick_for_tx, 5000*1000))
    // if (fyll_device_mode_2_rx_channel_match())
    if (fyll_test_tx_cnt < TEST_MAX_TX_CNT)
    {
        tick_for_tx = clock_time();

        if (fyll_tx_data(fyll_test_data, 32))
        {
            fyll_test_tx_cnt++;
        }

        while (fyll_get_state() != FYLL_IDLE)
        {
            ;
        }

        if (fyll_rx_data_ready())
        {
            fyll_rx_fifo_flush();
        }

        if (fyll_test_tx_cnt == TEST_MAX_TX_CNT)
        {
            fyll_trans_statistics();
            // fyll_test_tx_cnt = 0;
            // WaitMs(500);
        }

        // if (fyll_get_state() == FYLL_IDLE)
        // {
        //     if (fyll_tx_success())
        //     {
        //         printf("fyll device tx 8 bytes success!\r\n");
        //     }
        //     else
        //     {
        //         printf("fyll device tx failed!\r\n");
        //         fyll_test_tx_cnt = TEST_MAX_TX_CNT;
        //         fyll_trans_statistics();
        //     }
        // }
        // WaitMs(10);
    }
}
#endif

bool fyll_sample_send_one_packet(uint8_t *fyll_test_data, uint8_t length)
{
    // 1. 待发送数据写入发送缓冲区,并启动发送
    if (fyll_tx_data(fyll_test_data, length) != true)
    {
        // 无法启动发送
        //  1. 参数错误
        //  2. 发送缓冲区已满

        return false;
    }

    // 2. 等待发送完成,或超时退出.FYLL会自动在多个频道尝试发送多次,直到发送成功或者超过次数限制,进入空闲状态
    while (fyll_get_state() != FYLL_IDLE)
    {
        ;
    }

    return fyll_tx_success();
}

// bool fyll_sample_send_bulk()
// {
//     static unsigned long tick_for_tx = 0;

//     uint8_t fyll_test_data[1000] = {0,1,2,3,4,5,6,7};
//     uint16_t fyll_test_data_len = 1000;

//     bool res;
//     // 1. 待发送数据写入发送缓冲区,并启动发送
//     uint16_t tx_bytes;
//     do
//     {
//         tx_bytes = fyll_test_data_len > 32 ? 32 : fyll_test_data_len

//         if (fyll_tx_data(fyll_test_data, tx_bytes))
//         {
//             fyll_test_data_len -= tx_bytes;
//         }
//         else
//         {

//         }
//     }while(fyll_text_data_len != 0);

//     // 2. 等待发送完成,或超时退出.FYLL会自动在多个频道尝试发送多次,直到发送成功或者超过次数限制,进入空闲状态
//     while (fyll_get_state() != FYLL_IDLE)
//     {
//         ;
//     }

//     // 3. 检查发送结果
//     if (fyll_tx_success())
//     {
//         printf("fyll device tx bytes success!\r\n");
//     }
//     else
//     {
//         printf("fyll device tx failed!\r\n");
//     }
// }

#if FYLL_ROLE == FYLL_HOST

uint8_t fyll_test_data[FYLL_MAX_PAYLOAD_LENGTH];

uint8_t fyll_test_ack_data[32];

void fyll_test_task()
{
    static bool is_host_actived = false;
    if (is_host_actived == false)
    {
        is_host_actived = true;

        fyll_go_to_idle();
        fyll_ack_payload_write(fyll_test_ack_data, 32);
        fyll_rx_start();
    }

    uint8_t length;
    if (fyll_rx_data_ready())
    {
        fyll_rx_fifo_read(fyll_test_data, &length);

        fyll_ack_payload_write(fyll_test_ack_data, 32);

        printf("fyll host rx %d bytes\r\n", length);
    }

    static unsigned long tick = 0;
    if (clock_time_exceed(tick, 1000 * 1000))
    {
        tick = clock_time();
        fyll_trans_statistics();
    }
}
#endif
