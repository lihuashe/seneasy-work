/********************************************************************************************************
 * @file     AAA_24G_rf_frame.h
 *
 * @brief    This is the header file for KMD SDK
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

/*
 * led_rf_frame.h
 *
 *  Created on: Jan 13, 2014
 *      Author: xuzhen
 */

#ifndef _AAA_24G_RF_FRAME_H_
#define _AAA_24G_RF_FRAME_H_

#define HEAD_0  0x00
#define HEAD_1  0x53
#define HEAD_2  0x54
#define PAIR_CMD  0x55
#define PAIR_ACK_CMD 0x88
#define MOUSE_DATA_LEN_AAA  6

enum
{
    PIPE_PARING			= 0x00,
    PIPE_MOUSE			= 0x01,
    PIPE_KEYBOARD		= 0x02,
    PIPE_ALL   = 0X3F,
};

enum
{
    EMPTY_CMD			= 0x00,
    EMPTY_ACK_CMD,
    
	MOUSE_CMD,
	MOUSE_ACK_CMD,

	KB_CMD,
	KB_ACK_CMD,
    
};

typedef struct
{
    u8 btn;
#if (MOUSE_DATA_LEN_AAA==6)
    s16 x;
    s16 y;
#elif (MOUSE_DATA_LEN_AAA==4)
    s8 x;
    s8 y;
#endif
    s8 wheel;
} mouse_data_t;
typedef struct
{
	u32 did;

	u8 key[12];
}AES_KEY;

typedef struct
{
    u32 dma_len;

    u8  rf_len; 
    u8 dat[59];

} rf_pcket_t;


typedef struct
{
	u8  head_0;
	u8  head_1;
	u8  head_2;
	u32 did;
	u8  cmd;
	u8  cmd_1;
	u8 km_dat[6];
	u8 rsv1[3];
} pair_data_t;

typedef struct
{	
	u8 cmd;//data type
	u8 seq_no;
	u8 pno_no;
	
	u32 did;
#if PROTOCOL_CUSTOMER
	u8 key[11];
#else
	u8 key[12];
#endif
} pair_data_t_1;

typedef struct
{	
	u8 head0;
	u8 head1;
	u8 head2;
	u32 did;
	u8 cmd;
	u8 sw_version[3];
} pair_ack_data_t;

typedef struct
{
	u8  head_0;
	u8  head_1;
	u8  head_2;
	u32 did;
	u8  cmd;
	u8  cmd_1;
	u8 km_dat[6];
	u8 rsv1[3];
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


	
}Pair_status;


extern void set_pair_access_code(u32 code);
extern void rf_stx_to_rx(u8 *p, u32 rx_timeout_us);
extern void my_rf_start_srx (unsigned int tick,unsigned int rx_timeout);
extern void rf_drv_private_2m_init();
volatile extern u8 host_channel;

/*
typedef struct
{	
	u8 cmd;//data type
	u8 seq_no;
	u8 pno_no;
	
	u32 did;
	u8 key[12];

} pair_data_t;

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
	u8	pn_no;//bit7=0: no aes  =1: aes
	
	u32 did;

	u8 km_dat[6];//mouse data or kb
	u8  rsv1[3]; //for aes  16 bytes
	
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
*/

#endif /* LED_RF_FRAME_H_ */

