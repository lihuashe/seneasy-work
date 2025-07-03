#ifndef __KEY_PROCESS_H_
#define __KEY_PROCESS_H_

#include "stdint.h"
#include "bsp.h"

#define NUM_KEYS 4
#define KEY_BACK 0
#define KEY_PREV 1
#define KEY_NEXT 2
#define KEY_SELECT 3

#define DEBOUNCE_DELAY 10 // 设定去抖延迟的阈值

#define KEY_STATUS_UNDEFINED 0
#define KEY_STATUS_RELEASE 1
#define KEY_STATUS_SHORTPRESS 2
#define KEY_STATUS_LONGPRESS 3


extern uint8_t key_status[NUM_KEYS];
void Check_Key(uint8_t key_index, uint8_t is_down);
void Key_process(void);


#endif
