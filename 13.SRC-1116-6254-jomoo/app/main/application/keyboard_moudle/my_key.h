#ifndef __MY_KEY_H__
#define __MY_KEY_H__

#include "keyboard.h"
#include "help_tools.h"
#include "key_driver.h"
#include <types.h>

#define KEY_NUM     14
#define MAX_KEY     6
#define KEY_DELAY   200     //200ms


typedef enum {
    KEY_RELEASED0 = 0x0,   // 按键松开
    KEY_PRESSED1,           // 按键按下 
    KEY_STUCK2               // 按键卡住
}Enum_key_status;

typedef struct
{
    u8   type;                     //按下类型
    bool used;                     //表示是否使用过
    Enum_key_status status;        //表示按键的按下、松开和卡键
    u32  press_time;              //使用表示按键按下时间  
}Key_one;

typedef struct
{
    u8       press_num;          //表示按下按键数量
    bool     stuck_flag;         //表示按键整体有无卡键(对外机制)
    u8       key[MAX_KEY];       //表示按下的有效按键
    Key_one  data[KEY_NUM];      //表示按键数量,1-KEY_NUM,对S1-SKEY_NUM
}volatile Key_board;

typedef struct
{
    Enum_key_status       status;    //表示按下按键状态
    u8                    key;       //上次按键保存
}volatile Save_last_key;


//全局变量定义
extern Key_board g_key_board;
extern _attribute_data_retention_ Save_last_key g_last_key[KEY_NUM];



extern void key_init(void);
extern void revrse_add_sub(u8 *status,u8 max,u8 min,bool *revrse_flag);
extern void key_is_type(void);
extern void key_is_release(void);
extern void key_change(void);


#endif