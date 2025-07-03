/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     app_rcu.c
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
#include "app_rcu.h"
#include "stack/ble/ble.h"
#include "drivers.h"
#include "app_ui.h"
#include "app_custom.h"
#include "application/audio/gl_audio.h"
#include "application/keyboard/keyboard.h"
#include "./bat_module/battery_check.h"
#include "./led_module/led_driver.h"
#include "./key_module/key_driver.h"
#include "./key_module/key_handle.h"
#include "./ir_module/ir_driver.h"
#include "../led_module/aw21036_driver.h"
#include "../led_module/bct3236_driver.h"
#include "../led_module/ltr_sensor.h"
#include "../led_module/backlight_handle.h"
#include "./drivers/8278/flash/flash_type.h"
#include "../voice_module/app_audio.h"
#include "../profile_module/dis_profile.h"
#include "../app_utility/app_custom_lib.h"
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
extern u8  device_in_connection_state;
extern _attribute_data_retention_  u16 enterdeep_time;
extern _attribute_data_retention_  u16 service_change;
extern ble_sts_t app_terminate(u8 reason);
extern void app_adv_indirect(void);
extern void comb_key_stop_ir(void);
extern void app_enter_deep_timeset(void);
extern void app_set_adv_interval_downgrade_direct(void);

_attribute_data_retention_ own_addr_type_t app_own_address_type_user = OWN_ADDRESS_PUBLIC;
u8 user_unpair_wait_terminate = FALSE;
u8 tick_key_source = 0x00;
u8 tick_key_options = 0x00;
u8 tick_key_guide = 0x00;
u8 tick_key_subtitle = 0x00;
u8 tick_key_green = 0x00;
u8 tick_key_voice = 0x00;
u8 tick_key_back_home = 0x00;
_attribute_data_retention_ u8 app_rcu_pairing_flag = 0x00;
bool app_hidden_key_mode = false;//0x00 close hidden   0x01 open hidden
bool app_hidden_close_flag = false;
_attribute_data_retention_ u32 ftick_voice_reconn_send_code = 0;
_attribute_data_retention_ u32 app_voice_key_reconn_timer = 0; 
_attribute_data_retention_ u32 app_enter_single_tone_timer = 0; 
_attribute_data_retention_ u32 app_backlight_ic_close_timer = 0; 
_attribute_data_retention_ u32 app_hidden_close_timer = 0; 
_attribute_data_retention_ u32 hidden_LongPressTick = 0;
_attribute_data_retention_ u32 br_test_close_led_timer = 0; 
_attribute_data_retention_ u32 br_test_close_led_settime = 0;
_attribute_data_retention_ u32 br_test_led_duty_timer = 0; 
_attribute_data_retention_ u32 br_test_led_duty_settime = 0;
_attribute_data_retention_ u8 led_display_flag = 0;
_attribute_data_retention_ u8 led_display_keycode = 0;
_attribute_data_retention_ u8 sensor_init_error_cnt = 0;
_attribute_data_retention_ u32 ltr_sensor_timer = 0;
_attribute_data_retention_ u32 sensor_init_timer = 0; 
_attribute_data_retention_ u32 led_display_timer = 0;
_attribute_data_retention_ u32 ASL_SENSOR_INIT_timer = 0;
_attribute_data_retention_ u32 app_audio_delay_close_timer = 0;
_attribute_data_retention_ u32 led_bct3236_normalKey_00_timer = 0;
_attribute_data_retention_ u32 led_bct3236_normalKey_01_timer = 0;
_attribute_data_retention_ u32 led_bct3236_hiddenKey_timer = 0;
_attribute_data_retention_ u32 led_bct3236_allKey_00_timer = 0;
_attribute_data_retention_ u32 led_bct3236_allKey_01_timer = 0;
const u8 tbl_advData_TPV[31] = {
     0x02, 0x01, 0x05,                          // BLE limited discoverable mode and BR/EDR not supported
     0x03, 0x19, 0x80, 0x01,                    // 384, Generic Remote Control, Generic category
     0x03, 0x02, 0x12, 0x18,                    // incomplete list of service class UUIDs (0x1812-Human Interface Device)
     0x13, 0xFF, 0x3D, 0x03,                    // Manufacturer Specific
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x50, 0x48, 0x4C, 0x52, 0x43
};
const u8 tbl_scanRsp_TPV[7] = {
    0x06,0x09,'A','P','H','R','C'
};

#if (TL_SZ119_TVSSU)
extern const unsigned char SSU_KEY_HIDDEN[37];
extern const unsigned char SSU_KEY_NORMAL_SUBTITLE[37];
extern const unsigned char SSU_KEY_NORMAL_CC[37];
extern const unsigned char SSU_KEY_NORMAL_AD[37];
extern unsigned char ssu_isuserIRCodeTxing(void);
extern void SIMPLE_SETUP_Handle(unsigned short SSU_handle);
extern void simple_setup_key_register(const unsigned char SSU_key_table[36]);
extern void SSU_flash_address_register(unsigned int address);
extern void register_IR_TX_callback(unsigned char(*ssu_isuserIRCodeTxing)(void));
extern void register_IR_PWM_frequency(unsigned int clock_cnt);
#endif
/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief  app rcu timer task at main loop.
 * @param  none
 * @return none
 * @retval void
 */
void app_rcu_timer_task(void)
{
    if(app_enter_single_tone_timer && clock_time_exceed(app_enter_single_tone_timer, 2*1000*1000)){//send 2s pair
        #if (!CLOSE_CODE_PRINTF)
        printf("app_enter_single_tone_timer timeout\n");
        #endif
        app_enter_single_tone_timer = 0;
        app_rcu_enter_single_tone();
    }
    if(app_voice_key_reconn_timer && !clock_time_exceed(app_voice_key_reconn_timer, 5*1000*1000)){
        if(google_voice_ver == 10)
        {
            #if (!CLOSE_CODE_PRINTF)
            printf("app_voice_key_reconn_timer timeout   google_voice_ver == %d\n",google_voice_ver);
            #endif
            app_voice_key_reconn_timer = 0;
            key_type = VOICE_KEY;
            WaitMs(1000); //receive caps wait 20 ms  start voice
            google_voice_start(); // voice key to tv , not vocie start
            active_mic_open(); // just for debug mic open time out, no special function
            if(key_not_released == 0)
            {
                google_voice_ctl |= FLAG_AUDIO_CLOSE;
                audio_stop_reason = REASON_RELEASE_HTT;
                audio_start_reason = 0;
                ui_enable_mic(0);
                u8 consumer_key[2]={0};
                consumer_key[0] = 0;
                consumer_key[1] = 0;
                bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, (u8 *)&consumer_key, 2);
            }
        }
    }
    if(app_backlight_ic_close_timer && clock_time_exceed(app_backlight_ic_close_timer, 1*1500*1000)){
        if((device_in_connection_state == 1) && (ui_mic_enable == 0) && (key_not_released == 0) && (!device_led_busy()))
        {
            #if (!CLOSE_CODE_PRINTF)
            printf("app_backlight_ic_close_timer timeout\n");
            #endif
            app_backlight_ic_close_timer = 0;
            backlight_task = 0;
            aw21036_all_off();
            ALS_Disable();
            aw21036_Disable();
            if(device_in_connection_state == 0)
            {
                #if (!CLOSE_CODE_PRINTF)
                printf("[backlight] Idle to sleep \n");
                #endif
                cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);     
            }   
        } 
        else
        {
            #if (!CLOSE_CODE_PRINTF)
            printf("app_backlight_ic_close_timer FAIL REBACK\n"); 
            #endif
            //app_backlight_ic_close_timer = 0;
            app_backlight_ic_close_timer = clock_time() | 1;
            if((key_not_released == 0) && (!device_led_busy()) && (ui_mic_enable == 0))
            {
                app_backlight_ic_close_timer = 0;
                backlight_task = 0;
                aw21036_all_off();
                ALS_Disable();
                aw21036_Disable();     
                if(device_in_connection_state == 0)
                {
                    #if (!CLOSE_CODE_PRINTF)
                    printf("[backlight] Idle to sleep \n");
                    #endif
                    cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);     
                }           
            }
        }
        
    }
    if(app_hidden_close_timer && clock_time_exceed(app_hidden_close_timer, hidden_LongPressTick)){
        app_hidden_close_timer = 0;
        app_hidden_close_flag = false;
    }
    if(br_test_close_led_timer && clock_time_exceed(br_test_close_led_timer, br_test_close_led_settime)){
        br_test_close_led_timer = 0;
        
        app_backlight_ic_close_timer = 0;
        backlight_normal_Flag = false;
        backlight_hidden_Flag = false;
        backlight_task = 0;
        aw21036_all_off();
        ALS_Disable();
        aw21036_Disable();
        if(device_in_connection_state == 0)
        {
            #if (!CLOSE_CODE_PRINTF)
            printf("[backlight] Idle to sleep \n");
            #endif
            cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);     
        }   
        BR_TEST_CLOSE_FLAG = false;
    }
    if(br_test_led_duty_timer && clock_time_exceed(br_test_led_duty_timer, br_test_led_duty_settime)){
        br_test_led_duty_timer = 0;
        
        app_backlight_ic_close_timer = 0;
        backlight_normal_Flag = false;
        backlight_hidden_Flag = false;
        backlight_task = 0;
        aw21036_all_off();
        ALS_Disable();
        aw21036_Disable();
        if(device_in_connection_state == 0)
        {
            #if (!CLOSE_CODE_PRINTF)
            printf("[backlight] Idle to sleep \n");
            #endif
            cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);     
        }   
        BR_TEST_LED_DUTY_FLAG = false;
    }
    if(ltr_sensor_timer && clock_time_exceed(ltr_sensor_timer, 10*1000)){//10ms
        ltr_sensor_timer = 0;
        ALS_Read_Status();
    }
    if(sensor_init_timer && clock_time_exceed(sensor_init_timer, 10*1000)){//10ms
        sensor_init_timer = 0;
        ltr_sensor_init(); 
    }
    if(led_display_timer && clock_time_exceed(led_display_timer, 100*1000)){//100ms
        led_display_timer = 0;
        if(asl_status_error_cnt == 0)
            ALS_Read_Status();
    }
    if(ASL_SENSOR_INIT_timer && clock_time_exceed(ASL_SENSOR_INIT_timer, 10*1000)){//10ms
        ASL_SENSOR_INIT_timer = 0;
        ALS_Read_Status_Init();
    }
    if(app_audio_delay_close_timer && clock_time_exceed(app_audio_delay_close_timer, 300*1000)){//300ms
        app_audio_delay_close_timer = 0;
        google_voice_ctl |= FLAG_AUDIO_CLOSE;
        audio_stop_reason = REASON_RELEASE_HTT;
        audio_start_reason = 0;
        ui_enable_mic(0);
        g_delay_send_audio_stop = clock_time();
#if (FUNCTION_VOICE_DELAT_BL)
        if(backlight_normal_Flag == true)
        {
            #if (LED_AW21036_ENABLE)
                aw21036_backlight_NormalKey(AW_LED_OFF);
            #endif
            #if (LED_BCT3236_ENABLE)
                bct3236_backlight_NormalKey(AW_LED_OFF);
            #endif            
        }
        if(backlight_hidden_Flag == true)
        {
            #if (LED_AW21036_ENABLE)
                aw21036_backlight_HiddenKey(AW_LED_OFF);
            #endif
            #if (LED_BCT3236_ENABLE)
                bct3236_backlight_HiddenKey(AW_LED_OFF);
            #endif               
        }
#endif
    }

#if (LED_BCT3236_ENABLE)
    if(led_bct3236_normalKey_00_timer && clock_time_exceed(led_bct3236_normalKey_00_timer, 1)){
        led_bct3236_normalKey_00_timer = 0;
        if(backlight_rcu_ledflag)
        {
            if(openallcnt < 0xFF)
                openallcnt ++;
            bct3236_breath_NormalKey_00();
            if(openallcnt < backlight_rcu_ledcnt)
                led_bct3236_normalKey_00_timer = clock_time();
        }
        else
        {
            if(openallcnt > 0x00)         
                openallcnt --;
            bct3236_breath_NormalKey_00();
            if(openallcnt != 0)
                led_bct3236_normalKey_00_timer = clock_time();
            else
                bct3236_breath_NormalKey_00_OFF();       
        }
    }
    
    if(led_bct3236_normalKey_01_timer && clock_time_exceed(led_bct3236_normalKey_01_timer, 1)){
        led_bct3236_normalKey_01_timer = 0;
        if(backlight_rcu_ledflag)
        {
            if(openallcnt < 0xFF)
                openallcnt += 10;
            bct3236_breath_NormalKey_01();
            if(openallcnt < backlight_rcu_ledcnt - 10)
                led_bct3236_normalKey_01_timer = clock_time();
        }
        else
        {
            if(openallcnt > 0x00)         
                openallcnt --;
            bct3236_breath_NormalKey_01();
            if(openallcnt != 0)
                led_bct3236_normalKey_01_timer = clock_time();
            else
                bct3236_breath_NormalKey_01_OFF();   
        }
    }

    if(led_bct3236_hiddenKey_timer && clock_time_exceed(led_bct3236_hiddenKey_timer, 1)){
        led_bct3236_hiddenKey_timer = 0;
        if(backlight_rcu_ledflag)
        {
            if(openallcnt < 0xFF)
                openallcnt ++;
            bct3236_breath_HiddenKey();
            if(openallcnt < backlight_rcu_ledcnt)
                led_bct3236_hiddenKey_timer = clock_time();
        }
        else
        {
            if(openallcnt > 0x00)         
                openallcnt = 0;
            bct3236_breath_HiddenKey();
            if(openallcnt != 0)
                led_bct3236_hiddenKey_timer = clock_time();
            else
                bct3236_breath_HiddenKey_OFF();    
        }
    }

    if(led_bct3236_allKey_00_timer && clock_time_exceed(led_bct3236_allKey_00_timer, 1)){
        led_bct3236_allKey_00_timer = 0;
        if(backlight_rcu_ledflag)
        {
            if(openallcnt < 0xFF)
                openallcnt ++;
            bct3236_breath_AllKey_00();
            if(openallcnt < backlight_rcu_ledcnt)
                led_bct3236_allKey_00_timer = clock_time();
        }
        else
        {
            if(openallcnt > 0x00)         
                openallcnt --;
            bct3236_breath_AllKey_00();
            if(openallcnt != 0)
                led_bct3236_allKey_00_timer = clock_time();
            else
                bct3236_breath_AllKey_00_OFF();    
        }
    }

    if(led_bct3236_allKey_01_timer && clock_time_exceed(led_bct3236_allKey_01_timer, 1)){
        led_bct3236_allKey_01_timer = 0;
        if(backlight_rcu_ledflag)
        {
            if(openallcnt < 0xFF)
                openallcnt += 10;
            bct3236_breath_AllKey_01();
            if(openallcnt < backlight_rcu_ledcnt - 10)
                led_bct3236_allKey_01_timer = clock_time();
        }
        else
        {
            if(openallcnt > 0x00)         
                openallcnt --;
            bct3236_breath_AllKey_01();
            if(openallcnt != 0)
                led_bct3236_allKey_01_timer = clock_time();
            else
                bct3236_breath_AllKey_01_OFF(); 
        }
    }
#endif
}

/******************************************************************
 * @brief  app_dcdc_enable
 * @param  none
 * @return none
 * @retval void
 */
void app_dcdc_enable(void)
{
if(BR_TEST_CLOSE_FLAG == false)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] app_dcdc_enable!!!\r\n");
    #endif
    app_backlight_ic_close_timer = 0; 
    gpio_set_func(GPIO_PD7,  AS_GPIO);
    gpio_set_output_en(GPIO_PD7, 1);
    gpio_write(GPIO_PD7,0);
}
}

/******************************************************************
 * @brief  app_dcdc_delay_disable
 * @param  none
 * @return none
 * @retval void
 */
void app_dcdc_delay_disable(void)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] app_dcdc_disable!!!\r\n");
    #endif
    app_backlight_ic_close_timer = clock_time() | 1; 
    backlight_task = 1;
}

/******************************************************************
 * @brief  app_rcu_erase_pair
 * @param  none
 * @return none
 * @retval void
 */
void app_rcu_erase_pair(void)
{
    //Press and hold the Back key (>3s) while inserting batteries will also erase pair
    //move codes here
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] app_rcu_erase_pair -> device_in_connection_state=[%d]\n",device_in_connection_state);
    #endif
    device_led_setup(led_cfg[LED_SHINE_ERASE_PAIR]); //shine erase pair
    if(device_in_connection_state){
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> [work_debug] disconnect and unpair\n");
        #endif
        app_terminate(HCI_ERR_REMOTE_USER_TERM_CONN);
        user_unpair_wait_terminate = TRUE; // wait terminate then delete bonding
        app_rcu_unpair_wait_terminate();
    }
    else{
        u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
        if(bond_number)   //at least 1 bonding device exist
        {
            #if (!CLOSE_CODE_PRINTF)
            printf("--->>> [work_debug] bls_smp_eraseAllParingInformation\n");
            #endif
            bls_smp_eraseAllParingInformation();
        }
        else{
            #if (!CLOSE_CODE_PRINTF)
            printf("--->>> [work_debug] NO bond_number to erase\n");
            #endif
        }
    }
    device_in_connection_state = 0;
}

/******************************************************************
 * @brief  app_rcu_repairing
 * @param  none
 * @return none
 * @retval void
 */
int app_rcu_repairing(void)
{
    enterdeep_time = ADV_IDLE_ENTER_DEEP_TIME;
    #if (!CLOSE_CODE_PRINTF)
    printf("app_rcu_repairing::: device_in_connection_state: %d\r\n",device_in_connection_state);
    #endif
    app_adv_indirect();
    repairing_flag = 0;
#if BLE_AUDIO_ENABLE
    notify_get_rsp_en = 0;
#endif

    #if (!CLOSE_CODE_PRINTF)
    printf("AAA-re-pairing \r\n");
    #endif
    app_woble_clear_para();
    ll_resolvingList_reset();
    bls_smp_eraseAllParingInformation();
    WaitMs(20); //wait flash IO
    app_adv_indirect();

    return -1;
}

/******************************************************************
 * @brief  app_rcu_unpair_wait_terminate
 * @param  none
 * @return none
 * @retval void
 */
void app_rcu_unpair_wait_terminate(void)
{
    if(user_unpair_wait_terminate){
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> [work_debug] app_rcu_unpair_wait_terminate\n");
        #endif
        bls_smp_eraseAllParingInformation();
        user_unpair_wait_terminate = FALSE;
    }
}

/******************************************************************
 * @brief  app_rcu_enter_single_tone
 * @param  none
 * @return none
 * @retval void
 */
void app_rcu_enter_single_tone(void)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] app_rcu_enter_single_tone\n");
    #endif
    bls_ll_setAdvEnable(0);
    device_led_setup(led_cfg[LED_LONG_ON]);
    rf_emi_single_tone(RF_POWER_P3p50dBm, 42);
    while (1) {
        if(kb_scan_key (0, 1)){
            break;
        }
    }
    WaitMs(7000);
    device_led_setup(led_cfg[LED_LONG_OFF]);
    rf_emi_stop();
    aw21036_all_off();
    ALS_Disable();
    aw21036_Disable();     
    #if (!CLOSE_CODE_PRINTF)
    printf("[backlight] BBB-Idle to sleep \n");
    #endif
    cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);    
}

/******************************************************************
 * @brief  pressed_voice_key / app_rcu_send_voice_ble
 * @param  none
 * @return none
 * @retval void
 */
void pressed_voice_key(void)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] pressed_voice_key\r\n");
    #endif
    tick_key_voice = 0x01;
    //1  connected , send search
    if (device_in_connection_state) {
        if(google_voice_ctl & FLAG_NOTIFY_AUDIO_DATA) return;
        if(google_voice_ver != 10)
        {
            app_voice_key_reconn_timer = clock_time() | TRUE;
            return ;
        }
        WaitMs(20); //receive caps wait 20 ms  start voice
        key_type = VOICE_KEY;
        google_voice_start();
        active_mic_open();
    }
    //2  bond , not connected , send adv
    u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
    if(bond_number && (blc_ll_getCurrentState() != BLS_LINK_STATE_CONN) ){
        user_direct_adv(MY_DIRECT_ADV_TMIE);
        ftick_voice_reconn_send_code = TRUE|clock_time();
    }   
    //3 not bond , send ir
    if(bond_number ==0 ){
        ir_dispatch_philips(TYPE_IR_SEND, TRUE, IR_VOICE);
        blt_soft_timer_add(&ir_timer_stop_code, 150*1000); 
    }
}
void pressed_voice_key_release(void)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] pressed_voice_key_release\r\n");
    #endif
    tick_key_voice = 0x00;
}

void app_rcu_send_voice_ble(void)
{
    if (ftick_voice_reconn_send_code) {
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> [work_debug] app_rcu_send_voice_ble\r\n");
        #endif
        ftick_voice_reconn_send_code = 0;
        if(google_voice_ctl & FLAG_NOTIFY_AUDIO_DATA) return;
        if(google_voice_ver != 10)
        {
            app_voice_key_reconn_timer = clock_time() | TRUE;
            return ;
        }
        key_type = VOICE_KEY;
        WaitMs(200);
        google_voice_start();
        active_mic_open();
    }  
}

/******************************************************************
 * @brief  pressed_source_key / pressed_source_key_release
 * @param  none
 * @return none
 * @retval void
 */
void pressed_source_key(void)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] pressed_source_key, tick_key_source: %d\r\n",tick_key_source);
    #endif
    if(tick_key_source == 0x00)
    {
        tick_key_source = 0x01;
        comb_key_stop_ir();
        keyScanLongPressTick = clock_time() | 1; //none zero
        LongPressTick = 3*1000*1000;
        Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_PAIRING;
    }
}
void pressed_source_key_release(void)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] pressed_source_key_release\r\n");
    #endif
    if(tick_key_source == 0x01)//short press < 3s
    {
        ir_dispatch_philips(TYPE_IR_SEND, TRUE, IR_EK_SOURCE_S);
        blt_soft_timer_add(&ir_timer_stop_code, 150*1000);   
        if(app_rcu_pairing_flag == 0x55 || app_rcu_pairing_flag == 0x55)
        {
            #if (!CLOSE_CODE_PRINTF)
            printf("back key, stop adv\r\n");
            #endif
            app_rcu_pairing_flag = 0x00;
            device_led_off(APP_LED_BLUE);
            bls_ll_setAdvEnable(0);
            is_pairing_mode = 0;
            is_reconn_mode = 0;
            app_rcu_pairing_flag = 0;
            app_enter_deep_timeset();
            u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
            printf("bond_number_1=%x\r\n",bond_number);
        }
    }   
    else if(tick_key_source == 0x02)
    {
        //ir_dispatch_philips(TYPE_IR_SEND, TRUE, IR_EK_SOURCE_L); 
        //blt_soft_timer_add(&ir_timer_stop_code, 150*1000); 
    }
    tick_key_source = 0x00;
}

/******************************************************************
 * @brief  pressed_options_key / pressed_options_key_release
 * @param  none
 * @return none
 * @retval void
 */
void pressed_options_key(void)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] pressed_options_key, tick_key_options: %d\r\n",tick_key_options);
    #endif
    if(tick_key_options == 0x00)
    {
        tick_key_options = 0x01;
        comb_key_stop_ir();
        keyScanLongPressTick = clock_time() | 1; //none zero
        LongPressTick = 2*1000*1000;
        Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_KEYPRESS;
    }
}
void pressed_options_key_release(void)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] pressed_options_key_release\r\n");
    #endif
    if(tick_key_options == 0x01)//short press < 2s
        ir_dispatch_philips(TYPE_IR_SEND, FALSE, IR_EK_OPTIONS_S);      
    else if(tick_key_options == 0x02)
        ir_dispatch_philips(TYPE_IR_SEND, FALSE, IR_EK_OPTIONS_L); 
    tick_key_options = 0x00;
}

/******************************************************************
 * @brief  pressed_guide_key / pressed_guide_key_release
 * @param  none
 * @return none
 * @retval void
 */
void pressed_guide_key(void)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] pressed_guide_key, tick_key_guide: %d\r\n",tick_key_guide);
    #endif
    if(tick_key_guide == 0x00)
    {
        tick_key_guide = 0x01;
        comb_key_stop_ir();
        keyScanLongPressTick = clock_time() | 1; //none zero
        LongPressTick = 2*1000*1000;
        Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_KEYPRESS;
    }
}
void pressed_guide_key_release(void)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] pressed_guide_key_release\r\n");
    #endif
    if(tick_key_guide == 0x01)//short press < 2s
        ir_dispatch_philips(TYPE_IR_SEND, FALSE, IR_EK_GUIDE_S);      
    else if(tick_key_guide == 0x02)
        ir_dispatch_philips(TYPE_IR_SEND, FALSE, IR_EK_GUIDE_L); 
    tick_key_guide = 0x00;
}

/******************************************************************
 * @brief  pressed_subtitle_key / pressed_subtitle_key_release
 * @param  none
 * @return none
 * @retval void
 */
void pressed_subtitle_key(void)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] pressed_subtitle_key\r\n");
    #endif
    tick_key_subtitle = 0x01;
    comb_key_stop_ir();
    keyScanLongPressTick = clock_time() | 1; //none zero
    LongPressTick = 2*1000*1000;
    Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_KEYPRESS;
}
void pressed_subtitle_key_release(void)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] pressed_subtitle_key_release\r\n");
    #endif
    if(tick_key_subtitle == 0x01)//short press < 2s
    {
        ir_dispatch_philips(TYPE_IR_SEND, TRUE, IR_EK_SUBTITLE_S); 
        blt_soft_timer_add(&ir_timer_stop_code, 150*1000); 
    }     
    tick_key_subtitle = 0x00;
}

/******************************************************************
 * @brief  pressed_green_key / pressed_green_key_release
 * @param  none
 * @return none
 * @retval void
 */
void pressed_green_key(void)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] pressed_green_key\r\n");
    #endif
    tick_key_green = 0x01;
    comb_key_stop_ir();
    keyScanLongPressTick = clock_time() | 1; //none zero
    LongPressTick = 2*1000*1000;
    Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_KEYPRESS;
}
void pressed_green_key_release(void)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] pressed_green_key_release\r\n");
    #endif
    if(tick_key_green == 0x01)//short press < 2s
    {
        ir_dispatch_philips(TYPE_IR_SEND, TRUE, IR_EK_GREEN_S); 
        blt_soft_timer_add(&ir_timer_stop_code, 150*1000); 
    }     
    tick_key_green = 0x00;
}

/******************************************************************
 * @brief  pressed_123_key
 * @param  none
 * @return none
 * @retval void
 */
void pressed_123_key(void)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] pressed_123_key\r\n");
    #endif
    app_hidden_close_timer = 0;
    app_hidden_close_flag = false;
    if(!app_hidden_key_mode)
    {
        key_123_press_flag = 0x01;
        app_backlight_model_function(LED_NORMAL_ON5S_HIDDEN_ON5S);
        app_hidden_key_mode = true;
        #if (TL_SZ119_TVSSU)
            SIMPLE_SETUP_Handle(SIMPLESETUP_CMD_OUT_DP_H);
            simple_setup_key_register (SSU_KEY_HIDDEN);
            SSU_flash_address_register (APP_TL_SZ119_TVSSU_ADDR); 
            register_IR_TX_callback(ssu_isuserIRCodeTxing);
            register_IR_PWM_frequency(CLOCK_SYS_CLOCK_HZ / 1000000);
            ir_hw_initialed = 1;
            ir_driver_init_philips();
        #endif 
    }
    else
    {
        app_backlight_model_function(LED_NORMAL_ON2S_HIDDEN_OFF);
        app_hidden_key_mode = false;
        #if (TL_SZ119_TVSSU)
            SIMPLE_SETUP_Handle(SIMPLESETUP_CMD_OUT_DP_H);
            #if (PCB_VER == PCB_SRC_3215)
                if (strcmp((char*)rcu_project_name,"EU01") == 0)
                    simple_setup_key_register (SSU_KEY_NORMAL_SUBTITLE);
                else if (strcmp((char*)rcu_project_name,"RU01") == 0)
                    simple_setup_key_register (SSU_KEY_NORMAL_SUBTITLE);
                else if (strcmp((char*)rcu_project_name,"LM01") == 0)
                    simple_setup_key_register (SSU_KEY_NORMAL_CC);
            #endif
            #if (PCB_VER == PCB_SRC_3009)
                if (strcmp((char*)rcu_project_name,"EU01") == 0)
                    simple_setup_key_register (SSU_KEY_NORMAL_SUBTITLE);
                else if (strcmp((char*)rcu_project_name,"AP01") == 0)
                    simple_setup_key_register (SSU_KEY_NORMAL_AD);
                else if (strcmp((char*)rcu_project_name,"LM01") == 0)
                    simple_setup_key_register (SSU_KEY_NORMAL_CC);
            #endif
            SSU_flash_address_register (APP_TL_SZ119_TVSSU_ADDR); 
            register_IR_TX_callback(ssu_isuserIRCodeTxing);
            register_IR_PWM_frequency(CLOCK_SYS_CLOCK_HZ / 1000000);
            ir_hw_initialed = 1;
            ir_driver_init_philips();
        #endif 
    }
    app_backlight_key_release_handle();
}

/******************************************************************
 * @brief  app_backlight_handle
 * @param  none
 * @return none
 * @retval void
 */
void app_backlight_key_handle(u8 key_code)
{
    app_backlight_ic_close_timer = clock_time() | 1;
    backlight_task = 1;
    
#if (PCB_LED_VER == PCB_LED_INIT)
    if(key_code == KEY_NUM_0 || key_code == KEY_NUM_1 || key_code == KEY_NUM_2 || key_code == KEY_NUM_3  ||\
       key_code == KEY_NUM_4 || key_code == KEY_NUM_5 || key_code == KEY_NUM_6 || key_code == KEY_NUM_7  ||\
       key_code == KEY_NUM_8 || key_code == KEY_NUM_9 || key_code == KEY_NUM_0 || key_code == KEY_REWIND ||\
       key_code == KEY_PAUSE || key_code == KEY_PLAY  || key_code == KEY_FFWD  )
#elif (PCB_LED_VER == PCB_LED_POINT)
    if(key_code == KEY_NUM_0 || key_code == KEY_NUM_1 || key_code == KEY_NUM_2 || key_code == KEY_NUM_3  ||\
       key_code == KEY_NUM_4 || key_code == KEY_NUM_5 || key_code == KEY_NUM_6 || key_code == KEY_NUM_7  ||\
       key_code == KEY_NUM_8 || key_code == KEY_NUM_9 || key_code == KEY_NUM_0 || key_code == KEY_REWIND ||\
       key_code == KEY_PAUSE || key_code == KEY_PLAY  || key_code == KEY_FFWD  || key_code == KEY_SOURCE)
#endif
    {
        if(key_code == KEY_SOURCE)
        {
            if (strcmp((char*)rcu_project_name,"EU01") == 0)
            {
                if(app_hidden_key_mode || app_hidden_close_flag)
                {
                    app_hidden_close_timer = 0;
                    app_hidden_key_mode = true;
                    app_backlight_model_function(LED_NORMAL_ON2S_HIDDEN_ON2S);
                }else
                    app_backlight_model_function(LED_NORMAL_ON2S);                
            }
            else if (strcmp((char*)rcu_project_name,"LM01") == 0)
            {
                if(app_hidden_key_mode || app_hidden_close_flag)
                {
                    app_hidden_close_timer = 0;
                    app_hidden_key_mode = true;
                    key_123_press_flag = 0x00;
                    app_backlight_model_function(LED_NORMAL_ON5S_HIDDEN_ON5S);
                }else
                {
                    //if(backlight_normal_Flag)
                    //    app_backlight_model_function(LED_NORMAL_ON5S);   
                    app_backlight_model_function(LED_NORMAL_ON2S);    
                }                 
            }            
        }
        else
        {
            if(app_hidden_key_mode || app_hidden_close_flag)
            {
                app_hidden_close_timer = 0;
                app_hidden_key_mode = true;
                key_123_press_flag = 0x00;
                app_backlight_model_function(LED_NORMAL_ON5S_HIDDEN_ON5S);
            }else
            {
                //if(backlight_normal_Flag)
                //    app_backlight_model_function(LED_NORMAL_ON5S); 
                app_backlight_model_function(LED_NORMAL_ON2S);     
            }            
        }
    }
    else
    {
        if(app_hidden_key_mode || app_hidden_close_flag)
        {
            app_hidden_close_timer = 0;
            app_hidden_key_mode = true;
            app_backlight_model_function(LED_NORMAL_ON2S_HIDDEN_ON2S);
        }else
            app_backlight_model_function(LED_NORMAL_ON2S);
    }
    app_backlight_key_release_handle();
}

/******************************************************************
 * @brief  app_backlight_key_release_handle
 * @param  none
 * @return none
 * @retval void
 */
void app_backlight_key_release_handle(void)
{
    //背光灯释放超时
    if(tick_backlight_normal)
    {
        backlight_task = 1;
        app_backlight_ic_close_timer = 0; 
        if(tick_backlight_normal == 0x02)
            LedDisplayTick = 2200*1000;
        if(tick_backlight_normal == 0x05)
            LedDisplayTick = 5*1000*1000;
        app_backlight_normal_timer = clock_time() | 1; //none zero
        tick_backlight_normal = 0;
#if (FUNCTION_VOICE_DELAT_BL)
        if(ui_mic_enable)
        {
            LedDisplayTick = 15*1000*1000;
            return;
        }
#endif
    }
    if(tick_backlight_hidden)
    {
        backlight_task = 1;
        app_backlight_ic_close_timer = 0; 
        if(tick_backlight_hidden == 0x02)
            LedDisplayTick = 2200*1000;
        if(tick_backlight_hidden == 0x05)
            LedDisplayTick = 5*1000*1000;
        app_backlight_hidden_timer = clock_time() | 1; //none zero
        tick_backlight_hidden = 0;
#if (FUNCTION_VOICE_DELAT_BL)
        if(ui_mic_enable)
        {
            LedDisplayTick = 15*1000*1000;
            return;
        }
#endif
    }    
    if(tick_backlight_all)
    {
        backlight_task = 1;
        app_backlight_ic_close_timer = 0; 
        if(tick_backlight_all == 0x02)
            LedDisplayTick = 2200*1000;
        if(tick_backlight_all == 0x05)
            LedDisplayTick = 5*1000*1000;
        app_backlight_all_timer = clock_time() | 1; //none zero
        tick_backlight_all = 0;
#if (FUNCTION_VOICE_DELAT_BL)
        if(ui_mic_enable)
        {
            LedDisplayTick = 15*1000*1000;
            return;
        }
#endif
    }      
}

/******************************************************************
 * @brief  combkey_back_ok_timeout
 * @param  none
 * @return none
 * @retval void
 */
void combkey_back_ok_timeout(void)
{
    u8 consumer_key[8] = {0,0,0,0,0,0,0,0};
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] combkey_back_ok_timeout\r\n");
    #endif
    if(device_in_connection_state){
        consumer_key[0] = 0x00;
        consumer_key[1] = 0x00;
        bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, consumer_key, 2);
        WaitMs(2);
        key_type = CONSUMER_KEY;
        consumer_key[0] = 0xEC;
        consumer_key[1] = 0x00;
        bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, consumer_key, 2);
    }else{
        key_type = IR_KEY;
        if(!ir_not_released){
            ir_dispatch_philips(TYPE_IR_SEND, FALSE, IR_EK_GOOGLE_DEBUG);
            ir_not_released = 1;
        }
    }
}

/******************************************************************
 * @brief  combkey_back_down_timeout
 * @param  none
 * @return none
 * @retval void
 */
void combkey_back_down_timeout(void)
{
    u8 consumer_key[8] = {0,0,0,0,0,0,0,0};
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] combkey_back_down_timeout\r\n");
    #endif
    if(device_in_connection_state){
        consumer_key[0] = 0x00;
        consumer_key[1] = 0x00;
        bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, consumer_key, 2);
        WaitMs(2);
        key_type = CONSUMER_KEY;
        consumer_key[0] = 0xED;
        consumer_key[1] = 0x00;
        bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, consumer_key, 2);
    }else{
        key_type = IR_KEY;
        if(!ir_not_released){
            ir_dispatch_philips(TYPE_IR_SEND, FALSE, IR_EK_GOOGLE_TALKBACK); 
            ir_not_released = 1;
        }
    }
}

/******************************************************************
 * @brief  pressed_back_home_key_release / combkey_back_home_timeout
 * @param  none
 * @return none
 * @retval void
 */
void pressed_back_home_key_release(void)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] pressed_back_home_key_release --- Send IR_BACK\r\n");
    #endif
    if(tick_key_back_home == 0x01)//short press < 3s
        ir_dispatch_philips(TYPE_IR_SEND, FALSE, IR_BACK);      
    tick_key_back_home = 0x00;
}
void combkey_back_home_timeout(void)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [work_debug] combkey_back_home_timeout\r\n");
    #endif
    comb_key_stop_ir();
    WaitMs(200);
    key_type = IR_KEY;
    ir_dispatch_philips(TYPE_IR_SEND, FALSE, IR_EK_SOURCE_L); 
    ir_not_released = 1;
    tick_key_back_home = 0x02;
}

/******************************************************************
 * @brief  process_batt_poor
 * @param  none
 * @return none
 * @retval void
 */
extern _attribute_data_retention_ u16 batt_vol_mv;
void process_batt_poor(void)
{
    _attribute_data_retention_ static u16 key_count=0;  
    batt_poor_flag = 0x00;
    //printf("--->>> [work_debug] process_batt_poor::: batt_vol == %d mV, key_count == %d\r\n",batt_vol_mv,key_count);
    if((batt_vol_mv > VBAT_VOL_LOW) && (batt_vol_mv <= VBAT_VOL_GOOD)) //2.2V < V < 2.4V
    {
        key_count ++;
        WaitMs(100);
        if(key_count == 10)
        {
            ir_dispatch_philips(TYPE_IR_SEND, TRUE, IR_EK_LOW_BAT); 
            blt_soft_timer_add(&ir_timer_stop_code, 245*1000);
        }
        if(key_count == 20)
        {
            if(device_in_connection_state)
            {
                u8 key_buf[8] = {0,0,0,0,0,0,0,0};
                key_buf[2] = 0x71;
                bls_att_pushNotifyData (HID_NORMAL_KB_REPORT_INPUT_DP_H, key_buf, 8);
                WaitMs(5);
                key_type = CONSUMER_KEY;
                key_buf[2] = 0x00;
                bls_att_pushNotifyData (HID_NORMAL_KB_REPORT_INPUT_DP_H, key_buf, 8);                
            }
            else
            {
                ir_dispatch_philips(TYPE_IR_SEND, TRUE, IR_EK_LOW_BAT); 
                blt_soft_timer_add(&ir_timer_stop_code, 245*1000);
            }
        }
        if(key_count >= 20)
            key_count = 0;
    }
    if(batt_vol_mv > VBAT_VOL_GOOD)
        key_count = 0;
}

/******************************************************************
 * @brief  user_direct_adv
 * @param  none
 * @return none
 * @retval void
 */
void user_direct_adv(int n)
{
    u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
    smp_param_save_t  bondInfo;    
    if(bond_number)
    {
        bls_smp_param_loadByIndex( bond_number - 1, &bondInfo);  //get the latest bonding device (index: bond_number-1 )
        ll_resolvingList_reset();
        ll_resolvingList_add(bondInfo.peer_id_adrType,bondInfo.peer_id_addr,bondInfo.peer_irk,bondInfo.local_irk);
        ll_whiteList_reset();
        app_set_adv_interval_downgrade_direct();
        //printf("\r\npeer_addr=%x %x %x %x %x %x\r\n",bondInfo.peer_addr[0],bondInfo.peer_addr[1],bondInfo.peer_addr[2],bondInfo.peer_addr[3],bondInfo.peer_addr[4],bondInfo.peer_addr[5]);
        //printf("\r\npeer_id_addr=%x %x %x %x %x %x\r\n",bondInfo.peer_id_addr[0],bondInfo.peer_id_addr[1],bondInfo.peer_id_addr[2],bondInfo.peer_id_addr[3],bondInfo.peer_id_addr[4],bondInfo.peer_id_addr[5]);

        enterdeep_time = ADV_RECONN_ENTER_DEEP_COUNT;
        if((analog_read(USED_DEEP_ANA_REG) & OTA_FLG) == OTA_FLG)
        {
            #if (!CLOSE_CODE_PRINTF)
            printf("ana read = ota succ\r\n");
            #endif
            service_change = 0x55;
        }
        u8 status=0,rpa_flag=0;
        if (memcmp(bondInfo.peer_id_addr, bondInfo.peer_addr, 6) == 0)
        {
            rpa_flag = 0;
        }
        else
        {
            if(en_slave_rpa == 0)
                rpa_flag = 0;
            else
                rpa_flag = 1;
        }
        if(rpa_flag == 0)
        {
            //printf("host not support rpa\r\n");
            ll_whiteList_add(bondInfo.peer_addr_type,bondInfo.peer_addr);
            status = bls_ll_setAdvParam( MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
                                        ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY, app_own_address_type_user,
                                        bondInfo.peer_addr_type,  bondInfo.peer_addr,
                                        MY_APP_ADV_CHANNEL,
                                        ADV_FP_NONE);
        }
        else
        {
            //printf("host support rpa\r\n");
            app_own_address_type_user = OWN_ADDRESS_RESOLVE_PRIVATE_PUBLIC;
            //ll_resolvingList_setResolvablePrivateAddrTimer(60);
            status = bls_ll_setAdvParam( MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
                    ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY, app_own_address_type_user,
                    bondInfo.peer_id_adrType,  bondInfo.peer_id_addr,
                                                        MY_APP_ADV_CHANNEL,
                                                        ADV_FP_NONE);
        }

        if(status != BLE_SUCCESS) {      while(1); }  //debug: adv setting err
        //it is recommended that direct adv only last for several seconds, then switch to indirect adv
        bls_ll_setAdvDuration(n, 1);
        enterdeep_time = ADV_IDLE_ENTER_DEEP_COUNT;
        //bls_app_registerEventCallback (BLT_EV_FLAG_ADV_DURATION_TIMEOUT, &direct_adv);
        is_reconn_mode = 0x55;
        bls_ll_setAdvEnable(1);  //adv enable
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> [work_debug] user_direct_adv !!!\r\n");
        #endif
    }
}

_attribute_data_retention_ u8 Flash_Power_Flag = 0;
/******************************************************************
 * @brief  app_rcu_flash_lock_init
 * @param  none
 * @return none
 * @retval void
 */
void app_rcu_flash_lock_init(void)
{
    unsigned int mid = flash_read_mid();
    printf("--->>> [app_rcu_flash_lock_init] mid: 0x%x\r\n",mid);
    Flash_Power_Flag = 0x01;
    switch(mid)
    {
        case 0x1160c8:
            flash_lock_mid1160c8(FLASH_LOCK_LOW_64K_MID1160C8);
            break;
        case 0x1360c8:
            flash_lock_mid1360c8(FLASH_LOCK_LOW_384K_MID1360C8);//FLASH_LOCK_LOW_256K_MID1360C8
            break;
        case 0x1460c8:
            flash_lock_mid1460c8(FLASH_LOCK_LOW_768K_MID1460C8);
            break;
        case 0x11325e:
            flash_lock_mid11325e(FLASH_LOCK_LOW_64K_MID11325E);
            break;
        case 0x13325e:
            flash_lock_mid13325e(FLASH_LOCK_LOW_384K_MID13325E);//FLASH_LOCK_LOW_256K_MID13325E
            break;
        case 0x14325e:
            flash_lock_mid14325e(FLASH_LOCK_LOW_768K_MID14325E);
            break;
        default:
            break;
    }
}

/******************************************************************
 * @brief  app_rcu_flash_unlock_init
 * @param  none
 * @return none
 * @retval void
 */
void app_rcu_flash_unlock_init(void)
{
    unsigned int mid = flash_read_mid();
    printf("--->>> [app_rcu_flash_unlock_init] mid: 0x%x\r\n",mid);
    Flash_Power_Flag = 0;
    switch(mid)
    {
        case 0x1160c8:
            flash_unlock_mid1160c8();
            break;
        case 0x1360c8:
            flash_unlock_mid1360c8();
            break;
        case 0x1460c8:
            flash_unlock_mid1460c8();
            break;
        case 0x11325e:
            flash_unlock_mid11325e();
            break;
        case 0x13325e:
            flash_unlock_mid13325e();
            break;
        case 0x14325e:
            flash_unlock_mid14325e();
            break;
        default:
            break;
    }
}

/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
