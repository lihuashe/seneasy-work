#include "fct_2_4g.h"
#include "pan216_ext.h"

/*============================================================================*
 *                              Local Variables
 *============================================================================*/

enum LLCtrlCmd
{
    LL_CTRL_ACK,
    LL_CTRL_PAIR,
    LL_CTRL_CHANNEL_CONFIG,
};

enum LLAppCmd
{
    LL_APP_END,
    LL_APP_SUB_FRAME,
};

enum LLState
{
    IDLE,
    RX,
    WAIT_TX_ACK
};

#define __IS_LL_CTRL(byte) (byte >> 7)
#define __GET_LL_PID(byte) ((byte >> 5) & 0x3)
#define __GET_LL_CTRL_CMD(byte) (byte & 0x1F)

#define __SET_LL_CTRL_BIT(byte) (byte |= 0x80)
#define __SET_LL_PID(byte, tx_pid) (byte |= ((tx_pid & 0x3) << 5))
#define __SET_LL_CTRL_CMD(byte, cmd) (byte |= (cmd & 0x1F))

#define BUF_MAX_SIZE 64
static unsigned char tx_buf[BUF_MAX_SIZE] __attribute__((aligned(4)));
static unsigned char rx_bufDataLenByte;
static unsigned char tx_bufDataLenByte;
static unsigned char tx_frameCnt;
static unsigned char tx_frameSize;
static unsigned char rx_frameCnt;
static RingBuffer txRingBuf;
static RingBuffer *rxRingBuf;
static unsigned char tx_pid;
static unsigned char rx_pid = 5;
static unsigned char txRetryCnt;
static unsigned char txRetryMxaCnt = 3;
static unsigned char txRetryTimeoutMs = 5;
static unsigned char rxSubFrameTimeoutMs = 15;
static volatile unsigned char ll_state = RX;

static bool isRecvDone;

static uint32_t txRetryTick, rxSubFrameTick;

/*============================================================================*
 *                              Global Variables
 *============================================================================*/

/*============================================================================*
 *                              Local Functions
 *============================================================================*/



/*============================================================================*
 *                              HAL API Functions
 *============================================================================*/

const static RFConfig_t fctRfConfig = {
    .Channel = 4,      // 0 ~ 83 -> 2400MHz ~ 2483MHz
    .TxPower = PAN216_TXPWR_0dBm,       // 0 ~ 9
    .DataRate = PAN216_DR_2Mbps,        // 1Mbps, 2Mbps, 250Kbps
    .ChipMode = PAN216_CHIPMODE_FS01,   //
    .EnWhite = false,                   // false, true
    .Crc = PAN216_CRC_off,              // disable, 1byte, 2byte, 3byte
    .TxLen = 32,                        // 1 ~ 128
    .RxLen = 32,                        // 1 ~ 128
    .TxAddrWidth = PAN216_WIDTH_4BYTES, // 2 ~ 5
    .TxAddr = {0x95, 0x76, 0x51, 0x39},
    .RxAddrWidth = PAN216_WIDTH_4BYTES, // 2 ~ 5
    .RxAddr =
        {
            {true, {0x95, 0x76, 0x51, 0x39}},
            {false, {0}},
            {false, {0}},
            {false, {0}},
            {false, {0}},
        },
    .Endian = PAN216_ENDIAN_BIG, // big, little
    .crcSkipAddr = false,        // false, true
    .EnRxPlLenLimit = false,     // false, true

    // Do not edit the following fields
    .WorkMode = PAN216_WORKMODE_ENHANCE, 
    .EnDPL = true,
    .EnTxNoAck = true,
    .EnManuPid = true,
    .TRxDelayTimeUs = 0,
    .RxTimeoutUs = 30000,
    .AutoDelayUs = 0,
    .AutoMaxCnt = 0,
    .TxMode = PAN216_TX_MODE_SINGLE,
    .RxMode = PAN216_RX_MODE_CONTINOUS,
};

void __2_4g_driver_init()
{
    
}

uint8_t __getRxData(uint8_t *pData, uint8_t len)
{

}

void __sendData2Rx(uint8_t *pData, uint8_t len)
{

}

void __setChannel(uint8_t channel)
{

}

/*============================================================================*
 *                              Global Functions
 *============================================================================*/

void fct_2_4g_init(RingBuffer *rbuf)
{
    RingBuffer_Init(&txRingBuf, tx_buf, sizeof(tx_buf));
    rxRingBuf = rbuf;
}

bool fct_2_4g_isRecv()
{
    return false;
}

int8_t fct_2_4g_sendto(uint8_t *pData, uint8_t len)
{
    return 0;
}

void fct_2_4g_proc()
{
}
