/**********************************************************************************************************************
 * @file     fw_upgrade_thread.c
 * @version  V3.0
 * @date     2022/11/30
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "app_cfg.h"
#include "err_def.h"

#include "cmsis_os2.h"
#include "rtx_os.h"

#include "fw_upgrade_process.h"
#include "fw_upgrade_thread.h"


//=====================================================================================================================
// TYPEDEF
//=====================================================================================================================
#define CHECK_OS_RETURN(id, entity)           \
    do                                        \
    {                                         \
        if (NULL == id)                       \
        {                                     \
            PRINTF("%s new fail.\n", entity); \
            return ERR_STA_ERROR;             \
        }                                     \
    } while (0)

//=====================================================================================================================
// TYPEDEF
//=====================================================================================================================
static uint32_t fwUpgradeRcvTaskCbMem[osRtxThreadCbSize / sizeof(uint32_t)] __attribute__((section(".bss.os.thread.cb")));
static uint64_t fwUpgradeRcvTaskStackMem[1024 / sizeof(uint64_t)];

/* Message Queue */
static uint32_t fwUpgradeQueueCbMem[osRtxMessageQueueCbSize / sizeof(uint32_t)] __attribute__((section(".bss.os.msgqueue.cb")));
static uint32_t fwUpgradeQueueMqMem[osRtxMessageQueueMemSize(FW_UPGRADE_MSG_COUNT, sizeof(stFwUpgradeThread_t)) / sizeof(uint32_t)];

static osThreadId_t m_fwUpgradeRcvTaskId;
static osMessageQueueId_t m_fwUpgradeRcvQueueId;





/**
 * @brief  Firmware upgrade receive task handler.
 * @param  argument.
 */
static void fw_upgrade_thread_receive_task(void *argument)
{
    osStatus_t osStatus;
    stFwUpgradeThread_t stFwUpgradeThread;

    while (1)
    {
        osStatus = osMessageQueueGet(m_fwUpgradeRcvTaskId, &stFwUpgradeThread, NULL, osWaitForever);
        if (osStatus == osOK)
        {
            PRINTF("Rcv a msg.\n");
            fw_upgrade_process(stFwUpgradeThread.au8Data, stFwUpgradeThread.u16DataLen);
        }
    }
}

/**
 * @brief  Firmware upgrade task init.
 * @return status: @ref EN_ERR_STA_T.
 */
uint8_t fw_upgrade_thread_init(void)
{
    const osThreadAttr_t taskAttr =
    {
        .name = "fw_upgrade_rcv_task",
        .cb_mem = fwUpgradeRcvTaskCbMem,
        .cb_size = sizeof(fwUpgradeRcvTaskCbMem),
        .stack_mem = fwUpgradeRcvTaskStackMem,
        .stack_size = sizeof(fwUpgradeRcvTaskStackMem),
        .priority = osPriorityNormal
    };

    const osMessageQueueAttr_t queueAttr =
    {
        .name = "fw_upgrade_queue",
        .cb_mem = &fwUpgradeQueueCbMem,
        .cb_size = sizeof(fwUpgradeQueueCbMem),
        .mq_mem = fwUpgradeQueueMqMem,
        .mq_size = sizeof(fwUpgradeQueueMqMem)
    };

    m_fwUpgradeRcvTaskId = osThreadNew(fw_upgrade_thread_receive_task, NULL, &taskAttr);
    CHECK_OS_RETURN(m_fwUpgradeRcvTaskId, "fw_upgrade_thread_receive_task");
    PRINTF("m_fwUpgradeRcvTaskId: %08X\n", (uint32_t)m_fwUpgradeRcvTaskId);

    m_fwUpgradeRcvQueueId = osMessageQueueNew(FW_UPGRADE_MSG_COUNT, sizeof(stFwUpgradeThread_t), &queueAttr);
    CHECK_OS_RETURN(m_fwUpgradeRcvQueueId, "fw_upgrade_rcv_queue");
    PRINTF("m_fwUpgradeRcvQueueId: %08X\n", (uint32_t)m_fwUpgradeRcvQueueId);

    return ERR_STA_OK;
}

/**
 * @brief  Send message to queue.
 * @param  pu8Buf: Pointer to data buffer.
 * @param  u16Len: Buffer length.
 * @return status: @ref EN_ERR_STA_T.
 */
uint8_t fw_upgrade_thread_send_msg_to_queue(uint8_t *pu8Buf, uint16_t u16Len)
{
    stFwUpgradeThread_t stFwUpgradeThread;

    if ((NULL == pu8Buf) && (0 != u16Len))
    {
        return ERR_PARA_ERR;
    }

    if ((u16Len > FW_UPGRADE_MSG_DATA_LEN))
    {
        return ERR_PARA_ERR;
    }

    stFwUpgradeThread.u16DataLen = u16Len;
    memcpy(&stFwUpgradeThread.au8Data, pu8Buf, u16Len);

    if (osOK != osMessageQueuePut(m_fwUpgradeRcvQueueId, &stFwUpgradeThread, NULL, 0))
    {
        return ERR_STA_BUSY;
    }

    return ERR_STA_OK;
}

