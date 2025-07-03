#include "function.h"
#include "key_driver.h"
#include "bath_rcu.h"
#include "string.h"
#include "types.h"
#include "factory_test.h"
#include "pwm_handle.h"
#include "keyboard.h"
#include "my_led.h"
#include "my_key.h"


_attribute_data_retention_ u8 air_temperature_num[4] = {0, 1, 3, 5};

// 位置功能切换标志
_attribute_data_retention_ bool seation_fuction_switch_flag = false;
        
// 滤芯功能标志
_attribute_data_retention_ bool filter_element_switch_flag = false;
_attribute_data_retention_ u8 set_fun_filter_element = 0x03;
bool set_fun_filter_element_flag = false;

bool warm_air_air_temperature_wakeup_state = false;

// 配对变量声明
volatile u32 pair_return_time = 0;
bool pair_circumvention_flag = false;
bool Key_dis_pair_circumvention_flag = false;

// 配对射频放大标志
bool Rf_increase_flag = false;

volatile u32 led_pwm_lighting_duration_3s_tick = 0;
volatile u32 air_temperature_lights_up_3s_tick = 0;
volatile u32 led_pwm_lighting_off_time = 0;
volatile u32 set_fun_filter_element_time = 0;

// 单按键功能切换标志位
bool Key_function_release_switch_flag = false;
bool Key_function_switch_three_runed_flag = false;
_attribute_data_retention_ bool Key_function_switch[KEY_COMFIG];

// 进产测间隔时间
volatile u32 send_time_interval;

//档位功能标准位
_attribute_data_retention_ u8 now_mode = 0x0;
_attribute_data_retention_ u8 normal_wash = 0x0;
_attribute_data_retention_ u8 child_wash = 0x1;
_attribute_data_retention_ u8 women_wash = 0x2;
_attribute_data_retention_ u8 strong_wash = 0x3;
_attribute_data_retention_ u8 arr[1024] = {};

//模式变量定义
_attribute_data_retention_ Wash_event g_wash[WASH_MODE] = {};

//功能开关变量
_attribute_data_retention_ Function_on_off g_function = {};

void fun_init(void)
{
    for (u8 i = 0; i < WASH_MODE; i++)
    {
        g_wash[i].air_temperature.gear = AIR_TEMPERATURE;
        g_wash[i].seat_position.gear = SEAT_POSOTION;
        g_wash[i].seat_temperature.gear = SEAT_TEMPERATURE;
        g_wash[i].water_pressure.gear = WATRE_PRESSURE;
        g_wash[i].water_temperature.gear = WATRE_TEMPERATURE;
    }
    g_function.mute = MUTE_CLOSE_VALUE;                 // 静音标志位
    g_function.seat_foaming = AUTOMATIC_FOAM_VALUE;     // 自动发泡
    g_function.foot_sensation = FOOT_SENSATION_VALUE;    // 脚感开关
    g_function.Foot_feel_foam = FOOT_FEEL_FOAM_VALUE;    // 脚感发泡开
    g_function.night_light = SET_INTELLIGENT_VALUE;      // 夜灯开关
    g_function.pre_wetting = PRE_WETTING_VALUE;          // 预湿润开关
    g_function.sensor_light = SET_SENSOR_LIGHT_VALUE;    // 光感
    g_function.power_save = SET_FUNPOWER_VALUE;          // 省电模式
    g_function.auto_flip_cover = SET_FUN_AOTU_VALUE;     // 自动翻盖
    g_function.auto_flush = AUTO_FLUSH_VALUE;            // 自动冲刷
    g_function.switch_gesture = SET_GESTURE_VALUE;  
    arr[0] = 0;
}


/*****************************S1_(冲刷)按键事件*****************************/
/*
 * 键名：冲刷（大冲）
 * 码值：0x0e
 * 功能：显示上一次LED
 */
void Jomoo_water_scour_event(u8 key_code_val)
{
    led_set_brightness_time("10101",500,"1","0.255",1);
    led_pwm_lighting_duration_3s_tick = clock_time();
    Send_Bath_24G_Data(CMD_FLUSH_WATER_BIG, 0x00, 0x00, key_code_val);
}

int chon_flag = 1;
/*
 * 键名：冲刷（小冲）
 * 码值：0x0f
 * 功能：显示上一次LED
 */
void Jomoo_water_sml_event(u8 key_code_val)
{
    
    if (chon_flag == 0)
    {
        led_set_brightness_time("1",500,"1","0.255",1);
        led_set_brightness_time("2",500,"1.5","0.255",1);
        led_set_brightness_time("3",500,"2","0.255",1);
        led_set_brightness_time("4",500,"2.5","0.255",1);
        led_set_brightness_time("5",500,"3","0.255",1);
    }
    else
    {
        led_set_brightness_time("5",500,"0.75","0.7",2);
        led_set_brightness_time("4",500,"1","0.6",2);
        led_set_brightness_time("3",500,"1.25","0.5",2);
        led_set_brightness_time("2",500,"1.5","0.5",2);
        led_set_brightness_time("1",500,"1.75","0.5",2); 
    }
    chon_flag = -chon_flag;
    
    led_pwm_lighting_duration_3s_tick = clock_time();
    Send_Bath_24G_Data(CMD_FLUSH_WATER_SML, 0x00, 0x00, key_code_val);
}

/*
 * 键名：冲刷（自动冲刷开）
 * 码值：0x0e
 * LED： LED1亮灭分别表示冲刷的开和关，LED3长亮
 * 注意：Key_function_switch_event(key_code_val)判断3、8秒
 */
void Jomoo_aotu_scour_open_event(u8 key_code_val)
{
    if (Key_function_switch_event(key_code_val) == false && Key_function_release_switch_flag == true)
    {
        Key_function_switch[key_code_val] = true;
        Key_function_release_switch_flag = false;
        // 主体功能函数  
        g_function.auto_flush = ON;
        printf("g_function.foot_sensation :%d\r\n", g_function.foot_sensation);
        led_pwm_function(g_function.auto_flush, g_function.foot_sensation, 1, g_function.auto_flip_cover, g_function.power_save);
        led_pwm_status = 18;
        led_pwm_lighting_duration_3s_tick = clock_time();
        Send_Bath_24G_Data(CMD_ADJUST_PARAMETER, 0x00, 0x00, key_code_val);
    }
}

/*
 * 键名：冲刷（自动冲刷关）
 * 码值：0x0e
 * LED： LED1亮灭分别表示冲刷的开和关，LED3长亮
 * 注意：Key_function_switch_event(key_code_val)判断3、8秒
 */
void Jomoo_aotu_scour_off_event(u8 key_code_val)
{
    if (Key_function_switch_event(key_code_val) == true && Key_function_release_switch_flag == true)
    {
        Key_function_switch[key_code_val] = false;
        Key_function_release_switch_flag = false;
       
        g_function.auto_flush = OFF;
        led_pwm_function(g_function.auto_flush, g_function.foot_sensation, 1, g_function.auto_flip_cover, g_function.power_save);
        led_pwm_status = 18;
        led_pwm_lighting_duration_3s_tick = clock_time();
        Send_Bath_24G_Data(CMD_ADJUST_PARAMETER, 0x00, 0x00, key_code_val);
        key_cur_type = NO_ACTION;
    }
}

/*
 * 键名：冲刷（自动冲刷开关）
 * 码值：0x0e
 * LED： LED1亮灭分别表示冲刷的开和关，LED3长亮
 */
void Jomoo_aotu_scour_open_off_event(u8 key_code_val)
{
    if (g_function.auto_flush == 0x01)
        g_function.auto_flush = 0x00;
    else if (g_function.auto_flush == 0x00)
        g_function.auto_flush = 0x01;
    led_pwm_function(g_function.auto_flush, g_function.foot_sensation, 1, g_function.auto_flip_cover, g_function.power_save);
    led_pwm_status = 18;
    led_pwm_lighting_duration_3s_tick = clock_time();
    Send_Bath_24G_Data(CMD_ADJUST_PARAMETER, 0x00, 0x00, key_code_val);
}

/*****************************S2_(停止)按键事件*****************************/
/*
 * 键名：停止
 * 码值：0x00
 * LED： 显示水温档位
 */
void Jomoo_instant_stop_event(u8 key_code_val)
{
    if (set_fun_filter_element_flag == false)
    {
        Key_dis_pairing_flag = false;
        led_pwm_status = g_wash[now_mode].water_temperature.gear;
        if (g_sys.pair_status != PAIRING)
            led_pwm_status_open(led_pwm_status);
        led_pwm_lighting_duration_3s_tick = clock_time();
        now_mode = normal_wash;
        if (g_sys.pair_status != PAIRING && Key_dis_pairing_flag == false)
            Send_Bath_24G_Data(CMD_AUTO_STOP, 0x00, 0x00, key_code_val);
    }
    else
    {
        led_pwm_status = g_wash[now_mode].water_temperature.gear;
        set_fun_filter_element_time = 0;
        set_fun_filter_element_flag = false;
        led_pwm_status_open(led_pwm_status);
        led_pwm_lighting_duration_3s_tick = clock_time();
    }
}

/*
 * 键名：自动停止
 * 码值：0x00
 * LED： 显示水温档位
 */
void Jomoo_instant_auto_stop_event(u8 key_code_val)
{
    if (set_fun_filter_element_flag == false)
    {
        Key_dis_pairing_flag = false;
        led_pwm_status = water_temperature;
        if (g_sys.pair_status != PAIRING)
            led_pwm_status_open(led_pwm_status);
        led_pwm_lighting_duration_3s_tick = clock_time();
        now_mode = normal_wash;
        if (g_sys.pair_status != PAIRING && Key_dis_pairing_flag == false)
            Send_Bath_24G_Data(CMD_AUTO_STOP, 0x00, 0x00, key_code_val);
    }
    else if (set_fun_filter_element_flag == true)
    {
        led_pwm_status = water_temperature;
        set_fun_filter_element_time = 0;
        set_fun_filter_element_flag = false;
        led_pwm_status_open(led_pwm_status);
        led_pwm_lighting_duration_3s_tick = clock_time();
    }
}

/*
 * 键名：停止（配对）
 * 码值：0x55
 * LED： 135闪烁
 */
void Jomoo_pair_event(u8 key_code_val)
{
    long_press_3s_s2_pair = true;
    g_sys.pair_status = PAIRING;
    pair_led_start_time = clock_time();
    led_set_brightness_time("10101",600,"0.5","0.5",30);
    PAIR_STATUS = 1;
    bath_send_need_f = 1;
    Key_dis_pairing_flag = true;
}

/*****************************S3_(臀洗/移动)按键事件*****************************/
/*
 * 键名：臀洗/移动 （臀洗/宽洗）
 * 码值：0x08
 * LED： 显示水温档位
 */
void Jomoo_hip_washing_movement_event(u8 key_code_val)
{
    if (long_press_3s_s2_pair == true)
    {
        printf("---> Rf_increase_flag \r\n");
        Rf_increase_flag = true;
        WaitMs(100);
    }
    else if (g_sys.pair_status != PAIRING)
    {
        now_mode = strong_wash;
        led_pwm_status = g_wash[strong_wash].air_temperature.gear;
        led_pwm_status_open(led_pwm_status);
        led_pwm_lighting_duration_3s_tick = clock_time();
        Send_Bath_24G_Data(CMD_BUTTOCK_WASH, 0x00, 0x00, key_code_val);
    }
}

/*
 * 键名：臀洗/移动（强洗）
 * 码值：0x1b
 * LED： 显示水温档位
 */
void Jomoo_Strong_washing_event(u8 key_code_val)
{

    now_mode = strong_wash;
    led_pwm_status_open(led_pwm_status);
    led_pwm_lighting_duration_3s_tick = clock_time();
    Send_Bath_24G_Data(CMD_AID_DEFECATE, 0x00, 0x00, key_code_val);
}

/*****************************S4_(妇洗/移动)按键事件*****************************/
/*
 * 键名：妇洗/移动
 * 码值：0x0b
 * LED： 显示水温档位
 */
void Jomoo_women_washing_movement_event(u8 key_code_val)
{
    now_mode = women_wash;
    led_pwm_status = g_wash[women_wash].water_temperature.gear;
    led_pwm_status_open(led_pwm_status);
    led_pwm_lighting_duration_3s_tick = clock_time();
    Send_Bath_24G_Data(CMD_WOMEN_WASH, 0x00, 0x00, key_code_val);
}

/*
 * 键名：妇洗/移动（童洗）
 * 码值：0x0d
 * LED： 显示水温档位
 */
void Jomoo_child_washing_event(u8 key_code_val)
{
    Jomoo_restore_child_washing_initialization_parameters();

    now_mode = child_wash;
    led_pwm_status = g_wash[child_wash].water_temperature.gear;
    led_pwm_status_open(led_pwm_status);
    led_pwm_lighting_duration_3s_tick = clock_time();
    Send_Bath_24G_Data(CMD_CHILD_WASH, 0x00, 0x00, key_code_val);
}

/*****************************S5_(泡沫盾)按键事件*****************************/
/*
 * 键名：泡沫盾（单次发泡/魔力泡）
 * 码值：0x1f、0x20、0x01
 * LED：显示上一次LED
 */
void Jomoo_foam_shield_event(u8 key_code_val)
{
    led_pwm_status_open(led_pwm_status);
    led_pwm_lighting_duration_3s_tick = clock_time();
    Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0x20, 0x01, key_code_val);
}

/*
 * 键名：泡沫盾（着坐发泡）（自动发泡）
 * 码值：0x1f、0x20、0x04关/0x02开
 * LED：显示上一次LED
 */
void Jomoo_seat_foaming_event(u8 key_code_val)
{
    led_pwm_status_open(led_pwm_status);
    led_pwm_lighting_duration_3s_tick = clock_time();
    
    switch (g_function.seat_foaming)
    {
    case ON:
        Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0x20, 0x04, key_code_val);
        break;    
    case OFF:
        Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0x20, 0x02, key_code_val);
        break;
    }

    g_function.seat_foaming ^= 0x1;    
}
/*****************************S6_(水温)按键事件*****************************/
/*
 * 键名：水温
 * 码值：0x19
 * LED：1、LED显示：0-5档对应0-5个LED灯；2、档位顺序：（0123454321012345）
 */
void Jomoo_water_temperature_event(u8 key_code_val)
{
    
    revrse_add_sub(&g_wash[now_mode].water_temperature.gear,0x05,0x00,&g_wash[now_mode].water_temperature.fz_flag);
    led_pwm_status = g_wash[now_mode].water_temperature.gear;
    led_pwm_status_open(led_pwm_status);
    led_pwm_lighting_duration_3s_tick = clock_time();
    Send_Bath_24G_Data(CMD_ADJUST_PARAMETER, 0x00, 0x00, key_code_val);
    
    printf("water_temperature = %d \r\n", g_wash[now_mode].water_temperature.gear);
    printf("size = %d \r\n", sizeof(Wash_event));
}

/*
 * 键名：水温（节电关闭）
 * 码值：0x19
 * LED：1、LED显示：0-5档对应0-5个LED灯；2、档位顺序：（0123454321012345）
 * 注意：Key_function_switch_event(key_code_val)判断3、8秒
 */
void Jomoo_energy_conservation_off_event(u8 key_code_val)
{
    if (Key_function_switch_event(key_code_val) == false && Key_function_release_switch_flag == true)
    {
        Key_function_switch[key_code_val] = true;
        Key_function_release_switch_flag = false;
        // 主体功能函数
        g_function.power_save = 0;

        led_pwm_function(g_function.auto_flush, g_function.foot_sensation, 1, g_function.auto_flip_cover, g_function.power_save);
        led_pwm_status = 18;
        led_pwm_lighting_duration_3s_tick = clock_time();
        Send_Bath_24G_Data(CMD_ADJUST_PARAMETER, 0x00, 0x00, key_code_val);
        key_cur_type = NO_ACTION;
    }
}

/*
 * 键名：水温（节电开启）
 * 码值：0x19
 * LED：LED5亮灭分别表示节电的开和关，LED3长亮
 * 注意：Key_function_switch_event(key_code_val)判断3、8秒
 */
void Jomoo_energy_conservation_open_event(u8 key_code_val)
{
    if (Key_function_switch_event(key_code_val) == true && Key_function_release_switch_flag == true)
    {
        Key_function_switch[key_code_val] = false;
        Key_function_release_switch_flag = false;
        g_function.power_save = ON;
        led_pwm_function(g_function.auto_flush, g_function.foot_sensation, 1, g_function.auto_flip_cover, g_function.power_save);
        led_pwm_status = 18;
        led_pwm_lighting_duration_3s_tick = clock_time();
        Send_Bath_24G_Data(CMD_ADJUST_PARAMETER, 0x00, 0x00, key_code_val);
        key_cur_type = NO_ACTION;
    }
}

/*
 * 键名：水温（节电开关启）
 * 码值：0x19
 * LED：LED5亮灭分别表示节电的开和关，LED3长亮
 * 注意：Key_function_switch_event(key_code_val)判断3、8秒
 */
void Jomoo_energy_open_off_event(u8 key_code_val)
{
    g_function.power_save ^= 0x1;             //开关切换

    led_pwm_function(g_function.auto_flush, g_function.foot_sensation, 1, g_function.auto_flip_cover, g_function.power_save);
    led_pwm_status = 18;
    led_pwm_lighting_duration_3s_tick = clock_time();
    Send_Bath_24G_Data(CMD_ADJUST_PARAMETER, 0x00, 0x00, key_code_val);
}

/*****************************S7_(座温)按键事件*****************************/
/*
 * 键名：座温
 * 码值：0x19
 * LED：1、LED显示：0-5档对应0-5个LED灯；2、档位顺序：（0123454321012345）
 */
void Jomoo_seat_temperature_event(u8 key_code_val)
{
    revrse_add_sub(&g_wash[normal_wash].seat_temperature.gear,0x05,0x00,&g_wash[normal_wash].seat_temperature.fz_flag);
    led_pwm_status = g_wash[normal_wash].seat_temperature.gear;
    led_pwm_status_open(led_pwm_status);
    led_pwm_lighting_duration_3s_tick = clock_time();
    Send_Bath_24G_Data(CMD_ADJUST_PARAMETER, 0x00, 0x00, key_code_val);
}

/*
 * 键名：座温（夜灯开关）
 * 码值：0x19
 * LED：LED：显示上一次LED
 */
void Jomoo_intelligent_night_light_event(u8 key_code_val)
{
    g_function.night_light ^= 0x1;

    led_pwm_status_open(led_pwm_status);
    led_pwm_lighting_duration_3s_tick = clock_time();
    Send_Bath_24G_Data(CMD_ADJUST_PARAMETER, 0x00, 0x00, key_code_val);
}

/*****************************S8_(暖风/风温)按键事件*****************************/

/*
 * 键名：暖风/风温
 * 码值：0x04
 * LED：1、LED显示：0，1，2，3档对应0，1，3，5个LED灯；2、档位顺序（0123210123）
 * 注意：按其他键或灯熄灭，第一次按下发送当前档位；
 */
void Jomoo_warm_air_air_temperature_event(u8 key_code_val)
{

    if (key_static[0] == key_code_val && warm_air_air_temperature_wakeup_state == false)
    {
        warm_air_air_temperature_wakeup_state = true;
    }

    if (now_mode == child_wash)
    {
        now_mode = normal_wash;
        g_wash[normal_wash].seat_position.gear = g_wash[child_wash].seat_position.gear;
        g_wash[normal_wash].water_pressure.gear = g_wash[child_wash].water_pressure.gear;
    }

    if (key_static[1] == key_code_val && warm_air_air_temperature_wakeup_state == true)
    {
        revrse_add_sub(&g_wash[now_mode].air_temperature.gear,0x03,0x00,&g_wash[now_mode].air_temperature.fz_flag);
        led_pwm_status = air_temperature_num[g_wash[now_mode].air_temperature.gear];
    }
    else
    {
        led_pwm_status = air_temperature_num[g_wash[now_mode].air_temperature.gear];
    }

    led_pwm_status_open(led_pwm_status);
    led_pwm_lighting_duration_3s_tick = clock_time();
    Send_Bath_24G_Data(CMD_WARM_WIND, 0x00, 0x00, key_code_val);
    printf("air_temperature = %d \r\n", g_wash[now_mode].air_temperature.gear);
    printf("air_temperature_tx = %d \r\n", g_wash[child_wash].air_temperature.gear);
}

/*
 * 键名：风温
 * 码值：0x19
 * LED：显示风温档位；1、LED显示：0，1，2，3档对应0，1，3，5个LED灯；2、档位顺序（0123210123）
 * 注意：按其他键或灯熄灭，第一次按下发送当前档位；
 */
void Jomoo_air_temperature_event(u8 key_code_val)
{
    
    led_pwm_status = air_temperature_num[air_temperature];
    led_pwm_status_open(led_pwm_status);
    led_pwm_lighting_duration_3s_tick = clock_time();
    Send_Bath_24G_Data(CMD_ADJUST_PARAMETER, 0x00, 0x00, key_code_val);
    printf("air_temperature = %d \r\n", air_temperature);
}

/*
 * 键名：暖风
 * 码值：0x04
 * LED： 显示风温档位
 */
void Jomoo_warm_air_event(u8 key_code_val)
{
    led_pwm_status = air_temperature_num[g_wash[now_mode].water_pressure.gear];
    led_pwm_status_open(led_pwm_status);
    led_pwm_lighting_duration_3s_tick = clock_time();
    Send_Bath_24G_Data(CMD_WARM_WIND, 0x00, 0x00, key_code_val);
    printf("air_temperature = %d \r\n", g_wash[now_mode].water_pressure.gear);
}

/*
 * 键名：暖风/风温（脚感开关）
 * 码值：0x1f、0x24（0x02/0x01）
 * LED：LED2亮灭分别表示脚感的开和关，LED3长亮
 */
void Jomoo_foot_sensation_event(u8 key_code_val)
{

    switch (g_function.foot_sensation)
    {
    case ON:
        Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0x24, 0x02, key_code_val);
        break;    
    case OFF:
        Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0x24, 0x01, key_code_val);
        break;
    }

    g_function.foot_sensation ^= 0x1;

    led_pwm_function(g_function.auto_flush, g_function.foot_sensation, 1, g_function.auto_flip_cover, g_function.power_save);
    led_pwm_status = 18;
    led_pwm_lighting_duration_3s_tick = clock_time();
}

/*****************************S9_(前移)按键事件*****************************/
/*
 * 键名：前移
 * 码值：0x18
 * LED：显示上一次LED
 * 注意：档位(23456)按到6档之按其他键恢复为5挡
 */
void Jomoo_move_forward_event(u8 key_code_val)
{
    if (set_fun_filter_element_flag == false)
    {
        led_pwm_status_open(led_pwm_status);
        if (now_mode == women_wash || now_mode == child_wash)
        {
            g_wash[now_mode].seat_position.gear++;
            if (g_wash[now_mode].seat_position.gear >= 0x06)   
                g_wash[now_mode].seat_position.gear = 0x06;
        }
        Send_Bath_24G_Data(CMD_ADJUST_WASH, 0x00, 0x00, key_code_val);

        led_pwm_all_off();
        led_pwm_control(set_fun_filter_element);
        set_fun_filter_element_time = clock_time();
        Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0xC7, 0x04, key_code_val);
    }
}

/*
 * 键名：前移（清洁）
 * 码值：0x09
 * LED：显示上一次LED
 */
void Jomoo_nozzle_clean_event(u8 key_code_val)
{
    led_pwm_status_open(led_pwm_status);
    led_pwm_lighting_duration_3s_tick = clock_time();
    Send_Bath_24G_Data(CMD_CLEAN_NOZZLE, 0x00, 0x00, key_code_val);
}

/*
 * 键名：位置（显示灯效）
 * 码值：0x18
 * LED：显示档位灯效
 */
void Jomoo_seation_event(u8 key_code_val)
{

        Send_Bath_24G_Data(CMD_ADJUST_WASH, 0x00, 0x00, key_code_val);
   
        led_pwm_all_off();
        led_pwm_control(set_fun_filter_element);
        set_fun_filter_element_time = clock_time();
        Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0xC7, 0x04, key_code_val);
    
}

/*****************************S10_(后移)按键事件*****************************/
/*
 * 键名：位置（后移）
 * 码值：0x18
 * LED：显示上一次LED
 * 注意：档位(43210)按到0档之按其他键恢复为1挡
 */
void Jomoo_move_back_event(u8 key_code_val)
{
    if (set_fun_filter_element_flag == false)
    {
        led_pwm_status_open(led_pwm_status);
       
        led_pwm_lighting_duration_3s_tick = clock_time();
        Send_Bath_24G_Data(CMD_ADJUST_WASH, 0x00, 0x00, key_code_val);

        
        led_pwm_all_off();
        led_pwm_control(set_fun_filter_element);
        set_fun_filter_element_time = clock_time();
        Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0xC7, 0x04, key_code_val);
    }
}

/*
 * 键名：后移（静音）
 * 码值：0x1f、0x32（0x40开、0x20关）
 * LED：显示上一次LED
 */
void Jomoo_device_mute_event(u8 key_code_val)
{
    led_pwm_status_open(led_pwm_status);
    led_pwm_lighting_duration_3s_tick = clock_time();
    
    switch (g_function.mute)
    {
    case ON:
        Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0x32, 0x20, key_code_val);
        break;    
    case OFF:
        Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0x32, 0x40, key_code_val);
        break;
    }

    g_function.mute ^= 0x1;
}

/*****************************S11_(水压)按键事件*****************************/
/*
 * 键名：水压
 * 码值：0x18
 * LED：1-5档对应1-5个LED灯，1档1个LED，5档5个LED；2、档位顺序：（1234543212345）
 */
void Jomoo_water_pressure_event(u8 key_code_val)
{

    led_pwm_status = g_wash[now_mode].water_pressure.gear;

    led_pwm_status_open(led_pwm_status);
    led_pwm_lighting_duration_3s_tick = clock_time();
    Send_Bath_24G_Data(CMD_ADJUST_WASH, 0x00, 0x00, key_code_val);
    printf("water_pressure = %d \r\n", led_pwm_status);
}

/*
 * 键名：水压+
 * 码值：0x18
 * LED：1-5档对应1-5个LED灯，1档1个LED，5档5个LED；2、档位顺序：（1234543212345）
 */
void Jomoo_water_pressure_add_event(u8 key_code_val)
{
   

    led_pwm_status_open(led_pwm_status);
    led_pwm_lighting_duration_3s_tick = clock_time();
    Send_Bath_24G_Data(CMD_ADJUST_WASH, 0x00, 0x00, key_code_val);
    printf("water_pressure = %d \r\n", water_pressure);

}

/*
 * 键名：水压-
 * 码值：0x18
 * LED：1-5档对应1-5个LED灯，1档1个LED，5档5个LED；2、档位顺序：（1234543212345）
 */
void Jomoo_water_pressure_dec_event(u8 key_code_val)
{
   
    led_pwm_status_open(led_pwm_status);
    led_pwm_lighting_duration_3s_tick = clock_time();
    Send_Bath_24G_Data(CMD_ADJUST_WASH, 0x00, 0x00, key_code_val);
    printf("water_pressure = %d \r\n", water_pressure);
}

/*
 * 键名：水压（预湿润）（润壁）
 * 码值：0x1f、0x30、（0x00、0x04）
 * LED：显示上一次LED
 */
void Jomoo_pre_wetting_event(u8 key_code_val)
{
    led_pwm_status_open(led_pwm_status);

    switch (g_function.pre_wetting)
    {
    case ON:
        Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0x30, 0x00, key_code_val);
        break;    
    case OFF:
        Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0x30, 0x04, key_code_val);
        break;
    }

    g_function.pre_wetting ^= 0x1;
    led_pwm_lighting_duration_3s_tick = clock_time();
}

/*********************************组合键事件****************************/

// 组合键S1——S5事件
/*
 * 键名：冲刷+泡沫盾
 * 码值：0x1f、0x20、0x08
 * LED：显示上一次LED
 */
void Jomoo_combination_forced_foaming_event(u8 key_code_val)
{
    led_pwm_status_open(led_pwm_status);
    led_pwm_lighting_duration_3s_tick = clock_time();
    Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0x20, 0x08, key_code_val);
}

/*
 * 键名：妇洗+大冲（滤芯复位）
 * 码值：0x1f、0xC7 0x01
 * LED：5个LED灯亮0.5s，熄灭
 */
void Jomoo_combination_filter_reset_event(u8 key_code_val)
{
    led_pwm_all_open();
    led_pwm_lighting_duration_3s_tick = 0;
    led_pwm_lighting_off_time = clock_time();
    Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0xC7, 0x01, key_code_val);
}

/*
 * 键名：妇洗+小冲（滤芯停止）
 * 码值：0x1f、0xC7 0x02
 * LED：5个LED灯亮0.5s，熄灭
 */
void Jomoo_combination_filter_stop_event(u8 key_code_val)
{
    led_pwm_all_open();
    led_pwm_lighting_duration_3s_tick = 0;
    led_pwm_lighting_off_time = clock_time();
    Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0xC7, 0x02, key_code_val);
}

/*
 * 键名：大冲+小冲（隐藏功能）
 * 码值：0x1f、0xC7 0x02
 * LED：5个LED灯亮0.5s，熄灭
 */
void Jomoo_combination_filter_time_event(u8 key_code_val)
{
    led_pwm_lighting_duration_3s_tick = 0;
    set_fun_filter_element_flag = true;
    led_pwm_control(set_fun_filter_element);
    set_fun_filter_element_time = clock_time();
    Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0xC7, 0x04, key_code_val);
}

/****************************衍生款功能点补充****************************/

/*
 * 键名：座温+
 * 码值：0x19
 * LED：1、0档对应0个LED，2档对应2个LED，4档对应4个LED
 *      2、档位顺序：（024）按到4档之后，按其他按键，档位仍保持4档。
 *      3、按到4档之后，如果下一次按座温-，要发2档。
 * 注意：第一次按下直接调节档位
 */
void Jomoo_seat_temperature_add_event(u8 key_code_val)
{
    if (seat_temperature < 0x04)
        seat_temperature += 0x02;
    else if (seat_temperature == 0x04)
        seat_temperature = 0x04;

    led_pwm_status = seat_temperature;
    led_pwm_status_open(led_pwm_status);
    led_pwm_lighting_duration_3s_tick = clock_time();
    Send_Bath_24G_Data(CMD_ADJUST_PARAMETER, 0x00, 0x00, key_code_val);
}

/*
 * 键名：座温-
 * 码值：0x19
 * LED：1、0档对应0个LED，2档对应2个LED，4档对应4个LED
 *      2、档位顺序：（420）按到0档之后，按其他按键，档位仍保持0档。
 *      3、按到0档之后，如果下一次按座温+，要发2档。
 * 注意：第一次按下直接调节档位
 */
void Jomoo_seat_temperature_dec_event(u8 key_code_val)
{
    if (seat_temperature > 0x00)
        seat_temperature -= 0x02;
    else if (seat_temperature == 0x00)
        seat_temperature = 0x00;
    led_pwm_status = seat_temperature;
    led_pwm_status_open(led_pwm_status);
    led_pwm_lighting_duration_3s_tick = clock_time();
    Send_Bath_24G_Data(CMD_ADJUST_PARAMETER, 0x00, 0x00, key_code_val);
}

/*
 * 键名：翻盖
 * 码值：0×1f、0×5e、0×23
 * LED：显示上一次LED的状态
 */
void Jomoo_Toilet_flap_event(u8 key_code_val)
{
    led_pwm_status_open(led_pwm_status);
    led_pwm_lighting_duration_3s_tick = clock_time();
    Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0x5e, 0x23, key_code_val);
}

/*
 * 键名：翻圈
 * 码值：0×1f、0×5e、0×24
 * LED：显示上一次LED的状态
 */
void Jomoo_Toilet_roll_event(u8 key_code_val)
{
    led_pwm_status_open(led_pwm_status);
    led_pwm_lighting_duration_3s_tick = clock_time();
    Send_Bath_24G_Data(CMD_SYSTEM_FUNCTION, 0x5e, 0x24, key_code_val);
}

/*
 * 键名：助便
 * 码值：0×1b
 * LED：显示水温档位
 */
void Jomoo_Facilitating_stool_event(u8 key_code_val)
{
    led_pwm_status = 0;
    led_pwm_status_open(led_pwm_status);
    led_pwm_lighting_duration_3s_tick = clock_time();
    Send_Bath_24G_Data(CMD_AID_DEFECATE, 0x00, 0x00, key_code_val);
}

void Jomoo_null_event(u8 key_code_val)
{
}

/****************************共同功能点******************************/

// 配对成功判断事假
_attribute_ram_code_sec_ void Jomoo_pair_success_event(void)
{
    if (long_press_3s_s2_pair == true)
    {
        printf("pair_success\r\n");
        pair_led_succuss_time = clock_time();
        led_pwm_lighting_duration_3s_tick = clock_time();
        pair_circumvention_flag = false;
        Key_dis_pair_circumvention_flag = false;
        PAIR_STATUS = 0x00;
        bath_send_need_f = 0;
        dev_wake_time_tick = clock_time();
        pair_addrass_write_flash = true;
        Jomoo_pairing_successfully_restores_default_values();
        Key_dis_pairing_flag = true;
        Rf_increase_flag = false;
        led_pwm_init_all_led();
        led_pwm_all_open();
        pair_write_flash();
        long_press_3s_s2_pair = false;
        g_sys.pair_status = CONNECT;
    }
}

// 设备匹配事件
void Jomoo_Divece_pairing_event()
{

    // 规避中断直接运行时间
    if (g_sys.pair_status == PAIRING && Key_dis_pair_circumvention_flag == true)
    {
        printf("Key_dis_pairing_\r\n");
        g_sys.pair_status = NO_PAIR;
        PAIR_STATUS = 0xff;
        long_press_3s_s2_pair = false;
        led_pwm_all_off();
        interrupted_time = clock_time();
    }
    else if (g_sys.pair_status == PAIRING && Key_dis_pairing_flag == false)
    {
        Key_dis_pair_circumvention_flag = true;
    }


    // 按键主动结束配对进入休眠
    if (PAIR_STATUS == 0xff && key_cur_status == KEY_RELEASED && interrupted_time && !clock_time_exceed(interrupted_time, 15 * 1000 * 1000))
    {
        printf("interrupted_time\r\n");
        interrupted_time = 0;
        dev_now_wake_function();
    }
}

// 配对后功能复位
void Jomoo_pairing_successfully_restores_default_values()
{
    led_pwm_status = 3; // led_pwm上一次状态
    set_fun_filter_element = 0x03;

    filter_element_switch_flag = false;
    seation_fuction_switch_flag = false;

    // 0为加档 1为减档
    water_temperature_add_subtract_gear = 0;
    seat_temperature_add_subtract_gear = 0;

    Function_data_init();
    printf("haier_pairing_successfully_restores_default_values\r\n");
}

// 进入童洗档位初始化
void Jomoo_restore_child_washing_initialization_parameters()
{
    g_wash[child_wash].air_temperature.gear = 0x01;
    g_wash[child_wash].seat_position.gear = 0x05;
    g_wash[child_wash].water_pressure.gear = 0x01;
    g_wash[child_wash].water_temperature.gear = 0x01;
    
    printf("Jomoo_restore_child_washing_initialization_parameters\r\n");
}

// 进场场测模式事件
void powered_on_press_enter_production_testing()
{
    static u32 proc_continue_time;
    proc_continue_time = clock_time();
    if (factory_test_key_falg == true)
    {
        while (!clock_time_exceed(proc_continue_time, 1 * 100 * 1000))
        {
            kb_event.keycode[0] = 0;
            kb_event.keycode[1] = 0;
            kb_scan_key(0, 1);

            if ((kb_event.keycode[0] == FACTORY_KEY_1 && kb_event.keycode[1] == FACTORY_KEY_2) ||
                (kb_event.keycode[0] == FACTORY_KEY_2 && kb_event.keycode[1] == FACTORY_KEY_1))
            {
                printf("--->>> led_pwm_init_all_led();\r\n");
                led_pwm_init_all_led();
                tick_factory_test_check = clock_time();
                dev_wake_time_tick = 0; // 关闭3秒睡眠
                production_testing = 1;
                production_testing_power_on_led_off = 1;
                factory_channel_mode = 1;
                led_pwm_all_open();
                sleep_ms(200);
                led_pwm_all_off();
                sleep_ms(200);
                led_pwm_all_open();
                sleep_ms(200);
                led_pwm_all_off();
                sleep_ms(200);
                led_pwm_all_open();
                sleep_ms(200);
                led_pwm_all_off();
                printf("--->>>  rf_rx_process_factory\r\n");
                key_cur_type = NO_ACTION;
                kb_event.keycode[0] = 0;
                kb_event.keycode[1] = 0;
                kb_event.cnt = 0;
            }
            if ((kb_event.keycode[0] == SEND_MAC_KEY_1 && kb_event.keycode[1] == SEND_MAC_KEY_2) ||
                (kb_event.keycode[0] == SEND_MAC_KEY_2 && kb_event.keycode[1] == SEND_MAC_KEY_1))
            {
                printf("--->>> led_pwm_init_all_led();\r\n");

                tick_factory_test_check = clock_time();
                dev_wake_time_tick = 0;              // 关闭3秒睡眠
                send_device_mac_time = clock_time(); // 关闭3秒睡眠
                production_testing = 1;
                production_testing_power_on_led_off = 1;
                factory_channel_mode = 1;
                send_device_mac_flag = true;
                send_time_interval = clock_time();
                static bool factory_open_led_flag = false;
                printf("--->>>  send_devicce_mac\r\n");
                while (send_device_mac_time && !clock_time_exceed(send_device_mac_time, 6 * 1000 * 1000))
                {
                    if (factory_open_led_flag == false)
                    {
                        led_pwm_init_all_led();
                        led_pwm_all_open();
                        sleep_ms(200);
                        led_pwm_all_off();
                        sleep_ms(200);
                        led_pwm_all_open();
                        sleep_ms(200);
                        led_pwm_all_off();
                        sleep_ms(200);
                        led_pwm_all_open();
                        sleep_ms(200);
                        led_pwm_all_off();
                        factory_open_led_flag = true;
                    }
                    else if (factory_open_led_flag == true)
                    {
                        if (send_time_interval && !clock_time_exceed(send_time_interval, 2 * 100 * 1000))
                        {
                            rf_tx_process_factory();
                        }

                        if (send_time_interval && clock_time_exceed(send_time_interval, 5 * 100 * 1000))
                        {
                            send_time_interval = 0;
                            send_time_interval = clock_time();
                        }

                        if (clock_time_exceed(send_device_mac_time, 5 * 1000 * 1000))
                        {
                            send_device_mac_time = 0;
                            production_testing = 0;
                        }
                    }
                }
                key_cur_type = NO_ACTION;
                kb_event.keycode[0] = 0;
                kb_event.keycode[1] = 0;
                kb_event.cnt = 0;
            }
            factory_test_key_falg = false;
        }
    }
}

// 功能数据初始化事件
void Function_data_init()
{
    wakeup_src_value = 1;

    fun_init();

    for (int i = 0; i <= KEY_COMFIG; i++)
    {
        Key_function_switch[i] = true;
    }
}

// 功能切换
u8 Key_function_switch_event(u8 key_code_val)
{
    static u8 Key_function_switch_flag = 0x02;
    for (int i = 0; i <= KEY_COMFIG; i++)
    {
        if (key_code_val == i)
        {
            if (Key_function_switch[i] == false && key_cur_type == LONG_PRESS_THREE_S)
            {
                Key_function_switch_three_runed_flag = true;
                Key_function_release_switch_flag = true;
                printf("Key_function_switch_1:%d-->%d\r\n", i, Key_function_switch[i]);
                Key_function_switch_flag = Key_function_switch[i];
                return Key_function_switch_flag;
            }
            else if (Key_function_switch[i] == true && key_cur_type == LONG_PRESS_AIGHT_S && Key_function_switch_three_runed_flag == false)
            {
                Key_function_release_switch_flag = true;
                printf("Key_function_switch_2:%d-->%d\r\n", i, Key_function_switch[i]);
                Key_function_switch_flag = Key_function_switch[i];
                return Key_function_switch_flag;
            }
        }
    }
    return Key_function_switch_flag;
}
