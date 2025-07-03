/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      uart_transport.h
* @brief
* @details
* @author    chenjie jin
* @date      2018-04-08
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef __UART_TRANSPORT_H
#define __UART_TRANSPORT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include <stdint.h>
#include <stdbool.h>
#include "trace.h"

/* Defines ------------------------------------------------------------------*/

/* Enable print log or not */
#define UART_PRINT_LOG

/* Configure loop queue parameters */
#define UART_LOOP_QUEUE_MAX_SIZE             512

/* Configure UART packet buffer length */
#define CMD_SIZE                        50

#ifdef UART_PRINT_LOG
#define UART_DBG_BUFFER(MODULE, LEVEL, fmt, para_num,...) DBG_BUFFER_##LEVEL(TYPE_BEE2, SUBTYPE_FORMAT, MODULE, fmt, para_num, ##__VA_ARGS__)
#else
#define UART_DBG_BUFFER(MODULE, LEVEL, fmt, para_num,...) ((void)0)
#endif

/* Packet decode status */
typedef enum
{
    WaitHeader,
    WaitCMD,
    WaitParams,
    WaitCRC,
} T_UART_WAIT_STATE;

/* UART packet data structure */
typedef struct t_uart_packet_def
{
    uint8_t     uart_buf[CMD_SIZE];  /* command buffer */
    uint16_t    buf_index;       /* index of buffer */
    uint16_t    payload_len;     /* length of decoder payload */
    uint16_t    crc_len;         /* index of CRC */
    T_UART_WAIT_STATE   uart_status;         /* status of decoding */

} T_UART_PACKET_DEF;

void data_uart_init(uint8_t buadrate_opt);
void data_uart_transport_init(void);
bool data_uart_packet_decode(T_UART_PACKET_DEF *p_packet);
void data_uart_cmd_response(uint16_t opcode, uint8_t status, uint8_t *p_payload,
                            uint32_t payload_length);

#ifdef __cplusplus
}
#endif

#endif /*__UART_TRANSPORT_H*/

/******************* (C) COPYRIGHT 2017 Realtek Semiconductor Corporation *****END OF FILE****/

