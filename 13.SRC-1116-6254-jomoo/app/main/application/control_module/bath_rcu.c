/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     bath_rcu.c
* @brief
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "bath_rcu.h"
#include "driver.h"
#include "types.h"
#include "key_driver.h"

/**************************bath_rcu.c代码补缺区*********************************/

_attribute_data_retention_user u8 bath_send_need_f = 0;
_attribute_data_retention_user u8 device_channel = 0;

kb_data_t_aaa key_buf_aaa;


/******************************************************************
 * @brief  bath_rcu_CheckSum
 * @param  u8
 * @return none
 * @retval void
 */
u8 bath_rcu_CheckSum(u8 data[], int size)
{
    u8 checksum = 0;

    for (int i = 0; i < size; i++)
    {
        checksum += data[i];
    }

    return checksum;
}

/******************************************************************
 * @brief  Protocol_Byte2_Data
 * @param  u8
 * @return none
 * @retval void
 */
u8 Protocol_Byte2_Data(void)
{
    u8 data_byte2 = 0x00;

    data_byte2 = ((g_wash[now_mode].water_temperature.gear & 0x0F) << 4);
    data_byte2 |= (g_function.switch_gesture << 3);
    data_byte2 |= (g_function.switch_gesture << 2);
    data_byte2 |= (g_function.auto_flush << 1);
    data_byte2 |= (g_function.sensor_light << 0);
    return data_byte2;
}

/******************************************************************
 * @brief  Protocol_Byte3_Data
 * @param  u8
 * @return none
 * @retval void
 */
u8 Protocol_Byte3_Data(void)
{
    u8 data_byte3 = 0x00;

    data_byte3 = ((g_wash[now_mode].seat_position.gear & 0x0F) << 4);
    data_byte3 |= ((g_wash[now_mode].seat_temperature.gear & 0x0F) << 0);
    data_byte3 |= (g_function.power_save << 7);
    data_byte3 |= (0x00 << 3);
    return data_byte3;
}

/******************************************************************
 * @brief  Protocol_Byte4_Data
 * @param  u8
 * @return none
 * @retval void
 */
u8 Protocol_Byte4_Data(void)
{
    u8 data_byte4 = 0x00;
 
    data_byte4 = ((g_wash[now_mode].water_pressure.gear & 0x0F) << 4);  
    data_byte4 |= ((g_wash[now_mode].air_temperature.gear & 0x0F) << 0);
    data_byte4 |= (g_function.night_light << 7);
    data_byte4 |= (0x00 << 3);
    return data_byte4;
}

/******************************************************************
 * @brief  Send_Bath_24G_Data
 * @param  u8
 * @return none
 * @retval void
 */
void Send_Bath_24G_Data(u8 bath_cmd, u8 sys_1, u8 sys_2,u8 key_vaule)
{
    u8 RF_Data[6] = {0, 0, 0, 0, 0, 0};
    u8 cheak_data[4] = {0, 0, 0, 0};
    RF_Data[0] = PROTOCOL_HEAD;
    RF_Data[1] = bath_cmd;
    RF_Data[2] = Protocol_Byte2_Data();
    RF_Data[3] = Protocol_Byte3_Data();
    RF_Data[4] = Protocol_Byte4_Data();

    if (bath_cmd == CMD_SYSTEM_FUNCTION)
    {
        RF_Data[2] = sys_1;
        RF_Data[3] = sys_2;
        if ((sys_1 == 0xC7) && (sys_2 == 0x04))
            RF_Data[4] = set_fun_filter_element;
    }

    for (int i = 0; i < 4; i++)
        cheak_data[i] = RF_Data[i + 1];
    RF_Data[5] = bath_rcu_CheckSum(cheak_data, 4);

    memcpy(&rf_type.data_packet.cmd,&key_vaule , 1);
    memcpy(&rf_type.data_packet.km_dat[0], &RF_Data[0], 6);
    // my_fifo_push(&fifo_km, RF_Data, 6);

    // printf("--->>> [Send_Bath_24G_Data] ");
    // for(int i = 0; i < 6; i++)
    // printf(" %02X",p_bath_data->km_dat[i]);
    // printf("\r\n");

    device_channel = 0;
    bath_send_need_f = 2; // send data
}

#if 0
void Send_Pair_24G_Data()
{
    bath_send_need_f = 1;
}

/******************************************************************
 * @brief  Send_24G_Pair_Success
 * @param  u8
 * @return none
 * @retval void
 */
void Send_24G_Pair_Success(void)
{
    // printf("--->>> [work_debug] Send_24G_Pair_Success\r\n");
    u8 RF_Data[6] = {0, 0, 0, 0, 0, 0};
    u8 cheak_data[4] = {0, 0, 0, 0};
    RF_Data[0] = 0x44;
    RF_Data[1] = 0x45;
    RF_Data[2] = 0x0A;
    RF_Data[3] = 0x00;
    RF_Data[4] = 0x01;

    for (int i = 0; i < 4; i++)
        cheak_data[i] = RF_Data[i + 1];
    RF_Data[5] = bath_rcu_CheckSum(cheak_data, 4);
    my_fifo_push(&fifo_km, RF_Data, 6);
}

#define RCU_RF_24G_MODE 1
/* 2.4G SDK*/
#if RCU_RF_24G_MODE
// #include "AAA_public_Config.h"
_attribute_data_retention_user u32 loop_cnt;
_attribute_data_retention_user u32 idle_tick;
_attribute_data_retention_user u32 idle_count;
_attribute_data_retention_user u32 adv_count = 0;
_attribute_data_retention_user u32 adv_begin_tick;
void reset_idle_status()
{
    if (pair_flag)
    {
        return;
    }

    idle_count = 0;
    loop_cnt = 0;
    idle_tick = clock_time();
    // adv_begin_tick = idle_tick | 1;
    // adv_count = 0;
}

void adv_count_poll()
{
    u8 n;
    n = ((u32)(clock_time() - adv_begin_tick)) / CLOCK_16M_SYS_TIMER_CLK_1S;

    adv_begin_tick += n * CLOCK_16M_SYS_TIMER_CLK_1S;

    adv_count += n;
}

void enter_deep_aaa()
{
    // printf("--->>> [work_debug] enter_deep_aaa\r\n");
    need_suspend_flag = 1;
#if 0
    printf("enter deep\n");

	write_deep_ana0(DEEP_SLEEP_ANA_AAA);

/*
	for(int i=0;i<8;i++)
	{
		if(gpio_read(row_pins[i]))
		{
			cpu_set_gpio_wakeup(row_pins[i], 0, 1);
		}
		else
		{
			cpu_set_gpio_wakeup(row_pins[i], 1, 1);
		}
	 }
*/
	clear_pair_flag();
	
	//cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);

    cpu_sleep_wakeup(DEEPSLEEP_MODE_RET_SRAM_LOW32K, PM_WAKEUP_PAD, 0);
#endif
}

void idle_status_poll()
{
    u8 n;
    n = ((u32)(clock_time() - idle_tick)) / CLOCK_16M_SYS_TIMER_CLK_1S;

    idle_tick += n * CLOCK_16M_SYS_TIMER_CLK_1S;

    idle_count += n;
#if 0
    if (idle_count > 0xfffffff0)
    {
        idle_count = 0xfffffff0;
    }
#endif
}

#if 0
u8 need_enter_pair = 0;
void pair_process_aaa(void)
{
    static u32 tick = 0;
    if ((pair_flag == 0) && (key_buf_aaa.special_key_press_f == (T_F1_PRESS_AAA | T_ESC_PRESS_AAA)))
    {
        combination_flag = 1;
        if (clock_time_exceed(tick, 3000000))
        {
            need_enter_pair = 1;
        }
    }
    else
    {
        tick = clock_time();
    }

    if (need_enter_pair)
    {
        set_pair_flag();
        if (fun_mode == RF_1M_BLE_MODE)
        {
            if (connect_ok)
            {
                active_disconnect_reason = BLE_PAIR_REBOOT_ANA_AAA;
            }
            else
            {
                user_reboot(BLE_PAIR_REBOOT_ANA_AAA);
            }
        }
        else
        {
            d24_start_pair();
        }
    }
}
#endif

u8 kb_led_status;

#if (Microsoft_Swift_Pairing_ENABLE && (DEVICE_NAME_INCLUDE_MAC_DEBUG == 0))
#define DEV_POSITION 20
#define USER_DEVICE_NAME_MAX_LEN 11
_attribute_data_retention_user u8 tbl_advData_aaa[31] =
    {

        0x02,
        0x01,
        0x05, // BLE limited discoverable mode and BR/EDR not supported
        0x03,
        0x19,
        0xc1,
        0x03, // 384, Keyboard, Generic category,
        0x03,
        0x03,
        0x12,
        0x18, // incomplete list of service class UUIDs (0x1812-HID SERVICE, 180F-BATTERY)
        0x06,
        0xff,
        0x06,
        0x00,
        0x03,
        0x00,
        0x80,
        12,
        0x09,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
};
#endif

/******************************************************************
 * @brief  自动/停止(短)
 * @param  none
 * @return none
 * @retval void
 */
void pressed_auto_stop_key(void)
{
    // printf("--->>> [work_debug] pressed_auto_stop_key\r\n");
    //  keyScanLongPressTick = clock_time() | 1; //none zero
    //  LongPressTick = 3*1000*1000;
    //  Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_PAIRING;

    // led_position_water_temp();
    // if(app_bath_filter_element_timer == 0)
    //     Send_Bath_24G_Data(CMD_AUTO_STOP, 0x00, 0x00);
    // else
    //     app_bath_filter_element_timer = 0;
}

/******************************************************************
 * @brief  大冲(短)松开发码
 * @param  none
 * @return none
 * @retval void
 */
void pressed_flush_water_B_key(void)
{
    // printf("--->>> [work_debug] pressed_flush_water_B_key\r\n");
    if (tick_key_flush_water_b == 0x00)
    {
        // tick_key_flush_water_b = 0x01;
        // keyScanLongPressTick = clock_time() | 1; //none zero
        // if(set_fun_g_function.auto_flush == 0x01)
        //     LongPressTick = 8*1000*1000;
        // else
        //     LongPressTick = 3*1000*1000;
        // Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_KEYPRESS;
    }
}
#if 0
void pressed_flush_water_B_key_release(void)
{
    // printf("--->>> [work_debug] pressed_flush_water_B_key_release\r\n");
    if (tick_key_flush_water_b == 0x01) // short press < 3s / 8 s
    {
        rcu_device_led_save_last();
        Send_Bath_24G_Data(CMD_FLUSH_WATER_BIG, 0x00, 0x00);
    }
    else if (tick_key_flush_water_b == 0x02)
    {
        if (set_fun_g_function.auto_flush == 0x01)
        {
            set_fun_g_function.auto_flush = 0x00;
            rcu_device_led_g_function.auto_flush_close();
        }
        else
        {
            set_fun_g_function.auto_flush = 0x01;
            rcu_device_led_g_function.auto_flush_open();
        }
        Send_Bath_24G_Data(CMD_ADJUST_PARAMETER, 0x00, 0x00);
    }
    tick_key_flush_water_b = 0x00;
}
#endif

/******************************************************************
 * @brief  小冲(短)松开发码
 * @param  none
 * @return none
 * @retval void
 */
void pressed_flush_water_S_key(void)
{
    // printf("--->>> [work_debug] pressed_flush_water_S_key\r\n");
    if (tick_key_flush_water_s == 0x00)
        tick_key_flush_water_s = 0x01;
}
void pressed_flush_water_S_key_release(void)
{
    // printf("--->>> [work_debug] pressed_flush_water_S_key_release\r\n");
    if (tick_key_flush_water_s == 0x01)
    {
        rcu_device_led_save_last();
        //Send_Bath_24G_Data(CMD_FLUSH_WATER_SML, 0x00, 0x00);
    }
    tick_key_flush_water_s = 0x00;
}

/******************************************************************
 * @brief  翻盖(短)松开发码
 * @param  none
 * @return none
 * @retval void
 */
void pressed_flip_cover_key(void)
{
    // printf("--->>> [work_debug] pressed_flip_cover_key\r\n");
    if (tick_key_flip_cover == 0x00)
    {
        tick_key_flip_cover = 0x01;
        // keyScanLongPressTick = clock_time() | 1; //none zero
        // LongPressTick = 3*1000*1000;
        // Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_KEYPRESS;
    }
}
void pressed_flip_cover_key_release(void)
{
    // printf("--->>> [work_debug] pressed_flip_cover_key_release\r\n");
    if (tick_key_flip_cover == 0x01) // short press < 3s
    {
        rcu_device_led_save_last();
        //Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0x5E, 0x23);
    }
    else if (tick_key_flip_cover == 0x02)
    {
        if (set_fun_foot_feel == 0x01)
        {
            set_fun_foot_feel = 0x00;
            rcu_device_led_foot_feel_close();
            //Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0x24, 0x02);
        }
        else
        {
            set_fun_foot_feel = 0x01;
            rcu_device_led_foot_feel_open();
            //Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0x24, 0x01);
        }
    }
    tick_key_flip_cover = 0x00;
}

/******************************************************************
 * @brief  翻圈(短)松开发码
 * @param  none
 * @return none
 * @retval void
 */
void pressed_flip_circle_key(void)
{
    // printf("--->>> [work_debug] pressed_flip_circle_key\r\n");
    if (tick_key_flip_circle == 0x00)
    {
        tick_key_flip_circle = 0x01;
        // keyScanLongPressTick = clock_time() | 1; //none zero
        // LongPressTick = 3*1000*1000;
        // Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_KEYPRESS;
    }
}
void pressed_flip_circle_key_release(void)
{
    // printf("--->>> [work_debug] pressed_flip_circle_key_release\r\n");
    if (tick_key_flip_circle == 0x01) // short press < 3s
    {
        rcu_device_led_save_last();
        //Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0x5E, 0x24);
    }
    else if (tick_key_flip_circle == 0x02)
    {
        rcu_device_led_save_last();
       // Send_Bath_24G_Data(CMD_OUT_STINK, 0x00, 0x00);
    }
    tick_key_flip_circle = 0x00;
}

/******************************************************************
 * @brief  臀洗/宽幅(短)
 * @param  none
 * @return none
 * @retval void
 */
void pressed_buttock_wash_key(void)
{
    // printf("--->>> [work_debug] pressed_buttock_wash_key\r\n");
    led_position_water_temp();
    //Send_Bath_24G_Data(CMD_BUTTOCK_WASH, 0x00, 0x00);
}

/******************************************************************
 * @brief  妇洗/移动(短)松开发码
 * @param  none
 * @return none
 * @retval void
 */
void pressed_women_wash_key(void)
{
    // printf("--->>> [work_debug] pressed_women_wash_key\r\n");
    if (tick_key_women_wash == 0x00)
        tick_key_women_wash = 0x01;
}
void pressed_women_wash_key_release(void)
{
    // printf("--->>> [work_debug] pressed_women_wash_key_release\r\n");
    if (tick_key_women_wash == 0x01)
    {
        led_position_water_temp();
       // Send_Bath_24G_Data(CMD_WOMEN_WASH, 0x00, 0x00);
    }
    tick_key_women_wash = 0x00;
}

/******************************************************************
 * @brief  暖风(短)
 * @param  none
 * @return none
 * @retval void
 */
void pressed_warm_wind_key(void)
{
    // printf("--->>> [work_debug] pressed_warm_wind_key\r\n");
    led_position_wind_temp();
   // Send_Bath_24G_Data(CMD_WARM_WIND, 0x00, 0x00);
}

/******************************************************************
 * @brief  助便(短)
 * @param  none
 * @return none
 * @retval void
 */
void pressed_aid_defecate_key(void)
{
    // printf("--->>> [work_debug] pressed_aid_defecate_key\r\n");
    led_position_water_temp();
    //Send_Bath_24G_Data(CMD_AID_DEFECATE, 0x00, 0x00);
}

/******************************************************************
 * @brief  座温 ↑ (短)
 * @param  none
 * @return none
 * @retval void
 */
void pressed_seat_temp_up_key(void)
{
    // printf("--->>> [work_debug] pressed_seat_temp_up_key\r\n");
    if (set_pos_seat_temperature == POSITION_SEAT_TEMP_0)
        set_pos_seat_temperature = POSITION_SEAT_TEMP_1;
    else if (set_pos_seat_temperature == POSITION_SEAT_TEMP_1)
        set_pos_seat_temperature = POSITION_SEAT_TEMP_2;
    else if (set_pos_seat_temperature == POSITION_SEAT_TEMP_2)
        set_pos_seat_temperature = POSITION_SEAT_TEMP_3;
    else if (set_pos_seat_temperature == POSITION_SEAT_TEMP_3)
        set_pos_seat_temperature = POSITION_SEAT_TEMP_4;
    else if (set_pos_seat_temperature == POSITION_SEAT_TEMP_4)
        set_pos_seat_temperature = POSITION_SEAT_TEMP_5;
    led_position_seat_temp();
    //Send_Bath_24G_Data(CMD_ADJUST_PARAMETER, 0x00, 0x00);
}

/******************************************************************
 * @brief  座温 ↓ (短)松开发码
 * @param  none
 * @return none
 * @retval void
 */
void pressed_seat_temp_dn_key(void)
{
    // printf("--->>> [work_debug] pressed_seat_temp_dn_key\r\n");
    if (tick_key_seat_temp_dn == 0x00)
    {
        tick_key_seat_temp_dn = 0x01;
        // keyScanLongPressTick = clock_time() | 1; //none zero
        // LongPressTick = 3*1000*1000;
        // Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_KEYPRESS;
    }
}
void pressed_seat_temp_dn_key_release(void)
{
    // printf("--->>> [work_debug] pressed_seat_temp_dn_key_release\r\n");
    if (tick_key_seat_temp_dn == 0x01) // short press < 3s
    {
        if (set_pos_seat_temperature == POSITION_SEAT_TEMP_5)
            set_pos_seat_temperature = POSITION_SEAT_TEMP_4;
        else if (set_pos_seat_temperature == POSITION_SEAT_TEMP_4)
            set_pos_seat_temperature = POSITION_SEAT_TEMP_3;
        else if (set_pos_seat_temperature == POSITION_SEAT_TEMP_3)
            set_pos_seat_temperature = POSITION_SEAT_TEMP_2;
        else if (set_pos_seat_temperature == POSITION_SEAT_TEMP_2)
            set_pos_seat_temperature = POSITION_SEAT_TEMP_1;
        else if (set_pos_seat_temperature == POSITION_SEAT_TEMP_1)
            set_pos_seat_temperature = POSITION_SEAT_TEMP_0;
        led_position_seat_temp();
        //Send_Bath_24G_Data(CMD_ADJUST_PARAMETER, 0x00, 0x00);
    }
    else if (tick_key_seat_temp_dn == 0x02)
    {
        rcu_device_led_save_last();
       // Send_Bath_24G_Data(CMD_FEEL_TEMPERATURE, 0x00, 0x00);
    }
    tick_key_seat_temp_dn = 0x00;
}

/******************************************************************
 * @brief  水温 ↑ (短)
 * @param  none
 * @return none
 * @retval void
 */
void pressed_water_temp_up_key(void)
{
    // printf("--->>> [work_debug] pressed_water_temp_up_key\r\n");
    if (set_pos_water_temperature == POSITION_WATER_TEMP_0)
        set_pos_water_temperature = POSITION_WATER_TEMP_1;
    else if (set_pos_water_temperature == POSITION_WATER_TEMP_1)
        set_pos_water_temperature = POSITION_WATER_TEMP_2;
    else if (set_pos_water_temperature == POSITION_WATER_TEMP_2)
        set_pos_water_temperature = POSITION_WATER_TEMP_3;
    else if (set_pos_water_temperature == POSITION_WATER_TEMP_3)
        set_pos_water_temperature = POSITION_WATER_TEMP_4;
    else if (set_pos_water_temperature == POSITION_WATER_TEMP_4)
        set_pos_water_temperature = POSITION_WATER_TEMP_5;
    led_position_water_temp();
    //Send_Bath_24G_Data(CMD_ADJUST_PARAMETER, 0x00, 0x00);
}

/******************************************************************
 * @brief  水温 ↓ (短)松开发码
 * @param  none
 * @return none
 * @retval void
 */
void pressed_water_temp_dn_key(void)
{
    // printf("--->>> [work_debug] pressed_water_temp_dn_key\r\n");
    if (tick_key_water_temp_dn == 0x00)
    {
        tick_key_water_temp_dn = 0x01;
        // keyScanLongPressTick = clock_time() | 1; //none zero
        // LongPressTick = 3*1000*1000;
        // Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_KEYPRESS;
    }
}
void pressed_water_temp_dn_key_release(void)
{
    // printf("--->>> [work_debug] pressed_water_temp_dn_key_release\r\n");
    if (tick_key_water_temp_dn == 0x01) // short press < 3s
    {
        if (set_pos_water_temperature == POSITION_WATER_TEMP_5)
            set_pos_water_temperature = POSITION_WATER_TEMP_4;
        else if (set_pos_water_temperature == POSITION_WATER_TEMP_4)
            set_pos_water_temperature = POSITION_WATER_TEMP_3;
        else if (set_pos_water_temperature == POSITION_WATER_TEMP_3)
            set_pos_water_temperature = POSITION_WATER_TEMP_2;
        else if (set_pos_water_temperature == POSITION_WATER_TEMP_2)
            set_pos_water_temperature = POSITION_WATER_TEMP_1;
        else if (set_pos_water_temperature == POSITION_WATER_TEMP_1)
            set_pos_water_temperature = POSITION_WATER_TEMP_0;
        led_position_water_temp();
       // Send_Bath_24G_Data(CMD_ADJUST_PARAMETER, 0x00, 0x00);
    }
    else if (tick_key_water_temp_dn == 0x02)
    {
        if (set_fun_pre_humid == 0x01)
        {
            set_fun_pre_humid = 0x00;
          // Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0x30, 0x00);
        }
        else
        {
            set_fun_pre_humid = 0x01;
            //Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0x30, 0x04);
        }
        rcu_device_led_save_last();
    }
    tick_key_water_temp_dn = 0x00;
}

/******************************************************************
 * @brief  水压 ↑ (短)
 * @param  none
 * @return none
 * @retval void
 */
void pressed_water_pres_up_key(void)
{
    // printf("--->>> [work_debug] pressed_water_pres_up_key\r\n");
    if (tick_key_women_wash_fx)
    {
        if ((set_pos_water_pressure_FX == POSITION_WATER_PRESSURE_0) ||
            (set_pos_water_pressure_FX == POSITION_WATER_PRESSURE_1))
            set_pos_water_pressure_FX = POSITION_WATER_PRESSURE_2;
        else if (set_pos_water_pressure_FX == POSITION_WATER_PRESSURE_2)
            set_pos_water_pressure_FX = POSITION_WATER_PRESSURE_3;
        else if (set_pos_water_pressure_FX == POSITION_WATER_PRESSURE_3)
            set_pos_water_pressure_FX = POSITION_WATER_PRESSURE_4;
        else if (set_pos_water_pressure_FX == POSITION_WATER_PRESSURE_4)
            set_pos_water_pressure_FX = POSITION_WATER_PRESSURE_5;
        else if (set_pos_water_pressure_FX == POSITION_WATER_PRESSURE_5)
            set_pos_water_pressure_FX = POSITION_WATER_PRESSURE_6;
        led_position_water_pres();
       // Send_Bath_24G_Data(CMD_ADJUST_WASH, 0x00, 0x00);
        if (set_pos_water_pressure_FX == POSITION_WATER_PRESSURE_6)
            set_pos_water_pressure_FX = POSITION_WATER_PRESSURE_5;
    }
    else
    {
        if ((set_pos_water_pressure == POSITION_WATER_PRESSURE_0) ||
            (set_pos_water_pressure == POSITION_WATER_PRESSURE_1))
            set_pos_water_pressure = POSITION_WATER_PRESSURE_2;
        else if (set_pos_water_pressure == POSITION_WATER_PRESSURE_2)
            set_pos_water_pressure = POSITION_WATER_PRESSURE_3;
        else if (set_pos_water_pressure == POSITION_WATER_PRESSURE_3)
            set_pos_water_pressure = POSITION_WATER_PRESSURE_4;
        else if (set_pos_water_pressure == POSITION_WATER_PRESSURE_4)
            set_pos_water_pressure = POSITION_WATER_PRESSURE_5;
        else if (set_pos_water_pressure == POSITION_WATER_PRESSURE_5)
            set_pos_water_pressure = POSITION_WATER_PRESSURE_6;
        led_position_water_pres();
       // Send_Bath_24G_Data(CMD_ADJUST_WASH, 0x00, 0x00);
        if (set_pos_water_pressure == POSITION_WATER_PRESSURE_6)
            set_pos_water_pressure = POSITION_WATER_PRESSURE_5;
    }
}

/******************************************************************
 * @brief  水压 ↓ (短)松开发码
 * @param  none
 * @return none
 * @retval void
 */
void pressed_water_pres_dn_key(void)
{
    // printf("--->>> [work_debug] pressed_water_pres_dn_key\r\n");
    if (tick_key_water_pres_dn == 0x00)
    {
        tick_key_water_pres_dn = 0x01;
        // keyScanLongPressTick = clock_time() | 1; //none zero
        // LongPressTick = 3*1000*1000;
        // Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_KEYPRESS;
    }
}
void pressed_water_pres_dn_key_release(void)
{
    // printf("--->>> [work_debug] pressed_water_pres_dn_key_release\r\n");
    if (tick_key_water_pres_dn == 0x01) // short press < 3s
    {
        if (tick_key_women_wash_fx)
        {
            if ((set_pos_water_pressure_FX == POSITION_WATER_PRESSURE_6) ||
                (set_pos_water_pressure_FX == POSITION_WATER_PRESSURE_5))
                set_pos_water_pressure_FX = POSITION_WATER_PRESSURE_4;
            else if (set_pos_water_pressure_FX == POSITION_WATER_PRESSURE_4)
                set_pos_water_pressure_FX = POSITION_WATER_PRESSURE_3;
            else if (set_pos_water_pressure_FX == POSITION_WATER_PRESSURE_3)
                set_pos_water_pressure_FX = POSITION_WATER_PRESSURE_2;
            else if (set_pos_water_pressure_FX == POSITION_WATER_PRESSURE_2)
                set_pos_water_pressure_FX = POSITION_WATER_PRESSURE_1;
            else if (set_pos_water_pressure_FX == POSITION_WATER_PRESSURE_1)
                set_pos_water_pressure_FX = POSITION_WATER_PRESSURE_0;
            led_position_water_pres();
           // Send_Bath_24G_Data(CMD_ADJUST_WASH, 0x00, 0x00);
            if (set_pos_water_pressure_FX == POSITION_WATER_PRESSURE_0)
                set_pos_water_pressure_FX = POSITION_WATER_PRESSURE_1;
        }
        else
        {
            if ((set_pos_water_pressure == POSITION_WATER_PRESSURE_6) ||
                (set_pos_water_pressure == POSITION_WATER_PRESSURE_5))
                set_pos_water_pressure = POSITION_WATER_PRESSURE_4;
            else if (set_pos_water_pressure == POSITION_WATER_PRESSURE_4)
                set_pos_water_pressure = POSITION_WATER_PRESSURE_3;
            else if (set_pos_water_pressure == POSITION_WATER_PRESSURE_3)
                set_pos_water_pressure = POSITION_WATER_PRESSURE_2;
            else if (set_pos_water_pressure == POSITION_WATER_PRESSURE_2)
                set_pos_water_pressure = POSITION_WATER_PRESSURE_1;
            else if (set_pos_water_pressure == POSITION_WATER_PRESSURE_1)
                set_pos_water_pressure = POSITION_WATER_PRESSURE_0;
            led_position_water_pres();
           // Send_Bath_24G_Data(CMD_ADJUST_WASH, 0x00, 0x00);
            if (set_pos_water_pressure == POSITION_WATER_PRESSURE_0)
                set_pos_water_pressure = POSITION_WATER_PRESSURE_1;
        }
    }
    else if (tick_key_water_pres_dn == 0x02)
    {
        if (set_fun_night_light == 0x01)
            set_fun_night_light = 0x00;
        else
            set_fun_night_light = 0x01;
       // Send_Bath_24G_Data(CMD_ADJUST_PARAMETER, 0x00, 0x00);
        rcu_device_led_save_last();
    }
    tick_key_water_pres_dn = 0x00;
}

/******************************************************************
 * @brief  位置 ↑ (短)
 * @param  none
 * @return none
 * @retval void
 */
void pressed_seat_posi_up_key(void)
{
#if 0
    printf("--->>> [work_debug] pressed_seat_posi_up_key\r\n");
    if(app_bath_filter_element_timer == 0)
    {
        if(tick_key_women_wash_fx)
        {
            if((set_pos_seat_position_FX == POSITION_SEAT_0) ||\
            (set_pos_seat_position_FX == POSITION_SEAT_1)) 
                set_pos_seat_position_FX = POSITION_SEAT_2;
            else if(set_pos_seat_position_FX == POSITION_SEAT_2) 
                set_pos_seat_position_FX = POSITION_SEAT_3;
            else if(set_pos_seat_position_FX == POSITION_SEAT_3) 
                set_pos_seat_position_FX = POSITION_SEAT_4;
            else if(set_pos_seat_position_FX == POSITION_SEAT_4) 
                set_pos_seat_position_FX = POSITION_SEAT_5;
            else if(set_pos_seat_position_FX == POSITION_SEAT_5) 
                set_pos_seat_position_FX = POSITION_SEAT_6;
            rcu_device_led_save_last();
           // Send_Bath_24G_Data(CMD_ADJUST_WASH, 0x00, 0x00);
            if(set_pos_seat_position_FX == POSITION_SEAT_6) 
                set_pos_seat_position_FX = POSITION_SEAT_5;
        }
        else
        {
            if((set_pos_seat_position == POSITION_SEAT_0) ||\
            (set_pos_seat_position == POSITION_SEAT_1)) 
                set_pos_seat_position = POSITION_SEAT_2;
            else if(set_pos_seat_position == POSITION_SEAT_2) 
                set_pos_seat_position = POSITION_SEAT_3;
            else if(set_pos_seat_position == POSITION_SEAT_3) 
                set_pos_seat_position = POSITION_SEAT_4;
            else if(set_pos_seat_position == POSITION_SEAT_4) 
                set_pos_seat_position = POSITION_SEAT_5;
            else if(set_pos_seat_position == POSITION_SEAT_5) 
                set_pos_seat_position = POSITION_SEAT_6;
            rcu_device_led_save_last();
          //  Send_Bath_24G_Data(CMD_ADJUST_WASH, 0x00, 0x00);
            if(set_pos_seat_position == POSITION_SEAT_6) 
                set_pos_seat_position = POSITION_SEAT_5;            
        }

     }else
     {
         app_bath_filter_element_timer = clock_time() | TRUE;
         if(set_fun_filter_element == 1)
             set_fun_filter_element = 2;
        else if(set_fun_filter_element == 2)
             set_fun_filter_element = 3;
         else if(set_fun_filter_element == 3)
             set_fun_filter_element = 4;
     else if(set_fun_filter_element == 4)
            set_fun_filter_element = 5;
        rcu_device_led_filter_element();
       // Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0xC7, 0x04);
    }
#endif
}

/******************************************************************
 * @brief  位置 ↓ (短)松开发码
 * @param  none
 * @return none
 * @retval void
 */
void pressed_seat_posi_dn_key(void)
{
    // printf("--->>> [work_debug] pressed_seat_posi_dn_key\r\n");
    //  if(app_bath_filter_element_timer == 0)
    //  {
    //      if(tick_key_seat_posi_dn == 0x00)
    //      {
    //          tick_key_seat_posi_dn = 0x01;
    //          keyScanLongPressTick = clock_time() | 1; //none zero
    //          LongPressTick = 3*1000*1000;
    //          Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_KEYPRESS;
    //      }
    //  }else
    //  {
    //      app_bath_filter_element_timer = clock_time() | TRUE;
    //      if(set_fun_filter_element == 5)
    //          set_fun_filter_element = 4;
    //      else if(set_fun_filter_element == 4)
    //          set_fun_filter_element = 3;
    //      else if(set_fun_filter_element == 3)
    //          set_fun_filter_element = 2;
    //      else if(set_fun_filter_element == 2)
    //          set_fun_filter_element = 1;
    //      rcu_device_led_filter_element();
    //      Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0xC7, 0x04);
    //  }
}
void pressed_seat_posi_dn_key_release(void)
{
    // printf("--->>> [work_debug] pressed_seat_posi_dn_key_release\r\n");
    if (tick_key_seat_posi_dn == 0x01) // short press < 3s
    {
        if (tick_key_women_wash_fx)
        {
            if ((set_pos_seat_position_FX == POSITION_SEAT_6) ||
                (set_pos_seat_position_FX == POSITION_SEAT_5))
                set_pos_seat_position_FX = POSITION_SEAT_4;
            else if (set_pos_seat_position_FX == POSITION_SEAT_4)
                set_pos_seat_position_FX = POSITION_SEAT_3;
            else if (set_pos_seat_position_FX == POSITION_SEAT_3)
                set_pos_seat_position_FX = POSITION_SEAT_2;
            else if (set_pos_seat_position_FX == POSITION_SEAT_2)
                set_pos_seat_position_FX = POSITION_SEAT_1;
            else if (set_pos_seat_position_FX == POSITION_SEAT_1)
                set_pos_seat_position_FX = POSITION_SEAT_0;
            rcu_device_led_save_last();
           // Send_Bath_24G_Data(CMD_ADJUST_WASH, 0x00, 0x00);
            if (set_pos_seat_position_FX == POSITION_SEAT_0)
                set_pos_seat_position_FX = POSITION_SEAT_1;
        }
        else
        {
            if ((set_pos_seat_position == POSITION_SEAT_6) ||
                (set_pos_seat_position == POSITION_SEAT_5))
                set_pos_seat_position = POSITION_SEAT_4;
            else if (set_pos_seat_position == POSITION_SEAT_4)
                set_pos_seat_position = POSITION_SEAT_3;
            else if (set_pos_seat_position == POSITION_SEAT_3)
                set_pos_seat_position = POSITION_SEAT_2;
            else if (set_pos_seat_position == POSITION_SEAT_2)
                set_pos_seat_position = POSITION_SEAT_1;
            else if (set_pos_seat_position == POSITION_SEAT_1)
                set_pos_seat_position = POSITION_SEAT_0;
            rcu_device_led_save_last();
            //Send_Bath_24G_Data(CMD_ADJUST_WASH, 0x00, 0x00);
            if (set_pos_seat_position == POSITION_SEAT_0)
                set_pos_seat_position = POSITION_SEAT_1;
        }
    }
    else if (tick_key_seat_posi_dn == 0x02)
    {
       // Send_Bath_24G_Data(CMD_CLEAN_NOZZLE, 0x00, 0x00);
        rcu_device_led_save_last();
    }
    tick_key_seat_posi_dn = 0x00;
}

/******************************************************************
 * @brief  风温 ↑ (短)
 * @param  none
 * @return none
 * @retval void
 */
void pressed_wind_temp_up_key(void)
{
    // printf("--->>> [work_debug] pressed_wind_temp_up_key\r\n");
    if (set_pos_wind_temperature == POSITION_WIND_TEMP_0)
        set_pos_wind_temperature = POSITION_WIND_TEMP_1;
    else if (set_pos_wind_temperature == POSITION_WIND_TEMP_1)
        set_pos_wind_temperature = POSITION_WIND_TEMP_2;
    else if (set_pos_wind_temperature == POSITION_WIND_TEMP_2)
        set_pos_wind_temperature = POSITION_WIND_TEMP_3;
    led_position_wind_temp();
   // Send_Bath_24G_Data(CMD_ADJUST_PARAMETER, 0x00, 0x00);
}

/******************************************************************
 * @brief  风温 ↓ (短)松开发码
 * @param  none
 * @return none
 * @retval void
 */
void pressed_wind_temp_dn_key(void)
{
    // printf("--->>> [work_debug] pressed_wind_temp_dn_key\r\n");
    if (tick_key_wind_temp_dn == 0x00)
    {
        tick_key_wind_temp_dn = 0x01;
        // keyScanLongPressTick = clock_time() | 1; //none zero
        // LongPressTick = 3*1000*1000;
        // Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_KEYPRESS;
    }
}
void pressed_wind_temp_dn_key_release(void)
{
    // printf("--->>> [work_debug] pressed_wind_temp_dn_key_release\r\n");
    if (tick_key_wind_temp_dn == 0x01) // short press < 3s
    {
        if (set_pos_wind_temperature == POSITION_SEAT_3)
            set_pos_wind_temperature = POSITION_SEAT_2;
        else if (set_pos_wind_temperature == POSITION_SEAT_2)
            set_pos_wind_temperature = POSITION_SEAT_1;
        else if (set_pos_wind_temperature == POSITION_SEAT_1)
            set_pos_wind_temperature = POSITION_SEAT_0;
        led_position_wind_temp();
       // Send_Bath_24G_Data(CMD_ADJUST_PARAMETER, 0x00, 0x00);
    }
    else if (tick_key_wind_temp_dn == 0x02)
    {
        if (set_fun_atmosphere_light == 0x01)
        {
            set_fun_atmosphere_light = 0x00;
          //  Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0x33, 0x20);
        }
        else
        {
            set_fun_atmosphere_light = 0x01;
            //Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0x33, 0x10);
        }
        rcu_device_led_save_last();
    }
    tick_key_wind_temp_dn = 0x00;
}

/******************************************************************
 * @brief  长按"妇洗"+"大冲" 8s --- 滤芯复位
 * @param  none
 * @return none
 * @retval void
 */
void pressed_filter_element_reset(void)
{
    // printf("--->>> [work_debug] pressed_filter_element_reset\r\n");
    if (tick_key_women_wash_flush_water_b == 0x02)
    {
        rcu_device_led_all_05s();
        //Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0xC7, 0x01);
    }
    tick_key_women_wash_flush_water_b = 0x00;
}

/******************************************************************
 * @brief  长按"妇洗"+"小冲" 8s --- 滤芯停止
 * @param  none
 * @return none
 * @retval void
 */
void pressed_filter_element_stop(void)
{
    // printf("--->>> [work_debug] pressed_filter_element_stop\r\n");
    if (tick_key_women_wash_flush_water_s == 0x02)
    {
        rcu_device_led_all_05s();
       // Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0xC7, 0x02);
    }
    tick_key_women_wash_flush_water_s = 0x00;
}

/******************************************************************
 * @brief  长按"大冲"+"小冲" 8s --- 滤芯时间设定
 * @param  none
 * @return none
 * @retval void
 */
void pressed_filter_element_timeseting(void)
{
    // printf("--->>> [work_debug] pressed_filter_element_timeseting\r\n");
    if (tick_key_flush_water_b_flush_water_s == 0x02)
    {
        rcu_device_led_filter_element();
        //Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0xC7, 0x04);
        // app_bath_filter_element_timer = clock_time() | TRUE;
    }
    tick_key_flush_water_b_flush_water_s = 0x00;
}

#if 0
printf("--->>> [work_debug] C_volUP\n");
memset(keyboard_buf_aaa,0,8);
keyboard_buf_aaa[0] = C_volUP;
my_fifo_push(&fifo_km, keyboard_buf_aaa, 6);
keyboard_buf_aaa[0] = 0;
my_fifo_push(&fifo_km, keyboard_buf_aaa, 6);
#endif

u32 test_result;
u32 Test_AccessCode(u8 key_cnt)
{
    int combinations[24][4];
    int index = 0;

    int nums[] = {0x76, 0x51, 0x39, 0x95}; // 0x71
    // int nums[] = {0x71, 0x51, 0x39, 0x95};//0x76

    // int nums[] = {0x71, 0x76, 0x51, 0x39};//0x95

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (j == i)
                continue;

            for (int k = 0; k < 4; k++)
            {
                if (k == i || k == j)
                    continue;

                for (int l = 0; l < 4; l++)
                {
                    if (l == i || l == j || l == k)
                        continue;

                    combinations[index][0] = nums[i];
                    combinations[index][1] = nums[j];
                    combinations[index][2] = nums[k];
                    combinations[index][3] = nums[l];
                    index++;
                }
            }
        }
    }

    // 输出所有24种组合
    printf("--->>> [TEST] key_cnt == %d, Access Code: ", key_cnt);
    printf("%02x, %02x, %02x, %02x\n", combinations[key_cnt][0], combinations[key_cnt][1], combinations[key_cnt][2], combinations[key_cnt][3]);

    test_result = (combinations[key_cnt][0] << 24) | (combinations[key_cnt][1] << 16) | (combinations[key_cnt][2] << 8) | combinations[key_cnt][3];
    printf("test_result == 0x%04x\r\n", test_result);
    return test_result;
}

#if 0
/******************************************************************
 * @brief  Bath_Rcu_key0_Handle
 * @param  none
 * @return none
 * @retval void
 */
void Bath_Rcu_key0_Handle(u8 key0)
{
    if(app_bath_filter_element_timer == 0)
    {
        if(key0 == KEY_AUTO_STOP){
            tick_key_women_wash_fx = 0x00;
            p_bath_data->cmd = CMD_24G_AUTO_STOP;
            pressed_auto_stop_key();
            return;
        }
        if(key0 == KEY_FLUSH_WATER_B){
            p_bath_data->cmd = CMD_24G_FLUSH_WATER_B;
            pressed_flush_water_B_key();
            return;
        }
        if(key0 == KEY_FLUSH_WATER_S){
            p_bath_data->cmd = CMD_24G_FLUSH_WATER_S;
            pressed_flush_water_S_key();
            return;
        }
        if(key0 == KEY_FLIP_COVER){
            p_bath_data->cmd = CMD_24G_FLIP_COVER;
            pressed_flip_cover_key();
            return;
        }
        if(key0 == KEY_FLIP_CIRCLE){
            p_bath_data->cmd = CMD_24G_FLIP_CIRCLE;
            pressed_flip_circle_key();
            return;
        }
        if(key0 == KEY_BUTTOCK_WASH){
            tick_key_women_wash_fx = 0x00;
            p_bath_data->cmd = CMD_24G_BUTTOCK_WASH;
            pressed_buttock_wash_key();
            return;
        }
        if(key0 == KEY_WOMEN_WASH){
            tick_key_women_wash_fx = 0x01;
            p_bath_data->cmd = CMD_24G_WOMEN_WASH;
            pressed_women_wash_key();
            return;
        }
        if(key0 == KEY_WARM_WIND){
            p_bath_data->cmd = CMD_24G_WARM_WIND;
            pressed_warm_wind_key();
            return;
        }
        if(key0 == KEY_AID_DEFECATE){
            tick_key_women_wash_fx = 0x00;
            p_bath_data->cmd = CMD_24G_AID_DEFECATE;
            pressed_aid_defecate_key();
            return;
        }
        if(key0 == KEY_SEAT_TEMP_UP){
            p_bath_data->cmd = CMD_24G_SEAT_TEMP_UP;
            pressed_seat_temp_up_key();
            return;
        }
        if(key0 == KEY_SEAT_TEMP_DN){
            p_bath_data->cmd = CMD_24G_SEAT_TEMP_DN;
            pressed_seat_temp_dn_key();
            return;
        }
        if(key0 == KEY_WATER_TEMP_UP){
            p_bath_data->cmd = CMD_24G_WATER_TEMP_UP;
            pressed_water_temp_up_key();
            return;
        }
        if(key0 == KEY_WATER_TEMP_DN){
            p_bath_data->cmd = CMD_24G_WATER_TEMP_DN;
            pressed_water_temp_dn_key();
            return;
        }
        if(key0 == KEY_WATER_PRES_UP){
            p_bath_data->cmd = CMD_24G_WATER_PRES_UP;
            pressed_water_pres_up_key();
            return;
        }
        if(key0 == KEY_WATER_PRES_DN){
            p_bath_data->cmd = CMD_24G_WATER_PRES_DN;
            pressed_water_pres_dn_key();
            return;
        }
        if (key0 == KEY_WIND_TEMP_UP){
            p_bath_data->cmd = CMD_24G_WIND_TEMP_UP;
            pressed_wind_temp_up_key();
            return;
        }
        if (key0 == KEY_WIND_TEMP_DN){
            p_bath_data->cmd = CMD_24G_WIND_TEMP_DN;
            pressed_wind_temp_dn_key();
            return;
        }                    
    }
    if(key0 == KEY_AUTO_STOP){
        tick_key_women_wash_fx = 0x00;
        p_bath_data->cmd = CMD_24G_AUTO_STOP;
        pressed_auto_stop_key();
        return;
    }
    if(key0 == KEY_SEAT_POSI_UP){
        p_bath_data->cmd = CMD_24G_SEAT_POSI_UP;
        pressed_seat_posi_up_key();
        return;
    }
    if(key0 == KEY_SEAT_POSI_DN){
        p_bath_data->cmd = CMD_24G_SEAT_POSI_DN;
        pressed_seat_posi_dn_key();
        return;
    }
}

/******************************************************************
 * @brief  Bath_Rcu_key2_Handle
 * @param  none
 * @return none
 * @retval void
 */
void Bath_Rcu_key2_Handle(u8 key0, u8 key1)
{
    //妇洗 + 大冲
    if (((key0 == KEY_WOMEN_WASH) && (key1 == KEY_FLUSH_WATER_B)) || ((key0 == KEY_FLUSH_WATER_B) && (key1 == KEY_WOMEN_WASH)))
    {
        //printf("--->>> [work_debug] KEY_WOMEN_WASH + KEY_FLUSH_WATER_B : Filter Element Reset !!!\r\n");
        tick_key_women_wash_flush_water_b = 0x01;
        tick_key_flush_water_b = 0x00;
        tick_key_women_wash = 0x00;
        keyScanLongPressTick = clock_time() | 1; //none zero
        LongPressTick = 8*1000*1000;
        Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_KEYPRESS;
    }
    //妇洗 + 小冲
    else if (((key0 == KEY_WOMEN_WASH) && (key1 == KEY_FLUSH_WATER_S)) || ((key0 == KEY_FLUSH_WATER_S) && (key1 == KEY_WOMEN_WASH)))
    {
        //printf("--->>> [work_debug] KEY_WOMEN_WASH + KEY_FLUSH_WATER_S : Filter Element Stop !!!\r\n");
        tick_key_women_wash_flush_water_s = 0x01;
        tick_key_flush_water_s = 0x00;
        tick_key_women_wash = 0x00;
        keyScanLongPressTick = clock_time() | 1; //none zero
        LongPressTick = 8*1000*1000;
        Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_KEYPRESS;
    }
    //大冲 + 小冲
    else if (((key0 == KEY_FLUSH_WATER_B) && (key1 == KEY_FLUSH_WATER_S)) || ((key0 == KEY_FLUSH_WATER_S) && (key1 == KEY_FLUSH_WATER_B)))
    {
        //printf("--->>> [work_debug] KEY_FLUSH_WATER_B + KEY_FLUSH_WATER_S : Filter Element TimeSeting !!!\r\n");
        tick_key_flush_water_b_flush_water_s = 0x01;
        tick_key_flush_water_b = 0x00;
        tick_key_flush_water_s = 0x00;
        keyScanLongPressTick = clock_time() | 1; //none zero
        LongPressTick = 8*1000*1000;
        Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_KEYPRESS;
    }
}

#endif
#endif

/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
#endif