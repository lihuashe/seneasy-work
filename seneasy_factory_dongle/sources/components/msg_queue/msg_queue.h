/**********************************************************************************************************************
 * @file     msg_queue.h
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

#ifndef __MSG_QUEUE_H__
#define __MSG_QUEUE_H__


//=====================================================================================================================
// TYPEDEF
//=====================================================================================================================
typedef void *pMsgQueueId_t;


typedef enum
{
    QUEUE_MODE_OVERFLOW,       //!< If the queue is full, new element will overwrite the oldest.
    QUEUE_MODE_NO_OVERFLOW,    //!< If the queue is full, new element will not be accepted.
    QUEUE_MODE_NONE

} EN_QUEUE_MODE_T;

typedef enum
{
    QUEUE_ERR_NONE,
    QUEUE_ERR,
    QUEUE_STA_OK,
    QUEUE_STA_ERR,
    QUEUE_STA_BUSY,

} EN_QUEUE_ERR_STA_T;

typedef struct
{
    uint8_t u8Front;          //!< Queue front index.
    uint8_t u8Back;           //!< Queue back index.
    uint8_t u8Utilization;    //!< utilization of the queue.

} stQueueCb_t;


typedef struct
{
    stQueueCb_t     pstCb;           //!< Pointer to the instance control block.
    uint8_t        *pu32Buffer;      //!< Pointer to the memory that is used as storage.
    uint32_t        u32BufSize;      //!< Size of the queue.
    uint32_t        u32MsgCount;
    uint32_t        u32BlockSize;    //!< Size of one element.
    EN_QUEUE_MODE_T enMode;          //!< Mode of the queue.

} stQueueDef_t;




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
extern pMsgQueueId_t msg_queue_create(stQueueDef_t *pstQueue, uint8_t *pstQueueBuf, uint32_t u32BufSize,
                                      uint32_t u32MsgCount, uint32_t u32MsgSize, EN_QUEUE_MODE_T enMode);


/**
 * @brief  Determine if the message queue is full.
 * @param  pstQueue: point to the queue, @ref stQueueDef_t.
 * @return true or false.
 */
extern bool msg_queue_is_full(stQueueDef_t const *pstQueue);


/**
 * @brief  Determine if the message queue is full.
 * @param  pstQueue: point to the queue, @ref stQueueDef_t.
 * @return true or false.
 */
extern bool msg_queue_is_empty(stQueueDef_t const *pstQueue);


/**
 * @brief  Push a message in the message queue.
 * @param  pstQueue: point to the queue, @ref stQueueDef_t.
 * @param  pElement: Point to the element to be written.
 * @return queue state, @ref EN_QUEUE_ERR_STA_T.
 */
extern EN_QUEUE_ERR_STA_T msg_queue_push(stQueueDef_t *pstQueue, void const *pElement);


/**
 * @brief  Pop up a message.
 * @param  pstQueue: point to the queue, @ref stQueueDef_t.
 * @param  pElement: Point to the element to be pop.
 * @return queue state, @ref EN_QUEUE_ERR_STA_T.
 */
extern EN_QUEUE_ERR_STA_T msg_queue_pop(stQueueDef_t *pstQueue, void *pElement);

EN_QUEUE_ERR_STA_T msg_queue_peek(stQueueDef_t *pstQueue, void *pElement);

EN_QUEUE_ERR_STA_T msg_queue_clear_all(stQueueDef_t *pstQueue);


#endif /* __MSG_QUEUE_H__ */

