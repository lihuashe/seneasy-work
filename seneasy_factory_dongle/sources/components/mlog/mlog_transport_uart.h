/**
 * @file     mlog_transport_uart.h
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

#ifndef __MLOG_TRANSPORT_UART_H__
#define __MLOG_TRANSPORT_UART_H__

extern bool mlog_transport_uart_init(stUART_Handle_t *pstUART, stGPIO_Handle_t *pstGPIO, EN_GPIO_PIN_T enumPin,
                                     uint32_t u32BaudRate);

extern bool mlog_transport_uart_reinit(void);

extern void mlog_transport_uart_deinit(void);

extern bool mlog_transport_uart_reinit(void);

extern void mlog_transport_uart_print(void);

#endif /* __MLOG_TRANSPORT_UART_H__ */
