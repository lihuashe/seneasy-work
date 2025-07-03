/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     key_handle.c
* @brief    This is the entry of user code which the key handle module resides in.
* @details
* @author   chenjie jin
* @date     2020-02-25
* @version  v1.1
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <string.h>
#include "board.h"
#include "key_handle.h"
#include <trace.h>
#include "profile_server.h"
#include "hids_rmc.h"
#include "rcu_application.h"
#include "os_timer.h"
#include "swtimer.h"
#include "rcu_gap.h"
#include "gap_storage_le.h"
#include "rtl876x_wdg.h"
#include "rtl876x_keyscan.h"
#include "gap_bond_le.h"
#include "battery_driver.h"
#include <app_section.h>
#include "app_task.h"
#include <key_config.h>
#include "gap_adv.h"
#include "app_function.h"
#include "gap_conn_le.h"
#include "voltage_driver.h"
#include "led_config.h"
#include "production_test.h"


#if FEATURE_SUPPORT_MP_TEST_MODE
#include "mp_test.h"
#endif
#if (VOICE_FLOW_SEL == ATV_GOOGLE_VOICE_FLOW)
#include "atvv_service.h"
#endif
/*============================================================================*
 *                              Local Variables
 *============================================================================*/
/* Key Mapping Table Definiton */

/*============================================================================*
 *                              Global Variables
 *============================================================================*/
/* BLE HID code table definition */


T_KEY_STATUS_DATA g_key_data = {};
volatile KeyScan_Data g_keyscan_data = {};


/*============================================================================*
 *                              Local Functions
 *============================================================================*/

/**
 * @brief 获取当前时间
 * @param  
 * @return 
 */

extern inline uint32_t clock_time_tick(void)
{
  // 获取当前的供应商定时器计数ms
  uint32_t current_tick = os_sys_time_get();
  
  return current_tick;
}

/**
 * @brief 超时判断函数
 * @param ref 记录时间
 * @param span_us 超时时间
 * @return 
 */
extern inline uint32_t vendor_timer_exceed(unsigned int ref, unsigned int span_ms)
{
  // 获取当前的供应商定时器计数ms
  uint32_t current_tick = clock_time_tick();

  return (uint32_t)((current_tick - ref) > (span_ms)); // 40M = 40 ticks 
}

/**
 * @brief 这是一个负责一个按键对应事件的执行函数
 * @param one_key_arr 配置按键键值和执行函数的数组
 * @param one_key_arr_len 数组长度
 * @param p_keyscan_fifo_data 扫描到的按键信息
 * @return 
 */
void one_key_to_handle(OneKey *one_key_arr, 
                       uint8_t one_key_arr_len,
                       uint8_t key)
{

    for (uint8_t i = 0; i < one_key_arr_len; i++)
    {
        if (key == one_key_arr[i].key)
        {
            one_key_arr[i].do_handle(key);   
        }
        
    }
    
}

/**
 * @brief 这是一个负责两个按键对应事件的执行函数
 * @param two_key_arr 配置按键键值和执行函数的数组
 * @param two_key_arr_len 数组长度
 * @param p_keyscan_fifo_data 扫描到的按键信息
 * @return 
 */
void two_key_to_handle(TwoKey *two_key_arr, 
                       uint8_t two_key_arr_len,
                       uint8_t key1, uint8_t key2)
{

    for (uint8_t i = 0; i < two_key_arr_len; i++)
    {
        if ((key1 == two_key_arr[i].key1 && key2 == two_key_arr[i].key2)
            || (key1 == two_key_arr[i].key2 && key2 == two_key_arr[i].key1))
        {
            two_key_arr[i].do_handle(two_key_arr[i].key1, two_key_arr[i].key2);
        }
    }
    
}


/**
 * @brief 这是一个执行不同条件下一个按键的区分函数
 * @param p_keyscan_fifo_data 这是按键信息
 * @return 
 */
bool one_key_dis_type_to_do(uint8_t key)
{

    if (key)
    {
        g_key_data.key[0].key_status = KEY_PRESS;
        g_key_data.key[0].key = key;
        if (g_key_data.key[0].press_time == 0)
            g_key_data.key[0].press_time = clock_time_tick();
    }
    else
    {
        g_key_data.key[0].key_status = KEY_RELEASE;
    }
  
    return true;
}


/**
 * @brief 这是一个执行不同条件下两个按键的区分函数
 * @param p_keyscan_fifo_data 这是按键信息
 * @return 
 */
bool two_key_dis_type_to_do(uint8_t key1, uint8_t key2)
{
    //PRINT("two_key_dis_type_to_do\n");
    
    if (key2) 
    {
        g_key_data.key[1].key_status = KEY_PRESS;
        g_key_data.key[1].key = key2;
        g_key_data.key[1].press_time = clock_time_tick();
    }  
    
    return true;
}


/**
 * @brief 进入正常模式
 * @param  
 */
void system_goto_init(void)
{
    if (g_led_sys_status != LED_NORMAL_WORK)
    {
        led_system_ctrl(LED_BRIGHT_STOP);
        led_system_ctrl(LED_NORMAL_WORK);
        voltage_mode_gear_change(g_beauty_comb.mode, g_beauty_comb.gear);
        //开背光
        ctrl_gear_key_backlit_on();
        ctrl_mode_key_backlit_on();
        GPIO_INTConfig(GPIO_GetPin(KEY_IO0), ENABLE);//失能按键
        GPIO_INTConfig(GPIO_GetPin(KEY_IO1), ENABLE);
        g_key_data.key[0].press_time = 0;
        g_key_data.key[0].key = 0; 
    }
}



/**
 * @brief 关闭配对
 * @param  
 */
void key_ble_pair_stop(void)
{
    if (app_global_data.rcu_status == RCU_STATUS_ADVERTISING)
    {
        if (le_adv_stop() == GAP_CAUSE_SUCCESS)
        {
            PRINT("rcu_stop_adv\n");
            app_global_data.rcu_status = RCU_STATUS_STOP_ADVERTISING;
        }
        else
        {
            PRINT("rcu_stop_adv false\n");
        }
        system_goto_init();
    }
    else  if ((app_global_data.rcu_status == RCU_STATUS_CONNECTED)
    || (app_global_data.rcu_status == RCU_STATUS_PAIRED))
    {
        if (le_disconnect(0) == GAP_CAUSE_SUCCESS)
        {
            PRINT("le_disconnect command sent successfully");
        }
        else
        {
            PRINT("le_disconnect command sent failed");
        }
        system_goto_init(); 
    }
    
}

/**
 * @brief 检测按键按压时间是否符合事件执行标志
 * @param  
 */
void key_press_time_dis_event(void)
{   
    if ((paired_flag == 1 && app_global_data.rcu_status == RCU_STATUS_IDLE))
    {
        paired_flag = 0;
        app_global_data.rcu_status = RCU_STATUS_IDLE;
        system_goto_init();
    }
    //PRINT("key_press_num  %d\n", g_keyscan_data.key_press_num);
    if (g_key_data.key[0].key != 0 && g_key_data.key[1].key == 0)
    {
        //PRINT("key 0:%d\n", g_key_data.key[0].key);
        //PRINT("press_time 0:%ld\n", g_key_data.key[0].press_time);
        //PRINT("clock_time_tick:%ld\n", clock_time_tick());
        //PRINT("KEY %d\n",g_key_data.key[0].key);
        
        if ((g_key_data.key[0].key_status == KEY_RELEASE) && (g_key_data.key[0].press_time != 0) && (!vendor_timer_exceed(g_key_data.key[0].press_time, 1800) && vendor_timer_exceed(g_key_data.key[0].press_time, 50)) 
        && (vendor_timer_exceed(g_key_data.key[0].press_time, 50)))
        {
            key_ble_pair_stop();
            one_key_to_handle(one_key_short_press_handle, 2, g_key_data.key[0].key);
            g_key_data.key[0].press_time = 0;
            g_key_data.key[0].key = 0; 
     
        }
        else if ((g_key_data.key[0].key_status == KEY_PRESS) && (g_key_data.key[0].press_time != 0) && (vendor_timer_exceed(g_key_data.key[0].press_time, 1800)))
        {
            key_ble_pair_stop();
            one_key_to_handle(one_key_long_press_2s_handle, 2, g_key_data.key[0].key);
            g_key_data.key[0].press_time = 0;    
            g_key_data.key[0].key = 0;   
 
        }
        else if ((g_key_data.key[0].key_status == KEY_PRESS) && (g_key_data.key[0].press_time != 0) && (vendor_timer_exceed(g_key_data.key[0].press_time, 800)) && (power_is_on == false) && ( g_key_data.key[0].key == POWER_KEY))
        {
            one_key_to_handle(one_key_long_press_2s_handle, 2, g_key_data.key[0].key);
            g_key_data.key[0].press_time = 0;    
            g_key_data.key[0].key = 0;   
        }
    }
    if (g_key_data.key[0].key != 0 && g_key_data.key[1].key != 0 && power_is_on == false)
    {
        //PRINT("press_time:%ld\n", g_key_data.key[0].press_time);
        if ((g_key_data.key[0].key_status == KEY_PRESS) && (vendor_timer_exceed(g_key_data.key[0].press_time, 4800)) && (g_key_data.key[0].press_time != 0)
        && (g_key_data.key[1].key_status == KEY_PRESS) && (vendor_timer_exceed(g_key_data.key[1].press_time, 4800)) && (g_key_data.key[1].press_time != 0))
        {
            uint8_t save_key = g_key_data.key[1].key;
            g_key_data.key[0].key = 0;
            g_key_data.key[0].press_time = 0;
            g_key_data.key[1].key = 0;
            g_key_data.key[1].press_time = 0;
            one_key_to_handle(two_key_long_press_5s_handle, 1, save_key);
        }

    }
    
    for (uint8_t i = 0; i < 3; i++)
    {
        if (g_key_data.key[i].key_status == KEY_RELEASE || vendor_timer_exceed(g_key_data.key[0].press_time, 10 * 1000))
        {
            g_key_data.key[i].key = 0;
            g_key_data.key[i].press_time = 0;
        }
    }

}


/**
 * @brief 按键松开处理
 * @param  
 */
void key_handle_release_event(void)
{
    //PRINT("key_handle_release_event\n");
    for (uint8_t i = 0; i < 6; i++)
    {
        g_key_data.key[i].key_status = KEY_RELEASE;
    }
}


/**
 * @brief 按键io_msg处理，处理按键按下松开
 * @param p_keyscan_fifo_data 接收键值
 */
void key_handle_pressed_event(void *p_keyscan_fifo_data)
{
    memcpy((KeyScan_Data *)&g_keyscan_data, (KeyScan_Data *)p_keyscan_fifo_data, sizeof(KeyScan_Data));

    // PRINT("key 0:%d\n", g_keyscan_data.key[0]);
    // PRINT("key 1:%d\n", g_keyscan_data.key[1]);
    // PRINT("press_time 0:%ld\n", g_key_data.key[0].press_time);
    // PRINT("press_time 1:%ld\n", g_key_data.key[1].press_time);
    //多少个按键按下
    if (g_keyscan_data.key_press_num == 0)
    {
        key_handle_release_event();
        return;
    }
    else if (g_keyscan_data.key_press_num == 1)
    {
        one_key_dis_type_to_do(g_keyscan_data.key[0]);
    }
    else if (g_keyscan_data.key_press_num == 2)
    {     
        two_key_dis_type_to_do(g_keyscan_data.key[0], g_keyscan_data.key[1]);
    }
}

/**
 * @brief 按键失能
 * @param  
 */
void key_disable(void)
{
    GPIO_INTConfig(GPIO_GetPin(KEY_IO0), DISABLE);//失能按键
    GPIO_INTConfig(GPIO_GetPin(KEY_IO1), DISABLE);
    GPIO_ClearINTPendingBit(GPIO_GetPin(KEY_IO0));
    GPIO_ClearINTPendingBit(GPIO_GetPin(KEY_IO1));
    
    g_key_data.key[0].press_time = 0;
    g_key_data.key[0].key = 0;
    g_key_data.key[1].press_time = 0;
    g_key_data.key[1].key = 0;
}

/******************************************************************
 * @brief  这是按键处理配对事件
 * @param  none
 * @return none
 */
void key_handle_process_repairing_event(void)
{
    app_global_data.pair_failed_retry_cnt = 0;
    switch (app_global_data.rcu_status)
    {
    case RCU_STATUS_IDLE:
        {
            rcu_start_adv(ADV_UNDIRECT_PAIRING);
        }
        break;
    case RCU_STATUS_ADVERTISING:
        {
            if (app_global_data.adv_type == ADV_UNDIRECT_PAIRING)
            {
                os_timer_restart(&adv_timer, ADV_UNDIRECT_PAIRING_TIMEOUT);
            }
            else
            {
                rcu_stop_adv(STOP_ADV_REASON_PAIRING);
            }
        }
        break;
    case RCU_STATUS_PAIRED:
        {
            rcu_terminate_connection(DISCONN_REASON_PAIRING);
        }
        break;
    default:
        /* do nothing */
        break;
    }
}
/******************* (C) COPYRIGHT 2020 Realtek Semiconductor Corporation *****END OF FILE****/
