/**********************************************************************************************************************
 * @file     msg_queue.c
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2022/12/27
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
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "app_cfg.h"
#include "msg_queue.h"




/**
 * @brief  Create queue.
 * @param  pstQueue: Pointer to the queue, @ref stQueueDef_t.
 * @param  pstQueueBuf: Pointer the buffer to save data.
 * @param  u32BufSize: Buffer size.
 * @param  u32MsgCount: Number of the messages.
 * @param  u32MsgSize: Size of the message.
 * @param  enMode: Write mode, @ref EN_QUEUE_MODE_T.
 * @return msgQueueId_t.
 */
pMsgQueueId_t msg_queue_create(stQueueDef_t *pstQueue, uint8_t *pstQueueBuf, uint32_t u32BufSize,
                               uint32_t u32MsgCount, uint32_t u32MsgSize, EN_QUEUE_MODE_T enMode)
{
    if ((NULL == pstQueue) || (NULL == pstQueueBuf) || \
        (0U == u32BufSize) || (0U == u32MsgCount) || (0U == u32MsgSize))
    {
        return NULL;
    }

    pstQueue->pu32Buffer          = pstQueueBuf;
    pstQueue->u32BlockSize        = u32MsgSize;
    pstQueue->u32MsgCount         = (u32MsgCount - 1);
    pstQueue->u32BufSize          = u32BufSize;
    pstQueue->pstCb.u8Front       = 0;
    pstQueue->pstCb.u8Back        = 0;
    pstQueue->pstCb.u8Utilization = 0;
    pstQueue->enMode              = enMode;

    return pstQueue;
}

/**
 * @brief  Get the next idx of the message queue.
 * @param  pstQueue: point to the queue, @ref stQueueDef_t.
 * @param  u8Idx: index.
 * @return index.
 */
static uint8_t msg_queue_next_idx(stQueueDef_t const *pstQueue, uint8_t u8Idx)
{
    if (NULL == pstQueue)
    {
        return QUEUE_ERR;
    }

    return (u8Idx < pstQueue->u32MsgCount) ? (u8Idx + 1) : 0;
}

/**
 * @brief  Determine if the message queue is full.
 * @param  pstQueue: point to the queue, @ref stQueueDef_t.
 * @return true or false.
 */
bool msg_queue_is_full(stQueueDef_t const *pstQueue)
{
    if (NULL == pstQueue)
    {
        return QUEUE_ERR;
    }

    uint8_t front = pstQueue->pstCb.u8Front;
    uint8_t back = pstQueue->pstCb.u8Back;

    return (msg_queue_next_idx(pstQueue, back) == front);
}

/**
 * @brief  Determine if the message queue is full.
 * @param  pstQueue: point to the queue, @ref stQueueDef_t.
 * @return true or false.
 */
bool msg_queue_is_empty(stQueueDef_t const *pstQueue)
{
    if (NULL == pstQueue)
    {
        return QUEUE_ERR;
    }

    uint8_t front = pstQueue->pstCb.u8Front;
    uint8_t back = pstQueue->pstCb.u8Back;

    return (front == back);
}

/**
 * @brief  Push a message in the message queue.
 * @param  pstQueue: point to the queue, @ref stQueueDef_t.
 * @param  pElement: Point to the element to be written.
 * @return queue state, @ref EN_QUEUE_ERR_STA_T.
 */
EN_QUEUE_ERR_STA_T msg_queue_push(stQueueDef_t *pstQueue, void const *pElement)
{
    if (NULL == pstQueue || NULL == pElement)
    {
        return QUEUE_ERR;
    }

    bool bIsFull = msg_queue_is_full(pstQueue);

    if (!bIsFull || (pstQueue->enMode == QUEUE_MODE_OVERFLOW))
    {
        // Get write position.
        uint8_t u8Pos = pstQueue->pstCb.u8Back;

        pstQueue->pstCb.u8Back = msg_queue_next_idx(pstQueue, pstQueue->pstCb.u8Back);

        if (bIsFull)
        {
            // Overwrite the oldest element.
            pstQueue->pstCb.u8Front = msg_queue_next_idx(pstQueue, pstQueue->pstCb.u8Front);
        }

        // Determining if there is a memory leak
        if ((u8Pos) * (pstQueue->u32BlockSize) > (pstQueue->u32BufSize))
        {
            return QUEUE_STA_ERR;
        }

        // Write a new element.
        memcpy((pstQueue->pu32Buffer + u8Pos * pstQueue->u32BlockSize), pElement, pstQueue->u32BlockSize);
    }
    else
    {
        return QUEUE_STA_BUSY;
    }

    return QUEUE_STA_OK;
}

/**
 * @brief  Pop up a message.
 * @param  pstQueue: point to the queue, @ref stQueueDef_t.
 * @param  pElement: Point to the element to be pop.
 * @return queue state, @ref EN_QUEUE_ERR_STA_T.
 */
EN_QUEUE_ERR_STA_T msg_queue_pop(stQueueDef_t *pstQueue, void *pElement)
{
    if (NULL == pstQueue || NULL == pElement)
    {
        return QUEUE_ERR;
    }

    if (!msg_queue_is_empty(pstQueue))
    {
        // Get read position.
        uint8_t u8Pos = pstQueue->pstCb.u8Front;

        // Update next read position.
        pstQueue->pstCb.u8Front = msg_queue_next_idx(pstQueue, pstQueue->pstCb.u8Front);

        // Read element.
        memcpy(pElement, (pstQueue->pu32Buffer + u8Pos * pstQueue->u32BlockSize), pstQueue->u32BlockSize);
    }
    else
    {
        return QUEUE_STA_ERR;
    }

    return QUEUE_STA_OK;
}

EN_QUEUE_ERR_STA_T msg_queue_peek(stQueueDef_t *pstQueue, void *pElement)
{
    if (NULL == pstQueue || NULL == pElement)
    {
        return QUEUE_ERR;
    }

    if (!msg_queue_is_empty(pstQueue))
    {
        // Get read position.
        uint8_t u8Pos = pstQueue->pstCb.u8Front;

        // Update next read position.
        //pstQueue->pstCb.u8Front = msg_queue_next_idx(pstQueue, pstQueue->pstCb.u8Front);

        // Read element.
        memcpy(pElement, (pstQueue->pu32Buffer + u8Pos * pstQueue->u32BlockSize), pstQueue->u32BlockSize);
    }
    else
    {
        return QUEUE_STA_ERR;
    }

    return QUEUE_STA_OK;
}

EN_QUEUE_ERR_STA_T msg_queue_clear_all(stQueueDef_t *pstQueue)
{
    if (NULL == pstQueue)
    {
        return QUEUE_ERR;
    }
    
    pstQueue->pstCb.u8Front = 0;
    pstQueue->pstCb.u8Back  = 0;
    return QUEUE_STA_OK;
}