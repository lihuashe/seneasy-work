#ifndef __HELP_TOOLS__H__
#define __HELP_TOOLS__H__


#include "driver.h"


/**
 * @description: 使用宏，条件为 arr[_i].value == xx,到这个条件后会返回找到的i
 * 
 */
#define ARRAY_LOOP_UNTIL(size, condition) \
    ({ int _i; \
       for (_i = 0; _i < (size); _i++) { \
           if (condition) { \
               break; \
           } \
       } \
       _i; })




extern u32 time_char_change_int(char *time);
extern void printf_rf_data(void * data, u8 size, char * name);
extern u8 compare_time_gear(u32 time,u32 start_time,u32 end_time);

#endif
