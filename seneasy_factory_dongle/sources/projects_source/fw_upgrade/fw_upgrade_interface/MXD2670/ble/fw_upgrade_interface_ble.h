/**********************************************************************************************************************
 * @file     fw_upgrade_Interface_ota.h
 * @version  V3.0
 * @date     2022/07/29
 * @history
 * @note     OTA Service
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


#ifndef __FW_UPGRADE_INTERFACE_INTERFACE_H_
#define __FW_UPGRADE_INTERFACE_INTERFACE_H_

//=====================================================================================================================
// DEFINE
//=====================================================================================================================
#define MXD_BLE_OTA_SERVICE_UUID           (0xFD10)
#define MXD_BLE_OTA_WRITE_UUID             (0xFD11)
#define MXD_BLE_OTA_NOTIFY_UUID            (0xFD12)


//=====================================================================================================================
// TYPEDEF
//=====================================================================================================================
//typedef struct __attribute__((aligned(4)))
//{
//    // Information.
//    uint16_t u16MXDFlag;               // MXD Flag: 2A4B, customer can not change.
//    uint8_t  u8RomVer;                 // Rom version.
//    uint8_t  u8Boot2Ver;               // Secondary boot version.
//    uint32_t u32AppVer;                // Application version.
//    uint32_t u32NeedOtaFlag;           // Need upgrade or not.

//    // Firmware upgrade saved information.
//    uint32_t u32BinFlashAddrOffset;    // The offset address of bin code saved in flash.
//    uint32_t u32BinCodeSize;           // Bin code size
//    uint32_t u32BinCodeCrc;            // Bin code crc16

//    // Current running code region.
//    uint32_t u32RunningCodeAddrOffset; // Current running code offset address.
//    uint32_t u32Boo2InfoCrc;           // Struct information crc16.

//} stBoot2Info_t;


// Ble ota service received data callback function.
typedef void (*ble_ota_service_receive_data_callback_t)(uint8_t *pu8Buf, uint16_t u16Len);




/**
 * @brief  ble ota service init.
 * @return status, @ref
 */
extern uint32_t ble_ota_service_init(ble_ota_service_receive_data_callback_t pfnCallback);


/**
 * @brief  Send notification data.
 * @param  u16Handle: Ble connected handle.
 * @param  pu8Buf: Pointer to write buffer.
 * @param  u16Len: Buffer length.
 * @return status.
 */
extern uint32_t ble_ota_service_send_notify(uint16_t u16ConnHandle, uint8_t *pu8Buf, uint16_t u16Len);



#endif /* __FW_UPGRADE_INTERFACE_INTERFACE_H_ */

