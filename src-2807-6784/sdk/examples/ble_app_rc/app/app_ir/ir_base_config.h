/*******************************************************************************************
*                    Copyright (c) 2021, XIHAO Technology Co., Ltd.
* @file        ir_base_config.h
* @author      Reborn    
* @version     V1.0    
* @date        2020-12-30
* @brief       红外发送处理文件
*********************************************************************************************/

#ifndef __IR_BASE_CONFIG_H
#define __IR_BASE_CONFIG_H

#define	IR_PWM_MAX_CNT 			150		//最长缓存波形

#define LOW_LEVEL_STATUS		0		//低电平状态
#define HIGH_LEVEL_STATUS		1		//高电平状态

#define IR_SEND_DATA_TYPE		1
#define IR_SEND_REPEAT_TYPE		0

/**波形长度及高低电平*/
typedef struct 
{
	unsigned int level_time;			/**波形时间*/
	unsigned char level_feature;		/**波形状态(载波/无载波)*/
}FRAME_BUF_LEVEL_T;

/**红外发送参数结构体*/
typedef struct
{
    FRAME_BUF_LEVEL_T ir_first_frame_buff[IR_PWM_MAX_CNT];	/**第一帧数据*/
    FRAME_BUF_LEVEL_T ir_secend_frame_buff[IR_PWM_MAX_CNT]; /**第二帧数据*/
	unsigned char ir_first_frame_len;	/**第一帧长度*/
	unsigned char ir_secend_frame_len;	/**第二帧长度*/
	unsigned int ir_first_frame_time;	/**第一帧数据时间*/
	unsigned int ir_secend_frame_time;	/**第二帧数据时间*/
    unsigned int ir_carrier_fre;	/**载波频率*/
    unsigned char repeat_type;	/**重复类型*/
	unsigned char ir_reverse_flag;	/**反转标志*/
}IR_DATA_T;


/**红外发送参数结构体*/
typedef struct
{
    unsigned char ir_carrier_type;			/**红外载波类型*/
	unsigned char ir_send_status;			/**红外发送状态*/
	unsigned char ir_learn_status;			/**红外学习状态*/
	unsigned char repeat_type;              /**重复类型*/
	unsigned char ir_key_index;             /**红外当前按键索引*/
	unsigned char ir_send_data_type;
	
	unsigned int first_frame_buff[IR_PWM_MAX_CNT];
	unsigned int secend_frame_buff[IR_PWM_MAX_CNT];
	unsigned char first_frame_len;
	unsigned char secend_frame_len;

	unsigned int ir_frame_min;
	unsigned int ir_frame_cnt;
	unsigned char ir_release_status;
	
	
} IR_SEND_CNT_T;

#endif //__IR_BASE_CONFIG_H

