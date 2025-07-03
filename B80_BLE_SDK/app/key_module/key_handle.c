/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     key_handle.c
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
#include "key_handle.h"
#include "key_driver.h"
#include "../app.h"
#include "../application/keyboard/keyboard.h"
#include "../keyboard_rcu.h"
#include "../ir_module/ir_driver.h"
#include "../sensor_module/da217_driver.h"
#include "../pa_module//gsr2501_driver.h"
#include "../charge_module/lp4081b_driver.h"
#include "../factory_module/factory_profile.h"
#include "../led_module/led_driver.h"
#include "../touch_module/ft3308_driver.h"
#include "../touch_module/ft3308_factory.h"

#if (UI_KEYBOARD_ENABLE)
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
#if (STUCK_KEY_PROCESS_ENABLE)
    extern u32 stuckkey_keypresstimer;
#endif
extern u32 ota_program_offset;
u8 rcu_key_normal_flag = 0;
u8 rcu_key_normal_cnt = 0;
u8 rcu_key_power_flag = 0;
u8 rcu_key_factory_flag = 0;
/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief  key_handle_pressed_1_event
 * @param  none
 * @return none
 * @retval void
 */
void key_handle_pressed_1_event(void)
{   
    u8 key0 = kb_event.keycode[0];
    printf("--->>> [work_debug] key_pressed_1 - key0: S%d\r\n",key0);
    Accessibility_Shortcut_Flag = 0x55;
    rcu_key_normal_flag = 1;
    rcu_key_normal_cnt = key0;

#if (STUCK_KEY_PROCESS_ENABLE)
    if(stuckkey_keypresstimer == 0)
        stuckkey_keypresstimer = clock_time() | 1;
#endif

    rf_24G_deep_tick = clock_time();

#if(FACTORY_ENABLE)
    if(factory_mode_flag)
    {
        if(key0 == KEY_BACK)
        {
            if(pair_flag)
            {
                app_rcu_24g_pair_timer = 0; 
                rf_24G_deep_tick = clock_time();
                app_led_blink_timer = 0;
                pair_flag=0;
                analog_write(DEEP_ANA_REG1,0);      
                gpio_write(GPIO_LED_2,!LED_ON_LEVAL);  
                factory_mode_flag = 0x00;      
                app_factory_channel_flag = 0;  
                device_channel = 0; 
            }
        }
        if(factory_mode_flag == 0x01)
            return;

        if(factory_Gsensor_Flag == 0x00)
            ir_dispatch(TYPE_IR_SEND, TRUE, key0);
        else if(factory_Gsensor_Flag == 0x01)
        {
            if(Face_up_flag)
            {
                if(key0 == KEY_OK               || key0 == KEY_UP           || key0 == KEY_DOWN         || key0 == KEY_LEFT         || key0 == KEY_RIGHT            || \
                   key0 == KEY_BACK             || key0 == KEY_DB_WINDOWS   || key0 == KEY_SEL_BUTTON   || key0 == KEY_FLAME        || key0 == KEY_FREEZE_SCREEN    || \
                   key0 == KEY_DISABLE_TOUCH    || key0 == KEY_VOLUME_UP    || key0 == KEY_VOLUME_DOWN  || key0 == KEY_INPUT_SELECT || key0 == KEY_SETTING          || \
                   key0 == KEY_MOUSE_L          || key0 == KEY_MOUSE_R)
                    ir_dispatch(TYPE_IR_SEND, TRUE, key0);          
            }else
            {
                if(key0 == KEY_OK               || key0 == KEY_UP           || key0 == KEY_DOWN         || key0 == KEY_LEFT         || key0 == KEY_RIGHT            || \
                   key0 == KEY_BACK             || key0 == KEY_DB_WINDOWS   || key0 == KEY_SEL_BUTTON   || key0 == KEY_FLAME        || key0 == KEY_FREEZE_SCREEN    || \
                   key0 == KEY_DISABLE_TOUCH    || key0 == KEY_VOLUME_UP    || key0 == KEY_VOLUME_DOWN  || key0 == KEY_INPUT_SELECT || key0 == KEY_SETTING          || \
                   key0 == KEY_MOUSE_L          || key0 == KEY_MOUSE_R){
                }else
                    ir_dispatch(TYPE_IR_SEND, TRUE, key0);                     
            }
        }
        return;
    }
#endif

    if(Face_up_flag)
    {
        if(key0 == KEY_OK               || key0 == KEY_UP           || key0 == KEY_DOWN         || key0 == KEY_LEFT         || key0 == KEY_RIGHT            || \
           key0 == KEY_BACK             || key0 == KEY_DB_WINDOWS   || key0 == KEY_SEL_BUTTON   || key0 == KEY_FLAME        || key0 == KEY_FREEZE_SCREEN    || \
           key0 == KEY_DISABLE_TOUCH    || key0 == KEY_VOLUME_UP    || key0 == KEY_VOLUME_DOWN  || key0 == KEY_INPUT_SELECT || key0 == KEY_SETTING)
        {
            printf("--->>> [GSensor] Face_up_flag: 1 UP --- IR Key\r\n");
            //Enter 24G-Pair
            if (key0 == KEY_INPUT_SELECT)
            {
                keyScanLongPressTick = clock_time() | 1; //none zero
                LongPressTick = 3*1000*1000;
                Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_PAIRING;
            }
            #if (OTA_BLE_ENABLE)
            //Exit BLE Mode
            if (app_func_mode == RF_BLE_MODE)
            {
                if(key0 == KEY_BACK)
                {
                    printf("--->>> 2.4G start_reboot!\r\n");
                    start_reboot(); 
                }
            }
            #endif
            if(key0 == KEY_BACK)
            {
                if(pair_flag)
                {
                    app_rcu_24g_pair_timer = 0; 
                    rf_24G_deep_tick = clock_time();
                    app_led_blink_timer = 0;
                    gpio_write(GPIO_LED_1,!LED_ON_LEVAL);
                    pair_flag=0;
                    analog_write(DEEP_ANA_REG1,0);                       
                }
                if(app_ota_dfu_wait_timer)
                {
                    app_ota_dfu_wait_timer = 0;
                    printf("********** DFU OTA Waiting STOP !!! **********\r\n");
                    led_1_driver_init();
                    gpio_write(GPIO_LED_1, !LED_ON_LEVAL);
                    rcu_ota_dfu_flag = 0x00;
                    rf_24G_deep_tick = clock_time();			
                }
                if(app_ota_dfu_download_timer)
                {
                    app_ota_dfu_download_timer = 0;
                    printf("********** DFU OTA DownLoad STOP !!! **********\r\n");
                    led_1_driver_init();
                    rcu_ota_dfu_flag = 0x00;
                    rf_24G_deep_tick = clock_time();	
                    flash_erase_sector(ota_program_offset);
                    app_ota_dfu_download_timer = 0;	
                    led_1_driver_init();
                    gpio_write(GPIO_LED_1, !LED_ON_LEVAL);
                    start_reboot();	
                }
            }
            if(app_ship_mode_led == 0)
            {
                if (rcu_key_power_flag)
                {
                    ir_send_release(); 
                    return;
                }
                send_ir_key_handle(key0);
            }
        }
        else
        {
            #if(IR_MODULE_ENABLE)
            ir_send_release();
            #endif
            if(key0 == KEY_MOUSE_L)
            {
                report_mouse_L_flag = 0x01;
                report_mouse_R_flag = 0x00;
                get_mouse_report_move_press(0x01, 0x00, 0x00);
            }
            if(key0 == KEY_MOUSE_R)
            {
                report_mouse_R_flag = 0x01;
                report_mouse_L_flag = 0x00;
                get_mouse_report_move_press(0x02, 0x00, 0x00);
            }
            return;
        }
    }
    else
    {
       if(key0 == KEY_OK               || key0 == KEY_UP           || key0 == KEY_DOWN         || key0 == KEY_LEFT         || key0 == KEY_RIGHT            || \
          key0 == KEY_BACK             || key0 == KEY_DB_WINDOWS   || key0 == KEY_SEL_BUTTON   || key0 == KEY_FLAME        || key0 == KEY_FREEZE_SCREEN    || \
          key0 == KEY_DISABLE_TOUCH    || key0 == KEY_VOLUME_UP    || key0 == KEY_VOLUME_DOWN  || key0 == KEY_INPUT_SELECT || key0 == KEY_SETTING)
          {
                get_data_report_release();
                return;
          }  
        app_rcu_kb_pressed_flag = 1;
        printf("--->>> [GSensor] Face_up_flag: 0 DOWN --- HID Key\r\n");
    }
}

/******************************************************************
 * @brief  key_handle_pressed_2_event
 * @param  none
 * @return none
 * @retval void
 */
void key_handle_pressed_2_event(void)
{ 
    u8 key0 = kb_event.keycode[0];
    u8 key1 = kb_event.keycode[1];
    printf("--->>> [work_debug] key_pressed_2 - key0: S%d, key1: S%d\r\n",key0, key1);
    Accessibility_Shortcut_Flag = 0x55;
    rcu_key_normal_flag = 2;

#if (STUCK_KEY_PROCESS_ENABLE)
    if(stuckkey_keypresstimer == 0)
        stuckkey_keypresstimer = clock_time() | 1;
#endif

    if(Face_up_flag)
    {
    #if (CHARGE_ENABLE)
        //Enter Ship Mode
        if (((key0 == KEY_BACK) && (key1 == KEY_FLAME)) || ((key0 == KEY_FLAME) && (key1 == KEY_BACK)))
        {
            keyScanLongPressTick = clock_time() | 1; //none zero
            LongPressTick = 3*1000*1000;
            Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_SHIPMODE;
        }
    #endif

    #if (OTA_BLE_ENABLE)
        //Enter BLE OTA
        if (((key0 == KEY_DISABLE_TOUCH) && (key1 == KEY_SETTING)) || ((key0 == KEY_SETTING) && (key1 == KEY_DISABLE_TOUCH)))
        {
            keyScanLongPressTick = clock_time() | 1; //none zero
            LongPressTick = 3*1000*1000;
            Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_CHANGEMODE;
        }
        //Notify Dongle BLE OTA
        if (((key0 == KEY_DISABLE_TOUCH) && (key1 == KEY_INPUT_SELECT)) || ((key0 == KEY_INPUT_SELECT) && (key1 == KEY_DISABLE_TOUCH)))
        {
            keyScanLongPressTick = clock_time() | 1; //none zero
            LongPressTick = 3*1000*1000;
            Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_OTADONGLE;
            notify_dongleOTA_flag = NOTIFY_DG_BLE_OTA;
        }
    #endif
        //Notify RCU DFU OTA
        //if (((key0 == KEY_FREEZE_SCREEN) && (key1 == KEY_SETTING)) || ((key0 == KEY_SETTING) && (key1 == KEY_FREEZE_SCREEN)))
        if (((key0 == KEY_DISABLE_TOUCH) && (key1 == KEY_SETTING)) || ((key0 == KEY_SETTING) && (key1 == KEY_DISABLE_TOUCH)))
        {
            keyScanLongPressTick = clock_time() | 1; //none zero
            LongPressTick = 3*1000*1000;
            Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_OTADONGLE;
            notify_dongleOTA_flag = NOTIFY_RCU_DFU_OTA;
        }
        //Notify Dongle DFU OTA
        //if (((key0 == KEY_FREEZE_SCREEN) && (key1 == KEY_INPUT_SELECT)) || ((key0 == KEY_INPUT_SELECT) && (key1 == KEY_FREEZE_SCREEN)))
        if (((key0 == KEY_DISABLE_TOUCH) && (key1 == KEY_INPUT_SELECT)) || ((key0 == KEY_INPUT_SELECT) && (key1 == KEY_DISABLE_TOUCH)))
        {
            keyScanLongPressTick = clock_time() | 1; //none zero
            LongPressTick = 3*1000*1000;
            Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_OTADONGLE;
            notify_dongleOTA_flag = NOTIFY_DG_DFU_OTA;
        }

    #if(IR_MODULE_ENABLE)
        //static u8 ir_linkage_flag = 0x00;
        //ir_linkage_flag = ir_direction_key_linkage_handle(key0, key1);
        //if(ir_linkage_flag)
        //    return;

        if((key0 < 57) && (key1 >= 57))
            send_ir_key_handle(key1); 
        else if((key0 >= 57) && (key1 < 57))
            send_ir_key_handle(key0);
        else
            ir_send_release();     
    #endif        
    }
    else
        app_rcu_kb_pressed_flag = 1;

    #if (FACTORY_ENABLE)
    //Enter Factory Mode
    if (((key0 == KEY_6) && (key1 == KEY_ARROW_RIGHT)) || ((key0 == KEY_ARROW_RIGHT) && (key1 == KEY_6)))
    {
        rcu_key_factory_flag = 1;
        keyScanLongPressTick = clock_time() | 1; //none zero
        LongPressTick = 3*1000*1000;
        Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_FACTORYMODE;
    }
    if (((key0 == KEY_7) && (key1 == KEY_ARROW_RIGHT)) || ((key0 == KEY_ARROW_RIGHT) && (key1 == KEY_7)))
    {
        rcu_key_factory_flag = 2;
        keyScanLongPressTick = clock_time() | 1; //none zero
        LongPressTick = 3*1000*1000;
        Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_FACTORYMODE;
    }
    if (((key0 == KEY_8) && (key1 == KEY_ARROW_RIGHT)) || ((key0 == KEY_ARROW_RIGHT) && (key1 == KEY_8)))
    {
        rcu_key_factory_flag = 3;
        keyScanLongPressTick = clock_time() | 1; //none zero
        LongPressTick = 3*1000*1000;
        Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_FACTORYMODE;
    }
    if (((key0 == KEY_0) && (key1 == KEY_ARROW_RIGHT)) || ((key0 == KEY_ARROW_RIGHT) && (key1 == KEY_0)))
    {
        rcu_key_factory_flag = 4;
        keyScanLongPressTick = clock_time() | 1; //none zero
        LongPressTick = 3*1000*1000;
        Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_FACTORYMODE;
    }
    if (((key0 == KEY_MOUSE_L) && (key1 == KEY_SETTING)) || ((key0 == KEY_SETTING) && (key1 == KEY_MOUSE_L)))
    {
        rcu_key_factory_flag = 5;
        keyScanLongPressTick = clock_time() | 1; //none zero
        LongPressTick = 3*1000*1000;
        Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_FACTORYMODE;
    }
    #endif
}

/******************************************************************
 * @brief  key_handle_pressed_3_event
 * @param  none
 * @return none
 * @retval void
 */
void key_handle_pressed_3_event(void)
{ 
    u8 key0 = kb_event.keycode[0];
    u8 key1 = kb_event.keycode[1];
    u8 key2 = kb_event.keycode[2];
    printf("--->>> [work_debug] key_pressed_3 - key0: S%d, key1: S%d, key2: S%d\r\n",key0, key1, key2);
    Accessibility_Shortcut_Flag = 0x55;
    rcu_key_normal_flag = 3;

    u8 key0_cnt = 0;
    u8 key1_cnt = 0;
    u8 key2_cnt = 0;
    u8 key_all_cnt = 0;

#if (STUCK_KEY_PROCESS_ENABLE)
    if(stuckkey_keypresstimer == 0)
        stuckkey_keypresstimer = clock_time() | 1;
#endif

    if(Face_up_flag)
    {
        if(key0 >= 58)
            key0_cnt = 1;
        if(key1 >= 58)
            key1_cnt = 1;
        if(key2 >= 58)
            key2_cnt = 1;
        key_all_cnt = key0_cnt + key1_cnt + key2_cnt;

        if((key_all_cnt == 0) || (key_all_cnt == 3))
        {
            #if(IR_MODULE_ENABLE)
            ir_send_release();
            #endif
            return;
        }
        if(key_all_cnt == 1)
        {
            if(key0 >= 58)
                send_ir_key_handle(key0); 
            if(key1 >= 58)
                send_ir_key_handle(key1); 
            if(key2 >= 58)
                send_ir_key_handle(key2); 
            return;
        }
        if(key_all_cnt == 2)
        {
            if(kb_event.keycode[0] < 58)
            {
                key0 = kb_event.keycode[1];
                key1 = kb_event.keycode[2];
            }
            else if(kb_event.keycode[1] < 58)
            {
                key0 = kb_event.keycode[0];
                key1 = kb_event.keycode[2];
            }
            else if(kb_event.keycode[2] < 58)
            {
                key0 = kb_event.keycode[0];
                key1 = kb_event.keycode[1];
            }
        }

    #if (CHARGE_ENABLE)
        //Enter Ship Mode
        if (((key0 == KEY_BACK) && (key1 == KEY_FLAME)) || ((key0 == KEY_FLAME) && (key1 == KEY_BACK)))
        {
            keyScanLongPressTick = clock_time() | 1; //none zero
            LongPressTick = 3*1000*1000;
            Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_SHIPMODE;
        }
    #endif

    #if (OTA_BLE_ENABLE)
        //Enter BLE OTA
        if (((key0 == KEY_DISABLE_TOUCH) && (key1 == KEY_SETTING)) || ((key0 == KEY_SETTING) && (key1 == KEY_DISABLE_TOUCH)))
        {
            keyScanLongPressTick = clock_time() | 1; //none zero
            LongPressTick = 3*1000*1000;
            Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_CHANGEMODE;
        }
        //Notify Dongle OTA
        if (((key0 == KEY_DISABLE_TOUCH) && (key1 == KEY_INPUT_SELECT)) || ((key0 == KEY_INPUT_SELECT) && (key1 == KEY_DISABLE_TOUCH)))
        {
            keyScanLongPressTick = clock_time() | 1; //none zero
            LongPressTick = 3*1000*1000;
            Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_OTADONGLE;
        }
    #endif  

    #if(IR_MODULE_ENABLE)
        //static u8 ir_linkage_flag = 0x00;
        //ir_linkage_flag = ir_direction_key_linkage_handle(key0, key1);
        //if(ir_linkage_flag)
        //    return;

        if((key0 < 57) && (key1 >= 57))
            send_ir_key_handle(key1); 
        else if((key0 >= 57) && (key1 < 57))
            send_ir_key_handle(key0);
        else
            ir_send_release();     
    #endif        
    }
    else
        app_rcu_kb_pressed_flag = 1;
}

/******************************************************************
 * @brief  key_handle_release
 * @param  none
 * @return none
 * @retval void
 */
void key_handle_release(void)
{  
    printf("--->>> [work_debug] key_handle_release \r\n");
    report_release_flag = 0x01;
    rcu_key_normal_flag = 0;
    rcu_key_normal_cnt = 0;
    get_data_report_release();

    if(report_mouse_L_flag || report_mouse_R_flag)
    {
        report_mouse_R_flag = 0x02;
        report_mouse_L_flag = 0x02;
        get_mouse_report_move_press(0x00, 0x00, 0x00);
    }
 
    key_not_released = 0;
    keyScanLongPressTick = 0;
    LongPressTick = 0;
    Accessibility_Shortcut_Flag = 0;

#if (STUCK_KEY_PROCESS_ENABLE)
    	stuckkey_keypresstimer = 0;
#endif

	key_buf_aaa.special_key_press_f=0;
#if(IR_MODULE_ENABLE)
	ir_send_release();
#endif
    rf_24G_deep_tick = clock_time();

    app_rcu_kb_pressed_timer = 0;
    app_rcu_kb_pressed_flag = 0;

    if(rcu_key_power_flag)
        key_handle_pressed_power();
}

/******************************************************************
 * @brief  key_handle_LongPress
 * @param  key_mode
 * @return none
 * @retval void
 */
void key_handle_LongPress(u8 key_mode)
{
    //printf("--->>> [work_debug] key_handle_LongPress -> Accessibility_Shortcut_Mode: %x\r\n",key_mode);
        
    if(Accessibility_Shortcut_Mode == APP_ACCESSIBILITY_SHORTCUT_SINGLEKEY) return;
    if(Accessibility_Shortcut_Mode == APP_ACCESSIBILITY_SHORTCUT_PAIRING)
    {
        printf("--->>> [work_debug] APP_ACCESSIBILITY_SHORTCUT_PAIRING\r\n");
        set_pair_flag();
        d24_start_pair();
    }    
    else if(Accessibility_Shortcut_Mode == APP_ACCESSIBILITY_SHORTCUT_CHANGEMODE)
    {
        printf("--->>> [work_debug] APP_ACCESSIBILITY_SHORTCUT_CHANGEMODE\r\n");
        printf("--->>> Bluetooth start_reboot!\r\n");
        app_rcu_flash_unlock_init();//UNLOCK
        u8 rcu_mode_flash[2] = {0x01, 0x01};
        flash_erase_sector(CFG_BLE_24G_ADDR);
        flash_write_page (CFG_BLE_24G_ADDR, 2, rcu_mode_flash);
        app_rcu_flash_lock_init();  //LOCK
        start_reboot();
    }
    else if(Accessibility_Shortcut_Mode == APP_ACCESSIBILITY_SHORTCUT_OTADONGLE)
    {
        printf("--->>> [work_debug] APP_ACCESSIBILITY_SHORTCUT_OTADONGLE\r\n");
        app_notify_dongleOTA = 1;
        app_rcu_kb_pressed_flag = 1;
    }
    else if(Accessibility_Shortcut_Mode == APP_ACCESSIBILITY_SHORTCUT_SHIPMODE)
    {
        #if(CHARGE_ENABLE)
        printf("--->>> [work_debug] APP_ACCESSIBILITY_SHORTCUT_SHIPMODE\r\n");
        app_ship_mode_led = 1;
        app_led_blink_time_ms = 100;
		app_led_blink_timer = clock_time() | 1;
        #endif
    }
    else if(Accessibility_Shortcut_Mode == APP_ACCESSIBILITY_SHORTCUT_KEYPRESS)
    {
        printf("--->>> [work_debug] APP_ACCESSIBILITY_SHORTCUT_KEYPRESS ongoning\r\n");
    }
    #if (FACTORY_ENABLE)
    else if(Accessibility_Shortcut_Mode == APP_ACCESSIBILITY_SHORTCUT_FACTORYMODE)
    {
        printf("--->>> [work_debug] APP_ACCESSIBILITY_SHORTCUT_FACTORYMODE ongoning\r\n");
        factory_Gsensor_Flag = 0x01;
        if(factory_mode_flag == 2)
        {
            printf("Exit Factory Mode!!!\r\n");
            fts_enter_work_mode();
            app_factory_mode_timer = 0;
            gpio_write(GPIO_LED_2,!LED_ON_LEVAL);
            app_factory_channel_flag = 0;
            device_channel = 0;
            factory_mode_flag = 0;
            rf_24G_deep_tick = clock_time();
            #if (MODULE_WATCHDOG_ENABLE)
                wd_set_interval_ms(WATCHDOG_INIT_TIMEOUT,CLOCK_SYS_CLOCK_1MS);
                wd_start();
            #endif
        }
        else
        {
            app_factory_channel_flag = rcu_key_factory_flag;
            factory_mode_flag = 1;
            set_pair_flag();
            d24_start_pair();            
        }
    }
    #endif
    Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_NONE;
}

/******************************************************************
 * @brief  key_handle_deep_pressed
 * @param  none
 * @return none
 * @retval void
 */
void key_handle_deep_pressed(void)
{  
    if(kb_scan_key (KB_NUMLOCK_STATUS_POWERON, 1) && kb_event.cnt)
    {
        key_not_released = 1;
        printf("--->>> [work_debug] kb_event.cnt %d, kb_event.keycode %d\n",kb_event.cnt, kb_event.keycode[0]);
        Accessibility_Shortcut_Flag = 0x55;
        key_change_proc();
    }
}

/******************************************************************
 * @brief  key_handle_pressed_power / key_handle_release_power
 * @param  none
 * @return none
 * @retval void
 */
void key_handle_pressed_power(void)
{ 
    printf("--->>> [work_debug] POW_key pressed\r\n");
	if((rcu_wakeup_touch == 0x00) || (rcu_wakeup_touch == 0x01))
	{
		rcu_wakeup_touch = 0x02;
		FT3308_chip_rsin();
	}
    rf_24G_deep_tick = clock_time();
    key_not_released = 1;
    rcu_key_power_flag = 1;
    
#if (STUCK_KEY_PROCESS_ENABLE)
    if(stuckkey_keypresstimer == 0)
        stuckkey_keypresstimer = clock_time() | 1;
#endif
    if(app_led_2_blink_timer == 0)
    {
        if(app_rcu_batt_mv <= VBAT_VOL_LOW)
        {
            app_led_2_blink_time_ms = 250;
            app_led_2_blink_timer = clock_time() | 1;
        }			
    }
#if(FACTORY_ENABLE)
    if(factory_mode_flag)
    {
        if(factory_Gsensor_Flag == 0x00)
            ir_dispatch(TYPE_IR_SEND, TRUE, KEY_POWER);
        else if(factory_Gsensor_Flag == 0x01)
        {
            if(Face_up_flag)
                ir_dispatch(TYPE_IR_SEND, TRUE, KEY_POWER);
        }
        return;
    }
#endif

    if(Face_up_flag)
    {
        if (rcu_key_normal_flag)
        {
            ir_send_release(); 
            return;
        }
        send_ir_key_handle(KEY_POWER);
    }
        
    printf("--->>> [GSensor] Face_up_flag: %d --- POWER_KEY\r\n",Face_up_flag);
}
void key_handle_release_power(void)
{ 
    printf("--->>> [work_debug] POW_key released\r\n");
    key_not_released = 0;
    rcu_key_power_flag = 0;
#if (STUCK_KEY_PROCESS_ENABLE)
    	stuckkey_keypresstimer = 0;
#endif
#if(IR_MODULE_ENABLE)
    ir_send_release();
#endif
    printf("rcu_key_normal_flag: %d\r\n",rcu_key_normal_flag);
    if(rcu_key_normal_flag == 1)
    {
        kb_event.keycode[0] = rcu_key_normal_cnt;
        key_handle_pressed_1_event();
    }
}

/******************************************************************
 * @brief  send_ir_key_handle
 * @param  none
 * @return none
 * @retval void
 */
void send_ir_key_handle(u8 ir_key)
{
#if(IR_MODULE_ENABLE)
    switch (ir_key)
    {
    case KEY_POWER:
        ir_dispatch(TYPE_IR_SEND, TRUE, IR_POWER);
        break;
    case KEY_OK:
        ir_dispatch(TYPE_IR_SEND, TRUE, IR_OK);
        break;
    case KEY_UP:
        ir_dispatch(TYPE_IR_SEND, TRUE, IR_UP);
        break;
    case KEY_DOWN:
        ir_dispatch(TYPE_IR_SEND, TRUE, IR_DOWN);
        break;
    case KEY_LEFT:
        ir_dispatch(TYPE_IR_SEND, TRUE, IR_LEFT);
        break;
    case KEY_RIGHT:
        ir_dispatch(TYPE_IR_SEND, TRUE, IR_RIGHT);
        break;
    case KEY_BACK:
        ir_dispatch(TYPE_IR_SEND, TRUE, IR_BACK);
        break;
    case KEY_DB_WINDOWS:
        ir_dispatch(TYPE_IR_SEND, TRUE, IR_DB_WINDOWS);
        break;
    case KEY_SEL_BUTTON:
        ir_dispatch(TYPE_IR_SEND, TRUE, IR_SEL_BUTTON);
        break;
    case KEY_FLAME:
        ir_dispatch(TYPE_IR_SEND, TRUE, IR_FLAME);
        break;
    case KEY_FREEZE_SCREEN:
        ir_dispatch(TYPE_IR_SEND, TRUE, IR_FREEZE_SCREEN);
        break;
    case KEY_DISABLE_TOUCH:
        ir_dispatch(TYPE_IR_SEND, TRUE, IR_DISABLE_TOUCH);
        break;
    case KEY_VOLUME_UP:
        ir_dispatch(TYPE_IR_SEND, TRUE, IR_VOLUME_UP);
        break;
    case KEY_VOLUME_DOWN:
        ir_dispatch(TYPE_IR_SEND, TRUE, IR_VOLUME_DOWN);
        break;
    case KEY_INPUT_SELECT:
        ir_dispatch(TYPE_IR_SEND, TRUE, IR_INPUT_SELECT);
        break;
    case KEY_SETTING:
        ir_dispatch(TYPE_IR_SEND, TRUE, IR_SETTING);
        break;
    
    default:
        break;
    }
#endif
}

/******************************************************************
 * @brief  ir_direction_key_linkage_handle
 * @param  none
 * @return none
 * @retval void
 */
u8 ir_direction_key_linkage_handle(u8 ir_key0, u8 ir_key1)
{
    //KEY_OK
    if((ir_key0 == KEY_OK) || (ir_key1 == KEY_OK)) {
        if(ir_key0 == KEY_OK) {
            if((ir_key1 == KEY_UP) || (ir_key1 == KEY_DOWN) || (ir_key1 == KEY_LEFT) || (ir_key1 == KEY_RIGHT))
                return 1;
        }
        if(ir_key1 == KEY_OK) {
            if((ir_key0 == KEY_UP) || (ir_key0 == KEY_DOWN) || (ir_key0 == KEY_LEFT) || (ir_key0 == KEY_RIGHT))
                return 1;
        }        
    }
    //KEY_UP
    if((ir_key0 == KEY_UP) || (ir_key1 == KEY_UP)) {
        if(ir_key0 == KEY_UP) {
            if((ir_key1 == KEY_OK) || (ir_key1 == KEY_DOWN) || (ir_key1 == KEY_LEFT) || (ir_key1 == KEY_RIGHT))
                return 1;
        }
        if(ir_key1 == KEY_UP) {
            if((ir_key0 == KEY_OK) || (ir_key0 == KEY_DOWN) || (ir_key0 == KEY_LEFT) || (ir_key0 == KEY_RIGHT))
                return 1;
        }        
    }
    //KEY_DOWN
    if((ir_key0 == KEY_DOWN) || (ir_key1 == KEY_DOWN)) {
        if(ir_key0 == KEY_DOWN) {
            if((ir_key1 == KEY_OK) || (ir_key1 == KEY_UP) || (ir_key1 == KEY_LEFT) || (ir_key1 == KEY_RIGHT))
                return 1;
        }
        if(ir_key1 == KEY_DOWN) {
            if((ir_key0 == KEY_OK) || (ir_key0 == KEY_UP) || (ir_key0 == KEY_LEFT) || (ir_key0 == KEY_RIGHT))
                return 1;
        }        
    }
    //KEY_LEFT
    if((ir_key0 == KEY_LEFT) || (ir_key1 == KEY_LEFT)) {
        if(ir_key0 == KEY_LEFT) {
            if((ir_key1 == KEY_OK) || (ir_key1 == KEY_UP) || (ir_key1 == KEY_DOWN) || (ir_key1 == KEY_RIGHT))
                return 1;
        }
        if(ir_key1 == KEY_LEFT) {
            if((ir_key0 == KEY_OK) || (ir_key0 == KEY_UP) || (ir_key0 == KEY_DOWN) || (ir_key0 == KEY_RIGHT))
                return 1;
        }        
    }
    //KEY_RIGHT
    if((ir_key0 == KEY_RIGHT) || (ir_key1 == KEY_RIGHT)) {
        if(ir_key0 == KEY_RIGHT) {
            if((ir_key1 == KEY_OK) || (ir_key1 == KEY_UP) || (ir_key1 == KEY_DOWN) || (ir_key1 == KEY_LEFT))
                return 1;
        }
        if(ir_key1 == KEY_RIGHT) {
            if((ir_key0 == KEY_OK) || (ir_key0 == KEY_UP) || (ir_key0 == KEY_DOWN) || (ir_key0 == KEY_LEFT))
                return 1;
        }        
    }

    return 0;
} 

#endif
/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
