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
#include <board.h>
#include <string.h>
#include <adc_lib.h>
#include <rtl876x_rcc.h>
#include <rtl876x_adc.h>
#include <rtl876x_nvic.h>
#include <os_timer.h>
#include <swtimer.h>
#include <trace.h>
#include "battery_driver.h"
#include "bas.h"
#include "rcu_application.h"
#include "rcu_gap.h"
#include "app_task.h"
#include "app_section.h"
#include "led_config.h"
#include "usb_chg.h"
#if SUPPORT_BAT_LPC_FEATURE
#include "rtl876x_lpc.h"
#endif
#include "rtl876x_rtc.h"

#if SUPPORT_BAT_DETECT_FEATURE

/*============================================================================*
 *                              Local Variables
 *============================================================================*/
static T_BAT_INFO_STRUCT cur_bat_info;  /* current battery information */
#if SUPPORT_BAT_PERIODIC_DETECT_FEATURE
static TimerHandle_t bat_detect_timer = NULL;  /* batttery detect timer */
#endif

/*============================================================================*
 *                              Functions Declaration
 *============================================================================*/
static uint8_t bat_calculate_bat_level(uint16_t bat_value);
static uint16_t bat_calibrate_voltage(uint16_t data);
static void bat_handle_enter_low_power_event(void);
static void bat_handle_enter_normal_event(void);
static void bat_handle_key_pressed_event(void);

#if SUPPORT_BAT_LPC_FEATURE
static void bat_driver_lpc_init(void);
static void bat_driver_lpc_enter_dlps_config(void);
static void bat_driver_lpc_exit_dlps_config(void);
static void bat_lpc_handler(void) DATA_RAM_FUNCTION;
#endif

#if SUPPORT_BAT_PERIODIC_DETECT_FEATURE
static void bat_detect_timer_callback(TimerHandle_t pxTimer) DATA_RAM_FUNCTION;
#endif

void bat_enter_dlps_config(void) DATA_RAM_FUNCTION;
void bat_exit_dlps_config(void) DATA_RAM_FUNCTION;

/*============================================================================*
 *                              Local functions
 *============================================================================*/
/******************************************************************
 * @brief  update battery value, level and mode.
 * @param  src_dat - adc sample source data.
 * @return none
 * @retval void
 */
uint8_t bat_calculate_bat_level(uint16_t bat_value)
{
    uint8_t bat_level = 0;

    /*calculate bat level according to bat value*/
    if (bat_value >= 4180)  /* >4.2V, 100% */
    {
        bat_level = 100;
    }
    else if (bat_value >= 4000) /* 4.18~4.0V, 100%~80% */
    {
        bat_level = 80 + (bat_value - 4000) * (100 - 80) / (4150 - 4000);
    }
    else if (bat_value >= 3600) /* 4.0~3.6V, 80%~30% */
    {
        bat_level = 30 + (bat_value - 3600) * (80 - 30) / (4000 - 3600);
    }
    else if (bat_value >= 3400) /* 3.6~3.4V, 30%~15% */
    {
        bat_level = 15 + (bat_value - 3400) * (30 - 15) / (3600 - 3400);
    }
    else if (bat_value >= 3020) /* 3.4~3.0V, 15%~0% */
    {
        bat_level = (bat_value - 3020) * (15 - 0) / (3400 - 3020);
    }
    else /* <2.0V, 0% */
    {
        bat_level = 0;
    }

    APP_PRINT_ERROR2("[bat_calculate_bat_level] bat_value is %d, bat_level is %d", bat_value,
                     bat_level);

    return bat_level;
}

/******************************************************************
 * @brief  calculation battery calibration voltage.
 * @param  data - data read from ADC.
 * @return uint16_t - ADC voltage whose unit is mv.
 */
uint16_t bat_calibrate_voltage(uint16_t data)
{
    float adc_voltage = 0;
    ADC_ErrorStatus error_status = NO_ERROR;
    if (true == cur_bat_info.is_adc_efuse_existed)
    {
        adc_voltage = ADC_GetVoltage(DIVIDE_SINGLE_MODE, (int32_t)data, &error_status);
        if (error_status < 0)
        {

					APP_PRINT_WARN1("ADC parameter or efuse data error %d!", error_status);
            adc_voltage = (-5283) * (data * data / 100000000.0f) + (3.50388) * data + 93.8;
        }
    }
    else
    {
        adc_voltage = (-5283) * (data * data / 100000000.0f) + (3.50388) * data + 93.8;
    }

    
    if (GPIO_ReadInputDataBit(GPIO_GetPin(USB_DET)) && bat_chg_doing())
    {
        int temp_vol = 0;
        uint16_t usb_adc_voltage = (uint16_t)adc_voltage * 2 + 50;
        
        // 根据电压范围应用不同的偏移量
        if (usb_adc_voltage >= 3200 && usb_adc_voltage <= 3750) 
        {
            // 电压在3200-3700时，加200 
            temp_vol = 60;
        } 
        else if (usb_adc_voltage > 3750 && usb_adc_voltage <= 4000) 
        {
            // 电压在3700-3900时，加120
            temp_vol = 40;
        } 
        else if (usb_adc_voltage > 4000 && usb_adc_voltage <= 4200) 
        {
            // 电压在3900-4180时，减60
            temp_vol = -50;
        } 

        APP_PRINT_ERROR2("[bat_calculate_bat_level] usb_adc_voltage is %d,adc_voltage is %d", usb_adc_voltage,
                     adc_voltage * 2 + 50);
        return (uint16_t)(usb_adc_voltage - temp_vol);
    }

    return (uint16_t)adc_voltage * 2 + 50;//电阻分压，电压数据乘以2,二极管电压

}

/******************************************************************
 * @brief  handle enter low power mode event.
 * @param  none
 * @return none
 * @retval void
 */
void bat_handle_enter_low_power_event(void)
{
    /* double check battery mode */
    //PRINT("bat_handle_enter_low_power_event : %d\n", cur_bat_info.bat_mode);
    if (cur_bat_info.bat_mode == BAT_MODE_LOW_POWER)
    {
        APP_PRINT_INFO0("[bat_handle_enter_low_power_event] Enter low power mode");
#if SUPPORT_LED_INDICATION_FEATURE
#if FEATURE_SUPPORT_DEEP_SLEEP_STATE

#endif
#endif

        if (RCU_STATUS_ADVERTISING == app_global_data.rcu_status)
        {
            rcu_stop_adv(STOP_ADV_REASON_LOWPOWER);
        }
        else if ((RCU_STATUS_CONNECTED == app_global_data.rcu_status)
                 || (RCU_STATUS_PAIRED == app_global_data.rcu_status))
        {
            rcu_terminate_connection(DISCONN_REASON_LOW_POWER);
        }
        else
        {
            app_global_data.rcu_status = RCU_STATUS_LOW_POWER;
        }
    }
    else
    {
        APP_PRINT_WARN0("[bat_handle_enter_low_power_event] Invalid bat_mode");
    }
}

/******************************************************************
 * @brief  handle enter normal mode event.
 * @param  none
 * @return none
 * @retval void
 */
void bat_handle_enter_normal_event(void)
{
    /* double check battery mode */
    if (cur_bat_info.bat_mode == BAT_MODE_NORMAL)
    {
        APP_PRINT_INFO0("[bat_handle_enter_normal_event] Enter normal power mode");
#if SUPPORT_BAT_LPC_FEATURE
        bat_driver_lpc_init();
#endif
        if (RCU_STATUS_LOW_POWER == app_global_data.rcu_status)
        {
            app_global_data.rcu_status = RCU_STATUS_IDLE;
        }
        else
        {
            APP_PRINT_WARN0("[bat_handle_enter_normal_event] Invalid rcu_status");
        }
    }
    else
    {
        APP_PRINT_WARN0("[bat_handle_enter_normal_event] Invalid bat_mode");
    }
}

/******************************************************************
 * @brief  handle key pressed event.
 * @param  none
 * @return none
 * @retval void
 */
void bat_handle_key_pressed_event(void)
{
    if (cur_bat_info.bat_mode == BAT_MODE_NORMAL)
    {
#if SUPPORT_BAT_KEY_PRESS_DETECT_FEATURE
        cur_bat_info.bat_detect_index++;

        APP_PRINT_INFO1("[bat_handle_key_pressed_event] bat_detect_index is %d",
                        cur_bat_info.bat_detect_index);

        if (cur_bat_info.bat_detect_index >= BAT_DETECT_TRIGGER_CNT)
        {
            cur_bat_info.bat_detect_index = 0;
            bat_update_battery_info();
        }
#endif
    }
    else if (cur_bat_info.bat_mode == BAT_MODE_LOW_POWER)
    {
        bat_update_battery_info();
        if (cur_bat_info.bat_mode == BAT_MODE_LOW_POWER)
        {
#if SUPPORT_LED_INDICATION_FEATURE
#if FEATURE_SUPPORT_DEEP_SLEEP_STATE

#endif
#endif
        }
    }
}

#if SUPPORT_BAT_LPC_FEATURE
/******************************************************************
 * @brief  Initialize LPC driver.
 * @param  none
 * @return none
 */
void bat_driver_lpc_init(void)
{
    LPC_InitTypeDef LPC_InitStruct;
    LPC_StructInit(&LPC_InitStruct);
    LPC_InitStruct.LPC_Channel   = LPC_CHANNEL_VBAT ;
    LPC_InitStruct.LPC_Edge      = LPC_Vin_Below_Vth ;
    LPC_InitStruct.LPC_Threshold = BAT_LPC_COMP_VALUE;
    LPC_Init(&LPC_InitStruct);
    LPC_Cmd(ENABLE);
    RamVectorTableUpdate(LPCOMP_VECTORn, bat_lpc_handler);

    LPC_ResetCounter();
    LPC_SetCompValue(1);
    LPC_CounterCmd(ENABLE);
    LPC_ClearINTPendingBit(LPC_INT_LPCOMP_CNT);
    LPC_INTConfig(LPC_INT_LPCOMP_CNT, ENABLE);

    LPC_INTCmd(ENABLE);

    /* Config LPC interrupt */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = LPCOMP_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

/******************************************************************
 * @brief  LPC driver enter dlps config.
 * @param  none
 * @return none
 */
void bat_driver_lpc_enter_dlps_config(void)
{
    LPC_Cmd(DISABLE);

    LPC_ClearINTPendingBit(LPC_INT_LPCOMP_CNT);
    LPC_INTConfig(LPC_INT_LPCOMP_CNT, DISABLE);

    LPC_INTCmd(DISABLE);
}

/******************************************************************
 * @brief  LPC driver exit dlps config.
 * @param  none
 * @return none
 */
void bat_driver_lpc_exit_dlps_config(void)
{
    LPC_Cmd(ENABLE);

    LPC_ResetCounter();
    LPC_SetCompValue(1);
    LPC_CounterCmd(ENABLE);
    LPC_ClearINTPendingBit(LPC_INT_LPCOMP_CNT);
    LPC_INTConfig(LPC_INT_LPCOMP_CNT, ENABLE);

    LPC_INTCmd(ENABLE);
}

/******************************************************************
 * @brief  lpc interrupt handler function.
 * @param  none
 * @return none
 * @retval void
 */
void bat_lpc_handler(void)
{
    DBG_DIRECT("[bat_lpc_handler] LPC lower power triggered!");

    /* LPC counter comparator interrupt */
    if (LPC_GetFlagStatus(LPC_FLAG_LPCOMP_CNT) == SET)
    {
        LPC_INTConfig(LPC_INT_LPCOMP_CNT, DISABLE);
        LPC_INTCmd(DISABLE);
        DBG_DIRECT("[bat_lpc_handler] LPC lower power CNT triggered!");
        LPC_ResetCounter();

        LPC_ClearINTPendingBit(LPC_INT_LPCOMP_CNT);
        WDG_SystemReset(RESET_ALL_EXCEPT_AON, RESET_REASON_LPC_TRIGGER);
    }
}
#endif

#if SUPPORT_BAT_PERIODIC_DETECT_FEATURE
void bat_detect_timer_callback(TimerHandle_t pxTimer)
{
    APP_PRINT_ERROR0("[bat_detect_timer_callback] timeout");

    T_IO_MSG bee_io_msg;
    bee_io_msg.type = IO_MSG_TYPE_BAT_DETECT;
    bee_io_msg.subtype = IO_MSG_BAT_DETECT_TIMEOUT;
    if (false == app_send_msg_to_apptask(&bee_io_msg))
    {
        APP_PRINT_WARN0("[bat_detect_timer_callback] Send message failed");
    }

    os_timer_restart(&bat_detect_timer, BAT_PERIODIC_DETECT_TIMEOUT);
}
#endif

/*============================================================================*
 *                              Global functions
 *============================================================================*/
/******************************************************************
 * @brief    battery module enter DLPS config
 * @param    none
 * @return   none
 * @retval   void
 */
void bat_enter_dlps_config(void)
{
#if SUPPORT_BAT_LPC_FEATURE
    bat_driver_lpc_enter_dlps_config();
#endif
}

/******************************************************************
 * @brief    battery module exit DLPS config
 * @param    none
 * @return   none
 * @retval   void
 */
void bat_exit_dlps_config(void)
{
    bat_init_driver();

#if SUPPORT_BAT_LPC_FEATURE
    bat_driver_lpc_exit_dlps_config();
#endif
}

/******************************************************************
 * @brief    Initialize battery module data
 * @param    none
 * @return   none
 */
void bat_init_data(void)
{
    APP_PRINT_INFO0("[bat_init_data] init data");
    memset(&cur_bat_info, 0, sizeof(cur_bat_info));
    cur_bat_info.is_adc_efuse_existed = ADC_CalibrationInit();
    if (false == cur_bat_info.is_adc_efuse_existed)
    {
        APP_PRINT_WARN0("[bat_init_data] Read ADC efuse data error!");
    }
}

/******************************************************************
 * @brief  get battery mode.
 * @param  none
 * @return battery mode
 */
T_BAT_MODE bat_get_current_mode(void)
{
    return (T_BAT_MODE)cur_bat_info.bat_mode;
}

/******************************************************************
 * @brief  get battery voltage value.
 * @param  none
 * @return battery voltage value
 */
uint16_t bat_get_current_voltage_value(void)
{
    return cur_bat_info.bat_value;
}

/******************************************************************
 * @brief  get battery voltage level.
 * @param  none
 * @return battery voltage level
 */
uint8_t bat_get_current_voltage_level(void)
{
    return cur_bat_info.bat_level;
}

/******************************************************************
 * @brief  Initialize battery driver.
 * @param  none
 * @return none
 */
void bat_init_driver(void)
{
    Pad_Config(ADC_BAT_VET, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    
    ADC_DeInit(ADC);
    RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, ENABLE);

    ADC_InitTypeDef adcInitStruct;
    ADC_StructInit(&adcInitStruct);

    for (uint8_t index = 0; index < BAT_ADC_SAMPLE_CNT; index++)
    {
        adcInitStruct.ADC_SchIndex[index]   = EXT_SINGLE_ENDED(0);
    }
    adcInitStruct.ADC_Bitmap                = (1 << BAT_ADC_SAMPLE_CNT) - 1;
    adcInitStruct.ADC_SampleTime            = 255;

    ADC_Init(ADC, &adcInitStruct);

#if ADC_MODE_DIVIDE_OR_BYPASS
    ADC_BypassCmd(ADC_BAT_VET, ENABLE);
#endif

#if SUPPORT_BAT_PERIODIC_DETECT_FEATURE
    if (bat_detect_timer == NULL)
    {
        APP_PRINT_ERROR0("[bat_init_driver] initialize bat_detect_timer");
        /* bat_detect_timer is used to update battery info perodically */
        if (false == os_timer_create(&bat_detect_timer, "bat_detect_timer",  1, \
                                     BAT_PERIODIC_DETECT_TIMEOUT, false, bat_detect_timer_callback))
        {
            APP_PRINT_ERROR0("[bat_init_driver] timer creat failed!");
        }
        else
        {
            os_timer_start(&bat_detect_timer);
        }
    }
#endif
}

/******************************************************************
 * @brief  battery module nvic config
 * @param  none
 * @return none
 * @retval void
 */
void bat_nvic_config(void)
{
#if SUPPORT_BAT_LPC_FEATURE
    if (BAT_MODE_NORMAL == bat_get_current_mode())
    {
        APP_PRINT_INFO0("[bat_nvic_config] Init LPC");
        bat_driver_lpc_init();
    }
    else
    {
        APP_PRINT_INFO0("[bat_nvic_config] In low power mode. Don't init LPC");
    }
#endif
}


T_BAT_STATUS bat_pluse_value_info(void)
{

    T_BAT_STATUS status = BAT_STATUS_SUCCESS;
    uint32_t sum = 0;
    uint16_t adc_arr[BAT_ADC_SAMPLE_CNT];
    uint16_t min = 0xffff;
    uint16_t max = 0;

    ADC_INTConfig(ADC, ADC_INT_ONE_SHOT_DONE, ENABLE);
    ADC_Cmd(ADC, ADC_ONE_SHOT_MODE, ENABLE);

    uint32_t delay = 0;
    /* 5000 timeout: 1ms at 40M Clock */
    while ((ADC_GetINTStatus(ADC, ADC_INT_ONE_SHOT_DONE) != SET)
           && ((delay++ < 4000)));
    ADC_ClearINTPendingBit(ADC, ADC_INT_ONE_SHOT_DONE);

    /* get average value after remove minimal and maximal values */
    for (uint8_t index = 0; index < BAT_ADC_SAMPLE_CNT; index ++)
    {
        adc_arr[index] = ADC_ReadByScheduleIndex(ADC, index);

        sum += adc_arr[index];
        if (min > adc_arr[index])
        {
            min = adc_arr[index];
        }
        if (max < adc_arr[index])
        {
            max = adc_arr[index];
        }
    }
    sum = (sum - min - max) / (BAT_ADC_SAMPLE_CNT - 2);
    //PRINT("bat_adc_sample_average_value = %d \n", sum);

    /* calculate battery voltage value */
    cur_bat_info.bat_value = (uint16_t)bat_calibrate_voltage(sum);

    /* calculate battery level */
    cur_bat_info.bat_level = bat_calculate_bat_level(cur_bat_info.bat_value);
	

	return status;
}



/******************************************************************
 * @brief  get battery value.
 * @param  p_level
 * @param  p_value
 * @return T_BAT_STATUS - state
 */
T_BAT_STATUS bat_update_battery_info(void)
{

    T_BAT_STATUS status = BAT_STATUS_SUCCESS;
    uint32_t sum = 0;
    uint16_t adc_arr[BAT_ADC_SAMPLE_CNT];
    uint16_t min = 0xffff;
    uint16_t max = 0;

    ADC_INTConfig(ADC, ADC_INT_ONE_SHOT_DONE, ENABLE);
    ADC_Cmd(ADC, ADC_ONE_SHOT_MODE, ENABLE);

    uint32_t delay = 0;
    /* 5000 timeout: 1ms at 40M Clock */
    while ((ADC_GetINTStatus(ADC, ADC_INT_ONE_SHOT_DONE) != SET)
           && ((delay++ < 5000)));
    ADC_ClearINTPendingBit(ADC, ADC_INT_ONE_SHOT_DONE);

    /* get average value after remove minimal and maximal values */
    for (uint8_t index = 0; index < BAT_ADC_SAMPLE_CNT; index ++)
    {
        adc_arr[index] = ADC_ReadRawData(ADC, index);

        sum += adc_arr[index];
        if (min > adc_arr[index])
        {
            min = adc_arr[index];
        }
        if (max < adc_arr[index])
        {
            max = adc_arr[index];
        }
    }

    sum = (sum - min - max) / (BAT_ADC_SAMPLE_CNT - 2);
    APP_PRINT_WARN1("[bat_update_battery_info] bat_adc_sample_average_value = %d", sum);

    /* calculate battery voltage value */
    cur_bat_info.bat_value = (uint16_t)bat_calibrate_voltage(sum);

    /* calculate battery level */
    cur_bat_info.bat_level = bat_calculate_bat_level(cur_bat_info.bat_value);

    /* update battery mode */
    if (RCU_STATUS_LOW_POWER != app_global_data.rcu_status)
    {
        if (cur_bat_info.bat_value < BAT_ENTER_LOW_POWER_THRESHOLD)
        {
            cur_bat_info.bat_mode = BAT_MODE_LOW_POWER;

            T_IO_MSG bee_io_msg;
            bee_io_msg.type = IO_MSG_TYPE_BAT_DETECT;
            bee_io_msg.subtype = IO_MSG_BAT_DETECT_ENTER_LOW_POWER;
            if (false == app_send_msg_to_apptask(&bee_io_msg))
            {
                APP_PRINT_WARN0("[bat_update_battery_info] Send message failed");
                status = BAT_STATUS_SEND_MSG_FAIL;
            }
        }
    }
    else if (RCU_STATUS_LOW_POWER == app_global_data.rcu_status)
    {
        if (cur_bat_info.bat_value > BAT_ENTER_NORMAL_MODE_THRESHOLD)
        {
            cur_bat_info.bat_mode = BAT_MODE_NORMAL;

            T_IO_MSG bee_io_msg;
            bee_io_msg.type = IO_MSG_TYPE_BAT_DETECT;
            bee_io_msg.subtype = IO_MSG_BAT_DETECT_ENTER_NORMAL_MODE;
            if (false == app_send_msg_to_apptask(&bee_io_msg))
            {
                APP_PRINT_WARN0("[bat_update_battery_info] Send message failed");
                status = BAT_STATUS_SEND_MSG_FAIL;
            }
        }
    }

    APP_PRINT_INFO5("[bat_update_battery_info] bat_mode is %d, bat_value is %d, bat_level is %d, status is %d, rcu_status is %d",
                    cur_bat_info.bat_mode, cur_bat_info.bat_value, cur_bat_info.bat_level, status,
                    app_global_data.rcu_status);

    return status;
}

/******************************************************************
 * @brief  bat message handle.
 * @param  msg_sub_type - the msg type to handle.
 * @return none
 * @retval void
 */
void bat_msg_handle(uint16_t msg_sub_type)
{
    APP_PRINT_ERROR1("[bat_msg_handle] bat_msg_handle, type = %d!", msg_sub_type);

    if (msg_sub_type == IO_MSG_BAT_DETECT_ENTER_LOW_POWER)
    {
        bat_handle_enter_low_power_event();
    }
    else if (msg_sub_type == IO_MSG_BAT_DETECT_ENTER_NORMAL_MODE)
    {
        bat_handle_enter_normal_event();
    }
    else if (msg_sub_type == IO_MSG_BAT_DETECT_KEY_PRESSED)
    {
        bat_handle_key_pressed_event();
    }
    else if (msg_sub_type == IO_MSG_BAT_DETECT_TIMEOUT)
    {
        bat_update_battery_info();
    }
    else
    {
        APP_PRINT_WARN0("[bat_msg_handle] Invalid message type");
    }
}
#endif

/******************* (C) COPYRIGHT 2020 Realtek Semiconductor Corporation *****END OF FILE****/

