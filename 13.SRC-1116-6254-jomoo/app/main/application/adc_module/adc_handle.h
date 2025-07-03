/**
*********************************************************************************************************
*               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     adc_handle.h
* @brief    Related functions of battery management
* @details
* @author   lizefeng
* @date     2024-02-27
* @version  v1.0
*********************************************************************************************************
*/
#ifndef _ADC_HANDLE_H_
#define _ADC_HANDLE_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/


#include "driver.h"
#include "adc.h"

extern  short vbat_voltage ;
extern void power_supply_voltage_check_init();

#endif