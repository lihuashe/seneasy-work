/**********************************************************************************************************************
 * @file     ble_peripheral.c
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2022/02/13
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
 * Copyright (c) 2016~2022, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 *********************************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "gatt_uuid_def.h"
#include "gatt_client_api.h"
#include "gap_api.h"

#include "app_cfg.h"
#include "utility.h"

#include "bluetooth.h"
#include "ble_event.h"
#include "ble_central.h"

#define ST_SERVICE_128UUID_LEN         (22)        /**<The structure of service 128UUID len*/
#define ST_SERVICE_16UUID_LEN          (8)         /**<The structure of The service 16UUID len*/
#define ST_INC_SERVICE_128UUID_LEN     (24)        /**<The structure of The include service 128UUID len*/
#define ST_INC_SERVICE_16UUID_LEN      (10)        /**<The structure of The include service 16UUID len*/
#define ST_CHAR_128UUID_LEN            (23)        /**<The structure of The characterstic 128UUID len*/
#define ST_CHAR_16UUID_LEN             (9)         /**<The structure of The characterstic 16UUID len*/

#define SERVICE_TYPE                (0x80)      /**<The service TYPE*/
#define INC_SERVICE_TYPE            (0x82)      /**<The include service TYPE*/
#define CHAR_TYPE                   (0x83)      /**<The characteristic TYPE*/

#define CLIENT_DATA_BUF_SIZE 512

uint8_t  gu8ClientDataBuf[CLIENT_DATA_BUF_SIZE]  ;          //save data of service
uint8_t *gpu8ClientData = NULL;                     //client db mem pointer
uint16_t gu16ClientDataSize = 0;

uint8_t gu8ClientServiceCnt = 0;                 //service counter
uint8_t gu8ClientCharCnt = 0;                    //char counter

uint16_t gu16FindServiceIdx = 0;
uint16_t gu16FindCharIdx = 0;

uint8_t u8CharReadBuf[128];

ble_central_event_callback_t ble_central_event_callback;
ble_central_char_read_callback_t ble_central_char_read_callback;
ble_central_char_write_callback_t ble_central_char_write_callback;

/**
 * @brief  Add a service
 * @param  per add serv: len[8 or 22], 80, start hdl[2], end hdl[2], uuid[2 or 16]
 * @return SERVICE SIZE
 */
static uint16_t ble_central_add_a_service(gatt_client_service_t *per)
{
    uint8_t *pu8Data;
    if ( ( NULL == gpu8ClientData ) || ( ( gu16ClientDataSize + ST_SERVICE_128UUID_LEN ) > CLIENT_DATA_BUF_SIZE ) )
    {
        return 0;
    }

    pu8Data = gpu8ClientData + gu16ClientDataSize;

    pu8Data[1] = SERVICE_TYPE;
    rom_little_endian_store_16 ( pu8Data, 2, per->start_group_handle );
    rom_little_endian_store_16 ( pu8Data, 4, per->end_group_handle );

    if ( 0 == per->uuid16 )
    {
        pu8Data[0] = ST_SERVICE_128UUID_LEN; //128uuid
        memcpy ( pu8Data + 6, per->uuid128, 16 );
    }
    else
    {
        pu8Data[0] = ST_SERVICE_16UUID_LEN; //16uuid
        rom_little_endian_store_16 ( pu8Data, 6, per->uuid16 );
    }
    gu16ClientDataSize +=  pu8Data[0];
//    MLOGD ( 0x9101, "ble_central_add_a_service uuid=0x%x", per->uuid16 );

    return pu8Data[0];
}

/**
 * @brief  Add a characteristic
 * @param  per add char: len[9 or 23], 83, prop, start hdl[2], end hdl[2], uuid[2 or 16]
 * @return Characteristic len
 */
static uint16_t ble_central_add_a_characteristic(gatt_client_characteristic_t *per)
{
    uint8_t *pu8Data;
    if ( (NULL == gpu8ClientData) || (gu16ClientDataSize + ST_CHAR_128UUID_LEN) > CLIENT_DATA_BUF_SIZE )
    {
        return 0;
    }
    pu8Data = gpu8ClientData + gu16ClientDataSize;
    pu8Data[1] = CHAR_TYPE;

    pu8Data[2] = ( uint8_t ) ( per->properties & 0xff );
    rom_little_endian_store_16 ( pu8Data, 3, per->value_handle );
    rom_little_endian_store_16 ( pu8Data, 5, per->end_handle );

    if ( 0 == per->uuid16 )
    {
        pu8Data[0] = ST_CHAR_128UUID_LEN; //128uuid
        memcpy ( pu8Data + 7, per->uuid128, 16 );
    }
    else
    {
        pu8Data[0] = ST_CHAR_16UUID_LEN; //16uuid
        rom_little_endian_store_16 ( pu8Data, 7, per->uuid16 );
    }
    gu16ClientDataSize += pu8Data[0];
//    MLOGD ( 0x9103, "ble_central_add_a_characteristic uuid=0x%x", per->uuid16 );

    return pu8Data[0];
}

/**
 * @brief  Add a characteristic descriptor
 * @param  u8Property: Indicates properties of this characteristic descriptor.
 * @param  u16Handle: Characteristic descriptor Handle.
 * @param  u16Uuid: Characteristic descriptor uuid.
 * @return Characteristic descriptor len.
 */
static uint16_t ble_central_add_a_char_descriptor(uint8_t u8Property, uint16_t u16Handle, uint16_t u16Uuid)
{
    uint8_t *pu8Data;
    if ( ( NULL == gpu8ClientData ) || ( ( gu16ClientDataSize + 5 ) > CLIENT_DATA_BUF_SIZE ) || ( u16Uuid < GATT_CHAR_EXT_PROPS_UUID ) )
    {
        return 0;
    }
    pu8Data = gpu8ClientData + gu16ClientDataSize;
    pu8Data[0] = 5;
    pu8Data[1] = u16Uuid - GATT_CHAR_EXT_PROPS_UUID + 0x90;  //VAL RANGE [0x90:0x95] GATT_CHAR_EXT_PROPS_UUID:0x2900
    pu8Data[2] = u8Property;
    rom_little_endian_store_16 ( pu8Data, 3,  u16Handle );
    gu16ClientDataSize += pu8Data[0];
//    MLOGD ( 0x9104, "ble_central_add_a_char_descriptor uuid=0x%x", u16Uuid );

    return pu8Data[0];
}

/**
 * @brief  Get a item from client data buf.
 * @param  u8Type: 0x80 for service, 0x83 for charateristic,0x90-0x95 for char descrip
 * @param  u16Idx: Index of this type item in the client data buf, idx should big than 0(idx>0)
 * @return NULL or !NULL
 */
static uint8_t *ble_central_get_a_item(uint8_t u8Type, uint16_t u16Idx)
{
    uint8_t u8Len = 0;
    uint16_t u16Pos = 0;
    uint16_t u16Cnt = 0;
    uint8_t *pu8Buf = NULL;

    if ( NULL == gpu8ClientData )
    {
        return NULL;
    }

    while ( u16Pos < gu16ClientDataSize )
    {
        u8Len = gpu8ClientData[u16Pos];

        if ( u8Len < 5 ) //5 is the min size in client data base
        {
            return NULL;
        }

        pu8Buf = ( uint8_t * ) ( gpu8ClientData + u16Pos );

        if ( u8Type == pu8Buf[1] )
        {
            u16Cnt++;
            if ( u16Cnt == u16Idx )
            {
                return pu8Buf;
            }
        }
        u16Pos += u8Len;
    }
    return NULL;
}

/**
 * @brief  Get a primary service.
 * @param  pstService: Pointer to the primary service to get.
 * @param  u16Idx: Index of primary service.
 * @return true or false.
 */
static bool ble_central_get_a_service(gatt_client_service_t *pstService, uint16_t u16Idx)
{
    uint8_t *pu8Buff = NULL;

    pu8Buff = ble_central_get_a_item (SERVICE_TYPE, u16Idx);
    if(NULL == pu8Buff)
    {
        return false;
    }
    if(NULL != pu8Buff)
    {
        uint8_t len = pu8Buff[0];
        pstService->start_group_handle = rom_little_endian_read_16 (pu8Buff, 2);
        pstService->end_group_handle = rom_little_endian_read_16 (pu8Buff, 4);
        if(ST_SERVICE_128UUID_LEN == len)
        {
            memcpy(pstService->uuid128, pu8Buff + 6, 16);
        }
        else if(ST_SERVICE_16UUID_LEN == len)
        {
            pstService->uuid16 = rom_little_endian_read_16 (pu8Buff, 6);
        }
    }
    return true;
}

/**
 * @brief  Get a characteristic.
 * @param  pstChar: Pointer to the characteristic to get.
 * @param  u16Idx: Index of characteristic.
 * @param  u8IsWithDes: Indicates the characteristic has descriptor or not.
 * @return true or false.
 */
static bool ble_central_get_a_char_with_description(gatt_client_characteristic_t *pstChar, uint16_t u16Idx, uint8_t u8IsWithDes)
{
    uint8_t *pu8Buff = NULL;
    uint8_t u8Len = 0;
    uint16_t u16Pos = 0;
    uint16_t u16Cnt = 0;

    if(NULL == gpu8ClientData)
    {
        return false;
    }

    while(u16Pos < gu16ClientDataSize)
    {
        u8Len = gpu8ClientData[u16Pos];
        if(u8Len < 5)
        {
            return false;
        }

        pu8Buff = gpu8ClientData + u16Pos;
        if(CHAR_TYPE == pu8Buff[1])
        {
            pstChar->value_handle = rom_little_endian_read_16 (pu8Buff, 3);
            pstChar->end_handle = rom_little_endian_read_16 (pu8Buff, 5);
            pstChar->uuid16 = rom_little_endian_read_16 (pu8Buff, 7);

            if(u8IsWithDes)
            {
                if(pstChar->end_handle > pstChar->value_handle)
                {
                    u16Cnt++;
                }
            }
            else
            {
                u16Cnt++;
            }

            if(u16Cnt == u16Idx)
            {
                pstChar->properties = pu8Buff[2];
                if(ST_CHAR_128UUID_LEN == u8Len)
                {
                    memcpy(pstChar->uuid128, pu8Buff + 7, 16);
                }
                else if(ST_SERVICE_16UUID_LEN == u8Len)
                {
                    pstChar->uuid16 = rom_little_endian_read_16(pu8Buff, 7);
                }
                return true;
            }
        }
        u16Pos += u8Len;
    }
    return false;
}

/**
 * @brief  Get a characteristic descriptor handle.
 * @param  char_uuid: characteristic UUID.
 * @param  char_desp_uuid: characteristic descriptor UUID.
 * @param  char_desp_hdl: Pointer to the characteristic descriptor handle to get.
 * @return true or false.
 */
bool ble_central_get_a_char_desp_for_a_char(uint16_t char_uuid, uint16_t char_desp_uuid, uint16_t *char_desp_hdl)
{
    uint8_t *pbuff = NULL;
    uint16_t hdl = 0;
    uint16_t idx = 0;
    gatt_client_characteristic_t    character;

    if ( ( char_desp_uuid > 0x2908 ) || ( char_desp_uuid < 0x2900 ) )
    {
        return false;
    }

    char_desp_uuid = ( char_desp_uuid - 0x2900 ) + 0x90;

    idx = 1;
    character.uuid16 = 0;
    do
    {
        pbuff = ble_central_get_a_item ( CHAR_TYPE, idx );
        if ( NULL == pbuff )
        {
            return false;
        }
        if ( ( NULL != pbuff ) && ( pbuff[0] == ST_CHAR_16UUID_LEN ) )
        {
            character.value_handle = rom_little_endian_read_16 ( pbuff, 3 );
            character.end_handle = rom_little_endian_read_16 ( pbuff, 5 );
            character.uuid16 = rom_little_endian_read_16 ( pbuff, 7 );
            if ( char_uuid == character.uuid16 )
            {
                break;
            }
        }
        character.uuid16 = 0;
        idx++;
    }
    while ( NULL != pbuff );

    if ( ( 0 == character.uuid16 ) || ( character.value_handle >= character.end_handle ) )
    {
        return false;
    }

    idx = 1;
    hdl = 0;
    do
    {
        pbuff = ble_central_get_a_item ( char_desp_uuid, idx );
        if ( NULL == pbuff )
        {
            return false;
        }
        if ( ( NULL != pbuff ) && ( pbuff[0] == 5 ) )
        {
            hdl = rom_little_endian_read_16 ( pbuff, 3 );
            if ( ( hdl > character.value_handle ) && ( hdl <= character.end_handle ) )
            {
                break;
            }
        }
        hdl = 0;
        idx++;
    }
    while ( NULL != pbuff );

    if ( 0 == hdl )
    {
        return false;
    }
    *char_desp_hdl = hdl;

    return true;
}

/**
 * @brief  Parse discovered service.
 * @param  u16ConnHandle: Connection handle.
 * @param  u16OpCode: Opcode.
 * @param  u16ErrCode: Error code.
 * @param  u16OpCode: Opcode.
 * @param  pu8Buf: Service data buffer.
 * @param  u16Len: Buffer Len.
 * @return None.
 */
static void ble_central_parse_service(uint16_t u16ConnHandle, uint16_t u16OpCode, uint16_t u16ErrCode, uint8_t *pu8Buf, uint16_t u16Len)
{
    uint16_t u16Ret = 0;
    if ((u16ErrCode & 0x8000) == 0 && (u16ErrCode & 0x00FF) == 0 && (pu8Buf != NULL))
    {
        u16Ret = ble_central_add_a_service((gatt_client_service_t*)pu8Buf);
        if(u16Ret)
        {
            gu8ClientServiceCnt++;
        }
        else
        {
            PRINTF("Add service fail\n");
        }
    }
    else if((u16ErrCode & 0x8000) != 0)
    {
        PRINTF("Service find finish %d\n", gu8ClientServiceCnt);
        gatt_client_service_t stService;
        gu16FindServiceIdx = 1;
        if(ble_central_get_a_service(&stService, gu16FindServiceIdx))
        {
            rom_gatt_client_api_discover_characteristics_for_service ( u16ConnHandle, &stService );
            gu16FindServiceIdx++;
        }
    }
}

/**
 * @brief  Parse discovered characteristic.
 * @param  u16ConnHandle: Connection handle.
 * @param  u16OpCode: Opcode.
 * @param  u16ErrCode: Error code.
 * @param  u16OpCode: Opcode.
 * @param  pu8Buf: Characteristic data buffer.
 * @param  u16Len: Buffer Len.
 * @return None.
 */
static void ble_central_parse_characteristic(uint16_t u16ConnHandle, uint16_t u16OpCode, uint16_t u16ErrCode, uint8_t *pu8Buf, uint16_t u16Len)
{
    uint16_t u16Ret = 0;
    if((u16ErrCode & 0x8000) == 0 && (u16ErrCode & 0x00FF) == 0 && (pu8Buf != NULL))
    {
        u16Ret = ble_central_add_a_characteristic(( gatt_client_characteristic_t*)pu8Buf);
        if(u16Ret)
        {
            gu8ClientCharCnt++;
        }
        else
        {
            PRINTF("Add char fail\n");
        }
    }
    else if((u16ErrCode & 0x8000) != 0)
    {
        // The char of last service find end, start next
        if(gu16FindServiceIdx <= gu8ClientServiceCnt)
        {
            gatt_client_service_t stService;
            if(ble_central_get_a_service(&stService, gu16FindServiceIdx))
            {
                rom_gatt_client_api_discover_characteristics_for_service(u16ConnHandle, &stService);
                gu16FindServiceIdx++;
            }
        }
        else // Find all char start to find char des
        {
            PRINTF("Char find finish %d\n", gu8ClientCharCnt);
            gatt_client_characteristic_t stChar;
            memset(&stChar, 0, sizeof(stChar));
            gu16FindCharIdx = 1;
            while(gu16FindCharIdx < gu8ClientCharCnt)
            {
                if(ble_central_get_a_char_with_description(&stChar, gu16FindCharIdx, 1)) // find a char have char des
                {
                    rom_gatt_client_api_discover_characteristic_descriptors(u16ConnHandle, &stChar);
                    //rom_gatt_client_api_discover_characteristic_descriptors(u16ConnHandle, &stChar);
                    gu16FindCharIdx++;
                    break;
                }
                gu16FindCharIdx++;
            }
        }
    }
}

/**
 * @brief  Parse discovered characteristic descriptor.
 * @param  u16ConnHandle: Connection handle.
 * @param  u16OpCode: Opcode.
 * @param  u16ErrCode: Error code.
 * @param  u16OpCode: Opcode.
 * @param  pu8Buf: Characteristic descriptor data buffer.
 * @param  u16Len: Buffer Len.
 * @return None.
 */
static void ble_central_parse_char_des(uint16_t u16ConnHandle, uint16_t u16OpCode, uint16_t u16ErrCode, uint8_t *pu8Buf, uint16_t u16Len)
{
    if ((u16ErrCode & 0x8000) == 0 && (u16ErrCode & 0x00FF) == 0 && (pu8Buf != NULL))
    {
        gatt_client_characteristic_descriptor_t *pCharDes = (gatt_client_characteristic_descriptor_t*)pu8Buf;
        ble_central_add_a_char_descriptor(0, pCharDes->handle, pCharDes->uuid16);
    }
    else if ((u16ErrCode & 0x8000) != 0)
    {
        gatt_client_characteristic_t stChar;
        memset(&stChar, 0, sizeof(stChar));
        if(gu16FindCharIdx < gu8ClientCharCnt)
        {
            while(gu16FindCharIdx < gu8ClientCharCnt)
            {
                if(ble_central_get_a_char_with_description(&stChar, gu16FindCharIdx, 1)) // find a char have char des
                {
                    rom_gatt_client_api_discover_characteristic_descriptors(u16ConnHandle, &stChar);
                    gu16FindCharIdx++;
                    break;
                }
                gu16FindCharIdx++;
            }
        }

        if(gu16FindCharIdx == gu8ClientCharCnt)
        {
            // send event find service end
            PRINTF("find service end DataSize = %d\n", gu16ClientDataSize);
            if(NULL != ble_central_event_callback)
            {
                ble_central_event_callback(EN_BLE_CENTRAL_EVENT_FIND_SERVICE_FINISH, NULL, 0);
            }
        }
    }
}

/**
 * @brief  Parse characteristic read result.
 * @param  u16ConnHandle: Connection handle.
 * @param  u16OpCode: Opcode.
 * @param  u16ErrCode: Error code.
 * @param  u16OpCode: Opcode.
 * @param  pu8Buf: Characteristic read result data buffer.
 * @param  u16Len: Buffer Len.
 * @return None.
 */
static void ble_central_parse_char_read_result(uint16_t u16ConnHandle, uint16_t u16OpCode, uint16_t u16ErrCode, uint8_t *pu8Buf, uint16_t u16Len)
{
    static uint16_t u16DataLen = 0;
    if(0 == (u16ErrCode & 0x8000))
    {
        if(pu8Buf && u16Len)
        {
            memcpy(u8CharReadBuf, pu8Buf, u16Len);
            u16DataLen += u16Len;
        }
    }
    else if(0 != (u16ErrCode & 0x8000))
    {
        if(NULL != ble_central_char_read_callback)
        {
            ble_central_char_read_callback(u8CharReadBuf, u16DataLen);
        }
        u16DataLen = 0;
    }
}

/**
 * @brief  Parse characteristic write result.
 * @param  u16ConnHandle: Connection handle.
 * @param  u16OpCode: Opcode.
 * @param  u16ErrCode: Error code.
 * @param  u16OpCode: Opcode.
 * @param  pu8Buf: Characteristic write result data buffer.
 * @param  u16Len: Buffer Len.
 * @return None.
 */
static void ble_central_parse_char_write_result(uint16_t u16ConnHandle, uint16_t u16OpCode, uint16_t u16ErrCode, uint8_t *pu8Buf, uint16_t u16Len)
{
    u16ErrCode &= 0xFF;
    if(NULL != ble_central_char_write_callback)
    {
        ble_central_char_write_callback((uint8_t)u16ErrCode);
    }
}


/**
 * @brief  BLE client callback.
 * @param  u16ConnHandle: Connection handle.
 * @param  u16OpCode: Opcode.
 * @param  u16ErrCode: Error code.
 * @param  u16OpCode: Opcode.
 * @param  pu8Buf: Data buffer.
 * @param  u16Len: Buffer Len.
 * @return None.
 */
static uint32_t ble_client_callback(uint16_t u16ConnHandle, uint16_t u16OpCode, uint16_t u16ErrCode, uint8_t *pu8Buf, uint16_t u16Len)
{
    PRINTF("[Client][%d]connHandle: %d, opCode: %d, errCode: %04X\n", u16Len, u16ConnHandle, u16OpCode, u16ErrCode);

    switch (u16OpCode)
    {
        case GATTC_USER_PRIMARY_SERVICES_QUERY_ALL: // 4 Find out all primary services uuid.
        {
            ble_central_parse_service(u16ConnHandle, u16OpCode, u16ErrCode, pu8Buf, u16Len);
            break;
        }
        case GATTC_USER_CHARACTERISTICS_QUERY_ALL: // 6 Get the characteristic uuid of the indicate service.
        {
            ble_central_parse_characteristic(u16ConnHandle, u16OpCode, u16ErrCode, pu8Buf, u16Len);
            break;
        }
        case GATTC_USER_CHARACTERISTICS_DESCRIPTOR_QUERY: // 8
        {
            ble_central_parse_char_des(u16ConnHandle, u16OpCode, u16ErrCode, pu8Buf, u16Len);
            break;
        }
        case GATTC_USER_CHARACTERISTICS_READ_BY_HANDLE_RESULT:
        {
            ble_central_parse_char_read_result(u16ConnHandle, u16OpCode, u16ErrCode, pu8Buf, u16Len);
            break;
        }
        case GATTC_USER_CHARACTERISTICS_WRITE_RESULT:
        {
            ble_central_parse_char_write_result(u16ConnHandle, u16OpCode, u16ErrCode, pu8Buf, u16Len);
            break;
        }
        case GATTC_USER_NOTIFICATION: // 0
        {
            if(NULL != ble_central_event_callback)
            {
                ble_central_event_callback(EN_BLE_CENTRAL_EVENT_NOTIFY, pu8Buf, u16Len);
            }
            break;
        }
        // format:1d[indication opcode] hdl[2byte] data[u16Len-3]
        case GATTC_USER_INDICATION: // 1
        {
            break;
        }
        default: break;
    }

    return 0;
}

/**
 * @brief  Get a primary service handle by UUID.
 * @param  u8IsUuid128: Indicates the service UUID is 128bits or not..
 * @param  u16Uuid16: Service 16bits UUID, if service UUID is 128bits, it's can be 0.
 * @param  pu8Uuid128: Service 128bits UUID, if service UUID is 16bits, it's can be NULL.
 * @param  pu16StartHandle: Service start handle.
 * @param  pu16EndHandle: Service end handle.
 * @return true or false.
 */
bool ble_central_get_service_handle_by_uuid(uint8_t u8IsUuid128, uint16_t u16Uuid16, uint8_t *pu8Uuid128, uint16_t *pu16StartHandle, uint16_t *pu16EndHandle)
{
    bool bRet = false;
    uint8_t *pu8Data = NULL;
    uint16_t u16ServiceFindIdx = 1;
    if((NULL == pu16StartHandle) || (NULL == pu16EndHandle) || (u8IsUuid128 && (NULL == pu8Uuid128)))
    {
        return false;
    }
    while(u16ServiceFindIdx <= gu8ClientServiceCnt)
    {
        pu8Data = ble_central_get_a_item(SERVICE_TYPE, u16ServiceFindIdx);
        if(pu8Data)
        {
            if(u8IsUuid128 && (pu8Data[0] == ST_SERVICE_128UUID_LEN))
            {
                if(0 == memcmp(pu8Data+6, pu8Uuid128, 16))
                {
                    *pu16StartHandle = rom_little_endian_read_16(pu8Data, 2);
                    *pu16EndHandle = rom_little_endian_read_16(pu8Data, 4);
                    bRet = true;
                    break;
                }
            }
            else
            {
                if(u16Uuid16 == rom_little_endian_read_16(pu8Data, 6))
                {
                    *pu16StartHandle = rom_little_endian_read_16(pu8Data, 2);
                    *pu16EndHandle = rom_little_endian_read_16(pu8Data, 4);
                    bRet = true;
                    break;
                }
            }
        }
        u16ServiceFindIdx++;
    }
    return bRet;
}

/**
 * @brief  Get a characteristic handle by UUID.
 * @param  u8IsUuid128: Indicates the characteristic UUID is 128bits or not..
 * @param  u16Uuid16: Characteristic 16bits UUID, if characteristic UUID is 128bits, it's can be 0.
 * @param  pu8Uuid128: Characteristic 128bits UUID, if characteristic UUID is 16bits, it's can be NULL.
 * @param  pu16Handle: Characteristic handle.
 * @param  u8CharIdx: Characteristic index.
 * @return true or false.
 */
bool ble_central_get_char_value_handle_by_uuid(uint8_t u8IsUuid128, uint16_t u16Uuid16, uint8_t *pu8Uuid128, uint16_t *pu16Handle, uint8_t u8CharIdx)
{
    bool bRet = false;
    uint8_t *pu8Data = NULL;
    uint8_t u8Idx = 0;
    if(NULL == pu16Handle || (u8IsUuid128 && (NULL == pu8Uuid128)))
    {
        return false;
    }
    
    uint16_t u16CharFindIdx = 1;
    while(u16CharFindIdx <= gu8ClientCharCnt)
    {
        pu8Data = ble_central_get_a_item(CHAR_TYPE, u16CharFindIdx);
        if(pu8Data)
        {
            if(u8IsUuid128 && (pu8Data[0] == 23))
            {
                if(0 == memcmp(pu8Data+7, pu8Uuid128, 16))
                {
                    if(u8Idx == u8CharIdx)
                    {
                        *pu16Handle = rom_little_endian_read_16(pu8Data, 3);
                        bRet = true;
                        break;
                    }
                    else
                    {
                        u8Idx++;
                    }
                }
            }
            else
            {
                if(rom_little_endian_read_16(pu8Data, 7) == u16Uuid16)
                {
                    if(u8Idx == u8CharIdx)
                    {
                        *pu16Handle = rom_little_endian_read_16(pu8Data, 3);
                        bRet = true;
                        break;
                    }
                    else
                    {
                        u8Idx++;
                    }
                }
            }
        }
        u16CharFindIdx++;
    }
    return bRet;
}

/**
 * @brief  Get a characteristic descriptor handle by characteristic handle.
 * @param  u16CharHandle: Indicates the characteristic UUID.
 * @param  u16CharDesUuid: Characteristic descriptor UUID, if characteristic UUID is 128bits, it's can be 0.
 * @param  pu16Handle: Characteristic descriptor handle.
 * @return true or false.
 */
bool ble_central_get_char_des_by_char_handle(uint16_t u16CharHandle, uint16_t u16CharDesUuid, uint16_t *pu16Handle)
{
    bool bRet = false;
    uint8_t *pu8Data = NULL;
    uint16_t u16Idx = 1;
    uint16_t u16CharEndHandle = 0;
    if(NULL == pu16Handle || (GATT_CHAR_EXT_PROPS_UUID > u16CharDesUuid) || (GATT_REPORT_REF_UUID < u16CharDesUuid))
    {
        return false;
    }
    u16CharDesUuid = (u16CharDesUuid - GATT_CHAR_EXT_PROPS_UUID) + 0x90;
    // find char end value by char hand
    while(u16Idx <= gu8ClientCharCnt)
    {
        pu8Data = ble_central_get_a_item(CHAR_TYPE, u16Idx);
        if(pu8Data)
        {
            if(rom_little_endian_read_16(pu8Data, 3) == u16CharHandle)
            {
                u16CharEndHandle = rom_little_endian_read_16(pu8Data, 5);
                break;
            }
        }
        u16Idx++;
    }
    if(u16CharHandle == u16CharEndHandle)
    {
        return false;
    }
    // find char des by char value and char end value
    u16Idx = 1;
    do
    {
        pu8Data = ble_central_get_a_item(u16CharDesUuid, u16Idx);
        if((NULL != pu8Data) && (5 == pu8Data[0]))
        {
            *pu16Handle = rom_little_endian_read_16(pu8Data, 3);
            if((*pu16Handle > u16CharHandle) && (*pu16Handle <= u16CharEndHandle))
            {
                bRet = true;
                break;
            }
        }
        *pu16Handle = 0;
        u16Idx++;
    }while(NULL != pu8Data);
    
    return bRet;
}

/**
 * @brief  Read characteristic.
 * @param  u16ConnHandle: Connection handle.
 * @param  u16ValueHandle: Characteristic handle.
 * @param  read_cb: Characteristic read callback.
 * @return true or false.
 */
bool ble_central_read_char_value(uint16_t u16ConnHandle, uint16_t u16ValueHandle, ble_central_char_read_callback_t read_cb)
{
    if(NULL == read_cb)
    {
        return false;
    }
    ble_central_char_read_callback = read_cb;
    if(0 != rom_gatt_client_api_read_value_of_characteristic_using_value_handle(u16ConnHandle, u16ValueHandle))
    {
        return false;
    }
    return true;
}

/**
 * @brief  Write characteristic.
 * @param  u16ConnHandle: Connection handle.
 * @param  u16ValueHandle: Characteristic handle.
 * @param  pu8Data: Write data.
 * @param  u16DataLen: Data len.
 * @param  write_cb: Characteristic read callback.
 * @return true or false.
 */
bool ble_central_write_char_value(uint16_t u16ConnHandle, uint16_t u16ValueHandle, uint8_t *pu8Data, uint16_t u16DataLen, ble_central_char_write_callback_t write_cb)
{
    uint8_t u8Ret = 0;
    if((NULL == write_cb) || (NULL == pu8Data) || (0 == u16DataLen))
    {
        return false;
    }
    ble_central_char_write_callback = write_cb;
    PRINTF("u16ConnHandle %d u16ValueHandle %d\n", u16ConnHandle, u16ValueHandle);
    u8Ret = rom_gatt_client_api_write_value_of_characteristic(u16ConnHandle, u16ValueHandle, u16DataLen, pu8Data);
    PRINTF("u8Ret = %d\n", u8Ret);
    if(0 != u8Ret)
    {
        return false;
    }
    return true;
}

/**
 * @brief  Get connection handle.
 * @param  None.
 * @return Connection handle.
 */
uint16_t ble_central_get_conn_handle(void)
{
    return ble_get_connect_handle();
}

void ble_central_free_client_data_buffer(void)
{
    gu16ClientDataSize = 0;
    gu8ClientServiceCnt = 0;
    gu8ClientCharCnt = 0;
    gu16FindServiceIdx = 0;
    gu16FindCharIdx = 0;
}

/**
 * @brief  Discover primary services.
 * @param  u16ConnHandl: Connection handle.
 * @return Result code, @ref EN_LLC_SPEC_5_1_ERROR_CODE_T.
 */
uint8_t ble_central_discover_primary_services(uint16_t u16ConnHandl)
{
    return rom_gatt_client_api_discover_primary_services(u16ConnHandl);
}

void ble_central_send_security_req(uint16_t u16ConnHandl)
{
    rom_gap_api_send_security_req(u16ConnHandl);
}

bool ble_central_start_scan(void)
{
    return rom_gap_api_set_scan_enable(true, false);
}


/**
 * @brief  ble central init.
 * @param  ble_central_event_cb: BLE central event callback.
 * @return None.
 */
bool ble_central_init(ble_central_event_callback_t ble_central_event_cb)
{
    if(NULL == ble_central_event_cb)
    {
        return false;
    }
    ble_central_event_callback = ble_central_event_cb;
    gpu8ClientData = gu8ClientDataBuf;
    // 1. Set scan parameters.
    stGapSetScanParameters_t stScanParam =
    {
        .enumScanType             = GAP_SCAN_TYPE_ACTIVE,
        .u8ScanChannelMap         = BLE_SCAN_CHANNEL,
        .u16ScanInterval625us     = BLE_SCAN_INTERVAL,
        .u16ScanWindow625us       = BLE_SCAN_WINDOW,
        .enumOwnAddressType       = GAP_OWN_ADDRESS_TYPE_PUBLIC,
        .enumScanningFilterPolicy = GAP_SCAN_FILTER_POLICY_ACCEPT_ALL
    };
    rom_gap_api_set_scan_parameters(&stScanParam);

    rom_gatt_client_api_init(ble_client_callback);

    // 2. Set pair parameters.
    rom_gap_api_update_pair_para(IO_CAPABILITY_NO_INPUT_NO_OUTPUT, SM_AUTHREQ_BONDING  | SM_AUTHREQ_SECURE_CONNECTION, 16);
    
    // 3. Enable scan.
    rom_gap_api_set_scan_enable(true, false);
    
    return true;
}

