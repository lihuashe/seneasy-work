/********************************************************************************************************
 * @file     usbcdc.h
 *
 * @brief    This is the header file for KMD SDK
 *
 * @author	 KMD GROUP
 * @date         01,2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/

#pragma once

#if USB_CDC_ENABLE
#include "../../../../vendor/8278_USB_Demo/usb_default.h"

#include "../../../../drivers.h"
#include "../usbstd/HIDClassCommon.h"
#include "../usbstd/HIDReportData.h"
#include "../usbstd/CDCClassDevice.h"
#include "../usbstd/usbdesc.h"
#include "../types.h"


/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
    extern "C" {
#endif


extern unsigned char usb_cdc_data[CDC_TXRX_EPSIZE];
extern unsigned short usb_cdc_data_len;

extern unsigned int usb_cdc_tx_cnt;
extern unsigned char LineCoding[7];


/**
 * @brief		This function serves to send data to USB host in CDC device
 * @param[in] 	data_ptr -  the pointer of data, which need to be sent.
 * @param[in] 	data_len -  the length of data, which need to be sent.
 * @return 		none
 */
 void usb_cdc_tx_data_to_host(unsigned char * data_ptr, unsigned short data_len);
//void usb_cdc_irq_data_process(void);
void usb_cdc_rx_data_from_host(unsigned char* rx_buff);























/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
    }
#endif
#endif
