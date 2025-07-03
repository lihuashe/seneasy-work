/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     key_driver.c
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
#include "key_driver.h"
#include "stack/ble/ble.h"
#include "application/keyboard/keyboard.h"
#include "application/audio/gl_audio.h"
#include "app_audio.h"
#include "../app_ui.h"
#include "../ir_module/ir_driver.h"
#include "../ir_module/rc_ir.h"
#include "../led_module/led_driver.h"
#include "../app_rcu.h"
#include "../led_module/backlight_handle.h"
#include "../led_module/aw21036_driver.h"
#include "../led_module/bct3236_driver.h"
#include "key_handle.h"
#include "../led_module/ltr_sensor.h"
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
extern u8  *p_kb_map_normal;
extern u8  lowbat;
extern u8  device_in_connection_state;
extern u32 drive_pins[];
extern u32 latest_user_event_tick;
extern  _attribute_data_retention_ unsigned char *kb_p_map[1];
#if (STUCK_KEY_PROCESS_ENABLE)
    extern u32 stuckkey_keypresstimer;
#endif

_attribute_data_retention_ static int gpioWakeup_keyProc_cnt = 0;
_attribute_data_retention_    static u32 keyScanTick = 0;
#define GPIO_WAKEUP_KEYPROC_CNT    3

_attribute_data_retention_ u32 keyScanLongPressTick = 0;
_attribute_data_retention_ u32 LongPressTick = 0;
_attribute_data_retention_ APP_Accessibility_Shortcut_Mode  Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_NONE;
_attribute_data_retention_ u8  is_pairing_mode=0;
_attribute_data_retention_ u8  is_reconn_mode=0;
_attribute_data_retention_ u8  repairing_flag = 0;
_attribute_data_retention_ u8 Accessibility_Shortcut_Flag = 0;

/* 1. Pin definition */
const unsigned int Kb_Drive_Pins_RCU[KB_DRIVE_NUM] = KB_DRIVE_PINS;

/* 2. The following indexes are all extracted from Kb_Map_Normal_RCU */
const unsigned char Kb_CombKey_KeyId_RCU[9] = {};
const unsigned char Kb_Wakeup_Key_Map_Correspend_Index_RCU[12] =  {};
const unsigned char Kb_Map_devicelayout_Correspend_Index_RCU[9] =  {};
const unsigned char Kb_Map_Testmode_Seq_Correspend_Index_RCU[5] =  {};
const unsigned char navi_key_index_RCU[4] = {};

/* 3. Change key index and ir/ble value*/
#if (PCB_VER == PCB_SRC_4526)
const unsigned char Kb_Map_Normal_RCU[KB_SCAN_NUM][KB_DRIVE_NUM] = \
                        {\
                        {0,             1,              2,              3,              4,              5},           \
                        {6,             7,              8,              9,              10,             11},          \
                        {12,            13,             14,             15,             16,             17},          \
                        {18,            19,             20,             21,             22,             23},          \
                        {24,            25,             26,             27,             28,             29},          \
                        {30,            31,             32,             33,             34,             35},          \
                        {36,            37,             38,             39,             40,             41},          \
                        {42,            43,             44,             45,             46,             47}};

const unsigned char Kb_Map_Ble_RCU[KB_SCAN_NUM * KB_DRIVE_NUM] = {\
                        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,      \
                        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,      \
                        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,      \
                        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,      \
                        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,      \
                        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,      \
                        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        BR_CENTER,    \
                        BR_VOICE,       VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE};

const unsigned char Kb_Map_Ir_RCU[KB_SCAN_NUM * KB_DRIVE_NUM] =  {\
                        IR_AMBILIGHT,   IR_POWER,       IR_RED,         IR_GREEN,       IR_SET,         IR_VK_0,      \
                        IR_VK_6,        IR_SOURCE,      IR_PRIME_VIDEO, IR_YOUTUBE,     IR_PAUSE,       IR_YELLOW,    \
                        IR_VK_9,        IR_VK_8,        IR_VK_7,        IR_LEFT,        IR_NETFLIX,     IR_RAKUTEN_TV,\
                        IR_PLAY,        IR_BLUE,        IR_TEXT,        IR_VK_5,        IR_VK_4,        IR_HOME,      \
                        IR_TV,          IR_MENU,        IR_BACK,        IR_STOP,        IR_VK_1,        IR_VK_2,      \
                        IR_VK_3,        IR_REWIND,      IR_UP,          IR_RIGHT,       IR_DN,          IR_FFWD,      \
                        IR_CHN_DN,      IR_MUTE,        IR_VOL_DN,      IR_OPTIONS,     IR_GUIDE,       IR_CENTER,    \
                        IR_VOICE,       IR_CHN_UP,      IR_VOL_UP,      VK_NONE,        VK_NONE,        VK_NONE};
#elif (PCB_VER == PCB_SRC_3215)
const unsigned char Kb_Map_Normal_RCU[KB_SCAN_NUM][KB_DRIVE_NUM] = \
                        {\
                        {0,             1,              2,              3,              4,              5,              6},         \
                        {7,             8,              9,              10,             11,             12,             13},        \
                        {14,            15,             16,             17,             18,             19,             20},        \
                        {21,            22,             23,             24,             25,             26,             27},        \
                        {28,            29,             30,             31,             32,             33,             34}};

const unsigned char Kb_Map_Ble_RCU[KB_SCAN_NUM * KB_DRIVE_NUM] = {\
                        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,    \
                        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,    \
                        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,    \
                        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,    \
                        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE,        VK_NONE};

/*TPV_RCU_PROJECT:  EU01*/
const unsigned char Kb_Map_Ir_RCU_EU01[KB_SCAN_NUM * KB_DRIVE_NUM] =  {\
                        IR_GUIDE,       IR_LEFT,        IR_OPTIONS,     IR_BACK,        IR_VOL_UP,      IR_SOURCE,      IR_RED,     \
                        IR_UP,          IR_CENTER,      IR_DN,          IR_TV,          VK_NONE,        IR_VOICE,       IR_GREEN,   \
                        IR_INFO,        IR_RIGHT,       IR_SUBTITLE,    IR_HOME,        IR_CHN_UP,      IR_MENU,        IR_YELLOW,  \
                        IR_NETFLIX,     IR_OTT_FREE,    IR_OTT_CODE_01, IR_POWER,       IR_MUTE,        IR_VOL_DN,      IR_BLUE,    \
                        IR_OTT_AMAZON,  IR_OTT_DISNEY,  IR_OTT_CODE_02, IR_AMBILIGHT,   IR_LIST,        IR_CHN_DN,      VK_NONE};

const unsigned char Kb_Map_Ir_HIDDEN_CODE_EU01[KB_SCAN_NUM * KB_DRIVE_NUM] =  {\
                        IR_VK_1,        IR_VK_4,        IR_VK_7,        IR_BACK,        IR_VOL_UP,      IR_SOURCE,      IR_REWIND,  \
                        IR_VK_2,        IR_VK_5,        IR_VK_8,        IR_VK_0,        VK_NONE,        IR_VOICE,       IR_PAUSE,   \
                        IR_VK_3,        IR_VK_6,        IR_VK_9,        IR_HOME,        IR_CHN_UP,      IR_MENU,        IR_PLAY,    \
                        IR_NETFLIX,     IR_OTT_FREE,    IR_OTT_CODE_01, IR_POWER,       IR_MUTE,        IR_VOL_DN,      IR_FFWD,    \
                        IR_OTT_AMAZON,  IR_OTT_DISNEY,  IR_OTT_CODE_02, IR_AMBILIGHT,   IR_LIST,        IR_CHN_DN,      VK_NONE};

/*TPV_RCU_PROJECT:  LM01*/
const unsigned char Kb_Map_Ir_RCU_LM01[KB_SCAN_NUM * KB_DRIVE_NUM] =  {\
                        IR_GUIDE,       IR_LEFT,        IR_OPTIONS,     IR_BACK,        IR_VOL_UP,      IR_SOURCE,      IR_RED,     \
                        IR_UP,          IR_CENTER,      IR_DN,          IR_TV,          VK_NONE,        IR_VOICE,       IR_GREEN,   \
                        IR_INFO,        IR_RIGHT,       IR_CC,          IR_HOME,        IR_CHN_UP,      IR_MENU,        IR_YELLOW,  \
                        IR_NETFLIX,     IR_OTT_DISNEY,  IR_OTT_CODE_01, IR_POWER,       IR_MUTE,        IR_VOL_DN,      IR_BLUE,    \
                        IR_OTT_AMAZON,  IR_OTT_APPLE_TV,IR_OTT_CODE_02, IR_AMBILIGHT,   IR_LIST,        IR_CHN_DN,      VK_NONE};

const unsigned char Kb_Map_Ir_HIDDEN_CODE_LM01[KB_SCAN_NUM * KB_DRIVE_NUM] =  {\
                        IR_VK_1,        IR_VK_4,        IR_VK_7,        IR_BACK,        IR_VOL_UP,      IR_POINT,       IR_REWIND,  \
                        IR_VK_2,        IR_VK_5,        IR_VK_8,        IR_VK_0,        VK_NONE,        IR_VOICE,       IR_PAUSE,   \
                        IR_VK_3,        IR_VK_6,        IR_VK_9,        IR_HOME,        IR_CHN_UP,      IR_MENU,        IR_PLAY,    \
                        IR_NETFLIX,     IR_OTT_DISNEY,  IR_OTT_CODE_01, IR_POWER,       IR_MUTE,        IR_VOL_DN,      IR_FFWD,    \
                        IR_OTT_AMAZON,  IR_OTT_APPLE_TV,IR_OTT_CODE_02, IR_AMBILIGHT,   IR_LIST,        IR_CHN_DN,      VK_NONE};

#endif
extern void advertise_tick_retime(void);
extern void key_change_proc(void);
#if (TL_SZ119_TVSSU) 
    u8 rcu_ssu_callback = 0;   
    u8 key_delay_callback = 0;     
    extern unsigned char simple_setup_press_process(kb_data_t *SSU_key_event, unsigned char device_in_connection_state, int *det_key);
#endif
/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief  kb driver map init
 * @param  none
 * @return none
 * @retval void
 */
void kb_driver_map_init(void)
{
#if 0
    u8 len = app_custom_get_drive_len();
    //printf("len=%x\r\n",len);
    kb_p_map[0] = p_kb_map_normal;
    memcpy(drive_pins,Kb_Drive_Pins_RCU,len*4);
#endif
}

/******************************************************************
 * @brief  Get drive len
 * @param  none
 * @return none
 * @retval len
 */
u8 app_custom_get_drive_len(void)
{
    u8 len;
    len = sizeof(Kb_Drive_Pins_RCU)/sizeof(Kb_Drive_Pins_RCU[0]);
    return len;
}

/******************************************************************
 * @brief  proc_keyboard
 * @param  e,p,n
 * @return none
 * @retval void
 */
void proc_keyboard (u8 e, u8 *p, int n)
{
    //when key press gpio wakeup suspend, proc keyscan at least GPIO_WAKEUP_KEYPROC_CNT times
    //regardless of 8000 us interval
    if(e == BLT_EV_FLAG_GPIO_EARLY_WAKEUP){
        gpioWakeup_keyProc_cnt = GPIO_WAKEUP_KEYPROC_CNT;
    }
    else if(gpioWakeup_keyProc_cnt){
        gpioWakeup_keyProc_cnt --;
    }

    if(gpioWakeup_keyProc_cnt || clock_time_exceed(keyScanTick, 8000)){
        keyScanTick = clock_time();
    }
    else{
        return;
    }

    kb_event.keycode[0] = 0;
    int det_key = kb_scan_key (0, 1);

#if(DEEPBACK_FAST_KEYSCAN_ENABLE)
    if(deepback_key_state != DEEPBACK_KEY_IDLE){
        deepback_pre_proc(&det_key);
    }
#endif

    if(det_key){
        if(kb_event.cnt > 0)
        {
            if((backlight_task == 0) && (device_in_connection_state == 1))
            {
                bls_pm_setManualLatency(0);
                app_backlight_ic_close_timer = 0;
                ltr_sensor_init(); 
            }
            if (kb_event.keycode[0] != KEY_123)
            {
                if(led_display_timer == 0)
                    led_display_timer = clock_time() | 1;
                led_display_keycode = kb_event.keycode[0];
                led_display_flag = 0x01;   
            }              
        }
    } 

#if (TL_SZ119_TVSSU)
    rcu_ssu_callback = 0;
    if(device_in_connection_state)
        rcu_ssu_callback = 1;
    else
        rcu_ssu_callback = 0;

    if(det_key){
#if (STUCK_KEY_PROCESS_ENABLE)
        stuckkey_keypresstimer = clock_time() | 1;
#endif
        latest_user_event_tick = clock_time();  //record latest key change time
        key_not_released = 1;
        if(kb_event.cnt > 0)
        {    
            key_delay_callback ++; 
            if(kb_event.cnt == 2)
            {
                //Blocker Key
                u8 key0 = kb_event.keycode[0];
                u8 key1 = kb_event.keycode[1];
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
            }
        }
    }
#endif

#if (TL_SZ119_TVSSU)       
    if(rcu_ssu_callback)
        simple_setup_press_process(&kb_event, device_in_connection_state, &det_key); 
#endif
    if (det_key){
#if (MP_TEST_MODE)
        if (test_get_mode() == MODE_TEST) test_proc_key();
        else
#endif
        key_change_proc();
    }

#if(DEEPBACK_FAST_KEYSCAN_ENABLE)
    if(deepback_key_state != DEEPBACK_KEY_IDLE){
        deepback_post_proc();
    }
#endif

    if((keyScanLongPressTick == 0) && (Accessibility_Shortcut_Mode != APP_ACCESSIBILITY_SHORTCUT_NONE))
    {
        //printf("stop led\r\n");
        //device_led_off(APP_LED_BLUE);
        Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_NONE;
    }

    if(keyScanLongPressTick && clock_time_exceed(keyScanLongPressTick, LongPressTick))
    {
        keyScanLongPressTick = 0;
        advertise_tick_retime();
        //printf("Accessibility_Shortcut timeout and Accessibility_Shortcut_Mode=%x\r\n",Accessibility_Shortcut_Mode);
        key_handle_LongPress(Accessibility_Shortcut_Mode);

    }

    //app_key_send_store();
}

/******************************************************************
 * @brief  key_change_proc
 * @param  none
 * @return none
 * @retval void
 */
void key_change_proc(void)
{
#if (!TL_SZ119_TVSSU)
    latest_user_event_tick = clock_time();  //record latest key change time
    key_not_released = 1;

    //keyScanLongPressTick = 0;
    //LongPressTick = 0;
#endif

    if(kb_event.cnt >= 3){
        #if (!CLOSE_CODE_PRINTF)
        printf("[NOTICE] More than 2 buttons were pressed. key_type: [0x%x] \n",key_type);
        #endif
    }
    else if (kb_event.cnt == 2){
        key_handle_pressed_2_event();
    }
    else if(kb_event.cnt == 1){
        key_handle_pressed_1_event();
    }
    else{
        key_handle_release();
    }        
}

/******************************************************************
 * @brief  This function is implemented to achieve couch mode (1)
 * @param  none
 * @return none
 * @retval void
 */
void app_key_release_action(void){

#if  BLE_AUDIO_ENABLE
    if((audio_start_reason == REASON_HTT) && (google_voice_ctl & FLAG_NOTIFY_AUDIO_DATA)){
        #if (!CLOSE_CODE_PRINTF)
        printf("key release_audio_start_reason:0x%x\n",audio_start_reason);
        #endif
        app_audio_delay_close_timer = clock_time() | 1;
        /*
        google_voice_ctl |= FLAG_AUDIO_CLOSE;
        audio_stop_reason = REASON_RELEASE_HTT;
        audio_start_reason = 0;
        ui_enable_mic(0);
        g_delay_send_audio_stop = clock_time();
        */
    }
#endif  /* BLE_AUDIO_ENABLE */

#if (STUCK_KEY_PROCESS_ENABLE)
    stuckkey_keypresstimer = 0;
#endif

#if APP_IR_OVER_BLE
    if(is_programming_key_send() == 1)
    {
        printf("key up\r\n");
        ir_fallback_send(0);
    }
    programming_key_set(0);
#endif

    //printf(" app_key_release_action. key_type=%x\r\n",key_type);
    if(key_type == CONSUMER_KEY)
    {
        u16 consumer_key[4]={0};
        bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, (u8 *)&consumer_key, 4);
    }
    else if(key_type == KEYBOARD_KEY)
    {
        u8 key_buf[8] = {0,0,0,0,0,0,0,0};
        bls_att_pushNotifyData (HID_NORMAL_KB_REPORT_INPUT_DP_H, key_buf, 8); //release
    }

#if (REMOTE_IR_ENABLE)
    else if(key_type == IR_KEY)
    {
        if(ir_not_released){
            ir_not_released = 0;
            WaitMs(20);
            if(ir_send_ctrl.is_sending)
            {
                ir_dispatch_philips(TYPE_IR_RELEASE, 0, 0);
            }
            WaitMs(20);
        }
    }
#endif  /* REMOTE_IR_ENABLE */

    key_type = 0;
}

/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
