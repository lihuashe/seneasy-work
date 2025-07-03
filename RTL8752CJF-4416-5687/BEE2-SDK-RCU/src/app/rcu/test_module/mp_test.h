/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     mp_config.h
* @brief    .
* @details
* @author   ranhui_xia
* @date     2015-11-15
* @version  v1.0
*********************************************************************************************************
*/

#ifndef _MP_CONFIG_H
#define _MP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"
#include "stdbool.h"
#include "stdint.h"
#include "key_handle.h"

/*============================================================================*
 *                              Macros
 *============================================================================*/
#define FP_MAX_LINE_NUM 5
#define FP_MAC_ADDR_LEN 6
#define FP_LINK_KEY_LEN 16

#if (VOICE_FLOW_SEL == ATV_GOOGLE_VOICE_FLOW)
#define FP_CCCD_BITS_CNT 36
#define FP_CCCD_DATA_LEN 32
#else
#define FP_CCCD_BITS_CNT 16
#define FP_CCCD_DATA_LEN 12
#endif

#define MP_TEST_MODE_FLG_ENABLE_VALUE       0x74657374
#define MP_TEST_MODE_FLG_DISABLE_VALUE      0x50245150

bool mp_test_enable_test_mode_flag(void);
bool mp_test_disable_test_mode_flag(void);
bool mp_test_is_test_mode_flag_en(void);
bool mp_test_set_fast_pair_info(uint8_t index);
bool mp_test_clear_bond_info(void);
uint32_t mp_test_handle_two_keys_scenario(T_KEY_INDEX_DEF key_index_1, T_KEY_INDEX_DEF key_index_2);
bool mp_test_handle_comb_keys_timer_cb(uint32_t combine_status);
#if (MP_TEST_MODE_TRIG_SEL & MP_TEST_MODE_TRIG_BY_GPIO)
void mp_test_check_trig_gpio_status(void);
#endif
bool mp_test_load_fp_mac_addr(void);

#ifdef __cplusplus
}
#endif

#endif /* _PROFILE_INIT_H */
