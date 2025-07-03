
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "llc_llp_interface.h"

#include "llc_spec_5_1.h"

#include "ble_impl.h"
#include "llc_privacy.h"
#include "llc_tool.h"
#include "llc.h"

#include "llc_adv_channel_pdu.h"
#include "llc_data_channel_pdu.h"
#include "llc_acl_data.h"
#include "llc_scheduler.h"

#include "llc_data.h"
#include "llc_ctrl_procedure.h"
#include "llc_connection_role.h"
#include "llc_slave.h"
#include "llc_master.h"

#include "llc_legacy_adv.h"
#include "llc_legacy_scan.h"
#include "llc_legacy_initiator.h"
#include "llc_ext_adv.h"
#include "llc_ext_scan.h"
#include "llc_ext_initiator.h"

#include "llc_hook.h"

#include "llc_privacy.h"
#include "patch_llc_privacy.h"

bool rom_llc_privacy_is_adv_scan_init_active(void);

extern stLlcResolvingListDevice_t *g_pstResolvingList;
extern uint8_t                     g_u8ResolvingListMaxNum;
extern bool                        g_bAddressResolvingActive;

bool patch_llc_hook_prefix_privacy_is_adv_scan_init_active(bool *pbIsDone)
{
    if (rom_llc_ext_adv_is_any_advertising_set_enabled())
    {
        *pbIsDone = true;
        return true;
    }

    if (rom_llc_ext_scan_is_enabled())
    {
        *pbIsDone = true;
        return true;
    }

    if (rom_llc_ext_initiator_is_enabled())
    {
        *pbIsDone = true;
        return true;
    }

    *pbIsDone = false;

    return false;
}

void patch_llc_privacy(void)
{
    g_llcHookPrefixTable[LLC_HOOK_PREFIX_privacy_is_adv_scan_init_active] =
        patch_llc_hook_prefix_privacy_is_adv_scan_init_active;
}
