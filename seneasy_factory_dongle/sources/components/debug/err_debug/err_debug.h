/**
 * @file     err_debug.h
 * @version  V1.0
 * @date     2020/12/25
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

#ifndef __ERR_DEBUG_H__
#define __ERR_DEBUG_H__

/**
 * @brief  os_error_handler
 * @param  u32Code
 * @param  pObjectId: pointer to the object
 * @return uint32_t
 */
extern uint32_t os_error_handler(uint32_t u32Code, void *pObjectId);

/**
 * @brief  system_assert_debug
 *         Show the error of system assert
 * @param  adr - it defines in mem.h
 */
extern void system_assert_debug(uint16_t adr, uint32_t info);

extern void system_error_handler(uint16_t u16ErrModule, uint16_t u16ErrLine, uint32_t u32ErrInfo1,
                                 uint32_t u32ErrInfo2);

extern void err_debug_init(void);

#endif /* __ERR_DEBUG_H__ */
