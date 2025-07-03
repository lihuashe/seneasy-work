/**
 * @file     ble_dis.h
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2021/01/21
 * @history
 * @note     Device information service
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

#ifndef __BLE_DIS_H__
#define __BLE_DIS_H__

/**< Device Information Service Parameters */
#define DEVINFO_SYSTEM_ID           0
#define DEVINFO_MODEL_NUMBER        1
#define DEVINFO_SERIAL_NUMBER       2
#define DEVINFO_FIRMWARE_REV        3
#define DEVINFO_HARDWARE_REV        4
#define DEVINFO_SOFTWARE_REV        5
#define DEVINFO_MANUFACTURER_NAME   6
#define DEVINFO_11073_CERT_DATA     7
#define DEVINFO_PNP_ID              8

/**< IEEE 11073 authoritative body values */
#define DEVINFO_11073_BODY_EMPTY    0
#define DEVINFO_11073_BODY_IEEE     1
#define DEVINFO_11073_BODY_CONTINUA 2
#define DEVINFO_11073_BODY_EXP      254

/**< System ID length */
#define DEVINFO_SYSTEM_ID_LEN       8

/**< PnP ID length */
#define DEVINFO_PNP_ID_LEN          7

/**
 * @brief  Initializes the Device Information service by registering GATT
 *          attributes with the GATT server.
 * @return Error Status.
 */
extern uint8_t ble_dis_init(void);

/**
 * @brief  Set a Device Information parameter.
 * @param  param: Profile parameter ID
 * @param  u8Len: length of data to write
 * @param  pValue: pointer to data to write. This is dependent on the parameter
 *                 ID and WILL be cast to the appropriate data type (example: data
 *                 type of uint16_t will be cast to uint16_t pointer).
 * @return Error Status.
 */
extern uint8_t ble_dis_set_parameter(uint8_t u8Param, uint8_t u8Len, void *pValue);

/**
 * @brief   Get a Device Information parameter.
 * @param   u8Param: Profile parameter ID
 * @param   pValue: pointer to data to get.  This is dependent on the parameter
 *          ID and WILL be cast to the appropriate data type (example: data type
 *          of uint16_t will be cast to uint16_t pointer).
 * @return Error Status.
 */
extern uint8_t ble_dis_get_parameter(uint8_t u8Param, void *pValue);

#endif /* __BLE_DIS_H__ */
