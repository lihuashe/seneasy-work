/**
 * @file key_driver.c
 * @author likeliang (likeliang@seneasy.com)
 * @brief
 * @version 0.1
 * @date 2025-03-05
 *
 *
 */

#define __DEBUG__ __DEBUG_CLOSE__

#include "my_log.h"
#include "key_driver.h"
#include "string.h"
#include "types.h"
#include "time_utils.h"
#include "arr_utils.h"
/*============================================================================*
 *                              Local Variables
 *============================================================================*/

#ifndef JOINED_KEY_NUM
#define JOINED_KEY_NUM 0
#endif

#ifndef JOINED_KEY
#define JOINED_KEY {1}
#endif

static struct key_state_machine g_key_fsm;

#if STUCK_KEY_ENABLE
static u8 stuck_key_num = 0;       // 卡键数量
static u8 stuck_key_arr[NUM_KEYS]; // 卡键键值黑名单
#endif

#if DEBOUNCE_EN
static key_debounce_t key_debounce[KB_ROW_NUM][KB_COL_NUM] = {0};
#endif

#if JOINED_KEY_EN
static u8 joined_key[4] = JOINED_KEY;
static u8 joined_key_num = JOINED_KEY_NUM;
#endif
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
const uint16_t row_pins[KB_ROW_NUM] = KB_ROW_PINS;
const uint16_t col_pins[KB_COL_NUM] = KB_COL_PINS;

/*============================================================================*
 *                              Local Functions
 *============================================================================*/

bool key_code_is_exist(u8 *k_arr, u8 len, u8 key)
{
    foreach (i, len)
        if (k_arr[i] == key)
            return true;
    return false;
}

bool add_element(u8 array[], u8 *size, u8 element)
{
    // 检查数组是否已满
    if (*size >= MAX_DETECT_KEY_NUM)
    {
        return false; // 数组已满，无法添加
    }

#if JOINED_KEY_EN
    if (key_code_is_exist((u8 *)joined_key, joined_key_num, element))
    {
        // 遍历数组检查是否存在
        if (key_code_is_exist(array, *size, joined_key[0]))
            return false;
        else
        {
            array[*size] = joined_key[0];
            (*size)++;
            return true;
        }
    }
#endif

    // 添加元素
    array[*size] = element;
    (*size)++;
    return true;
}

#if JOINED_KEY_EN
bool joined_key_isPress()
{
    for (size_t i = 0; i < g_key_fsm.current_kb_data.cnt; i++)
    {
        if (key_code_is_exist((u8 *)joined_key, joined_key_num, g_key_fsm.current_kb_data.keycode[i]))
            return true;
    }
    return false;
}

bool joined_key_lastIsRelease()
{
    for (size_t i = 0; i < joined_key_num; i++)
    {
        if (key_code_is_exist(g_key_fsm.last_kb_data.keycode, g_key_fsm.last_kb_data.cnt, joined_key[i]))
            return false;
    }
    return true;
}
#endif

void kb_init_row(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 配置行引脚为输入带上拉
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT | GPIO_MODE_EVT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    for (int i = 0; i < KB_ROW_NUM; i++)
    {
        GPIO_InitStruct.Pin = row_pins[i];
        HAL_GPIO_Init(KB_ROW_PORT, &GPIO_InitStruct);
    }

    // 配置列引脚为推挽输出
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    for (int i = 0; i < KB_COL_NUM; i++)
    {
        GPIO_InitStruct.Pin = col_pins[i];
        HAL_GPIO_Init(KB_COL_PORT, &GPIO_InitStruct);
        HAL_GPIO_WritePin(KB_COL_PORT, col_pins[i], GPIO_PIN_SET);
    }
}

void kb_init_col(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 配置行引脚为推挽输出
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    for (int i = 0; i < KB_ROW_NUM; i++)
    {
        GPIO_InitStruct.Pin = row_pins[i];
        HAL_GPIO_Init(KB_ROW_PORT, &GPIO_InitStruct);
        HAL_GPIO_WritePin(KB_ROW_PORT, row_pins[i], GPIO_PIN_SET);
    }

    // 配置列引脚为输入带上拉
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT | GPIO_MODE_EVT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    for (int i = 0; i < KB_COL_NUM; i++)
    {
        GPIO_InitStruct.Pin = col_pins[i];
        HAL_GPIO_Init(KB_COL_PORT, &GPIO_InitStruct);
    }
}

/**
 * @brief 行列交替扫描扫描
 *
 * @param[out] kb_data
 * @return KeyboardStatus
 * KEYBOARD_STATUS_NULL - 没有变化；
 * KEYBOARD_STATUS_RELEASE - 按键释放
 * KEYBOARD_STATUS_PRESS - 按键按下
 *
 */
KeyboardStatus kb_scan(keyscan_data_t *kb_data, bool fast_scan)
{
    KeyboardStatus key_change = KEYBOARD_STATUS_NULL;
    // TODO 行列交替扫描
    return key_change;
}

/**
 * @brief 逐行扫描
 *
 * @param[out] kb_data
 * @return KeyboardStatus
 * KEYBOARD_STATUS_NULL - 没有变化；
 * KEYBOARD_STATUS_RELEASE - 按键释放
 * KEYBOARD_STATUS_PRESS - 按键按下
 *
 */
KeyboardStatus kb_scan_row(keyscan_data_t *kb_data, bool fast_scan)
{
    KeyboardStatus key_change = KEYBOARD_STATUS_NULL;
#if DEBOUNCE_EN
    uint8_t debounce_threshold_cnt = fast_scan ? 0 : DEBOUNCE_THRESHOLD;
#endif
    for (uint8_t col = 0; col < KB_COL_NUM; col++)
    {
        // 设置当前列低电平
        HAL_GPIO_WritePin(KB_COL_PORT, col_pins[col], GPIO_PIN_RESET);

        // 延时等待电平稳定
        delay_us(20);

        // 读取行状态
        for (uint8_t row = 0; row < KB_ROW_NUM; row++)
        {
            // 读取当前键状态（1表示按下）
            uint8_t current_state = (HAL_GPIO_ReadPin(KB_ROW_PORT, row_pins[row]) == GPIO_PIN_RESET);
            uint8_t keycode = row * KB_COL_NUM + col + 1;
#if DEBOUNCE_EN
            if (current_state != key_debounce[row][col].stable_state)
            {
                if (++key_debounce[row][col].counts >= debounce_threshold_cnt)
                {
                    key_debounce[row][col].stable_state = current_state;
                    key_debounce[row][col].counts = 0;
                    // 按键按下，检查是否已存在
                    if (kb_data->cnt < MAX_DETECT_KEY_NUM && key_debounce[row][col].stable_state &&
                        !key_code_is_exist(kb_data->keycode, kb_data->cnt, keycode))
                    {
                        kb_data->keycode[kb_data->cnt++] = keycode;
                        key_change = KEYBOARD_STATUS_PRESS;
                        // my_log(">D%d[%s] PRESS key : %d\n", __LINE__, __FUNCTION__, keycode);
                    }
                    // 按键释放
                    if (!key_debounce[row][col].stable_state)
                    {
                        key_change = KEYBOARD_STATUS_RELEASE;
                        ARRAR_REMOVE(kb_data->keycode, kb_data->cnt, keycode, u8);
                        // my_log(">D%d[%s] RELEASE key : %d\n", __LINE__, __FUNCTION__, keycode);
                    }
                }
            }
            else
            {
                key_debounce[row][col].counts = 0;
            }
#else
            // 按键按下，检查是否已存在
            if (kb_data->cnt < MAX_DETECT_KEY_NUM && current_state &&
                !key_code_is_exist(kb_data->keycode, kb_data->cnt, keycode))
            {
                kb_data->keycode[kb_data->cnt++] = keycode;
                key_change = KEYBOARD_STATUS_PRESS;
#if __DEBUG__ >= __DEBUG_DEV__
                my_log(">D%d[%s] PRESS key :", __LINE__, __FUNCTION__);
                foreach (i, kb_data->cnt)
                    my_log("%d ", kb_data->keycode[i]);
                my_log("\n");
#endif
            }
            // 按键释放
            if (!current_state && key_code_is_exist(kb_data->keycode, kb_data->cnt, keycode))
            {
                ARRAR_REMOVE(kb_data->keycode, kb_data->cnt, keycode, u8);
                key_change = KEYBOARD_STATUS_RELEASE;
#if __DEBUG__ >= __DEBUG_DEV__
                my_log(">D%d[%s] RELEASE key : %d\n", __LINE__, __FUNCTION__, keycode);
#endif
            }
#endif
        }
        // 恢复行高电平
        HAL_GPIO_WritePin(KB_COL_PORT, col_pins[col], GPIO_PIN_SET);
    }

    return key_change;
}

/**
 * @brief 逐列扫描
 *
 * @param[out] kb_data
 * @return KeyboardStatus
 * KEYBOARD_STATUS_NULL - 没有变化；
 * KEYBOARD_STATUS_RELEASE - 按键释放
 * KEYBOARD_STATUS_PRESS - 按键按下
 *
 */
KeyboardStatus kb_scan_col(keyscan_data_t *kb_data, bool fast_scan)
{
    KeyboardStatus key_change = KEYBOARD_STATUS_NULL;
#if DEBOUNCE_EN
    uint8_t debounce_threshold_cnt = fast_scan ? 0 : DEBOUNCE_THRESHOLD;
#endif
    for (uint8_t row = 0; row < KB_ROW_NUM; row++) // 外循环改为遍历行
    {
        // 设置当前行低电平
        HAL_GPIO_WritePin(KB_ROW_PORT, row_pins[row], GPIO_PIN_RESET);

        // 延时等待电平稳定
        delay_us(20);

        // 读取列状态
        for (uint8_t col = 0; col < KB_COL_NUM; col++) // 内循环遍历列
        {
            // 读取当前列状态（1表示按下）
            uint8_t current_state = (HAL_GPIO_ReadPin(KB_COL_PORT, col_pins[col]) == GPIO_PIN_RESET);
            // 修正keycode计算为行优先
            uint8_t keycode = col * KB_ROW_NUM + row + 1;
#if DEBOUNCE_EN
            if (current_state != key_debounce[row][col].stable_state)
            {
                if (++key_debounce[row][col].counts >= debounce_threshold_cnt)
                {
                    key_debounce[row][col].stable_state = current_state;
                    key_debounce[row][col].counts = 0;
                    // 按键按下处理
                    if (kb_data->cnt < MAX_DETECT_KEY_NUM && key_debounce[row][col].stable_state &&
                        !key_code_is_exist(kb_data->keycode, kb_data->cnt, keycode))
                    {
                        kb_data->keycode[kb_data->cnt++] = keycode;
                        key_change = KEYBOARD_STATUS_PRESS;
                    }
                    // 按键释放处理
                    if (!key_debounce[row][col].stable_state)
                    {
                        key_change = KEYBOARD_STATUS_RELEASE;
                        ARRAR_REMOVE(kb_data->keycode, kb_data->cnt, keycode, u8);
                    }
                }
            }
            else
            {
                key_debounce[row][col].counts = 0;
            }
#else
            // 无消抖处理
            if (kb_data->cnt < MAX_DETECT_KEY_NUM && current_state &&
                !key_code_is_exist(kb_data->keycode, kb_data->cnt, keycode))
            {
                kb_data->keycode[kb_data->cnt++] = keycode;
                key_change = KEYBOARD_STATUS_PRESS;
#if __DEBUG__ >= __DEBUG_DEV__
                my_log(">D%d[%s] PRESS key :", __LINE__, __FUNCTION__);
                foreach (i, kb_data->cnt)
                    my_log("%d ", kb_data->keycode[i]);
                my_log("\n");
#endif
            }
            if (!current_state && key_code_is_exist(kb_data->keycode, kb_data->cnt, keycode))
            {
                ARRAR_REMOVE(kb_data->keycode, kb_data->cnt, keycode, u8);
                key_change = KEYBOARD_STATUS_RELEASE;
#if __DEBUG__ >= __DEBUG_DEV__
                my_log(">D%d[%s] RELEASE key : %d\n", __LINE__, __FUNCTION__, keycode);
#endif
            }
#endif
        }
        // 恢复行高电平
        HAL_GPIO_WritePin(KB_ROW_PORT, row_pins[row], GPIO_PIN_SET);
    }

    return key_change;
}

/*============================================================================*
 *                              Global Functions
 *============================================================================*/

void kb_init(void)
{
#if __DEBUG__ >= __DEBUG_INFO__
    my_log(">I%d[%s] \n", __LINE__, __FUNCTION__);
#endif
#if SCAN_METHOD == ROW_SCAN
    kb_init_row();
#elif SCAN_METHOD == COL_SCAN
    kb_init_col();
#elif SCAN_METHOD == ROW_COL_SCAN
    kb_init_row();
#endif
}

#if STUCK_KEY_ENABLE
/**
 * @brief
 * @param key_evt
 * @return {none}
 */
void check_stuck_key(struct key_event *key_evt)
{
    // 移除黑名单
    if (key_evt->type == KEY_EVENT_TYPE_RELEASE && stuck_key_num > 0)
    {
        foreach (i, stuck_key_num)
        {
            if (!key_code_is_exist(key_evt->key_code, key_evt->cnt, stuck_key_arr[i])) // 卡键按键是否还在
                continue;
            // 存在，移除黑名单
            ARRAR_REMOVE_POS(stuck_key_arr, stuck_key_num, i, u8);
            i--;
            // TODO 修改唤醒电平
        }
    }

    // 加入黑名单
    if (key_evt->type == KEY_EVENT_TYPE_PRESSED &&
        key_evt->keep_time_ms >= KEY_STUCK_TIME_MS &&
        stuck_key_num < NUM_KEYS)
    {
        foreach (i, key_evt->cnt)
        {
            if (key_code_is_exist(stuck_key_arr, stuck_key_num, key_evt->key_code[i])) // 卡键按键是否还在
                continue;
            // 不存在则加入黑名单
            stuck_key_arr[stuck_key_num++] = key_evt->key_code[i];
        }
        // TODO 进入睡眠
    }

    // 从按键事件里移除卡键键值
    foreach (i, key_evt->cnt)
    {
        if (key_code_is_exist(stuck_key_arr, stuck_key_num, key_evt->key_code[i])) // 卡键按键是否还在
        {                                                                          // 按键在黑名单，移除按键
            ARRAR_REMOVE_POS(key_evt->key_code, key_evt->cnt, i, u8);
            i--;
        }
    }
}
#endif

/**
 * @brief 唤醒快速一次扫描键盘获取按键事件，并回调处理
 * @param callback 函数指针，按键处理回调函数
 * @return {none}
 */
void kb_fast_scan(key_handle callback)
{
    struct key_event key_evt_tmp;
    memset((void *)&key_evt_tmp, 0, sizeof(struct key_event));
    g_key_fsm.last_kb_data = g_key_fsm.current_kb_data;
#if SCAN_METHOD == ROW_SCAN
    g_key_fsm.current_status = kb_scan_row(&g_key_fsm.current_kb_data, true);
#elif SCAN_METHOD == COL_SCAN
    g_key_fsm.current_status = kb_scan_col(&g_key_fsm.current_kb_data, true);
#elif SCAN_METHOD == ROW_COL_SCAN
    g_key_fsm.current_status = kb_scan(&g_key_fsm.current_kb_data, true);
#endif
    kb_get_key_event(&key_evt_tmp);
#if 0
    my_log("kb_fast_scan k->cnt : %d \n", key_evt_tmp.cnt);
    my_log("k->keep_time_ms : %d \n", key_evt_tmp.keep_time_ms);
    my_log("key0 : %d \n", key_evt_tmp.key_code[0]);
    my_log("key1 : %d \n", key_evt_tmp.key_code[1]);
#endif
    if (callback)
    {
        callback(&key_evt_tmp);
    }
}

/**
 * @brief 扫描键盘获取按键事件，并回调处理
 * @param callback 函数指针，按键处理回调函数
 * @return {none}
 */
void key_loop(key_handle callback)
{
    static u32 clock_t;
    if (!clock_time_exceed(clock_t, KEY_SCAN_INTERAL_MS))
        return;
    struct key_event key_evt_tmp;
    memset((void *)&key_evt_tmp, 0, sizeof(struct key_event));
    g_key_fsm.last_kb_data = g_key_fsm.current_kb_data;
#if SCAN_METHOD == ROW_SCAN
    g_key_fsm.current_status = kb_scan_row(&g_key_fsm.current_kb_data, false);
#elif SCAN_METHOD == COL_SCAN
    g_key_fsm.current_status = kb_scan_col(&g_key_fsm.current_kb_data, false);
#elif SCAN_METHOD == ROW_COL_SCAN
    g_key_fsm.current_status = kb_scan(&g_key_fsm.current_kb_data, false);
#endif

#if JOINED_KEY_EN
    if (g_key_fsm.current_status == KEYBOARD_STATUS_NULL && g_key_fsm.current_kb_data.cnt ||
        (joined_key_isPress() && !joined_key_lastIsRelease()))
#else
    if (g_key_fsm.current_status == KEYBOARD_STATUS_NULL && g_key_fsm.current_kb_data.cnt)
#endif
    {
        g_key_fsm.keep_time_ms += get_clock_exceed_time(clock_t);
    }
    else
        g_key_fsm.keep_time_ms = 0;

    clock_t = clock_time();

    if (!g_key_fsm.current_kb_data.cnt &&
        g_key_fsm.current_status == KEYBOARD_STATUS_NULL)
        return;

    kb_get_key_event(&key_evt_tmp);
#if 0
    my_log("key_loop k->cnt : %d \n", key_evt_tmp.cnt);
    my_log("k->keep_time_ms : %d \n", key_evt_tmp.keep_time_ms);
    my_log("key0 : %d \n", key_evt_tmp.key_code[0]);
    my_log("key1 : %d \n", key_evt_tmp.key_code[1]);
#endif
#if STUCK_KEY_ENABLE
    check_stuck_key(&key_evt_tmp);
#endif
    if (callback && key_evt_tmp.cnt)
    {
        callback(&key_evt_tmp);
#if AUTO_CLICK_AFTER_RELEASE_EN
        if (key_evt_tmp.type == KEY_EVENT_TYPE_RELEASE)
        {
            key_evt_tmp.cnt = g_key_fsm.current_kb_data.cnt;
            memcpy(key_evt_tmp.key_code, g_key_fsm.current_kb_data.keycode, g_key_fsm.current_kb_data.cnt);
            key_evt_tmp.keep_time_ms = 0;
            key_evt_tmp.type = KEY_EVENT_TYPE_PRESSED;
            callback(&key_evt_tmp);
        }
#endif
    }
}

/**
 * @brief 从键盘状态机获取按键事件
 * @param[out] key_buf 当前有cnt个处于按键类型type的按键事件以及该事件持续的时间keep_time_ms,具体键值key_code数组
 * @return {none}
 */
void kb_get_key_event(struct key_event *k_evt)
{
    keyscan_data_t *current_kb_data = &g_key_fsm.current_kb_data;
    keyscan_data_t *last_kb_data = &g_key_fsm.last_kb_data;
    if (g_key_fsm.current_status == KEYBOARD_STATUS_RELEASE)
    {
        foreach (i, last_kb_data->cnt)
        {
            if (!key_code_is_exist(current_kb_data->keycode, current_kb_data->cnt, last_kb_data->keycode[i]))
            {
#if JOINED_KEY_EN
                if (joined_key_isPress() && key_code_is_exist((u8 *)joined_key, joined_key_num, last_kb_data->keycode[i]))
                {
                    continue;
                }
#endif
                add_element(k_evt->key_code, &k_evt->cnt, last_kb_data->keycode[i]);
            }
        }

        k_evt->type = KEY_EVENT_TYPE_RELEASE;
    }
    else
    {
        for (size_t i = 0; i < current_kb_data->cnt; i++)
        {
            add_element(k_evt->key_code, &k_evt->cnt, current_kb_data->keycode[i]);
        }
        k_evt->keep_time_ms = g_key_fsm.keep_time_ms;
        k_evt->type = KEY_EVENT_TYPE_PRESSED;
    }
}

#if JOINED_KEY_EN
/**
 * @brief 设置连体按键
 * arr = NULL, num = 0 关闭连体按键
 * @param arr
 * @param num
 */
void kb_set_joined_key(u8 *arr, u8 num)
{
#if __DEBUG__ >= __DEBUG_ERR__
    if (num > 4)
    {
        printf("\033[1;31m>E%d[%s] out of range \n\033[0m", __LINE__, __FUNCTION__);
        return;
    }
#endif
    if (!arr)
    {
        joined_key_num = 0;
        return;
    }
    memcpy(joined_key, arr, num);
    joined_key_num = num;
}
#endif

void key_config_wakeupPin()
{
#if SCAN_METHOD == ROW_SCAN || SCAN_METHOD == ROW_COL_SCAN
    for (uint8_t col = 0; col < KB_COL_NUM; col++)
    {
        HAL_GPIO_WritePin(KB_COL_PORT, col_pins[col], GPIO_PIN_RESET); // 驱动列置0
    }
#elif SCAN_METHOD == COL_SCAN
    for (uint8_t row = 0; row < KB_ROW_NUM; row++)
    {
        HAL_GPIO_WritePin(KB_COL_PORT, row_pins[row], GPIO_PIN_RESET); // 驱动行置0
    }
#endif
}

void key_reset_wakeupPin()
{
#if SCAN_METHOD == ROW_SCAN || SCAN_METHOD == ROW_COL_SCAN
    for (uint8_t col = 0; col < KB_COL_NUM; col++)
    {
        HAL_GPIO_WritePin(KB_COL_PORT, col_pins[col], GPIO_PIN_SET);
    }
#elif SCAN_METHOD == COL_SCAN
    for (uint8_t row = 0; row < KB_ROW_NUM; row++)
    {
        HAL_GPIO_WritePin(KB_COL_PORT, row_pins[row], GPIO_PIN_SET);
    }
#endif
}

u8 key_get_pressed_num()
{
#if STUCK_KEY_ENABLE
    return (g_key_fsm.current_kb_data.cnt - stuck_key_num);
#else
    return g_key_fsm.current_kb_data.cnt;
#endif
}

#if STUCK_KEY_ENABLE
u8 key_get_stuck_key_num()
{
    return stuck_key_num;
}
#endif