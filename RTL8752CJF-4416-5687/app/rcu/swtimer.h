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
#define ADV_UNDIRECT_RECONNECT_TIMEOUT 5000  /* 5s */
#define ADV_UNDIRECT_DISCON_RECONNECT_TIMEOUT 30000  /* 30s */
#define ADV_UNDIRECT_POWER_TIMEOUT 5000  /* 5s */
#define ADV_UNDIRECT_PAIRING_TIMEOUT 30000  /* 30s */
#define ADV_UNDIRECT_PROMPT_TIMEOUT 5000  /* 5s */
#define PAIRING_EXCEPTION_TIMEOUT 10000  /* 10s */
#define NO_ACTION_DISCON_TIMEOUT 300000  /* 300s */
#define UPDATE_CONN_PARAMS_TIMEOUT 5000  /* 5s */
#define DFU_PROCESS_WAIT_TIMEOUT   5000  /* 5s */
#define PAIR_FAIL_DISCONN_TIMEOUT  2000  /* 2s */

#define SINGLE_PAIR_ADV_TIMEOUT    20  /* 20ms */
#define VOICE_OVERTIME_LED_OFF_TIMEOUT    15000  /* 15s */
#define PAIR_SUCCESS_TIMEOUT  2500        /* 2.5s */
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
extern TimerHandle_t next_state_time_out;
extern TimerHandle_t single_pair_adv_time_out;
extern TimerHandle_t voice_overtime_led_off_timer;
extern TimerHandle_t pair_success_led_exit_timer;
/*============================================================================*
 *                         Functions
 *============================================================================*/
void sw_timer_init(void);

#ifdef __cplusplus
}
#endif

#endif /*_SWTIMER__*/

/******************* (C) COPYRIGHT 2017 Realtek Semiconductor Corporation *****END OF FILE****/
