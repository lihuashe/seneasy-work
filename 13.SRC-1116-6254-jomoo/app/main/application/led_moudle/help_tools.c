/*
 * @Author: lihuashe lihuashe@seneasy.com
 * @LastEditors: lihuashe
 * @LastEditTime: 2024-09-05 11:50:38
 * @file: help_tools.c
 * @Description: Copyright 2024 Guangdong Seneasy Intelligent Technology Co., Ltd. All rights reserved.
 */
#include "help_tools.h"



/**
 * @description: 这是一个将字符串转化成整数的函数
 * @param {char} *time   char s ms -> int ms
 * @return {*} 返回转化好的int时间
 */
u32 time_char_change_int(char *time)
{    
    u32  i = 1;
    u32 ctime = 0;
    char *save_time = 0;
    
    while(1)
    {    
        if (*time >= '0' && *time <= '9')
            ctime =  ctime * 10 + (*time - 48);
        else if (*time == '.')
        {  
            save_time = time;
            do{   
                i = i * 10;
                time++;
                if (*time == '\0')
                    break;
            } while(*(time+1) >= '0' && *(time+1) <= '9');
            time = save_time;
        }

        if ((*time == 's') || (*time == 'S'))
        {
            ctime = ctime * 1000 / i;                              //转化毫秒
            break;
        }
        if ((*time == 's' || *time == 'S') && (*(time-1) == 'm' || *(time-1) == 'M'))
        {
            ctime =  ctime;
            break;
        }

        time++;
        if (*time == '\0')
        {
            ctime = ctime * 1000 / i;                              //转化毫秒
            break;
        }
    }
    return ctime;
}



/**
 * @description: 这是一个打印数据包里面数据的函数
 * @param {void *} data 数据包起始地址
 * @param {u8} size 数据包大小
 * @param {char *} name 打印名
 * @return {*} 
 */
void printf_rf_data(void * data, u8 size, char * name)
{
    printf("\r\n");
    printf("printf_rf_data %s: ", name);
    printf("[ ");
    for (int i = 0; i < size; i++)
        printf("%01x ", *((u8 *)data + i));
    printf("]\r\n");
    // Device_flash_acq = true;
}




/**
 * @description: 这个一个比较函数
 * @param {u32} time :要限定的时间
 * @param {u32} start_time :限定时间的起始范围
 * @param {u32} end_time :限定时间的终点范围
 * @return {*} 返回1说明在范围内，返回0说明不在范围内
 */
inline u8 compare_time_gear(u32 time,u32 start_time,u32 end_time)
{

    return clock_time_exceed(time, start_time) && !clock_time_exceed(time,end_time);
}