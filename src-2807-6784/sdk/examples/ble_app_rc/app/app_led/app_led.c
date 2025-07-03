#include "peripheral.h"

#include "app_led.h"

#define APP_LOG_DOMAIN      "led"
#define APP_LOG_LEVEL       APP_LED_LOG_LEVEL
#include "co_debug.h"

#if (BLT_APP_LED_ENABLE)

void led_dcdc_en_ctrl_handle(uint8_t en);
void led_ctrl_timer_cb(co_timer_t *timer, void *param);

static co_timer_t led_ctrl_timer;
device_led_t device_led[LED_MAX_CONT];
const led_cfg_t led_cfg[] = LED_MAP_ARR;

 /**
 * @brief		开关led 硬件配置
 * @param[in]	on-开关
 * @param[in]	led_index-led索引
 */
void device_led_on_off(unsigned char on, unsigned char led_index)
{
	device_led[led_index].isOn = on;

	if (on)
	{
		gpio_write(BITMASK(device_led[led_index].led_port), device_led[led_index].on_leval);
	}
	else
	{
		gpio_write(BITMASK(device_led[led_index].led_port), ~device_led[led_index].on_leval);
	}
}


 /**
 * @brief		led 初始化
 * @param[in]	led_index-led索引
 * @param[in]	led_type-led硬件类型
 * @param[in]	port-端口
 * @param[in]	bit-端口对应的io口
 * @param[in]	on_leval-点亮输出的电平
 */
void device_led_init(unsigned char led_index,unsigned char port, unsigned char on_leval)
{  
	pinmux_config(port, PINMUX_GPIO_MODE_CFG);
	pmu_pin_mode_set(BITMASK(port), PMU_PIN_MODE_PP);
	gpio_set_direction(BITMASK(port), GPIO_OUTPUT);


	if (on_leval)
	{		
		device_led[led_index].on_leval = ~0;
	}
	else
	{
		device_led[led_index].on_leval = 0;
	}

	
	device_led[led_index].led_port = port;
	
	
	device_led_on_off(0,led_index);
}




int device_led_setup(led_cfg_t led_cfg)
{
	APP_LOG_INF("[led] priority:%d repeatCount:%d\r\n",led_cfg.priority,led_cfg.repeatCount);
	device_LED_DC(true); 
	uint8_t led_index = led_cfg.led_index;		 
	if( device_led[led_index].repeatCount &&  device_led[led_index].priority > led_cfg.priority)
	{
		return 1; 
	}
	else
	{
		device_led[led_index].onTime_ms = led_cfg.onTime_ms;
		device_led[led_index].offTime_ms = led_cfg.offTime_ms;
		device_led[led_index].repeatCount = led_cfg.repeatCount;
		device_led[led_index].priority = led_cfg.priority;
		device_led[led_index].along_on = 0;
		
        if(led_cfg.repeatCount == LED_SINGLE_STATE)
		{ 
        	device_led[led_index].repeatCount = 0;
        	if (device_led[led_index].onTime_ms > 0)
        	{
        		device_led[led_index].along_on = 1;
        	}
        }
        else
		{ 
        	if(!device_led[led_index].onTime_ms)
			{ 
        		device_led[led_index].offTime_ms *= device_led[led_index].repeatCount;
        		device_led[led_index].repeatCount = 1;
        	}
        	else if(!device_led[led_index].offTime_ms)
			{
        		device_led[led_index].onTime_ms *= device_led[led_index].repeatCount;
        	    device_led[led_index].repeatCount = 1;
        	}
        }

		/**长亮或长灭不会补偿灯光*/
		if (device_led[led_index].isOn && device_led[led_index].repeatCount > 1 && device_led[led_index].onTime_ms > 0)
		{
			device_led[led_index].repeatCount++;
		}

		device_led[led_index].startTick = 0;
		device_led[led_index].cur_timer = 0;
		
		device_led_on_off(device_led[led_index].onTime_ms ? 1 : 0, led_index);
		
      	co_timer_set(&led_ctrl_timer, LED_PERIOD, TIMER_ONE_SHOT, led_ctrl_timer_cb, NULL);
	}
	
	return 0;
}


void led_ctrl_timer_cb(co_timer_t *timer, void *param)
{
	bool event_flag = false;
	
	for (int led_index=0; led_index<LED_MAX_CONT; led_index++)
	{
		if(device_led[led_index].repeatCount)
		{
			event_flag = true;
			device_led[led_index].cur_timer += LED_PERIOD;
			
			if(device_led[led_index].isOn)
			{
				if((device_led[led_index].cur_timer - device_led[led_index].startTick) > device_led[led_index].onTime_ms)
				{
					device_led_on_off(0,led_index);
					if(device_led[led_index].offTime_ms)
					{ 
						device_led[led_index].startTick += device_led[led_index].onTime_ms;
					}
					else{
						device_led[led_index].repeatCount = 0;
					}
				}
			}
			else
			{
				if((device_led[led_index].cur_timer - device_led[led_index].startTick) > device_led[led_index].offTime_ms)
				{
					if(--device_led[led_index].repeatCount)
					{
						device_led_on_off(1,led_index);
						device_led[led_index].startTick += device_led[led_index].offTime_ms;
					}
				}
			}

			if (device_led[led_index].repeatCount==0)
			{
                device_led_on_off(0,led_index);
			}
		}
	}
	
	if (event_flag)
    {
		co_timer_set(&led_ctrl_timer, LED_PERIOD, TIMER_ONE_SHOT, led_ctrl_timer_cb, NULL);
	}
}

/**
 * @brief  	   led module reset.
 * @param      void.
 * @return     void.
 * @note       none.
**/
void led_module_reset(void)
{
	co_timer_del(&led_ctrl_timer);
	for (int led_index=0; led_index<LED_MAX_CONT; led_index++)
	{
		device_led[led_index].repeatCount = 0;
		device_led[led_index].priority = 0;
	}
}

void reset_designate_led_index(uint8_t led_index)
{
	device_led[led_index].repeatCount = 0;
	device_led[led_index].priority = 0;
	device_led_on_off(0,led_index);
}


/**
 * @brief  	   led module init.
 * @param      void.
 * @return     void.
 * @note       none.
**/
void led_module_init(void)
{

#if (BLT_APP_LED_ENABLE & LED1_LED_FLAG)
    device_led_init(LED1_LED, LED1_PORT, LED1_ON_LEVAL);       
#endif
	
#if (BLT_APP_LED_ENABLE & LED2_LED_FLAG)
    device_led_init(LED2_LED, LED2_PORT, LED2_ON_LEVAL);    
#endif
#if (BLT_APP_LED_ENABLE & LED3_LED_FLAG)
    device_led_init(LED3_LED, LED3_PORT, LED3_ON_LEVAL);    
#endif
}



void led_module_deinit(void)
{
#if (BLT_APP_LED_ENABLE & LED1_LED_FLAG)     
	pinmux_config(LED1_PORT,PINMUX_GPIO_MODE_CFG);
	gpio_set_direction(BIT_MASK(LED1_PORT),GPIO_INPUT);
	pmu_pin_mode_set(BIT_MASK(LED1_PORT),PMU_PIN_MODE_PD);
#endif

#if (BLT_APP_LED_ENABLE & LED2_LED_FLAG)   
	pinmux_config(LED2_PORT,PINMUX_GPIO_MODE_CFG);
	gpio_set_direction(BIT_MASK(LED2_PORT),GPIO_INPUT);
	pmu_pin_mode_set(BIT_MASK(LED2_PORT),PMU_PIN_MODE_PD);
#endif
	
#if (BLT_APP_LED_ENABLE & LED3_LED_FLAG)   
	pinmux_config(LED3_PORT,PINMUX_GPIO_MODE_CFG);
	gpio_set_direction(BIT_MASK(LED3_PORT),GPIO_INPUT);
	pmu_pin_mode_set(BIT_MASK(LED3_PORT),PMU_PIN_MODE_PD);
#endif
}

/**
 * @brief		led 是否空闲
 * @return      0 空闲
 */
unsigned char device_led_busy(unsigned char led_index)
{
#if (BLT_APP_LED_ENABLE)
	return (device_led[led_index].repeatCount || device_led[led_index].along_on) ? 1:0;
#endif
}

/**
 * @brief		所有led 是否空闲
 * @return      0 空闲
 */
unsigned char device_all_led_busy(void)
{
	unsigned char all_led_busy = 0;
#if (BLT_APP_LED_ENABLE)
	for (int led_index = 0; led_index<LED_MAX_CONT; led_index++)
	{
		all_led_busy |= device_led_busy(led_index);
	}
#endif
	return all_led_busy;
}

/**
 * @brief		LED_DCEN
 */
void device_LED_DC(unsigned char on)
{
	pinmux_config(LED_DC_EN_PORT,PINMUX_GPIO_MODE_CFG);
	gpio_set_direction(BIT_MASK(LED_DC_EN_PORT),GPIO_INPUT);
	pmu_pin_mode_set(BIT_MASK(LED_DC_EN_PORT),PMU_PIN_MODE_PD);
	if (on)
		gpio_write(LED_DC_EN_PORT, GPIO_HIGH);
	else
		gpio_write(LED_DC_EN_PORT, GPIO_LOW);
}



#endif /* BLT_APP_LED_ENABLE */
