/**********************************************************************************************************************
 * @file     hw_flash.c
 * @author   Maxscend SoC Software Team
 * @version  V1.1
 * @date     2022/01/13
 * @history
 * @note
 **********************************************************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2021, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 *********************************************************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "RegMXD2670.h"
#include "err_def.h"
#include "sflash.h"

#include "type_def.h"
#include "utility.h"

#include "hw_flash.h"


//=====================================================================================================================
// DEFINE
//=====================================================================================================================
#define FLASH_CMD_TIMEOUT_US (5000) // unit us, qspi clock > 2KHz
#define FLASH_TW_TIMEOUT_US (15000) // unit us, datasheet is tW = 12ms


//=====================================================================================================================
// LOCAL VARIABLES
//=====================================================================================================================
static FLASH_SEND_CMD_CB_FUNC rom_hw_flash_send_cmd_cb = NULL;




/**
 * @brief  Init Sflash Controller.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  u8DlyCfg: Flash delay config.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_init(stQSPI_Handle_t *pstFlash, uint8_t u8DlyCfg)
{
    if (!IS_VALID_FLASH_HANDLE(pstFlash))
    {
        return ERR_STA_ERROR;
    }

    pstFlash->QSPI_AUTOM_RSP_CFG = 0;
    pstFlash->QSPI_DELAY_CFG = (u8DlyCfg & QSPI_INPUT_DELAY_MSK) << QSPI_INPUT_DELAY_SHIFT;

    // Cache can not read flash with suspend
    pstFlash->QSPI_FLASH_BUSY &= ~((ENABLE & QSPI_FLASH_BUSY_MSK) << QSPI_FLASH_BUSY_SHIFT);

    return ERR_STA_OK;
}

/**
 * @brief  Set flash encrypt and decrypt key.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  u32Key: Flash encrypt and decrypt key, 32bit.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_set_encrypt_key(stQSPI_Handle_t *pstFlash, uint32_t u32Key)
{
    if (!IS_VALID_FLASH_HANDLE(pstFlash))
    {
        return ERR_STA_ERROR;
    }

    pstFlash->QSPI_SCRAMBLE_KEY = (u32Key & QSPI_SCRAMBLE_KEY_MSK) << QSPI_SCRAMBLE_KEY_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  Enable flash encrypt function.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_enable_encrypt(stQSPI_Handle_t *pstFlash)
{
    if (!IS_VALID_FLASH_HANDLE(pstFlash))
    {
        return ERR_STA_ERROR;
    }

    pstFlash->QSPI_SCRAMBLE_EN = (ENABLE & QSPI_SCRAMBLE_EN_MSK) << QSPI_SCRAMBLE_EN_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  Disable flash encrypt function.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_disable_encrypt(stQSPI_Handle_t *pstFlash)
{
    if (!IS_VALID_FLASH_HANDLE(pstFlash))
    {
        return ERR_STA_ERROR;
    }

    pstFlash->QSPI_SCRAMBLE_EN &= ~((ENABLE & QSPI_SCRAMBLE_EN_MSK) << QSPI_SCRAMBLE_EN_SHIFT);

    return ERR_STA_OK;
}

/**
 * @brief  Send a flash command.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  u32CmdCfg: Pointer to command config.
 * @param  u32LenCfg: Pointer to length config.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_send_command(stQSPI_Handle_t *pstFlash, uint32_t u32CmdCfg, uint32_t u32LenCfg)
{
    uint32_t u32Val = 0;
    uint32_t u32Timeout = 0;
    bool bIsNeedWaitFlg = false;

    if (!IS_VALID_FLASH_HANDLE(pstFlash))
    {
        return ERR_STA_ERROR;
    }

    if (rom_hw_flash_send_cmd_cb != NULL)
    {
        return rom_hw_flash_send_cmd_cb(pstFlash, u32CmdCfg, u32LenCfg);
    }

    /* 1. Check ACK flag to make sure the last command has finished or no other command are working */
    u32Timeout = 0;
    while (pstFlash->QSPI_MEM_STATUS & MEM_IN_BUSY_STATUS)
    {
        u32Timeout++;
        if (FLASH_CMD_TIMEOUT_US <= u32Timeout)
        {
            return ERR_STA_BUSY;
        }
        rom_delay_us(1);
    }

    /* 2. Clear the RX FIFO */
    u32Timeout = 0;
    while (pstFlash->QSPI_INT_FLAG & QSPI_INT_RXFIFO_NOT_EMPTY)
    {
        u32Val = pstFlash->QSPI_RX_FIFO;

        pstFlash->QSPI_INT_CLR = QSPI_INT_RXFIFO_NOT_EMPTY;

        u32Timeout++;
        if (FLASH_CMD_TIMEOUT_US <= u32Timeout)
        {
            return ERR_STA_ERROR;
        }
        rom_delay_us(1);
    };

    /* 3. Clear the TX FIFO */
    u32Timeout = 0;
    while (!(pstFlash->QSPI_INT_FLAG & QSPI_INT_TXFIFO_EMPTY))
    {
        u32Timeout++;
        if (FLASH_CMD_TIMEOUT_US <= u32Timeout)
        {
            return ERR_STA_ERROR;
        }
        rom_delay_us(1);
    }
    pstFlash->QSPI_INT_CLR = QSPI_INT_TXFIFO_EMPTY;

    /* 4. Configure the current command. */
    pstFlash->QSPI_MEM_CMD = u32CmdCfg & BYTE_INVALIDE;

    /* If size of flash more than 16MBytes, QSPI_MEM_TX_CFG[11] need set to 1. need test. */
    pstFlash->QSPI_MEM_TX_CFG = (u32CmdCfg >> HALF_WORD_BITS) & QSPI_MEM_TX_CFG_MSK;
    pstFlash->QSPI_MEM_DATA_LEN = (u32LenCfg >> HALF_WORD_BITS) & HALF_WORD_INVALIDE;
    pstFlash->QSPI_MEM_LEN_CFG = (((u32LenCfg & QSPI_MEM_ADDR_LEN_MSK) << QSPI_MEM_ADDR_LEN_SHIFT) |
                                  (((u32LenCfg >> BYTE_BITS) & QSPI_MEM_CMD_LEN_MSK) << QSPI_MEM_CMD_LEN_SHIFT));

    /* Start transmit command/address/data. */
    pstFlash->QSPI_FLASH_TX_EN = QSPI_FLASH_TX_EN_MSK << QSPI_FLASH_TX_EN_SHIFT;

    rom_delay_cycles(24); /* should wait 3 AHB clock */

    switch (u32CmdCfg & BYTE_INVALIDE)
    {
        case SFLASH_CMD_READ:
        case SFLASH_CMD_DREAD:
        case SFLASH_CMD_QREAD:
        case SFLASH_CMD_PP:
        case SFLASH_CMD_DPP:
        case SFLASH_CMD_QPP:
        case SFLASH_CMD_RUID:
        case SFLASH_CMD_DP:
        {
            break;
        }
        default:
        {
            bIsNeedWaitFlg = true;
            break;
        }
    }

    /* 5. Waiting current command finished. */
    if (bIsNeedWaitFlg)
    {
        u32Timeout = 0;
        while (pstFlash->QSPI_MEM_STATUS & MEM_IN_BUSY_STATUS)
        {
            u32Timeout++;
            if (FLASH_CMD_TIMEOUT_US <= u32Timeout)
            {
                return ERR_STA_TIMEOUT;
            }
            rom_delay_us(1);
        }
    }

    return ERR_STA_OK;
}

/**
 * @brief  Software Reset.
 *         After send comand RST(0x99), need wait 30us at least.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_reset(stQSPI_Handle_t *pstFlash)
{
    EN_ERR_STA_T enRet;

    enRet = rom_hw_flash_send_command(pstFlash, SFLASH_RSTEN_CMD_CFG, SFLASH_RSTEN_LEN_CFG);
    ERR_RETURN_IF_ERROR(enRet);

    enRet = rom_hw_flash_send_command(pstFlash, SFLASH_RST_CMD_CFG, SFLASH_RST_LEN_CFG);
    ERR_RETURN_IF_ERROR(enRet);

    rom_delay_us(35);

    return ERR_STA_OK;
}

/**
 * @brief  sFlash controller read sip flash status[15:0].
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  pu16Status: Status[15:0].
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_read_status(stQSPI_Handle_t *pstFlash, uint16_t *pu16Status)
{
    uint32_t u32Timeout = 0;

    uint16_t u16StaTmp0 = 0;
    uint16_t u16StaTmp1 = 0;
    uint16_t u16StausTmp = 0;
    EN_ERR_STA_T enRet;

    if ((!IS_VALID_FLASH_HANDLE(pstFlash)) || (NULL == pu16Status))
    {
        return ERR_STA_ERROR;
    }

    /* 1. Clear the RX FIFO */
    u32Timeout = 0;
    while (pstFlash->QSPI_INT_FLAG & QSPI_INT_RXFIFO_NOT_EMPTY)
    {
        u16StaTmp0 = pstFlash->QSPI_RX_FIFO;
        pstFlash->QSPI_INT_CLR = QSPI_INT_RXFIFO_NOT_EMPTY;

        u32Timeout++;
        if (FLASH_CMD_TIMEOUT_US <= u32Timeout)
        {
            return ERR_STA_BUSY;
        }
        rom_delay_us(1);
    }

    /* 2. Read BIT7~0 status */
    do
    {
        /* Read SR BIT7~0 */
        enRet = rom_hw_flash_send_command(pstFlash, SFLASH_RDSR_CMD_CFG, SFLASH_RDSR_LEN_CFG);
        ERR_RETURN_IF_ERROR(enRet);

        /* Sflash controller ACK bit indicate the whole SPI transmite finished. */
        u32Timeout = 0;
        pstFlash->QSPI_INT_CLR = QSPI_INT_RXFIFO_NOT_EMPTY;
        while (!(pstFlash->QSPI_INT_FLAG & QSPI_INT_RXFIFO_NOT_EMPTY))
        {
            u32Timeout++;
            if (FLASH_CMD_TIMEOUT_US <= u32Timeout)
            {
                return ERR_STA_TIMEOUT;
            }
            rom_delay_us(1);
        };

        u16StaTmp0 = (pstFlash->QSPI_RX_FIFO & BYTE_MSK);
        u16StaTmp1 = ((pstFlash->QSPI_RX_FIFO >> BYTE_BITS) & BYTE_MSK); //???
        if (u16StaTmp0 == u16StaTmp1)
        {
            break;
        }

    } while (1);

    *pu16Status = u16StaTmp0;

    /* 3. Read BIT15~8 status */
    do
    {
        /* read SR BIT15-8 */
        enRet = rom_hw_flash_send_command(pstFlash, SFLASH_HRDSR_CMD_CFG, SFLASH_HRDSR_LEN_CFG);
        ERR_RETURN_IF_ERROR(enRet);

        /* Sflash controller ACK bit indicate the whole SPI transmite finished. */
        u32Timeout = 0;
        pstFlash->QSPI_INT_CLR = QSPI_INT_RXFIFO_NOT_EMPTY;
        while (!(pstFlash->QSPI_INT_FLAG & QSPI_INT_RXFIFO_NOT_EMPTY))
        {
            u32Timeout++;
            if (FLASH_CMD_TIMEOUT_US <= u32Timeout)
            {
                return ERR_STA_TIMEOUT;
            }
            rom_delay_us(1);
        };

        u16StaTmp0 = (pstFlash->QSPI_RX_FIFO & BYTE_MSK);
        u16StaTmp1 = ((pstFlash->QSPI_RX_FIFO >> BYTE_BITS) & BYTE_MSK);
        if (u16StaTmp0 == u16StaTmp1)
        {
            break;
        }

    } while (1);

    *pu16Status |= (u16StaTmp0 & BYTE_MSK) << BYTE_BITS;
    u16StausTmp = *pu16Status;

    return ERR_STA_OK;
}

/**
 * @brief  Flash controller write sip flash status[15:0].
 *         The Write Status Register (WRSR) command allows new values to be written to the Status Register. Before it
 *         can be accepted, a Write Enable (WREN) command must previously have been executed. The WRSR command has no
 *         effect on S15, S10, S1 and S0 of the Status Register.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  u16Status: new values to be written to the Status Register[15:0].
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_write_status(stQSPI_Handle_t *pstFlash, uint16_t u16Status)
{
    uint32_t u32Timeout = 0;
    EN_ERR_STA_T enRet;

    if ((!IS_VALID_FLASH_HANDLE(pstFlash)))
    {
        return ERR_STA_ERROR;
    }

    /* 1. Send Enable Write Register Command */
    enRet = rom_hw_flash_send_command(pstFlash, SFLASH_WREN_CMD_CFG, SFLASH_WREN_LEN_CFG);
    ERR_RETURN_IF_ERROR(enRet);

    /* 2. Send the values to be written to the SR */
    u32Timeout = 0;
    while (!(pstFlash->QSPI_INT_FLAG & QSPI_INT_TXFIFO_EMPTY))
    {
        u32Timeout++;
        if (FLASH_CMD_TIMEOUT_US <= u32Timeout)
        {
            return ERR_STA_ERROR;
        }
        rom_delay_us(1);
    }
    pstFlash->QSPI_INT_CLR = QSPI_INT_TXFIFO_EMPTY;

    pstFlash->QSPI_TX_FIFO = u16Status;

    /* 3. Update the Status Register as WRITE FIFO */
    enRet = rom_hw_flash_send_command(pstFlash, SFLASH_WRSR_CMD_CFG, SFLASH_WRSR_LEN_CFG);
    ERR_RETURN_IF_ERROR(enRet);

    enRet = rom_hw_flash_wait_idle(pstFlash, FLASH_TW_TIMEOUT_US);
    ERR_RETURN_IF_ERROR(enRet);

    return ERR_STA_OK;
}

/**
 * @brief  sFlash controller read sip flash status[15:8].
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  pu16Status: Status[15:8].
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_read_status_high(stQSPI_Handle_t *pstFlash, uint8_t *pu8Status)
{
    uint32_t u32Timeout = 0;
    uint8_t u8StaTmp0 = 0;
    uint8_t u8StaTmp1 = 0;

    EN_ERR_STA_T enRet;

    if ((!IS_VALID_FLASH_HANDLE(pstFlash)) || (NULL == pu8Status))
    {
        return ERR_STA_ERROR;
    }

    /* 1. Clear the RX FIFO */
    u32Timeout = 0;
    while (pstFlash->QSPI_INT_FLAG & QSPI_INT_RXFIFO_NOT_EMPTY)
    {
        u8StaTmp0 = pstFlash->QSPI_RX_FIFO;
        pstFlash->QSPI_INT_CLR = QSPI_INT_RXFIFO_NOT_EMPTY;

        u32Timeout++;
        if (FLASH_CMD_TIMEOUT_US <= u32Timeout)
        {
            return ERR_STA_BUSY;
        }
        rom_delay_us(1);
    }

    /* 2. Read BIT15~8 status */
    do
    {
        /* read SR BIT15-8 */
        enRet = rom_hw_flash_send_command(pstFlash, SFLASH_HRDSR_CMD_CFG, SFLASH_HRDSR_LEN_CFG);
        ERR_RETURN_IF_ERROR(enRet);

        /* Sflash controller ACK bit indicate the whole SPI transmite finished. */
        u32Timeout = 0;
        pstFlash->QSPI_INT_CLR = QSPI_INT_RXFIFO_NOT_EMPTY;
        while (!(pstFlash->QSPI_INT_FLAG & QSPI_INT_RXFIFO_NOT_EMPTY))
        {
            u32Timeout++;
            if (FLASH_CMD_TIMEOUT_US <= u32Timeout)
            {
                return ERR_STA_TIMEOUT;
            }
            rom_delay_us(1);
        };

        u8StaTmp0 = (pstFlash->QSPI_RX_FIFO & BYTE_MSK); //???
        u8StaTmp1 = ((pstFlash->QSPI_RX_FIFO >> BYTE_BITS) & BYTE_MSK);
        if (u8StaTmp0 == u8StaTmp1)
        {
            break;
        }

    } while (1);

    *pu8Status = u8StaTmp0;

    return ERR_STA_OK;
}

/**
 * @brief  sFlash controller read sip flash status[7:0].
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  pu16Status: Status[15:8].
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_read_status_low(stQSPI_Handle_t *pstFlash, uint8_t *pu8Status)
{
    uint32_t u32Timeout = 0;
    uint8_t u8StaTmp0 = 0;
    uint8_t u8StaTmp1 = 0;

    EN_ERR_STA_T enRet;

    if ((!IS_VALID_FLASH_HANDLE(pstFlash)) || (NULL == pu8Status))
    {
        return ERR_STA_ERROR;
    }

    /* 1. Clear the RX FIFO */
    u32Timeout = 0;
    while (pstFlash->QSPI_INT_FLAG & QSPI_INT_RXFIFO_NOT_EMPTY)
    {
        u8StaTmp0 = pstFlash->QSPI_RX_FIFO;
        pstFlash->QSPI_INT_CLR = QSPI_INT_RXFIFO_NOT_EMPTY;

        u32Timeout++;
        if (FLASH_CMD_TIMEOUT_US <= u32Timeout)
        {
            return ERR_STA_BUSY;
        }
        rom_delay_us(1);
    }

    /* 2. Read BIT7~0 status */
    do
    {
        /* read SR BIT7-0 */
        enRet = rom_hw_flash_send_command(pstFlash, SFLASH_RDSR_CMD_CFG, SFLASH_RDSR_LEN_CFG);
        ERR_RETURN_IF_ERROR(enRet);

        /* Sflash controller ACK bit indicate the whole SPI transmite finished. */
        u32Timeout = 0;
        pstFlash->QSPI_INT_CLR = QSPI_INT_RXFIFO_NOT_EMPTY;
        while (!(pstFlash->QSPI_INT_FLAG & QSPI_INT_RXFIFO_NOT_EMPTY))
        {
            u32Timeout++;
            if (FLASH_CMD_TIMEOUT_US <= u32Timeout)
            {
                return ERR_STA_TIMEOUT;
            }
            rom_delay_us(1);
        };

        u8StaTmp0 = (pstFlash->QSPI_RX_FIFO & BYTE_MSK);
        u8StaTmp1 = ((pstFlash->QSPI_RX_FIFO >> BYTE_BITS) & BYTE_MSK);
        if (u8StaTmp0 == u8StaTmp1)
        {
            break;
        }

    } while (1);

    *pu8Status = u8StaTmp0;

    return ERR_STA_OK;
}

/**
 * @brief  sFlash wait until the flash operation is finished.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  u32TimeoutUs: Timeout of WIP. The Write in Progress (WIP) bit indicates whether the memory is busy in
 *                       program/erase/write status register progress.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_wait_idle(stQSPI_Handle_t *pstFlash, uint32_t u32TimeoutUs)
{
    uint32_t u32Timeout = 0;
    uint16_t u16Status = 0;

    while (u32Timeout < u32TimeoutUs)
    {
        u32Timeout++;
        if (ERR_STA_OK == rom_hw_flash_read_status(pstFlash, &u16Status))
        {
            if (!(u16Status & SFLASH_STATUS_WIP))
            {
                return ERR_STA_OK;
            }
        }
        rom_delay_us(1);
    }

    return ERR_STA_TIMEOUT;
}

/**
 * @brief  sFlash release flash from read enhanced mode.
 *         This function release from read enhanced mode.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  enBusMode: Access mode as below, @ref EN_FLASH_BUS_MODE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_release_read_enhanced(stQSPI_Handle_t *pstFlash, EN_FLASH_BUS_MODE_T enBusMode)
{
    EN_ERR_STA_T enRet;

    if ((!IS_VALID_FLASH_HANDLE(pstFlash)) || (!IS_VALID_FLASH_BUS_MODE(enBusMode)))
    {
        return ERR_STA_ERROR;
    }

    if (pstFlash->QSPI_FLASH_CONTINUE_STATUS & FLASH_IN_CONTINUE_READ_MODE)
    {
        if (BUS_MODE_DPI == enBusMode)
        {
            pstFlash->QSPI_TX_FIFO = BYTE_INVALIDE;
        }

        enRet = rom_hw_flash_send_command(pstFlash, SFLASH_CRMR_CMD_CFG, SFLASH_CRMR_LEN_CFG);
        ERR_RETURN_IF_ERROR(enRet);
    }

    return ERR_STA_OK;
}

/**
 * @brief  Set SIP flash quad access mode.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  enEn: Enable or Disable Quad mode @ref EN_QSPI_ENABLE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_set_quad_enable(stQSPI_Handle_t *pstFlash, EN_QSPI_ENABLE_T enEn)
{
    uint16_t u16Status = 0;
    EN_ERR_STA_T enRet;

    if ((!IS_VALID_FLASH_HANDLE(pstFlash)) || (!IS_VALID_QSPI_ENABLE(enEn)))
    {
        return ERR_STA_ERROR;
    }

    /* 1. Read current flash status register */
    enRet = rom_hw_flash_read_status(pstFlash, &u16Status);
    ERR_RETURN_IF_ERROR(enRet);

    /* 2. Send Enable Write Register Command */
    enRet = rom_hw_flash_send_command(pstFlash, SFLASH_WREN_CMD_CFG, SFLASH_WREN_LEN_CFG);
    ERR_RETURN_IF_ERROR(enRet);

    /* 3. Clear QE( Quad Enable ) bit to disable Quad Access. */
    u16Status &= ~(SFLASH_STATUS_QE_MSK << SFLASH_STATUS_QE_SHIFT);
    u16Status |= (enEn & SFLASH_STATUS_QE_MSK) << SFLASH_STATUS_QE_SHIFT;
    pstFlash->QSPI_TX_FIFO = u16Status;

    /* 4. Update the Status Register as WRITE FIFO */
    enRet = rom_hw_flash_send_command(pstFlash, SFLASH_WRSR_CMD_CFG, SFLASH_WRSR_LEN_CFG);
    ERR_RETURN_IF_ERROR(enRet);

    enRet = rom_hw_flash_wait_idle(pstFlash, FLASH_TW_TIMEOUT_US);
    ERR_RETURN_IF_ERROR(enRet);

    enRet = rom_hw_flash_read_status(pstFlash, &u16Status);
    ERR_RETURN_IF_ERROR(enRet);

    if (QSPI_ENABLE == enEn)
    {
        if (!(u16Status & (QSPI_ENABLE << SFLASH_STATUS_QE_SHIFT)))
        {
            return ERR_STA_ERROR;
        }
    }
    else
    {
        if ((u16Status & (QSPI_ENABLE << SFLASH_STATUS_QE_SHIFT)))
        {
            return ERR_STA_ERROR;
        }
    }

    return ERR_STA_OK;
}

/**
 * @brief  Set SIP Flash exit abnormal state.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_exit_abnormal_state(stQSPI_Handle_t *pstFlash)
{
    EN_ERR_STA_T enRet;

    if (NULL == pstFlash)
    {
        return ERR_STA_ERROR;
    }

    enRet = rom_hw_flash_release_deep_power_down(pstFlash);
    ERR_RETURN_IF_ERROR(enRet);

    enRet = rom_hw_flash_release_read_enhanced(pstFlash, BUS_MODE_SPI);
    ERR_RETURN_IF_ERROR(enRet);

    enRet = rom_hw_flash_set_quad_enable(pstFlash, QSPI_DISABLE);
    ERR_RETURN_IF_ERROR(enRet);

    return rom_hw_flash_reset(pstFlash);
}

/**
 * @brief  Get SIP Flash Mmemory ID by SFlash Controller.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  pu32ID: Pointer to a 32-bit to save the memory id.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_get_id(stQSPI_Handle_t *pstFlash, uint32_t *pu32ID)
{
    uint32_t u32Timeout = 0;
    EN_ERR_STA_T enRet;

    if ((!IS_VALID_FLASH_HANDLE(pstFlash)) || (NULL == pu32ID))
    {
        return ERR_STA_ERROR;
    }

    enRet = rom_hw_flash_send_command(pstFlash, SFLASH_RDID_CMD_CFG, SFLASH_RDID_LEN_CFG);
    ERR_RETURN_IF_ERROR(enRet);

    pstFlash->QSPI_INT_CLR = QSPI_INT_RXFIFO_NOT_EMPTY;
    while (!(pstFlash->QSPI_INT_FLAG & QSPI_INT_RXFIFO_NOT_EMPTY))
    {
        u32Timeout++;
        if (FLASH_CMD_TIMEOUT_US <= u32Timeout)
        {
            return ERR_STA_TIMEOUT;
        }
        rom_delay_us(1);
    };

    *pu32ID = pstFlash->QSPI_RX_FIFO;

    return ERR_STA_OK;
}

/**
 * @brief  Get SIP Flash Mmemory Unique ID by SFlash Controller.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  pu8ID: Pointer to a 128-bit to save the unique id.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_get_unique_id(stQSPI_Handle_t *pstFlash, uint8_t *pu8ID)
{
    uint16_t i = 0;
    uint8_t u8Cnt = 0;
    uint32_t u32Data = 0;
    uint32_t u32Timeout = 0;
    uint16_t u16Length = UINT128_BYTES;

    EN_ERR_STA_T enRet;

    if ((!IS_VALID_FLASH_HANDLE(pstFlash)) || (NULL == pu8ID))
    {
        return ERR_STA_ERROR;
    }

    enRet = rom_hw_flash_send_command(pstFlash, SFLASH_RUID_CMD_CFG, SFLASH_RUID_LEN_CFG);
    ERR_RETURN_IF_ERROR(enRet);

    while (u16Length)
    {
        if (pstFlash->QSPI_INT_FLAG & QSPI_INT_RXFIFO_NOT_EMPTY)
        {
            u32Timeout = 0;
            u32Data = pstFlash->QSPI_RX_FIFO;

            u8Cnt = u16Length < UINT32_BYTES ? u16Length : UINT32_BYTES;

            for (i = 0; i < u8Cnt; i++)
            {
                *pu8ID++ = (u32Data >> (i * BYTE_BITS));
                u16Length--;
            }

            pstFlash->QSPI_INT_CLR = QSPI_INT_RXFIFO_NOT_EMPTY;
        }

        u32Timeout++;
        if (FLASH_CMD_TIMEOUT_US <= u32Timeout)
        {
            return ERR_STA_TIMEOUT;
        }
        rom_delay_us(1);
    }

    return rom_hw_flash_wait_idle(pstFlash, FLASH_CMD_TIMEOUT_US);
}

/**
 * @brief  Get into deep power down work mode.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  enBusMode: Access mode as below, @ref EN_FLASH_BUS_MODE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_enter_deep_power_down(stQSPI_Handle_t *pstFlash, EN_FLASH_BUS_MODE_T enBusMode)
{
    EN_ERR_STA_T enRet;

    enRet = rom_hw_flash_release_read_enhanced(pstFlash, enBusMode);
    ERR_RETURN_IF_ERROR(enRet);

    enRet = rom_hw_flash_send_command(pstFlash, SFLASH_DP_CMD_CFG, SFLASH_DP_LEN_CFG);
    ERR_RETURN_IF_ERROR(enRet);

    rom_delay_us(3);

    return ERR_STA_OK;
}

/**
 * @brief  Release from DEEP POWER DOWN state.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_release_deep_power_down(stQSPI_Handle_t *pstFlash)
{
    EN_ERR_STA_T enRet;

    enRet = rom_hw_flash_send_command(pstFlash, SFLASH_RDP_CMD_CFG, SFLASH_RDP_LEN_CFG);
    ERR_RETURN_IF_ERROR(enRet);

    rom_delay_us(8);

    return ERR_STA_OK;
}

/**
 * @brief  Set SIP FLASH MEMORY block Protect.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  u16BlockCfg: Block protect area configuration.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_set_block_protect(stQSPI_Handle_t *pstFlash, uint16_t u16BlockCfg)
{
    uint16_t u16Status = 0;
    EN_ERR_STA_T enRet;

    if ((!IS_VALID_FLASH_HANDLE(pstFlash)))
    {
        return ERR_STA_ERROR;
    }

    enRet = rom_hw_flash_read_status(pstFlash, &u16Status);
    ERR_RETURN_IF_ERROR(enRet);

    u16Status &= ~(SFLASH_STATUS_BP_MSK << SFLASH_STATUS_BP_SHIFT);
    u16Status |= u16BlockCfg;

    enRet = rom_hw_flash_send_command(pstFlash, SFLASH_WREN_CMD_CFG, SFLASH_WREN_LEN_CFG);
    ERR_RETURN_IF_ERROR(enRet);

    pstFlash->QSPI_TX_FIFO = u16Status;

    enRet = rom_hw_flash_send_command(pstFlash, SFLASH_WRSR_CMD_CFG, SFLASH_WRSR_LEN_CFG);
    ERR_RETURN_IF_ERROR(enRet);

    return rom_hw_flash_wait_idle(pstFlash, FLASH_TW_TIMEOUT_US);
}

/**
 * @brief  Erase sip flash memory with indicated @ref EN_SFLASH_ERASE_SIZE_T number.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  enSize: The range of Erase, @ref EN_SFLASH_ERASE_SIZE_T.
 * @param  u16Num: Number of Erase.
 * @param  u32Timeoutus: Timeout of flash page/sector/block erase, unit is us.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_erase(stQSPI_Handle_t *pstFlash, EN_SFLASH_ERASE_SIZE_T enSize,
                                uint16_t u16Num, uint32_t u32Timeoutus)
{
    uint32_t u32CmdCfg = 0;
    uint32_t u32LenCfg = 0;

    EN_ERR_STA_T enRet;

    if ((!IS_VALID_FLASH_HANDLE(pstFlash)))
    {
        return ERR_STA_ERROR;
    }

    enRet = rom_hw_flash_release_read_enhanced(pstFlash, BUS_MODE_QPI);
    ERR_RETURN_IF_ERROR(enRet);

    enRet = rom_hw_flash_send_command(pstFlash, SFLASH_WREN_CMD_CFG, SFLASH_WREN_LEN_CFG);
    ERR_RETURN_IF_ERROR(enRet);

    switch (enSize)
    {
        case ERASE_PAGE:
        {
            u32CmdCfg = SFLASH_PE_CMD_CFG;
            u32LenCfg = SFLASH_PE_LEN_CFG;
            pstFlash->QSPI_MEM_ADDR = u16Num << SFLASH_PAGE_SHIFT_BITS;
            break;
        }
        case ERASE_SECTOR:
        {
            u32CmdCfg = SFLASH_SE_CMD_CFG;
            u32LenCfg = SFLASH_SE_LEN_CFG;
            pstFlash->QSPI_MEM_ADDR = u16Num << SFLASH_SECTOR_SHIFT_BITS;
            break;
        }
        case ERASE_BLOCK_32KB:
        {
            u32CmdCfg = SFLASH_BE32K_CMD_CFG;
            u32LenCfg = SFLASH_BE32K_LEN_CFG;
            pstFlash->QSPI_MEM_ADDR = u16Num << SFLASH_BLOCK_32KB_SHIFT_BITS;
            break;
        }
        case ERASE_BLOCK_64KB:
        {
            u32CmdCfg = SFLASH_BE_CMD_CFG;
            u32LenCfg = SFLASH_BE_LEN_CFG;
            pstFlash->QSPI_MEM_ADDR = u16Num << SFLASH_BLOCK_64KB_SHIFT_BITS;
            break;
        }
        default:
            return ERR_PARA_ERR;
    }

    enRet = rom_hw_flash_send_command(pstFlash, u32CmdCfg, u32LenCfg);
    ERR_RETURN_IF_ERROR(enRet);

    return rom_hw_flash_wait_idle(pstFlash, u32Timeoutus);
}

/**
 * @brief  Erase sip flash memory.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  u32Timeoutus: Timeout of flash chip erase, unit is us.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_chip_erase(stQSPI_Handle_t *pstFlash, uint32_t u32Timeoutus)
{
    EN_ERR_STA_T enRet;

    enRet = rom_hw_flash_release_read_enhanced(pstFlash, BUS_MODE_QPI);
    ERR_RETURN_IF_ERROR(enRet);

    enRet = rom_hw_flash_send_command(pstFlash, SFLASH_WREN_CMD_CFG, SFLASH_WREN_LEN_CFG);
    ERR_RETURN_IF_ERROR(enRet);

    enRet = rom_hw_flash_send_command(pstFlash, SFLASH_CE_CMD_CFG, SFLASH_CE_LEN_CFG);
    ERR_RETURN_IF_ERROR(enRet);

    return rom_hw_flash_wait_idle(pstFlash, u32Timeoutus);
}

/**
 * @brief  Write the buffer data to sip flash memory in one page from indicated start address with indicated access mode.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  u32Addr: Start Address of SIP FLASH MEMORY.
 * @param  pu8Buf: Pointer to a buffer which will be written to flash memory.
 * @param  u16Len: Length of write data.
 * @param  enMode: Access mode as below, @ref EN_FLASH_BUS_MODE_T.
 * @param  u16TimeoutUs: Timeout of flash page program, unit is us.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_write_bytes_in_page(stQSPI_Handle_t *pstFlash, uint32_t u32Addr, uint8_t *pu8Buf,
                                              uint16_t u16Len, EN_FLASH_BUS_MODE_T enMode, uint16_t u16TimeoutUs)
{
    uint16_t i = 0;
    uint8_t u8Cnt = 0;
    uint16_t u16ReLen = 0;
    uint32_t u32CmdCfg = 0;
    uint32_t u32LenCfg = 0;
    uint32_t u32Data = 0;
    uint32_t u32Timeout = 0;

    EN_ERR_STA_T enRet;

    if ((!IS_VALID_FLASH_HANDLE(pstFlash)) || (0 == u16Len) || (NULL == pu8Buf))
    {
        return ERR_STA_ERROR;
    }

    if (SFLASH_PAGE_BYTE_SIZE < (u32Addr & SFLASH_PAGE_MASK) + u16Len)
    {
        return ERR_STA_ERROR;
    }

    enRet = rom_hw_flash_send_command(pstFlash, SFLASH_WREN_CMD_CFG, SFLASH_WREN_LEN_CFG);
    ERR_RETURN_IF_ERROR(enRet);

    switch (enMode)
    {
        case BUS_MODE_SPI:
        {
            u32CmdCfg = SFLASH_PP_CMD_CFG;
            u32LenCfg = SFLASH_PP_LEN_CFG + (((u16Len - 1) & HALF_WORD_MSK) << HALF_WORD_BITS);
            break;
        }
        case BUS_MODE_DPI: /*P25Q40SU, P25Q16SU Not Support Dual Input Page Program (DPP) instruction */
        {
            u32CmdCfg = SFLASH_DPP_CMD_CFG;
            u32LenCfg = SFLASH_DPP_LEN_CFG + (((u16Len - 1) & HALF_WORD_MSK) << HALF_WORD_BITS);
            break;
        }
        case BUS_MODE_QPI:
        {
            u32CmdCfg = SFLASH_QPP_CMD_CFG;
            u32LenCfg = SFLASH_QPP_LEN_CFG + (((u16Len - 1) & HALF_WORD_MSK) << HALF_WORD_BITS);
            break;
        }
        default:
        {
            return ERR_STA_ERROR;
        }
    }

    u32Data = 0;
    u16ReLen = u16Len;

    u8Cnt = (u16ReLen < UINT32_BYTES) ? u16ReLen : UINT32_BYTES;
    for (i = 0; i < u8Cnt; i++)
    {
        u32Data |= ((*pu8Buf++) << (i * BYTE_BITS));
        u16ReLen--;
    }

    pstFlash->QSPI_MEM_ADDR = u32Addr;
    pstFlash->QSPI_TX_FIFO = u32Data;

    enRet = rom_hw_flash_send_command(pstFlash, u32CmdCfg, u32LenCfg);
    ERR_RETURN_IF_ERROR(enRet);

    u32Timeout = 0;
    while (u16ReLen)
    {
        pstFlash->QSPI_INT_CLR = QSPI_INT_TXFIFO_FULL;
        if (!(pstFlash->QSPI_INT_FLAG & QSPI_INT_TXFIFO_FULL))
        {
            u32Data = 0;
            u8Cnt = (u16ReLen < UINT32_BYTES) ? u16ReLen : UINT32_BYTES;
            for (i = 0; i < u8Cnt; i++)
            {
                u32Data |= ((*pu8Buf++) << (i * BYTE_BITS));
                u16ReLen--;
            }
            pstFlash->QSPI_TX_FIFO = u32Data;
        }

        u32Timeout++;
        if (FLASH_CMD_TIMEOUT_US <= u32Timeout)
        {
            return ERR_STA_TIMEOUT;
        }
        rom_delay_us(1);
    }

    /* 4. Watigng current command finished. */
    u32Timeout = 0;
    while (pstFlash->QSPI_MEM_STATUS & MEM_IN_BUSY_STATUS)
    {
        u32Timeout++;
        if (FLASH_CMD_TIMEOUT_US <= u32Timeout)
        {
            return ERR_STA_TIMEOUT;
        }
        rom_delay_us(1);
    }

    return rom_hw_flash_wait_idle(pstFlash, u16TimeoutUs);
}

/**
 * @brief  Write the buffer data to sip flash memory from indicated start address with indicated access mode
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  u32Addr: Start Address of SIP FLASH MEMORY.
 * @param  pu8Buf: Pointer to a buffer which will be written to flash memory.
 * @param  u32Len: Length of write data, maximum length is 64 * 1024 Bytes.
 * @param  enMode: Access mode as below, @ref EN_FLASH_BUS_MODE_T.
 * @param  u16TimeoutUs: Timeout of flash page program, unit is us.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_write_bytes(stQSPI_Handle_t *pstFlash, uint32_t u32Addr, uint8_t *pu8Buf,
                                      uint32_t u32Len, EN_FLASH_BUS_MODE_T enMode, uint16_t u16TimeoutUs)
{
    uint32_t u32SrcAddr = u32Addr;
    uint32_t u32Length = u32Len;
    uint16_t u16ReLen = 0;

    EN_ERR_STA_T enRet;

    if ((0 == u32Length) || (u32Length > FLASH_TRX_MAX_MSK) || (NULL == pu8Buf))
    {
        return ERR_STA_ERROR;
    }

    enRet = rom_hw_flash_release_read_enhanced(pstFlash, enMode);
    ERR_RETURN_IF_ERROR(enRet);

    while (u32Length)
    {
        u16ReLen = SFLASH_PAGE_BYTE_SIZE - (u32SrcAddr & SFLASH_PAGE_MASK);

        if (u16ReLen > u32Length)
        {
            u16ReLen = u32Length;
        }

        rom_hw_flash_write_bytes_in_page(pstFlash, u32SrcAddr, pu8Buf, u16ReLen, enMode, u16TimeoutUs);
        ERR_RETURN_IF_ERROR(enRet);

        u32Length -= u16ReLen;
        u32SrcAddr += u16ReLen;
        pu8Buf += u16ReLen;
    }

    return ERR_STA_OK;
}

#if 0
/**
 * @brief  Erase sip flash memory with indicated @ref EN_SFLASH_ERASE_SIZE_T number, can be suspend
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  enSize: The range of Erase, @ref EN_SFLASH_ERASE_SIZE_T
 * @param  u16Num: Number of enSize Erase.
 * @param  u32Timeoutus: Timeout of flash page/sector/block erase, unit is us.

 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_erase_suspendable ( stQSPI_Handle_t* pstFlash, EN_SFLASH_ERASE_SIZE_T enSize, 
                                              uint16_t u16Num, uint32_t u32Timeoutus )
{
    uint32_t u32CmdCfg = 0;
    uint32_t u32LenCfg = 0;

    EN_ERR_STA_T enRet;


    if ( ( !IS_VALID_FLASH_HANDLE ( pstFlash ) ) )
    {
        return ERR_STA_ERROR;
    }

    // For more security, before erase cmd send finished, should disable irq
    uint32_t u32Primask = __get_PRIMASK();
    __disable_irq();

    /* Configure suspend */
    pstFlash->QSPI_FLASH_SUSPEND_CTRL =
        ( ( 15 & QSPI_FLASH_SUS_UNIT_CFG_MSK ) << QSPI_FLASH_SUS_UNIT_CFG_SHIFT ) | \
        ( ( 20 & QSPI_FLASH_SUS_TERS_MSK ) << QSPI_FLASH_SUS_TERS_SHIFT )         | \
        ( ( 255 & QSPI_FLASH_SUS_TPEFOR_MSK ) << QSPI_FLASH_SUS_TPEFOR_SHIFT )    | \
        ( ( 30 & QSPI_FLASH_SUS_TPESL_MSK ) << QSPI_FLASH_SUS_TPESL_SHIFT );

    pstFlash->QSPI_MEM_CACHE_CMD1 =
        ( ( 0x0B & MEM_CACHE_READ_CMD_MSK ) << MEM_CACHE_READ_CMD_SHIFT )  | \
        ( ( 0x75 & MEM_CACHE_SUS_CMD_MSK ) << MEM_CACHE_SUS_CMD_SHIFT )    | \
        ( ( 0x7A & MEM_CACHE_RES_CMD_MSK ) << MEM_CACHE_RES_CMD_SHIFT );

    pstFlash->QSPI_FLASH_CHECK_BUSY_INTERVAL =
        ( ( 320 & CHECK_BUSY_PERIOD_MSK ) << CHECK_BUSY_PERIOD_SHIFT ) | \
        ( ( 3200 & CHECK_BUSY_INIT_PERIOD_MSK ) << CHECK_BUSY_INIT_PERIOD_SHIFT );

    pstFlash->QSPI_FLASH_CHECK_BUSY_CMD =
        ( ( 0x05 & CHECK_BUSY_CMD_MSK ) << CHECK_BUSY_CMD_SHIFT )      | \
        ( ( BUS_MODE_SPI & CHECK_BUSY_CMD_MODE_MSK ) << CHECK_BUSY_CMD_MODE_SHIFT );
    
    enRet = rom_hw_flash_release_read_enhanced ( pstFlash, BUS_MODE_QPI );
    ERR_RETURN_IF_ERROR ( enRet );

    enRet = rom_hw_flash_send_command ( pstFlash, SFLASH_WREN_CMD_CFG, SFLASH_WREN_LEN_CFG );
    ERR_RETURN_IF_ERROR ( enRet );

    /* Enable flash auto busy function, flash controller will send command of 0x05 automatically */
    FLASH->QSPI_FLASH_AUTO_BUSY_EN |= ( ENABLE & AUTO_BUSY_EN_MSK ) << AUTO_BUSY_EN_SHIFT;

    /* Enable flash suspend function, flash controller send command of 75h/0Bh/7Ah automatically */
    FLASH->QSPI_FLASH_SUSPEND_EN = ( ENABLE & SUSPEND_EN_MSK ) << SUSPEND_EN_SHIFT;

    switch ( enSize )
    {
        case ERASE_PAGE:
        {
            u32CmdCfg = SFLASH_PE_CMD_CFG;
            u32LenCfg = SFLASH_PE_LEN_CFG;
            pstFlash->QSPI_MEM_ADDR = u16Num << SFLASH_PAGE_SHIFT_BITS;
            break;
        }
        case ERASE_SECTOR:
        {
            u32CmdCfg = SFLASH_SE_CMD_CFG;
            u32LenCfg = SFLASH_SE_LEN_CFG;
            pstFlash->QSPI_MEM_ADDR = u16Num << SFLASH_SECTOR_SHIFT_BITS;
            break;
        }
        case ERASE_BLOCK_32KB:
        {
            u32CmdCfg = SFLASH_BE32K_CMD_CFG;
            u32LenCfg = SFLASH_BE32K_LEN_CFG;
            pstFlash->QSPI_MEM_ADDR = u16Num << SFLASH_BLOCK_32KB_SHIFT_BITS;
            break;
        }
        case ERASE_BLOCK_64KB:
        {
            u32CmdCfg = SFLASH_BE_CMD_CFG;
            u32LenCfg = SFLASH_BE_LEN_CFG;
            pstFlash->QSPI_MEM_ADDR = u16Num << SFLASH_BLOCK_64KB_SHIFT_BITS;
            break;
        }
        default:
            return ERR_PARA_ERR;
    }

    enRet = rom_hw_flash_send_command ( pstFlash, u32CmdCfg, u32LenCfg );
    ERR_RETURN_IF_ERROR ( enRet );

    // When erase cmd send finished, can be suspend in irq
    __set_PRIMASK ( u32Primask ); 

    /* When QSPI_FLASH_AUTO_BUSY_FLAG is 0, must disable AUTO_BUSY function */
    while ( ( FLASH->QSPI_FLASH_AUTO_BUSY_FLAG >> AUTO_BUSY_FLAG_SHIFT ) & AUTO_BUSY_FLAG_MSK ) {};

    FLASH->QSPI_FLASH_AUTO_BUSY_EN &= ~( ( ENABLE & AUTO_BUSY_EN_MSK ) << AUTO_BUSY_EN_SHIFT );

    enRet = rom_hw_flash_wait_idle ( pstFlash, u32Timeoutus );
    ERR_RETURN_IF_ERROR ( enRet );

    // Cache can not read flash with suspend
    pstFlash->QSPI_FLASH_BUSY &= ~ ( ( ENABLE & QSPI_FLASH_BUSY_MSK ) << QSPI_FLASH_BUSY_SHIFT );

    return ERR_STA_OK;
}

/**
 * @brief  Write the buffer data to sip flash memory in one page, can be suspended by CACHE read.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  u32Addr: Start Address of SIP FLASH MEMORY
 * @param  pu8Buf: Pointer to a buffer which will be written to flash memory
 * @param  u16Len: Length of write data
 * @param  enMode: Access mode as below, @ref EN_FLASH_BUS_MODE_T
 * @param  u16TimeoutUs: Timeout of flash page program, unit is us.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_write_bytes_in_page_suspendable ( stQSPI_Handle_t* pstFlash, uint32_t u32Addr, uint8_t* pu8Buf,
                                                            uint16_t u16Len, EN_FLASH_BUS_MODE_T enMode, uint16_t u16TimeoutUs )
{
    uint16_t i = 0;
    uint8_t  u8Cnt = 0;
    uint16_t u16ReLen  = 0;
    uint32_t u32CmdCfg = 0;
    uint32_t u32LenCfg = 0;
    uint32_t u32Data   = 0;
    uint32_t u32Timeout = 0;

    EN_ERR_STA_T enRet;


    if ( ( !IS_VALID_FLASH_HANDLE ( pstFlash ) ) || ( 0 == u16Len ) || ( NULL == pu8Buf ) )
    {
        return ERR_STA_ERROR;
    }

    if ( SFLASH_PAGE_BYTE_SIZE < ( u32Addr & SFLASH_PAGE_MASK ) + u16Len )
    {
        return ERR_STA_ERROR;
    }

    // For more security, before program cmd excute finished, should disable irq
    uint32_t u32Primask = __get_PRIMASK();
    __disable_irq();

    /* Configure suspend */
    pstFlash->QSPI_FLASH_SUSPEND_CTRL =
        ( ( 15 & QSPI_FLASH_SUS_UNIT_CFG_MSK ) << QSPI_FLASH_SUS_UNIT_CFG_SHIFT ) | \
        ( ( 20 & QSPI_FLASH_SUS_TERS_MSK ) << QSPI_FLASH_SUS_TERS_SHIFT )         | \
        ( ( 255 & QSPI_FLASH_SUS_TPEFOR_MSK ) << QSPI_FLASH_SUS_TPEFOR_SHIFT )    | \
        ( ( 30 & QSPI_FLASH_SUS_TPESL_MSK ) << QSPI_FLASH_SUS_TPESL_SHIFT );

    pstFlash->QSPI_MEM_CACHE_CMD1 =
        ( ( 0x0B & MEM_CACHE_READ_CMD_MSK ) << MEM_CACHE_READ_CMD_SHIFT )  | \
        ( ( 0x75 & MEM_CACHE_SUS_CMD_MSK ) << MEM_CACHE_SUS_CMD_SHIFT )    | \
        ( ( 0x7A & MEM_CACHE_RES_CMD_MSK ) << MEM_CACHE_RES_CMD_SHIFT );

    pstFlash->QSPI_FLASH_CHECK_BUSY_INTERVAL =
        ( ( 320 & CHECK_BUSY_PERIOD_MSK ) << CHECK_BUSY_PERIOD_SHIFT ) | \
        ( ( 3200 & CHECK_BUSY_INIT_PERIOD_MSK ) << CHECK_BUSY_INIT_PERIOD_SHIFT );

    pstFlash->QSPI_FLASH_CHECK_BUSY_CMD =
        ( ( 0x05 & CHECK_BUSY_CMD_MSK ) << CHECK_BUSY_CMD_SHIFT )      | \
        ( ( BUS_MODE_SPI & CHECK_BUSY_CMD_MODE_MSK ) << CHECK_BUSY_CMD_MODE_SHIFT );

    enRet = rom_hw_flash_send_command ( pstFlash, SFLASH_WREN_CMD_CFG, SFLASH_WREN_LEN_CFG );
    ERR_RETURN_IF_ERROR ( enRet );

    /* Enable flash auto busy function, flash controller will send command of 0x05 automatically */
    FLASH->QSPI_FLASH_AUTO_BUSY_EN |= ( ENABLE & AUTO_BUSY_EN_MSK ) << AUTO_BUSY_EN_SHIFT;

    /* Enable flash suspend function, flash controller send command of 75h/0Bh/7Ah automatically */
    FLASH->QSPI_FLASH_SUSPEND_EN = ( ENABLE & SUSPEND_EN_MSK ) << SUSPEND_EN_SHIFT;

    switch ( enMode )
    {
        case BUS_MODE_SPI:
        {
            u32CmdCfg = SFLASH_PP_CMD_CFG;
            u32LenCfg = SFLASH_PP_LEN_CFG + ( ( ( u16Len - 1 ) & HALF_WORD_MSK ) << HALF_WORD_BITS );
            break;
        }
        case BUS_MODE_DPI:/*P25Q40SU, P25Q16SU Not Support Dual Input Page Program (DPP) instruction */
        {
            u32CmdCfg = SFLASH_DPP_CMD_CFG;
            u32LenCfg = SFLASH_DPP_LEN_CFG + ( ((u16Len - 1) & HALF_WORD_MSK) << HALF_WORD_BITS );
            break;
        }
        case BUS_MODE_QPI:
        {
            u32CmdCfg = SFLASH_QPP_CMD_CFG;
            u32LenCfg = SFLASH_QPP_LEN_CFG + ( ( ( u16Len - 1 ) & HALF_WORD_MSK ) << HALF_WORD_BITS );
            break;
        }
        default:
            return ERR_STA_ERROR;
    }

    u32Data = 0;
    u16ReLen = u16Len;

    u8Cnt = ( u16ReLen < UINT32_BYTES ) ? u16ReLen : UINT32_BYTES;
    for ( i = 0; i < u8Cnt; i++ )
    {
        u32Data |= ( ( *pu8Buf++ ) << ( i * BYTE_BITS ) );
        u16ReLen--;
    }

    pstFlash->QSPI_MEM_ADDR = u32Addr;
    pstFlash->QSPI_TX_FIFO = u32Data;

    enRet = rom_hw_flash_send_command ( pstFlash, u32CmdCfg, u32LenCfg );
    ERR_RETURN_IF_ERROR ( enRet );


    u32Timeout = 0;
    while ( u16ReLen )
    {
        pstFlash->QSPI_INT_CLR = QSPI_INT_TXFIFO_FULL;
        if ( ! ( pstFlash->QSPI_INT_FLAG & QSPI_INT_TXFIFO_FULL ) )
        {
            u32Data = 0;
            u8Cnt   = ( u16ReLen < UINT32_BYTES ) ? u16ReLen : UINT32_BYTES;
            for ( i = 0; i < u8Cnt; i++ )
            {
                u32Data |= ( ( *pu8Buf++ ) << ( i * BYTE_BITS ) );
                u16ReLen--;
            }
            pstFlash->QSPI_TX_FIFO = u32Data;
        }

        u32Timeout ++;
        if ( FLASH_CMD_TIMEOUT_US <= u32Timeout )
        {
            return ERR_STA_TIMEOUT;
        }
        rom_delay_us ( 1 );
    }

    __set_PRIMASK ( u32Primask ); // When program cmd and data tx finished, can be suspended in irq

    /* When QSPI_FLASH_AUTO_BUSY_FLAG is 0, must disable AUTO_BUSY function */
    while ( ( FLASH->QSPI_FLASH_AUTO_BUSY_FLAG >> AUTO_BUSY_FLAG_SHIFT ) & AUTO_BUSY_FLAG_MSK ) {};

    FLASH->QSPI_FLASH_AUTO_BUSY_EN &= ~( ( ENABLE & AUTO_BUSY_EN_MSK ) << AUTO_BUSY_EN_SHIFT );

    /* 4. Watigng current command finished. */
    u32Timeout = 0;
    while ( pstFlash->QSPI_MEM_STATUS & MEM_IN_BUSY_STATUS )
    {
        u32Timeout ++;
        if ( FLASH_CMD_TIMEOUT_US <= u32Timeout )
        {
            return ERR_STA_TIMEOUT;
        }
        rom_delay_us ( 1 );
    }

    enRet = rom_hw_flash_wait_idle ( pstFlash, u16TimeoutUs );
    ERR_RETURN_IF_ERROR ( enRet );

    // Cache can not read flash with suspend
    pstFlash->QSPI_FLASH_BUSY &= ~ ( ( ENABLE & QSPI_FLASH_BUSY_MSK ) << QSPI_FLASH_BUSY_SHIFT );

    return ERR_STA_OK;
}

/**
 * @brief  Write the buffer data to  sip flash memory, can be suspended by CACHE read
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  u32Addr: Start Address of SIP FLASH MEMORY
 * @param  pu8Buf: Pointer to a buffer which will be written to flash memory
 * @param  u32Len: Length of write data, maximum length is 64 * 1024 Bytes;
 * @param  enMode: Access mode as below, @ref EN_FLASH_BUS_MODE_T
 * @param  u16TimeoutUs: Timeout of flash page program, unit is us.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_write_bytes_suspendable ( stQSPI_Handle_t* pstFlash, uint32_t u32Addr, uint8_t* pu8Buf,
                                                    uint32_t u32Len, EN_FLASH_BUS_MODE_T enMode, uint16_t u16TimeoutUs )
{
    uint32_t u32SrcAddr = u32Addr;
    uint32_t u32Length  = ( u32Len & FLASH_TRX_MAX_MSK );
    uint16_t u16ReLen   = 0;

    EN_ERR_STA_T enRet;


    if ( ( 0 == u32Length ) || ( NULL == pu8Buf ) )
    {
        return ERR_STA_ERROR;
    }

    enRet = rom_hw_flash_release_read_enhanced ( pstFlash, enMode );
    ERR_RETURN_IF_ERROR ( enRet );

    while ( u32Length )
    {
        u16ReLen = SFLASH_PAGE_BYTE_SIZE - ( u32SrcAddr & SFLASH_PAGE_MASK );

        if ( u16ReLen > u32Length )
        {
            u16ReLen = u32Length;
        }

        rom_hw_flash_write_bytes_in_page_suspendable ( pstFlash, u32SrcAddr, pu8Buf, u16ReLen, enMode, u16TimeoutUs );
        ERR_RETURN_IF_ERROR ( enRet );

        u32Length  -= u16ReLen;
        u32SrcAddr += u16ReLen;
        pu8Buf     += u16ReLen;
    }

    return ERR_STA_OK;
}
#endif

/**
 * @brief  Read sip flash memory from indicated start address with indicated access mode.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  u32Addr: Start Address of SIP FLASH MEMORY.
 * @param  pu8Buf: Pointer to a buffer which used to save the readed data.
 * @param  u32Len: Length of read data, maximum length is 64 * 1024 Bytes;
 * @param  enMode: Access mode as below, @ref EN_FLASH_BUS_MODE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_flash_read_bytes(stQSPI_Handle_t *pstFlash, uint32_t u32Addr, uint8_t *pu8Buf,
                                     uint32_t u32Len, EN_FLASH_BUS_MODE_T enMode)
{
    uint16_t i = 0;
    uint8_t u8Cnt = 0;
    uint32_t u32Length = u32Len;
    uint32_t u32CmdCfg = 0;
    uint32_t u32LenCfg = 0;
    uint32_t u32Data = 0;
    uint32_t u32Timeout = 0;

    EN_ERR_STA_T enRet;

    if ((!IS_VALID_FLASH_HANDLE(pstFlash)) || (NULL == pu8Buf))
    {
        return ERR_STA_ERROR;
    }

    if ((0 == u32Length) || (u32Length > FLASH_TRX_MAX_LENGTH))
    {
        return ERR_STA_ERROR;
    }

    enRet = rom_hw_flash_release_read_enhanced(pstFlash, enMode);
    ERR_RETURN_IF_ERROR(enRet);

    switch (enMode)
    {
    case BUS_MODE_SPI:
    {
        u32CmdCfg = SFLASH_READ_CMD_CFG;
        u32LenCfg = SFLASH_READ_LEN_CFG + ((u32Length - 1) << HALF_WORD_BITS);
        break;
    }
    case BUS_MODE_DPI:
    {
        u32CmdCfg = SFLASH_DREAD_CMD_CFG;
        u32LenCfg = SFLASH_DREAD_LEN_CFG + ((u32Length - 1) << HALF_WORD_BITS);
        break;
    }
    case BUS_MODE_QPI:
    {
        u32CmdCfg = SFLASH_QREAD_CMD_CFG;
        u32LenCfg = SFLASH_QREAD_LEN_CFG + ((u32Length - 1) << HALF_WORD_BITS);
        break;
    }
    default:
        return ERR_STA_ERROR;
    }

    pstFlash->QSPI_MEM_ADDR = u32Addr;

    enRet = rom_hw_flash_send_command(pstFlash, u32CmdCfg, u32LenCfg);
    ERR_RETURN_IF_ERROR(enRet);

    u32Timeout = 0;
    while (u32Length)
    {
        if (pstFlash->QSPI_INT_FLAG & QSPI_INT_RXFIFO_NOT_EMPTY)
        {
            u32Timeout = 0;
            u32Data = pstFlash->QSPI_RX_FIFO;

            u8Cnt = (u32Length < UINT32_BYTES) ? u32Length : UINT32_BYTES;

            for (i = 0; i < u8Cnt; i++)
            {
                *pu8Buf++ = (u32Data >> (i * BYTE_BITS));
                u32Length--;
            }

            pstFlash->QSPI_INT_CLR = QSPI_INT_RXFIFO_NOT_EMPTY;
        }

        u32Timeout++;
        if (u32Timeout >= FLASH_CMD_TIMEOUT_US)
        {
            return ERR_STA_TIMEOUT;
        }
        rom_delay_us(1);
    }

    return rom_hw_flash_wait_idle(pstFlash, FLASH_CMD_TIMEOUT_US);
}
