/**
 * @file led_evt.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-03-19
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "led_evt.h"
#include "arr_utils.h"
#include "time_utils.h"

/*============================================================================*
 *                              Local Variables
 *============================================================================*/

static LedEventManager manager = {.len = 0};
#if LED_EVT_GRADIENT_EN == 1
static LedEventManager gradient_manager = {.len = 0};
#endif
/*============================================================================*
 *                              Global Variables
 *============================================================================*/

/*============================================================================*
 *                              Local Functions
 *============================================================================*/

/**
 * @brief
 *
 * @param id
 * @return - 返回匹配id的位置
 * 		   -1 - 没有匹配
 */
int __match_pos_byID(unsigned char id, LedEventManager *ledManeger)
{
	for (int i = 0; i < ledManeger->len; i++)
	{
		if (ledManeger->arr[i].id == id)
			return i;
	}
	return -1;
}

/*============================================================================*
 *                              Global Functions
 *============================================================================*/

/**
 * @brief 根据led_cfg设置led闪烁
 *        相同优先级会重复设置
 *
 * @param id led ID
 * @param led_cfg led闪烁配置
 * onTime_ms, 点亮时间
 * offTime_ms, 熄灭时间
 * repeatCount, 重复次数 LED_EVT_FOREVER
 * priority, 优先级	0x00 < 0x01 < 0x02 < 0x04 < 0x08 < 0x10 < 0x20 < 0x40 < 0x80
 * @return 0 - 成功
 * 		   1 - 参数不正确
 * 		   2 - 设置事件上限
 * 		   3 - 已存在优先级更高的事件
 */
int led_evt_setup(unsigned char id, LedConfig led_cfg)
{
	if (led_cfg.onTime_ms == 0 || led_cfg.repeatCount == 0)
		return 1;
	int index = 0;
#if LED_EVT_GRADIENT_EN == 1
	index = __match_pos_byID(id, &gradient_manager);
	// 渐变事件是否存在该led
	if (index != -1)
	{
		if (gradient_manager.arr[index].priority > led_cfg.priority)
		{
			return 3;
		}
		led_evt_reset(id);
	}
#endif
	index = __match_pos_byID(id, &manager);
	if (manager.len > LED_EVT_MAX_NUM - 1 && index == -1)
		return 2;

	if (index == -1)
	{
		index = manager.len;
		manager.len++;
	}
	else if (manager.arr[index].priority > led_cfg.priority)
	{
		return 3;
	}

	manager.arr[index].id = id;
	manager.arr[index].onTime_ms = led_cfg.onTime_ms;
	manager.arr[index].offTime_ms = led_cfg.offTime_ms;
	manager.arr[index].repeatCount = led_cfg.repeatCount;
	manager.arr[index].priority = led_cfg.priority;

	manager.arr[index].startTick = clock_time();
	led_on(id);

	return 0;
}
#if LED_EVT_GRADIENT_EN == 1
/**
 * @brief 设置led渐变事件
 *
 * @param id
 * @param led_cfg
 * onTime_ms, 点亮时间
 * offTime_ms, 熄灭时间
 * repeatCount, 无效
 * priority, 优先级	0x00 < 0x01 < 0x02 < 0x04 < 0x08 < 0x10 < 0x20 < 0x40 < 0x80
 *
 * @return 0 - 成功
 * 		   1 - 参数不正确
 * 		   2 - 设置事件上限
 * 		   3 - 已存在优先级更高的事件
 */
int led_evt_setup_gradient(unsigned char id, LedConfig led_cfg)
{
	if (led_cfg.onTime_ms < 100 || led_cfg.offTime_ms < 100)
		return 1;
	int index = __match_pos_byID(id, &manager);

	// 闪烁事件是否存在该led
	if (index != -1)
	{
		if (manager.arr[index].priority > led_cfg.priority)
		{
			return 3;
		}
		led_evt_reset(id);
	}

	index = __match_pos_byID(id, &gradient_manager);
	if (gradient_manager.len > LED_EVT_MAX_NUM - 1 && index == -1)
		return 2;

	if (index == -1)
	{
		index = gradient_manager.len;
		gradient_manager.len++;
	}
	else if (gradient_manager.arr[index].priority > led_cfg.priority)
	{
		return 3;
	}

	gradient_manager.arr[index].id = id;
	gradient_manager.arr[index].onTime_ms = led_cfg.onTime_ms;
	gradient_manager.arr[index].offTime_ms = led_cfg.offTime_ms;
	gradient_manager.arr[index].repeatCount = led_cfg.repeatCount;
	gradient_manager.arr[index].priority = led_cfg.priority;

	gradient_manager.arr[index].startTick = clock_time();
	if (led_cfg.onTime_ms > 100)
	{
		gradient_manager.arr[index].repeatCount = 1;
	}
	else if (led_cfg.offTime_ms > 100)
	{
		gradient_manager.arr[index].repeatCount = 0;
	}

	return 0;
}
#endif
/**
 * @brief 重置已配置闪烁的led，关闭led
 *
 * @param id led ID
 * @return 0 - 失败
 *         1 - 成功
 */
int led_evt_reset(unsigned char id)
{
	led_off(id);
	int index = __match_pos_byID(id, &manager);
	if (index != -1)
	{
		ARRAR_REMOVE_POS(manager.arr, manager.len, index, LedEvent);
		return 1;
	}
#if LED_EVT_GRADIENT_EN == 1
	index = __match_pos_byID(id, &gradient_manager);
	if (index != -1)
	{
		ARRAR_REMOVE_POS(gradient_manager.arr, gradient_manager.len, index, LedEvent);
		return 1;
	}
#endif
	return 0;
}

int led_evt_is_busy()
{
#if LED_EVT_GRADIENT_EN == 1
	return manager.len || gradient_manager.len;
#else
	return manager.len;
#endif
}

void led_evt_proc(void)
{
	for (int i = 0; i < manager.len; i++)
	{
		if (led_state(manager.arr[i].id))
		{
			if (clock_time_exceed(manager.arr[i].startTick, (manager.arr[i].onTime_ms)))
			{
				led_off(manager.arr[i].id);

				manager.arr[i].startTick += (manager.arr[i].onTime_ms);
			}
		}
		else
		{
			if (clock_time_exceed(manager.arr[i].startTick, (manager.arr[i].offTime_ms)))
			{
				if (manager.arr[i].repeatCount == 0xFF)
				{
					led_on(manager.arr[i].id);

					manager.arr[i].startTick += (manager.arr[i].offTime_ms);
				}
				else if (--manager.arr[i].repeatCount)
				{
					led_on(manager.arr[i].id);

					manager.arr[i].startTick += (manager.arr[i].offTime_ms);
				}
				else
				{
					ARRAR_REMOVE_POS(manager.arr, manager.len, i, LedEvent);
					i--;
				}
			}
		}
	}
#if LED_EVT_GRADIENT_EN == 1
	for (int i = 0; i < gradient_manager.len; i++)
	{
		int brightness = led_get_brightness(gradient_manager.arr[i].id);
		if (gradient_manager.arr[i].repeatCount == 1)
		{
			if (clock_time_exceed(gradient_manager.arr[i].startTick, (gradient_manager.arr[i].onTime_ms / 100)))
			{
				led_set_brightness(gradient_manager.arr[i].id, brightness + 1);
			}
		}
		else if (gradient_manager.arr[i].repeatCount == 0)
		{
			if (clock_time_exceed(gradient_manager.arr[i].startTick, (gradient_manager.arr[i].offTime_ms / 100)))
			{
				led_set_brightness(gradient_manager.arr[i].id, brightness - 1);
			}
		}
		brightness = led_get_brightness(gradient_manager.arr[i].id);
		if (brightness == 100 || brightness == 0)
		{
			if (gradient_manager.arr[i].offTime_ms < 100 || gradient_manager.arr[i].onTime_ms < 100)
			{
				ARRAR_REMOVE_POS(gradient_manager.arr, gradient_manager.len, i, LedEvent);
				i--;
			}
			else
			{
				gradient_manager.arr[i].repeatCount ^= 1;
			}
			
		}
		
	}
#endif
}
