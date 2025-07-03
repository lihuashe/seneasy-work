
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "RegMXD2670.h"

#include "llc_llp_interface.h"
#include "llp_hw.h"
#include "llp.h"

#include "llp_sleep.h"
#include "patch_llp_sleep.h"
#include "utility.h"

#include "llp_hook.h"

extern volatile bool g_bIsLlpSleeping;

void patch_llp_hook_prefix_sleep_wakeup(bool *pbIsDone)
{
    *pbIsDone = true;

    uint32_t u32Primask = __get_PRIMASK();
    __disable_irq();
    if (g_bIsLlpSleeping)
    {
        uint32_t u32SlpLen = reg_ltc_ret_pd_slp_len;
        uint32_t u32SlpCnt = reg_ltc_slp_cnt;

        if (u32SlpCnt >= u32SlpLen)
        {
            rom_delay_us(100);
        }
        else if (u32SlpCnt + 5 >= u32SlpLen)
        {
            rom_delay_us(200 + 100);
        }
        else
        {
            rom_llp_hw_wakeup();
        }

        g_bIsLlpSleeping = false;
    }

    __set_PRIMASK(u32Primask);
}

void patch_llp_sleep(void)
{
    g_llpHookPrefixTable[LLP_HOOK_PREFIX_sleep_wakeup] = patch_llp_hook_prefix_sleep_wakeup;
}
