/******************************************************************************
 * @file     app_ui.c
 *
 * @brief    for TLSR chips
 *
 * @author   public@telink-semi.com;
 * @date     Sep. 30, 2010
 *
 * @attention
 *
 *  Copyright (C) 2019-2020 Telink Semiconductor (Shanghai) Co., Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *****************************************************************************/

#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "app_config.h"
#include "application/keyboard/keyboard.h"
#include "application/usbstd/usbkeycode.h"
#include "../common/blt_led.h"
#include "../common/blt_soft_timer.h"
#include "rc_ir.h"
#include "battery_check.h"
#include "app_audio.h"
#include "app_flash_write.h"
#include "app_ir.h"
#include "app_ui.h"

#if (MP_TEST_MODE)
#include "app_test.h"
#endif

#include "app_custom.h"
#include "app_custom_lib.h"
#include "./app_find_me/app_fms.h"
#include "./key_module/key_driver.h"
#include "./led_module/led_driver.h"
#include "./ir_module/ir_driver.h"
#include "app_rcu.h"
#include "../led_module/aw21036_driver.h"
#include "../led_module/bct3236_driver.h"
#include "../led_module/ltr_sensor.h"
#include "../led_module/backlight_handle.h"
#include "../profile_module/dis_profile.h"
#include "../profile_module/seneasy_profile.h"

u8 ir_hw_initialed = 0;  //note: can not be retention variable
_attribute_data_retention_ u8  key_type;
_attribute_data_retention_ int key_not_released;
_attribute_data_retention_ u8  ota_is_working = 0;
_attribute_data_retention_ u8  app_mtu_size = 23;

_attribute_data_retention_ int ir_not_released=0;
_attribute_data_retention_ u8  user_key_mode;
_attribute_data_retention_ u8  ota_flag = 0;  //0:pause 1:continue
_attribute_data_retention_ u8  testmode_key_seq[5]={0};
_attribute_data_retention_ u8  testmode_key_seq_count=0;

/* 221116 by ken.  This variable is used to wait for google voice mic open cmd */
_attribute_data_retention_ u32 g_delay_en_pm = 0;


extern u8 wakeup_key_keycount;
extern u8  comb_key_keyid[9];
extern u32 latest_user_event_tick;
extern u8  lowbat;
extern u8  device_in_connection_state;
extern u8  flag_schedule_ota;
extern u8  device_timeout_state;
extern u8  wakeup_src;
extern u8  wakeup_key_keyid;
//extern u8  wakeup_key_send_flag;
//extern u8  wakeup_packet_format;
extern u8  en_powerkey_cache;
extern u8  detpowerkey;

extern int ir_fallback_send(u8 key_down);
extern void device_led_off(u8 led);
extern void app_enter_deep_timeset(void);
extern void app_set_adv_interval_downgrade_indirect(void);
extern u8  app_custom_is_wakeup_key(u8 keyid);
extern ble_sts_t app_terminate(u8 reason);
extern int bls_smp_param_deleteByIndex(u8 index);
extern void app_stop_wakeup_loop(void);
extern void key_handle_pressed_1_event(void);
#if (TL_SZ119_TVSSU) 
extern unsigned char isIRCodeTxing(void);
#endif

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

void app_pairing_error(void)
{
    device_led_setup(led_cfg[LED_PAIR_ERROR]);
}

extern u32 keyScanLongPressTick;


#if (REMOTE_IR_ENABLE)
//ir key
//#define TYPE_IR_SEND               1
//#define TYPE_IR_RELEASE            2

///////////////////// key mode //////////////////////
#define KEY_MODE_BLE               0    //ble key
#define KEY_MODE_IR                1    //ir  key


_attribute_data_retention_ u8 *p_kb_map_ble;
_attribute_data_retention_ u8 *p_kb_map_ir;
_attribute_data_retention_ u8 *p_kb_map_normal;


void ir_dispatch(u8 type, u8 syscode ,u8 ircode){
    #if (TL_SZ119_TVSSU)
    /* lijin add */
    unsigned char busy = isIRCodeTxing();
    if(busy) {
        //extern unsigned char irRepeatCount;
        // extern DMASTRUCTDATA dma;
        //omni_TVSSU_printf("Co SSU busy:%d,repeat:%X\r\n",busy,irRepeatCount);
        return;
    }
    #endif
    if(!ir_hw_initialed){
        ir_hw_initialed = 1;
        ir_driver_init_philips();
    }
#if (TL_SZ119_TVSSU) 
    /* lijin add */
    pwm_set_cycle_and_duty(PWM0_ID, PWM_CARRIER_CYCLE_TICK,  PWM_CARRIER_HIGH_TICK );   //config carrier: 38k, 1/3 duty
    pwm_set_dma_address(&T_dmaData_buf);
#endif
    if(type == TYPE_IR_SEND){
        ir_send_ctrl.is_full_sequence = 0;
        ir_nec_send(syscode,~(syscode),ircode);

    }
    else if(type == TYPE_IR_RELEASE){
        ir_send_release();
    }
}



void ir_dispatch_ext(u8 type, u8 keyindex){
#if !CLOSE_CODE_RAM
    u8 data[4];
    if(!ir_hw_initialed){
        ir_hw_initialed = 1;
        rc_ir_init();
    }
    if(type == TYPE_IR_SEND){
        ir_send_ctrl.is_full_sequence = 0;

        if(keyindex == VOICE)
        {
         if(app_custom_get_device_type() == REMOTE_G10)
             keyindex = 9;
         else
             keyindex = 18;
        }

        if(nec_ir_addr1_table[keyindex] == 0xff)
        {
            data[0] = 0x88;
        }
        else
            data[0] = nec_ir_addr1_table[keyindex];

        if(nec_ir_addr2_table[keyindex] == 0xff)
        {
            data[1] = ~data[0];
        }
        else
            data[1] = nec_ir_addr2_table[keyindex];

        data[2] = nec_ir_cmd1_table[keyindex];
        if(nec_ir_cmd2_table[keyindex] == 0xff)
        {
            data[3] = ~nec_ir_cmd1_table[keyindex];
        }
        else
            data[3] = nec_ir_cmd2_table[keyindex];

        //printf("\r\nir_dispatch_ext ir_cm1_value=%x,ir_cm2_value=%x,\r\n",data[2],data[3]);
        ir_nec_send_ext(data);

    }
    else if(type == TYPE_IR_RELEASE){
        ir_send_release();
    }
#endif
}


#endif

#if (BLE_REMOTE_OTA_ENABLE)
#include "app_ota.h"
extern u8  mac_public[6];
u8 app_enter_ota_mode(u8 *data)
{
    printf("start data:");
    array_printf(data,4);
    extern u32 ota_program_offset;
    extern int ota_program_bootAddr;
    printf("ota_program_offset=%x\r\n",ota_program_offset);
    printf("ota_program_offset=%x\r\n",ota_program_bootAddr);

    u8 check_data[4];
    bls_ota_get_random(check_data);

    if(memcmp(check_data,data,4 )){

        u8 report_data[3] ={CLIENT_REPORT_CMD, TYPE_OTA_RESULT, GOOGLE_OTA_START_KEY_ERR};

        bls_att_pushNotifyData(OTA_CMD_OUT_DP_H, report_data, 3);
        #if (!CLOSE_CODE_PRINTF)
        printf("OTA_START_KEY_ERR\n");
        #endif

        return 1;
    }
    bls_pm_setSuspendMask (SUSPEND_DISABLE);
    ota_flag = 1;

    //add user code
    #if (!CLOSE_CODE_PRINTF)
    printf("ota start red led on\r\n");
    #endif
    device_led_setup(led_cfg[LED_OTA_START]);
    ota_is_working = 1;
    bls_ota_setTimeout(90 * 1000 * 1000); //set OTA timeout  90 seconds
    return 0;
}

#if (CUSTOM_OTA_SET)
/***************************************************************************
 ***     Use the following functions to modify various types of keys     ***
 *** These 3 keys can be applied to google or configured by yourselves   ***
 **************************************************************************/


/**
 * change authentication plaintext and plaintext key.
 * This key needs to be consistent with the OTA key(app_AuthKey.txt) selected in the demo ota app.
 * Otherwise it will trigger an authentication error
 **/
void app_ota_get_plaintext_key(u8 *plaintext,u8 *cus_auth_key){

    u8 auth_plaintext[16] = {'D','e','v','i','c','e','M','o','d','e','l','0',0x00,0x00,0x00,0x00,};
    u8 auth_key[16] = {'C','u','s','t','o','m','e','r','A','u','t','h','K','e','y','0',};  // APP key;

    memcpy(plaintext,auth_plaintext,16);
    memcpy(cus_auth_key,auth_key,16);
}


/**
 * Set FW encryption key
 * This key needs to be consistent with the FW_ENC_TEST_KEY in the 8271_ble_remote folder.
 * Otherwise, the OTA will fail due to decryption errors
 **/
void app_ota_get_fw_decryption_key(u8 *dec_key){

    u8 key[16] = {'F','w','_','E','n','c','_','S','a','m','p','l','e','K','e','y',};
    memcpy(dec_key,key,16);
}

/**
 * change ECDSA public key
 * The public key and PrivateSample.pem here are a pair.
 * Users can use OpenSSL to generate public and private keys by themselves
 **/
void app_ota_get_ecdsa_pub_key(u8 *pub_key){
    u8 key[64] = {	0x9f, 0xbb, 0x58, 0xd1, 0x8b, 0x31, 0xc2, 0x41, 0xa0, 0x99, 0x78, 0x10, 0xdb, 0x1a, 0x54, 0x67, 
					0x11, 0x11, 0xce, 0x42, 0x7c, 0xef, 0x1d, 0x4f, 0x5a, 0x86, 0x8c, 0x88, 0xa9, 0x76, 0x9c, 0x1b, 
					0x78, 0x4a, 0xe1, 0x25, 0x0e, 0xc6, 0xed, 0x2d, 0x57, 0xec, 0x9f, 0xf8, 0x4f, 0x08, 0x93, 0x8a, 
					0x7f, 0xd2, 0xd4, 0x08, 0x6f, 0x64, 0xd0, 0x48, 0xd7, 0xe5, 0x54, 0xfa, 0xf9, 0xbe, 0xce, 0x05, };
    memcpy(pub_key,key,64);
}
#endif

void app_ota_result(u8 result)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("google_ota_result result=%x\r\n",result);
    #endif
    u8 report_data[3];
    report_data[0] = CLIENT_REPORT_CMD;
    report_data[1] = TYPE_OTA_RESULT;
    report_data[2] = (u8)result;
    if(device_in_connection_state)
    {
        u16 i = 0;
        while(bls_att_pushNotifyData(OTA_CMD_OUT_DP_H, report_data, 3) &&  i < 2000){
        sleep_us(100);
        i++;
        }

        i = 0;
        while(app_terminate(HCI_ERR_REMOTE_USER_TERM_CONN) &&  i < 2000){
        sleep_us(100);
        i++;
        }

        i = 0;
        while(blc_ll_getTxFifoNumber() &&  i < 400){
        sleep_us(10000);//printf("waiting TX fifo is empty\n"); //waiting TX fifo is empty
        i++;
        }
    }
    if(result == 0)
    {
        #if (!CLOSE_CODE_PRINTF)
        printf("ota end red led off\r\n");
        #endif
        device_led_setup(led_cfg[LED_OTA_END]);
        #if (!CLOSE_CODE_PRINTF)
        printf("ota_suc and write_ana\r\n");
        #endif
        analog_write(USED_DEEP_ANA_REG, analog_read(USED_DEEP_ANA_REG) | OTA_FLG);
        #if (!CLOSE_CODE_PRINTF)
        printf("analog_read(USED_DEEP_ANA_REG)=%x\r\n",analog_read(USED_DEEP_ANA_REG));
        #endif
    }
}

extern u16 count_schedule_ota;
void app_schedule_ota(u8 *data)
{
    //array_printf(data,4);
    u16 count = (data[0]<<8) + data[1];

    if(count ==0)
    {
        count_schedule_ota = 90;

    }else if(count >= 1440){    //24 hour
        count_schedule_ota = 1440;
    }else{
        count_schedule_ota = count;
    }
    #if (!CLOSE_CODE_PRINTF)
    printf("app_schedule_ota:0x%d\n",count);
    #endif

    flag_schedule_ota = 0x55;
    analog_write(DEEP_ANA_REG2, 0);
}

int app_ota_timeoutcb(void)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("app_ota_timeoutcb\r\n");
    #endif
    app_ota_terminate(GOOGLE_OTA_TIMEOUT);
    return -1;
}

extern u8  google_ota_start_flag;
void app_ota_timeout(void)
{
    app_pairing_error();
    blt_soft_timer_add(app_ota_timeoutcb, 1000000);
}

void app_ota_status(u8 status)
{
    if(device_in_connection_state == 0) return;
    if(ota_is_working == 0) return;
    ble_sts_t stat;
    if((status == 0) && (ota_flag == 1))
    {
        stat = app_ota_pause();
        ota_flag = 0;
        printf("app_ota_pause stat=%x\r\n",stat);
    }

    if((status == 1) && (ota_flag == 0))
    {
        ble_sts_t stat;
        stat = app_ota_continue();
        ota_flag = 1;
        printf("app_ota_continue stat=%x\r\n",stat);
    }
}

void app_ota_slave_terminate(void)
{
    #if (!CLOSE_CODE_PRINTF)
    printf("app_ota_slave_terminate\n");
    #endif
    u8 report_data[3] = {CLIENT_REPORT_CMD, TYPE_OTA_RESULT, GOOGLE_OTA_USER_TERMINATE};

    u16 i = 0;
    while(bls_att_pushNotifyData(OTA_CMD_OUT_DP_H, report_data, 3) &&  i < 2000){
       sleep_us(100);
       i++;
    }
}


void app_schedule_ota_enter_deep(void)
{
    u16 my_sch_ota_time[7]= {0,90,120,180,300,480,780};
    u8 my_index = analog_read(DEEP_ANA_REG2);

    u16 my_entry_deep_time = 0;

    /* restart schedule ota*/
    if((0x55 == flag_schedule_ota) && (0x55 == device_timeout_state)){
        #if (!CLOSE_CODE_PRINTF)
        printf("[NOTICE] Restart schedule OTA !!! \n");
        #endif
        my_entry_deep_time = count_schedule_ota;
        my_index = 1;
    }else if(my_index){
        #if (!CLOSE_CODE_PRINTF)
        printf("[NOTICE] Current schedule OTA index: %d !!! \n", my_index);
        #endif
        my_entry_deep_time = (my_index<7)?my_sch_ota_time[my_index]:0;
        if(APP_WAKEUP_SRC_DEEP_TIMER == app_ui_get_wakeup_src()){
            my_index++;
            /* Cannot be set to >=7 here. After waking up, it is necessary to judge
             * whether it is due to schedule OTA according to whether my_index is 0 or not. */
            if(my_index > 7){
                my_index=0;
                my_entry_deep_time = 0;
                analog_write(DEEP_ANA_REG2, my_index);
                #if (!CLOSE_CODE_PRINTF)
                printf("[NOTICE] Clear schedule ota Task !!! \n");
                #endif
            }
        }
    }

    if(my_entry_deep_time){
        printf("Entry schedule OTA index: %d time: %d min \n", my_index-1,my_entry_deep_time );

        analog_write(DEEP_ANA_REG2, my_index);
        cpu_long_sleep_wakeup_32k_rc(DEEPSLEEP_MODE,PM_WAKEUP_TIMER|PM_WAKEUP_PAD,my_entry_deep_time*CLOCK_32K_CLOCK_1MIN);
//        cpu_long_sleep_wakeup_32k_rc(DEEPSLEEP_MODE,PM_WAKEUP_TIMER|PM_WAKEUP_PAD,my_entry_deep_time*CLOCK_32K_CLOCK_1S/2);  // for testing
    }else{

//        if(s_p_app_fms_flag_adv[0]) return;

#if FIND_ME_ENABLE
        /* If the schedule OTA is not working, the value of my_entry_deep_time should be 0 */
        printf("[NOTICE] Check periodic wakeup for FMS !!! \n");
        app_fms_periodic_wake_up();
#endif

#if (TEST_SW_LED)
        if(led_test_sw_flag)
        {
            if(led_test_sleep == 0x01)
            {
                aw21036_all_off();
                ALS_Disable();
                aw21036_Disable();

                printf("[NOTICE] Idle to sleep \n");
                cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);            
            }            
        }
        else
        {
            aw21036_all_off();
            ALS_Disable();
            aw21036_Disable();

            printf("[NOTICE] Idle to sleep \n");
            cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);
            //cpu_sleep_wakeup(DEEPSLEEP_MODE_RET_SRAM_LOW32K, PM_WAKEUP_PAD, 0);
        }
#else
        aw21036_all_off();
        ALS_Disable();
        aw21036_Disable();

        printf("[NOTICE] Idle to sleep \n");
        cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);
        //cpu_sleep_wakeup(DEEPSLEEP_MODE_RET_SRAM_LOW32K, PM_WAKEUP_PAD, 0);
#endif
    }
}

void app_schedule_ota_send_notify_to_continue_ota(void)
{
    u8 dat[2] = {0};
    u8 report_data[4] = {CLIENT_REPORT_CMD,0x04,0x73,0x6f};
    u8 data = analog_read(DEEP_ANA_REG2);

    dat[0] = 0x73;
    dat[1] = 0x6f;
    if((APP_WAKEUP_SRC_DEEP_TIMER == app_ui_get_wakeup_src()) && (data))//wake up for timer
    {
        #if (!CLOSE_CODE_PRINTF)
        printf("app_schedule_ota_send_notify_to_continue_ota\r\n");
        #endif
        bls_att_pushNotifyData(HID_CONSUME_REPORT_INPUT_DP_H, dat, 2);
        bls_att_pushNotifyData(OTA_CMD_OUT_DP_H, report_data, 4);
        analog_write(DEEP_ANA_REG2, 0);
    }
}

void app_schedule_ota_wakeup_host(void)
{
    u8 data = analog_read(DEEP_ANA_REG2);
    u8 flag=0;

    if((APP_WAKEUP_SRC_DEEP_TIMER == app_ui_get_wakeup_src()) && (data) )
    {
        flag = app_custom_is_enable_wakeup_key();
        if(flag)
        {
            #if (!CLOSE_CODE_PRINTF)
            printf("app_schedule_ota_wakeup_host\r\n");
            #endif
//            wakeup_key_send_flag = 2;
            app_woble_schedule_ota(wakeup_key_keycount);
        }
    }
}

#endif

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


//This function process ...
void deep_wakeup_proc(void)
{

#if(DEEPBACK_FAST_KEYSCAN_ENABLE)
    //if deepsleep wakeup is wakeup by GPIO(key press), we must quickly scan this
    //press, hold this data to the cache, when connection established OK, send to master
    //deepsleep_wakeup_fast_keyscan
    if(analog_read(USED_DEEP_ANA_REG) & CONN_DEEP_FLG){
        if(kb_scan_key (KB_NUMLOCK_STATUS_POWERON, 1) && kb_event.cnt){
            deepback_key_state = DEEPBACK_KEY_CACHE;
            key_not_released = 1;
            memcpy(&kb_event_cache,&kb_event,sizeof(kb_event));
        }

        analog_write(USED_DEEP_ANA_REG, analog_read(USED_DEEP_ANA_REG) & (~CONN_DEEP_FLG));
    }
#endif
}

void deepback_pre_proc(int *det_key)
{
#if (DEEPBACK_FAST_KEYSCAN_ENABLE)
    // to handle deepback key cache
    if(!(*det_key) && deepback_key_state == DEEPBACK_KEY_CACHE && blc_ll_getCurrentState() == BLS_LINK_STATE_CONN \
            && clock_time_exceed(bls_ll_getConnectionCreateTime(), 25000)){

        memcpy(&kb_event,&kb_event_cache,sizeof(kb_event));
        *det_key = 1;

        if(key_not_released || kb_event_cache.keycode[0] == VOICE){  //no need manual release
            deepback_key_state = DEEPBACK_KEY_IDLE;
        }
        else{  //need manual release
            deepback_key_tick = clock_time();
            deepback_key_state = DEEPBACK_KEY_WAIT_RELEASE;
        }
    }
#endif
}

void deepback_post_proc(void)
{
#if (DEEPBACK_FAST_KEYSCAN_ENABLE)
    //manual key release
    if(deepback_key_state == DEEPBACK_KEY_WAIT_RELEASE && clock_time_exceed(deepback_key_tick,150000)){
        key_not_released = 0;
        u8         key_buf[8] = {0};
        key_buf[2] = 0;
        bls_att_pushNotifyData (HID_NORMAL_KB_REPORT_INPUT_DP_H, key_buf, 8); //release
        deepback_key_state = DEEPBACK_KEY_IDLE;
    }
#endif
}

#include "application/audio/gl_audio.h"

extern u8 app_audio_key_flags;
extern u8 htt_audio_model_key_press_flags;



#if (STUCK_KEY_PROCESS_ENABLE)
     extern u32 stuckkey_keypresstimer;
#endif

extern void app_adv_indirect(void);
u8 app_repairing(void)
{
    if(repairing_flag == 0x55)
    {
        #if (!CLOSE_CODE_PRINTF)
         printf("app_repairing\r\n");
         #endif
         app_adv_indirect();
         repairing_flag = 0;
#if BLE_AUDIO_ENABLE
         notify_get_rsp_en = 0;
#endif
         return 1;
    }
    return 0;
}

#define  KEY_CACHE_TIMEOUT   8000000
#define  KEY_CACHE_MAX       22

_attribute_data_retention_ u8  app_key_store_count=0;
_attribute_data_retention_ u8  app_key_store_buffer[KEY_CACHE_MAX]={0};
u8 app_keycache(u8 keyindex)
{
    u8 i;

    u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
    if(bond_number == 0) return 0;

#if (!CLOSE_CODE_PRINTF)
    printf("%s en_cache:[%d] key_id:[%d] \n", __FUNCTION__, en_powerkey_cache, keyindex);
#endif
    if(en_powerkey_cache == 0)
    {
        //printf("[NOTICE] check key ID !!! ");
        if(keyindex == 0) return 0;
    }else{
        //printf("[NOTICE] push key to buffer !!! ");
    }


    if(device_in_connection_state == 0)
    {
        if(app_key_store_count < KEY_CACHE_MAX)
        {
            for(i=0;i<app_key_store_count;i++)
            {
                if(app_key_store_buffer[i] == keyindex)
                {
                    i = 0x55;
                    break;
                }
            }
            if(i != 0x55)
                app_key_store_buffer[app_key_store_count++] = keyindex;
        }
        else
        {
            app_key_store_count = KEY_CACHE_MAX;
            for(i=0; i<KEY_CACHE_MAX-1; i++)
            {
                app_key_store_buffer[i] = app_key_store_buffer[i+1];
            }
            app_key_store_buffer[i] = keyindex;
        }
        //printf("app_keycache = \r\n");
        //for(i=0;i<app_key_store_count;i++)
        //    printf(" %x",app_key_store_buffer[i]);
        return 0;
    }
    else if(device_in_connection_state && app_key_store_count != 0)
    {
        if(app_key_store_count < KEY_CACHE_MAX)
        {
            app_key_store_buffer[app_key_store_count++] = keyindex;
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

int app_cachekey_send_timer(void)
{
    u8 i;
    u8 key0;
    u8 key_value;
    //u8 key_buf[8] = {0,0,0,0,0,0,0,0};
    //u16 consumer_key[2]={0};

    if(device_in_connection_state == 0) return 0;
    if(app_key_store_count == 0) return -1;
    #if (!CLOSE_CODE_PRINTF)
    printf("app_key_send_store,app_key_store_count=%x\r\n",app_key_store_count);
    #endif

    key0 = app_key_store_buffer[0];
    #if (!CLOSE_CODE_PRINTF)
    printf("send cache key key=%x\r\n",key0);
    #endif
    {
#if APP_IR_OVER_BLE
        programming_key_set(0);
#endif
        key_value = p_kb_map_ble[key0];
        #if 0
        if(key_value >= CR_MEDIA_KEY_INDEX )
        {
            consumer_key[0] = vk_consumer_map[key_value-CR_MEDIA_KEY_INDEX];
            consumer_key[1] = 0;
            #if (!CLOSE_CODE_PRINTF)
            printf("consume key=%x \r\n",consumer_key[0]);     //down
            #endif
            bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, (u8 *)&consumer_key, 4);
            consumer_key[0] = 0;                             //release
            consumer_key[1] = 0;
            bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, (u8 *)&consumer_key, 4);

        }
        else
        {
            #if (!CLOSE_CODE_PRINTF)
            printf("KEYBOARD_KEY_key_value=%x \r\n",key_value);
            #endif
            key_buf[2] = key_value;                       //down
            bls_att_pushNotifyData (HID_NORMAL_KB_REPORT_INPUT_DP_H, key_buf, 8);

            key_buf[2] = 0;                               //release
            bls_att_pushNotifyData (HID_NORMAL_KB_REPORT_INPUT_DP_H, key_buf, 8); //release
        }
        #endif
    }

    for(i=1; i<app_key_store_count; i++)
    {
        app_key_store_buffer[i-1] = app_key_store_buffer[i];
    }
    app_key_store_count --;
    if(app_key_store_count == 0)
        return -1;
    else
        return 0;
}


void app_key_send_store(void)
{
    u8 i;
    u8 key0;
    u8 key_value;
    u8 key_buf[8] = {0,0,0,0,0,0,0,0};
    u16 consumer_key[2]={0};

    if(app_key_store_count == 0) return;
    if(device_in_connection_state == 0) return;
    #if (!CLOSE_CODE_PRINTF)
    printf("app_key_send_store,app_key_store_count=%x\r\n",app_key_store_count);
    #endif

    key0 = app_key_store_buffer[0];
    #if (!CLOSE_CODE_PRINTF)
    printf("send cache key key=%x\r\n",key0);
    #endif
    /*
    if(ir_key_is_suppress(key0) == 0)
    {
        printf("is suppress\r\n");
        if(ir_fallback_send_key_code(key0,1))
        {
            ir_fallback_send(0);   //release
        }
    }
    else
    */
    {
#if APP_IR_OVER_BLE
        programming_key_set(0);
#endif
        key_value = p_kb_map_ble[key0];
        if(key_value >= CR_MEDIA_KEY_INDEX )
        {
            consumer_key[0] = vk_consumer_map[key_value-CR_MEDIA_KEY_INDEX];
            consumer_key[1] = 0;
            #if (!CLOSE_CODE_PRINTF)
            printf("consume key=%x \r\n",consumer_key[0]);   //down
            #endif
            bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, (u8 *)&consumer_key, 4);
            consumer_key[0] = 0;                             //release
            consumer_key[1] = 0;
            bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, (u8 *)&consumer_key, 4);

        }
        else
        {
            #if (!CLOSE_CODE_PRINTF)
            printf("KEYBOARD_KEY_key_value=%x \r\n",key_value);
            #endif
            key_buf[2] = key_value;                       //down
            bls_att_pushNotifyData (HID_NORMAL_KB_REPORT_INPUT_DP_H, key_buf, 8);

            key_buf[2] = 0;                               //release
            bls_att_pushNotifyData (HID_NORMAL_KB_REPORT_INPUT_DP_H, key_buf, 8); //release
        }
    }

    for(i=1; i<app_key_store_count; i++)
    {
        app_key_store_buffer[i-1] = app_key_store_buffer[i];
    }
    app_key_store_count --;
}


u8 app_is_navi_key(u8 data0,u8 data1)
{
    u8 flag[2]={5,5};

    if(data0 == data1) return 0;
    if((flag[0]!=5) && (flag[1]!=5))
        return 1;
    return 0;
}

void app_release_hid_key(void)
{
    u8 key_buf[8] = {0,0,0,0,0,0,0,0};
    u16 consumer_key[2]={0};
    u16 i=0;

    if(device_in_connection_state)
    {
        while(bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, (u8 *)&consumer_key, 4) && i<2000)
        {
            sleep_us(100);
            i++;
        }
        i = 0;
        while(bls_att_pushNotifyData (HID_NORMAL_KB_REPORT_INPUT_DP_H, key_buf, 8) && i<2000)
        {
            sleep_us(100);
            i++;
        }
        i = 0;
        while(app_terminate(HCI_ERR_REMOTE_USER_TERM_CONN) &&  i < 2000)
        {
            sleep_us(100);
            i++;
        }
        i =0;
        while(blc_ll_getTxFifoNumber() && i<400)
        {
            sleep_us(1000);
            i++;
        }
        #if (!CLOSE_CODE_PRINTF)
        printf("app_release_hid_key\r\n");
        #endif
    }

}
void comb_key_stop_ir(void)
{
    if(device_in_connection_state == 0)
    {
        #if (!CLOSE_CODE_PRINTF)
        printf("comb_key_stop_ir_1\r\n");
        #endif
        if(ir_not_released)ir_not_released = 0;
        if( ir_send_ctrl.is_sending)
        {
            #if (!CLOSE_CODE_PRINTF)
             printf("comb_key_stop_ir_2\r\n");
             #endif
#if APP_IR_OVER_BLE
             programming_key_set(0);
#endif
             ir_send_release();
        }
    }
    else
    {
        //factory reset and key mute is ir programmed
        if(ir_send_ctrl.is_sending)
        {
            #if (!CLOSE_CODE_PRINTF)
             printf("comb_key_stop_ir and ir release\r\n");
             #endif
#if APP_IR_OVER_BLE
             programming_key_set(0);
#endif
             ir_send_release();
        }
    }
}

#define FACTORY_RESET_TIMEOUT 100000

_attribute_data_retention_     u8 factory_reset_flag = 0;
_attribute_data_retention_     u8 factory_reset_temi_conn_flag = 0;
_attribute_data_retention_     u8 factory_reset_timer_count = 0;

int app_factory_reset_timer(void)
{
    u8 bond_number;
    //printf("app_factory_reset_timer\r\n");
    if(device_in_connection_state)
    {
#if BLE_AUDIO_ENABLE
        #if (!CLOSE_CODE_PRINTF)
        printf("app_factory_reset_timer_google_voice_ctl=%x\r\n",google_voice_ctl);
        #endif
        u16 i=0;
        if(((google_voice_ctl & FLAG_AUDIO_CLOSE) == 0) || (ota_is_working))
        {
            if(ota_is_working)
            {
                app_ota_slave_terminate();
                device_led_off(APP_LED_WHITE);
            }
            if(factory_reset_temi_conn_flag == 0)
            {
                i = 0;
                while(app_terminate(HCI_ERR_REMOTE_USER_TERM_CONN) &&  i < 2000){
                sleep_us(100);
                i++;
                }
            }
            else
            {
                if(blc_ll_getTxFifoNumber())
                {
                    factory_reset_temi_conn_flag++;
                    if(factory_reset_temi_conn_flag < 20)
                    {
                        return 0;
                    }
                }
            }
        }
        else
#endif
        {
            factory_reset_timer_count++;
            if(factory_reset_timer_count < 20)
            {
                return 0;
            }
        }
    }

#if APP_IR_OVER_BLE
    ir_flash_factory();
#endif
    flash_erase_sector(MAC_DATA_SECT_ADDR);
//    u8 data=0;
//    flash_read_page(APP_DATA_UPDATE_OVER_OTA_ADDR, 1, &data);
//    if(data != 0xff)
//        flash_erase_sector(APP_DATA_UPDATE_OVER_OTA_ADDR);

    /* Preserve NEC IR data during Factory rest in v2.10 */
//    flash_read_page(APP_NEC_IR_CODE_TABLE, 1, &data);
//    if(data != 0xff)
//        flash_erase_sector(APP_NEC_IR_CODE_TABLE);

    bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
    #if (!CLOSE_CODE_PRINTF)
    printf("bond_number=%x\r\n",bond_number);
    #endif
    if(bond_number)
    {
        #if (!CLOSE_CODE_PRINTF)
        printf("delete bond info\r\n");
        #endif
        bls_smp_param_deleteByIndex(0);
    }
    factory_reset_flag = 0;
    factory_reset_temi_conn_flag = 0;
    factory_reset_timer_count = 0;
#if BLE_AUDIO_ENABLE
    google_voice_ctl = 0;
#endif
    ota_is_working = 0;

    return -1;
}

#if BLE_AUDIO_ENABLE
void app_mic_led(u8 en)
{
    if(audio_start_reason == REASON_PTT)
    {
        if(en)
        {
            device_led_setup(led_cfg[LED_SHINE_PTT]);
        }
        else
        {
            if(factory_reset_flag == 0)
            {
                device_led_setup(led_cfg[LED_AUDIO_OFF]);
            }
        }
    }
    else
    {
        if(en)
        {
            app_dcdc_enable();
            device_led_setup(led_cfg[LED_AUDIO_ON]);
        }
        else
        {
            if(factory_reset_flag == 0)
            {
                device_led_setup(led_cfg[LED_AUDIO_OFF]);
                app_dcdc_delay_disable();
            }
        }
    }
}
#endif

void app_keypress_led(u8         ir_key)
{
    if(lowbat == 0x55)
    {
        #if (!CLOSE_CODE_PRINTF)
        printf("low bat\r\n");
        #endif
        device_led_setup(led_cfg[LED_SHINE_LOWBAT]);
        if((is_pairing_mode == 0) && (is_reconn_mode == 0) && (ui_mic_enable == 0))
        {
            if(device_in_connection_state == 1)
            {
                device_led_setup(led_cfg[LED_SHINE_FAST]);
            }
        }
        return;
    }
    else
    {
          if((is_pairing_mode == 0) && (is_reconn_mode == 0) && (ui_mic_enable == 0))
          {
              if(device_in_connection_state == 1)
              {
                  device_led_setup(led_cfg[LED_SHINE_FAST]);
              }
          }
          if((ota_is_working == 0) && (device_in_connection_state == 0))
          {
              device_led_setup(led_cfg[LED_KEYPRESS]);
          }
    }
    if(ir_key)
    {
        if((is_pairing_mode == 0) && (ota_is_working == 0) && (ui_mic_enable == 0))
        {
            #if (!CLOSE_CODE_PRINTF)
            printf("ir db key press\r\n");
            #endif
            device_led_setup(led_cfg[LED_SHINE_FAST]);
            device_led_setup(led_cfg[LED_KEYPRESS]);
        }
    }
}

void app_connect_led(void)
{
    device_led_setup(led_cfg[LED_SHINE_SUCCESS]);
}

void app_pairing_led(void)
{
    device_led_setup(led_cfg[LED_SHINE_PAIRING]);
}

void app_accessibility_short_key_to_single_key(void)
{
    if(Accessibility_Shortcut_Mode != APP_ACCESSIBILITY_SHORTCUT_NONE)
    {
        #if (!CLOSE_CODE_PRINTF)
        printf("app_accessibility_short_key_to_single_key\r\n");
        #endif
        Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_SINGLEKEY;
    }
    #if 0
    if((is_pairing_mode == 0))
    {
        printf("app_accessibility_short_key_to_single_key_close led\r\n");
        device_led_off(APP_LED_BLUE);
    }
    #endif
    tick_key_back_home = 0x00;
    key_handle_pressed_1_event();
}

int app_poweron_timeout(void)
{
     u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();
     #if (!CLOSE_CODE_PRINTF)
     printf("app_poweron_timeout\r\n");
     #endif
     if((bond_number == 0) && (en_ble_adv))
     {
         is_pairing_mode = 0x55;
#if BLE_AUDIO_ENABLE
         notify_get_rsp_en = 1;
#endif
         app_pairing_led();
         app_adv_indirect();
     }

     return -1;
}

int app_usermode_to_testmode_timeout(void)
{
    #if (!CLOSE_CODE_PRINTF)
     printf("app_usermode_to_testmode_timeout\r\n");
     #endif
     detpowerkey = 0;

     return -1;
}

void app_testmode_seq_record(u8 keyid)
{
#if !CLOSE_CODE_RAM
    if(detpowerkey)
    {
        printf("app_testmode_seq_record\r\n");
        if(app_custom_get_device_type() == REMOTE_G10)
        {
             if(testmode_key_seq[0] == Kb_Map_Testmode_Seq_Correspend_Index_RCU[0])
             {
                 if(testmode_key_seq_count<=4)
                 {
                     testmode_key_seq[testmode_key_seq_count] = keyid;
                     testmode_key_seq_count++;
                 }
             }
             else
             {
                 if(keyid == Kb_Map_Testmode_Seq_Correspend_Index_RCU[0])
                 {
                     testmode_key_seq[0] = Kb_Map_Testmode_Seq_Correspend_Index_RCU[0];
                     testmode_key_seq_count++;
                 }
             }
        }
        if(testmode_key_seq_count == 5)
        {
            printf("testmode_key_seq[0] =%d, %d, %d, %d, %d\r\n",testmode_key_seq[0],testmode_key_seq[1],testmode_key_seq[2],testmode_key_seq[3],testmode_key_seq[4]);
            if(app_custom_get_device_type() == REMOTE_G10)
            {
                if(memcmp(testmode_key_seq,Kb_Map_Testmode_Seq_Correspend_Index_RCU,5) == 0)
                   {
                       //printf("user mode to test mode g10\r\n");
                    flash_erase_sector(APP_TEST_SECT_ADDR);
                    start_reboot();
                   }
            }
        }
    }
#endif
}


void app_factory_reset(void)
{
    u8 audio_open=0;

    //printf("Factory Reset Mode ongoning\r\n");
    if(lowbat == 0x55)
    {
        return;
    }

    //global vari
    factory_reset_flag = 0x55;
    is_reconn_mode = 0;
    is_pairing_mode = 0;

    flag_ccc_data = 0;
#if BLE_AUDIO_ENABLE
    atv_char_ctl_ccc = 0;
    atv_char_rx_ccc = 0;
#endif
    write_ccc_info(&flag_ccc_data);
    //led
    device_led_setup(led_cfg[LED_SHINE_SUCCESS]);

    //ir
    write_ir_key_event_notify(0);
#if APP_IR_OVER_BLE
    ir_table_init();
#endif
    analog_write(DEEP_ANA_REG2, 0);
    bls_ll_setAdvEnable(0);
    if(device_in_connection_state)
    {
        //printf("Accessibility_Shortcut_Mode=%x\r\n",Accessibility_Shortcut_Mode);
#if BLE_AUDIO_ENABLE
        if(ui_mic_enable)
        {
            audio_open = 1;
            extern u8 app_audio_key_stop(u8 reason);
            app_audio_key_stop(REASON_OTHERS);
        }
#endif
    }
    extern void app_enter_deep_timeset(void);
    app_enter_deep_timeset();
    if(audio_open)
        blt_soft_timer_add(app_factory_reset_timer, FACTORY_RESET_TIMEOUT);
    else
        app_factory_reset_timer();

}


extern u32    scan_pin_need;

void app_ui_init_normal(void)
{
    kb_wake_config();

    bls_app_registerEventCallback (BLT_EV_FLAG_GPIO_EARLY_WAKEUP, &proc_keyboard);

    #if (BLE_AUDIO_ENABLE)
        #if (BLE_DMIC_ENABLE)
            dmic_gpio_reset();
        #else
            amic_gpio_reset();
        #endif
    #endif


#if (BLT_APP_LED_ENABLE)
    led_driver_init();
    device_led_off(APP_LED_WHITE);

    //aw21036_init();  
    ltr_sensor_init();

    // TODO
    if(APP_WAKEUP_SRC_POWER_ON == app_ui_get_wakeup_src())
    {
        //device_led_setup(led_cfg[LED_POWER_ON]);  //only poweron
    }
#endif

#if (REMOTE_IR_ENABLE)
    user_key_mode = analog_read(USED_DEEP_ANA_REG) & IR_MODE_DEEP_FLG ? KEY_MODE_IR : KEY_MODE_BLE;
#endif


#if (BLT_TEST_SOFT_TIMER_ENABLE)
    blt_soft_timer_init();

#if (MP_TEST_MODE)
    if((APP_WAKEUP_SRC_POWER_ON == app_ui_get_wakeup_src()) && (test_get_mode() != MODE_TEST))
#else
    if((APP_WAKEUP_SRC_POWER_ON == app_ui_get_wakeup_src()))
#endif
    {
        //blt_soft_timer_add(app_poweron_timeout, 1000000);
    }
    if(detpowerkey == 1)
    {
        blt_soft_timer_add(app_usermode_to_testmode_timeout, 20000000);
    }
#endif
}



void app_ui_init_deepRetn(void)
{
    kb_wake_config();

    #if (BLE_AUDIO_ENABLE)
        #if (BLE_DMIC_ENABLE)
            dmic_gpio_reset();
        #else
            amic_gpio_reset();
        #endif
    #endif

#if (BLT_APP_LED_ENABLE)
    
    led_driver_init();

    //aw21036_init();
    //ltr_sensor_init();    
#endif

#if (REMOTE_IR_ENABLE)
    user_key_mode = analog_read(USED_DEEP_ANA_REG) & IR_MODE_DEEP_FLG ? KEY_MODE_IR : KEY_MODE_BLE;
#endif
}


extern u16 atv_char_ctl_ccc;
extern u16 atv_char_rx_ccc;
int att_ccc_control(void* p){
    #if (!CLOSE_CODE_PRINTF)
    printf("att_ccc_control\n");
    #endif
    rf_packet_att_data_t *pw = (rf_packet_att_data_t *)p;
    u16 handle = pw->handle;

    u16 data = (pw->dat[1] << 8) + pw->dat[0];

    array_printf(pw->dat,(pw->l2cap - 3));

    u8 save_to_flash = 1;

    switch(handle){
        case DeviceInformation_serialNum_DP_H:
        {
            u8 sendBuff[2] = {0};
            memset(my_SerialNumtrs, 0, 30);
            #if (!CLOSE_CODE_PRINTF)
            printf("--->>> [work_debug] DeviceInformation_serialNum_DP_H\n");
            #endif
            memcpy(my_SerialNumtrs, pw->dat, 4);
            if ((strcmp((char*)my_SerialNumtrs,"EU01") != 0) && (strcmp((char*)my_SerialNumtrs,"LM01") != 0)) 
            {
                #if (!CLOSE_CODE_PRINTF)
                printf("--->>> Default_Project = [EU01]\r\n");
                #endif
                strncpy((char*)my_SerialNumtrs,"EU01",4); 
            }
            sendBuff[0] = 0x00;
            sendBuff[1] = 0x0D;
            seneasy_rcu_write_fw_addr(my_SerialNumtrs);
            bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, sendBuff, 2);
            break;
        }
#if  BLE_AUDIO_ENABLE

        case AUDIO_GOOGLE_RX_CCC_H:
        {
            #if (!CLOSE_CODE_PRINTF)
            printf("AUDIO_GOOGLE_RX_CCC_H\n");
            #endif
            atv_char_rx_ccc = data;

            if(data){
                if(flag_ccc_data & FLAG_GOOGLE_RX_CCC){
                    save_to_flash = 0;
                }else{
                    flag_ccc_data |= FLAG_GOOGLE_RX_CCC;
                }

            }else{
                if(flag_ccc_data & FLAG_GOOGLE_RX_CCC){
                    flag_ccc_data &= ~FLAG_GOOGLE_RX_CCC;
                }else{
                    save_to_flash = 0;
                }
            }
            break;
        }
        case AUDIO_GOOGLE_CTL_CCC_H:
        {
            #if (!CLOSE_CODE_PRINTF)
            printf("AUDIO_GOOGLE_CTL_CCC_H\n");
            #endif
            atv_char_ctl_ccc = data;

            if(data){
                if(flag_ccc_data & FLAG_GOOGLE_CTL_CCC){
                    save_to_flash = 0;
                }else{
                    flag_ccc_data |= FLAG_GOOGLE_CTL_CCC;
                }

            }else{
                if(flag_ccc_data & FLAG_GOOGLE_CTL_CCC){
                    flag_ccc_data &= ~FLAG_GOOGLE_CTL_CCC;
                }else{
                    save_to_flash = 0;
                }
            }
            break;
        }
 #endif
        #if 0
        case OTA_CMD_OUT_CCC_H:
        {
            printf("OTA_CMD_OUT_CCC_H\n");

            extern u16 my_Otaccc;
            my_Otaccc = data;

            save_to_flash = 0;
            break;
        }
        #endif
        default:
        {
            save_to_flash = 0;
            break;
        }
    }

    if(save_to_flash){
        //save ccc data to flash
        write_ccc_info(&flag_ccc_data);
    }

    return 0;
}

int app_is_key_released(void)
{
    return key_not_released;
}


/* Wake up source related functions */
_attribute_data_retention_ static u8 s_app_ui_wakeup_src = APP_WAKEUP_SRC_NONE;
static void app_ui_set_wakeup_src(APP_UI_WAKEUP_SRC_T wakeup_src){

    if(wakeup_src >= APP_WAKEUP_SRC_NOT_USE) return;
    s_app_ui_wakeup_src = wakeup_src;

    printf("wakeup source: {%s} !!!! \n",
                (APP_WAKEUP_SRC_POWER_ON==s_app_ui_wakeup_src)?"POWER_ON":
                (APP_WAKEUP_SRC_DEEP_PAD==s_app_ui_wakeup_src)?"DEEP_PAD":
                ((APP_WAKEUP_SRC_DEEP_TIMER==s_app_ui_wakeup_src)?"DEEP_TIMER":"SOFT_RESET"));
}

APP_UI_WAKEUP_SRC_T app_ui_get_wakeup_src(void){

    return s_app_ui_wakeup_src;
}

void app_ui_check_wakeup_src(u8 deepRetWakeUp){
    if((analog_read(USED_DEEP_ANA_REG) & POWER_ON_FLG) == 0){
        app_ui_set_wakeup_src(APP_WAKEUP_SRC_POWER_ON);
    }else if(!deepRetWakeUp){

        u8 my_power_flags = analog_read(0X44);
        #if (!CLOSE_CODE_PRINTF)
        printf(" my_power_flags: 0x%x \n", my_power_flags);
        #endif
        if(WAKEUP_STATUS_PAD == (my_power_flags & (WAKEUP_STATUS_TIMER|WAKEUP_STATUS_PAD))){
            app_ui_set_wakeup_src(APP_WAKEUP_SRC_DEEP_PAD);
        }else if(my_power_flags & APP_WAKEUP_SRC_DEEP_TIMER){
            app_ui_set_wakeup_src(APP_WAKEUP_SRC_DEEP_TIMER);
        }else{
            app_ui_set_wakeup_src(APP_WAKEUP_SRC_SOFT_RESET);
        }
    }

    /* Keep the POWER_ON_FLG bit as 1. This bit will be cleared when power on */
    analog_write(USED_DEEP_ANA_REG, analog_read(USED_DEEP_ANA_REG) | POWER_ON_FLG);
}


void app_ui_check_delay_en_pm(void){
    /* 221116 by ken. clear delay PM flag. The delay condition is to press the ASSIST and CENTER buttons */
    if(g_delay_en_pm && clock_time_exceed(g_delay_en_pm, 2000000)){
        g_delay_en_pm = 0;
        #if (!CLOSE_CODE_PRINTF)
        printf("[NOTICE] Clear delay PM flag ");
        #endif
    }
}
