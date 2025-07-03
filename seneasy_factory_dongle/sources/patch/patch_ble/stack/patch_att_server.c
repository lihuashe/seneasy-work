/*
MLOG_ID_RANGE: 0x5214~0x527F
MLOG_ID_USED:  0x5214~0x5217
*/
/**********************************************************************************************************************
 * @file     patch_att_server.c
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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "bluetooth.h"
#include "att_db_util.h"
#include "host_stack_conn_hdl.h"

#include "gatt_client_api.h"
#include "stack_utility.h"
#include "host_stack_mem.h"
#include "sm.h"

#include "att_db.h"
#include "patch_att_db.h"

#include "le_device_db.h"

#include "att_server.h" ///////NEED to add patch

#include "gatt_service_api.h"
#include "ble_cb_event_def.h"
#include "ble_callback.h"
#include "host_hook.h"
#include "mlog.h"
#include "l2cap_signaling.h"

void patch_att_packet_handler(bool *bIsDone, uint16_t conn_handle, uint8_t *packet, uint16_t size)
{
    *bIsDone = true;

    uint8_t            opcode     = packet[0];
    host_connection_t *p_host_con = rom_host_connection_for_handle(conn_handle);

    if (!p_host_con)
    {
        MLOG0E(0x5214, "att_packet_handler get host conn error");
        return;
    }

    att_connection_t *p_att_state  = &p_host_con->st_att_state;
    t_gatt_client    *p_gatt_state = &p_host_con->st_gatt_client_state;
    MLOGD(0x5215, "att_packet_handler opcode=0x%x", opcode);

    switch (opcode)
    {
        case ATT_EXCHANGE_MTU_REQUEST:
        {
            if (0 == rom_handle_exchange_mtu_request(p_att_state, packet))
            {
                st_mtu_exchange_complete_event mtu_exchange_complete_event;

                mtu_exchange_complete_event.conn_handle = conn_handle;
                mtu_exchange_complete_event.mtu         = p_att_state->mtu;
                ble_callback_send_event(MSG_BLE_MTU_EXCHANGED_IND, &mtu_exchange_complete_event,
                                        sizeof(st_mtu_exchange_complete_event));
            }
            break;
        }
        case ATT_EXCHANGE_MTU_RESPONSE:
        {
            uint16_t remote_rx_mtu = rom_stack_utility_little_endian_read_16(packet, 1);
            uint16_t min_mtu_act   = remote_rx_mtu;

            if (remote_rx_mtu < p_att_state->max_mtu)
            {
                min_mtu_act = remote_rx_mtu;
            }
            else
            {

                min_mtu_act = p_att_state->max_mtu;
            }

            if ((min_mtu_act < p_att_state->mtu) && (min_mtu_act >= 23))
            {
                p_att_state->mtu = min_mtu_act;
            }
            // else  ,min_mtu_act should not bigger than p_att_state->mtu

            MLOG88D(0x5216, "recv ATT_EXCHANGE_MTU_RESPONSE client_rx_mtu=%d,mtu=%d", remote_rx_mtu, p_att_state->mtu);
            p_att_state->mtu_exchange_state = MTU_IS_EXCHANGED;
            rom_gatt_client_handle_exchange_mtu_response(conn_handle, p_att_state->mtu);

            st_mtu_exchange_complete_event mtu_exchange_complete_event;

            mtu_exchange_complete_event.conn_handle = conn_handle;
            mtu_exchange_complete_event.mtu         = p_att_state->mtu;
            ble_callback_send_event(MSG_BLE_MTU_EXCHANGED_IND, &mtu_exchange_complete_event,
                                    sizeof(st_mtu_exchange_complete_event));
            break;
        }
        case ATT_FIND_INFORMATION_REQUEST:
            rom_handle_find_information_request(p_att_state, packet); // ok
            break;
        case ATT_FIND_BY_TYPE_VALUE_REQUEST:
            rom_handle_find_by_type_value_request(p_att_state, packet, size); // ok
            break;
        case ATT_READ_BY_TYPE_REQUEST:
            patch_handle_read_by_type_request(p_att_state, packet, size);
            break;
        case ATT_READ_REQUEST:
            patch_handle_read_request(p_att_state, packet);
            break;
        case ATT_READ_BLOB_REQUEST:
            patch_handle_read_blob_request(p_att_state, packet);
            break;
#if 0 // not support
    case ATT_READ_MULTIPLE_REQUEST:
        patch_handle_read_multiple_request ( p_att_state, packet, size );
        break;
#endif
        case ATT_READ_BY_GROUP_TYPE_REQUEST:
            rom_handle_read_by_group_type_request(p_att_state, packet, size); // ok
            break;
        case ATT_WRITE_REQUEST:
            patch_handle_write_request(p_att_state, packet, size);
            break;
        case ATT_PREPARE_WRITE_REQUEST:
            patch_handle_prepare_write_request(p_att_state, packet, size);
            break;
        case ATT_HANDLE_VALUE_CONFIRMATION:
            patch_handle_value_indication(p_att_state);
            break;
        case ATT_EXECUTE_WRITE_REQUEST:
            patch_handle_execute_write_request(p_att_state, packet);
            break;
        case ATT_WRITE_COMMAND:
            patch_handle_write_command(p_att_state, packet, size);
            break;
#if 0 // def DEFINE_CMAC_AND_SIGNED_WRITE   //not support       
    case ATT_SIGNED_WRITE_COMMAND:
        rom_handle_signed_write_command_pre ( p_att_state, packet, size );
        break;
#endif
        // for att client function
        case ATT_HANDLE_VALUE_NOTIFICATION:
            // MLOGD ( 0x5217, "att_packet_handler  case ATT_HANDLE_VALUE_NOTIFICATION size=0x%x", size );
            rom_gatt_client_handle_value_notification(p_gatt_state, packet, size); // ok
            break;
        case ATT_READ_BY_GROUP_TYPE_RESPONSE:
            rom_gatt_client_handle_read_by_group_type_response(p_gatt_state, packet, size);
            break;
        case ATT_HANDLE_VALUE_INDICATION:
            rom_gatt_client_handle_value_indication(p_gatt_state, packet, size);
            break;
        case ATT_READ_BY_TYPE_RESPONSE:
            rom_gatt_client_handle_read_by_type_response(p_gatt_state, packet, size);
            break;
        case ATT_READ_RESPONSE:
            rom_gatt_client_handle_read_response(p_gatt_state, packet, size);
            break;
        case ATT_FIND_BY_TYPE_VALUE_RESPONSE:
            rom_gatt_client_handle_find_by_type_value_response(p_gatt_state, packet, size);
            break;
        case ATT_FIND_INFORMATION_REPLY:
            rom_gatt_client_handle_find_information_reply(p_gatt_state, packet, size);
            break;
        case ATT_WRITE_RESPONSE:
            rom_gatt_client_handle_write_response(p_gatt_state, packet, size);
            break;
        case ATT_READ_BLOB_RESPONSE:
            rom_gatt_client_handle_read_blob_response(p_gatt_state, packet, size);
            break;
        case ATT_PREPARE_WRITE_RESPONSE:
            rom_gatt_client_handle_prepare_write_response(p_gatt_state, packet, size);
            break;
        case ATT_EXECUTE_WRITE_RESPONSE:
            rom_gatt_client_handle_execute_write_response(p_gatt_state, packet, size);
            break;
#if 0 // not support
    case ATT_READ_MULTIPLE_RESPONSE:
        rom_gatt_client_handle_read_multiple_response ( p_gatt_state, packet, size );
        break;
#endif
        case ATT_ERROR_RESPONSE:
            rom_gatt_client_handle_error_response(p_gatt_state, packet, size);
            break;

        default:
            rom_handle_rfu_request_opcode(p_att_state, opcode, ATT_ERROR_REQUEST_NOT_SUPPORTED);
            break;
    }

    {
        rom_gatt_client_run(p_gatt_state);
    }
}

void patch_att_packet_handler_init(void)
{
    g_hostHookPrefixTable[HOST_HOOK_PREFIX_att_packet_handler] = patch_att_packet_handler;
}