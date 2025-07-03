/**
 * @file key_evt_handle.c
 * @author likeliang (likeliang@seneasy.com)
 * @brief
 * @version 0.1
 * @date 2025-03-05
 *
 *
 */
#include "key_driver.h"
#include "arr_utils.h"
#include "key_evt_handle.h"
#include <string.h>
#include "my_log.h"
/*============================================================================*
 *                              Local Variables
 *============================================================================*/
static KeyMap g_key_map;
#if KEY_EVENT_MASK_EN
static KeyEventMask_List g_key_evt_mask;
#endif

#if SAVE_LAST_KEY_EVENT_HANDLE_EN
static _KeyEvent g_last_evt;
#endif

#if LONG_RELEASE_EN
static uint16_t last_press_time_ms; // 上次长按时间
#endif

static key_handle g_ndef_comb_click_ck;
static key_handle g_all_release_ck;

#define MAX_LONG_CLICK_KEY 3
static u8 long_click_num;                                           // 长按按键数量
static u8 long_click_key_arr[MAX_LONG_CLICK_KEY];                   // 长按事件键值
static _KeyEvent last_long_click_evt = {KB_NULL, KB_NULL, 0, NULL}; // 记录长按事件，避免多次触发

/*============================================================================*
 *                              Global Variables
 *============================================================================*/

/*============================================================================*
 *                              Local Functions
 *============================================================================*/

/**
 * @brief 判断time_ms是否在time_s允许的误差内（是否相等），误差由宏KEY_TIME_TOL_MS控制
 * @param time_ms 比较的时间ms
 * @param time_s 比较的时间s
 * @return {none}
 */
static inline bool __is_time_s(s32 time_ms, s32 trigger_time_ms)
{
    if (trigger_time_ms == 0) // 按键按下事件
    {
        if (time_ms == 0)
            return true;
        else
            return false;
    }
    return (time_ms >= trigger_time_ms - KEY_TIME_TOL_MS) && (time_ms <= trigger_time_ms + KEY_TIME_TOL_MS);
}

#if KEY_EVENT_MASK_EN
static inline bool __mask_cmp(KeyEventMask *k_mask1, KeyEventMask *k_mask2)
{
    if (k_mask1->code == k_mask2->code &&
        k_mask1->type == k_mask2->type)
        return true;
    return false;
}

/**
 * @brief 查找k_mask是否再屏蔽列表里
 * @param k_mask
 * @return {u8} 0 不存在；在g_key_evt_mask的第n个（1，2，3，...）位置找到
 */
static u8 __mask_is_exist(KeyEventMask *k_mask)
{
    foreach (i, g_key_evt_mask.len)
    {
        // 事件和键值都屏蔽
        if (g_key_evt_mask.mask_arr[i].code != KB_NULL &&
            g_key_evt_mask.mask_arr[i].type != KEY_EVENT_TYPE_NULL &&
            __mask_cmp(&g_key_evt_mask.mask_arr[i], k_mask))
            return i + 1;

        // 事件屏蔽
        if (g_key_evt_mask.mask_arr[i].code == KB_NULL &&
            g_key_evt_mask.mask_arr[i].type == k_mask->type)
            return i + 1;

        // 键值屏蔽
        if (g_key_evt_mask.mask_arr[i].type == KEY_EVENT_TYPE_NULL &&
            g_key_evt_mask.mask_arr[i].code == k_mask->code)
            return i + 1;
    }
    return 0;
}

/**
 * @brief 查找并移除按键事件k_evt里被屏蔽的事件或键值
 * @param k_evt 查找并移除按键事件k_evt里被屏蔽的事件或键值
 * @return {u8} 按键事件还剩的事件数量
 */
static u8 __key_evt_mask_check(struct key_event *k_evt)
{
    if (!g_key_evt_mask.mask_flag) // 未开启按键事件屏蔽
        return k_evt->cnt;

    KeyEventMask k_mask;
    k_mask.type = k_evt->type;
    foreach (i, k_evt->cnt)
    {
        k_mask.code = k_evt->key_code[i];
        u8 pos = __mask_is_exist(&k_mask);
        if (!pos && g_key_evt_mask.mask_flag == KEY_FILTER_BLACK) // 黑名单状态时，不存在继续，否则移除
            continue;
        else if (pos && g_key_evt_mask.mask_flag == KEY_FILTER_WHITE) // 白名单状态时，存在继续，否则移除
            continue;

        ARRAR_REMOVE_POS(k_evt->key_code, k_evt->cnt, pos - 1, u8);
        i--;
    }
    return k_evt->cnt;
}
#endif

static inline bool __key_event_cmp(_KeyEvent *k1, _KeyEvent *k2)
{
    // my_log("time_ms : %d , c_time_ms: %d\n", k1->trigger_time_ms, k2->trigger_time_ms);
    // my_log("key0 : %d, c_key0 : %d\n", k1->code1, k2->code1);
    // my_log("key1 : %d, c_key1 : %d\n\n", k1->code2, k2->code2);
    if (k1->code1 == k2->code1 &&
        k1->code2 == k2->code2 &&
        k1->trigger_time_ms == k2->trigger_time_ms)
        return true;
    return false;
}

#if SAVE_LAST_KEY_EVENT_HANDLE_EN
static void __add_last_evt_handle(void *e_h)
{
    if (!e_h)
        return;
    g_last_evt.handle = e_h;
}
#endif

bool find_key_code(struct key_event *k_evt, u8 key)
{
    if (key == KB_CODE_ANY && k_evt->cnt)
        return true;
    foreach (i, k_evt->cnt)
        if (k_evt->key_code[i] == key)
            return true;
    return false;
}

/**
 * @brief 移除长按事件后的释放事件
 * @param key_evt
 * @return {none}
 */
void remove_after_long_click(struct key_event *key_evt)
{
    foreach (i, key_evt->cnt)
    {
        if (key_evt->keep_time_ms == 0 &&
            key_code_is_exist(long_click_key_arr, long_click_num, key_evt->key_code[i]))
        {
            if (key_evt->type == KEY_EVENT_TYPE_RELEASE)
                ARRAR_REMOVE(long_click_key_arr, long_click_num, key_evt->key_code[i], u8);
            // 从按键事件里移除按键
            ARRAR_REMOVE_POS(key_evt->key_code, key_evt->cnt, i, u8);
            i--;
        }
    }
}

/**
 * @brief 记录长按的按键值，为了移除长按事件后的释放事件
 *
 * @param key_evt
 */
void save_long_click_key(struct key_event *key_evt)
{
    if (key_evt->type == KEY_EVENT_TYPE_PRESSED &&
        long_click_num < MAX_LONG_CLICK_KEY)
    {
        foreach (i, key_evt->cnt)
        {
            if (key_code_is_exist(long_click_key_arr, long_click_num, key_evt->key_code[i])) // 按键是否存在
                continue;
            // 不存在则记录
            long_click_key_arr[long_click_num++] = key_evt->key_code[i];
        }
    }
}

/*============================================================================*
 *                              Global Functions
 *============================================================================*/

/**
 * @brief 重置所有按键映射为km所指向的按键处理数组
 * @param km 所有按键事件映射结构体，不可为空并且其他按键事件未设置映射的数组长度需要初始化为零
 * @return {none}
 */
void set_key_map(KeyMap *km)
{
    // NULL
    if (!km)
        return;
    g_key_map = *km;
}

/**
 * @brief 获取所有按键映射
 * @param[out] km 输出所有按键事件映射结构体，不可为空(NULL)
 * @return {none}
 */
void get_key_map(KeyMap *km)
{
    // NULL
    if (!km)
        return;
    *km = g_key_map;
}

/**
 * @brief 单独设置释放事件的按键映射处理，不会影响其他事件的映射
 * @param km 按键事件处理数组，不可为空
 * @param len 数组长度
 * @return {none}
 */
void set_release_key_map(const KeyEvent *km, u8 len)
{
    g_key_map.release_handle_arr = (_KeyEvent *)km;
    g_key_map.release_arr_len = len;
}

#if LONG_RELEASE_EN
/**
 * @brief 单独设置上次按下时间后释放事件的按键映射处理，不会影响其他事件的映射
 * @param km 按键事件处理数组，不可为空
 * @param len 数组长度
 * @return {none}
 */
void set_long_release_key_map(const KeyEvent *km, u8 len)
{
    g_key_map.long_release_handle_arr = (_KeyEvent *)km;
    g_key_map.long_release_arr_len = len;
}
#endif
/**
 * @brief 单独设置点击事件的按键映射处理，不会影响其他事件的映射
 * @param km 按键事件处理数组，不可为空
 * @param len 数组长度
 * @return {none}
 */
void set_click_key_map(const KeyEvent *km, u8 len)
{
    g_key_map.click_handle_arr = (_KeyEvent *)km;
    g_key_map.click_arr_len = len;
}

#if REPEAT_EN
void set_repeat_key_map(const KeyEvent *km, u8 len)
{
    g_key_map.repeat_handle_arr = (_KeyEvent *)km;
    g_key_map.repeat_arr_len = len;
}
#endif

void register_ndef_comb_click_callback(key_handle cb)
{
    g_ndef_comb_click_ck = cb;
}

void register_all_release_callback(key_handle cb)
{
    g_all_release_ck = cb;
}

/**
 * @brief 重置按键所有回调和配置
 *
 */
void key_evt_reset_all()
{
    memset(&g_key_map, 0, sizeof(g_key_map));
#if KEY_EVENT_MASK_EN
    memset(&g_key_evt_mask, 0, sizeof(g_key_evt_mask));
#endif

    memset(&g_ndef_comb_click_ck, 0, sizeof(g_ndef_comb_click_ck));
    memset(&g_all_release_ck, 0, sizeof(g_all_release_ck));
#if JOINED_KEY_EN
    kb_set_joined_key(NULL, 0);
#endif
}

#if KEY_EVENT_MASK_EN
/**
 * @brief
 * 设置屏蔽按键输入事件，在key_evt_handle(struct key_event *k)函数中忽略传入的按键事件；
 * 忽略按键类型为tpye且按键为code；待定
 * 此操作会清除上一次按键屏蔽设置；
 * 按键键值code为KB_NULL，只屏蔽按键类型；
 * 按键类型tpye为KEY_EVENT_TYPE_NULL，只屏蔽按键键值；
 * 按键键值code为KB_NULL且按键类型tpye也为KEY_EVENT_TYPE_NULL，无效；
 * @param l 按键屏蔽列表
 * @return {none}
 */
void key_evt_set_mask_list(KeyEventMask_List *l)
{
    if (!l)
        return;
    g_key_evt_mask = *l;
}

/**
 * @brief
 * @param[out] l 输出按键屏蔽列表
 * @return {u8} 当前存在的按键屏蔽数量
 */
u8 key_evt_get_mask_list(KeyEventMask_List *l)
{
    if (!l)
        return 0;
    *l = g_key_evt_mask;
    return g_key_evt_mask.len;
}

/**
 * @brief
 * 添加按键事件屏蔽
 * 按键键值code为KB_NULL，只屏蔽按键类型；
 * 按键类型tpye为KEY_EVENT_TYPE_NULL，只屏蔽按键键值；
 * 按键键值code为KB_NULL且按键类型tpye也为KEY_EVENT_TYPE_NULL，无效；
 * @param l
 * @return {none}
 */
void key_evt_add_mask_list(KeyEventMask_List *l)
{
    if (!l)
        return;
    foreach (i, l->len)
    {
        if (!__mask_is_exist(&l->mask_arr[i]) && g_key_evt_mask.len < NUM_KEYS)
        {
            g_key_evt_mask.mask_arr[g_key_evt_mask.len++] = l->mask_arr[i];
        }
    }
}

/**
 * @brief 添加按键事件屏蔽
 * @param k_mask
 * 按键键值code为KB_NULL，只屏蔽按键类型；
 * 按键类型tpye为KEY_EVENT_TYPE_NULL，只屏蔽按键键值；
 * 按键键值code为KB_NULL且按键类型tpye也为KEY_EVENT_TYPE_NULL，无效；
 * @return {none}
 */
void key_evt_add_mask(KeyEventMask *k_mask)
{
    if (!k_mask)
        return;

    if (!__mask_is_exist(k_mask) && g_key_evt_mask.len < NUM_KEYS)
    {
        g_key_evt_mask.mask_arr[g_key_evt_mask.len++] = *k_mask;
    }
}

/**
 * @brief 从屏蔽按键列表里移除l列表里的按键事件
 * @param l
 * @return {none}
 */
void key_evt_remove_mask_list(KeyEventMask_List *l)
{
    if (!l)
        return;
    foreach (i, l->len)
    {
        u8 pos = __mask_is_exist(&l->mask_arr[i]);
        if (pos)
        {
            ARRAR_REMOVE_POS(g_key_evt_mask.mask_arr, g_key_evt_mask.len, pos - 1, KeyEventMask);
        }
    }
}

void key_evt_clear_mask_list()
{
    g_key_evt_mask.len = 0;
    key_evt_set_mask_mode(KEY_FILTER_NONE);
}

/**
 * @brief 从屏蔽按键列表里移除l列表里的按键事件
 * @param l
 * @return {none}
 */
void key_evt_remove_mask(KeyEventMask *k_mask)
{
    if (!k_mask)
        return;

    u8 pos = __mask_is_exist(k_mask);
    if (pos)
    {
        ARRAR_REMOVE_POS(g_key_evt_mask.mask_arr, g_key_evt_mask.len, pos - 1, KeyEventMask);
    }
}

/**
 * @brief 设置按键屏蔽模式
 * @param mode KEY_FILTER_WHITE：白名单；KEY_FILTER_BLACK：黑名单；KEY_FILTER_NONE：屏蔽关闭；默认：0
 * @return {none}
 */
void key_evt_set_mask_mode(s8 mode)
{
    g_key_evt_mask.mask_flag = mode;
}

s8 key_evt_get_mask_mode()
{
    return g_key_evt_mask.mask_flag;
}
#endif

#if SAVE_LAST_KEY_EVENT_HANDLE_EN
/**
 * @brief 获取一个上一次事件处理函数
 * @return EventHandler 上一次事件处理函数
 */
void *key_evt_get_last_evt_handle()
{
    return g_last_evt.handle;
}
#endif

/**
 * @brief 按键事件处理，可在函数：
 * key_evt_set_mask_list(KeyEventMask_List *l)
 * key_evt_get_mask_list(KeyEventMask_List *l)
 * 设置屏蔽事件
 * @param k
 * @return {none}
 */
void key_evt_handle(struct key_event *k)
{
    // my_log("k->cnt : %d \n", k->cnt);
    // my_log("k->keep_time_ms : %d \n", k->keep_time_ms);
    // my_log("key0 : %d \n", k->key_code[0]);
    // my_log("key1 : %d \n", k->key_code[1]);

#if !REPEAT_EN
    if (k->keep_time_ms > 0 && k->keep_time_ms < LONG_CLICK_TIME_MS)
        return;
#endif

#if KEY_EVENT_MASK_EN
    if (!__key_evt_mask_check(k))
        return;
#endif
#if DROP_RELEASE_EVENT
    remove_after_long_click(k);
#endif

    if (!k->keep_time_ms)
    {
        // 清除上次长按事件触发记录
        memset(&last_long_click_evt, 0, sizeof(last_long_click_evt));
    }

    switch (k->type)
    {
    case KEY_EVENT_TYPE_PRESSED:
#if LONG_RELEASE_EN
        last_press_time_ms = k->keep_time_ms;
#endif
        if (!k->cnt)
            return;
        pressed_key(k);
        break;
    case KEY_EVENT_TYPE_RELEASE:
        release_key(k);
        break;
    default:
        break;
    }
}

/**
 * @brief 处理按键释放事件
 * @param k 按键事件
 * @return {none}
 */
void release_key(struct key_event *k)
{
    if (!key_get_pressed_num() && g_all_release_ck)
        g_all_release_ck(k);
#if ONLY_SINGLE_RELEASE
    if (!key_get_pressed_num())
    {
#endif

#if LONG_RELEASE_EN
        foreach (i, g_key_map.long_release_arr_len)
        {
            if (find_key_code(k, g_key_map.long_release_handle_arr[i].code1) &&
                g_key_map.long_release_handle_arr[i].code2 == KB_NULL &&
                g_key_map.long_release_handle_arr[i].trigger_time_ms <= last_press_time_ms &&
                g_key_map.long_release_handle_arr[i].trigger_time_ms + LONG_RELEASE_TIME_TOL_MS >= last_press_time_ms)
            {
                ((EventHandler)g_key_map.long_release_handle_arr[i].handle)(k->key_code[0]);
#if SAVE_LAST_KEY_EVENT_HANDLE_EN
                __add_last_evt_handle(g_key_map.long_release_handle_arr[i].handle);
#endif
                return;
            }
        }
#endif

        foreach (i, g_key_map.release_arr_len)
        {
            if (find_key_code(k, g_key_map.release_handle_arr[i].code1) &&
                g_key_map.release_handle_arr[i].code2 == KB_NULL)
            {

                ((EventHandler)g_key_map.release_handle_arr[i].handle)(k->key_code[0]);

#if SAVE_LAST_KEY_EVENT_HANDLE_EN
                __add_last_evt_handle(g_key_map.release_handle_arr[i].handle);
#endif
            }
        }
#if ONLY_SINGLE_RELEASE
    }
#endif
}

/**
 * @brief 处理按键按下事件
 * @param k
 * @return {none}
 */
void pressed_key(struct key_event *k)
{
    bool is_comb_click = false;
    switch (k->cnt)
    {
    case 1: // 单个按键事件
        foreach (i, g_key_map.click_arr_len)
        {

            if (find_key_code(k, g_key_map.click_handle_arr[i].code1) &&
                g_key_map.click_handle_arr[i].code2 == KB_NULL &&
                __is_time_s(k->keep_time_ms, g_key_map.click_handle_arr[i].trigger_time_ms))
            {
                if (__key_event_cmp(&last_long_click_evt, &g_key_map.click_handle_arr[i]))
                    continue;

                ((EventHandler)g_key_map.click_handle_arr[i].handle)(k->key_code[0]);

#if SAVE_LAST_KEY_EVENT_HANDLE_EN
                __add_last_evt_handle(g_key_map.click_handle_arr[i].handle);
#endif
                if (g_key_map.click_handle_arr[i].trigger_time_ms > 0)
                {
                    save_long_click_key(k);
                    last_long_click_evt = g_key_map.click_handle_arr[i];
                }
            }
        }
#if REPEAT_EN
        foreach (i, g_key_map.repeat_arr_len)
        {
            if (find_key_code(k, g_key_map.repeat_handle_arr[i].code1) &&
                g_key_map.repeat_handle_arr[i].code2 == KB_NULL)
            {
#if SAVE_LAST_KEY_EVENT_HANDLE_EN
                __add_last_evt_handle(g_key_map.repeat_handle_arr[i].handle);
#endif
                ((RepeatEventHandler)g_key_map.repeat_handle_arr[i].handle)(k->keep_time_ms, k->key_code[0]);
            }
        }
#endif
        break;

    case 2: // 组合按键事件
        foreach (i, g_key_map.click_arr_len)
        {
            if (find_key_code(k, g_key_map.click_handle_arr[i].code1) &&
                find_key_code(k, g_key_map.click_handle_arr[i].code2))
            {
                is_comb_click = true;

                if (__is_time_s(k->keep_time_ms, g_key_map.click_handle_arr[i].trigger_time_ms))
                {
                    if (__key_event_cmp(&last_long_click_evt, &g_key_map.click_handle_arr[i]))
                        continue;

                    // my_log("k->cnt : %d, i: %d\n", k->cnt, i);
                    // my_log("k->keep_time_ms : %d \n", k->keep_time_ms);
                    // my_log("key0 : %d, c_key0 : %d\n", k->key_code[0], g_key_map.click_handle_arr[i].code1);
                    // my_log("key1 : %d, c_key1 : %d\n\n", k->key_code[1], g_key_map.click_handle_arr[i].code2);
                    ((CombEventHandler)g_key_map.click_handle_arr[i]
                         .handle)(k->key_code[0], k->key_code[1]);
#if SAVE_LAST_KEY_EVENT_HANDLE_EN
                    __add_last_evt_handle(g_key_map.click_handle_arr[i].handle);
#endif
                    if (g_key_map.click_handle_arr[i].trigger_time_ms > 0)
                    {
                        save_long_click_key(k);
                        last_long_click_evt = g_key_map.click_handle_arr[i];
                    }
                }
            }
        }
        if (g_ndef_comb_click_ck && !is_comb_click)
            g_ndef_comb_click_ck(k);
        break;

    default:
        if (g_ndef_comb_click_ck)
            g_ndef_comb_click_ck(k);
        break;
    }
}
