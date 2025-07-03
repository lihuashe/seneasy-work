/**********************************************************************************************************************
 * @file     fw_upgrade_flash.c
 * @version  V3.0
 * @date     2022/07/29
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
 * Copyright (c) 2016~2022, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 *********************************************************************************************************************/

#include <stdbool.h>
#include <stdint.h>

#include "RegMXD2670.h"
#include "err_def.h"
#include "sflash.h"
#include "hw_flash.h"




/**
 * @brief  Read sip flash memory from indicated start address with indicated access mode.
 * @param  u32Addr: Start Address of SIP FLASH MEMORY.
 * @param  pu8Buf: Pointer to a buffer which used to save the read data.
 * @param  u32Len: Length of read data, maximum length is 64 * 1024 Bytes;
 * @return status: @ref EN_ERR_STA_T
 */
uint8_t fw_upgrade_flash_read(uint8_t *pu8Buf, uint32_t u32Addr, uint16_t u32Len)
{
    return rom_hw_flash_read_bytes(FLASH, u32Addr, pu8Buf, u32Len, BUS_MODE_QPI);
}

/**
 * @brief  Write the buffer data to sip flash memory from indicated start address with indicated access mode.
 * @param  u32Addr: Start Address of SIP FLASH MEMORY.
 * @param  pu8Buf: Pointer to a buffer which will be written to flash memory.
 * @param  u32Len: Length of write data, maximum length is 64 * 1024 Bytes.
 * @return status: @ref EN_ERR_STA_T.
 */
uint8_t fw_upgrade_flash_write(uint8_t *pu8Buf, uint32_t u32Addr, uint16_t u32Len)
{
    return rom_hw_flash_write_bytes(FLASH, u32Addr, pu8Buf, u32Len, BUS_MODE_QPI, FLASH_PROGRAM_TIMEOUT_US);
}

/**
 * @brief  Erase sip flash memory.
 * @param  u32Addr: Start Address of SIP FLASH MEMORY.
 * @param  u32Len: Length of erase data.
 * @return status: @ref EN_ERR_STA_T.
 */
uint8_t fw_upgrade_flash_erase(uint32_t u32Addr, uint32_t u32Len)
{
    uint32_t addr = u32Addr;
    uint32_t len = u32Len;
    uint32_t eraseSize;
    uint8_t retCode = 0;

    do
    {
        if ((len >= SFLASH_BLOCK_64KB_BYTE_SIZE) && (addr % SFLASH_BLOCK_64KB_BYTE_SIZE == 0))
        {
            retCode = rom_hw_flash_erase(FLASH, ERASE_BLOCK_64KB, addr >> SFLASH_BLOCK_64KB_SHIFT_BITS,
                                         FLASH_PAGE_ERASE_TIMEOUT_US);
            eraseSize = SFLASH_BLOCK_64KB_BYTE_SIZE;
        }
        else if ((len >= SFLASH_BLOCK_32KB_BYTE_SIZE) && (addr % SFLASH_BLOCK_32KB_BYTE_SIZE == 0))
        {
            retCode = rom_hw_flash_erase(FLASH, ERASE_BLOCK_32KB, addr >> SFLASH_BLOCK_32KB_SHIFT_BITS,
                                         FLASH_PAGE_ERASE_TIMEOUT_US);
            eraseSize = SFLASH_BLOCK_32KB_BYTE_SIZE;
        }
        else if ((len >= SFLASH_SECTOR_BYTE_SIZE) && (addr % SFLASH_SECTOR_BYTE_SIZE == 0))
        {
            retCode = rom_hw_flash_erase(FLASH, ERASE_SECTOR, addr >> SFLASH_SECTOR_SHIFT_BITS,
                                         FLASH_PAGE_ERASE_TIMEOUT_US);
            eraseSize = SFLASH_SECTOR_BYTE_SIZE;
        }
        else
        {
            retCode = rom_hw_flash_erase(FLASH, ERASE_PAGE, addr >> SFLASH_PAGE_SHIFT_BITS,
                                         FLASH_PAGE_ERASE_TIMEOUT_US);
            eraseSize = SFLASH_PAGE_BYTE_SIZE;
        }

        if (ERR_STA_OK != retCode)
        {
            return ERR_STA_ERROR;
        }

        len -= eraseSize;
        addr += eraseSize;

    } while (addr < (u32Addr + u32Len));

    return ERR_STA_OK;
}


