#include "peripheral.h"
#include "app_batt.h"
#include "batt_service.h"
#include "app_keyboard.h"

#if (REMOTE_IR_ENABLE)
#include "app_ir.h"
#endif

#define APP_LOG_DOMAIN      "batt"
#define APP_LOG_LEVEL       APP_BATT_LOG_LEVEL
#include "co_debug.h"

static uint8_t get_batt_level(void);

static uint8_t get_batt_level(void)
{
    uint16_t result = BATT_DETECT_MAX_LEVEL;

	result = adc_voltage_read_by_channel(ADC_CH_1, 3)+5; //参数2 0/1/2 采样率 
	
	APP_LOG_INF("vbat:%d\r\n",result);

	g_board_status.batt_mv = result;
	
	g_board_status.batt_val_sum += result;
	
    if(result<BATT_DETECT_MIN_LEVEL) result = BATT_DETECT_MIN_LEVEL;
    if(result>BATT_DETECT_MAX_LEVEL) result = BATT_DETECT_MAX_LEVEL;

		
	if ((++g_board_status.batt_det_cnt) >= BATT_DET_MAX_CNT)
	{
		if (g_board_status.batt_val_sum/g_board_status.batt_det_cnt <= LOW_BATT_PROMPT_LEVEL)
		{
			APP_LOG_INF("LOW_BATT_PROMPT_LEVEL:%d\r\n",g_board_status.batt_val_sum/g_board_status.batt_det_cnt);
			g_board_status.low_batt_status = 1;
		}
		else if (g_board_status.batt_val_sum/g_board_status.batt_det_cnt > LOW_BATT_PROMPT_LEVEL+50)
		{
			APP_LOG_INF("LOW_BATT_PROMPT_LEVEL:%d\r\n",g_board_status.batt_val_sum/g_board_status.batt_det_cnt);
			g_board_status.low_batt_status = 0;
		}

		if (g_board_status.batt_val_sum/g_board_status.batt_det_cnt <= LOW_BATT_IR_PRODECT_LEVEL)
		{
			g_board_status.ir_low_protect = 1;
		}
		else if (g_board_status.batt_val_sum/g_board_status.batt_det_cnt > LOW_BATT_IR_PRODECT_LEVEL)
		{
			g_board_status.ir_low_protect = 0;
		}
		
		if (g_board_status.batt_val_sum/g_board_status.batt_det_cnt <= LOW_BATT_BLE_PRODECT_LEVEL)
		{
			g_board_status.ble_low_protect = 1;
		}
		else if (g_board_status.batt_val_sum/g_board_status.batt_det_cnt > LOW_BATT_BLE_PRODECT_LEVEL)
		{
			g_board_status.ble_low_protect = 0;
		}

		g_board_status.batt_det_cnt = 0;
		g_board_status.batt_val_sum = 0;
	}
	
    return (result - BATT_DETECT_MIN_LEVEL)/10;
}

/**
 * @brief        检测电池处理
 */
void batt_detect_handle(void)
{
	if (
	#if (REMOTE_IR_ENABLE) //红外发送过程中不能检测电池电量
		g_ir_send_cnt.ir_send_status == IR_SEND_STATUS_IDLE && 
		g_ir_send_cnt.ir_learn_status == IR_LEARN_STATUS_IDLE &&	
	#endif
		!g_board_status.ui_mic_enable)
	{
		g_board_status.batt_cur_val =  get_batt_level();
		APP_LOG_INF("g_board_status.batt_cur_val = %d\r\n",g_board_status.batt_cur_val);
		
	}
   
}

/**
 * @brief        电池检测初始化
 */
void app_batt_init(void)
{
	get_batt_level(); // 上电检测一次电池电量
	g_board_status.batt_det_cnt = 0;
	g_board_status.batt_val_sum = 0;
}

