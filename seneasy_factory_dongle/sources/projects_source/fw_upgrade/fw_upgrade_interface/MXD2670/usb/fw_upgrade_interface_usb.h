/**********************************************************************************************************************
 * @file     fw_upgrade_Interface_usb.h
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

#ifndef __FW_UPGRADE_INTERFACE_USB_H__
#define __FW_UPGRADE_INTERFACE_USB_H__


//=====================================================================================================================
// typedef
//=====================================================================================================================
typedef uint32_t ( *fw_upgrade_interface_usb_callback ) ( uint16_t *pu16TxIndex, uint16_t *pu16RxIndex );

typedef struct
{
    uint8_t* pu8TxBuf;
    uint16_t u16TxBufLen;
    uint8_t* pu8RxBuf;
    uint16_t u16RxBufLen;
    fw_upgrade_interface_usb_callback pfnCB;

} stFwUpgradeInterfaceUartInit_t;




/**
 * @brief  Init io, usb and dma.
 * @param  pstInit: Pointer to a stFwUpgradePhyUartInit_t structure.
 * @return status: @ref EN_ERR_STA_T.
 */
extern uint8_t fw_upgrade_interface_usb_init(stFwUpgradeInterfaceUartInit_t *pstInit);


/**
 * @brief  Deinit usb io.
 */
extern void fw_upgrade_interface_usb_deinit(void);


/**
 * @brief  Re-init usb.
 */
extern void fw_upgrade_interface_usb_reinit(void);


/**
 * @brief  Transmit an amount of data in blocking mode.
 * @param  pu8buf: Pointer to data buffer.
 * @param  u16Len: Amount of data to be sent.
 */
extern void fw_upgrade_interface_usb_send_by_blocking(uint8_t* pu8buf, uint16_t u16Len);


/**
 * @brief  Get tx and rx transmit index.
 * @param  pu16TxIndex: pointer to dma usb tx index.
 * @param  pu16RxIndex: pointer to dma usb rx index.
 */
extern void fw_upgrade_interface_usb_get_dma_trx_index(uint16_t* pu16TxIndex, uint16_t* pu16RxIndex);




#endif /* __FW_UPGRADE_INTERFACE_USB_H__ */

