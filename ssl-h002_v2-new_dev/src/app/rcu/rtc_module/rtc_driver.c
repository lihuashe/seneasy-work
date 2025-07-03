/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     battery_driver.c
* @brief    rcu vbat adc sample.
* @details
* @author   chenjie
* @date     2020-05-11
* @version  v1.1
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "board.h"
#include "rtl876x_rtc.h"
#include "rtl876x_nvic.h"
#include "trace.h"
#include "rtc_driver.h"
#include "app_section.h"
#include "rtl876x_wdg.h"
#include "stdio.h"

/*============================================================================*
 *                              Macro Definitions
 *============================================================================*/
/** Prescaler value.
  * 12 bits prescaler for COUNTER frequency (32768/(PRESCALER+1)).
  * If use internal 32KHz, (32000/(PRESCALER+1)).
  * Must be written when RTC 32-bit counter is stopped.
  */
#define RTC_PRESCALER_VALUE     31  /* f = 1000Hz */

/*============================================================================*
 *                              Local Variables
 *============================================================================*/
static uint32_t rtc_ovf_cnt = 0;
static rtc_driver_raw_cb rtc_driver_comp_0_cb = NULL;

/*============================================================================*
 *                              Functions Declaration
 *============================================================================*/
void RTC_Handler(void) DATA_RAM_FUNCTION;

/*============================================================================*
 *                              Local functions
 *============================================================================*/

/*============================================================================*
 *                              Global functions
 *============================================================================*/
/**
  * @brief  Initialize rtc peripheral.
  * @param   No parameter.
  * @return  void
  */
void rtc_driver_init(void)
{
    APP_PRINT_INFO0("[driver_rtc_init] init RTC");
    rtc_ovf_cnt = 0;
    RTC_DeInit();
    RTC_SetPrescaler(RTC_PRESCALER_VALUE);

    /* Config RTC interrupt */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    RTC_SystemWakeupConfig(ENABLE);
    RTC_NvCmd(ENABLE);
    /* Start RTC */
    RTC_ResetCounter();
    RTC_Cmd(ENABLE);
}

/**
  * @brief  Deinitialize rtc peripheral.
  * @param   No parameter.
  * @return  void
  */
void rtc_driver_deinit(void)
{
    APP_PRINT_INFO0("[driver_rtc_init] deinit RTC");
    RTC_DeInit();
}

/**
  * @brief  Get RTC timestamp in us uint
  * @param   No parameter.
  * @return  uint64_t time in us
  */
uint64_t rtc_driver_get_us(void)
{
    uint64_t time_in_us;

    time_in_us = (((uint64_t)(1) << 32) * rtc_ovf_cnt + RTC_GetCounter()) * 1000 *
                 (1 + RTC_PRESCALER_VALUE) / 32;

    APP_PRINT_INFO3("[rtc_driver_get_us] %d.%d%d sec", (uint32_t)(time_in_us / 1000000),
                    (uint32_t)((time_in_us % 1000000) / 1000),
                    (uint32_t)(time_in_us % 1000));

    return time_in_us;
}

/**
  * @brief  Get RTC timestamp in ms uint
  * @param   No parameter.
  * @return  uint64_t time in ms
  */
uint64_t rtc_driver_get_ms(void)
{
    return (rtc_driver_get_us() / 1000);
}

/**
  * @brief  Get RTC timestamp in secnods uint
  * @param   No parameter.
  * @return  uint64_t time in ms
  */
uint32_t rtc_driver_get_seconds(void)
{
    return (rtc_driver_get_us() / 1000000);
}

/**
  * @brief  Set RTC comp 0 params
  * @param   time_ms - time in ms
  * @param   cb_func - callback function
  * @return  true or false
  */
bool rtc_driver_set_comp_0_params(uint32_t time_ms, rtc_driver_raw_cb cb_func)
{
    bool result = false;
    uint32_t comp_value;

    if (time_ms == 0)
    {
        result =  false;
    }
    else
    {
        if (cb_func)
        {
            rtc_driver_comp_0_cb = cb_func;
        }

        comp_value = (uint32_t)((uint64_t)(time_ms) * 32 / (1 + RTC_PRESCALER_VALUE));
        comp_value = ((RTC_GetCounter() + comp_value) & 0xFFFFFFFF);
        RTC_SetCompValue(RTC_COMP0, comp_value);

        RTC_ClearCompINT(RTC_COMP0);
        RTC_INTConfig(RTC_INT_COMP0, ENABLE);

        result = true;
    }

    APP_PRINT_INFO2("[rtc_driver_set_comp_0_params] result = %d, time_ms = %d", result, time_ms);

    return result;
}

/**
  * @brief  Clear RTC comp 0
  * @param   No parameter.
  * @return  void
  */
void rtc_driver_clear_comp_0(void)
{
    RTC_INTConfig(RTC_INT_COMP0, DISABLE);
    RTC_ClearCompINT(RTC_COMP0);
}

/**
  * @brief  RTC interrupt handle function.
  * @param  None.
  * @return None.
  */
void RTC_Handler(void)
{
    /* RTC overflow interrupt handle */
    if (RTC_GetINTStatus(RTC_INT_OVF) == SET)
    {
        APP_PRINT_INFO0("[RTC_Handler] RTC_INT_OVF");
        rtc_ovf_cnt++;
        RTC_ClearOverFlowINT();
    }

    if (RTC_GetINTStatus(RTC_INT_COMP0) == SET)
    {
        APP_PRINT_INFO0("[RTC_Handler] RTC_INT_COMP0");
        rtc_driver_clear_comp_0();

        if (rtc_driver_comp_0_cb)
        {
            rtc_driver_comp_0_cb();
        }
    }
}

/******************* (C) COPYRIGHT 2020 Realtek Semiconductor Corporation *****END OF FILE****/

