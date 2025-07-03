
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "llc_llp_interface.h"
#include "llc_spec_5_1.h"

#include "ble_impl.h"

#include "controller_interface.h"
#include "llc_scheduler.h"

#include "llc_address.h"
#include "llc_privacy.h"
#include "llc_adv_channel_pdu.h"
#include "llc_acl_data.h"
#include "llc_data_channel_pdu.h"
#include "llc_data.h"
#include "llc_ctrl_procedure.h"
#include "llc_connection_role.h"
#include "llc_master.h"

#include "llc_legacy_adv.h"
#include "llc_legacy_scan.h"
#include "llc_legacy_initiator.h"
#include "llc_ext_scan.h"
#include "llc_ext_initiator.h"

#include "llc_hook.h"

uint8_t patch_llc_hook_prefix_address_set_random_address(bool *pbIsDone, uint8_t pu8RandomAddr[6])
{
    if (rom_llc_ext_scan_is_enabled())
    {
        *pbIsDone = true;
        return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
    }

    if (rom_llc_ext_initiator_is_enabled())
    {
        *pbIsDone = true;
        return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
    }

    *pbIsDone = false;

    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}

void patch_llc_address(void)
{
    g_llcHookPrefixTable[LLC_HOOK_PREFIX_address_set_random_address] = patch_llc_hook_prefix_address_set_random_address;
}
