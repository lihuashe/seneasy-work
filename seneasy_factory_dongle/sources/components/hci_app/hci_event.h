/**
 * @file     hci_event.h
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2023/7/11
 * @history
 * @note
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2023, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 */

#ifndef __HCI_EVENT_H__
#define __HCI_EVENT_H__

// event code
#define CMD_CMPLT_EVT_CODE             (0x0E)

#define MXD_CMD_STATUS_CODE            (0x01)
#define MXD_CNNCT_CMPLT_CODE           (0x02)
#define MXD_DISCNNCT_CMPLT_CODE        (0x03)
#define MXD_ADV_RPT_RESLT_CODE         (0x04)
#define MXD_ENCRYPTION_STATUS_CHG_CODE (0x05)
#define MXD_CNNCT_UPDATE_CODE          (0x06)
#define MXD_MTU_RPT_CODE               (0x08)
#define MXD_NUM_OF_CMPLT_PKTS_CODE     (0x09)
#define MXD_DATA_RPT_CODE              (0x0A)
#define MXD_READY_TO_WORK              (0x0B)
#define MXD_PASSKEY_INPUT_IND          (0x0C)
#define MXD_PASSKEY_DISPLAY_IND        (0x0D)
#define MXD_PASSKEY_DISPLAY_CANCEL_IND (0x0E)
#define MXD_KEYPRESS_NOTIFICATION      (0x0F)
#define MXD_PERI_TIMER_NOTIFY          (0x10)
#define MXD_BOND_DEVICE_RPT            (0x20)
#define MXD_BOND_DEVICE_CMPLT          (0x21)
#define MXD_SPECIFIED_EVENT_CODE       (0x3F)
#define MXD_UNKNOW_PACKET              (0xFF)

/**
 * @brief  Hci app event packet
 * @param  pstEventBuf: Data point which hci event buffer
 * @param  pstEvent: Data point which hci event
 */
extern void hci_event_generate(stHciBuf_t *pstEventBuf, stEvent_t *pstEvent);

#endif /* __HCI_EVENT_H__ */
