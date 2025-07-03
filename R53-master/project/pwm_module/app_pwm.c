/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     app_pwm.c
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "app_pwm.h"
#include "../app_config.h"
#include "../../sdk/drivers/8278/pwm.h"
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
#define  TEST_PWM_NORMAL_MODE_1				1
#define SIN_VALUE_NUM 90
static u16 sin_value[SIN_VALUE_NUM] = {
	0, 35,70, 105,139,174,208,242,276,
	309,342,375,407,438,469,500,530,559,
	588,616,643,669,695,719,743,766,788,
	809,829,848,866,883,899,914,927,940,
	951,961,970,978,985,990,995,998,999,
	1000,999,998,995,990,985,978,970,961,
	951,940,927,914,899,883,866,848,829,
	809,788,766,743,719,695,669,643,616,
	588,559,530,500,469,438,407,375,342,
	309,276,242,208,174,139,105,70,35,
};
/*********************************************************************************
    PWM0   :  PA2.  PC1. PD5
    PWM1   :  PA3.  PC3.
    PWM2   :  PA4.  PC4.
    PWM3   :  PB0.  PD2.
    PWM4   :  PB1.  PB4.
    PWM5   :  PB2.  PB5.
    PWM0_N :  PA0.  PB3.  PC4	PD5
    PWM1_N :  PC1.  PD3.
    PWM2_N :  PD4.
    PWM3_N :  PC5.
    PWM4_N :  PC0.  PC6.
    PWM5_N :  PC7.
 *********************************************************************************/

/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief  blue led pwm init
 * @param  none
 * @return none
 * @retval void
 */
void blue_led_pwm_init(void)
{
	pwm_set_clk(CLOCK_SYS_CLOCK_HZ, CLOCK_SYS_CLOCK_HZ);
	gpio_set_func(GPIO_LED, AS_PWM5);
	pwm_set_mode(PWM5_ID, PWM_NORMAL_MODE);
	pwm_set_phase(PWM5_ID, 0);   //no phase at pwm beginning
	pwm_set_cycle_and_duty(PWM5_ID, (u16) (1000 * CLOCK_SYS_CLOCK_1US),  (u16) (0 * CLOCK_SYS_CLOCK_1US) );
	pwm_start(PWM5_ID);
}

/******************************************************************
 * @brief  blue led pwm disable
 * @param  none
 * @return none
 * @retval void
 */
void blue_led_pwm_disable(void)
{
	pwm_stop(PWM5_ID);	
	gpio_set_func(GPIO_LED, AS_GPIO);
}

/******************************************************************
 * @brief  blue breathing light
 * @param  none
 * @return none
 * @retval void
 */
void blue_breathing_light(void)
{
	static u16 brightness_cnt =0;
	static u32 pwm_Tick = 0;
	if (clock_time_exceed(pwm_Tick, 50 * 1000))
	{
		pwm_Tick = clock_time();
		brightness_cnt++;
		if (brightness_cnt >= SIN_VALUE_NUM)
			brightness_cnt = 0;
		pwm_set_cmp(PWM5_ID, sin_value[brightness_cnt] * CLOCK_SYS_CLOCK_1US);
	}
}

/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
