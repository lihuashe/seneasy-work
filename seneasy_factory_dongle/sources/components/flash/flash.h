/**
 * @file     flash.h
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

#ifndef __FLASH_H__
#define __FLASH_H__

/**
 * @brief  Read sip flash memory from indicated start address with indicated access mode.
 * @param  u32Addr: Start Address of SIP FLASH MEMORY.
 * @param  pu8Buf: Pointer to a buffer which used to save the readed data.
 * @param  u32Len: Length of read data.
 * @param  enMode: Access mode as below, @ref EN_FLASH_BUS_MODE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T flash_read(uint32_t u32Addr, uint8_t *pu8Buf, uint32_t u32Len, EN_FLASH_BUS_MODE_T enMode);

/**
 * @brief  Write the buffer data to  sip flash memory from indicated start address with indicated access mode.
 * @param  u32Addr: Start Address of SIP FLASH MEMORY.
 * @param  pu8Buf: Pointer to a buffer which will be written to flash memory.
 * @param  u32Len: Length of write data, maximum length is 64 * 1024 Bytes;
 * @param  enMode: Access mode as below, @ref EN_FLASH_BUS_MODE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T flash_write(uint32_t u32Addr, uint8_t *pu8Buf, uint32_t u32Len, EN_FLASH_BUS_MODE_T enMode);

/**
 * @brief  Erase sip flash memory with indicated @ref EN_SFLASH_ERASE_SIZE_T number.
 * @param  enSize: The range of Erase, @ref EN_SFLASH_ERASE_SIZE_T.
 * @param  u16Num: Number of Erase.
 * @return status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T flash_erase(EN_SFLASH_ERASE_SIZE_T enSize, uint16_t u16Num);

#endif /* __FLASH_H__ */
