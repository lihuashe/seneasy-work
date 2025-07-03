/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     led_driver.c
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
#include "led_driver.h"

#if(UI_LED_ENABLE)
/*============================================================================*
 *                              Global Variables
 *============================================================================*/

/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief  led_1_driver_init / led_2_driver_init
 * @param  none
 * @return none
 * @retval void
 */
void led_1_driver_init(void)
{
    gpio_set_func(GPIO_LED_1 ,AS_GPIO);
    gpio_set_output_en(GPIO_LED_1, 1); 	//enable output
    gpio_set_input_en(GPIO_LED_1, 0);	//disable input
    gpio_write(GPIO_LED_1,!LED_ON_LEVAL);
}
void led_2_driver_init(void)
{
    gpio_set_func(GPIO_LED_2 ,AS_GPIO);
    gpio_set_output_en(GPIO_LED_2, 1); 	//enable output
    gpio_set_input_en(GPIO_LED_2, 0);	//disable input
    gpio_write(GPIO_LED_2,!LED_ON_LEVAL);
}

/******************************************************************
 * @brief  led_driver_alloff
 * @param  none
 * @return none
 * @retval void
 */
void led_driver_alloff(void)
{
    gpio_write(GPIO_LED_1, !LED_ON_LEVAL);
    gpio_write(GPIO_LED_2, !LED_ON_LEVAL);
}

/******************************************************************
 * @brief  led_driver_allon
 * @param  none
 * @return none
 * @retval void
 */
void led_driver_allon(void)
{
   // gpio_write(GPIO_LED_1, LED_ON_LEVAL);
   // gpio_write(GPIO_LED_2, LED_ON_LEVAL);
}

#endif
/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
