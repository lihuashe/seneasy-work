/**********************************************************************************************************************
 * @file     fw_upgrade_memory.h
 * @version  V1.0
 * @date     2022/3/24
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
 * Copyright (c) 2016~2020, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 *********************************************************************************************************************/

#ifndef __FW_UPGRADE_MEMORY_H__
#define __FW_UPGRADE_MEMORY_H__


//=====================================================================================================================
// TYPEDEF
//=====================================================================================================================
typedef struct
{
    uint16_t u16RxIdx;
    uint16_t u16TxIdx;
    uint8_t *pu8Buf;
    uint16_t u16BufMaxLen;

} stRingBuf_t;

extern void fw_upgrade_memory_init(uint8_t *pu8RingBuf, uint32_t u32RingBufLen);

extern bool fw_upgrade_memory_is_empty(void);
extern uint16_t fw_upgrade_memory_get_length(void);

extern uint16_t fw_upgrade_memory_read(uint8_t *pu8RxBuf, uint16_t u16WantedRxLen);
extern void fw_upgrade_memory_write(uint16_t u16Len);

extern void fw_upgrade_memory_reinit(void);

#endif /* __FW_UPGRADE_MEMORY_H__ */


