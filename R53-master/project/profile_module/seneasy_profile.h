/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     seneasy_profile.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef SENEASY_PROFILE_H_
#define SENEASY_PROFILE_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include  "../app_config.h"
#include "../../sdk/tl_common.h"
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
extern const u8 uuid_service_seneasy_rcu[16];
extern const u8 uuid_char_seneasy_rcu_rd[16];
extern const u8 uuid_char_seneasy_rcu_wr[16];
extern const u8 char_decl_seneasy_rcu_rd[19];
extern const u8 char_decl_seneasy_rcu_wd[19];
extern u16 char_desc_smart_rcu_ccc;
extern _attribute_data_retention_ u8 seneasy_rcu_data_ccc;
extern u8 seneasy_key_flag;
extern _attribute_data_retention_ u8 seneasy_get_Lux_flag;
#if (FUNCTION_CALIBRATION_BL)
extern _attribute_data_retention_ u8 seneasy_bl_a_float;
extern _attribute_data_retention_ u16 seneasy_bl_a_data;
extern _attribute_data_retention_ u8 seneasy_bl_b_float;
extern _attribute_data_retention_ u16 seneasy_bl_b_data;
#endif
/*============================================================================*
 *                          Functions
 *============================================================================*/
int seneasy_rcu_svc_attr_write_cb(void *p);
int seneasy_rcu_svc_attr_read_cb(void *p);
int seneasy_rcu_ccc_update_cb(void *p);
void seneasy_rcu_write_fw_addr(u8 *fw_addr);
void seneasy_rcu_read_fw_addr(u8 *fw_addr);
void seneasy_rcu_key_handle(u8 notify_key);
#if (FUNCTION_CALIBRATION_BL)
void seneasy_rcu_write_test_bl_addr(u8 *fw_addr);
void read_flash_rcu_test_bl(void);
#endif
#endif /* SENEASY_PROFILE_H_ */
