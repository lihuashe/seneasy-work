/**
 * @file     ble_mus.c
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2022/6/9
 * @history
 * @note     Maxscend UART Service
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

#ifndef __BLE_MUS_H__
#define __BLE_MUS_H__

/**
 * @brief  TYPEDEF.
 */
/**@brief Maxscend UART Service event handler type. */
typedef void (*ble_mus_data_handler_t)(uint16_t u16ConnHandle, uint8_t *pu8Data, uint16_t u16DataLen);

/**
 * @brief  Initializing the Maxscend UART Service.
 * @param  pfnDataHandler: Pointer to function for handling received data.
 * @return uint32_t: Return status.
 */
extern uint32_t ble_mus_init(ble_mus_data_handler_t pfnDataHandler);

uint16_t ble_mus_transfer(uint16_t u16ConnHandle, uint8_t *pu8Data, uint16_t u16DataLen);

#endif /* __BLE_MUS_H__ */
