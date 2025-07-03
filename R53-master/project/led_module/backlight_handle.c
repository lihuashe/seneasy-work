/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     backlight_handle.c
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
#include "backlight_handle.h"
#include "led_driver.h"
#include "ltr_sensor.h"
#include "aw21036_driver.h"
#include "bct3236_driver.h"
#include "../key_module/key_handle.h"
#include "../key_module/key_driver.h"
#include "../../sdk/drivers/8278/i2c.h"
#include "../../sdk/drivers/8278/gpio.h"
#include "../app_rcu.h"
#include "../profile_module/dis_profile.h"
#include "../bat_module/battery_check.h"
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
#define test_key_count      34//SRC3009-32  SRC3215-34
u8 tick_key_up_home = 0x00;
u8 tick_key_power = 0x00;
u8 tick_key_ambilight = 0x00;
u8 tick_key_ott_3 = 0x00;
u8 tick_key_ott_4 = 0x00;
u8 tick_backlight_normal = 0x00;
u8 tick_backlight_hidden = 0x00;
u8 tick_backlight_all = 0x00;
u8 key_level_flag = 0;//defalut 100%
static u8 test_toggle_one_cnt = 0;
extern _attribute_data_retention_ u32 keyScanLongPressTick;
extern _attribute_data_retention_ u32 LongPressTick;
extern _attribute_data_retention_ APP_Accessibility_Shortcut_Mode  Accessibility_Shortcut_Mode;
extern u8 device_in_connection_state;
_attribute_data_retention_ u32 app_backlight_normal_timer = 0; 
_attribute_data_retention_ u32 app_backlight_hidden_timer = 0; 
_attribute_data_retention_ u32 app_backlight_all_timer = 0; 
_attribute_data_retention_ u32 LedDisplayTick = 0;
_attribute_data_retention_ bool led_test_sw_flag = false;
_attribute_data_retention_ bool backlight_normal_Flag = false; 
_attribute_data_retention_ bool backlight_hidden_Flag = false; 
_attribute_data_retention_ u8 key_123_press_flag = 0x00; 
_attribute_data_retention_ u8 key_press_quick_timer = 0x00; 
_attribute_data_retention_ int backlight_task = 0;
//led_on_buf
u8 led_on_buf[36] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//br_val level
u8 br_val_level[20] = {
    0xFF,   //100%  //500 Lux | Level:6
    0xF2,   //95%
    0xE5,   //90%
    0xD8,   //85%
    0xCC,   //80%   //400 Lux | Level:5
    0xBF,   //75%   
    0xB2,   //70% 
    0xA5,   //65%   //300 Lux | Level:4
    0x99,   //60% 
    0x8C,   //55%  
    0x7F,   //50%   //200 Lux | Level:3
    0x72,   //45%
    0x66,   //40%  
    0x59,   //35%   //100 Lux | Level:2
    0x4C,   //30%  
    0x3F,   //25%  
    0x33,   //20%   //50 Lux  | Level:1
    0x26,   //15%
    0x19,   //10%
    0x0C,   //5%    //10 Lux  | Level:0
#if 0
    0x0C,   //5%
    0x19,   //10%   //500 Lux | Level:6
    0x26,   //15%
    0x33,   //20%
    0x3F,   //25%   //400 Lux | Level:5
    0x4C,   //30%
    0x59,   //35%   
    0x66,   //40%   //300 Lux | Level:4
    0x72,   //45%   
    0x7F,   //50%   
    0x8C,   //55%   //200 Lux | Level:3
    0x99,   //60%
    0xA5,   //65%   
    0xB2,   //70%   //100 Lux | Level:2
    0xBF,   //75%   
    0xCC,   //80%   
    0xD8,   //85%   //50 Lux  | Level:1
    0xE5,   //90%
    0xF2,   //95%
    0xFF    //100%  //10 Lux  | Level:0
#endif
};
#if (TEST_SW_LED)
u8 led_test_sleep = 0x00;
#endif
/*============================================================================*
 *                              Functions Declaration
 *============================================================================*/
extern void advertise_tick_retime(void);
extern void app_backlight_HiddenKey_Display(u8 led_flag, u32 led_timer);
extern void LED_Test_SW_Toggle_ONE(void);

/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief  app backlight task at main loop.
 * @param  none
 * @return none
 * @retval void
 */
void app_backlight_task(void)
{
    if(app_backlight_normal_timer && clock_time_exceed(app_backlight_normal_timer, LedDisplayTick)){
        //backlight_task = 0;
        app_backlight_ic_close_timer = clock_time() | 1;
        advertise_tick_retime();
        #if (!CLOSE_CODE_PRINTF)
        printf("app_backlight_normal_timer timeout\n");
        #endif
        app_backlight_normal_timer = 0;
        aw21036_breath_quick(1);
        app_backlight_NormalKey_Display(AW_LED_OFF, 0);
    }
    if(app_backlight_hidden_timer && clock_time_exceed(app_backlight_hidden_timer, LedDisplayTick)){
        //backlight_task = 0;
        app_backlight_ic_close_timer = clock_time() | 1;   
        advertise_tick_retime();
        #if (!CLOSE_CODE_PRINTF)
        printf("app_backlight_hidden_timer timeout\n");
        #endif
        app_backlight_hidden_timer = 0;
        aw21036_breath_quick(1);
        app_backlight_HiddenKey_Display(AW_LED_OFF, 0);
    }
    if(app_backlight_all_timer && clock_time_exceed(app_backlight_all_timer, LedDisplayTick)){
        //backlight_task = 0;
        app_backlight_ic_close_timer = clock_time() | 1;   
        advertise_tick_retime();
        #if (!CLOSE_CODE_PRINTF)
        printf("app_backlight_all_timer timeout\n");
        #endif
        app_backlight_all_timer = 0;
        if((backlight_normal_Flag == true) && (backlight_hidden_Flag == true))
        {
            #if (LED_AW21036_ENABLE)
                aw21036_backlight_AllKey(AW_LED_OFF);
            #endif
            #if (LED_BCT3236_ENABLE)
                bct3236_backlight_AllKey(AW_LED_OFF);
            #endif            
        }
        else
        {
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
        } 
        //aw21036_all_off(); 
        app_hidden_key_mode = false;
        app_hidden_close_flag = true; 
        if(ASL_Lux_Calc <= BR_LUX_LEVEL_1)
            hidden_LongPressTick = 200 * 1000;         
        else if(ASL_Lux_Calc <= BR_LUX_LEVEL_2)
            hidden_LongPressTick = 400 * 1000;       
        else if(ASL_Lux_Calc <= BR_LUX_LEVEL_3)
            hidden_LongPressTick = 600 * 1000;         
        else if(ASL_Lux_Calc <= BR_LUX_LEVEL_4)
            hidden_LongPressTick = 800 * 1000;        
        else
            hidden_LongPressTick = 1000 * 1000;                      
        app_hidden_close_timer = clock_time() | 1;
    }
}

/******************************************************************
 * @brief  app_backlight_model_function
 * @param  none
 * @return none
 * @retval void
 */
void app_backlight_model_function(u8 light_model)
{
    app_backlight_normal_timer = 0;
    app_backlight_hidden_timer = 0;
    app_backlight_all_timer = 0;
    tick_backlight_normal = 0;
    tick_backlight_hidden = 0;
    tick_backlight_all = 0;

    //printf("--->>> [work_debug] light_model: %d\r\n",light_model);
    switch (light_model)
    {
    case LED_NORMAL_ON2S:  //normal:off--->on 2s  hidden: N/A
        app_backlight_NormalKey_Display(AW_LED_ON, 2*1000*1000);
        break;
    case LED_NORMAL_ON5S:  //normal:off--->on 5s  hidden: N/A
        app_backlight_NormalKey_Display(AW_LED_ON, 5*1000*1000);
        break; 
    case LED_NORMAL_ON2S_HIDDEN_ON2S:  //normal:off--->on 2s  hidden: off--->on 2s
        if((backlight_normal_Flag == false) && (backlight_hidden_Flag == false))
        {
            #if (LED_AW21036_ENABLE)
                aw21036_backlight_AllKey(AW_LED_ON);
            #endif
            #if (LED_BCT3236_ENABLE)
                bct3236_backlight_AllKey(AW_LED_ON);
            #endif            
        }
        else
        {
            if(backlight_normal_Flag == false)
            {
                #if (LED_AW21036_ENABLE)
                    aw21036_backlight_NormalKey(AW_LED_ON);
                #endif
                #if (LED_BCT3236_ENABLE)
                    bct3236_backlight_NormalKey(AW_LED_ON);
                #endif
            }
            if(backlight_hidden_Flag == false)
            {
                #if (LED_AW21036_ENABLE)
                    aw21036_backlight_HiddenKey(AW_LED_ON);
                #endif
                #if (LED_BCT3236_ENABLE)
                    bct3236_backlight_HiddenKey(AW_LED_ON);
                #endif
            }
        }
        tick_backlight_all = 0x02;
        break; 
    case LED_NORMAL_ON5S_HIDDEN_ON5S:  //normal:off--->on 5s  hidden: off--->on 5s
        if((backlight_normal_Flag == false) && (backlight_hidden_Flag == false))
        {
            #if (LED_AW21036_ENABLE)
                aw21036_backlight_AllKey(AW_LED_ON);
            #endif
            #if (LED_BCT3236_ENABLE)
                bct3236_backlight_AllKey(AW_LED_ON);
            #endif            
        }
        else
        {
            if(key_123_press_flag == 0x01)
            {
                key_123_press_flag = 0x00;
                aw21036_breath_quick(0);
            }
            if(backlight_normal_Flag == false)
            {
                #if (LED_AW21036_ENABLE)
                    aw21036_backlight_NormalKey(AW_LED_ON);
                #endif
                #if (LED_BCT3236_ENABLE)
                    bct3236_backlight_NormalKey(AW_LED_ON);
                #endif                
            }
            if(backlight_hidden_Flag == false)
            {
                #if (LED_AW21036_ENABLE)
                    aw21036_backlight_HiddenKey(AW_LED_ON);
                #endif
                #if (LED_BCT3236_ENABLE)
                    bct3236_backlight_HiddenKey(AW_LED_ON);
                #endif                 
            }
        }
        tick_backlight_all = 0x05;
        break; 
    case LED_NORMAL_ON2S_HIDDEN_OFF:  //normal:off--->on 2s  hidden: on--->off
        Quick_WaitMs_Flag = 1;
        //app_backlight_model_function(LED_NORMAL_ON2S);
        if(backlight_hidden_Flag == true)
        {
            key_press_quick_timer = 0x01;
            #if (LED_AW21036_ENABLE)
                aw21036_backlight_HiddenKey(AW_LED_OFF);
            #endif
            #if (LED_BCT3236_ENABLE)
                LedDisplayTick = 2200*1000;
                app_backlight_normal_timer = clock_time() | 1; //none zero
                bct3236_backlight_HiddenKey(AW_LED_OFF);
            #endif                
        }
        break; 
    default:
        break;
    }
}

/******************************************************************
 * @brief  app_backlight_NormalKey_Display
 * @param  led_flag, led_timer
 * @return none
 * @retval void
 */
void app_backlight_NormalKey_Display(u8 led_flag, u32 led_timer)
{
    if(led_flag == AW_LED_ON)
    {
        if(backlight_normal_Flag == false)
        {
            #if (LED_AW21036_ENABLE)
                aw21036_backlight_NormalKey(AW_LED_ON);
            #endif
            #if (LED_BCT3236_ENABLE)
                bct3236_backlight_NormalKey(AW_LED_ON);
            #endif
        }  
        if(led_timer == 2*1000*1000)
            tick_backlight_normal = 0x02;
        if(led_timer == 5*1000*1000)
            tick_backlight_normal = 0x05;
    }
    else if(led_flag == AW_LED_OFF)
    {
        if(backlight_normal_Flag == true)
        {
            #if (LED_AW21036_ENABLE)
                aw21036_backlight_NormalKey(AW_LED_OFF);
            #endif
            #if (LED_BCT3236_ENABLE)
                bct3236_backlight_NormalKey(AW_LED_OFF);
            #endif            
        }
    }
}

/******************************************************************
 * @brief  app_backlight_HiddenKey_Display
 * @param  led_flag, led_timer
 * @return none
 * @retval void
 */
void app_backlight_HiddenKey_Display(u8 led_flag, u32 led_timer)
{
    if(led_flag == AW_LED_ON)
    {
        if(backlight_hidden_Flag == false)
        {
            #if (LED_AW21036_ENABLE)
                aw21036_backlight_HiddenKey(AW_LED_ON);
            #endif
            #if (LED_BCT3236_ENABLE)
                bct3236_backlight_HiddenKey(AW_LED_ON);
            #endif            
        }
        if(led_timer == 2*1000*1000)
            tick_backlight_hidden = 0x02;
        if(led_timer == 5*1000*1000)
            tick_backlight_hidden = 0x05;        
    }
    else if(led_flag == AW_LED_OFF)
    {
        if(backlight_hidden_Flag == true)
        {
            #if (LED_AW21036_ENABLE)
                aw21036_backlight_HiddenKey(AW_LED_OFF);
            #endif
            #if (LED_BCT3236_ENABLE)
                bct3236_backlight_HiddenKey(AW_LED_OFF);
            #endif
        } 
    }
}

/******************************************************************
 * @brief  aw21036_backlight_NormalKey
 * @param  led_flag
 * @return none
 * @retval void
 */
void aw21036_backlight_NormalKey(u8 led_flag)
{
    if((app_get_bat_vol() > 2300) && (ASL_Lux_Calc <= BR_LUX_LEVEL_1))
    {
    u8 LED_COL_TEMP = 0x00;
    i2c_master_init(AW21036_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );
#if (PCB_LED_VER == PCB_LED_INIT)
    i2c_write_byte(REG_GCFG0,  1, 0xE0);
	i2c_write_byte(REG_GCFG1,  1, 0x17);
#elif (PCB_LED_VER == PCB_LED_POINT)
    if ((strcmp((char*)rcu_project_name,"EU01") == 0) ||\
        (strcmp((char*)rcu_project_name,"RU01") == 0))
    {
        i2c_write_byte(REG_GCFG0,  1, 0xE0);
	    i2c_write_byte(REG_GCFG1,  1, 0x1F);        
    }
    else if (strcmp((char*)rcu_project_name,"LM01") == 0)
    {
        i2c_write_byte(REG_GCFG0,  1, 0xE0);
	    i2c_write_byte(REG_GCFG1,  1, 0x17);     
    }
#endif
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [backlight_NormalKey] ASL_Lux_Calc: %d Lux\r\n",ASL_Lux_Calc);
    #endif
    if(ASL_Lux_Calc <= BR_LUX_LEVEL_0){
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> Sensor For %d Lux, Brightness Level 0: 5%% \r\n",BR_LUX_LEVEL_0);
        #endif
        key_level_flag = 19; 
    }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_1){
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> Sensor For %d Lux, Brightness Level 1: 25%% \r\n",BR_LUX_LEVEL_1);
        #endif
        key_level_flag = 15;         
    }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_2){
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> Sensor For %d Lux, Brightness Level 2: 50%% \r\n",BR_LUX_LEVEL_2);
        #endif
        key_level_flag = 10;         
    }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_3){
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> Sensor For %d Lux, Brightness Level 3: 75%% \r\n",BR_LUX_LEVEL_3);
        #endif
        key_level_flag = 5;         
    }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_4){
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> Sensor For %d Lux, Brightness Level 4: 100%% \r\n",BR_LUX_LEVEL_4);
        #endif
        key_level_flag = 0;       
    }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_5){
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> Sensor For %d Lux, Brightness Level 5: 120%% \r\n",BR_LUX_LEVEL_5);
        #endif
        key_level_flag = 0;
        LED_COL_TEMP = 0x33;//加至70%         
    }else{
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> Sensor For %d Lux, Brightness Level 6: 140%% \r\n",BR_LUX_LEVEL_5);
        #endif
        key_level_flag = 0;
        LED_COL_TEMP = 0x66;//加至90%         
    }
    aw21036_breath_one(LED_POWER,     led_flag, LED_COL_23 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_AMBILIGHT, led_flag, LED_COL_33 + LED_COL_TEMP, br_val_level[key_level_flag]);
#if (PCB_LED_VER == PCB_LED_INIT)
    aw21036_breath_one(LED_SOURCE,    led_flag, LED_COL_17 + LED_COL_TEMP, br_val_level[key_level_flag]);
#elif (PCB_LED_VER == PCB_LED_POINT)
    if ((strcmp((char*)rcu_project_name,"EU01") == 0) ||\
        (strcmp((char*)rcu_project_name,"RU01") == 0))
        aw21036_breath_one(LED_SOURCE,    led_flag, LED_COL_17 + LED_COL_TEMP, br_val_level[key_level_flag]);
#endif
    aw21036_breath_one(LED_VOICE,     led_flag, LED_COL_20 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_MENU,      led_flag, LED_COL_22 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_BACK,      led_flag, LED_COL_18 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_HOME,      led_flag, LED_COL_21 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_VOL_L,     led_flag, LED_COL_25 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_VOL_R,     led_flag, LED_COL_27 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_123,       led_flag, LED_COL_16 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_CHN_L,     led_flag, LED_COL_29 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_CHN_R,     led_flag, LED_COL_31 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_OTT_1,     led_flag, LED_COL_19 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_OTT_2,     led_flag, LED_COL_26 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_OTT_3,     led_flag, LED_COL_24 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_OTT_4,     led_flag, LED_COL_30 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_OTT_5,     led_flag, LED_COL_28 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_OTT_6,     led_flag, LED_COL_32 + LED_COL_TEMP, br_val_level[key_level_flag]);
/*
    aw21036_breath_one(LED_POWER,     led_flag, LED_COL_23, LED_BR_23);
    aw21036_breath_one(LED_AMBILIGHT, led_flag, LED_COL_33, LED_BR_33);
    aw21036_breath_one(LED_SOURCE,    led_flag, LED_COL_17, LED_BR_17);
    aw21036_breath_one(LED_VOICE,     led_flag, LED_COL_20, LED_BR_20);
    aw21036_breath_one(LED_MENU,      led_flag, LED_COL_22, LED_BR_22);
    aw21036_breath_one(LED_BACK,      led_flag, LED_COL_18, LED_BR_18);
    aw21036_breath_one(LED_HOME,      led_flag, LED_COL_21, LED_BR_21);
    aw21036_breath_one(LED_VOL_L,     led_flag, LED_COL_25, LED_BR_25);
    aw21036_breath_one(LED_VOL_R,     led_flag, LED_COL_27, LED_BR_27);
    aw21036_breath_one(LED_123,       led_flag, LED_COL_16, LED_BR_16);
    aw21036_breath_one(LED_CHN_L,     led_flag, LED_COL_29, LED_BR_29);
    aw21036_breath_one(LED_CHN_R,     led_flag, LED_COL_31, LED_BR_31);
    aw21036_breath_one(LED_OTT_1,     led_flag, LED_COL_19, LED_BR_19);
    aw21036_breath_one(LED_OTT_2,     led_flag, LED_COL_26, LED_BR_26);
    aw21036_breath_one(LED_OTT_3,     led_flag, LED_COL_24, LED_BR_24);
    aw21036_breath_one(LED_OTT_4,     led_flag, LED_COL_30, LED_BR_30);
    aw21036_breath_one(LED_OTT_5,     led_flag, LED_COL_28, LED_BR_28);
    aw21036_breath_one(LED_OTT_6,     led_flag, LED_COL_32, LED_BR_32);
*/
    if(led_flag)
    {
        backlight_normal_Flag = true;
        //WaitMs(led_on_wait_ms);
    }
    else
    {
        backlight_normal_Flag = false;
        //WaitMs(led_off_wait_ms);
    }
    }
    if((app_get_bat_vol() > 2300) && (ASL_Lux_Calc > BR_LUX_LEVEL_1))
    {
        if(led_flag)
            aw21036_all_off();
        u8 LED_COL_TEMP = 0x00;
        i2c_master_init(AW21036_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );
    #if (PCB_LED_VER == PCB_LED_INIT)
        i2c_write_byte(REG_GCFG0,  1, 0xE0);
        i2c_write_byte(REG_GCFG1,  1, 0x17);
    #elif (PCB_LED_VER == PCB_LED_POINT)
        if ((strcmp((char*)rcu_project_name,"EU01") == 0) ||\
            (strcmp((char*)rcu_project_name,"RU01") == 0))
        {
            i2c_write_byte(REG_GCFG0,  1, 0xE0);
            i2c_write_byte(REG_GCFG1,  1, 0x1F);        
        }
        else if (strcmp((char*)rcu_project_name,"LM01") == 0)
        {
            i2c_write_byte(REG_GCFG0,  1, 0xE0);
            i2c_write_byte(REG_GCFG1,  1, 0x17);     
        }
    #endif
        if(ASL_Lux_Calc <= BR_LUX_LEVEL_0){
            key_level_flag = 19; 
        }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_1){
            key_level_flag = 15;         
        }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_2){
            key_level_flag = 10;         
        }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_3){
            key_level_flag = 5;         
        }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_4){
            key_level_flag = 0;       
        }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_5){
            key_level_flag = 0;
            LED_COL_TEMP = 0x33;//加至70%         
        }else{
            key_level_flag = 0;
            LED_COL_TEMP = 0x66;//加至90%         
        }
        aw21036_breath_one(LED_AMBILIGHT, led_flag,   LED_COL_33 + LED_COL_TEMP, br_val_level[key_level_flag]);
        aw21036_breath_one(LED_VOL_L,     led_flag,   LED_COL_25 + LED_COL_TEMP, br_val_level[key_level_flag]);
        aw21036_breath_one(LED_VOL_R,     led_flag,   LED_COL_27 + LED_COL_TEMP, br_val_level[key_level_flag]);
        aw21036_breath_one(LED_CHN_L,     led_flag,   LED_COL_29 + LED_COL_TEMP, br_val_level[key_level_flag]);
        aw21036_breath_one(LED_CHN_R,     led_flag,   LED_COL_31 + LED_COL_TEMP, br_val_level[key_level_flag]);
        if(led_flag)
            backlight_normal_Flag = true;
        else
            backlight_normal_Flag = false;   
    }
}

/******************************************************************
 * @brief  aw21036_backlight_HiddenKey
 * @param  led_flag
 * @return none
 * @retval void
 */
void aw21036_backlight_HiddenKey(u8 led_flag)
{
    if(app_get_bat_vol() <= 2300)
        ASL_Lux_Calc = BR_LUX_LEVEL_0;

    u8 LED_COL_TEMP = 0x00;
    i2c_master_init(AW21036_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );
#if (PCB_LED_VER == PCB_LED_INIT)
    i2c_write_byte(REG_GCFG0,  1, 0x1F);
	i2c_write_byte(REG_GCFG1,  1, 0x10);
#elif (PCB_LED_VER == PCB_LED_POINT)
    if ((strcmp((char*)rcu_project_name,"EU01") == 0) ||\
        (strcmp((char*)rcu_project_name,"RU01") == 0))
    {
        i2c_write_byte(REG_GCFG0,  1, 0x1F);
	    i2c_write_byte(REG_GCFG1,  1, 0x10);        
    }
    else if (strcmp((char*)rcu_project_name,"LM01") == 0)
    {
        i2c_write_byte(REG_GCFG0,  1, 0x1F);
        i2c_write_byte(REG_GCFG1,  1, 0x18);        
    }
#endif
    #if (!CLOSE_CODE_PRINTF)
    printf("--->>> [aw21036_backlight_HiddenKey] ASL_Lux_Calc: %d Lux\r\n",ASL_Lux_Calc);
    #endif
    if(ASL_Lux_Calc <= BR_LUX_LEVEL_0){
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> Sensor For %d Lux, Brightness Level 0: 5%% \r\n",BR_LUX_LEVEL_0);
        #endif
        key_level_flag = 19; 
    }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_1){
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> Sensor For %d Lux, Brightness Level 1: 25%% \r\n",BR_LUX_LEVEL_1);
        #endif
        key_level_flag = 15;         
    }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_2){
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> Sensor For %d Lux, Brightness Level 2: 50%% \r\n",BR_LUX_LEVEL_2);
        #endif
        key_level_flag = 10;         
    }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_3){
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> Sensor For %d Lux, Brightness Level 3: 75%% \r\n",BR_LUX_LEVEL_3);
        #endif
        key_level_flag = 5;         
    }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_4){
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> Sensor For %d Lux, Brightness Level 4: 100%% \r\n",BR_LUX_LEVEL_4);
        #endif
        key_level_flag = 0;         
    }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_5){
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> Sensor For %d Lux, Brightness Level 5: 120%% \r\n",BR_LUX_LEVEL_5);
        #endif
        key_level_flag = 0;
        LED_COL_TEMP = 0x33;//加至70%           
    }else{
        #if (!CLOSE_CODE_PRINTF)
        printf("--->>> Sensor For %d Lux, Brightness Level 6: 140%% \r\n",BR_LUX_LEVEL_5);
        #endif
        key_level_flag = 0; 
        LED_COL_TEMP = 0x66;//加至90%         
    }
    aw21036_breath_one(LED_NUM_1,     led_flag, LED_COL_1 + LED_COL_TEMP,  br_val_level[key_level_flag]);
    if(ASL_Lux_Calc >= BR_LUX_LEVEL_4)
    {
        aw21036_breath_one(LED_NUM_2,     led_flag, 0xFF,  br_val_level[key_level_flag]);
        aw21036_breath_one(LED_NUM_5,     led_flag, 0xFF,  br_val_level[key_level_flag]);
        aw21036_breath_one(LED_NUM_6,     led_flag, 0xFF,  br_val_level[key_level_flag]);
        aw21036_breath_one(LED_NUM_8,     led_flag, 0xFF,  br_val_level[key_level_flag]);
    }
    else
    {
        aw21036_breath_one(LED_NUM_2,     led_flag, LED_COL_2,  br_val_level[key_level_flag]);
        aw21036_breath_one(LED_NUM_5,     led_flag, LED_COL_5,  br_val_level[key_level_flag]);
        aw21036_breath_one(LED_NUM_6,     led_flag, LED_COL_6,  br_val_level[key_level_flag]);
        aw21036_breath_one(LED_NUM_8,     led_flag, LED_COL_8,  br_val_level[key_level_flag]);
    }
    //aw21036_breath_one(LED_NUM_2,     led_flag, LED_COL_2 + LED_COL_TEMP,  br_val_level[key_level_flag]);
    aw21036_breath_one(LED_NUM_3,     led_flag, LED_COL_3 + LED_COL_TEMP,  br_val_level[key_level_flag]);
    aw21036_breath_one(LED_NUM_4,     led_flag, LED_COL_4 + LED_COL_TEMP,  br_val_level[key_level_flag]);
    //aw21036_breath_one(LED_NUM_5,     led_flag, LED_COL_5 + LED_COL_TEMP,  br_val_level[key_level_flag]);
    //aw21036_breath_one(LED_NUM_6,     led_flag, LED_COL_6 + LED_COL_TEMP,  br_val_level[key_level_flag]);
    aw21036_breath_one(LED_NUM_7,     led_flag, LED_COL_7 + LED_COL_TEMP,  br_val_level[key_level_flag]);
    //aw21036_breath_one(LED_NUM_8,     led_flag, LED_COL_8 + LED_COL_TEMP,  br_val_level[key_level_flag]);
    aw21036_breath_one(LED_NUM_9,     led_flag, LED_COL_9 + LED_COL_TEMP,  br_val_level[key_level_flag]);
    aw21036_breath_one(LED_NUM_0,     led_flag, LED_COL_10 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_REWIND,    led_flag, LED_COL_11 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_PAUSE,     led_flag, LED_COL_12 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_PLAY,      led_flag, LED_COL_13 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_FFWD,      led_flag, LED_COL_14 + LED_COL_TEMP, br_val_level[key_level_flag]);
#if (PCB_LED_VER == PCB_LED_POINT)
    if (strcmp((char*)rcu_project_name,"LM01") == 0)
        aw21036_breath_one(LED_SOURCE,    led_flag, LED_COL_17 + LED_COL_TEMP, br_val_level[key_level_flag]);
#endif

/*
    aw21036_breath_one(LED_NUM_1,     led_flag, LED_COL_1,  LED_BR_1);
    aw21036_breath_one(LED_NUM_2,     led_flag, LED_COL_2,  LED_BR_2);
    aw21036_breath_one(LED_NUM_3,     led_flag, LED_COL_3,  LED_BR_3);
    aw21036_breath_one(LED_NUM_4,     led_flag, LED_COL_4,  LED_BR_4);
    aw21036_breath_one(LED_NUM_5,     led_flag, LED_COL_5,  LED_BR_5);
    aw21036_breath_one(LED_NUM_6,     led_flag, LED_COL_6,  LED_BR_6);
    aw21036_breath_one(LED_NUM_7,     led_flag, LED_COL_7,  LED_BR_7);
    aw21036_breath_one(LED_NUM_8,     led_flag, LED_COL_8,  LED_BR_8);
    aw21036_breath_one(LED_NUM_9,     led_flag, LED_COL_9,  LED_BR_9);
    aw21036_breath_one(LED_NUM_0,     led_flag, LED_COL_10, LED_BR_10);
    aw21036_breath_one(LED_REWIND,    led_flag, LED_COL_11, LED_BR_11);
    aw21036_breath_one(LED_PAUSE,     led_flag, LED_COL_12, LED_BR_12);
    aw21036_breath_one(LED_PLAY,      led_flag, LED_COL_13, LED_BR_13);
    aw21036_breath_one(LED_FFWD,      led_flag, LED_COL_14, LED_BR_14);
*/
    if(led_flag)
    {
        backlight_hidden_Flag = true;
        //WaitMs(led_on_wait_ms);
    }
    else
    {
        backlight_hidden_Flag = false;
        if(key_press_quick_timer == 0x01)
        {
            key_press_quick_timer = 0x00;
            //WaitMs(led_off_key_wait_ms);
        }//else
            //WaitMs(led_off_wait_ms);
    }
}

/******************************************************************
 * @brief  aw21036_backlight_AllKey
 * @param  led_flag
 * @return none
 * @retval void
 */
void aw21036_backlight_AllKey(u8 led_flag)
{
    if(app_get_bat_vol() > 2300)
    {
        if(ASL_Lux_Calc <= BR_LUX_LEVEL_1){

    u8 LED_COL_TEMP = 0x00;
    i2c_master_init(AW21036_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );

    if(ASL_Lux_Calc <= BR_LUX_LEVEL_0)
        key_level_flag = 19; 
    else if(ASL_Lux_Calc <= BR_LUX_LEVEL_1)
        key_level_flag = 15;         
    else if(ASL_Lux_Calc <= BR_LUX_LEVEL_2)
        key_level_flag = 10;         
    else if(ASL_Lux_Calc <= BR_LUX_LEVEL_3)
        key_level_flag = 5;         
	else if(ASL_Lux_Calc <= BR_LUX_LEVEL_4)
        key_level_flag = 0;         
    else if(ASL_Lux_Calc <= BR_LUX_LEVEL_5)
    {
        key_level_flag = 0; 
        LED_COL_TEMP = 0x33;//加至70%  
    }        
    else
    {
        key_level_flag = 0; 
        LED_COL_TEMP = 0x66;//加至90% 
    } 
#if 0
	for (int i = 0; i < AW21036_LED_NUM; i++)
		aw21036_set_color(REG_COL0 + i, LED_COL_ALL);

    aw21036_set_brightness(REG_FADEH, br_val_level[key_level_flag]);
    //aw21036_set_brightness(REG_FADEH, LED_BR_ALL);
	aw21036_set_brightness(REG_FADEL, 0x00);
	i2c_write_byte(REG_GCFG0,  1, 0xff);
	i2c_write_byte(REG_GCFG1,  1, 0x1f);
	i2c_write_byte(REG_PATCT0, 1, led_timer_T0 | led_timer_T1);
	i2c_write_byte(REG_PATCT1, 1, led_timer_T2 | led_timer_T3);
    i2c_write_byte(REG_PATCT2, 1, 0x00);
	i2c_write_byte(REG_PATCT3, 1, 0x00);
	if(led_flag){
	    i2c_write_byte(REG_PATCFG, 1, 0x0d);
    }else{
	    i2c_write_byte(REG_PATCFG, 1, 0x05);
    }
	i2c_write_byte(REG_PATGO, 1, 0x01);
#else

    i2c_write_byte(REG_GCFG0,  1, 0xff);
	i2c_write_byte(REG_GCFG1,  1, 0x1f);

    if(ASL_Lux_Calc >= BR_LUX_LEVEL_4)
    {
        aw21036_breath_one(LED_NUM_2,     led_flag, 0xFF,  br_val_level[key_level_flag]);
        aw21036_breath_one(LED_NUM_5,     led_flag, 0xFF,  br_val_level[key_level_flag]);
        aw21036_breath_one(LED_NUM_6,     led_flag, 0xFF,  br_val_level[key_level_flag]);
        aw21036_breath_one(LED_NUM_8,     led_flag, 0xFF,  br_val_level[key_level_flag]);
    }
    else
    {
        aw21036_breath_one(LED_NUM_2,     led_flag, LED_COL_2,  br_val_level[key_level_flag]);
        aw21036_breath_one(LED_NUM_5,     led_flag, LED_COL_5,  br_val_level[key_level_flag]);
        aw21036_breath_one(LED_NUM_6,     led_flag, LED_COL_6,  br_val_level[key_level_flag]);
        aw21036_breath_one(LED_NUM_8,     led_flag, LED_COL_8,  br_val_level[key_level_flag]);
    }

    aw21036_breath_one(LED_POWER,     led_flag, LED_COL_23 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_AMBILIGHT, led_flag, LED_COL_33 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_SOURCE,    led_flag, LED_COL_17 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_VOICE,     led_flag, LED_COL_20 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_MENU,      led_flag, LED_COL_22 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_BACK,      led_flag, LED_COL_18 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_HOME,      led_flag, LED_COL_21 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_VOL_L,     led_flag, LED_COL_25 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_VOL_R,     led_flag, LED_COL_27 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_123,       led_flag, LED_COL_16 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_CHN_L,     led_flag, LED_COL_29 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_CHN_R,     led_flag, LED_COL_31 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_OTT_1,     led_flag, LED_COL_19 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_OTT_2,     led_flag, LED_COL_26 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_OTT_3,     led_flag, LED_COL_24 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_OTT_4,     led_flag, LED_COL_30 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_OTT_5,     led_flag, LED_COL_28 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_OTT_6,     led_flag, LED_COL_32 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_NUM_1,     led_flag, LED_COL_1 + LED_COL_TEMP,  br_val_level[key_level_flag]);
    //aw21036_breath_one(LED_NUM_2,     led_flag, LED_COL_2 + LED_COL_TEMP,  br_val_level[key_level_flag]);
    aw21036_breath_one(LED_NUM_3,     led_flag, LED_COL_3 + LED_COL_TEMP,  br_val_level[key_level_flag]);
    aw21036_breath_one(LED_NUM_4,     led_flag, LED_COL_4 + LED_COL_TEMP,  br_val_level[key_level_flag]);
    //aw21036_breath_one(LED_NUM_5,     led_flag, LED_COL_5 + LED_COL_TEMP,  br_val_level[key_level_flag]);
    //aw21036_breath_one(LED_NUM_6,     led_flag, LED_COL_6 + LED_COL_TEMP,  br_val_level[key_level_flag]);
    aw21036_breath_one(LED_NUM_7,     led_flag, LED_COL_7 + LED_COL_TEMP,  br_val_level[key_level_flag]);
    //aw21036_breath_one(LED_NUM_8,     led_flag, LED_COL_8 + LED_COL_TEMP,  br_val_level[key_level_flag]);
    aw21036_breath_one(LED_NUM_9,     led_flag, LED_COL_9 + LED_COL_TEMP,  br_val_level[key_level_flag]);
    aw21036_breath_one(LED_NUM_0,     led_flag, LED_COL_10 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_REWIND,    led_flag, LED_COL_11 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_PAUSE,     led_flag, LED_COL_12 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_PLAY,      led_flag, LED_COL_13 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_FFWD,      led_flag, LED_COL_14 + LED_COL_TEMP, br_val_level[key_level_flag]);
#endif

    if(led_flag)
    {
        backlight_normal_Flag = true;
        backlight_hidden_Flag = true;
        //WaitMs(led_on_wait_ms);
    }
    else
    {
        backlight_normal_Flag = false;
        backlight_hidden_Flag = false;
        //WaitMs(led_off_wait_ms);
    }
    }
    else{

    if(led_flag)
        aw21036_all_off();
    u8 LED_COL_TEMP = 0x00;
    i2c_master_init(AW21036_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );

    if(ASL_Lux_Calc <= BR_LUX_LEVEL_0)
        key_level_flag = 19; 
    else if(ASL_Lux_Calc <= BR_LUX_LEVEL_1)
        key_level_flag = 15;         
    else if(ASL_Lux_Calc <= BR_LUX_LEVEL_2)
        key_level_flag = 10;         
    else if(ASL_Lux_Calc <= BR_LUX_LEVEL_3)
        key_level_flag = 5;         
	else if(ASL_Lux_Calc <= BR_LUX_LEVEL_4)
        key_level_flag = 0;         
    else if(ASL_Lux_Calc <= BR_LUX_LEVEL_5)
    {
        key_level_flag = 0; 
        LED_COL_TEMP = 0x33;//加至70%  
    }        
    else
    {
        key_level_flag = 0; 
        LED_COL_TEMP = 0x66;//加至90% 
    } 
    i2c_write_byte(REG_GCFG0,  1, 0xff);
	i2c_write_byte(REG_GCFG1,  1, 0x1f);

    if(ASL_Lux_Calc >= BR_LUX_LEVEL_4)
    {
        aw21036_breath_one(LED_NUM_2,     led_flag, 0xFF,  br_val_level[key_level_flag]);
        aw21036_breath_one(LED_NUM_5,     led_flag, 0xFF,  br_val_level[key_level_flag]);
        aw21036_breath_one(LED_NUM_6,     led_flag, 0xFF,  br_val_level[key_level_flag]);
        aw21036_breath_one(LED_NUM_8,     led_flag, 0xFF,  br_val_level[key_level_flag]);
    }
    else
    {
        aw21036_breath_one(LED_NUM_2,     led_flag, LED_COL_2,  br_val_level[key_level_flag]);
        aw21036_breath_one(LED_NUM_5,     led_flag, LED_COL_5,  br_val_level[key_level_flag]);
        aw21036_breath_one(LED_NUM_6,     led_flag, LED_COL_6,  br_val_level[key_level_flag]);
        aw21036_breath_one(LED_NUM_8,     led_flag, LED_COL_8,  br_val_level[key_level_flag]);
    }

    aw21036_breath_one(LED_AMBILIGHT, led_flag, LED_COL_33 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_VOL_L,     led_flag, LED_COL_25 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_VOL_R,     led_flag, LED_COL_27 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_CHN_L,     led_flag, LED_COL_29 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_CHN_R,     led_flag, LED_COL_31 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_NUM_1,     led_flag, LED_COL_1 + LED_COL_TEMP,  br_val_level[key_level_flag]);
    aw21036_breath_one(LED_NUM_3,     led_flag, LED_COL_3 + LED_COL_TEMP,  br_val_level[key_level_flag]);
    aw21036_breath_one(LED_NUM_4,     led_flag, LED_COL_4 + LED_COL_TEMP,  br_val_level[key_level_flag]);
    aw21036_breath_one(LED_NUM_7,     led_flag, LED_COL_7 + LED_COL_TEMP,  br_val_level[key_level_flag]);
    aw21036_breath_one(LED_NUM_9,     led_flag, LED_COL_9 + LED_COL_TEMP,  br_val_level[key_level_flag]);
    aw21036_breath_one(LED_NUM_0,     led_flag, LED_COL_10 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_REWIND,    led_flag, LED_COL_11 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_PAUSE,     led_flag, LED_COL_12 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_PLAY,      led_flag, LED_COL_13 + LED_COL_TEMP, br_val_level[key_level_flag]);
    aw21036_breath_one(LED_FFWD,      led_flag, LED_COL_14 + LED_COL_TEMP, br_val_level[key_level_flag]);

    if(led_flag)
    {
        backlight_normal_Flag = true;
        backlight_hidden_Flag = true;
    }
    else
    {
        backlight_normal_Flag = false;
        backlight_hidden_Flag = false;
    }

    }
    }
    else
        aw21036_backlight_HiddenKey(led_flag);
}

/******************************************************************
 * @brief  LED_Test_SW_Function
 * @param  none
 * @return none
 * @retval void
 */
void LED_Test_SW_Function(u8 key0)
{
#if 0
    if (key0 == KEY_POWER)
        pressed_power_key();
    if (key0 == KEY_AMBILIGHT)
        pressed_ambilight_key();
#if (test_key_count == 32)
    if (key0 == KEY_OTT_3)
        pressed_ott_3_key();
    if (key0 == KEY_OTT_4)
        pressed_ott_4_key();
#elif (test_key_count == 34)
    if (key0 == KEY_OTT_5)
        pressed_ott_3_key();
    if (key0 == KEY_OTT_6)
        pressed_ott_4_key();
#endif
#endif

    if(key0 == KEY_123)     //循环切换按键背光
        LED_Test_SW_Toggle_ONE();
    if (key0 == KEY_GREEN)  //背光降低一步
        LED_Test_SW_Increase_Decrease_BR(0);
    if (key0 == KEY_YELLOW) //背光增加一步
        LED_Test_SW_Increase_Decrease_BR(1);
    if (key0 == KEY_RED)    //背光调至最暗
    {
        key_level_flag = 19;
        LED_Test_SW_Increase_Decrease_BR(0);
    }
    if (key0 == KEY_BLUE)   //背光调至最亮
    {
        key_level_flag = 0;
        LED_Test_SW_Increase_Decrease_BR(1);        
    }
}

/******************************************************************
 * @brief  LED_Test_SW_Change
 * @param  none
 * @return none
 * @retval void
 */
void LED_Test_SW_Change(void)
{
#if (TEST_SW_LED)
    app_backlight_normal_timer = 0;
    app_backlight_hidden_timer = 0;
    app_backlight_all_timer = 0;
    tick_backlight_normal = 0;
    tick_backlight_hidden = 0;
    tick_backlight_all = 0;
    backlight_task = 0;

    aw21036_all_on(LED_BR_ALL, LED_COL_ALL);
    WaitMs(250);
    aw21036_all_off();
    WaitMs(250);
    aw21036_all_on(LED_BR_ALL, LED_COL_ALL);
    WaitMs(250);
    aw21036_all_off();
    WaitMs(250);
    aw21036_all_on(LED_BR_ALL, LED_COL_ALL);
    WaitMs(250);
    aw21036_all_off();
    led_test_sw_flag = !led_test_sw_flag;
    analog_write(DEEP_ANA_REG7, led_test_sw_flag);
    if(led_test_sw_flag == false)
    {
        backlight_normal_Flag = false;
        backlight_hidden_Flag = false;
    }
    else
        test_toggle_one_cnt = 0;

    led_driver_init();
    device_led_off(APP_LED_WHITE);

    aw21036_init();   
#endif
}

static bool test_toggle_all_flag = false;
/******************************************************************
 * @brief  LED_Test_SW_Toggle_ALL
 * @param  none
 * @return none
 * @retval void
 */
void LED_Test_SW_Toggle_ALL(void)
{
    test_toggle_all_flag = !test_toggle_all_flag;
    if(test_toggle_all_flag)
    {
        aw21036_all_on(LED_BR_ALL, LED_COL_ALL);
        memset(led_on_buf, 0, AW21036_LED_NUM);
        for(int i = 0; i < AW21036_LED_NUM; i++)
            led_on_buf[i] = 1;

        key_level_flag = 0;
    }
    else
    {
        aw21036_all_off();     
        memset(led_on_buf, 0, AW21036_LED_NUM);    
        WaitMs(1000);
        pressed_ambilight_key_release();
    }
}

/******************************************************************
 * @brief  LED_Test_SW_Toggle_ONE
 * @param  none
 * @return none
 * @retval void
 */
void LED_Test_SW_Toggle_ONE(void)
{
    aw21036_all_off(); 
    gpio_write(GPIO_LED,1);

    key_level_flag = 0;

    test_toggle_all_flag = false;
    if(test_toggle_one_cnt == 0)
    {
        gpio_set_func(GPIO_LED, AS_GPIO);
        gpio_set_output_en(GPIO_LED, 1);
        gpio_set_func(GPIO_PD7,  AS_GPIO);
        gpio_set_output_en(GPIO_PD7, 1);
        gpio_write(GPIO_PD7,0);
        gpio_write(GPIO_LED,0);
    }
    else if(test_toggle_one_cnt == 1){
        aw21036_onoff_one(LED_POWER,     AW_LED_ON, LED_COL_1,  LED_BR_1);
    }else if(test_toggle_one_cnt == 2){
        aw21036_onoff_one(LED_AMBILIGHT, AW_LED_ON, LED_COL_2,  LED_BR_2);
    }else if(test_toggle_one_cnt == 3){
        aw21036_onoff_one(LED_SOURCE,    AW_LED_ON, LED_COL_4,  LED_BR_4);
    }else if(test_toggle_one_cnt == 4){
        aw21036_onoff_one(LED_VOICE,     AW_LED_ON, LED_COL_5,  LED_BR_5);
    }else if(test_toggle_one_cnt == 5){
        aw21036_onoff_one(LED_MENU,      AW_LED_ON, LED_COL_6,  LED_BR_6);
    }else if(test_toggle_one_cnt == 6){
        aw21036_onoff_one(LED_NUM_1,     AW_LED_ON, LED_COL_7,  LED_BR_7);
    }else if(test_toggle_one_cnt == 7){
        aw21036_onoff_one(LED_NUM_2,     AW_LED_ON, LED_COL_8,  LED_BR_8);
    }else if(test_toggle_one_cnt == 8){
        aw21036_onoff_one(LED_NUM_3,     AW_LED_ON, LED_COL_9,  LED_BR_9);
    }else if(test_toggle_one_cnt == 9){
        aw21036_onoff_one(LED_NUM_4,     AW_LED_ON, LED_COL_10, LED_BR_10);
    }else if(test_toggle_one_cnt == 10){
        aw21036_onoff_one(LED_NUM_5,     AW_LED_ON, LED_COL_11, LED_BR_11);
    }else if(test_toggle_one_cnt == 11){
        aw21036_onoff_one(LED_NUM_6,     AW_LED_ON, LED_COL_12, LED_BR_12);
    }else if(test_toggle_one_cnt == 12){
        aw21036_onoff_one(LED_NUM_7,     AW_LED_ON, LED_COL_13, LED_BR_13);
    }else if(test_toggle_one_cnt == 13){
        aw21036_onoff_one(LED_NUM_8,     AW_LED_ON, LED_COL_14, LED_BR_14);
    }else if(test_toggle_one_cnt == 14){
        aw21036_onoff_one(LED_NUM_9,     AW_LED_ON, LED_COL_15, LED_BR_15);
    }else if(test_toggle_one_cnt == 15){
        aw21036_onoff_one(LED_BACK,      AW_LED_ON, LED_COL_16, LED_BR_16);
    }else if(test_toggle_one_cnt == 16){
        aw21036_onoff_one(LED_NUM_0,     AW_LED_ON, LED_COL_17, LED_BR_17);
    }else if(test_toggle_one_cnt == 17){
        aw21036_onoff_one(LED_HOME,      AW_LED_ON, LED_COL_18, LED_BR_18);
    }else if(test_toggle_one_cnt == 18){
        aw21036_onoff_one(LED_VOL_L,     AW_LED_ON, LED_COL_30, LED_BR_30);
        aw21036_onoff_one(LED_VOL_R,     AW_LED_ON, LED_COL_31, LED_BR_31);
    }else if(test_toggle_one_cnt == 19){
        aw21036_onoff_one(LED_123,       AW_LED_ON, LED_COL_19, LED_BR_19);
    }else if(test_toggle_one_cnt == 20){
        aw21036_onoff_one(LED_CHN_L,     AW_LED_ON, LED_COL_32, LED_BR_32);
        aw21036_onoff_one(LED_CHN_R,     AW_LED_ON, LED_COL_33, LED_BR_33);
    }else if(test_toggle_one_cnt == 21){
        aw21036_onoff_one(LED_REWIND,    AW_LED_ON, LED_COL_20, LED_BR_20);
    }else if(test_toggle_one_cnt == 22){
        aw21036_onoff_one(LED_PAUSE,     AW_LED_ON, LED_COL_21, LED_BR_21);
    }else if(test_toggle_one_cnt == 23){
        aw21036_onoff_one(LED_PLAY,      AW_LED_ON, LED_COL_22, LED_BR_22);
    }else if(test_toggle_one_cnt == 24){
        aw21036_onoff_one(LED_FFWD,      AW_LED_ON, LED_COL_23, LED_BR_23);
    }else if(test_toggle_one_cnt == 25){
        aw21036_onoff_one(LED_OTT_1,     AW_LED_ON, LED_COL_24, LED_BR_24);
    }else if(test_toggle_one_cnt == 26){
        aw21036_onoff_one(LED_OTT_2,     AW_LED_ON, LED_COL_25, LED_BR_25);
    }else if(test_toggle_one_cnt == 27){
        aw21036_onoff_one(LED_OTT_3,     AW_LED_ON, LED_COL_26, LED_BR_26);
    }else if(test_toggle_one_cnt == 28){
        aw21036_onoff_one(LED_OTT_4,     AW_LED_ON, LED_COL_27, LED_BR_27);
    }
#if (test_key_count == 34)
    else if(test_toggle_one_cnt == 29){
        aw21036_onoff_one(LED_OTT_5,     AW_LED_ON, LED_COL_28, LED_BR_28);
    }else if(test_toggle_one_cnt == 30){
        aw21036_onoff_one(LED_OTT_6,     AW_LED_ON, LED_COL_29, LED_BR_29);
    }
#endif
    if(test_toggle_one_cnt != 0)
    {
        memset(led_on_buf, 0, AW21036_LED_NUM);
        led_on_buf[test_toggle_one_cnt] = 1;        
    }
#if (test_key_count == 32)
    if(test_toggle_one_cnt == 28)
        test_toggle_one_cnt = 0;
    else
        test_toggle_one_cnt ++;
#elif (test_key_count == 34)
    if(test_toggle_one_cnt == 30)
        test_toggle_one_cnt = 0;
    else
        test_toggle_one_cnt ++;
#endif
}

/******************************************************************
 * @brief  LED_Test_SW_Increase_Decrease_BR
 * @param  BR_Flag
 * @return none
 * @retval void
 */
void LED_Test_SW_Increase_Decrease_BR(u8 BR_Flag)
{
    if(BR_Flag == 1)//Increase
    {
        if(key_level_flag < 20 && key_level_flag > 0)
            key_level_flag --;
    }else//Decrease
    {
        if(key_level_flag < 19 && key_level_flag >= 0)
            key_level_flag ++;
    }

    if(led_on_buf[1] == 1){
        aw21036_onoff_one(LED_POWER,     AW_LED_ON, LED_COL_1,  br_val_level[key_level_flag]);
    }if(led_on_buf[2] == 1){
        aw21036_onoff_one(LED_AMBILIGHT, AW_LED_ON, LED_COL_2,  br_val_level[key_level_flag]);
    }if(led_on_buf[3] == 1){
        aw21036_onoff_one(LED_SOURCE,    AW_LED_ON, LED_COL_4,  br_val_level[key_level_flag]);
    }if(led_on_buf[4] == 1){
        aw21036_onoff_one(LED_VOICE,     AW_LED_ON, LED_COL_5,  br_val_level[key_level_flag]);
    }if(led_on_buf[5] == 1){
        aw21036_onoff_one(LED_MENU,      AW_LED_ON, LED_COL_6,  br_val_level[key_level_flag]);
    }if(led_on_buf[6] == 1){
        aw21036_onoff_one(LED_NUM_1,     AW_LED_ON, LED_COL_7,  br_val_level[key_level_flag]);
    }if(led_on_buf[7] == 1){
        aw21036_onoff_one(LED_NUM_2,     AW_LED_ON, LED_COL_8,  br_val_level[key_level_flag]);
    }if(led_on_buf[8] == 1){
        aw21036_onoff_one(LED_NUM_3,     AW_LED_ON, LED_COL_9,  br_val_level[key_level_flag]);
    }if(led_on_buf[9] == 1){
        aw21036_onoff_one(LED_NUM_4,     AW_LED_ON, LED_COL_10, br_val_level[key_level_flag]);
    }if(led_on_buf[10] == 1){
        aw21036_onoff_one(LED_NUM_5,     AW_LED_ON, LED_COL_11, br_val_level[key_level_flag]);
    }if(led_on_buf[11] == 1){
        aw21036_onoff_one(LED_NUM_6,     AW_LED_ON, LED_COL_12, br_val_level[key_level_flag]);
    }if(led_on_buf[12] == 1){
        aw21036_onoff_one(LED_NUM_7,     AW_LED_ON, LED_COL_13, br_val_level[key_level_flag]);
    }if(led_on_buf[13] == 1){
        aw21036_onoff_one(LED_NUM_8,     AW_LED_ON, LED_COL_14, br_val_level[key_level_flag]);
    }if(led_on_buf[14] == 1){
        aw21036_onoff_one(LED_NUM_9,     AW_LED_ON, LED_COL_15, br_val_level[key_level_flag]);
    }if(led_on_buf[15] == 1){
        aw21036_onoff_one(LED_BACK,      AW_LED_ON, LED_COL_16, br_val_level[key_level_flag]);
    }if(led_on_buf[16] == 1){
        aw21036_onoff_one(LED_NUM_0,     AW_LED_ON, LED_COL_17, br_val_level[key_level_flag]);
    }if(led_on_buf[17] == 1){
        aw21036_onoff_one(LED_HOME,      AW_LED_ON, LED_COL_18, br_val_level[key_level_flag]);
    }if(led_on_buf[18] == 1){
        aw21036_onoff_one(LED_VOL_L,     AW_LED_ON, LED_COL_30, br_val_level[key_level_flag]);
        aw21036_onoff_one(LED_VOL_R,     AW_LED_ON, LED_COL_31, br_val_level[key_level_flag]);
    }if(led_on_buf[19] == 1){
        aw21036_onoff_one(LED_123,       AW_LED_ON, LED_COL_19, br_val_level[key_level_flag]);
    }if(led_on_buf[20] == 1){
        aw21036_onoff_one(LED_CHN_L,     AW_LED_ON, LED_COL_32, br_val_level[key_level_flag]);
        aw21036_onoff_one(LED_CHN_R,     AW_LED_ON, LED_COL_33, br_val_level[key_level_flag]);
    }if(led_on_buf[21] == 1){
        aw21036_onoff_one(LED_REWIND,    AW_LED_ON, LED_COL_20, br_val_level[key_level_flag]);
    }if(led_on_buf[22] == 1){
        aw21036_onoff_one(LED_PAUSE,     AW_LED_ON, LED_COL_21, br_val_level[key_level_flag]);
    }if(led_on_buf[23] == 1){
        aw21036_onoff_one(LED_PLAY,      AW_LED_ON, LED_COL_22, br_val_level[key_level_flag]);
    }if(led_on_buf[24] == 1){
        aw21036_onoff_one(LED_FFWD,      AW_LED_ON, LED_COL_23, br_val_level[key_level_flag]);
    }if(led_on_buf[25] == 1){
        aw21036_onoff_one(LED_OTT_1,     AW_LED_ON, LED_COL_24, br_val_level[key_level_flag]);
    }if(led_on_buf[26] == 1){
        aw21036_onoff_one(LED_OTT_2,     AW_LED_ON, LED_COL_25, br_val_level[key_level_flag]);
    }if(led_on_buf[27] == 1){
        aw21036_onoff_one(LED_OTT_3,     AW_LED_ON, LED_COL_26, br_val_level[key_level_flag]);
    }if(led_on_buf[28] == 1){
        aw21036_onoff_one(LED_OTT_4,     AW_LED_ON, LED_COL_27, br_val_level[key_level_flag]);
    }
#if (test_key_count == 34)
    if(led_on_buf[29] == 1){
        aw21036_onoff_one(LED_OTT_5,     AW_LED_ON, LED_COL_28, br_val_level[key_level_flag]);
    }if(led_on_buf[30] == 1){
        aw21036_onoff_one(LED_OTT_6,     AW_LED_ON, LED_COL_29, br_val_level[key_level_flag]);
    }
#endif
}

/******************************************************************
 * @brief  LED_Test_SW_Duty
 * @param  BR_Flag
 * @return none
 * @retval void
 */
void LED_Test_SW_Duty(u8 duty_level)
{
    u8 led_flag = 1;
    u8 LED_COL_TEMP = 0x00;
    u8 br_duty_level[] = {
      //0   1   2   3    4   5   6   7   8  9  
        0,  3,  5,  7,  10, 13, 15, 18, 20, 23,
        25, 28, 31, 33, 36, 38, 41, 43, 46, 48,
        51, 53, 56, 59, 61, 64, 66, 69, 71, 74,
        76, 79, 82, 84, 87, 89, 92, 94, 97, 99,
        102,104,107,110,112,115,117,120,122,125,
        127,130,133,135,138,140,143,145,148,150,
        153,155,158,161,163,166,168,171,173,176,
        178,181,184,186,189,191,194,196,199,201,
        204,206,209,212,214,217,219,222,224,227,
        229,232,235,237,240,242,245,247,250,252,
        255
    };
    i2c_master_init(AW21036_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );

    backlight_task = 1;
    app_backlight_ic_close_timer = 0; 
    app_backlight_all_timer = 0;
    app_backlight_hidden_timer = 0;
    app_backlight_normal_timer = 0;

    if(duty_level == 0)
        led_flag = 0;
    else
        led_flag = 1;
     
    if(duty_level >= 70)
        LED_COL_TEMP = 0x33;//加至70%          
    if(duty_level >= 90)
        LED_COL_TEMP = 0x66;//加至90% 

    i2c_write_byte(REG_GCFG0,  1, 0xff);
	i2c_write_byte(REG_GCFG1,  1, 0x1f);

    if(duty_level >= 70)
    {
        aw21036_breath_one(LED_NUM_2,     led_flag, 0xFF,  br_duty_level[duty_level]);
        aw21036_breath_one(LED_NUM_5,     led_flag, 0xFF,  br_duty_level[duty_level]);
        aw21036_breath_one(LED_NUM_6,     led_flag, 0xFF,  br_duty_level[duty_level]);
        aw21036_breath_one(LED_NUM_8,     led_flag, 0xFF,  br_duty_level[duty_level]);
    }
    else
    {
        aw21036_breath_one(LED_NUM_2,     led_flag, LED_COL_2,  br_duty_level[duty_level]);
        aw21036_breath_one(LED_NUM_5,     led_flag, LED_COL_5,  br_duty_level[duty_level]);
        aw21036_breath_one(LED_NUM_6,     led_flag, LED_COL_6,  br_duty_level[duty_level]);
        aw21036_breath_one(LED_NUM_8,     led_flag, LED_COL_8,  br_duty_level[duty_level]);
    }
    aw21036_breath_one(LED_POWER,     led_flag, LED_COL_23 + LED_COL_TEMP, br_duty_level[duty_level]);
    aw21036_breath_one(LED_AMBILIGHT, led_flag, LED_COL_33 + LED_COL_TEMP, br_duty_level[duty_level]);
    aw21036_breath_one(LED_SOURCE,    led_flag, LED_COL_17 + LED_COL_TEMP, br_duty_level[duty_level]);
    aw21036_breath_one(LED_VOICE,     led_flag, LED_COL_20 + LED_COL_TEMP, br_duty_level[duty_level]);
    aw21036_breath_one(LED_MENU,      led_flag, LED_COL_22 + LED_COL_TEMP, br_duty_level[duty_level]);
    aw21036_breath_one(LED_BACK,      led_flag, LED_COL_18 + LED_COL_TEMP, br_duty_level[duty_level]);
    aw21036_breath_one(LED_HOME,      led_flag, LED_COL_21 + LED_COL_TEMP, br_duty_level[duty_level]);
    aw21036_breath_one(LED_VOL_L,     led_flag, LED_COL_25 + LED_COL_TEMP, br_duty_level[duty_level]);
    aw21036_breath_one(LED_VOL_R,     led_flag, LED_COL_27 + LED_COL_TEMP, br_duty_level[duty_level]);
    aw21036_breath_one(LED_123,       led_flag, LED_COL_16 + LED_COL_TEMP, br_duty_level[duty_level]);
    aw21036_breath_one(LED_CHN_L,     led_flag, LED_COL_29 + LED_COL_TEMP, br_duty_level[duty_level]);
    aw21036_breath_one(LED_CHN_R,     led_flag, LED_COL_31 + LED_COL_TEMP, br_duty_level[duty_level]);
    aw21036_breath_one(LED_OTT_1,     led_flag, LED_COL_19 + LED_COL_TEMP, br_duty_level[duty_level]);
    aw21036_breath_one(LED_OTT_2,     led_flag, LED_COL_26 + LED_COL_TEMP, br_duty_level[duty_level]);
    aw21036_breath_one(LED_OTT_3,     led_flag, LED_COL_24 + LED_COL_TEMP, br_duty_level[duty_level]);
    aw21036_breath_one(LED_OTT_4,     led_flag, LED_COL_30 + LED_COL_TEMP, br_duty_level[duty_level]);
    aw21036_breath_one(LED_OTT_5,     led_flag, LED_COL_28 + LED_COL_TEMP, br_duty_level[duty_level]);
    aw21036_breath_one(LED_OTT_6,     led_flag, LED_COL_32 + LED_COL_TEMP, br_duty_level[duty_level]);
    aw21036_breath_one(LED_NUM_1,     led_flag, LED_COL_1 + LED_COL_TEMP,  br_duty_level[duty_level]);
    //aw21036_breath_one(LED_NUM_2,     led_flag, LED_COL_2 + LED_COL_TEMP,  br_duty_level[duty_level]);
    aw21036_breath_one(LED_NUM_3,     led_flag, LED_COL_3 + LED_COL_TEMP,  br_duty_level[duty_level]);
    aw21036_breath_one(LED_NUM_4,     led_flag, LED_COL_4 + LED_COL_TEMP,  br_duty_level[duty_level]);
    //aw21036_breath_one(LED_NUM_5,     led_flag, LED_COL_5 + LED_COL_TEMP,  br_duty_level[duty_level]);
    //aw21036_breath_one(LED_NUM_6,     led_flag, LED_COL_6 + LED_COL_TEMP,  br_duty_level[duty_level]);
    aw21036_breath_one(LED_NUM_7,     led_flag, LED_COL_7 + LED_COL_TEMP,  br_duty_level[duty_level]);
    //aw21036_breath_one(LED_NUM_8,     led_flag, LED_COL_8 + LED_COL_TEMP,  br_duty_level[duty_level]);
    aw21036_breath_one(LED_NUM_9,     led_flag, LED_COL_9 + LED_COL_TEMP,  br_duty_level[duty_level]);
    aw21036_breath_one(LED_NUM_0,     led_flag, LED_COL_10 + LED_COL_TEMP, br_duty_level[duty_level]);
    aw21036_breath_one(LED_REWIND,    led_flag, LED_COL_11 + LED_COL_TEMP, br_duty_level[duty_level]);
    aw21036_breath_one(LED_PAUSE,     led_flag, LED_COL_12 + LED_COL_TEMP, br_duty_level[duty_level]);
    aw21036_breath_one(LED_PLAY,      led_flag, LED_COL_13 + LED_COL_TEMP, br_duty_level[duty_level]);
    aw21036_breath_one(LED_FFWD,      led_flag, LED_COL_14 + LED_COL_TEMP, br_duty_level[duty_level]);

    if(led_flag)
    {
        backlight_normal_Flag = true;
        backlight_hidden_Flag = true;
        //WaitMs(led_on_wait_ms);
    }
    else
    {
        backlight_normal_Flag = false;
        backlight_hidden_Flag = false;
        //WaitMs(led_off_wait_ms);
    }
}

/******************************************************************
 * @brief  LED_Test_SW_Duty_EachLed
 * @param  BR_Flag
 * @return none
 * @retval void
 */
void LED_Test_SW_Duty_EachLed(u8 duty_level, u8 each_led)
{
    u8 led_flag = 1;
    u8 LED_COL_TEMP = 0x00;
    u8 br_duty_level[] = {
      //0   1   2   3    4   5   6   7   8  9  
        0,  3,  5,  7,  10, 13, 15, 18, 20, 23,
        25, 28, 31, 33, 36, 38, 41, 43, 46, 48,
        51, 53, 56, 59, 61, 64, 66, 69, 71, 74,
        76, 79, 82, 84, 87, 89, 92, 94, 97, 99,
        102,104,107,110,112,115,117,120,122,125,
        127,130,133,135,138,140,143,145,148,150,
        153,155,158,161,163,166,168,171,173,176,
        178,181,184,186,189,191,194,196,199,201,
        204,206,209,212,214,217,219,222,224,227,
        229,232,235,237,240,242,245,247,250,252,
        255
    };
    i2c_master_init(AW21036_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );

    backlight_task = 1;
    app_backlight_ic_close_timer = 0; 
    app_backlight_all_timer = 0;
    app_backlight_hidden_timer = 0;
    app_backlight_normal_timer = 0;

    if(duty_level == 0)
        led_flag = 0;
    else
        led_flag = 1;
     
    if(duty_level >= 70)
        LED_COL_TEMP = 0x33;//加至70%          
    if(duty_level >= 90)
        LED_COL_TEMP = 0x66;//加至90% 

    led_driver_init();
    gpio_write(GPIO_LED, 1);

    switch (each_led)
    {
    case 0x02:
        if(led_flag == 0)
            gpio_write(GPIO_LED, 1);
        else
            gpio_write(GPIO_LED, 0);
        break;
    case 0x03:
        aw21036_onoff_one(LED_POWER,     led_flag, LED_COL_23 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;            
    case 0x04:
        aw21036_onoff_one(LED_AMBILIGHT, led_flag, LED_COL_33 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;   
    case 0x05:
        aw21036_onoff_one(LED_SOURCE,    led_flag, LED_COL_17 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;  
    case 0x06:
        aw21036_onoff_one(LED_VOICE,     led_flag, LED_COL_20 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;  
    case 0x07:
        aw21036_onoff_one(LED_MENU,      led_flag, LED_COL_22 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;  
    case 0x08:
        aw21036_onoff_one(LED_NUM_1,     led_flag, LED_COL_1 + LED_COL_TEMP,  br_duty_level[duty_level]);
        break;  
    case 0x09:
        if(duty_level >= 70)
            aw21036_onoff_one(LED_NUM_2,     led_flag, 0xFF,        br_duty_level[duty_level]);
        else
            aw21036_onoff_one(LED_NUM_2,     led_flag, LED_COL_2,   br_duty_level[duty_level]);
        break;  
    case 0x0A:
        aw21036_onoff_one(LED_NUM_3,     led_flag, LED_COL_3 + LED_COL_TEMP,  br_duty_level[duty_level]);
        break;  
    case 0x0B:
        aw21036_onoff_one(LED_NUM_4,     led_flag, LED_COL_4 + LED_COL_TEMP,  br_duty_level[duty_level]);
        break;  
    case 0x0C:
        if(duty_level >= 70)
            aw21036_onoff_one(LED_NUM_5,     led_flag, 0xFF,        br_duty_level[duty_level]);
        else
            aw21036_onoff_one(LED_NUM_5,     led_flag, LED_COL_5,   br_duty_level[duty_level]);
        break;  
    case 0x0D:
        if(duty_level >= 70)
            aw21036_onoff_one(LED_NUM_6,     led_flag, 0xFF,        br_duty_level[duty_level]);
        else
            aw21036_onoff_one(LED_NUM_6,     led_flag, LED_COL_6,   br_duty_level[duty_level]);
        break;  
    case 0x0E:
        aw21036_onoff_one(LED_NUM_7,     led_flag, LED_COL_7 + LED_COL_TEMP,  br_duty_level[duty_level]);
        break;  
    case 0x0F:
        if(duty_level >= 70)
            aw21036_onoff_one(LED_NUM_8,     led_flag, 0xFF,        br_duty_level[duty_level]);
        else
            aw21036_onoff_one(LED_NUM_8,     led_flag, LED_COL_8,   br_duty_level[duty_level]);
        break;   
    case 0x10:
        aw21036_onoff_one(LED_NUM_9,     led_flag, LED_COL_9 + LED_COL_TEMP,  br_duty_level[duty_level]);
        break;  
    case 0x11:
        aw21036_onoff_one(LED_BACK,      led_flag, LED_COL_18 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;  
    case 0x12:
        aw21036_onoff_one(LED_NUM_0,     led_flag, LED_COL_10 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;  
    case 0x13:
        aw21036_onoff_one(LED_HOME,      led_flag, LED_COL_21 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;  
    case 0x14:
        aw21036_onoff_one(LED_VOL_L,     led_flag, LED_COL_25 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_VOL_R,     led_flag, LED_COL_27 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;  
    case 0x15:
        aw21036_onoff_one(LED_123,       led_flag, LED_COL_16 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;  
    case 0x16:
        aw21036_onoff_one(LED_CHN_L,     led_flag, LED_COL_29 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_CHN_R,     led_flag, LED_COL_31 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;  
    case 0x17:
        aw21036_onoff_one(LED_REWIND,    led_flag, LED_COL_11 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;  
    case 0x18:
        aw21036_onoff_one(LED_PAUSE,     led_flag, LED_COL_12 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;  
    case 0x19:
        aw21036_onoff_one(LED_PLAY,      led_flag, LED_COL_13 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;  
    case 0x1A:
        aw21036_onoff_one(LED_FFWD,      led_flag, LED_COL_14 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;  
    case 0x1B:
        aw21036_onoff_one(LED_OTT_1,     led_flag, LED_COL_19 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;  
    case 0x1C:
        aw21036_onoff_one(LED_OTT_2,     led_flag, LED_COL_26 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;  
    case 0x1D:
        aw21036_onoff_one(LED_OTT_3,     led_flag, LED_COL_24 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;  
    case 0x1E:
        aw21036_onoff_one(LED_OTT_4,     led_flag, LED_COL_30 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;  
    case 0x1F:
        aw21036_onoff_one(LED_OTT_5,     led_flag, LED_COL_28 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;  
    case 0x20:
        aw21036_onoff_one(LED_OTT_6,     led_flag, LED_COL_32 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;

    default:
        break;
    }
}

/******************************************************************
 * @brief  LED_Test_SW_Duty_GroupLed
 * @param  BR_Flag
 * @return none
 * @retval void
 */
void LED_Test_SW_Duty_GroupLed(u8 duty_level, u8 group_led)
{
    u8 led_flag = 1;
    u8 LED_COL_TEMP = 0x00;
    u8 br_duty_level[] = {
      //0   1   2   3    4   5   6   7   8  9  
        0,  3,  5,  7,  10, 13, 15, 18, 20, 23,
        25, 28, 31, 33, 36, 38, 41, 43, 46, 48,
        51, 53, 56, 59, 61, 64, 66, 69, 71, 74,
        76, 79, 82, 84, 87, 89, 92, 94, 97, 99,
        102,104,107,110,112,115,117,120,122,125,
        127,130,133,135,138,140,143,145,148,150,
        153,155,158,161,163,166,168,171,173,176,
        178,181,184,186,189,191,194,196,199,201,
        204,206,209,212,214,217,219,222,224,227,
        229,232,235,237,240,242,245,247,250,252,
        255
    };
    i2c_master_init(AW21036_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );

    backlight_task = 1;
    app_backlight_ic_close_timer = 0; 
    app_backlight_all_timer = 0;
    app_backlight_hidden_timer = 0;
    app_backlight_normal_timer = 0;

    if(duty_level == 0)
        led_flag = 0;
    else
        led_flag = 1;
     
    if(duty_level >= 70)
        LED_COL_TEMP = 0x33;//加至70%          
    if(duty_level >= 90)
        LED_COL_TEMP = 0x66;//加至90% 

    led_driver_init();
    gpio_write(GPIO_LED, 1);

    switch (group_led)
    {
    case 0x01://所有按键 All Key LED / Mic LED
        if(led_flag == 0)
            gpio_write(GPIO_LED, 1);
        else
            gpio_write(GPIO_LED, 0);
        if(duty_level >= 70)
        {
            aw21036_onoff_one(LED_NUM_2,     led_flag, 0xFF,  br_duty_level[duty_level]);
            aw21036_onoff_one(LED_NUM_5,     led_flag, 0xFF,  br_duty_level[duty_level]);
            aw21036_onoff_one(LED_NUM_6,     led_flag, 0xFF,  br_duty_level[duty_level]);
            aw21036_onoff_one(LED_NUM_8,     led_flag, 0xFF,  br_duty_level[duty_level]);
        }
        else
        {
            aw21036_onoff_one(LED_NUM_2,     led_flag, LED_COL_2,  br_duty_level[duty_level]);
            aw21036_onoff_one(LED_NUM_5,     led_flag, LED_COL_5,  br_duty_level[duty_level]);
            aw21036_onoff_one(LED_NUM_6,     led_flag, LED_COL_6,  br_duty_level[duty_level]);
            aw21036_onoff_one(LED_NUM_8,     led_flag, LED_COL_8,  br_duty_level[duty_level]);
        }
        aw21036_onoff_one(LED_POWER,     led_flag, LED_COL_23 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_AMBILIGHT, led_flag, LED_COL_33 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_SOURCE,    led_flag, LED_COL_17 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_VOICE,     led_flag, LED_COL_20 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_MENU,      led_flag, LED_COL_22 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_BACK,      led_flag, LED_COL_18 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_HOME,      led_flag, LED_COL_21 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_VOL_L,     led_flag, LED_COL_25 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_VOL_R,     led_flag, LED_COL_27 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_123,       led_flag, LED_COL_16 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_CHN_L,     led_flag, LED_COL_29 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_CHN_R,     led_flag, LED_COL_31 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_OTT_1,     led_flag, LED_COL_19 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_OTT_2,     led_flag, LED_COL_26 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_OTT_3,     led_flag, LED_COL_24 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_OTT_4,     led_flag, LED_COL_30 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_OTT_5,     led_flag, LED_COL_28 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_OTT_6,     led_flag, LED_COL_32 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_NUM_1,     led_flag, LED_COL_1 + LED_COL_TEMP,  br_duty_level[duty_level]);
        //aw21036_onoff_one(LED_NUM_2,     led_flag, LED_COL_2 + LED_COL_TEMP,  br_duty_level[duty_level]);
        aw21036_onoff_one(LED_NUM_3,     led_flag, LED_COL_3 + LED_COL_TEMP,  br_duty_level[duty_level]);
        aw21036_onoff_one(LED_NUM_4,     led_flag, LED_COL_4 + LED_COL_TEMP,  br_duty_level[duty_level]);
        //aw21036_onoff_one(LED_NUM_5,     led_flag, LED_COL_5 + LED_COL_TEMP,  br_duty_level[duty_level]);
        //aw21036_onoff_one(LED_NUM_6,     led_flag, LED_COL_6 + LED_COL_TEMP,  br_duty_level[duty_level]);
        aw21036_onoff_one(LED_NUM_7,     led_flag, LED_COL_7 + LED_COL_TEMP,  br_duty_level[duty_level]);
        //aw21036_onoff_one(LED_NUM_8,     led_flag, LED_COL_8 + LED_COL_TEMP,  br_duty_level[duty_level]);
        aw21036_onoff_one(LED_NUM_9,     led_flag, LED_COL_9 + LED_COL_TEMP,  br_duty_level[duty_level]);
        aw21036_onoff_one(LED_NUM_0,     led_flag, LED_COL_10 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_REWIND,    led_flag, LED_COL_11 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_PAUSE,     led_flag, LED_COL_12 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_PLAY,      led_flag, LED_COL_13 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_FFWD,      led_flag, LED_COL_14 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;
    case 0x02://隐藏按键 Hidden Keys
        if(duty_level >= 70)
        {
            aw21036_onoff_one(LED_NUM_2,     led_flag, 0xFF,  br_duty_level[duty_level]);
            aw21036_onoff_one(LED_NUM_5,     led_flag, 0xFF,  br_duty_level[duty_level]);
            aw21036_onoff_one(LED_NUM_6,     led_flag, 0xFF,  br_duty_level[duty_level]);
            aw21036_onoff_one(LED_NUM_8,     led_flag, 0xFF,  br_duty_level[duty_level]);
        }
        else
        {
            aw21036_onoff_one(LED_NUM_2,     led_flag, LED_COL_2,  br_duty_level[duty_level]);
            aw21036_onoff_one(LED_NUM_5,     led_flag, LED_COL_5,  br_duty_level[duty_level]);
            aw21036_onoff_one(LED_NUM_6,     led_flag, LED_COL_6,  br_duty_level[duty_level]);
            aw21036_onoff_one(LED_NUM_8,     led_flag, LED_COL_8,  br_duty_level[duty_level]);
        }
        aw21036_onoff_one(LED_NUM_1,     led_flag, LED_COL_1 + LED_COL_TEMP,  br_duty_level[duty_level]);
        //aw21036_onoff_one(LED_NUM_2,     led_flag, LED_COL_2 + LED_COL_TEMP,  br_duty_level[duty_level]);
        aw21036_onoff_one(LED_NUM_3,     led_flag, LED_COL_3 + LED_COL_TEMP,  br_duty_level[duty_level]);
        aw21036_onoff_one(LED_NUM_4,     led_flag, LED_COL_4 + LED_COL_TEMP,  br_duty_level[duty_level]);
        //aw21036_onoff_one(LED_NUM_5,     led_flag, LED_COL_5 + LED_COL_TEMP,  br_duty_level[duty_level]);
        //aw21036_onoff_one(LED_NUM_6,     led_flag, LED_COL_6 + LED_COL_TEMP,  br_duty_level[duty_level]);
        aw21036_onoff_one(LED_NUM_7,     led_flag, LED_COL_7 + LED_COL_TEMP,  br_duty_level[duty_level]);
        //aw21036_onoff_one(LED_NUM_8,     led_flag, LED_COL_8 + LED_COL_TEMP,  br_duty_level[duty_level]);
        aw21036_onoff_one(LED_NUM_9,     led_flag, LED_COL_9 + LED_COL_TEMP,  br_duty_level[duty_level]);
        aw21036_onoff_one(LED_NUM_0,     led_flag, LED_COL_10 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_REWIND,    led_flag, LED_COL_11 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_PAUSE,     led_flag, LED_COL_12 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_PLAY,      led_flag, LED_COL_13 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_FFWD,      led_flag, LED_COL_14 + LED_COL_TEMP, br_duty_level[duty_level]);        
        break;            
    case 0x03://正常按键 Normal Keys
        aw21036_onoff_one(LED_POWER,     led_flag, LED_COL_23 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_AMBILIGHT, led_flag, LED_COL_33 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_SOURCE,    led_flag, LED_COL_17 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_VOICE,     led_flag, LED_COL_20 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_MENU,      led_flag, LED_COL_22 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_BACK,      led_flag, LED_COL_18 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_HOME,      led_flag, LED_COL_21 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_VOL_L,     led_flag, LED_COL_25 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_VOL_R,     led_flag, LED_COL_27 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_123,       led_flag, LED_COL_16 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_CHN_L,     led_flag, LED_COL_29 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_CHN_R,     led_flag, LED_COL_31 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_OTT_1,     led_flag, LED_COL_19 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_OTT_2,     led_flag, LED_COL_26 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_OTT_3,     led_flag, LED_COL_24 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_OTT_4,     led_flag, LED_COL_30 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_OTT_5,     led_flag, LED_COL_28 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_OTT_6,     led_flag, LED_COL_32 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;   
    case 0x04://数字键 Digit Keys
        if(duty_level >= 70)
        {
            aw21036_onoff_one(LED_NUM_2,     led_flag, 0xFF,  br_duty_level[duty_level]);
            aw21036_onoff_one(LED_NUM_5,     led_flag, 0xFF,  br_duty_level[duty_level]);
            aw21036_onoff_one(LED_NUM_6,     led_flag, 0xFF,  br_duty_level[duty_level]);
            aw21036_onoff_one(LED_NUM_8,     led_flag, 0xFF,  br_duty_level[duty_level]);
        }
        else
        {
            aw21036_onoff_one(LED_NUM_2,     led_flag, LED_COL_2,  br_duty_level[duty_level]);
            aw21036_onoff_one(LED_NUM_5,     led_flag, LED_COL_5,  br_duty_level[duty_level]);
            aw21036_onoff_one(LED_NUM_6,     led_flag, LED_COL_6,  br_duty_level[duty_level]);
            aw21036_onoff_one(LED_NUM_8,     led_flag, LED_COL_8,  br_duty_level[duty_level]);
        }
        aw21036_onoff_one(LED_NUM_1,     led_flag, LED_COL_1 + LED_COL_TEMP,  br_duty_level[duty_level]);
        //aw21036_onoff_one(LED_NUM_2,     led_flag, LED_COL_2 + LED_COL_TEMP,  br_duty_level[duty_level]);
        aw21036_onoff_one(LED_NUM_3,     led_flag, LED_COL_3 + LED_COL_TEMP,  br_duty_level[duty_level]);
        aw21036_onoff_one(LED_NUM_4,     led_flag, LED_COL_4 + LED_COL_TEMP,  br_duty_level[duty_level]);
        //aw21036_onoff_one(LED_NUM_5,     led_flag, LED_COL_5 + LED_COL_TEMP,  br_duty_level[duty_level]);
        //aw21036_onoff_one(LED_NUM_6,     led_flag, LED_COL_6 + LED_COL_TEMP,  br_duty_level[duty_level]);
        aw21036_onoff_one(LED_NUM_7,     led_flag, LED_COL_7 + LED_COL_TEMP,  br_duty_level[duty_level]);
        //aw21036_onoff_one(LED_NUM_8,     led_flag, LED_COL_8 + LED_COL_TEMP,  br_duty_level[duty_level]);
        aw21036_onoff_one(LED_NUM_9,     led_flag, LED_COL_9 + LED_COL_TEMP,  br_duty_level[duty_level]);
        aw21036_onoff_one(LED_NUM_0,     led_flag, LED_COL_10 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;  
    case 0x05://播放簇键 Play Cluster Keys
        aw21036_onoff_one(LED_REWIND,    led_flag, LED_COL_11 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_PAUSE,     led_flag, LED_COL_12 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_PLAY,      led_flag, LED_COL_13 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_FFWD,      led_flag, LED_COL_14 + LED_COL_TEMP, br_duty_level[duty_level]);        
        break;  
    case 0x06://摇杆灯环键 Rocker Switches Light Ring
        aw21036_onoff_one(LED_VOL_L,     led_flag, LED_COL_25 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_VOL_R,     led_flag, LED_COL_27 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_CHN_L,     led_flag, LED_COL_29 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_CHN_R,     led_flag, LED_COL_31 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;  
    case 0x07://OTT Keys
        aw21036_onoff_one(LED_OTT_1,     led_flag, LED_COL_19 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_OTT_2,     led_flag, LED_COL_26 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_OTT_3,     led_flag, LED_COL_24 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_OTT_4,     led_flag, LED_COL_30 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_OTT_5,     led_flag, LED_COL_28 + LED_COL_TEMP, br_duty_level[duty_level]);
        aw21036_onoff_one(LED_OTT_6,     led_flag, LED_COL_32 + LED_COL_TEMP, br_duty_level[duty_level]);
        break;  

    default:
        break;
    }
}

/******************************************************************
 * @brief  pressed_power_key / pressed_power_key_release
 * @param  none
 * @return none
 * @retval void
 */
void pressed_power_key(void)
{
    tick_key_power = 0x01;
    keyScanLongPressTick = clock_time() | 1; //none zero
    LongPressTick = 2*1000*1000;
    Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_KEYPRESS;
}
void pressed_power_key_release(void)
{
    if(tick_key_power == 0x01)//short press < 2s
        LED_Test_SW_Toggle_ONE();
    tick_key_power = 0x00;
}

/******************************************************************
 * @brief  pressed_ambilight_key / pressed_ambilight_key_release
 * @param  none
 * @return none
 * @retval void
 */
void pressed_ambilight_key(void)
{
    tick_key_ambilight = 0x01;
#if (TEST_SW_LED)
    keyScanLongPressTick = clock_time() | 1; //none zero
    LongPressTick = 2*1000*1000;
    Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_KEYPRESS;
#endif
}
void pressed_ambilight_key_release(void)
{
    tick_key_ambilight = 0x00;
#if (TEST_SW_LED)
    if(led_test_sw_flag)
    {
        led_test_sleep = 0x01;
        app_schedule_ota_enter_deep();        
    }
#endif
}

/******************************************************************
 * @brief  pressed_ott_3_key / pressed_ott_3_key_release
 * @param  none
 * @return none
 * @retval void
 */
void pressed_ott_3_key(void)
{
    tick_key_ott_3 = 0x01;
    keyScanLongPressTick = clock_time() | 1; //none zero
    LongPressTick = 2*1000*1000;
    Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_KEYPRESS;
}
void pressed_ott_3_key_release(void)
{
    if(tick_key_ott_3 == 0x01)//short press < 2s
        LED_Test_SW_Increase_Decrease_BR(1);
    tick_key_ott_3 = 0x00;
}

/******************************************************************
 * @brief  pressed_ott_4_key / pressed_ott_4_key_release
 * @param  none
 * @return none
 * @retval void
 */
void pressed_ott_4_key(void)
{
    tick_key_ott_4 = 0x01;
    keyScanLongPressTick = clock_time() | 1; //none zero
    LongPressTick = 2*1000*1000;
    Accessibility_Shortcut_Mode = APP_ACCESSIBILITY_SHORTCUT_KEYPRESS;
}
void pressed_ott_4_key_release(void)
{
    if(tick_key_ott_4 == 0x01)//short press < 2s
        LED_Test_SW_Increase_Decrease_BR(0);
    tick_key_ott_4 = 0x00;
}


//BCT3236
/******************************************************************
 * @brief  bct3236_backlight_NormalKey
 * @param  led_flag
 * @return none
 * @retval void
 */
void bct3236_backlight_NormalKey(u8 led_flag)
{
    if((app_get_bat_vol() > 2300) && (ASL_Lux_Calc <= BR_LUX_LEVEL_1))
    {
    if(ASL_Lux_Calc <= BR_LUX_LEVEL_0){
        key_level_flag = 19; 
    }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_1){
        key_level_flag = 15;         
    }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_2){
        key_level_flag = 10;         
    }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_3){
        key_level_flag = 5;         
    }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_4){
        key_level_flag = 0;       
    }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_5){
        key_level_flag = 0;     
    }else{
        key_level_flag = 0;     
    }

    backlight_rcu_ledflag = led_flag;
    backlight_rcu_ledcnt = br_val_level[key_level_flag];
    if(backlight_rcu_ledflag)
        openallcnt = 0;
    else
        openallcnt = br_val_level[key_level_flag];
    led_bct3236_normalKey_00_timer = clock_time();

    if(led_flag)
        backlight_normal_Flag = true;
    else
        backlight_normal_Flag = false;
    }
    if((app_get_bat_vol() > 2300) && (ASL_Lux_Calc > BR_LUX_LEVEL_1))
    {
        if(ASL_Lux_Calc <= BR_LUX_LEVEL_0){
            key_level_flag = 19; 
        }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_1){
            key_level_flag = 15;         
        }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_2){
            key_level_flag = 10;         
        }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_3){
            key_level_flag = 5;         
        }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_4){
            key_level_flag = 0;       
        }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_5){
            key_level_flag = 0;       
        }else{
            key_level_flag = 0;      
        }

        backlight_rcu_ledflag = led_flag;
        backlight_rcu_ledcnt = br_val_level[key_level_flag];
        if(backlight_rcu_ledflag)
            openallcnt = 0;
        else
            openallcnt = br_val_level[key_level_flag];
        led_bct3236_normalKey_01_timer = clock_time();

        if(led_flag)
            backlight_normal_Flag = true;
        else
            backlight_normal_Flag = false;   
    }
}

/******************************************************************
 * @brief  bct3236_backlight_HiddenKey
 * @param  led_flag
 * @return none
 * @retval void
 */
void bct3236_backlight_HiddenKey(u8 led_flag)
{
    if(app_get_bat_vol() <= 2300)
        ASL_Lux_Calc = BR_LUX_LEVEL_0;

    if(ASL_Lux_Calc <= BR_LUX_LEVEL_0){
        key_level_flag = 19; 
    }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_1){
        key_level_flag = 15;         
    }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_2){
        key_level_flag = 10;         
    }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_3){
        key_level_flag = 5;         
    }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_4){
        key_level_flag = 0;         
    }else if(ASL_Lux_Calc <= BR_LUX_LEVEL_5){
        key_level_flag = 0;          
    }else{
        key_level_flag = 0;        
    }

    backlight_rcu_ledflag = led_flag;
    backlight_rcu_ledcnt = br_val_level[key_level_flag];
    if(backlight_rcu_ledflag)
        openallcnt = 0;
    else
        openallcnt = br_val_level[key_level_flag];
    led_bct3236_hiddenKey_timer = clock_time();

    if(led_flag)
        backlight_hidden_Flag = true;
    else
    {
        backlight_hidden_Flag = false;
        if(key_press_quick_timer == 0x01)
            key_press_quick_timer = 0x00;
    }
}

/******************************************************************
 * @brief  bct3236_backlight_AllKey
 * @param  led_flag
 * @return none
 * @retval void
 */
void bct3236_backlight_AllKey(u8 led_flag)
{
    if(app_get_bat_vol() > 2300)
    {
        if(ASL_Lux_Calc <= BR_LUX_LEVEL_1){

    if(ASL_Lux_Calc <= BR_LUX_LEVEL_0)
        key_level_flag = 19; 
    else if(ASL_Lux_Calc <= BR_LUX_LEVEL_1)
        key_level_flag = 15;         
    else if(ASL_Lux_Calc <= BR_LUX_LEVEL_2)
        key_level_flag = 10;         
    else if(ASL_Lux_Calc <= BR_LUX_LEVEL_3)
        key_level_flag = 5;         
	else if(ASL_Lux_Calc <= BR_LUX_LEVEL_4)
        key_level_flag = 0;         
    else if(ASL_Lux_Calc <= BR_LUX_LEVEL_5)
        key_level_flag = 0;        
    else
        key_level_flag = 0; 

    backlight_rcu_ledflag = led_flag;
    backlight_rcu_ledcnt = br_val_level[key_level_flag];
    if(backlight_rcu_ledflag)
        openallcnt = 0;
    else
        openallcnt = br_val_level[key_level_flag];
    led_bct3236_allKey_00_timer = clock_time();

    if(led_flag)
    {
        backlight_normal_Flag = true;
        backlight_hidden_Flag = true;
    }
    else
    {
        backlight_normal_Flag = false;
        backlight_hidden_Flag = false;
    }
    }
    else{

    if(ASL_Lux_Calc <= BR_LUX_LEVEL_0)
        key_level_flag = 19; 
    else if(ASL_Lux_Calc <= BR_LUX_LEVEL_1)
        key_level_flag = 15;         
    else if(ASL_Lux_Calc <= BR_LUX_LEVEL_2)
        key_level_flag = 10;         
    else if(ASL_Lux_Calc <= BR_LUX_LEVEL_3)
        key_level_flag = 5;         
	else if(ASL_Lux_Calc <= BR_LUX_LEVEL_4)
        key_level_flag = 0;         
    else if(ASL_Lux_Calc <= BR_LUX_LEVEL_5)
        key_level_flag = 0;      
    else
        key_level_flag = 0; 

    backlight_rcu_ledflag = led_flag;
    backlight_rcu_ledcnt = br_val_level[key_level_flag];
    if(backlight_rcu_ledflag)
        openallcnt = 0;
    else
        openallcnt = br_val_level[key_level_flag];
    led_bct3236_allKey_01_timer = clock_time();

    if(led_flag)
    {
        backlight_normal_Flag = true;
        backlight_hidden_Flag = true;
    }
    else
    {
        backlight_normal_Flag = false;
        backlight_hidden_Flag = false;
    }

    }
    }
    else
    {
        #if (LED_AW21036_ENABLE)
            aw21036_backlight_HiddenKey(led_flag);
        #endif
        #if (LED_BCT3236_ENABLE)
            bct3236_backlight_HiddenKey(led_flag);
        #endif        
    }
}

/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
