#ifndef __KEY_IR_H
#define __KEY_IR_H

#include "board_config.h"
#include "ir_base_config.h"
#include "peripheral.h"

#define TYPE_DEFAULT_IR_SEND	1	//发送红外默认码
#define TYPE_RUR_LEARN_SEND     2	//发送红外rur码
#define TYPE_MANU_LEARN_SEND    3	//发送红外学习码
#define TYPE_VERSATILE_SEND		4	//发送万能码
#define TYPE_IR_RELEASE			5	//发送释放码


#define 	IR_INTERRUPT_STATUS_IDLE	0
#define 	IR_INTERRUPT_STATUS_START	1
#define		IR_INTERRUPT_STATUS_WAIT	2
		
		
#define		MAX_CARRIER_INTERVAL	60		//最大载波间隔60us
#define 	MIN_CARRIER_INTERVAL	14		//最小载波间隔14us
#define 	CARRIER_CACH_LEN		256		//256倍数,容易保存
#define 	LERANDATABUFMAX 		3000	//最大学习数据缓存buff

/*载波类型*/
enum {
	CARRIER_TYPE = 1,
	UNCARRIER_TYPE,
};


/**红外重复类型*/
enum
{
	IR_SENDING_NONE = 0,
	FIRST_FRAME_REPEAT,							/**一帧重复*/
	SECOND_FRAME_REPEAT,						/**第两帧重复*/
	SECOND_FRAME_REPEAT_FIRST_STOP,				/**第二帧开始重复,松手第一帧*/
};


#define IR_SEND_STATUS_IDLE			0	/**红外发送状态*/
#define IR_SEND_STATUS_BUSY			1

#define IR_LEARN_STATUS_IDLE		0	/**红外学习状态*/
#define IR_LEARN_STATUS_BUSY			1	



/**组包数据类型*/
enum
{
	FIRST_FRAME_DATA_LEVEL = 0,	/**第一帧数据*/
	SECOND_FRAME_DATA_LEVEL,	/**第二帧数据*/
	FIRST_FRAME_STOP_LEVEL,		/**第一帧停止*/
	SECOND_FRAME_STOP_LEVEL,	/**第二帧停止*/
};

#if (IR_LEARN_EN)
#define CCR_MAX 			0xFFFF
#define CARRIER_CACH_LEN				256
#define CARRIER_CAL_MAX			30

/**红外学习相关的结构体*/
typedef struct {
	uint32_t ir_data_buf[CARRIER_CACH_LEN];		/**保存将红外学习来的时间间隔保存到该数据中*/
    uint16_t data_index;						/**学习间隔个数*/
	uint32_t ir_learn_data_buf[CARRIER_CAL_MAX];	/**ir脉冲间隔缓存*/
	uint16_t ir_learn_data_buf_cnt;					/**ir脉冲间隔缓存个数*/
	uint32_t pwm_high_cnt_last;
	uint32_t pwm_low_cnt_last;
	uint32_t last_irrx_data;
	uint32_t recv_count_cnt;
	uint32_t pwm_period;    //the number of system tick of a pwm period
//	uint16_t pwm_interrupt_cnt;
//	uint32_t pwm_cnt;    //pwm time
//	uint32_t low_cnt;    //low level time
//  uint16_t pwm_duty_cnt;      //the number of system tick of pwm duty
} TYPEDEFIRLEARN;




bool check_ir_learn_comb_key(void);
void ir_learn_key_task(unsigned char remode_mode);
void ir_learn_handle(unsigned char remode_mode);
#endif //IR_LEARN_EN



extern IR_DATA_T g_ir_data;
extern IR_SEND_CNT_T g_ir_send_cnt;



void app_ir_init(void);
void ir_dispatch(unsigned char send_type, unsigned char key_index, unsigned short data_code, unsigned char key_type,int group_num);
void levle_value_package_handle(unsigned char level_status,unsigned char level_features,unsigned int level_value,IR_DATA_T *out_ir_data);



#if (REMOTE_IR_ENABLE)
#define ir_default_code_send(key_index,data_code)	ir_dispatch(TYPE_DEFAULT_IR_SEND,key_index,data_code,0,-1)	//发送默认的红外码
#define ir_release_send()							ir_dispatch(TYPE_IR_RELEASE,0,0,0,-1)	//发送释放码
#define ir_learn_cod_send(key_index) ir_dispatch(TYPE_MANU_LEARN_SEND,key_index,0,0,-1)		//发送学习码
#define ir_versatle_default_send(key_index)			ir_dispatch(TYPE_VERSATILE_SEND,key_index,0,0,-1)	//发送默认万能码
#define ir_versatle_choose_send(key_index,group_num)	ir_dispatch(TYPE_VERSATILE_SEND,key_index,0,0,group_num)	//发送指定万能码
#define ir_act_code_send(key_index)			ir_dispatch(TYPE_RUR_LEARN_SEND,key_index,0,0,-1)	//发送act
#else
#define ir_default_code_send(key_index,syscode,data_code)
#define ir_release_send()
#define ir_learn_cod_send(key_index,learn_key_info)
#endif //REMOTE_IR_ENABLE



#endif //__KEY_IR_H
