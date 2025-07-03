/**********************************************************************************************************************
 * @file     fw_upgrade_Interface_uart.h
 * @version  V1.0
 * @date     2022/11/29
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

#ifndef __FW_UPGRADE_INTERFACE_UART_H__
#define __FW_UPGRADE_INTERFACE_UART_H__


//=====================================================================================================================
// typedef
//=====================================================================================================================
typedef uint32_t ( *fw_upgrade_interface_uart_callback ) ( uint16_t *pu16TxIndex, uint16_t *pu16RxIndex );

typedef struct
{
    uint8_t* pu8TxBuf;
    uint16_t u16TxBufLen;
    uint8_t* pu8RxBuf;
    uint16_t u16RxBufLen;
    fw_upgrade_interface_uart_callback pfnCB;

} stFwUpgradeInterfaceUartInit_t;




/**
 * @brief  Init io, uart and dma.
 * @param  pstInit: Pointer to a stFwUpgradePhyUartInit_t structure.
 * @return status: @ref EN_ERR_STA_T.
 */
extern uint8_t fw_upgrade_interface_uart_init(stFwUpgradeInterfaceUartInit_t *pstInit);


/**
 * @brief  Deinit uart io.
 */
extern void fw_upgrade_interface_uart_deinit(void);


/**
 * @brief  Re-init uart.
 */
extern void fw_upgrade_interface_uart_reinit(void);


/**
 * @brief  Transmit an amount of data in blocking mode.
 * @param  pu8buf: Pointer to data buffer.
 * @param  u16Len: Amount of data to be sent.
 */
extern void fw_upgrade_interface_uart_send_by_blocking(uint8_t* pu8buf, uint16_t length);


/**
 * @brief  Get tx and rx transmit index.
 * @param  pu16TxIndex: pointer to dma uart tx index.
 * @param  pu16RxIndex: pointer to dma uart rx index.
 */
extern void fw_upgrade_interface_uart_get_dma_trx_index(uint16_t* pu16TxIndex, uint16_t* pu16RxIndex);




#endif /* __FW_UPGRADE_INTERFACE_UART_H__ */

