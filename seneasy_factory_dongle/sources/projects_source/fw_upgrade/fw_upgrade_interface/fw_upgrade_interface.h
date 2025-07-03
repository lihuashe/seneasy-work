/**********************************************************************************************************************
 * @file     fw_upgrade_interface.h
 * @version  V1.0
 * @date     2021/3/6
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

#ifndef __FW_UPGRADE_INTERFACE_H__
#define __FW_UPGRADE_INTERFACE_H__


//=====================================================================================================================
// typedef
//=====================================================================================================================
typedef enum
{
    FW_UPGRADE_ITERFACE_UART = 0x01,
    FW_UPGRADE_ITERFACE_SPI  = 0x02,
    FW_UPGRADE_ITERFACE_USB  = 0x03,
    FW_UPGRADE_ITERFACE_BLE  = 0x04,

} enFwUpgradePhyInterface_t;

typedef uint32_t (*fw_upgrade_phy_trx_int_callback)(uint16_t *pu16TxIndex, uint16_t *pu16RxIndex);

typedef struct 
{
    uint8_t *pu8TxBuf;
    uint16_t u16TxBufLen;
    uint8_t *pu8RxBuf;
    uint16_t u16RxBufLen;
    fw_upgrade_phy_trx_int_callback pfnCB;

} stFwUpgradeInterfaceInit_t;




/**
 * @brief  Init io, peripheral.
 * @param  enInterface: Communication interface.
 * @param  pstInit: Pointer to a stFwUpgradePhyInit_t structure.
 */
extern void fw_upgrade_interface_init(enFwUpgradePhyInterface_t enInterface, stFwUpgradeInterfaceInit_t *pstInit);


/**
 * @brief  Deinit tp phy.
 */
extern void fw_upgrade_interface_deinit(void);


/**
 * @brief  Re-init tp phy.
 */
extern void fw_upgrade_interface_reinit(void);


/**
 * @brief  Transmit an amount of data in blocking mode.
 * @param  pu8buf: Pointer to data buffer.
 * @param  u16Len: Amount of data to be sent.
 */
extern void fw_upgrade_interface_send(uint8_t *pu8buf, uint16_t length);


/**
 * @brief  Get tx and rx transmit index.
 * @param  pu16TxIndex: pointer to dma uart tx index.
 * @param  pu16RxIndex: pointer to dma uart rx index.
 */
extern void fw_upgrade_interface_get_dma_trx_index(uint16_t *pu16TxIndex, uint16_t *pu16RxIndex);


#endif /* __FW_UPGRADE_INTERFACE_H__ */

