/**
 * @file     debug.c
 * @author   Maxscend SoC Software Team
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

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "RegMXD2670.h"
#include "err_def.h"
#include "hw_crg.h"
#include "hw_gpio.h"
#include "hw_uart.h"

#include "app_cfg.h"
#include "debug.h"

/**
 * @brief  Local variables.
 */
static uint32_t m_u32DebugIoTx;
static uint32_t m_u32DebugUartBps;
static uint16_t m_u16DebugTxIdx;
static uint16_t m_u16DebugRxIdx;

static stUART_Handle_t *m_pstDebugUartHandle;
static stGPIO_Handle_t *m_pstDebugGpioHandle;
static stDebugBuf_t    *m_pstDebugBuf = NULL;

/**
 * @brief  App debug init.
 * @param  pstGPIO: GPIO handle, Should be PERI_GPIOA/GPIOB.
 * @param  enPin: The pin which would be set tx function.
 * @param  pstUART: UART handle, Should be PERI_UART0/PERI_UART1/PERI_UART2.
 * @param  u32BaudRate: Uart baudrate.
 * @param  pststDebugBuf: Point to app debug buf struct.
 * @return retuen status, @ref EN_HAL_STA_T.
 */
EN_ERR_STA_T debug_init(stGPIO_Handle_t *pstGPIO, uint32_t u32Pin, stUART_Handle_t *pstUART, uint32_t u32BaudRate,
                        stDebugBuf_t *pststDebugBuf)
{
    if ((!(IS_VALID_UART_HANDLE(pstUART))) || (!IS_VALID_GPIO_HANDLE(pstGPIO)))
    {
        return ERR_PARA_ERR;
    }

    m_pstDebugBuf = pststDebugBuf;

    if (NULL != m_pstDebugBuf)
    {
        m_u16DebugTxIdx = 0;
        m_u16DebugRxIdx = 0;
    }

    m_pstDebugUartHandle = pstUART;
    m_pstDebugGpioHandle = pstGPIO;
    m_u32DebugIoTx       = u32Pin;
    m_u32DebugUartBps    = (uint32_t)u32BaudRate;

    return debug_reinit();
}

/**
 * @brief  Set app debug io high impedance.
 */
void debug_deinit(void)
{
    rom_hw_gpio_set_pin_input_output(m_pstDebugGpioHandle, m_u32DebugIoTx, GPIO_MODE_IMPEDANCE);
    rom_hw_gpio_set_pin_pull_mode(m_pstDebugGpioHandle, m_u32DebugIoTx, GPIO_PULL_NONE);

    if (UART0 == m_pstDebugUartHandle)
    {
        rom_hw_crg_disable_clk_gate(CRG_UART0_CLK_GATE);
    }
    else if (UART1 == m_pstDebugUartHandle)
    {
        rom_hw_crg_disable_clk_gate(CRG_UART1_CLK_GATE);
    }
    else
    {
        rom_hw_crg_disable_clk_gate(CRG_UART2_CLK_GATE);
    }
}

/**
 * @brief  App debug reinit.
 * @return retuen status, @ref EN_HAL_STA_T.
 */
EN_ERR_STA_T debug_reinit(void)
{
    EN_ERR_STA_T enRet;
    stUartInit_t stUartInit;

    if (UART0 == m_pstDebugUartHandle)
    {
        rom_hw_crg_enable_clk_gate(CRG_UART0_CLK_GATE);
        enRet = rom_hw_gpio_set_pin_pid(m_pstDebugGpioHandle, m_u32DebugIoTx, PID_UART0_TXD);
    }
    else if (UART1 == m_pstDebugUartHandle)
    {
        rom_hw_crg_enable_clk_gate(CRG_UART1_CLK_GATE);
        enRet = rom_hw_gpio_set_pin_pid(m_pstDebugGpioHandle, m_u32DebugIoTx, PID_UART1_TXD);
    }
    else
    {
        rom_hw_crg_enable_clk_gate(CRG_UART2_CLK_GATE);
        enRet = rom_hw_gpio_set_pin_pid(m_pstDebugGpioHandle, m_u32DebugIoTx, PID_UART2_TXD);
    }
    ERR_RETURN_IF_ERROR(enRet);

    enRet = rom_hw_gpio_set_pin_pull_mode(m_pstDebugGpioHandle, m_u32DebugIoTx, GPIO_PULL_UP);
    ERR_RETURN_IF_ERROR(enRet);

    stUartInit.u32UartBaudRate = m_u32DebugUartBps;

    // TxFifoThld and RxFifoThld is 8, StopBit is 1, 8bit, no parity, little-ending
    stUartInit.unUartCfg.u32UartCfg = UART_INIT_DEFAULT(UART_PARITY_NONE);

    return rom_hw_uart_init(m_pstDebugUartHandle, &stUartInit);
}

/**
 * @brief  Check app debug is print done.
 * @return false(0) - app debug is working.
 * @return true(1)  - app debug is idle.
 */
bool debug_print_complete(void)
{
    if (NULL == m_pstDebugBuf)
    {
        return true;
    }

    return (m_u16DebugTxIdx == m_u16DebugRxIdx);
}

/**
 * @brief  Print all data by uart
 */
void debug_printf(void)
{
    if (NULL == m_pstDebugBuf)
    {
        return;
    }

    while (m_u16DebugTxIdx != m_u16DebugRxIdx)
    {
        rom_hw_uart_send_byte(m_pstDebugUartHandle, m_pstDebugBuf->pu8buf[m_u16DebugTxIdx]);

        m_u16DebugTxIdx++;
        if (m_u16DebugTxIdx >= m_pstDebugBuf->u16MaxLen)
        {
            m_u16DebugTxIdx = 0;
        }
    }
}

/**
 * @brief  Print hex string.
 * @param  pPreStr: Preamble string.
 * @param  pu8Buf: Data buffer.
 * @param  u16BufLen: Buffer length.
 */
void debug_printf_hex_string(char *pPreStr, const uint8_t *pu8Buf, uint16_t u16BufLen)
{
    uint16_t i;

    PRINTF("[%s][%d]:", pPreStr, u16BufLen);

    for (i = 0; i < u16BufLen; i++)
    {
        PRINTF(" %02X", pu8Buf[i]);
    }

    PRINTF("\n");
}

/**
 * @brief
 * @param  pre
 * @param  buff
 * @param  len
 */
void app_printf_dec(char *pre, const uint8_t *buff, uint16_t len)
{
    printf("[%s][%d]:", pre, len);
    for (int i = 0; i < len; i++)
    {
        printf(" %d", buff[i]);
    }
    printf("\n");
}

/**
 * @brief  Print char
 * @param  pre
 * @param  buff
 * @param  len
 */
void app_printf_char(char *pre, const char *buff, uint16_t len)
{
    printf("[%s][%d]:", pre, len);
    for (int i = 0; i < len; i++)
    {
        printf("%c", buff[i]);
    }
    printf("\n");
}

/**
 * @brief  printf function
 * @param  c
 * @param  f
 * @return c
 */
int fputc(int c, FILE *f)
{
    if (NULL == m_pstDebugBuf)
    {
        uint8_t data = c & 0xFF;

        // rom_hw_uart_transmit( m_pstDebugUartHandle, &data, 1 );
        rom_hw_uart_send_byte(m_pstDebugUartHandle, c & 0xFF);
    }
    else
    {
        m_pstDebugBuf->pu8buf[m_u16DebugRxIdx] = c;

        m_u16DebugRxIdx++;
        if (m_u16DebugRxIdx >= m_pstDebugBuf->u16MaxLen)
        {
            m_u16DebugRxIdx = 0;
        }
    }

    return c;
}
