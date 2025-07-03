/**********************************************************************************************************************
 * @file     fw_upgrade_Interface_usb.c
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
#include "hw_usb_dcd.h"

#include "app_cfg.h"
#include "boards.h"
#include "utility.h"

#include "usb_device_dfu.h"
#include "fw_upgrade_interface_usb.h"


//=====================================================================================================================
// Define
//=====================================================================================================================
#if (0 == FW_UPGRADE_USB_DEBUG_ENABLED)
    #undef  PRINTF
    #define PRINTF(fmt, args...)
#endif

#if defined(__MAIN_PROCESSOR)
    #define DMA_SYS_CTRL_INT (SYS_CTRL_MP)
#else
    #define DMA_SYS_CTRL_INT (SYS_CTRL_CP)
#endif


#if (1 == FW_UPGRADE_USB_ENABLED)
//=====================================================================================================================
// LOCAL VARIABLES
//=====================================================================================================================
static stUART_Handle_t *m_stUartHandle = NULL;
static stFwUpgradeInterfaceUartInit_t m_stUartInit;

static uint16_t m_u16TxIndex = 0;
static uint16_t m_u16RxIndex = 0;





/**
 * @brief  UART IO Init.
 * @param  penPeriTxID: pointer to dma uart tx peripheral id.
 * @param  penPeriRxID: pointer to dma uart rx peripheral id.
 */
static void fw_upgrade_interface_io_init(EN_DMA_PERI_SEL_T *penPeriTxID, EN_DMA_PERI_SEL_T *penPeriRxID)
{
    
}

/**
 * @brief  Init io, uart and dma.
 * @param  pstInit: Pointer to a stFwUpgradePhyUartInit_t structure.
 * @return status: @ref EN_ERR_STA_T.
 */
uint8_t fw_upgrade_interface_usb_init(stFwUpgradeInterfaceUartInit_t *pstInit)
{
    

    return ERR_STA_OK;
}

/**
 * @brief  Deinit usb io.
 */
void fw_upgrade_interface_usb_deinit(void)
{
    
}

/**
 * @brief  Re-init usb.
 */
void fw_upgrade_interface_usb_reinit(void)
{
    fw_upgrade_interface_usb_init(&m_stUartInit);
}

/**
 * @brief  Transmit an amount of data in blocking mode.
 * @param  pu8buf: Pointer to data buffer.
 * @param  u16Len: Amount of data to be sent.
 */
void fw_upgrade_interface_usb_send_by_blocking(uint8_t *pu8buf, uint16_t u16Len)
{
    usb_device_send_cdc_data(pu8buf, u16Len);
}

/**
 * @brief  Get dma tx and rx transmit index.
 * @param  pu16TxIndex: pointer to an index which means usb had transmitted byte numbers.
 * @param  pu16RxIndex: pointer to an index which means usb had received byte numbers.
 */
void fw_upgrade_interface_usb_get_dma_trx_index(uint16_t *pu16TxIndex, uint16_t *pu16RxIndex)
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

