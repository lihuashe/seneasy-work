/**********************************************************************************************************************
 * @file     hw_uart.c
 * @author   Maxscend SoC Software Team
 * @version  V1.1
 * @date     2022/1/13
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

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "RegMXD2670.h"
#include "err_def.h"
#include "hw_pmu.h"

#include "type_def.h"
#include "utility.h"

#include "hw_uart.h"


//=====================================================================================================================
// DEFINE
//=====================================================================================================================
#define UART_MAX_TIMEOUT                   (10 * 1000) // unit us




/**
 * @brief  switch uart baudrate to divisor value.
 * @param  u32BaudRate: Baudrate.
 * @return divisor value corresponding input baudrate.
 */
uint32_t rom_hw_uart_get_divisor(uint32_t u32BaudRate)
{
    uint16_t u16CapIntv = 0;
    uint16_t u16TrigIntv = 0;

    switch (u32BaudRate)
    {
        case UART_BAUDRATE_2400:
            return UART_DIVISOR_2400;

        case UART_BAUDRATE_9600:
            return UART_DIVISOR_9600;

        case UART_BAUDRATE_14400:
            return UART_DIVISOR_14400;

        case UART_BAUDRATE_19200:
            return UART_DIVISOR_19200;

        case UART_BAUDRATE_38400:
            return UART_DIVISOR_38400;

        case UART_BAUDRATE_57600:
            return UART_DIVISOR_57600;

        case UART_BAUDRATE_115200:
            return UART_DIVISOR_115200;

        case UART_BAUDRATE_256000:
            return UART_DIVISOR_256000;

        case UART_BAUDRATE_921600:
            return UART_DIVISOR_921600;

        case UART_BAUDRATE_2000000:
            return UART_DIVISOR_2000000;

        case UART_BAUDRATE_1000000:
            return UART_DIVISOR_1000000;

        case UART_BAUDRATE_1500000:
            return UART_DIVISOR_1500000;

        case UART_BAUDRATE_1600000:
            return UART_DIVISOR_1600000;

        default:
        {
            u16CapIntv = SYS_CLOCK_16M_HZ / u32BaudRate - 1;
            u16TrigIntv = (u16CapIntv + 1) / 7;

            return (((u16CapIntv & UART_CAP_INTV_MSK) << UART_CAP_INTV_SHIFT) |
                    ((u16TrigIntv & UART_TRIG_INTVAL_MSK) << UART_TRIG_INTVAL_SHIFT));
        }
    }
}

/**
 * @brief  DeInitializes the UART peripheral.
 * @param  pstUART: UART handle, Should be UART0 / UART1 / UART2.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_uart_deinit(stUART_Handle_t *pstUART)
{
    if (!(IS_VALID_UART_HANDLE(pstUART)))
    {
        return ERR_PARA_ERR;
    }

    pstUART->UART_CFG = UART_CONFIG_DEFAULT;
    pstUART->UART_INT_EN = 0x00000000;
    pstUART->UART_INT_CLR = 0xFFFFFFFF;
    pstUART->UART_CLK_CFG = UART_CLK_CFG_DEFAULT;
    pstUART->UART_RXFIFO_CLR = UART_RXFIFO_CLR_MSK << UART_RXFIFO_CLR_SHIFT;
    pstUART->UART_TXFIFO_CLR = UART_TXFIFO_CLR_MSK << UART_TXFIFO_CLR_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  Initializes the UART mode according to the specified parameters in
 *         the stUartInit_t and create the associated handle.
 * @param  pstUART: UART handle, Should be UART0 / UART1 / UART2.
 * @param  stUartInit_t: Pointer to a stUartInit_t structure that contains the initial structure.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_uart_init(stUART_Handle_t *pstUART, stUartInit_t *pstInitType)
{
    uint32_t baud = pstInitType->u32UartBaudRate;

    if ((!(IS_VALID_UART_HANDLE(pstUART))) || (NULL == pstInitType))
    {
        return ERR_PARA_ERR;
    }

    if (0 == pstInitType->u32UartBaudRate)
    {
        return ERR_PARA_ERR;
    }

    /* Config uart txFifo thd, txFifo thd, rxtimeout, stopbit, byte size, parity, bit little endian */
    pstUART->UART_CFG = pstInitType->unUartCfg.u32UartCfg;

    return rom_hw_uart_set_baudrate(pstUART, pstInitType->u32UartBaudRate);
}

/**
 * @brief  Set uart baudrate.
 * @param  pstUART: UART handle, Should be UART0 / UART1 / UART2.
 * @param  u32BaudRate: baudrate.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_uart_set_baudrate(stUART_Handle_t *pstUART, uint32_t u32BaudRate)
{
    if ((!(IS_VALID_UART_HANDLE(pstUART))) || (0 == u32BaudRate))
    {
        return ERR_PARA_ERR;
    }

    /* set baudrate devisor value the value = (Mainclock / baudrate) -1 baud_interval trig_interval */
    pstUART->UART_CLK_CFG = rom_hw_uart_get_divisor(u32BaudRate);

    return ERR_STA_OK;
}

/**
 * @brief  Set UART DATA BITS.
 *         Databit is upward compatible, high data bits can correctly receive data with low data bits,
 *         databit5 maximum is 31, databit6 maximum is 63,
 *         databit7 maximum is 127, databit8 maximum is 255.
 * @param  pstUART: UART handle, Should be UART0 / UART1 / UART2.
 * @param  enSize: UART data size, @ref EN_UART_DATA_SIZE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_uart_set_data_sizes(stUART_Handle_t *pstUART, EN_UART_DATA_SIZE_T enSize)
{
    uint32_t u32Cfg = 0;

    if ((!(IS_VALID_UART_HANDLE(pstUART))) || (!IS_UART_DATA_BITS_VALID(enSize)))
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = pstUART->UART_CFG;

    u32Cfg &= ~(UART_DATA_SIZE_MSK << UART_DATA_SIZE_SHIFT);
    u32Cfg |= (enSize & UART_DATA_SIZE_MSK) << UART_DATA_SIZE_SHIFT;

    pstUART->UART_CFG = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Set UART stop bit.
 * @param  pstUART: UART handle, Should be UART0 / UART1 / UART2.
 * @param  enStopbits: UART stop bit, @ref EN_UART_STOP_SIZE_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_uart_set_stop_sizes(stUART_Handle_t *pstUART, EN_UART_STOP_SIZE_T enStopbits)
{
    uint32_t u32Cfg = 0;

    if ((!(IS_VALID_UART_HANDLE(pstUART))) || (!IS_UART_STOP_BITS_VALID(enStopbits)))
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = pstUART->UART_CFG;

    u32Cfg &= ~(UART_STOP_SIZE_MSK << UART_STOP_SIZE_SHIFT);
    u32Cfg |= (enStopbits & UART_STOP_SIZE_MSK) << UART_STOP_SIZE_SHIFT;

    pstUART->UART_CFG = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Set UART parity.
 * @param  pstUART: UART handle, Should be UART0 / UART1 / UART2.
 * @param  enParity: UART parity, @ref EN_UART_PARITY_BITS_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_uart_set_parity(stUART_Handle_t *pstUART, EN_UART_PARITY_BITS_T enParity)
{
    uint32_t u32Cfg = 0;

    if ((!(IS_VALID_UART_HANDLE(pstUART))) || (!IS_VALID_UART_PARITY(enParity)))
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = pstUART->UART_CFG;

    u32Cfg &= ~(UART_PARITY_MODE_MSK << UART_PARITY_MODE_SHIFT);
    u32Cfg |= (enParity & UART_PARITY_MODE_MSK) << UART_PARITY_MODE_SHIFT;

    pstUART->UART_CFG = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Set UART endian mode.
 * @param  pstUART: UART handle, Should be UART0 / UART1 / UART2.
 * @param  enMode: UART endian mode, @ref EN_UART_ENDIAN_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_uart_set_mode(stUART_Handle_t *pstUART, EN_UART_ENDIAN_T enMode)
{
    uint32_t u32Cfg = 0;

    if ((!(IS_VALID_UART_HANDLE(pstUART))) || (!IS_VALID_UART_ENDIAN(enMode)))
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = pstUART->UART_CFG;

    u32Cfg &= ~(UART_ENDIAN_MODE_MSK << UART_ENDIAN_MODE_SHIFT);
    u32Cfg |= (enMode & UART_ENDIAN_MODE_MSK) << UART_ENDIAN_MODE_SHIFT;

    pstUART->UART_CFG = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Set Tx FIFO under threshold level, When TX Fifo is under this threshold, it trigger a interrupt.
 * @param  pstUART: UART handle, Should be UART0 / UART1 / UART2.
 * @param  u8Thld: Threshold for UART TX FIFO.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_uart_set_txfifo_thld(stUART_Handle_t *pstUART, uint8_t u8Thld)
{
    uint32_t u32Cfg = 0;

    if (!(IS_VALID_UART_HANDLE(pstUART)) || (0 == u8Thld))
    {
        return ERR_PARA_ERR;
    }

    if (u8Thld > UART_TXFIFO_THLD_MAX)
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = pstUART->UART_CFG;

    u32Cfg &= ~(UART_TX_FIFO_THLD_MSK << UART_TX_FIFO_THLD_SHIFT);
    u32Cfg |= (u8Thld & UART_TX_FIFO_THLD_MSK) << UART_TX_FIFO_THLD_SHIFT;

    pstUART->UART_CFG = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Set rx FIFO over threshold level, When RX Fifo is over the threshold, it trigger a interrupt.
 * @param  pstUART: UART handle, Should be UART0 / UART1 / UART2.
 * @param  u8Thld: Threshold for UART RX FIFO.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_uart_set_rxfifo_thld(stUART_Handle_t *pstUART, uint8_t u8Thld)
{
    uint32_t u32Cfg = 0;

    if (!(IS_VALID_UART_HANDLE(pstUART)) || (0 == u8Thld))
    {
        return ERR_PARA_ERR;
    }

    if (u8Thld > UART_TXFIFO_THLD_MAX)
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = pstUART->UART_CFG;

    u32Cfg &= ~(UART_RX_FIFO_THLD_MSK << UART_RX_FIFO_THLD_SHIFT);
    u32Cfg |= (u8Thld & UART_RX_FIFO_THLD_MSK) << UART_RX_FIFO_THLD_SHIFT;

    pstUART->UART_CFG = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Uart config the send line break function.
 * @param  pstUART: UART handle, Should be UART0 / UART1 / UART2.
 * @param  enEn: Enalbe or disable RX line break function.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_uart_line_break_enable(stUART_Handle_t *pstUART, EN_UART_LB_EN_T enEn)
{
    uint32_t u32Cfg = 0;

    if (!(IS_VALID_UART_HANDLE(pstUART)) || (!IS_VALID_UART_LINEBREAK_EN(enEn)))
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = pstUART->UART_CFG;

    u32Cfg &= ~(UART_LINE_BREAK_EN_MSK << UART_LINE_BREAK_EN_SHIFT);
    u32Cfg |= (enEn & UART_LINE_BREAK_EN_MSK) << UART_LINE_BREAK_EN_SHIFT;

    pstUART->UART_CFG = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Set uart rx timeout.
 * @param  pstUART: UART handle, Should be UART0 / UART1 / UART2.
 * @param  u8SymbolNum: Time out value, unit: ms.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_uart_set_rx_timeout(stUART_Handle_t *pstUART, uint8_t u8SymbolNum)
{
    if (!(IS_VALID_UART_HANDLE(pstUART)))
    {
        return ERR_PARA_ERR;
    }

    pstUART->UART_TIMEOUT = (u8SymbolNum & UART_RX_TIMEOUT_THLD_MSK) << UART_RX_TIMEOUT_THLD_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  Uart config flow function.
 * @param  pstUART: UART handle, Should be UART0 / UART1 / UART2.
 * @param  enEn: Enalbe or disable flow function, @ref EN_UART_FLOW_EN_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_uart_flow_enable(stUART_Handle_t *pstUART, EN_UART_FLOW_EN_T enEn)
{
    uint32_t u32Cfg = 0;

    if (!(IS_VALID_UART_HANDLE(pstUART)) || (!IS_VALID_UART_FLOW_EN(enEn)))
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = pstUART->UART_CFG;

    u32Cfg &= ~(UART_FLOW_CTRL_EN_MSK << UART_FLOW_CTRL_EN_SHIFT);
    u32Cfg |= (enEn & UART_FLOW_CTRL_EN_MSK) << UART_FLOW_CTRL_EN_SHIFT;

    pstUART->UART_CFG = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  Set rts threshold.
 * @param  pstUART: UART handle, Should be UART0 / UART1 / UART2.
 * @param  u8Thld: Threshold for UART RTS.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_uart_set_rts_thld(stUART_Handle_t *pstUART, uint8_t u8Thld)
{
    uint32_t u32Cfg = 0;

    if (!(IS_VALID_UART_HANDLE(pstUART)))
    {
        return ERR_PARA_ERR;
    }

    if (u8Thld > UART_RTS_THLD_MSK)
    {
        return ERR_PARA_ERR;
    }

    u32Cfg = pstUART->UART_CFG;

    u32Cfg &= ~(UART_RTS_THLD_MSK << UART_RTS_THLD_SHIFT);
    u32Cfg |= (u8Thld & UART_RTS_THLD_MSK) << UART_RTS_THLD_SHIFT;

    pstUART->UART_CFG = u32Cfg;

    return ERR_STA_OK;
}

/**
 * @brief  uart configur clear fifo function.
 * @param  pstUART: UART handle, Should be UART0 / UART1 / UART2.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_uart_clear_rxfifo(stUART_Handle_t *pstUART)
{
    if (!IS_VALID_UART_HANDLE(pstUART))
    {
        return ERR_PARA_ERR;
    }

    pstUART->UART_RXFIFO_CLR = UART_RXFIFO_CLR_MSK << UART_RXFIFO_CLR_SHIFT;

    return ERR_STA_OK;
}

/**
 * @brief  Get uart tx fifo count.
 * @param  pstUART: UART handle, Should be UART0 / UART1 / UART2.
 * @param  pu8Cnt: Tx fifo count values.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_uart_get_txfifo_cnt(stUART_Handle_t *pstUART, uint8_t *pu8Cnt)
{
    if ((!IS_VALID_UART_HANDLE(pstUART)) || (NULL == pu8Cnt))
    {
        return ERR_PARA_ERR;
    }

    *pu8Cnt = (pstUART->UART_TXFIFO_CNT >> UART_TX_FIFO_CNT_SHIFT) & UART_TX_FIFO_CNT_MSK;

    return ERR_STA_OK;
}

/**
 * @brief  Get uart rx fifo count.
 * @param  pstUART: UART handle, Should be UART0 / UART1 / UART2.
 * @param  pu8Cnt: Rx fifo count values.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_uart_get_rxfifo_cnt(stUART_Handle_t *pstUART, uint8_t *pu8Cnt)
{
    if ((!IS_VALID_UART_HANDLE(pstUART)) || (NULL == pu8Cnt))
    {
        return ERR_PARA_ERR;
    }

    *pu8Cnt = (pstUART->UART_RXFIFO_CNT >> UART_RX_FIFO_CNT_SHIFT) & UART_RX_FIFO_CNT_MSK;

    return ERR_STA_OK;
}

/**
 * @brief  Get uart interrupt flag(status).
 * @param  pstUART: UART handle, Should be UART0 / UART1 / UART2.
 * @param  pu16Falg: Indicate which interrupt flag will be read, @ref EN_UART_INT_FLAG_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_uart_get_interrupt_flag(stUART_Handle_t *pstUART, uint16_t *pu16Falg)
{
    if (!IS_VALID_UART_HANDLE(pstUART))
    {
        return ERR_PARA_ERR;
    }

    *pu16Falg = pstUART->UART_INT_FLAG & UART_INT_MASK;

    return ERR_STA_OK;
}

/**
 * @brief  Set uart interrupt enable.
 * @param  pstUART: UART handle, Should be UART0 / UART1 / UART2.
 * @param  u16Msk: Indicate which interrupt will be enable, @ref EN_UART_INT_FLAG_T.
 *                 bit-1 means enable
 *                 bit-0 means no impact.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_uart_enable_interrupt(stUART_Handle_t *pstUART, uint16_t u16Msk)
{
    if (!IS_VALID_UART_HANDLE(pstUART))
    {
        return ERR_PARA_ERR;
    }

    pstUART->UART_INT_CLR = u16Msk;

    pstUART->UART_INT_EN |= u16Msk;

    return ERR_STA_OK;
}

/**
 * @brief  set uart interrupt enable.
 * @param  pstUART: UART handle, Should be UART0 / UART1 / UART2.
 * @param  u16Msk: indicate which interrupt will be disable, @ref EN_UART_INT_FLAG_T.
 *                 bit-1 means disable
 *                 bit-0 means no impact.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_uart_disable_interrupt(stUART_Handle_t *pstUART, uint16_t u16Msk)
{
    if (!IS_VALID_UART_HANDLE(pstUART))
    {
        return ERR_PARA_ERR;
    }

    pstUART->UART_INT_CLR = u16Msk;

    pstUART->UART_INT_EN &= ~(u16Msk);

    return ERR_STA_OK;
}

/**
 * @brief  Clear uart indicated interrupt flag.
 * @param  pstUART: UART handle, Should be UART0 / UART1 / UART2.
 * @param  u16Msk: Indicate which flag will be clear, @ref EN_UART_INT_FLAG_T.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_uart_clear_interrupt_flag(stUART_Handle_t *pstUART, uint16_t u16Msk)
{
    if (!IS_VALID_UART_HANDLE(pstUART))
    {
        return ERR_PARA_ERR;
    }

    pstUART->UART_INT_CLR = u16Msk;

    return ERR_STA_OK;
}

/**
 * @brief  Get a byte data in non blocking mode.
 * @param  pstUART: UART handle, Should be UART0 / UART1 / UART2.
 * @param  pu8Data: Pointer to data buffer.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_uart_get_byte(stUART_Handle_t *pstUART, uint8_t *pu8Data)
{
    if ((!(IS_VALID_UART_HANDLE(pstUART))) || (NULL == pu8Data))
    {
        return ERR_PARA_ERR;
    }

    if (0 == ((pstUART->UART_RXFIFO_CNT >> UART_RX_FIFO_CNT_SHIFT) & UART_RX_FIFO_CNT_MSK))
    {
        return ERR_STA_ERROR;
    }

    *pu8Data = (uint8_t)((pstUART->UART_RX_FIFO >> UART_RX_FIFO_DATA_SHIFT) & UART_RX_FIFO_DATA_MSK);

    return ERR_STA_OK;
}

/**
 * @brief  Receives an amount of data in non blocking mode.
 * @param  pstUART: UART handle, Should be UART0 / UART1 / UART2.
 * @param  pu8Data: Pointer to data buffer.
 * @param  u8BufSize: Amount of data to be received.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_uart_receive(stUART_Handle_t *pstUART, uint8_t *pu8Buf, uint8_t u8BufSize)
{
    uint8_t i = 0;
    uint8_t u8Cnt = 0;
    EN_ERR_STA_T enRet;

    if ((!(IS_VALID_UART_HANDLE(pstUART))) || (NULL == pu8Buf) || (0 == u8BufSize))
    {
        return ERR_PARA_ERR;
    }

    enRet = rom_hw_uart_get_rxfifo_cnt(pstUART, &u8Cnt);
    ERR_RETURN_IF_ERROR(enRet);

    if (u8Cnt > u8BufSize)
    {
        u8Cnt = u8BufSize;
    }

    for (i = 0; i < u8Cnt; i++)
    {
        rom_hw_uart_get_byte(pstUART, pu8Buf);
        pu8Buf++;
    }

    return ERR_STA_OK;
}

/**
 * @brief  Transmits single data through the UART when TX FIFO is not fulled, blocking while sending.
 * @param  pstUART: UART handle, Should be UART0 / UART1 / UART2.
 * @param  u8Data: The data to transmit.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_uart_send_byte(stUART_Handle_t *pstUART, uint8_t u8Data)
{
    uint32_t u32Timout = 0;

    if (!IS_VALID_UART_HANDLE(pstUART))
    {
        return ERR_PARA_ERR;
    }

    while (u32Timout < UART_MAX_TIMEOUT)
    {
        u32Timout++;

        if (pstUART->UART_TXFIFO_CNT < UART_TX_FIFO_CNT_MAX)
        {
            pstUART->UART_TX_FIFO = u8Data;
            return ERR_STA_OK;
        }
        rom_delay_us(1);
    }

    return ERR_STA_TIMEOUT;
}

/**
 * @brief  Transmit an amount of data in blocking mode.
 * @param  pstUART: UART handle, Should be UART0 / UART1 / UART2.
 * @param  pu8Data: Pointer to data buffer.
 * @param  u16Len: Amount of data to be sent.
 * @return status: @ref EN_ERR_STA_T.
 */
EN_ERR_STA_T rom_hw_uart_transmit(stUART_Handle_t *pstUART, uint8_t *pu8Buf, uint16_t u16Len)
{
    uint16_t i = 0;
    uint32_t u32Timout = 0;

    EN_ERR_STA_T enRet = ERR_STA_OK;

    if ((!(IS_VALID_UART_HANDLE(pstUART))) || (NULL == pu8Buf) || (0 == u16Len))
    {
        return ERR_PARA_ERR;
    }

    if (UART2 == pstUART)
    {
        UART2->UART2_PHY_CFG = 0x00;
    }

    pstUART->UART_TXFIFO_CLR = UART_TXFIFO_CLR_MSK << UART_TXFIFO_CLR_SHIFT;

    for (i = 0; i < u16Len; i++)
    {
        pstUART->UART_INT_CLR = UART_INT_TX_FINISHED;

        enRet = rom_hw_uart_send_byte(pstUART, *pu8Buf);
        ERR_RETURN_IF_ERROR(enRet);

        pu8Buf++;
    }

    while ((pstUART->UART_INT_FLAG & UART_INT_TX_FINISHED) == 0)
    {
        u32Timout++;
        if (UART_MAX_TIMEOUT <= u32Timout)
        {
            return ERR_STA_TIMEOUT;
        }
        rom_delay_us(1);
    }

    return ERR_STA_OK;
}

