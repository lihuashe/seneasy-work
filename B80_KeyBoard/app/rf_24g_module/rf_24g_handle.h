/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     rf_24g_hangle.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef RF_24G_HANDLE_H_
#define RF_24G_HANDLE_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "../app_config.h"
#include "../../tl_common.h"

/*============================================================================*
*                        Export Global Variables
*============================================================================*/
extern _attribute_data_retention_ u8 pair_flag;
typedef struct{
	u32 dev_mac; 		//00
	u16 group_id;		//04~05
	u16 rf_vid;			//06~07
	u8  bat_type;		//008
	u8	tx_power;		//09
	u8	paring_tx_power;//0a
	u8  emi_tx_power;   //00b
	u8	wheel_direct;   //0c 
    u8	internal_cap;   //0d
	u8  sensor_direct;  //0e
	u8 	aes_enable; 	//0f
    u8  pub_key[16];    //10~1f
	u8  device_name[18];//20~31
} custom_cfg_t;
extern custom_cfg_t user_cfg;

enum
{
    EMPTY_CMD 			= 0x00,
    EMPTY_ACK_CMD 		= 1,
    PAIR_CMD 			= 2,
    PAIR_ACK_CMD 		= 3,
	MOUSE_CMD 			= 4,
	MOUSE_ACK_CMD 		= 5,
	KB_CMD 				= 6,
	KB_ACK_CMD 			= 7,
	RECONNECT_CMD 		= 8,
	RECONNECT_ACK_CMD	= 9,
	D24G_OTA_CMD		= 10,
	D24G_OTA_ACK_CMD 	= 11,
	DG_OTA_CMD			= 12,
	DG_OTA_ACK_CMD 		= 13,
	HEART_BEAT_CMD		= 14,
	HEART_BEAT_ACK_CMD	= 15,
	RCU_FW_CMD			= 16,
	RCU_FW_ACK_CMD		= 17,
	RCU_MAC_CMD			= 18,
	RCU_MAC_ACK_CMD		= 19,
	FACTORY_TEST_CMD	= 20,
	FACTORY_TEST_ACK_CMD = 21,
};

typedef struct
{
    u32 dma_len;
    u8  rf_len; 
    u8  dat[59];
} rf_packet_t;

typedef struct
{	
	u8  cmd;			//data type
	u8  seq_no;
	u8  pno_no;
	u32 did;
	u8  key[12];
} pair_data_t;
pair_data_t *p_pair_dat;

typedef struct
{	
	u8 cmd;//data type
	u8 seq_no;
	u8 pno_no;	
	u8 tick_0;
	u8 tick_1;
	u8 chn;
	u8 host_led_status;
	u32 gid;
	u32 did;
	u8 key[12];
} pair_ack_data_t;

typedef struct
{
	u8	cmd;
	u8	seq_no;
	u8	pn_no;		//bit7=0: no aes  =1: aes
	u32 did;
	u8  km_dat[6];	//mouse data or kb
	u8  rsv1[3]; 	//for aes  16 bytes
	u16 crc16;
} km_3_c_1_data_t;

typedef struct
{
	u8 cmd;//data type
	u8 seq_no;
	u8 pno_no;
	u8 tick_0;
	u8 tick_1;
	u8 chn;
	u8 host_led_status;
} km_ack_data_t;

typedef struct
{
	u32 dongle_id;
    u8  key[12];	//12
	u8  slave_mac_addr[4];
	u8  mode;
	u8  mast_id;
	u16 temp1; 		//24
	u8  temp2[8]; 	//32
} FLASH_DEV_INFO_AAA;

#define TX_BUF_SIZE_AAA   10
typedef struct
{
	u8 type;
	u8 len;
	u8 buf[TX_BUF_SIZE_AAA-2];
}TX_PACKET_AAA;

typedef enum
{
    STATE_POWERON = 0,
#if (FREQUENY_HOPPING_1K == 0)
    STATE_SYNCING,
#endif
    STATE_PAIRING,
    STATE_NORMAL,
	STATE_OTA,
	STATE_NONE,
} DEVICE_STATE;

extern _attribute_data_retention_ u32 dongle_id;
extern _attribute_data_retention_ u32 idle_tick;
extern _attribute_data_retention_ u32 idle_count;
extern _attribute_data_retention_ u8 device_status;
extern _attribute_data_retention_ u8 dev_info_idx;
extern _attribute_data_retention_ volatile unsigned int rf_state;
extern _attribute_data_retention_ FLASH_DEV_INFO_AAA flash_dev_info;
extern rf_packet_t rf_pair_buf;
extern rf_packet_t rf_km_buf;
extern km_3_c_1_data_t *p_km_data;
extern u8 need_suspend_flag; 
/*============================================================================*
 *                          Functions
 *============================================================================*/
void rf_24g_main_loop(void);
void rf_24g_config(void);
void rf_24g_user_init(void);
void reset_idle_status(void);
void check_rf_complet_status(void);
int flash_info_load_aaa(u32 s_addr, u8 *d_addr,  int len );

#endif /* RF_24G_HANDLE_H_ */
