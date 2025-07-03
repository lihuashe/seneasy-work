/*
MLOG_ID_RANGE: 0x5297~0x52FF
MLOG_ID_USED:  0x5297~0x52A8
*/
/**********************************************************************************************************************
 * @file     patch_gatts_api.c
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2023/07/21
 * @history
 * @note     Patch_for_mxd2670_rom_lib_2
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

/* // New Mlog Range 5280~52ff,  used 5296~~~
MLOG_ID_RANGE: 0x5280~0x52FF
MLOG_ID_USED:  0x5280~0x5296
*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "bluetooth.h"
#include "host_stack_conn_hdl.h"
#include "gap_def.h"

#include "gatt_service_api.h"
#include "patch_gatt_service_api.h"// New patch

#include "att_db_util.h"
#include "le_device_db.h"
#include "att_server.h"
#include "gatt_client_api.h"
#include "stack_utility.h"

#include "gap_api.h"
#include "gatt_uuid_def.h"
#include "ble_callback.h"
#include "mlog.h"

 


typedef struct
{
    uint16_t      start_handle;
    uint16_t      end_handle;

} serv_hdl_st;

static gatts_serv_info_st *gpst_gatt_serv_info=NULL ;
static uint16_t g_u16_gatts_serv_info_num=0;
// #define ATT_CHARACTERISTIC_GAP_DEVICE_NAME_VALUE_HANDLE 0x0003
// #define ATT_CHARACTERISTIC_GAP_APPEARANCE_VALUE_HANDLE 0x0005
// #define ATT_CHARACTERISTIC_GATT_SERVER_CHANGE_VALUE_HANDLE 0x000E
// #define ATT_CHARACTERISTIC_GATT_SERVER_CHANGE_CLIENT_CFG_HANDLE 0x000F
// const uint8_t gap_gatt_profile_data[]  =
// {
//     // 0x0001 PRIMARY_SERVICE-GAP_SERVICE
//     0x0a, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x28, 0x00, 0x18,
//     // 0x0002 CHARACTERISTIC-GAP_DEVICE_NAME-READ | WRITE | DYNAMIC
//     0x0d, 0x00, 0x02, 0x00, 0x02, 0x00, 0x03, 0x28, 0x02, 0x03, 0x00, 0x00, 0x2a,
//     // 0x0003 VALUE-GAP_DEVICE_NAME-READ | WRITE | DYNAMIC-''
//     0x08, 0x00, 0x02, 0x01, 0x03, 0x00, 0x00, 0x2a,
//     // 0x0008 CHARACTERISTIC-GAP_APPEARANCE-READ | WRITE | DYNAMIC
//     0x0d, 0x00, 0x02, 0x00, 0x04, 0x00, 0x03, 0x28, 0x02, 0x05, 0x00, 0x01, 0x2a,
//     // 0x0009 VALUE-GAP_APPEARANCE-READ | WRITE | DYNAMIC-''
//     0x08, 0x00, 0x02, 0x01, 0x05, 0x00, 0x01, 0x2a,
// #if 0
//     // 0x0004 CHARACTERISTIC-GAP_PERIPHERAL_PRIVACY_FLAG-READ | WRITE | DYNAMIC
//     0x0d, 0x00, 0x02, 0x00, 0x06, 0x00, 0x03, 0x28, 0x0a, 0x07, 0x00, 0x02, 0x2a,
//     // 0x0005 VALUE-GAP_PERIPHERAL_PRIVACY_FLAG-READ | WRITE | DYNAMIC-''
//     0x08, 0x00, 0x0a, 0x01, 0x07, 0x00, 0x02, 0x2a,
//     // 0x000a CHARACTERISTIC-GAP_RECONNECTION_ADDRESS-READ | WRITE | DYNAMIC
//     0x0d, 0x00, 0x02, 0x00, 0x08, 0x00, 0x03, 0x28, 0x08, 0x09, 0x00, 0x03, 0x2a,
//     // 0x000b VALUE-GAP_RECONNECTION_ADDRESS-READ | WRITE | DYNAMIC-''
//     0x08, 0x00, 0x08, 0x01, 0x09, 0x00, 0x03, 0x2a,
//     // 0x0006 CHARACTERISTIC-GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS-READ
//     0x0d, 0x00, 0x02, 0x00, 0x0a, 0x00, 0x03, 0x28, 0x02, 0x0b, 0x00, 0x04, 0x2a,
//     // 0x0007 VALUE-GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS-READ-'FF FF FF 00 03 FF FF'
//     0x10, 0x00, 0x02, 0x00, 0x0b, 0x00, 0x04, 0x2a, 0x10, 0x00, 0x10, 0x00, 0x13, 0x00, 0xc8, 0x00,
// #endif

//     // 0x000c PRIMARY_SERVICE-GATT_SERVICE
//     0x0a, 0x00, 0x02, 0x00, 0x0c, 0x00, 0x00, 0x28, 0x01, 0x18,
//     // 0x000d CHARACTERISTIC-GATT_SERVICE_CHANGED-Indication
//     0x0d, 0x00, 0x02, 0x00, 0x0d, 0x00, 0x03, 0x28, 0x20, 0x0e, 0x00, 0x05, 0x2a,
//     // 0x000e VALUE-GATT_SERVICE_CHANGED-READ-''
//     0x08, 0x00, 0x20, 0x10, 0x0e, 0x00, 0x05, 0x2a,
//     //  CLIENT_CHARACTERISTIC_CONFIGURATION
//     0x08, 0x00, 0x0a, 0x01, 0x0f, 0x00, 0x02, 0x29,


// };

 
// GAP Profile - Name attribute for SCAN RSP data
uint32_t patch_gatt_find_server ( uint16_t  attribute_handle )
{
    int i = 0;
    for ( i = 0; i < g_u16_gatts_serv_info_num; i++ )
    {
        if ( ( gpst_gatt_serv_info[i].start_handle <= attribute_handle )
                && ( gpst_gatt_serv_info[i].end_handle >= attribute_handle ) )
        {
            break;
        }
    }
    if ( gpst_gatt_serv_info[i].pfnCB == NULL )
    {
        return g_u16_gatts_serv_info_num;
    }
    return i;
}
uint16_t patch_mxd_att_get_attribute_length_callback ( uint16_t conn_handle, uint16_t attribute_handle )
{
    uint16_t  att_value_len = 0;
    int i = 0;
    i = patch_gatt_find_server ( attribute_handle );
    if ( g_u16_gatts_serv_info_num == i )
    {
        return 0;
    }

    if ( NULL != gpst_gatt_serv_info[i].pfnCB->pfn_get_attribute_len_cb )
    {
        return gpst_gatt_serv_info[i].pfnCB->pfn_get_attribute_len_cb ( conn_handle, attribute_handle );
    }
    return 0;
}
uint16_t patch_mxd_att_read_callback ( uint16_t conn_handle, uint16_t attribute_handle, uint16_t offset, uint8_t *buffer, uint16_t buffer_size )
{

//  uint16_t  att_value_len = 0;
    int i = 0;

    i = patch_gatt_find_server ( attribute_handle );
    if ( g_u16_gatts_serv_info_num == i )
    {
        return 0;
    }
    if ( NULL != gpst_gatt_serv_info[i].pfnCB->pfn_read_attr_cb )
    {
        return gpst_gatt_serv_info[i].pfnCB->pfn_read_attr_cb ( conn_handle, attribute_handle,   offset,   buffer,   buffer_size );
    }
    return 0;


}

// write requests
uint32_t patch_mxd_att_write_callback ( uint16_t conn_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size )
{
    uint32_t i = 0;

    i = patch_gatt_find_server ( attribute_handle );
    if ( g_u16_gatts_serv_info_num == i )
    {
        return 0;
    }
    if ( NULL != gpst_gatt_serv_info[i].pfnCB->pfn_write_attr_cb )
    {
        return gpst_gatt_serv_info[i].pfnCB->pfn_write_attr_cb ( conn_handle, attribute_handle, transaction_mode,  offset,   buffer,   buffer_size );
    }

    return 0;
}


void patch_gatts_api_init ( uint8_t *pu8ProfileBuf, uint16_t u16ProfileBufLen ,gatts_serv_info_st *pstGattsServInfoBuffer,uint16_t u16GattsServInfoBufferNum)
{
    if(NULL == pstGattsServInfoBuffer || NULL ==pu8ProfileBuf || 0==u16ProfileBufLen || 0==u16GattsServInfoBufferNum)
    {
        // setup le device db
        MLOG0E ( 0x5297, "patch_gatts_api_init  pstGattsServInfoBuffer or pu8ProfileBuf not OK, pls check"  );
        return;        
    }
    gpst_gatt_serv_info = pstGattsServInfoBuffer;
    g_u16_gatts_serv_info_num = u16GattsServInfoBufferNum;
    
    for ( uint8_t i = 0; i < u16GattsServInfoBufferNum; i++ )
    {
        gpst_gatt_serv_info[i].start_handle = 0 ;
        gpst_gatt_serv_info[i].end_handle = 0 ;
    }

    rom_att_db_util_init ( pu8ProfileBuf,  u16ProfileBufLen, NULL, 0, 0x00 );

    // setup le device db
    MLOGD ( 0x5298, "gatts_init  profile_buflen=%d", u16ProfileBufLen );


}



uint16_t patch_gatts_api_add_service_start ( bool is_uuid128, uint16_t uuid16, uint8_t *pu8ServUuid128,  gatt_serviceCBs_t  *pServiceCBs )
{
    uint16_t start_hdl = 0;
    uint32_t i = 0;
    for ( i = 0; i < g_u16_gatts_serv_info_num; i++ )
    {
        if ( gpst_gatt_serv_info[i].start_handle == 0 )
        {
            break;
        }
    }
    if ( g_u16_gatts_serv_info_num == i )
    {
        MLOGE ( 0x5299, "gatts_api_add_service_start. server is full =%d ", i );
        return 0;
    }
    if ( is_uuid128 )
    {
        if(pu8ServUuid128 ==  NULL)
        {
            MLOG0E ( 0x529a, "patch_gatts_api_add_service_start. pu8ServUuid128== NULL");
            return 0;
        }
        start_hdl = rom_att_db_util_add_service_uuid128 ( pu8ServUuid128,GATT_PRIMARY_SERVICE_UUID );
        MLOGXD ( 0x529b, "gatts_api_add_service_start. uuid128=", pu8ServUuid128, 16 );
    }
    else
    {
        start_hdl = rom_att_db_util_add_service_uuid16 ( uuid16,GATT_PRIMARY_SERVICE_UUID );
        MLOGD ( 0x529c, "gatts_api_add_service_start. uuid16bit=0x%x ", uuid16 );
    }
    if ( 0 == start_hdl )
    {
        return 0;
    }
    gpst_gatt_serv_info[i].start_handle = start_hdl;
    gpst_gatt_serv_info[i].end_handle = 0;
    gpst_gatt_serv_info[i].pfnCB = pServiceCBs;


    MLOG1616D ( 0x529d, "gatts_api_add_service_start. start handle=0x%x  server index=%d", start_hdl, i );

    return start_hdl;
}

uint16_t patch_gatts_api_add_second_service_start ( bool is_uuid128, uint16_t uuid16, uint8_t *pu8ServUuid128,  gatt_serviceCBs_t  *pServiceCBs )
{
    uint16_t start_hdl = 0;
    uint32_t i = 0;
    for ( i = 0; i < g_u16_gatts_serv_info_num; i++ )
    {
        if ( gpst_gatt_serv_info[i].start_handle == 0 )
        {
            break;
        }
    }
    if ( g_u16_gatts_serv_info_num == i )
    {
        MLOGE ( 0x529e, "patch_gatts_api_add_second_service_start. server is full =%d ", i );
        return 0;
    }
    if ( is_uuid128 )
    {
        if(pu8ServUuid128 ==  NULL)
        {
            MLOG0E ( 0x529f, "patch_gatts_api_add_second_service_start. pu8ServUuid128== NULL");
            return 0;
        }
        start_hdl = rom_att_db_util_add_service_uuid128 ( pu8ServUuid128,GATT_SECONDARY_SERVICE_UUID );
        MLOGXD ( 0x52a0, "patch_gatts_api_add_second_service_start. uuid128=", pu8ServUuid128, 16 );
    }
    else
    {
        start_hdl = rom_att_db_util_add_service_uuid16 ( uuid16, GATT_SECONDARY_SERVICE_UUID );
        MLOGD ( 0x52a1, "patch_gatts_api_add_second_service_start. uuid16bit=0x%x ", uuid16 );
    }
    if ( 0 == start_hdl )
    {
        return 0;
    }
    gpst_gatt_serv_info[i].start_handle = start_hdl;
    gpst_gatt_serv_info[i].end_handle = 0;
    gpst_gatt_serv_info[i].pfnCB = pServiceCBs;


    MLOG1616D ( 0x52a2, "patch_gatts_api_add_second_service_start. start handle=0x%x  server index=%d", start_hdl, i );

    return start_hdl;
}
uint16_t patch_gatts_api_add_service_end ( void )
{
    // uint16_t start_hdl=0;
    uint32_t i = 0;
    for ( i = 0; i < g_u16_gatts_serv_info_num; i++ )
    {
        if ( ( gpst_gatt_serv_info[i].end_handle == 0 ) && gpst_gatt_serv_info[i].start_handle )
        {
            break;
        }
    }
    if ( g_u16_gatts_serv_info_num == i )
    {
        MLOGE ( 0x52a3, "gatts_api_add_service_end. not find, server is full =%d ", i );
        return 0;
    }


    gpst_gatt_serv_info[i].end_handle =   rom_att_db_util_get_last_handle();

    MLOG1616D ( 0x52a4, "gatts_api_add_service_end[i].end_handle=0x%x  att_db_size=%d", gpst_gatt_serv_info[i].end_handle, rom_att_db_util_get_size ( ) );
    return gpst_gatt_serv_info[i].end_handle;
}

uint16_t patch_gatts_api_add_char ( bool is_uuid128, uint16_t uuid16, uint8_t *pu8ServUuid128, uint16_t  properties, uint8_t   *data, uint16_t  data_len )
{
    uint16_t ret = 0;
    if ( is_uuid128 )
    {
        if(pu8ServUuid128 ==  NULL)
        {
            MLOG0E ( 0x52a5, "rom_gatts_api_add_include_service. pu8ServUuid128== NULL");
            return 0;
        }
        ret =  rom_att_db_util_add_characteristic_uuid128 ( pu8ServUuid128, properties,  data,    data_len );
        MLOGXD ( 0x52a6, "gatts_api_add_char. uuid128= ", pu8ServUuid128, 16 );
    }
    else
    {
        ret =  rom_att_db_util_add_characteristic_uuid16 ( uuid16, properties,  data,    data_len );
        MLOGD ( 0x52a7, "gatts_api_add_char. uuid16=0x%x ", uuid16 );
    }
    MLOG1616D ( 0x52a8, "gatts_api_add_char handle=0x%x properties=0x%x ", ret, properties );

    return ret;

}

