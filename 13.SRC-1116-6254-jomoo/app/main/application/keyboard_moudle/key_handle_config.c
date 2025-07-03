#include "key_driver.h"
#include "keyboard.h"
#include "bath_rcu.h"
#include "string.h"
#include "types.h"
#include "factory_test.h"
#include "function.h"
#include "key_handle_config.h"

u8 instant_press_event;
u8 release_press_event;
u8 short_press_event;
u8 press_three_s_event;
u8 press_aight_s_event;
u8 press_2_long_aight_s_event;

#if (NOW_SOFT_VERSION == SRC_1116_6242)

// 立即触发按键配置
KeyEvent INSTANT_PRESS_EVENT[] = {
    {KB_CODE_S1, Jomoo_instant_auto_stop_event, "Jomoo_instant_auto_stop_event"},
    {KB_CODE_S6, Jomoo_seat_temperature_event, "Jomoo_seat_temperature_event"},
    {KB_CODE_S7, Jomoo_water_temperature_event, "Jomoo_water_temperature_event"},
    {KB_CODE_S8, Jomoo_water_pressure_event, "Jomoo_water_pressure_event"},
};

// 按键松开配置
KeyEvent RELEASE_PRESS_EVENT[] = {
    {KB_NULL, Jomoo_null_event, "Jomoo_null_event"}};

// 短按按键配置
KeyEvent SHORT_PRESS_EVENT[] = {
    {KB_CODE_S2, Jomoo_hip_washing_movement_event, "Jomoo_hip_washing_movement_event"},
    {KB_CODE_S3, Jomoo_women_washing_movement_event, "Jomoo_women_washing_movement_event"},
    {KB_CODE_S4, Jomoo_water_scour_event, "Jomoo_water_scour_event"},
    {KB_CODE_S5, Jomoo_warm_air_air_temperature_event, "Jomoo_warm_air_air_temperature_event"},
    {KB_CODE_S9, Jomoo_seation_event, "Jomoo_seation_event"},
    {KB_CODE_S10, Jomoo_Toilet_roll_event, "Jomoo_Toilet_roll_event"},
    {KB_CODE_S11, Jomoo_Toilet_flap_event, "Jomoo_Toilet_flap_event"},
};

// 长按三秒按键配置
KeyEvent PRESS_THREE_S_EVENT[] = {
    {KB_CODE_S1, Jomoo_pair_event, "Jomoo_pair_event"},
    {KB_CODE_S2, Jomoo_Strong_washing_event, "Jomoo_Strong_washing_event"},
    {KB_CODE_S3, Jomoo_child_washing_event, "Jomoo_child_washing_event"},
    {KB_CODE_S4, Jomoo_aotu_scour_open_off_event, "Jomoo_aotu_scour_open_off_event"},
    {KB_CODE_S5, Jomoo_pre_wetting_event, "Jomoo_pre_wetting_event"},
    {KB_CODE_S9, Jomoo_energy_open_off_event, "Jomoo_energy_conservation_off_event"},
    {KB_CODE_S10, Jomoo_intelligent_night_light_event, "Jomoo_intelligent_night_light_event"},
    {KB_CODE_S11, Jomoo_nozzle_clean_event, "Jomoo_nozzle_clean_event"},
};

// 长按八秒按键配置
KeyEvent PRESS_AIGHT_S_EVENT[] = {
    {KB_NULL, Jomoo_null_event, "Jomoo_null_event"}};

// 组合键配置
KeyCombinationEvent PRESS_2_LONG_AIGHT_S_EVENT[] = {
    {KB_NULL, KB_NULL, Jomoo_null_event, "Jomoo_null_event"}};

#elif (NOW_SOFT_VERSION == SRC_1116_6254)

// 立即触发按键配置
KeyEvent INSTANT_PRESS_EVENT[] = {
    {KB_CODE_S1, Jomoo_instant_stop_event, "Jomoo_instant_stop_event"},
};

// 按键松开配置
KeyEvent RELEASE_PRESS_EVENT[] = {
    {KB_CODE_S3, Jomoo_hip_washing_movement_event, "Jomoo_hip_washing_movement_event"},
    {KB_CODE_S4, Jomoo_women_washing_movement_event, "Jomoo_women_washing_movement_event"},
};

// 短按按键配置
KeyEvent SHORT_PRESS_EVENT[] = {
    {KB_CODE_S2, Jomoo_water_sml_event, "Jomoo_water_sml_event"},
    {KB_CODE_S5, Jomoo_foam_shield_event, "Jomoo_foam_shield_event"},
    {KB_CODE_S6, Jomoo_seat_temperature_event, "Jomoo_seat_temperature_event"},
    {KB_CODE_S7, Jomoo_water_temperature_event, "Jomoo_water_temperature_event"},
    {KB_CODE_S8, Jomoo_water_pressure_event, "Jomoo_water_pressure_event"},
    {KB_CODE_S9, Jomoo_warm_air_air_temperature_event,"Jomoo_warm_air_air_temperature_event"},
    {KB_CODE_S10, Jomoo_move_back_event,"Jomoo_move_back_event"},
    {KB_CODE_S11, Jomoo_move_forward_event,"Jomoo_move_forward_event"},
};

// 长按三秒按键配置
KeyEvent PRESS_THREE_S_EVENT[] = {
    {KB_CODE_S1, Jomoo_pair_event, "Jomoo_pair_event"},
    {KB_CODE_S2, Jomoo_water_scour_event, "Jomoo_water_scour_event"},
    {KB_CODE_S5, Jomoo_seat_foaming_event, "Jomoo_seat_foaming_event"},
    {KB_CODE_S6, Jomoo_energy_conservation_off_event, "Jomoo_energy_conservation_off_event"},
    {KB_CODE_S7, Jomoo_intelligent_night_light_event, "Jomoo_intelligent_night_light_event"},
    {KB_CODE_S8, Jomoo_nozzle_clean_event, "Jomoo_nozzle_clean_event"},
    {KB_CODE_S9, Jomoo_pre_wetting_event,"Jomoo_pre_wetting_event"},
    {KB_CODE_S10, Jomoo_foot_sensation_event,"Jomoo_foot_sensation_event"},
    {KB_CODE_S11, Jomoo_aotu_scour_open_event,"Jomoo_aotu_scour_open_event"},
};

// 长按八秒按键配置
KeyEvent PRESS_AIGHT_S_EVENT[] = {
    {KB_CODE_S6, Jomoo_energy_conservation_open_event, "Jomoo_energy_conservation_open_event"},
    {KB_CODE_S11, Jomoo_aotu_scour_off_event, "Jomoo_aotu_scour_off_event"},
};

// 组合键配置
KeyCombinationEvent PRESS_2_LONG_AIGHT_S_EVENT[] = {
    {KB_CODE_S3, KB_CODE_S4, Jomoo_combination_filter_reset_event, "Jomoo_combination_filter_reset_event"},
    {KB_CODE_S2, KB_CODE_S4, Jomoo_combination_filter_stop_event, "Jomoo_combination_filter_stop_event"},
    {KB_CODE_S3, KB_CODE_S2, Jomoo_combination_filter_time_event, "Jomoo_combination_filter_time_event"},
};
#endif

void Variable_tansfer()
{
    instant_press_event = sizeof(INSTANT_PRESS_EVENT);
    release_press_event = sizeof(RELEASE_PRESS_EVENT);
    short_press_event = sizeof(SHORT_PRESS_EVENT);
    press_three_s_event = sizeof(PRESS_THREE_S_EVENT);
    press_aight_s_event = sizeof(PRESS_AIGHT_S_EVENT);
    press_2_long_aight_s_event = sizeof(PRESS_2_LONG_AIGHT_S_EVENT);
}
