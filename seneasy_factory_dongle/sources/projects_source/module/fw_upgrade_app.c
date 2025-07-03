/**********************************************************************************************************************
 * @file     fw_upgrade_app.c
 * @version  V3.0
 * @date     2022/12/2
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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "app_cfg.h"

#include "fw_upgrade_interface.h"
#include "fw_upgrade_memory.h"
#include "fw_upgrade_process.h"
#include "fw_upgrade_app.h"


//=====================================================================================================================
// LOCAL VARIABLES
//=====================================================================================================================
static uint8_t m_au8RxRingBuf[1024];
static uint8_t m_au8ProtocolCmdBuf[1024];




static uint32_t fw_upgrade_interface_receive_data(uint16_t *pu16TxIndex, uint16_t *pu16RxIndex)
{
    // 1. Check data buffer is empty?
    if (fw_upgrade_memory_is_empty())
    {
        return 1;
    }

    // 2. Check data buffer length
    if (fw_upgrade_memory_get_length() < 6)
    {
        return 1;
    }

    uint16_t len = fw_upgrade_memory_read(m_au8ProtocolCmdBuf, fw_upgrade_memory_get_length());
    fw_upgrade_memory_write(len);

    // PRINTF("[%d]RCV: ", len);
    // for (int i = 0; i < len;i++)
    // {
    //     PRINTF("%02X ", m_au8ProtocolCmdBuf[i]);
    // }
    // PRINTF("\n");

    fw_upgrade_process(m_au8ProtocolCmdBuf, len);

    return 0;
}

/**
 * @brief  Firmware upgrade init.
 * @param  u8Interface: Communication interface, @ref enFwUpgradePhyInterface_t.
 */
uint8_t fw_upgrade_init(uint8_t u8Interface)
{
    stFwUpgradeInterfaceInit_t stInterfaceInit =
    {
        .pfnCB = fw_upgrade_interface_receive_data,

        .pu8TxBuf = NULL,
        .u16TxBufLen = 0,

        .pu8RxBuf = m_au8RxRingBuf,
        .u16RxBufLen = sizeof(m_au8RxRingBuf),
    };

    // 1. interface init
    fw_upgrade_interface_init(u8Interface, &stInterfaceInit);

    // 2. memory init
    fw_upgrade_memory_init(m_au8RxRingBuf, sizeof(m_au8RxRingBuf));

    // 3. pfnParseCB
    // soc_transfer_protocol_init(pfnParseCB);

    return 0;
}

