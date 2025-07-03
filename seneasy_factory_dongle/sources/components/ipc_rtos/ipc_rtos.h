/**********************************************************************************************************************
 * @file     ipc_rtos.h
 * @version  V1.0
 * @date     2021/03/17
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

#ifndef __IPC_RTOS_H__
#define __IPC_RTOS_H__


//=====================================================================================================================
// DEFINE
//=====================================================================================================================
#define IPC_MSG_COUNT                      ( 16 )
#define IPC_MSG_DATA_LEN                   ( 32 )

#if __MAIN_PROCESSOR
    #define IPC_MSG_EXTRA_DATA_COUNT       ( 8 )
    #define IPC_MSG_EXTRA_DATA_LEN         ( 144 )

    #define IPC_RX_TASK_PRIORITY           ( osPriorityNormal )
    #define IPC_RX_TASK_STACK_SIZE         ( 1024 )
#else
    #define IPC_MSG_EXTRA_DATA_COUNT       ( 4 )
    #define IPC_MSG_EXTRA_DATA_LEN         ( 144 )

    #define IPC_RX_TASK_PRIORITY           ( osPriorityNormal )
    #define IPC_RX_TASK_STACK_SIZE         ( 1024 )
#endif


//=====================================================================================================================
// TYPEDEF
//=====================================================================================================================
typedef struct
{
    uint32_t  u32Cmd;
    uint8_t  *pu8Data;
    uint32_t  u32DataLen;
    uint8_t  *pu8ExtraData;
    uint32_t  u32ExtraDataLen;

} stIpcMsg_t __attribute__((aligned(4)));


typedef void (*ipc_rx_msg_cb_func)(stIpcMsg_t *pstIpcMsg);




/**
 * @brief  IPC Send Message by Non-Blocking
 *         This API Can Call in IRQ Handler.
 * @param  u32Cmd: Command of Message
 * @param  pu8Data: Data of Message
 * @param  u32DataLen: Data Length of Message
 * @param  pu8ExtraData: Extra Data of Message
 * @param  u32ExtraDataLen: Extra Data Length of Message
 * @return HW status: @ref EN_ERR_STA_T 
 */
extern EN_ERR_STA_T ipc_rtos_send_msg_nonblocking(uint32_t u32Cmd, uint8_t* pu8Data, uint32_t u32DataLen, 
                                             uint8_t* pu8ExtraData, uint32_t u32ExtraDataLen);


/**
 * @brief  IPC Send Message by Blocking
 *         This API Can Not Call in IRQ Handler.
 * @param  u32Cmd: Command of Message
 * @param  pu8Data: Data of Message
 * @param  u32DataLen: Data Length of Message
 * @param  pu8ExtraData: Extra Data of Message
 * @param  u32ExtraDataLen: Extra Data Length of Message
 * @return HW status: @ref EN_ERR_STA_T
 */
extern EN_ERR_STA_T ipc_rtos_send_msg_blocking(uint32_t u32Cmd, uint8_t* pu8Data, uint32_t u32DataLen, 
                                          uint8_t* pu8ExtraData, uint32_t u32ExtraDataLen);


/**
 * @brief  Inter-Processor Communication(IPC) Initialization.
 * @param  pfnCallback: IPC receive callback function.
 * @return HW status: @ref EN_ERR_STA_T
 */
extern EN_ERR_STA_T ipc_rtos_init(ipc_rx_msg_cb_func pfnCallback);


#endif /* __IPC_RTOS_H__ */


