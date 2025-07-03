#ifndef _PAN216_EXT_H_
#define _PAN216_EXT_H_

#include "pan216.h"

#if (!EASY_RF)

#define CALIBRATION_DEBUG_ON   0

#define FILTER_OFFSET_CAL_MODE 0
#define FILTER_BW_CAL_MODE     1
#define FILTER_DCOC_CAL_MODE   2

#define ADDR_MAX_LEN 5
#define PIPE_MAX_NUM 6


typedef enum
{
    REF_CLK_32M,
    REF_CLK_16M
} PAN216_ClkSel_t;

// CRC encoding scheme

typedef enum
{
    PAN216_PRIMODE_DIS,
    PAN216_PRIMODE_S2,
    PAN216_PRIMODE_S8,
} PAN216_S2S8Mode_t;

typedef enum
{
    PAN216_IRQ_PINMUX_DEFAULT = 0, // IRQ pinmux default
    PAN216_IRQ_PINMUX_OCLK,        // IRQ muxed as OCLK output.
    PAN216_IRQ_PINMUX_PACTRL,      // IRQ muxed as external PA.
} PAN216_IRQPinMux_t;

typedef struct
{
    bool EnPipe;
    u8 Addr[ADDR_MAX_LEN];
} RxPipeAddr_t;

typedef struct
{
    uint8_t Channel;
    PAN216_TxPower_t TxPower;
    PAN216_DataRate_t DataRate;
    PAN216_Crc_t Crc;
    PAN216_ChipMode_t ChipMode;
    PAN216_WorkMode_t WorkMode;

    PAN216_AddrWidth_t TxAddrWidth;
    uint8_t TxAddr[ADDR_MAX_LEN];
    PAN216_AddrWidth_t RxAddrWidth;
    RxPipeAddr_t RxAddr[PIPE_MAX_NUM];

    u8 TxLen;
    u8 RxLen;
    bool EnWhite;

    bool EnDPL;
    bool EnTxNoAck;
    bool crcSkipAddr;       // 0-include address, 1-skip address
    PAN216_Endian_t Endian; // 0-little endian,   1-big endian

    u16 TRxDelayTimeUs;
    u16 RxTimeoutUs;
    u16 AutoDelayUs;
    u8 AutoMaxCnt;
    u8 EnManuPid;
    bool EnRxPlLenLimit;

    PAN216_TxMode_t TxMode;
    PAN216_RxMode_t RxMode;

#ifdef ATCMD
    bool EnAPL;
    u8 scrSkipAddr; // 0-include address, 1-skip address
    u8 BLEHeadNum;  // 0: disable, 1: len, 2: head0&len, 3: head0&head1&len
    u8 BleHeadLen;
    u8 BLEHead0;
    u8 BLEHead1;
    u8 S2S8Mode;
    u8 WhiteInit;        // ble white init val
    u8 BleWLMatchMode;   // 0:disable, 1-6:match 1-6 bytes
    u8 BleWLMatchStart;  // in payload, start position of match
    u8 BleFilterWL[6];   // filter value array
    u8 BleFilterLenMode; // 0: disable, 1:equal, 2:exceed, 3:beneath
    u8 BleFilterLen;     // filter length
    u8 TxPid;
    u8 RxPid;
    u8 TxBuf[128];
    u8 RxBuf[128];
#endif

} RFConfig_t;

typedef struct
{
    PAN216_BLEChannel_t Channel;
    PAN216_TxPower_t TxPower;
    PAN216_DataRate_t DataRate;
    u8 TxLen;
    u8 RxLen;

    u8 BLEHeadNum; // 0: disable, 1: len, 2: head0&len, 3: head0&head1&len
    u8 BleHeadLen;
    u8 BLEHead0;
    u8 BLEHead1;
    PAN216_S2S8Mode_t S2S8Mode;
    u8 WhiteInit;                               // ble white init val
    PAN216_BLEWLMatchMode_t WhiteListMatchMode; // 0:disable, 1-6:match 1-6 bytes
    u8 WhiteListOffset;                         // in payload, start position of match
    u8 WhiteList[6];                           // filter value array
    u8 WhiteListLen;                           // filter value length
    PAN216_BleLenFilter_t LengthFilterMode;     // 0: disable, 1:equal, 2:exceed, 3:beneath
} BLEConfig_t;

PAN216_Err_t PAN216_BleConfig(BLEConfig_t *Config);
PAN216_Err_t PAN216_SetUpConfig(RFConfig_t *pCfg);

PAN216_Err_t PAN216_SetTxMode(PAN216_TxMode_t TxMode);
PAN216_Err_t PAN216_SetRxMode(PAN216_RxMode_t RxMode);
PAN216_Err_t PAN216_GetChipMode(PAN216_ChipMode_t *chipMode);
PAN216_Err_t PAN216_SetWorkMode(PAN216_WorkMode_t Mode);
PAN216_Err_t PAN216_EnableDynamicPL(u8 NewState);
PAN216_Err_t PAN216_EnableTxNoAck(u8 NewState);
PAN216_Err_t PAN216_SetAutoRetrans(u16 DelayUs, u8 MaxCnt);
PAN216_Err_t PAN216_SetAckPipe(u8 pipe);

/**Timeout */
PAN216_Err_t PAN216_SetWaitAckTimeout(u16 AckTimeoutUs);
PAN216_Err_t PAN216_SetTRxTransTime(u16 SwitchTimeUs);
PAN216_Err_t PAN216_EnableFifo128bytes(bool NewState);
/*************************************************************************
 *   CRC and Whiten Operations.
 **************************************************************************/
PAN216_Err_t PAN216_CrcSkipAddr(bool is_excluded);
PAN216_Err_t PAN216_WhiteSkipAddr(bool addr_excluded);
PAN216_Err_t PAN216_EnableWhiten(bool NewState);

/*************************************************************************
 *   BLE Configuration Operations.
 **************************************************************************/
PAN216_Err_t PAN216_SetEndian(PAN216_Endian_t Endian);
PAN216_Err_t PAN216_SetNordicPktHeader(u8 HeaderEn, u8 HeaderLen);
PAN216_Err_t PAN216_SetBleWhitelist(u8 Start, u8 *FilterBuf, u8 FilterLen);
PAN216_Err_t PAN216_SetBleLenFilter(PAN216_BleLenFilter_t FilterType);
PAN216_Err_t PAN216_SetBleWLMatchMode(PAN216_BLEWLMatchMode_t MatchMode);
PAN216_Err_t PAN216_SetS2S8Mode(PAN216_S2S8Mode_t mode);
PAN216_Err_t PAN216_WriteNordicPktHeader(u8 Header0, u8 Header1, u8 Length);
/*************************************************************************
 *   Chip Interrupt Request Operations.
 **************************************************************************/

PAN216_Err_t PAN216_setIRQPinMux(PAN216_IRQPinMux_t Mux);

/*************************************************************************
 *   PID Operations.
 **************************************************************************/
PAN216_Err_t PAN216_EnableManualPid(u8 NewState);
PAN216_Err_t PAN216_WriteTxManualPid(u8 Pid);
PAN216_Err_t PAN216_WriteRxManualPid(u8 Pid);
u8 PAN216_ReadTxNoackBit(void);

/*************************************************************************
 *   Carrier Wave
 **************************************************************************/
PAN216_Err_t PAN216_TxCarrierWave(void);
PAN216_Err_t PAN216_ExitCarrierWave(void);

PAN216_Err_t PAN216_SetTxPower(PAN216_TxPower_t TxPower);
/*************************************************************************
 *   Configuration Operations.
 **************************************************************************/
/*Basic configuraions*/
PAN216_Err_t PAN216_SetDataRate(PAN216_DataRate_t DataRate);
PAN216_Err_t PAN216_SetCrcScheme(PAN216_Crc_t Crc);
PAN216_Err_t PAN216_SetRxPayloadLen(u8 PayloadLen);
PAN216_Err_t PAN216_SetTxPayloadLen(u8 PayloadLen);
PAN216_Err_t PAN216_EnableRxPipe(PAN216_Pipe_t Pipe);
PAN216_Err_t PAN216_DisableRxPipe(PAN216_Pipe_t Pipe);
// Chip mode
PAN216_Err_t PAN216_SetChipMode(PAN216_ChipMode_t chipMode, PAN216_Endian_t endian,
                                bool crcSkipAddr);

PAN216_Err_t PAN216_RxLengthLimit(bool NewState);
PAN216_Err_t PAN216_SoftReset(void);
u8 PAN216_ReadLQI(void);
PAN216_Err_t PAN216_ConfigLQI(PAN216_DataRate_t DataRate);

// Calibration Operations
PAN216_Err_t PAN216_ReadVCOCode(void);
PAN216_Err_t PAN216_ReadTpCode(void);
void ReadBWDcocCal(uint8_t mode, uint8_t index);
#endif
#endif
