/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     app_task.c
* @brief    application task.
* @details
* @author   barry_bian
* @date     2020-02-21
* @version  v1.1
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "board.h"
#include <os_msg.h>
#include <os_task.h>
#include <trace.h>
#include <app_task.h>
#include <app_msg.h>
#include <app_task.h>
#include <rcu_application.h>
#include "mem_config.h"
#include "rtl876x_uart.h"
#include "otp_config.h"
#include "app_section.h"
#include "key_handle.h"
#include "rtl876x_gpio.h"
#include "os_sync.h"
#include "led_config.h"
#include "production_test.h"
#include "gap_adv.h"
#include "gap_bond_le.h"
#include "gap_conn_le.h"

#if (AON_WDG_ENABLE == 1)
#include "rtl876x_aon_wdg.h"
#endif

/*============================================================================*
 *                              Macros
 *============================================================================*/
#define APP_TASK_PRIORITY           1   /* Task priorities. */

#define APP_TASK_STACK_SIZE         512 * 8

#define MAX_NUMBER_OF_GAP_MESSAGE                   0x20
#define MAX_NUMBER_OF_IO_MESSAGE                    0x20
#define MAX_NUMBER_OF_PRODUCT_TEST_IO_MESSAGE       0x05
#define MAX_NUMBER_OF_EVENT_MESSAGE (MAX_NUMBER_OF_GAP_MESSAGE + MAX_NUMBER_OF_IO_MESSAGE)

/*============================================================================*
 *                          Global Variables
 *============================================================================*/
void *app_task_handle = NULL;
void *evt_queue_handle = NULL;
void *io_queue_handle  = NULL;
void *product_test_io_queue_handle  = NULL;
void *i2c_mutex_handle = NULL;


/*============================================================================*
 *                              Functions Declaration
 *============================================================================*/
static void app_main_task(void *p_param);

bool app_send_msg_to_apptask(T_IO_MSG *p_msg) DATA_RAM_FUNCTION;
void app_task_seq_on(void) DATA_RAM_FUNCTION;

/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief   handle function of app main task
 * @param   *p_param
 * @return  none
 * @retval  void
 */
void app_main_task(void *p_param)
{
    uint8_t event;
    
    os_msg_queue_create(&io_queue_handle, MAX_NUMBER_OF_IO_MESSAGE, sizeof(T_IO_MSG));
    os_msg_queue_create(&evt_queue_handle, MAX_NUMBER_OF_EVENT_MESSAGE, sizeof(uint8_t));
    os_msg_queue_create(&product_test_io_queue_handle, MAX_NUMBER_OF_PRODUCT_TEST_IO_MESSAGE, sizeof(T_IO_MSG));
    
    gap_start_bt_stack(evt_queue_handle, io_queue_handle, MAX_NUMBER_OF_GAP_MESSAGE);
    
#if (ROM_WATCH_DOG_ENABLE == 1)
    extern void reset_watch_dog_timer_enable(void);
    reset_watch_dog_timer_enable();
#endif

#if (AON_WDG_ENABLE == 1)
    aon_wdg_init(1, AON_WDG_TIME_OUT_PERIOD_SECOND);
    aon_wdg_enable();
#endif
    
    app_task_seq_on();

    while (true)
    {
        if (os_msg_recv(evt_queue_handle, &event, 0xFFFFFFFF) == true)
        {
            
            if (event == EVENT_IO_TO_APP)
            {
                T_IO_MSG io_msg;
                //APP_PRINT_INFO1("***os_msg_recv: %d***", io_msg.type);
                //PRINT("io_msg.type:%d\n", io_msg.type);
                if (os_msg_recv(io_queue_handle, &io_msg, 0) == true)
                {        
                    //APP_PRINT_INFO1("io_msg.type : %d", io_msg.type);
                    app_handle_io_msg(io_msg);
                }    
                key_press_time_dis_event();
            }
            else
            {
                gap_handle_msg(event);
            }
        }
    }
}

/*============================================================================*
 *                              Global Functions
 *============================================================================*/
/******************************************************************
 * @brief    send msg queue to app task.
 *
 * @param    p_handle   The handle to the message queue being peeked.
 *
 * @return           The status of the message queue peek.
 * @retval true      Message queue was peeked successfully.
 * @retval false     Message queue was failed to peek.
 */
bool app_send_msg_to_apptask(T_IO_MSG *p_msg)
{
    uint8_t event = EVENT_IO_TO_APP;

    if (os_msg_send(io_queue_handle, p_msg, 0) == false)
    {
        APP_PRINT_ERROR0("send_io_msg_to_app fail");
        return false;
    }
    if (os_msg_send(evt_queue_handle, &event, 0) == false)
    {
        APP_PRINT_ERROR0("send_evt_msg_to_app fail");
        return false;
    }
    return true;
}

/******************************************************************
 * @brief   app_task_init() init app task
 * @param   none
 * @return  none
 * @retval  void
 */
void app_task_init(void)
{
    os_task_create(&app_task_handle, "app", app_main_task, 0, APP_TASK_STACK_SIZE,
                   APP_TASK_PRIORITY);
}


/**
 * @brief 上电执行
 * @param  
 */
void app_task_seq_on(void)
{
    //创建互斥锁保护I2C
    os_mutex_create(&i2c_mutex_handle);

    ADC_BAT_VET_C_OPEN();//上电开启电池供电

    bct3236_write_reg_and_data(0x00, 0x01);
    
    RamVectorTableUpdate(Uart1_VECTORn, data_uart1_interrupt_handler);
    uart_init(115200);
    
    if (app_global_data.rcu_status == RCU_STATUS_ADVERTISING)
    {
        if (le_adv_stop() == GAP_CAUSE_SUCCESS)
        {
            PRINT("rcu_stop_adv\n");
            app_global_data.rcu_status = RCU_STATUS_STOP_ADVERTISING;
        }
    }
    else  if ((app_global_data.rcu_status == RCU_STATUS_CONNECTED)
    || (app_global_data.rcu_status == RCU_STATUS_PAIRED))
    {
        if (le_disconnect(0) == GAP_CAUSE_SUCCESS)
        {
            PRINT("le_disconnect command sent successfully\n");
        }

    }

    seq_on_check_continue_aging();
    
    if (GPIO_ReadInputDataBit(GPIO_GetPin(USB_DET)))
    {
        usb_chg_task_init();
    }
    else
    {
        bat_pluse_value_info();
        uint8_t bat_level = bat_get_current_voltage_level();//获取电量，并且如果低于26%闪灯
        PRINT("bat_level :%d \n", bat_level);
        PRINT("bat_voltage :%d \n", bat_get_current_voltage_value());
        if (bat_level > 0)
        {
            if(bat_level < 26)
            {
                key_disable();
                POWER_ON_OPEN();//开机
                led_system_ctrl(LED_LOW_POWER);
            }
        
        }
        else
        {
            key_disable();//低电量不允许开机
        }
    }
    
}

/******************* (C) COPYRIGHT 2020 Realtek Semiconductor Corporation *****END OF FILE****/
