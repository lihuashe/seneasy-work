#include "app_mode.h"
#include "peripheral.h"
#include "key_scan.h"
#include "app_gap.h"
#include "app_keyboard.h"
#include "app_led.h"
#include "hid_service.h"
#if (REMOTE_IR_ENABLE)
#include "app_ir.h"
#endif

#define APP_LOG_DOMAIN      "amode"
#define APP_LOG_LEVEL       APP_MODE_LOG_LEVEL
#include "co_debug.h"



static co_timer_t remote_mode_base_timer;
static co_timer_t remote_key_hold_timer;


/******************remote task handle**************/
#define REMOTE_BASE_TICK 					10


void remote_mode_tick_handle(co_timer_t *timer, void *param)
{
	g_board_status.remote_mode_base_tick += REMOTE_BASE_TICK;
	remote_mode_loop_task();
	//APP_LOG_INF("tick test:%d",app_env.remote_mode_base_tick);
}

unsigned int remote_clock(void)
{
	return g_board_status.remote_mode_base_tick;
}

unsigned int remote_clock_exceed(unsigned int ref, unsigned int ms)
{
	return ((unsigned int)(g_board_status.remote_mode_base_tick - ref) > ms);
}


/**
 * @brief      获取RCU特殊模式
 * @param[in]  none
 * @return     none
 */
unsigned char get_remote_mode(void)
{
	return g_board_status.remote_mode;
}

/**
 * @brief      获取RCU特殊模式时间
 * @param[in]  none
 * @return     none
 */
unsigned int get_remote_mode_tick(void)
{
	return g_board_status.remote_mode_tick;
}


/**
 * @brief      RCU特殊模式设置
 * @param[in]  remote_mode-rcu 模式
 * @return     none
 */
void set_remote_mode(unsigned char remote_mode)
{
	g_board_status.remote_mode = remote_mode;
	g_board_status.remote_mode_tick = g_board_status.remote_mode_base_tick;
	APP_LOG_INF("remote set mode:%d\r\n",remote_mode);
}

/**
 * @brief      RCU特殊模式触发接收回调
 * @param[in]  remote_mode-rcu hold 完触发的模式
 * @param[in]  mode_hold_tick-rcu hold 时间
 * @return     none
 */
static void remote_mode_trigger_finish(co_timer_t *timer, void *param)
{
	//LOG_DBG_FUNC();

	switch (g_board_status.remote_hold_mode)
	{
		case REMOTE_PAIR_HOLD:
		{
			set_remote_mode(REMOTE_PAIR_START);
		}
		break;
		
		default:
			break;
	}

	if (get_remote_mode() != REMOTE_IDLE)
	{
				remote_sleep_disable(REMOTE_ENTER_MODE,true); //禁止休眠
		g_board_status.remote_mode_base_tick = 0;
		co_timer_set(&remote_mode_base_timer, REMOTE_BASE_TICK, TIMER_REPEAT,
				 remote_mode_tick_handle, NULL);

	}

	g_board_status.remote_hold_mode = REMOTE_HOLD_IDLE;
}


/**
 * @brief		遥控器模式hold触发
 * @param[in]	none
 * @return      none
 */
void remote_mode_hold_trigger(void)
{	
    /*按键发生改变,如果没有进入特殊模式,初始化特殊模式*/
	if (g_board_status.remote_hold_mode != REMOTE_HOLD_IDLE)
	{
		co_timer_del(&remote_key_hold_timer);
		g_board_status.remote_hold_mode = REMOTE_HOLD_IDLE;
	}
	
	//卡键过程中,任何模式不能触发
	if(g_keyboard_ctl.stuck_flag || g_board_status.ble_low_protect)
	{
		return;
	}

	//语音过程中,任何模式都不能触发
	if (g_board_status.ui_mic_enable)
	{
		APP_LOG_INF("mic open ,not switch rcu mode\r\n");
		return;
	}
	
	if (check_pair_comb_key())
	{	
		g_board_status.rcu_reconnecting_count = 0;
		g_board_status.remote_hold_timer = PAIR_HOLD_TIMEOUT;
		g_board_status.remote_hold_mode = REMOTE_PAIR_HOLD;
	}	
	else
	{
		;//note
	}

	if (g_board_status.remote_hold_mode != REMOTE_HOLD_IDLE)
	{
		co_timer_set(&remote_key_hold_timer, g_board_status.remote_hold_timer, TIMER_ONE_SHOT,
                     remote_mode_trigger_finish, NULL);
	}
}


/**
 * @brief		遥控器模式loop 分发
 * @param[in]	none
 * @return      none
 */
void remote_mode_key_task(void)
{
	unsigned char remote_mode = get_remote_mode();

	if (remote_mode >= REMOTE_PAIR_START && remote_mode <= REMOTE_PAIR_END)
	{
		remote_pair_key_task(remote_mode);
	}
}

/**
 * @brief		遥控器模式loop 分发
 * @param[in]	none
 * @return      none
 */
void remote_mode_loop_task(void)
{
	unsigned char remote_mode = get_remote_mode();

	if (remote_mode >= REMOTE_PAIR_START && remote_mode <= REMOTE_PAIR_END)
	{
		remote_pair_handle(remote_mode);
	}	
	else if (remote_mode == REMOTE_TASK_FINISH)
	{
        //清除remote loop任务
		remote_sleep_disable(REMOTE_ENTER_MODE,false); //进入休眠
		set_remote_mode(REMOTE_IDLE);
		co_timer_del(&remote_mode_base_timer);
	}
    else
    {
        ;//none
    }
}

