/**********************************************************************************************************************
 * @file     fw_upgrade_process.h
 * @version  V3.0
 * @date     2022/07/29
 * @history
 * @note     OTA Service
 **********************************************************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2022, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 *********************************************************************************************************************/


#ifndef __FW_UPGRADE_PROCESS_H__
#define __FW_UPGRADE_PROCESS_H__


//=====================================================================================================================
// DEFINE
//=====================================================================================================================
#define FW_UPGRADE_PACKET_LEN_MIN          ( 6 )
#define FW_UPGRADE_CMD_UPGRADE_REQ_LEN_MIN ( 13 )
#define FW_UPGRADE_PACKET_START_CODE       ( 0x3D4C )
#define FW_UPGRADE_PACKET_TAIL_CODE        ( 0xA1B2 )

#define FW_UPGRADE_EVENT_HEADER_LEN        ( 4 )

#define FW_UPGRADE_PACKET_SIZE             ( 50 )
#define FW_UPGRADE_PAGE_SIZE               ( 500 )

#define FW_UPGRADE_CACHE_BUF_SIZE          ( 512 )


//=====================================================================================================================
// TYPEDEF
//=====================================================================================================================
typedef enum
{
    FW_UPGRADE_CMD       = 0x01,
    FW_UPGRADE_RESPONSE  = 0x02,
    FW_UPGRADE_REPORT    = 0x03,

} enFwUpgradeCmdType_t;

typedef enum
{
    FW_UPGRADE_CMD_MIN             = 1,
    FW_UPGRADE_CMD_GET_FW_VERSION  = 1,
    FW_UPGRADE_CMD_HANDSHAKE       = 2,
    FW_UPGRADE_CMD_UPGRADE_REQ     = 3,
    FW_UPGRADE_CMD_SEND_CODE       = 4,
    FW_UPGRADE_CMD_SEND_CODE_DONE  = 5,
    FW_UPGRADE_CMD_RESET_SOC       = 6,
    FW_UPGRADE_CMD_MAX             = FW_UPGRADE_CMD_RESET_SOC,

} enFwUpgradeCmd_t;

typedef enum
{
    FW_UPGRADE_STEP_GET_FW_VERSION  = 0,
    FW_UPGRADE_STEP_HANDSHAKE       = 1,
    FW_UPGRADE_STEP_UPGRADE_REQ     = 2,
    FW_UPGRADE_STEP_SEND_CODE       = 3,
    FW_UPGRADE_STEP_SEND_CODE_DONE  = 4,
    FW_UPGRADE_STEP_RESET_SOC       = 5,

} enFwUpgradeStep_t;

typedef enum
{
    FW_UPGRADE_HANDSHAKE_SUCCESS     = 0x00,
    FW_UPGRADE_HANDSHAKE_FAIL        = 0x01,

} enfwUpgradeHandshakestate_t;

typedef struct __attribute__((packed))
{
    uint8_t  u8CmdType;     // Command Type, @ref enFwUpgradeCmdType_t.
    uint8_t  u8Cmd;         // Command, @ref enFwUpgradeCmd_t.
    uint16_t u16DataLen;    // Data Length.
    uint8_t *pu8Data;       // Pointer to Save Data.
    uint16_t u16Crc;        // The Struct CRC

} stFwUpgradeProtocol_t;

typedef enum
{
    SOC_TYPE_MXD2660 = 0x01,
    SOC_TYPE_MXD2657 = 0x02,
    SOC_TYPE_MXD2670 = 0x03,

} enSocType_t;

typedef enum
{
    FW_UPGRADE_HANDSHAKE_BYPASS         = 0x01,
    FW_UPGRADE_HANDSHAKE_ENABLE         = 0x02,
    FW_UPGRADE_HANDSHAKE_ENCRYP_ENABLE  = 0x03,

} enFwUpgradeHandshakeType_t;

typedef struct __attribute__ ((packed))
{
    uint8_t  u8SocType;    // SoC Type, @ref enSocType_t.
    uint8_t  u8RomVer;     // ROM Version.
    uint8_t  u8Boot2Ver;   // Boot2 Version.
    uint16_t u16AppVer;    // Application version.
    uint8_t  u8Handshake;  // Handshake flag, @ref enFwUpgradeHandshakeType_t.
    uint8_t  au8Mac[6];    // Local MAC.

} stFwUpgradeVer_t;

typedef enum
{
    FW_UPGRADE_NOT_ALLOWED = 0x00,
    FW_UPGRADE_ALLOWED     = 0x01,
    FW_UPGRADE_VER_ERROR   = 0x03,

} enFwUpgradeFlag_t;

typedef struct __attribute__ ((packed))
{
    uint8_t  u8UpgradeFlag;  // @ref enFwUpgradeFlag_t.
    uint16_t u16PacketSize;
    uint16_t u16PageSize;

} stFwUpgradeTransmit_t;

typedef struct __attribute__ ((packed))
{
    uint32_t u32RcvCodeLen;
    uint32_t u32RcvCodeCRC;
    uint32_t u32FlashAddrOffset;
    uint32_t u32FlashSavedCodeCRC;

} stFwUpgradeProcess_t;

typedef struct __attribute__((aligned(4)))
{
    //Information
    uint8_t  u8SocType;
    uint8_t  u8RomVer;
    uint8_t  u8Boot2Ver;
    uint16_t u16AppCodeVer;
    uint32_t u32BinCodeSize;
    uint32_t u32BinCodeCrc;

} stFwUpgradeFileInfo_t;

//typedef enum
//{
//    FW_UPGRADE_FLASH_WRITE_SUCCESS     = 0x00,
//    FW_UPGRADE_FLASH_WRITE_CRC16_ERROR = 0x01,
//    FW_UPGRADE_FLASH_WRITE_FAIL        = 0x02
//    
//} enfwUpgradeFlashWriteFlag_t;


//typedef struct __attribute__ ((packed))
//{
//    uint8_t  OTA_FLASH_WRITE_STATE; 
//    uint32_t FLASH_ADDR;

//} stfwUpgradeFlashWritestateFAIL_t;

// Firmware upgrade code information.
typedef struct __attribute__((aligned(4)))
{
     // Information
    uint16_t u16MXDFlag;               // MXD Flag: 2A4B, customer can not change.
    uint8_t  u8RomVer;                 // Rom version.
    uint8_t  u8Boot2Ver;               // Secondary boot version.
    uint32_t u32NeedFwUpgradeFlag;     // Need upgrade or not.

    // Firmware upgrade saved information
    uint32_t u32BinFlashAddrOffset;    // The offset address of bin code saved in flash.
    uint32_t u32BinCodeSize;           // Bin code size
    uint32_t u32BinCodeCrc;            // Bin code crc32

    // Current running code region.
    uint32_t u32RunningCodeAddrOffset; // Current running code offset address.
    uint32_t u32RunningCodeSize;       // Current running code size.

    uint32_t u32StructCrc;             // Struct information crc32.

} stFwUpgradeCodeInfo_t;

typedef struct __attribute__((packed))
{
    uint8_t  u8Ret;
    uint32_t u32FlashAddrOffset;

} stFwUpgradeFlashError_t;

typedef struct __attribute__((packed))
{
    uint8_t  u8Ret;
    uint32_t u32RcvCodeCrc;
    uint32_t u32FlashCodeCrc;

} stFwUpgradeSendCodeState_t;

typedef struct __attribute__((packed))
{
    uint8_t  u8Ret;
    uint16_t u16ResetTime;

} stFwUpgradeResetState_t;


extern uint8_t fw_upgrade_process(uint8_t *pu8Buf, uint16_t u16Len);


#endif /* __FW_UPGRADE_PROCESS_H__ */

