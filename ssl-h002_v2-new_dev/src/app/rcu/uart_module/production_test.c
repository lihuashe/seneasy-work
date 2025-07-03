/**
*********************************************************************************************************
*               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     production_test.c
* @brief    
* @details
* @author   lihuashe
* @date     2024-12-18
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "board.h"
#include "trace.h"
#include "rtl876x_pinmux.h"
#include "os_timer.h"
#include "rtl876x.h"
#include "uart_driver.h"
#include "app_msg.h"
#include <stdarg.h>
#include "led_config.h"
#include "voltage_driver.h"
#include "production_test.h"
#include "battery_driver.h"
#include "ftl.h"
#include "single_tone.h"
#include <os_task.h>

static Production_Hv_Lv_Test voltage_aging;
bool production_flag = false;
void *product_test_task_handle = NULL;
Production_Aging_Test save_aging;
volatile bool continue_aging = false;

// 函数：比较两个数组是否相等
static inline bool compare_arrays(const uint8_t *array1, const uint8_t *array2, size_t length) {
    for (size_t i = 0; i < length; i++) {
        if (array1[i] != array2[i] && array1[i] != 0) {
			return false; // 如果有任何元素不相等，返回 false
        }
    }
    return true; // 所有元素相等，返回 true
}


/**
 * @brief 产测命令执行
 * @param cmd_arr 包含命令的数组
 * @param buf uart命令
 * @param buf_len uart命令长度
 */
void production_test_cmd_do(const Production_Test *cmd_arr, char *buf)
{
    uint8_t tmp = 0;
    //PRINT_ARRAY(buf, 8);

    if ((uint8_t)buf[0] != 0xAA || (uint8_t)buf[1] != 0x55) 
    {
        //PRINT("AA 55 02 %02X 01 %02X FE\n", buf[4], ~buf[4]);
        uint8_t data[] = {0xAA, 0x55, 0x02, 0x00, 0x01, 0x02, 0xFE};
        HEX_PRINT(data, sizeof(data));//码头校验
        return ;
    }
    for(uint8_t j = 0; j <= buf[2]; j++){
        tmp += buf[j + 2];
    }
    if(tmp != buf[buf[2] + 3] && buf[3] != 0x0E)//较验不成功
    { 
        uint8_t data[] = {0xAA, 0x55, 0x02, 0x00, 0x02, 0x02, 0xFE};
        HEX_PRINT(data, sizeof(data));
        return ;
    }
    
    if (production_flag == false && continue_aging == false)
    {
        production_flag = true;
        os_timer_stop(&bat_check_timer);
        lp4081h_stop_chg();
        led_system_ctrl(LED_BRIGHT_STOP);
    }
    
    for (int8_t i = 0; i < sizeof(test_cmd)/sizeof(Production_Test); i++)
    {
        // 逐位比较
        if (compare_arrays(cmd_arr[i].uart_cmd, (const uint8_t *)buf, sizeof(cmd_arr[i].uart_cmd))) 
        {
            if (cmd_arr[i].test_cmd_do != NULL) 
            {
                cmd_arr[i].test_cmd_do((uint8_t *)buf); // 调用命令处理函数
            }
    
            if(buf[3] <= 0x08)
            { 
                uint8_t data[] = {0xAA, 0x55, 0x02, buf[3], 0x00, 0x02 + buf[3], 0xFE};
                HEX_PRINT(data, sizeof(data));//指令执行成功
            }
            break; // 找到匹配后可以退出循环
        }
        
    }
}


/**
 * @brief 串口产测接口，处理产测数据
 * @param production_test_msg 产测CMD
 */
void production_test_uart_msg(T_IO_MSG production_test_msg)
{
    uint8_t rx_rx[32];
    UART_RX_BUF *rx_data = (UART_RX_BUF *)(production_test_msg.u.buf);

    memcpy((void *)rx_rx, (char *)(rx_data->rx_buf), sizeof(rx_rx));

    //PRINT_ARRAY(rx_rx, 30);
    APP_PRINT_INFO1("rx_rx : %s", rx_rx);
    production_test_cmd_do(test_cmd, (char *)rx_rx);
}



/**
 * @brief 产测任务
 * @param p_param 
 */
void product_test_task(void *p_param)
{
    T_IO_MSG io_msg;
    
    while(1)
    {
        if (os_msg_recv(product_test_io_queue_handle, &io_msg, 0xFFFF) == true)
        {        
            if (io_msg.type == IO_MSG_TYPE_UART)
            {
                if (io_msg.subtype == IO_MSG_SUBTYPE_PRODUCTION)
                {
                    production_test_uart_msg(io_msg);
                }
            }
        }     

    }

}


/**
 * @brief 创建产测任务
 * @param  
 */
void product_test_task_init(void)
{
    APP_PRINT_INFO0("product_test_task Init");
    
    if (product_test_task_handle == NULL)
        os_task_create(&product_test_task_handle, "product_test_task", product_test_task, 0, 512 * 2, 1);
}




/**
 * @brief 获取老化产测状态
 * @param cmd 
 */
void get_production_aging_test_result(uint8_t *cmd)
{

    switch (voltage_aging.volatile_mode)
    {
        case HV_TEST_MODE:
            {
                //PRINT("AA 55 02 33 %02X %02X FE\n",voltage_aging.test_status, ~voltage_aging.test_status);

                uint8_t data[] = {0xAA, 0x55, 0x02, 0x13, voltage_aging.test_status, 0x02 + 0x13 + voltage_aging.test_status, 0xFE};
                HEX_PRINT(data, sizeof(data));//指令执行成功
            }
            break;
        
        case LV_24V_TEST_MODE:
            {
                //PRINT("AA 55 02 33 %02X %02X FE\n",voltage_aging.test_status, ~voltage_aging.test_status);

                uint8_t data[] = {0xAA, 0x55, 0x02, 0x23, voltage_aging.test_status, 0x02 + 0x23 + voltage_aging.test_status, 0xFE};
                HEX_PRINT(data, sizeof(data));//指令执行成功
            }
            break;
        
        case LV_14V_TEST_MODE:
            {
                //PRINT("AA 55 02 33 %02X %02X FE\n",voltage_aging.test_status, ~voltage_aging.test_status);

                uint8_t data[] = {0xAA, 0x55, 0x02, 0x33, voltage_aging.test_status, 0x02 + 0x33 + voltage_aging.test_status, 0xFE};
                HEX_PRINT(data, sizeof(data));//指令执行成功
            }
            break;
        
        default:
            break;
    }  

}

/**
 * @brief 退出老化测试
 * @param cmd 
 */
void exit_production_aging_test(uint8_t *cmd)
{
    Production_Aging_Test save_aging_data = {0, 0};
    ftl_save(&save_aging_data, FTL_PRODUCT_TEST_ADDR, sizeof(save_aging_data));//清空保存的数据
    
    voltage_aging.test_status = NO_AGING_TEST;
    led_system_ctrl(LED_BRIGHT_STOP);
    out_voltage_off();
    continue_aging = false;
    POWER_ON_OFF();//关机
}

/**
 * @brief 设置模式开启电压
 * @param mode 老化模式
 * @param test_time 老化模式的时间h/min
 */
void set_voltage_mode(uint8_t mode, uint32_t test_time)
{
    voltage_aging.volatile_mode = mode;
    voltage_aging.test_time = test_time;
    voltage_mode_gear_change(mode, 5);
    os_timer_restart(&production_hv_lv_aging_timer, test_time);
}

void aging_cmd_check(uint8_t cmd)
{
    led_system_ctrl(LED_BRIGHT_STOP);
    delete_usb_chg_task();//进入老化后用电池供电也不退出

    switch (cmd)
    {
        case HV_1800V_AGING_HOUR:
            {
                if (voltage_aging.test_status == AGING_TEST_END)
                {
                    out_voltage_off();
                    
                    uint8_t led_arr[] = {7, 8, 9, 20, 21, 22, 23, 24, 25};
                    bct3236_set_led_group(led_arr, sizeof(led_arr), 0xff, 0x01);
                    ctrl_gear_key_backlit_on();//背光
                    ctrl_mode_key_backlit_on();
                }
                else
                {
                    uint8_t led_arr[] = {WHITE_LED_GEAR_3, WHITE_LED_GEAR_4, WHITE_LED_GEAR_5};
                    bct3236_set_led_group(led_arr, sizeof(led_arr), 0xff, 0x01);
                    
                    set_voltage_mode(HV_TEST_MODE, AGING_HOUR);
                }
            }  
            break;

        case HV_1800V_AGING_MIN:
            {   
                if (voltage_aging.test_status == AGING_TEST_END)
                {
                    out_voltage_off();

                    uint8_t led_arr[] = {7, 8, 9, 20, 21, 22, 23, 24};
                    bct3236_set_led_group(led_arr, sizeof(led_arr), 0xff, 0x01);
                    ctrl_gear_key_backlit_on();//背光
                    ctrl_mode_key_backlit_on();
                }
                else
                {
                    uint8_t led_arr[] = {WHITE_LED_GEAR_3, WHITE_LED_GEAR_4, WHITE_LED_GEAR_5};         //档位
                    bct3236_set_led_group(led_arr, sizeof(led_arr), 0xff, 0x01);
                    ctrl_gear_key_backlit_on();//背光
                    ctrl_mode_key_backlit_on();

                    set_voltage_mode(HV_TEST_MODE, AGING_MIN);
                }
            }  
            break;

        case LV_24V_AGING_HOUR:
            {
                if (voltage_aging.test_status == AGING_TEST_END)
                {
                    out_voltage_off();

                    uint8_t led_arr[] = {7, 8, 20, 21, 22, 23};
                    bct3236_set_led_group(led_arr, sizeof(led_arr), 0xff, 0x01);
                    ctrl_gear_key_backlit_on();//背光
                    ctrl_mode_key_backlit_on();
                }
                else
                {
                    uint8_t led_arr[] = {WHITE_LED_GEAR_3, WHITE_LED_GEAR_4};
                    bct3236_set_led_group(led_arr, sizeof(led_arr), 0xff, 0x01);

                    set_voltage_mode(LV_24V_TEST_MODE, AGING_HOUR);
                }
            } 
        break;

        case LV_24V_AGING_MIN:
            {
                if (voltage_aging.test_status == AGING_TEST_END)
                {
                    out_voltage_off();

                    uint8_t led_arr[] = {7, 8, 20, 21, 22};
                    bct3236_set_led_group(led_arr, sizeof(led_arr), 0xff, 0x01);
                    ctrl_gear_key_backlit_on();//背光
                    ctrl_mode_key_backlit_on();
                }
                else
                {
                    uint8_t led_arr[] = {WHITE_LED_GEAR_3, WHITE_LED_GEAR_4};
                    bct3236_set_led_group(led_arr, sizeof(led_arr), 0xff, 0x01);
                    ctrl_gear_key_backlit_on();//背光
                    ctrl_mode_key_backlit_on();

                    set_voltage_mode(LV_24V_TEST_MODE, AGING_MIN);
                }
            }   
        break;

        case LV_14V_AGING_HOUR:
            {
                if (voltage_aging.test_status == AGING_TEST_END)
                {
                    out_voltage_off();

                    uint8_t led_arr[] = {7, 20, 21};
                    bct3236_set_led_group(led_arr, sizeof(led_arr), 0xff, 0x01);
                    ctrl_gear_key_backlit_on();//背光
                    ctrl_mode_key_backlit_on();
                }
                else
                {
                    uint8_t led_arr[] = {WHITE_LED_GEAR_3};
                    bct3236_set_led_group(led_arr, sizeof(led_arr), 0xff, 0x01);

                    set_voltage_mode(LV_14V_TEST_MODE, AGING_HOUR);
                }
            }
        break;

        case LV_14V_AGING_MIN:
            {
                if (voltage_aging.test_status == AGING_TEST_END)
                {
                    out_voltage_off();

                    uint8_t led_arr[] = {7, 20};
                    bct3236_set_led_group(led_arr, sizeof(led_arr), 0xff, 0x01);
                    ctrl_gear_key_backlit_on();//背光
                    ctrl_mode_key_backlit_on();
                }
                else
                {
                    uint8_t led_arr[] = {WHITE_LED_GEAR_3};
                    bct3236_set_led_group(led_arr, sizeof(led_arr), 0xff, 0x01);
                    ctrl_gear_key_backlit_on();//背光
                    ctrl_mode_key_backlit_on();

                    set_voltage_mode(LV_14V_TEST_MODE, AGING_MIN);
                }
                
            }  
        break;
        
        default:
            break;
    }
    
}

/**
 * @brief 开始老化测试
 * @param cmd 测试指令
 */
void production_aging_test(uint8_t *cmd)
{
    Production_Hv_Lv_Test init_aginng = INIT_PRODUCTION_HV_LV_TEST();
    voltage_aging = init_aginng;

    if ((cmd[3] & 0x0F) == 0x02)
    {
        save_aging.aging_loop_count = voltage_aging.aging_loop_count = cmd[4] * 6;//十分钟
    }
    else
    {
        save_aging.aging_loop_count = voltage_aging.aging_loop_count = cmd[4];//小时
    }
    
    
    voltage_aging.test_status = AGING_TESTING;
    
    save_aging.test_cmd = voltage_aging.test_cmd = cmd[3];

    //PRINT(" production_aging_test\n");

    aging_cmd_check(cmd[3]);
    
    uint32_t status = ftl_save(&save_aging, FTL_PRODUCT_TEST_ADDR, sizeof(save_aging));
    //PRINT("status = %d  len = %d \n", status, sn_len);// 成功  
    //PRINT("AA 55 02 %02X 02 %02X FE\n", cmd[4], ~cmd[4]);
    uint8_t data[] = {0xAA, 0x55, 0x02, cmd[3], 0x00, cmd[2] + cmd[3], 0xFE};
    HEX_PRINT(data, sizeof(data));//指令执行成功
}


/**
 * @brief 上电检测是否处于老化未完成
 * @param  
 */
void seq_on_check_continue_aging(void)
{
    Production_Aging_Test data;

    ftl_load(&data, FTL_PRODUCT_TEST_ADDR, sizeof(data));

    if (data.aging_loop_count != 0)
    {
        all_sys_function_off();
        POWER_ON_OPEN();
        continue_aging = true;

        product_test_task_init();
        voltage_aging.aging_loop_count = data.aging_loop_count;
        voltage_aging.test_cmd = data.test_cmd;
        aging_cmd_check(voltage_aging.test_cmd);    
    }
}

/**
 * @brief 作为老化测试的定时器回调
 * @param pxTimer 
 */
void production_aging_test_timer_cb(TimerHandle_t pxTimer)
{
    // uint8_t data[] = {voltage_aging.test_status, voltage_aging.now_aging_loop_count, voltage_aging.aging_loop_count};
    // HEX_PRINT(data, sizeof(data));//指令执行成功

    if (voltage_aging.test_status == AGING_TESTING)
    {
        led_system_ctrl(LED_BRIGHT_STOP);
        ctrl_gear_key_backlit_on();//背光
        ctrl_mode_key_backlit_on();
        voltage_aging.test_status = AGING_TEST_STOP;
        voltage_aging.now_aging_loop_count++;
        
        out_voltage_off();

        if (voltage_aging.test_time == AGING_HOUR)
        {
            os_timer_restart(&production_hv_lv_aging_timer, AGING_HOUR_STOP);
        }
        else if (voltage_aging.test_time == AGING_MIN)
        {
            os_timer_restart(&production_hv_lv_aging_timer, AGING_MIN_STOP);
        }
        
        if (voltage_aging.now_aging_loop_count >= voltage_aging.aging_loop_count)
        {
            Production_Aging_Test save_aging_data = {0, 0};
            ftl_save(&save_aging_data, FTL_PRODUCT_TEST_ADDR, sizeof(save_aging_data));//清空保存的数据

            os_timer_stop(&production_hv_lv_aging_timer);
            voltage_aging.test_status = AGING_TEST_END;
            aging_cmd_check(voltage_aging.test_cmd);
        }
    }
    else if (voltage_aging.test_status == AGING_TEST_STOP)
    {
        voltage_aging.test_status = AGING_TESTING;
        
        aging_cmd_check(voltage_aging.test_cmd);
    }
    
}


/**
 * @brief 开关美容红灯
 * @param cmd 
 */
static void beauty_red_led_on_off(uint8_t *cmd)
{
    
    if (cmd[5] == 0x03)
    {
        ctrl_red_led_all_off();
    }
    else if (cmd[5] == 0x04)
    {
        ctrl_red_led(0xff);
    }

}


/**
 * @brief 开关输出电压
 * @param cmd 
 */
static void test_all_voltage_on_off(uint8_t *cmd)
{
    switch (cmd[3])
    {
        case 0x0A:                                                           
            {
                if (cmd[4] == 0x00)
                {
                    out_voltage_off();
                }
                else if (cmd[4] == 0x01)
                {
                    voltage_mode_gear_change(HV_TEST_MODE, 5);
                }
            }
            break;
        
        case 0x0B:
            {
                if (cmd[4] == 0x00)
                {
                    out_voltage_off();
                }
                else if (cmd[4] == 0x01)
                {
                    voltage_mode_gear_change(LV_24V_TEST_MODE, 5);
                }
            }
            break;
        
        case 0x0C:
            {
                if (cmd[4] == 0x00)
                {
                    out_voltage_off();
                }
                else if (cmd[4] == 0x01)
                {
                    voltage_mode_gear_change(LV_14V_TEST_MODE, 5);
                }
            }
            break;
        
        default:
            break;

    }

    uint8_t data[] = {0xAA, 0x55, 0x02, cmd[3], 0x00, 0x02 + cmd[3], 0xFE};
    HEX_PRINT(data, sizeof(data));
}


/**
 * @brief 蜂鸣器关闭
 * @param cmd 
 */
static void test_buzzer_off(uint8_t *cmd)
{
    buzzer_stop();
}

/**
 * @brief 设置蜂鸣器的频率
 * @param cmd 
 */
static void test_buzzer_set_on(uint8_t *cmd)
{
    uint8_t zz = 0x03 + 0x06 + cmd[4] + cmd[5];
    //PRINT("test_buzzer_set_on\n");
    if (zz == cmd[6])
    {
        if (cmd[4] == 0x01)
        {
            MusicBuzzer test_buzzer_500hz = {1, {2000, cmd[5] * 1000}};
            play_one_music(&test_buzzer_500hz);      
        }
        else if (cmd[4] == 0x02)
        {
            MusicBuzzer test_buzzer_1000hz = {1, {1000, cmd[5] * 1000}};
            play_one_music(&test_buzzer_1000hz);     
        }

    }
 
}


/**
 * @brief 获取固件版本
 * @param cmd 
 */
static void get_firmware_version(uint8_t *cmd)
{
    uint8_t version[2] = {FIRMWARE_VERSION};

    //PRINT("AA 55 03 0C %02X %02X 0E FE\n",version[0], version[1]);
    uint8_t data[] = {0xAA, 0x55, 0x03, 0x0C, version[0], version[1], 0x03 + 0x0C + version[0] + version[1], 0xFE};
    HEX_PRINT(data, sizeof(data));
}

/**
 * @brief 获取充电状态
 * @param cmd 
 */
static void get_chg_status(uint8_t *cmd)
{
    uint8_t bat_status = lp4081h_get_bat_status();
    
    if ((bat_status & LP4081H_CHG_DONE) == LP4081H_CHG_DONE)
    {
        //PRINT("AA 55 02 09 07 12 FE\n");
        uint8_t data[] = {0xAA, 0x55, 0x02, 0x09, 0x07, 0x12, 0xFE};
        HEX_PRINT(data, sizeof(data));
    }
    else if ((bat_status & LP4081H_CONST_CHG) == LP4081H_CONST_CHG)
    {
        //PRINT("AA 55 02 09 05 10 FE\n");
        uint8_t data[] = {0xAA, 0x55, 0x02, 0x09, 0x05, 0x10, 0xFE};
        HEX_PRINT(data, sizeof(data));
    }
    else if ((bat_status & LP4081H_TRICKLE_CHG) == LP4081H_TRICKLE_CHG)
    {
        //PRINT("AA 55 02 09 02 0D FE\n");
        uint8_t data[] = {0xAA, 0x55, 0x02, 0x09, 0x02, 0x0D, 0xFE};
        HEX_PRINT(data, sizeof(data));
    }
    else if ((bat_status & LP4081H_NO_CHG) == LP4081H_NO_CHG)
    {
        //PRINT("AA 55 02 09 00 0B FE\n");
        uint8_t data[] = {0xAA, 0x55, 0x02, 0x09, 0x00, 0x0B, 0xFE};
        HEX_PRINT(data, sizeof(data));
    }
   
}


/**
 * @brief 获取电池电量
 * @param cmd 
 */
static void get_bat_dianliang(uint8_t *cmd)
{
    uint16_t bat_value;
    bat_pluse_value_info();

    bat_value = bat_get_current_voltage_value();
    
    
    uint8_t bat_v = bat_value / 20;
    uint8_t xx = 2 + 0x0b + 0 + bat_v;
    
    //PRINT("bat_value : %d \n", bat_value);
    uint8_t data[] = {0xAA, 0x55, 0x02, 0x0B, bat_v, xx, 0xFE};
    HEX_PRINT(data, sizeof(data));
}



/**
 * @brief 产测设置档位模式
 * @param cmd 
 */
static void set_mode_gear(uint8_t *cmd)
{ 
    uint8_t zz = 0x03 + 0x07 + cmd[4] + cmd[5];
    
    if (zz == cmd[6])
    {
        ctrl_mode_led(cmd[4]);
        ctrl_gear_led(cmd[5] + 1);
        voltage_mode_gear_change(cmd[4], cmd[5] + 1);
    }
 
}


/**
 * @brief 设置sn
 * @param cmd 
 */
static void set_sn_num(uint8_t *cmd)
{
    uint8_t save_sn[32];
    uint8_t sn_len = ((cmd[2] ) / 4 + 1) * 4 ;

    if (cmd[2] <= 30)
    {
        memcpy(save_sn, &cmd[2], sn_len);
        
        uint32_t  status = ftl_save(save_sn, FTL_DEVICE_SN_ADDR, sn_len);

        //PRINT("status = %d  len = %d \n", status, sn_len);// 成功       
        uint8_t data[] = {0xAA, 0x55, 0x02, 0x0E, 0x02, 0x12, 0xFE};
        HEX_PRINT(data, sizeof(data));
    }

}

/**
 * @brief 获取sn码值
 * @param cmd 
 */
static void get_sn_num(uint8_t *cmd)
{
    uint8_t save_sn[32];

    if (cmd[4] <= 30)
    {
        ftl_load(save_sn, FTL_DEVICE_SN_ADDR, FTL_DEVICE_SN_LEN);

        uint8_t data1[] = {0xAA, 0x55, save_sn[0], 0x0F};
        HEX_PRINT(data1, sizeof(data1));

        for (uint8_t i = 0; i < cmd[4]; i++) 
        { 
            HEX_PRINT(&save_sn[2 + i], 1); 
        }  

        uint8_t data2[] = {0xB2, 0xFE};
        HEX_PRINT(data2, sizeof(data2));
    }
}


/**
 * @brief 按键背光灯
 * @param cmd 
 */
static void key_backlit_led(uint8_t *cmd)
{
    if (cmd[5] == 4)
    {
        ctrl_gear_key_backlit_off();
        ctrl_mode_key_backlit_off();
    }
    else if (cmd[5] == 5)
    {
        ctrl_gear_key_backlit_on();
        ctrl_mode_key_backlit_on();
    }

}


/**
 * @brief 控制档位led
 * @param cmd 
 */
static void gear_ctrl_led(uint8_t *cmd)
{
    if (cmd[5] == 6)
    {
        ctrl_gear_led_all_off();
    }
    else if (cmd[5] == 7)
    {
        ctrl_gear_led(5);
    }
}


/**
 * @brief 控制模式led
 * @param cmd 
 */
static void mode_ctrl_led(uint8_t *cmd)
{
    if (cmd[5] == 7)
    {
        ctrl_mode_led_all_off();
    }
    else if (cmd[5] == 8)
    {
        ctrl_mode_led_all_on();
    }

}

/**
 * @brief 充电红灯显示
 * @param cmd 
 */
static void test_chg_red_led(uint8_t *cmd)
{
    if (cmd[5] == 5)
    {
        os_timer_stop(&led_tick_timer);
        ctrl_chg_red_led_off();
    }
    else if (cmd[5] == 6)
    {
        ctrl_chg_red_led_set(0xff);
    }

}

/**
 * @brief 关机
 * @param cmd 
 */
static void test_power_off(uint8_t *cmd)
{
    led_system_ctrl(LED_BRIGHT_STOP);
    out_voltage_off();
    POWER_ON_OFF();//关机
}

/**
 * @brief 单载波测试
 * @param cmd 
 */
static void test_single_tone(uint8_t *cmd)
{
    uint8_t single_tone_timeout = 0;
    
    if (cmd[3] == 0)
    {
        single_tone_timeout = 1;
    }
    else
    {
        single_tone_timeout = cmd[4];
    }

    HEX_PRINT(cmd, 7);

    set_single_tone_init(single_tone_timeout * 100);
}

