/**
****************************************************************************
* @file    att_db.h
* @brief
 * @author   Maxscend SoC Software Team
* @version V1.0.0
* @date    2016/11/18
* @history  modify by xiaoyu at 26/Jan/2018
* @note
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/

#ifndef __ATT_H__
#define __ATT_H__


// new java-style iterator
typedef struct att_iterator
{
    // private
    uint8_t const * att_ptr;
    // public
    uint16_t size;
    uint16_t flags;
    uint16_t handle;
    uint8_t  const * uuid;
    uint16_t value_len;
    uint8_t  const * value;
} att_iterator_t;




extern uint8_t rom_handle_exchange_mtu_request(att_connection_t* p_att_state, uint8_t * request_buffer);
extern void rom_handle_find_information_request(att_connection_t* p_att_state, uint8_t * request_buffer);
extern void rom_handle_find_by_type_value_request(att_connection_t* p_att_state, uint8_t * request_buffer, uint16_t attribute_len);
//extern void rom_handle_read_by_type_request(att_connection_t* p_att_state, uint8_t * request_buffer, uint16_t attribute_len);
//extern void rom_handle_read_request(att_connection_t* p_att_state, uint8_t * request_buffer);
//extern void rom_handle_read_blob_request(att_connection_t* p_att_state, uint8_t * request_buffer);
//extern void rom_handle_read_multiple_request(att_connection_t* p_att_state, uint8_t * request_buffer, uint16_t request_len);
extern void rom_handle_read_by_group_type_request(att_connection_t* p_att_state, uint8_t * request_buffer, uint16_t attribute_len);
//extern void rom_handle_write_request(att_connection_t* p_att_state, uint8_t * request_buffer, uint16_t request_len);
//extern void rom_handle_prepare_write_request(att_connection_t* p_att_state, uint8_t * request_buffer, uint16_t request_len);
//extern void rom_handle_execute_write_request(att_connection_t* p_att_state, uint8_t * request_buffer);
//extern void rom_handle_write_command(att_connection_t* p_att_state, uint8_t * request_buffer, uint16_t request_len);
extern uint16_t rom_att_prepare_handle_value_notification(att_connection_t* p_att_state, uint16_t handle, uint8_t *value, uint16_t value_len, uint8_t * response_buffer);
extern uint16_t rom_att_prepare_handle_value_indication(att_connection_t* p_att_state, uint16_t handle, uint8_t *value, uint16_t value_len, uint8_t * response_buffer);
extern void rom_handle_signed_write_command(att_connection_t* p_att_state, uint8_t * request_buffer, uint16_t request_len);
//extern void rom_handle_value_indication(att_connection_t* p_att_state);
extern void rom_handle_signed_write_command_pre(att_connection_t* p_att_state, uint8_t * request_buffer, uint16_t request_len);
extern void rom_uuid_add_bluetooth_prefix(uint8_t *uuid, uint32_t shortUUID);

//extern uint16_t rom_mxd_att_get_attribute_length_callback(uint16_t conn_handle, uint16_t attribute_handle);
//extern uint16_t rom_mxd_att_read_callback(uint16_t conn_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size);
//extern uint32_t rom_mxd_att_write_callback(uint16_t conn_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size);
extern void rom_handle_rfu_request_opcode ( att_connection_t *p_att_state, uint8_t  request_type,uint8_t error_code );

#endif // __ATT_H
