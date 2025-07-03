/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     rf_24g_handle.c
* @brief
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "rf_24g_handle.h"
#include "rf_24g_driver.h"
#include "../key_module/key_driver.h"
#include "../led_module/led_driver.h"
#include "../keyboard_rcu.h"
#include "../app.h"
#include "../factory_module/factory_profile.h"
#include "../../sdk/chip/B80/drivers/lib/include/rf_drv.h"

/*============================================================================*
 *                              Global Variables
 *============================================================================*/
#define DEVICE_TYPE_INDEX 2 // 1 mouse, 2 keyboard
#define SCAN_INTERVAL_TIME 6

#define DEFAULT_NORMAL_TX_POWER RF_POWER_N0p22dBm /**< -0.2 dbm */
#define DEFAULT_PAIR_TX_POWER RF_POWER_N15p01dBm  /**< -15.0 dbm */
#define DEFAULT_EMI_TX_POWER RF_POWER_N0p22dBm	  /**< -0.2 dbm */

u8 pub_key[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
_attribute_data_retention_ u8 private_key[16] =
	{
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
_attribute_data_retention_ u32 dongle_id;
_attribute_data_retention_ u8 device_name_len = 0;
_attribute_data_retention_ FLASH_DEV_INFO_AAA flash_dev_info;
_attribute_data_retention_ u8 pair_flag = 0;
_attribute_data_retention_ u8 device_status = 0;
_attribute_data_retention_ u32 wakeup_next_tick = 0;
_attribute_data_retention_ u32 idle_tick;
_attribute_data_retention_ u32 idle_count;
_attribute_data_retention_ u32 loop_cnt;
_attribute_data_retention_ u32 tick_loop = 0;
_attribute_data_retention_ volatile u16 no_ack = 0;
_attribute_data_retention_ u32 adv_begin_tick;
_attribute_data_retention_ u32 adv_count = 0;
_attribute_data_retention_ u8 dev_info_idx;
u8 need_suspend_flag = 0;
custom_cfg_t user_cfg;

rf_packet_t rf_pair_buf =
	{
		20, // dma_len
		19, // rf_len
};
pair_data_t *p_pair_dat = (pair_data_t *)&rf_pair_buf.dat[0];

rf_packet_t rf_km_buf =
	{
#if DATA_3_CHOOSE_1_ENABLE
		55, // dma_len
		54, // rf_len
#else
		14, // dma_len
		13, // rf_len
#endif
};
km_3_c_1_data_t *p_km_data = (km_3_c_1_data_t *)&rf_km_buf.dat[0];

/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief  rf_24g_main_loop
 * @param  none
 * @return none
 * @retval void
 */
void rf_24g_main_loop(void)
{
	if (need_suspend_flag)
	{
		keyboard_rcu_loop();
		tick_loop = clock_time() | 1;
	}
	else if (clock_time_exceed(tick_loop, SCAN_INTERVAL_TIME * 1000))
	{
		tick_loop += SCAN_INTERVAL_TIME * CLOCK_16M_SYS_TIMER_CLK_1MS;
		wakeup_next_tick = clock_time() | 1;
		keyboard_rcu_loop();
	}

#if TRAN_MODE == FYLL_MODE
	d24g_fyll_loop();
#else
	d24g_rf_loop();
#endif

	rcu_main_loop();
}

/******************************************************************
 * @brief  rf_24g_config
 * @param  none
 * @return none
 * @retval void
 */
void rf_24g_config(void)
{
	random_generator_init(); // this is must
	flash_read_page(CFG_ADR_MAC, sizeof(custom_cfg_t), (u8 *)&user_cfg.dev_mac);
	if (user_cfg.dev_mac == U32_MAX)
	{
		generateRandomNum(4, (u8 *)&user_cfg.dev_mac);
		app_rcu_flash_unlock_init(); // UNLOCK
		flash_write_page(CFG_ADR_MAC, 4, (u8 *)&user_cfg.dev_mac);
		app_rcu_flash_lock_init(); // LOCK
	}
	if (user_cfg.paring_tx_power == U8_MAX)
		user_cfg.paring_tx_power = DEFAULT_PAIR_TX_POWER;

	if (user_cfg.tx_power == U8_MAX)
		user_cfg.tx_power = DEFAULT_NORMAL_TX_POWER;

	if (user_cfg.emi_tx_power == U8_MAX)
		user_cfg.emi_tx_power = DEFAULT_EMI_TX_POWER;

	// aes Key
	for (u8 i = 0; i < 16; i++)
	{
		if (user_cfg.pub_key[i] != 0xff)
		{
			memcpy(pub_key, &user_cfg.pub_key[0], 16);
			break;
		}
	}
	// flash_dev_info.dongle_id = user_cfg.dev_mac;
}

/******************************************************************
 * @brief  rf_24g_user_init
 * @param  none
 * @return none
 * @retval void
 */
void rf_24g_user_init(void)
{
	// printf("rf_24g_user_init\n");
	u32 device_id = ((user_cfg.dev_mac & 0xFFFFFF00) | DEVICE_TYPE_INDEX);

	p_pair_dat->cmd = PAIR_CMD;
	p_pair_dat->did = device_id;
	p_km_data->did = device_id;

#if (AES_METHOD == 1)
	p_km_data->cmd = KB_CMD | (1 << 7);
	memcpy((u8 *)&private_key[0], (u8 *)&device_id, 4);
#else
	p_km_data->cmd = KB_CMD;
#endif

#if D24G_OTA_ENABLE_AAA
	p_ota_data->cmd = D24G_OTA_CMD;
	p_ota_data->did = device_id;
	p_ota_ack_data.did = device_id;
#endif

	dongle_id = flash_dev_info.dongle_id;
	memcpy((u8 *)&private_key[0], (u8 *)&device_id, 4);

#if ENTER_PAIR_WHEN_NEVER_PAIRED_ENABLE
	if ((dongle_id == U32_MAX) || (dongle_id == 0))
		set_pair_flag();
#endif

	if (pair_flag)
	{
		device_status = STATE_PAIRING;
		rf_rx_timeout_us = D24G_PAIR_TIMER_OUT;
#if (AES_METHOD == 0)
		p_pair_dat->did = device_id;
#elif (AES_METHOD == 1)
		generateRandomNum(12, &private_key[4]);
		aes_user_encryption(pub_key, private_key, (u8 *)&p_pair_dat->did);
#endif
	}
#if D24G_OTA_ENABLE_AAA
	else if (d24g_ota_status)
	{
		device_status = STATE_OTA;
		rf_rx_timeout_us = D24G_OTA_TIMER_OUT;
	}
#endif
	else
	{
		device_status = STATE_NORMAL;
		rf_rx_timeout_us = D24G_COMMUNICATION_TIMER_OUT;
#if (AES_METHOD == 1)
		memcpy((u8 *)&private_key[4], (u8 *)&flash_dev_info.key[0], 12);
#endif
	}
	wakeup_next_tick = clock_time();
	reset_idle_status();
}

/******************************************************************
 * @brief  reset_idle_status
 * @param  none
 * @return none
 * @retval void
 */
void reset_idle_status(void)
{
	if (pair_flag)
	{
		return;
	}
	idle_count = 0;
	loop_cnt = 0;
	idle_tick = clock_time();
	// adv_begin_tick = idle_tick | 1;
	// adv_count = 0;
}

/******************************************************************
 * @brief  save_dev_info_flash
 * @param  none
 * @return none
 * @retval void
 */
void save_dev_info_flash()
{
	app_rcu_flash_unlock_init(); // UNLOCK
	flash_erase_sector(CFG_DEVICE_MODE_ADDR);
	flash_write_page(CFG_DEVICE_MODE_ADDR, 4, (u8 *)&flash_dev_info.dongle_id);
	app_rcu_flash_lock_init(); // LOCK
	printf("[save_dev_info_flash]flash_dev_info.dongle_id = %x\r\n", flash_dev_info.dongle_id);
}

/******************************************************************
 * @brief  flash_info_load_aaa
 * @param  none
 * @return none
 * @retval void
 */
// len must 4 bytes bei beisu  beacuse *(volatile u32 *)(s_addr + idx) must 4 bytes bei beisu
int flash_info_load_aaa(u32 s_addr, u8 *d_addr, int len)
{
	int idx;
	u32 buf;
	for (idx = 0; idx < (4096 - len); idx += len)
	{
		flash_read_page((u32)(s_addr + idx), 4, (u8 *)(&buf));
		if (buf == U32_MAX)
			break;
	}
	idx -= len;
	if (idx < 0) // no binding
		return idx;
	flash_read_page((u32)(s_addr + idx), len, d_addr);

	if (idx > 3000) // 3k, erase flash
	{
		app_rcu_flash_unlock_init(); // UNLOCK
		flash_erase_sector((u32)s_addr);
		sleep_us(10);
		flash_write_page((u32)s_addr, len, d_addr);
		idx = 0;
		app_rcu_flash_lock_init(); // LOCK
	}
	return idx;
}

/******************************************************************
 * @brief  check_rf_complet_status
 * @param  none
 * @return none
 * @retval void
 */
void check_rf_complet_status(void)
{
	rf_state = RF_IDLE_STATUS;
	static u32 ack_miss_no = 0;

	if (device_ack_received)
	{
		check_rf_fast_setting_flag();
		ack_miss_no = 0;
		no_ack = 0;
		start_rf_tick = 0;

		if (device_status <= STATE_PAIRING)
		{
			if (pair_success_flag)
			{
				device_status = STATE_NORMAL;
				connect_ok = 1;

				if (flash_dev_info.dongle_id != dongle_id)
				{
					flash_dev_info.dongle_id = dongle_id;
#if (AES_METHOD == 1)
					memcpy((u8 *)&flash_dev_info.key[0], (u8 *)&private_key[4], 12);
#endif
					save_dev_info_flash();
				}
				clear_pair_flag();
				reset_idle_status();
				// clear_pair_flag();
			}
			my_fifo_reset(&fifo_km);
		}
		else if (device_status == STATE_NORMAL)
		{
			if (keyboard_send_need_f) // skip to next packet
				my_fifo_pop(&fifo_km);
			connect_ok = 1;
		}
#if D24G_OTA_ENABLE_AAA
		else if (device_status == STATE_OTA)
		{
			if (keyboard_send_need_f)
			{
				my_fifo_pop(&fifo_km);

				if (ota_buff_valid_flag)
				{
					u8 ota_error_flag = d24g_ota_write(&ota_buff);

					if (ota_error_flag)
					{
						printf("-err flag=%d.\n", ota_error_flag);

						d24g_ota_resut(ota_error_flag);
					}

					ota_buff_valid_flag = 0;

					idle_status_poll();
				}
			}
			connect_ok = 1;
		}
#endif

		keyboard_send_need_f = 0;
	}
	else
	{
		no_ack++;
		ack_miss_no++;

		if (ack_miss_no >= 2)
			device_channel = get_next_channel_with_mask(0, device_channel);

		if (no_ack > 125)
		{
			no_ack = 0;
			connect_ok = 0;
			if (pair_flag == 0)
				device_status = STATE_NONE;
			keyboard_send_need_f = 0;
		}
#if (FACTORY_ENABLE)
		if (factory_mode_flag)
		{
			if (p_km_data->cmd == FACTORY_TEST_CMD)
				WaitMs(10);
		}
#endif
	}
}

void clear_pair_flag(void)
{
	rf_24G_deep_tick = clock_time();
	app_rcu_24g_pair_timer = 0;
	app_led_blink_timer = 0;

	if (factory_mode_flag)
		gpio_write(GPIO_LED_2, LED_ON_LEVAL);
	else
	{
		gpio_write(GPIO_LED_1, LED_ON_LEVAL);
		app_led_on_timer = clock_time() | 1;
	}
	pair_flag = 0;
	analog_write(DEEP_ANA_REG1, 0);
}
void set_pair_flag()
{
	if (factory_mode_flag)
		led_2_driver_init();
	else
		led_1_driver_init();
	app_led_blink_time_ms = 250;
	app_led_blink_timer = clock_time() | 1;
	app_rcu_24g_pair_timer = clock_time() | 1;
	pair_flag = 1;
	analog_write(DEEP_ANA_REG1, 1);
}
void d24_start_pair(void)
{
	printf("--->>> [work_debug] d24_start_pair !!!\r\n");
	set_pair_flag();
	// start_reboot();
	device_status = STATE_PAIRING;
	adv_count = 0;
	adv_begin_tick = clock_time() | 1;
}

void idle_status_poll(void)
{
	u8 n;
	n = ((u32)(clock_time() - idle_tick)) / CLOCK_16M_SYS_TIMER_CLK_1S;
	idle_tick += n * CLOCK_16M_SYS_TIMER_CLK_1S;
	idle_count += n;
}

void adv_count_poll(void)
{
	u8 n;
	n = ((u32)(clock_time() - adv_begin_tick)) / CLOCK_16M_SYS_TIMER_CLK_1S;
	adv_begin_tick += n * CLOCK_16M_SYS_TIMER_CLK_1S;
	adv_count += n;
	// printf("adv_count == %d\r\n",adv_count);
}

/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
