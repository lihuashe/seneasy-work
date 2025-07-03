#include "os_timer.h"
#include <trace.h>
#include "board.h"
#include "swtimer.h"
#include "rcu_gap.h"
#include "rcu_application.h"
#include "key_handle.h"
#include "keyscan_driver.h"
#include "gap_conn_le.h"
#include "gap_bond_le.h"
#include "hids_rmc.h"
#include "rtl876x_keyscan.h"
#include "rtl876x_pinmux.h"
#include "single_tone.h"
#if RCU_VOICE_EN
#include "voice.h"
#include "voice_driver.h"
#endif

#if IR_FUN
#include "ir_app_config.h"
#endif

#include "mem_config.h"
#if SCENARIO_SWITCH_EN
#include "overlay_mgr.h"
#endif
#include "app_section.h"
#include "led_driver.h"

/*============================================================================*
 *                              Global Variables
 *============================================================================*/
TimerHandle_t adv_timer;
#if FEATURE_SUPPORT_NO_ACTION_DISCONN
TimerHandle_t no_act_disconn_timer;
#endif
TimerHandle_t update_conn_params_timer;
TimerHandle_t next_state_time_out;
TimerHandle_t single_pair_adv_time_out;
TimerHandle_t voice_overtime_led_off_timer;
TimerHandle_t pair_success_led_exit_timer;
/*============================================================================*
 *                              Loacal Functions
 *============================================================================*/
static void adv_timer_callback(TimerHandle_t p_timer);
static void next_state_timeout_timer_callback(TimerHandle_t p_timer);
#if FEATURE_SUPPORT_NO_ACTION_DISCONN
static void no_act_disconn_timer_callback(TimerHandle_t p_timer);
#endif
static void update_conn_params_timer_cb(TimerHandle_t p_timer);

/**
 * @brief advertising timer callback
 *
 * adv_timer_callback is used to stop advertising after timeout
 *
 * @param p_timer - timer handler
 * @return none
 * @retval void
 */
void adv_timer_callback(TimerHandle_t p_timer) DATA_RAM_FUNCTION;
void adv_timer_callback(TimerHandle_t p_timer)
{
    if (app_global_data.rcu_status == RCU_STATUS_ADVERTISING)
    {
        if (app_global_data.adv_type != ADV_DIRECT_HDC)
        {
            rcu_stop_adv(STOP_ADV_REASON_TIMEOUT);
            LED_BLINK_EXIT(LED_1, LED_TYPE_BLINK_PAIR_ADV);
            LED_OFF(LED_1);
        }
    }
}
void pair_success_led_exit_callback(TimerHandle_t p_timer) DATA_RAM_FUNCTION;
void pair_success_led_exit_callback(TimerHandle_t p_timer)
{
    APP_PRINT_INFO0("[pair_success_led_exit_callback] timeout");
    LED_BLINK_EXIT(LED_1, LED_TYPE_BLINK_PAIR_ADV);
    LED_OFF(LED_1);        
}
void voice_overtime_led_off_timer_callback(TimerHandle_t p_timer) DATA_RAM_FUNCTION;
void voice_overtime_led_off_timer_callback(TimerHandle_t p_timer)
{
    voice_handle_atvv_audio_stop(ATV_STOP_REASON_AUDIO_TRANSFER_TIMEOUT);
    LED_OFF(LED_1);
}

void single_pair_adv_timeout_timer_callback(TimerHandle_t p_timer) DATA_RAM_FUNCTION;
void single_pair_adv_timeout_timer_callback(TimerHandle_t p_timer)
{
    static uint16_t key_MM_single_pressed_count = 0;
    key_MM_single_pressed_count ++;
    if(key_MM_single_pressed_count < 100 && keyscan_global_data.is_all_key_released == true){
        key_MM_single_pressed_count = 0;
        os_timer_stop(&single_pair_adv_time_out);
        APP_PRINT_INFO0("[single_pair_adv_timeout_timer_callback] key_MM_single_pressed_count<2s");
        rcu_stop_adv(STOP_ADV_REASON_TIMEOUT);
        APP_PRINT_INFO0("[SINGLE_PAIR_ADV_TIMEOUT]");
        WDG_SystemReset(RESET_ALL, SINGLE_TONE_TIMEOUT_RESET);
    }  
    if(key_MM_single_pressed_count == 100 && keyscan_global_data.is_all_key_released != true){
        LED_ON(LED_1);
        single_tone_init();
        APP_PRINT_INFO0("[single_pair_adv_timeout_timer_callback] LED_ON");
    }
    if(key_MM_single_pressed_count > 100 && keyscan_global_data.is_all_key_released == true){
        single_tone_timeout_start();
        os_timer_stop(&single_pair_adv_time_out);
    }


}
/**
 * @brief pair failed disconnection timer callback
 *
 * pair_fail_disconn_timer_callback is used to disconnect for pair failed
 *
 * @param p_timer - timer handler
 * @return none
 * @retval void
 */
void next_state_timeout_timer_callback(TimerHandle_t p_timer) DATA_RAM_FUNCTION;
void next_state_timeout_timer_callback(TimerHandle_t p_timer)
{
    if (app_global_data.rcu_status == RCU_STATUS_CONNECTED)
    {
        APP_PRINT_INFO0("[pair_fail_disconn_timer_callback] timeout");
        rcu_terminate_connection(DISCONN_REASON_PAIR_FAILED);
    }
}

/**
 * @brief update connection parameters timer callback
 *
 * update_conn_params_timer is used to update desired connection parameters after timeout
 *
 * @param p_timer - timer handler
 * @return none
 * @retval void
 */
void update_conn_params_timer_cb(TimerHandle_t p_timer) DATA_RAM_FUNCTION;
void update_conn_params_timer_cb(TimerHandle_t p_timer)
{
    if (app_global_data.rcu_status == RCU_STATUS_PAIRED)
    {
#if RCU_VOICE_EN
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

#if FEATURE_SUPPORT_NO_ACTION_DISCONN
/**
 * @brief no action disconnect timer callback
 *
 * no_act_disconn_timer_callback is used to terminate connection after timeout
 *
 * @param p_timer - timer handler
 * @return none
 * @retval void
 */
void no_act_disconn_timer_callback(TimerHandle_t p_timer) DATA_RAM_FUNCTION;
void no_act_disconn_timer_callback(TimerHandle_t p_timer)
{
    if (app_global_data.rcu_status == RCU_STATUS_PAIRED)
    {
        APP_PRINT_INFO0("[RCU] Idle No Action Timeout, Disconnect.");
        rcu_terminate_connection(DISCONN_REASON_TIMEOUT);
    }
}
#endif

/*============================================================================*
 *                              Global Functions
 *============================================================================*/
void sw_timer_init()
{
    /* adv_timer is used to stop advertising after timeout */
    if (false == os_timer_create(&adv_timer, "adv_timer",  1, \
                                 ADV_UNDIRECT_PAIRING_TIMEOUT, false, adv_timer_callback))
    {
        APP_PRINT_INFO0("[sw_timer_init] init adv_timer failed");
    }

    /* pair_fail_disconn_timer is used to disconnect for pair failed */
    if (false == os_timer_create(&next_state_time_out, "pair_fail_disconn_timer",  1, \
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

    /* update_conn_params_timer is used to update desired connection parameters after timeout */
    if (false == os_timer_create(&update_conn_params_timer, "update_conn_params_timer", 1
                                 , UPDATE_CONN_PARAMS_TIMEOUT, false, update_conn_params_timer_cb))
    {
        APP_PRINT_INFO0("[sw_timer_init] init update_conn_params_timer failed");
    }
    if (false == os_timer_create(&single_pair_adv_time_out, "single_pair_adv_time_out", 1
                                 , SINGLE_PAIR_ADV_TIMEOUT, true, single_pair_adv_timeout_timer_callback))
    {
        APP_PRINT_INFO0("[sw_timer_init] init single_pair_adv_time_out failed");
    }
    if (false == os_timer_create(&voice_overtime_led_off_timer, "voice_overtime_led_off_timer", 1
                                 , VOICE_OVERTIME_LED_OFF_TIMEOUT, false, voice_overtime_led_off_timer_callback))
    {
        APP_PRINT_INFO0("[sw_timer_init] init voice_overtime_led_off_timer failed");
    }
    if (false == os_timer_create(&pair_success_led_exit_timer, "pair_success_led_exit_timer",  1, \
                                 PAIR_SUCCESS_TIMEOUT, false, pair_success_led_exit_callback))
    {
        APP_PRINT_INFO0("[sw_timer_init] init pair_success_led_exit_timer failed");
    }
    keyscan_init_timer();

    key_handle_init_timer();

// #if IR_LEARN_MODE
//     ir_learn_init_timer();
// #endif

}
