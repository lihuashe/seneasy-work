#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "llc_llp_interface.h"

#include "llp_hw.h"
#include "llp.h"
#include "patch_llp.h"
#include "llp_sleep.h"
#include "patch_llp_sleep.h"

#include "llc_spec_5_1.h"
#include "llc_data_channel_pdu.h"
#include "llc_acl_data.h"
#include "llc_scheduler.h"
#include "llc_data.h"
#include "llc_ctrl_procedure.h"
#include "llc_connection_role.h"
#include "llc.h"

#include "extended_controller_interface.h"

#include "patch_llc_address.h"
#include "patch_llc_privacy.h"
#include "patch_llc_white_list.h"
#include "patch_llc_priority.h"

#include "patch_llc.h"

#include "hci.h"

#include "patch_ble.h"
#include "patch_att_server.h"

#include "patch_stack_task.h"

void patch_ble_init(void)
{
    rom_llc_set_start_adv_fragment_callback(patch_llp_start_adv_fragment);
    rom_llc_set_start_scan_fragment_callback(patch_llp_start_scan_fragment);
    rom_llc_set_start_init_fragment_callback(patch_llp_start_init_fragment);
    rom_llc_set_start_slave_fragment_callback(patch_llp_start_slave_fragment);
    rom_llc_set_start_master_fragment_callback(patch_llp_start_master_fragment);

    patch_llp_sleep();
    patch_llp();

    patch_llc_privacy();
    patch_llc_address();
    patch_llc_white_list();
    patch_llc_priority();

    // patch_gatts_api
    patch_att_packet_handler_init();
    
    patch_stack_task();
}

void ble_extended_init(void)
{
    rom_llc_set_start_ext_adv_fragment_callback(patch_llp_start_ext_adv_fragment);
    rom_llc_set_start_ext_scan_fragment_callback(patch_llp_start_ext_scan_fragment);
    rom_llc_set_start_ext_init_fragment_callback(patch_llp_start_ext_init_fragment);

    extended_controller_interface();
    extended_llc_init();
}
