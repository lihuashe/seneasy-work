/**
*********************************************************************************************************
*               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     bct3236_driver.c
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
#include "bct3236_driver.h"

/*============================================================================*
 *                              Global Variables
 *============================================================================*/



/*============================================================================*
 *                              Must Functions 
 *============================================================================*/
void bct3236_init(void)
{
    //PRINT("bct3236_init !!!\r\n");

    Pad_Config(LED_EN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
   
    Pinmux_Config(LED_EN, DWGPIO);

    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    // 配置引脚为模式，启用上拉电阻
    
    GPIO_InitStruct.GPIO_Pin = GPIO_GetPin(LED_EN); 

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN; // 设置为输入模式
    GPIO_InitStruct.GPIO_ITCmd = DISABLE; // 中断
    GPIO_InitStruct.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_ENABLE; // 启用去抖动
    GPIO_InitStruct.GPIO_DebounceTime = 20; // 适用去抖动时间
    GPIO_Init(&GPIO_InitStruct);
    
    LED_EN_OPEN();

    i2c_init();
}

void bct3236_write_byte(uint8_t *data, uint8_t data_len)
{
	i2c_write_byte(BCT3236_ADDR, data, data_len);
    
    // if (data_len >= 2)
    //     PRINT("status:%d data:%02X  %d\n",status, data[0], data[1]);
}

void bct3236_addr_set_send(void)
{
	uint8_t addr = BCT3236_ADDR;
    bct3236_write_byte(&addr, 1);
}

/*============================================================================*
 *                              Local Functions
 *============================================================================*/

void bct3236_write_reg_and_data(uint8_t reg_addr, uint8_t data)
{
#if NEED_SEND_BCT3236_ADDR
    bct3236_addr_set_send();
#endif
    uint8_t reg_data[2] = {reg_addr, data};
    bct3236_write_byte(reg_data, 2);
    //PRINT("reg_data:%d data:%d\n",reg_addr, data);
}

/*
每个寄存器包含三个位：
D7:D3：未使用。
D2:D1（SL）：输出电流设置（IOUT），可以设置为：
00：IMAX（最大电流）。
01：IMAX/2。
10：IMAX/3。
11：IMAX/4。
D0（OUT）：LED状态，0表示LED关闭，1表示LED开启。
*/
/**
 * @brief 
 * @param pin_x      哪一个引脚控制，"1"控制引脚1，"1-x",控制1-x的引脚都同样设置（1-36）
 * @param brightness led亮度控制  0x0-0xff
 * @param led_ctrl   写入led状态寄存器数据
 */
void bct3236_set_pinx(char *pin_x, uint8_t brightness, uint8_t led_ctrl)
{
    //PRINT("bct3236_set_pinx\r\n");
    
    int pin_start = 0; // 引脚编号
    int pin_end = 0;
    uint8_t reg_brightness_data[37] = {};
    uint8_t reg_led_ctrl_data[37] = {};

    bct3236_write_reg_and_data(Bct3236_Software_Mode_Register, 0x01);
    
    if (strlen(pin_x) <= 2)
    {
        sscanf(pin_x, "%d", &pin_start);
        pin_end = pin_start;
    }
    else
    {
        sscanf(pin_x, "%d-%d", &pin_start, &pin_end);
    }
    
    //PRINT("pin_start:%d pin_end:%d\r\n", pin_start, pin_end);
    //写一次寄存器地址，后面自动递增  
    //循环遍历所有引脚，设置亮度

    reg_brightness_data[0] = pin_start;
    for (uint8_t i = pin_start; i <= pin_end; i++)
    {
        reg_brightness_data[i - pin_start + 1] = brightness;
    }
    // 写入亮度寄存器
    bct3236_write_byte(reg_brightness_data, pin_end - pin_start + 2);


    //写一次寄存器地址，后面自动递增  
    reg_led_ctrl_data[0] = Bct3236_Led_Control_Register + pin_start - 1;
    for (uint8_t i = pin_start; i <= pin_end; i++)
    {
        reg_led_ctrl_data[i - pin_start + 1] = led_ctrl;
    }
    // 写入亮度寄存器
    bct3236_write_byte(reg_led_ctrl_data, pin_end - pin_start + 2);

    bct3236_write_reg_and_data(Bct3236_Brightness_Update_Register, 0x00);
    bct3236_write_reg_and_data(Bct3236_Global_Control_Register, 0x00);

    os_delay(1);
}




/**
 * @brief 用于灯组控制
 * @param pin_arr     灯组  例{1,3,6,8,9}
 * @param pin_arr_len 组长  5
 * @param brightness  亮度   
 * @param led_ctrl    开关及其电流
 */
void bct3236_set_led_group(uint8_t *pin_arr, uint8_t pin_arr_len, uint8_t brightness, uint8_t led_ctrl)
{
    // 打印函数名
    //PRINT("bct3236_set_led_group\r\n");

    // 使能软件模式
    bct3236_write_reg_and_data(Bct3236_Software_Mode_Register, 0x01);

    // 遍历每个引脚
    for (uint8_t i = 0; i < pin_arr_len; i++) 
    {
        // 写入亮度寄存器
        bct3236_write_reg_and_data(pin_arr[i], brightness);   
    }

    // 遍历每个引脚
    for (uint8_t i = 0; i < pin_arr_len; i++) 
    {
        // 写入LED控制寄存器
        bct3236_write_reg_and_data(Bct3236_Brightness_Update_Register + pin_arr[i], led_ctrl); 
    }

    bct3236_write_reg_and_data(Bct3236_Brightness_Update_Register, 0x00);
    bct3236_write_reg_and_data(Bct3236_Global_Control_Register, 0x00);

    os_delay(1);//最好有延时
}



/**
 * @brief 实现每一个引脚的亮度、电流配置
 * @param pin_arr 保存引脚号、亮度、电流
 * @param pin_arr_len 数组长度
 */
void bct3236_every_led_config(Bct3236_Led_Config_t *pin_arr, uint8_t pin_arr_len)
{
    // 打印函数名
    //PRINT("bct3236_set_everyled\r\n");

    // 使能软件模式
    bct3236_write_reg_and_data(Bct3236_Software_Mode_Register, 0x01);

    // 遍历每个引脚
    for (uint8_t i = 0; i < pin_arr_len; i++) 
    {
        
        bct3236_write_reg_and_data(pin_arr[i].led_index, pin_arr[i].light);   
    }

    // 遍历每个引脚
    for (uint8_t i = 0; i < pin_arr_len; i++) 
    {
        // 写入亮度寄存器
        bct3236_write_reg_and_data(Bct3236_Brightness_Update_Register + pin_arr[i].led_index, pin_arr[i].iout); 
    }

    bct3236_write_reg_and_data(Bct3236_Brightness_Update_Register, 0x00);
    bct3236_write_reg_and_data(Bct3236_Global_Control_Register, 0x00);

    os_delay(1);
}


void bct3236_led_all_on(void)
{
    bct3236_set_pinx("1-36", 0xfe, 1);
}


void bct3236_led_all_off(void)
{
    bct3236_set_pinx("1-36", 0x00, 0);
}


/******************* (C) COPYRIGHT 2025 Sensasy Corporation *****END OF FILE****/
