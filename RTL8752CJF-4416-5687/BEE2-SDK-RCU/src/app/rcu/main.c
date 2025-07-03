/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    This is the entry of user code which the main function resides in.
* @details
* @author   chenjie_jin
* @date     2018-04-12
* @version  v0.3
*********************************************************************************************************
*/
#include <stdlib.h>
#include <board.h>
#include <os_sched.h>
#include <string.h>
#include <trace.h>
#include <gap.h>
#include <gap_bond_le.h>
#include <profile_server.h>
#include <rcu_application.h>
#include <dlps.h>
#include <rtl876x_keyscan.h>
#include <rtl876x_io_dlps.h>
#include <keyscan_driver.h>
#include <app_task.h>
#include <swtimer.h>
#include <os_timer.h>
#include <bas.h>
#include <dis.h>
#include <hids_rmc.h>
#include <vendor_service.h>
#include <rcu_ota_service.h>
#include <rcu_gap.h>
#include <app_section.h>
#if SUPPORT_SILENT_OTA
#include <rcu_dfu_service.h>
#endif
#if RCU_VOICE_EN
#include <voice.h>
#endif
#if IR_FUN
#include <ir_app_config.h>
#include <ir_learn_config.h>
#endif
#include "test_mode.h"
#if MP_TEST_MODE_SUPPORT_SINGLE_TONE_TEST
#include <single_tone.h>
#endif

#if FEATURE_SUPPORT_MP_TEST_MODE
#include <mp_test.h>
#include <ftl.h>
#include <data_uart_test.h>
#endif
#include <mem_config.h>
#if SCENARIO_SWITCH_EN
#include <app_section.h>
#include <overlay_mgr.h>
#endif
#include <rtl876x_pinmux.h>
#include <key_handle.h>
#include <voice_driver.h>
#if (VOICE_FLOW_SEL == ATV_GOOGLE_VOICE_FLOW)
#include <atvv_service.h>
#elif (VOICE_FLOW_SEL == RTK_GATT_VOICE_FLOW)
#include <voice_service.h>
#endif

#include "battery_driver.h"
#include "led_driver.h"
#include "rtc_driver.h"

/*============================================================================*
 *                         Macros
 *============================================================================*/
#define app_system_on_interrupt_handler System_Handler

/*============================================================================*
 *                              Global Variables
 *============================================================================*/

/*============================================================================*
*                              Global Functions
*============================================================================*/
/**
* @brief  global_data_init() contains the initialization of global data.
*/
void global_data_init(void)
{
    app_init_global_data();
    keyscan_init_data();
    key_handle_init_data();
#if RCU_VOICE_EN
    voice_driver_init_data();
    voice_driver_init_codec_params();
#if (VOICE_FLOW_SEL == ATV_GOOGLE_VOICE_FLOW)
    voice_handle_atvv_init_data();
#endif
#endif
}

/******************************************************************
 * @brief  app_pinmux_config() contains the initialization of app pinmux config.
 */
void app_pinmux_config(void)
{
    /*only when in normal mode and fast pair mode, keyscan can be config*/
    if ((app_global_data.test_mode == NOT_TEST_MODE)
        || (app_global_data.test_mode == AUTO_PAIR_WITH_FIX_ADDR_MODE))
    {
        keyscan_pinmux_config();
    }
}

/******************************************************************
 * @brief  app_pad_config() contains the initialization of app pad config.
 */
void app_pad_config(void)
{
    /*only when in normal mode and fast pair mode, keyscan can be config*/
    if ((app_global_data.test_mode == NOT_TEST_MODE)
        || (app_global_data.test_mode == AUTO_PAIR_WITH_FIX_ADDR_MODE))
    {
        keyscan_init_pad_config();
    }
}

/******************************************************************
 * @brief  app_nvic_config() contains the initialization of app NVIC config.
 */
void app_nvic_config(void)
{
    keyscan_nvic_config();
    rcu_bat_nvic_config();
    rtc_driver_init();
}

/**
* @brief    Board_Init() contains the initialization of pinmux settings and pad settings.
*
*               All the pinmux settings and pad settings shall be initiated in this function.
*               But if legacy driver is used, the initialization of pinmux setting and pad setting
*               should be peformed with the IO initializing.
*
* @return  void
*/
void board_init(void)
{
    app_pinmux_config();
    app_pad_config();
}

/**
* @brief    driver_init() contains the initialization of peripherals.
*
*               Both new architecture driver and legacy driver initialization method can be used.
*
* @return  void
*/
void driver_init(void)
{
    /*rcu battery module init*/
    rcu_bat_init();

#if LED_EN
    /*led module init*/
    led_module_init();
#endif

    if ((app_global_data.test_mode == NOT_TEST_MODE)
        || (app_global_data.test_mode == AUTO_PAIR_WITH_FIX_ADDR_MODE))
    {
        /*normal mode keyscan init*/
        keyscan_init_driver(KeyScan_Debounce_Enable);
    }
#if MP_TEST_MODE_SUPPORT_DATA_UART_TEST
    else if (app_global_data.test_mode == DATA_UART_TEST_MODE)
    {
        /*data uart mode uart init*/
        uart_test_init();
    }
#endif
}

#if DLPS_EN
/**
 * @brief this function will be called before enter DLPS
 *
 *  set PAD and wakeup pin config for enterring DLPS
 *
 * @param none
 * @return none
 * @retval void
*/
void app_enter_dlps_config(void) DATA_RAM_FUNCTION;
void app_enter_dlps_config(void)
{
    if ((app_global_data.test_mode == NOT_TEST_MODE)
        || (app_global_data.test_mode == AUTO_PAIR_WITH_FIX_ADDR_MODE))
    {
        keyscan_enter_dlps_config();
    }
#if MP_TEST_MODE_SUPPORT_DATA_UART_TEST
    else if (app_global_data.test_mode == DATA_UART_TEST_MODE)
    {
        /*config uart tx/rx pins*/
        Pad_Config(MP_TEST_UART_TX_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE,
                   PAD_OUT_LOW);
        Pad_Config(MP_TEST_UART_RX_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE,
                   PAD_OUT_LOW);

        /*config mp check pin*/
        Pad_Config(MP_TEST_TRIG_PIN_2, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
                   PAD_OUT_LOW);
    }
#endif

#if IR_FUN
    Pad_Config(IR_SEND_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
#endif

#if IR_LEARN_MODE
#if (IR_LEARN_TRIG_MODE == IR_LEARN_TRIG_RISING_EDGE)
    Pad_Config(IR_LEARN_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
#elif (IR_LEARN_TRIG_MODE == IR_LEARN_TRIG_FALL_EDGE)
    Pad_Config(IR_LEARN_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
#endif
#endif

#if BAT_LPC_EN
    lpc_enter_dlps_config();
#endif
}

/**
 * @brief this function will be called after exit DLPS
 *
 *  set PAD and wakeup pin config for enterring DLPS
 *
 * @param none
 * @return none
 * @retval void
*/
void app_exit_dlps_config(void) DATA_RAM_FUNCTION;
void app_exit_dlps_config(void)
{
    if ((app_global_data.test_mode == NOT_TEST_MODE)
        || (app_global_data.test_mode == AUTO_PAIR_WITH_FIX_ADDR_MODE))
    {
        keyscan_exit_dlps_config();
    }

#if IR_FUN
    Pad_Config(IR_SEND_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
#endif

#if IR_LEARN_MODE
#if (IR_LEARN_TRIG_MODE == IR_LEARN_TRIG_RISING_EDGE)
    Pad_Config(IR_LEARN_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
#elif (IR_LEARN_TRIG_MODE == IR_LEARN_TRIG_FALL_EDGE)
    Pad_Config(IR_LEARN_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);
#endif
#endif

#if BAT_LPC_EN
    lpc_exit_dlps_config();
#endif
}

/**
 * @brief app_dlps_check_cb() contains the setting about app dlps callback.
*/
bool app_dlps_check_cb(void) DATA_RAM_FUNCTION;
bool app_dlps_check_cb(void)
{
    return (
               keyscan_global_data.is_allowed_to_enter_dlps
#if RCU_VOICE_EN
               && voice_driver_global_data.is_allowed_to_enter_dlps
#endif
#if IR_FUN
               && ir_send_check_dlps()
#endif
#if IR_LEARN_MODE
               && ir_learn_check_dlps()
#endif
#if MP_TEST_MODE_SUPPORT_DATA_UART_TEST
               &&  uart_test_check_dlps_allowed()
#endif
#if BAT_EN
               &&  bat_allow_enter_dlps()
#endif
           );
}
#endif

/**
* @brief  pwr_mgr_init() contains the setting about power mode.
*/
void pwr_mgr_init(void)
{
#if DLPS_EN
    if (false == dlps_check_cb_reg(app_dlps_check_cb))
    {
        APP_PRINT_ERROR0("Error: dlps_check_cb_reg(app_dlps_check_cb) failed!");
    }
    DLPS_IORegUserDlpsEnterCb(app_enter_dlps_config);
    DLPS_IORegUserDlpsExitCb(app_exit_dlps_config);
    DLPS_IORegister();
    lps_mode_set(LPM_DLPS_MODE);
#else
    lps_mode_set(LPM_ACTIVE_MODE);
#endif
}

/******************************************************************
 * @fn          Initial profile
 * @brief      Add simple profile service and register callbacks
 *
 * @return     void
 */
void app_le_profile_init(void)
{
#if (RCU_VOICE_EN && (VOICE_FLOW_SEL == ATV_GOOGLE_VOICE_FLOW || VOICE_FLOW_SEL == RTK_GATT_VOICE_FLOW))
    server_init(7);
#else
    server_init(6);
#endif
    app_global_data.bas_srv_id = bas_add_service((void *)app_profile_callback);
    app_global_data.dis_srv_id = dis_add_service((void *)app_profile_callback);
    app_global_data.hid_srv_id = hids_add_service((void *)app_profile_callback);
    app_global_data.vendor_srv_id = vendor_svc_add_service((void *)app_profile_callback);
    app_global_data.ota_srv_id = ota_add_service((void *)app_profile_callback);
#if RCU_VOICE_EN
#if (VOICE_FLOW_SEL == ATV_GOOGLE_VOICE_FLOW)
    app_global_data.atvv_srv_id = atvv_add_service(app_profile_callback);
#elif (VOICE_FLOW_SEL == RTK_GATT_VOICE_FLOW)
    app_global_data.voice_srv_id = voice_service_add_service(app_profile_callback);
#endif
#endif
#if SUPPORT_SILENT_OTA
    app_global_data.dfu_srv_id = dfu_add_service((void *)app_profile_callback);
#endif
    server_register_app_cb(app_profile_callback);
}

/**
* @brief  app_system_on_interrupt_handler() contains the handler for System_On interrupt.
*/
void app_system_on_interrupt_handler(void) DATA_RAM_FUNCTION;
void app_system_on_interrupt_handler(void)
{
    APP_PRINT_INFO0("[app_system_on_interrupt_handler] system on interrupt");
    uint8_t tmpVal;
    NVIC_DisableIRQ(System_IRQn);
    tmpVal = btaon_fast_read_safe(0x2b);
    if (tmpVal & BIT7)
    {
        /* pad signal wake up event */
        btaon_fast_write_safe(0x2b, (tmpVal | BIT7));
        keyscan_init_driver(KeyScan_Debounce_Disable);
        keyscan_global_data.is_allowed_to_enter_dlps =
            false;  /* not allow to enter DLPS before interrupt or timeout */
        /* Set flag and start timer to handle unexpection condition:
           1. Not Keyscan row PAD wake up system;
           2. PAD wake up signal disappear before Keyscan works; */
        keyscan_global_data.is_key_pressed = true;
        if (!os_timer_restart(&keyscan_timer, KEYSCAN_SW_INTERVAL))
        {
            APP_PRINT_ERROR0("[System_Handler] keyscan_timer restart failed!");
            keyscan_global_data.is_allowed_to_enter_dlps = true;
        }
    }
    NVIC_ClearPendingIRQ(System_IRQn);
}

/**
* @brief  task_init() contains the initialization of all the tasks.
*
*           There are four tasks are initiated.
*           Lowerstack task and upperstack task are used by bluetooth stack.
*           Application task is task which user application code resides in.
*           Emergency task is reserved.
*
* @return  void
*/
void task_init(void)
{
    app_task_init();
}

/**
* @brief  app_normal_power_on_seq() contains the app normal power on sequence.
*/
void app_normal_power_on_seq(void)
{
    board_init();
    driver_init();
    le_gap_init(1);
    rcu_le_gap_init();
    app_le_profile_init();
    pwr_mgr_init();
    sw_timer_init();
    task_init();
}

int main(void)
{
    extern uint32_t random_seed_value;
    srand(random_seed_value);

    global_data_init();

    /* check test mode */
    app_global_data.test_mode = get_test_mode();
    reset_test_mode();

    APP_PRINT_INFO1("Test Mode is %d", app_global_data.test_mode);

    switch (app_global_data.test_mode)
    {
    case NOT_TEST_MODE:
        {
#if (MP_TEST_MODE_TRIG_SEL & MP_TEST_MODE_TRIG_BY_GPIO)
            mp_test_check_trig_gpio_status();
#endif
            app_normal_power_on_seq();
        }
        break;

#if MP_TEST_MODE_SUPPORT_SINGLE_TONE_TEST
    case SINGLE_TONE_MODE:
        {
            WDG_Disable();  /* Avoid unexpected reboot */
            single_tone_init();
        }
        break;
#endif

#if MP_TEST_MODE_SUPPORT_FAST_PAIR_TEST
    case AUTO_PAIR_WITH_FIX_ADDR_MODE:
        {
            mp_test_load_fp_mac_addr();
            app_normal_power_on_seq();
        }
        break;
#endif

#if MP_TEST_MODE_SUPPORT_DATA_UART_TEST
    case DATA_UART_TEST_MODE:
        {
            app_normal_power_on_seq();
        }
        break;
#endif

    default:
        break;
    }
    os_sched_start();
    return 0;
}
