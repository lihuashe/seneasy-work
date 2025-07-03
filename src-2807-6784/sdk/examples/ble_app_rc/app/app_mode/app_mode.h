#ifndef __KEY_MODE_H
#define __KEY_MODE_H

#include "board_config.h"

unsigned int remote_clock_exceed(unsigned int ref, unsigned int ms);
unsigned char get_remote_mode(void);
unsigned int remote_clock(void);
unsigned int get_remote_mode_tick(void);
void set_remote_mode(unsigned char remote_mode);
void remote_mode_hold_trigger(void);
void remote_mode_key_task(void);
void remote_mode_loop_task(void);

#endif	//__KEY_MODE_H
