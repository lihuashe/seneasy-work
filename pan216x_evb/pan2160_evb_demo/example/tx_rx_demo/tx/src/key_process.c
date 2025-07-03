#include "key_process.h"

uint8_t key_status[NUM_KEYS] = {KEY_STATUS_UNDEFINED, KEY_STATUS_UNDEFINED, KEY_STATUS_UNDEFINED};
static int press_counter[NUM_KEYS] = {0};  // 按下去抖计数器
static int release_counterr[NUM_KEYS] = {0};  // 抬起去抖计数器


void Check_Key(uint8_t key_index, uint8_t is_down)
{
    if (key_status[key_index] == KEY_STATUS_UNDEFINED)
    {
        if (is_down)
        {
            release_counterr[key_index] = 0;
        }
        else{
            release_counterr[key_index] ++;
            if(release_counterr[key_index] > (DEBOUNCE_DELAY * 10))
            {
                press_counter[key_index] = 0;
                key_status[key_index] = KEY_STATUS_RELEASE;
            }
        }
    }
    else if (key_status[key_index] == KEY_STATUS_RELEASE)
    {
        if (is_down)
        {
            press_counter[key_index] ++;
            release_counterr[key_index] = 0;
            if (press_counter[key_index] > DEBOUNCE_DELAY)
            {
                key_status[key_index] = KEY_STATUS_SHORTPRESS;
            }
        }
        else
        {
            press_counter[key_index] = 0;
            release_counterr[key_index] ++;
        }
    }
    else if (key_status[key_index] == KEY_STATUS_SHORTPRESS)
    {
        if (is_down)
        {
            key_status[key_index] = KEY_STATUS_LONGPRESS;
        }
        else
        {
            press_counter[key_index] = 0;
            release_counterr[key_index] = 0;
            key_status[key_index] = KEY_STATUS_UNDEFINED;
        }
    }
    else if (key_status[key_index] == KEY_STATUS_LONGPRESS)
    {
        if (!is_down)
        {
            press_counter[key_index] = 0;
            release_counterr[key_index] = 0;
            key_status[key_index] = KEY_STATUS_UNDEFINED;
        }
    }
}

void Key_process(void)
{
    Check_Key(KEY_BACK, KEY1_DOWN);
    Check_Key(KEY_PREV, KEY2_DOWN);
    Check_Key(KEY_NEXT, KEY3_DOWN);
    Check_Key(KEY_SELECT, KEY4_DOWN);
}
