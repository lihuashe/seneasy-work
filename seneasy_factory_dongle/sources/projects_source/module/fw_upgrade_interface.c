/**********************************************************************************************************************
 * @file     fw_upgrade_interface.c
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2022/8/19
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
#include <string.h>

#include "app_cfg.h"
#include "utility.h"
#include "err_def.h"

#if (1 == FW_UPGRADE_UART_ENABLED)
    #include "fw_upgrade_interface_uart.h"
#endif

#if (1 == FW_UPGRADE_USB_ENABLED)
    #include "fw_upgrade_interface_usb.h"
#endif

#include "fw_upgrade_interface.h"


//=====================================================================================================================
// LOCAL VARIABLES
//=====================================================================================================================
static enFwUpgradePhyInterface_t m_enInterface;




/**
 * @brief  Init io, peripheral.
 * @param  enInterface: Communication interface.
 * @param  pstInit: Pointer to a stFwUpgradePhyInit_t structure.
 */
void fw_upgrade_interface_init(enFwUpgradePhyInterface_t enInterface, stFwUpgradeInterfaceInit_t *pstInit)
{
    m_enInterface = enInterface;

    switch (m_enInterface)
    {
        #if (1 == FW_UPGRADE_UART_ENABLED)
        case FW_UPGRADE_ITERFACE_UART:
        {
            fw_upgrade_interface_uart_init((stFwUpgradeInterfaceUartInit_t *)pstInit);
            break;
        }
        #endif
        case FW_UPGRADE_ITERFACE_SPI:
        {
            break;
        }
        case FW_UPGRADE_ITERFACE_USB:
        {
            break;
        }
        case FW_UPGRADE_ITERFACE_BLE:
        {
            break;
        }
        default: break;
    }
}

/**
 * @brief  
 */
void fw_upgrade_interface_deinit(void)
{
    switch (m_enInterface)
    {
        #if (1 == FW_UPGRADE_UART_ENABLED)
        case FW_UPGRADE_ITERFACE_UART:
        {
            fw_upgrade_interface_uart_deinit();
            break;
        }
        #endif
        case FW_UPGRADE_ITERFACE_SPI:
        {
            break;
        }
        case FW_UPGRADE_ITERFACE_USB:
        {
            break;
        }
        case FW_UPGRADE_ITERFACE_BLE:
        {
            break;
        }
        default: break;
    }
}

/**
 * @brief  
 */
void fw_upgrade_interface_reinit(void)
{
    switch (m_enInterface)
    {
        #if (1 == FW_UPGRADE_UART_ENABLED)
        case FW_UPGRADE_ITERFACE_UART:
        {
            fw_upgrade_interface_uart_reinit();
            break;
        }
        #endif
        case FW_UPGRADE_ITERFACE_SPI:
        {
            break;
        }
        case FW_UPGRADE_ITERFACE_USB:
        {
            break;
        }
        case FW_UPGRADE_ITERFACE_BLE:
        {
            break;
        }
        default: break;
    }
}

/**
 * @brief  Transmit an amount of data in blocking mode.
 * @param  pu8buf: Pointer to data buffer.
 * @param  u16Len: Amount of data to be sent.
 */
void fw_upgrade_interface_send(uint8_t *pu8buf, uint16_t length)
{
    switch (m_enInterface)
    {
        #if (1 == FW_UPGRADE_UART_ENABLED)
            case FW_UPGRADE_ITERFACE_UART:
            { 
                fw_upgrade_interface_uart_send_by_blocking(pu8buf, length);
                break;
            }
        #endif
        case FW_UPGRADE_ITERFACE_SPI:
        {
            break;
        }
        #if (1 == FW_UPGRADE_USB_ENABLED)
            case FW_UPGRADE_ITERFACE_USB:
            {
                uint16_t u16StartCode = 0x3D4C;
                uint16_t u16TailCode = 0xA1B2;
                uint8_t data[64] = {0x4C, 0x3D};
                
                memcpy(&data[2], pu8buf, length);
                data[length + 2] = 0xB2;
                data[length + 3] = 0xA1;
                fw_upgrade_interface_usb_send_by_blocking(data, length + 4);
                break;
            }
        #endif
        case FW_UPGRADE_ITERFACE_BLE:
        {
            break;
        }
        default: break;
    }
}

/**
 * @brief  Get tx and rx transmit index.
 * @param  pu16TxIndex: pointer to dma uart tx index.
 * @param  pu16RxIndex: pointer to dma uart rx index.
 */
void fw_upgrade_interface_get_dma_trx_index(uint16_t *pu16TxIndex, uint16_t *pu16RxIndex)
{
    switch (m_enInterface)
    {
        #if (1 == FW_UPGRADE_UART_ENABLED)
        case FW_UPGRADE_ITERFACE_UART:
        {
            fw_upgrade_interface_uart_get_dma_trx_index(pu16TxIndex, pu16RxIndex);
            break;
        }
        #endif
        case FW_UPGRADE_ITERFACE_SPI:
        {
            break;
        }
        case FW_UPGRADE_ITERFACE_USB:
        {
            break;
        }
        case FW_UPGRADE_ITERFACE_BLE:
        {
            break;
        }
        default: break;
    }
}


