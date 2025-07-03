#include "pm_handle.h"
#include "keyboard.h"
#include "driver.h"
#include "key_driver.h"
#include "function.h"

/******************************************************************
 * @brief  kb_wake_config(void)
 * @details Set sleep pin
 * @param  none
 * @return none
 * @retval void
 ******************************************************************/
volatile u32 dev_wake_time_tick = 0;
bool led_pwm_lighting_off_flag = false;

void key_pressed_check_event(void)
{
	u8 len = app_custom_get_drive_len();
	for (int i = 0; i < len; i++)
	{
		if (gpio_read(drive_pins[i]))
		{
			key_cur_status = KEY_PRESSED;
		}
	}
}


/******************************************************************
 * @brief  dev_wake_function()
 * @details Device sleep function, add dev_wake_time_tick=clock_time; And set the timestamp delay duration
 * @param  none
 * @return none
 * @retval void
 ******************************************************************/

void device_sleep_event()
{
	if (g_key_board.stuck_flag == false)
	{
		cpu_sleep_wakeup(DEEPSLEEP_MODE_RET_SRAM_LOW16K, PM_WAKEUP_PAD, 0);
	}
#if STUCK_KEY_COMFIGURATION
	else if (g_key_board.stuck_flag == true)
	{
		Stuck_sleep_wakeup_event();
	}
#endif
}

void dev_wake_function()
{
#if DEVICE_WACK_ENABLE
	if (set_fun_filter_element_flag == false)
	{
		if (led_pwm_lighting_off_flag == true && (led_pwm_lighting_duration_3s_tick && clock_time_exceed(led_pwm_lighting_duration_3s_tick, SLEEP_TIME_VALUE * 1000 * 1000)))
		{
			led_pwm_all_off();
			led_pwm_lighting_off_flag = false;
			led_pwm_lighting_duration_3s_tick = 0;
		}
		else if (led_pwm_lighting_duration_3s_tick && clock_time_exceed(led_pwm_lighting_duration_3s_tick, SLEEP_TIME_VALUE * 1000 * 1000))
		{
			led_pwm_all_off();
			led_pwm_lighting_duration_3s_tick = 0;
			led_pwm_lighting_off_flag = true;
		}
		else
		{
			if (long_press_3s_s2_pair == false)
				if (((dev_wake_time_tick && clock_time_exceed(dev_wake_time_tick, SLEEP_TIME_VALUE * 1000 * 1000))) ||
					(led_pwm_lighting_off_flag == true && led_pwm_lighting_duration_3s_tick == 0) || send_device_mac_flag == true)
				{
					if (key_cur_status == KEY_RELEASED)
					{
						// 设备进入休眠
						printf("--->TIME_DEVICE_GOES_TO_SLEEP<---\r\n");
						device_sleep_event();
					}
				}
		}
	}

	if (led_pwm_lighting_off_time && clock_time_exceed(led_pwm_lighting_off_time, 5 * 1000 * 100))
	{
		led_pwm_all_off();
		if (key_cur_status == KEY_RELEASED)
		{
			printf("--->LED_GOES_TO_SLEEP<---\r\n");
			device_sleep_event();
		}
	}

	
	//控制亮灯开光的时间
	led_contral_on_off();
	

	if (set_fun_filter_element_time && clock_time_exceed(set_fun_filter_element_time, 10 * 1000 * 1000))
	{
		led_pwm_all_off();
		if (key_cur_status == KEY_RELEASED)
		{
			printf("--->HIDE_GOES_TO_SLEEP<---\r\n");
			device_sleep_event();
		}
	}
#endif
}

/******************************************************************
 * @brief  dev_now_wake_function(void)
 * @details 设备立即睡眠
 * @param  none
 * @return none
 * @retval void
 ******************************************************************/
void dev_now_wake_function()
{
#if DEVICE_WACK_ENABLE
	led_pwm_all_off();
	if (key_cur_status == KEY_RELEASED)
	{
		printf("--->NOW_DEVICE_GOES_TO_SLEEP<---\r\n");
		device_sleep_event();
	}

#endif
}
