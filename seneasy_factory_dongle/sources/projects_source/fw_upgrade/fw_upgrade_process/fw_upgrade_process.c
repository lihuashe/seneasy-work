/**********************************************************************************************************************
 * @file     ble_ota_service.c
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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "RegMXD2670.h"
#include "err_def.h"
#include "hw_crg.h"

#include "app_cfg.h"
#include "version.h"
#include "crc.h"
#include "utility.h"

#include "fw_upgrade_interface.h"
#include "fw_upgrade_flash.h"
#include "fw_upgrade_process.h"


//=====================================================================================================================
// LOCAL VARIABLES
//=====================================================================================================================
// Running code info.
static stFwUpgradeVer_t m_stLocalDeviceVer =
{
    .u8SocType   = SOC_TYPE_MXD2670,
    .u8RomVer    = ROM_VERSION,
    .u8Boot2Ver  = BOOT2_VERSION,
    .u16AppVer   = APP_VERSION,
    .u8Handshake = FW_UPGRADE_HANDSHAKE_ENABLE,
};

// FW upgrade code info.
static stFwUpgradeCodeInfo_t m_stCodeInfo =
{
    .u16MXDFlag = MXD_BLE_SOC_FLAG,
    .u8RomVer   = ROM_VERSION,
    .u8Boot2Ver = BOOT2_VERSION,
    .u32NeedFwUpgradeFlag = FW_UPGRADE_FLAG,

    .u32BinFlashAddrOffset = FLASH_FW_UPGRADE_CODE_ADDR_OFFSET,

    .u32RunningCodeAddrOffset = FLASH_MP_APP_CODE_ADDR_OFFSET,
    .u32RunningCodeSize = FLASH_MP_APP_CODE_SIZE_MAX,
};

// Firmware upgrade file version.
static stFwUpgradeFileInfo_t m_stFwUpgradeFileVer;
static stFwUpgradeTransmit_t m_stTransmit;
static stFwUpgradeProcess_t m_stFwUpgradeProcess;

static uint8_t m_au8FwUpgradeBuf[FW_UPGRADE_CACHE_BUF_SIZE];
static uint8_t m_au8FwUpgradeEventBuf[FW_UPGRADE_CACHE_BUF_SIZE];

static uint8_t m_au8HandShakeData[] = {32, 0, 2, 68, 86, 52};
static uint8_t m_u8UpgradeStepFlag = FW_UPGRADE_STEP_GET_FW_VERSION;
static uint32_t m_u32RcvCodeLen = 0;




/**
 * @brief  Unpack firmware upgrade protocol.
 * @param  pu8Buf: Pointer to data buffer.
 * @param  u16Len: Buffer length.
 * @return status: @ref EN_ERR_STA_T.
 */
static uint8_t fw_upgrade_process_unpack(uint8_t *pu8Buf, uint16_t u16Len, stFwUpgradeProtocol_t *pstProtocol)
{
    uint16_t u16CRC;
    uint16_t u16CRCTemp;
    uint16_t u16DataLenTemp;
    uint16_t u16StartCode;
    uint16_t u16PackageTail;

    if (NULL == pstProtocol)
    {
        return ERR_PARA_ERR;
    }

    u16StartCode = BUILD_UINT16(pu8Buf[0], pu8Buf[1]);
    if (FW_UPGRADE_PACKET_START_CODE != u16StartCode)
    {
        PRINTF("Start Code error %04X.\n", u16StartCode);
        return ERR_PARA_ERR;
    }

    u16PackageTail = BUILD_UINT16(pu8Buf[u16Len - 2], pu8Buf[u16Len - 1]);
    if (FW_UPGRADE_PACKET_TAIL_CODE != u16PackageTail)
    {
        PRINTF("Package Tail error %04X.\n", u16PackageTail);
        return ERR_PARA_ERR;
    }

    // 1. Check data length.
    if ((u16Len - 4) < FW_UPGRADE_PACKET_LEN_MIN)
    {
        PRINTF("fw upgrade unpack packet len error.\n");
        return ERR_PARA_ERR;
    }

    // 2. Check command type.
    if (FW_UPGRADE_CMD != pu8Buf[2])
    {
        PRINTF("fw upgrade unpack cmd type error.\n");
        return ERR_PARA_ERR;
    }
    pstProtocol->u8CmdType = pu8Buf[2];

    // 3. Check command.
    if (pu8Buf[3] < FW_UPGRADE_CMD_MIN || pu8Buf[3] > FW_UPGRADE_CMD_MAX)
    {
        PRINTF("fw upgrade unpack cmd error.\n");
        return ERR_PARA_ERR;
    }
    pstProtocol->u8Cmd = pu8Buf[3];

    // 4. Check data length.
    u16DataLenTemp = BUILD_UINT16(pu8Buf[4], pu8Buf[5]);
    if (u16DataLenTemp + FW_UPGRADE_PACKET_LEN_MIN != (u16Len - 4))
    {
        PRINTF("fw upgrade unpack data len error.\n");
        return ERR_PARA_ERR;
    }
    pstProtocol->u16DataLen = u16DataLenTemp;

    // 5. Check crc.
    u16CRC = rom_get_crc16_ccitt(&pu8Buf[2], u16DataLenTemp + 4, 0);
    u16CRCTemp = BUILD_UINT16(pu8Buf[u16Len - 4], pu8Buf[u16Len - 3]);

    // PRINTF("%d, %d\n", u16CRC, u16CRCTemp);

    if (u16CRC != u16CRCTemp)
    {
        PRINTF("fw upgrade unpack cmd crc error.  src %04x  rec %04x\n", u16CRC, u16CRCTemp);
        return ERR_PARA_ERR;
    }
    pstProtocol->u16Crc = u16CRCTemp;

    // 6. Read cmd data.
    if (pstProtocol->u16DataLen != 0)
    {
        pstProtocol->pu8Data = &pu8Buf[6];
    }

    // PRINTF("\nfw upgrade unpack: %d, %d, %04X\n", pstProtocol->u8Cmd, pstProtocol->u16DataLen, pstProtocol->u16Crc);

    return ERR_STA_OK;
}

/**
 * @brief  Calculate the CRC16 of the response packet and send the response packet.
 * @param  u8Cmd: Command.
 * @param  pu8Data: Pointer to the response data.
 * @param  u16DataLen: Response data length.
 */
static void fw_upgrade_process_response(uint8_t u8Cmd, uint8_t *pu8Data, uint16_t u16DataLen)
{
    uint16_t u16CRC;
    uint16_t u16Len;

    stFwUpgradeProtocol_t stResp =
    {
        .u8CmdType  = FW_UPGRADE_RESPONSE,
        .u8Cmd      = u8Cmd,
        .u16DataLen = u16DataLen
    };


    u16Len = FW_UPGRADE_EVENT_HEADER_LEN + stResp.u16DataLen;

    // Calculate the CRC16.
    memcpy(m_au8FwUpgradeEventBuf, (uint8_t *)&stResp, FW_UPGRADE_EVENT_HEADER_LEN);
    memcpy(&m_au8FwUpgradeEventBuf[FW_UPGRADE_EVENT_HEADER_LEN], pu8Data, u16DataLen);
    u16CRC = rom_get_crc16_ccitt(m_au8FwUpgradeEventBuf, u16Len, 0);

    m_au8FwUpgradeEventBuf[u16Len] = LO_UINT16(u16CRC);
    m_au8FwUpgradeEventBuf[u16Len + 1] = HI_UINT16(u16CRC);

    // Send response data.
    fw_upgrade_interface_send(m_au8FwUpgradeEventBuf, u16Len + 2);
}

/**
 * @brief  Process firmware upgrade get version command.
 * @param  pstProtocol: Pointer to fw upgrade protocol struct.
 * @return status: @ref EN_ERR_STA_T.
 */
static uint8_t fw_upgrade_process_fw_version(stFwUpgradeProtocol_t *pstProtocol)
{
    uint8_t u8Ret;
    uint8_t au8Mac[6] = LOCAL_DEVICE_MAC;

    PRINTF("Get running code info.\n");

    // Update parameter first.
    u8Ret = ERR_PARA_ERR;
    m_u8UpgradeStepFlag = FW_UPGRADE_STEP_GET_FW_VERSION;

    if (0 != pstProtocol->u16DataLen)
    {
        PRINTF("[%d]Parameter Error: %d\n", __LINE__, pstProtocol->u16DataLen);
        fw_upgrade_process_response(pstProtocol->u8Cmd, &u8Ret, sizeof(u8Ret));
        return u8Ret;
    }

    // Erase fw upgrade information.
    fw_upgrade_flash_erase(FLASH_FW_UPGRADE_CODE_INFO_ADDR_OFFSET, sizeof(stFwUpgradeCodeInfo_t));

    // Set device address.
    memcpy(m_stLocalDeviceVer.au8Mac, au8Mac, sizeof(au8Mac));

    // Calculate CRC16 and send response.
    fw_upgrade_process_response(pstProtocol->u8Cmd, (uint8_t *)&m_stLocalDeviceVer, sizeof(m_stLocalDeviceVer));

    // Update fw upgrade step.
    if (m_stLocalDeviceVer.u8Handshake == FW_UPGRADE_HANDSHAKE_BYPASS)
    {
        m_u8UpgradeStepFlag = FW_UPGRADE_STEP_UPGRADE_REQ;
    }
    else if (m_stLocalDeviceVer.u8Handshake == FW_UPGRADE_HANDSHAKE_ENABLE)
    {
        m_u8UpgradeStepFlag = FW_UPGRADE_STEP_HANDSHAKE;
    }
    else
    {
        PRINTF("Handshake not define\n");
    }

    return ERR_STA_OK;
}

/**
 * @brief  Process firmware upgrade handshake command.
 * @param  pstProtocol: Pointer to fw upgrade protocol struct.
 * @return status: @ref EN_ERR_STA_T.
 */
static uint8_t fw_upgrade_process_handshake(stFwUpgradeProtocol_t *pstProtocol)
{
    uint8_t u8Ret;
    uint8_t u8State;

    PRINTF("Firmware upgrade handshake.\n");

    // Check data length.
    if (sizeof(m_au8HandShakeData) != pstProtocol->u16DataLen)
    {
        PRINTF("[%d]Parameter Error: %d\n", __LINE__, pstProtocol->u16DataLen);

        u8Ret = ERR_PARA_ERR;
        u8State = FW_UPGRADE_HANDSHAKE_FAIL;
        m_u8UpgradeStepFlag = FW_UPGRADE_STEP_GET_FW_VERSION;
        fw_upgrade_process_response(pstProtocol->u8Cmd, &u8State, sizeof(u8State));
        return u8Ret;
    }

    // Check handshake data.
    if ((0 == memcmp(m_au8HandShakeData, pstProtocol->pu8Data, sizeof(m_au8HandShakeData))))
    {
        u8Ret = ERR_STA_OK;
        u8State = FW_UPGRADE_HANDSHAKE_SUCCESS;
        m_u8UpgradeStepFlag = FW_UPGRADE_STEP_UPGRADE_REQ;
    }
    else
    {
        u8Ret = ERR_STA_ERROR;
        u8State = FW_UPGRADE_HANDSHAKE_FAIL;
        m_u8UpgradeStepFlag = FW_UPGRADE_STEP_GET_FW_VERSION;
    }

    fw_upgrade_process_response(pstProtocol->u8Cmd, &u8State, sizeof(u8State));

    return u8Ret;
}

/**
 * @brief  Process firmware upgrade request command.
 * @param  pstProtocol: Pointer to fw upgrade protocol struct.
 * @return status: @ref EN_ERR_STA_T.
 */
static uint8_t fw_upgrade_process_upgrade_request(stFwUpgradeProtocol_t *pstProtocol)
{
    uint8_t u8FwUpgradeLicense = FW_UPGRADE_ALLOWED;

    PRINTF("Firmware upgrade request.\n");

    // Check data length.
    if (FW_UPGRADE_CMD_UPGRADE_REQ_LEN_MIN != pstProtocol->u16DataLen)
    {
        PRINTF("[%d]Parameter Error: %d\n", __LINE__, pstProtocol->u16DataLen);

        u8FwUpgradeLicense = FW_UPGRADE_NOT_ALLOWED;
        m_u8UpgradeStepFlag = FW_UPGRADE_STEP_GET_FW_VERSION;
        fw_upgrade_process_response(pstProtocol->u8Cmd, &u8FwUpgradeLicense, sizeof(u8FwUpgradeLicense));
        return ERR_PARA_ERR;
    }

    // Firmware upgrade file information.
    m_stFwUpgradeFileVer.u8SocType  = pstProtocol->pu8Data[0];
    m_stFwUpgradeFileVer.u8RomVer   = pstProtocol->pu8Data[1];
    m_stFwUpgradeFileVer.u8Boot2Ver = pstProtocol->pu8Data[2];
    m_stFwUpgradeFileVer.u16AppCodeVer  = BUILD_UINT16(pstProtocol->pu8Data[3], pstProtocol->pu8Data[4]);
    m_stFwUpgradeFileVer.u32BinCodeSize = BUILD_UINT32(pstProtocol->pu8Data[5], pstProtocol->pu8Data[6],
                                                        pstProtocol->pu8Data[7], pstProtocol->pu8Data[8]);
    m_stFwUpgradeFileVer.u32BinCodeCrc  = BUILD_UINT32(pstProtocol->pu8Data[9], pstProtocol->pu8Data[10],
                                                        pstProtocol->pu8Data[11], pstProtocol->pu8Data[12]);

    // FW upgrade code info.
    m_stCodeInfo.u32BinCodeSize = m_stFwUpgradeFileVer.u32BinCodeSize;
    m_stCodeInfo.u32BinCodeCrc = m_stFwUpgradeFileVer.u32BinCodeCrc;

    if (FW_UPGRADE_ALLOWED == u8FwUpgradeLicense)
    {
        // Check device version.
        if ((m_stLocalDeviceVer.u8Boot2Ver == m_stFwUpgradeFileVer.u8Boot2Ver) \
         && (m_stLocalDeviceVer.u8RomVer == m_stFwUpgradeFileVer.u8RomVer)     \
         && (m_stLocalDeviceVer.u8SocType == m_stFwUpgradeFileVer.u8SocType))
        {
            // Erase firmware upgrade code flash.
            fw_upgrade_flash_erase(FLASH_FW_UPGRADE_CODE_ADDR_OFFSET, FLASH_FW_UPGRADE_CODE_SIZE_MAX);

            // Init fw upgrade variables.
            m_u32RcvCodeLen = 0;

            m_stFwUpgradeProcess.u32FlashAddrOffset = FLASH_FW_UPGRADE_CODE_ADDR_OFFSET;
            m_stFwUpgradeProcess.u32RcvCodeLen = 0;
            m_stFwUpgradeProcess.u32RcvCodeCRC = CRC32_DEFAULT;
            m_stFwUpgradeProcess.u32FlashSavedCodeCRC = CRC32_DEFAULT;

            // Pack response data.
            m_stTransmit.u8UpgradeFlag = u8FwUpgradeLicense;
            m_stTransmit.u16PacketSize = FW_UPGRADE_PACKET_SIZE;
            m_stTransmit.u16PageSize = FW_UPGRADE_PAGE_SIZE;

            // Calculate CRC16 and send response.
            m_u8UpgradeStepFlag = FW_UPGRADE_STEP_SEND_CODE;
            fw_upgrade_process_response(pstProtocol->u8Cmd, (uint8_t *)&m_stTransmit, sizeof(stFwUpgradeTransmit_t));

            PRINTF("Start to receive firmware upgrade code.\n");

            return ERR_STA_OK;
        }
        else
        {
            u8FwUpgradeLicense = FW_UPGRADE_VER_ERROR;
        }
    }
    else
    {
        u8FwUpgradeLicense = FW_UPGRADE_NOT_ALLOWED;
    }

    fw_upgrade_process_response(pstProtocol->u8Cmd, &u8FwUpgradeLicense, sizeof(u8FwUpgradeLicense));

    return ERR_PARA_ERR;
}

uint32_t u32FlashOffset = FLASH_FW_UPGRADE_CODE_ADDR_OFFSET;

/**
 * @brief  Process firmware upgrade send code command.
 * @param  pstProtocol: Pointer to received command data.
 * @return status: @ref EN_ERR_STA_T.
 */
static uint8_t fw_upgrade_process_send_fw(stFwUpgradeProtocol_t *pstProtocol)
{
    uint8_t  u8Ret;
    uint32_t u32CRC;

    // Check data length.
    if (pstProtocol->u16DataLen > m_stTransmit.u16PacketSize)
    {
        PRINTF("[%d]Parameter Error: %d\n", __LINE__, pstProtocol->u16DataLen);

        u8Ret = ERR_PARA_ERR;
        m_u8UpgradeStepFlag = FW_UPGRADE_STEP_GET_FW_VERSION;
        fw_upgrade_process_response(pstProtocol->u8Cmd, &u8Ret, sizeof(u8Ret));
        return ERR_PARA_ERR;
    }

    // Save fw upgrade code to temporary buffer.
    memcpy(&m_au8FwUpgradeBuf[m_u32RcvCodeLen], pstProtocol->pu8Data, pstProtocol->u16DataLen);
    m_stFwUpgradeProcess.u32RcvCodeLen += pstProtocol->u16DataLen;
    m_u32RcvCodeLen += pstProtocol->u16DataLen;

    // Write code to flash.
    if (m_u32RcvCodeLen >= m_stTransmit.u16PageSize \
        || m_stFwUpgradeProcess.u32RcvCodeLen >= m_stFwUpgradeFileVer.u32BinCodeSize)
    {
        PRINTF("Rcv code: %d%%\n", m_stFwUpgradeProcess.u32RcvCodeLen * 100 / m_stFwUpgradeFileVer.u32BinCodeSize);

        // Calculate received code crc.
        u32CRC = m_stFwUpgradeProcess.u32RcvCodeCRC;
        u32CRC = rom_get_crc32(m_au8FwUpgradeBuf, m_u32RcvCodeLen, u32CRC);
        m_stFwUpgradeProcess.u32RcvCodeCRC = u32CRC;

        // Write code to flash.
        u8Ret = fw_upgrade_flash_write(m_au8FwUpgradeBuf, m_stFwUpgradeProcess.u32FlashAddrOffset, m_u32RcvCodeLen);
        m_stFwUpgradeProcess.u32FlashAddrOffset += m_u32RcvCodeLen;
        m_u32RcvCodeLen = 0;

        if (ERR_STA_OK == u8Ret)
        {
            fw_upgrade_process_response(pstProtocol->u8Cmd, &u8Ret, sizeof(u8Ret));
        }
        else
        {
            stFwUpgradeFlashError_t stState;
            stState.u8Ret = 0x02;
            stState.u32FlashAddrOffset = m_stFwUpgradeProcess.u32FlashAddrOffset;

            fw_upgrade_process_response(pstProtocol->u8Cmd, (uint8_t *)&stState, sizeof(stState));
            return ERR_PARA_ERR;
        }
    }

    // Next step: Verify fw upgrade code.
    if (m_stFwUpgradeProcess.u32RcvCodeLen >= m_stFwUpgradeFileVer.u32BinCodeSize)
    {
        m_u8UpgradeStepFlag = FW_UPGRADE_STEP_SEND_CODE_DONE;
    }

    return u8Ret;
}

/**
 * @brief  Process firmware upgrade send code done command.
 * @param  pstProtocol: Pointer to received command data.
 * @return status: @ref EN_ERR_STA_T.
 */
static uint8_t fw_upgrade_process_send_fw_done(stFwUpgradeProtocol_t *pstProtocol)
{
    uint8_t  u8Ret;
    uint32_t u32CRC;
    uint32_t u32AddrOffset;
    uint32_t u32CodeLen;
    uint32_t u32CodeTempLen;

    // Check data length.
    if (0 != pstProtocol->u16DataLen)
    {
        u8Ret = ERR_PARA_ERR;
        m_u8UpgradeStepFlag = FW_UPGRADE_STEP_GET_FW_VERSION;
        fw_upgrade_process_response(pstProtocol->u8Cmd, &u8Ret, sizeof(u8Ret));
        return ERR_PARA_ERR;
    }

    // Verify flash
    if (m_stFwUpgradeProcess.u32RcvCodeLen >= m_stFwUpgradeFileVer.u32BinCodeSize)
    {
        PRINTF("m_stFwUpgradeProcess.u32RcvCodeLen: %d\n",m_stFwUpgradeProcess.u32RcvCodeLen);
        u32CRC = CRC32_DEFAULT;
        u32AddrOffset = FLASH_FW_UPGRADE_CODE_ADDR_OFFSET;
        u32CodeTempLen = m_stFwUpgradeFileVer.u32BinCodeSize;

        while (u32CodeTempLen)
        {
            u32CodeLen = (u32CodeTempLen > sizeof(m_au8FwUpgradeBuf)) ? sizeof(m_au8FwUpgradeBuf) : u32CodeTempLen;
            
            fw_upgrade_flash_read(m_au8FwUpgradeBuf, u32AddrOffset, u32CodeLen);

            u32CRC = rom_get_crc32(m_au8FwUpgradeBuf, u32CodeLen, u32CRC);
            
            u32AddrOffset += u32CodeLen;
            u32CodeTempLen -= u32CodeLen;
        }

        m_stFwUpgradeProcess.u32FlashSavedCodeCRC = u32CRC;
    }

    PRINTF("Receive code done. %08X, %08X, %08X.\n",
            m_stFwUpgradeFileVer.u32BinCodeCrc,
            m_stFwUpgradeProcess.u32RcvCodeCRC,
            m_stFwUpgradeProcess.u32FlashSavedCodeCRC);

    if ((m_stFwUpgradeFileVer.u32BinCodeCrc == m_stFwUpgradeProcess.u32RcvCodeCRC) \
        && (m_stFwUpgradeFileVer.u32BinCodeCrc == m_stFwUpgradeProcess.u32FlashSavedCodeCRC))
    {
        // Calculate code info crc.
        u32CodeLen = sizeof(stFwUpgradeCodeInfo_t) - sizeof(m_stCodeInfo.u32StructCrc);
        u32CRC = rom_get_crc32((uint8_t *)&m_stCodeInfo, u32CodeLen, CRC32_DEFAULT);
        m_stCodeInfo.u32StructCrc = u32CRC;

        // Write code info to flash.
        fw_upgrade_flash_erase(FLASH_FW_UPGRADE_CODE_INFO_ADDR_OFFSET, sizeof(stFwUpgradeCodeInfo_t));
        fw_upgrade_flash_write((uint8_t *)&m_stCodeInfo, FLASH_FW_UPGRADE_CODE_INFO_ADDR_OFFSET,
                            sizeof(stFwUpgradeCodeInfo_t));

        // Next step: SoC Reset.
        u8Ret = ERR_STA_OK;
        m_u8UpgradeStepFlag = FW_UPGRADE_STEP_RESET_SOC;
        fw_upgrade_process_response(pstProtocol->u8Cmd, &u8Ret, sizeof(u8Ret));
    }
    else
    {
        stFwUpgradeSendCodeState_t ststate;
        ststate.u8Ret = u8Ret;
        ststate.u32RcvCodeCrc = m_stFwUpgradeProcess.u32RcvCodeCRC;
        ststate.u32FlashCodeCrc = m_stFwUpgradeProcess.u32FlashSavedCodeCRC;

        // Calculate CRC16 and send response.
        fw_upgrade_process_response(pstProtocol->u8Cmd, (uint8_t *)&ststate, sizeof(ststate));
        return ERR_STA_ERROR;
    }

    return ERR_STA_OK;
}

/**
 * @brief  Process firmware upgrade system reset command.
 * @param  pstProtocol: Pointer to received command data.
 * @return status: @ref EN_ERR_STA_T.
 */
static uint8_t fw_upgrade_process_soc_reset(stFwUpgradeProtocol_t *pstProtocol)
{
    stFwUpgradeResetState_t stRstState =
    {
        .u8Ret = ERR_STA_OK,
        .u16ResetTime = 50,
    };

    PRINTF("Prepare to system reset.\n");

    if (0 != pstProtocol->u16DataLen)
    {
        PRINTF("[%d]Parameter Error: %d\n", __LINE__, pstProtocol->u16DataLen);

        stRstState.u8Ret = ERR_PARA_ERR;
        fw_upgrade_process_response(pstProtocol->u8Cmd, &stRstState.u8Ret, sizeof(stRstState.u8Ret));
        return stRstState.u8Ret;
    }

    // System reset.
    fw_upgrade_process_response(pstProtocol->u8Cmd, (uint8_t *)&stRstState, sizeof(stRstState));
    rom_delay_ms(stRstState.u16ResetTime);
    rom_hw_crg_system_reset();

    return stRstState.u8Ret;
}

/**
 * @brief  Firmware upgrade process.
 * @param  pu8Buf: Pointer to buffer.
 * @param  u16Len: Buffer length.
 * @return status: @ref EN_ERR_STA_T.
 */
uint8_t fw_upgrade_process(uint8_t *pu8Buf, uint16_t u16Len)
{
    uint8_t u8Ret;
    stFwUpgradeProtocol_t stProtocol;

    // Unpack firmware upgrade protocol.
    u8Ret = fw_upgrade_process_unpack(pu8Buf, u16Len, &stProtocol);
    if (ERR_STA_OK != u8Ret)
    {
        PRINTF("fw upgrade unpack error.\n");
        return ERR_STA_ERROR;
    }

    switch (stProtocol.u8Cmd)
    {
        case FW_UPGRADE_CMD_GET_FW_VERSION:
        {
            u8Ret = fw_upgrade_process_fw_version(&stProtocol);
            break;
        }
        case FW_UPGRADE_CMD_HANDSHAKE:
        {
            if (FW_UPGRADE_STEP_HANDSHAKE == m_u8UpgradeStepFlag)
            {
                u8Ret = fw_upgrade_process_handshake(&stProtocol);
            }
            break;
        }
        case FW_UPGRADE_CMD_UPGRADE_REQ:
        {
            if (FW_UPGRADE_STEP_UPGRADE_REQ == m_u8UpgradeStepFlag)
            {
                u8Ret = fw_upgrade_process_upgrade_request(&stProtocol);
            }
            break;
        }
        case FW_UPGRADE_CMD_SEND_CODE:
        {
            if (FW_UPGRADE_STEP_SEND_CODE == m_u8UpgradeStepFlag)
            {
                u8Ret = fw_upgrade_process_send_fw(&stProtocol);
            }
            break;
        }
        case FW_UPGRADE_CMD_SEND_CODE_DONE:
        {
            if (FW_UPGRADE_STEP_SEND_CODE_DONE == m_u8UpgradeStepFlag)
            {
                u8Ret = fw_upgrade_process_send_fw_done(&stProtocol);
            }
            break;
        }
        case FW_UPGRADE_CMD_RESET_SOC:
        {
            if (FW_UPGRADE_STEP_RESET_SOC == m_u8UpgradeStepFlag)
            {
                fw_upgrade_process_soc_reset(&stProtocol);
            }
            break;
        }
        default: break;
    }

    return u8Ret;
}

