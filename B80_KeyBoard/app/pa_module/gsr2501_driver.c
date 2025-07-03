/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     gsr2501_driver.c
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
#include "gsr2501_driver.h"

#if(PA_MODULE_ENABLE)
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
#define	GPIO_PA_TXEN  			GPIO_PA6
#define	GPIO_PA_RXEN            GPIO_PA1
/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief  pa_module_init
 * @param  none
 * @return none
 * @retval void
 */
void pa_module_init(void)
{
    gpio_set_func(GPIO_PA_TXEN ,AS_GPIO);
    gpio_set_output_en(GPIO_PA_TXEN, 1); 	//enable output
    gpio_set_input_en(GPIO_PA_TXEN, 0);	    //disable input
    gpio_write(GPIO_PA_TXEN, Level_High);

    gpio_set_func(GPIO_PA_RXEN ,AS_GPIO);
    gpio_set_output_en(GPIO_PA_RXEN, 1); 	//enable output
    gpio_set_input_en(GPIO_PA_RXEN, 0);	    //disable input
    gpio_write(GPIO_PA_RXEN, Level_High);
}

/******************************************************************
 * @brief  pa_set_mode
 * @param  none
 * @return none
 * @retval void
 */
void pa_set_mode(u8 pa_mode)
{
    //printf("--->>> [work_debug] PA mode: %d\r\n", pa_mode);
    switch (pa_mode)
    {
    case MODE_NONE:
        gpio_write(GPIO_PA_TXEN, Level_High);
        gpio_write(GPIO_PA_RXEN, Level_High);
        break;
    case MODE_TRANSMIT:
        gpio_write(GPIO_PA_TXEN, Level_High);
        gpio_write(GPIO_PA_RXEN, Level_Low);
        break;
    case MODE_RECEIVE:
        gpio_write(GPIO_PA_TXEN, Level_Low);
        gpio_write(GPIO_PA_RXEN, Level_High);
        break;
    case MODE_BYPASS:
        gpio_write(GPIO_PA_TXEN, Level_Low);
        gpio_write(GPIO_PA_RXEN, Level_Low);
        break;    
    default:
        break;
    }
}

#endif
/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
