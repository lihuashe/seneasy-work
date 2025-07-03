/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <key_config.h>
#include "app_function.h"


/*短按事件处理*/
OneKey one_key_short_press_handle[] = {

    {GEAR_KEY , &beauty_comb_mode_gear_add_sub_handle},
    {POWER_KEY, &beauty_comb_mode_switch_handle}
};

/*长按2s事件处理*/
OneKey one_key_long_press_2s_handle[] = {

    {GEAR_KEY , &beauty_comb_meso_mode_on_off_handle},
    {POWER_KEY, &power_on_off_handle}
};

/*长按事件处理*/
OneKey two_key_long_press_5s_handle[] = {

    {GEAR_KEY,&power_and_gear_pair_ble_handle}
};
