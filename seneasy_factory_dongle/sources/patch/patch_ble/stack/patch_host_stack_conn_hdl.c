

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "bluetooth.h"
#include "hci.h"
#include "host_stack_conn_hdl.h"
#include "host_stack_mem.h"
#include "host_interface.h"
#include "att_server.h"
#include "llc_spec_5_1.h"
#include "sm.h"
#include "patch_host_stack_conn_hdl.h"

extern host_connection_t *gp_host_conn;
extern void rom_host_stack_mem_free_on_evt_disconnected(st_stack_acl_data_mngr_list_t *p_acl_data_mngr_list);
extern void rom_gatt_client_on_event_disconn_commpelete ( t_gatt_client *peripheral );

void patch_host_connection_on_disconn_event ( uint16_t conn_handle )
{
    uint16_t i = 0;
    rom_clear_tx_acl_data_on_disconn_event (   conn_handle );
    if ( NULL == gp_host_conn )
    {
        return ;
    }
    for ( i = 0; i < g_ble_mem_cfg.max_conn_num; i++ )
    {
        if ( gp_host_conn[i].conn_handle == conn_handle )
        {
            gp_host_conn[i].conn_handle  = 0;
            gp_host_conn[i].conn_state = 0;
            gp_host_conn[i].rx_cached_num = 0;  
            gp_host_conn[i].st_att_state.conn_handle  = 0;

            rom_sm_event_disconn_complete_for_multi_con ( &gp_host_conn[i].st_sm_state );
            rom_host_stack_mem_free_on_evt_disconnected(&gp_host_conn[i].st_acl_data_mngr_list); 
            gp_host_conn[i].st_sm_state.conn_handle  = 0;
            gp_host_conn[i].st_att_state.conn_handle  =0;
            gp_host_conn[i].st_gatt_client_state.conn_handle  = 0;
            gp_host_conn[i].st_sm_state.conn_state = 0;
            gp_host_conn[i].st_att_state.connection_state = 0;
            gp_host_conn[i].st_gatt_client_state.connection_state = 0;
            rom_gatt_client_on_event_disconn_commpelete ( &gp_host_conn[i].st_gatt_client_state);
            //add
            gp_host_conn[i].st_gatt_client_state.gatt_client_state = P_READY;
        }
    }

}
