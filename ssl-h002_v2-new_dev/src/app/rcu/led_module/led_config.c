/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     led_config.h
* @brief    
* @details
* @author   lihuashe
* @date     2024-12-20
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "led_config.h"
#include "rcu_application.h"
#include "voltage_driver.h"

/*============================================================================*
 *                              Global Variables
 *============================================================================*/


uint8_t led_max_bright = 0xff;
uint8_t led_imax = 0x01;
uint8_t g_led_sys_status;

/*============================================================================*
 *                              Functions Declaration
 *============================================================================*/


/**
 * @brief 设置按键背光关
 * @param  
 */
void ctrl_gear_key_backlit_off(void)
{
    bct3236_set_pinx("3-4", 0x00, 0x00);
}

/**
 * @brief 设置按键背光开
 * @param  
 */
void ctrl_gear_key_backlit_on(void)
{
    bct3236_set_pinx("3-4", led_max_bright, led_imax);
}

/**
 * @brief 设置按键背光关
 * @param  
 */
void ctrl_mode_key_backlit_off(void)
{
    bct3236_set_pinx("5-6", 0x00, 0x00);
}

/**
 * @brief 设置按键背光开
 * @param  
 */
void ctrl_mode_key_backlit_on(void)
{
    bct3236_set_pinx("5-6", led_max_bright, led_imax);
}


/**
 * @brief 关闭所有档位灯
 * @param  
 */
void ctrl_gear_led_all_off(void)
{
    bct3236_set_pinx("10-19", 0x00, 0x00);
}


/**
 * @brief 控制档位亮灯
 * @param gear 现在档位
 */
void ctrl_gear_led(uint8_t gear)
{

    switch (gear)
    {
        case 1:
            {
                uint8_t led_brr[] = {WHITE_LED_GEAR_1, WHITE_LED_GEAR_2, WHITE_LED_GEAR_3, WHITE_LED_GEAR_4};
                bct3236_set_led_group(led_brr, sizeof(led_brr), 0, 0);
                uint8_t led_arr[] = {WHITE_LED_GEAR_5};
                bct3236_set_led_group(led_arr, sizeof(led_arr), led_max_bright, led_imax);
            }
            break;

        case 2:
            {
                uint8_t led_arr[] = {WHITE_LED_GEAR_4, WHITE_LED_GEAR_5};
                bct3236_set_led_group(led_arr, sizeof(led_arr), led_max_bright, led_imax);
            }
            break;

        case 3:
            {
                uint8_t led_arr[] = {WHITE_LED_GEAR_1, WHITE_LED_GEAR_4, WHITE_LED_GEAR_5};
                bct3236_set_led_group(led_arr, sizeof(led_arr), led_max_bright, led_imax);
            }
            break;

        case 4:
            {
                uint8_t led_arr[] = {WHITE_LED_GEAR_1, WHITE_LED_GEAR_3, WHITE_LED_GEAR_4, WHITE_LED_GEAR_5};
                bct3236_set_led_group(led_arr, sizeof(led_arr), led_max_bright, led_imax);
            }
            break;

        case 5:
            {
                uint8_t led_arr[] = {WHITE_LED_GEAR_1, WHITE_LED_GEAR_2, WHITE_LED_GEAR_3, WHITE_LED_GEAR_4, WHITE_LED_GEAR_5};
                bct3236_set_led_group(led_arr, sizeof(led_arr), led_max_bright, led_imax);
            }
            break;

        default:
            
            break;
    }
}


/**
 * @brief 模式灯全关
 * @param  
 */
void ctrl_mode_led_all_off(void)
{
    bct3236_set_pinx("7-9", 0x00, 0x00);
}

/**
 * @brief 模式灯全开
 * @param  
 */
void ctrl_mode_led_all_on(void)
{
    bct3236_set_pinx("7-9", led_max_bright, led_imax);
}


/**
 * @brief 对应模式灯
 * @param mode 
 */
void ctrl_mode_led(uint8_t mode)
{

    switch (mode)
    {
        case FACE_MODE:
            {
                uint8_t led_arr[] = {8, 9};
                bct3236_set_led_group(led_arr, sizeof(led_arr), 0, 0);
                bct3236_set_pinx("7", led_max_bright, led_imax);
            }
            break;

        case SCALP_MODE:
            {
                uint8_t led_arr[] = {7, 8};
                bct3236_set_led_group(led_arr, sizeof(led_arr), 0, 0);
                bct3236_set_pinx("9", led_max_bright, led_imax);
            }    
            break;

        case FACE_MESO_MODE:
            {
                bct3236_set_pinx("9", 0x00, 0x00);
                bct3236_set_pinx("7-8", led_max_bright, led_imax);
            }
            break;

        case SCALP_MESO_MODE: 
            {
                bct3236_set_pinx("7", 0x00, 0x00);
                bct3236_set_pinx("8-9", led_max_bright, led_imax);
            }
            break;
       
        default:
            break;
    }
}

/**
 * @brief 控制模式和档位的led
 * @param mode 模式
 * @param gear 档位
 */
void ctrl_mode_gear_led(uint8_t mode, uint8_t gear)
{
    PRINT("ctrl_mode_gear_led\n");
    ctrl_mode_led(mode);
    ctrl_gear_led(gear);
    
    if (gear != 0)
    {
        for (uint8_t i = 0; i < sizeof(v_led_map)/sizeof(Voltage_Led_Map); i++)
        {
            if (v_led_map[i].mode == mode && v_led_map[i].gear == gear)
            {
                now_led_map = v_led_map[i];
            }
        }
        led_mode = now_led_map.led_mode;
        now_breath = hv_lv_red_led;
        led_breath_handler.led_on = &ctrl_red_led;
        led_breath_handler.breath_timer_t = &led_event_timer_t_set;

        led_blink_handler.led_on  = &ctrl_red_led;
        led_blink_handler.led_off = &ctrl_red_led_all_off;
        led_blink_handler.blink_timer_t = &led_event_timer_t_set;
        led_blink_handler.blink_stop_timer_t = &led_event_stop_timer_t;

        led_blink_ctrl_event(&now_led_map.blink, &led_blink_handler, LED_RESET);
        os_timer_start(&led_tick_timer);
    }  
}


/**
 * @brief 关闭所有红灯
 * @param  
 */
void ctrl_red_led_all_off(void)
{
    bct3236_set_pinx("20-25", 0x00, 0x00);
}


/**
 * @brief 控制红灯亮度
 * @param brightness 
 */
void ctrl_red_led(uint8_t brightness)
{
    bct3236_set_pinx("20-25", brightness, led_imax);
}


/**
 * @brief 低电量闪灯
 * @param  
 */
void ctrl_low_power_blink(void)
{
    ctrl_red_led(0xfe);
}

/**
 * @brief 蓝牙亮灯
 * @param brightness 
 */
void ctrl_led_ble_pair_set(uint8_t brightness)
{
    static bool pair_flag = false;
    
    if ((app_global_data.rcu_status == RCU_STATUS_PAIRED) && pair_flag == false)
    {
        pair_flag = true;
        paired_flag = 1;
        led_system_ctrl(LED_PAIRED_OK);

        now_led_map = led_ble_paired_map;
        led_mode = now_led_map.led_mode;

        led_blink_handler.led_on  = &ctrl_led_ble_pair_set;
        led_blink_handler.led_off = &ctrl_gear_led_all_off;
        led_blink_handler.blink_timer_t = &led_event_timer_t_set;
        led_blink_handler.blink_stop_timer_t = &led_event_stop_timer_t;

        led_blink_ctrl_event(&now_led_map.blink, &led_blink_handler, LED_DELETE_LOOP);
        led_blink_ctrl_event(&now_led_map.blink, &led_blink_handler, LED_DELETE_COUNT);
        os_timer_start(&led_tick_timer);
    }
    else if (app_global_data.rcu_status == RCU_STATUS_DISCONNECTING)
    {
        pair_flag = false;
        led_system_ctrl(LED_BRIGHT_STOP);

        ctrl_mode_gear_led(g_beauty_comb.mode, g_beauty_comb.gear);
        voltage_mode_gear_change(g_beauty_comb.mode, g_beauty_comb.gear);
    }

    if (app_global_data.rcu_status == RCU_STATUS_ADVERTISING)
    {
        pair_flag = false;
        led_blink_ctrl_event(&now_led_map.blink, &led_blink_handler, LED_DELETE_LOOP);
        uint8_t led_arr[] = {WHITE_LED_GEAR_2,WHITE_LED_GEAR_3, WHITE_LED_GEAR_5};
        bct3236_set_led_group(led_arr, sizeof(led_arr), led_max_bright, led_imax);
    }

}

/**
 * @brief 配对成功亮灯
 * @param  
 */
void ctrl_led_ble_pair_start(void)
{
    ctrl_gear_led_all_off();
    
    now_led_map = led_ble_pair_map;
    led_mode = now_led_map.led_mode;

    led_blink_handler.led_on  = &ctrl_led_ble_pair_set;
    led_blink_handler.led_off = &ctrl_gear_led_all_off;
    led_blink_handler.blink_timer_t = &led_event_timer_t_set;
    led_blink_handler.blink_stop_timer_t = &led_event_stop_timer_t;

    led_blink_ctrl_event(&now_led_map.blink, &led_blink_handler, LED_RESET);
    os_timer_start(&led_tick_timer);
}



/**
 * @brief ota亮灯亮度设置
 * @param brightness 
 */
void ctrl_ota_led_set(uint8_t brightness)
{
    uint8_t led_arr[] = {WHITE_LED_GEAR_1, WHITE_LED_GEAR_2, WHITE_LED_GEAR_3, WHITE_LED_GEAR_4, WHITE_LED_GEAR_5};
    bct3236_set_led_group(led_arr, sizeof(led_arr), brightness, led_imax);
}

/**
 * @brief ota升级亮灯
 * @param  
 */
void ctrl_ota_led_start(void)
{
    led_mode = LED_BREATH_MODE;
    now_breath = led_ble_ota_map;

    led_breath_handler.led_on  = &ctrl_ota_led_set;
    led_breath_handler.breath_timer_t = &led_event_timer_t_set;

    key_disable();
    led_blink_ctrl_event(&now_led_map.blink, &led_blink_handler, LED_RESET);
    os_timer_start(&led_tick_timer);
}

/**
 * @brief 充电开始
 * @param  
 */
void ctrl_chg_red_start(void)
{
    led_mode = LED_BREATH_MODE;
    now_breath = chg_red_led;
    
    led_breath_handler.led_on  = &ctrl_chg_red_led_set;
    led_breath_handler.breath_timer_t = &led_event_timer_t_set;

    os_timer_start(&led_tick_timer);
}

/**
 * @brief 设置充电红灯亮度
 * @param brightness 亮度
 */
void ctrl_chg_red_led_set(uint8_t brightness)
{
    uint8_t led_arr[] = {RED_LED_CHG_1, RED_LED_CHG_2, RED_LED_CHG_3, RED_LED_CHG_4, RED_LED_CHG_5};

    bct3236_set_led_group(led_arr, sizeof(led_arr), brightness, led_imax);
}


/**
 * @brief 关闭充电红灯
 * @param  
 */
void ctrl_chg_red_led_off(void)
{
    uint8_t led_arr[] = {RED_LED_CHG_1, RED_LED_CHG_2, RED_LED_CHG_3, RED_LED_CHG_4, RED_LED_CHG_5};

    bct3236_set_led_group(led_arr, sizeof(led_arr), 0, 0x00);
}


/**
 * @brief 低电量闪烁开启
 * @param brightness 
 */
void low_power_red_led_set(uint8_t brightness)
{
    ctrl_chg_red_led_set(brightness);
}

/**
 * @brief 低电量闪烁关闭
 * @param  
 */
void low_power_red_led_off(void)
{
    static uint8_t low_power_loop = 0;

    ctrl_chg_red_led_off();
    low_power_loop++;
    
    if (low_power_loop % 3 == 0)
    {
        play_music_map(MUSIC_LOW_POWER);
        os_timer_restart(&led_tick_timer, 500);
    }
    if (low_power_loop >= (now_led_map.blink.loop_count * now_led_map.blink.blink_count))
    {
        os_delay(200);
        all_sys_function_off();
        POWER_ON_OFF();//关机
    }
    
}

/**
 * @brief 低电量闪灯
 * @param  
 */
void ctrl_low_power_red_led_start(void)
{
    now_led_map = low_power_map;
    led_mode = now_led_map.led_mode;

    led_blink_handler.led_on  = &low_power_red_led_set;
    led_blink_handler.led_off = &low_power_red_led_off;
    led_blink_handler.blink_timer_t = &led_event_timer_t_set;
    led_blink_handler.blink_stop_timer_t = &led_event_stop_timer_t;

    led_blink_ctrl_event(&now_led_map.blink, &led_blink_handler, LED_RESET);
    os_timer_start(&led_tick_timer);
}


/**
 * @brief 封装一个定时器
 * @param time_ms 
 */
void led_event_timer_t_set(uint32_t time_ms)
{
    os_timer_restart(&led_tick_timer, time_ms);
}


/**
 * @brief 停止时钟
 * @param  
 */
void led_event_stop_timer_t(void)
{
    os_timer_stop(&led_tick_timer);
}


/**
 * @brief 控制led闪烁
 * @param led led参数
 * @param led_handler 执行函数结构体
 * @param set_mode 对函数内部static数据处理的模式
 * @return 返回true为执行完毕
 */
bool led_blink_ctrl_event(Blink_Led_Config *led, Blink_Led_Handler *led_handler, uint8_t set_mode)
{
    static bool led_flag = false;
    static uint8_t blink_count = 0;
    static uint16_t loop_count = 0;
    
    if (led_handler->led_on == NULL || led_handler->led_off == NULL || led_handler->blink_timer_t == NULL || led_handler->blink_stop_timer_t == NULL) //避免访问空地址
        return false;
    
    switch (set_mode)//特殊处理
    {
        case LED_RESET:
            {
                led_flag = false;
                blink_count = 0;
                loop_count = 0;
            }
            return false;
        case LED_DELETE_LOOP:
            {
                loop_count = 0;
            }
            return false;
        case LED_DELETE_COUNT:
            {
                blink_count = 0;
            }
            return false;
        
        default:
            break;
    }

    //PRINT("loop_count :%d \n", loop_count);
    if ((loop_count >= (led->loop_count * led->blink_count * 2)) && (led->loop_count * led->blink_count * 2) != 0)//循环次数
    {
        led_handler->blink_stop_timer_t();
        loop_count = 0;
        
        return true;
    }
    else
    {
        loop_count++;
    }

    if (led_flag == false && blink_count < led->blink_count)//亮灯
    {
        led_flag = true;
        led_handler->led_on(led->brightness);
        led_handler->blink_timer_t(led->keep_time);

        return false;
    }
    else if (led_flag == true && blink_count < led->blink_count)//关灯
    {
        led_flag = false;
        blink_count++;
        led_handler->blink_timer_t(led->wait_time);
        led_handler->led_off();

        return false;
    }
    
    if (blink_count >= led->blink_count)
    {
        led_flag = false;
        blink_count = 0;
        if (led->loop_time <= 1)
        {
            led_handler->blink_timer_t(1);
        }
        else
        {
            led_handler->blink_timer_t(led->loop_time);
        }
        
        return true;
    }

    return false;
}



/**
 * @brief 控制led呼吸
 * @param led led参数
 * @param led_handler 执行函数结构体
 * @param set_mode 
 * @return 返回true为执行完毕
 */
bool led_breath_ctrl_event(Breath_Led_Config *led, Breath_Led_Handler *led_handler)
{

    if (led_handler->led_on == NULL || led_handler->breath_timer_t == NULL)
        return false;

    if (led->reverse_led_flag == false)
    {
        if (led->current_brightness < led->start_brightness)
            led->current_brightness += led->step_brightness;
        if (led->current_brightness >= led->start_brightness)
            led->reverse_led_flag = true;
    }
    else if (led->reverse_led_flag == true)
    {
        if (led->current_brightness > led->step_brightness)
            led->current_brightness -= led->step_brightness;
        if (led->current_brightness <= led->step_brightness)
        {
            led->reverse_led_flag = false;
            led->current_brightness = led->end_brightness;
        }
            
    }
    
    if (led->current_brightness == led->end_brightness)
    {
        led_handler->led_on(led->current_brightness);
        led_handler->breath_timer_t(led->led_end_keep_time);

        return true;
    }
    else
    {
        //PRINT("current:%d\n", led->current_brightness);
        led_handler->led_on(led->current_brightness);
        led_handler->breath_timer_t(led->led_breath_time);
    }

    return false;
}

/**
 * @brief led滴答回调
 * @param pxTimer 
 */
void led_ctrl_timer_cb(TimerHandle_t pxTimer)
{

    if (led_mode == LED_BLINK_MODE)
    {
        led_blink_ctrl_event(&now_led_map.blink, &led_blink_handler, LED_NORMAL);
    }
    else if (led_mode == LED_BREATH_MODE)
    {
        led_breath_ctrl_event(&now_breath, &led_breath_handler);
    }
    else if (led_mode == LED_BLINK_AND_BREATH_MODE)
    {
        static uint8_t blink_or_breath = LED_BLINK_MODE;

        if (blink_or_breath == LED_BLINK_MODE)
        {
            if (led_blink_ctrl_event(&now_led_map.blink, &led_blink_handler, LED_NORMAL))
            {
                blink_or_breath = LED_BREATH_MODE;
            }
        }
        else if (blink_or_breath == LED_BREATH_MODE)
        {
            if (led_breath_ctrl_event(&now_breath, &led_breath_handler))
            {
                blink_or_breath = LED_BLINK_MODE;
            }
        }

    }

}



/**
 * @brief LED的总控制，负责全部灯效的管控
 * @param led_status 
 */
void led_system_ctrl(uint8_t led_status)
{
    g_led_sys_status = led_status;

    switch (led_status)//特殊处理
    {
        case LED_NORMAL_WORK:
            {
                ctrl_mode_gear_led(g_beauty_comb.mode, g_beauty_comb.gear);
            }
            break;
        case LED_CHGING:
            {
                ctrl_chg_red_start();//开启呼吸灯
            }
            break;
        case LED_CHG_DONE:
            {
                ctrl_gear_led(5);//5个白灯亮
            }
            break;
        case LED_LOW_POWER:
            {
                ctrl_low_power_red_led_start();
            }
            break;
        case LED_PAIRING_START:
            {
                ctrl_led_ble_pair_start();
            }
            break;
        case LED_PAIRED_OK:
            {
                ctrl_gear_led(5);//5个白灯亮
            }
            break;
        case LED_OTA_START:
            {
                ctrl_ota_led_start();
            }
            break;
        case LED_BRIGHT_STOP:
            {
                uint32_t led_stop = os_lock();//进入临界

                Voltage_Led_Map led_stop_map = {0, 0, LED_BLINK_MODE, {0, 0, 0, 0, 0, 0}};
                now_led_map = led_stop_map;
                led_mode = now_led_map.led_mode;
                
                led_blink_handler.led_on  = NULL;
                led_blink_handler.led_off = NULL;
                os_timer_stop(&led_tick_timer);

                bct3236_set_pinx("1-25", 0x00, 0x00); 
                os_unlock(led_stop);
            }
            break;
    
        
        default:
            break;
    }
}



/******************* (C) COPYRIGHT 2024 Sensasy Corporation *****END OF FILE****/
