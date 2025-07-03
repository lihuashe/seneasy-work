/**
 * @file     mlog_protocol.h
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

#ifndef __MLOG_PROTOCOL_H__
#define __MLOG_PROTOCOL_H__

#define MLOG_PROTOCOL_BUFFER_SIZE (1024)

typedef void (*callback_mlog_protocol_print)(uint8_t *pu8Buffer, uint16_t u16Length);

extern void mlog_protocol_print_callback_register(callback_mlog_protocol_print callback);

extern void mlog_protocol_print(void);

#endif /* __MLOG_PROTOCOL_H__ */
