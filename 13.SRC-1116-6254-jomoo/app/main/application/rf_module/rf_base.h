#ifndef __RF_BASE_H__
#define __RF_BASE_H__

#include "help_tools.h"
#include <types.h>



typedef struct
{
	u32 dma_len;
	u8 rf_len;
	u8 dat[59];

} rf_packet_t;


typedef struct
{
    u32 dma_len;
	u8  rf_len;
	u8  head_0;
	u8  head_1;
	u8  head_2;
	u32 did;
	u8  cmd;
	u8  cmd_1;
	u8  km_dat[6];
	u8  rsv1[3];

}rf_send_dat;

typedef struct
{
	u8 head_0;
	u8 head_1;
	u8 head_2;
	u32 did;
	u8 cmd;
	u8 cmd_1;
	u8 km_dat[6]; // mouse data or kb
	u8 rsv1[3];	  // for aes  16 bytes
}bath_data_t;

typedef struct
{
//    rf_send_dat pair_data;      //广播数据包
//    rf_send_dat cmd_data;       //按键指令数据包
   rf_send_dat data_packet;    //数据包
}Rf_type;

typedef struct
{
   u8 choose:2;      //当前信道位置
   u8 list[4];       //存储信道列表
}Channel_list;


extern Rf_type rf_type;
extern _attribute_data_retention_ Channel_list channel_pipo;




#endif