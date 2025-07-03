
#ifndef _PAN216_H_
#define _PAN216_H_
#include "pan216_reg.h"

#if (EASY_RF)
#include "easy_reg.h"
#define RF_CONFIG(x) EASY_RF_##x
#else
#include "common_reg.h"
#endif

#define RF_IT_TX_IRQ         0x80
#define RF_IT_MAX_RT_IRQ     0x40
#define RF_IT_ADDR_ERR_IRQ   0x20
#define RF_IT_CRC_ERR_IRQ    0x10
#define RF_IT_LEN_ERR_IRQ    0x08
#define RF_IT_PID_ERR_IRQ    0x04
#define RF_IT_RX_TIMEOUT_IRQ 0x02
#define RF_IT_RX_IRQ         0x01

#define STATE_CMD_DEEPSLEEP  0
#define STATE_CMD_SLEEP      1
#define STATE_CMD_STB1       2
#define STATE_CMD_STB2       3
#define STATE_CMD_STB3       4
#define STATE_CMD_TX         5
#define STATE_CMD_RX         6

typedef enum
{
    PAN216_TX,
    PAN216_RX,
    PAN216_TRX_UNDEFINE,
} PAN216_TRXMode_t;

typedef enum
{
    PAN216_WIDTH_2BYTES = 2,
    PAN216_WIDTH_3BYTES,
    PAN216_WIDTH_4BYTES,
    PAN216_WIDTH_5BYTES,
} PAN216_AddrWidth_t;

typedef enum
{
    PAN216_PIPE0 = (uint8_t)0x00, // pipe0
    PAN216_PIPE1 = (uint8_t)0x01, // pipe1
    PAN216_PIPE2 = (uint8_t)0x02, // pipe2
    PAN216_PIPE3 = (uint8_t)0x03, // pipe3
    PAN216_PIPE4 = (uint8_t)0x04, // pipe4
    PAN216_PIPE5 = (uint8_t)0x05, // pipe5
} PAN216_Pipe_t;

// Enumeration type that defines the transmission modes of the
typedef enum
{
    PAN216_BLE_LEN_FILTER_DISABLE, // BLE packet length filter disabled
    PAN216_BLE_LEN_FILTER_EQUAL,   // BLE packet length filter enabled for equal length
    PAN216_BLE_LEN_FILTER_EXCEED,  // BLE packet length filter enabled for length exceed
                                   // payload_length
    PAN216_BLE_LEN_FILTER_BENEATH  // BLE packet length filter enabled for length beneath
                                   // payload_length
} PAN216_BleLenFilter_t;

/**
 * @brief BLE WhiteList Match Mode, Filtered by contents of
 *
 */
typedef enum
{
    PAN216_BLE_WhiteList_DISABLE,
    PAN216_BLE_WhiteList_1Byte,
    PAN216_BLE_WhiteList_2Byte,
    PAN216_BLE_WhiteList_3Byte,
    PAN216_BLE_WhiteList_4Byte,
    PAN216_BLE_WhiteList_5Byte,
    PAN216_BLE_WhiteList_6Byte,
} PAN216_BLEWLMatchMode_t;

typedef enum
{
    BLE_CHANNEL_37 = 2,  // 2402MHz
    BLE_CHANNEL_38 = 26, // 2426MHz
    BLE_CHANNEL_39 = 80, // 2480MHz
} PAN216_BLEChannel_t;

/**
 * @brief BLE Whiten Init Phase for channel 37，38，39
 *
 */
typedef enum
{
    PAN216_BLE_WH_INIPHA_CH37 = 0x53,
    PAN216_BLE_WH_INIPHA_CH38 = 0x33,
    PAN216_BLE_WH_INIPHA_CH39 = 0x73,
} PAN216_BLE_WhIniPhase_t;

typedef enum
{
    PAN216_CRC_off,   // CRC disabled
    PAN216_CRC_1byte, // 1-byte CRC
    PAN216_CRC_2byte, // 2-byte CRC
    PAN216_CRC_3byte, // 3-byte CRC
    PAN216_CRC_Max
} PAN216_Crc_t;

// RF output power in TX mode
typedef enum
{
    PAN216_TXPWR_n45dBm = -45,
    PAN216_TXPWR_n44dBm = -44,
    PAN216_TXPWR_n41dBm = -41,
    PAN216_TXPWR_n40dBm = -40,
    PAN216_TXPWR_n37dBm = -37,
    PAN216_TXPWR_n33dBm = -33,
    PAN216_TXPWR_n29dBm = -29,
    PAN216_TXPWR_n28dBm = -28,
    PAN216_TXPWR_n25dBm = -25,
    PAN216_TXPWR_n24dBm = -24,
    PAN216_TXPWR_n23dBm = -23,
    PAN216_TXPWR_n22dBm = -22,
    PAN216_TXPWR_n21dBm = -21,
    PAN216_TXPWR_n20dBm = -20,
    PAN216_TXPWR_n19dBm = -19,
    PAN216_TXPWR_n18dBm = -18,
    PAN216_TXPWR_n17dBm = -17,
    PAN216_TXPWR_n16dBm = -16,
    PAN216_TXPWR_n15dBm = -15,
    PAN216_TXPWR_n14dBm = -14,
    PAN216_TXPWR_n13dBm = -13,
    PAN216_TXPWR_n12dBm = -12,
    PAN216_TXPWR_n11dBm = -11,
    PAN216_TXPWR_n10dBm = -10,
    PAN216_TXPWR_n9dBm = -9,
    PAN216_TXPWR_n8dBm = -8,
    PAN216_TXPWR_n7dBm = -7,
    PAN216_TXPWR_n6dBm = -6,
    PAN216_TXPWR_n5dBm = -5,
    PAN216_TXPWR_n4dBm = -4,
    PAN216_TXPWR_n3dBm = -3,
    PAN216_TXPWR_n2dBm = -2,
    PAN216_TXPWR_n1dBm = -1,
    PAN216_TXPWR_0dBm_LOWPWR = 99,
    PAN216_TXPWR_0dBm = 0,
    PAN216_TXPWR_1dBm = 1,
    PAN216_TXPWR_2dBm = 2,
    PAN216_TXPWR_3dBm = 3,
    PAN216_TXPWR_4dBm = 4,
    PAN216_TXPWR_5dBm = 5,
    PAN216_TXPWR_6dBm = 6,
    PAN216_TXPWR_7dBm = 7,
    PAN216_TXPWR_8dBm = 8,
    PAN216_TXPWR_9dBm = 9,
    PAN216_TXPWR_9_5dBm = 10,
} PAN216_TxPower_t;

// Enumeration of data rate
typedef enum
{
    PAN216_DR_1Mbps,   // 1Mbps data rate
    PAN216_DR_2Mbps,   // 2Mbps data rate
    PAN216_DR_250Kbps, // 250kbps data rate
    PAN216_DR_Max
} PAN216_DataRate_t;

// Enumeration of work mode
typedef enum
{
    PAN216_CHIPMODE_XN297,
PAN216_CHIPMODE_FS01,
    PAN216_CHIPMODE_FS32,
    PAN216_CHIPMODE_BLE,
} PAN216_ChipMode_t;

// Enumeration of enhance mode
typedef enum
{
    PAN216_WORKMODE_NORMAL,
    PAN216_WORKMODE_ENHANCE,
} PAN216_WorkMode_t;

// Enumeration type that defines the transmission modes of the
typedef enum
{
    PAN216_TX_MODE_SINGLE,    // Single transmission mode
    PAN216_TX_MODE_CONTINOUS, // Continuous transmission mode
} PAN216_TxMode_t;

// Enumeration type that defines the transmission modes of the
typedef enum
{
    PAN216_RX_MODE_SINGLE,              // Single packet reception mode
    PAN216_RX_MODE_SINGLE_WITH_TIMEOUT, // Single packet reception mode with timeout
    PAN216_RX_MODE_CONTINOUS            // Continuous packet reception mode
} PAN216_RxMode_t;

typedef enum
{
    PAN216_ENDIAN_LITTLE,
    PAN216_ENDIAN_BIG,
} PAN216_Endian_t;

/*************************************************************************
 *  Chip Status Operations.
 **************************************************************************/
PAN216_Err_t PAN216_Init(void);
void PAN216_Calibration(void);
void PAN216_FinishCalibration(void);
PAN216_Err_t PAN216_EnterSleep(void);
PAN216_Err_t PAN216_ExitSleep(void);
PAN216_Err_t PAN216_ExitTxMode(void);
PAN216_Err_t PAN216_ExitRxMode(void);
PAN216_Err_t PAN216_TxStart(void);
PAN216_Err_t PAN216_RxStart(void);
PAN216_Err_t PAN216_PrepareTRxWithAck(PAN216_TRXMode_t Mode, bool EnAck);

/*************************************************************************
 *   Configuration Operations.
 **************************************************************************/
PAN216_Err_t PAN216_SetChannel(uint8_t Channel);
PAN216_Err_t PAN216_SetAddrWidth(PAN216_AddrWidth_t AddrWidth);
PAN216_Err_t PAN216_SetRxAddr(PAN216_Pipe_t Pipe, uint8_t *Addr, uint8_t len);
PAN216_Err_t PAN216_SetTxAddr(uint8_t *Addr, uint8_t len);
PAN216_Pipe_t PAN216_GetRxPipeNum(void);

/*************************************************************************
 *   IRQ Operations.
 **************************************************************************/
PAN216_Err_t PAN216_ConfigIT(uint8_t RF_IT);
uint8_t PAN216_GetIRQFlags(void);
PAN216_Err_t PAN216_ClearIRQFlags(uint8_t flags);
PAN216_Err_t PAN216_EnableInterfaceMuxIRQ(bool NewState);

/*************************************************************************
 *   FIFO and Frame Operations.
 **************************************************************************/
PAN216_Err_t PAN216_WritePayload(uint8_t *pBuf, uint8_t Length);
PAN216_Err_t PAN216_SendPacket(uint8_t *pBuf, uint8_t Length);
uint8_t PAN216_GetRecvLen(void);
PAN216_Err_t PAN216_ReadNordicPktHeader(uint8_t *Header0, uint8_t *Header1, uint8_t *Length);
PAN216_Err_t PAN216_SetWhiteInitVal(uint8_t val);

/*************************************************************************
 *   RSSI Operations.
 **************************************************************************/
uint16_t PAN216_GetRssiCode(void);
int PAN216_GetRssiDBm(void);
int PAN216_GetRealTimeRSSI(void);

#endif
