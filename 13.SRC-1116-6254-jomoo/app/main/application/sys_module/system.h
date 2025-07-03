#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "help_tools.h"
#include <types.h>


typedef enum {
    NORMAL_MODE  = 0x0,    // 正常工作模式
    FACTORY_MODE = 0x1,    // 产测工作模式 
    OTA_MODE     = 0x2     // OTA升级模式
}Enum_system_status;


typedef enum {
    NO_PAIR = 0x0,    // 未进行配对
    PAIRING = 0x1,    // 配对模式中 
    CONNECT = 0x2     // 连接中
}Enum_pair_status;


typedef enum {
    NO_SLEEP  = 0x0,   // 正常工作
    SLEEP     = 0x1,   // 休眠
    DEEPSLEEP = 0x2    // 深度休眠
}Enum_sleep_status;


typedef struct
{
    Enum_system_status     system_mode;         //表示系统处于的正常、产测和OTA模式
    Enum_pair_status       pair_status;         //表示系统是处于未配对、配对和连接  
    Enum_sleep_status      sleep_status;        //表示系统的工作状态
    u8                     other;               //拓展口
}System_status;


//全局系统状态变量定义
extern _attribute_data_retention_ volatile System_status g_sys;




#endif