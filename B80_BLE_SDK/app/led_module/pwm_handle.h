/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     pwm_handle.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef PWM_HANDLE_H_
#define PWM_HANDLE_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "../app_config.h"
#include "../../tl_common.h"

#if(UI_LED_ENABLE)
/*============================================================================*
*                        Export Global Variables
*============================================================================*/

/*============================================================================*
 *                          Functions
 *============================================================================*/
void led_pwm_init(void);
void led_pwm_disable(void);
void led_breathing_light(void);

#endif
#endif /* PWM_HANDLE_H_ */
