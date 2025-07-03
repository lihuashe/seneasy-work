/**
 * @file 2_4g_io.c
 * @author likeliang (likeliang@seneasy.com)
 * @brief 2.4G数据通信流，负责通信机制控制
 * @version 0.1
 * @date 2025-03-25
 *
 * @copyright Copyright (c) 2025
 *
 */
#define __DEBUG__ __DEBUG_CLOSE__

#include "pan216_ext.h"
#include "py32f0xx_hal.h"
#include "time_utils.h"
#include "2_4g_io.h"
#include <string.h>
#include "app.h"

/*============================================================================*
 *                              Local Variables
 *============================================================================*/

#define INTERVAL_Xa 60
#define INTERVAL_Xb 180
static volatile uint32_t last_tx_time_ms = 0;
static rf_io_buf_t pingpong_buf[2];
static uint8_t pingpong_buf_index = 0; // rf正在发送的buf
static uint8_t rf_io_status = RF_IO_STATUS_IDEL;
static volatile uint8_t xn297l_send_index;
static uint8_t Tx_length;
static uint8_t TxBuf[48];

/*============================================================================*
 *                              Global Variables
 *============================================================================*/

const RFConfig_t a7535_gRfConfig = {
    .Channel = A7535_CHANNEL,      // 0 ~ 83 -> 2400MHz ~ 2483MHz
    .TxPower = PAN216_TXPWR_0dBm,       // 0 ~ 9
    .DataRate = PAN216_DR_1Mbps,        // 1Mbps, 2Mbps, 250Kbps
    .ChipMode = PAN216_CHIPMODE_FS01,   //
    .EnWhite = false,                   // false, true
    .Crc = PAN216_CRC_off,              // disable, 1byte, 2byte, 3byte
    .TxLen = 46,                        // 1 ~ 128
    .RxLen = 46,                        // 1 ~ 128
    .TxAddrWidth = PAN216_WIDTH_4BYTES, // 2 ~ 5
    .TxAddr = {0x15, 0x59, 0x23, 0xc6, 0x29},
    .RxAddrWidth = PAN216_WIDTH_4BYTES, // 2 ~ 5
    .RxAddr =
        {
            {true, {0xF3, 0xF3, 0xF0, 0xF0}},
            {false, {0}},
            {false, {0}},
            {false, {0}},
            {false, {0}},
        },
    .Endian = PAN216_ENDIAN_BIG, // big, little
    .crcSkipAddr = false,        // false, true
    .EnRxPlLenLimit = false,     // false, true

    // Do not edit the following fields
    .WorkMode = PAN216_WORKMODE_NORMAL, // Normal
    .EnDPL = false,
    .EnTxNoAck = true,
    .EnManuPid = false,
    .TRxDelayTimeUs = 0,
    .RxTimeoutUs = 30000,
    .AutoDelayUs = 0,
    .AutoMaxCnt = 0,
    .TxMode = PAN216_TX_MODE_SINGLE,
    .RxMode = PAN216_RX_MODE_CONTINOUS,
};

const RFConfig_t xn297l_gRfConfig = {
    .Channel = 35,                      // 0 ~ 83 -> 2400MHz ~ 2483MHz
    .TxPower = PAN216_TXPWR_0dBm,       // 0 ~ 9
    .DataRate = PAN216_DR_1Mbps,        // 1Mbps, 2Mbps, 250Kbps
    .ChipMode = PAN216_CHIPMODE_XN297,  //
    .EnWhite = true,                    // false, true
    .Crc = PAN216_CRC_2byte,            // disable, 1byte, 2byte, 3byte
    .TxLen = 30,                        // 1 ~ 128
    .RxLen = 30,                        // 1 ~ 128
    .TxAddrWidth = PAN216_WIDTH_5BYTES, // 2 ~ 5
    .TxAddr = {XN297L_CTRL_ADDR},
    .RxAddrWidth = PAN216_WIDTH_5BYTES, // 2 ~ 5
    .RxAddr =
        {
            {true, {XN297L_CTRL_ADDR}},
            {false, {0}},
            {false, {0}},
            {false, {0}},
            {false, {0}},
        },
    .Endian = PAN216_ENDIAN_BIG, // big, little
    .crcSkipAddr = false,        // false, true
    .EnRxPlLenLimit = false,     // false, true

    // Do not edit the following fields
    .WorkMode = PAN216_WORKMODE_ENHANCE, // Normal
    .EnDPL = false,
    .EnTxNoAck = true,
    .EnManuPid = false,
    .TRxDelayTimeUs = 0,
    .RxTimeoutUs = 0,
    .AutoDelayUs = 0,
    .AutoMaxCnt = 0,
    .TxMode = PAN216_TX_MODE_SINGLE,
    .RxMode = PAN216_RX_MODE_CONTINOUS,
};

bool g_xn297l_pair_f;
TIM_HandleTypeDef rf_2_4g_TimHandle;
/*============================================================================*
 *                              Local Functions
 *============================================================================*/

static uint16_t double_bits(uint8_t input)
{
    uint16_t result = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        if (input & 0x80)
        {
            result |= (1 << (15 - i * 2));
            result |= (1 << (14 - i * 2));
        }
        else
        {
            result &= ~(0 << (15 - i * 2));
            result &= ~(0 << (14 - i * 2));
        }
        input = input << 1;
    }

    return ((result & 0xFF00) >> 8) | ((result & 0x00FF) << 8);
}

static uint8_t decode_double_bits(uint16_t encoded)
{
    uint8_t result = 0;
    for (int i = 0; i < 8; i++)
    {
        uint16_t two_bits = (encoded >> (14 - i * 2)) & 0x3; // 0x3 = 0b11

        if (two_bits == 0x3) // 11 -> 1
        {
            result |= (1 << (7 - i));
        }
        else if (two_bits == 0x0) // 00 -> 0
        {
            // result
        }
    }
    return ((result & 0xF0) >> 4) | ((result & 0x0F) << 4);
}

static void data_1mbps_to_500kbps(const uint8_t *data, uint8_t data_len, uint8_t *playload, uint8_t playload_len)
{
    if (playload_len < data_len * 2)
        return;
    uint16_t *playload_p = (uint16_t *)playload;

    for (int i = data_len - 1; i >= 0; i--)
    {
        playload_p[i] = double_bits(data[i]);
    }
}

static void decode_data_1mbps_to_500kbps(const uint16_t *encoded, uint8_t encoded_len, uint8_t *decoded, uint8_t decoded_len)
{
    if (decoded_len < encoded_len)
        return;

    for (int i = 0; i < encoded_len; i++)
    {
        decoded[i] = decode_double_bits(encoded[i]);
    }
}

/**
 * @brief FV-85BH1C 跳频通信协议
 *
 */
static void rf_2_4g_channel_hop(void)
{
    uint8_t channel_table[] = XN297L_CHANNEL_TABLE;
    static uint8_t channel_index = 0;

    PAN216_SetChannel(channel_table[channel_index]);

    if (channel_index < sizeof(channel_table) - 1)
    {
        channel_index++;
    }
    else
    {
        channel_index = 0;
    }
}

static void rf_2_4g_send_packet(uint8_t *payload, uint8_t length)
{
    static uint8_t cnt = 0;
    static uint8_t last_pingpong_buf_index = 0;

    if (app.chipMode == XN297L)
    {
        TxBuf[0] = cnt;
        memcpy(&TxBuf[1], payload, length);
        Tx_length = length + 1;
        HAL_TIM_Base_Start_IT(&rf_2_4g_TimHandle);
        if (pingpong_buf_index != last_pingpong_buf_index)
        {
            cnt++;
            last_pingpong_buf_index = pingpong_buf_index;
        }
    }
    else if (app.chipMode == A7535)
    {
        data_1mbps_to_500kbps(payload, length, TxBuf, length * 2);
        RF_SendPacket(TxBuf, length * 2);
    }
}

/*============================================================================*
 *                              Global Functions
 *============================================================================*/

void rf_2_4g_init()
{
    switch (app.chipMode)
    {
    case A7535:
        PAN216_SetUpConfig((RFConfig_t *)&a7535_gRfConfig);
        break;
    case XN297L:
        PAN216_SetUpConfig((RFConfig_t *)&xn297l_gRfConfig);
        break;
    default:
        return;
    }
    // 配置定时器6ms触发一次中断
    rf_2_4g_TimHandle.Instance = TIM17;                                        /* Select TIM17 */
    rf_2_4g_TimHandle.Init.Period = 14400 - 1;                                 /* Auto-reload value */
    rf_2_4g_TimHandle.Init.Prescaler = 10 - 1;                                 /* Prescaler of 1000-1 */
    rf_2_4g_TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;             /* Clock not divided */
    rf_2_4g_TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;                   /* Up counting mode */
    rf_2_4g_TimHandle.Init.RepetitionCounter = 1 - 1;                          /* No repetition */
    rf_2_4g_TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; /* Auto-reload register not buffered */
    if (HAL_TIM_Base_Init(&rf_2_4g_TimHandle) != HAL_OK)                       /* Initialize TIM16 */
    {
#if __DEBUG__ >= __DEBUG_ERR__
        my_log("\033[1;31m>E%d[%s] rf_2_4g_TimHandle Init Fail\n\033[0m", __LINE__, __FUNCTION__);
#endif
    }
    HAL_TIM_Base_Stop_IT(&rf_2_4g_TimHandle);

    /* Set interrupt priority */
    HAL_NVIC_SetPriority(TIM17_IRQn, 1, 0);
    /* Enable TIM1 interrupt */
    HAL_NVIC_EnableIRQ(TIM17_IRQn);
}

void rf_2_4g_tim17_irq()
{
    xn297l_send_index++;
    if (XN297L_TX_CNT <= xn297l_send_index)
    {
        xn297l_send_index = 0;
        HAL_TIM_Base_Stop_IT(&rf_2_4g_TimHandle);
    }
    RF_SendPacket(TxBuf, Tx_length);
    rf_2_4g_channel_hop();
    last_tx_time_ms = clock_time();
}

void rf_2_4g_proc(void)
{
    if (!pingpong_buf[pingpong_buf_index].len)
        return;

    switch (rf_io_status)
    {
    case RF_IO_STATUS_IDEL:
        rf_2_4g_send_packet(pingpong_buf[pingpong_buf_index].data, pingpong_buf[pingpong_buf_index].len);
        rf_io_status = RF_IO_STATUS_SEND_1st_FRAME;
        last_tx_time_ms = clock_time();
        break;
    case RF_IO_STATUS_SEND_1st_FRAME:
        if (clock_time_exceed(last_tx_time_ms, INTERVAL_Xa))
        {
            rf_2_4g_send_packet(pingpong_buf[pingpong_buf_index].data, pingpong_buf[pingpong_buf_index].len);
            rf_io_status = RF_IO_STATUS_SEND_2nd_FRAME;
            last_tx_time_ms = clock_time();
        }
        break;
    case RF_IO_STATUS_SEND_2nd_FRAME:
        if (clock_time_exceed(last_tx_time_ms, INTERVAL_Xb))
        {
            pingpong_buf[pingpong_buf_index].len = 0;
            pingpong_buf_index++;
            pingpong_buf_index %= 2;
            rf_io_status = RF_IO_STATUS_IDEL;
            if (g_xn297l_pair_f)
            {
                g_xn297l_pair_f = false;
                uint8_t pair_addr[] = {XN297L_CTRL_ADDR};
                PAN216_SetTxAddr(pair_addr, sizeof(pair_addr));
            }
        }
    default:
        break;
    }
}

void rf_2_4g_entryPair()
{
    uint8_t pair_addr[] = {XN297L_PAIR_ADDR};
    PAN216_SetTxAddr(pair_addr, sizeof(pair_addr));
    g_xn297l_pair_f = true;
}

void rf_2_4g_sendto(uint8_t *data, uint8_t len)
{
    uint8_t i = pingpong_buf_index;
    if (rf_io_status != RF_IO_STATUS_IDEL)
    // if (pingpong_buf[pingpong_buf_index].len)
    {
        i++;
        i %= 2;
    }

    memcpy(&pingpong_buf[i].data, data, len);
    pingpong_buf[i].len = len;
#if __DEBUG__ >= __DEBUG_DEV__
    my_log(">D%d[%s] : ", __LINE__, __FUNCTION__);
    for (size_t i = 0; i < len; i++)
    {
        my_log("%02X, ", data[i]);
    }
    my_log("\n");
#endif
}

uint8_t rf_2_4g_read(uint8_t *data, uint8_t len)
{
    return RF_ReadPacket(data, len);
}

bool rf_2_4g_is_busy()
{
    return rf_io_status != RF_IO_STATUS_IDEL;
}
