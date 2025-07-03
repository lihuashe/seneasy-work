/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     rf_24g_driver.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef RF_24G_DRIVER_H_
#define RF_24G_DRIVER_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "../app_config.h"
#include "../../tl_common.h"

/*============================================================================*
*                        Export Global Variables
*============================================================================*/
#if (AES_METHOD == 1 || DATA_3_CHOOSE_1_ENABLE == 1)
	#define D24G_PAIR_TIMER_OUT 			400 //unit 1us
	#define D24G_COMMUNICATION_TIMER_OUT	300 //unit 1us
#else
	#define D24G_PAIR_TIMER_OUT 			350 //unit 1us
	#define D24G_COMMUNICATION_TIMER_OUT	220 //unit 1us
#endif
	#define D24G_OTA_TIMER_OUT 				500 //unit 1us
	#define KEY_PRESS_HOLD_CHECK_TIME		980  //unit 1ms

typedef enum{
	RF_IDLE_STATUS		= 0,
    RF_TX_START_STATUS	= 1,
    RF_TX_END_STATUS	= 2,
    RF_RX_START_STATUS	= 3,
    RF_RX_END_STATUS	= 4,
	RF_RX_TIMEOUT_STATUS= 5,
}RF_STATUS_USER;

_attribute_data_retention_ volatile unsigned int rf_rx_timeout_us;
extern _attribute_data_retention_ u8 device_channel;
/*============================================================================*
 *                          Functions
 *============================================================================*/
void rf_24g_driver_init(void);
void set_pair_access_code(u32 code);
void set_data_access_code(u32 code);
void rf_irq_src_allclr(void);
_attribute_ram_code_ void rf_24g_irq_handle(void);
_attribute_ram_code_sec_ void rf_rx_fast_settle_init(void);
_attribute_ram_code_sec_ void rf_rx_fast_settle_en(void);
_attribute_ram_code_sec_ void rf_rx_fast_settle_dis(void);
_attribute_ram_code_sec_ void rf_tx_fast_settle_init(void);
_attribute_ram_code_sec_ void rf_tx_fast_settle_en(void);
_attribute_ram_code_sec_ void rf_tx_fast_settle_dis(void);
_attribute_ram_code_sec_ void check_rf_fast_setting_time(void);
_attribute_ram_code_sec_ void check_rf_fast_setting_flag(void);
_attribute_ram_code_sec_  u8 get_next_channel_with_mask(u32 mask, u8 chn);
_attribute_ram_code_ void my_rf_start_srx(unsigned int tick,unsigned int rx_timeout);
_attribute_ram_code_ void rf_stx_to_rx(u8 *p,u32 rx_timeout_us);
_attribute_ram_code_ void irq_device_rx(void);

#endif /* RF_24G_DRIVER_H_ */
