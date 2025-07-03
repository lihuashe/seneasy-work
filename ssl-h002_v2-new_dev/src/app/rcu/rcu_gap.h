/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rcu_gap.h
* @brief
* @details
* @author    Chuanguo Xue
* @date      2020-02-25
* @version   v1.0
* *********************************************************************************************************
*/


#ifndef _RCU_GAP_H
#define _RCU_GAP_H

#include <stdint.h>
#include "rcu_application.h"

#ifdef __cplusplus
extern "C" {
#endif

void rcu_le_gap_init(void);
bool rcu_start_adv(T_ADV_TYPE adv_type);
bool rcu_stop_adv(T_STOP_ADV_REASON stop_adv_reason);
bool rcu_terminate_connection(T_DISCONN_REASON disconn_reason);
void rcu_update_conn_params(uint16_t interval, uint16_t latency, uint16_t timeout);
#if (FEATURE_SUPPORT_REMOVE_LINK_KEY_BEFORE_PAIRING && FEATURE_SUPPORT_RECOVER_PAIR_INFO)
void get_bond_info(void);
void set_bond_info(void);
void delete_last_bond_info(void);
extern uint16_t le_get_dev_bond_info_len(void);
extern bool le_get_dev_bond_info(T_LE_KEY_ENTRY *p_entry, uint8_t *p_data);
extern T_LE_KEY_ENTRY *le_set_dev_bond_info(uint16_t length, uint8_t *p_data, bool *exist);
#endif
extern bool BTIF_VendorGetResponse(uint8_t *pData, uint8_t len);
#ifdef __cplusplus
}
#endif

#endif /* _PROFILE_INIT_H */
