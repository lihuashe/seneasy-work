/**********************************************************************************************************************
 * @file     usb_device_dfu.c
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2022/12/20
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "RegMXD2670.h"
#include "err_def.h"
#include "hw_usb_ctrl.h"
#include "hw_usb_dcd.h"

#include "app_cfg.h"
#include "utility.h"
#include "debug.h"

#include "usb_device_descriptor.h"
#include "usb_device_dfu_descriptor.h"
#include "usb_device_dfu.h"


//=====================================================================================================================
// LOCAL VARIABLE
//=====================================================================================================================
static uint8_t m_u8UartRate[4];
static uint8_t m_u8UartCharFormat = 0;
static uint8_t m_u8UartParityType = 0;
static uint8_t m_u8UartDataBits = 0;
static uint8_t m_u8UsbCdcIsOpen;




/**
 * @brief  Reset USB device endpiont.
 */
void usb_device_reset_endpoint(void)
{
    rom_hw_usb_ctrl_device_end_point_reset(EP_ADDR_CDC_BULK_IN, EN_TYPE_BULK_IN);
    rom_hw_usb_ctrl_device_end_point_reset(EP_ADDR_CDC_BULK_OUT, EN_TYPE_BULK_OUT);
    rom_hw_usb_ctrl_endpoint_interrupt_enable(EP_ADDR_CDC_BULK_OUT);
    rom_hw_usb_ctrl_endpoint_interrupt_enable(EP_ADDR_CDC_BULK_IN);
}

/**
 * @brief  USB device cdc class request.
 * @param  pu8Setup: Pointer to setup buffer.
 */
void usb_device_cdc_class_request(uint8_t *pu8Setup)
{
    uint8_t Request[7];
    uint16_t u16CdcCmd;

    u16CdcCmd = BUILD_UINT16(pu8Setup[1], pu8Setup[0]);

    // GET_LINE_CODING
    if (CDC_CMD_GET_LINE_CODING == u16CdcCmd)
    {
        memcpy(pu8Setup, (uint8_t *)&m_u8UartRate, 4);
        pu8Setup[4] = m_u8UartCharFormat;
        pu8Setup[5] = m_u8UartParityType;
        pu8Setup[6] = m_u8UartDataBits;

        rom_hw_usb_ctrl_device_control_send(pu8Setup, 7, 3);
    }

    // SetControlLineState
    else if (CDC_CMD_SET_CONTROL_LINE_STATE == u16CdcCmd)
    {
        if (0x00 == pu8Setup[2])
        {
            m_u8UsbCdcIsOpen = false;
        }
        else if (0x01 == pu8Setup[2])
        {
            m_u8UsbCdcIsOpen = true;
        }
    }

    // SET_LINE_CODING
    else if (CDC_CMD_SET_LINE_CODING == u16CdcCmd)
    {
        uint32_t u32DataLen = 0;
        uint32_t u32Baudrate;

        rom_hw_usb_ctrl_device_setup_receive(Request, 7, &u32DataLen);

        memcpy((uint8_t *)&m_u8UartRate, Request, 4);
        m_u8UartCharFormat = Request[4];
        m_u8UartParityType = Request[5];
        m_u8UartDataBits = Request[6];
        u32Baudrate = BUILD_UINT32(m_u8UartRate[0], m_u8UartRate[1], m_u8UartRate[2], m_u8UartRate[3]);

        PRINTF("Uart baudrate: %d\n", u32Baudrate);
        PRINTF("Uart format: %d\n", m_u8UartCharFormat);
        PRINTF("Uart parity: %d\n", m_u8UartParityType);
        PRINTF("Uart databit: %d\n", m_u8UartDataBits);
    }
}

/**
 * @brief  Get USB device cdc status.
 * @return USB CDC is open or not.
 */
uint8_t usb_device_get_cdc_status(void)
{
    return m_u8UsbCdcIsOpen;
}

/**
 * @brief  USB device cdc recive data.
 * @param  pu8Buf: Pointer to save received data.
 * @param  pu32Len: Pointer to received data length.
 * @return Error code, @ref USB_DEVICE_ERR_CODE.
 */
EN_USB_DEVICE_ERR_CODE usb_device_recive_cdc_data(uint8_t *pu8Buf, uint32_t *pu32Len)
{
    return rom_hw_usb_ctrl_device_bulk_receive(EP_ADDR_CDC_BULK_OUT, pu8Buf, CDC_USB_DATA_PACKET_SIZE, pu32Len, 5);
}

/**
 * @brief  USB device cdc send data.
 * @param  pu8Buf: Pointer to send buffer.
 * @param  u32Len: Send data buffer length.
 * @return Error code, @ref USB_DEVICE_ERR_CODE.
 */
EN_USB_DEVICE_ERR_CODE usb_device_send_cdc_data(uint8_t *pu8Buf, uint32_t u32Len)
{
    return rom_hw_usb_ctrl_device_bulk_send(EP_ADDR_CDC_BULK_IN, pu8Buf, u32Len, 5);
}


