/**
*********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     keyscan_basic_driver.c
* @brief    keyscan module basic driver
* @details
* @author   mandy
* @date     2023-06-15
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <board.h>
#include <string.h>
#include <trace.h>
#include <keyscan_basic_driver.h>
#include <rtl876x_rcc.h>
#include <rtl876x_keyscan.h>
#include <rtl876x_pinmux.h>
#include <app_section.h>
#include <rtl876x_gpio.h>
#include "platform_utils.h"
#include "rtl876x.h"
#include "system_rtl876x.h"
#include "app_msg.h"
#include "app_task.h"



/*============================================================================*
 *                              Local Variables
 *============================================================================*/

/*============================================================================*
 *                              Global Variables
 *============================================================================*/
T_KEYSCAN_GLOBAL_DATA keyscan_global_data;
volatile KeyScan_Data g_keyscan_io_data = {};

/*============================================================================*
 *                              Functions Declaration
 *============================================================================*/
void keyscan_io_pad_config(PAD_Mode AON_PAD_Mode,
                            PAD_PWR_Mode AON_PAD_PwrOn,
                            PAD_Pull_Mode AON_PAD_Pull,
                            PAD_OUTPUT_ENABLE_Mode AON_PAD_E,
                            PAD_OUTPUT_VAL AON_PAD_O) DATA_RAM_FUNCTION;

                               
void keyscan_io_interrupt_handler(void) DATA_RAM_FUNCTION;                        
void keyscan_init_pad_config(void) DATA_RAM_FUNCTION;
void keyscan_io_init_pad_config(void) DATA_RAM_FUNCTION;
void keyscan_io_init_driver(uint32_t is_debounce) DATA_RAM_FUNCTION;
void keyscan_init_driver(uint32_t manual_sel, uint32_t is_debounce) DATA_RAM_FUNCTION;

/*============================================================================*
 *                              Local Functions
 *============================================================================*/


/*============================================================================*
 *                              Global Functions
 *============================================================================*/



void keyscan_io_init_driver(uint32_t is_debounce)
{

    keyscan_io_init_pad_config();

    /* Enable GPIO clock */
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;

#ifdef  KEY_IO0
    GPIO_InitStruct.GPIO_Pin = GPIO_GetPin(KEY_IO0); // 配置 KEY_PIN
#endif
#ifdef  KEY_IO1
    GPIO_InitStruct.GPIO_Pin = GPIO_GetPin(KEY_IO0) | GPIO_GetPin(KEY_IO1); // 配置 KEY_PIN
#endif
#ifdef  KEY_IO2
    GPIO_InitStruct.GPIO_Pin = GPIO_GetPin(KEY_IO0) | GPIO_GetPin(KEY_IO1) | GPIO_GetPin(KEY_IO2); // 配置 KEY_PIN
#endif
#ifdef  KEY_IO3
    GPIO_InitStruct.GPIO_Pin = GPIO_GetPin(KEY_IO0) | GPIO_GetPin(KEY_IO1) | GPIO_GetPin(KEY_IO2) | GPIO_GetPin(KEY_IO3); // 配置 KEY_PIN
#endif
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN; // 设置为输入模式
    GPIO_InitStruct.GPIO_ITCmd = ENABLE; // 中断
    GPIO_InitStruct.GPIO_ITTrigger = GPIO_INT_BOTH_EDGE; // 边缘触发
    GPIO_InitStruct.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW; // 设置为高电平有效
    GPIO_InitStruct.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_ENABLE; // 启用去抖动
    GPIO_InitStruct.GPIO_DebounceTime = is_debounce; // 适用去抖动时间
     
    GPIO_Init(&GPIO_InitStruct);
   
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = GPIO9_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 2;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    
    NVIC_InitStruct.NVIC_IRQChannel = GPIO4_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 2;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    GPIO_MaskINTConfig(GPIO_GetPin(KEY_IO0), DISABLE);
    GPIO_INTConfig(GPIO_GetPin(KEY_IO0),  ENABLE);//开机先不启用档位键
    GPIO_MaskINTConfig(GPIO_GetPin(KEY_IO1), DISABLE);
    GPIO_INTConfig(GPIO_GetPin(KEY_IO1), ENABLE);
}



/******************************************************************
 * @brief  keyscan one io pad config
 * @param  AON_PAD_Mode
 * @param  AON_PAD_PwrOn
 * @param  AON_PAD_Pull
 * @param  AON_PAD_E
 * @param  AON_PAD_O
 * @return none
 * @retval void
 */
void keyscan_io_pad_config(PAD_Mode AON_PAD_Mode,
                            PAD_PWR_Mode AON_PAD_PwrOn,
                            PAD_Pull_Mode AON_PAD_Pull,
                            PAD_OUTPUT_ENABLE_Mode AON_PAD_E,
                            PAD_OUTPUT_VAL AON_PAD_O)
{

#ifdef KEY_IO0
    Pad_Config(KEY_IO0, AON_PAD_Mode, AON_PAD_PwrOn, AON_PAD_Pull, AON_PAD_E, AON_PAD_O);
    Pinmux_Config(KEY_IO0, DWGPIO);
#endif
#ifdef KEY_IO1
    Pad_Config(KEY_IO1, AON_PAD_Mode, AON_PAD_PwrOn, AON_PAD_Pull, AON_PAD_E, AON_PAD_O);
    Pinmux_Config(KEY_IO1, DWGPIO);
#endif
#ifdef KEY_IO2
    Pad_Config(KEY_IO2, AON_PAD_Mode, AON_PAD_PwrOn, AON_PAD_Pull, AON_PAD_E, AON_PAD_O);
    Pinmux_Config(KEY_IO2, DWGPIO);
#endif
#ifdef KEY_IO3
    Pad_Config(KEY_IO3, AON_PAD_Mode, AON_PAD_PwrOn, AON_PAD_Pull, AON_PAD_E, AON_PAD_O);
    Pinmux_Config(KEY_IO3, DWGPIO);
#endif

}



void keyscan_io_init_pad_config(void)
{
    keyscan_io_pad_config(PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
    
    keyscan_global_data.is_pinmux_setted = true;
}



/**
 * @brief 这是根据按键中断进行发送处理的函数
 * @param key 那个按键
 * @param key_status 按下或者松开
 */
void get_value_handler(uint8_t key, uint8_t key_status)
{

    T_IO_MSG key_io_msg;
    key_io_msg.type = IO_MSG_TYPE_KEYSCAN;
    bool flag = false;
    //PRINT("key %d   key_status %d\n", key, key_status);
    for (uint8_t i = 0;i < 3; i++)
    {
        if (flag == false)
        {
            //PRINT("g_keyscan_io_data.key[i] %d\n", g_keyscan_io_data.key[i]);
            if (g_keyscan_io_data.key[i] == 0)
            {
                if (key && g_keyscan_io_data.key_status[i] == KEY_RELEASE && key_status)
                {
                    flag = true;
                    g_keyscan_io_data.key[i] = key;
                    g_keyscan_io_data.key_status[i] = KEY_PRESS;
                    g_keyscan_io_data.key_press_num++;
                    key_io_msg.subtype = IO_MSG_KEYSCAN_RX_PKT;
                    key_io_msg.u.buf   = (void *)(&g_keyscan_io_data);
                    if (false == app_send_msg_to_apptask(&key_io_msg))
                    {
                        PRINT("send key_press_num++ message failed!\n");
                    }
                    return ;
                }
            } 
            else if (g_keyscan_io_data.key[i] == key)
            {            
                if (g_keyscan_io_data.key_press_num > 0)
                {
                    flag = true;
                    g_keyscan_io_data.key[i] = 0;
                    g_keyscan_io_data.key_status[i] = KEY_RELEASE;                 
                    g_keyscan_io_data.key_press_num--;

                    for (uint8_t k = 0;k < 3; k++)
                    {
                        if (g_keyscan_io_data.key[k+1] != 0)
                        {
                            g_keyscan_io_data.key[i] = g_keyscan_io_data.key[k+1];
                            g_keyscan_io_data.key_status[i] = KEY_PRESS;
                            g_keyscan_io_data.key[k+1] = 0;
                            g_keyscan_io_data.key_status[k+1] = KEY_RELEASE;
                        }
                    }

                    key_io_msg.subtype = IO_MSG_KEYSCAN_RX_PKT;
                    key_io_msg.u.buf   = (void *)(&g_keyscan_io_data);
                    if (false == app_send_msg_to_apptask(&key_io_msg))
                    {
                        PRINT("send key_press_num-- message failed!\n");
                    }
                    return ;
                }       
            }
            else
            {
                continue;
            }
        }     
    }

}



void GPIO9_Handler(void)
{   
    GPIO_INTConfig(GPIO_GetPin(KEY_IO0), DISABLE);
    GPIO_MaskINTConfig(GPIO_GetPin(KEY_IO0), ENABLE);

    uint8_t key_io = GPIO_ReadInputDataBit(GPIO_GetPin(KEY_IO0));
	
    if (key_io == 0)
        key_io = 1;
    else
        key_io = 0;
    //PRINT("GPIO20_Handler! %d\n", key_io);
    get_value_handler(GEAR_KEY, key_io);

    GPIO_ClearINTPendingBit(GPIO_GetPin(KEY_IO0));
    GPIO_MaskINTConfig(GPIO_GetPin(KEY_IO0), DISABLE);
    GPIO_INTConfig(GPIO_GetPin(KEY_IO0), ENABLE);
}


void GPIO4_Handler(void)
{
    GPIO_INTConfig(GPIO_GetPin(KEY_IO1), DISABLE);
    GPIO_MaskINTConfig(GPIO_GetPin(KEY_IO1), ENABLE);

    uint8_t key_io = GPIO_ReadInputDataBit(GPIO_GetPin(KEY_IO1));
    
    if (key_io == 0)
        key_io = 1;
    else
        key_io = 0;
    //PRINT("GPIO21_Handler! %d\n", key_io);
    get_value_handler(POWER_KEY, key_io);

    GPIO_ClearINTPendingBit(GPIO_GetPin(KEY_IO1));
    GPIO_MaskINTConfig(GPIO_GetPin(KEY_IO1), DISABLE);
    GPIO_INTConfig(GPIO_GetPin(KEY_IO1), ENABLE);
	
}


/**
 * @brief 弥补电源键上电检测按键漏掉的问题
 * @param  
 */
void power_key_check(void)
{
    if (GPIO_ReadInputDataBit(GPIO_GetPin(POWER_KEY)))
    {
        get_value_handler(POWER_KEY, 1);
    }
}

/******************* (C) COPYRIGHT 2023 Realtek Semiconductor Corporation *****END OF FILE****/
