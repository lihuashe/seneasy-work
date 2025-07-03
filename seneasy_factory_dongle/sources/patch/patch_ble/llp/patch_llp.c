#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "RegMXD2670.h"

#include "llc_llp_interface.h"

#include "mlog.h"

#include "llp_hw.h"
#include "llp.h"
#include "patch_llp.h"
#include "llp_adv.h"
#include "patch_llp_adv.h"
#include "llp_scan.h"
#include "patch_llp_scan.h"
#include "llp_initiator.h"
#include "patch_llp_initiator.h"
#include "llp_slave.h"
#include "patch_llp_slave.h"
#include "llp_master.h"
#include "patch_llp_master.h"
#include "llp_ext_adv.h"
#include "patch_llp_ext_adv.h"
#include "llp_ext_scan.h"
#include "patch_llp_ext_scan.h"
#include "llp_ext_initiator.h"
#include "patch_llp_ext_initiator.h"
#include "llp_sleep.h"
#include "patch_llp_sleep.h"
#include "llp_rssi.h"

#include "utility.h"
#include "mlog.h"

#include "llp_hook.h"
#include "rom_callback.h"

extern volatile stLlpFragment_t *g_llpWorkingLlpFragment;

extern uint64_t rom_llp_scan_init_start_time_adjust(uint64_t u64StartTime, bool bIsSecondaryChannel,
                                                    EN_LL_PHY_T enumPhy);
extern uint32_t rom_llp_scan_init_work_time_adjust(uint32_t u32WorkTime, bool bIsSecondaryChannel, EN_LL_PHY_T enumPhy);
extern uint32_t rom_llp_scan_init_anchor_window_adjust(uint32_t u32AnchorWindowUs, bool bIsSecondaryChannel,
                                                       EN_LL_PHY_T enumPhy);
extern void     rom_llp_scan_init_mode_rx_en_time_adjust(bool bIsSecondaryChannel);
extern void     rom_llp_scan_init_mode_rx_en_time_restore(void);

EN_LLP_ERR_T patch_llp_start_adv_fragment(stLlpAdvFragmentParam_t *pstParam)
{
    if (NULL != g_llpWorkingLlpFragment)
    {
        return LLP_ERR_SYSTEM;
    }

    rom_llp_sleep_wakeup();

    EN_LLP_ERR_T err = patch_llp_adv_fragment_start(pstParam);
    if (LLP_ERR_NONE == err)
    {
        g_llpWorkingLlpFragment = (volatile stLlpFragment_t *)&g_stLlpAdvFragment;
        rom_llp_sleep_on_fragment_config_succeed(pstParam->u64StartTimeUs);
    }

    return err;
}

EN_LLP_ERR_T patch_llp_start_scan_fragment(stLlpScanFragmentParam_t *pstParam)
{
    if (NULL != g_llpWorkingLlpFragment)
    {
        return LLP_ERR_SYSTEM;
    }

    rom_llp_sleep_wakeup();

    ////@ref redmine issues 2339(http://10.5.5.64/issues/2339)
    pstParam->u64StartTimeUs = rom_llp_scan_init_start_time_adjust(pstParam->u64StartTimeUs, false, pstParam->enumPhy);
    pstParam->u32WorkTimeUs  = rom_llp_scan_init_work_time_adjust(pstParam->u32WorkTimeUs, false, pstParam->enumPhy);
    EN_LLP_ERR_T err         = patch_llp_scan_fragment_start(pstParam);
    rom_llp_scan_init_mode_rx_en_time_adjust(false);

    if (LLP_ERR_NONE == err)
    {
        g_llpWorkingLlpFragment = (volatile stLlpFragment_t *)&g_stLlpScanFragment;
        rom_llp_sleep_on_fragment_config_succeed(pstParam->u64StartTimeUs);
    }

    return err;
}

EN_LLP_ERR_T patch_llp_start_init_fragment(stLlpInitFragmentParam_t *pstParam)
{
    if (NULL != g_llpWorkingLlpFragment)
    {
        return LLP_ERR_SYSTEM;
    }

    rom_llp_sleep_wakeup();

    ////@ref redmine issues 2339(http://10.5.5.64/issues/2339)
    pstParam->u64StartTimeUs = rom_llp_scan_init_start_time_adjust(pstParam->u64StartTimeUs, false, pstParam->enumPhy);
    pstParam->u32WorkTimeUs  = rom_llp_scan_init_work_time_adjust(pstParam->u32WorkTimeUs, false, pstParam->enumPhy);
    EN_LLP_ERR_T err         = patch_llp_initiator_fragment_start(pstParam);
    rom_llp_scan_init_mode_rx_en_time_adjust(false);

    if (LLP_ERR_NONE == err)
    {
        g_llpWorkingLlpFragment = (volatile stLlpFragment_t *)&g_stLlpInitiatorFragment;
        rom_llp_sleep_on_fragment_config_succeed(pstParam->u64StartTimeUs);
    }

    return err;
}

EN_LLP_ERR_T patch_llp_start_slave_fragment(stLlpSlaveFragmentParam_t *pstParam)
{
    if (NULL != g_llpWorkingLlpFragment)
    {
        return LLP_ERR_SYSTEM;
    }

    rom_llp_sleep_wakeup();

    EN_LLP_ERR_T err = patch_llp_slave_fragment_start(pstParam);
    if (LLP_ERR_NONE == err)
    {
        g_llpWorkingLlpFragment = (volatile stLlpFragment_t *)&g_stLlpSlaveFragment;
        rom_llp_sleep_on_fragment_config_succeed(pstParam->u64StartTimeUs);
    }

    return err;
}

EN_LLP_ERR_T patch_llp_start_master_fragment(stLlpMasterFragmentParam_t *pstParam)
{
    if (NULL != g_llpWorkingLlpFragment)
    {
        return LLP_ERR_SYSTEM;
    }

    rom_llp_sleep_wakeup();

    EN_LLP_ERR_T err = patch_llp_master_fragment_start(pstParam);
    if (LLP_ERR_NONE == err)
    {
        g_llpWorkingLlpFragment = (volatile stLlpFragment_t *)&g_stLlpMasterFragment;
        rom_llp_sleep_on_fragment_config_succeed(pstParam->u64StartTimeUs);
    }

    return err;
}

EN_LLP_ERR_T patch_llp_start_ext_adv_fragment(stLlpExtAdvFragmentParam_t *pstParam)
{
    if (NULL != g_llpWorkingLlpFragment)
    {
        return LLP_ERR_SYSTEM;
    }

    rom_llp_sleep_wakeup();

    EN_LLP_ERR_T err = patch_llp_ext_adv_fragment_start(pstParam);
    if (LLP_ERR_NONE == err)
    {
        g_llpWorkingLlpFragment = (volatile stLlpFragment_t *)&g_stLlpExtAdvFragment;
        rom_llp_sleep_on_fragment_config_succeed(pstParam->u64StartTimeUs);
    }

    return err;
}

EN_LLP_ERR_T patch_llp_start_ext_scan_fragment(stLlpExtScanFragmentParam_t *pstParam)
{
    if (NULL != g_llpWorkingLlpFragment)
    {
        return LLP_ERR_SYSTEM;
    }

    rom_llp_sleep_wakeup();

    ////@ref redmine issues 2339(http://10.5.5.64/issues/2339)
    pstParam->u64StartTimeUs =
        rom_llp_scan_init_start_time_adjust(pstParam->u64StartTimeUs, pstParam->bIsSecondaryChannel, pstParam->enumPhy);
    pstParam->u32WorkTimeUs =
        rom_llp_scan_init_work_time_adjust(pstParam->u32WorkTimeUs, pstParam->bIsSecondaryChannel, pstParam->enumPhy);
    pstParam->u32AnchorWindowUs = rom_llp_scan_init_anchor_window_adjust(
        pstParam->u32AnchorWindowUs, pstParam->bIsSecondaryChannel, pstParam->enumPhy);
    EN_LLP_ERR_T err = patch_llp_ext_scan_fragment_start(pstParam);
    rom_llp_scan_init_mode_rx_en_time_adjust(pstParam->bIsSecondaryChannel);

    if (LLP_ERR_NONE == err)
    {
        g_llpWorkingLlpFragment = (volatile stLlpFragment_t *)&g_stLlpExtScanFragment;
        rom_llp_sleep_on_fragment_config_succeed(pstParam->u64StartTimeUs);
    }

    return err;
}

EN_LLP_ERR_T patch_llp_start_ext_init_fragment(stLlpExtInitFragmentParam_t *pstParam)
{
    if (NULL != g_llpWorkingLlpFragment)
    {
        return LLP_ERR_SYSTEM;
    }

    rom_llp_sleep_wakeup();

    ////@ref redmine issues 2339(http://10.5.5.64/issues/2339)
    pstParam->u64StartTimeUs =
        rom_llp_scan_init_start_time_adjust(pstParam->u64StartTimeUs, pstParam->bIsSecondaryChannel, pstParam->enumPhy);
    pstParam->u32WorkTimeUs =
        rom_llp_scan_init_work_time_adjust(pstParam->u32WorkTimeUs, pstParam->bIsSecondaryChannel, pstParam->enumPhy);
    pstParam->u32AnchorWindowUs = rom_llp_scan_init_anchor_window_adjust(
        pstParam->u32AnchorWindowUs, pstParam->bIsSecondaryChannel, pstParam->enumPhy);
    EN_LLP_ERR_T err = patch_llp_ext_initiator_fragment_start(pstParam);
    rom_llp_scan_init_mode_rx_en_time_adjust(pstParam->bIsSecondaryChannel);

    if (LLP_ERR_NONE == err)
    {
        g_llpWorkingLlpFragment = (volatile stLlpFragment_t *)&g_stLlpExtInitiatorFragment;
        rom_llp_sleep_on_fragment_config_succeed(pstParam->u64StartTimeUs);
    }

    return err;
}

void patch_llp(void)
{
    // nothing now
}
