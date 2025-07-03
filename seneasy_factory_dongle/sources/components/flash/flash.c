/**
 * @file     flash.c
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2022/4/9
 * @history
 * @note     Application code of flash module.
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2022, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 */

#include <stdint.h>

#include "RegMXD2670.h"
#include "err_def.h"
#include "sflash.h"
#include "hw_flash.h"
#include "patch_hw_flash.h"

/**
 * @brief  Read sip flash memory from indicated start address with indicated access mode.
 * @param  u32Addr: Start Address of SIP FLASH MEMORY.
 * @param  pu8Buf: Pointer to a buffer which used to save the readed data.
 * @param  u32Len: Length of read data.
 * @param  enMode: Access mode as below, @ref EN_FLASH_BUS_MODE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T flash_read(uint32_t u32Addr, uint8_t *pu8Buf, uint32_t u32Len, EN_FLASH_BUS_MODE_T enMode)
{
    EN_ERR_STA_T enRet      = ERR_STA_OK;
    uint32_t     u32Primask = __get_PRIMASK();

    __disable_irq();

    enRet = patch_hw_flash_read_bytes(FLASH, u32Addr, pu8Buf, u32Len, enMode);

    __set_PRIMASK(u32Primask);

    return enRet;
}

/**
 * @brief  Write the buffer data to  sip flash memory from indicated start address with indicated access mode.
 * @param  u32Addr: Start Address of SIP FLASH MEMORY.
 * @param  pu8Buf: Pointer to a buffer which will be written to flash memory.
 * @param  u32Len: Length of write data, maximum length is 64 * 1024 Bytes;
 * @param  enMode: Access mode as below, @ref EN_FLASH_BUS_MODE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T flash_write(uint32_t u32Addr, uint8_t *pu8Buf, uint32_t u32Len, EN_FLASH_BUS_MODE_T enMode)
{
    EN_ERR_STA_T enRet      = ERR_STA_OK;
    uint32_t     u32Primask = __get_PRIMASK();

    __disable_irq();

    enRet = patch_hw_flash_write_bytes(FLASH, u32Addr, pu8Buf, u32Len, enMode, FLASH_PROGRAM_TIMEOUT_US);

    __set_PRIMASK(u32Primask);

    return enRet;
}

/**
 * @brief  Erase sip flash memory with indicated @ref EN_SFLASH_ERASE_SIZE_T number.
 * @param  enSize: The range of Erase, @ref EN_SFLASH_ERASE_SIZE_T.
 * @param  u16Num: Number of Erase.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T flash_erase(EN_SFLASH_ERASE_SIZE_T enSize, uint16_t u16Num)
{
    EN_ERR_STA_T enRet      = ERR_STA_OK;
    uint32_t     u32Primask = __get_PRIMASK();

    __disable_irq();

    enRet = patch_hw_flash_erase(FLASH, enSize, u16Num, FLASH_PAGE_ERASE_TIMEOUT_US);

    __set_PRIMASK(u32Primask);

    return enRet;
}
