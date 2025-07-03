/*
 * @Author: lihuashe lihuashe@seneasy.com
 * @LastEditors: lihuashe
 * @LastEditTime: 2024-09-06 15:20:18
 * @file: my_led.c
 * @Description: Copyright 2024 Guangdong Seneasy Intelligent Technology Co., Ltd. All rights reserved.
 */


#include "my_led.h"

//我的代码
// 初始化 LED

Led_status g_leds; // LED 数组

void led_init() 
{   
    for (u8 i = 0; i < LEDS_NUM; i++)
         led_init_set(i);
}


void led_init_set(uint8_t id) 
{
    
    if (id < LEDS_NUM) 
    {
        g_leds.status[id].id = id;
        g_leds.status[id].brightness = 0;
        g_leds.status[id].is_on = false;
        g_leds.status[id].on_time = 0;      // 默认亮灯起始时间为 0
        g_leds.status[id].off_time = 0;     // 默认亮灯关闭为 0
        g_leds.status[id].start_delay = 0;
        g_leds.status[id].end_delay = 0;
        g_leds.status[id].blink_count = 0;  // 默认亮灯次数为 0
    }  
}


/**
 * @description: 设置单独或者每个灯的状态设置，1代表亮，0代表灭
 * @param {u8} led_1
 * @param {u8} led_2
 * @param {u8} led_3
 * @param {u8} led_4
 * @param {u8} led_5
 * @return {*}
 */
void led_set_every(u8 led_1, u8 led_2, u8 led_3, u8 led_4, u8 led_5)
{
    char led[LEDS_NUM] = {led_1,led_2,led_3,led_4,led_5};

    led_set_brightness_time(&led[0],500,"0.5s","0.255s",1); //这是启用默认设置来设置每个灯
         
}



/**
 * @description: 这里能够单独设置灯的亮度，亮灯时间，亮灯的次数和间隔
 * @param {char} *leds：各个灯状态
 * @param {u16} brightness：亮度
 * @param {char} *off_time：传输时间用“s,ms”不区分大小写
 * @param {char} *end_delay：传输时间用“s,ms”不区分大小写
 * @param {u8} count：亮灯次数
 * @return {*}
 */
void led_set_brightness_time(char *leds, u16 brightness, char *off_time, char *end_delay, u8 count){

    u8 ccount = 0;
    u32 ctime = 0,delay = 0;
 
    led_pwm_init_all_led();

    if (count == 0)
        return ;

    ctime  = time_char_change_int(off_time);
    delay = time_char_change_int(end_delay);
   
    //设置每个灯
    if (strlen(leds) == 1)
    {
        u8 led = *leds - '1';
        
        g_leds.all_on_off = ON;
        g_leds.status[led].is_on = 1;
        g_leds.status[led].on_time = clock_time(); 
        g_leds.status[led].off_time = ctime;
        g_leds.status[led].end_delay = delay;
        g_leds.status[led].blink_count = count;
        g_leds.status[led].brightness = brightness;
        
        pwm_set_cycle_and_duty(PWM0_ID + led, PWM_CLOCK, (unsigned short)(g_leds.status[led].brightness * CLOCK_SYS_CLOCK_1US));      
    
    }
    else
    {
        led_pwm_all_off();
        while(ccount < LEDS_NUM)
        {   
            if (*leds <= 9)
                g_leds.status[ccount].is_on = (bool)*leds;
            else
                g_leds.status[ccount].is_on = (bool)(*leds - '0');
            ccount++; 
            leds++;
        }
        
        for (int i = 0; i < LEDS_NUM ; i++) 
        {
            g_leds.status[i].on_time = clock_time();
            g_leds.status[i].off_time = ctime;
            g_leds.status[i].end_delay = delay;
            g_leds.status[i].blink_count = count;
            g_leds.status[i].brightness = brightness;
            
            if (g_leds.status[i].is_on != 0)
            {
                g_leds.all_on_off = ON;
                pwm_set_cycle_and_duty(PWM0_ID + i, PWM_CLOCK, (unsigned short)(g_leds.status[i].brightness * CLOCK_SYS_CLOCK_1US));
            }
        }
    }    
}

void led_contral_on_off(void)
{
    for (u8 i = 0;i < LEDS_NUM; i++)
	{
		if(g_leds.status[i].blink_count == 0)
			continue;
		
		if (g_leds.status[i].blink_count > 1)
		{
			if (g_leds.status[i].start_delay != 0 && clock_time_exceed(g_leds.status[i].start_delay,g_leds.status[i].end_delay * 1000)  && g_leds.status[i].is_on == 1)
			{
				pwm_set_cycle_and_duty(PWM0_ID + i, PWM_CLOCK, (unsigned short)(g_leds.status[i].brightness * CLOCK_SYS_CLOCK_1US));
				g_leds.status[i].start_delay = 0;
				g_leds.status[i].on_time = clock_time();
				g_leds.status[i].blink_count--;
                //printf(">>>>>>>>on\n");
			}
		}
		if (g_leds.status[i].on_time != 0 && clock_time_exceed(g_leds.status[i].on_time,g_leds.status[i].off_time * 1000))
		{
			pwm_set_cycle_and_duty(PWM0_ID + i, PWM_CLOCK, PWM_CLCOK_OFF);
			g_leds.status[i].on_time = 0;
            if (g_leds.status[i].blink_count == 1)
            {
                g_leds.status[i].start_delay = 0;
            }	    
            else
			    g_leds.status[i].start_delay = clock_time();
            //printf(">>>>>>>>off\n");
		}
		
			
		pwm_start(PWM0_ID + i);
	}
}

// 切换 LED 状态
void led_toggle(uint8_t id) {
    if (id < LEDS_NUM) 
    {
        g_leds.status[id].is_on = !g_leds.status[id].is_on;
        g_leds.status[id].brightness = g_leds.status[id].is_on ? 500 : 0; // 切换时设置为全亮或全灭
        // 这里可以添加 PWM 设置代码
        printf("LED %d toggled to %s\n", id, g_leds.status[id].is_on ? "ON" : "OFF");
    }
}







//分界线。。。。。。。。。。。。。