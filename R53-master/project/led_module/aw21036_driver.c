/**
*********************************************************************************************************
*               Copyright(c) 2023, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     aw21036_driver.c
* @brief    
* @details
* @author   chenjiatao
* @date     2023-06-06
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "aw21036_driver.h"
#include "led_driver.h"
#include "ltr_sensor.h"
#include "backlight_handle.h"
#include "../../sdk/drivers/8278/i2c.h"
#include "../../sdk/drivers/8278/gpio.h"
#include "../app_rcu.h"
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
u8 LED_BR0_Data  = 255;
u8 LED_COL0_Data = 255;
u8 Quick_WaitMs_Flag = 0;
_attribute_data_retention_ u8 BackLight_Connect_Init = 0;
_attribute_data_retention_ bool BR_TEST_CLOSE_FLAG = false;
_attribute_data_retention_ bool BR_TEST_LED_DUTY_FLAG = false;
/*============================================================================*
 *                              Functions Declaration
 *============================================================================*/
#if (LED_AW21036_ENABLE)
static void aw21036_set_global_current(u8 current)
{
	i2c_write_byte(REG_GCCR, 1, current);
}
#endif
void aw21036_set_brightness(u8 index, u8 brightness)
{
	i2c_write_byte(index, 1, brightness);
}
void aw21036_set_color(u8 index, u8 color)
{
	i2c_write_byte(index, 1, color);
}
void aw21036_update_brightness_to_display(void)
{
	i2c_write_byte(REG_UPDATE, 1, 0x00);
}
/*============================================================================*
 *                              Local Functions
 *============================================================================*/
/******************************************************************
 * @brief  aw21036 init
 * @param  none
 * @return none
 * @retval void
 */
void aw21036_init(void)
{
#if (LED_AW21036_ENABLE)
#if (PCB_VER == PCB_SRC_3215)
if(BR_TEST_CLOSE_FLAG == false)
{
	//WaitMs(10);
    gpio_set_func(GPIO_PC4,  AS_GPIO);
    gpio_set_output_en(GPIO_PC4, 1);
    gpio_write(GPIO_PC4,1);

    user_i2c_gpio_set(I2C_GPIO_GROUP_C2C3);
	i2c_master_init(AW21036_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );

	i2c_write_byte(REG_RESET, 1, 0x00);
	//WaitMs(10);
    /* 1. enable chip and enable auto power-saving 使能芯片--active模式 */
    //0x01:63kHz  0x11:32kHz  0x21:4kHz  0x31:2kHz  0x41:1kHz  0x51:500Hz  0X61:244Hz  0x71:122Hz
    i2c_write_byte(REG_GCR, 1, 0x01);
    /* 2. check id */
    unsigned char chip_id = i2c_read_byte(REG_RESET, 1);
    //printf("--->>> AW21036_Sensor Chip_id  == 0x%02x\r\n",chip_id);
	/* 3. config global current 设置全局电流 */
    aw21036_set_global_current(0xff);
	if(chip_id != 0x18)
	{
		sensor_init_error_cnt ++;
		if(sensor_init_error_cnt <= 10)
			sensor_init_timer = clock_time() | 1;
		else
			sensor_init_error_cnt = 0;
	}
	else
	{
		//printf("--->>> Sensor_Chip_id: 0x%02x, init_error_cnt: %d\r\n",chip_id,sensor_init_error_cnt);
		sensor_init_error_cnt = 0;
		if(led_display_flag)
			ALS_Read_Status();
	}
	Quick_WaitMs_Flag = 0;
}
#endif
#endif
}

/******************************************************************
 * @brief  aw21036_Disable
 * @param  none
 * @return none
 * @retval void
 */
void aw21036_Disable(void)
{
#if (LED_AW21036_ENABLE)
#if (PCB_VER == PCB_SRC_3215)
	//aw21036_all_off();
    //i2c_master_init(AW21036_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );
    //i2c_write_byte(REG_GCR, 1, 0x00);
    gpio_write(GPIO_PD7,1);
    //gpio_write(GPIO_PC4,0);
#endif
#endif
}

/******************************************************************
 * @brief  aw21036_all_on
 * @param  set_br0, set_col0
 * @return none
 * @retval void
 */
void aw21036_all_on(u8 set_br0, u8 set_col0)
{
#if (LED_AW21036_ENABLE)
    i2c_master_init(AW21036_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );
	int i = 0;
	for (i = 0; i < AW21036_LED_NUM; i++)
    {
        /* 1.set BR for brightness  设置每个LED的恒流电流，255个等级*/   
        aw21036_set_brightness(REG_BR0 + i, set_br0);
        /* 2.set COL for color 		设置每个LED的PWM，255个等级*/  
        aw21036_set_color(REG_COL0 + i, set_col0);  
    }
	aw21036_update_brightness_to_display();	
#endif
}

/******************************************************************
 * @brief  aw21036_all_off
 * @param  none
 * @return none
 * @retval void
 */
void aw21036_all_off(void)
{
#if (LED_AW21036_ENABLE)
    i2c_master_init(AW21036_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );
	int i = 0;
	for (i = 0; i < AW21036_LED_NUM; i++)
    {
		aw21036_set_color(REG_COL0 + i, 0x00);
        aw21036_set_brightness(REG_BR0 + i, 0x00);
    }
    aw21036_update_brightness_to_display();
#endif
}

/******************************************************************
 * @brief  aw21036_onoff_one
 * @param  led_cnt, led_flag, col_val, br_val
 * @return none
 * @retval void
 */
void aw21036_onoff_one(u8 led_cnt, u8 led_flag, u8 col_val, u8 br_val)
{
#if (LED_AW21036_ENABLE)
    i2c_master_init(AW21036_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );
    if(led_flag){
        aw21036_set_color(REG_COL0 + led_cnt - 1, col_val);
        aw21036_set_brightness(REG_BR0 + led_cnt - 1, br_val);        
    }else{
        aw21036_set_color(REG_COL0 + led_cnt - 1, 0x00);
        aw21036_set_brightness(REG_BR0 + led_cnt - 1, 0x00);
    }
    aw21036_update_brightness_to_display();	
#endif
}

/******************************************************************
 * @brief  aw21036_breath_quick
 * @param  led_cnt, led_flag, col_val, br_val
 * @return none
 * @retval void
 */
void aw21036_breath_quick(u8 led_flag)
{
#if (LED_AW21036_ENABLE)
    i2c_master_init(AW21036_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );
	aw21036_set_color(REG_COL0 + 35, LED_COL_ALL);

    if(ASL_Lux_Calc <= BR_LUX_LEVEL_0)
        key_level_flag = 19; 
    else if(ASL_Lux_Calc <= BR_LUX_LEVEL_1)
        key_level_flag = 15;         
    else if(ASL_Lux_Calc <= BR_LUX_LEVEL_2)
        key_level_flag = 10;         
    else if(ASL_Lux_Calc <= BR_LUX_LEVEL_3)
        key_level_flag = 5;         
	else if(ASL_Lux_Calc <= BR_LUX_LEVEL_4)
        key_level_flag = 0;         
    else if(ASL_Lux_Calc <= BR_LUX_LEVEL_5)
        key_level_flag = 0;         
    else
        key_level_flag = 0;         

	aw21036_set_brightness(REG_FADEH, br_val_level[key_level_flag]);
	//aw21036_set_brightness(REG_FADEH, LED_BR_ALL);
	aw21036_set_brightness(REG_FADEL, 0x00);
	i2c_write_byte(REG_GCFG0,  1, 0x00);
	i2c_write_byte(REG_GCFG1,  1, 0x10);
	i2c_write_byte(REG_PATCT0, 1, 0x10 | 0x01);
	i2c_write_byte(REG_PATCT1, 1, 0x10 | 0x01);
    i2c_write_byte(REG_PATCT2, 1, 0x00);
	i2c_write_byte(REG_PATCT3, 1, 0x00);
	if(led_flag){
	    i2c_write_byte(REG_PATCFG, 1, 0x0d);
    }else{
	    i2c_write_byte(REG_PATCFG, 1, 0x05);
    }
	i2c_write_byte(REG_PATGO, 1, 0x01);
    //WaitMs(150);
	if(Quick_WaitMs_Flag == 1)
	{
		Quick_WaitMs_Flag = 0;
		WaitMs(150);
	}
#endif
}

/******************************************************************
 * @brief  aw21036_breath_one
 * @param  led_cnt, led_flag, col_val, br_val
 * @return none
 * @retval void
 */
void aw21036_breath_one(u8 led_cnt, u8 led_flag, u8 col_val, u8 br_val)
{
#if (LED_AW21036_ENABLE)
    i2c_master_init(AW21036_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );
	aw21036_set_color(REG_COL0 + led_cnt - 1, col_val);
	aw21036_set_brightness(REG_FADEH, br_val);
	aw21036_set_brightness(REG_FADEL, 0x00);
	//i2c_write_byte(REG_GCFG0,  1, 0xff);
	//i2c_write_byte(REG_GCFG1,  1, 0x1f);
	i2c_write_byte(REG_PATCT0, 1, led_timer_T0 | led_timer_T1);
	if(key_press_quick_timer == 0x01)
		i2c_write_byte(REG_PATCT1, 1, led_timer_key_T2 | led_timer_key_T3);
	else
		i2c_write_byte(REG_PATCT1, 1, led_timer_T2 | led_timer_T3);
    i2c_write_byte(REG_PATCT2, 1, 0xD0);
	i2c_write_byte(REG_PATCT3, 1, 0x00);
	if(led_flag){
	    i2c_write_byte(REG_PATCFG, 1, 0x0d);
    }else{
	    i2c_write_byte(REG_PATCFG, 1, 0x05);
    }
	i2c_write_byte(REG_PATGO, 1, 0x01);
#endif
}

/******************************************************************
 * @brief  aw21036_all_breath
 * @param  none
 * @return none
 * @retval void
 */
void aw21036_all_breath(void)
{
#if (LED_AW21036_ENABLE)
	#if (!CLOSE_CODE_PRINTF)
	printf("--->>> [work_debug] aw21036_all_breath\r\n");
	#endif
	/* 1. set color */
	aw21036_set_color(REG_GCOLR, 0xff);
	aw21036_set_color(REG_GCOLG, 0xff);
	aw21036_set_color(REG_GCOLB, 0xff);
	/* 2. set brigthness */
	aw21036_set_brightness(REG_FADEH, 0xff);
	aw21036_set_brightness(REG_FADEL, 0x10);
	/* 3. group	GEn=1/PATEN=1/GCOLDIS=0 */
	i2c_write_byte(REG_GCFG0, 1, 0xff);
	i2c_write_byte(REG_GCFG1, 1, 0x0f);
	/* 4. set pattern times T1/T2/T3/T4 */
	i2c_write_byte(REG_PATCT0, 1, 0x30 | 0x03);
	i2c_write_byte(REG_PATCT1, 1, 0x30 | 0x03);
	/* 5. set pattern0_breath start/end phase */
	i2c_write_byte(REG_PATCT2, 1, 0x00);
	/* 6. set pattern0_breath loop times -> forever */
	i2c_write_byte(REG_PATCT3, 1, 0x00);
	/* 7. set auto_breath mode */
	i2c_write_byte(REG_PATCFG, 1, 0x07);
	/* 8. start breath */
	i2c_write_byte(REG_PATGO, 1, 0x01);
#endif
}

/******************************************************************
 * @brief  aw21036_breath_off
 * @param  none
 * @return none
 * @retval void
 */
void aw21036_breath_off(void)
{
#if (LED_AW21036_ENABLE)
	#if (!CLOSE_CODE_PRINTF)
	printf("--->>> [work_debug] aw21036_breath_off\r\n");
	#endif
	aw21036_set_color(REG_GCOLR, 0x00);
	aw21036_set_color(REG_GCOLG, 0x00);
	aw21036_set_color(REG_GCOLB, 0x00);
	i2c_write_byte(REG_PATCFG, 1, 0x04);
	i2c_write_byte(REG_PATGO, 1, 0x00);
	i2c_write_byte(REG_GCFG0, 1, 0x00);
	i2c_write_byte(REG_GCFG1, 1, 0x00);
#endif
}


void aw21036_init_TEST(void)
{
#if (LED_AW21036_ENABLE)
if(BR_TEST_CLOSE_FLAG == false)
{
    //I2C pin set
    gpio_set_func(GPIO_PD7,  AS_GPIO);
    gpio_set_output_en(GPIO_PD7, 1);
    gpio_write(GPIO_PD7,0);
    gpio_set_func(GPIO_PC4,  AS_GPIO);
    gpio_set_output_en(GPIO_PC4, 1);
    gpio_write(GPIO_PC4,1);
    user_i2c_gpio_set(I2C_GPIO_GROUP_C2C3);
	i2c_master_init(AW21036_DEVICE_ADDR, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );
	i2c_write_byte(REG_RESET, 1, 0x00);
    i2c_write_byte(REG_GCR, 1, 0x01);
    /* 2. check id */
    unsigned char chip_id = i2c_read_byte(REG_RESET, 1);
    printf("--->>> AW21036_Sensor Chip_id  == 0x%02x\r\n",chip_id);
	/* 3. config global current 设置全局电流 */
    aw21036_set_global_current(0xff);
	if(chip_id != 0x18)
	{
		sensor_init_error_cnt ++;
		if(sensor_init_error_cnt <= 10)
			sensor_init_timer = clock_time() | 1;
		else
			sensor_init_error_cnt = 0;
	}
	else
	{
		ltr_sensor_init(); 
		sensor_init_error_cnt = 0;
	}
}
#endif
}

/******************* (C) COPYRIGHT 2023 Sensasy Corporation *****END OF FILE****/
