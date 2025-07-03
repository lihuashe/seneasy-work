/**********************************************************************************************************************
 * @file     fw_upgrade_flash.h
 * @version  V3.0
 * @date     2022/11/29
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


#ifndef __FW_UPGRADE_FLASH_H__
#define __FW_UPGRADE_FLASH_H__




/**
 * @brief  Read sip flash memory from indicated start address with indicated access mode.
 * @param  u32Addr: Start Address of SIP FLASH MEMORY.
 * @param  pu8Buf: Pointer to a buffer which used to save the read data.
 * @param  u32Len: Length of read data, maximum length is 64 * 1024 Bytes;
 * @return status: @ref EN_ERR_STA_T
 */
extern uint8_t fw_upgrade_flash_read(uint8_t *pu8Buf, uint32_t u32Addr, uint16_t u32Len);


/**
 * @brief  Write the buffer data to sip flash memory from indicated start address with indicated access mode.
 * @param  u32Addr: Start Address of SIP FLASH MEMORY.
 * @param  pu8Buf: Pointer to a buffer which will be written to flash memory.
 * @param  u32Len: Length of write data, maximum length is 64 * 1024 Bytes.
 * @return status: @ref EN_ERR_STA_T.
 */
extern uint8_t fw_upgrade_flash_write(uint8_t *pu8Buf, uint32_t u32Addr, uint16_t u32Len);


/**
 * @brief  Erase sip flash memory.
 * @param  u32Addr: Start Address of SIP FLASH MEMORY.
 * @param  u32Len: Length of erase data.
 * @return status: @ref EN_ERR_STA_T.
 */
extern uint8_t fw_upgrade_flash_erase(uint32_t u32Addr, uint32_t u32Len);




#endif /* __FW_UPGRADE_FLASH_H__ */

