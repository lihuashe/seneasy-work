/*
MLOG_ID_RANGE: 0x5115~0x517F
MLOG_ID_USED:  0x5115~0x5123
*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "bluetooth.h"
#include "host_stack_conn_hdl.h"
#include "att_db_util.h"

#include "att_db.h"
#include "patch_att_db.h"

#include "stack_utility.h"
#include "host_stack_mem.h"
#include "gap_def.h"
#include "gatt_service_api.h"

#include "att_server.h"
#include "le_device_db.h"
#include "gatt_uuid_def.h"
#include "l2cap_signaling.h"
#include "ble_callback.h"
#include "mlog.h"
#include "host_hook.h"


#define CANCEL_ALL_PREPARE_WRITE 0
#define IMMADIATELY_WRITE_ALL_PENDING_PREPARED_VALUES 1

//declaration
// extern uint16_t patch_mxd_att_get_attribute_length_callback ( uint16_t conn_handle, uint16_t attribute_handle );
// extern uint16_t patch_mxd_att_read_callback ( uint16_t conn_handle, uint16_t attribute_handle, uint16_t offset, uint8_t *buffer, uint16_t buffer_size );
// extern uint32_t patch_mxd_att_write_callback ( uint16_t conn_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size );
// Buetooth Base UUID 00000000-0000-1000-8000-00805F9B34FB in little endian
extern const uint8_t bluetooth_base_uuid;

extern uint32_t att_validate_security_AUTHENTICATION_REQUIRED_for_input_key_pair;

extern void rom_att_db_global_variable_initialze ( void );

extern int32_t rom_is_Bluetooth_Base_UUID ( uint8_t const *uuid );  //defined in rom lib

extern uint16_t rom_uuid16_from_uuid ( uint16_t uuid_len, uint8_t *uuid );  //defined in rom lib

extern void rom_att_iterator_init ( att_iterator_t *it );  //defined in rom lib

extern int32_t rom_att_iterator_has_next ( att_iterator_t *it );  //defined in rom lib

extern void rom_att_iterator_fetch_next ( att_iterator_t *it );  //defined in rom lib

extern int32_t rom_att_iterator_match_uuid16 ( att_iterator_t *it, uint16_t uuid );  //defined in rom lib

extern int32_t rom_att_iterator_match_uuid ( att_iterator_t *it, uint8_t *uuid, uint16_t uuid_len );  //defined in rom lib

extern uint32_t rom_att_find_handle ( att_iterator_t *it, uint16_t handle );  //defined in rom lib

// experimental client API
extern uint16_t rom_att_uuid_for_handle ( uint16_t attribute_handle );  //defined in rom lib

void patch_att_update_value_len ( uint16_t conn_handle, att_iterator_t *it )
{
    if ( ( it->flags & ATT_PROPERTY_DYNAMIC ) == 0 )
    {
        return;
    }
    //  it->value_len =  mxd_att_read_callback(it->handle, 0, NULL, 0);
    it->value_len =  patch_mxd_att_get_attribute_length_callback ( conn_handle, it->handle );
    return;
}

uint16_t patch_att_copy_value ( uint16_t conn_handle, att_iterator_t *it, uint16_t offset, uint8_t *buffer, uint16_t buffer_size )
{
    uint16_t bytes_to_copy;
    MLOG1616D ( 0x5115, "att_copy_value hdl=%x buffer_size=%d  ", it->handle,  buffer_size );

    // DYNAMIC
    if ( ( it->flags & ATT_PROPERTY_DYNAMIC ) )
    {
        return patch_mxd_att_read_callback ( conn_handle, it->handle, offset, buffer, buffer_size );
    }

    // STATIC
    bytes_to_copy = it->value_len - offset;
    MLOG1616D ( 0x5116, "att_copy_value it->value_len=%d offset=%d  ", it->value_len,  offset );

    if ( bytes_to_copy > buffer_size )
    {
        bytes_to_copy = buffer_size;
    }
    memcpy ( buffer, it->value + offset, bytes_to_copy ); //modify  weike 20160528
    return bytes_to_copy;
}


extern uint16_t rom_setup_error ( uint8_t *response_buffer, uint16_t request, uint16_t handle, uint8_t error_code );  //defined in rom lib


extern uint16_t rom_setup_error_read_not_permitted ( uint8_t *response_buffer, uint16_t request, uint16_t start_handle );  //defined in rom lib

extern uint16_t rom_setup_error_write_not_permitted ( uint8_t *response_buffer, uint16_t request, uint16_t start_handle );  //defined in rom lib

extern uint16_t rom_setup_error_atribute_not_found ( uint8_t *response_buffer, uint16_t request, uint16_t start_handle );  //defined in rom lib

extern uint16_t rom_setup_error_invalid_handle ( uint8_t *response_buffer, uint16_t request, uint16_t handle );  //defined in rom lib

extern uint16_t rom_setup_error_invalid_offset ( uint8_t *response_buffer, uint16_t request, uint16_t handle );  //defined in rom lib

extern uint16_t rom_setup_error_invalid_value_length ( uint8_t *response_buffer, uint16_t request, uint16_t handle );  //defined in rom lib

extern uint8_t rom_att_validate_security ( att_connection_t *p_att_state, att_iterator_t *it );  //defined in rom lib



void patch_handle_read_by_type_request ( att_connection_t *p_att_state, uint8_t *request_buffer, uint16_t attribute_len )
{
    uint16_t offset   = 1;
    uint16_t pair_len = 0;
    uint8_t request_type = ATT_READ_BY_TYPE_REQUEST;
    att_iterator_t it;
    uint8_t error_code = 0;
    uint16_t first_matching_but_unreadable_handle = 0;
    uint8_t *att_rsp_buffer;
    uint16_t response_len;
    uint16_t start_handle = rom_stack_utility_little_endian_read_16 ( request_buffer, 1 );
    uint16_t end_handle = rom_stack_utility_little_endian_read_16 ( request_buffer, 3 );
    uint32_t attribute_type_len;
    MLOG1616D ( 0x5117, "handle_read_by_type_request s hdl=%x %x", start_handle, end_handle );
    MLOG1616D ( 0x5118, "handle_read_by_type_request s len=%d %x", attribute_len, request_buffer[5] );

    att_rsp_buffer = rom_l2cap_get_avail_data_buffer (p_att_state->mtu + 4);
    if ( NULL == att_rsp_buffer )
    {
        //no enough buffer
        MLOG0E ( 0x5119, "handle_read_by_type_request  get buffer fail" );
        return;
    }

    if ( attribute_len <= 7 )
    {
        attribute_type_len = 2;
    }
    else
    {
        attribute_type_len = 16;
    }

    if ( start_handle > end_handle || start_handle == 0 )
    {
        response_len =  rom_setup_error_invalid_handle ( att_rsp_buffer, request_type, start_handle );
    }
    else
    {

        rom_att_iterator_init ( &it );
        while ( rom_att_iterator_has_next ( &it ) )
        {
            uint16_t this_pair_len;
            uint16_t bytes_copied ;
            rom_att_iterator_fetch_next ( &it );
            MLOG1616D ( 0x511a, "att_iterator_fetch_next sz=%x hdl=%x", it.size, it.handle );
            MLOG1616D ( 0x511b, "att_iterator_fetch_next flag=%x uuid=%x", it.flags, it.uuid[0] | it.uuid[1] << 8 );
            if ( !it.handle )
            {
                break;
            }
            if ( it.handle < start_handle )
            {
                continue;
            }
            if ( it.handle > end_handle )
            {
                break;    // (1)
            }

            // does current attribute match
            if ( !rom_att_iterator_match_uuid ( &it, &request_buffer[5], attribute_type_len ) )
            {
                continue;
            }

            // skip handles that cannot be read but rembember that there has been at least one
            if ( ( it.flags & ATT_PROPERTY_READ ) == 0 )
            {
                if ( first_matching_but_unreadable_handle == 0 )
                {
                    first_matching_but_unreadable_handle = it.handle;
                }
                continue;
            }

            // check security requirements
            error_code = rom_att_validate_security ( p_att_state, &it );
            if ( error_code )
            {
                break;
            }

            patch_att_update_value_len ( p_att_state->conn_handle, &it );


            // check if value has same len as last one
            this_pair_len = 2 + it.value_len;
            if ( offset > 1 )
            {
                if ( pair_len != this_pair_len )
                {
                    break;
                }
            }

            // first
            if ( offset == 1 )
            {
                pair_len = this_pair_len;
                att_rsp_buffer[offset] = pair_len;
                offset++;
            }


            // space?
            if ( offset + pair_len > ( p_att_state->mtu ) )
            {
                if ( offset > 2 )
                {
                    break;
                }
                it.value_len = p_att_state->mtu - 4;
                att_rsp_buffer[1] = 2 + it.value_len;
            }

            // store
            rom_stack_utility_little_endian_store_16 ( att_rsp_buffer, offset, it.handle );
            offset += 2;
            bytes_copied = patch_att_copy_value ( p_att_state->conn_handle, &it, 0, att_rsp_buffer + offset, it.value_len );
            offset += bytes_copied;
            MLOGD ( 0x511c, "att_copy_value =%d  ", bytes_copied );

        }
        // at least one attribute could be read
        if ( offset > 1 )
        {
            att_rsp_buffer[0] = ATT_READ_BY_TYPE_RESPONSE;
            response_len = offset;
        }
        else if ( error_code )
        {
            response_len = rom_setup_error ( att_rsp_buffer, request_type, start_handle, error_code );
        }
        else if ( first_matching_but_unreadable_handle )
        {
            response_len = rom_setup_error_read_not_permitted ( att_rsp_buffer, request_type, first_matching_but_unreadable_handle );
        }
        else
        {
            response_len = rom_setup_error_atribute_not_found ( att_rsp_buffer, request_type, start_handle );
        }
    }
    MLOGD ( 0x511d, "response_lenz=%d", response_len );

    rom_att_tx_buffer_commit ( p_att_state->conn_handle, att_rsp_buffer, response_len );


}

void patch_handle_read_request ( att_connection_t *p_att_state, uint8_t *request_buffer )
{
    uint8_t request_type = ATT_READ_REQUEST;
    uint16_t offset   = 1;
    uint8_t error_code;
    uint16_t bytes_copied;
    att_iterator_t it;
    uint8_t *att_rsp_buffer;
    uint16_t response_len;
    uint16_t handle = rom_stack_utility_little_endian_read_16 ( request_buffer, 1 );
    uint32_t ok = rom_att_find_handle ( &it, handle );

    att_rsp_buffer = rom_l2cap_get_avail_data_buffer (p_att_state->mtu + 4);
    if ( NULL == att_rsp_buffer )
    {
        //no enough buffer
        MLOG0E ( 0x511e, "Error:get buffer fail" );
        return;
    }

    if ( !ok )
    {
        response_len = rom_setup_error_invalid_handle ( att_rsp_buffer, request_type, handle );
    }
    else if ( ( it.flags & ATT_PROPERTY_READ ) == 0 )
    {
        response_len = rom_setup_error_read_not_permitted ( att_rsp_buffer, request_type, handle );
    }
    else
    {

        // check security requirements
        error_code = rom_att_validate_security ( p_att_state, &it );
        if ( error_code )
        {
            response_len = rom_setup_error ( att_rsp_buffer, request_type, handle, error_code );
        }
        else
        {

            patch_att_update_value_len ( p_att_state->conn_handle, &it );


            // limit data
            if ( offset + it.value_len > ( p_att_state->mtu ) )
            {
                it.value_len = ( p_att_state->mtu - 1 );
            }

            // store
            bytes_copied = patch_att_copy_value ( p_att_state->conn_handle, &it, 0, att_rsp_buffer + offset, it.value_len );
            offset += bytes_copied;

            att_rsp_buffer[0] = ATT_READ_RESPONSE;
            response_len = offset;
        }
    }
    rom_att_tx_buffer_commit ( p_att_state->conn_handle, att_rsp_buffer, response_len );
}

void patch_handle_read_blob_request ( att_connection_t *p_att_state, uint8_t *request_buffer )
{
    uint8_t request_type = ATT_READ_BLOB_REQUEST;
    uint16_t offset   = 1;
    uint16_t bytes_copied;
    att_iterator_t it;
    uint8_t error_code;
    uint8_t *att_rsp_buffer;
    uint16_t response_len;
    uint16_t handle = rom_stack_utility_little_endian_read_16 ( request_buffer, 1 );
    uint16_t value_offset = rom_stack_utility_little_endian_read_16 ( request_buffer, 3 );
    uint32_t ok = rom_att_find_handle ( &it, handle );

    att_rsp_buffer = rom_l2cap_get_avail_data_buffer (p_att_state->mtu + 4);
    if ( NULL == att_rsp_buffer )
    {
        //no enough buffer
        MLOG0E ( 0x511f, "Error:get buffer fail" );
        return;
    }

    if ( !ok )
    {
        response_len = rom_setup_error_invalid_handle ( att_rsp_buffer, request_type, handle );
    }
    else if ( ( it.flags & ATT_PROPERTY_READ ) == 0 )
    {
        response_len = rom_setup_error_read_not_permitted ( att_rsp_buffer, request_type, handle );
    }
    else
    {
        // check security requirements
        error_code = rom_att_validate_security ( p_att_state, &it );
        if ( error_code )
        {
            response_len = rom_setup_error ( att_rsp_buffer, request_type, handle, error_code );
        }
        else
        {

            patch_att_update_value_len ( p_att_state->conn_handle, &it );

            if ( value_offset > it.value_len )
            {
                response_len = rom_setup_error_invalid_offset ( att_rsp_buffer, request_type, handle );
            }
            else
            {

                // limit data

                if ( offset + it.value_len - value_offset > p_att_state->mtu )
                {
                    it.value_len = p_att_state->mtu - 1 + value_offset;
                }

                // store
                bytes_copied = patch_att_copy_value ( p_att_state->conn_handle, &it, value_offset, att_rsp_buffer + offset, it.value_len - value_offset );
                offset += bytes_copied;

                att_rsp_buffer[0] = ATT_READ_BLOB_RESPONSE;
                response_len = offset;
            }
        }
    }
    rom_att_tx_buffer_commit ( p_att_state->conn_handle, att_rsp_buffer, response_len );
}

void patch_handle_read_multiple_request ( att_connection_t *p_att_state, uint8_t *request_buffer, uint16_t request_len )
{
    uint16_t offset   = 1;
    int32_t i;
    uint8_t error_code = 0;
    uint16_t handle = 0;
    uint8_t request_type = ATT_READ_MULTIPLE_REQUEST;
    uint8_t *att_rsp_buffer;
    uint16_t response_len;
    uint8_t *handles = &request_buffer[1];
    uint32_t num_handles = ( request_len - 1 ) >> 1;

    att_rsp_buffer = rom_l2cap_get_avail_data_buffer (p_att_state->mtu + 4);
    if ( NULL == att_rsp_buffer )
    {
        //no enough buffer
        MLOG0E ( 0x5120, "Error:get buffer fail" );
        return;
    }

    for ( i = 0; i < num_handles; i++ )
    {
        uint16_t bytes_copied ;
        att_iterator_t it;
        int32_t ok;
        handle = rom_stack_utility_little_endian_read_16 ( handles, i << 1 );

        if ( handle == 0 )
        {
            response_len = rom_setup_error_invalid_handle ( att_rsp_buffer, request_type, handle );
            break;
        }
        ok = rom_att_find_handle ( &it, handle );
        if ( !ok )
        {
            response_len = rom_setup_error_invalid_handle ( att_rsp_buffer, request_type, handle );
            break;
        }

        // check if handle can be read
        if ( ( it.flags & ATT_PROPERTY_READ ) == 0 )
        {
            response_len = rom_setup_error ( att_rsp_buffer, request_type, handle, ATT_ERROR_READ_NOT_PERMITTED );
            break;
        }

        // check security requirements
        error_code = rom_att_validate_security ( p_att_state, &it );
        if ( error_code )
        {
            response_len = rom_setup_error ( att_rsp_buffer, request_type, handle, error_code );
            break;
        }

        patch_att_update_value_len ( p_att_state->conn_handle, &it );

        // limit data
        if ( offset + it.value_len > p_att_state->mtu )
        {
            it.value_len = p_att_state->mtu - 1;
        }

        // store
        bytes_copied = patch_att_copy_value ( p_att_state->conn_handle, &it, 0, att_rsp_buffer + offset, it.value_len );
        offset += bytes_copied;
        response_len = offset;
        att_rsp_buffer[0] = ATT_READ_MULTIPLE_RESPONSE;
    }
    rom_att_tx_buffer_commit ( p_att_state->conn_handle, att_rsp_buffer, response_len );
}

void patch_handle_write_request ( att_connection_t *p_att_state, uint8_t *request_buffer, uint16_t request_len )
{
    uint8_t request_type = ATT_WRITE_REQUEST;
    uint8_t *att_rsp_buffer;
    uint16_t response_len;
    uint16_t handle = rom_stack_utility_little_endian_read_16 ( request_buffer, 1 );
    att_iterator_t it;
    uint8_t error_code ;
    uint32_t ok = rom_att_find_handle ( &it, handle );

    att_rsp_buffer = rom_l2cap_get_avail_data_buffer (16);
    if ( NULL == att_rsp_buffer )
    {
        //no enough buffer
        MLOG0E ( 0x5121, "Error:get buffer fail" );
        return;
    }

    if ( !ok )
    {
        response_len = rom_setup_error_invalid_handle ( att_rsp_buffer, request_type, handle );
    }
    else if ( ( it.flags & ATT_PROPERTY_WRITE ) == 0 )
    {
        response_len = rom_setup_error_write_not_permitted ( att_rsp_buffer, request_type, handle );
    }
    else if ( ( ( it.value_len + 3 ) < request_len ) && ( 0 == ( it.flags & ATT_PROPERTY_DYNAMIC ) ) )
    {
        response_len = rom_setup_error_invalid_value_length ( att_rsp_buffer, request_type, handle );
    }
    else if ( ( it.flags & ATT_PROPERTY_DYNAMIC ) == 0 )
    {
        response_len = rom_setup_error_write_not_permitted ( att_rsp_buffer, request_type, handle );
    }
    else
    {
        error_code = rom_att_validate_security ( p_att_state, &it );
        if ( error_code )
        {
            response_len = rom_setup_error ( att_rsp_buffer, request_type, handle, error_code );
        }
        else
        {
            error_code = patch_mxd_att_write_callback ( p_att_state->conn_handle, handle, ATT_TRANSACTION_MODE_NONE, 0, request_buffer + 3, request_len - 3 );
            if ( error_code )
            {
                response_len = rom_setup_error ( att_rsp_buffer, request_type, handle, error_code );
            }
            else
            {
                att_rsp_buffer[0] = ATT_WRITE_RESPONSE;
                response_len = 1;
            }
        }
    }
    rom_att_tx_buffer_commit ( p_att_state->conn_handle, att_rsp_buffer, response_len );
}

void patch_handle_prepare_write_request ( att_connection_t *p_att_state, uint8_t *request_buffer, uint16_t request_len )
{
    uint8_t request_type = ATT_PREPARE_WRITE_REQUEST;
    att_iterator_t it;
    int32_t ok;
    uint8_t error_code ;
    uint8_t *att_rsp_buffer;
    uint16_t response_len;
    uint16_t handle = rom_stack_utility_little_endian_read_16 ( request_buffer, 1 );
    uint16_t offset = rom_stack_utility_little_endian_read_16 ( request_buffer, 3 );

    att_rsp_buffer = rom_l2cap_get_avail_data_buffer (p_att_state->mtu + 4);
    if ( NULL == att_rsp_buffer )
    {
        //no enough buffer
        MLOG0E ( 0x5122, "Error:get buffer fail" );
        return;
    }

    ok = rom_att_find_handle ( &it, handle );
    if ( !ok )
    {
        response_len =  rom_setup_error_invalid_handle ( att_rsp_buffer, request_type, handle );
    }
    else if ( ( it.flags & ATT_PROPERTY_WRITE ) == 0 )
    {
        response_len =  rom_setup_error_write_not_permitted ( att_rsp_buffer, request_type, handle );
    }
    else if ( ( it.flags & ATT_PROPERTY_DYNAMIC ) == 0 )
    {
        response_len =  rom_setup_error_write_not_permitted ( att_rsp_buffer, request_type, handle );
    }
    else
    {
        // check security requirements
        error_code = rom_att_validate_security ( p_att_state, &it );
        if ( error_code )
        {
            response_len = rom_setup_error ( att_rsp_buffer, request_type, handle, error_code );
        }
        else
        {
            error_code = patch_mxd_att_write_callback ( p_att_state->conn_handle, handle, ATT_TRANSACTION_MODE_ACTIVE, offset, request_buffer + 5, request_len - 5 );
            if ( error_code )
            {
                response_len = rom_setup_error ( att_rsp_buffer, request_type, handle, error_code );
            }
            else
            {
                memcpy ( att_rsp_buffer, request_buffer, request_len );
                att_rsp_buffer[0] = ATT_PREPARE_WRITE_RESPONSE;
                response_len = request_len;
                p_att_state->att_prepare_write_handle = handle;
            }
        }
    }
    rom_att_tx_buffer_commit ( p_att_state->conn_handle, att_rsp_buffer, response_len );
}

void patch_handle_value_indication ( att_connection_t *p_att_state )
{
    //release_data_ptr_list ( &g_conn_cfg[idx].rx_data_pdu.data_list, 1 );
    if ( p_att_state->att_handle_value_indication_handle )
    {
        uint16_t hdl = p_att_state->att_handle_value_indication_handle;

        p_att_state->att_handle_value_indication_handle = 0;
        //notify APP indication is done by wenjie
        patch_mxd_att_write_callback ( p_att_state->conn_handle, hdl, ATT_TRANSACTION_MODE_CONFIRM_FOR_INDICAT, 0, 0, 0 );
        //att_indication_timeout_stop ( idx );

    }

}

// MARK: ATT_EXECUTE_WRITE_REQUEST 0x18
// NOTE: security has been verified by handle_prepare_write_request
void patch_handle_execute_write_request ( att_connection_t *p_att_state, uint8_t *request_buffer )
{
    uint8_t request_type = ATT_EXECUTE_WRITE_REQUEST;
    uint8_t *att_rsp_buffer;
    uint16_t response_len;
    uint8_t error_code = 0 ;
    uint16_t transaction_mode = ATT_TRANSACTION_MODE_CANCEL;

    att_rsp_buffer = rom_l2cap_get_avail_data_buffer (16);
    if ( NULL == att_rsp_buffer )
    {
        //no enough buffer
        MLOG0E ( 0x5123, "Error:get buffer fail" );
        return;
    }

    if ( IMMADIATELY_WRITE_ALL_PENDING_PREPARED_VALUES == request_buffer[1] )
    {

        transaction_mode = ATT_TRANSACTION_MODE_EXECUTE;
    }
    else
    {
        transaction_mode = ATT_TRANSACTION_MODE_CANCEL;
    }
    error_code = patch_mxd_att_write_callback ( p_att_state->conn_handle, p_att_state->att_prepare_write_handle, transaction_mode, 0, 0, 0 );
    if ( error_code )
    {
        response_len = rom_setup_error ( att_rsp_buffer, request_type, 0, error_code );
    }
    else
    {
        att_rsp_buffer[0] = ATT_EXECUTE_WRITE_RESPONSE;
        response_len = 1;
    }
    rom_att_tx_buffer_commit ( p_att_state->conn_handle, att_rsp_buffer, response_len );
}

// MARK: ATT_WRITE_COMMAND 0x52
// Core 4.0, vol 3, part F, 3.4.5.3
// "No Error Response or Write Response shall be sent in response to this command"
void patch_handle_write_command ( att_connection_t *p_att_state, uint8_t *request_buffer, uint16_t request_len )
{
    uint16_t handle = rom_stack_utility_little_endian_read_16 ( request_buffer, 1 );
    att_iterator_t it;
    int32_t ok = rom_att_find_handle ( &it, handle );
    if ( !ok || ( ( it.flags & ATT_PROPERTY_DYNAMIC ) == 0 ) || ( ( it.flags & ATT_PROPERTY_WRITE_WITHOUT_RESPONSE ) == 0 ) || ( rom_att_validate_security ( p_att_state, &it ) ) )
    {
    }
    else
    {

        patch_mxd_att_write_callback ( p_att_state->conn_handle, handle, ATT_TRANSACTION_MODE_NONE, 0, request_buffer + 3, request_len - 3 );
    }
}


