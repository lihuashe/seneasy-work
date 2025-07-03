/*
MLOG_ID_RANGE: 0x5080~0x50BF
MLOG_ID_USED:  0x5080~0x5091
*/
/*MLOGIDRANGE:0x5080 ~ 0x50BF :  stack_ctrl.c*/
/**********************************************************************************************************************
 * @file     patch_stack_task.c
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2023/12/25
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
#include "stack_utility.h"
#include "error_code.h"
#include "host_stack_mem.h"
#include "gatt_client_api.h"
#include "hci.h"
#include "host_stack_conn_hdl.h"
#include "patch_host_stack_conn_hdl.h"
#include "sm.h"
#include "llc_spec_5_1.h"
#include "gap_def.h"
#include "gatt_service_api.h"
#include "att_db_util.h"
#include "att_server.h"
#include "sm.h"
#include "ble_cb_event_def.h"
#include "gatt_client_api.h"
#include "gap_api.h"
#include "l2cap_signaling.h"
#include "ble_callback.h"
#include "stack_task.h"
#include "host_hook.h"
#include "mlog.h"

extern void  rom_host_handle_rx_rsp_after_free_a_buffer();
extern void rom_rx_acl_data_handler ( uint16_t conn_handle ,bool bRepeat);
extern void rom_stack_task_command_opcode_parse(uint16_t Command_Opcode, uint8_t *pu8OGF, uint16_t *pu16OCF);

extern void rom_stack_task_on_recv_link_control_command_complete(uint16_t OCF, uint8_t* Return_Parameters, uint8_t Return_Parameters_Size);
extern void rom_stack_task_on_recv_status_parameters_command_complete(uint16_t OCF, uint8_t* Return_Parameters, uint8_t Return_Parameters_Size);
extern void rom_stack_task_on_recv_le_controller_command_complete(uint16_t OCF, uint8_t* Return_Parameters, uint8_t Return_Parameters_Size);
extern void rom_stack_task_on_recv_vendor_command_complete(uint16_t OCF, uint8_t* Return_Parameters, uint8_t Return_Parameters_Size);

extern void rom_stack_task_on_recv_link_control_command_status(uint16_t OCF, uint8_t Status);
extern void rom_stack_task_on_recv_status_parameters_command_status(uint16_t OCF, uint8_t Status);
extern void rom_stack_task_on_recv_le_controller_command_status(uint16_t OCF, uint8_t Status);
extern void rom_stack_task_on_recv_vendor_command_status(uint16_t OCF, uint8_t Status);

void patch_on_prefix_st_ctrl_task_handler (bool* bIsDone,  stack_msg_st *pmsg)
{
    if( ST_CTRL_DISCONNECT_COMPLETE_EVT == pmsg->module_type )
    {
        stHciEventParamDisconnectionComplete_t *stdis = ( stHciEventParamDisconnectionComplete_t * ) pmsg->p_ext_info;

        MLOG88D ( 0x5092, "patch disconnnection handle=0x%x--reason=0x%x", stdis->Connection_Handle, stdis->Reason );

        patch_host_connection_on_disconn_event ( stdis->Connection_Handle );
        rom_host_handle_rx_rsp_after_free_a_buffer();
        ble_callback_send_event ( MSG_BLE_DISCONNECTED_IND, pmsg->p_ext_info, sizeof ( stHciEventParamDisconnectionComplete_t ) );
        *bIsDone = true;
    }
}

void patch_stack_task(void)
{
    g_hostHookPrefixTable[HOST_HOOK_PREFIX_host_stack_task_handler] = patch_on_prefix_st_ctrl_task_handler;
}