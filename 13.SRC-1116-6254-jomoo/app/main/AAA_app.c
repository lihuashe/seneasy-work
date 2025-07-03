/********************************************************************************************************
 * @file     AAA_app.c
 *
 * @brief    This is the source file for KMD SDK
 *
 * @author	 KMD GROUP
 * @date         01,2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/

#include "AAA_project_config.h"
#include "AAA_app_config.h"
#include "AAA_24G_rf_frame.h"
#include "ir_driver.h"
#include "bath_rcu.h"
#include "utility.h"
#include "factory_test.h"
#include "system.h"
#include "driver.h"
#include "rf_drv.h"
#include "pwm_handle.h"
#include "function.h"
#include "ota_slaver.h"
#include "key_handle_config.h"
#include "../drivers/flash/flash_type.h"



#define SIMU_DATA_DEBUG_ENABLE 0

#define HOPING_FREQ 0

#define RF_FAST_TX_SETTING_ENABLE 1
#define RF_FAST_RX_SETTING_ENABLE 1

#if (RF_FAST_TX_SETTING_ENABLE || RF_FAST_RX_SETTING_ENABLE)

volatile u32 tick_rf_fast_setting = 0;
volatile int rf_fast_setting_flag = 0; // 0 init 1:time out  2:enable fast set
#endif

// t_mouse_inf mouse_inf,last_mouse_inf;

u8 usb_device_status;
u8 need_enter_suspend_flag = 0;

volatile int rx_fail = 0;
volatile u8 chn_mask = 0x80;
volatile u8 last_chn_mask = 0;

u8 deep_flag = POWER_ON_ANA_AAA;
static u8   fw_versoion[2] = {0x00, 0x05};
// u32 rx_tick=0;
// u32 rx_mouse_cnt=0;
// u32 mouse_to_pc_cnt=0;
// u8 has_write_cfg=0;
// u8 debug_report_rate=0;
// u32 tick_pc_breath=0;

volatile u32 tick_factory_version_ir_send = 0;
volatile u32 tick_factory_tx_data = 0;
u8 factory_test_mode_flag = POWER_ON_STATE;

#if (AES_METHOD == 0)
#define ID_LEN 4
#elif (AES_METHOD == 1)
#define ID_LEN 16
#endif
typedef void (*callback_rx_func)(u8 *);
u8 *raw_pkt = NULL;
callback_rx_func p_post = NULL;
u8 bin_crc[4];
u32 pair_accesscode = 0x39517695;
u32 pair_sum_count = 1;

USB_FIFO_DATA_S usb_fifo_aaa;
u8 mouse_not_release = 0;
volatile u32 dongle_id = 0;

volatile u32 kb_tick = 0;
volatile u32 ms_tick = 0;
s16 x_smoother;
s16 y_smoother;

// u8 allow_suspend = 0;
// u32 tick_allow_suspend = 0;
// u32 tick_host_start_power_on=0;
// u32 tick_host_start_power_off=0;
u32 tick_suspend_interval;
static mouse_data_t mouse_last_data = {0, 0, 0, 0};
// static u8 consume_last_data[2]={0,0};
// static u8 sys_last_data=0;
static u8 normal_last_data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

callback_rx_func p_keyboard_data_handle_post = NULL;
volatile u8 to_usb_keyboard_data[8];

callback_rx_func p_mouse_data_handle_post = NULL;
volatile u8 to_usb_mouse_data[8];

// u8 need_wake_up_host_flag=0;
// u32 tick_resume_host;
u8 allow_send_to_usb_flag = 1;
int custom_binding_idx = 0;

volatile bool Writes_flash_flag = false;
bool Device_flash_acq = true;
u32 blt_ota_start_tick;
static u32 key_ir_send_tick = 0;
u8 usb_start_flag = 0;


rf_packet_t ack_rf_dat;

//系统变量设置
_attribute_data_retention_ volatile System_status g_sys;


#if (HOPING_FREQ == 1)
volatile u32 tick_fh = 0;
frqHopping_data_t frq_hopping_data =
	{
		FRQ_HOPPING_MODE_NORMAL,
		8,
		2,
		0,
		120000,
		0};
volatile u8 ll_chn_sel;
volatile u8 ll_chn_pkt[16] = {0};
volatile int device_packet_received;
#endif
volatile unsigned char rf_rx_buff[PKT_BUFF_SIZE * 2] __attribute__((aligned(4)));
int rf_rx_wptr;
volatile u8 host_channel = 0;

AES_KEY key_enc[3];

u32 power_on_tick = 0;
volatile u8 rf_pair_enable = 1;
u8 pair_success_flag;
custom_cfg_t user_config;
u8 small_rcu_flag = 0;

u32 ota_program_offset;
// 设备状态
volatile u32 Device_fixed_id;
volatile u8 PAIR_STATUS = 0;

// 配对参数
u8 DEVICE_PAIR_STATUS = 0x00; // 0x02成功 0x03

//设备RF配置区
_attribute_data_retention_ bool DEVICE_PAIRED_STATUS = 0x00;

Rf_type rf_type = {{19,18}};
_attribute_data_retention_ Channel_list channel_pipo = {0,{0,4,10,14}};


//-------------------- rf init------------------

#define FRE_OFFSET 0
#define FRE_STEP 5
#define MAX_RF_CHANNEL 16
const unsigned char rf_chn[MAX_RF_CHANNEL] =
	{
		FRE_OFFSET + 5,
		FRE_OFFSET + 9,
		FRE_OFFSET + 13,
		FRE_OFFSET + 17,
		FRE_OFFSET + 22,
		FRE_OFFSET + 26,
		FRE_OFFSET + 30,
		FRE_OFFSET + 35,
		FRE_OFFSET + 40,
		FRE_OFFSET + 45,
		FRE_OFFSET + 50,
		FRE_OFFSET + 55,
		FRE_OFFSET + 60,
		FRE_OFFSET + 65,
		FRE_OFFSET + 70,
		FRE_OFFSET + 76,
};



unsigned short crc16_user(unsigned char *pD, int len)
{
	unsigned short crc16_poly_user[2] = {0, 0xa001};
	unsigned short crc = 0xffff;
	int i, j;

	for (j = len; j > 0; j--)
	{
		unsigned char ds = *pD++;
		for (i = 0; i < 8; i++)
		{
			crc = (crc >> 1) ^ crc16_poly_user[(crc ^ ds) & 1];
			ds = ds >> 1;
		}
	}

	return crc;
}


u8 update_channel_mask(u8 mask, u8 chn, u8 *chn_pkt)
{
#if (HOPING_FREQ == 1)
	static int ll_chn_sel_chg, ll_chn_hold;
	if (clock_time_exceed(tick_fh, frq_hopping_data.fre_hp_always_time_us))
	{
		chn_pkt[ll_chn_sel_chg] = 0;
		chn_pkt[!ll_chn_sel_chg] = frq_hopping_data.frq_hp_chn_pkt_rcvd_max;
	}
	if (ll_chn_hold)
	{
		ll_chn_hold--;
		chn_pkt[0] = chn_pkt[1] = 0;
	}
	int diff = chn_pkt[ll_chn_sel] - chn_pkt[!ll_chn_sel];
	int hit_th = diff > frq_hopping_data.frq_hp_hit_diff_num;
	if (chn_pkt[ll_chn_sel] >= frq_hopping_data.frq_hp_chn_pkt_rcvd_max || hit_th)
	{
		int dual_chn[2];
		dual_chn[0] = mask & 0x0f;
		dual_chn[1] = mask >> 4;
		if (hit_th)
		{					  // change channel
			ll_chn_hold = 32; // 32ms
			chn = dual_chn[!ll_chn_sel];
			for (int i = 0; i < 8; i++)
			{
				chn = LL_NEXT_CHANNEL(chn);
				if ((ll_chn_sel && chn != dual_chn[1]))
				{
					mask = (mask & 0xf0) | chn;
					break;
				}
				else if (!ll_chn_sel && chn != dual_chn[0])
				{
					mask = (mask & 0x0f) | (chn << 4);
					break;
				}
			}
			tick_fh = clock_time();
			ll_chn_sel_chg = !ll_chn_sel; // remember latest channel change
		}
		chn_pkt[0] = chn_pkt[1] = 0;
	}
#endif
	return mask;
}

#define FRE_OFFSET 0
// const unsigned char rf_chn[16] =
// {
//     FRE_OFFSET + 5, FRE_OFFSET + 9, FRE_OFFSET + 13, FRE_OFFSET + 17,
//     FRE_OFFSET + 22, FRE_OFFSET + 26, FRE_OFFSET + 30, FRE_OFFSET + 35,
//     FRE_OFFSET + 40, FRE_OFFSET + 45, FRE_OFFSET + 50, FRE_OFFSET + 55,
//     FRE_OFFSET + 60, FRE_OFFSET + 65, FRE_OFFSET + 70, FRE_OFFSET + 76,
// };

_attribute_ram_code_sec_ void rf_set_channel_aaa(signed char chn, unsigned short set)
{
	rf_set_channel(rf_chn[chn], 0);
	// rf_set_channel (chn, set);
}

void user_rf_ack_send(int rf_length)
{
	ack_rf_dat.dma_len = rf_length + 1;
	ack_rf_dat.rf_len = rf_length;
	PIN_DEBUG_RF_TX_LEVEL(1);

	rf_set_tx_pipe(0);
	rf_set_tx_rx_off();
	rf_start_stx((void *)&ack_rf_dat.dma_len, clock_time());
	if (small_rcu_flag == 1)
	{
		WaitMs(1);
	}
	else
	{
		WaitMs(10);
	}

	// printf("\r\n");
	// printf("user_rf_ack_send ack_rf_dat.rf_len: %d\r\n",ack_rf_dat.rf_len);
	// printf("[ ");
	// for(int i = 0; i < ack_rf_dat.rf_len; i++)
	// 	printf("%1x ",ack_rf_dat.dat[i]);
	// printf("]\r\n");
}

// rf_set_channel(59,0);

void clear_pair_enable_flag()
{
	printf("---clear_pair_enable_flag\n");
	rf_pair_enable = 0;
	pair_success_flag = 0;
	// if(user_config.paring_type!=GOLDEN_DONGLE_PARING_TYPE)
	{
		// rf_rx_acc_code_enable(0x06);//mouse&keboard
	}
}

_attribute_ram_code_sec_ void check_first_normal_data()
{

#if (GOLD_TEST_DONGLE == 0)
	static u32 first_rx_normal_data_tick = 0;
	if (first_rx_normal_data_tick == 0)
	{
		first_rx_normal_data_tick = clock_time() | 1;
	}
	else if (rf_pair_enable && clock_time_exceed(first_rx_normal_data_tick, 1000000))
	{
		clear_pair_enable_flag();
	}
#endif
}

u8 push_usb_fifo_aaa(u8 type, u8 *buf, u8 len)
{
	USB_DATA_S *p = (USB_DATA_S *)usb_fifo_aaa.fifo[usb_fifo_aaa.wptr & (USB_FIFO_NUM - 1)];
	p->type = type;
	memcpy(p->buf, buf, len);
	usb_fifo_aaa.wptr++;
	int fifo_use = (usb_fifo_aaa.wptr - usb_fifo_aaa.rptr) & (USB_FIFO_NUM * 2 - 1);
	if (fifo_use > USB_FIFO_NUM)
	{
		usb_fifo_aaa.rptr++; // overlap older data
	}
	return 1;
}

static inline u32 rf_access_code_24to32(u32 code)
{
	u32 r = (code & 0x00ffff00) >> 8;
	u32 p = code & 0xff;
	u32 t = 0;
	for (int i = 0; i < 8; i++)
	{
		t = t << 2;
		t |= ((p & BIT(i)) ? 1 : 2);
		t = t << 2;
		t |= (r & 0x03);
		r = r >> 2;
	}
	return t;
}

void set_pair_access_code(u32 code)
{
	write_reg32(0x800408, ((code & 0xffffff00) | 0x71));
	write_reg8(0x80040c, code);
}

void set_data_access_code(u32 code)
{
	write_reg32(0x800408, ((code & 0xffffff00) | 0x77));
	write_reg8(0x80040c, code);
}

static inline u32 rf_get_access_code1(void)
{
	return read_reg8(0x800414) | (read_reg32(0x800410) & 0xffffff00);
}

#if (RF_FAST_TX_SETTING_ENABLE || RF_FAST_RX_SETTING_ENABLE)

#if RF_FAST_RX_SETTING_ENABLE
_attribute_ram_code_sec_ void rf_rx_fast_settle_init()
{
	// RX_SETTLE_TIME_44US)
	write_reg8(0x1284, (read_reg8(0x1284) & 0x87) | (0x6 << 3));

	write_reg8(0x12b0, 0x00);
	write_reg8(0x12b1, read_reg8(0x12b1) & 0xfe);
	write_reg8(0x12b2, 0x10);
	write_reg8(0x12b3, read_reg8(0x12b3) & 0xfe);
	write_reg8(0x12b4, 0x10);
	write_reg8(0x12b5, read_reg8(0x12b5) & 0xfe);
	write_reg8(0x12b6, 0x32);
	write_reg8(0x12b7, read_reg8(0x12b7) & 0xfe);
	write_reg8(0x12b8, 0x58);
	write_reg8(0x12b9, read_reg8(0x12b9) & 0xfe);
	write_reg8(0x12ba, 0x58);
	write_reg8(0x12bb, read_reg8(0x12bb) & 0xfe);
}

_attribute_ram_code_sec_ void rf_rx_fast_settle_en(void)
{
	write_reg8(0x1229, read_reg8(0x1229) | 0x01);
}

_attribute_ram_code_sec_ void rf_rx_fast_settle_dis(void)
{
	write_reg8(0x1229, read_reg8(0x1229) & 0xfe);
}

#endif
#if RF_FAST_TX_SETTING_ENABLE
_attribute_ram_code_sec_ void rf_tx_fast_settle_init()
{
	// TX_SETTLE_TIME_53US)
	write_reg8(0x1284, (read_reg8(0x1284) & 0xf8) | 0x02);

	write_reg8(0x12a4, 0x00);
	write_reg8(0x12a5, read_reg8(0x12a5) & 0xfe);
	write_reg8(0x12a6, 0x10);
	write_reg8(0x12a7, read_reg8(0x12a7) & 0xfe);
	write_reg8(0x12a8, 0x62);
	write_reg8(0x12a9, read_reg8(0x12a9) & 0xfe);
	write_reg8(0x12aa, 0x64);
	write_reg8(0x12ab, read_reg8(0x12ab) & 0xfe);
	write_reg8(0x12ac, 0x6a);
	write_reg8(0x12ad, read_reg8(0x12ad) & 0xfe);
	write_reg8(0x12ae, 0x6a);
	write_reg8(0x12af, read_reg8(0x12af) & 0xfe);
	write_reg8(0x12bc, 0x62);
	write_reg8(0x12bd, read_reg8(0x12bd) & 0xfe);
}

_attribute_ram_code_sec_ void rf_tx_fast_settle_en(void)
{
	write_reg8(0x1229, read_reg8(0x1229) | 0x02);
}

_attribute_ram_code_sec_ void rf_tx_fast_settle_dis(void)
{
	write_reg8(0x1229, read_reg8(0x1229) & 0xfd);
}
#endif

void my_rf_start_stx(void *addr, unsigned int tick)
{
	write_reg32(0x800f18, tick);
	write_reg8(0x800f16, read_reg8(0x800f16) | 0x04); // Enable cmd_schedule mode
	write_reg8(0x800f00, 0x85);						  // single TX
	write_reg16(0x800c0c, (unsigned short)((unsigned int)addr));
}

_attribute_ram_code_sec_ void check_rf_fast_setting_time()
{
#if (RF_FAST_TX_SETTING_ENABLE || RF_FAST_RX_SETTING_ENABLE)
	if (clock_time_exceed(tick_rf_fast_setting, 2000000))
	{
		tick_rf_fast_setting = clock_time();
		rf_fast_setting_flag = 1;
	}
#endif
}

_attribute_ram_code_sec_ void check_rf_fast_setting_flag()
{
#if (RF_FAST_TX_SETTING_ENABLE || RF_FAST_RX_SETTING_ENABLE)
	if (rf_fast_setting_flag == 1) // timeout
	{
		rf_fast_setting_flag = 0;
	}
	else
	{
		rf_fast_setting_flag = 2;
	}
#endif
}

_attribute_ram_code_sec_ void rf_set_tx_rx_setting()
{
#if (RF_FAST_TX_SETTING_ENABLE || RF_FAST_RX_SETTING_ENABLE)
	if (rf_fast_setting_flag < 2)
	{
#if RF_FAST_TX_SETTING_ENABLE
		write_reg8(0x0f04, 0x70); // tx settle time: Default 150us, minimum 113us(0x70+1)
		rf_tx_fast_settle_dis();
#endif

#if RF_FAST_RX_SETTING_ENABLE
		write_reg8(0x0f0c, 0x54); // rx settle time: Default 150us, minimum 85us(0x54+1)
		rf_rx_fast_settle_dis();
#endif
	}
	else // if(rf_ca_cnt==1)
	{
#if RF_FAST_TX_SETTING_ENABLE
		write_reg8(0x0f04, 65); // tx settle time: Default 150us, minimum 113us(0x70+1)
		rf_tx_fast_settle_en();
#endif

#if RF_FAST_RX_SETTING_ENABLE
		write_reg8(0x0f0c, 48); // rx settle time: Default 150us, minimum 85us(0x54+1)
		rf_rx_fast_settle_en();
#endif
	}
#endif
}
#endif

void rf_user_init()
{
	rf_mode_init();
	rf_set_pri_2M_mode();

	rf_rx_buffer_set((u8 *)rf_rx_buff, PKT_BUFF_SIZE, 0);

	write_reg8(0x405, (read_reg8(0x405) & 0xf8) | 5); // access_byte_num[2:0]
	write_reg8(0x420, 35);

	rf_set_power_level_index(RF_POWER_P8p99dBm);

	// write_reg32(0x800408,0x39517671);  //for 8366
	// write_reg8(0x80040c,0x95);
	set_pair_access_code(pair_accesscode);

	// write_reg32(0x800410,0xd3f03577);  //for 8366
	// write_reg8(0x800414,0xe7);

	// write_reg32(0x800418,0xc0300c03); //for 8366

	write_reg16(0xf06, 0);	  // rx wait time
	write_reg8(0x0f0c, 0x54); // rx settle time: Default 150us, minimum 85us(0x54+1)

	write_reg16(0xf0e, 0);	  // tx wait time
	write_reg8(0x0f04, 0x70); // tx settle time: Default 150us, minimum 113us(0x70+1)

	write_reg8(0xf10, 0); // wait time on NAK

	write_reg8(0x402, 0x42);

#if (RF_FAST_TX_SETTING_ENABLE || RF_FAST_RX_SETTING_ENABLE)
	rf_fast_setting_flag = 0;
	tick_rf_fast_setting = clock_time() | 1;
#if RF_FAST_RX_SETTING_ENABLE // must fist  call rx fast settle init
	rf_rx_fast_settle_init();
	rf_rx_fast_settle_dis();
#endif

#if RF_FAST_TX_SETTING_ENABLE
	rf_tx_fast_settle_init();
	rf_tx_fast_settle_dis();
#endif
#endif

	rf_set_tx_rx_off();
	rf_rx_acc_code_enable(0X01); //
	rf_tx_acc_code_select(0);

	irq_disable();
	rf_irq_clr_src(0xffff);
	irq_enable_type(FLD_IRQ_ZB_RT_EN);
	rf_irq_disable(FLD_RF_IRQ_ALL);
	rf_irq_enable(FLD_RF_IRQ_TX | FLD_RF_IRQ_RX);
}

_attribute_ram_code_sec_ void rf_tx_irq_handle()
{
	rf_set_tx_rx_off();
	rf_set_txmode(); // add for save

#if (RF_FAST_TX_SETTING_ENABLE || RF_FAST_RX_SETTING_ENABLE)
	check_rf_fast_setting_flag();
	rf_set_tx_rx_setting();
#endif
	if (factory_channel_begin == 2)
	{
		rf_set_channel_aaa(factory_channel, RF_CHN_TABLE);
		rf_set_rxmode();
	}
	else if (factory_channel_begin != 2)
	{
		rf_set_channel_aaa(host_channel, RF_CHN_TABLE);
		rf_set_rxmode();
	}
}

u8 check_from_ram(pair_data_t *p)
{
	static u16 buf[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	if (memcmp(buf, &p->did, ID_LEN))
	{
		memcpy(buf, &p->did, ID_LEN);

		return 0;
	}

	return 1;
}

int set_device_id_in_firmware(u8 *p)
{
	rf_packet_t *p_rf_data_pkt = (rf_packet_t *)p;
	pair_data_t *p_pair_data = (pair_data_t *)&p_rf_data_pkt->dat[0];
	u8 device_index = 0; // 0 mouse 1 keyboard
	u8 dec[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#if (AES_METHOD == 0)
	memcpy(&dec[0], &p_pair_data->did, ID_LEN);
#elif (AES_METHOD == 1)
	if (p_pair_data->cmd & BIT(7)) // AES ENABLE
	{
		memcpy(encKey, pub_key, 16);
		aes_decrypt(encKey, (u8 *)&p_pair_data->did, &dec[0]);
		memcpy((u8 *)&p_pair_data->did, dec, ID_LEN);
	}
#endif
	switch (p_pair_data->rsv1[0])
	{
	case 0x00:
		device_index = 0;
		custom_binding_idx = 0;
		break;
	case 0x01:
		device_index = 1;
		custom_binding_idx = 1;
		break;
	case 0x02:
		device_index = 2;
		custom_binding_idx = 2;
		break;
	default:
		break;
	}
	// printf("custom_binding_idx = %d,device_index = %d\n\r",custom_binding_idx,device_index);
	// printf("p_pair_data->did = %4x\n\r",p_pair_data->did);
	if (memcmp((u8 *)&key_enc[device_index].did, dec, ID_LEN) == 0)
	{
		pair_success_flag = 1;
		power_on_tick = clock_time() | 1;
		user_config.paring_limit_t = 2;
		return 1;
	}
	else if (custom_binding_idx < PARING_MAX_NUM)
	{
		key_enc[device_index].did = p_pair_data->did;
		memcpy((u8 *)&key_enc[device_index].did, dec, ID_LEN);
		u32 adr = 0;
		adr = (ID_SAVE_STORAGE_ADDR + ID_LEN * custom_binding_idx);

#if HW_IS_FLASH
		// u8 tmp_buf[16];
		u32 did = 0xffffffff;
		flash_write_page(adr, ID_LEN, (u8 *)&did);
		printf("---did  write = 0x%04x.\n", key_enc[device_index].did);
		flash_write_page(adr, ID_LEN, (u8 *)&key_enc[device_index].did);
		// flash_read_page(adr, ID_LEN, tmp_buf);	//Read the stored content back out
		flash_read_page(adr, ID_LEN, (u8 *)&did);
		printf("---did read = 0x%04x.\n", did);
		// custom_binding_idx++;
#else
		rf_set_tx_rx_off();
		unsigned char r = irq_disable();
		u32 temp;
		for (u8 i = 0; i < 4; i++)
		{
			otp_set_active_mode();
			otp_write(adr, 1, (u32 *)&dec[0]);
			otp_read(adr, 1, (u32 *)&temp);
			if (memcmp(&dec[0], &temp, 4) == 0)
			{
				custom_binding_idx++;
				break;
			}
			else
			{
				custom_binding_idx++;
				if (custom_binding_idx >= PARING_MAX_NUM)
				{
					break;
				}
				adr = (ID_SAVE_STORAGE_ADDR + ID_LEN * custom_binding_idx);
			}
		}
		irq_restore(r);
		rf_set_channel_aaa(host_channel, RF_CHN_TABLE);
		rf_set_rxmode();
#endif
		return 1;
	}

	return 0;
}

_attribute_ram_code_sec_ u8 rf_pair_response(rf_packet_t *p_rf_rx_data)
{
	static u8 rx_id_same_cnt = 0;

	pair_data_t *p_pair_data = (pair_data_t *)&p_rf_rx_data->dat[0];
	pair_ack_data_t *p_pair_ack_data = (pair_ack_data_t *)&ack_rf_dat.dat[0];
	// p_pair_ack_data->chn=chn_mask;
#if PAIR_CHECK_RSSI_ENABLE_AAA
	static u32 rx_rssi = 0;
	static u8 rssi_last;
	u8 *p_rf_rx_data_start = (u8 *)&p_rf_rx_data->dma_len;
	u8 rssi_now = GET_RX_RSSI(p_rf_rx_data_start);

	if (!rx_rssi)
	{
		rx_rssi = rssi_now;
	}

	if (abs_aaa(rssi_last - rssi_now) < 17)
	{
		rx_rssi = ((rx_rssi << 1) + rssi_now) / 3;
	}
	rssi_last = rssi_now;
	// printf("rx_rssi = %d\n\r",rx_rssi);
	if (rx_rssi > (16 - user_config.rssi_threshold))
#endif
	{
		if (check_from_ram(p_pair_data)) // check ram
		{
			rx_id_same_cnt++;

			if (rx_id_same_cnt >= 2)
			{
				p_pair_ack_data->head0 = HEAD_0;
				p_pair_ack_data->head1 = HEAD_1;
				p_pair_ack_data->head2 = HEAD_2;
				p_pair_ack_data->cmd = PAIR_ACK_CMD;
				if (p_pair_data->rsv1[0] == 0x02)
				{
					small_rcu_flag = 1;
				}
				else
				{
					small_rcu_flag = 0;
				}
				memcpy(&p_pair_ack_data->did, &p_pair_data->did, ID_LEN);
				if (pair_success_flag)
				{
					host_channel = 0;
					rf_set_channel_aaa(host_channel, RF_CHN_TABLE);
					rf_set_rxmode();
					for (u8 i = 0; i < 12; i++)
					{
						user_rf_ack_send(18);
					}
					host_channel = 1;
					rf_set_channel_aaa(host_channel, RF_CHN_TABLE);
					rf_set_rxmode();
					for (u8 i = 0; i < 12; i++)
					{
						user_rf_ack_send(18);
					}
					host_channel = 10;
					rf_set_channel_aaa(host_channel, RF_CHN_TABLE);
					rf_set_rxmode();
					for (u8 i = 0; i < 12; i++)
					{
						user_rf_ack_send(18);
					}
					host_channel = 14;
					rf_set_channel_aaa(host_channel, RF_CHN_TABLE);
					rf_set_rxmode();
					for (u8 i = 0; i < 12; i++)
					{
						user_rf_ack_send(18);
					}
				}

				p_post = (void *)set_device_id_in_firmware;

				return 1;
			}
		}
		else
		{
			rx_id_same_cnt = 0;
			pair_success_flag = 0;
		}
	}

	return 0;
}

u8 get_next_channel_with_mask(u32 mask, u8 chn)
{
	PIN_DEBUG_RF_CHN_NEXT_TOGGLE;
#if (HOPING_FREQ == 0)
	// printf("next_channel=%d.\n",(chn+3)%15);
	return (chn + 3) % 15;
#else
	int chn_high = (mask >> 4) & 0x0f;
	if (mask & LL_CHANNEL_SEARCH_FLAG)
	{
		return LL_NEXT_CHANNEL(chn);
	}
	else if (chn_high != chn)
	{
		ll_chn_sel = 1;
		return chn_high;
	}
	else
	{
		ll_chn_sel = 0;
		return mask & 0x0f;
	}
#endif
}
void soft_tick_poll()
{
	static u32 tick;
#if (RF_FAST_TX_SETTING_ENABLE || RF_FAST_RX_SETTING_ENABLE)
	check_rf_fast_setting_time();
#endif

	if (clock_time_exceed(tick, 1000))
	{
		tick = clock_time();
		// 选择产品信道
		if (factory_channel_mode == 0)
		{
			host_channel = channel_pipo.list[channel_pipo.choose++];

			rf_set_tx_rx_off();
			rf_set_tx_rx_setting();
			rf_set_channel_aaa(host_channel, RF_CHN_TABLE);
			rf_set_rxmode();
		}
#if RF_24G_FACTORY_ENABLE
		else if (factory_channel_mode == 1 && ota_enable_begin != 2)
		{
			if (factory_channel_begin != 2)
			{
				if (host_channel == 15)
					host_channel = 3;
				else if (host_channel == 3)
					host_channel = 7;
				else if (host_channel == 7)
					host_channel = 11;
				else
					host_channel = 15;
				rf_set_tx_rx_off();
				rf_set_tx_rx_setting();
				rf_set_channel_aaa(host_channel, RF_CHN_TABLE);
				rf_set_rxmode();
			}
		}
#endif
	}
#if (HOPING_FREQ == 1)
	chn_mask = update_channel_mask(chn_mask, host_channel, (u8 *)&ll_chn_pkt[0]);
#endif
}

_attribute_ram_code_sec_ void irq_host_timer1(void)
{
}

void call_mouse_data_handle(u8 *buf)
{
	ms_tick = clock_time();
	mouse_data_t *ms_dat = (mouse_data_t *)&buf[0];

	check_first_normal_data();

	if (ms_dat->x || ms_dat->y)
	{
		x_smoother = ms_dat->x;
		y_smoother = ms_dat->y;
		adaptive_smoother();
		ms_dat->x = x_smoother;
		ms_dat->y = y_smoother;
	}
	if (memcmp(&mouse_last_data.btn, &ms_dat->btn, sizeof(mouse_data_t)) || (ms_dat->x != 0) || (ms_dat->y != 0) || (ms_dat->wheel != 0))
	{
		memcpy(&mouse_last_data.btn, &ms_dat->btn, sizeof(mouse_data_t));
		push_usb_fifo_aaa(MOUSE_DATA_TYPE, &ms_dat->btn, sizeof(mouse_data_t));
	}
}

void call_keyboard_data_handle(u8 *buf)
{
	check_first_normal_data();
	kb_data_handle(buf);
}

#if (AES_METHOD == 1)
volatile u8 aes_dec[59];
#endif
_attribute_ram_code_sec_ u8 rf_km_data_repsonse(rf_packet_t *p_rf_rx_data, u8 hw_crc_ok)
{
	u8 ret = 0;
	// static u8 slave_seq_no[2]={0xff,0xff};
	u8 *p_km_data_temp = (u8 *)&p_rf_rx_data->dat[0];
	km_ack_data_t *p_km_ack_data = (km_ack_data_t *)&ack_rf_dat.dat[0];
	p_km_ack_data->chn = chn_mask;

	if (hw_crc_ok)
	{
		ret = 1;
	}
#if DATA_3_CHOOSE_1_ENABLE
	else
	{
		u16 crc_c = 0;
		u16 crc_s[3] = {0, 0, 0};
		u8 *p_src1;
		u8 *p_src2;
		u8 *p_src3;
		u8 crc_position = sizeof(km_3_c_1_data_t) - 2;
		p_src1 = &p_km_data_temp[0];
		p_src2 = p_src1 + sizeof(km_3_c_1_data_t);
		p_src3 = p_src2 + sizeof(km_3_c_1_data_t);
		crc_s[0] = p_src1[crc_position] + (p_src1[crc_position + 1] << 8);
		crc_s[1] = p_src2[crc_position] + (p_src2[crc_position + 1] << 8);
		crc_s[2] = p_src3[crc_position] + (p_src3[crc_position + 1] << 8);
		for (u8 i = 0; i < 3; i++)
		{
			crc_c = crc16_user(p_km_data_temp, sizeof(km_3_c_1_data_t) - 2);
			if ((crc_c == crc_s[0]) || (crc_c == crc_s[1]) || (crc_c == crc_s[2]))
			{
				ret = 1;
				break;
			}
			else
			{
				p_km_data_temp += sizeof(km_3_c_1_data_t);
			}
		}
	}
#endif

	if (ret == 1)
	{
		km_3_c_1_data_t *km_ptr = (km_3_c_1_data_t *)&p_km_data_temp[0];
		// p_km_ack_data->seq_no=km_ptr->seq_no;
		p_km_ack_data->host_led_status = (host_keyboard_status & 0x1f) | ((usb_device_status & 0x7) << 5);

#if CHECK_DEVICE_ID_ENABLE
		// printf("km_ptr->did = %4x\n\r",km_ptr->did);
		// printf("key_enc[0].did = %4x\n\r",key_enc[0].did);
		// printf("key_enc[1].did = %4x\n\r",key_enc[1].did);
		// printf("key_enc[2].did = %4x\n\r",key_enc[2].did);
		if (km_ptr->did != 0x00)
		{
			if (key_enc[0].did != km_ptr->did && key_enc[1].did != km_ptr->did && key_enc[2].did != km_ptr->did)
			{
				return 0;
			}
		}
#endif
		if (km_ptr->rsv1[0] == 0x00 || km_ptr->rsv1[0] == 0x01 || km_ptr->rsv1[0] == 0x02)
		{
			if (p_keyboard_data_handle_post == NULL)
			{
				// p_keyboard_data_handle_post=(void *)call_keyboard_data_handle;
				// printf("--->>> [work_debug] Recv_Data [ ");
				// for(int i = 0; i < 6; i++)
				// 		printf("%x ",km_ptr->km_dat[i]);
				// printf("]\r\n");
				if (key_enc[0].did == km_ptr->did || key_enc[1].did == km_ptr->did || key_enc[2].did == km_ptr->did || km_ptr->did == 0)
				{ // do not equal
					memcpy((u8 *)to_usb_keyboard_data, &km_ptr->km_dat[0], 8);
					kb_data_handle((u8 *)to_usb_keyboard_data);
				}
			}

			return 1;
		}
	}

	return 0;
}

#if 0

_attribute_ram_code_sec_ void rf_rx_irq_handle2()
{
	u8 need_ack=0;
	u8 hw_crc_ok=0;

	raw_pkt = (u8 *) (rf_rx_buff + rf_rx_wptr * PKT_BUFF_SIZE);
	rf_rx_wptr = (rf_rx_wptr + 1) & 1;
	rf_rx_buffer_set((u8*)(rf_rx_buff + rf_rx_wptr * PKT_BUFF_SIZE),PKT_BUFF_SIZE,0);
	rf_packet_t *p_rf_rx_data=(rf_packet_t*)raw_pkt;

    if(RF_TPLL_PACKET_CRC_OK(raw_pkt)&&RF_TPLL_PACKET_LENGTH_OK(raw_pkt))
    {
		PIN_DEBUG_RF_RX_CRC_OK_TOGGLE;
		hw_crc_ok=1;
    }

    if(rf_pair_enable)
    {
		if(hw_crc_ok)
		{
			pair_data_t *pair_data_ptr=(pair_data_t*)&p_rf_rx_data->dat[0];

		#if (AES_METHOD == 1)
			if (pair_data_ptr->cmd == (PAIR_CMD|BIT(7))) 
		#else
			if (pair_data_ptr->cmd == PAIR_CMD) 
		#endif
			{
				need_ack = rf_pair_response(p_rf_rx_data);
            }
			else 
			{
				need_ack = rf_km_data_repsonse(p_rf_rx_data, hw_crc_ok);
			}
		}
    } 
	else 
	{
		need_ack = rf_km_data_repsonse(p_rf_rx_data, hw_crc_ok);
	}


	if(need_ack)
	{
		PIN_DEBUG_RF_RX_DATA_OK_TOGGLE;
	#if (HOPING_FREQ==1)
		device_packet_received=1;
		ll_chn_pkt[ll_chn_sel]++;
		tick_fh = clock_time ();
		host_channel = get_next_channel_with_mask (chn_mask, host_channel);
	#endif
		rx_fail=0;
	}
    raw_pkt[0] = 1;
    //device_packet_received++;
}

#endif

_attribute_ram_code_sec_ void rf_rx_irq_handle()
{
	
	raw_pkt = (u8 *)(rf_rx_buff + rf_rx_wptr * PKT_BUFF_SIZE);
	rf_rx_wptr = (rf_rx_wptr + 1) & 1;
	rf_rx_buffer_set((u8 *)(rf_rx_buff + rf_rx_wptr * PKT_BUFF_SIZE), PKT_BUFF_SIZE, 0);
	rf_packet_t *p_rf_rx_data = (rf_packet_t *)raw_pkt;

	OTA_UPDATE_IRP_HANDLE(raw_pkt, p_rf_rx_data);
#if RF_24G_FACTORY_ENABLE

	if (production_testing == 1)
	{
#if RF_EMI_RSSI_ENABLE
		unsigned int emi_rssibuf = 0;
		emi_rssibuf = (read_reg8(0x449));
		if (emi_rssibuf < 45)
		{
			// printf("RX: emi_rssibuf low== %d\r\n", emi_rssibuf);
			return;
		}
#endif
		if (RF_TPLL_PACKET_CRC_OK(raw_pkt) && RF_TPLL_PACKET_LENGTH_OK(raw_pkt))
		{
			PIN_DEBUG_RF_RX_CRC_OK_TOGGLE;
			rf_rx_process_factory(p_rf_rx_data);
		}
	}
#endif

	if (g_sys.pair_status == PAIRING)
	{
		if (RF_TPLL_PACKET_CRC_OK(raw_pkt) && RF_TPLL_PACKET_LENGTH_OK(raw_pkt))
		{
			rf_set_tx_rx_off();
			rf_set_txmode();
			PIN_DEBUG_RF_RX_CRC_OK_TOGGLE;
			pair_data_t *pair_data_ptr = (pair_data_t *)&p_rf_rx_data->dat[0];
			//配对确认包里面的信息
			if (pair_data_ptr->cmd == PAIR_ACK_CMD && pair_data_ptr->head_0 == 0x00 && pair_data_ptr->head_1 == 0x53 && pair_data_ptr->head_2 == 0x54 && pair_data_ptr->did == Device_fixed_id)
			{
				Jomoo_pair_success_event();
				rf_set_tx_rx_off();
			}
		}
	}


}

static unsigned short tsync_crc16(unsigned short crc, unsigned char *pD, int len)
{
#define poly 0x8408
	int i, j;
	for (j = len; j > 0; j--)
	{
		unsigned int ds = *pD++;
		for (i = 0; i < 8; i++)
		{
			int bit = (crc ^ ds) & 1;
			crc = crc >> 1;
			if (bit)
			{
				crc = crc ^ poly;
			}
			ds = ds >> 1;
		}
	}
	return crc;
}

static inline unsigned int tsync_access_code_16to32(unsigned short code)
{
	unsigned int r = 0;
	for (int i = 0; i < 16; i += 2)
	{
		r = r << 4;
		int c = (code >> i) & 3;
		r |= (c << 2) | ((~c) & 3);
	}
	return r;
}

void custom_init()
{
#if HW_IS_FLASH
	flash_read_page(CFG_MAC_ADDR, sizeof(custom_cfg_t), (u8 *)&user_config.mac_addr);
	if (user_config.mac_addr == U32_MAX)
	{
		generateRandomNum(4, (u8 *)&user_config.mac_addr);
		app_rcu_flash_unlock_init();
		flash_write_page(CFG_MAC_ADDR, 4, (u8 *)&user_config.mac_addr);
		app_rcu_flash_lock_init();
		user_config.access_code_type = 0;
	}
#else
	otp_read(CFG_MAC_ADDR, sizeof(custom_cfg_t) / 4, (u32 *)&user_config.mac_addr);
#endif
	printf("---user_cfg.dev_mac=0x%04x.\r\n", user_config.mac_addr);

	if (user_config.access_code_type != U8_MAX)
	{
		dongle_id = user_config.mac_addr;
	}
	else
	{
		int c = tsync_crc16(0xffff, (u8 *)&user_config.mac_addr, 4);
		dongle_id = tsync_access_code_16to32(c);
	}
	printf("---dongle_id=0x%04x.\n", dongle_id);

	if (user_config.rf_id != 0xffff)
	{
		pair_accesscode = rf_access_code_16to32(user_config.rf_id);
	}

	if (user_config.rssi_threshold == U8_MAX)
	{
		user_config.rssi_threshold = 0;
	}

	if ((user_config.paring_limit_t == 0) || (user_config.paring_limit_t == U8_MAX))
	{
		user_config.paring_limit_t = 10;
	}

#if EMI_TEST_FUN_ENABLE_AAA
	if ((user_config.tx_power_emi == 0) || (user_config.tx_power_emi == U8_MAX))
	{
		user_config.tx_power_emi = DEFAULT_EMI_TX_POWER;
	}
#endif

#if (AES_METHOD == 1)
	for (u8 i = 0; i < 16; i++)
	{
		if (user_config.encKey[i] != 0xff)
		{
			memcpy(pub_key, &user_config.encKey[0], 16);

			break;
		}
	}
#endif

#if USB_DESCRIPTOR_MY_SELF
	custom_set_usb_cfg_params(&user_config);
#endif

	// AES_KEY tmp_key;
	// u8 index = 0;

	for (u8 i = 0; i < 3; i++)
	{
		flash_read_page(ID_SAVE_STORAGE_ADDR + ID_LEN * i, ID_LEN, (u8 *)&key_enc[i].did);
	}

	// for (u8 i = 0; i < PARING_MAX_NUM; i++)
	// {
	// #if HW_IS_FLASH
	//     flash_read_page(ID_SAVE_STORAGE_ADDR + ID_LEN*i, ID_LEN, (u8*)&tmp_key.did);
	// #else
	//     otp_read(ID_SAVE_STORAGE_ADDR+ID_LEN*i, ID_LEN/4, (u32*)&tmp_key.did);
	// #endif
	//     if (tmp_key.did == U32_MAX)
	//     {
	// 		break;
	//     }
	// 	else
	// 	{
	// 		// index = (tmp_key.did  & 0x03) - 1;
	// 		memcpy(&key_enc[i].did, &tmp_key.did, ID_LEN);
	// 		custom_binding_idx++;
	// 	}
	// }

#if HW_IS_FLASH // When the offset exceeds 3000, Store from the start
				// if((ID_LEN*custom_binding_idx)>3000)
				// {
				// 	custom_binding_idx=0;

	// 	flash_erase_sector(ID_SAVE_STORAGE_ADDR);

	// 	if((key_enc[0].did!=U32_MAX)&&(key_enc[0].did!=0))
	// 	{
	// 		flash_write_page(ID_SAVE_STORAGE_ADDR+custom_binding_idx*ID_LEN,ID_LEN,(u8*)&key_enc[0].did);

	// 		custom_binding_idx++;
	// 	}

	// 	if((key_enc[1].did!=U32_MAX)&&(key_enc[1].did!=0))
	// 	{
	// 		flash_write_page(ID_SAVE_STORAGE_ADDR+custom_binding_idx*ID_LEN,ID_LEN,(u8*)&key_enc[1].did);

	// 		custom_binding_idx++;
	// 	}
	// }
#endif

	flash_read_page(PAIR_DEVICE_SAVE_ADDR, ID_LEN, (u8 *)&Device_fixed_id);
	printf("Device_fixed_id:%4x\r\n", Device_fixed_id);
	if (Device_fixed_id == U32_MAX)
	{
		static u32 pair_device_dongle_id = U32_MAX;
		pair_device_dongle_id = user_config.mac_addr << 8;
		pair_device_dongle_id++;
		app_rcu_flash_unlock_init();
		flash_write_page(PAIR_DEVICE_SAVE_ADDR, ID_LEN, (u8 *)&pair_device_dongle_id);
		app_rcu_flash_lock_init();
		flash_read_page(PAIR_DEVICE_SAVE_ADDR, ID_LEN, (u8 *)&Device_fixed_id);
		printf("--->PAIR_DEVICE_SAVE_ADDR = 0x%04x.\r\n", Device_fixed_id);
	}
	// tick_factory_test_check = clock_time() | 1;
	tick_factory_version_ir_send = clock_time() | 1;
	factory_test_mode_flag = POWER_ON_STATE;
	printf("---custom_binding_idx=%d.\r\n", custom_binding_idx);
	printf("---key_enc[0].did=0x%04x.\r\n", key_enc[0].did);
	printf("---key_enc[1].did=0x%04x.\r\n", key_enc[1].did);
	printf("---key_enc[2].did=0x%04x.\r\n", key_enc[2].did);
	printf("---user_config.paring_limit_t = %d.\r\n", user_config.paring_limit_t);
}
void usb_custom_init()
{
	// enable USB manual interrupt(in auto interrupt mode,USB device would be USB printer device)
	usb_init_interrupt();

#if USB_DESCRIPTOR_MY_SELF
#if OTA_ENABLE_AAA
	usb_register_set_report(app_hid_set_report_handle);

	write_reg8(0x10e, (1 << USB_EDP_MOUSE) | (1 << USB_EDP_KEYBOARD_IN) | (1 << USB_EDP_SPP_IN));

	notify_rsp_buf_init();
#else
	write_reg8(0x10e, (1 << USB_EDP_MOUSE) | (1 << USB_EDP_KEYBOARD_IN));
#endif
#endif

	// deepsleep_dp_dm_gpio_low_wake_enable(); // TODO: check B80 have not this interface

	// enable USB DP pull up 1.5k
	usb_set_pin_en();
}
void timer1_init_aaa()
{
#if (MY_CHANNEL_ENABLE == 0)
	timer1_set_mode(TIMER_MODE_SYSCLK, 0, 8 * CLOCK_SYS_CLOCK_1MS);
	timer_start(TIMER1);
#endif
}

#define FW_OFFSET_IN_FLASH 0x0000

// 射频初始化
void rf_drv_private_2m_init()
{
	// tbl_rf_init
	write_reg8(0x12d2, 0x9b); // DCOC_DBG0
	write_reg8(0x12d3, 0x19); // DCOC_DBG1
	write_reg8(0x127b, 0x0e); // BYPASS_FILT_1
	write_reg8(0x1279, 0x38);

	// start  add follow setting follow 8278 drv
	write_reg8(0x124a, 0x0e); // POW_001_010_L
	write_reg8(0x124b, 0x09); // POW_001_010_H
	write_reg8(0x124e, 0x09); // POW_101_100_L
	write_reg8(0x124f, 0x0f); // POW_101_100_H
	write_reg8(0x1254, 0x0e); // POW_001_010_L
	write_reg8(0x1255, 0x09); // POW_001_010_H
	write_reg8(0x1256, 0x0c); // POW_011_100_L
	write_reg8(0x1257, 0x08); // POW_011_100_H
	write_reg8(0x1258, 0x09); // POW_101_100_L
	write_reg8(0x1259, 0x0f); // POW_101_100_H
	write_reg8(0x1276, 0x50); // FREQ_CORR_CFG2_0
	write_reg8(0x1277, 0x73); // FREQ_CORR_CFG2_1
	// For optimum Rx chain
	write_reg8(0x134e, 0x45); // CBPF_TRIM_I && CBPF_TRIM_Q
	write_reg8(0x134c, 0x02); // LNA_ITRIM=0x01(default)(change to 0x02[TBD])
	// end add

	// tbl_rf_private_2m
	write_reg8(0x1220, 0x04); // SC_CODE
	write_reg8(0x1221, 0x2a); // IF_FREQ
	write_reg8(0x1222, 0x43); // HPMC_EXP_DIFF_COUNT_L
	write_reg8(0x1223, 0x06); // HPMC_EXP_DIFF_COUNT_H
	write_reg8(0x1254, 0x0e); // AGC_THRSHLD1_2M_0
	write_reg8(0x1255, 0x09); // AGC_THRSHLD1_2M_1
	write_reg8(0x1256, 0x0c); // AGC_THRSHLD2_2M_0
	write_reg8(0x1257, 0x08); // AGC_THRSHLD2_2M_1
	write_reg8(0x1258, 0x09); // AGC_THRSHLD3_2M_0
	write_reg8(0x1259, 0x0f); // AGC_THRSHLD3_2M_1

	write_reg8(0x400, 0x1f); // tx mode
	write_reg8(0x402, 0x42); // preamble length
	write_reg8(0x404, 0xea); // nordic head
	write_reg8(0x421, 0xa1); // len_0_en
	write_reg8(0x430, 0x3c); //<1>hd_timestamp
	// AGC table
	write_reg8(0x460, 0x36); // grx_0
	write_reg8(0x461, 0x46); // grx_1
	write_reg8(0x462, 0x51); // grx_2
	write_reg8(0x463, 0x61); // grx_3
	write_reg8(0x464, 0x6d); // grx_4
	write_reg8(0x465, 0x78); // grx_5

	////Add start the following settings compared with tbl_rf_zigbee_250k and these settings are default values.
	write_reg8(0x122a, 0x90);
	write_reg8(0x122c, 0x38);
	write_reg8(0x123d, 0x00);
	write_reg8(0x420, 0x1e);
	write_reg8(0x405, 0x04);
	write_reg8(0x422, 0x04);
	write_reg8(0x408, 0xc9);
	write_reg8(0x409, 0x8a);
	write_reg8(0x40a, 0x11);
	write_reg8(0x40b, 0xf8);
	write_reg8(0x407, 0x01);
	write_reg8(0x403, 0x44);
	//// add end

	write_reg16(0xf06, 0);	  // rx wait time
	write_reg8(0x0f0c, 0x54); // rx settle time: Default 150us, minimum 85us(0x54+1)

	write_reg16(0xf0e, 0);	  // tx wait time
	write_reg8(0x0f04, 0x70); // tx settle time: Default 150us, minimum 113us(0x70+1)		//////can open short time

	write_reg8(0xf10, 0); // wait time on NAK

#if (RF_FAST_TX_SETTING_ENABLE || RF_FAST_RX_SETTING_ENABLE)
	rf_fast_setting_flag = 0;
	tick_rf_fast_setting = clock_time() | 1;
#if RF_FAST_RX_SETTING_ENABLE // must fist  call rx fast settle init
	rf_rx_fast_settle_init();
	rf_rx_fast_settle_dis();
#endif

#if RF_FAST_TX_SETTING_ENABLE
	rf_tx_fast_settle_init();
	rf_tx_fast_settle_dis();
#endif
#endif

	reg_dma_chn_en |= FLD_DMA_CHN_RF_RX | FLD_DMA_CHN_RF_TX;
	// g_RFMode = RF_MODE_PRIVATE_2M;
	//  rf_rx_buffer_set((u8 *)rx_packet, RX_FRAME_SIZE, 0);

	// rf_acc_len_set(5);
	write_reg8(0x405, (read_reg8(0x405) & 0xf8) | 5); // access_byte_num[2:0]
	write_reg8(0x420, 35);

	rf_set_tx_rx_off();
	rf_rx_acc_code_enable(0x01);
	rf_tx_acc_code_select(0);

	irq_disable();
	irq_clr_src();
	// rf_irq_src_allclr();
	irq_enable_type(FLD_IRQ_ZB_RT_EN); // enable RF irq
	rf_irq_disable(FLD_RF_IRQ_ALL);
	rf_irq_enable(FLD_RF_IRQ_TX | FLD_RF_IRQ_RX | FLD_RF_IRQ_RX_TIMEOUT | FLD_RF_IRQ_FIRST_TIMEOUT); //
	irq_enable();
}

void user_init()
{
	rf_user_init();

	// OTA初始化
	OTA_INIT();

	power_supply_voltage_check_init();

	set_led_brightness();

	// 按键参数初始化
	Variable_tansfer();

#if (UI_KEYBOARD_ENABLE)
	/////////// keyboard gpio wakeup init ////////
	u32 pin[] = KB_DRIVE_PINS;
	for (int i = 0; i < (sizeof(pin) / sizeof(*pin)); i++)
	{
		cpu_set_gpio_wakeup(pin[i], Level_High, 1); // drive pin pad high wakeup deepsleep
	}
#endif

#if STUCK_KEY_COMFIGURATION
	Stuck_key_release_event();
#endif

	dev_wake_time_tick = clock_time(); // 设备休眠时间戳 设定3秒无更新进休眠

	irq_enable();

	power_on_tick = clock_time() | 1;
}

const u16 consumer_list[] = {
	0x221, // 0xa3 C_WWW_SEARCH
	0x223, // 0xa4 C_WWW_HOME
	0x224, // 0xa5 C_WWW_BACKWARD
	0x225, // 0xa6 C_WWW_FORWARD
	0x226, // 0xa7 C_WWW_STOP
	0x227, // 0xa8 C_WWW_REFRESH
	0x22A, // 0xa9 C_MY_FAVORITE
	0x183, // 0xaa C_MEDIA_SELECT

	0x18A, // 0xab C_EMAIL
	0x192, // 0xac C_CALCULATOR
	0x194, // 0xad C_MY_COMPUTER
	0xB5,  // 0xae C_NEXT_TRACK
	0xB6,  // 0xaf C_PRE_TRACK
	0xB7,  // 0xb0 C_STOP
	0xCD,  // 0xb1 C_PLAY_PAUSE
	0xE2,  // 0xb2 C_MUTE

	0xE9,  // 0xb3 C_VOL_INC
	0xEA,  // 0xb4 C_VOL_DEC
	0x00,  // 0xb5 telinkԶ
	0x22D, // 0xb6 USAGE ZOOM IN
	0x22E, // 0xb7	 USAGE ZOOM OUT
	0x236, // 0xb8	 USAGE PAN LEFT
	0x237, // 0xb9	 USAGE PAN RIGHT
	0x30B, // 0xba	C_BRIGHT_INC

	0x30A, // 0xbb	C_BRIGHT_DEC
	0xB8,  // 0Xbc	 c_rject
	0x30,  // 0Xbd	C_POWER
	0x19E, // 0Xbe	C_TERMINAL_LOCK
};

volatile u8 has_consume_key_press = 0;
volatile u8 has_normal_key_press = 0;
volatile u8 has_system_key_press = 0;


u32 need_enter_suspend_tick;
u8 ir_send_data[6] = {0};
u8 last_ir_send_data[6] = {0};
u8 kb_data_handle(u8 *buf)
{
	// u16 consume_key=0;
	// u8 system_key=0;
	// u8 normal_key[8]={0,0,0,0,0,0,0,0};
	// u8 nk_offset=2;

	u8 len = 0;
	for (u8 i = 0; i < 6; i++)
	{
		if (buf[0] == 0x4c)
		{
			len++;
			ir_send_data[i] = buf[i];
			// printf("buf[%d] = 0x%1x\r\n",i,buf[i]);
		}
		else
			break;
	}
	if (len != 0 && (memcmp(last_ir_send_data, ir_send_data, 6) != 0))
	{
		printf("ir_send_data send\r\n");
		memcpy(last_ir_send_data, ir_send_data, 6);
		key_ir_send_tick = clock_time();
		ir_send(ir_send_data);
	}
	// if(len==0)//release
	// {
	// 	has_normal_key_press=0;
	// 	has_consume_key_press=0;
	// 	has_system_key_press=0;
	// 	if(need_enter_suspend_tick==0)
	// 	{
	// 	special_key_wake_up_pc();//for fn
	// 	}
	// }
	// else
	// {
	// 	for(u8 i=0;i<len;i++)
	// 	{
	// 		if((buf[i]>=0xa0)&&(buf[i]<=0xa2))
	// 		{
	// 			system_key=1<<(buf[i]-0xa0);
	// 			has_system_key_press=1;
	// 		}
	// 		else if((buf[i]>=0xa3)&&(buf[i]<=0xbe))
	// 		{
	// 			consume_key=consumer_list[buf[i]-0xa3];
	// 			has_consume_key_press=1;
	// 		}
	// 		else if((buf[i]>=0xe0)&&(buf[i]<=0xe7))
	// 		{
	// 			normal_key[0] |= (1 << (buf[i]-0xe0));
	// 			has_normal_key_press=1;
	// 		}
	// 		else
	// 		{
	// 			normal_key[nk_offset++]=buf[i];
	// 			has_normal_key_press=1;
	// 		}
	// 	}
	// }

	// kb_tick=clock_time();
	// if(memcmp(normal_last_data, normal_key,8))
	// {
	// 	memcpy(normal_last_data, normal_key,8);
	// 	push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, normal_key, 8);
	// }
	// if(memcmp(consume_last_data, (u8*)&consume_key,2))
	// {
	// 	memcpy(consume_last_data, (u8*)&consume_key,2);
	// 	special_key_wake_up_pc();//for fn
	// 	push_usb_fifo_aaa(CONSUME_DATA_TYPE, (u8*)&consume_key, 2);
	// }
	// if(sys_last_data!=system_key)
	// {
	// 	sys_last_data=system_key;
	// 	special_key_wake_up_pc();//for fn
	// 	push_usb_fifo_aaa(SYSTEM_DATA_TYPE, &system_key, 1);
	// }

	return 1;
}


static inline void check_kb_led_status()
{
#if USER_DEBUG
	static u8 last_led_status = 0xff;
	if (last_led_status != host_keyboard_status)
	{
		last_led_status = host_keyboard_status;
		gpio_write(D1_LED_PIN, last_led_status & 0x01);
		gpio_write(D2_LED_PIN, last_led_status & 0x02);
		gpio_write(D3_LED_PIN, last_led_status & 0x04);
	}
#endif
}

void mcu_enter_suspend()
{
	if (clock_time_exceed(tick_suspend_interval, 40000))
	{
		u8 r = irq_disable();

		cpu_sleep_wakeup(SUSPEND_MODE, PM_WAKEUP_TIMER | PM_WAKEUP_CORE_USB, clock_time() + 200 * CLOCK_16M_SYS_TIMER_CLK_1MS);

		rf_user_init();

		timer1_init_aaa();

		irq_restore(r);
		tick_suspend_interval = clock_time() | 1;
		irq_enable();
	}
}

static inline void check_pair_timeout()
{
	if (power_on_tick && clock_time_exceed(power_on_tick, user_config.paring_limit_t * 1000000))
	{
		power_on_tick = 0;
		clear_pair_enable_flag();
	}
}

void usb_host_status_check(void)
{
	static u8 last_status = 0xff;

	if (reg_usb_host_conn & BIT(7))
	{
#if 0
		if((reg_usb_mdev & BIT(2)) && (reg_irq_src & FLD_IRQ_USB_PWDN_EN)) 
		{
			usb_device_status = USB_DEVICE_CHECK_PC_SLEEP;
		} 
		else 
		{
			usb_device_status = USB_DEVICE_CONNECT_PC;
		}
#else
		if ((reg_irq_src & FLD_IRQ_USB_PWDN_EN))
		{
			if (reg_usb_mdev & BIT(2))
			{
				usb_device_status = USB_DEVICE_CHECK_PC_SLEEP;
			}
			else
			{
				usb_device_status = USB_DEVICE_UNPLUG;
			}
		}
		else
		{

			usb_device_status = USB_DEVICE_CONNECT_PC;
		}
#endif
	}
	else
	{
		usb_device_status = USB_DEVICE_DISCONECT_PC;
	}

	if (last_status != usb_device_status)
	{
		printf("---usb_device_status=%d.\r\n", usb_device_status);

		if ((usb_device_status == USB_DEVICE_CONNECT_PC)) // fist press key then restart pc
		{
			// led_status_out(host_keyboard_status);
			allow_send_to_usb_flag = 1;
			if (last_status == USB_DEVICE_DISCONECT_PC)
			{
				// usb_io_printf("pc_power_on");
				if (has_normal_key_press)
				{
					push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, normal_last_data, 8);
				}
			}
			need_enter_suspend_flag = 0;
		}
		else if (usb_device_status == USB_DEVICE_CHECK_PC_SLEEP)
		{
			need_enter_suspend_flag = 1;
			need_enter_suspend_tick = clock_time() | 1;
			// led_status_out(0);
		}
		else if (usb_device_status == USB_DEVICE_DISCONECT_PC)
		{
			need_enter_suspend_flag = 1;
			need_enter_suspend_tick = clock_time() | 1;
			// led_status_out(0);
		}
		last_status = usb_device_status;
	}

	if ((usb_device_status == USB_DEVICE_CHECK_PC_SLEEP) && (need_enter_suspend_flag))
	{
		mcu_enter_suspend();
	}
}

void rf_stx_to_rx(u8 *p, u32 rx_timeout_us)
{
#if (RF_FAST_TX_SETTING_ENABLE || RF_FAST_RX_SETTING_ENABLE)
	rf_set_tx_rx_setting();
#endif

	PIN_DEBUG_RF_TX_LEVEL(1);
	PIN_DEBUG_RF_RX_LEVEL(1);
	PIN_DEBUG_RF_RX_TIMEOUT_LEVEL(1);
	PIN_DEBUG_RF_RX_CRC_OK_LEVEL(1);
	PIN_DEBUG_RF_RX_DATA_OK_LEVEL(1);

#if RF_TX_TO_RX_AUTO_MODE_ENABLE
	rf_trx_state_set(RF_MODE_AUTO, rf_chn[device_channel]);
	rf_start_stxtorx(p, ClockTime(), rx_timeout_us);
#else
	// rf_set_tx_pipe(0);
	// rf_start_stx(p, ClockTime());
	rf_set_channel(rf_chn[device_channel], 0);
	my_rf_start_stx(p, ClockTime());
#endif
	/*
		printf("rf_chn[device_channel] == %d, TX: ",rf_chn[device_channel]);
		printf("[ ");
		for(int i = 0; i < 23; i++)
			printf("%02x ",p[i]);
		printf("]\r\n");
	*/
}

void check_rf_complet_status()
{
	if (device_channel == 0)
		device_channel = 4; // 2422
	else if (device_channel == 4)
		device_channel = 10; // 2450
	else if (device_channel == 10)
		device_channel = 14; // 2470
	else
		device_channel = 0; // 2405
}

void pair_write_flash()
{
	static u32 pair_earase_date = 0xffffffff;
	// 设备配对成功写入FLASH
	if (pair_addrass_write_flash == true && Writes_flash_flag == true)
	{
		app_rcu_flash_unlock_init(); // UNLOCK
		flash_write_page(PAIR_DEVICE_SAVE_ADDR, ID_LEN, (u8 *)&pair_earase_date);
		flash_write_page(PAIR_DEVICE_SAVE_ADDR, ID_LEN, (u8 *)&Device_fixed_id);
		app_rcu_flash_lock_init();
		// FLASH获取设备信息
		flash_read_page(PAIR_DEVICE_SAVE_ADDR, ID_LEN, (u8 *)&Device_fixed_id);
		// printf("--->PAIR_DEVICE_SAVE_ADDR = 0x%04x.\r\n", Device_fixed_id);
		Writes_flash_flag = false;
		pair_addrass_write_flash = false;
	}
}

void d24g_rf_loop()
{

	check_rf_fast_setting_time();

	// 设备发送匹配信号
	if (g_sys.pair_status == PAIRING)
	{
		// printf("send_pair_flag\r\n");
		if (bath_send_need_f == 1)
		{
			if (Writes_flash_flag == false)
			{
				static u8 Device_before_bit = 0xff;
				Device_before_bit = 0x000000ff & Device_fixed_id;
				if (Device_before_bit == 0xfe)
				{
					Device_fixed_id = 0xffffff00 & Device_fixed_id;
				}

				Writes_flash_flag = true;
				++Device_fixed_id;
			}
			set_pair_access_code(0x39517695);
			rf_type.data_packet.dma_len = 19;
			rf_type.data_packet.rf_len = 18;
			rf_type.data_packet.head_0 = 0x00;
			rf_type.data_packet.head_1 = 0x53;
			rf_type.data_packet.head_2 = 0x54;
			rf_type.data_packet.did = Device_fixed_id;
			rf_type.data_packet.cmd = CMD_24G_PAIR_CMD;
			rf_type.data_packet.rsv1[1] = DEVICE_PAIR_RSV_1;
			rf_type.data_packet.rsv1[2] = DEVICE_PAIR_RSV_2;
		}

#if (NOW_PLATE == HORIZONTAL_PLATE)
		if (Rf_increase_flag == false)
			rf_set_power_level_index(130);
		else if (Rf_increase_flag == true)
			rf_set_power_level_index(132);

		static volatile u32 pair_send_time;
		if (!clock_time_exceed(pair_send_time, 5 * 10 * 1000))
		{
			PIN_DEBUG_RF_TX_LEVEL(1);
			rf_set_tx_rx_off();
			rf_start_stx((void *)&rf_type.data_packet, clock_time());
			reg_rf_irq_status = 0xffff;
			WaitMs(1);
		}
		else
		{
			rf_set_tx_rx_off();
			rf_set_tx_rx_setting();
			rf_set_rxmode();
			if (clock_time_exceed(pair_send_time, 10 * 10 * 1000))
			{
				pair_send_time = clock_time();
			}
		}
#endif
#if (NOW_PLATE == VERTICAL_PLATE)
		if (Rf_increase_flag == false)
			rf_set_power_level_index(130);
		else if (Rf_increase_flag == true)
			rf_set_power_level_index(142);

		static volatile u32 pair_send_time;
		if (!clock_time_exceed(pair_send_time, 5 * 10 * 1000))
		{
			PIN_DEBUG_RF_TX_LEVEL(1);
			rf_set_tx_rx_off();
			rf_start_stx((void *)&rf_type.data_packet, clock_time());
			reg_rf_irq_status = 0xffff;
			WaitMs(1);
		}
		else
		{
			rf_set_tx_rx_off();
			rf_set_tx_rx_setting();
			rf_set_rxmode();
			if (clock_time_exceed(pair_send_time, 10 * 10 * 1000))
			{
				pair_send_time = clock_time();
			}
		}
#endif

		// rf_send_count++;
		// if (rf_send_count <= 144)
		// {
		// 	rf_set_tx_rx_off();
		// 	rf_set_txmode();
		// 	rf_start_stx((void *)&rf_type.data_packet, clock_time());
		// 	reg_rf_irq_status = 0xffff;
		// }
		// else if (rf_send_count >= 750)
		// {
		// 	rf_set_tx_rx_off();
		// 	rf_set_rxmode();
		// 	rf_send_count = 0;
		// 	DEVICE_PAIR_STATUS = 0x01;
		// }
		// rf_start_stx((void *)&rf_type.data_packet, clock_time());
		// reg_rf_irq_status = 0xffff;
		// rf_set_tx_rx_off();
		// WaitMs(10);

		/*************** mode three ********************/
	}

	if (bath_send_need_f == 2)
	{
		set_pair_access_code(0x39517695);
		rf_type.data_packet.head_0 = 0x00;
		rf_type.data_packet.head_1 = 0x53;
		rf_type.data_packet.head_2 = 0x54;
		rf_type.data_packet.did = Device_fixed_id;
		rf_type.data_packet.cmd_1 = 0x11;
		// rf_set_power_level_index(RF_POWER_P11p46dBm);
	}
	if (bath_send_need_f >= 2)
	{
		rf_set_power_level_index(RF_POWER_P0p59dBm);
		PIN_DEBUG_RF_TX_LEVEL(1);
		rf_set_tx_pipe(0);
		rf_set_tx_rx_off();
		rf_start_stx((void *)&rf_type.data_packet, clock_time());
		WaitMs(1);
		if (bath_send_need_f >= 2)
		{
			bath_send_need_f++;
			if (bath_send_need_f >= 108)
			{
				printf_rf_data(&rf_type.data_packet.head_0,rf_type.data_packet.rf_len,"cmd_data");
				bath_send_need_f = 0;
			}
		}
	}
	check_rf_complet_status();
}

void my_rf_start_srx(unsigned int tick, unsigned int rx_timeout)
{
	write_reg8(0x800f00, 0x80);			   // disable srx
	write_reg32(0x800f28, rx_timeout - 1); // first timeout
	write_reg32(0x800f18, tick);
	write_reg8(0x800f16, read_reg8(0x800f16) | 0x04); // Enable cmd_schedule mode
	write_reg16(0x800f00, 0x3f86);					  // srx
}

void main_loop(void)
{

	usb_handle_irq(); // must first

#if OTA_ENABLE_AAA
	usb_update_loop();
#endif

	usb_host_status_check();

#if USER_DEBUG
	test_button();
#endif

#if EMI_TEST_FUN_ENABLE_AAA

	// usb_host_cmd_proc();

	if (emi_flg)
	{
		return;
	}
#endif

#if KM_DATA_HANDLE_CALL_BACK_ENABLE
	if (p_keyboard_data_handle_post)
	{
		(*p_keyboard_data_handle_post)((u8 *)to_usb_keyboard_data); // Run the data handler function
		p_keyboard_data_handle_post = NULL;
	}

	if (p_mouse_data_handle_post)
	{
		(*p_mouse_data_handle_post)((u8 *)to_usb_mouse_data); // Run the data handler function
		p_mouse_data_handle_post = NULL;
	}
#endif

	if (p_post)
	{
		(*p_post)(raw_pkt);
		p_post = NULL;
	}

	// pull_usb_data();
	// check_kb_led_status();
	// check_pair_timeout();
#if RF_24G_FACTORY_ENABLE
	rf_tx_process_factory();
	factory_test_check();
#endif
	soft_tick_poll();

	d24g_rf_loop();

	// key_event_loop();
#if (UI_KEYBOARD_ENABLE)
	proc_keyboard_Test(0, 0, 0);
#endif

	if (need_enter_suspend_tick && clock_time_exceed(need_enter_suspend_tick, 3 * 1000 * 1000))
	{
		need_enter_suspend_tick = 0;
	}
}
int sm_sum_x, sm_pre_x, sm_sum_y, sm_pre_y;

void iir_smoother()
{
	sm_sum_x = sm_sum_x - sm_pre_x + x_smoother;
	sm_pre_x = sm_sum_x / 2;
	x_smoother = sm_pre_x;

	sm_sum_y = sm_sum_y - sm_pre_y + y_smoother;
	sm_pre_y = sm_sum_y / 2;
	y_smoother = sm_pre_y;
}

static inline void iir_smoother_clear(void)
{
	sm_sum_x = 0;
	sm_pre_x = 0;

	sm_sum_y = 0;
	sm_pre_y = 0;
}
#define sm_dyn_pth1 6
#define sm_dyn_pth2 4
u8 adaptive_smoother()
{
	static u8 asm_flg = 0;
	static u32 sm_last_smoother_tick = 0;

	// auto clear asm sum when no data for a long time
	if (asm_flg && clock_time_exceed(sm_last_smoother_tick, 100000))
	{
		asm_flg = 0;
		iir_smoother_clear();
	}
	if (!asm_flg)
	{
		if ((abs(x_smoother) > sm_dyn_pth1) || (abs(y_smoother) > sm_dyn_pth1))
		{
			asm_flg = 1;
			iir_smoother();
		}
		else
		{
			asm_flg = 0;
			iir_smoother_clear();
		}
	}
	else
	{
		if ((abs(x_smoother) < sm_dyn_pth2) && (abs(y_smoother) < sm_dyn_pth2))
		{
			asm_flg = 0;
			iir_smoother_clear();
		}
		else
		{
			asm_flg = 1;
			iir_smoother();
		}
	}

	if (asm_flg)
	{
		sm_last_smoother_tick = clock_time();
	}

	return asm_flg;
}
void ir_key_data_check_timer(void)
{
	if (clock_time_exceed(key_ir_send_tick, 260000))
	{
		key_ir_send_tick = clock_time();
		memset(last_ir_send_data, 0, sizeof(last_ir_send_data));
	}
}
void factory_version_ir_send(void)
{
	static bool test_flag = 0;
	u8 ir_factory_send_data[6] = {0};
	if (clock_time_exceed(tick_factory_version_ir_send, 1000000))
	{
		if (test_flag == 0)
		{
			test_flag = 1;
			tick_factory_version_ir_send = clock_time() | 1;
			ir_factory_send_data[0] = user_config.mac_addr & 0xff;
			ir_factory_send_data[1] = (user_config.mac_addr >> 8) & 0xff;
			ir_factory_send_data[2] = (user_config.mac_addr >> 16) & 0xff;
			ir_factory_send_data[3] = (user_config.mac_addr >> 24) & 0xff;
			ir_factory_send_data[4] = 0;
			ir_factory_send_data[5] = 0;
			ir_send(ir_factory_send_data);
		}
		else
		{
			test_flag = 0;
			tick_factory_version_ir_send = clock_time() | 1;
			ir_factory_send_data[0] = fw_versoion[0];
			ir_factory_send_data[1] = fw_versoion[1];
			ir_factory_send_data[2] = 0;
			ir_factory_send_data[3] = 0;
			ir_factory_send_data[4] = 0;
			ir_factory_send_data[5] = 0;
			ir_send(ir_factory_send_data);
		}
		// printf("factory_version_ir_send \n\r");
	}
}

/******************************************************************
 * @brief  app_rcu_flash_lock_init
 * @param  none
 * @return none
 * @retval void
 */
void app_rcu_flash_lock_init(void)
{
	unsigned int mid = flash_read_mid();
	printf("--->>> [app_rcu_flash_lock_init] mid: 0x%x\r\n", mid);
	switch (mid)
	{
	case 0x1160c8:
		flash_lock_mid1160c8(FLASH_LOCK_ALL_128K_MID1160C8);
		break;
	case 0x114485:
		flash_lock_mid114485(FLASH_LOCK_ALL_128K_MID114485);
	default:
		break;
	}
}

/******************************************************************
 * @brief  app_rcu_flash_unlock_init
 * @param  none
 * @return none
 * @retval void
 */
void app_rcu_flash_unlock_init(void)
{
	unsigned int mid = flash_read_mid();
	printf("--->>> [app_rcu_flash_unlock_init] mid: 0x%x\r\n", mid);
	switch (mid)
	{
	case 0x1160c8:
		flash_unlock_mid1160c8();
		break;
	case 0x114485:
		flash_unlock_mid114485();
		break;
	default:
		break;
	}
}
