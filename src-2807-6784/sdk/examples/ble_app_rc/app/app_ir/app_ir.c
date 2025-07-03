#include "app_mode.h"
#include "peripheral.h"
#include "pinmux.h"
#include "key_scan.h"
#include "app_gap.h"
#include "app_keyboard.h"
#include "app_led.h"
#include "app_ir.h"
#include "driver_nec_code.h"
#include "irtx.h"
#include "irrx.h"

#define APP_LOG_DOMAIN      "air"
#define APP_LOG_LEVEL       APP_IR_LOG_LEVEL
#include "co_debug.h"

#if (REMOTE_IR_ENABLE)
IR_DATA_T g_ir_data;			//红外发送控制

IR_SEND_CNT_T g_ir_send_cnt;	//红外发送时的参数

static co_timer_t ir_send_dtype_timer;	//红外发送数据类型定时

static void default_ir_send(uint32_t ir_code, uint32_t min_cnt);
static void ir_send_release(void);
void send_data_start_timer(unsigned char ms);
static void remote_send_data_finish(co_timer_t *timer, void *param);
void ir_frame_fifo_change(uint8_t carr_type, FRAME_BUF_LEVEL_T* src_frame, uint8_t src_frame_len, uint32_t* dest_frame, uint8_t* dest_frame_len, uint32_t carrier_fre);
void irtx_callback(void);

static unsigned char learn_key_index;

/**
 * @brief		默认红外码发送
 * @param[in]	ir_code-红外码值
 */
static void default_ir_send(uint32_t ir_code, uint32_t min_cnt)
{
	if (g_ir_send_cnt.ir_learn_status  == IR_LEARN_STATUS_BUSY || g_ir_send_cnt.repeat_type != IR_SENDING_NONE)
	{
		return;
	}
	APP_LOG_INF_FUNC();
	
	//停止上一次的发码
	ir_tx_stop();
	pinmux_config(PIN_IR_CONTROL, PINMUX_IRTX_OUT_CFG);
	
	//解码
    ir_nec_decode(((DEFAULT_NEC_USER_CODE & 0xFF00) >> 8),(DEFAULT_NEC_USER_CODE & 0xFF), ir_code,&g_ir_data);
	
	//设置当前发送索引,第一帧长度,重复类型,数据buff,红外pwm状态
	g_ir_send_cnt.repeat_type = g_ir_data.repeat_type;
	g_ir_send_cnt.ir_carrier_type = CARRIER_TYPE;
	
	g_ir_send_cnt.first_frame_len = 0;
	g_ir_send_cnt.secend_frame_len = 0;
	
	g_ir_send_cnt.ir_frame_min = min_cnt;
	g_ir_send_cnt.ir_frame_cnt = 0;
	g_ir_send_cnt.ir_release_status	= 0;	
	
	ir_frame_fifo_change(g_ir_send_cnt.ir_carrier_type,g_ir_data.ir_first_frame_buff,g_ir_data.ir_first_frame_len,
	g_ir_send_cnt.first_frame_buff,&g_ir_send_cnt.first_frame_len,g_ir_data.ir_carrier_fre);
	ir_frame_fifo_change(g_ir_send_cnt.ir_carrier_type,g_ir_data.ir_secend_frame_buff,g_ir_data.ir_secend_frame_len,
	g_ir_send_cnt.secend_frame_buff,&g_ir_send_cnt.secend_frame_len,g_ir_data.ir_carrier_fre);
	
	pmu_lowpower_prevent(PMU_LP_USER);
	
	
    ir_tx_config_t config = {
        .mode           = FIFO,
        .carrier_freq0  = g_ir_data.ir_carrier_fre,
        .duty_cycle0    = (double)1 / 3,
        .carrier_freq1  = 50000,
        .duty_cycle1    = (double)1 / 2,
        .data_src       = g_ir_send_cnt.first_frame_buff,
        .data_num       = g_ir_send_cnt.first_frame_len,
        
        .bUse_intr      = true,
        .fifo_trigger_lvl = 5,
        .fifo_cnt_intr_mask = FIFO_EMPTY_INTR_MASK,  //IR_TX有5个中断源。根据选择配置
        .callback[4]    = irtx_callback,
        
    };
	
	ir_tx_config(config);
	
	ir_tx_start();
	send_data_start_timer(g_ir_data.ir_first_frame_time/1000);
	g_ir_send_cnt.ir_send_status = IR_SEND_STATUS_BUSY;
}


/**
 * @brief		红外释放
 */
static void ir_send_release(void)
{
	if (g_ir_send_cnt.ir_send_status == IR_SEND_STATUS_BUSY)
	{
		if(g_ir_send_cnt.ir_frame_cnt >= g_ir_send_cnt.ir_frame_min)
		{
			if (g_ir_send_cnt.repeat_type == IR_SENDING_NONE || g_ir_send_cnt.ir_send_data_type == IR_SEND_REPEAT_TYPE)
			{
				ir_tx_stop();
				remote_sleep_disable(IR_SEND_SLEEP_TYPE,false);
				g_ir_send_cnt.ir_send_status = IR_SEND_STATUS_IDLE;
				APP_LOG_INF("ir send release\r\n");
			}
			g_ir_send_cnt.repeat_type = IR_SENDING_NONE;					
		}
		else
		{
			g_ir_send_cnt.ir_release_status = 1;
		}

	}
}


/**
 * @brief		红外电平组包
 * @param[in]	level_status-电平状态
 * @param[in]	level_features-帧
 * @param[in]	level_value-电平值
 * @param[out]	out_ir_data-输出组包数据
 */
void levle_value_package_handle(unsigned char level_status,unsigned char level_features,unsigned int level_value,IR_DATA_T *out_ir_data)
{	
	switch(level_features)
	{
		case FIRST_FRAME_DATA_LEVEL:
			if(out_ir_data->ir_first_frame_len != 0 
				&& out_ir_data->ir_first_frame_buff[out_ir_data->ir_first_frame_len-1].level_feature == level_status)
			{
				out_ir_data->ir_first_frame_buff[out_ir_data->ir_first_frame_len-1].level_time += level_value;
			}
			else
			{
				out_ir_data->ir_first_frame_buff[out_ir_data->ir_first_frame_len].level_time = level_value;
				out_ir_data->ir_first_frame_buff[out_ir_data->ir_first_frame_len].level_feature = level_status;
				out_ir_data->ir_first_frame_len += 1;
			}
			 
			out_ir_data->ir_first_frame_time += level_value;
			break;
		case SECOND_FRAME_DATA_LEVEL:
			 if(out_ir_data->ir_secend_frame_len != 0 
				 && out_ir_data->ir_secend_frame_buff[out_ir_data->ir_secend_frame_len-1].level_feature == level_status)
			 {
				 out_ir_data->ir_secend_frame_buff[out_ir_data->ir_secend_frame_len-1].level_time += level_value;
			 }
			 else
			 {
				out_ir_data->ir_secend_frame_buff[out_ir_data->ir_secend_frame_len].level_time = level_value;
				out_ir_data->ir_secend_frame_buff[out_ir_data->ir_secend_frame_len].level_feature = level_status;
				out_ir_data->ir_secend_frame_len += 1;
			 }
			 out_ir_data->ir_secend_frame_time += level_value;
			break;
		case FIRST_FRAME_STOP_LEVEL:
			if (out_ir_data->ir_first_frame_len != 0 && 
				 out_ir_data->ir_first_frame_buff[out_ir_data->ir_first_frame_len-1].level_feature == level_status)
			{
				//out_ir_data->ir_first_frame_time -= out_ir_data->ir_first_frame_buff[out_ir_data->ir_first_frame_len-1].level_time;
				out_ir_data->ir_first_frame_buff[out_ir_data->ir_first_frame_len-1].level_time += level_value;
			}
			else
			{
				out_ir_data->ir_first_frame_buff[out_ir_data->ir_first_frame_len].level_time = level_value;
				out_ir_data->ir_first_frame_buff[out_ir_data->ir_first_frame_len].level_feature = level_status;
				out_ir_data->ir_first_frame_len += 1;
			}
			break;
		case SECOND_FRAME_STOP_LEVEL:
			if (out_ir_data->ir_secend_frame_len != 0 && 
			out_ir_data->ir_secend_frame_buff[out_ir_data->ir_secend_frame_len-1].level_feature == level_status)
			{
				out_ir_data->ir_secend_frame_buff[out_ir_data->ir_secend_frame_len-1].level_time += level_value;
			}
			else
			{
				out_ir_data->ir_secend_frame_buff[out_ir_data->ir_secend_frame_len].level_time = level_value;
				out_ir_data->ir_secend_frame_buff[out_ir_data->ir_secend_frame_len].level_feature = level_status;
				out_ir_data->ir_secend_frame_len += 1;
			}
			break;
		default :
			break;
	}
}


void irtx_callback(void)
{	
  if(g_ir_send_cnt.ir_send_status == IR_SEND_STATUS_BUSY)
  {
	 if(g_ir_send_cnt.repeat_type == FIRST_FRAME_REPEAT) 
		{
			g_ir_send_cnt.ir_frame_cnt++;
			if(g_ir_send_cnt.ir_release_status && g_ir_send_cnt.ir_frame_cnt >= g_ir_send_cnt.ir_frame_min)
			{
				ir_tx_stop();
				remote_sleep_disable(IR_SEND_SLEEP_TYPE,false);
				g_ir_send_cnt.ir_send_status = IR_SEND_STATUS_IDLE;
				APP_LOG_INF("ir send release\r\n");
				g_ir_send_cnt.repeat_type = IR_SENDING_NONE;	
				return;
			}
			send_data_start_timer(g_ir_data.ir_first_frame_time/1000);	
			ir_tx_start();
		}
		else if (g_ir_send_cnt.repeat_type == SECOND_FRAME_REPEAT)
		{
			g_ir_send_cnt.ir_frame_cnt++;
			if(g_ir_send_cnt.ir_release_status && g_ir_send_cnt.ir_frame_cnt >= g_ir_send_cnt.ir_frame_min)
			{
				ir_tx_stop();
				remote_sleep_disable(IR_SEND_SLEEP_TYPE,false);
				g_ir_send_cnt.ir_send_status = IR_SEND_STATUS_IDLE;
				APP_LOG_INF("ir send release\r\n");
				g_ir_send_cnt.repeat_type = IR_SENDING_NONE;	
				return;
			}
			
			ir_tx_set_src(g_ir_send_cnt.secend_frame_buff,g_ir_send_cnt.secend_frame_len);
			send_data_start_timer(g_ir_data.ir_secend_frame_time/1000);	
			ir_tx_start();
		}
    else  
    {
			ir_send_release();
    }
	}
}


static void remote_send_data_finish(co_timer_t *timer, void *param)
{
	APP_LOG_DBG_FUNC();
	co_timer_del(&ir_send_dtype_timer);
	g_ir_send_cnt.ir_send_data_type = IR_SEND_REPEAT_TYPE;
}

void send_data_start_timer(unsigned char ms)
{
	co_timer_set(&ir_send_dtype_timer, ms+1, TIMER_ONE_SHOT,
				 remote_send_data_finish, NULL);
	g_ir_send_cnt.ir_send_data_type = IR_SEND_DATA_TYPE;
}


void ir_frame_fifo_change(uint8_t carr_type, FRAME_BUF_LEVEL_T* src_frame, uint8_t src_frame_len, uint32_t* dest_frame, uint8_t* dest_frame_len, uint32_t carrier_fre)
{
	int start_index = -1;

	for (int i=0;i<src_frame_len;i++)
	{
		if (src_frame[i].level_feature == HIGH_LEVEL_STATUS)
		{
			start_index = i;
			APP_LOG_INF("src_frame_len:%d,start_index:%d\r\n",src_frame_len,start_index);
			break;
		}
	}
	
	if (*dest_frame_len != 0)
	{
		APP_LOG_ERR("dest first len err\r\n");
		return;
	}
	
	//第一个电平要以载波开始
	if (start_index < 0)
	{
		APP_LOG_ERR("first level err\r\n");
		return;
	}
	
	if ((src_frame_len-start_index)%2 != 0)
	{
		APP_LOG_ERR("level len err:%d\r\n",src_frame_len-start_index);
		return;
	}
	
	printf("carr ir:%d\r\n",carrier_fre);	
	printf("send ir:");	
	for (int i=0; i<src_frame_len; i++)
	{
		printf("%d ",src_frame[i].level_time);
	}
	
	//第一个电平要以载波开始,电平长度要偶数
	for (int i=start_index; i<src_frame_len; i += 2)
	{
		unsigned short carrier_level = 0;
		unsigned short uncarrier_level = 0;
		
		if (carr_type == CARRIER_TYPE)
		{
			 carrier_level = (src_frame[i].level_time*carrier_fre)/1000000;
			 uncarrier_level = (src_frame[i+1].level_time*carrier_fre)/1000000;
		}
		else
		{
			carrier_level = 0x01;
			uncarrier_level = (src_frame[i+1].level_time*carrier_fre)/1000000;
		}
		
		dest_frame[*dest_frame_len] = (0x8000|carrier_level)|(0x7FFF0000&(uncarrier_level<<16));
		*dest_frame_len += 1;
		
		//APP_LOG_INF("dest_frame[%d]:%x\r\n",*dest_frame_len-1,dest_frame[*dest_frame_len-1]);
	}
	
	
	APP_LOG_INF("fifo_change success\r\n");
}
	
/**
 * @brief		红外码发送
 * @param[in]	send_type: 发码类型
 * @param[in]	key_indx : 按键索引
 * @param[in]	syscode  : 默认客户码
 * @param[in]	data_code: 数据码
 * @param[in]	key_type : SSU按键类型
 * @param[in]	 temp_learn_key_info ：手动红外学习标志信息
 */
void ir_dispatch(unsigned char send_type, unsigned char key_index, unsigned short data_code, unsigned char key_type,int group_num)
{
	#if (LOW_BATT_PRODECT_ENABLE)
	if (g_board_status.ir_low_protect)
	{
		return;
	}	
	#endif
	
	APP_LOG_INF("ir_send_type:%d\r\n",send_type);
	if(send_type == TYPE_DEFAULT_IR_SEND)
	{
		default_ir_send(data_code,0);
	}
	else if(send_type == TYPE_IR_RELEASE)
	{
		ir_send_release();
	}
}


void app_ir_init(void)
{
	pinmux_config(PIN_IR_CONTROL, PINMUX_IRTX_OUT_CFG);
	
}

#endif