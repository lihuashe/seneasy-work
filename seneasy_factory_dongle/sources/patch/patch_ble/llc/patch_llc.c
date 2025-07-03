#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "llc_llp_interface.h"


#include "llc_spec_5_1.h"


#include "llc_adv_channel_pdu.h"
#include "llc_data_channel_pdu.h"
#include "llc_acl_data.h"

#include "llc_scheduler.h"

#include "llc_address.h"
#include "llc_data.h"
#include "llc_ctrl_procedure.h"
#include "llc_connection_role.h"
#include "llc_slave.h"
#include "llc_master.h"

#include "llc_ext_adv.h"
#include "llc_ext_scan.h"
#include "llc_ext_initiator.h"

#include "llc.h"
#include "patch_llc.h"


volatile api_llp_start_ext_adv_fragment g_llcExtAdvFragmentStartCallback = NULL;
volatile api_llp_start_ext_scan_fragment g_llcExtScanFragmentStartCallback = NULL;
volatile api_llp_start_ext_init_fragment g_llcExtInitFragmentStartCallback = NULL;


void extended_llc_init(void)
{
    rom_llc_ext_adv_init();
    rom_llc_ext_scan_init();
    rom_llc_ext_initiator_init();
}

void rom_llc_set_start_ext_adv_fragment_callback(api_llp_start_ext_adv_fragment callback)
{
    g_llcExtAdvFragmentStartCallback = callback;
}


EN_LLP_ERR_T rom_llc_start_ext_adv_fragment(stLlpExtAdvFragmentParam_t *pstParam)
{
    if (NULL == g_llcExtAdvFragmentStartCallback)
    {
        return LLP_ERR_UNSUPPORT;
    }
    else
    {
        return g_llcExtAdvFragmentStartCallback(pstParam);
    }
}


void rom_llc_set_start_ext_scan_fragment_callback(api_llp_start_ext_scan_fragment callback)
{
    g_llcExtScanFragmentStartCallback = callback;
}

EN_LLP_ERR_T rom_llc_start_ext_scan_fragment(stLlpExtScanFragmentParam_t *pstParam)
{
    if (NULL == g_llcExtScanFragmentStartCallback)
    {
        return LLP_ERR_UNSUPPORT;
    }
    else
    {
        return g_llcExtScanFragmentStartCallback(pstParam);
    }
}


void rom_llc_set_start_ext_init_fragment_callback(api_llp_start_ext_init_fragment callback)
{
    g_llcExtInitFragmentStartCallback = callback;
}

EN_LLP_ERR_T rom_llc_start_ext_init_fragment(stLlpExtInitFragmentParam_t *pstParam)
{
    if (NULL == g_llcExtInitFragmentStartCallback)
    {
        return LLP_ERR_UNSUPPORT;
    }
    else
    {
        return g_llcExtInitFragmentStartCallback(pstParam);
    }
}
