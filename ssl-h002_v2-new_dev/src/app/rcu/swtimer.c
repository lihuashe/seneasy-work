/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     swtimer.c
* @brief    software timers source file.
* @details  include initialization and callback funcitions of software timers
* @author
* @date     2020-02-24
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "os_timer.h"
#include <trace.h>
#include "board.h"
#include "swtimer.h"
#include "rcu_gap.h"
#include "rcu_application.h"
#include "key_handle.h"
#include "keyscan_driver.h"
#include "gap_bond_le.h"
#include "rtl876x_keyscan.h"
#include "mem_config.h"
#include "app_section.h"
#include "buzzer.h"
#include "uart_driver.h"
#include "app_function.h"
#include "production_test.h"
#include "led_config.h"

#if SUPPORT_VOICE_FEATURE
#include "voice_driver.h"
#endif
#if SCENARIO_SWITCH_EN
#include "overlay_mgr.h"
#endif
#if (AON_WDG_ENABLE == 1)
#include <app_task.h>
#if FEATURE_SUPPORT_DEEP_SLEEP_STATE
#include "rtl876x_aon_wdg.h"
#endif
#endif
#if FEATURE_SUPPORT_DEEP_SLEEP_STATE
#include "dlps.h"
#if (SUPPORT_IR_TX_FEATURE && SUPPORT_IR_LEARN_FEATURE)
#include "ir_learn_handle.h"
#endif
#endif

/*============================================================================*
 *                              Global Variables
 *============================================================================*/
TimerHandle_t adv_timer;
#if FEATURE_SUPPORT_NO_ACTION_DISCONN
TimerHandle_t no_act_disconn_timer;
#endif
TimerHandle_t update_conn_params_timer;
TimerHandle_t next_state_check_timer;
TimerHandle_t buzzer_repeat_timer; //蜂鸣器重复开启定时器
TimerHandle_t power_auto_off_timer;
TimerHandle_t production_hv_lv_aging_timer;
TimerHandle_t bat_check_timer;
TimerHandle_t led_tick_timer;
#if (AON_WDG_ENABLE == 1)
void *aon_watch_dog_wake_up_dlps_timer;
#define AON_WDG_TIMER_WAKEUP_DLPS_PERIOD     ((AON_WDG_TIME_OUT_PERIOD_SECOND - 1) * 1000)
#endif
#if FEATURE_SUPPORT_DEEP_SLEEP_STATE
TimerHandle_t enter_dss_timer;
#endif
/*============================================================================*
 *                              Functions Declaration
 *============================================================================*/
static void adv_timer_callback(TimerHandle_t p_timer) DATA_RAM_FUNCTION;
static void next_state_timeout_timer_callback(TimerHandle_t p_timer) DATA_RAM_FUNCTION;
static void update_conn_params_timer_cb(TimerHandle_t p_timer) DATA_RAM_FUNCTION;
#if FEATURE_SUPPORT_NO_ACTION_DISCONN
static void no_act_disconn_timer_callback(TimerHandle_t p_timer) DATA_RAM_FUNCTION;
#endif
#if (AON_WDG_ENABLE == 1)
static void aon_watch_dog_wake_up_dlps_callback(TimerHandle_t p_timer) DATA_RAM_FUNCTION;
#endif
#if FEATURE_SUPPORT_DEEP_SLEEP_STATE
static void enter_dss_timer_cb(TimerHandle_t p_timer) DATA_RAM_FUNCTION;
#endif
/*============================================================================*
 *                              Loacal Functions
 *============================================================================*/
/******************************************************************
 * @brief advertising timer callback
 *
 * adv_timer_callback is used to stop advertising after timeout
 *
 * @param p_timer - timer handler
 * @return none
 * @retval void
 */
void adv_timer_callback(TimerHandle_t p_timer)
{
    if (app_global_data.rcu_status == RCU_STATUS_ADVERTISING)
    {
        if (app_global_data.adv_type != ADV_DIRECT_HDC)
        {
            rcu_stop_adv(STOP_ADV_REASON_TIMEOUT);
            led_system_ctrl(LED_BRIGHT_STOP);//关灯，关闭一些动作
            
            led_system_ctrl(LED_NORMAL_WORK);
            voltage_mode_gear_change(g_beauty_comb.mode, g_beauty_comb.gear);
        }
    }
}

/******************************************************************
 * @brief pair failed disconnection timer callback
 *
 * pair_fail_disconn_timer_callback is used to disconnect for pair failed
 *
 * @param p_timer - timer handler
 * @return none
 * @retval void
 */
void next_state_timeout_timer_callback(TimerHandle_t p_timer)
{
    PRINT("next_state_timeout_timer_callbac\n");
    if (app_global_data.rcu_status == RCU_STATUS_CONNECTED)
    {
        APP_PRINT_INFO0("[pair_fail_disconn_timer_callback] timeout");
        rcu_terminate_connection(DISCONN_REASON_PAIR_FAILED);
    }
}

/******************************************************************
 * @brief update connection parameters timer callback
 *
 * update_conn_params_timer is used to update desired connection parameters after timeout
 *
 * @param p_timer - timer handler
 * @return none
 * @retval void
 */
void update_conn_params_timer_cb(TimerHandle_t p_timer)
{
    PRINT("update_conn_params_timer_cb\n");
    if (app_global_data.rcu_status == RCU_STATUS_PAIRED)
    {
#if SUPPORT_VOICE_FEATURE
        if (voice_driver_global_data.is_voice_driver_working == true)
        {
            /* voice module is working, start timer to attempt later */
            os_timer_restart(&update_conn_params_timer, UPDATE_CONN_PARAMS_TIMEOUT);
        }
        else
#endif
        {
            rcu_update_conn_params(RCU_CONNECT_INTERVAL, RCU_CONNECT_LATENCY,
                                   RCU_SUPERVISION_TIMEOUT);
            app_set_latency_status(LATENCY_SYS_UPDATE_BIT, LANTENCY_ON);  /* turn on latency */
        }
    }
    else
    {
        APP_PRINT_WARN1("[update_conn_params_timer_cb] Invalid rcu status: %d", app_global_data.rcu_status);
    }
}

#if FEATURE_SUPPORT_DEEP_SLEEP_STATE
/******************************************************************
 * @brief enter dss timer callback
 *
 * enter_dss_timer_cb is used to enter dss after timeout
 *
 * @param p_timer - timer handler
 * @return none
 * @retval void
 */
void enter_dss_timer_cb(TimerHandle_t p_timer)
{
    APP_PRINT_INFO0("[enter_dss_timer_cb]");

    if (((RCU_STATUS_IDLE != app_global_data.rcu_status) &&
         (RCU_STATUS_LOW_POWER != app_global_data.rcu_status))
        || (false == keyscan_global_data.is_all_key_released)
#if (SUPPORT_IR_TX_FEATURE && SUPPORT_IR_LEARN_FEATURE)
        || (IR_LEARN_IDLE != ir_learn_get_current_state())
#endif
       )
    {
        APP_PRINT_INFO2("[enter_dss_timer_cb] rcu status is %d.is_all_key_released is %d. can not enter dss",
                        app_global_data.rcu_status, keyscan_global_data.is_all_key_released);
#if (SUPPORT_IR_TX_FEATURE && SUPPORT_IR_LEARN_FEATURE)
        APP_PRINT_INFO1("[enter_dss_timer_cb] ir_learn_state = %d", ir_learn_get_current_state());
#endif

        os_timer_restart(&enter_dss_timer, ENTER_DSS_TIMEOUT);
        APP_PRINT_INFO0("[enter_dss_timer_cb] restart enter_dss_timer");
        return;
    }

#if (AON_WDG_ENABLE == 1)
    AON_WDG_Disable();
#endif
    platform_pm_stop_all_non_excluded_timer();
    lps_mode_set(PLATFORM_POWERDOWN);
    allow_to_enter_dss_flag = true;
}
#endif

#if FEATURE_SUPPORT_NO_ACTION_DISCONN
/******************************************************************
 * @brief no action disconnect timer callback
 *
 * no_act_disconn_timer_callback is used to terminate connection after timeout
 *
 * @param p_timer - timer handler
 * @return none
 * @retval void
 */
void no_act_disconn_timer_callback(TimerHandle_t p_timer)
{
    if (app_global_data.rcu_status == RCU_STATUS_PAIRED)
    {
        APP_PRINT_INFO0("[RCU] Idle No Action Timeout, Disconnect.");
        rcu_terminate_connection(DISCONN_REASON_TIMEOUT);
    }
}
#endif

#if (AON_WDG_ENABLE == 1)
void aon_watch_dog_wake_up_dlps_callback(TimerHandle_t pxTimer)
{
    //send message to app task in which reset the aon watch dog timer
    T_IO_MSG bee_io_msg = {0};
    bee_io_msg.type = IO_MSG_TYPE_RESET_AON_WDG_TIMER;
    if (false == app_send_msg_to_apptask(&bee_io_msg))
    {
        APP_PRINT_ERROR0("[WDG] send IO_MSG_TYPE_RESET_AON_WDG_TIMER message failed!");
    }
}
#endif

/*============================================================================*
 *                              Global Functions
 *============================================================================*/
/******************************************************************
 * @brief software timer init function
 * @param  none
 * @return none
 * @retval void
 */
void sw_timer_init(void)
{

    /* adv_timer is used to stop advertising after timeout */
    if (false == os_timer_create(&adv_timer, "adv_timer",  1, \
                                 ADV_UNDIRECT_PAIRING_TIMEOUT, false, adv_timer_callback))
    {
        APP_PRINT_INFO0("[sw_timer_init] init adv_timer failed");
    }

    /* pair_fail_disconn_timer is used to disconnect for pair failed */
    if (false == os_timer_create(&next_state_check_timer, "pair_fail_disconn_timer",  1, \
                                 PAIR_FAIL_DISCONN_TIMEOUT, false, next_state_timeout_timer_callback))
    {
        APP_PRINT_INFO0("[sw_timer_init] init pairing_exception_timer failed");
    }

#if FEATURE_SUPPORT_NO_ACTION_DISCONN
    /* no_act_disconn_timer is used to disconnect after timeout if there is on action under connection */
    if (false == os_timer_create(&no_act_disconn_timer, "no_act_disconn_timer",  1, \
                                 NO_ACTION_DISCON_TIMEOUT, false, no_act_disconn_timer_callback))
    {
        APP_PRINT_INFO0("[sw_timer_init] init no_act_disconn_timer failed");
    }
#endif

#if (AON_WDG_ENABLE == 1)
    if (false == os_timer_create(&aon_watch_dog_wake_up_dlps_timer, "aon_watch_dog_wake_up_dlps_timer",
                                 1, \
                                 AON_WDG_TIMER_WAKEUP_DLPS_PERIOD, true, aon_watch_dog_wake_up_dlps_callback))
    {
        APP_PRINT_INFO0("[sw_timer_init] init aon_watch_dog_wake_up_dlps_callback failed");
    }
    else
    {
        os_timer_start(&aon_watch_dog_wake_up_dlps_timer);
        APP_PRINT_INFO0("Start aon_watch_dog_wake_up_dlps_callback!");
    }
#endif

    /* update_conn_params_timer is used to update desired connection parameters after timeout */
    if (false == os_timer_create(&update_conn_params_timer, "update_conn_params_timer", 1
                                 , UPDATE_CONN_PARAMS_TIMEOUT, false, update_conn_params_timer_cb))
    {
        APP_PRINT_INFO0("[sw_timer_init] init update_conn_params_timer failed");
    }

#if FEATURE_SUPPORT_DEEP_SLEEP_STATE
    /* enter_dss_timer is used to update enter dss after timeout */
    if (false == os_timer_create(&enter_dss_timer, "enter_dss_timer", 1
                                 , ENTER_DSS_TIMEOUT, false, enter_dss_timer_cb))
    {
        APP_PRINT_INFO0("[sw_timer_init] initenter_dss_timer failed");
    }
    else
    {
        os_timer_restart(&enter_dss_timer, ENTER_DSS_TIMEOUT);
        APP_PRINT_INFO0("[sw_timer_init] restart enter_dss_timer");
    }
#endif

    if (false == os_timer_create(&buzzer_repeat_timer, "buzzer_adv_exit_timer",  1, \
                                 BUZZER_REPEAT_RUN_TIME, true, buzzer_repeat_timer_cb))  // 创建反复开启蜂鸣器定时器
    {
        APP_PRINT_INFO0("[pwm_init_driver] init buzzer_adv_exit_timer failed");
    }

    if (false == os_timer_create(&power_auto_off_timer, "power_auto_off_timer",  1, \
                                 POWER_AUTO_OFF_TIME, true, power_auto_off_timer_cb))  // 创建定时关机
    {
        APP_PRINT_INFO0("[power_auto_off_timer] init power_auto_off_timer failed");
    }

    if (false == os_timer_create(&production_hv_lv_aging_timer, "production_aging_test_timer",  1, \
                                 POWER_AUTO_OFF_TIME, true, production_aging_test_timer_cb))  // 创建产测测试时间
    {
        APP_PRINT_INFO0("[production_aging_test] init production_aging_test_timer failed");
    }

    if (false == os_timer_create(&bat_check_timer, "bat_check_timer",  1, \
                                 BAT_STATUS_CHECK_TIME, true, bat_chg_status_check_timer_cb))  // 创建充电过程中定时检测
    {
        APP_PRINT_INFO0("[bat_check] init bat_check_timer failed" );
    }

    if (false == os_timer_create(&led_tick_timer, "chg_led_tick_timer",  1, \
                                 TIMER_LED_SET_TIME, true, led_ctrl_timer_cb))  // 创建充电过程中定时呼吸红灯
    {
        APP_PRINT_INFO0("[led_tick] init chg_led_tick_timer failed" );
    }
    
}

/******************* (C) COPYRIGHT 2020 Realtek Semiconductor Corporation *****END OF FILE****/
