/**********************************************************************************************************************
 * @file     fw_upgrade_thread.h
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
 * Copyright (c) 2016~2021, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 *********************************************************************************************************************/


#ifndef __FW_UPGRADE_THREAD_H__
#define __FW_UPGRADE_THREAD_H__


//=====================================================================================================================
// DEFINE
//=====================================================================================================================
#define FW_UPGRADE_MSG_COUNT               ( 4 )
#define FW_UPGRADE_MSG_DATA_LEN            ( 256 ) // Data length of each message.


//=====================================================================================================================
// TYPEDEF
//=====================================================================================================================
typedef struct __attribute__((packed))
{
    uint16_t u16DataLen;
    uint8_t  au8Data[FW_UPGRADE_MSG_DATA_LEN];

} stFwUpgradeThread_t;




/**
 * @brief  Firmware upgrade task init.
 * @return status: @ref EN_ERR_STA_T.
 */
extern uint8_t fw_upgrade_thread_init(void);


/**
 * @brief  Send message to queue.
 * @param  pu8Buf: Pointer to data buffer.
 * @param  u16Len: Buffer length.
 * @return status: @ref EN_ERR_STA_T.
 */
extern uint8_t fw_upgrade_thread_send_msg_to_queue(uint8_t *pu8Buf, uint16_t u16Len);


#endif /* __FW_UPGRADE_THREAD_H__ */

