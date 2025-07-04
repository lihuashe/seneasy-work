/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     app_pwm.h
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

#ifndef APP_PWM_H_
#define APP_PWM_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "../app_config.h"
#include "../../sdk/tl_common.h"
/*============================================================================*
*                        Export Global Variables
*============================================================================*/


/*============================================================================*
 *                          Functions
 *============================================================================*/
void blue_led_pwm_init(void);
void blue_led_pwm_disable(void);
void blue_breathing_light(void);

#endif /* IR_DRIVER_H_ */
