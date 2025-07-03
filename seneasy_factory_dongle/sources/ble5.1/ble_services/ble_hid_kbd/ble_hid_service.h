/**
 * @file     ble_hid_service.h
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2021/09/04
 * @history
 * @note     Human Interface Device Service
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2021, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 */

#ifndef __BLE_HID_SERVICE_H__
#define __BLE_HID_SERVICE_H__

#define KBD_REPORT_NUM       8
#define CONSUMER_REPORT_NUM  8
#define AUDIO_SBC_REPORT_NUM 120

extern uint32_t ble_hid_service_init(void);

bool ble_hid_kbd_report(uint16_t u16ConnHandle, uint8_t *pReport, uint8_t u8DateLen);
bool ble_hid_consumer_report(uint16_t u16ConnHandle, uint16_t *pReport, uint8_t u8DateLen);

#endif
