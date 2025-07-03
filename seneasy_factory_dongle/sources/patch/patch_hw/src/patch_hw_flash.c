/**
 * @file     hw_flash.c
 * @author   Maxscend SoC Software Team
 * @version  V1.1
 * @date     2023/6/10
 * @history
 * @note
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2023, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "RegMXD2670.h"
#include "err_def.h"
#include "sflash.h"

#include "type_def.h"
#include "utility.h"

#include "hw_flash.h"

/**
 * @brief  DEFINE
 */
#define FLASH_TW_TIMEOUT_US  (15000) // unit us, datasheet is tW = 12ms
#define FLASH_CMD_TIMEOUT_US (5000)  // unit us, qspi clock > 2KHz

/**
 * @brief  Release flash from read enhanced mode.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @return status: @ref EN_ERR_STA_T.
 */
__RAM_FUNC EN_ERR_STA_T patch_hw_flash_release_read_enhanced(stQSPI_Handle_t *pstFlash)
{
    uint8_t      u8Count = 2;
    EN_ERR_STA_T enRet;

    if (!IS_VALID_FLASH_HANDLE(pstFlash))
    {
        return ERR_STA_ERROR;
    }

    while (u8Count--)
    {
        enRet = rom_hw_flash_send_command(pstFlash, SFLASH_CRMR_CMD_CFG, SFLASH_CRMR_LEN_CFG);
        ERR_RETURN_IF_ERROR(enRet);
    }

    return ERR_STA_OK;
}

/**
 * @brief  Read flash status from bit15 to bit0.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  pu16Status: Pointer to save the status.
 * @return status: @ref EN_ERR_STA_T.
 */
__RAM_FUNC EN_ERR_STA_T patch_hw_flash_read_status(stQSPI_Handle_t *pstFlash, uint16_t *pu16Status)
{
    EN_ERR_STA_T enRet;

    enRet = patch_hw_flash_release_read_enhanced(pstFlash);
    ERR_RETURN_IF_ERROR(enRet);

    return rom_hw_flash_read_status(pstFlash, pu16Status);
}

/**
 * @brief  Flash controller write XiP flash status[15:0].
 *         The Write Status Register (WRSR) command allows new values to be written to the Status Register. Before it
 *         can be accepted, a Write Enable (WREN) command must previously have been executed. The WRSR command has no
 *         effect on S15, S10, S1 and S0 of the Status Register.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  u16Status: new values to be written to the Status Register[15:0].
 * @return status: @ref EN_ERR_STA_T.
 */
__RAM_FUNC EN_ERR_STA_T patch_hw_flash_write_status(stQSPI_Handle_t *pstFlash, uint16_t u16Status)
{
    EN_ERR_STA_T enRet;

    enRet = patch_hw_flash_release_read_enhanced(pstFlash);
    ERR_RETURN_IF_ERROR(enRet);

    return rom_hw_flash_write_status(pstFlash, u16Status);
}

/**
 * @brief  sFlash controller read XiP flash status[15:8].
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  pu16Status: Status[15:8].
 * @return status: @ref EN_ERR_STA_T.
 */
__RAM_FUNC EN_ERR_STA_T patch_hw_flash_read_status_high(stQSPI_Handle_t *pstFlash, uint8_t *pu8Status)
{
    EN_ERR_STA_T enRet;

    enRet = patch_hw_flash_release_read_enhanced(pstFlash);
    ERR_RETURN_IF_ERROR(enRet);

    return rom_hw_flash_read_status_high(pstFlash, pu8Status);
}

/**
 * @brief  sFlash controller read XiP flash status[7:0].
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  pu16Status: Status[15:8].
 * @return status: @ref EN_ERR_STA_T.
 */
__RAM_FUNC EN_ERR_STA_T patch_hw_flash_read_status_low(stQSPI_Handle_t *pstFlash, uint8_t *pu8Status)
{
    EN_ERR_STA_T enRet;

    enRet = patch_hw_flash_release_read_enhanced(pstFlash);
    ERR_RETURN_IF_ERROR(enRet);

    return rom_hw_flash_read_status_low(pstFlash, pu8Status);
}

/**
 * @brief  Set XiP flash quad access mode.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  enEn: Enable or Disable Quad mode @ref EN_QSPI_ENABLE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
__RAM_FUNC EN_ERR_STA_T patch_hw_flash_set_quad_enable(stQSPI_Handle_t *pstFlash, EN_QSPI_ENABLE_T enEn)
{
    EN_ERR_STA_T enRet;

    enRet = patch_hw_flash_release_read_enhanced(pstFlash);
    ERR_RETURN_IF_ERROR(enRet);

    return rom_hw_flash_set_quad_enable(pstFlash, enEn);
}

/**
 * @brief  Set XiP Flash exit abnormal state.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @return status: @ref EN_ERR_STA_T.
 */
__RAM_FUNC EN_ERR_STA_T patch_hw_flash_exit_abnormal_state(stQSPI_Handle_t *pstFlash)
{
    EN_ERR_STA_T enRet;

    enRet = patch_hw_flash_release_read_enhanced(pstFlash);
    ERR_RETURN_IF_ERROR(enRet);

    return rom_hw_flash_exit_abnormal_state(pstFlash);
}

/**
 * @brief  Get XiP Flash Mmemory ID by SFlash Controller.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  pu32ID: Pointer to a 32-bit to save the memory id.
 * @return status: @ref EN_ERR_STA_T.
 */
__RAM_FUNC EN_ERR_STA_T patch_hw_flash_get_id(stQSPI_Handle_t *pstFlash, uint32_t *pu32ID)
{
    EN_ERR_STA_T enRet;

    enRet = patch_hw_flash_release_read_enhanced(pstFlash);
    ERR_RETURN_IF_ERROR(enRet);

    return rom_hw_flash_get_id(pstFlash, pu32ID);
}

/**
 * @brief  Get XiP Flash Mmemory Unique ID by SFlash Controller.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  pu8ID: Pointer to a 128-bit to save the unique id.
 * @return status: @ref EN_ERR_STA_T.
 */
__RAM_FUNC EN_ERR_STA_T patch_hw_flash_get_unique_id(stQSPI_Handle_t *pstFlash, uint8_t *pu8ID)
{
    uint16_t i          = 0;
    uint8_t  u8Cnt      = 0;
    uint32_t u32Data    = 0;
    uint32_t u32Timeout = 0;
    uint16_t u16Length  = UINT128_BYTES;

    EN_ERR_STA_T enRet;

    if ((!IS_VALID_FLASH_HANDLE(pstFlash)) || (NULL == pu8ID))
    {
        return ERR_STA_ERROR;
    }

    enRet = patch_hw_flash_release_read_enhanced(pstFlash);
    ERR_RETURN_IF_ERROR(enRet);

    enRet = rom_hw_flash_send_command(pstFlash, SFLASH_RUID_CMD_CFG, SFLASH_RUID_LEN_CFG);
    ERR_RETURN_IF_ERROR(enRet);

    while (u16Length)
    {
        if (pstFlash->QSPI_INT_FLAG & QSPI_INT_RXFIFO_NOT_EMPTY)
        {
            u32Timeout = 0;
            u32Data    = pstFlash->QSPI_RX_FIFO;

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
 * @return status: @ref EN_ERR_STA_T.
 */
__RAM_FUNC EN_ERR_STA_T patch_hw_flash_enter_deep_power_down(stQSPI_Handle_t *pstFlash)
{
    EN_ERR_STA_T enRet;

    enRet = patch_hw_flash_release_read_enhanced(pstFlash);
    ERR_RETURN_IF_ERROR(enRet);

    return rom_hw_flash_enter_deep_power_down(pstFlash, BUS_MODE_QPI);
}

/**
 * @brief  Release from DEEP POWER DOWN state.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @return status: @ref EN_ERR_STA_T.
 */
__RAM_FUNC EN_ERR_STA_T patch_hw_flash_release_deep_power_down(stQSPI_Handle_t *pstFlash)
{
    EN_ERR_STA_T enRet;

    enRet = patch_hw_flash_release_read_enhanced(pstFlash);
    ERR_RETURN_IF_ERROR(enRet);

    return rom_hw_flash_release_deep_power_down(pstFlash);
}

/**
 * @brief  Set XiP FLASH MEMORY block Protect.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  u16BlockCfg: Block protect area configuration.
 * @return status: @ref EN_ERR_STA_T.
 */
__RAM_FUNC EN_ERR_STA_T patch_hw_flash_set_block_protect(stQSPI_Handle_t *pstFlash, uint16_t u16BlockCfg)
{
    uint16_t     u16Status = 0;
    EN_ERR_STA_T enRet;

    if ((!IS_VALID_FLASH_HANDLE(pstFlash)))
    {
        return ERR_STA_ERROR;
    }

    enRet = patch_hw_flash_read_status(pstFlash, &u16Status);
    ERR_RETURN_IF_ERROR(enRet);

    u16Status &=
        ~((SFLASH_STATUS_BP_MSK << SFLASH_STATUS_BP_SHIFT) | (SFLASH_STATUS_CMP_MSK << SFLASH_STATUS_CMP_SHIFT));
    u16Status |= (u16BlockCfg & SFLASH_STATUS_BP_MSK) << SFLASH_STATUS_BP_SHIFT;

    enRet = rom_hw_flash_send_command(pstFlash, SFLASH_WREN_CMD_CFG, SFLASH_WREN_LEN_CFG);
    ERR_RETURN_IF_ERROR(enRet);

    pstFlash->QSPI_TX_FIFO = u16Status;

    enRet = rom_hw_flash_send_command(pstFlash, SFLASH_WRSR_CMD_CFG, SFLASH_WRSR_LEN_CFG);
    ERR_RETURN_IF_ERROR(enRet);

    return rom_hw_flash_wait_idle(pstFlash, FLASH_TW_TIMEOUT_US);
}

/**
 * @brief  Erase XiP flash memory with indicated @ref EN_SFLASH_ERASE_SIZE_T number.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  enSize: The range of Erase, @ref EN_SFLASH_ERASE_SIZE_T.
 * @param  u16Num: Number of Erase.
 * @param  u32Timeoutus: Timeout of flash page/sector/block erase, unit is us.
 * @return status: @ref EN_ERR_STA_T.
 */
__RAM_FUNC EN_ERR_STA_T patch_hw_flash_erase(stQSPI_Handle_t *pstFlash, EN_SFLASH_ERASE_SIZE_T enSize, uint16_t u16Num,
                                             uint32_t u32Timeoutus)
{
    EN_ERR_STA_T enRet;

    enRet = patch_hw_flash_release_read_enhanced(pstFlash);
    ERR_RETURN_IF_ERROR(enRet);

    return rom_hw_flash_erase(pstFlash, enSize, u16Num, u32Timeoutus);
}

/**
 * @brief  Erase XiP flash memory.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  u32Timeoutus: Timeout of flash chip erase, unit is us.
 * @return status: @ref EN_ERR_STA_T.
 */
__RAM_FUNC EN_ERR_STA_T patch_hw_flash_chip_erase(stQSPI_Handle_t *pstFlash, uint32_t u32Timeoutus)
{
    EN_ERR_STA_T enRet;

    enRet = patch_hw_flash_release_read_enhanced(pstFlash);
    ERR_RETURN_IF_ERROR(enRet);

    return rom_hw_flash_chip_erase(pstFlash, u32Timeoutus);
}

/**
 * @brief  Write the buffer data to XiP flash memory from indicated start address with indicated access mode
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  u32Addr: Start Address of XiP FLASH MEMORY.
 * @param  pu8Buf: Pointer to a buffer which will be written to flash memory.
 * @param  u32Len: Length of write data, maximum length is 64 * 1024 Bytes.
 * @param  enMode: Access mode as below, @ref EN_FLASH_BUS_MODE_T.
 * @param  u16TimeoutUs: Timeout of flash page program, unit is us.
 * @return status: @ref EN_ERR_STA_T.
 */
__RAM_FUNC EN_ERR_STA_T patch_hw_flash_write_bytes(stQSPI_Handle_t *pstFlash, uint32_t u32Addr, uint8_t *pu8Buf,
                                                   uint32_t u32Len, EN_FLASH_BUS_MODE_T enMode, uint16_t u16TimeoutUs)
{
    EN_ERR_STA_T enRet;

    enRet = patch_hw_flash_release_read_enhanced(pstFlash);
    ERR_RETURN_IF_ERROR(enRet);

    return rom_hw_flash_write_bytes(pstFlash, u32Addr, pu8Buf, u32Len, enMode, u16TimeoutUs);
}

/**
 * @brief  Read XiP flash memory from indicated start address with indicated access mode.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  u32Addr: Start Address of XiP FLASH MEMORY.
 * @param  pu8Buf: Pointer to a buffer which used to save the readed data.
 * @param  u32Len: Length of read data, maximum length is 64 * 1024 Bytes;
 * @param  enMode: Access mode as below, @ref EN_FLASH_BUS_MODE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
__RAM_FUNC EN_ERR_STA_T patch_hw_flash_read_bytes(stQSPI_Handle_t *pstFlash, uint32_t u32Addr, uint8_t *pu8Buf,
                                                  uint32_t u32Len, EN_FLASH_BUS_MODE_T enMode)
{
    EN_ERR_STA_T enRet;

    enRet = patch_hw_flash_release_read_enhanced(pstFlash);
    ERR_RETURN_IF_ERROR(enRet);

    return rom_hw_flash_read_bytes(pstFlash, u32Addr, pu8Buf, u32Len, enMode);
}

/**
 * @brief  Erase XiP flash memory which can be suspend by CACHE read.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  enSize: The range of Erase, @ref EN_SFLASH_ERASE_SIZE_T.
 * @param  u16Num: Number of enSize Erase.
 * @param  u32Timeoutus: Timeout of flash page/sector/block erase, unit is us.
 * @return status: @ref EN_ERR_STA_T.
 */
__RAM_FUNC EN_ERR_STA_T patch_hw_flash_erase_suspendable(stQSPI_Handle_t *pstFlash, EN_SFLASH_ERASE_SIZE_T enSize,
                                                         uint16_t u16Num, uint32_t u32Timeoutus)
{
    uint32_t u32Ret    = 0;
    uint32_t u32CmdCfg = 0;
    uint32_t u32LenCfg = 0;

    if ((!IS_VALID_FLASH_HANDLE(pstFlash)))
    {
        return ERR_STA_ERROR;
    }

    // For more security, before erase cmd send finished, should disable irq
    uint32_t u32Primask = __get_PRIMASK();
    __disable_irq();

    /* Configure suspend */
    pstFlash->QSPI_FLASH_SUSPEND_CTRL =
        ((QSPI_FLASH_SUS_UNIT_10US & QSPI_FLASH_SUS_UNIT_CFG_MSK) << QSPI_FLASH_SUS_UNIT_CFG_SHIFT)
        | ((QSPI_FLASH_SUS_TERS & QSPI_FLASH_SUS_TERS_MSK) << QSPI_FLASH_SUS_TERS_SHIFT)
        | ((QSPI_FLASH_SUS_TPEFOR & QSPI_FLASH_SUS_TPEFOR_MSK) << QSPI_FLASH_SUS_TPEFOR_SHIFT)
        | ((QSPI_FLASH_SUS_TPESL & QSPI_FLASH_SUS_TPESL_MSK) << QSPI_FLASH_SUS_TPESL_SHIFT);

    pstFlash->QSPI_FLASH_CHECK_BUSY_INTERVAL = ((32 & CHECK_BUSY_PERIOD_MSK) << CHECK_BUSY_PERIOD_SHIFT)
                                               | ((3200 & CHECK_BUSY_INIT_PERIOD_MSK) << CHECK_BUSY_INIT_PERIOD_SHIFT);

    pstFlash->QSPI_FLASH_CHECK_BUSY_CMD = ((SFLASH_CMD_RDSR & CHECK_BUSY_CMD_MSK) << CHECK_BUSY_CMD_SHIFT)
                                          | ((BUS_MODE_SPI & CHECK_BUSY_CMD_MODE_MSK) << CHECK_BUSY_CMD_MODE_SHIFT);

    do
    {
        u32Ret |= rom_hw_flash_release_read_enhanced(pstFlash, BUS_MODE_QPI);

        u32Ret |= rom_hw_flash_send_command(pstFlash, SFLASH_WREN_CMD_CFG, SFLASH_WREN_LEN_CFG);

        /* Enable flash auto busy function, flash controller will send command of SFLASH_CMD_RDSR automatically */
        pstFlash->QSPI_FLASH_AUTO_BUSY_EN |= (ENABLE & AUTO_BUSY_EN_MSK) << AUTO_BUSY_EN_SHIFT;

        /* Enable flash suspend function, flash controller send command of 75h/0Bh/7Ah automatically */
        pstFlash->QSPI_FLASH_SUSPEND_EN = (ENABLE & SUSPEND_EN_MSK) << SUSPEND_EN_SHIFT;

        switch (enSize)
        {
            case ERASE_PAGE:
            {
                u32CmdCfg               = SFLASH_PE_CMD_CFG;
                u32LenCfg               = SFLASH_PE_LEN_CFG;
                pstFlash->QSPI_MEM_ADDR = u16Num << SFLASH_PAGE_SHIFT_BITS;
                break;
            }
            case ERASE_SECTOR:
            {
                u32CmdCfg               = SFLASH_SE_CMD_CFG;
                u32LenCfg               = SFLASH_SE_LEN_CFG;
                pstFlash->QSPI_MEM_ADDR = u16Num << SFLASH_SECTOR_SHIFT_BITS;
                break;
            }
            case ERASE_BLOCK_32KB:
            {
                u32CmdCfg               = SFLASH_BE32K_CMD_CFG;
                u32LenCfg               = SFLASH_BE32K_LEN_CFG;
                pstFlash->QSPI_MEM_ADDR = u16Num << SFLASH_BLOCK_32KB_SHIFT_BITS;
                break;
            }
            case ERASE_BLOCK_64KB:
            {
                u32CmdCfg               = SFLASH_BE_CMD_CFG;
                u32LenCfg               = SFLASH_BE_LEN_CFG;
                pstFlash->QSPI_MEM_ADDR = u16Num << SFLASH_BLOCK_64KB_SHIFT_BITS;
                break;
            }
            default:
                return ERR_PARA_ERR;
        }

        u32Ret |= rom_hw_flash_send_command(pstFlash, u32CmdCfg, u32LenCfg);
    } while (0);

    // When erase cmd send finished, can be suspend in irq
    __set_PRIMASK(u32Primask);
    ERR_RETURN_IF_ERROR(u32Ret);

    /* When QSPI_FLASH_AUTO_BUSY_FLAG is 0, must disable AUTO_BUSY function */
    while ((pstFlash->QSPI_FLASH_AUTO_BUSY_FLAG >> AUTO_BUSY_FLAG_SHIFT) & AUTO_BUSY_FLAG_MSK)
    {
    };

    pstFlash->QSPI_FLASH_AUTO_BUSY_EN &= ~((ENABLE & AUTO_BUSY_EN_MSK) << AUTO_BUSY_EN_SHIFT);

    return rom_hw_flash_wait_idle(pstFlash, u32Timeoutus);
}

/**
 * @brief  Write the buffer data to XiP flash memory in one page, which can be suspended by CACHE read.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  u32Addr: Start Address of XiP FLASH MEMORY.
 * @param  pu8Buf: Pointer to a buffer which will be written to flash memory.
 * @param  u16Len: Length of write data.
 * @param  enMode: Access mode as below, @ref EN_FLASH_BUS_MODE_T.
 * @param  u16TimeoutUs: Timeout of flash page program, unit is us.
 * @return status: @ref EN_ERR_STA_T.
 */
__RAM_FUNC EN_ERR_STA_T patch_hw_flash_write_bytes_in_page_suspendable(stQSPI_Handle_t *pstFlash, uint32_t u32Addr,
                                                                       uint8_t *pu8Buf, uint16_t u16Len,
                                                                       EN_FLASH_BUS_MODE_T enMode,
                                                                       uint16_t            u16TimeoutUs)
{
    uint16_t i          = 0;
    uint8_t  u8Cnt      = 0;
    uint32_t u32Ret     = 0;
    uint16_t u16ReLen   = 0;
    uint32_t u32CmdCfg  = 0;
    uint32_t u32LenCfg  = 0;
    uint32_t u32Data    = 0;
    uint32_t u32Timeout = 0;

    if ((!IS_VALID_FLASH_HANDLE(pstFlash)) || (0 == u16Len) || (NULL == pu8Buf))
    {
        return ERR_STA_ERROR;
    }

    if (SFLASH_PAGE_BYTE_SIZE < (u32Addr & SFLASH_PAGE_MASK) + u16Len)
    {
        return ERR_STA_ERROR;
    }

    // For more security, before program cmd excute finished, should disable irq
    uint32_t u32Primask = __get_PRIMASK();
    __disable_irq();

    do
    {
        /* Configure suspend */
        pstFlash->QSPI_FLASH_SUSPEND_CTRL =
            ((QSPI_FLASH_SUS_UNIT_10US & QSPI_FLASH_SUS_UNIT_CFG_MSK) << QSPI_FLASH_SUS_UNIT_CFG_SHIFT)
            | ((QSPI_FLASH_SUS_TERS & QSPI_FLASH_SUS_TERS_MSK) << QSPI_FLASH_SUS_TERS_SHIFT)
            | ((QSPI_FLASH_SUS_TPEFOR & QSPI_FLASH_SUS_TPEFOR_MSK) << QSPI_FLASH_SUS_TPEFOR_SHIFT)
            | ((QSPI_FLASH_SUS_TPESL & QSPI_FLASH_SUS_TPESL_MSK) << QSPI_FLASH_SUS_TPESL_SHIFT);

        pstFlash->QSPI_FLASH_CHECK_BUSY_INTERVAL =
            ((320 & CHECK_BUSY_PERIOD_MSK) << CHECK_BUSY_PERIOD_SHIFT)
            | ((3200 & CHECK_BUSY_INIT_PERIOD_MSK) << CHECK_BUSY_INIT_PERIOD_SHIFT);

        pstFlash->QSPI_FLASH_CHECK_BUSY_CMD = ((SFLASH_CMD_RDSR & CHECK_BUSY_CMD_MSK) << CHECK_BUSY_CMD_SHIFT)
                                              | ((BUS_MODE_SPI & CHECK_BUSY_CMD_MODE_MSK) << CHECK_BUSY_CMD_MODE_SHIFT);

        u32Ret |= rom_hw_flash_send_command(pstFlash, SFLASH_WREN_CMD_CFG, SFLASH_WREN_LEN_CFG);

        /* Enable flash auto busy function, flash controller will send command of SFLASH_CMD_RDSR automatically */
        pstFlash->QSPI_FLASH_AUTO_BUSY_EN |= (ENABLE & AUTO_BUSY_EN_MSK) << AUTO_BUSY_EN_SHIFT;

        /* Enable flash suspend function, flash controller send command of 75h/0Bh/7Ah automatically */
        pstFlash->QSPI_FLASH_SUSPEND_EN = (ENABLE & SUSPEND_EN_MSK) << SUSPEND_EN_SHIFT;

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
        }

        u32Data  = 0;
        u16ReLen = u16Len;

        u8Cnt = (u16ReLen < UINT32_BYTES) ? u16ReLen : UINT32_BYTES;
        for (i = 0; i < u8Cnt; i++)
        {
            u32Data |= ((*pu8Buf++) << (i * BYTE_BITS));
            u16ReLen--;
        }

        pstFlash->QSPI_MEM_ADDR = u32Addr;
        pstFlash->QSPI_TX_FIFO  = u32Data;

        u32Ret |= rom_hw_flash_send_command(pstFlash, u32CmdCfg, u32LenCfg);

        u32Timeout = 0;
        while (u16ReLen)
        {
            pstFlash->QSPI_INT_CLR = QSPI_INT_TXFIFO_FULL;
            if (!(pstFlash->QSPI_INT_FLAG & QSPI_INT_TXFIFO_FULL))
            {
                u32Data = 0;
                u8Cnt   = (u16ReLen < UINT32_BYTES) ? u16ReLen : UINT32_BYTES;
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
                u32Ret |= ERR_STA_TIMEOUT;
                break;
            }
            rom_delay_us(1);
        }
    } while (0);

    __set_PRIMASK(u32Primask); // When program cmd and data tx finished, can be suspended in irq.
    ERR_RETURN_IF_ERROR(u32Ret);

    /* When QSPI_FLASH_AUTO_BUSY_FLAG is 0, must disable AUTO_BUSY function */
    while ((pstFlash->QSPI_FLASH_AUTO_BUSY_FLAG >> AUTO_BUSY_FLAG_SHIFT) & AUTO_BUSY_FLAG_MSK)
    {
    };

    pstFlash->QSPI_FLASH_AUTO_BUSY_EN &= ~((ENABLE & AUTO_BUSY_EN_MSK) << AUTO_BUSY_EN_SHIFT);

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
 * @brief  Write the buffer data to XiP flash memory which can be suspended by CACHE read.
 * @param  pstFlash: FLASH handle, Should be FLASH(FLASH0) / FLASH1.
 * @param  u32Addr: Start Address of XiP FLASH MEMORY.
 * @param  pu8Buf: Pointer to a buffer which will be written to flash memory.
 * @param  u32Len: Length of write data, maximum length is 64 * 1024 Bytes.
 * @param  enMode: Access mode as below, @ref EN_FLASH_BUS_MODE_T.
 * @param  u16TimeoutUs: Timeout of flash page program, unit is us.
 * @return status: @ref EN_ERR_STA_T.
 */
__RAM_FUNC EN_ERR_STA_T patch_hw_flash_write_bytes_suspendable(stQSPI_Handle_t *pstFlash, uint32_t u32Addr,
                                                               uint8_t *pu8Buf, uint32_t u32Len,
                                                               EN_FLASH_BUS_MODE_T enMode, uint16_t u16TimeoutUs)
{
    uint32_t u32SrcAddr = u32Addr;
    uint32_t u32Length  = (u32Len & FLASH_TRX_MAX_MSK);
    uint16_t u16ReLen   = 0;

    EN_ERR_STA_T enRet;

    if ((0 == u32Length) || (NULL == pu8Buf))
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

        patch_hw_flash_write_bytes_in_page_suspendable(pstFlash, u32SrcAddr, pu8Buf, u16ReLen, enMode, u16TimeoutUs);
        ERR_RETURN_IF_ERROR(enRet);

        u32Length -= u16ReLen;
        u32SrcAddr += u16ReLen;
        pu8Buf += u16ReLen;
    }

    return ERR_STA_OK;
}
