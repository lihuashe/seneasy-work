/**********************************************************************************************************************
 * @file     fw_upgrade_Interface_uart.c
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2022/5/9
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
 * Copyright (c) 2016~2020, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 *********************************************************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "RegMXD2670.h"
#include "err_def.h"
#include "hw_crg.h"
#include "hw_dma.h"
#include "hw_gpio.h"
#include "hw_uart.h"
#include "hw_sys_ctrl.h"

#include "app_cfg.h"
#include "boards.h"
#include "utility.h"

#include "fw_upgrade_Interface_uart.h"


//=====================================================================================================================
// Define
//=====================================================================================================================
#if (0 == FW_UPGRADE_UART_DEBUG_ENABLED)
    #undef  PRINTF
    #define PRINTF(fmt, args...)
#endif

#if defined(__MAIN_PROCESSOR)
    #define DMA_SYS_CTRL_INT (SYS_CTRL_MP)
#else
    #define DMA_SYS_CTRL_INT (SYS_CTRL_CP)
#endif


#if (1 == FW_UPGRADE_UART_ENABLED)
//=====================================================================================================================
// LOCAL VARIABLES
//=====================================================================================================================
static stUART_Handle_t *m_stUartHandle = NULL;
static stFwUpgradeInterfaceUartInit_t m_stUartInit;

static uint16_t m_u16TxIndex = 0;
static uint16_t m_u16RxIndex = 0;




/**
 * @brief  DMA IRQ7 interrupt handler
 */
void DMA_IRQ_HANDLER_FW_UPGRADE(void)
{
    uint8_t u8IntMsk;

    rom_hw_dma_get_interrupt_flag(DMA_HANDLE_FW_UPGRADE_RX, &u8IntMsk);
    rom_hw_dma_clear_interrupt_flag(DMA_HANDLE_FW_UPGRADE_RX, u8IntMsk);

    if (u8IntMsk & DMA_INT_MSK)
    {
        rom_hw_dma_get_trx_pointer(DMA_HANDLE_FW_UPGRADE_RX, &m_u16RxIndex);
        PRINTF("DMA RX INT: %02X, %d\n", u8IntMsk, m_u16RxIndex);
    }

    rom_hw_dma_get_interrupt_flag(DMA_HANDLE_FW_UPGRADE_TX, &u8IntMsk);
    rom_hw_dma_clear_interrupt_flag(DMA_HANDLE_FW_UPGRADE_TX, u8IntMsk);

    if (u8IntMsk & DMA_INT_MSK)
    {
        rom_hw_dma_get_trx_pointer(DMA_HANDLE_FW_UPGRADE_TX, &m_u16TxIndex);
        PRINTF("DMA TX INT: %02X, %d\n", u8IntMsk, m_u16TxIndex);
    }

    if (NULL != m_stUartInit.pfnCB)
    {
        m_stUartInit.pfnCB(&m_u16TxIndex, &m_u16RxIndex);
    }
}

/**
 * @brief  UART IO Init.
 * @param  penPeriTxID: pointer to dma uart tx peripheral id.
 * @param  penPeriRxID: pointer to dma uart rx peripheral id.
 */
static void fw_upgrade_interface_io_init(EN_DMA_PERI_SEL_T *penPeriTxID, EN_DMA_PERI_SEL_T *penPeriRxID)
{
    if (UART_HANDLE_FW_UPGRADE == UART0)
    {
        *penPeriTxID = DMA_PERI_UART0_TX;
        *penPeriRxID = DMA_PERI_UART0_RX;

        rom_hw_crg_enable_clk_gate(CRG_UART0_CLK_GATE);
        rom_hw_gpio_set_pin_pid(GPIO_PORT_FW_UPGRADE_RX, GPIO_PIN_FW_UPGRADE_RX, PID_UART0_RXD);
        rom_hw_gpio_set_pin_pid(GPIO_PORT_FW_UPGRADE_TX, GPIO_PIN_FW_UPGRADE_TX, PID_UART0_TXD);
    }
    else if (UART_HANDLE_FW_UPGRADE == UART1)
    {
        *penPeriTxID = DMA_PERI_UART1_TX;
        *penPeriRxID = DMA_PERI_UART1_RX;

        rom_hw_crg_enable_clk_gate(CRG_UART1_CLK_GATE);
        rom_hw_gpio_set_pin_pid(GPIO_PORT_FW_UPGRADE_RX, GPIO_PIN_FW_UPGRADE_RX, PID_UART1_RXD);
        rom_hw_gpio_set_pin_pid(GPIO_PORT_FW_UPGRADE_TX, GPIO_PIN_FW_UPGRADE_TX, PID_UART1_TXD);
    }
    else
    {
        *penPeriTxID = DMA_PERI_UART2_TX;
        *penPeriRxID = DMA_PERI_UART2_RX;

        rom_hw_crg_enable_clk_gate(CRG_UART2_CLK_GATE);
        rom_hw_gpio_set_pin_pid(GPIO_PORT_FW_UPGRADE_RX, GPIO_PIN_FW_UPGRADE_RX, PID_UART2_RXD);
        rom_hw_gpio_set_pin_pid(GPIO_PORT_FW_UPGRADE_TX, GPIO_PIN_FW_UPGRADE_TX, PID_UART2_TXD);
    }

    rom_hw_gpio_set_pin_pull_mode(GPIO_PORT_FW_UPGRADE_RX, GPIO_PIN_FW_UPGRADE_RX, GPIO_PULL_UP);
    rom_hw_gpio_set_pin_pull_mode(GPIO_PORT_FW_UPGRADE_TX, GPIO_PIN_FW_UPGRADE_TX, GPIO_PULL_UP);
}

/**
 * @brief  Init io, uart and dma.
 * @param  pstInit: Pointer to a stFwUpgradePhyUartInit_t structure.
 * @return status: @ref EN_ERR_STA_T.
 */
uint8_t fw_upgrade_interface_uart_init(stFwUpgradeInterfaceUartInit_t *pstInit)
{
    EN_DMA_PERI_SEL_T enPeriTxID;
    EN_DMA_PERI_SEL_T enPeriRxID;
    stUartInit_t stUartInit;
    stDmaInit_t stDmaInit;

    if (NULL == pstInit || NULL == pstInit->pu8RxBuf)
    {
        return ERR_PARA_ERR;
    }

    m_u16TxIndex = 0;
    m_u16RxIndex = 0;
    memcpy(&m_stUartInit, pstInit, sizeof(stFwUpgradeInterfaceUartInit_t));

    // 1. io init
    fw_upgrade_interface_io_init(&enPeriTxID, &enPeriRxID);

    // 2. UART init
    // 2.1 UART init
    stUartInit.u32UartBaudRate = UART_BPS_FW_UPGRADE;
    stUartInit.unUartCfg.u32UartCfg = UART_INIT_DEFAULT(UART_PARITY_NONE);
    rom_hw_uart_init(UART_HANDLE_FW_UPGRADE, &stUartInit);

    // 2.2 Send a byte for testing interface
    rom_hw_uart_send_byte(UART_HANDLE_FW_UPGRADE, 0x55);

    // 3. DMA interrupt init
    rom_hw_sys_ctrl_enable_peri_int(DMA_SYS_CTRL_INT, DMA_IRQ_FW_UPGRADE);
    NVIC_ClearPendingIRQ(DMA_IRQ_FW_UPGRADE);
    NVIC_SetPriority(DMA_IRQ_FW_UPGRADE, 0x03);
    NVIC_EnableIRQ(DMA_IRQ_FW_UPGRADE);

    // 4. DMA init
    // 4.1 Enable dma clock gate
    rom_hw_crg_enable_clk_gate(CRG_DMA_AHB_CLK_GATE);

    if ((NULL != pstInit->pu8TxBuf) && (0 != pstInit->u16TxBufLen))
    {
        // 4.2 DMA tx parameter init
        stDmaInit.u32SrcAddr = (uint32_t)pstInit->pu8TxBuf,
        stDmaInit.u32DstAddr = ((uint32_t)&UART_HANDLE_FW_UPGRADE->UART_TX_FIFO);
        stDmaInit.unPeriSel.stPeriSel.enPeriID = enPeriTxID,
        stDmaInit.unCfg.u32DMACfg = DMA_INIT_MEM_TO_PERI_DEFAULT_CFG(pstInit->u16TxBufLen, DMA_DATA_1BYTE);

        // 4.3 Configure dma uart tx
        rom_hw_dma_init(DMA_HANDLE_FW_UPGRADE_TX, &stDmaInit);
        rom_hw_dma_set_interrupt_channel(DMA_HANDLE_FW_UPGRADE_TX, DMA_INT_FW_UPGRADE);
        rom_hw_dma_enable_interrupt(DMA_HANDLE_FW_UPGRADE_TX, DMA_INT_TIMEOUT);
        rom_hw_dma_enable(DMA_HANDLE_FW_UPGRADE_TX);
    }

    // 4.4 DMA rx parameter init
    stDmaInit.u32SrcAddr = ((uint32_t)&UART_HANDLE_FW_UPGRADE->UART_RX_FIFO);
    stDmaInit.u32DstAddr = (uint32_t)pstInit->pu8RxBuf;
    stDmaInit.u32TimeOut = 1000;
    stDmaInit.unPeriSel.stPeriSel.enPeriID = enPeriRxID;
    stDmaInit.unCfg.u32DMACfg = DMA_INIT_PERI_TO_MEM_DEFAULT_CFG(pstInit->u16RxBufLen, DMA_DATA_1BYTE);

    // 4.5 Configure dma uart rx
    rom_hw_dma_init(DMA_HANDLE_FW_UPGRADE_RX, &stDmaInit);
    rom_hw_dma_set_interrupt_channel(DMA_HANDLE_FW_UPGRADE_RX, DMA_INT_FW_UPGRADE);
    rom_hw_dma_enable_interrupt(DMA_HANDLE_FW_UPGRADE_RX, DMA_INT_TIMEOUT);
    rom_hw_dma_enable(DMA_HANDLE_FW_UPGRADE_RX);

    return ERR_STA_OK;
}

/**
 * @brief  Deinit uart io.
 */
void fw_upgrade_interface_uart_deinit(void)
{
    rom_hw_gpio_set_pin_pid(GPIO_PORT_FW_UPGRADE_RX, GPIO_PIN_FW_UPGRADE_RX, PID_GPIO_PIN);
    rom_hw_gpio_set_pin_pid(GPIO_PORT_FW_UPGRADE_TX, GPIO_PIN_FW_UPGRADE_TX, PID_GPIO_PIN);
    rom_hw_gpio_set_pin_input_output(GPIO_PORT_FW_UPGRADE_RX, GPIO_PIN_FW_UPGRADE_RX, GPIO_MODE_IMPEDANCE);
    rom_hw_gpio_set_pin_input_output(GPIO_PORT_FW_UPGRADE_TX, GPIO_PIN_FW_UPGRADE_TX, GPIO_MODE_IMPEDANCE);
    rom_hw_gpio_set_pin_pull_mode(GPIO_PORT_FW_UPGRADE_RX, GPIO_PIN_FW_UPGRADE_RX, GPIO_PULL_NONE);
    rom_hw_gpio_set_pin_pull_mode(GPIO_PORT_FW_UPGRADE_TX, GPIO_PIN_FW_UPGRADE_TX, GPIO_PULL_NONE);

    if (UART_HANDLE_FW_UPGRADE == UART0)
    {
        rom_hw_crg_disable_clk_gate(CRG_UART0_CLK_GATE);
    }
    else if (UART_HANDLE_FW_UPGRADE == UART1)
    {
        rom_hw_crg_disable_clk_gate(CRG_UART1_CLK_GATE);
    }
    else
    {
        rom_hw_crg_disable_clk_gate(CRG_UART2_CLK_GATE);
    }
}

/**
 * @brief  Re-init uart.
 */
void fw_upgrade_interface_uart_reinit(void)
{
    fw_upgrade_interface_uart_init(&m_stUartInit);
}

/**
 * @brief  Transmit an amount of data in blocking mode.
 * @param  pu8buf: Pointer to data buffer.
 * @param  u16Len: Amount of data to be sent.
 */
void fw_upgrade_interface_uart_send_by_blocking(uint8_t *pu8buf, uint16_t length)
{
    uint16_t start_code = 0x3D4C;
    uint16_t Package_tail = 0xA1B2;

    rom_hw_uart_transmit(UART_HANDLE_FW_UPGRADE, (uint8_t *)&start_code, 2);
    rom_hw_uart_transmit(UART_HANDLE_FW_UPGRADE, (uint8_t *)pu8buf, length);
    rom_hw_uart_transmit(UART_HANDLE_FW_UPGRADE, (uint8_t *)&Package_tail, 2);
}

/**
 * @brief  Get dma tx and rx transmit index.
 * @param  pu16TxIndex: pointer to an index which means uart had transmitted byte numbers.
 * @param  pu16RxIndex: pointer to an index which means uart had received byte numbers.
 */
void fw_upgrade_interface_uart_get_dma_trx_index(uint16_t *pu16TxIndex, uint16_t *pu16RxIndex)
{
    if (NULL != pu16TxIndex)
    {
        *pu16TxIndex = m_u16TxIndex;
    }

    if (NULL != pu16RxIndex)
    {
        *pu16RxIndex = m_u16RxIndex;
    }
}

#endif /* (1 == FW_UPGRADE_UART_ENABLED) */

