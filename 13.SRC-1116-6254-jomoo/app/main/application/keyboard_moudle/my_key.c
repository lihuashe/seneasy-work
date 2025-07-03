/*
 * @Author: lihuashe lihuashe@seneasy.com
 * @Date: 2024-08-20 09:04:07
 * @LastEditors: lihuashe lihuashe@seneasy.com
 * @LastEditTime: 2024-09-02 16:20:56
 * @FilePath: \13.SRC-1116-6254-jomoo\app\main\application\keyboard_moudle\my_key.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "my_key.h"

Key_board g_key_board = {};
_attribute_data_retention_ Save_last_key g_last_key[KEY_NUM] = {};


//按键延时
bool key_delay = false;


/**
 * @description: key的恢复默认初始化
 * @return {*}
 */
void key_init(void)
{
    g_key_board.key[0] = 0;
    g_key_board.key[1] = 0;
    g_key_board.key[2] = 0;
    g_key_board.press_num = 0;
    g_key_board.stuck_flag = false;

    for (u8 i = 0; i < KEY_NUM; i++)
    {
        g_key_board.data[i].status = KEY_RELEASED0;
        g_key_board.data[i].type = 0;
        g_key_board.data[i].press_time = 0;
    }
}

/**
 * @description: 这是一个指定档位反转的函数
 * @param {u8} *status:是哪种状态
 * @param {u8} max:是开始反转最大值
 * @param {u8} min:是开始反转最小值
 * @param {bool} *revrse_flag：是记录反转方向的标志位
 * @return {*}
 */
void inline revrse_add_sub(u8 *status,u8 max,u8 min,bool *revrse_flag)
{
    if (*revrse_flag == 0)
    {
        if (*status < max)
            *status = *status + 1;
        if (*status >= max)
            *revrse_flag = 1;
    }
    else if (*revrse_flag == 1)
    {
        if (*status > min)
            *status= *status - 1;
        if (*status <= min)
            *revrse_flag = 0;
    }

}



/**
 * @description: 单个按键执行按键结构体及其其他状态更新
 * @return {*}
 */
void key_change_one_press(void)
{
    u8 key1 = 0;

#if RF_24G_FACTORY_ENABLE
    factory_test_key_values = kb_event.keycode[0];
    if (production_testing == 1)
    {
        key_handle_factory_1_event();
    }
#endif


    key1 = g_key_board.key[0];
   
    //只有按键松开并且按键间隔时间大于按键延时才行
    if (g_key_board.data[key1].status == KEY_RELEASED0 && key_delay == false)
    {
        key_delay = true;
        g_key_board.data[key1].status = KEY_PRESSED1;

        if (production_testing == 0)
        {
            g_key_board.data[key1].press_time = clock_time();   
            g_key_board.press_num = 1;
            printf("--->>> INSTANT_HANDLE_PRESS :%d \r\n",g_key_board.key[0]);

            // 按键存储
            if (key_wake_static_flag == false)
            {
                key_wake_static_flag = true;
                key_static[0] = g_key_board.key[0];
            }
            else if (key_wake_static_flag == true)
            {
                key_static[1] = key_static[0];
                key_static[0] = g_key_board.key[0];
            }
        }
    }
}

/**
 * @description: 两个按键执行按键结构体及其其他状态更新
 * @return {*}
 */
void key_change_two_press(void)
{
    u8 key1 = 0xff,key2 = 0xff;

    key1 = g_key_board.key[0];
    key2 = g_key_board.key[1];

    g_key_board.data[key1].press_time = clock_time();
    g_key_board.data[key2].press_time = clock_time();
    g_key_board.press_num = 2;

    if (g_key_board.data[key1].status == KEY_RELEASED0 || g_key_board.data[key2].status == KEY_RELEASED0)
    {
        g_key_board.data[key1].status = KEY_PRESSED1;
        g_key_board.data[key2].status = KEY_PRESSED1;
        printf(" --->key[0] = %d\r\n", key1);
        printf(" --->key[1] = %d\r\n", key2);
    }

}


/**
 * @description: 按键类型判断更新信息到结构体，并执行按键对应事件
 * @return {*}
 */
void key_is_type(void)
{
    u8 key = 0;

    if (g_key_board.key[0] == 0)
        return ;

    for (int i = 0; i < MAX_KEY ; i++)
    {   
        key = g_key_board.key[i];
        dev_wake_time_tick = clock_time();

        if (key == 0 || g_key_board.data[key].status == KEY_STUCK2 || g_key_board.data[key].press_time == 0)
            continue;
        
        if (key == 1 && g_key_board.data[key].type != INSTANT_HANDLE_PRESS && !clock_time_exceed(g_key_board.data[key].press_time, 200 * 1000))
        {
            g_key_board.data[key].type = INSTANT_HANDLE_PRESS;
            g_key_board.data[key].used = false;
        }    
        if (clock_time_exceed(g_key_board.data[key].press_time, 15 * 1000 * 1000))
        {
            g_key_board.data[key].status = KEY_STUCK2;                           //按键卡住
            g_key_board.stuck_flag = true;
            g_last_key[key].status = KEY_STUCK2;
            g_key_board.data[key].type = NO_ACTION;
            printf("--->>> long press 15s %d\r\n", key);
        }
        else if (g_key_board.data[key].type != LONG_PRESS_AIGHT_S && compare_time_gear(g_key_board.data[key].press_time,8 * 1000 *1000, 15 * 1000 *1000))
        {
            g_key_board.data[key].type = LONG_PRESS_AIGHT_S;
            g_key_board.data[key].used = false;
            printf("--->>> long press 8s %d\r\n",key);
        }
        else if (g_key_board.data[key].type != LONG_PRESS_THREE_S && compare_time_gear(g_key_board.data[key].press_time,3 * 1000 *1000, 8 * 1000 *1000))
        {
            g_key_board.data[key].type = LONG_PRESS_THREE_S;
            g_key_board.data[key].used = false;
            printf("--->>> long press 3s %d\r\n", key);
        }
        else if (g_key_board.data[key].status == KEY_RELEASED0 && !clock_time_exceed(g_key_board.data[key].press_time, 3 * 1000 * 1000))
        {
            g_key_board.data[key].type = SHORT_PRESS;
            g_key_board.data[key].used = false;
            printf("--->>> no press 3s %d\r\n", key);
        }               
    }     

    if (production_testing == 0)
        key_event_loop();

    if (g_key_board.data[g_key_board.key[0]].status == KEY_RELEASED0)
    {
        g_key_board.stuck_flag = false;
        key_init();
        Key_release_data_init();
    }

    if (key_delay == true && clock_time_exceed(g_key_board.data[key].press_time,  KEY_DELAY * 1000))
    {
        key_delay = false;
    }
}


/**
 * @description: 通过与上一次扫描键值的对比，判断按键是否释放
 * @return {*}
 */
void key_is_release(void)
{
    u8 same_comparison = 0;

    if (kb_event.cnt == 0)
    { 
        g_key_board.data[g_key_board.key[0]].status = KEY_RELEASED0;
        key_cur_status = KEY_RELEASED;
    }  
    else
    {
        
        for (u8 i = 0; i < KEY_NUM; i++)
        {
            same_comparison = 0;

            if (g_last_key[i].key == 0)
                continue;
                
            for (u8 k = 0;k < kb_event.cnt; k++)
            {
                if (g_last_key[i].key == kb_event.keycode[k])
                    same_comparison++;
            }
            if (same_comparison == 0)
            {
                g_key_board.data[i].status = KEY_RELEASED0;
                g_last_key[i].status = KEY_RELEASED0;
                g_key_board.data[i].press_time = 0;
            }
        } 
    }

    for (u8 k = 0;k < kb_event.cnt; k++)
        g_last_key[kb_event.keycode[k]].key = kb_event.keycode[k];
}



/**
 * @description: 按键触发事件，进行按键整体决策
 * @return {*}
 */
void key_change(void)
{
    u8 count_key = 0; 

    key_cur_status = KEY_PRESSED;

#if STUCK_KEY_COMFIGURATION 

    for (int i = 0; i < kb_event.cnt; i++)
    {
        if (g_key_board.data[kb_event.keycode[i]].status != KEY_STUCK2 && g_last_key[kb_event.keycode[i]].status != KEY_STUCK2)
        {
            g_key_board.key[count_key] = kb_event.keycode[i];
            count_key++;
        }

        printf("--->>> keycnt: %d\r\n", kb_event.cnt);
    } 
#else
    g_key_board.key[0] = kb_event.keycode[0];
    g_key_board.key[1] = kb_event.keycode[1];
    count_key = kb_event.cnt;
#endif
    
    switch(count_key)
    {
        case 1 : 
            key_change_one_press();
            break;
        case 2 :
            key_change_two_press();
            break;
    }
         
    key_is_release();       
}










