#include "key_driver.h"
#include "keyboard.h"
#include "bath_rcu.h"
#include "string.h"
#include "types.h"
#include "factory_test.h"
#include "function.h"
#include "key_handle_config.h"


static u32 keyScanTick = 0;

/*********************** 变量声明 **********************/

const u8 keyScancodes[NUM_KEYS] = {
    KB_CODE_S1, KB_CODE_S2, KB_CODE_S3, KB_CODE_S4,
    KB_CODE_S5, KB_CODE_S6, KB_CODE_S7, KB_CODE_S8,
    KB_CODE_S9, KB_CODE_S10, KB_CODE_S11, KB_CODE_S12,
    KB_CODE_S13, KB_CODE_S14, KB_CODE_S15, KB_CODE_S16};

// 按键引脚端口配置
u32 Drive_key[ROW_CNT] = {Row0, Row1, Row2, Row3};
u32 Scan_key[COL_CNT] = {Col1, Col2, Col3, Col4};

unsigned char g_ks_row[ROW_CNT] = {KS_PB6, KS_PB7, KS_PC0, KS_PC1};
unsigned char g_ks_col[COL_CNT] = {KS_PD4, KS_PB2, KS_PB4, KS_PB5};


// 时钟初始化
volatile u32 key_end_time = 0;
volatile u32 interrupted_time = 0;

// 按键存储变量声明
u8 key_static[2] = {0xff, 0xff};

// 设备休眠状态
bool key_wake_static_flag = false;
_attribute_data_retention_ bool wakeup_src_value = 0;

// 按键初始化变量声明

u8 ret_kb_code = KESYCAN_END_FLAG;
bool RELEASE_HANDLE = false;



// 按键事件结构体声明
Key_current_type key_cur_type = NO_ACTION;
Key_current_status key_cur_status = KEY_RELEASED;

// 按鍵匹配标志位

bool Key_pairing_flag = false;
bool Key_dis_pairing_flag = false;
bool long_press_3s_s2_pair = false;
bool pair_addrass_write_flash = false;

// 时间标志位
volatile u32 stuck_key_time_flag = 0;
volatile u32 pair_led_start_time = 0;
volatile u32 pair_led_succuss_time = 0;

// 卡机按键标志位
_attribute_data_retention_ bool stuck_flag = false;

// 产测按键值
u32 tick_factory_test_check = 0;
u8 factory_test_key_values = 0xfb;
_attribute_data_retention_ bool factory_test_key_falg = true;
_attribute_data_retention_ bool production_testing_power_on_led_off = 0;

// 判断卡键是否释放
void Stuck_key_release_event()
{
#if STUCK_KEY_COMFIGURATION
    u8 len = app_custom_get_drive_len();

    u8 gpio_read_cnt = 0;
    for (int i = 0; i < len; i++)
    {
        if (gpio_read(drive_pins[i]))
        {
            gpio_read_cnt++;
        }
    }
    if (gpio_read_cnt == 0)
    {
        g_key_board.stuck_flag = false;
        printf("--->>> init stuck key release\r\n");
    }

#endif
}

// 卡键休眠函数
void Stuck_sleep_wakeup_event()
{
#if STUCK_KEY_PROCESS_ENABLE
    u8 len = app_custom_get_drive_len();
    for (int i = 0; i < len; i++)
    {
        extern u8 stuckKeyPress[];
        if (stuckKeyPress[i])
            cpu_set_gpio_wakeup(drive_pins[i], 0, 1);
        else
            cpu_set_gpio_wakeup(drive_pins[i], 1, 1);
    }
    stuck_flag = true;
    printf("--->>> into stuck key sleep success\r\n");
    cpu_sleep_wakeup(DEEPSLEEP_MODE_RET_SRAM_LOW16K, PM_WAKEUP_PAD, 0);
    printf("--->>> into deep fail\r\n");
#endif
}

// 防卡键事件处理
void Stuck_key_press_event()
{
#if STUCK_KEY_PROCESS_ENABLE
    if (g_key_board.stuck_flag && stuck_key_time_flag && clock_time_exceed(stuck_key_time_flag, STUCK_KEY_ENTERDEEP_TIME * 1000 * 1000))
    {
        printf("stuck key\r\n");
        stuck_key_time_flag = clock_time();
        Stuck_sleep_wakeup_event();
    }
#endif
}

// 按键释放参数初始化事件
void Key_release_data_init()
{
    key_cur_status = KEY_RELEASED;
    Key_function_release_switch_flag = false;
    Key_function_switch_three_runed_flag = false;

//    printf("--->>> Key_released\r\n");
}

// 时间循环扫描处理标志位
void key_event_loop()
{
    // 配对事件
    Jomoo_Divece_pairing_event();

    // 按键处理
    key_change_proc_old();

    // 防卡键事件
//    Stuck_key_press_event();
}

/************************************按键配置******************************/
// 立即触发事件
void Handle_pressed_1_INSTANT_PRESS_EVENT(u8 key_code_val)
{
    for (int i = 0; i < instant_press_event / sizeof(INSTANT_PRESS_EVENT[0]); ++i)
    {
        if (INSTANT_PRESS_EVENT[i].key_code == key_code_val)
        {
            INSTANT_PRESS_EVENT[i].handler(key_code_val);
            printf("--->>> %s\r\n", INSTANT_PRESS_EVENT[i].event_name);
            key_cur_type = NO_ACTION;
            break;
        }
    }
}

// 按键松开发码
void Handle_pressed_1_RELEASE_PRESS_EVENT(u8 key_code_val)
{
    u8 key = g_key_board.key[0];

    if (g_key_board.data[key].status == KEY_RELEASED0)
    {
        for (int i = 0; i < release_press_event / sizeof(RELEASE_PRESS_EVENT[0]); ++i)
        {
            if (RELEASE_PRESS_EVENT[i].key_code == key_code_val)
            {
                RELEASE_PRESS_EVENT[i].handler(key_code_val);
                printf("--->>> %s\r\n", RELEASE_PRESS_EVENT[i].event_name);
                key_cur_type = NO_ACTION;
                break;
            }
        }
    }
}

// 短按事件
void Handle_pressed_1_SHORT_PRESS_EVENT(u8 key_code_val)
{
    for (int i = 0; i < short_press_event / sizeof(SHORT_PRESS_EVENT[0]); ++i)
    {
        if (SHORT_PRESS_EVENT[i].key_code == key_code_val)
        {
            SHORT_PRESS_EVENT[i].handler(key_code_val);
            printf("--->>> %s\r\n", SHORT_PRESS_EVENT[i].event_name);
            key_cur_type = NO_ACTION;
            break;
        }
    }
}

// 长按三秒事件
void Handle_pressed_1_LONG_PRESS_THREE_S_EVENT(u8 key_code_val)
{

    for (int i = 0; i < press_three_s_event / sizeof(PRESS_THREE_S_EVENT[0]); ++i)
    {
        if (PRESS_THREE_S_EVENT[i].key_code == key_code_val)
        {
            PRESS_THREE_S_EVENT[i].handler(key_code_val);
            printf("--->>> %s\r\n", PRESS_THREE_S_EVENT[i].event_name);
            key_cur_type = NO_ACTION;
            break;
        }
    }
}

// 长按八秒事件
void Handle_pressed_1_LONG_PRESS_AIGHT_S_EVENT(u8 key_code_val)
{
    for (int i = 0; i < press_aight_s_event / sizeof(PRESS_AIGHT_S_EVENT[0]); ++i)
    {
        if (PRESS_AIGHT_S_EVENT[i].key_code == key_code_val)
        {
            PRESS_AIGHT_S_EVENT[i].handler(key_code_val);
            printf("--->>> %s\r\n", PRESS_AIGHT_S_EVENT[i].event_name);
            key_cur_type = NO_ACTION;
            break;
        }
    }

    if (!set_fun_filter_element_flag && !long_press_3s_s2_pair)
    {
        led_pwm_lighting_duration_3s_tick = clock_time();
    }
}

// 单键事件
void key_handle_pressed_1_event()
{
    u8 key = g_key_board.key[0];

    key_cur_type = g_key_board.data[key].type;

    if (g_key_board.data[key].used == true)
        return ;
    else
        g_key_board.data[key].used = true;
    
    switch(g_key_board.data[key].type)
    {
        case INSTANT_HANDLE_PRESS:
            Handle_pressed_1_INSTANT_PRESS_EVENT(key);
            break;
        case SHORT_PRESS:
            Handle_pressed_1_SHORT_PRESS_EVENT(key);
            break;
        case LONG_PRESS_THREE_S:
            Handle_pressed_1_LONG_PRESS_THREE_S_EVENT(key);
            break;
        case LONG_PRESS_AIGHT_S:
            Handle_pressed_1_LONG_PRESS_AIGHT_S_EVENT(key);
            break;
    }

    switch(g_key_board.data[key].status)
    {   
        case KEY_RELEASED0:
            Handle_pressed_1_RELEASE_PRESS_EVENT(key);
            break;
        default :
            break;
    }
       
}

void Handle_pressed_2_LONG_PRESS_AIGHT_S_EVENT(u8 key_code_val)
{
    for (int i = 0; i < press_2_long_aight_s_event / sizeof(PRESS_2_LONG_AIGHT_S_EVENT[0]); ++i)
    {
        if ((g_key_board.key[0] == PRESS_2_LONG_AIGHT_S_EVENT[i].code1 && g_key_board.key[1] == PRESS_2_LONG_AIGHT_S_EVENT[i].code2) ||
            (g_key_board.key[0] == PRESS_2_LONG_AIGHT_S_EVENT[i].code2 && g_key_board.key[1] == PRESS_2_LONG_AIGHT_S_EVENT[i].code1))
        {
            PRESS_2_LONG_AIGHT_S_EVENT[i].handler(0x23);
            printf("--->>> %s\r\n", PRESS_2_LONG_AIGHT_S_EVENT[i].event_name);
            break;
        }
    }
}

// 组合键事件
void key_handle_pressed_2_event()
{
#if KEY_COMBINE_ENABLE
    u8 key = g_key_board.key[0];
    if (g_key_board.data[key].type == LONG_PRESS_AIGHT_S)
    {
        Handle_pressed_2_LONG_PRESS_AIGHT_S_EVENT(key);
    }
#endif
}

// 按键松开执行事件
void key_change_proc_old(void)
{
    if (set_fun_filter_element_flag == false && (g_sys.pair_status != PAIRING || g_key_board.key[0] == PAIR_STOP_KEY || g_key_board.key[0] == PAIR_RF_INCREASE_KEY))
    {
        if (g_key_board.press_num == 2)
            key_handle_pressed_2_event();
        else if (g_key_board.press_num == 1)
            if (production_testing == 0)
                key_handle_pressed_1_event();
    }
    else if (set_fun_filter_element_flag == true)
    {
        if (g_key_board.key[0] == FILTERE_ELEMENT_STOP || g_key_board.key[0] == FILTERE_ELEMENT_ADD || g_key_board.key[0] == FILTERE_ELEMENT_DEC)
            key_handle_pressed_1_event();
    }
}

#if (UI_KEYBOARD_ENABLE)



/**
 * @brief      this function is used to detect if key pressed or released.
 * @param[in]  e - LinkLayer Event type
 * @param[in]  p - data pointer of event
 * @param[in]  n - data length of event
 * @return     none
 */
void proc_keyboard_Test(u8 e, u8 *p, int n)
{
    if (clock_time_exceed(keyScanTick, 8000))
    {
        keyScanTick = clock_time();
    }
    else
    {
        return;
    }

    kb_event.keycode[0] = 0;
    int det_key =  kb_scan_key(0, 1);

    if (det_key)
    {
        key_change();  
    }

    key_is_type(); 
}

#endif