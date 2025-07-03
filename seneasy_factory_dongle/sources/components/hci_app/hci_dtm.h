/**
 * @file     hci_dtm.h
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2023/7/11
 * @history
 * @note
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2023, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 */

#ifndef __HCI_DTM_H__
#define __HCI_DTM_H__

/**
 * @brief  Hci dtm cmd handler
 * @param  u16Opcode: op code.
 * @param  pu8Buf: payload data buffer
 * @param  u16Len: payload data buffer length
 * @param  pstEvent: event struct point
 */
extern void hci_dtm_cmd_handler(uint16_t u16Opcode, uint8_t *pu8Buf, uint16_t u16Len, stEvent_t *pstEvent);

#endif /* __HCI_DTM_H__ */
