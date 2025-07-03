/**
 * @file     mlog_transport_uart.c
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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "RegMXD2670.h"
#include "err_def.h"
#include "hw_gpio.h"
#include "hw_uart.h"
#include "hw_crg.h"

#include "mlog_protocol.h"
#include "mlog_transport_uart.h"

static void mlog_transport_uart_protocol_print_impl(uint8_t *pu8Buffer, uint16_t u16Length);

static EN_GPIO_PIN_T    m_enumLogIoTx;
static uint32_t         m_u32LogUartBps;
static stUART_Handle_t *m_pstLogUartHandle;
static stGPIO_Handle_t *m_pstLogGpioHandle;

bool mlog_transport_uart_init(stUART_Handle_t *pstUART, stGPIO_Handle_t *pstGPIO, EN_GPIO_PIN_T enumPin,
                              uint32_t u32BaudRate)
{
    if ((!(IS_VALID_UART_HANDLE(pstUART))))
    {
        return false;
    }

    m_enumLogIoTx      = enumPin;
    m_u32LogUartBps    = (uint32_t)u32BaudRate;
    m_pstLogUartHandle = pstUART;
    m_pstLogGpioHandle = pstGPIO;

    return mlog_transport_uart_reinit();
}

void mlog_transport_uart_deinit(void)
{
    mlog_protocol_print_callback_register(NULL);

    rom_hw_gpio_set_pin_input_output(m_pstLogGpioHandle, m_enumLogIoTx, GPIO_MODE_IMPEDANCE);

    if (UART0 == m_pstLogUartHandle)
    {
        rom_hw_crg_disable_clk_gate(CRG_UART0_CLK_GATE);
    }
    else
    {
        rom_hw_crg_disable_clk_gate(CRG_UART1_CLK_GATE);
    }
}

bool mlog_transport_uart_reinit(void)
{
    EN_ERR_STA_T enRet;
    stUartInit_t stUartInit;

    if (UART0 == m_pstLogUartHandle)
    {
        rom_hw_crg_enable_clk_gate(CRG_UART0_CLK_GATE);
        enRet = rom_hw_gpio_set_pin_pid(m_pstLogGpioHandle, m_enumLogIoTx, PID_UART0_TXD);
    }
    else
    {
        rom_hw_crg_enable_clk_gate(CRG_UART1_CLK_GATE);
        enRet = rom_hw_gpio_set_pin_pid(m_pstLogGpioHandle, m_enumLogIoTx, PID_UART1_TXD);
    }
    ERR_RETURN_IF_ERROR(enRet);

    enRet = rom_hw_gpio_set_pin_pull_mode(m_pstLogGpioHandle, m_enumLogIoTx, GPIO_PULL_UP);
    ERR_RETURN_IF_ERROR(enRet);

    stUartInit.u32UartBaudRate = m_u32LogUartBps;

    // TxFifoThld and RxFifoThld is 8, StopBit is 1, 8bit, no parity, little-ending
    stUartInit.unUartCfg.u32UartCfg = UART_INIT_DEFAULT(UART_PARITY_NONE);

    enRet = rom_hw_uart_init(m_pstLogUartHandle, &stUartInit);
    if (ERR_STA_OK == enRet)
    {
        mlog_protocol_print_callback_register(mlog_transport_uart_protocol_print_impl);
        return true;
    }
    else
    {
        return false;
    }
}

void mlog_transport_uart_print(void)
{
    mlog_protocol_print();
}

static void mlog_transport_uart_protocol_print_impl(uint8_t *pu8Buffer, uint16_t u16Length)
{
    rom_hw_uart_transmit(m_pstLogUartHandle, pu8Buffer, u16Length);
}
