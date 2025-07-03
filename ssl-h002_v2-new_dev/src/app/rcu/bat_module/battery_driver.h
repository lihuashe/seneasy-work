/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     battery_driver.h
* @details
* @author   chenjie_jin
* @date     2020-05-11
* @version  v1.1
*********************************************************************************************************
*/

#ifndef _BAT_DRIVER_H_
#define _BAT_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <board.h>

#if SUPPORT_BAT_DETECT_FEATURE
/*============================================================================*
 *                              Macro Definitions
 *============================================================================*/
#define BAT_ADC_SAMPLE_CNT       8   /* battery adc samples counts, ragne 3-16 */
#if SUPPORT_BAT_KEY_PRESS_DETECT_FEATURE
/* battery adc detection trigger count */
#if FEATURE_SUPPORT_DEEP_SLEEP_STATE
#define BAT_DETECT_TRIGGER_CNT   1   //this value must be 1 if dss is enabled.
#else
#define BAT_DETECT_TRIGGER_CNT   10  //this value can be changed if dss is disabled.
#endif
#endif
#if SUPPORT_BAT_PERIODIC_DETECT_FEATURE
#define BAT_PERIODIC_DETECT_TIMEOUT  (600 * 1000)  /* timeout in unit 1ms */
#endif


#define ADC_DIVIDE_MODE             0
#define ADC_BYPASS_MODE             1
#define ADC_MODE_DIVIDE_OR_BYPASS   ADC_DIVIDE_MODE

#define ADC_BAT_VET                 P2_2
/*============================================================================*
 *                              Types
 *============================================================================*/
typedef enum
{
    BAT_STATUS_SUCCESS = 0,
    BAT_STATUS_FAIL = 1,
    BAT_STATUS_PENDING = 2,
    BAT_STATUS_SEND_MSG_FAIL = 3,
} T_BAT_STATUS;

typedef enum
{
    BAT_MODE_NORMAL = 0,      /* battery normal mode */
    BAT_MODE_LOW_POWER = 1,   /* battery low power mode */
} T_BAT_MODE;

typedef enum
{
    IO_MSG_BAT_DETECT_ENTER_LOW_POWER    = 0, /* enter battery low power mode event */
    IO_MSG_BAT_DETECT_ENTER_NORMAL_MODE  = 1, /* enter battery low power mode event */
    IO_MSG_BAT_DETECT_LPC                = 2, /* LPC trigger event */
    IO_MSG_BAT_DETECT_KEY_PRESSED        = 3, /* key pressed battery event */
    IO_MSG_BAT_DETECT_TIMEOUT            = 4, /* battery timeout event */
} T_IO_MSG_BAT_DETECT;

typedef struct
{
    bool is_adc_efuse_existed;  /* to indicate whether adc efuse data is right or not */
    T_BAT_MODE bat_mode;  /* current battery mode */
    uint8_t bat_detect_index; /* current battery detect index */
    uint8_t bat_level;  /* battery level, range 0-100 */
    uint16_t bat_value;  /* battery value in uint mV */
} T_BAT_INFO_STRUCT;

/*============================================================================*
 *                       Interface Functions
 *============================================================================*/
void bat_init_data(void);
T_BAT_MODE bat_get_current_mode(void);
uint16_t bat_get_current_voltage_value(void);
uint8_t bat_get_current_voltage_level(void);
void bat_init_driver(void);
void bat_nvic_config(void);
T_BAT_STATUS bat_pluse_value_info(void);
T_BAT_STATUS bat_update_battery_info(void);
void bat_msg_handle(uint16_t msg_sub_type);
void bat_enter_dlps_config(void);
void bat_exit_dlps_config(void);

#ifdef __cplusplus
}
#endif

#endif

#endif

/******************* (C) COPYRIGHT 2020 Realtek Semiconductor Corporation *****END OF FILE****/

