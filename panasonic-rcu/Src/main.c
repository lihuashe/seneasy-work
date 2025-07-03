/**
 ******************************************************************************
 * @file    main.c
 * @author  MCU Application Team
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2023 Puya Semiconductor Co.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by Puya under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "key_evt_handle.h"
#include "led_evt.h"
#include "app.h"
#include "my_log.h"
#include "time_utils.h"
#include "led_bar.h"
#include "2_4g_io.h"
#include "led.h"
#include "pm.h"
#include "my_test.h"
#include "fct.h"

void (*activity_init)(void);
void (*activity_proc)(void);

void activity_selector()
{
#if FCT_EN
    if (try2entry_fct())
    {
        activity_init = fct_init;
        activity_proc = fct_proc;
    }
    else
#endif
    {
        activity_init = app_init;
        activity_proc = app_proc;
    }
}

/**
 * @brief  Main program.
 * @retval int
 */
int main(void)
{
    /* Reset of all peripherals, Initializes the Systick. */
    HAL_Init();
    // 电源、时钟初始化
    pm_init();

    led_init();
    kb_init();
    RF_Bus_With_PAN216_Init();

    activity_selector();
    activity_init();

    HAL_Delay(10);
#if __DEBUG__ >= __DEBUG_INFO__
    my_log(">I%d[%s] entry system \n", __LINE__, __FUNCTION__);
#endif
    while (1)
    {
        key_loop(key_evt_handle);
        led_evt_proc();
        activity_proc();
        pm_refreshIWDG();
    }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void APP_ErrorHandler(void)
{
#if __DEBUG__ >= __DEBUG_ERR__
    my_log("\033[1;31m>E%d[%s]\n\033[0m", __LINE__, __FUNCTION__);
#endif
    /* Infinite loop */
    while (1)
    {
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
if (htim == &rf_2_4g_TimHandle)
        rf_2_4g_tim17_irq();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       for example: printf("Wrong parameters value: file %s on line %d\r\n", file, line)  */
    /* Infinite loop */
    while (1)
    {
    }
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
