/**
 * @file     debug.h
 * @version  V1.0
 * @date     2020/05/07
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

#ifndef __DEBUG_H__
#define __DEBUG_H__

/**
 * @brief  TYPEDEF STRUCT
 */
typedef struct
{
    uint8_t *pu8buf;
    uint16_t u16MaxLen;

} stDebugBuf_t;

/**
 * @brief  App debug init.
 * @param  pstGPIO: GPIO handle, Should be GPIOA/GPIOB.
 * @param  enPin: The pin which would be set tx function.
 * @param  pstUART: UART handle, Should be PERI_UART0/PERI_UART1/PERI_UART2.
 * @param  u32BaudRate: Uart baudrate.
 * @param  pststDebugBuf: Point to app debug buf struct.
 * @return retuen status, @ref EN_HAL_STA_T.
 */
extern EN_ERR_STA_T debug_init(stGPIO_Handle_t *pstGPIO, uint32_t u32Pin, stUART_Handle_t *pstUART,
                               uint32_t u32BaudRate, stDebugBuf_t *pststDebugBuf);

/**
 * @brief  Set app debug io high impedance
 */
extern void debug_deinit(void);

/**
 * @brief  App debug reinit
 * @return retuen status, @ref EN_HAL_STA_T
 */
extern EN_ERR_STA_T debug_reinit(void);

/**
 * @brief  Check app debug is print done
 * @return status: false(0) - app debug is working.
 *                 true(1)  - app debug is idle.
 */
extern bool debug_print_complete(void);

/**
 * @brief  Print all data by uart.
 */
extern void debug_printf(void);

/**
 * @brief  Print hex string.
 * @param  pPreStr: Preamble string.
 * @param  pu8Buf: Data buffer.
 * @param  u16BufLen: Buffer length.
 */
extern void debug_printf_hex_string(char *pPreStr, const uint8_t *pu8Buf, uint16_t u16BufLen);

#endif /* __DEBUG_H__ */
