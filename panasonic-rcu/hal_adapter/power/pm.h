/**
 * @file mp.h
 * @author likeliang (likeliang@seneasy.com)
 * @brief 
 * @version 0.1
 * @date 2025-03-24
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef HAL_ADAPTER_POWER_MP_H_
#define HAL_ADAPTER_POWER_MP_H_
#include "py32f0xx_hal.h"
#ifdef __cplusplus
extern "C" {
#endif

#define PM_IWDG_EN 1

extern LPTIM_HandleTypeDef LPTIMConf;

void pm_init();
void pm_set_sysclock(uint32_t clk_source);
int pm_entry_sleep();
uint16_t pm_VCC();
void pm_refreshIWDG();

#ifdef __cplusplus
}
#endif

#endif  // HAL_ADAPTER_POWER_MP_H_
