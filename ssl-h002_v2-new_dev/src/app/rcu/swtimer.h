/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      swtimer.h
* @brief     header file of software timer implementation
* @details
* @author    KEN_MEI
* @date      2017-02-08
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef _SWTIMER__
#define _SWTIMER__

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <board.h>

/*============================================================================*
 *                              Macros
 *============================================================================*/
#define ADV_UNDIRECT_RECONNECT_FIRST_TIMEOUT 10000  /* 10s */
#define ADV_UNDIRECT_RECONNECT_TIMEOUT 3000  /* 3s */
#define ADV_UNDIRECT_POWER_TIMEOUT 12000  /* 12s */
#define ADV_UNDIRECT_PAIRING_TIMEOUT 30000  /* 30s */
#define ADV_UNDIRECT_PROMPT_TIMEOUT 5000  /* 5s */
#define PAIRING_EXCEPTION_TIMEOUT 10000  /* 10s */
#define NO_ACTION_DISCON_TIMEOUT 300000  /* 300s */
#define UPDATE_CONN_PARAMS_TIMEOUT 4000  /* 5s */
#define UPDATE_CONN_PARAMS_SHORT_TIMEOUT 4000  /* 1s */
#define DFU_PROCESS_WAIT_TIMEOUT   5000  /* 5s */
#define PAIR_FAIL_DISCONN_TIMEOUT  3000  /* 2s */
#if FEATURE_SUPPORT_DEEP_SLEEP_STATE
#define ENTER_DSS_TIMEOUT 3000  /* 3s */
#endif
/*============================================================================*
 *                         Types
 *============================================================================*/
typedef void *TimerHandle_t;

/*============================================================================*
*                        Export Global Variables
*============================================================================*/
extern TimerHandle_t adv_timer;
#if FEATURE_SUPPORT_NO_ACTION_DISCONN
extern TimerHandle_t no_act_disconn_timer;
#endif
extern TimerHandle_t update_conn_params_timer;
extern TimerHandle_t next_state_check_timer;
extern TimerHandle_t power_auto_off_timer;
extern TimerHandle_t buzzer_repeat_timer; //蜂鸣器重复开启定时器
extern TimerHandle_t production_hv_lv_aging_timer;
extern TimerHandle_t bat_check_timer;
extern TimerHandle_t led_tick_timer;
#if FEATURE_SUPPORT_DEEP_SLEEP_STATE
extern TimerHandle_t enter_dss_timer;
#endif
/*============================================================================*
 *                         Functions
 *============================================================================*/
void sw_timer_init(void);

#ifdef __cplusplus
}
#endif

#endif /*_SWTIMER__*/

/******************* (C) COPYRIGHT 2017 Realtek Semiconductor Corporation *****END OF FILE****/
