/**********************************************************************************************************************
 * @file     fw_upgrade_memory.c
 * @author   Maxscend SoC Software Team
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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "utility.h"

#include "fw_upgrade_interface.h"
#include "fw_upgrade_memory.h"


//=====================================================================================================================
// LOCAL VARIABLES
//=====================================================================================================================
static stRingBuf_t m_stRxRingBuf;




/**
 * @brief  Initial ring buf for serial data parse
 * @param  pstInit: Pointer to a stSocTpPhyInit_t structure.
 * @param  u32RingBufLen
 */
void fw_upgrade_memory_init(uint8_t *pu8RingBuf, uint32_t u32RingBufLen)
{
    m_stRxRingBuf.pu8Buf = pu8RingBuf;
    m_stRxRingBuf.u16BufMaxLen = u32RingBufLen;

    m_stRxRingBuf.u16TxIdx = 0;
    m_stRxRingBuf.u16RxIdx = 0;
}

/**
 * @brief  
 */
void fw_upgrade_memory_reinit(void)
{
    m_stRxRingBuf.u16TxIdx = 0;
    m_stRxRingBuf.u16RxIdx = 0;
}

/**
 * @brief  Initial ring buf for serial data parse
 * @return
 * @return
 */
bool fw_upgrade_memory_is_empty(void)
{
    fw_upgrade_interface_get_dma_trx_index(NULL, &m_stRxRingBuf.u16RxIdx);

    return (m_stRxRingBuf.u16TxIdx == m_stRxRingBuf.u16RxIdx);
}

/**
 * @brief
 * @return
 */
uint16_t fw_upgrade_memory_get_length(void)
{
    return ((m_stRxRingBuf.u16RxIdx + m_stRxRingBuf.u16BufMaxLen - m_stRxRingBuf.u16TxIdx) &
            (m_stRxRingBuf.u16BufMaxLen - 1));
}

/**
 * @brief  Read data from ring buffer.
 * @param  pu8RxBuf: Pointer to received data buffer.
 * @param  u16WantedRxLen: Wanted received data length.
 */
uint16_t fw_upgrade_memory_read(uint8_t *pu8RxBuf, uint16_t u16WantedRxLen)
{
    uint16_t u16RxLen, u16TmpLen;

    u16RxLen = (m_stRxRingBuf.u16RxIdx + m_stRxRingBuf.u16BufMaxLen - m_stRxRingBuf.u16TxIdx) & (m_stRxRingBuf.u16BufMaxLen - 1);

    u16RxLen = (u16RxLen > u16WantedRxLen) ? u16WantedRxLen : u16RxLen;

    if (m_stRxRingBuf.u16RxIdx > m_stRxRingBuf.u16TxIdx)
    {
        memcpy(pu8RxBuf, &m_stRxRingBuf.pu8Buf[m_stRxRingBuf.u16TxIdx], u16RxLen);
    }
    else
    {
        u16TmpLen = m_stRxRingBuf.u16BufMaxLen - m_stRxRingBuf.u16TxIdx;

        memcpy(pu8RxBuf, &m_stRxRingBuf.pu8Buf[m_stRxRingBuf.u16TxIdx], u16TmpLen);
        memcpy(&pu8RxBuf[u16TmpLen], m_stRxRingBuf.pu8Buf, m_stRxRingBuf.u16RxIdx);
    }

    return u16RxLen;
}

/**
 * @brief
 * @param  u16DataLen
 */
void fw_upgrade_memory_write(uint16_t u16Len)
{
    m_stRxRingBuf.u16TxIdx = (m_stRxRingBuf.u16TxIdx + u16Len) & (m_stRxRingBuf.u16BufMaxLen - 1);
}


