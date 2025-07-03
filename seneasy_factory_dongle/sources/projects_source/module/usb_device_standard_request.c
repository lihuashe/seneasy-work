/**********************************************************************************************************************
 * @file     usb_device_standard_request.c
 * @author   Maxscend AE Team
 * @version  V1.0
 * @date     2021/02/25
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
#include "type_def.h"
#include "utility.h"
#include "debug.h"

#include "usb_utility.h"
#include "hw_usb_phy.h"
#include "hw_usb_ctrl.h"
#include "hw_usb_timeout.h"
#include "hw_usb_dcd.h"
#include "hw_usb_hcd.h"
#include "hal_usb.h"

#include "usb_device_descriptor.h"
#include "usb_device_dfu_descriptor.h"

#include "usb_device_standard_request.h"

//=====================================================================================================================
// DEFINE
//=====================================================================================================================
#define PRINT_ENABLE 1
#if PRINT_ENABLE
#define PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define PRINTF(fmt, ...)
#endif

//=====================================================================================================================
// GLOBAL VARIABLE
//=====================================================================================================================
uint8_t gDeviceStringLangID[] = {0x04, 0x03, 0x09, 0x04};
uint8_t Setup[8] = {0};
uint8_t Request[64];
const uint8_t gDeviceString_Manu[] = "maxscend";         // max length: 32bytes
const uint8_t gDeviceString_SerialNumber[] = "20220926"; // max length: 32bytes
uint8_t gDeviceQualifier[10] = {10, 6, 0x10, 0x01, 0, 0, 0, 64, 1, 0};
uint8_t *gDeviceString_Index;

struct usb_std_desp_t gsUsbStdDesp =
    {
        .u8Mode = USB_DEVICE_UNKONWN,
        .pu8DevDesp = NULL,
        .pstConfigDesp = {.u16Length = 0, .pu8Buff = NULL},
        .pstInterfaceDesp = {.u16Length = 0, .pu8Buff = NULL},
        .pstEndpointDesp = {.u16Length = 0, .pu8Buff = NULL},
        .pstDevStringLangeID = {.u16Length = 0, .pu8Buff = NULL},
        .pstDevStringManu = {.u16Length = 0, .pu8Buff = NULL},
        .pstDevProductString = {.u16Length = 0, .pu8Buff = NULL},
        .pstgDevStringSerialNum = {.u16Length = 0, .pu8Buff = NULL},
        .pstDeviceQualifier = {.u16Length = 0, .pu8Buff = NULL},
        .pstHIDReportDesp = {.u16Length = 0, .pu8Buff = NULL},
        .pstDevStringIndex = {.u16Length = 0, .pu8Buff = NULL},
};

/**********************************************************************************************************************
 * @brief  device endpiont reset
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
static void usb_device_endpoint_reset(uint8_t u8Mode)
{
    if (USB_DEVICE_DONGLE_CDC_AUDIO == u8Mode)
    {
        // reset audio mic endpoint
        rom_hw_usb_ctrl_device_end_point_reset(EP_ADDR_AUDIO_ISO_IN, EN_TYPE_ISO_IN);
        rom_hw_usb_ctrl_device_iso_send(EP_ADDR_AUDIO_ISO_IN, NULL, 0);

        // reset cdc endpoint
        rom_hw_usb_ctrl_device_end_point_reset(EP_ADDR_CDC_BULK_IN, EN_TYPE_BULK_IN);
        rom_hw_usb_ctrl_device_end_point_reset(EP_ADDR_CDC_BULK_OUT, EN_TYPE_BULK_OUT);
        rom_hw_usb_ctrl_endpoint_interrupt_enable(EP_ADDR_CDC_BULK_OUT);
        rom_hw_usb_ctrl_endpoint_interrupt_enable(EP_ADDR_CDC_BULK_IN);
    }
    else if (USB_DEVICE_CDC == u8Mode)
    {
        rom_hw_usb_ctrl_device_end_point_reset(EP_ADDR_CDC_BULK_IN, EN_TYPE_BULK_IN);
        rom_hw_usb_ctrl_device_end_point_reset(EP_ADDR_CDC_BULK_OUT, EN_TYPE_BULK_OUT);
        rom_hw_usb_ctrl_endpoint_interrupt_enable(EP_ADDR_CDC_BULK_OUT);
        rom_hw_usb_ctrl_endpoint_interrupt_enable(EP_ADDR_CDC_BULK_IN);
    }
}

/**********************************************************************************************************************
 * @brief  Set USB descriptor mode
 * @param  u8Mode: USB_DEVICE_TYPE_T
 * @output
 * @return None.
 *********************************************************************************************************************/
uint8_t usb_device_set_standard_descriptor(uint8_t u8Mode)
{
    struct usb_std_desp_t *pstUsbStdDesp = &gsUsbStdDesp;
    if (u8Mode == USB_DEVICE_DONGLE_CDC_AUDIO)
    {
        pstUsbStdDesp->pu8DevDesp = gUsbDeviceDescriptor;

        pstUsbStdDesp->pstConfigDesp.pu8Buff = (uint8_t *)gUsbConfgDesAudioCDC;
        pstUsbStdDesp->pstConfigDesp.u16Length = (gUsbConfgDesAudioCDC[3] << 8) + gUsbConfgDesAudioCDC[2];

        pstUsbStdDesp->pstInterfaceDesp.pu8Buff = NULL;
        pstUsbStdDesp->pstInterfaceDesp.u16Length = 0;

        pstUsbStdDesp->pstEndpointDesp.pu8Buff = NULL;
        pstUsbStdDesp->pstEndpointDesp.u16Length = 0;

        pstUsbStdDesp->pstDevStringLangeID.pu8Buff = gDeviceStringLangID;
        pstUsbStdDesp->pstDevStringLangeID.u16Length = sizeof(gDeviceStringLangID);

        pstUsbStdDesp->pstDevStringManu.pu8Buff = (uint8_t *)gDeviceString_Manu;
        pstUsbStdDesp->pstDevStringManu.u16Length = strlen((char *)gDeviceString_Manu);

        pstUsbStdDesp->pstDevProductString.pu8Buff = (uint8_t *)gDeviceProductString;
        pstUsbStdDesp->pstDevProductString.u16Length = strlen((char *)gDeviceProductString);

        pstUsbStdDesp->pstgDevStringSerialNum.pu8Buff = (uint8_t *)gDeviceString_SerialNumber;
        pstUsbStdDesp->pstgDevStringSerialNum.u16Length = strlen((char *)gDeviceString_SerialNumber);

        pstUsbStdDesp->pstDeviceQualifier.pu8Buff = gDeviceQualifier;
        pstUsbStdDesp->pstDeviceQualifier.u16Length = sizeof(gDeviceQualifier);

        pstUsbStdDesp->pstDevStringIndex.pu8Buff = NULL;
        pstUsbStdDesp->pstDevStringIndex.u16Length = 0;
    }
    else if (u8Mode == USB_DEVICE_CDC)
    {
        pstUsbStdDesp->pu8DevDesp = g_au8UsbDeviceDescriptor;

        pstUsbStdDesp->pstConfigDesp.pu8Buff = (uint8_t *)g_au8UsbConfigurationDescriptor;
        pstUsbStdDesp->pstConfigDesp.u16Length = g_au8UsbConfigurationDescriptor[2];

        pstUsbStdDesp->pstDevStringLangeID.pu8Buff = (uint8_t *)g_au8UsbStringDescriptorLangID;
        pstUsbStdDesp->pstDevStringLangeID.u16Length = g_au8UsbStringDescriptorLangID[0];

        pstUsbStdDesp->pstDeviceQualifier.pu8Buff = g_au8DeviceQualifierDescriptor;
        pstUsbStdDesp->pstDeviceQualifier.u16Length = sizeof(g_au8DeviceQualifierDescriptor);

        pstUsbStdDesp->pstDevStringManu.pu8Buff = (uint8_t *)gDeviceString_Manu;
        pstUsbStdDesp->pstDevStringManu.u16Length = strlen((char *)gDeviceString_Manu);

        pstUsbStdDesp->pstDevProductString.pu8Buff = (uint8_t *)gDeviceProductString;
        pstUsbStdDesp->pstDevProductString.u16Length = strlen((char *)gDeviceProductString);

        pstUsbStdDesp->pstgDevStringSerialNum.pu8Buff = (uint8_t *)gDeviceString_SerialNumber;
        pstUsbStdDesp->pstgDevStringSerialNum.u16Length = strlen((char *)gDeviceString_SerialNumber);

        pstUsbStdDesp->pstDevStringIndex.pu8Buff = NULL;
        pstUsbStdDesp->pstDevStringIndex.u16Length = 0;
    }
    else
    {
        PRINTF("no find match usb mode descirpor\r\n");
    }
    return 0;
}


/**********************************************************************************************************************
 * @brief  USB model choose.
 * @param  u8Mode: usb word model
 * @param  u16UsbVid:
 * @param  u16UsbPid:
 * @output
 * @return None.
 *********************************************************************************************************************/
void usb_device_model_select(uint8_t u8Mode, uint16_t u16UsbVid, uint16_t u16UsbPid)
{
    uint8_t u8Result = 0;

    u8Result = usb_device_set_standard_descriptor(u8Mode);
    if (u8Result)
        return;

    struct usb_std_desp_t *pstUsbStdDesp = &gsUsbStdDesp;
    struct std_device_desp_t *pstStdDevDesp = (struct std_device_desp_t *)pstUsbStdDesp->pu8DevDesp;

    pstUsbStdDesp->u8Mode = u8Mode;
    pstStdDevDesp->u16IdVendor = u16UsbVid;
    pstStdDevDesp->u16IdProduct = u16UsbPid;

    if (pstUsbStdDesp->u8Mode == USB_DEVICE_DONGLE_CDC_AUDIO)
    {
        usb_device_endpoint_reset(USB_DEVICE_DONGLE_CDC_AUDIO);
    }

    else if (pstUsbStdDesp->u8Mode == USB_DEVICE_CDC)
    {
        usb_device_endpoint_reset(USB_DEVICE_CDC);
    }
}

/**********************************************************************************************************************
 * @brief  Sends the reply data to control the transmission command.
 * @param  u16Resp: reply data
 * @param  u8Len: reply len,1 or 2
 * @output
 * @return None.
 *********************************************************************************************************************/
void usb_device_send_resp(uint16_t u16Resp, uint8_t u8Len)
{
    u16Resp = CpuToLe16(u16Resp);
    rom_hw_usb_ctrl_device_control_send((uint8_t *)&u16Resp, u8Len, 3);
}

/**********************************************************************************************************************
 * @brief  Process the get descriptor command.
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
void usb_device_descriptor_get(void)
{
    uint8_t u8StringBuf[32 * 2 + 2];
    const uint8_t *pu8UsbSendPtr = NULL;
    uint16_t u16Len = 0;
    struct usb_std_desp_t *pstUsbStdDesp = &gsUsbStdDesp;
    switch (Setup[3])
    {
    case USB_DT_DEVICE:
        PRINTF("USB_DT_DEVICE_Descriptor\n");
        pu8UsbSendPtr = pstUsbStdDesp->pu8DevDesp;
        u16Len = pu8UsbSendPtr[0];
        break;

    case USB_DT_CONFIG:
        PRINTF("USB_DT_CONFIG_Descriptor\r\n");
        pu8UsbSendPtr = pstUsbStdDesp->pstConfigDesp.pu8Buff;
        u16Len = pstUsbStdDesp->pstConfigDesp.u16Length;
        break;

    case USB_DT_STRING:
        PRINTF("USB_DT_STRING.");
        if (Setup[2] == 0) // lang ids
        {
            PRINTF("langId\n");
            pu8UsbSendPtr = pstUsbStdDesp->pstDevStringLangeID.pu8Buff;
            u16Len = pstUsbStdDesp->pstDevStringLangeID.u16Length;
            break;
        }
        else if (Setup[2] == 1) // manu
        {
            PRINTF("manu\r\n");
            pu8UsbSendPtr = pstUsbStdDesp->pstDevStringManu.pu8Buff;
        }
        else if (Setup[2] == 2) // product
        {
            PRINTF("product\r\n");
            pu8UsbSendPtr = pstUsbStdDesp->pstDevProductString.pu8Buff;
        }
        else if (Setup[2] == 4) // debug effect
        {
            PRINTF("debugEffect\r\n");
            pu8UsbSendPtr = pstUsbStdDesp->pstDevStringIndex.pu8Buff;
        }
        else // serial number
        {
            PRINTF("serialNum\r\n");
            pu8UsbSendPtr = pstUsbStdDesp->pstgDevStringSerialNum.pu8Buff;
        }

        for (u16Len = 0; u16Len < 32; u16Len++)
        {
            if (pu8UsbSendPtr[u16Len] == '\0')
            {
                break;
            }
            u8StringBuf[2 + u16Len * 2 + 0] = pu8UsbSendPtr[u16Len];
            u8StringBuf[2 + u16Len * 2 + 1] = 0x00;
        }

        u16Len = u16Len * 2 + 2;
        u8StringBuf[0] = u16Len;
        u8StringBuf[1] = 0x03;
        pu8UsbSendPtr = u8StringBuf;
        break;

    case USB_DT_INTERFACE:
        // The PC will not send this instruction by default
        break;

    case USB_DT_ENDPOINT:
        // The PC will not send this instruction by default
        break;

    case USB_DT_DEVICE_QUALIFIER:
        PRINTF("USB_DT_DEVICE_QUALIFIER\n");
        pu8UsbSendPtr = pstUsbStdDesp->pstDeviceQualifier.pu8Buff;
        break;

    case USB_DT_HID_REPORT:
        PRINTF("USB_DT_HID_REPORT.interfaceNum:%d\n", Setup[4]);
        pu8UsbSendPtr = pstUsbStdDesp->pstHIDReportDesp.pu8Buff;
        u16Len = pstUsbStdDesp->pstHIDReportDesp.u16Length;
        break;

    default:
        PRINTF("UNKNOW_DT_REQ:%d\r\n", Setup[3]);
        rom_hw_usb_ctrl_device_stall_send_response(EP_ADDR_CONTROL);
        return;
    }

    if (u16Len > (Setup[7] * 256 + Setup[6]))
    {
        u16Len = Setup[7] * 256 + Setup[6];
    }
    debug_printf_hex_string("ctrlSend", pu8UsbSendPtr, u16Len);
    if (rom_hw_usb_ctrl_device_control_send((uint8_t *)pu8UsbSendPtr, u16Len, 10) == 0)
    {
        PRINTF("[ctrlSend]ok\n");
    }
    else
    {
        PRINTF("[ctrlSend]Err\n");
    }
}

/**********************************************************************************************************************
 * @brief  USB device standard request.
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
void usb_device_standard_request(void)
{
    uint8_t u8Resp[8];

    switch (Setup[1])
    {
    case USB_REQ_GET_STATUS:
    {
        PRINTF("USB_REQ_GET_STATUS\r\n");
        u8Resp[0] = 0x00;
        u8Resp[1] = 0x00;
        rom_hw_usb_ctrl_device_control_send((uint8_t *)&u8Resp, 2, 10);
        break;
    }

    case USB_REQ_CLEAR_FEATURE:
    {
        //            u8Resp[0] = 0x00;
        //            u8Resp[1] = 0x00;
        //            rom_hw_usb_ctrl_device_control_send ( ( uint8_t* ) &u8Resp, 2, 10 );
        //            PRINTF ( "USB_REQ_CLEAR_FEATURE\r\n");
        break;
    }

    case USB_REQ_SET_FEATURE:
    {
        PRINTF("USB_REQ_SET_FEATURE\r\n");
        rom_hw_usb_ctrl_device_stall_send_response(Setup[4]);
        break;
    }

    case USB_REQ_SET_ADDRESS:
    {
        PRINTF("USB_REQ_SET_ADDRESS\r\n");
        rom_hw_usb_ctrl_device_addr_set(Setup[2] & 0x7F);
        break;
    }

    case USB_REQ_GET_DESCRIPTOR:
    {
        PRINTF("USB_REQ_GET_DESCRIPTOR\r\n");
        usb_device_descriptor_get();
        break;
    }

    case USB_REQ_SET_DESCRIPTOR:
    {
        PRINTF("USB_REQ_SET_DESCRIPTOR\r\n");
        break;
    }

    case USB_REQ_GET_CONFIGURATION:
    {
        PRINTF("USB_REQ_GET_CONFIGURATION\r\n");
        u8Resp[0] = 0x01;
        rom_hw_usb_ctrl_device_control_send((uint8_t *)&u8Resp, 1, 3);
        break;
    }

    case USB_REQ_SET_CONFIGURATION:
    {
        struct usb_std_desp_t *pstUsbStdDesp = &gsUsbStdDesp;
        PRINTF("USB_REQ_SET_CONFIGURATION\r\n");
        if (pstUsbStdDesp->u8Mode == USB_DEVICE_DONGLE_CDC_AUDIO)
        {
            usb_device_endpoint_reset(USB_DEVICE_DONGLE_CDC_AUDIO);
        }
        else if (pstUsbStdDesp->u8Mode == USB_DEVICE_CDC)
        {
            usb_device_endpoint_reset(USB_DEVICE_CDC);
        }
        break;
    }

    case USB_REQ_GET_INTERFACE:
    {
        PRINTF("USB_REQ_GET_INTERFACE\r\n");
        u8Resp[0] = 0x00;
        rom_hw_usb_ctrl_device_control_send((uint8_t *)&u8Resp, 1, 3);
        break;
    }

    case USB_REQ_SET_INTERFACE:
    {
        PRINTF("USB_REQ_SET_INTERFACE\r\n");
        break;
    }

    case USB_REQ_SYNCH_FRAME:
    {
        PRINTF("USB_REQ_SYNCH_FRAME\r\n");
        break;
    }

    default:
    {
        PRINTF("USB_REQ_SEND_STALL\r\n");
        rom_hw_usb_ctrl_device_stall_send_response(EP_ADDR_CONTROL);
        break;
    }
    }
}

static uint8_t m_u8UartRate[4];
static uint8_t m_u8UartCharFormat = 0;
static uint8_t m_u8UartParityType = 0;
static uint8_t m_u8UartDataBits = 0;
static uint8_t m_u8UsbCdcIsOpen;

static void usb_device_cdc_class_request(uint8_t *pu8Setup)
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

static void usb_device_audio_request(uint8_t *pu8Setup)
{
    uint16_t u16AudioCmd;

    u16AudioCmd = BUILD_UINT16(pu8Setup[1], pu8Setup[0]);

    if (u16AudioCmd == USB_AUDIO_SET_CUR_EP)
    {
        return;
    }

    if (u16AudioCmd == USB_AUDIO_GET_CUR_EP)
    {
        pu8Setup[0] = BREAK_UINT32(AUDIO_SAMPLING_RATE, 0);
        pu8Setup[1] = BREAK_UINT32(AUDIO_SAMPLING_RATE, 1);
        pu8Setup[2] = BREAK_UINT32(AUDIO_SAMPLING_RATE, 2);

        rom_hw_usb_ctrl_device_control_send(pu8Setup, 3, 3);
        return;
    }

    // Setup[3]: Control, Setup[5]: Entity
    if ((pu8Setup[3] == 0x01) && (pu8Setup[5] == AUDIO_SPEAKER_FU_ID))
    {
        switch (u16AudioCmd)
        {
        case USB_AUDIO_GET_CUR:
        {
            pu8Setup[0] = 0;
            rom_hw_usb_ctrl_device_control_send(pu8Setup, 1, 3);
            break;
        }
        case USB_AUDIO_SET_CUR:
        {
            PRINTF("set.sta-?\n");
            break;
        }
        default:
        {
            PRINTF("Err-%s %d\n", __FILE__, __LINE__);
            break;
        }
        }
    }
    else if ((pu8Setup[3] == 0x02) && (pu8Setup[5] == AUDIO_SPEAKER_FU_ID))
    {
        switch (u16AudioCmd)
        {
        case USB_AUDIO_GET_MIN:
        {
            usb_device_send_resp((uint16_t)USB_AUDIO_MIN_VOLUME, 2);
            break;
        }
        case USB_AUDIO_GET_MAX:
        {
            usb_device_send_resp((uint16_t)USB_AUDIO_MAX_VOLUME, 2);
            break;
        }
        case USB_AUDIO_GET_RES:
        {
            usb_device_send_resp((uint16_t)USB_AUDIO_RES_VOLUME, 2);
            break;
        }
        case USB_AUDIO_GET_CUR:
        {
            uint32_t Vol = 0;
            usb_device_send_resp(Vol, 2);
            break;
        }
        case USB_AUDIO_SET_CUR:
        {
            PRINTF("set.cur-ch[%d]?\n", pu8Setup[2]);
            break;
        }
        default:
        {
            PRINTF("Err-%s %d\n", __FILE__, __LINE__);
            break;
        }
        }
    }
    else if ((pu8Setup[3] == 0x01) && (pu8Setup[5] == AUDIO_MIC_FU_ID))
    {
        switch (u16AudioCmd)
        {
        case USB_AUDIO_GET_CUR:
        {
            uint32_t u32AudioMicMuteFlag;
            usb_device_send_resp(u32AudioMicMuteFlag, 1);
            break;
        }
        case USB_AUDIO_SET_CUR:
        {
            PRINTF("set.sta-?\n");
            break;
        }
        default:
        {
            PRINTF("Err-%s %d\n", __FILE__, __LINE__);
            break;
        }
        }
    }
    else if ((pu8Setup[3] == 0x02) && (pu8Setup[5] == AUDIO_MIC_FU_ID))
    {
        switch (u16AudioCmd)
        {
        case USB_AUDIO_GET_MIN:
        {
            usb_device_send_resp((uint16_t)USB_AUDIO_MIN_VOLUME, 2);
            break;
        }
        case USB_AUDIO_GET_MAX:
        {
            usb_device_send_resp((uint16_t)(USB_AUDIO_MAX_VOLUME * 4), 2);
            break;
        }
        case USB_AUDIO_GET_RES:
        {
            usb_device_send_resp((uint16_t)USB_AUDIO_RES_VOLUME, 2);
            break;
        }
        case USB_AUDIO_GET_CUR:
        {
            uint32_t u32Vol = 0;
            usb_device_send_resp(u32Vol * 4, 2);
            break;
        }
        case USB_AUDIO_SET_CUR:
        {
            uint32_t u32Vol = 0;
            usb_device_send_resp(u32Vol * 4, 2);
            break;
        }
        default:
        {
            PRINTF("Err-%s %d\n", __FILE__, __LINE__);
            break;
        }
        }
    }
    else if (pu8Setup[5] == AUDIO_MIC_SL_ID)
    {
        switch (u16AudioCmd)
        {
        case USB_AUDIO_GET_CUR:
        {
            uint16_t u16Temp = 0x01;
            usb_device_send_resp(u16Temp, 1);
            break;
        }
        default:
        {
            PRINTF("Err-%s %d\n", __FILE__, __LINE__);
            break;
        }
        }
    }
    else if (u16AudioCmd == USB_AUDIO_SET_IDLE)
    {
        PRINTF("setIdle\n");
    }
    else
    {
        usb_device_send_resp(0x0000, 1);
    }
}

/**********************************************************************************************************************
 * @brief  USB device class request.
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
void usb_device_class_request(uint8_t *pu8Setup)
{
    struct usb_std_desp_t *pstUsbStdDesp = &gsUsbStdDesp;
    PRINTF("class request.mode:%d,setup[4]:%d\r\n", pstUsbStdDesp->u8Mode, pu8Setup[4]);
    if (pstUsbStdDesp->u8Mode == USB_DEVICE_DONGLE_CDC_AUDIO)
    {
        if (AUDIO_ATL_INTERFACE_NUM == pu8Setup[4])
        {
            usb_device_audio_request(pu8Setup);
        }
        else if (AUDIO_SRM_IN_INTERFACE_NUM == pu8Setup[4])
        {
            PRINTF("AUDIO_SRM_IN_INTERFACE_NUM\n");
        }
        else if (CDC_CTRL_INTERFACE_NUM == pu8Setup[4] || 
                 CDC_DATA_INTERFACE_NUM == pu8Setup[4])
        {
            usb_device_cdc_class_request(pu8Setup);
        }
        else
        {
            usb_device_audio_request(pu8Setup);
        }
    }
    else if (pstUsbStdDesp->u8Mode == USB_DEVICE_CDC)
    {
        usb_device_cdc_class_request(pu8Setup);
    }
    else
    {
        PRINTF("class request mode error\r\n");
    }
}

/**********************************************************************************************************************
 * @brief  USB device manufacturer request.
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
void usb_device_manufactur_request(void)
{
}

/**********************************************************************************************************************
 * @brief  USB device unkown request.
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
void usb_device_other_request(void)
{
    rom_hw_usb_ctrl_device_stall_send_response(EP_ADDR_CONTROL);
}

/**********************************************************************************************************************
 * @brief  start up grate.
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
void usb_start_up_grate(uint32_t u32UpdateResource)
{
}

/**********************************************************************************************************************
 * @brief  Handle control commands from PC
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
void usb_device_request_process(void)
{
    uint8_t u8BusEvent;
    uint32_t u32DataLeng;
    uint8_t u8ReqType;

    u8BusEvent = rom_hw_usb_ctrl_device_bus_event_get();

    if (u8BusEvent & 0x04)
    {
        rom_hw_usb_ctrl_device_addr_set(0);
    }

    if (rom_hw_usb_ctrl_device_setup_receive(Setup, 8, &u32DataLeng) != EN_DEVICE_NONE_ERR)
    {
        return;
    }

    if (Setup[0])
    {
        debug_printf_hex_string("setup", Setup, 8);
    }

    u8ReqType = (Setup[0] & 0x60) >> 5;
    PRINTF("Step1: USB Host Request, Type - %d.", u8ReqType);
    switch (u8ReqType)
    {
    case 0:
        PRINTF("DeviceStandardReq\n");
        usb_device_standard_request();
        break;

    case 1:
        PRINTF("DeviceClassReq\n");
        usb_device_class_request(Setup);
        break;

    case 2:
        PRINTF("DeviceManufacturReq\n");
        usb_device_manufactur_request();
        break;

    case 3:
        PRINTF("DeviceOtherReq\n");
        usb_device_other_request();
        break;
    }
}
