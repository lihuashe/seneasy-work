/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     factory_profile.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef FACTORY_PROFILE_H_
#define FACTORY_PROFILE_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "../app_config.h"
#include "../../tl_common.h"

#if(FACTORY_ENABLE)
/*============================================================================*
*                        Export Global Variables
*============================================================================*/
extern const u8 uuid_service_factory_rcu[16];
extern const u8 uuid_char_factory_rcu_rd[16];
extern const u8 uuid_char_factory_rcu_wr[16];
extern const u8 char_decl_factory_rcu_rd[19];
extern const u8 char_decl_factory_rcu_wd[19];
extern u16 char_desc_factory_rcu_ccc;
extern u8 factory_rcu_data_ccc;
extern u8 factory_mode_flag;
extern u8 factory_report_ram_flag;
extern u8 raw_buff[20];
extern u8 factory_Gsensor_Flag;
typedef struct
{	
	u8 cmd;//data type
	u8 seq_no;
	u8 pno_no;	
	u8 tick_0;
	u8 tick_1;
	u8 chn;
	u8 buf[40];
} factory_24G_data_t;

typedef struct
{
	u8	cmd;
	u8	seq_no;
	u8	pn_no;		//bit7=0: no aes  =1: aes
	u32 did;
	u8  km_dat[20];	//mouse data or kb
	u8  rsv1[3]; 	//for aes  16 bytes
	u16 crc16;
} factory_c_1_data_t;

//Factory CMD
#define CMD_REQ_SOFTWARE_VERSION   	    0x01
#define CMD_REQ_MAC_ADDRESS  	        0x02
#define CMD_REQ_SINGLETONE_MODE       	0x03
#define CMD_REQ_EXIT_FACTORY            0x04
#define CMD_REQ_ONOFF_GSENSOR           0x05
#define CMD_REQ_DATA_GSENSOR            0x06
#define CMD_REQ_ONOFF_LED               0x07
#define CMD_REQ_PAIR_STATUS             0x08
#define CMD_REQ_RCU_BATTERY             0x09
#define CMD_REQ_RETRY_PAIR              0x0A
#define CMD_REQ_SEND_TX_DATA	 	    0x0D
#define CMD_REQ_ENTER_SHIP_MODE      	0x0F
#define CMD_REQ_TOUCH_IC_VERSION      	0x10
/*============================================================================*
 *                          Functions
 *============================================================================*/
int factory_rcu_svc_attr_write_cb(void *p);
int factory_rcu_svc_attr_read_cb(void *p);
int factory_rcu_ccc_update_cb(void *p);
void factory_rcu_enter_single_tone(void);
void report_factory_status(u8 factory_status, u8 *raw_buf);
void recv_factory_data(u8 *factory_buf);
#endif
#endif /* FACTORY_PROFILE_H_ */
