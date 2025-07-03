/**********************************************************************************************************************
 * @file     patch_att_db.h
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

#ifndef __PATCH_ATT_DB_H__
#define __PATCH_ATT_DB_H__

extern void patch_handle_read_by_type_request ( att_connection_t *p_att_state, uint8_t *request_buffer, uint16_t attribute_len );

extern void patch_handle_read_request ( att_connection_t *p_att_state, uint8_t *request_buffer );

extern void patch_handle_read_blob_request ( att_connection_t *p_att_state, uint8_t *request_buffer );

extern void patch_handle_read_multiple_request ( att_connection_t *p_att_state, uint8_t *request_buffer, uint16_t request_len );

extern void patch_handle_write_request ( att_connection_t *p_att_state, uint8_t *request_buffer, uint16_t request_len );

extern void patch_handle_prepare_write_request ( att_connection_t *p_att_state, uint8_t *request_buffer, uint16_t request_len );

extern void patch_handle_value_indication ( att_connection_t *p_att_state );

extern void patch_handle_execute_write_request ( att_connection_t *p_att_state, uint8_t *request_buffer );

extern void patch_handle_write_command ( att_connection_t *p_att_state, uint8_t *request_buffer, uint16_t request_len );

extern uint16_t patch_mxd_att_get_attribute_length_callback ( uint16_t conn_handle, uint16_t attribute_handle );
extern uint16_t patch_mxd_att_read_callback ( uint16_t conn_handle, uint16_t attribute_handle, uint16_t offset, uint8_t *buffer, uint16_t buffer_size );
extern uint32_t patch_mxd_att_write_callback ( uint16_t conn_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size );

#endif