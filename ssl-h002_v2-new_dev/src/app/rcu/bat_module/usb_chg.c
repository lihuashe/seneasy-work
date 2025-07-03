/**
*********************************************************************************************************
*               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     usb_chg.c
* @brief    
* @details
* @author   lihuashe
* @date     2024-12-20
* @version  v1.0
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
#include "bas.h"
#include "rcu_application.h"
#include "rcu_gap.h"
#include "app_task.h"
#include "rtl876x_tim.h"
#include "rtl876x_uart.h"
#include "app_section.h"
#include "usb_chg.h"
#include "led_config.h"
#include "production_test.h"
#include "voltage_driver.h"
#include <os_msg.h>
#include <os_task.h>

static bool usb_chg_out_status = false;//插拔usb
static bool bat_chg_done = false;
static void *usb_chg_task_handle = NULL;
/*============================================================================*
 *                              Must Functions 
 *============================================================================*/


void power_on_io_init(void)
{

    Pad_Config(POWER_ON, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(ADC_BAT_VET_C, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);

    /* Normal mode */
    Pinmux_Config(POWER_ON, DWGPIO);
    Pinmux_Config(ADC_BAT_VET_C, DWGPIO);

    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    // 配置引脚为模式，启用上拉电阻
    
    GPIO_InitStruct.GPIO_Pin = GPIO_GetPin(POWER_ON) | GPIO_GetPin(ADC_BAT_VET_C); 

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT; // 设置为输出模式
    GPIO_InitStruct.GPIO_ITCmd = DISABLE; // 禁用中断
    GPIO_InitStruct.GPIO_ControlMode = GPIO_SOFTWARE_MODE; 
    GPIO_Init(&GPIO_InitStruct);
}



/**
 * @brief 这是usb读取gpio控制初始化
 * @param  
 */
void usb_chg_gpio_init(void)
{
    
    Pad_Config(USB_DET, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
   
    Pinmux_Config(USB_DET, DWGPIO);

    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    // 配置引脚为模式，启用上拉电阻
    
    GPIO_InitStruct.GPIO_Pin = GPIO_GetPin(USB_DET) ; 

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN; // 设置为输入模式
    GPIO_InitStruct.GPIO_ITCmd = ENABLE; // 使能中断
    GPIO_InitStruct.GPIO_ITTrigger = GPIO_INT_BOTH_EDGE; // 边缘触发
    GPIO_InitStruct.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_HIGH; // 设置为高电平有效
    GPIO_InitStruct.GPIO_ControlMode = GPIO_HARDWARE_MODE; 
    GPIO_InitStruct.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_ENABLE; // 启用去抖动
    GPIO_InitStruct.GPIO_DebounceTime = 20; // 适用去抖动时间
    GPIO_Init(&GPIO_InitStruct);
      
     
   
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = USB_DET_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    GPIO_MaskINTConfig(GPIO_GetPin(USB_DET), DISABLE);
    GPIO_INTConfig(GPIO_GetPin(USB_DET), ENABLE);
}




/**
 * @brief 初始化任务
 * @param  
 */
void usb_chg_task_init(void)
{
    APP_PRINT_INFO0("usb_chg_task_init");

    if (GPIO_ReadInputDataBit(GPIO_GetPin(USB_DET)))
    {
        if (usb_chg_task_handle == NULL && continue_aging == false)
            os_task_create(&usb_chg_task_handle, "usb_chg_task", usb_chg_task, 0, 512, 1);
    }
    
}


/**
 * @brief 删除任务
 * @param  
 */
void delete_usb_chg_task(void)
{
    APP_PRINT_INFO0("delete_usb_chg_task\n");
    static bool delete_usb = false;

    if (usb_chg_task_handle != NULL && delete_usb == false)
    {
        PRINT("delete_usb_chg_task\n");
        delete_usb = true;
        os_task_delete(usb_chg_task_handle);
    }     

}

void all_sys_function_off(void)
{
    uint32_t all_function_stop = os_lock();//进入临界

    WDG_Disable();//关闭看门狗
    led_system_ctrl(LED_BRIGHT_STOP);//关闭所有灯光
    os_timer_stop(&power_auto_off_timer);
    key_disable();
    out_voltage_off();
    os_unlock(all_function_stop);

    os_delay(10);    //等待关闭结束
}

void usb_chg_task(void *p_param)
{
    //PRINT("usb_check_chg: %d\n", GPIO_ReadInputDataBit(GPIO_GetPin(USB_DET)));
    if (GPIO_ReadInputDataBit(GPIO_GetPin(USB_DET)))
    {
        all_sys_function_off();
        product_test_task_init();
        usb_set_chg_mode();

        static bool bat_chg_check_flag = false; 

        while (1)
        {
            if (bat_chg_check_flag == false)
            {
                POWER_ON_OPEN();
                bat_chg_check_flag = true;
                uint8_t bat_status = lp4081h_get_bat_status();
                if (bat_status & LP4081H_PGD == LP4081H_PGD)
                {
                    led_system_ctrl(LED_CHGING);//开启呼吸灯
                    lp4081h_start_chg();
                    os_timer_start(&bat_check_timer);//开启充电检测
                }  
            }

            if (usb_chg_out_status == true)
            {
                led_system_ctrl(LED_BRIGHT_STOP);
                POWER_ON_OFF();//关机
            }

            if (bat_chg_done == true)
            {
                bat_chg_done = false;
                led_system_ctrl(LED_BRIGHT_STOP);
                led_system_ctrl(LED_CHG_DONE);//5个白灯亮
            }

            os_delay(10);
        }
    }
}

void USB_DET_Handler(void)
{
    //PRINT("USB_DET_Handler\n");
    GPIO_INTConfig(GPIO_GetPin(USB_DET), DISABLE);
    GPIO_MaskINTConfig(GPIO_GetPin(USB_DET), ENABLE);
    
    if (!GPIO_ReadInputDataBit(GPIO_GetPin(USB_DET)))
    {
        usb_chg_out_status = true;  
    }
    else
    {
        T_IO_MSG usb_io_msg;
        usb_io_msg.type = IO_MSG_TYPE_GPIO;
        
        usb_io_msg.subtype = IO_MSG_SUBTYPE_USB_CHG;

        if (false == app_send_msg_to_apptask(&usb_io_msg))
        {
            PRINT("send usb_chg message failed!\n");
        }
    }
		
    
    GPIO_ClearINTPendingBit(GPIO_GetPin(USB_DET));
    GPIO_MaskINTConfig(GPIO_GetPin(USB_DET), DISABLE);
    GPIO_INTConfig(GPIO_GetPin(USB_DET), ENABLE);
	
}

/**
 * @brief 设置充电初始化
 * @param  
 */
void usb_set_chg_mode(void)
{   
    //lp4081h_chg_system_set(1, "440mA", "650mA", "4.2V", "550mA", "15mA");
    lp4081h_default_config();
    //bat_get_chg_status();
}


/**
 * @brief 获取电池充电状态
 * @param  
 */
void bat_get_chg_status(void)
{
    uint8_t bat_status = lp4081h_get_bat_status();
    
    if ((bat_status & LP4081H_CHG_DONE) == LP4081H_CHG_DONE)
    {
        PRINT("LP4081H_CHG_DONE\n");
    }
    else if ((bat_status & LP4081H_CONST_CHG) == LP4081H_CONST_CHG)
    {
        PRINT("LP4081H_CONST_CHG\n");
    }
    else if ((bat_status & LP4081H_TRICKLE_CHG) == LP4081H_TRICKLE_CHG)
    {
        PRINT("LP4081H_TRICKLE_CHG\n");
    }
    else if ((bat_status & LP4081H_NO_CHG) == LP4081H_NO_CHG)
    {
        PRINT("LP4081H_NO_CHG\n");
    }
     
}


/**
 * @brief 定时检测充电状态
 * @param pxTimer 
 */
void bat_chg_status_check_timer_cb(TimerHandle_t pxTimer)
{
    int temper = lp4081h_get_bat_temperature();
    uint8_t bat_status = lp4081h_get_bat_status();
    
    bat_pluse_value_info();
    uint16_t bat_value = bat_get_current_voltage_value();//充电电压高于电池电压0.06v左右

    PRINT("bat_value = %d\n", bat_value);
    if ((bat_status & LP4081H_CHG_DONE) == LP4081H_CHG_DONE)
    {
        //PRINT("LP4081H_CHG_DONE\n");
        if (bat_value >= 4000)
        {
            PRINT("LP4081H_CHG_DONE\n");
            bat_chg_done = true;
            lp4081h_stop_chg();
            os_timer_restart(&bat_check_timer, 30 * 60 * 1000);
        }
        else
        {
            PRINT("bat_value < 4000\n");
            lp4081h_start_chg();
            os_timer_restart(&bat_check_timer, 3 * 60 * 1000);
        }
        
        return ;
    }
    else if ((bat_status & LP4081H_CONST_CHG) == LP4081H_CONST_CHG)
    {
        //PRINT("LP4081H_CONST_CHG\n");
    }
    else if ((bat_status & LP4081H_TRICKLE_CHG) == LP4081H_TRICKLE_CHG)
    {
        //PRINT("LP4081H_TRICKLE_CHG\n");
    }
    else if ((bat_status & LP4081H_NO_CHG) == LP4081H_NO_CHG)
    {
        //PRINT("LP4081H_NO_CHG\n");
        lp4081h_start_chg();
        os_timer_restart(&bat_check_timer, 3 * 60 * 1000);
    }

    

    if (temper == 0xffff)
    {
        //PRINT("bat chg status check erron\n");
    } 
    else if (temper > 45)
    {
        lp4081h_stop_chg();
        //PRINT("bat chg temper > 45\n");
    }
    else if (temper < 45)
    {
        lp4081h_start_chg();
        os_timer_restart(&bat_check_timer, 3 * 60 * 1000);
    
        //PRINT("bat chg temper < 45\n");
    }
}


bool bat_chg_doing(void)
{
    uint8_t bat_status = lp4081h_get_bat_status();

    if ((bat_status & LP4081H_NO_CHG) == LP4081H_NO_CHG || (bat_status & LP4081H_CHG_DONE) == LP4081H_CHG_DONE)
    {
        return false;
    }

    return true;
}
/******************* (C) COPYRIGHT 2024 Sensasy Corporation *****END OF FILE****/

