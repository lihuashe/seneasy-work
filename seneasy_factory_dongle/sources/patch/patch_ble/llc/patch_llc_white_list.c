#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "llc_llp_interface.h"

#include "llc_spec_5_1.h"
#include "ble_impl.h"
#include "llc_scheduler.h"

#include "llc_adv_channel_pdu.h"
#include "llc_data_channel_pdu.h"
#include "llc_acl_data.h"
#include "llc_data.h"
#include "llc_ctrl_procedure.h"
#include "llc_connection_role.h"
#include "llc_slave.h"
#include "llc_master.h"
#include "llc_legacy_adv.h"
#include "llc_ext_adv.h"
#include "llc_ext_scan.h"
#include "llc_ext_initiator.h"

#include "llc_hook.h"

#include "llc_white_list.h"
#include "patch_llc_white_list.h"

extern stLlcWhiteListDevice_t *g_pstWhiteList;
extern uint8_t                 g_u8WhiteListMaxNum;

uint8_t patch_llc_hook_prefix_white_list_clear(bool *pbIsDone)
{
    if (rom_llc_ext_adv_is_white_list_in_use())
    {
        *pbIsDone = true;
        return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
    }

    if (rom_llc_ext_scan_is_white_list_in_use())
    {
        *pbIsDone = true;
        return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
    }

    if (rom_llc_ext_initiator_is_white_list_in_use())
    {
        *pbIsDone = true;
        return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
    }

    *pbIsDone = false;
    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}

uint8_t patch_llc_hook_prefix_white_list_add_device(bool *pbIsDone, uint8_t u8AddressType, uint8_t pu8Address[6])
{
    if (rom_llc_ext_adv_is_white_list_in_use())
    {
        *pbIsDone = true;
        return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
    }

    if (rom_llc_ext_scan_is_white_list_in_use())
    {
        *pbIsDone = true;
        return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
    }

    if (rom_llc_ext_initiator_is_white_list_in_use())
    {
        *pbIsDone = true;
        return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
    }

    *pbIsDone = false;

    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}

uint8_t patch_llc_hook_prefix_white_list_remove_device(bool *pbIsDone, uint8_t u8AddressType, uint8_t pu8Address[6])
{
    if (rom_llc_ext_adv_is_white_list_in_use())
    {
        *pbIsDone = false;
        return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
    }

    if (rom_llc_ext_scan_is_white_list_in_use())
    {
        *pbIsDone = false;
        return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
    }

    if (rom_llc_ext_initiator_is_white_list_in_use())
    {
        *pbIsDone = false;
        return LLC_SPEC_5_1_ERROR_CODE_COMMAND_DISALLOWED;
    }

    *pbIsDone = false;

    return LLC_SPEC_5_1_ERROR_CODE_SUCCESS;
}

void patch_llc_white_list(void)
{
    g_llcHookPrefixTable[LLC_HOOK_PREFIX_white_list_clear]         = patch_llc_hook_prefix_white_list_clear;
    g_llcHookPrefixTable[LLC_HOOK_PREFIX_white_list_add_device]    = patch_llc_hook_prefix_white_list_add_device;
    g_llcHookPrefixTable[LLC_HOOK_PREFIX_white_list_remove_device] = patch_llc_hook_prefix_white_list_remove_device;
}
