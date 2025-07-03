/**********************************************************************************************************************
 * @file     ipc_general_queue.h
 * @version  V1.0
 * @date     2022/12/28
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

#ifndef __IPC_GENERAL_QUEUE_H__
#define __IPC_GENERAL_QUEUE_H__


//=====================================================================================================================
// DEFINE
//=====================================================================================================================
#define IPC_MSG_COUNT                      ( 2 )
#define IPC_MSG_DATA_SIZE                  ( 512 )

#define IpcMessageBufferMemSize(msgCount, msgSize)  (4*(msgCount)*(3+(((msgSize)+3)/4)))


//=====================================================================================================================
// TYPEDEF
//=====================================================================================================================
typedef struct __attribute__((aligned(4)))
{
    uint32_t  u32DataLen;
    uint8_t  *pu8Data;

} stIpcMsg_t;


// Receive message callback function.
typedef uint8_t (*ipc_rx_msg_cb_func)(uint8_t *pu8Data, uint32_t u32DataLen);


typedef struct __attribute__((aligned(4)))
{
    uint8_t  *pu8Buf;
    uint32_t  u32BufSize;
    uint32_t  u32MsgCount;
    uint32_t  u32MsgSize;

    EN_QUEUE_MODE_T enMode;
    ipc_rx_msg_cb_func pfnCallback;

} stIpcInit_t;




/**
 * @brief  General ipc Initialization.
 * @param  pstIpcInit: Pointer to ipc init struct.
 * @return status: @ref EN_ERR_STA_T.
 */
extern uint8_t ipc_general_init(stIpcInit_t *pstIpcInit);


/**
 * @brief  Send ipc message by blocking. This API can not be called from Interrupt Service Routines (ISR).
 * @param  pu8Data: Pointer to the data buffer which will be sent.
 * @param  u32DataLen: Data length.
 * @return status: @ref EN_ERR_STA_T.
 */
extern uint8_t ipc_general_send_msg_blocking(uint8_t *pu8Data, uint32_t u32DataLen);


/**
 * @brief  IPC Send Message by Non-Blocking. This API Can Call in IRQ Handler.
 * @param  pu8Data: Data of Message.
 * @param  u32DataLen: Data Length of Message.
 * @return HW status: @ref EN_ERR_STA_T.
 */
extern EN_ERR_STA_T ipc_general_send_msg_nonblocking(uint8_t *pu8Data, uint32_t u32DataLen);




#endif /* __IPC_GENERAL_QUEUE_H__ */

