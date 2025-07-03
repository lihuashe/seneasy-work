

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include "app_function.h"
#include "app_task.h"
#include "voltage_driver.h"
#include "bct3236_driver.h"
#include "led_config.h"
#include "battery_driver.h"
#include "rtl876x_tim.h"


/*============================================================================*
 *                              Global Variables
 *============================================================================*/

Beauty_Comb_Mode g_beauty_comb = {0, 1, false};
bool power_is_on = false;
static bool five_min = false;
uint8_t paired_flag = 0;
/**
 * @description: 这是一个指定档位反转的函数
 * @param {u8} *status:是哪种状态
 * @param {u8} max:是开始反转最大值
 * @param {u8} min:是开始反转最小值
 * @param {bool} *revrse_flag：是记录反转方向的标志位
 * @return {*}
 */
void revrse_add_sub(uint8_t *status,uint8_t max,uint8_t min,bool *revrse_flag)
{
    if (*revrse_flag == 0)
    {
        if (*status < max)
            *status = *status + 1;
        if (*status >= max)
            *revrse_flag = 1;
    }
    else if (*revrse_flag == 1)
    {
        if (*status > min)
            *status= *status - 1;
        if (*status <= min)
            *revrse_flag = 0;
    }

}


/**
 * @brief 这是关机事件
 * @param key_value 
 */
void power_on_off_handle(uint8_t key_value)
{

    APP_PRINT_INFO0("power_on_off_handle\n");
    
    if (power_is_on == false)
    {
        //开机
        power_is_on = true;
        POWER_ON_OPEN();
        play_music_map(MUSIC_POWER_ON);
        //开背光
        ctrl_gear_key_backlit_on();
        ctrl_mode_key_backlit_on();
        //开始为一档
        led_system_ctrl(LED_NORMAL_WORK);
        voltage_mode_gear_change(g_beauty_comb.mode, g_beauty_comb.gear);

        os_timer_start(&power_auto_off_timer);//开启自动关机
    }
    else
    {
        all_sys_function_off();
        POWER_ON_OFF();//关机
    }
    
}

/**
 * @brief 这是按键触发配对事件
 * @param key_value 
 */
void power_and_gear_pair_ble_handle(uint8_t key_value)
{
    APP_PRINT_INFO0("power_on_and_gear_pair_ble_handle\n");
    //开机
    power_is_on = true;
    POWER_ON_OPEN();
    os_timer_start(&power_auto_off_timer);//开启自动关机
    play_music_map(MUSIC_PAIR);
    out_voltage_off();
    key_handle_process_repairing_event();
    os_delay(5);    //等待配对广播配置完毕
    led_system_ctrl(LED_BRIGHT_STOP);//关灯，关闭一些动作
    led_system_ctrl(LED_PAIRING_START);
}

/** 
 * @brief 这是开关按键处理FACE_MODE 和 SCALP_MODE 以及 MESO 组合的事件
 * @param key_value 
 */
void beauty_comb_mode_switch_handle(uint8_t key_value)
{
    play_music_map(MUSIC_KEY_CLICKED);

    if (g_beauty_comb.mode == SCALP_MODE)
    {
        g_beauty_comb.mode = FACE_MODE;
        PRINT("FACE_MODE\n");
    }
    else if (g_beauty_comb.mode == FACE_MODE)
    {
        g_beauty_comb.mode = SCALP_MODE;
        PRINT("SCALP_MODE\n");
    }
    else if (g_beauty_comb.mode == SCALP_MESO_MODE)
    {
        g_beauty_comb.mode = FACE_MESO_MODE;
        PRINT("FACE_MESO_MODE\n");
    }
    else if (g_beauty_comb.mode == FACE_MESO_MODE)
    {
        g_beauty_comb.mode = SCALP_MESO_MODE;
        PRINT("SCALP_MESO_MODE\n");
    }
    
    led_system_ctrl(LED_NORMAL_WORK);
    voltage_mode_gear_change(g_beauty_comb.mode, g_beauty_comb.gear);
}

/**
 * @brief 这是用档位按键切换MESO模式的函数
 * @param key_value 
 */
void beauty_comb_meso_mode_on_off_handle(uint8_t key_value)
{
    APP_PRINT_INFO0("beauty_comb_meso_mode_on_off_handle");
    play_music_map(MUSIC_KEY_CLICKED);

    if (g_beauty_comb.mode == SCALP_MODE)
    {
        g_beauty_comb.mode = SCALP_MESO_MODE;
        PRINT("SCALP_MESO_MODE\n");
    }
    else if (g_beauty_comb.mode == FACE_MODE)
    {
        g_beauty_comb.mode = FACE_MESO_MODE;
        PRINT("FACE_MESO_MODE\n");
    }
    else if (g_beauty_comb.mode == SCALP_MESO_MODE)
    {
        g_beauty_comb.mode = SCALP_MODE;
        PRINT("SCALP_MODE\n");
    }
    else if (g_beauty_comb.mode == FACE_MESO_MODE)
    {
        g_beauty_comb.mode = FACE_MODE;
        PRINT("FACE_MODE\n");
    }
    
    led_system_ctrl(LED_NORMAL_WORK);
    voltage_mode_gear_change(g_beauty_comb.mode, g_beauty_comb.gear);
    
}

/**
 * @brief 这是模式共用档位的的加减的函数
 * @param key_value 
 */
void beauty_comb_mode_gear_add_sub_handle(uint8_t key_value)
{  
    APP_PRINT_INFO0("beauty_comb_mode_gear_add_sub_handle");
    play_music_map(MUSIC_KEY_CLICKED);

    
    if (g_beauty_comb.gear < 5)
    {
        g_beauty_comb.gear++;
    }
    else if(g_beauty_comb.gear == 5)
    {
        g_beauty_comb.gear = 1; 
    }
    
    PRINT("g_beauty_comb.gear:%d\n", g_beauty_comb.gear);
    
    led_system_ctrl(LED_NORMAL_WORK);
    voltage_mode_gear_change(g_beauty_comb.mode, g_beauty_comb.gear);
}

/**
 * @brief 自动关机回调函数
 * @param pxTimer 
 */
void power_auto_off_timer_cb(TimerHandle_t pxTimer)
{
    APP_PRINT_INFO0("power_auto_off_timer_cb");
    if (five_min == true)
    {
        power_on_off_handle(2);
    }

    if (five_min == false)
    {
        play_music_map(MUSIC_WORK_TIME);
        five_min = true;
    }  
}



