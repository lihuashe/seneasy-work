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
#include "app_audio.h"
#include "stack/ble/ble.h"
#include "application/keyboard/keyboard.h"
#include "application/audio/gl_audio.h"
#include "../app_ui.h"
#include "../app_rcu.h"
#include "../led_module/led_driver.h"
#include "../ir_module/ir_driver.h"
#include "../ir_module/rc_ir.h"
#include "../led_module/aw21036_driver.h"
#include "../led_module/bct3236_driver.h"
#include "../led_module/backlight_handle.h"
#include "../led_module/ltr_sensor.h"
#include "../profile_module/dis_profile.h"
#include "../app_utility/app_custom_lib.h"
#include "../profile_module/seneasy_profile.h"
#include "../bat_module/battery_check.h"
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
extern u8  lowbat;
extern u8  device_in_connection_state;
extern u8  *p_kb_map_ble;
extern u8  wakeup_key_keycount;
extern u32 latest_user_event_tick;
#if (STUCK_KEY_PROCESS_ENABLE)
    extern u32 stuckkey_keypresstimer;
#endif
bool spcial_combine_key = 0;
u8 batt_poor_flag = 0x00;
u8 key_clear_pair_flag = 0x00;
extern void advertise_tick_retime(void);
extern void app_enter_deep_timeset(void);
extern u8 app_keycache(u8 keyindex);
extern void pressed_123_key(void);
extern void comb_key_stop_ir(void);
extern u8 app_is_navi_key(u8 data0,u8 data1);
extern void app_ota_slave_terminate(void);
extern ble_sts_t app_terminate(u8 reason);
extern void ir_dispatch(u8 type, u8 syscode, u8 ircode);
static const u16 vk_consumer_map[] = {
    GOOGLE_MKEY_NOTIFICATION,
    GOOGLE_MKEY_LIVETV,
    GOOGLE_MKEY_TELETEXT,
    GOOGLE_MKEY_USERPROFILE,
    GOOGLE_MKEY_ALLAPPS,
    GOOGLE_MKEY_FASTREWIND,
    GOOGLE_MKEY_RECORD,
    GOOGLE_MKEY_PLAYPAUSE,
    GOOGLE_MKEY_FASTFORWARD,
    GOOGLE_MKEY_INFO,
    GOOGLE_MKEY_SUBTITLE,
    GOOGLE_MKEY_RED,
    GOOGLE_MKEY_GREEN,
    GOOGLE_MKEY_YELLOW,
    GOOGLE_MKEY_BLUE,
    GOOGLE_MKEY_YOUTUBE,
    GOOGLE_MKEY_NETFLIX,
    GOOGLE_MKEY_DISNEY,
    GOOGLE_MKEY_HBOMAX,
    GOOGLE_MKEY_UP,
    GOOGLE_MKEY_DN,
    GOOGLE_MKEY_LEFT,
    GOOGLE_MKEY_RIGHT,
    GOOGLE_MKEY_CENTER,
    GOOGLE_MKEY_HOME,
    GOOGLE_MKEY_BACK,
    GOOGLE_MKEY_POWER,
    GOOGLE_MKEY_VOL_MUTE,
    GOOGLE_MKEY_CHN_UP,
    GOOGLE_MKEY_CHN_DN,
    GOOGLE_MKEY_GUIDE,
    GOOGLE_MKEY_BOOKMARK,
    GOOGLE_MKEY_ASSIST,
    GOOGLE_MKEY_INPUT,
    GOOGLE_MKEY_DASHBOARD,
};
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
    //u8 key_value;
    //u8 key_buf[8] = {0,0,0,0,0,0,0,0};
    u16 consumer_key[2]={0};
    printf("--->>> key_pressed_1 - S%d\r\n",key0);
    keyScanLongPressTick = 0;
    LongPressTick = 0;
    if(Quick_WaitMs_Flag)
    {
        if(ui_mic_enable)
            Quick_WaitMs_Flag = 0;  
    }
    //特殊组合按键逻辑
    if(spcial_combine_key == 1)
    {
        if (ir_send_ctrl.is_sending)
        { 
            WaitMs(110);
            key_type = IR_KEY;
            ir_send_release();
            WaitMs(70);
        }
        if ((blc_ll_getCurrentState() == BLS_LINK_STATE_CONN) )
        {
            WaitMs(5);
            key_type = CONSUMER_KEY;
            consumer_key[0] = 0;
            consumer_key[1] = 0;
            bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, (u8 *)&consumer_key, 2);
        }   
        return;
    }

#if (TL_SZ119_TVSSU)
    if((rcu_ssu_callback == 0) && (key_delay_callback > 1))
    {
        key_delay_callback ++;
        WaitMs(40);
    }
#else
    WaitMs(40);
#endif

    //retime
    advertise_tick_retime();
#if (!TL_SZ119_TVSSU)
#if (STUCK_KEY_PROCESS_ENABLE)
    stuckkey_keypresstimer = clock_time() | 1;
#endif
#endif
    //comb_key_stop_ir();
    Accessibility_Shortcut_Flag = 0x55;

    if (key0 == KEY_UP || key0 == KEY_DOWN || key0 == KEY_LEFT || key0 == KEY_RIGHT || key0 == KEY_CENTER /*|| key0 == KEY_VOICE*/)
    {
        u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
        if(bond_number && (blc_ll_getCurrentState() != BLS_LINK_STATE_CONN) ){
            user_direct_adv(MY_DIRECT_ADV_TMIE);
        }       
    }

#if (RCU_CONFIG_TPV)
    if((app_rcu_pairing_flag == 0x55) && (key0 != KEY_SOURCE) && (key0 != KEY_UP) && (key0 != KEY_DOWN) && (key0 != KEY_LEFT) && (key0 != KEY_RIGHT) && (key0 != KEY_CENTER))
    {
        #if (!CLOSE_CODE_PRINTF)
        printf("back key, stop adv\r\n");
        #endif
        device_led_off(APP_LED_BLUE);
        bls_ll_setAdvEnable(0);
        is_pairing_mode = 0;
        is_reconn_mode = 0;
        app_rcu_pairing_flag = 0x00;
        app_enter_deep_timeset();
        u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
        printf("bond_number_1=%x\r\n",bond_number);
    }
    if(key0 != KEY_VOICE)
        ftick_voice_reconn_send_code = 0;
#endif

#if (BLE_AUDIO_ENABLE)
    if (key0 == KEY_VOICE)
    {
#if (!TL_SZ119_TVSSU)
        led_display_keycode = key0;
        led_display_flag = 0x01;  
        if(asl_status_error_cnt == 0)
            ALS_Read_Status();
#endif
        if(seneasy_rcu_data_ccc)
            seneasy_rcu_key_handle(key0);
        pressed_voice_key();
        return;
    }
    else
    {
        if((audio_start_reason == REASON_HTT) && (google_voice_ctl & FLAG_NOTIFY_AUDIO_DATA)){
            #if (!CLOSE_CODE_PRINTF)
            printf("key release_audio_start_reason:0x%x\n",audio_start_reason);
            #endif
            google_voice_ctl |= FLAG_AUDIO_CLOSE;
            audio_stop_reason = REASON_RELEASE_HTT;
            audio_start_reason = 0;
            ui_enable_mic(0);
            g_delay_send_audio_stop = clock_time();
        }
    }
#endif

    key_type = IR_KEY;
    //app_keypress_led(0);
    if(app_keycache(key0) == 1)
    {
        return;
    }

    if (key0 == KEY_SOURCE || key0 == KEY_UP || key0 == KEY_DOWN || key0 == KEY_LEFT || key0 == KEY_RIGHT || key0 == KEY_CENTER){
    }
    else
    {
        if(is_pairing_mode == 0x55 || app_rcu_pairing_flag == 0x55)
        {
            #if (!CLOSE_CODE_PRINTF)
            printf("back key, stop adv\r\n");
            #endif
            app_rcu_pairing_flag = 0x00;
            device_led_off(APP_LED_BLUE);
            bls_ll_setAdvEnable(0);
            is_pairing_mode = 0;
            is_reconn_mode = 0;
            app_enter_deep_timeset();
            u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
            printf("bond_number_1=%x\r\n",bond_number);
        }
    }

    if(seneasy_rcu_data_ccc)
    {
        seneasy_key_flag = 0x01;
        seneasy_rcu_key_handle(key0);
    }    

#if (TEST_SW_LED)
    led_test_sw_flag = analog_read(DEEP_ANA_REG7);
    if (key0 == KEY_AMBILIGHT)
        pressed_ambilight_key();  
    if(led_test_sw_flag)
    {
        LED_Test_SW_Function(key0);
        return;
    }
#endif

    if (key0 == KEY_123)
    {
        led_display_flag = 0x02;
        //pressed_123_key();
        if(asl_status_error_cnt == 0)
            ALS_Read_Status();
        return;
    }

    /* Enable Hidden Key Codes */
    if(key0 == KEY_NUM_0 || key0 == KEY_NUM_1 || key0 == KEY_NUM_2 || key0 == KEY_NUM_3  ||\
       key0 == KEY_NUM_4 || key0 == KEY_NUM_5 || key0 == KEY_NUM_6 || key0 == KEY_NUM_7  ||\
       key0 == KEY_NUM_8 || key0 == KEY_NUM_9 || key0 == KEY_NUM_0 || key0 == KEY_REWIND ||\
       key0 == KEY_PAUSE || key0 == KEY_PLAY  || key0 == KEY_FFWD  || key0 == KEY_SOURCE)
    {
        if(app_hidden_key_mode)
        {
            if(key0 == KEY_SOURCE){
                if(strcmp((char*)rcu_project_name,"LM01") != 0)
                {
                    pressed_source_key();
                    return; 
                }
            }
            key_type = IR_KEY;
            if(!ir_not_released){
                u8 key_value = 0;
#if (PCB_VER == PCB_SRC_4526)
                key_value = Kb_Map_Ir_HIDDEN_CODE[key0];
#elif (PCB_VER == PCB_SRC_3215)
                if (strcmp((char*)rcu_project_name,"EU01") == 0)
                    key_value = Kb_Map_Ir_HIDDEN_CODE_EU01[key0];
                else if (strcmp((char*)rcu_project_name,"LM01") == 0)
                    key_value = Kb_Map_Ir_HIDDEN_CODE_LM01[key0];
#endif
                ir_dispatch_philips(TYPE_IR_SEND, TRUE, key_value);
                ir_not_released = 1;
            }
            return;
        }
    }    

    if (key0 == KEY_SOURCE)
    {
        pressed_source_key();
        return;
    }
    /*
    if (key0 == KEY_OPTIONS)
    {
        pressed_options_key();
        return;
    }
    if (key0 == KEY_GUIDE)
    {
        pressed_guide_key();
        return;
    }
    */

#if (PCB_VER == PCB_SRC_3215)
    if (strcmp((char*)rcu_project_name,"EU01") == 0)
    {
        if (key0 == KEY_SUBTITLE)
        {
            pressed_subtitle_key();
            return;
        }
    }
#endif   

    if (key0 == KEY_GREEN)
    {
        pressed_green_key();
        return;
    }

    if(device_in_connection_state)
    {
        /*
        if(key0 == KEY_CENTER)
        {
            printf("--->>> [work_debug] KEYBOARD_KEY_key_value: 0x%04X \r\n",Kb_Map_Ble_RCU[key0]);
            key_type = CONSUMER_KEY;
            key_buf[0] = U16_LO(Kb_Map_Ble_RCU[key0]);
            key_buf[1] = U16_HI(Kb_Map_Ble_RCU[key0]);
            bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, key_buf, 2);
        }
        else
        */
        {
            key_type = IR_KEY;
            if(!ir_not_released){
                u8 key_value = 0;
#if (PCB_VER == PCB_SRC_4526)
                key_value = Kb_Map_Ir_RCU[key0];
#elif (PCB_VER == PCB_SRC_3215)
                if (strcmp((char*)rcu_project_name,"EU01") == 0)
                    key_value = Kb_Map_Ir_RCU_EU01[key0];
                else if (strcmp((char*)rcu_project_name,"LM01") == 0)
                    key_value = Kb_Map_Ir_RCU_LM01[key0];
#endif
                ir_dispatch_philips(TYPE_IR_SEND, TRUE, key_value);
                ir_not_released = 1;
            }            
        }
    }
    else
    {
        key_type = IR_KEY;
        if(!ir_not_released){
            u8 key_value = 0;
#if (PCB_VER == PCB_SRC_4526)
            key_value = Kb_Map_Ir_RCU[key0];
#elif (PCB_VER == PCB_SRC_3215)
            if (strcmp((char*)rcu_project_name,"EU01") == 0)
                key_value = Kb_Map_Ir_RCU_EU01[key0];
            else if (strcmp((char*)rcu_project_name,"LM01") == 0)
                key_value = Kb_Map_Ir_RCU_LM01[key0];
#endif
            ir_dispatch_philips(TYPE_IR_SEND, TRUE, key_value);
            ir_not_released = 1;
        }
    }
    //WaitMs(10);
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
    //u8 key_value;
    u8 key_buf[8] = {0,0,0,0,0,0,0,0};
    u16 consumer_key[2]={0};
    printf("--->>> key_pressed_2 - S%d,S%d\r\n",key0, key1);
    keyScanLongPressTick = 0;
    LongPressTick = 0;
    WaitMs(20);

#if (!TL_SZ119_TVSSU)
    //Blocker Key
    if((key0 == KEY_OPTIONS) || (key1 == KEY_OPTIONS)){
        if(key0 == KEY_OPTIONS){
            if((key1 == KEY_DOWN) || (key1 == KEY_LEFT))        return;
        }if(key1 == KEY_OPTIONS){
            if((key0 == KEY_DOWN) || (key0 == KEY_LEFT))        return;
        }
    }
    if((key0 == KEY_SUBTITLE) || (key1 == KEY_SUBTITLE)){
        if(key0 == KEY_SUBTITLE){
            if((key1 == KEY_DOWN) || (key1 == KEY_RIGHT))       return;
        }if(key1 == KEY_SUBTITLE){
            if((key0 == KEY_DOWN) || (key0 == KEY_RIGHT))       return;
        }
    }
    if((key0 == KEY_VOL_UP) || (key1 == KEY_VOL_UP)){
        if(key0 == KEY_VOL_UP){
            if((key1 == KEY_MUTE) || (key1 == KEY_VOL_DN))      return;
        }if(key1 == KEY_VOL_UP){
            if((key0 == KEY_MUTE) || (key0 == KEY_VOL_DN))      return;
        }
    }
    if((key0 == KEY_VOL_DN) || (key1 == KEY_VOL_DN)){
        if(key0 == KEY_VOL_DN){
            if((key1 == KEY_MUTE) || (key1 == KEY_VOL_UP))      return;
        }if(key1 == KEY_VOL_DN){
            if((key0 == KEY_MUTE) || (key0 == KEY_VOL_UP))      return;
        }
    }
    if((key0 == KEY_MUTE) || (key1 == KEY_MUTE)){
        if(key0 == KEY_MUTE){
            if((key1 == KEY_VOL_UP) || (key1 == KEY_VOL_DN))    return;
        }if(key1 == KEY_MUTE){
            if((key0 == KEY_VOL_UP) || (key0 == KEY_VOL_DN))    return;
        }
    }
    if((key0 == KEY_CHN_UP) || (key1 == KEY_CHN_UP)){
        if(key0 == KEY_CHN_UP){
            if((key1 == KEY_LIST) || (key1 == KEY_CHN_DN))      return;
        }if(key1 == KEY_CHN_UP){
            if((key0 == KEY_LIST) || (key0 == KEY_CHN_DN))      return;
        }
    }
    if((key0 == KEY_CHN_DN) || (key1 == KEY_CHN_DN)){
        if(key0 == KEY_CHN_DN){
            if((key1 == KEY_LIST) || (key1 == KEY_CHN_UP))      return;
        }if(key1 == KEY_CHN_DN){
            if((key0 == KEY_LIST) || (key0 == KEY_CHN_UP))      return;
        }
    }
    if((key0 == KEY_LIST) || (key1 == KEY_LIST)){
        if(key0 == KEY_LIST){
            if((key1 == KEY_CHN_UP) || (key1 == KEY_CHN_DN))    return;
        }if(key1 == KEY_LIST){
            if((key0 == KEY_CHN_UP) || (key0 == KEY_CHN_DN))    return;
        }
    }
#endif

    //retime
    advertise_tick_retime();
    Accessibility_Shortcut_Flag = 0x55;
#if (!TL_SZ119_TVSSU)
#if (STUCK_KEY_PROCESS_ENABLE)
    stuckkey_keypresstimer = clock_time() | 1;
#endif
#endif

#if (TEST_SW_LED)
    if(led_test_sw_flag)
    {
        if (((key0 == KEY_UP) && (key1 == KEY_HOME)) || ((key0 == KEY_UP) && (key1 == KEY_HOME)))
        {
            printf("--->>> [work_debug] KEY_UP + KEY_HOME : LED TEST SW  !!!!!!!!!! \n");
            tick_key_up_home = 0x01;
            keyScanLongPressTick = clock_time() | 1; //none zero
            LongPressTick = 3*1000*1000;
            Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_KEYPRESS;
        }
        return;
    }
#endif

#if (BLE_AUDIO_ENABLE)
    if (key0 == KEY_VOICE || key1 == KEY_VOICE){
    }else{
        if((audio_start_reason == REASON_HTT) && (google_voice_ctl & FLAG_NOTIFY_AUDIO_DATA)){
            #if (!CLOSE_CODE_PRINTF)
            printf("key release_audio_start_reason:0x%x\n",audio_start_reason);
            #endif
            google_voice_ctl |= FLAG_AUDIO_CLOSE;
            audio_stop_reason = REASON_RELEASE_HTT;
            audio_start_reason = 0;
            ui_enable_mic(0);
            g_delay_send_audio_stop = clock_time();
        }
    }
#endif

#if (BLE_AUDIO_ENABLE)
    if (key0 == KEY_VOICE || key1 == KEY_VOICE){
    }else{
        if((audio_start_reason == REASON_HTT) && (google_voice_ctl & FLAG_NOTIFY_AUDIO_DATA)){
            #if (!CLOSE_CODE_PRINTF)
            printf("key release_audio_start_reason:0x%x\n",audio_start_reason);
            #endif
            google_voice_ctl |= FLAG_AUDIO_CLOSE;
            audio_stop_reason = REASON_RELEASE_HTT;
            audio_start_reason = 0;
            ui_enable_mic(0);
            g_delay_send_audio_stop = clock_time();
        }
    }
#endif


/*
    //KEY_BACK + KEY_CENTER
    else if (((key0 == KEY_BACK) && (key1 == KEY_CENTER)) || ((key0 == KEY_CENTER) && (key1 == KEY_BACK)))
    {
        comb_key_stop_ir();
        printf("--->>> [work_debug] KEY_BACK + KEY_CENTER : bug report start  !!!!!!!!!! \n");
        spcial_combine_key = 1;
        keyScanLongPressTick = clock_time() | 1; //none zero
        LongPressTick = 3*1000*1000;
        Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_BUGREPORT;
    }
    //KEY_BACK + KEY_DOWN
    else if (((key0 == KEY_BACK) && (key1 == KEY_DOWN)) || ((key0 == KEY_DOWN) && (key1 == KEY_BACK)))
    {
        comb_key_stop_ir();
        printf("--->>> [work_debug] KEY_BACK + KEY_DOWN : talk back start  !!!!!!!!!! \n");
        spcial_combine_key = 1;
        keyScanLongPressTick = clock_time() | 1; //none zero
        LongPressTick = 7*1000*1000;
        Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_DOWNBACK;
    }
    //KEY_MUTE + KEY_VK_0
    else if (((key0 == KEY_MUTE) && (key1 == KEY_VK_0)) || ((key0 == KEY_VK_0) && (key1 == KEY_MUTE)))
    {
        comb_key_stop_ir();
        printf("--->>> [work_debug] KEY_MUTE + KEY_VK_0 : clear pair  !!!!!!!!!! \n");
        keyScanLongPressTick = clock_time() | 1; //none zero
        LongPressTick = 3*1000*1000;
        Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_DEEPBACK;
    }
*/
/*
    //KEY_BACK + KEY_HOME
    if (((key0 == KEY_BACK) && (key1 == KEY_HOME)) || ((key0 == KEY_HOME) && (key1 == KEY_BACK)))
    {
        printf("--->>> [work_debug] KEY_BACK + KEY_HOME : pair start !!!\r\n");
        spcial_combine_key = 1;
        tick_key_back_home = 0x01;
        keyScanLongPressTick = clock_time() | 1; //none zero
        LongPressTick = 3*1000*1000;
        Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_PAIRING;
    }
*/
    //KEY_RED + KEY_BLUE
    if (((key0 == KEY_RED) && (key1 == KEY_BLUE)) || ((key0 == KEY_BLUE) && (key1 == KEY_RED)))
    {
        comb_key_stop_ir();
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> [work_debug] KEY_RED + KEY_BLUE : clear pair  !!!!!!!!!! \n");
        #endif
        keyScanLongPressTick = clock_time() | 1; //none zero
        LongPressTick = 3*1000*1000;
        Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_DEEPBACK;
    }
    else
    {
        if(device_in_connection_state == 0)
        {
            if(app_is_navi_key(key0,key1) == 0)
            {
                if(tick_key_source != 0x02)
                    tick_key_source = 0x00;
                if(tick_key_options != 0x02)
                    tick_key_options = 0x00;
                if(tick_key_guide != 0x02)
                    tick_key_guide = 0x00;

                #if (!CLOSE_CODE_PRINTF)
                printf(" stop double key in ir mode\r\n");
                printf("ir_send_ctrl.is_sending =%x\r\n",ir_send_ctrl.is_sending);
                #endif
                if(ir_send_ctrl.is_sending)
                {
                    key_type = IR_KEY;
                    WaitMs(110);
                    ir_send_release();
                    ir_not_released=0;
                    WaitMs(90);
                }
            }
            else
            {
                #if (!CLOSE_CODE_PRINTF)
                printf("double key is navi in ir mode\r\n");
                #endif
                if(ir_send_ctrl.is_sending == IR_SENDING_NONE)
                {
                    key_type = IR_KEY;
                    //ir_dispatch_ext(TYPE_IR_SEND, key0);
                    u8 key_value = 0;
#if (PCB_VER == PCB_SRC_4526)
                    key_value = Kb_Map_Ir_RCU[key0];
#elif (PCB_VER == PCB_SRC_3215)
                    if (strcmp((char*)rcu_project_name,"EU01") == 0)
                        key_value = Kb_Map_Ir_RCU_EU01[key0];
                    else if (strcmp((char*)rcu_project_name,"LM01") == 0)
                        key_value = Kb_Map_Ir_RCU_LM01[key0];
#endif
                    ir_dispatch_philips(TYPE_IR_SEND, FALSE, key_value);
                    if(ir_not_released == 0) ir_not_released = 1;
                }
            }
        }
        else
        {
            if(app_is_navi_key(key0,key1) == 0)
            {
                //printf(" stop double key in rf mode\r\n");
                if(key_type == CONSUMER_KEY)
                {
                    consumer_key[0] = 0;
                    consumer_key[1] = 0;
                    bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, (u8 *)&consumer_key, 4);
                }
                else if(key_type == KEYBOARD_KEY)
                {
                    key_buf[2] = 0;
                    bls_att_pushNotifyData (HID_NORMAL_KB_REPORT_INPUT_DP_H, key_buf, 8); //release
                }
                if(ir_send_ctrl.is_sending)
                {
                    key_type = IR_KEY;
                    ir_send_release();
                    ir_not_released=0;
                }
            }
        }
    } 
    //WaitMs(10);
}

/******************************************************************
 * @brief  key_handle_release
 * @param  none
 * @return none
 * @retval void
 */
void key_handle_release(void)
{  
    batt_poor_flag = 0x00;
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] key_handle_release \r\n");
    #endif
    key_not_released = 0;
    keyScanLongPressTick = 0;
    LongPressTick = 0;
    
    advertise_tick_retime();
    
    if((Accessibility_Shortcut_Mode == APP_ACCESSIBILITY_SHORTCUT_DEEPVOLDN) || app_enter_single_tone_timer)
    {
        bls_ll_setAdvEnable(0);
        app_enter_single_tone_timer = 0;
        aw21036_all_off();
        ALS_Disable();
        aw21036_Disable();     
        #if (!CLOSE_CODE_PRINTF)
        printf("[backlight] BBB-Idle to sleep \n");
        #endif
        cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);     
    }

    if(tick_key_source)
    {
        batt_poor_flag = 0x01;
        pressed_source_key_release();
    }
        
/*
    if(tick_key_options)
        pressed_options_key_release();
    if(tick_key_guide)
        pressed_guide_key_release();
    if(tick_key_back_home)
        pressed_back_home_key_release();
*/
    if(tick_key_subtitle)
    {
        batt_poor_flag = 0x01;
        pressed_subtitle_key_release();
    }
        

    if(tick_key_green)
    {
        batt_poor_flag = 0x01;
        pressed_green_key_release();
    }

    if(tick_key_voice)
    {
        batt_poor_flag = 0x01;
        pressed_voice_key_release();
    }
        

    app_key_release_action();
    Accessibility_Shortcut_Flag = 0;

  if(spcial_combine_key == 1)
    {
        if (ir_send_ctrl.is_sending)
        { 
            WaitMs(110);
            key_type = IR_KEY;
            #if (!CLOSE_CODE_PRINTF)
            printf("--->>> [work_debug] spcial_combine_key == 1 --- ir_send_release \r\n");
            #endif
            ir_send_release();
            WaitMs(70);
        }
    }
    spcial_combine_key = 0;
    if(ir_not_released)ir_not_released = 0;

    if(batt_poor_flag == 0)
        process_batt_poor();

#if (TEST_SW_LED)
    led_test_sw_flag = analog_read(DEEP_ANA_REG7);
    if(tick_key_ambilight)
        pressed_ambilight_key_release();  
    if(led_test_sw_flag)
    {
        if(tick_key_power)
            pressed_power_key_release();      
        if(tick_key_ott_3)
            pressed_ott_3_key_release();
        if(tick_key_ott_4)
            pressed_ott_4_key_release();
    }
#endif
    //app_backlight_key_release_handle();
    if(seneasy_rcu_data_ccc)
    {
        seneasy_key_flag = 0x00;
        seneasy_rcu_key_handle(0x00);
    }
}

/******************************************************************
 * @brief  key_handle_LongPress
 * @param  key_mode
 * @return none
 * @retval void
 */
void key_handle_LongPress(u8 key_mode)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] key_handle_LongPress -> Accessibility_Shortcut_Mode: %x\r\n",key_mode);
    #endif
    if(Accessibility_Shortcut_Mode == APP_ACCESSIBILITY_SHORTCUT_SINGLEKEY) return;
    if(Accessibility_Shortcut_Mode == APP_ACCESSIBILITY_SHORTCUT_PAIRING)
    {
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> [work_debug] APP_ACCESSIBILITY_SHORTCUT_PAIRING ongoning\r\n");
        #endif
        if((tick_key_source == 0x01) && (app_rcu_pairing_flag == 0x00))
        {
            tick_key_source = 0x02;
            app_rcu_enter_pair();
        }
        if((tick_key_source == 0x01) && (app_rcu_pairing_flag == 0x55))
            tick_key_source = 0x03;

#if 0
        if(tick_key_back_home == 0x01)
        {
            tick_key_back_home = 0x02;
#if (RCU_CONFIG_TPV)
            combkey_back_home_timeout();
#endif
        }

        if((tick_key_source == 0x02) || (tick_key_back_home == 0x02))
        {
            if(lowbat == 0x55)
            {
                return;
            }
            device_led_setup(led_cfg[LED_SHINE_PAIRING]);
            app_rcu_pairing_flag = 0x55;
            is_pairing_mode = 0x55;
            if(is_reconn_mode)
                is_reconn_mode = 0;
    #if BLE_AUDIO_ENABLE
            notify_get_rsp_en = 1;
    #endif
            if(device_in_connection_state == 0)
            {
    //           app_stop_wakeup_loop();
                app_woble_clear_para();
                ll_resolvingList_reset();
    #if APP_IR_OVER_BLE
                ir_table_init();
                ir_flash_set_flag(0);
    #endif
                bls_smp_eraseAllParingInformation();
                WaitMs(20); //wait flash IO

                bls_ll_setAdvDuration(MY_DIRECT_ADV_TMIE, 0);
                bls_ll_setAdvEnable(0);
                bls_ll_setAdvDuration(ADV_IDLE_ENTER_DEEP_TIME*1000*1000, 1);

                //app_set_adv_interval_downgrade_indirect();
                extern void app_adv_indirect(void);
                app_adv_indirect();
            }
            else
            {
                if(ota_is_working)
                {
                    app_ota_slave_terminate();
                    device_led_off(APP_LED_WHITE);
                }
                printf("re-pairing \r\n");
                u16 i=0;
                while(app_terminate(HCI_ERR_REMOTE_USER_TERM_CONN) &&  i < 2000){
                sleep_us(100);
                i++;
                }
                user_unpair_wait_terminate = true;
                WaitMs(500);
                repairing_flag = 0x55;

                extern void app_adv_indirect(void);
                app_adv_indirect();
            }
        }
#endif
    }
    else if(Accessibility_Shortcut_Mode == APP_ACCESSIBILITY_SHORTCUT_BUGREPORT)
    {
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> [work_debug] APP_ACCESSIBILITY_SHORTCUT_BUGREPORT ongoning\r\n");
        #endif
        combkey_back_ok_timeout();
    }
    else if(Accessibility_Shortcut_Mode == APP_ACCESSIBILITY_SHORTCUT_DOWNBACK)
    {
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> [work_debug] APP_ACCESSIBILITY_SHORTCUT_DOWNBACK ongoning\r\n");
        #endif
        combkey_back_down_timeout();
    }
    else if(Accessibility_Shortcut_Mode == APP_ACCESSIBILITY_SHORTCUT_DEEPBACK)
    {
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> [work_debug] APP_ACCESSIBILITY_SHORTCUT_DEEPBACK ongoning\r\n");
        #endif
        app_rcu_erase_pair();
        key_clear_pair_flag = 0x01;
        #if (TL_SZ119_TVSSU)
        for(int ssu_flash = 0; ssu_flash < 11; ssu_flash++){
            flash_erase_sector_1(APP_TL_SZ119_TVSSU_ADDR+ssu_flash*0x1000);
        }
        #endif
    }
    else if(Accessibility_Shortcut_Mode == APP_ACCESSIBILITY_SHORTCUT_DEEPVOLDN)
    {
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> [work_debug] APP_ACCESSIBILITY_SHORTCUT_DEEPVOLDN ongoning\r\n");
        #endif
        extern void app_adv_indirect(void);
        app_adv_indirect();
        app_enter_single_tone_timer = clock_time() | 1; //none zero
    }
    else if(Accessibility_Shortcut_Mode == APP_ACCESSIBILITY_SHORTCUT_KEYPRESS)
    {
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> [work_debug] APP_ACCESSIBILITY_SHORTCUT_KEYPRESS ongoning\r\n");
        #endif


#if (TEST_SW_LED)
        if(tick_key_ambilight == 0x01)
        {
            tick_key_ambilight = 0x00;
            LED_Test_SW_Change();
        }
#if 0
        if(tick_key_power == 0x01)
        {
            tick_key_power = 0x02;
            LED_Test_SW_Toggle_ALL();
        }
        if(tick_key_ambilight == 0x01)
        {
            tick_key_ambilight = 0x00;
            LED_Test_SW_Change();
        }
        if(tick_key_ott_3 == 0x01)
        {
            tick_key_ott_3 = 0x02;
            key_level_flag = 19;
            LED_Test_SW_Increase_Decrease_BR(1);
        }
        if(tick_key_ott_4 == 0x01)
        {
            tick_key_ott_4 = 0x02;
            key_level_flag = 0;
            LED_Test_SW_Increase_Decrease_BR(0);            
        }
#endif
        if(tick_key_up_home == 0x01)
        {
            tick_key_up_home = 0x00;
            LED_Test_SW_Toggle_ALL();
        }
#endif
        if(tick_key_options == 0x01)

            tick_key_options = 0x02;
        if(tick_key_guide == 0x01)
            tick_key_guide = 0x02;
        if(tick_key_subtitle == 0x01)
        {
            ir_dispatch_philips(TYPE_IR_SEND, TRUE, IR_EK_SUBTITLE_L); 
            blt_soft_timer_add(&ir_timer_stop_code, 150*1000); 
            tick_key_subtitle = 0x00;
        }
        if(tick_key_green == 0x01)
        {
            ir_dispatch_philips(TYPE_IR_SEND, TRUE, IR_EK_GREEN_L); 
            blt_soft_timer_add(&ir_timer_stop_code, 150*1000); 
            tick_key_green = 0x00;
        }
    }
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
        deepback_key_state = DEEPBACK_KEY_CACHE;
        key_not_released = 1;
        memcpy(&kb_event_cache,&kb_event,sizeof(kb_event));
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> [work_debug] kb_event.cnt %d, kb_event.keycode %d\n",kb_event.cnt, kb_event.keycode[0]);
        #endif
        Accessibility_Shortcut_Flag = 0x55;
        /*
        if((kb_event.cnt == 1) && (kb_event.keycode[0] == KEY_BACK))
        {
            printf("--->>> [work_debug] press and hold ---> KEY_BACK\r\n");
            //in voice mode need disable enter pairing mode
            keyScanLongPressTick = clock_time() | 1; //none zero
            LongPressTick = 3*1000*1000;
            Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_DEEPBACK;
        }
        */
        if((kb_event.cnt == 1) && (kb_event.keycode[0] == KEY_VOL_DN))
        {
            #if (!CLOSE_CODE_PRINTF)
            printf("--->>> [work_debug] press and hold ---> KEY_VOL_DN\r\n");
            #endif
            device_led_setup(led_cfg[LED_LONG_OFF]);
            keyScanLongPressTick = clock_time() | 1; //none zero
            LongPressTick = 1*1000*1000;
            Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_DEEPVOLDN;
        }
    }
}

/******************************************************************
 * @brief  app_rcu_enter_pair
 * @param  none
 * @return none
 * @retval void
 */
void app_rcu_enter_pair(void)
{
    printf("app_get_bat_vol: %d mv\r\n",app_get_bat_vol());
    if(app_get_bat_vol() <= 2400)
    {
        if(tick_key_source == 0x02)
        {
            ir_dispatch_philips(TYPE_IR_SEND, TRUE, IR_EK_LOW_BAT); 
            blt_soft_timer_add(&ir_timer_stop_code, 245*1000);     
            device_led_setup(led_cfg[LED_PAIR_LOWBAT]);     
        }
    }
    else
    {
    app_rcu_pairing_flag = 0x55;
    is_pairing_mode = 0x55;
    if(is_reconn_mode)
        is_reconn_mode = 0;
#if BLE_AUDIO_ENABLE
    notify_get_rsp_en = 1;
#endif
    if(device_in_connection_state == 0)
    {
        device_led_setup(led_cfg[LED_SHINE_PAIRING]);
//           app_stop_wakeup_loop();
        app_woble_clear_para();
        ll_resolvingList_reset();
#if APP_IR_OVER_BLE
        ir_table_init();
        ir_flash_set_flag(0);
#endif
        bls_smp_eraseAllParingInformation();
        WaitMs(20); //wait flash IO

        if(tick_key_source == 0x02)
        {
            app_backlight_ic_close_timer = 0;
            ir_dispatch_philips(TYPE_IR_SEND, TRUE, IR_EK_SOURCE_L); 
            blt_soft_timer_add(&ir_timer_stop_code, 150*1000); 
        }
        WaitMs(150);

        bls_ll_setAdvDuration(MY_DIRECT_ADV_TMIE, 0);
        bls_ll_setAdvEnable(0);
        bls_ll_setAdvDuration(ADV_IDLE_ENTER_DEEP_TIME*1000*1000, 1);

        //app_set_adv_interval_downgrade_indirect();
        extern void app_adv_indirect(void);
        app_adv_indirect();
    }
    else
    {
        if(ota_is_working)
        {
            app_ota_slave_terminate();
            device_led_off(APP_LED_WHITE);
        }
        #if (!CLOSE_CODE_PRINTF)
        printf("re-pairing \r\n");
        #endif
        u16 i=0;
        while(app_terminate(HCI_ERR_REMOTE_USER_TERM_CONN) &&  i < 2000){
        sleep_us(100);
        i++;
        }
        user_unpair_wait_terminate = true;
        WaitMs(500);
        repairing_flag = 0x55;

        //extern void app_adv_indirect(void);
        //app_adv_indirect();
        device_led_setup(led_cfg[LED_SHINE_PAIRING]);
        if(tick_key_source == 0x02)
        {
            app_backlight_ic_close_timer = 0;
            ir_dispatch_philips(TYPE_IR_SEND, TRUE, IR_EK_SOURCE_L); 
            blt_soft_timer_add(&ir_timer_stop_code, 150*1000); 
        }
    }
    }
}

/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
