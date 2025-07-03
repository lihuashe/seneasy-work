
#ifndef APP_MAIN_APPLICATION_UTILS_TIME_UTILS_H_
#define APP_MAIN_APPLICATION_UTILS_TIME_UTILS_H_

#include "py32f0xx_hal.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define clock_time HAL_GetTick

bool clock_time_exceed(uint32_t start_time, uint32_t timeout_ms);
unsigned int get_clock_exceed_time(unsigned int ref);
void delay_us(uint32_t us);

#ifdef __cplusplus
}
#endif

#endif  // APP_MAIN_APPLICATION_UTILS_TIME_UTILS_H_
