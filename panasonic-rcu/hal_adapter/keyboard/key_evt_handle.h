
#ifndef APP_KEYBOARD_MODULE_KEY_EVT_HANDLE_H_
#define APP_KEYBOARD_MODULE_KEY_EVT_HANDLE_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include "key_config.h"
#include "key_driver.h"

    enum KeyFilterType
    {
        KEY_FILTER_BLACK = -1,
        KEY_FILTER_NONE,
        KEY_FILTER_WHITE,
    };

    typedef void (*EventHandler)(u8);
    typedef void (*CombEventHandler)(u8, u8);
    typedef void (*RepeatEventHandler)(u32 time_ms, u8 key);

#define KEY_RELEASE_EVENT_ITEM(code, handle) {code, KB_NULL, 0, handle}

#if LONG_RELEASE_EN
#define KEY_LONG_RELEASE_EVENT_ITEM(code, time_ms, handle) {code, KB_NULL, time_ms, handle}
#endif

#if REPEAT_EN
#define KEY_REPEAT_EVENT_ITEM(code, handle) {code, KB_NULL, 0, handle}
#endif

#define KEY_CLICK_EVENT_ITEM(code, handle) {code, KB_NULL, 0, handle}
#define KEY_LONG_CLICK_EVENT_ITEM(code, time_ms, handle) {code, KB_NULL, time_ms, handle}
#define KEY_COMB_CLICK_EVENT_ITEM(code1, code2, handle) {code1, code2, 0, handle}
#define KEY_COMB_LONG_CLICK_EVENT_ITEM(code1, code2, time_ms, handle) {code1, code2, time_ms, handle}

    typedef struct
    {
        u8 code1;
        u8 code2;
        u16 trigger_time_ms;
        void *handle;
    } KeyEvent;

    typedef struct
    {
        u8 code1;
        u8 code2;
        u16 trigger_time_ms;
        void *handle;
    } _KeyEvent;

    // 按键配置结构体声明
    typedef struct
    {
        _KeyEvent *release_handle_arr;
        _KeyEvent *click_handle_arr;
#if LONG_RELEASE_EN
        _KeyEvent *long_release_handle_arr;
#endif

#if REPEAT_EN
        _KeyEvent *repeat_handle_arr;
#endif
        u8 release_arr_len;
        u8 click_arr_len;
#if LONG_RELEASE_EN
        u8 long_release_arr_len;
#endif

#if REPEAT_EN
        u8 repeat_arr_len;
#endif
    } KeyMap;

    typedef struct
    {
        u8 code;
        KEY_EVENT_TYPE type;
    } KeyEventMask;

    typedef struct
    {
        KeyEventMask mask_arr[NUM_KEYS];
        s8 mask_flag;
        u8 len;
    } KeyEventMask_List;

    void set_key_map(KeyMap *km);
    void get_key_map(KeyMap *km);

    void set_release_key_map(const KeyEvent *km, u8 len);
    void set_click_key_map(const KeyEvent *km, u8 len);
#if LONG_RELEASE_EN
    void set_long_release_key_map(const KeyEvent *km, u8 len);
#endif

#if REPEAT_EN
    void set_repeat_key_map(const KeyEvent *km, u8 len);
#endif
    void register_ndef_comb_click_callback(key_handle cb);
    void register_all_release_callback(key_handle cb);

    void key_evt_handle(struct key_event *k);
    void release_key(struct key_event *k);
    void pressed_key(struct key_event *k);

#if KEY_EVENT_MASK_EN
    void key_evt_set_mask_list(KeyEventMask_List *l);
    u8 key_evt_get_mask_list(KeyEventMask_List *l);
    void key_evt_add_mask_list(KeyEventMask_List *l);
    void key_evt_remove_mask_list(KeyEventMask_List *l);
    void key_evt_add_mask(KeyEventMask *k_mask);
    void key_evt_remove_mask(KeyEventMask *k_mask);
    void key_evt_clear_mask_list();

    void key_evt_set_mask_mode(s8 mode);
    s8 key_evt_get_mask_mode();
#endif

    void key_evt_reset_all();

    void *key_evt_get_last_evt_handle();

#ifdef __cplusplus
}
#endif

#endif // APP_KEYBOARD_MODULE_KEY_EVT_HANDLE_H_
