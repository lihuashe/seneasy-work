#ifndef _TIMER_H
#define _TIMER_H

#include "SYSCFG.h";
#include "uart.h";
#include "FT61F0AX.h";
#include "stdint.h"

void tim4_init(void);
void timer_isr(void);
void exit_system_timer(void);
void system_tick_exceed(uint16_t time);

#endif