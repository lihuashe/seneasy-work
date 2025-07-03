
#define FACTORY_TEST_24G 1

/**
*********************************************************************************************************
*               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     factory_test.h
* @brief
* @details
* @author   lizefeng
* @date     2024-02-29
* @version  v1.0
*********************************************************************************************************
*/

#ifndef FACTORY_TEST_H_
#define FACTORY_TEST_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/

#include "driver.h"

/*============================================================================*
 *                        Export Global Variables
 *============================================================================*/
//*********************************代码补全区**************************************//
#if FACTORY_TEST_24G
extern volatile int device_ack_received;
extern u8 device_status;
extern _attribute_data_retention_ u32 app_bath_factory_test_timer;

//*********************************代码补全区**************************************//
#if FACTORY_TEST_24G
typedef struct
{
	u8 head_0;
	u8 head_1;
	u8 cmd_0;
	u8 cmd_1;
	u8 len;
	u32 did;
	u8 value_dat[2];
	u8 software_dat[2];
} factory_data_t;

extern factory_data_t *p_factory_data;

typedef struct
{
	u8 head_0;
	u8 head_1;
	u8 cmd_0;
	u8 cmd_1;
	u8 len;
	u8 software_id[4];
	u8 software_version[2];
	u8 mac_dat[6];
} factory_soft_mac_data_t;
extern factory_soft_mac_data_t *p_factory_soft_mac_data;

typedef struct
{
	u8 head_0;
	u8 head_1;
	u8 cmd_0;
	u8 cmd_1;
	u8 len;
	u32 did;
	u8 mac_dat[6];
} factory_mac_data_t;
extern factory_mac_data_t *p_factory_mac_data;

typedef struct
{
	u8 head_0;
	u8 head_1;
	u8 cmd_0;
	u8 cmd_1;
	u8 len;
	u32 did;
	u8 value_dat[2];
} factory_ack_data_t;
extern factory_ack_data_t *p_factory_ack_data;


#endif

extern volatile u32 send_device_mac_time;
extern bool send_device_mac_flag;

extern u32 factory_dongle_id;
extern u8 factory_cmd;
extern u8 factory_channel_mode;
extern volatile u8 factory_channel;
extern volatile u8 factory_channel_begin;
extern bool production_testing;
extern bool exit_factory_test;
extern const unsigned char rf_chn[16];

/*============================================================================*
 *                          Functions
 *============================================================================*/

void app_rcu_factory_test(void);
void bath_rcu_enter_factory_test(void);
void Bath_Factory_Test_Key(u8 bath_key);
extern void rf_rx_process_factory(rf_packet_t *p_rf_data);
extern void rf_tx_process_factory(void);
void bath_rcu_enter_single_tone(void);
extern void powered_on_press_enter_production_testing();
extern void key_handle_factory_1_event();

extern void factory_test_check(void);
#endif /* FACTORY_TEST_H_ */

#endif
