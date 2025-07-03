/**********************************************************************************************************************
 * @file     usb.c
 * @author   Maxscend AE Team
 * @version  V1.0
 * @date     2023/01/11
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

#include "ARMCM33_DSP_FP.h"
#include "err_def.h"
#include "RegMXD2670.h"

#include "utility.h"
#include "hw_crg.h"
#include "debug.h"

#include "hw_usb_phy.h"
#include "hw_usb_ctrl.h"
#include "hw_sys_ctrl.h"
#include "hw_usb_dcd.h"
#include "hal_usb.h"
#include "hal_pmu.h"

#include "hw_usb_ctrl.h"
#include "fw_upgrade_process.h"
#include "usb_device_descriptor.h"
#include "usb_device_dfu.h"
#include "usb_device_standard_request.h"

#include "usb_device.h"


//=====================================================================================================================
// DEFINE
//=====================================================================================================================
#define PRINT_ENABLE 1
#if PRINT_ENABLE
#define PRINTF(fmt, args...) printf(fmt, ##args)
#else
#define PRINTF(fmt, args...)
#endif


const uint16_t gcu16USBEpNumMask[] = {0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
                                      0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000
                                     };

struct usb_fp_callback_t gstUsbFpCb[7] =
{
    {0,         NULL},
    {1,         NULL},
    {2,         NULL},
    {3,         NULL},
    {4,         NULL},
    {5,         NULL},
    {6,         NULL},
};

static usb_cdc_recive_cb_t cdc_recive_cb;

/**********************************************************************************************************************
 * @brief  USB module interrupt entry function, where the specific endpoint interrupt is judged,
 * @brief  and then the callback function is executed
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
void USB_IRQ_Handler ( void )
{
    uint8_t u8EpIndex, u8EpNum, u8IntrUSB;
    uint16_t u16Intrx;
    const struct usb_fp_callback_t* pstUsbFpCb = USB_INVAILD_EPCB;

    rom_hw_usb_ctrl_get_interrupt_flag ( &u8EpIndex, &u16Intrx, &u8IntrUSB );

    //PRINTF("usb ctrl intrrupt, u8Index = 0x%02X,u16Intrx = 0x%04X,u8IntrUSB = 0x%02X\r\n",u8EpIndex,u16Intrx,u8IntrUSB);

    u16Intrx &= USB_MASK_EP_INVAILD_INT; //maxscend usb endpoint num max is 7(include EP0)
    for ( u8EpNum = 0; ( u8EpNum < ( USB_REG_FIELD_BIT << 1 ) ) && ( u16Intrx >> u8EpNum ); u8EpNum++ )
    {
        if ( u16Intrx & gcu16USBEpNumMask[u8EpNum] )
        {
            pstUsbFpCb = &gstUsbFpCb[ ( u8EpNum % USB_REG_FIELD_BIT )];
            if ( __builtin_expect ( !! ( pstUsbFpCb == USB_INVAILD_EPCB ), 0 ) )
            {
                continue;
            }
            else if ( __builtin_expect ( !! ( pstUsbFpCb->fpcb == NULL ), 0 ) )
            {
                continue;
            }
            ( *pstUsbFpCb->fpcb ) ( u8EpNum >= USB_REG_FIELD_BIT, pstUsbFpCb->u8EpNum );
        }
    }
}

/**********************************************************************************************************************
 * @brief USB phy interrupt
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
void USB_PHY_IRQ_Handler ( void )
{
    uint32_t u32Mask = 0;
    rom_hw_sys_ctrl_get_usb_phy_int_flag ( &u32Mask );
//    PRINTF("USB_PHY_IRQ_Handler\r\n");
    rom_hw_sys_ctrl_clear_usb_phy_int_flag ( u32Mask );
    u32Mask = 0;
}

/**********************************************************************************************************************
 * @brief  usb int init
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
void usb_clock_init ( void )
{
    //turn on pll to make sure 48MHZ for USB CLK
    rom_hal_pmu_enable_pll_clk ( EN_DCXO_HCLK_FREQ_16M, 2000 );
    rom_hal_usb_clock_enable ( 1 );
}

/**********************************************************************************************************************
 * @brief  usb int init
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
void usb_ctrl_int_init ( void )
{
    rom_hw_sys_ctrl_enable_peri_int ( SYS_CTRL_MP, USB_CTRL_IRQ );
    NVIC_ClearPendingIRQ ( USB_CTRL_IRQ );
    NVIC_SetPriority ( USB_CTRL_IRQ, 0x3 );
    NVIC_EnableIRQ ( USB_CTRL_IRQ );
    
    __enable_irq();
}

/**********************************************************************************************************************
 * @brief  usb int init
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
void usb_phy_int_init ( void )
{
    rom_hw_sys_ctrl_enable_peri_int ( SYS_CTRL_MP, USB_PHY_IRQ );
    rom_hw_sys_ctrl_enable_usb_phy_pd_int();
    
    NVIC_ClearPendingIRQ ( USB_PHY_IRQ );
    NVIC_SetPriority ( USB_PHY_IRQ, 0x3 );
    NVIC_EnableIRQ ( USB_PHY_IRQ );
    
    __enable_irq();
}


/**********************************************************************************************************************
 * @brief  usb_endpoint_callback_register
* @param  epZero :ctrl endpoint cb,epTrx:other endpoint cb
 * @output
 * @return None.
 *********************************************************************************************************************/
void usb_endpoint_callback_register ( FPCALLBACK epCtrl, FPCALLBACK epTrx )
{
    gstUsbFpCb[0].fpcb = epCtrl;
    gstUsbFpCb[1].fpcb = epTrx;
    gstUsbFpCb[2].fpcb = epTrx;
    gstUsbFpCb[3].fpcb = epTrx;
    gstUsbFpCb[4].fpcb = epTrx;
    gstUsbFpCb[5].fpcb = epTrx;
    gstUsbFpCb[6].fpcb = epTrx;
}

/**
 * @brief  USB device trx endpoint handler.
 * @param  u8Type: USB endpoint type, @ref EN_USB_EP_INT_TPYE_T.
 * @param  u8EpNum: Endpoint number.
 */
static void usb_device_trx_endpoint_handle(uint8_t u8Type, uint8_t u8EpNum)
{
    uint32_t u32RcvLen = 0;
    uint8_t  u8RcvData[64];

    if (EN_USB_INT_EP_RX == u8Type)
    {
        if (EP_ADDR_HID_INT_OUT == u8EpNum)
        {
//            usb_device_receive_hid_data(u8RcvData, &u32RcvLen);//yangxun
            debug_printf_hex_string("HID Rcv", u8RcvData, u32RcvLen);
        }
        else if(EP_ADDR_CDC_BULK_OUT == u8EpNum)
        {
            usb_device_recive_cdc_data(u8RcvData, &u32RcvLen);
//            app_debug_printf_hex_string("CDC Rcv", u8RcvData, u32RcvLen);
            // usb_device_send_cdc_data(u8RcvData, u32RcvLen);
//            fw_upgrade_process(u8RcvData, u32RcvLen);
            if (cdc_recive_cb)
            {
                cdc_recive_cb(u8RcvData, u32RcvLen);
            }

        }
    }
    else if (EN_USB_INT_EP_TX == u8Type)
    {
        u8EpNum = u8EpNum | 0x80;

        if (EP_ADDR_HID_INT_IN == u8EpNum)
        {
            // PRINTF("EP_ADDR_HID_IN_INT, HID TX END\r\n");
        }
        else if (EP_ADDR_CDC_BULK_IN == u8EpNum)
        {
            // PRINTF("EP_ADDR_CDC_BULK_IN_INT, CDC TX END\r\n");
        }
    }
}

/**
 * @brief  USB device ctrl endpoint handler.
 * @param  u8Type: USB endpoint type, @ref EN_USB_EP_INT_TPYE_T.
 * @param  u8EpNum: Endpoint number.
 */
static void usb_device_ctrl_endpoint_handle(uint8_t u8Type, uint8_t u8EpNum)
{
    /*EP0,rx and tx share*/
    if (EN_USB_INT_EP_RX == u8Type || EN_USB_INT_EP_TX == u8Type)
    {
        usb_device_request_process();
    }
}

#if USB_HID_EN
/**********************************************************************************************************************
 * @brief  device hid data recive
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
EN_USB_DEVICE_ERR_CODE usb_device_hid_data_recive ( uint8_t *pu8ReceiveBuf, uint32_t *pu32Len )
{
    uint8_t ret = 0;
    ret = rom_hw_usb_ctrl_device_interrupt_receive ( EP_ADDR_HID_INT_OUT, pu8ReceiveBuf, HID_USB_PACKET_SIZE, pu32Len, 5 );
    return ret;
}

/**********************************************************************************************************************
 * @brief  device hid data send
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
EN_USB_DEVICE_ERR_CODE usb_device_hid_data_send ( uint8_t *pu8SendBuf, uint32_t u32Len )
{
    uint8_t ret = 0;
    ret = rom_hw_usb_ctrl_device_interrupt_send ( EP_ADDR_HID_INT_IN, pu8SendBuf, u32Len, 5 );
    return ret;
}

/**********************************************************************************************************************
 * @brief  usb device hid keboard data report
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
EN_USB_DEVICE_ERR_CODE usb_device_hid_keyboard_data_report ( uint8_t *pu8SendBuf, uint32_t u32Len )
{
    EN_USB_DEVICE_ERR_CODE enRet = 0;
    uint8_t pu8HidReportBuf[9] = {};
    pu8HidReportBuf[0] = HID_KEYBOARD_REPORT_ID;
    memcpy(&pu8HidReportBuf[1],pu8SendBuf,u32Len);
    enRet = rom_hw_usb_ctrl_device_interrupt_send ( EP_ADDR_HID_INT_IN, pu8HidReportBuf, 9, 5 );
    return enRet;
}

/**********************************************************************************************************************
 * @brief  usb device hid consumer data report
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
EN_USB_DEVICE_ERR_CODE usb_device_hid_consumer_data_report ( uint8_t *pu8SendBuf, uint32_t u32Len )
{
    EN_USB_DEVICE_ERR_CODE enRet = 0;
    uint8_t pu8HidReportBuf[17] = {};
    pu8HidReportBuf[0] = HID_CONSUMER_REPORT_ID;
    memcpy(&pu8HidReportBuf[1],pu8SendBuf,u32Len);
    enRet = rom_hw_usb_ctrl_device_interrupt_send ( EP_ADDR_HID_INT_IN, pu8HidReportBuf, 17, 5 );
    return enRet;
}
#endif

/**********************************************************************************************************************
 * @brief  usb device send audio mic data
 * @param  None.
 * @output
 * @return None.
 *********************************************************************************************************************/
EN_USB_DEVICE_ERR_CODE usb_device_send_mic_data(uint8_t *pu8Buf, uint32_t u32Len)
{
    uint32_t u32SentLen = u32Len;
    uint32_t u32SentIdx = 0;
    uint8_t u8TimeOutCount = (u32Len<<1);
    if( NULL == pu8Buf )
    {
        return EN_DEVICE_PARA_ERR;
    }
    
    while( 1 )
    {
        if( AUDIO_USB_PACKET_SIZE < u32SentLen )
        {
            if( EN_DEVICE_NONE_ERR == rom_hw_usb_ctrl_device_iso_send(EP_ADDR_AUDIO_ISO_IN, &pu8Buf[u32SentIdx], AUDIO_USB_PACKET_SIZE))
            {
                u32SentIdx+=AUDIO_USB_PACKET_SIZE;
                u32SentLen-=AUDIO_USB_PACKET_SIZE;
            }
        }
        else
        {
            if( EN_DEVICE_NONE_ERR == rom_hw_usb_ctrl_device_iso_send(EP_ADDR_AUDIO_ISO_IN, &pu8Buf[u32SentIdx], u32SentLen))
            {
                break;
            }
        }
        u8TimeOutCount--;
        if( u8TimeOutCount == 0 )
        {
            return EN_ISO_SEND_ERR;
        }
        rom_delay_us(1000);
    }
    
    return EN_DEVICE_NONE_ERR;
}

void usb_cdc_recive_callback_register(usb_cdc_recive_cb_t cb)
{
    cdc_recive_cb = cb;
}

void usb_device_init ( void )
{
    for(uint8_t i=0;i<EN_USB_HOOK_MAX;i++)
    {
        USBCallBackHook[i] = NULL;
    }
    
    rom_hal_usb_clock_reset();
    usb_clock_init();
    usb_phy_int_init();
    usb_ctrl_int_init();
    usb_endpoint_callback_register ( usb_device_ctrl_endpoint_handle, usb_device_trx_endpoint_handle );

    rom_hal_usb_device_init();
    usb_device_set_standard_descriptor ( USB_DEVICE_DONGLE_CDC_AUDIO );
    usb_device_model_select ( USB_DEVICE_DONGLE_CDC_AUDIO, USB_DEVICE_VID, USB_DEVICE_PID );

    rom_hal_usb_device_software_disconnect();
    rom_delay_ms(20);
    rom_hal_usb_device_software_connect();
}


