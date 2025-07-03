/**********************************************************************************************************************
 * @file     patch_gatt_service_api.h
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2023/07/21
 * @history
 * @note     Patch for mxd2670 rom_lib_2
 **********************************************************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2023, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 *********************************************************************************************************************/

#ifndef __PATCH_GATT_SERVICE_API_H__
#define __PATCH_GATT_SERVICE_API_H__

extern void patch_gatts_api_init ( uint8_t *pu8ProfileBuf, uint16_t u16ProfileBufLen ,\
gatts_serv_info_st *pstGattsServInfoBuffer,uint16_t u16GattsServInfoBufferNum);

extern uint16_t patch_gatts_api_add_service_start ( bool is_uuid128, uint16_t uuid16, \
uint8_t *pu8ServUuid128,  gatt_serviceCBs_t  *pServiceCBs );

extern uint16_t patch_gatts_api_add_service_end ( void );

extern uint16_t patch_gatts_api_add_char ( bool is_uuid128, uint16_t uuid16, \
uint8_t *pu8ServUuid128, uint16_t  properties, uint8_t   *data, uint16_t  data_len );


#endif