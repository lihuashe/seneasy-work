/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     uart_driver.h
* @brief    
* @details
* @author   lihuashe
* @date     2024-12-18
* @version  v1.0
*********************************************************************************************************
*/
#ifndef _UART_H_
#define _UART_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "board.h"
#include "stdio.h"
#include <stdarg.h>
#include <string.h>
#include "app_msg.h"
#include <os_msg.h>
#include "app_section.h"

// 假设这些枚举和结构体已经定义
#define IO_MSG_SUBTYPE_PRODUCTION   0xfe

#define UART_RX_TRIGGER_LEVEL   32
#define UART_RX_TRIGGER_VALUE   32

#define UART_TX_PIN             P3_0
#define UART_RX_PIN             P3_1



#define DEBUG_MODE          1
#define PRINT_MODE          1


#if PRINT_MODE == 1

    #define PRINT(fmt, ...) \
        do { \
        char buffer[64]; /* 定义一个缓冲区来存储格式化字符串 */ \
        int len = snprintf(buffer, sizeof(buffer), fmt, ##__VA_ARGS__); \
        uart_senddata(UART1, (const uint8_t *)buffer, len); \
    } while(0)

    // 打印数组的宏
    #define PRINT_ARRAY(arr, size) \
    do { \
        PRINT("Array: "); \
        for (size_t i = 0; i < (size); i++) { \
            PRINT("%02X ", (arr)[i]); \
        } \
        PRINT("\n"); \
    } while (0)
#else
    
    #define PRINT(fmt, ...) // 在非调试模式下不打印
    #define PRINT_ARRAY(arr, size) // 在非调试模式下不打印数组

#endif

#define HEX_PRINT(buf, len) \
        do { \
        uart_senddata(UART1, (const uint8_t *)buf, len); \
    } while(0)

typedef struct 
{
    bool     rx_get;
    uint8_t  *rx_buf;
    uint16_t buf_len;
}UART_RX_BUF;


extern UART_RX_BUF g_uart_rx;

extern void uart_init(uint32_t baud_rate);

extern bool uart_send_msg_to_task(T_IO_MSG *p_msg);

extern void data_uart1_interrupt_handler(void) DATA_RAM_FUNCTION;

extern void uart_senddata(UART_TypeDef *UARTx, const uint8_t *pSend_Buf, uint16_t vCount) DATA_RAM_FUNCTION;
	  
extern int  uart_putchar(int ch) DATA_RAM_FUNCTION;

#ifdef __cplusplus
}
#endif

#endif

/******************* (C) COPYRIGHT 2020 Realtek Semiconductor Corporation *****END OF FILE****/
