/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    This is the entry of user code which the main function resides in.
* @details
* @author   chenjie_jin
* @date     2020-02-21
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
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
#include <bas.h>
#include <dis.h>
#include <hids_rmc.h>
#include <vendor_service.h>
#include <ota_service.h>
#include <rcu_gap.h>
#include <app_section.h>
#include <mem_config.h>
#include <key_handle.h>
#include "test_mode.h"
#include "battery_driver.h"
#include "led_config.h"
#include "voltage_driver.h"

#if SUPPORT_SILENT_OTA
#include <dfu_service.h>
#endif
#if SUPPORT_VOICE_FEATURE
#include <voice.h>
#endif
#if SUPPORT_IR_TX_FEATURE
#include <ir_send_driver.h>
#include <ir_learn_driver.h>
#endif
#if MP_TEST_MODE_SUPPORT_SINGLE_TONE_TEST
#include <single_tone.h>
#endif
#if FEATURE_SUPPORT_MP_TEST_MODE
#include <mp_test.h>
#include <data_uart_test.h>
#endif
#if SCENARIO_SWITCH_EN
#include <app_section.h>
#include <overlay_mgr.h>
#endif
#if (VOICE_FLOW_SEL == ATV_GOOGLE_VOICE_FLOW)
#include <atvv_service.h>
#elif (VOICE_FLOW_SEL == RTK_GATT_VOICE_FLOW)
#include <voice_service.h>
#endif
#include <rtc_driver.h>
#include "rtl876x_pinmux.h"
/*============================================================================*
 *                              Functions Declaration
 *============================================================================*/
static void global_data_init(void);
static void app_pinmux_config(void);
static void app_pad_config(void);
static void board_init(void);
static void driver_init(void);
#if DLPS_EN
static void app_enter_dlps_config(void) DATA_RAM_FUNCTION;
static void app_exit_dlps_config(void) DATA_RAM_FUNCTION;
static bool app_dlps_check_cb(void) DATA_RAM_FUNCTION;
#endif
static void pwr_mgr_init(void);
static void task_init(void);
static void app_normal_power_on_seq(void);

#define app_system_on_interrupt_handler System_Handler
void app_system_on_interrupt_handler(void) DATA_RAM_FUNCTION;

/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief  global_data_init() contains the initialization of global data.
 * @param  none
 * @return none
 * @retval void
 */
void global_data_init(void)
{
    app_init_global_data();
   
#if SUPPORT_VOICE_FEATURE
    voice_driver_init_data();
    voice_driver_init_codec_params();
#if (VOICE_FLOW_SEL == ATV_GOOGLE_VOICE_FLOW)
    voice_handle_atvv_init_data();
#endif
#endif
#if SUPPORT_BAT_DETECT_FEATURE
    bat_init_data();
#endif
#if FEATURE_SUPPORT_MP_TEST_MODE
    mp_test_init_data();
#endif
}

/******************************************************************
 * @brief  app_pinmux_config() contains the initialization of app pinmux config.
 * @param  none
 * @return none
 * @retval void
 */
void app_pinmux_config(void)
{
    /*only when in normal mode and fast pair mode, keyscan can be config*/
    if ((app_global_data.test_mode == NOT_TEST_MODE)
        || (app_global_data.test_mode == AUTO_PAIR_WITH_FIX_ADDR_MODE))
    {
        //keyscan_pinmux_config();
    }
}

/******************************************************************
 * @brief  app_pad_config() contains the initialization of app pad config.
 * @param  none
 * @return none
 * @retval void
 */
void app_pad_config(void)
{
    /*only when in normal mode and fast pair mode, keyscan can be config*/
    if ((app_global_data.test_mode == NOT_TEST_MODE)
        || (app_global_data.test_mode == AUTO_PAIR_WITH_FIX_ADDR_MODE))
    {
        //keyscan_init_pad_config();
    }
}

/******************************************************************
 * @brief  Board_Init() contains the initialization of pinmux settings and pad settings.
 *
 *               All the pinmux settings and pad settings shall be initiated in this function.
 *               But if legacy driver is used, the initialization of pinmux setting and pad setting
 *               should be peformed with the IO initializing.
 * @param  none
 * @return none
 * @retval void
 */
void board_init(void)
{
    app_pinmux_config();
    app_pad_config();
}

/******************************************************************
 * @brief  driver_init() contains the initialization of peripherals.
 *
 *               Both new architecture driver and legacy driver initialization method can be used.
 * @param  none
 * @return none
 * @retval void
 */
void driver_init(void)
{
#if SUPPORT_LED_INDICATION_FEATURE
    /*led module init*/
    //led_module_init();
#endif

#if MP_TEST_MODE_SUPPORT_DATA_UART_TEST
    if (app_global_data.test_mode == DATA_UART_TEST_MODE)
    {
        /*data uart mode uart init*/
        uart_test_init();
    }
#endif


#if SUPPORT_BAT_DETECT_FEATURE
    bat_init_driver();
#endif
}

#if DLPS_EN
/******************************************************************
 * @brief  this function will be called before enter DLPS
 *
 *  set PAD and wakeup pin config for enterring DLPS
 * @param  none
 * @return none
 * @retval void
 */
void app_enter_dlps_config(void)
{
    if ((app_global_data.test_mode == NOT_TEST_MODE)
        || (app_global_data.test_mode == AUTO_PAIR_WITH_FIX_ADDR_MODE))
    {
        //keyscan_enter_dlps_config();
    }
#if MP_TEST_MODE_SUPPORT_DATA_UART_TEST
    else if (app_global_data.test_mode == DATA_UART_TEST_MODE)
    {
        uart_test_enter_dlps_config();
    }
#endif

#if SUPPORT_BAT_DETECT_FEATURE
    bat_enter_dlps_config();
#endif


}

/******************************************************************
 * @brief this function will be called after exit DLPS
 *
 *  set PAD and wakeup pin config for enterring DLPS
 *
 * @param  none
 * @return none
 * @retval void
 */
void app_exit_dlps_config(void)
{
    if ((app_global_data.test_mode == NOT_TEST_MODE)
        || (app_global_data.test_mode == AUTO_PAIR_WITH_FIX_ADDR_MODE))
    {
        //keyscan_exit_dlps_config();
    }
#if SUPPORT_BAT_DETECT_FEATURE
    bat_exit_dlps_config();
#endif

#if SUPPORT_IR_TX_FEATURE
    ir_send_exit_dlps_config();
#endif

#if SUPPORT_IR_LEARN_FEATURE
    ir_learn_exit_dlps_config();
#endif
}

/******************************************************************
 * @brief app_dlps_check_cb() contains the setting about app dlps callback.
 * @param  none
 * @retval ture   able to enter DLPS
 * @retval false  unable to enter DLPS
 */
bool app_dlps_check_cb(void)
{
    return (
               
#if SUPPORT_VOICE_FEATURE
               && voice_driver_dlps_check()
#endif
#if SUPPORT_IR_TX_FEATURE
               && ir_send_check_dlps()
#endif
#if SUPPORT_IR_LEARN_FEATURE
               && ir_learn_check_dlps()
#endif
#if MP_TEST_MODE_SUPPORT_DATA_UART_TEST
               &&  uart_test_check_dlps()
#endif
           1);
}
#endif

/******************************************************************
 * @brief  pwr_mgr_init() contains the setting about power mode.
 * @param  none
 * @return none
 * @retval void
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
    lps_mode_set(PLATFORM_DLPS_PFM);
#else
//    lps_mode_set(LPM_ACTIVE_MODE);
#endif
}

/******************************************************************
 * @brief  Add simple profile service and register callbacks
 * @param  none
 * @return none
 * @retval void
 */
static void app_le_profile_init(void)
{
#if (SUPPORT_VOICE_FEATURE && (VOICE_FLOW_SEL == ATV_GOOGLE_VOICE_FLOW || VOICE_FLOW_SEL == RTK_GATT_VOICE_FLOW))
    server_init(7);
#else
    server_init(6);
#endif
    app_global_data.bas_srv_id = bas_add_service((void *)app_profile_callback);
    app_global_data.dis_srv_id = dis_add_service((void *)app_profile_callback);
    app_global_data.hid_srv_id = hids_add_service((void *)app_profile_callback);
    app_global_data.vendor_srv_id = vendor_svc_add_service((void *)app_profile_callback);
    app_global_data.ota_srv_id = ota_add_service((void *)app_profile_callback);
#if SUPPORT_VOICE_FEATURE
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

/******************************************************************
 * @brief  app_system_on_interrupt_handler() contains the handler for System_On interrupt.
 * @param  none
 * @return none
 * @retval void
 */
void app_system_on_interrupt_handler(void)
{
    APP_PRINT_INFO0("[app_system_on_interrupt_handler] sytem on interrupt");

    NVIC_DisableIRQ(System_IRQn);

    if (SET == System_DebounceWakeupStatus())
    {
        APP_PRINT_INFO0("[app_system_on_interrupt_handler] pad signal wake up");
        /* pad signal wake up event */
        keyscan_global_data.is_allowed_to_enter_dlps = false;

        /*register trigger manual mode init*/
        //keyscan_init_driver(KeyScan_Manual_Sel_Bit, KeyScan_Debounce_Disable);
    }
    NVIC_ClearPendingIRQ(System_IRQn);
}

/******************************************************************
 * @brief  task_init() contains the initialization of all the tasks.
 *
 *           There are four tasks are initiated.
 *           Lowerstack task and upperstack task are used by bluetooth stack.
 *           Application task is task which user application code resides in.
 *           Emergency task is reserved.
 * @param  none
 * @return none
 * @retval void
*/
void task_init(void)
{
    APP_PRINT_INFO0("task_init");
    app_task_init();
}


/**
 * @brief 重要的驱动初始化，必须放在其他初始化之后，不然可能导致内存初始化异常
 * @param  
 */
void my_device_init(void)
{
    pwm_init_driver();

    keyscan_io_init_driver(20);
    
    bct3236_init();

    voltage_init();
    
    usb_chg_gpio_init();
    
    power_on_io_init();
    
    APP_PRINT_INFO0("system power on");
    //POWER_ON_OPEN();
}



/******************************************************************
 * @brief  app_normal_power_on_seq() contains the app normal power on sequence.
 * @param  none
 * @return none
 * @retval void
 */
void app_normal_power_on_seq(void)
{
    board_init();
    driver_init();
    le_gap_init(1);
    gap_lib_init();
    rcu_le_gap_init();
    app_le_profile_init();
    pwr_mgr_init();
    sw_timer_init();
    my_device_init();
    task_init();
}

/*============================================================================*
 *                              Global Functions
 *============================================================================*/
/******************************************************************
 * @brief  app_nvic_config() contains the initialization of app NVIC config.
 *
 * @param  none
 * @return none
 * @retval void
 */
void app_nvic_config(void)
{
    if ((app_global_data.test_mode == NOT_TEST_MODE)
        || (app_global_data.test_mode == AUTO_PAIR_WITH_FIX_ADDR_MODE))
    {
#if FEATURE_SUPPORT_DEEP_SLEEP_STATE
        if (false == app_global_data.wake_up_from_dss)
#endif
        {
            /*key trigger manual mode init*/
            //keyscan_init_driver(KeyScan_Manual_Sel_Key, KeyScan_Debounce_Enable);
        }
        //keyscan_nvic_config();
    }
#if SUPPORT_BAT_DETECT_FEATURE
    bat_nvic_config();
#endif

    rtc_driver_init();
}

/******************************************************************
 * @brief  main() is a start of main codes.
 * @param  none
 * @retval 0
 */
int main(void)
{
#if FEAUTRE_SUPPORT_FLASH_2_BIT_MODE
	
	
    if (FLASH_SUCCESS == flash_try_high_speed(FLASH_MODE_2BIT))
    {
        APP_PRINT_INFO0("Switch to 2-Bit flash mode");
    }
#endif
    extern uint32_t random_seed_value;
    srand(random_seed_value);
    global_data_init();

    /* check test mode */
    app_global_data.test_mode = get_test_mode();
    reset_test_mode();
		
	// uint32_t data_ram_size = 0, buffer_ram_size = 0;
    // data_ram_size = os_mem_peek(RAM_TYPE_DATA_ON);//the remainig data ram size
    // buffer_ram_size = os_mem_peek(RAM_TYPE_BUFFER_ON);//the remainig buffer ram size
    // APP_PRINT_INFO2("[app_print_remaining_ram_size] data_ram_size = %d, buffer_ram_size = %d",
    //                 data_ram_size, buffer_ram_size);

    APP_PRINT_INFO1("Test Mode is %d", app_global_data.test_mode);

#if FEATURE_SUPPORT_DEEP_SLEEP_STATE
    APP_PRINT_INFO1("reset reason is 0x%x", reset_reason_get());
    if (RESET_REASON_POWERDOWN == reset_reason_get())
    {
        APP_PRINT_INFO0("wake up from dss");
        app_global_data.wake_up_from_dss = true;
    }
#endif

    switch (app_global_data.test_mode)
    {
    case NOT_TEST_MODE:
        {
            APP_PRINT_INFO0("RCU is in NOT_TEST_MODE");
#if (MP_TEST_MODE_TRIG_SEL & MP_TEST_MODE_TRIG_BY_GPIO)
            mp_test_check_trig_gpio_status();
#endif
            app_normal_power_on_seq();
        }
        break;

#if MP_TEST_MODE_SUPPORT_SINGLE_TONE_TEST
    case SINGLE_TONE_MODE:
        {
            APP_PRINT_INFO0("RCU is in SINGLE_TONE_MODE");
            WDG_Disable();  /* Avoid unexpected reboot */
            single_tone_init();
        }
        break;
#endif

#if MP_TEST_MODE_SUPPORT_FAST_PAIR_TEST
    case AUTO_PAIR_WITH_FIX_ADDR_MODE:
        {
            APP_PRINT_INFO0("RCU is in AUTO_PAIR_WITH_FIX_ADDR_MODE");
#if (SUPPORT_VOICE_FEATURE  && (VOICE_FLOW_SEL == ATV_GOOGLE_VOICE_FLOW))
            app_global_data.is_atv_voice_notify_en = true;
#endif
            mp_test_load_fp_mac_addr();
            app_normal_power_on_seq();
        }
        break;
#endif

#if MP_TEST_MODE_SUPPORT_DATA_UART_TEST
    case DATA_UART_TEST_MODE:
        {
            APP_PRINT_INFO0("RCU is in DATA_UART_TEST_MODE");
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

/******************* (C) COPYRIGHT 2020 Realtek Semiconductor Corporation *****END OF FILE****/
