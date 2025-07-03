/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     key_config.h
* @brief    
* @details
* @author   lihuashe
* @date     2024-12-18
* @version  v1.0
*********************************************************************************************************
*/
#ifndef _KEY_CONFIG_H_
#define _KEY_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <key_handle.h>

//键值设定
#define GEAR_KEY     1
#define POWER_KEY    2


typedef void (*u8_One_Key_Func)(uint8_t);

typedef void (*u8_u8_One_Key_Map_Func)(uint8_t, uint16_t);

typedef void (*u8_u8_Two_Key_Func)(uint8_t, uint8_t);


typedef struct
{
    uint8_t key;              //键值
    u8_One_Key_Func do_handle;//按键对应事件
}OneKey; 

typedef struct
{
    uint8_t  key;
    uint16_t key_map;
    u8_u8_One_Key_Map_Func do_handle;
}OneKeyMap; 


typedef struct
{
    uint8_t key1;
    uint8_t key2;
    u8_u8_Two_Key_Func do_handle;
}TwoKey;


extern OneKey one_key_short_press_handle[];

extern OneKey one_key_long_press_2s_handle[];

extern OneKey two_key_long_press_5s_handle[];

#ifdef __cplusplus
}
#endif

#endif
